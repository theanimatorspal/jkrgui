#include "Udp.hpp"
#include "Message.hpp"
namespace Jkr::Network {

void UDP::Recieve(Func inRecieve) {
    mUDPSocket.async_receive(asio::buffer(mBuffer.data(), sizeof(char) * mBuffer.size()),
                             [=, this](std::error_code ec, size_t inSize) {
                                 if (not ec) {
                                     std::cout << "Received:" << inSize << std::endl;
                                     inRecieve(mBuffer);
                                 } else {
                                     std::cout << "ERROR:" << ec.message() << std::endl;
                                 }
                             });
}
void UDP::Send(v<char> inData, std::string inDestination, int inPort) {
    asio::ip::udp::endpoint Remote(asio::ip::address::from_string(s(inDestination)), inPort);
    mUDPSocket.async_send_to(asio::buffer(&inData, sizeof(char) * inData.size()),
                             Remote,
                             [&](std::error_code ec, size_t inSize) {
                                 if (not ec) {
                                     std::cout << "Sent" << inSize << std::endl;
                                 } else {
                                     std::cout << "ERROR:" << ec.message() << std::endl;
                                 }
                             });
}

void UDP::SetBufferSize(uint32_t inSize) {
    mBufferSize = inSize;
    mBuffer.resize(inSize);
}

} // namespace Jkr::Network