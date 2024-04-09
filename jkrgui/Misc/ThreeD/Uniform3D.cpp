#include "Uniform3D.hpp"
#include "TestUtils.hpp"

using namespace Jkr::Misc::_3D;

void Uniform3D::AddTexture(int inDstBinding, s inFileName) {
          Up<ImageType> Image = MakeUp<ImageType>(mInstance);
          Image->Setup(inFileName);
          Image->Register(0, inDstBinding, 0, *mVulkanDescriptorSet);
          mImages[inDstBinding] = mv(Image);
}

void Uniform3D::AddBuffer(int inDstBinding, size_t inSize) {
          Up<BufferType> Buffer = MakeUp<BufferType>(mInstance);
          Buffer->Setup(inSize);
          mBuffers[inDstBinding] = mv(Buffer);
}