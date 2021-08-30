#include <iostream>
#include <memory>
#include <string>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include "leveldbserviceimpl.h"
#include "leveldbcontrollerimpl.h"

void RunServer()
{
    std::string server_address("0.0.0.0:50051");
    leveldb::WriteBatchController wbcontroller;
    leveldb::IteratorController itcontroller;
    leveldb::DBController dbcontroller(&wbcontroller, &itcontroller);
    leveldb::DBService dbservice(&dbcontroller);
    leveldb::WriteBatchService wbservice(&wbcontroller);
    leveldb::IteratorService itservice(&itcontroller);

    grpc::EnableDefaultHealthCheckService(true);
    grpc::ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // Register "service" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *synchronous* service.
    builder.RegisterService(&dbservice);
    builder.RegisterService(&wbservice);
    builder.RegisterService(&itservice);
    // Finally assemble the server.
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;

    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
}

int main(int argc, char** argv)
{
    RunServer();
    return 0;
}
