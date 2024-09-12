#pragma once
#include "Network.hpp"

namespace Jkr::Network {

template <typename T>
concept IsContainer = requires(T &t) {
    t.data();
    t.size();
};

struct MessageHeader {
    int mId        = -1;
    uint32_t mSize = 0;
};

struct Message {
    MessageHeader mHeader;
    std::vector<uint8_t> mBody;
    size_t size() const { return mBody.size(); }
    template <typename DataType> void Insert(const DataType &inData) { *this << inData; }
    template <typename DataType> DataType Get() {
        DataType type;
        *this >> type;
        return type;
    }

    friend std::ostream &operator<<(std::ostream &os, const Message &msg) {
        os << "ID:" << int(msg.mHeader.mId) << " Size:" << msg.mHeader.mSize;
        return os;
    }
    template <typename DataType> friend Message &operator<<(Message &msg, const DataType &data) {
        static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to be pushed");
        size_t i = msg.mBody.size();
        msg.mBody.resize(i + sizeof(DataType));
        std::memcpy(msg.mBody.data() + i, &data, sizeof(DataType));
        msg.mHeader.mSize = msg.size();
        return msg;
    }
    template <typename DataType> friend Message &operator>>(Message &msg, DataType &data) {
        static_assert(std::is_standard_layout<DataType>::value,
                      "Data is too complex to be recieved");
        size_t i = msg.mBody.size() - sizeof(DataType);
        std::memcpy(&data, msg.mBody.data() + i, sizeof(DataType));
        msg.mBody.resize(i);
        msg.mHeader.mSize = msg.size();
        return msg;
    }

    /// @brief Multiple things cannot be >> in the message after a container is pushed
    ///
    ///
    template <typename DataType> void InsertEXT(DataType indata) {
        if constexpr (std::is_standard_layout_v<DataType>) {
            *this << indata;
        } else if constexpr (IsContainer<DataType>) {
            mBody.clear();
            const size_t data_size = indata.size() * sizeof(indata.front());
            mBody.resize(data_size);
            auto src = indata.data();
            auto dst = mBody.data();
            std::memcpy(dst, src, data_size);
            mHeader.mSize = mBody.size();
        }
    }

    /// @brief This will not erase the whole message, but you cannot
    /// add more stuff or take out more stuff from here
    ///
    ///
    template <typename DataType> DataType GetEXT() {
        DataType IncomingData;
        size_t IncomingDataSize = (*this).mHeader.mSize;

        if constexpr (std::is_standard_layout_v<DataType>) {
            (*this) >> IncomingData;
            return IncomingData;
        } else if constexpr (IsContainer<DataType>) {
            IncomingData.resize(IncomingDataSize / sizeof(std::underlying_type<DataType>));
            auto dst = IncomingData.data();
            auto src = mBody.data();
            std::memcpy(dst, src, IncomingDataSize);
            return IncomingData;
        }
    }
};

struct Connection;

struct OwnedMessage : public Message {
    sp<Connection> mRemote = nullptr;
};
} // namespace Jkr::Network