#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>

#include "proto/helloworld.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using helloworld::HelloRequest;
using helloworld::HelloReply;
using helloworld::Greeter;

// Logic and data behind the server's behavior.
class GreeterServiceImpl final : public Greeter::Service {
    Status SayHello(ServerContext* context, const HelloRequest* request,
        HelloReply* reply) override {
        std::string prefix("Hello ");
        std::string client_message = request->name();
        reply->set_message(prefix + client_message);
#ifdef _WIN32
        std::cout << "Server on Windows: Received message '" << client_message << "' from client." << std::endl;
        std::cout << "Server on Windows: Sending response to client: " << reply->message() << std::endl;
#elif defined(__linux__)
        std::cout << "Server on Linux: Received message '" << client_message << "' from client." << std::endl;
        std::cout << "Server on Linux: Sending response to client: " << reply->message() << std::endl;
#else
        std::cout << "Server: Received message '" << client_message << "' from client." << std::endl;
        std::cout << "Server: Sending response to client: " << reply->message() << std::endl;
#endif
        return Status::OK;
    }
};

void RunServer() {
    std::string server_address("0.0.0.0:50051");
    GreeterServiceImpl service;

    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // Register "service" as the instance through which we'll communicate with
    // clients. In this case it corresponds to a *synchronous* service.
    builder.RegisterService(&service);
    // Finally assemble the server.
    std::unique_ptr<Server> server(builder.BuildAndStart());
#ifdef _WIN32
    std::cout << "Server on Windows: Listening on " << server_address << std::endl;
#elif defined(__linux__)
    std::cout << "Server on Linux: Listening on " << server_address << std::endl;
#else
    std::cout << "Server: Listening on " << server_address << std::endl;
#endif
    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
}

int main(int argc, char** argv) {
    RunServer();
    return 0;
}
