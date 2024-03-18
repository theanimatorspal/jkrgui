#pragma once
#include "../CustomImagePainter.hpp"
#include "../Renderer_base.hpp"
#include "Shape_base.hpp"
#include "VulkanImage.hpp"
#include "glm/fwd.hpp"
#include <unordered_map>

namespace Jkr::Renderer {
enum class FillType { Fill, Image, ContinousLine };
class Shape : public Shape_base, Renderer_base {
          using sb = Shape_base;
          using rb = Renderer_base;
          using CopyCommandType = std::function<void(const ksai::VulkanCommandBuffer&, ksai::VulkanImageBase&, ksai::VulkanImageBase&)>;

      public:
          Shape(const Instance& inInstance, std::unordered_map<FillType, Up<PainterCache>>& inPainterCaches)
              : mInstance(inInstance), mPainterCaches(inPainterCaches) {}
          Shape(const Instance& inInstance,
                Window& inCompatibleWindow,
                std::unordered_map<FillType, Up<PainterCache>>& inPainterCaches,
                uint32_t inVarDesCount = 5000);
          void Add(Jkr::Generator& inShape, int inX, int inY, uint32_t inZ, uint32_t& outId);
          void AddImage(const std::string_view inFileName, uint32_t& outIndex);
          void AddImage(uint32_t inWidth, uint32_t inHeight, uint32_t& outIndex);
          void AddImage(v<uc> inImage, ui inWidth, ui inHeight, uint32_t& outIndex);
          void UpdateImage(ui inId, v<uc> inImage, ui inWidth, ui inHeight);
          void CopyToImage(uint32_t inId, uint32_t inWidth, uint32_t inHeight, CustomPainterImage& inPainterImage);
          void CopyToImage(uint32_t inId, CustomPainterImage& inPainterImage);
          void Update(uint32_t inId, Jkr::Generator& inShape, int inX, int inY, uint32_t inZ);
          void Dispatch(Window& inWindow, CmdParam inParam = CmdParam::None);
          void BindFillMode(FillType inType, Window& inWindow, CmdParam inParam = CmdParam::UI);
          void BindImage(Window& inWindow, uint32_t inImageId, CmdParam inParam = CmdParam::UI);
          void BindShapes(Window& inWindow);
          void Draw(Window& inWindow,
                    glm::vec4 inColor,
                    uint32_t inWindowW,
                    uint32_t inWindowH,
                    uint32_t inStartShapeId,
                    uint32_t inEndShapeId,
                    glm::mat4 inMatrix);

          ui AddEXT(Jkr::Generator& inShape, glm::vec3 inPosition);
          ui AddImageEXT(const std::string_view inFileName);
          ui AddImageEXT(uint32_t inWidth, uint32_t inHeight);
          ui AddImageEXT(v<uc> inImage, ui inWidth, ui inHeight);
          void UpdateEXT(ui inId, Jkr::Generator& inShape, glm::vec3 inPosition);
          void DrawEXT(Window& inWindow, glm::vec4 inColor, ui inStartShapeId, ui inEndShapeId, glm::mat4 inMatrix);

      private:
          struct PushConstant {
                    glm::mat4 mMatrix;
                    glm::vec4 mColor;
                    glm::vec4 mParams;
          };
          uint32_t mTotalNoOfVerticesRendererCanHold = rb::InitialRendererElementArraySize;
          uint32_t mTotalNoOfIndicesRendererCanHold = rb::InitialRendererElementArraySize;
          FillType mCurrentFillMode = FillType::Fill;

      private:
          void CheckAndResize(const Jkr::Generator& inShape);
          const Instance& mInstance;
          std::unordered_map<FillType, Up<PainterCache>>& mPainterCaches;
          std::unordered_map<FillType, Up<Painter>> mPainters;
          Up<Primitive> mPrimitive;

      private:
          std::vector<Up<ImageType>> mImages;
          std::vector<uint32_t> mImagesToBeCopiedIds;
          std::vector<VulkanImageBase*> mImagesToBeCopiedFrom;
          std::vector<CopyCommandType> mImageCopyCommands;

#ifndef JKR_USE_VARIABLE_DES_INDEXING
          std::vector<Up<VulkanDescriptorSet>> mVulkanPerImageDescriptorSets;
#else
          Up<VulkanDescriptorSet> mVarDesVulkanDescriptorSet;
#endif

#ifdef JKR_USE_VARIABLE_DES_INDEXING
          uint32_t mCurrentImageIndex;
#endif

      private:
          glm::vec2 mSizeFactor = {1, 1};

      public:
          SETTER SetSizeFactor(glm::vec2 inFactor) { mSizeFactor = inFactor; }
};
} // namespace Jkr::Renderer

inline ksai::ui Jkr::Renderer::Shape::AddEXT(Jkr::Generator& inShape, glm::vec3 inPosition) {
          ui i;
          Add(inShape, inPosition.x, inPosition.y, inPosition.z, i);
          return i;
}

inline void Jkr::Renderer::Shape::CopyToImage(uint32_t inId, CustomPainterImage& inPainterImage) {
          CopyToImage(inId,
                      inPainterImage.GetPainterParam().GetStorageImagePtr()->GetImageExtent().width,
                      inPainterImage.GetPainterParam().GetStorageImagePtr()->GetImageExtent().height,
                      inPainterImage);
}

inline ksai::ui Jkr::Renderer::Shape::AddImageEXT(const std::string_view inFileName) {
          ui i;
          AddImage(inFileName, i);
          return i;
}

inline ksai::ui Jkr::Renderer::Shape::AddImageEXT(uint32_t inWidth, uint32_t inHeight) {
          ui i;
          AddImage(inWidth, inHeight, i);
          return i;
}

inline ksai::ui Jkr::Renderer::Shape::AddImageEXT(v<uc> inImage, ui inWidth, ui inHeight) {
          ui i;
          AddImage(inImage, inWidth, inHeight, i);
          return i;
}

inline void Jkr::Renderer::Shape::UpdateEXT(ui inId, Jkr::Generator& inShape, glm::vec3 inPosition) {
          Update(inId, inShape, inPosition.x, inPosition.y, inPosition.z);
}

inline void Jkr::Renderer::Shape::DrawEXT(Window& inWindow, glm::vec4 inColor, ui inStartShapeId, ui inEndShapeId, glm::mat4 inMatrix) {
          PushConstant Push;
          Push.mColor = inColor;
          Push.mMatrix = inMatrix;

#ifdef JKR_USE_VARIABLE_DES_INDEXING
          Push.mParams.r = mCurrentImageIndex;
#endif

          mPainters[mCurrentFillMode]->Draw_EXT<PushConstant>(*mPrimitive,
                                                              Push,
                                                              inWindow,
                                                              sb::GetEndIndexOffsetAbsolute(inEndShapeId) -
                                                               sb::GetIndexOffsetAbsolute(inStartShapeId), // TODO
                                                              1,
                                                              sb::GetIndexOffsetAbsolute(inStartShapeId),
                                                              0);
}