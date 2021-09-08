#ifndef CLIENT_H
#define CLIENT_H

#include "protos/leveldb.grpc.pb.h"
#include <grpcpp/grpcpp.h>

class DBClient
{
public:
    DBClient(std::shared_ptr<grpc::Channel> channel);
    leveldb::grpc::Status Open(const std::string& dbname, const leveldb::grpc::Options& options);
    leveldb::grpc::Status Close(const std::string& dbname);
    leveldb::grpc::Status Put(const std::string& dbname, const leveldb::grpc::WriteOptions& options, const std::string& key, const std::string& value);
    leveldb::grpc::Status Delete(const std::string& dbname, const leveldb::grpc::WriteOptions& options, const std::string& key);
    leveldb::grpc::Status Write(const std::string& dbname, const leveldb::grpc::WriteOptions& options, const std::string& wbname);
    leveldb::grpc::Status Get(const std::string& dbname, const leveldb::grpc::ReadOptions& options, const std::string& key, std::string* value);
    leveldb::grpc::Status NewIterator(const std::string& dbname, const leveldb::grpc::ReadOptions& options, const std::string& itname);
    leveldb::grpc::Status DestroyIterator(const std::string& dbname, const std::string& itname);
    leveldb::grpc::Status Destroy(const std::string& dbname, const leveldb::grpc::Options& options);
    leveldb::grpc::Status Repair(const std::string& dbname, const leveldb::grpc::Options& options);

private:
    std::unique_ptr<leveldb::grpc::DBRPC::Stub> stub_;
};

class WriteBatchClient
{
public:
    WriteBatchClient(std::shared_ptr<grpc::Channel> channel);
    leveldb::grpc::Status Create(const std::string& wbname);
    leveldb::grpc::Status Destroy(const std::string& wbname);
    leveldb::grpc::Status Clear(const std::string& wbname);
    leveldb::grpc::Status Put(const std::string& wbname, const std::string& key, const std::string& value);
    leveldb::grpc::Status Delete(const std::string& wbname, const std::string& key);
    leveldb::grpc::Status Append(const std::string& wbname, const std::string& sourcewbname);

private:
    std::unique_ptr<leveldb::grpc::WriteBatchRPC::Stub> stub_;
};

class IteratorClient
{
public:
    IteratorClient(std::shared_ptr<grpc::Channel> channel);
    leveldb::grpc::Status Valid(const std::string& itname, bool *valid);
    leveldb::grpc::Status SeekToFirst(const std::string& itname);
    leveldb::grpc::Status SeekToLast(const std::string& itname);
    leveldb::grpc::Status Seek(const std::string& itname, const std::string& target);
    leveldb::grpc::Status Next(const std::string& itname);
    leveldb::grpc::Status Prev(const std::string& itname);
    leveldb::grpc::Status Key(const std::string& itname, std::string* key);
    leveldb::grpc::Status Value(const std::string& itname, std::string* value);

private:
    std::unique_ptr<leveldb::grpc::IteratorRPC::Stub> stub_;

};

#endif // CLIENT_H
