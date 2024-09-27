#pragma once
#include "Network.hpp"
#include "Message.hpp"
namespace Jkr::Network {

class UDP {
    using Func = std::function<void(Message)>;
    using s    = std::string;
    using sv   = std::string_view;

    public:
    UDP(int inPort) : mUDPSocket(mContext, asio::ip::udp::endpoint(asio::ip::udp::v4(), inPort)) {
        mContext.run();
    }
    /// @todo Improve std::string is copied and else.
    void Recieve(Func inMessageRecFunc, int inPort);
    void Send(const Message &inMessage, std::string inDestination, int inPort);

    private:
    asio::io_context mContext;
    asio::ip::udp::socket mUDPSocket;
    Message mTemporaryMessageBuffer;
};

} // namespace Jkr::Network