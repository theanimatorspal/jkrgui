#include "Udp.hpp"
#include "Message.hpp"
namespace Jkr::Network {

void UDP::Recieve(Func inRecieve, int inPort) {
    mUDPSocket.async_receive(
         asio::buffer(&mTemporaryMessageBuffer.mHeader, sizeof(MessageHeader)),
         [&](std::error_code ec, size_t inSize) {
             if (not ec) {
                 std::cout << "Received:" << inSize << "\n";
                 auto &msg = mTemporaryMessageBuffer;
                 msg.mBody.resize(msg.size());
                 mUDPSocket.async_receive(
                      asio::buffer(mTemporaryMessageBuffer.mBody.data(), sizeof(MessageHeader)),
                      [&](std::error_code ec, size_t inSize) {
                          if (not ec) {
                              std::cout << "Received:" << inSize << "\n";
                              inRecieve(mTemporaryMessageBuffer);
                              Recieve(inRecieve, inPort);
                          }
                      });
             }
         });
}
void UDP::Send(const Message &inMessage, std::string inDestination, int inPort) {
    asio::ip::udp::endpoint Remote(asio::ip::address::from_string(s(inDestination)), inPort);
    mUDPSocket.async_send_to(
         asio::buffer(&inMessage.mHeader, sizeof(MessageHeader)),
         Remote,
         [&](std::error_code ec, size_t inSize) {
             if (not ec) {
                 std::cout << "Sent" << inSize << "\n";
                 mUDPSocket.async_send_to(
                      asio::buffer(inMessage.mBody.data(), inMessage.mBody.size()),
                      Remote,
                      [&](std::error_code ec, size_t inSize) {
                          if (not ec) {
                              std::cout << "Sent" << inSize << "\n";
                          }
                      });
             }
         });
}
} // namespace Jkr::Network