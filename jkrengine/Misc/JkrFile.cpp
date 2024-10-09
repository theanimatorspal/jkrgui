#include "JkrFile.hpp"

namespace Jkr::Misc {

using namespace ksai;
using namespace std;

FileJkr::FileJkr(s inFileName) {
    mFileName   = inFileName;
    bool exists = false;
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
                std::cout << "This File Seems not to be a JkrGUI file, it will be overriden\n";
            }
        } catch (const std::exception &e) {
            std::cout << "ERROR:" << e.what() << std::endl;
        }
    } else {
        mFile = std::fstream(inFileName, ios_base::out | ios_base::binary);
    }
}

FileJkr::~FileJkr() { Commit(); }

void FileJkr::Commit() {
    if (mWrites > 0) {
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

} // namespace Jkr::Misc