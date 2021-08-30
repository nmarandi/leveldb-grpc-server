#ifndef LEVELDBCONTROLLERIMPL_H
#define LEVELDBCONTROLLERIMPL_H

#include <map>
#include <memory>

#include "leveldbcontroller.h"

namespace leveldb
{
class WriteBatchController;
class IteratorController;
class DB;
class Iterator;

class DBController :public IDBController
{
public:
    DBController(WriteBatchController* wbcontroller, IteratorController* itcontroller);
    virtual ~DBController();
    virtual Status Open(const std::string& dbname, const Options& options)override;
    virtual Status Close(const std::string& dbname)override;
    virtual Status Put(const std::string& dbname, const WriteOptions& options, const Slice& key, const Slice& value)override;
    virtual Status Delete(const std::string& dbname, const WriteOptions& options, const Slice& key)override;
    virtual Status Write(const std::string& dbname, const WriteOptions& options, const std::string& wbname)override;
    virtual Status Get(const std::string& dbname, const ReadOptions& options, const Slice& key, std::string* value)override;
    virtual Status NewIterator(const std::string& dbname, const ReadOptions& options, const std::string& itname)override;
    virtual Status DestroyIterator(const std::string& dbname, const std::string& itname)override;
    virtual Status Destroy(const std::string& dbname, const Options& options)override;
    virtual Status Repair(const std::string& dbname, const Options& options)override;

private:
    std::map<std::string, std::shared_ptr<DB>> _dbs;
    WriteBatchController* _wbcontroller;
    IteratorController* _itcontroller;
};

class WriteBatchController : public IWriteBatchController
{
public:
    WriteBatchController();
    virtual ~WriteBatchController();
    virtual Status Create(const std::string& wbname)override;
    virtual Status Destroy(const std::string& wbname)override;
    virtual Status Clear(const std::string& wbname)override;
    virtual Status Put(const std::string& wbname, const Slice& key, const Slice& value)override;
    virtual Status Delete(const std::string& wbname, const Slice& key)override;
    virtual Status Append(const std::string& wbname, const std::string& sourcewbname)override;
private:
    friend class DBController;
    Status getWriteBatch(const std::string& wbname, WriteBatch** wb);
    std::map<std::string, std::shared_ptr<WriteBatch>> _wbs;
};

class IteratorController : public IIteratorController
{
public:
    IteratorController();
    virtual ~IteratorController();
    virtual Status Valid(const std::string& itname, bool * valid)override;
    virtual Status SeekToFirst(const std::string& itname)override;
    virtual Status SeekToLast(const std::string& itname)override;
    virtual Status Seek(const std::string& itname, const Slice& target)override;
    virtual Status Next(const std::string& itname)override;
    virtual Status Prev(const std::string& itname)override;
    virtual Status Key(const std::string& itname, std::string* key)override;
    virtual Status Value(const std::string& itname, std::string* value)override;
private:
    friend class DBController;
    Status addIterator(const std::string& itname, std::shared_ptr<Iterator> it);
    Status removeIterator(const std::string& itname);
    Status getIterator(const std::string& itname, Iterator** it);
    std::map<std::string, std::shared_ptr<Iterator>> _its;
};
}

#endif // LEVELDBCONTROLLERIMPL_H
