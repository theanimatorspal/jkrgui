#pragma once
#include <fstream>
#include <sstream>
#include "ksai_config.hpp"
namespace Jkr::Misc {

using namespace ksai;
using size_type = size_t;
///@todo Rename this to FileHeaderIdSize or something similar
constexpr int IdSize = 16;

template <typename T>
concept IsContainer = requires(T &t) {
    t.data();
    t.size();
};

template <typename T> v<char> Serialize(T inT) {
    std::vector<char> out;
    ///@warning this "not IsContainer<T>" is written for Android NDK, as it returns true for
    /// std::string in is_standard_layout<>
    if constexpr (std::is_standard_layout_v<T> and not IsContainer<T>) {
        out.resize(sizeof(T));
        std::memcpy(out.data(), &inT, sizeof(T));
    } else if constexpr (IsContainer<T>) {
        size_type sizeOfUnderlyingType = sizeof(typename T::value_type);
        size_type size                 = sizeOfUnderlyingType * inT.size();
        out.resize(size);
        std::memcpy(out.data(), inT.data(), size);
    }
    return out;
}

template <typename T> void PushVector(std::vector<T> &original, const std::vector<T> &toPush) {
    original.insert(original.end(), toPush.begin(), toPush.end());
}

template <typename T> T Retrive(v<char> inData) {
    T out;
    if constexpr (std::is_standard_layout_v<T> and not IsContainer<T>) {
        std::memcpy(&out, inData.data(), inData.size());
    } else if constexpr (IsContainer<T>) {
        size_type sizeOfUnderlyingType = sizeof(typename T::value_type);
        assert(inData.size() % sizeOfUnderlyingType == 0 && "Yeah, data should be valid here");
        size_type size = inData.size() / sizeOfUnderlyingType;
        out.resize(size);
        std::memcpy(out.data(), inData.data(), inData.size());
    }
    return out;
}
/*
"HEADER_START" -> IdSize bytes
LOCATION -> size_type bytes <Doesn't store anything significant>
SIZE_OF_HEADER -> size_type bytes
----------------------
ID -> IdSize bytes
LOCATION -> size_type bytes
SIZE_OF_ID -> size_type bytes
ID
LOCATION
SIZE_OF_ID
ID
LOCATION
SIZE_OF_ID
----------------------
"HEADER_END" -> IdSize bytes
>>>>DATA<<<<<<

*/

struct File {
    struct Header {
        char mId[IdSize];
        size_type mLocation;
        size_type mSize;
    };
    GETTER GetFileName() const { return mFileName; }
    GETTER &GetFileContents() { return mFileContents; }

    File();
    File(s inFileName);
    ~File();
    template <typename T> void Write(const sv inId, T inData) {
        auto data   = Serialize(inData);
        auto header = Header{"", mData.size(), data.size()};
        auto hash = Hash(inId);
        strcpy(header.mId, hash.data());

        if (not mFileContents.contains(hash)) {
            mDebugStringStream << "WROTE::> " << header.mId << " : " << header.mLocation << " : "
                               << header.mSize / 1024.0f << " KiB\n";

            mFileContents[hash] = header;
            PushVector(mHeader, Serialize(header));
            PushVector(mData, data);
        }
        mWrites++;
    }
    bool HasEntry(const sv inId) {
        if (mFileContents.contains(Hash(inId))) {
            return true;
        }
        return false;
    }
    bool IsEmpty() { return mFileContents.empty(); }
    template <typename T> T Read(const sv inId) {
        auto header = mFileContents[Hash(inId)];
        mDebugStringStream << "READ::> " << header.mId << " : " << header.mLocation << " : "
                           << header.mSize / 1024.0f << " KiB" << std::endl;
        auto out = v<char>(mData.begin() + header.mLocation,
                           mData.begin() + header.mLocation + header.mSize);

        return Retrive<T>(out);
    }
    template <typename T> T ReadType(const sv inId, T inType)
    {
        return Read<T>(inId);
    }
    template <typename T> T ReadAndErase(const sv inId) {
        auto out = Read<T>(inId);
        mFileContents.erase(Hash(inId));
        return out;
    }
    void PutDataFromMemory(v<char> &inData);
    v<char> GetDataFromMemory();

    void Commit();
    void Clear();

    private:
    s Hash(const sv input);
    bool mOnlyInMemory = false;
    v<char> mData;
    v<char> mHeader;
    umap<s, Header> mFileContents; // This is for retrieval
    ///@note This is for debugging, IDK why it is not printing whole thing
    std::stringstream mDebugStringStream;

    std::fstream mFile;
    s mFileName;
    int mWrites = 0;
};

} // namespace Jkr::Misc