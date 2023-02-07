#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <thread>
#include <chrono>

#include <grpcpp/grpcpp.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>

#include <chat.grpc.pb.h>
#include <chat.pb.h>

#include <sdbus-c++/sdbus-c++.h>

using namespace std;

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::ClientWriter;
using grpc::ClientReaderWriter;
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

class Profile {
    std::string username;
    std::string displayName;
public:
    Profile(std::string username, std::string displayName) :
        username{username}, displayName{displayName} {}
    std::string GetUserName() const {
        return username;
    }
    std::string GetDisplayName() const {
        return displayName;
    }
};

struct ChatMessage {
    uint32_t cid;
    uint32_t whom_cid;
    std::string who;
    std::string whom;
    std::string message;

    ChatMessage(uint32_t cid, uint32_t whom_cid, string who, string whom, string message) :
        cid{cid}, whom_cid{whom_cid}, who{who}, whom{whom}, message{message} {}
};

class Chatting {
    uint32_t cid; /* Unique chat ID local to the system */
    uint32_t whom_cid;
    std::string who;
    std::string whom;
    std::vector<ChatMessage> messages; /* Add timestamp */
    static uint32_t totalChats;
public:
    Chatting(uint32_t cid, std::string who, std::string whom) :
        cid{cid}, whom_cid{0}, who{who}, whom{whom} {}

    void set_whom_cid(const uint32_t cid) {
        whom_cid = cid;
    }

    std::string FormatMessage(const ChatMessage &cm) {
        return "[" + cm.who + " " + cm.whom + "] " + cm.message;
    }

    void View() {
        cout << "\n++++++++++++++++++++++++++++++++++++++++\n";
        cout << "Chat Window [" << whom << "]\n";
        for (const auto &cm : messages) {
            cout << FormatMessage(cm) << endl;
        }
        cout << "\n++++++++++++++++++++++++++++++++++++++++\n";
    }

    void Add(ChatMessage &cm) {
        messages.push_back(cm);
    }

    ChatMessage CreateSenderChatMessage(std::string &message) {
        ChatMessage cm{cid, whom_cid, who, whom, message};
        return cm;
    }

    ChatMessage CreateReceiverChatMessage(std::string &message) {
        ChatMessage cm{whom_cid, cid, whom, who, message};
        return cm;
    }

    StreamRequest CreateStreamChatMessage(ChatMessage &cm) {
        StreamRequest req;
        Message *m = new Message{};

        m->set_sender_cid(cm.cid);
        m->set_receiver_cid(cm.whom_cid);
        m->set_who(cm.who);
        m->set_whom(cm.whom);
        m->set_msg(cm.message);

        req.set_allocated_msg_req(m);

        return req;
    }

    static uint32_t GetChatID() {
        return ++totalChats;
    }
};

uint32_t Chatting::totalChats = 0;

class ChatClient {
        std::unique_ptr<Chat::Stub> stub_;
        std::unique_ptr<ClientContext> context;
        std::shared_ptr<Channel> channel_;
        std::unique_ptr<ClientReaderWriter<StreamRequest, StreamResponse>> stream_;
        std::unique_ptr<Profile> profile;
        std::vector<std::string> activeFriends;
        std::unordered_map<std::string, std::unique_ptr<Chatting>> activeChats;
public:
    ChatClient(std::shared_ptr<Channel> channel) :
        stub_{Chat::NewStub(channel)},
        channel_{channel},
        profile{} {

        context = std::make_unique<grpc::ClientContext>();
        stream_ = stub_->ChatStream(context.get());

#ifdef UNIT_TEST
        std::unique_ptr<Profile> p = std::make_unique<Profile>("bluebard", "bluebird");
        profile = std::move(p);

        activeFriends.push_back("blue001");
        activeFriends.push_back("green30");
        activeFriends.push_back("orange");
#endif
    }

    void ResetStream() {
        context->TryCancel();
        stream_->Finish();

        context = std::make_unique<grpc::ClientContext>();
        stream_ = stub_->ChatStream(context.get());
    }

    bool SignUp(std::string username, std::string displayName, std::string password) {
        /*
         * Create Profile
         */
        std::unique_ptr<Profile> p = std::make_unique<Profile>(username, displayName);
        profile = std::move(p);

        cout << "SignUp: " << profile->GetDisplayName() << endl;

        return true;
    }

    bool SendSessionStartRequest(std::string username) {
        StreamRequest req;
        SessionStartRequest *m = new SessionStartRequest{};

        m->set_username(username);

        req.set_allocated_start_req(m);

        stream_->Write(req);

        return true;
    }

    bool ReceiveSessionStartResponse(SessionStartResponse m) {

        cout << "Session start response : sid = " << m.session_id() << " status = " << m.status() << endl;

        return true;
    }

    bool SendSessionStopRequest(uint32_t sid, std::string username) {
        StreamRequest req;
        SessionStopRequest *m = new SessionStopRequest{};

        m->set_session_id(sid);
        m->set_username(username);

        req.set_allocated_stop_req(m);

        stream_->Write(req);

        return true;
    }

    bool ReceiveSessionStopResponse(SessionStopResponse m) {

        cout << "Session stop response : status = " << m.status() << endl;

        return true;
    }

    bool Login(std::string username, std::string password) {
        /*
         * Load profile
         */
        if (!profile) {
            return false;
        }

        /*
         * Send start session request to the server
         */
        cout << "Login: Sending session start request for user " << username << endl;
        SendSessionStartRequest(profile->GetUserName());

        return true;
    }

