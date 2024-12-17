#include "JkrLuaExe.hpp"
#include <Network/Client.hpp>
#include <Network/Server.hpp>
#include <Network/Udp.hpp>

using namespace Jkr::Network;

// Specialization of Messages
static std::vector<Up<ClientInterface>> Clients;
static Up<ServerInterface> Server;
static Jkr::Network::TsQueue<Message> MessageBuffer;

OnClientValidationFunctionType OnClientValidationFunction = [](sp<Connection> inConnection) {};

OnClientConnectionFunctionType OnClientConnectionFunction = [](sp<Connection> inConnection) {
    std::cout << "A Client has Connected" << "\n";
    return true;
};

OnClientDisConnectionFunctionType OnClientDisConnectionFunction = [](sp<Connection>) {
    std::cout << "Client has been disconnected\n";
    return true;
};

OnMessageFunctionType OnMessageFunction = [](sp<Connection>, Message &inMessage) {
    MessageBuffer.push_back(inMessage);
};

/* ============================================================

    NETWORK SERVER

============================================================== */

static v<s> StartServer(int inPort) {
    Server = MakeUp<ServerInterface>(static_cast<uint16_t>(inPort));
    Server->Start(OnClientValidationFunction, OnClientConnectionFunction);

    v<s> endpoints_string;
    asio::io_context io_context;
    std::string host_Name = asio::ip::host_name();
    asio::ip::tcp::resolver resolver(io_context);
    asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(host_Name, "");
    for (const auto &endpoint : endpoints) {
        asio::ip::address addr = endpoint.endpoint().address();
        if (addr.is_v4()) {
            endpoints_string.push_back(addr.to_string());
        }
    }
    return endpoints_string;
}
static void StopServer() { Server->Stop(); }
static void UpdateServer(int inMaxMessages, bool inShouldWait) {
    Server->Update(OnMessageFunction, inMaxMessages, inShouldWait);
}

/// @brief I don't think this function is even necessary
/// I don't know man, I should Investigate further.
///
static void SendMessageToClientFromServer(int inClient, const Message &inMessage) {
    Server->MessageClient(
         Clients[inClient]->GetConnection(), inMessage, OnClientDisConnectionFunction);
}

