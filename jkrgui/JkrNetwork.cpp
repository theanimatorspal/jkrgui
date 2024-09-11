#include "JkrLuaExe.hpp"
#include <Network/Client.hpp>
#include <Network/Server.hpp>

using namespace Jkr::Network;
// Specialization of Messages
template <> void Jkr::Network::Message::InsertSpecial(Message &inMessage, sol::function indata) {
    auto data = indata.dump();
    inMessage << data;
}
template <>
sol::function Jkr::Network::Message::GetSpecial(Message &inMessage, sol::function indata) {
    sol::basic_bytecode<> code;
    code.as_string_view();
}

static std::vector<Up<ClientInterface>> Clients;
static Up<ServerInterface> Server;
static std::vector<Message> MessageBuffer;
static std::mutex MessageAccessMutex;

OnClientValidationFunctionType OnClientValidationFunction = [](sp<Connection>) {};

OnClientConnectionFunctionType OnClientConnectionFunction = [](sp<Connection>) { return true; };

OnClientDisConnectionFunctionType OnClientDisConnectionFunction = [](sp<Connection>) {
    std::cout << "Client has been disconnected\n";
    return true;
};

OnMessageFunctionType OnMessageFunction = [](sp<Connection>, Message &inMessage) {
    std::scoped_lock<std::mutex> Lock(MessageAccessMutex);
    std::cout << "A Message has been Received\n";
    MessageBuffer.push_back(inMessage);
};

/* ============================================================

    NETWORK SERVER

============================================================== */

static void StartServer(int inPort) {
    Server = MakeUp<ServerInterface>(static_cast<uint16_t>(inPort));
    Server->Start(OnClientValidationFunction, OnClientConnectionFunction);
}
static void StopServer() { Server->Stop(); }
static void UpdateServer(int inMaxMessages, bool inShouldWait) {
    Server->Update(OnMessageFunction, inMaxMessages, inShouldWait);
}

static void MessagesBufferAccessStart() { MessageAccessMutex.lock(); }
static void MessagesBufferAccessEnd() { MessageAccessMutex.unlock(); }

static void SendMessageToClientFromServer(int inClient, const Message &inMessage) {
    Server->MessageClient(
         Clients[inClient]->GetConnection(), inMessage, OnClientDisConnectionFunction);
}

static void SendMessageToAllClientFromServer(const Message &inMessage) {
    Server->MessageAllClient(inMessage, OnClientDisConnectionFunction);
}

/* ============================================================

    NETWORK CLIENT

============================================================== */

static void AddClient() { Clients.push_back(MakeUp<ClientInterface>()); }
static void ConnectFromClient(int inId, const std::string_view inHost, int inPort) {
    Clients[inId]->Connect(OnClientValidationFunction, inHost, inPort);
}
static bool IsConnectedClient(int inId) { return Clients[inId]->IsConnected(); }
static void SendMessageFromClient(int inId, const Message &inMessage) {
    Clients[inId]->Send(inMessage);
}
static bool IsIncomingMessagesEmptyClient(int inId) {
    return Clients[inId]->GetIncomingQMessages().empty();
}
static Message PopFrontIncomingMessagesClient(int inId) {
    return Clients[inId]->GetIncomingQMessages().pop_front();
}

namespace JkrEXE {
/* ============================================================

    NETWORK BINDINGS

============================================================== */
void CreateNetworkBindings(sol::state &s) {
    auto Jkr = s["Jkr"].get_or_create<sol::table>();
    Jkr.new_usertype<MessageHeader>("MessageHeader",
                                    sol::call_constructor,
                                    sol::default_constructor,
                                    "mId",
                                    &MessageHeader::mId,
                                    "mSize",
                                    &MessageHeader::mSize);
    Jkr.new_usertype<Message>("Message",
                              sol::call_constructor,
                              sol::default_constructor,
                              "mHeader",
                              &Message::mHeader,
                              "mBody",
                              &Message::mBody,
                              "InsertFloat",
                              &Message::Insert<float>,
                              "GetFloat",
                              &Message::Get<float>
                              //   "InsertString",
                              //   &Message::Insert<std::string>
                              //   "InsertFunction",
                              //   &Message::Insert<sol::function>
    );

    Jkr.set_function("StartServer", &StartServer);
    Jkr.set_function("StopServer", &StopServer);
    Jkr.set_function("UpdateServer", &UpdateServer);

    ///@brief you cannot use the ServerUpdate function while using these two
    /// also this is for the server not for the client
    Jkr.set_function("GetTrapMessagesBuffer", [&]() {
        MessagesBufferAccessStart();
        return MessageBuffer;
    });
    Jkr.set_function("ReleaseMessagesBuffer", [&]() { MessagesBufferAccessEnd(); });

    Jkr.set_function("AddClient", &AddClient);
    Jkr.set_function("ConnectFromClient", &ConnectFromClient);
    Jkr.set_function("IsConnectedClient", &IsConnectedClient);
    Jkr.set_function("SendMessageFromClient", &SendMessageFromClient);
    Jkr.set_function("IsIncomingMessageEmptyClient", &IsIncomingMessagesEmptyClient);
    Jkr.set_function("PopIncomingMessagesClient", &PopFrontIncomingMessagesClient);
}

} // namespace JkrEXE