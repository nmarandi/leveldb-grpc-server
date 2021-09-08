#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>

#include "client.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

inline void leveldbStatusCheck(leveldb::grpc::Status status)
{
    if (status.code() != leveldb::grpc::Status_Code::Status_Code_kOk) {
        std::cout << status.message() << std::endl;
        exit(-1);
    }
}

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
    WriteBatchClient wbclient(
        grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials()));
    IteratorClient itclient(
        grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials()));
    // *********************************************
    std::string db_name = "example_db";
    leveldb::grpc::Options options;
    options.set_create_if_missing(true);
    leveldbStatusCheck(dbclient.Open(db_name, options));
    leveldb::grpc::WriteOptions woptions;
    leveldbStatusCheck(dbclient.Put(db_name, woptions, "Hello", "World"));
    std::string value;
    leveldb::grpc::ReadOptions roptions;
    leveldbStatusCheck(dbclient.Get(db_name, roptions, "Hello", &value));
    std::cout << "value for \"Hello\" key: " << value << std::endl;
    //leveldbStatusCheck(dbclient.Delete(db_name, "Hello"));
    // *********************************************
    std::string wb_name = "example_wb";
    auto status = wbclient.Create(wb_name); 
    if (status.code() != leveldb::grpc::Status_Code::Status_Code_kOk && status.code() != leveldb::grpc::Status_Code::Status_Code_kInvalidArgument) {
        std::cout << status.message() << std::endl;
        exit(-1);
    }
    leveldbStatusCheck(wbclient.Put(wb_name, "FOO", "BAR"));
    leveldbStatusCheck(wbclient.Put(wb_name, "foo", "bar"));
    leveldbStatusCheck(dbclient.Write(db_name, woptions, wb_name));
    leveldbStatusCheck(dbclient.Get(db_name, roptions, "FOO", &value));
    std::cout << "value for \"FOO\" key: " << value << std::endl;
    leveldbStatusCheck(dbclient.Get(db_name, roptions, "foo", &value));
    std::cout << "value for \"foo\" key: " << value << std::endl;
    //leveldbStatusCheck(wbclient.Clear(wb_name));
    //leveldbStatusCheck(wbclient.Delete(wb_name, "FOO"));
    //leveldbStatusCheck(wbclient.Delete(wb_name, "foo"));
    //leveldbStatusCheck(dbclient.Write(db_name, wb_name));
    // *********************************************
    std::string it_name = "example_it";
    leveldb::grpc::ReadOptions ropts;
    status = dbclient.NewIterator(db_name, ropts, it_name);
    if (status.code() != leveldb::grpc::Status_Code::Status_Code_kOk && status.code() != leveldb::grpc::Status_Code::Status_Code_kInvalidArgument) {
        std::cout << status.message() << std::endl;
        exit(-1);
    }
    leveldbStatusCheck(itclient.SeekToFirst(it_name));
    bool valid;
    leveldbStatusCheck(itclient.Valid(it_name, &valid));
    std::string key;
    while (valid) {
        leveldbStatusCheck(itclient.Key(it_name, &key));
        leveldbStatusCheck(itclient.Value(it_name, &value));
        std::cout << "value for \"" << key << "\" key: " << value << std::endl;
        leveldbStatusCheck(itclient.Next(it_name));
        leveldbStatusCheck(itclient.Valid(it_name, &valid));
    }

    return 0;
}