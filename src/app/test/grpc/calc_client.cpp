#include <string>

#include <grpcpp/grpcpp.h>
#include "calculator.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using calculator::Calc;
using calculator::CalcRequest;
using calculator::CalcReply;
using calculator::AddReq;
using calculator::DeleteReq;

class CalcClient {
    public:
        CalcClient(std::shared_ptr<Channel> channel) : stub_(Calc::NewStub(channel)) {}

        CalcRequest CreateCalcAddRequest(int a, int b) {
            CalcRequest n;
            AddReq *req = new AddReq{};

            req->set_a(a);
            req->set_b(b);

            n.set_allocated_add_req(req);

            return n;
        }

        CalcRequest CreateCalcDelRequest(int a, int b) {
            CalcRequest n;
            DeleteReq *req = new DeleteReq{};

            req->set_a(a);
            req->set_b(b);

            n.set_allocated_del_req(req);

            return n;
        }

        Status sendRequest(CalcRequest &req, CalcReply &reply) {
            ClientContext context;

            return stub_->sendRequest(&context, req, &reply);
        }

    private:
        std::unique_ptr<Calc::Stub> stub_;
};

void Run() {
    std::string address("0.0.0.0:5000");
    CalcClient client(
        grpc::CreateChannel(
            address,
            grpc::InsecureChannelCredentials()
        )
    );

    Status status;
    CalcRequest req;
    CalcReply reply;

    int a = 5, b = 2;

    req = client.CreateCalcAddRequest(a, b);

    status = client.sendRequest(req, reply);
    if (status.ok()) {
        std::cout << a << " + " << b << " = " << reply.result() << std::endl;
    } else {
        std::cout << status.error_code() << ": " << status.error_message() << std::endl;
    }

    req = client.CreateCalcDelRequest(a, b);
    status = client.sendRequest(req, reply);
    if (status.ok()) {
        std::cout << a << " - " << b << " = " << reply.result() << std::endl;
    } else {
        std::cout << status.error_code() << ": " << status.error_message() << std::endl;
    }
}

int main()
{
    Run();

    return 0;
}
