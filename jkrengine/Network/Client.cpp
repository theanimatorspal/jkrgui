#include "Client.hpp"

using namespace Jkr::Network;

bool ClientInterface::Connect(OnClientValidationFunctionType& inFunction,
                              const std::string_view inHost,
                              const uint16_t inPort) {
    try {
        asio::ip::tcp::resolver resolver(mContext);
        asio::ip::tcp::resolver::results_type endpoints =
             resolver.resolve(inHost, std::to_string(inPort));

        mConnection = MakeUp<Connection>(
             Connection::Owner::Client, mContext, asio::ip::tcp::socket(mContext), mQMessagesIn);

        mConnection->ConnectToServer(inFunction, endpoints);

        mThreadContext = std::thread([this]() { mContext.run(); });

    } catch (std::exception& e) {
        ksai_print("Client Exception:");
        ksai_print(e.what());
    }
    return false;
}

void ClientInterface::Disconnect() {
    if (mThreadContext.joinable()) mThreadContext.join();
    if (IsConnected()) {
        mConnection->Disconnect();
        mContext.stop();
    }
}

bool ClientInterface::IsConnected() {
    if (mConnection)
        return mConnection->IsConnected();
    else
        return false;
}

void ClientInterface::Send(const Message& inMessage) {
    if (IsConnected()) mConnection->Send(inMessage);
}