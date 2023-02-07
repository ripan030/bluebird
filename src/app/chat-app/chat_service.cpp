#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

#include <grpcpp/grpcpp.h>
#include <chat.grpc.pb.h>
#include <chat.pb.h>

using namespace std;

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerWriter;
using grpc::ServerReaderWriter;
using grpc::Status;

using chat::Chat;
using chat::LoginRequest;
using chat::LoginResponse;
using chat::LogoutRequest;
using chat::LoginResponse;
using chat::StreamRequest;
using chat::StreamResponse;
using chat::SessionStartRequest;
using chat::SessionStartResponse;
using chat::SessionStopRequest;
using chat::SessionStopResponse;
using chat::Message;

class User {
    std::string username; /* Unique alpha numeric username */
    std::vector<std::string> friends; /* User name of friends */
};

class Session {
    uint32_t sid;
    std::string username;
    ServerReaderWriter<StreamResponse, StreamRequest> *stream;
    static uint32_t n_session;
public:
    Session() = default;
    Session(uint32_t sid, std::string name,
            ServerReaderWriter<StreamResponse, StreamRequest> *stream) :
        sid{sid}, username{username}, stream{stream} {
            cout << "new session created\n";
        }

    void SetSession(uint32_t sid, std::string name,
            ServerReaderWriter<StreamResponse, StreamRequest> *stream) {
        this->sid = sid;
        this->username = name;
        this->stream = stream;
    }

    void SendStreamResponse(StreamResponse rsp) {
        stream->Write(rsp);
    }

    static uint32_t GetSessionID() {
        return ++n_session;
    }
};

uint32_t Session::n_session;

class ChatImpl final : public Chat::Service {
    std::unordered_map<ServerReaderWriter<StreamResponse, StreamRequest> *,
        std::string> stream2username;
    std::unordered_map<std::string, std::unique_ptr<Session>> sessions;

public:
    StreamResponse CreateSessonStartResponse(uint32_t sid, uint32_t status) {
        StreamResponse n;
        SessionStartResponse *rsp = new SessionStartResponse{};

        rsp->set_session_id(sid);
        rsp->set_status(status);

        n.set_allocated_start_resp(rsp);

        return n;
    }

    StreamResponse CreateSessonStopResponse(uint32_t status) {
        StreamResponse n;
        SessionStopResponse *rsp = new SessionStopResponse{};

        rsp->set_status(status);

        n.set_allocated_stop_resp(rsp);

        return n;
    }

    StreamResponse CreateUserChatMessage(Message &m) {
        StreamResponse n;
        Message *rsp = new Message{m};

        n.set_allocated_msg_resp(rsp);

        return n;
    }

    StreamResponse CreateUserChatMessage(uint32_t sender_cid, uint32_t receiver_cid,
            std::string who, std::string whom, std::string message) {
        StreamResponse n;
        Message *rsp = new Message{};

        rsp->set_sender_cid(sender_cid);
        rsp->set_receiver_cid(receiver_cid);
        rsp->set_who(who);
        rsp->set_whom(whom);
        rsp->set_msg(message);

        n.set_allocated_msg_resp(rsp);

        return n;
    }

    void ReceiveSessionStartRequest(ServerReaderWriter<StreamResponse, StreamRequest> *stream,
            SessionStartRequest m) {
        auto username{m.username()};

        std::cout << "Received Session start request from " << username << std::endl;

        auto find_stream = stream2username.find(stream);
        if (find_stream != stream2username.end()) {
            cout << "Stream already exits\n";
            stream->Write(CreateSessonStartResponse(0, 1));
            return;
        }

        cout << "New stream request, create a new session....\n";

        stream2username[stream] = username;

        uint32_t sid = Session::GetSessionID();
        auto session = std::make_unique<Session>();

        session->SetSession(sid, username, stream);

        sessions[username] = std::move(session);

        cout << "Sending session start response to user " << username << endl;
        stream->Write(CreateSessonStartResponse(0, 0));
    }

    void ReceiveSessionStopRequest(ServerReaderWriter<StreamResponse, StreamRequest> *stream,
            SessionStopRequest m) {
        auto sid{m.session_id()};
        auto username{m.username()};
        uint32_t status = 0;

        /*
         * TODO: Clear session context corresponding to sid
         */

        stream->Write(CreateSessonStopResponse(status));
    }

    void ReceiveUserChatMessage(ServerReaderWriter<StreamResponse, StreamRequest> *stream,
            Message m) {
#ifdef UNIT_TEST
        /*
         * Echo a copy to the sender for testing
         */
        std::cout << "Sending echo message from server to user " << m.who() << std::endl;
        stream->Write(CreateUserChatMessage(0, m.sender_cid(), {"Server"}, m.who(), m.msg()));
#endif

        auto s = sessions.find(m.whom());
        if (s == sessions.end()) {
            /*
             * Push notification?
             */
            std::cout << "Session doesn't exist for user " << m.whom() << std::endl;
            return;
        }

        const auto &c = s->second;

        c->SendStreamResponse(CreateUserChatMessage(m));
    }

    Status ChatStream(ServerContext *ctx,
            ServerReaderWriter<StreamResponse, StreamRequest> *stream) {
        StreamRequest req;

        while (stream->Read(&req)) {
            if (req.has_start_req()) {
                ReceiveSessionStartRequest(stream, req.start_req());
            } else if (req.has_stop_req()) {
                ReceiveSessionStopRequest(stream, req.stop_req());
            } else if (req.has_msg_req()) {
                ReceiveUserChatMessage(stream, req.msg_req());
            } else {
                std::cout << "Unknown request\n";
            }
        }

        auto find_user = stream2username.find(stream);
        if (find_user != stream2username.end()) {
            cout << "User disconnected " << find_user->second << endl;

            auto find_session = sessions.find(find_user->second);
            if (find_session != sessions.end()) {
                sessions.erase(find_session);
            }

            stream2username.erase(find_user);
        }

        cout << "Client close its write stream\n";

        return Status::OK;
    }
};

void Run() {
    const std::string address{"0.0.0.0:5000"};
    ChatImpl service;
    ServerBuilder builder;

	builder.AddListeningPort(address, grpc::InsecureServerCredentials());
	builder.RegisterService(&service);

	std::unique_ptr<Server> server(builder.BuildAndStart());
	std::cout << "Server listening on " << address << std::endl;
	server->Wait();
}

int main()
{
    Run();

    return 0;
}
