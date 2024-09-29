#pragma once
#include "Network.hpp"
#include "Message.hpp"
namespace Jkr::Network {

class UDP {
    ///@todo Inspect for performance, ensure there is no latency anywhere except the network side
    using Func = std::function<void(std::vector<char>)>;
    using s    = std::string;
    using sv   = std::string_view;

    public:
    UDP(int inPort) : mUDPSocket(mContext, asio::ip::udp::endpoint(asio::ip::udp::v4(), inPort)) {
        mThreadContext = std::thread([this]() {
            try {
                mContext.run();
            } catch (std::exception &e) {
                std::cout << e.what() << std::endl;
            }
        });
    }
    ~UDP() {
        mContext.stop();
        if (mThreadContext.joinable()) {
            mThreadContext.join();
        }
    }
    void Recieve(Func inMessageRecFunc);
    void Send(v<char> inData, std::string inDestination, int inPort);
    void SetBufferSize(uint32_t inSize);

    private:
    asio::io_context mContext;
    asio::ip::udp::socket mUDPSocket;
    std::thread mThreadContext;

    ui mBufferSize = 0;
    v<char> mBuffer;
};

} // namespace Jkr::Network