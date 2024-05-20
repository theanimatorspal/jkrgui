#pragma once
#include "Network.hpp"

namespace Jkr::Network {

struct MessageHeader {
    int mId        = -1;
    uint32_t mSize = 0;
};

struct Message {
    MessageHeader mHeader;
    std::vector<uint8_t> mBody;
    size_t size() const { return mBody.size(); }
    friend std::ostream& operator<<(std::ostream& os, const Message& msg) {
        os << "ID:" << int(msg.mHeader.mId) << " Size:" << msg.mHeader.mSize;
        return os;
    }
    template <typename DataType> friend Message& operator<<(Message& msg, const DataType& data) {
        static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to be pushed");
        size_t i = msg.mBody.size();
        msg.mBody.resize(i + sizeof(DataType));
        std::memcpy(msg.mBody.data() + i, &data, sizeof(DataType));
        msg.mHeader.mSize = msg.size();
        return msg;
    }
    template <typename DataType> friend Message& operator>>(Message& msg, DataType& data) {
        static_assert(std::is_standard_layout<DataType>::value,
                      "Data is too complex to be recieved");
        size_t i = msg.mBody.size() - sizeof(DataType);
        std::memcpy(&data, msg.mBody.data() + i, sizeof(DataType));
        msg.mBody.resize(i);
        msg.mHeader.mSize = msg.size();
        return msg;
    }
};

struct Connection;

struct OwnedMessage : public Message {
    sp<Connection> mRemote = nullptr;
};
} // namespace Jkr::Network