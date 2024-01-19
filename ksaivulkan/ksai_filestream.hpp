#include <ksai_config.hpp>

#ifndef ANDROID
#else
#include <SDL.h>
#include <SDL_rwops.h>
#include <SDL_system.h>
#endif

namespace ksai {
using fileinputstream = std::ifstream;
using fileoutputstream = std::ofstream;
//class fileinputstream : public std::ifstream {
//public:
//    fileinputstream(const std::string_view infilename, std::ios_base::openmode inmode)
//    {
//        ksai_print("File Path: %s", infilename.data());
//        mFile = SDL_RWFromFile(infilename.data(), "r");
//        if (not mFile) {
//            ksai_print("File Not Found Bruh");
//        }
//    }
//    ~fileinputstream()
//    {
//        if (mFile) {
//            SDL_RWclose(mFile);
//        }
//    }
//
//    std::string readtostring()
//    {
//        SDL_RWseek(mFile, 0, RW_SEEK_END);
//        size_t size = SDL_RWtell(mFile);
//        ksai_print("String Read Size:: %d", (int)size);
//        SDL_RWseek(mFile, 0, RW_SEEK_SET);
//
//        std::vector<char> str;
//        str.resize(size);
//        char* data = str.data();
//        SDL_RWread(mFile, data, sizeof(char), size);
//        std::string strin(str.data());
//        return strin;
//    }
//
//    size_t read(char_type* ptr, std::streamsize count)
//    {
//        if (not mFile)
//            return 0;
//        return SDL_RWread(mFile, ptr, sizeof(char_type), count);
//    }
//
//private:
//    SDL_RWops* mFile = nullptr;
//};
//
//class fileoutputstream : public std::ofstream {
//public:
//    fileoutputstream(const std::string_view infilename, std::ios_base::openmode inmode)
//    {
//        mFile = SDL_RWFromFile(infilename.data(), "w");
//        if (not mFile) {
//            ksai_print("File Not Found Bruh");
//        }
//    }
//
//    size_t write(char_type* ptr, std::streamsize count)
//    {
//        if (not mFile)
//            return 0;
//        return SDL_RWwrite(mFile, ptr, sizeof(char_type), count);
//    }
//
//private:
//    SDL_RWops* mFile = nullptr;
//};
//#endif // !ANDROID
}
