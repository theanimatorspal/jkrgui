#include "Connection.hpp"
#include "Server.hpp"

using namespace Jkr::Network;
void Connection::ConnectToServer(OnClientValidationFunctionType& inFunction,
                                 asio::ip::tcp::resolver::results_type& inEndPoints) {
    if (mOwnerType == Owner::Client) {
        asio::async_connect(
             mSocket, inEndPoints, [&, this](std::error_code ec, asio::ip::tcp::endpoint endpoint) {
                 if (not ec) {
                     ReadValidation(inFunction);
                     //  ReadHeader();
                 }
             });
    }
}

void Connection::ConnectToClient(OnClientValidationFunctionType& inOnClientValidationFunction,
                                 uint32_t inUid) {
    if (mOwnerType == Owner::Server) {
        if (mSocket.is_open()) {
            mId = inUid;
            WriteValidation();
            ReadValidation(inOnClientValidationFunction);
        }
    }
}
void Connection::ReadHeader() {
    asio::async_read(mSocket,
                     asio::buffer(&mMessageTemporaryIn.mHeader, sizeof(MessageHeader)),
                     [this](std::error_code ec, std::size_t length) {
                         if (not ec) {
                             if (mMessageTemporaryIn.mHeader.mSize > 0) {
                                 mMessageTemporaryIn.mBody.resize(
                                      mMessageTemporaryIn.mHeader.mSize);
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
void Connection::ReadBody() {
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

void Connection::WriteHeader() {
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

void Connection::WriteBody() {
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

void Connection::AddToIncomingMessageQueue() {
    if (mOwnerType == Owner::Server) {
        OwnedMessage OwnedMessage = {mMessageTemporaryIn};
        OwnedMessage.mRemote      = this->shared_from_this();
        mQMessagesIn.push_back(OwnedMessage);
    } else {
        mQMessagesIn.push_back(OwnedMessage{mMessageTemporaryIn});
    }

    ReadHeader();
}

void Connection::Disconnect() {
    if (IsConnected()) {
        asio::post(mContext, [this]() { mSocket.close(); });
    }
}

bool Connection::IsConnected() const { return mSocket.is_open(); }

void Connection::Send(const Message& msg) {
    asio::post(mContext, [this, msg]() {
        bool IsWritingMessage = not mQMessagesOut.empty();
        mQMessagesOut.push_back(msg);
        if (not IsWritingMessage) {
            WriteHeader();
        }
    });
}

uint64_t Connection::Scramble(uint64_t inInput) {
    //     uint64_t out = inInput ^ 0xDEABCEDEF;
    //     out          = (out & 0xF0F0F0F0F0) >> 4 | (out & 0xF0F0F0F0) << 4;
    //     return out ^ 0xC0DEFACE1234;
    return inInput;
}

Connection::Connection(Owner inParent,
                       asio::io_context& inAsioContext,
                       asio::ip::tcp::socket inSocket,
                       TsQueue<OwnedMessage>& inOwnedMessages)
    : mContext(inAsioContext), mSocket(std::move(inSocket)), mQMessagesIn(inOwnedMessages) {
    mOwnerType = inParent;
    if (mOwnerType == Owner::Server) {
        mHandShakeOut   = uint64_t(std::chrono::system_clock::now().time_since_epoch().count());
        mHandshakeCheck = Scramble(mHandShakeOut);
    } else {
        mHandShakeIn  = 0;
        mHandShakeOut = 0;
    }
}

void Connection::WriteValidation() {
    asio::async_write(mSocket,
                      asio::buffer(&mHandShakeOut, sizeof(uint64_t)),
                      [this](std::error_code ec, std::size_t length) {
                          if (not ec) {
                              if (mOwnerType == Owner::Client) ReadHeader();
                          } else {
                              mSocket.close();
                              ksai_print("Error Wirte Validation:");
                              ksai_print(ec.message().c_str());
                          }
                      });
}

void Connection::ReadValidation(OnClientValidationFunctionType& inOnClientValidated) {

    asio::async_read(mSocket,
                     asio::buffer(&mHandShakeIn, sizeof(uint64_t)),
                     [this, &inOnClientValidated](std::error_code ec, std::size_t length) {
                         if (not ec) {
                             if (mOwnerType == Owner::Server) {
                                 if (mHandShakeIn == mHandshakeCheck) {
                                     ksai_print("Client Validated");
                                     inOnClientValidated(this->shared_from_this());
                                     ReadHeader();
                                 } else {
                                     ksai_print("Client Disconnected (Failed Validation)");
                                     mSocket.close();
                                 }
                             } else {
                                 mHandShakeOut = Scramble(mHandShakeIn);
                                 WriteValidation();
                             }
                         } else {
                             ksai_print("Client Disconnected (Read Validation)");
                             mSocket.close();
                         }
                     });
}