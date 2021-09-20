#ifndef LEVELDBSERVICEIMPL_H
#define LEVELDBSERVICEIMPL_H

#include "protos/leveldb.grpc.pb.h"
#include "leveldbcontroller.h"

namespace leveldb
{

class DBService final : public grpc::DBRPC::Service
{
public:
    DBService(IDBController* dbcontroller);
    virtual ~DBService();
    virtual ::grpc::Status Open(::grpc::ServerContext* context, const grpc::DBReq* request, grpc::DBRes* response)override;
    virtual ::grpc::Status Close(::grpc::ServerContext* context, const grpc::DBReq* request, grpc::DBRes* response)override;
    virtual ::grpc::Status Put(::grpc::ServerContext* context, const grpc::DBReq* request, grpc::DBRes* response)override;
    virtual ::grpc::Status Delete(::grpc::ServerContext* context, const grpc::DBReq* request, grpc::DBRes* response)override;
    virtual ::grpc::Status Write(::grpc::ServerContext* context, const grpc::DBReq* request, grpc::DBRes* response)override;
    virtual ::grpc::Status Get(::grpc::ServerContext* context, const grpc::DBReq* request, grpc::DBRes* response)override;
    virtual ::grpc::Status NewIterator(::grpc::ServerContext* context, const grpc::DBReq* request, grpc::DBRes* response)override;
    virtual ::grpc::Status DestroyIterator(::grpc::ServerContext* context, const grpc::DBReq* request, grpc::DBRes* response)override;
    virtual ::grpc::Status Destroy(::grpc::ServerContext* context, const grpc::DBReq* request, grpc::DBRes* response)override;
    virtual ::grpc::Status Repair(::grpc::ServerContext* context, const grpc::DBReq* request, grpc::DBRes* response)override;
private:
    IDBController* _dbcontroller;
};

class WriteBatchService final : public grpc::WriteBatchRPC::Service
{
public:
    WriteBatchService(IWriteBatchController* wbcontroller);
    virtual ~WriteBatchService();
    virtual ::grpc::Status Create(::grpc::ServerContext* context, const grpc::WriteBatchReq* request, grpc::WriteBatchRes* response)override;
    virtual ::grpc::Status Destroy(::grpc::ServerContext* context, const grpc::WriteBatchReq* request, grpc::WriteBatchRes* response)override;
    virtual ::grpc::Status Clear(::grpc::ServerContext* context, const grpc::WriteBatchReq* request, grpc::WriteBatchRes* response)override;
    virtual ::grpc::Status Put(::grpc::ServerContext* context, const grpc::WriteBatchReq* request, grpc::WriteBatchRes* response)override;
    virtual ::grpc::Status Delete(::grpc::ServerContext* context, const grpc::WriteBatchReq* request, grpc::WriteBatchRes* response)override;
    virtual ::grpc::Status Append(::grpc::ServerContext* context, const grpc::WriteBatchReq* request, grpc::WriteBatchRes* response)override;
private:
    IWriteBatchController* _wbcontroller;
};

class IteratorService final : public grpc::IteratorRPC::Service
{
public:
    IteratorService(IIteratorController* itcontroller);
    virtual ~IteratorService();
    virtual ::grpc::Status Valid(::grpc::ServerContext* context, const grpc::IteratorReq* request, grpc::IteratorRes* response)override;
    virtual ::grpc::Status SeekToFirst(::grpc::ServerContext* context, const grpc::IteratorReq* request, grpc::IteratorRes* response)override;
    virtual ::grpc::Status SeekToLast(::grpc::ServerContext* context, const grpc::IteratorReq* request, grpc::IteratorRes* response)override;
    virtual ::grpc::Status Seek(::grpc::ServerContext* context, const grpc::IteratorReq* request, grpc::IteratorRes* response)override;
    virtual ::grpc::Status Next(::grpc::ServerContext* context, const grpc::IteratorReq* request, grpc::IteratorRes* response)override;
    virtual ::grpc::Status Prev(::grpc::ServerContext* context, const grpc::IteratorReq* request, grpc::IteratorRes* response)override;
    virtual ::grpc::Status Key(::grpc::ServerContext* context, const grpc::IteratorReq* request, grpc::IteratorRes* response)override;
    virtual ::grpc::Status Value(::grpc::ServerContext* context, const grpc::IteratorReq* request, grpc::IteratorRes* response)override;
    virtual ::grpc::Status Iterate(::grpc::ServerContext* context, const grpc::IteratorReq* request, grpc::IteratorRes* response)override;
private:
    IIteratorController* _itcontroller;
};
}

#endif // LEVELDBSERVICEIMPL_H
