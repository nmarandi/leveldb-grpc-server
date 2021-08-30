#ifndef LEVELDBCONTROLLER_H
#define LEVELDBCONTROLLER_H

#include "leveldb/status.h"
namespace leveldb
{

struct Options;
struct ReadOptions;
struct WriteOptions;
class WriteBatch;

class IDBController
{
public:
    IDBController() {};
    virtual ~IDBController() {};
    virtual Status Open(const std::string& dbname, const Options& options) = 0;
    virtual Status Close(const std::string& dbname) = 0;
    virtual Status Put(const std::string& dbname, const WriteOptions& options, const Slice& key, const Slice& value) = 0;
    virtual Status Delete(const std::string& dbname, const WriteOptions& options, const Slice& key) = 0;
    virtual Status Write(const std::string& dbname, const WriteOptions& options, const std::string& wbname) = 0;
    virtual Status Get(const std::string& dbname, const ReadOptions& options, const Slice& key, std::string* value) = 0;
    virtual Status NewIterator(const std::string& dbname, const ReadOptions& options, const std::string& itname) = 0;
    virtual Status DestroyIterator(const std::string& dbname, const std::string& itname) = 0;
    virtual Status Destroy(const std::string& dbname, const Options& options) = 0;
    virtual Status Repair(const std::string& dbname, const Options& options) = 0;
};

class IWriteBatchController
{
public:
    IWriteBatchController() {};
    virtual ~IWriteBatchController() {};
    virtual Status Create(const std::string& wbname) = 0;
    virtual Status Destroy(const std::string& wbname) = 0;
    virtual Status Clear(const std::string& wbname) = 0;
    virtual Status Put(const std::string& wbname, const Slice& key, const Slice& value) = 0;
    virtual Status Delete(const std::string& wbname, const Slice& key) = 0;
    virtual Status Append(const std::string& wbname, const std::string& sourcewbname) = 0;
};

class IIteratorController
{
public:
    IIteratorController() {};
    virtual ~IIteratorController() {};
    virtual Status Valid(const std::string& itname, bool *valid) = 0;
    virtual Status SeekToFirst(const std::string& itname) = 0;
    virtual Status SeekToLast(const std::string& itname) = 0;
    virtual Status Seek(const std::string& itname, const Slice& target) = 0;
    virtual Status Next(const std::string& itname) = 0;
    virtual Status Prev(const std::string& itname) = 0;
    virtual Status Key(const std::string& itname, std::string* key) = 0;
    virtual Status Value(const std::string& itname, std::string* value) = 0;
};
}

#endif // LEVELDBCONTROLLER_H
