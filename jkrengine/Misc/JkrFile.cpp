#include "JkrFile.hpp"

namespace Jkr::Misc {

using namespace ksai;
using namespace std;

template <typename T>
concept IsContainer = requires(T &t) {
    t.data();
    t.size();
};

template <typename T> v<char> Serialize(T inT) {
    std::vector<char> out;
    ///@warning this not IsContainer<T> is written for Android NDK, as it returns true for
    /// std::string in is_standard_layout<>
    if constexpr (is_standard_layout_v<T> and not IsContainer<T>) {
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
    if constexpr (is_standard_layout_v<T> and not IsContainer<T>) {
        std::memcpy(&out, inData.data(), inData.size());
    } else if constexpr (IsContainer<T>) {
        assert(is_standard_layout_v<T>);
        size_type sizeOfUnderlyingType = sizeof(typename T::value_type);
        assert(inData.size() % sizeOfUnderlyingType == 0 && "Yeah, data is not valid here");
        size_type size = inData.size() / sizeOfUnderlyingType;
        out.resize(size);
        std::memcpy(out.data(), inData.data(), inData.size());
    }
    return out;
}

FileJkr::FileJkr(s inFileName) {
    bool exists = false;
    if (filesystem::exists(inFileName)) {
        exists = true;
    }
    mFile = std::fstream(inFileName, ios_base::in | ios_base::out | ios_base::binary);
    if (exists) {
        char id[IdSize];
        mFile.read(id, IdSize);
        if (strcmp(id, "HEADER_START")) {
            mFile.read(id, sizeof(size_type));
            mFile.read(id, sizeof(size_type));
        } else {
            assert(false && "ERROR: This file is not right, probably not jkrgui file");
        }
    }
}

void FileJkr::Commit() {
    auto fileheader_start = Serialize(Header{"HEADER_START", 0, mHeader.size()});
    auto &headercontents  = mHeader;
    auto fileheader_end   = Serialize(Header{"HEADER_END", 0, mHeader.size()});
    auto &data            = mData;

    mFile.write(fileheader_start.data(), fileheader_start.size());
    mFile.write(headercontents.data(), headercontents.size());
    mFile.write(fileheader_end.data(), fileheader_end.size());
    mFile.write(data.data(), data.size());
}

template <> void FileJkr::Write<v<glm::vec3>>(const char inId[IdSize], v<glm::vec3> inData) {
    auto data                   = Serialize(inData);
    auto header                 = Header{"", mData.size(), data.size()};

    mFileContents[string(inId)] = header;
    PushVector(mHeader, Serialize(header));
    PushVector(mData, data);
}

template <> v<glm::vec3> FileJkr::Read(const char inId[IdSize]) {
    auto header = mFileContents[string(inId)];
    auto out =
         v<char>(mData.begin() + header.mLocation, mData.begin() + header.mLocation + header.mSize);

    return Retrive<v<glm::vec3>>(out);
}

} // namespace Jkr::Misc