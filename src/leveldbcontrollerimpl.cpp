#include "leveldbcontrollerimpl.h"
#include "leveldb/db.h"
#include "leveldb/write_batch.h"
#include "config.h"

namespace leveldb
{

DBController::DBController(WriteBatchController* wbcontroller, IteratorController* itcontroller)
    : IDBController(), _wbcontroller(wbcontroller), _itcontroller(itcontroller)
{

}

DBController::~DBController()
{

}

Status DBController::Open(const std::string& dbname, const Options& options)
{
    DB *db;
    Status status = Status::OK();
    if ( _dbs.find(dbname) == _dbs.end() ) {
        status = DB::Open(options, config::db_file_base_path+dbname, &db);
        if (status.ok()) {
            _dbs[dbname] = std::shared_ptr<DB>(db);
        }
    }
    return status;
}

Status DBController::Close(const std::string& dbname)
{
    Status status = Status::NotFound(dbname, "is not open");
    auto db = _dbs.find(dbname);
    if ( db != _dbs.end() ) {
        status = Status::OK();
        _dbs.erase(dbname);
    }
    return status;
}

template<typename ... Args, typename T>
inline Status CallObjectMethod(Status(T::*func)(Args ... a), const std::string& name, std::map<std::string, std::shared_ptr<T>> &maps, Args&& ... a)
{
    Status status = Status::NotFound(name, "does not exist");
    auto db = maps.find(name);
    if ( db != maps.end() ) {
        status = ((*(db->second)).*func)(std::forward<Args>(a)...);
    }
    return status;
}

template<typename ... Args, typename T>
inline Status CallObjectMethod(void(T::*func)(Args ... a), const std::string& name, std::map<std::string, std::shared_ptr<T>> &maps, Args&& ... a)
{
    Status status = Status::NotFound(name, "does not exist");
    auto db = maps.find(name);
    if ( db != maps.end() ) {
        status = Status::OK();
        ((*(db->second)).*func)(std::forward<Args>(a)...);
    }
    return status;
}

Status DBController::Put(const std::string& dbname, const WriteOptions& options, const Slice& key, const Slice& value)
{
    return CallObjectMethod(&DB::Put, dbname, _dbs, options, key, value);
}

Status DBController::Delete(const std::string& dbname, const WriteOptions& options, const Slice& key)
{
    return CallObjectMethod(&DB::Delete, dbname, _dbs, options, key);
}

Status DBController::Write(const std::string& dbname, const WriteOptions& options, const std::string& wbname)
{
    WriteBatch* wb;
    Status status = _wbcontroller->getWriteBatch(wbname, &wb);
    if (!status.ok()) {
        return status;
    }
    return CallObjectMethod(&DB::Write, dbname, _dbs, options, &*wb);
}

Status DBController::Get(const std::string& dbname, const ReadOptions& options, const Slice& key, std::string* value)
{
    return CallObjectMethod(&DB::Get, dbname, _dbs, options, key, &*value);
}

Status DBController::NewIterator(const std::string& dbname, const ReadOptions& options, const std::string& itname)
{
    Status status = Status::NotFound(dbname, "does not open");
    auto db = _dbs.find(dbname);
    if ( db != _dbs.end() ) {
        return _itcontroller->addIterator(itname, std::shared_ptr<Iterator>(db->second->NewIterator(options)));
    }
    return status;
}

Status DBController::DestroyIterator(const std::string& dbname, const std::string& itname)
{
    return _itcontroller->removeIterator(itname);
}

Status DBController::Destroy(const std::string& dbname, const Options& options)
{
    Status status = Status::NotFound(dbname, "does not open");
    auto db = _dbs.find(dbname);
    if ( db != _dbs.end() ) {
        return DestroyDB(dbname, options);
    }
    return status;
}

Status DBController::Repair(const std::string& dbname, const Options& options)
{
    Status status = Status::NotFound(dbname, "does not open");
    auto db = _dbs.find(dbname);
    if ( db != _dbs.end() ) {
        return Repair(dbname, options);
    }
    return status;
}

WriteBatchController::WriteBatchController() : IWriteBatchController()
{

}

WriteBatchController::~WriteBatchController()
{

}

Status WriteBatchController::Create(const std::string& wbname)
{
    Status status = Status::InvalidArgument(wbname, "does exist");
    if ( _wbs.find(wbname) == _wbs.end() ) {
        status = Status::OK();
        _wbs[wbname] = std::shared_ptr<WriteBatch>(new WriteBatch());
    }
    return status;
}

Status WriteBatchController::Destroy(const std::string& wbname)
{
    Status status = Status::NotFound(wbname, "does not exist");
    auto wb = _wbs.find(wbname);
    if ( wb != _wbs.end() ) {
        status = Status::OK();
        _wbs.erase(wbname);
    }
    return status;
}

Status WriteBatchController::Clear(const std::string& wbname)
{
    return CallObjectMethod(&WriteBatch::Clear, wbname, _wbs);
}

Status WriteBatchController::Put(const std::string& wbname, const Slice& key, const Slice& value)
{
    return CallObjectMethod(&WriteBatch::Put, wbname, _wbs, key, value);
}

Status WriteBatchController::Delete(const std::string& wbname, const Slice& key)
{
    return CallObjectMethod(&WriteBatch::Delete, wbname, _wbs, key);
}

Status WriteBatchController::Append(const std::string& wbname, const std::string& sourcewbname)
{
    WriteBatch* wb1;
    Status status = getWriteBatch(wbname, &wb1);
    if (!status.ok()) {
        return status;
    }
    WriteBatch* wb2;
    status = getWriteBatch(sourcewbname, &wb2);
    if (!status.ok()) {
        return status;
    }
    wb1->Append(*wb2);
    return Status::OK();
}

Status WriteBatchController::getWriteBatch(const std::string& wbname, WriteBatch** wb)
{
    Status status = Status::NotFound(wbname, "does not exist");
    auto wb_ = _wbs.find(wbname);
    if ( wb_ != _wbs.end() ) {
        status = Status::OK();
        *wb = wb_->second.get();
    }
    return status;
}

IteratorController::IteratorController() : IIteratorController()
{

}

IteratorController::~IteratorController()
{

}

Status IteratorController::Valid(const std::string& itname, bool * valid)
{
    Iterator* it;
    auto status = getIterator(itname, &it);
    if (status.ok()) {
        status = Status::OK();
        *valid = it->Valid();
    }
    return status;
}

Status IteratorController::SeekToFirst(const std::string& itname)
{
    return CallObjectMethod(&Iterator::SeekToFirst, itname, _its);
}

Status IteratorController::SeekToLast(const std::string& itname)
{
    return CallObjectMethod(&Iterator::SeekToLast, itname, _its);
}

Status IteratorController::Seek(const std::string& itname, const Slice& target)
{
    return CallObjectMethod(&Iterator::Seek, itname, _its, target);
}

Status IteratorController::Next(const std::string& itname)
{
    return CallObjectMethod(&Iterator::Next, itname, _its);
}

Status IteratorController::Prev(const std::string& itname)
{
    return CallObjectMethod(&Iterator::Prev, itname, _its);
}

Status IteratorController::Key(const std::string& itname, std::string* key)
{
    Iterator* it;
    auto status = getIterator(itname, &it);
    if (status.ok()) {
        status = Status::OK();
        *key = it->key().ToString();
    }
    return status;
}

Status IteratorController::Value(const std::string& itname, std::string* value)
{
    Iterator* it;
    auto status = getIterator(itname, &it);
    if (status.ok()) {
        status = Status::OK();
        *value = it->value().ToString();
    }
    return status;
}

Status IteratorController::Iterate(const std::string& itname, std::string* key, std::string* value, bool *valid)
{
    Iterator* it;
    auto status = getIterator(itname, &it);
    if (status.ok()) {
        status = Status::OK();
        *value = it->value().ToString();
        *key = it->key().ToString();
        it->Next();
        *valid = it->Valid();
    }
    return status;
}

Status IteratorController::addIterator(const std::string& itname, std::shared_ptr<Iterator> it)
{
    Status status = Status::InvalidArgument(itname, "does exist");
    auto it_ = _its.find(itname);
    if ( it_ == _its.end() ) {
        status = Status::OK();
        _its[itname] = it;
    }
    return status;
}

Status IteratorController::removeIterator(const std::string& itname)
{
    Status status = Status::NotFound(itname, "does not exist");
    auto it_ = _its.find(itname);
    if ( it_ != _its.end() ) {
        status = Status::OK();
        _its.erase(itname);
    }
    return status;
}

inline Status IteratorController::getIterator(const std::string& itname, Iterator** it)
{
    Status status = Status::NotFound(itname, "does not exist");
    auto it_ = _its.find(itname);
    if ( it_ != _its.end() ) {
        status = Status::OK();
        *it = it_->second.get();
    }
    return status;
}

} // namespace leveldb
