#pragma once

#include "Network.hpp"
#include "Connection.hpp"

namespace Jkr::Network {
class ServerInterface {
    public:
    using OnClientConnectionFunctionType    = std::function<bool(sp<Connection>)>;
    using OnClientDisConnectionFunctionType = std::function<void(sp<Connection>)>;
    using OnMessageFunctionType             = std::function<void(sp<Connection>, Message&)>;
    using OnClientValidationFunctionType    = Connection::OnClientValidationFunctionType;
    ServerInterface(uint16_t inPort)
        : mAsioAcceptor(mAsioContext, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), inPort)) {}
    ~ServerInterface() { Stop(); }
    bool Start(OnClientValidationFunctionType& inOnClientValidationFunction,
               OnClientConnectionFunctionType& inOnClientConnectFunction);
    void Stop();
    // ASYNC
    void WaitForClientConnection(OnClientValidationFunctionType& inFunction,
                                 OnClientConnectionFunctionType& inOnClientConnectFunction);
    void MessageClient(sp<Connection> inClient,
                       const Message& inMsg,
                       OnClientDisConnectionFunctionType& inOnclientDisconnect);
    void MessageAllClient(const Message& inMsg,
                          OnClientDisConnectionFunctionType& inOnclientDisconnect,
                          sp<Connection> inIgnoreClient = nullptr);
    void Update(OnMessageFunctionType& inOnMessageFunction,
                size_t inMaxMessages = -1,
                bool inWait          = false);

    protected:
    TsQueue<OwnedMessage> mQMessagesIn;
    std::deque<sp<Connection>> mQConnections;
    asio::io_context mAsioContext;
    std::thread mThreadContext;

    asio::ip::tcp::acceptor mAsioAcceptor;
    uint32_t mIdCounter = 10000;
};
} // namespace Jkr::Network