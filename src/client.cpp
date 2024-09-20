#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>

#include "proto/helloworld.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using helloworld::HelloRequest;
using helloworld::HelloReply;
using helloworld::Greeter;

class GreeterClient {
public:
    GreeterClient(std::shared_ptr<Channel> channel)
        : stub_(Greeter::NewStub(channel)) {}

    // Assembles the client's payload, sends it and presents the response back
    // from the server.
    std::string SayHello(const std::string& user) {
#ifdef _WIN32
        std::cout << "Client on Windows: Sending Hello message to the server..." << std::endl;
#elif __linux__
        std::cout << "Client on Linux: Sending Hello message to the server..." << std::endl;
#else
        std::cout << "Client on Linux: Sending Hello message to the server..." << std::endl;
#endif

        // Data we are sending to the server.
        HelloRequest request;
        request.set_name(user);

        // Container for the data we expect from the server.
        HelloReply reply;

        // Context for the client. It could be used to convey extra information to
        // the server and/or tweak certain RPC behaviors.
        ClientContext context;

        // The actual RPC.
        Status status = stub_->SayHello(&context, request, &reply);

        // Act upon its status.
        if (status.ok()) {
#ifdef _WIN32
            std::cout << "Client on Windows: Received response from server: " << reply.message() << std::endl;
#elif __linux__
            std::cout << "Client on Linux: Received response from server: " << reply.message() << std::endl;
#else
            std::cout << "Client: Received response from server: " << reply.message() << std::endl;
#endif
            return reply.message();
        }
        else {
#ifdef _WIN32
            std::cout << "Client on Windows: RPC failed with code " << status.error_code() << ": " << status.error_message() << std::endl;
#elif __linux__
            std::cout << "Client on Linux: RPC failed with code " << status.error_code() << ": " << status.error_message() << std::endl;
#else
            std::cout << "Client: RPC failed with code " << status.error_code() << ": " << status.error_message() << std::endl;
#endif
            return "RPC failed";
        }
    }

private:
    std::unique_ptr<Greeter::Stub> stub_;
};

int main(int argc, char** argv) {
    // Determine the target server address, client.exe --target=172.xx.xx.xx:50051
    std::string target_str;
    std::string arg_str("--target");
    if (argc > 1) {
        std::string arg_val = argv[1];
        size_t start_pos = arg_val.find(arg_str);
        if (start_pos != std::string::npos) {
            start_pos += arg_str.size();
            if (arg_val[start_pos] == '=') {
                target_str = arg_val.substr(start_pos + 1);
            }
            else {
                std::cout << "The only correct argument syntax is --target=" << std::endl;
                return 0;
            }
        }
        else {
            std::cout << "The only acceptable argument is --target=" << std::endl;
            return 0;
        }
    }
    else {
#ifdef _WIN32
        target_str = "172.xx.xx.xx:50051";
#elif __linux__
        target_str = "172.xx.xx.xx:50051";
#else
        target_str = "127.0.0.1:50051";
#endif
    }

#ifdef _WIN32
    std::cout << "Client on Windows: Connecting to server at " << target_str << std::endl;
#elif __linux__
    std::cout << "Client on Linux: Connecting to server at " << target_str << std::endl;
#else
    std::cout << "Client: Connecting to server at " << target_str << std::endl;
#endif

    // Instantiate the client. It requires a channel, out of which the actual RPCs
    // are created. This channel models a connection to an endpoint specified by
    // the target.
    GreeterClient greeter(grpc::CreateChannel(
        target_str, grpc::InsecureChannelCredentials()));

#ifdef _WIN32
    std::string user("world from Windows");
#elif __linux__
    std::string user("world from Linux");
#else
    std::string user("world");
#endif

    std::string reply = greeter.SayHello(user);

    // Log the result of the RPC
#ifdef _WIN32
    std::cout << "Client on Windows: Greeter received: " << reply << std::endl;
#elif __linux__
    std::cout << "Client on Linux: Greeter received: " << reply << std::endl;
#else
    std::cout << "Client: Greeter received: " << reply << std::endl;
#endif

    return 0;
}