    void Logout() {
        if (!profile) {
            return;
        }

        /*
         * Send stop session request to the server
         */

        cout << "LogOut: " << profile->GetDisplayName() << endl;
    }

    std::string GetActiveFriendsList() {
        std::string users;
        for (const auto &user : activeFriends) {
           users += user;
           users += " ";
        }

        return users;
    }

    bool SendUserChatMessage(std::string whom, std::string message) {
        if (!profile) {
            return false;
        }

        /*
         * Find chats using whom
         */
        auto chat = activeChats.find(whom);
        if (chat == activeChats.end()) {
            auto who = profile->GetUserName();
            activeChats[whom] = std::make_unique<Chatting>(Chatting::GetChatID(), who, whom);
        }

        const auto &c = activeChats[whom];
        ChatMessage cm{c->CreateSenderChatMessage(message)};

        cout << c->FormatMessage(cm) << endl;

        c->Add(cm);
        stream_->Write(c->CreateStreamChatMessage(cm));

        return true;
    }

    void ReceiveUserChatMessage(Message m) {
        auto who{profile->GetUserName()};
        auto whom{m.who()};
        auto cid{m.receiver_cid()};
        auto whom_cid{m.sender_cid()};
        auto message{m.msg()};

        auto chat = activeChats.find(whom);
        if (chat == activeChats.end()) {
            activeChats[whom] = std::make_unique<Chatting>(Chatting::GetChatID(), who, whom);
        }

        const auto &c = activeChats[whom];

        c->set_whom_cid(whom_cid);

        ChatMessage cm{c->CreateReceiverChatMessage(message)};

        cout << c->FormatMessage(cm) << endl;

        c->Add(cm);

        c->View();
    }

    int GetChannelState() const {
        return channel_->GetState(true);
    }

    void StartChat() {
        /*
         * Create a thread to send messages
         */
        std::thread writer([this]() {
            /*
             * Setup sdbus event loop
             */
            const char *serviceName{"org.bluebird.chatapp"};
            const char *objectPath{"/org/bluebird/chatapp"};
            const char* interfaceName = "org.bluebird.Chatapp";

            auto connection = sdbus::createSessionBusConnection(serviceName);
            auto chatapp = sdbus::createObject(*connection, objectPath);

            chatapp->registerMethod("Signup").onInterface(interfaceName).implementedAs([this](std::string username, std::string displayName, std::string password)->bool {
                return SignUp(username, displayName, password);
            });
            chatapp->registerMethod("Login").onInterface(interfaceName).implementedAs([this](std::string username, std::string password)->bool {
                return Login(username, password);
            });

            chatapp->registerMethod("Logout").onInterface(interfaceName).implementedAs([this](){
                Logout();
            });

            chatapp->registerMethod("GetActiveFriendsList").onInterface(interfaceName).implementedAs([this]()->std::string {
                return GetActiveFriendsList();
            });

            chatapp->registerMethod("ChatOne").onInterface(interfaceName).implementedAs([this](std::string whom, std::string message)->bool {
                return SendUserChatMessage(whom, message);
            });

            chatapp->finishRegistration();

            connection->enterEventLoop();
        });

        std::thread channel_state([this]() {
            int prev_state = -1;
            int curr_state = -1;
            while (1) {
                curr_state = GetChannelState();

                auto state_str = [](int state)-> std::string {
                    switch (state) {
                    case GRPC_CHANNEL_IDLE:
                        return {"IDLE"};
                    case GRPC_CHANNEL_CONNECTING:
                        return {"CONNECTING"};
                    case GRPC_CHANNEL_READY:
                        return {"READY"};
                    case GRPC_CHANNEL_TRANSIENT_FAILURE:
                        return {"TRANSIENT FAILURE"};
                    case GRPC_CHANNEL_SHUTDOWN:
                        return {"SHUTDOWN"};
                    default:
                        return {"UNKNOWN"};
                    }
                };

                if (curr_state != prev_state) {
                    prev_state = curr_state;

                    cout << "Channel state " << state_str(curr_state) << endl;

                    if (curr_state == GRPC_CHANNEL_READY) {
                        ResetStream();
                    }
                }

                std::this_thread::sleep_for (std::chrono::seconds(8));
            }
        });

        /*
         * Start listening to the stream
         */
        StreamResponse resp;
        while (1) {
            while (stream_->Read(&resp)) {
                if (resp.has_msg_resp()) {
                    ReceiveUserChatMessage(resp.msg_resp());
                } else if (resp.has_start_resp()) {
                    ReceiveSessionStartResponse(resp.start_resp());
                } else if (resp.has_stop_resp()) {
                    ReceiveSessionStopResponse(resp.stop_resp());
                } else {
                    cout << "Unknown stream response\n";
                }
            }

            /*
             * Server close its write stream
             */
            std::this_thread::sleep_for (std::chrono::seconds(8));
        }

        writer.join();
        channel_state.join();

        stream_->Finish();
    }

    void Run() {
        std::string address("0.0.0.0:5000");
        ChatClient client(
            grpc::CreateChannel(
                address,
                grpc::InsecureChannelCredentials()
            )
        );
    }
};

int main()
{
    std::string address("0.0.0.0:5000");
    ChatClient client(
        grpc::CreateChannel(
            address,
            grpc::InsecureChannelCredentials()
        )
    );

    client.StartChat();

    client.Logout();

    return 0;
}
