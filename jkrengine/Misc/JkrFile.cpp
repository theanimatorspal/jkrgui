#include "JkrFile.hpp"

namespace Jkr::Misc {

using namespace ksai;
using namespace std;

FileJkr::FileJkr() { mOnlyInMemory = true; }

void FileJkr::PutDataFromMemory(v<char> &inData) {
    std::stringstream sstream;
    sstream << sv(inData.begin(), inData.end());
    char id[IdSize];
    size_type HeaderSize = 0;
    sstream.read(id, IdSize);
    if (strcmp(id, "HEADER_START") == 0) {
        sstream.read(reinterpret_cast<char *>(&HeaderSize),
                     sizeof(size_type)); // Skip the location
        sstream.read(reinterpret_cast<char *>(&HeaderSize), sizeof(size_type));

        while (strcmp(id, "HEADER_END") != 0) {
            sstream.read(id, IdSize);
            size_type location, size;
            sstream.read(reinterpret_cast<char *>(&location), sizeof(size_type));
            sstream.read(reinterpret_cast<char *>(&size), sizeof(size_type));
            auto header = Header{"", location, size};
            strcpy(header.mId, id);
            mFileContents[s(id)] = header;
            if (strcmp(id, "HEADER_END") != 0) {
                PushVector(mHeader, Serialize(header));
            }
        }

        auto Datasize = mFileContents["HEADER_END"].mSize;
        mData.resize(Datasize);
        sstream.read(mData.data(), Datasize);
    } else {
        Log("This File Seems not to be a JkrGUI file, it will be overriden", "WARNING");
    }
}

v<char> FileJkr::GetDataFromMemory() {
    auto fileheader_start = Serialize(Header{"HEADER_START", 0, mHeader.size()});
    auto &headercontents  = mHeader;
    auto fileheader_end   = Serialize(Header{"HEADER_END", 0, mData.size()});
    auto &data            = mData;

    v<char> out;
    out.resize(fileheader_start.size() + headercontents.size() + fileheader_end.size() +
               data.size());

    auto O = out.begin();
    std::copy(fileheader_start.begin(), fileheader_start.end(), O);
    std::copy(headercontents.begin(), headercontents.end(), O + fileheader_start.size());
    std::copy(fileheader_end.begin(),
              fileheader_end.end(),
              O + fileheader_start.size() + headercontents.size());
    std::copy(data.begin(),
              data.end(),
              O + fileheader_start.size() + headercontents.size() + fileheader_end.size());
    return out;
}

FileJkr::FileJkr(s inFileName) {
    mOnlyInMemory = true;
    mFileName     = inFileName;
    bool exists   = false;
    if (filesystem::exists(inFileName)) {
        mFile = std::fstream(inFileName, ios_base::in | ios_base::out | ios_base::binary);
        try {
            char id[IdSize];
            size_type HeaderSize = 0;
            mFile.read(id, IdSize);
            if (strcmp(id, "HEADER_START") == 0) {
                mFile.read(reinterpret_cast<char *>(&HeaderSize),
                           sizeof(size_type)); // Skip the location
                mFile.read(reinterpret_cast<char *>(&HeaderSize), sizeof(size_type));

                while (strcmp(id, "HEADER_END") != 0) {
                    mFile.read(id, IdSize);
                    size_type location, size;
                    mFile.read(reinterpret_cast<char *>(&location), sizeof(size_type));
                    mFile.read(reinterpret_cast<char *>(&size), sizeof(size_type));
                    auto header = Header{"", location, size};
                    strcpy(header.mId, id);
                    mFileContents[s(id)] = header;
                    if (strcmp(id, "HEADER_END") != 0) {
                        PushVector(mHeader, Serialize(header));
                    }
                }

                auto Datasize = mFileContents["HEADER_END"].mSize;
                mData.resize(Datasize);
                mFile.read(mData.data(), Datasize);
            } else {
                Log("This File Seems not to be a JkrGUI file, it will be overriden", "ERROR");
            }
        } catch (const std::exception &e) {
            Log(e.what(), "ERROR");
        }
    } else {
        mFile = std::fstream(inFileName, ios_base::out | ios_base::binary);
    }
}

FileJkr::~FileJkr() {
    if (not mOnlyInMemory) {
        Commit();
        Log(mDebugStringStream.str());
    }
}

void FileJkr::Commit() {
    if (mWrites > 0 and not mOnlyInMemory) {
        auto fileheader_start = Serialize(Header{"HEADER_START", 0, mHeader.size()});
        auto &headercontents  = mHeader;
        auto fileheader_end   = Serialize(Header{"HEADER_END", 0, mData.size()});
        auto &data            = mData;

        mFile.seekg(0, ios_base::beg);
        mFile.write(fileheader_start.data(), fileheader_start.size());
        mFile.write(headercontents.data(), headercontents.size());
        mFile.write(fileheader_end.data(), fileheader_end.size());
        mFile.write(data.data(), data.size());
    }
}

void FileJkr::Clear() {
    mHeader.clear();
    mData.clear();
    mFileContents.clear();
}

s FileJkr::Hash(const sv input) {
    std::hash<sv> hasher;
    size_t hash = hasher(input); 

    std::ostringstream oss;
    oss << std::hex << std::setw(28) << std::setfill('0') << hash;

    return oss.str().substr(0, 28); 
}

} // namespace Jkr::Misc