#pragma once
#include "TsQueue.hpp"
#include "Message.hpp"

namespace Jkr::Network {
class ServerInterface;
class Connection : public std::enable_shared_from_this<Connection> {
    public:
    using OnClientValidationFunctionType = std::function<void(sp<Connection>)>;
    enum class Owner { Server, Client };
    GETTER GetId() { return mId; }
    void ConnectToServer(OnClientValidationFunctionType& inFunction,
                         asio::ip::tcp::resolver::results_type& inEndPoints);
    void ConnectToClient(OnClientValidationFunctionType& inOnClientValidationFunction,
                         ServerInterface* inServer,
                         uint32_t inUid = 0);
    void ReadHeader();
    void ReadBody();
    void WriteHeader();
    void WriteBody();
    void AddToIncomingMessageQueue();
    void Disconnect();
    bool IsConnected() const;
    void Send(const Message& msg);
    uint64_t Scramble(uint64_t inInput);
    void WriteValidation();
    void ReadValidation(OnClientValidationFunctionType& inFunction,
                        ServerInterface* inInterface = nullptr);

    Connection(Owner inParent,
               asio::io_context& inAsioContext,
               asio::ip::tcp::socket inSocket,
               TsQueue<OwnedMessage>& inOwnedMessages);
    ~Connection() = default;

    protected:
    asio::ip::tcp::socket mSocket;
    asio::io_context& mContext;
    TsQueue<Message> mQMessagesOut;
    TsQueue<OwnedMessage>& mQMessagesIn;
    Message mMessageTemporaryIn;
    Owner mOwnerType = Owner::Server;
    uint32_t mId     = 0;

    // Handshake Validation
    uint64_t mHandShakeOut   = 0;
    uint64_t mHandShakeIn    = 0;
    uint64_t mHandshakeCheck = 0;
};
} // namespace Jkr::Network