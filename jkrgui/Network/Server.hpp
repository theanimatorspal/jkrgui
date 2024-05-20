#pragma once

#include "Network.hpp"
#include "Connection.hpp"

namespace Jkr::Network {
class ServerInterface {
    public:
    using OnClientConnectionFunctionType    = std::function<bool(sp<Connection>)>;
    using OnClientDisConnectionFunctionType = std::function<void(sp<Connection>)>;
    using OnMessageFunctionType             = std::function<void(sp<Connection>, Message&)>;
    ServerInterface(uint16_t inPort)
        : mAsioAcceptor(mAsioContext, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), inPort)) {}
    ~ServerInterface() { Stop(); }
    bool Start(OnClientConnectionFunctionType inOnClientConnectFunction) {
        try {
            WaitForClientConnection(inOnClientConnectFunction);
            mThreadContext = std::thread([this]() { mAsioContext.run(); });
        } catch (const std::exception& e) {
            ksai_print("[SERVER] Exception: ");
            ksai_print(e.what());
        }

        ksai_print("[SERVER] Started\n");
        return false;
    }
    void Stop() {
        mAsioContext.stop();
        if (mThreadContext.joinable()) mThreadContext.join();
        ksai_print("[SERVER] Stopped");
    }

    // ASYNC
    void WaitForClientConnection(OnClientConnectionFunctionType inOnClientConnectFunction) {
        mAsioAcceptor.async_accept([=, this](std::error_code ec, asio::ip::tcp::socket socket) {
            if (not ec) {
                ksai_print("[SERVER]  New Connection: ");
                ksai_print(socket.remote_endpoint());
                auto newCon = mksh<Connection>(
                     Connection::Owner::Server, mAsioContext, std::move(socket), mQMessagesIn);

                if (inOnClientConnectFunction(newCon)) {
                    mQConnections.push_back(newCon);
                    mQConnections.back()->ConnectToClient(mIdCounter++);
                    ksai_print((s("[") + std::to_string(mQConnections.back()->GetId()) +
                                s("] Connection Approved"))
                                    .c_str());
                } else {
                    ksai_print("[-----] Connection Denied");
                }

            } else {
                ksai_print("[SERVER] New Connection Error: ");
                ksai_print(ec.message().c_str());
            }

            WaitForClientConnection(inOnClientConnectFunction);
        });
    }
    void MessageClient(sp<Connection> inClient,
                       const Message& inMsg,
                       OnClientDisConnectionFunctionType& inOnclientDisconnect) {
        if (inClient and inClient->IsConnected()) {
            inClient->Send(inMsg);
        } else {
            inOnclientDisconnect(inClient);
            inClient.reset();
            mQConnections.erase(std::remove(mQConnections.begin(), mQConnections.end(), inClient),
                                mQConnections.end());
        }
    }

    void MessageAllClient(const Message& inMsg,
                          OnClientDisConnectionFunctionType& inOnclientDisconnect,
                          sp<Connection> inIgnoreClient = nullptr) {
        bool InvalidClientExists = false;
        for (auto& client : mQConnections) {
            if (client and client->IsConnected()) {
                if (client != inIgnoreClient) client->Send(inMsg);
            } else {
                inOnclientDisconnect(client);
                client.reset();
            }
        }
        if (InvalidClientExists)
            mQConnections.erase(std::remove(mQConnections.begin(), mQConnections.end(), nullptr),
                                mQConnections.end());
    }

    void Update(OnMessageFunctionType& inOnMessageFunction, size_t inMaxMessages = -1) {
        size_t MessageCount = 0;
        while (MessageCount < inMaxMessages and not mQMessagesIn.empty()) {
            auto msg = mQMessagesIn.pop_front();
            inOnMessageFunction(msg.mRemote, msg);
            MessageCount++;
        }
    }

    protected:
    TsQueue<OwnedMessage> mQMessagesIn;
    std::deque<sp<Connection>> mQConnections;
    asio::io_context mAsioContext;
    std::thread mThreadContext;

    asio::ip::tcp::acceptor mAsioAcceptor;
    uint32_t mIdCounter = 10000;
};
} // namespace Jkr::Network