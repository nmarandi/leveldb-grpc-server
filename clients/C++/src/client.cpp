#include "client.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using namespace leveldb::grpc;

leveldb::grpc::Status grpc_error_to_leveldb_status(const ::grpc::Status& status)
{
    leveldb::grpc::Status leveldbstatus;
    leveldbstatus.set_code(Status_Code::Status_Code_kIOError);
    leveldbstatus.set_message(status.error_message());
    return leveldbstatus;
}

DBClient::DBClient(std::shared_ptr<Channel> channel)
    : stub_(DBRPC::NewStub(channel)) {}

leveldb::grpc::Status DBClient::Open(const std::string& dbname, const Options& options)
{
    DBReq request;
    DB* db = new DB();
    db->set_name(dbname);
    request.set_allocated_db(db);
    auto* options_ = new Options(options);
    request.set_allocated_options(options_);

    DBRes reply;
    ClientContext context;

    ::grpc::Status status = stub_->Open(&context, request, &reply);

    if (status.ok()) {
        return reply.status();
    }
    return grpc_error_to_leveldb_status(status);
}

leveldb::grpc::Status DBClient::Close(const std::string& dbname)
{
    DBReq request;
    DB* db = new DB();
    db->set_name(dbname);
    request.set_allocated_db(db);

    DBRes reply;
    ClientContext context;

    ::grpc::Status status = stub_->Close(&context, request, &reply);

    if (status.ok()) {
        return reply.status();
    }
    return grpc_error_to_leveldb_status(status);
}

leveldb::grpc::Status DBClient::Put(const std::string& dbname, const WriteOptions& options, const std::string& key, const std::string& value)
{
    DBReq request;
    DB* db = new DB();
    db->set_name(dbname);
    request.set_allocated_db(db);
    request.set_key(key);
    request.set_value(value);
    auto* options_ = new WriteOptions(options);
    request.set_allocated_write_options(options_);

    DBRes reply;
    ClientContext context;

    ::grpc::Status status = stub_->Put(&context, request, &reply);

    if (status.ok()) {
        return reply.status();
    }
    return grpc_error_to_leveldb_status(status);
}

leveldb::grpc::Status DBClient::Delete(const std::string& dbname, const WriteOptions& options, const std::string& key)
{
    DBReq request;
    DB* db = new DB();
    db->set_name(dbname);
    request.set_allocated_db(db);
    request.set_key(key);
    auto* options_ = new WriteOptions(options);
    request.set_allocated_write_options(options_);

    DBRes reply;
    ClientContext context;

    ::grpc::Status status = stub_->Delete(&context, request, &reply);

    if (status.ok()) {
        return reply.status();
    }
    return grpc_error_to_leveldb_status(status);
}

leveldb::grpc::Status DBClient::Write(const std::string& dbname, const WriteOptions& options, const std::string& wbname)
{
    DBReq request;
    DB* db = new DB();
    db->set_name(dbname);
    request.set_allocated_db(db);
    WriteBatch* wb = new WriteBatch();
    wb->set_name(wbname);
    request.set_allocated_write_batch(wb);
    auto* options_ = new WriteOptions(options);
    request.set_allocated_write_options(options_);

    DBRes reply;
    ClientContext context;

    ::grpc::Status status = stub_->Write(&context, request, &reply);

    if (status.ok()) {
        return reply.status();
    }
    return grpc_error_to_leveldb_status(status);
}

leveldb::grpc::Status DBClient::Get(const std::string& dbname, const leveldb::grpc::ReadOptions& options, const std::string& key, std::string* value)
{
    DBReq request;
    DB* db = new DB();
    db->set_name(dbname);
    request.set_allocated_db(db);
    request.set_key(key);
    auto opt = new ReadOptions(options);
    request.set_allocated_read_options(opt);

    DBRes reply;
    ClientContext context;

    ::grpc::Status status = stub_->Get(&context, request, &reply);

    if (status.ok()) {

        if (reply.status().code() == Status_Code::Status_Code_kOk) {
            *value = reply.value();
        }
        return reply.status();
    }
    return grpc_error_to_leveldb_status(status);
}

leveldb::grpc::Status DBClient::NewIterator(const std::string& dbname, const ReadOptions& options, const std::string& itname)
{
    DBReq request;
    DB* db = new DB();
    db->set_name(dbname);
    request.set_allocated_db(db);
    auto opt = new ReadOptions(options);
    request.set_allocated_read_options(opt);
    Iterator *it = new Iterator();
    it->set_name(itname);
    request.set_allocated_iterator(it);

    DBRes reply;
    ClientContext context;

    ::grpc::Status status = stub_->NewIterator(&context, request, &reply);

    if (status.ok()) {
        return reply.status();
    }
    return grpc_error_to_leveldb_status(status);
}

