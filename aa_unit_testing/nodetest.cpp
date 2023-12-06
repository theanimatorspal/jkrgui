#include "glTF_Model_base.hpp"
#include <Renderers/ThreeD/glTF_Model_base.hpp>

int main()
{
    Jkr::Instance Instance;
    Jkr::Renderer::glTF::glTF_Model_base mModel(Instance,
                                                "D:\\glTF-Sample-Models\\2.0\\Box\\glTF\\Box.gltf");
}
