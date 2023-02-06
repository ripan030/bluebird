#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <thread>

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
        ClientContext context;
        std::unique_ptr<ClientReaderWriter<StreamRequest, StreamResponse>> stream_;
        std::unique_ptr<Profile> profile;
        std::vector<std::string> activeFriends;
        std::unordered_map<std::string, std::unique_ptr<Chatting>> activeChats;
public:
    ChatClient(std::shared_ptr<Channel> channel) :
        stub_{Chat::NewStub(channel)}, profile{} {
        std::unique_ptr<ClientReaderWriter<StreamRequest, StreamResponse>> stream(stub_->ChatStream(&context));

        stream_ = std::move(stream);

#ifdef UNIT_TEST
        std::unique_ptr<Profile> p = std::make_unique<Profile>("bluebard", "bluebird");
        profile = std::move(p);

        activeFriends.push_back("blue001");
        activeFriends.push_back("green30");
        activeFriends.push_back("orange");
#endif
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

        /*
         * Start listening to the stream
         */
        StreamResponse resp;
        while (stream_->Read(&resp)) {
            if (resp.has_msg_resp()) {
                ReceiveUserChatMessage(resp.msg_resp());
            }
        }

        writer.join();
        stream_->Finish();
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
