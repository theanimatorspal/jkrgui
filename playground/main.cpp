#include <Misc/JkrFile.hpp>

int main() {
    using namespace Jkr::Misc;
    {
        FileJkr file("hello.jkr");
        v<glm::vec3> vec;
        vec.push_back(glm::vec3(1, 2, 3));
        vec.push_back(glm::vec3(3, 2, 1));
        vec.push_back(glm::vec3(3, 2, 5));
        file.Write("THIS", vec);
        file.Write("DARSHAN_STRING", std::string("MY NAME IS DARSHAN KOIRALA"));
        file.Write("FUCK", std::string("ANOTEHR STRING TO FURTHER TEST THINS THINGS"));
    }

    {
        FileJkr file("hello.jkr");
        auto vec = file.Read<v<glm::vec3>>("THIS");
        for (auto &v : vec) {
            std::cout << std::format("VEC: {}, {}, {}\n", v.x, v.y, v.z);
        }
        auto Name = file.Read<std::string>("DARSHAN_STRING");
        std::cout << Name << std::endl;
        auto AnotherString = file.Read<std::string>("FUCK");
        std::cout << AnotherString << std::endl;
    }
}
