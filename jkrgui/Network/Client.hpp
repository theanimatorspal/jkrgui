#pragma once
#include "Connection.hpp"

namespace Jkr::Network {
class ClientInterface {
    public:
    ClientInterface() : mSocket(mContext) {}
    ~ClientInterface() { Disconnect(); }
    bool Connect(const std::string_view inHost, const uint16_t inPort) {
        try {
            asio::ip::tcp::resolver resolver(mContext);
            asio::ip::tcp::resolver::results_type endpoints =
                 resolver.resolve(inHost, std::to_string(inPort));

            mConnection = MakeUp<Connection>(Connection::Owner::Client,
                                             mContext,
                                             asio::ip::tcp::socket(mContext),
                                             mQMessagesIn);

            mConnection->ConnectToServer(endpoints);

            mThreadContext = std::thread([this]() { mContext.run(); });

        } catch (std::exception& e) {
            ksai_print("Client Exception:");
            ksai_print(e.what());
        }
        return false;
    }

    void Disconnect() {
        if (IsConnected()) {
            mConnection->Disconnect();
            mContext.stop();
            if (mThreadContext.joinable()) mThreadContext.join();
        }
    }

    bool IsConnected() {
        if (mConnection)
            return mConnection->IsConnected();
        else
            return false;
    }

    void Send(const Message& inMessage) {
        if (IsConnected()) mConnection->Send(inMessage);
    }

    GETTER& GetIncomingQMessages() { return mQMessagesIn; }

    protected:
    // handles data transfer
    asio::io_context mContext;
    std::thread mThreadContext;
    asio::ip::tcp::socket mSocket;
    up<Connection> mConnection;

    private:
    TsQueue<OwnedMessage> mQMessagesIn;
};
} // namespace Jkr::Network