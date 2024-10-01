#include <fstream>
#include "ksai_config.hpp"

namespace Jkr::Misc {
using namespace ksai;
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

using size_type      = size_t;
constexpr int IdSize = 128;
struct FileJkr {
    struct Header {
        char mId[IdSize];
        size_type mLocation;
        size_type mSize;
    };

    FileJkr(s inFileName);
    template <typename T> void Write(const char inId[IdSize], T inData);
    template <typename T> T Read(const char inId[IdSize]);
    void Commit();

    private:
    v<char> mData;
    v<char> mHeader;
    umap<std::string, Header> mFileContents; // This is for retrieval

    ///@note this is what it is going to be written to the file after you hit Commit
    v<char> mCommittedData;
    std::fstream mFile;
};

} // namespace Jkr::Misc