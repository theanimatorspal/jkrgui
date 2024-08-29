#include "JkrLuaExe.hpp"
#include <Network/Client.hpp>
#include <Network/Server.hpp>

using namespace Jkr::Network;
static std::vector<Up<ClientInterface>> Clients;
static Up<ServerInterface> Server;
static std::vector<Message> MessageBuffer;
static std::mutex MessageAccessMutex;

OnClientValidationFunctionType OnClientValidationFunction = [](sp<Connection>) {};

OnClientConnectionFunctionType OnClientConnectionFunction = [](sp<Connection>) { return false; };

OnMessageFunctionType OnMessageFunction                   = [](sp<Connection>, Message &inMessage) {
    std::scoped_lock<std::mutex> Lock(MessageAccessMutex);
    MessageBuffer.push_back(inMessage);
};

/* ============================================================

    NETWORK SERVER

============================================================== */

void ServerStart(int inPort) {
    Server = MakeUp<ServerInterface>(static_cast<uint16_t>(inPort));
    Server->Start(OnClientValidationFunction, OnClientConnectionFunction);
}
void ServerStop() { Server->Stop(); }
void ServerUpdate(int inMaxMessages, bool inShouldWait) {
    Server->Update(OnMessageFunction, inMaxMessages, inShouldWait);
}

void MessagesBufferAccessStart() { MessageAccessMutex.lock(); }
void MessagesBufferAccessEnd() { MessageAccessMutex.unlock(); }

/* ============================================================

    NETWORK CLIENT

============================================================== */

void AddClient() { Clients.push_back(MakeUp<ClientInterface>()); }
void ConnectFromClient(int inId, const std::string_view inHost, int inPort) {
    Clients[inId]->Connect(OnClientValidationFunction, inHost, inPort);
}
bool IsConnectedClient(int inId) { return Clients[inId]->IsConnected(); }
void SendMessageFromClient(int inId, const Message &inMessage) { Clients[inId]->Send(inMessage); }
bool IsIncomingMessagesEmptyClient(int inId) {
    return Clients[inId]->GetIncomingQMessages().empty();
}
Message PopFrontIncomingMessagesClient(int inId) {
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
                              &Message::Get<float>);

    Jkr.set_function("ServerStart", &ServerStart);
    Jkr.set_function("ServerStop", &ServerStop);
    Jkr.set_function("ServerUpdate", &ServerUpdate);
    Jkr.set_function("MessagesBufferAccessStart", &MessagesBufferAccessStart);
    Jkr.set_function("MessagesBufferAccessEnd", &MessagesBufferAccessEnd);

    Jkr.set_function("AddClient", &AddClient);
    Jkr.set_function("ConnectFromClient", &ConnectFromClient);
    Jkr.set_function("IsConnectedClient", &IsConnectedClient);
    Jkr.set_function("SendMessageFromClient", &SendMessageFromClient);
    Jkr.set_function("IsIncomingMessageEmptyClient", &IsIncomingMessagesEmptyClient);
    Jkr.set_function("PopIncomingMessagesClient", &PopFrontIncomingMessagesClient);

    Jkr["NetworkGlobalMessageBuffer"] = std::ref(MessageBuffer);
}

} // namespace JkrEXE