#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>

#include "protos/leveldb.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using namespace leveldb::grpc;

inline void leveldbStatusExceptionOnError(leveldb::grpc::Status status)
{
    if (status.code() != Status_Code::Status_Code_kOk) {
        std::cout << status.message();
        exit(-1);
    }
}
class DBClient
{
public:
    DBClient(std::shared_ptr<Channel> channel)
        : stub_(DBRPC::NewStub(channel)) {}

    leveldb::grpc::Status Open(const std::string& db_name)
    {
        DBReq request;
        DB* db = new DB();
        db->set_name(db_name);
        request.set_allocated_db(db);
        Options* options = new Options();
        options->set_create_if_missing(true);
        request.set_allocated_options(options);

        DBRes reply;
        ClientContext context;

        ::grpc::Status status = stub_->Open(&context, request, &reply);

        if (status.ok()) {
            return reply.status();
        }
        leveldb::grpc::Status leveldbstatus;
        leveldbstatus.set_code(Status_Code::Status_Code_kIOError);
        leveldbstatus.set_message(status.error_message());
        return leveldbstatus;
    }

    leveldb::grpc::Status Put(const std::string& db_name, const std::string& key, const std::string& value)
    {
        DBReq request;
        DB* db = new DB();
        db->set_name(db_name);
        request.set_allocated_db(db);
        request.set_key(key);
        request.set_value(value);

        DBRes reply;
        ClientContext context;

        ::grpc::Status status = stub_->Put(&context, request, &reply);

        if (status.ok()) {
            return reply.status();
        }
        leveldb::grpc::Status leveldbstatus;
        leveldbstatus.set_code(Status_Code::Status_Code_kIOError);
        leveldbstatus.set_message(status.error_message());
        return leveldbstatus;
    }

    leveldb::grpc::Status Get(const std::string& db_name, const std::string& key, std::string* value)
    {
        DBReq request;
        DB* db = new DB();
        db->set_name(db_name);
        request.set_allocated_db(db);
        request.set_key(key);

        DBRes reply;
        ClientContext context;

        ::grpc::Status status = stub_->Get(&context, request, &reply);

        if (status.ok()) {

            if (reply.status().code() == Status_Code::Status_Code_kOk) {
                *value = reply.value();
            }
            return reply.status();
        }
        leveldb::grpc::Status leveldbstatus;
        leveldbstatus.set_code(Status_Code::Status_Code_kIOError);
        leveldbstatus.set_message(status.error_message());
        return leveldbstatus;
    }

    leveldb::grpc::Status Delete(const std::string& db_name, const std::string& key)
    {
        DBReq request;
        DB* db = new DB();
        db->set_name(db_name);
        request.set_allocated_db(db);
        request.set_key(key);

        DBRes reply;
        ClientContext context;

        ::grpc::Status status = stub_->Delete(&context, request, &reply);

        if (status.ok()) {
            return reply.status();
        }
        leveldb::grpc::Status leveldbstatus;
        leveldbstatus.set_code(Status_Code::Status_Code_kIOError);
        leveldbstatus.set_message(status.error_message());
        return leveldbstatus;
    }

private:
    std::unique_ptr<DBRPC::Stub> stub_;
};

int main(int argc, char** argv)
{
    std::string target_str;
    std::string arg_str("--target");
    if (argc > 1) {
        std::string arg_val = argv[1];
        size_t start_pos = arg_val.find(arg_str);
        if (start_pos != std::string::npos) {
            start_pos += arg_str.size();
            if (arg_val[start_pos] == '=') {
                target_str = arg_val.substr(start_pos + 1);
            } else {
                std::cout << "The only correct argument syntax is --target="
                          << std::endl;
                return 0;
            }
        } else {
            std::cout << "The only acceptable argument is --target=" << std::endl;
            return 0;
        }
    } else {
        target_str = "localhost:50051";
    }
    DBClient dbclient(
        grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials()));
    std::string db_name = "example_db";
    leveldbStatusExceptionOnError(dbclient.Open(db_name));
    leveldbStatusExceptionOnError(dbclient.Put(db_name, "Hello", "World"));
    std::string value;
    leveldbStatusExceptionOnError(dbclient.Get(db_name, "Hello", &value));
    std::cout << "value for \"Hello\" key: " << value << std::endl;
    leveldbStatusExceptionOnError(dbclient.Delete(db_name, "Hello"));

    return 0;
}