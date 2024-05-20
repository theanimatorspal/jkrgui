#pragma once
#include "TsQueue.hpp"
#include "Network.hpp"
#include "Message.hpp"

namespace Jkr::Network {
class Connection : public std::enable_shared_from_this<Connection> {

    public:
    enum class Owner { Server, Client };
    GETTER GetId() { return mId; }
    void ConnectToServer(asio::ip::tcp::resolver::results_type& inEndPoints) {
        if (mOwnerType == Owner::Client) {
            asio::async_connect(mSocket,
                                inEndPoints,
                                [this](std::error_code ec, asio::ip::tcp::endpoint endpoint) {
                                    if (not ec) {
                                        ReadHeader();
                                    }
                                });
        }
    }

    void ConnectToClient(uint32_t inUid = 0) {
        if (mOwnerType == Owner::Server) {
            if (mSocket.is_open()) {
                mId = inUid;
                ReadHeader();
            }
        }
    }
    void ReadHeader() {
        asio::async_read(
             mSocket,
             asio::buffer(&mMessageTemporaryIn.mHeader, sizeof(MessageHeader)),
             [this](std::error_code ec, std::size_t length) {
                 if (not ec) {
                     if (mMessageTemporaryIn.mHeader.mSize > 0) {
                         mMessageTemporaryIn.mBody.resize(mMessageTemporaryIn.mHeader.mSize);
                         ReadBody();
                     } else {
                         AddToIncomingMessageQueue();
                     }
                 } else {
                     ksai_print(s("[") + std::to_string(mId) + s("] Read Header Failed."));
                     mSocket.close();
                 }
             });
    }
    void ReadBody() {
        asio::async_read(
             mSocket,
             asio::buffer(mMessageTemporaryIn.mBody.data(), mMessageTemporaryIn.mBody.size()),
             [this](std::error_code ec, std::size_t length) {
                 if (not ec) {
                     AddToIncomingMessageQueue();
                 } else {
                     ksai_print(s("[") + std::to_string(mId) + s("] Read Body Failed."));
                     mSocket.close();
                 }
             });
    }

    void WriteHeader() {
        asio::async_write(mSocket,
                          asio::buffer(&mQMessagesOut.front().mHeader, sizeof(MessageHeader)),
                          [this](std::error_code ec, std::size_t length) {
                              if (not ec) {
                                  if (mQMessagesOut.front().mBody.size() > 0) {
                                      WriteBody();
                                  } else {
                                      mQMessagesOut.pop_front();
                                      if (not mQMessagesOut.empty()) {
                                          WriteHeader();
                                      }
                                  }
                              } else {
                                  ksai_print(s("[") + std::to_string(mId) +
                                             s("] Write Header Failed."));
                                  mSocket.close();
                              }
                          });
    }

    void WriteBody() {
        asio::async_write(
             mSocket,
             asio::buffer(mQMessagesOut.front().mBody.data(), mQMessagesOut.front().mBody.size()),
             [this](std::error_code ec, std::size_t length) {
                 if (not ec) {
                     mQMessagesOut.pop_front();
                     if (not mQMessagesOut.empty()) {
                         WriteHeader();
                     }
                 } else {
                     ksai_print(s("[") + std::to_string(mId) + s("] Write Body Failed."));
                     mSocket.close();
                 }
             });
    }

    void AddToIncomingMessageQueue() {
        if (mOwnerType == Owner::Server) {
            OwnedMessage OwnedMessage = {mMessageTemporaryIn};
            OwnedMessage.mRemote      = this->shared_from_this();
            mQMessagesIn.push_back(OwnedMessage);
        } else {
            mQMessagesIn.push_back(OwnedMessage{mMessageTemporaryIn});
        }

        ReadHeader();
    }

    void Disconnect() {
        if (IsConnected()) {
            asio::post(mContext, [this]() { mSocket.close(); });
        }
    }

    bool IsConnected() const { return mSocket.is_open(); }
    void Send(const Message& msg) {
        asio::post(mContext, [this, msg]() {
            bool IsWritingMessage = not mQMessagesOut.empty();
            mQMessagesOut.push_back(msg);
            if (not IsWritingMessage) {
                WriteHeader();
            }
        });
    }

    Connection(Owner inParent,
               asio::io_context& inAsioContext,
               asio::ip::tcp::socket inSocket,
               TsQueue<OwnedMessage>& inOwnedMessages)
        : mContext(inAsioContext), mSocket(std::move(inSocket)), mQMessagesIn(inOwnedMessages) {
        mOwnerType = inParent;
    }
    ~Connection() {}

    protected:
    asio::ip::tcp::socket mSocket;
    asio::io_context& mContext;
    TsQueue<Message> mQMessagesOut;
    TsQueue<OwnedMessage>& mQMessagesIn;
    Message mMessageTemporaryIn;
    Owner mOwnerType = Owner::Server;
    uint32_t mId     = 0;
};
} // namespace Jkr::Network