leveldb::grpc::Status DBClient::DestroyIterator(const std::string& dbname, const std::string& itname)
{
    DBReq request;
    DB* db = new DB();
    db->set_name(dbname);
    request.set_allocated_db(db);
    Iterator *it = new Iterator();
    it->set_name(itname);
    request.set_allocated_iterator(it);

    DBRes reply;
    ClientContext context;

    ::grpc::Status status = stub_->DestroyIterator(&context, request, &reply);

    if (status.ok()) {
        return reply.status();
    }
    return grpc_error_to_leveldb_status(status);
}

leveldb::grpc::Status DBClient::Destroy(const std::string& dbname, const leveldb::grpc::Options& options)
{
    DBReq request;
    DB* db = new DB();
    db->set_name(dbname);
    request.set_allocated_db(db);
    auto* options_ = new Options(options);
    request.set_allocated_options(options_);

    DBRes reply;
    ClientContext context;

    ::grpc::Status status = stub_->Destroy(&context, request, &reply);

    if (status.ok()) {
        return reply.status();
    }
    return grpc_error_to_leveldb_status(status);
}

leveldb::grpc::Status DBClient::Repair(const std::string& dbname, const leveldb::grpc::Options& options)
{
    DBReq request;
    DB* db = new DB();
    db->set_name(dbname);
    request.set_allocated_db(db);
    auto* options_ = new Options(options);
    request.set_allocated_options(options_);

    DBRes reply;
    ClientContext context;

    ::grpc::Status status = stub_->Repair(&context, request, &reply);

    if (status.ok()) {
        return reply.status();
    }
    return grpc_error_to_leveldb_status(status);
}

WriteBatchClient::WriteBatchClient(std::shared_ptr<grpc::Channel> channel)
    : stub_(WriteBatchRPC::NewStub(channel)) {}

leveldb::grpc::Status WriteBatchClient::Create(const std::string& wbname)
{
    WriteBatchReq request;
    WriteBatch* wb = new WriteBatch();
    wb->set_name(wbname);
    request.set_allocated_write_batch(wb);

    WriteBatchRes reply;
    ClientContext context;

    ::grpc::Status status = stub_->Create(&context, request, &reply);

    if (status.ok()) {
        return reply.status();
    }
    return grpc_error_to_leveldb_status(status);
}

leveldb::grpc::Status WriteBatchClient::Destroy(const std::string& wbname)
{
    WriteBatchReq request;
    WriteBatch* wb = new WriteBatch();
    wb->set_name(wbname);
    request.set_allocated_write_batch(wb);

    WriteBatchRes reply;
    ClientContext context;

    ::grpc::Status status = stub_->Destroy(&context, request, &reply);

    if (status.ok()) {
        return reply.status();
    }
    return grpc_error_to_leveldb_status(status);
}

leveldb::grpc::Status WriteBatchClient::Clear(const std::string& wbname)
{
    WriteBatchReq request;
    WriteBatch* wb = new WriteBatch();
    wb->set_name(wbname);
    request.set_allocated_write_batch(wb);

    WriteBatchRes reply;
    ClientContext context;

    ::grpc::Status status = stub_->Clear(&context, request, &reply);

    if (status.ok()) {
        return reply.status();
    }
    return grpc_error_to_leveldb_status(status);
}

leveldb::grpc::Status WriteBatchClient::Put(const std::string& wbname, const std::string& key, const std::string& value)
{
    WriteBatchReq request;
    WriteBatch* wb = new WriteBatch();
    wb->set_name(wbname);
    request.set_allocated_write_batch(wb);
    request.set_key(key);
    request.set_value(value);

    WriteBatchRes reply;
    ClientContext context;

    ::grpc::Status status = stub_->Put(&context, request, &reply);

    if (status.ok()) {
        return reply.status();
    }
    return grpc_error_to_leveldb_status(status);
}

leveldb::grpc::Status WriteBatchClient::Delete(const std::string& wbname, const std::string& key)
{
    WriteBatchReq request;
    WriteBatch* wb = new WriteBatch();
    wb->set_name(wbname);
    request.set_allocated_write_batch(wb);
    request.set_key(key);

    WriteBatchRes reply;
    ClientContext context;

    ::grpc::Status status = stub_->Delete(&context, request, &reply);

    if (status.ok()) {
        return reply.status();
    }
    return grpc_error_to_leveldb_status(status);
}