static void BroadcastServer(const Message &inMessage) {
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

/* ============================================================

    NETWORK UDP

============================================================== */
static Jkr::Network::TsQueue<std::vector<char>> MessageBufferUDP;
static Up<UDP> UDPHandle;

auto OnReceive = [](v<char> inMessage) {
    if (MessageBufferUDP.count() < 3) {
        MessageBufferUDP.push_back(inMessage);
    }
};

void StartUDP(int inPort) { UDPHandle = mu<UDP>(inPort); }
void SendUDP(std::vector<char> inMessage, std::string inDestination, int inPort) {
    UDPHandle->Send(inMessage, inDestination, inPort);
}
void SendUDPBlocking(std::vector<char> inMessage, std::string inDestination, int inPort) {
    UDPHandle->SendBlocking(inMessage, inDestination, inPort);
}

static bool FirstTime = true;
void ReceiveUDP() {
    if (FirstTime) {
        UDPHandle->Recieve(OnReceive);
        FirstTime = false;
    }
}

void ReceiveUDPBlocking() { UDPHandle->RecieveBlocking(OnReceive); }

template <typename T> std::vector<char> ConvertToVChar(T inT) {
    std::vector<char> charVec;
    if constexpr (std::is_standard_layout_v<T>) {
        charVec.resize(sizeof(T));
        std::memcpy(charVec.data(), &inT, sizeof(T));
    }
    return charVec;
}

///@warning
///@param t This parameter is just to inform sol2 to get the type of datatype NOT TO MODIFY IT,
/// just pass a vec2() vec3() or any type from the lua side to get this right
template <typename T> T ConvertFromVChar(T t, std::vector<char> inCharVector) {
    T out;
    if constexpr (std::is_standard_layout_v<T>) {
        std::memcpy(&out, inCharVector.data(), inCharVector.size());
    }
    return out;
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
    Jkr.new_usertype<Message>(
         "Message",
         sol::call_constructor,
         sol::default_constructor,
         "mHeader",
         &Message::mHeader,
         "mBody",
         &Message::mBody,
         "InsertFloat",
         &Message::Insert<float>,
         "GetFloat",
         &Message::Get<float>,
         "InsertString",
         &Message::InsertEXT<std::string>,
         "GetString",
         &Message::GetEXT<std::string>,
         "InsertFunction",
         [](Message &inMsg, sol::function inFunction) {
             auto tosend = inFunction.dump();
             inMsg.InsertEXT(tosend);
         },
         "GetFunction",
         [](Message &inMsg) -> std::string {
             sol::basic_bytecode<> code = inMsg.GetEXT<decltype(code)>();
             return std::string(code.as_string_view());
         },
         "InsertFile",
         [](Message &inMsg, std::string_view inFileName) {
             std::ifstream file = std::ifstream(std::string(inFileName), std::ios::binary);
             auto FileSize      = filesystem::file_size(inFileName);
             std::vector<char> Bytes;
             Bytes.resize(FileSize);
             file.read(Bytes.data(), FileSize);
             inMsg.InsertEXT(Bytes);
         },
         /// @brief this downloads and writes the file, doesn't do anything else, returns nil
         ///
         ///
         "GetFile",
         [](Message &inMsg, std::string_view inFileName) {
             std::vector<char> Bytes = inMsg.GetEXT<decltype(Bytes)>();
             std::ofstream file =
                  std::ofstream(std::string(inFileName), std::ios::binary | std::ios::trunc);
             file.write(Bytes.data(), Bytes.size());
         },
         "InsertVec3",
         &Message::Insert<glm::vec3>,
         "GetVec3",
         &Message::Get<glm::vec3>,
         "InsertVChar",
         &Message::Insert<v<char>>,
         "GetVChar",
         &Message::Get<v<char>>);

    Jkr.set_function("StartServer", &StartServer);
    Jkr.set_function("StopServer", &StopServer);
    Jkr.set_function("UpdateServer", &UpdateServer);
    Jkr.set_function("BroadcastServer", &BroadcastServer);

    ///@brief you cannot use the ServerUpdate function while using these two
    /// also this is for the server not for the client
    Jkr.set_function("IsMessagesBufferEmpty", [&]() {
        UpdateServer(500000000, false);
        return MessageBuffer.empty();
    });
    Jkr.set_function("PopFrontMessagesBuffer", [&]() { return MessageBuffer.pop_front(); });

    Jkr.set_function("AddClient", &AddClient);
    Jkr.set_function("ConnectFromClient", &ConnectFromClient);
    Jkr.set_function("IsConnectedClient", &IsConnectedClient);
    Jkr.set_function("SendMessageFromClient", &SendMessageFromClient);
    Jkr.set_function("IsIncomingMessagesEmptyClient", &IsIncomingMessagesEmptyClient);
    Jkr.set_function("PopFrontIncomingMessagesClient", &PopFrontIncomingMessagesClient);

    Jkr.set_function("StartUDP", &StartUDP);
    Jkr.set_function("SendUDP", &SendUDP);
    Jkr.set_function("SendUDPBlocking", &SendUDPBlocking);
    Jkr.set_function("ReceiveUDP", &ReceiveUDP);
    Jkr.set_function("IsMessagesBufferEmptyUDP", [&]() {
        ReceiveUDP();
        return MessageBufferUDP.empty();
    });
    Jkr.set_function("PopFrontMessagesBufferUDP", [&]() { return MessageBufferUDP.pop_front(); });
    Jkr.set_function("SetBufferSizeUDP", [&](int inSize) { UDPHandle->SetBufferSize(inSize); });
    Jkr.set_function("ConvertFromVChar",
                     sol::overload(&ConvertFromVChar<glm::vec2>,
                                   &ConvertFromVChar<glm::vec3>,
                                   &ConvertFromVChar<glm::vec4>));
    Jkr.set_function("ConvertToVChar",
                     sol::overload(&ConvertToVChar<glm::vec2>,
                                   &ConvertToVChar<glm::vec3>,
                                   &ConvertToVChar<glm::vec4>));
}

} // namespace JkrEXE