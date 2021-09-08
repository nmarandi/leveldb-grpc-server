#include "leveldbserviceimpl.h"
#include "leveldb/db.h"

namespace leveldb
{

DBService::DBService(IDBController* dbcontroller)
    : Service(), _dbcontroller(dbcontroller)
{

}

DBService::~DBService()
{

}

Options createOptionFromMessage(const grpc::Options& options)
{
    Options options_;
    options_.create_if_missing = options.create_if_missing();
    options_.error_if_exists = options.error_if_exists();
    options_.paranoid_checks = options.paranoid_checks();
    options_.write_buffer_size = options.write_buffer_size();
    options_.max_open_files = options.max_open_files();
    options_.block_size = options.block_size();
    options_.block_restart_interval = options.block_restart_interval();
    options_.max_file_size = options.max_file_size();
    options_.compression = (CompressionType)options.compression();
    options_.reuse_logs = options.reuse_logs();
    return options_;
}

WriteOptions createWriteOptionFromMessage(const grpc::WriteOptions& write_options)
{
    WriteOptions write_options_;
    write_options_.sync = write_options.sync();
    return write_options_;
}

ReadOptions createReadOptionFromMessage(const grpc::ReadOptions& read_options)
{
    ReadOptions read_options_;
    read_options_.fill_cache = read_options.fill_cache();
    read_options_.verify_checksums = read_options.verify_checksums();
    return read_options_;
}

grpc::Status* createGrpcStatusFromStatus(const Status& status)
{
    grpc::Status* status_ = new grpc::Status();
    if (status.ok()) {
        status_->set_code(grpc::Status::Code::Status_Code_kOk);
    } else if (status.IsNotFound()) {
        status_->set_code(grpc::Status::Code::Status_Code_kNotFound);
    } else if (status.IsCorruption()) {
        status_->set_code(grpc::Status::Code::Status_Code_kCorruption);
    } else if (status.IsIOError()) {
        status_->set_code(grpc::Status::Code::Status_Code_kIOError);
    } else if (status.IsNotSupportedError()) {
        status_->set_code(grpc::Status::Code::Status_Code_kNotSupported);
    } else if (status.IsInvalidArgument()) {
        status_->set_code(grpc::Status::Code::Status_Code_kInvalidArgument);
    }
    status_->set_message(status.ToString());
    return status_;
}

::grpc::Status DBService::Open(::grpc::ServerContext* context, const grpc::DBReq* request, grpc::DBRes* response)
{
    Options options = createOptionFromMessage(request->options());
    auto status = _dbcontroller->Open(request->db().name(), options);
    response->set_allocated_status(createGrpcStatusFromStatus(status));
    return ::grpc::Status::OK;
}

::grpc::Status DBService::Close(::grpc::ServerContext* context, const grpc::DBReq* request, grpc::DBRes* response)
{
    auto status = _dbcontroller->Close(request->db().name());
    response->set_allocated_status(createGrpcStatusFromStatus(status));
    return ::grpc::Status::OK;
}

::grpc::Status DBService::Put(::grpc::ServerContext* context, const grpc::DBReq* request, grpc::DBRes* response)
{
    WriteOptions write_option = createWriteOptionFromMessage(request->write_options());
    auto status = _dbcontroller->Put(request->db().name(), write_option, request->key(), request->value());
    response->set_allocated_status(createGrpcStatusFromStatus(status));
    return ::grpc::Status::OK;
}

::grpc::Status DBService::Delete(::grpc::ServerContext* context, const grpc::DBReq* request, grpc::DBRes* response)
{
    WriteOptions write_option = createWriteOptionFromMessage(request->write_options());
    auto status = _dbcontroller->Delete(request->db().name(), write_option, request->key());
    response->set_allocated_status(createGrpcStatusFromStatus(status));
    return ::grpc::Status::OK;
}

::grpc::Status DBService::Write(::grpc::ServerContext* context, const grpc::DBReq* request, grpc::DBRes* response)
{
    WriteOptions write_option = createWriteOptionFromMessage(request->write_options());
    auto status = _dbcontroller->Write(request->db().name(), write_option, request->write_batch().name());
    response->set_allocated_status(createGrpcStatusFromStatus(status));
    return ::grpc::Status::OK;
}

::grpc::Status DBService::Get(::grpc::ServerContext* context, const grpc::DBReq* request, grpc::DBRes* response)
{
    ReadOptions read_option = createReadOptionFromMessage(request->read_options());
    std::string value;
    auto status = _dbcontroller->Get(request->db().name(), read_option, request->key(), &value);
    response->set_allocated_status(createGrpcStatusFromStatus(status));
    response->set_value(value);
    return ::grpc::Status::OK;
}

::grpc::Status DBService::NewIterator(::grpc::ServerContext* context, const grpc::DBReq* request, grpc::DBRes* response)
{
    ReadOptions read_option = createReadOptionFromMessage(request->read_options());
    auto status = _dbcontroller->NewIterator(request->db().name(), read_option, request->iterator().name());
    grpc::Iterator* iterator = new grpc::Iterator();
    iterator->set_name(request->iterator().name());
    response->set_allocated_status(createGrpcStatusFromStatus(status));
    response->set_allocated_iterator(iterator);
    return ::grpc::Status::OK;
}

::grpc::Status DBService::DestroyIterator(::grpc::ServerContext* context, const grpc::DBReq* request, grpc::DBRes* response)
{
    auto status = _dbcontroller->DestroyIterator(request->db().name(), request->iterator().name());
    response->set_allocated_status(createGrpcStatusFromStatus(status));
    return ::grpc::Status::OK;
}

::grpc::Status DBService::Destroy(::grpc::ServerContext* context, const grpc::DBReq* request, grpc::DBRes* response)
{
    Options options = createOptionFromMessage(request->options());
    auto status = _dbcontroller->Destroy(request->db().name(), options);
    response->set_allocated_status(createGrpcStatusFromStatus(status));
    return ::grpc::Status::OK;
}

::grpc::Status DBService::Repair(::grpc::ServerContext* context, const grpc::DBReq* request, grpc::DBRes* response)
{
    Options options = createOptionFromMessage(request->options());
    auto status = _dbcontroller->Repair(request->db().name(), options);
    response->set_allocated_status(createGrpcStatusFromStatus(status));
    return ::grpc::Status::OK;
}

WriteBatchService::WriteBatchService(IWriteBatchController* wbcontroller)
    : Service(), _wbcontroller(wbcontroller)
{

}

WriteBatchService::~WriteBatchService()
{

}

::grpc::Status WriteBatchService::Create(::grpc::ServerContext* context, const grpc::WriteBatchReq* request, grpc::WriteBatchRes* response)
{
    auto status = _wbcontroller->Create(request->write_batch().name());
    response->set_allocated_status(createGrpcStatusFromStatus(status));
    return ::grpc::Status::OK;
}

::grpc::Status WriteBatchService::Destroy(::grpc::ServerContext* context, const grpc::WriteBatchReq* request, grpc::WriteBatchRes* response)
{
    auto status = _wbcontroller->Destroy(request->write_batch().name());
    response->set_allocated_status(createGrpcStatusFromStatus(status));
    return ::grpc::Status::OK;
}

::grpc::Status WriteBatchService::Clear(::grpc::ServerContext* context, const grpc::WriteBatchReq* request, grpc::WriteBatchRes* response)
{
    auto status = _wbcontroller->Clear(request->write_batch().name());
    response->set_allocated_status(createGrpcStatusFromStatus(status));
    return ::grpc::Status::OK;
}

::grpc::Status WriteBatchService::Put(::grpc::ServerContext* context, const grpc::WriteBatchReq* request, grpc::WriteBatchRes* response)
{
    auto status = _wbcontroller->Put(request->write_batch().name(), request->key(), request->value());
    response->set_allocated_status(createGrpcStatusFromStatus(status));
    return ::grpc::Status::OK;
}

::grpc::Status WriteBatchService::Delete(::grpc::ServerContext* context, const grpc::WriteBatchReq* request, grpc::WriteBatchRes* response)
{
    auto status = _wbcontroller->Delete(request->write_batch().name(), request->key());
    response->set_allocated_status(createGrpcStatusFromStatus(status));
    return ::grpc::Status::OK;
}

::grpc::Status WriteBatchService::Append(::grpc::ServerContext* context, const grpc::WriteBatchReq* request, grpc::WriteBatchRes* response)
{
    auto status = _wbcontroller->Append(request->write_batch().name(), request->write_batch_append().name());
    response->set_allocated_status(createGrpcStatusFromStatus(status));
    return ::grpc::Status::OK;
}

IteratorService::IteratorService(IIteratorController* itcontroller)
    : Service(), _itcontroller(itcontroller)
{

}

IteratorService::~IteratorService()
{

}

::grpc::Status IteratorService::Valid(::grpc::ServerContext* context, const grpc::IteratorReq* request, grpc::IteratorRes* response)
{
    bool valid;
    auto status = _itcontroller->Valid(request->iterator().name(), &valid);
    response->set_allocated_status(createGrpcStatusFromStatus(status));
    response->set_valid(valid);
    return ::grpc::Status::OK;
}

::grpc::Status IteratorService::SeekToFirst(::grpc::ServerContext* context, const grpc::IteratorReq* request, grpc::IteratorRes* response)
{
    auto status = _itcontroller->SeekToFirst(request->iterator().name());
    response->set_allocated_status(createGrpcStatusFromStatus(status));
    return ::grpc::Status::OK;
}

::grpc::Status IteratorService::SeekToLast(::grpc::ServerContext* context, const grpc::IteratorReq* request, grpc::IteratorRes* response)
{
    auto status = _itcontroller->SeekToLast(request->iterator().name());
    response->set_allocated_status(createGrpcStatusFromStatus(status));
    return ::grpc::Status::OK;
}

::grpc::Status IteratorService::Seek(::grpc::ServerContext* context, const grpc::IteratorReq* request, grpc::IteratorRes* response)
{
    auto status = _itcontroller->Seek(request->iterator().name(), request->target_key());
    response->set_allocated_status(createGrpcStatusFromStatus(status));
    return ::grpc::Status::OK;
}

::grpc::Status IteratorService::Next(::grpc::ServerContext* context, const grpc::IteratorReq* request, grpc::IteratorRes* response)
{
    auto status = _itcontroller->Next(request->iterator().name());
    response->set_allocated_status(createGrpcStatusFromStatus(status));
    return ::grpc::Status::OK;
}

::grpc::Status IteratorService::Prev(::grpc::ServerContext* context, const grpc::IteratorReq* request, grpc::IteratorRes* response)
{
    auto status = _itcontroller->Prev(request->iterator().name());
    response->set_allocated_status(createGrpcStatusFromStatus(status));
    return ::grpc::Status::OK;
}

::grpc::Status IteratorService::Key(::grpc::ServerContext* context, const grpc::IteratorReq* request, grpc::IteratorRes* response)
{
    std::string key;
    auto status = _itcontroller->Key(request->iterator().name(), &key);
    response->set_allocated_status(createGrpcStatusFromStatus(status));
    response->set_key(key);
    return ::grpc::Status::OK;
}

::grpc::Status IteratorService::Value(::grpc::ServerContext* context, const grpc::IteratorReq* request, grpc::IteratorRes* response)
{
    std::string value;
    auto status = _itcontroller->Key(request->iterator().name(), &value);
    response->set_allocated_status(createGrpcStatusFromStatus(status));
    response->set_value(value);
    return ::grpc::Status::OK;
}

} // namespace leveldb
