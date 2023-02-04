#include <iostream>
#include <string>

#include <grpcpp/grpcpp.h>
#include "calculator.grpc.pb.h"
#include "calculator.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using calculator::Calc;
using calculator::CalcRequest;
using calculator::CalcReply;

class CalcImpl final : public Calc::Service {
    Status sendRequest(
        ServerContext *context,
        const CalcRequest *req,
        CalcReply *reply
    ) override {
        int result = 0;

        if (req->has_add_req()) {
            result = req->add_req().a() + req->add_req().b();
        } else if (req->has_del_req()) {
            result = req->del_req().a() - req->del_req().b();
        } else {
            std::cout << "Unsupported operation\n";
        }

        reply->set_result(result);

        return Status::OK;
    }
};

void Run() {
    std::string address("0.0.0.0:5000");
    CalcImpl service;
    ServerBuilder builder;

    builder.AddListeningPort(address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on port: " << address << std::endl;

    server->Wait();
}

int main()
{
    Run();

    return 0;
}
