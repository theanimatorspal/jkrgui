#include "Udp.hpp"
#include "Message.hpp"
namespace Jkr::Network {

void UDP::Recieve(Func inRecieve) {
    mUDPSocket.async_receive(asio::buffer(mBuffer.data(), sizeof(char) * mBuffer.size()),
                             [=, this](std::error_code ec, size_t inSize) {
                                 if (not ec) {
                                     inRecieve(mBuffer);
                                 } else {
                                     Log(ec.message(), "ERROR");
                                 }
                                 this->Recieve(inRecieve);
                             });
}

void UDP::RecieveBlocking(Func inRecieve) {
    int size = mUDPSocket.receive(asio::buffer(mBuffer.data(), sizeof(char) * mBuffer.size()));
    if (size > 0) {
        inRecieve(mBuffer);
    }
}

void UDP::Send(v<char> inData, std::string inDestination, int inPort) {
    asio::ip::udp::endpoint Remote(asio::ip::address::from_string(s(inDestination)), inPort);
    mUDPSocket.async_send_to(asio::buffer(inData.data(), sizeof(char) * inData.size()),
                             Remote,
                             [&](std::error_code ec, size_t inSize) {
                                 if (not ec) {
                                     Log("Sent :" + std::to_string(inSize));
                                 } else {
                                     Log(ec.message(), "ERROR");
                                 }
                             });
}

void UDP::SendBlocking(v<char> inData, std::string inDestination, int inPort) {
    asio::ip::udp::endpoint Remote(asio::ip::address::from_string(s(inDestination)), inPort);
    mUDPSocket.send_to(asio::buffer(inData.data(), sizeof(char) * inData.size()), Remote);
}
void UDP::SetBufferSize(uint32_t inSize) {
    mBufferSize = inSize;
    mBuffer.resize(inSize);
}

} // namespace Jkr::Network