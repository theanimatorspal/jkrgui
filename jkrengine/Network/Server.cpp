#include "Server.hpp"

using namespace Jkr::Network;

bool ServerInterface::Start(OnClientValidationFunctionType &inOnClientValidationFunction,
                            OnClientConnectionFunctionType &inOnClientConnectFunction) {
    try {
        WaitForClientConnection(inOnClientValidationFunction, inOnClientConnectFunction);
        mThreadContext = std::thread([this]() { mAsioContext.run(); });
    } catch (const std::exception &e) {
        Log("[SERVER] Exception: ");
        Log(e.what());
    }

    Log("[SERVER] Started\n");
    return false;
}
void ServerInterface::Stop() {
    mAsioContext.stop();
    if (mThreadContext.joinable()) mThreadContext.join();
    Log("[SERVER] Stopped");
}

// ASYNC
void ServerInterface::WaitForClientConnection(
     OnClientValidationFunctionType &inOnClientValidationFunction,
     OnClientConnectionFunctionType &inOnClientConnectFunction) {
    mAsioAcceptor.async_accept([&, this](std::error_code ec, asio::ip::tcp::socket socket) {
        if (not ec) {
            Log("[SERVER]  New Connection: ");
            Log(socket.remote_endpoint().address().to_string());
            auto newCon = mksh<Connection>(
                 Connection::Owner::Server, mAsioContext, std::move(socket), mQMessagesIn);

            if (inOnClientConnectFunction(newCon)) {
                mQConnections.push_back(std::move(newCon));
                mQConnections.back()->ConnectToClient(inOnClientValidationFunction, mIdCounter++);
                Log((s("[") + std::to_string(mQConnections.back()->GetId()) +
                            s("] Connection Approved"))
                                .c_str());
            } else {
                Log("[-----] Connection Denied");
            }

        } else {
            Log("[SERVER] New Connection Error: ");
            Log(ec.message().c_str());
        }

        WaitForClientConnection(inOnClientValidationFunction, inOnClientConnectFunction);
    });
}
void ServerInterface::MessageClient(sp<Connection> inClient,
                                    const Message &inMsg,
                                    OnClientDisConnectionFunctionType &inOnclientDisconnect) {
    if (inClient and inClient->IsConnected()) {
        inClient->Send(inMsg);
    } else {
        inOnclientDisconnect(inClient);
        inClient.reset();
        mQConnections.erase(std::remove(mQConnections.begin(), mQConnections.end(), inClient),
                            mQConnections.end());
    }
}

void ServerInterface::MessageAllClient(const Message &inMsg,
                                       OnClientDisConnectionFunctionType &inOnclientDisconnect,
                                       sp<Connection> inIgnoreClient) {
    bool InvalidClientExists = false;
    for (auto &client : mQConnections) {
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

void ServerInterface::Update(OnMessageFunctionType &inOnMessageFunction,
                             size_t inMaxMessages,
                             bool inWait) {

    if (inWait) {
        mQMessagesIn.wait();
    }
    size_t MessageCount = 0;
    while (MessageCount < inMaxMessages and not mQMessagesIn.empty()) {
        auto msg = mQMessagesIn.pop_front();
        inOnMessageFunction(msg.mRemote, msg);
        MessageCount++;
    }
}