leveldb::grpc::Status WriteBatchClient::Append(const std::string& wbname, const std::string& sourcewbname)
{
    WriteBatchReq request;
    WriteBatch* wb = new WriteBatch();
    wb->set_name(wbname);
    request.set_allocated_write_batch(wb);
    WriteBatch* wbs = new WriteBatch();
    wbs->set_name(sourcewbname);
    request.set_allocated_write_batch_append(wbs);

    WriteBatchRes reply;
    ClientContext context;

    ::grpc::Status status = stub_->Append(&context, request, &reply);

    if (status.ok()) {
        return reply.status();
    }
    return grpc_error_to_leveldb_status(status);
}

IteratorClient::IteratorClient(std::shared_ptr<grpc::Channel> channel)
    : stub_(IteratorRPC::NewStub(channel)) {}

leveldb::grpc::Status IteratorClient::Valid(const std::string& itname, bool * valid)
{
    IteratorReq request;
    Iterator* it = new Iterator();
    it->set_name(itname);
    request.set_allocated_iterator(it);

    IteratorRes reply;
    ClientContext context;

    ::grpc::Status status = stub_->Valid(&context, request, &reply);

    if (status.ok()) {
        *valid = reply.valid();
        return reply.status();
    }
    return grpc_error_to_leveldb_status(status);
}

leveldb::grpc::Status IteratorClient::SeekToFirst(const std::string& itname)
{
    IteratorReq request;
    Iterator* it = new Iterator();
    it->set_name(itname);
    request.set_allocated_iterator(it);

    IteratorRes reply;
    ClientContext context;

    ::grpc::Status status = stub_->SeekToFirst(&context, request, &reply);

    if (status.ok()) {
        return reply.status();
    }
    return grpc_error_to_leveldb_status(status);
}

leveldb::grpc::Status IteratorClient::SeekToLast(const std::string& itname)
{
    IteratorReq request;
    Iterator* it = new Iterator();
    it->set_name(itname);
    request.set_allocated_iterator(it);

    IteratorRes reply;
    ClientContext context;

    ::grpc::Status status = stub_->SeekToLast(&context, request, &reply);

    if (status.ok()) {
        return reply.status();
    }
    return grpc_error_to_leveldb_status(status);
}

leveldb::grpc::Status IteratorClient::Seek(const std::string& itname, const std::string& target)
{
    IteratorReq request;
    Iterator* it = new Iterator();
    it->set_name(itname);
    request.set_allocated_iterator(it);
    request.set_target_key(target);

    IteratorRes reply;
    ClientContext context;

    ::grpc::Status status = stub_->Seek(&context, request, &reply);

    if (status.ok()) {
        return reply.status();
    }
    return grpc_error_to_leveldb_status(status);
}

leveldb::grpc::Status IteratorClient::Next(const std::string& itname)
{
    IteratorReq request;
    Iterator* it = new Iterator();
    it->set_name(itname);
    request.set_allocated_iterator(it);

    IteratorRes reply;
    ClientContext context;

    ::grpc::Status status = stub_->Next(&context, request, &reply);

    if (status.ok()) {
        return reply.status();
    }
    return grpc_error_to_leveldb_status(status);
}

leveldb::grpc::Status IteratorClient::Prev(const std::string& itname)
{
    IteratorReq request;
    Iterator* it = new Iterator();
    it->set_name(itname);
    request.set_allocated_iterator(it);

    IteratorRes reply;
    ClientContext context;

    ::grpc::Status status = stub_->Prev(&context, request, &reply);

    if (status.ok()) {
        return reply.status();
    }
    return grpc_error_to_leveldb_status(status);
}

leveldb::grpc::Status IteratorClient::Key(const std::string& itname, std::string* key)
{
    IteratorReq request;
    Iterator* it = new Iterator();
    it->set_name(itname);
    request.set_allocated_iterator(it);

    IteratorRes reply;
    ClientContext context;

    ::grpc::Status status = stub_->Key(&context, request, &reply);

    if (status.ok()) {
        *key = reply.key();
        return reply.status();
    }
    return grpc_error_to_leveldb_status(status);
}

leveldb::grpc::Status IteratorClient::Value(const std::string& itname, std::string* value)
{
    IteratorReq request;
    Iterator* it = new Iterator();
    it->set_name(itname);
    request.set_allocated_iterator(it);

    IteratorRes reply;
    ClientContext context;

    ::grpc::Status status = stub_->Value(&context, request, &reply);

    if (status.ok()) {
        *value = reply.value();
        return reply.status();
    }
    return grpc_error_to_leveldb_status(status);
}
