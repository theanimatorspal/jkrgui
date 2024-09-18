#pragma once
#include "../CustomImagePainter.hpp"
#include "../Renderer_base.hpp"
#include "Renderers/Renderer_base.hpp"
#include "Shape_base.hpp"
#include "VulkanImage.hpp"
#include "glm/fwd.hpp"
#include <unordered_map>

namespace Jkr::Renderer {
enum class FillType { Fill, Image, ContinousLine };
class Shape : public Shape_base, Renderer_base {
    using sb              = Shape_base;
    using rb              = Renderer_base;
    using CopyCommandType = std::function<void(
         const ksai::VulkanCommandBuffer &, ksai::VulkanImageBase &, ksai::VulkanImageBase &)>;

    public:
    GETTER &GetImages() { return mImages; }

    Shape(const Instance &inInstance,
          std::unordered_map<FillType, Up<PainterCache>> &inPainterCaches)
        : mInstance(inInstance), mPainterCaches(inPainterCaches) {}
    Shape(const Instance &inInstance,
          Window_base &inCompatibleWindow,
          std::unordered_map<FillType, Up<PainterCache>> &inPainterCaches,
          uint32_t inVarDesCount = 5000);
    void Add(Jkr::Generator &inShape, float inX, float inY, float inZ, uint32_t &outId);
    void Update(uint32_t inId, Jkr::Generator &inShape, float inX, float inY, float inZ);
    void Dispatch(Window_base &inWindow, CmdParam inParam = CmdParam::None);
    void BindFillMode(FillType inType, Window_base &inWindow, CmdParam inParam = CmdParam::UI);
    void BindShapes(Window_base &inWindow, CmdParam inParam = CmdParam::UI);
    void Draw(Window_base &inWindow,
              glm::vec4 inColor,
              uint32_t inWindowW,
              uint32_t inWindowH,
              uint32_t inStartShapeId,
              uint32_t inEndShapeId,
              glm::mat4 inMatrix);

    // TODO Needs to be separated
    void AddImage(const std::string_view inFileName, uint32_t &outIndex);
    void AddImage(uint32_t inWidth, uint32_t inHeight, uint32_t &outIndex);
    void AddImage(v<uc> &inImage, ui inWidth, ui inHeight, uint32_t &outIndex);
    void UpdateImage(ui inId, v<uc> &inImage, ui inWidth, ui inHeight);
    void CopyToImage(uint32_t inId,
                     uint32_t inWidth,
                     uint32_t inHeight,
                     CustomPainterImage &inPainterImage);
    void CopyFromImage(uint32_t inId,
                       uint32_t inWidth,
                       uint32_t inHeight,
                       CustomPainterImage &inPainterImage);
    void CopyFromImage(uint32_t inId, CustomPainterImage &inPainterImage);
    void CopyToImage(uint32_t inId, CustomPainterImage &inPainterImage);
    void BindImage(Window_base &inWindow, uint32_t inImageId, CmdParam inParam = CmdParam::UI);

    glm::vec2 GetImageSize(const sv inFileName);
    ui AddImageEXT(const std::string_view inFileName);
    ui AddImageEXT(uint32_t inWidth, uint32_t inHeight);
    ui AddImageEXT(v<uc> inImage, ui inWidth, ui inHeight);

    ui AddEXT(Jkr::Generator &inShape, glm::vec3 inPosition);
    void UpdateEXT(ui inId, Jkr::Generator &inShape, glm::vec3 inPosition);
    void DrawEXT(Window_base &inWindow,
                 glm::vec4 inColor,
                 ui inStartShapeId,
                 ui inEndShapeId,
                 glm::mat4 inMatrix,
                 CmdParam inParam);

    private:
    struct PushConstant {
        glm::mat4 mMatrix;
        glm::vec4 mColor;
        glm::vec4 mParams;
    };
    uint32_t mTotalNoOfVerticesRendererCanHold = rb::InitialRendererElementArraySize;
    uint32_t mTotalNoOfIndicesRendererCanHold  = rb::InitialRendererElementArraySize;
    FillType mCurrentFillMode                  = FillType::Fill;

    private:
    void CheckAndResize(const Jkr::Generator &inShape);
    const Instance &mInstance;
    std::unordered_map<FillType, Up<PainterCache>> &mPainterCaches;
    std::unordered_map<FillType, Up<Painter>> mPainters;
    Up<Primitive> mPrimitive;

    // TODO Really bad design, separate this to a Copier
    private:
    std::vector<Up<ImageType>> mImages;
    std::vector<uint32_t> mImagesToBeCopiedIds;
    std::vector<VulkanImageBase *> mImagesToBeCopiedFrom;
    std::vector<CopyCommandType> mImageCopyCommands;

#ifndef JKR_USE_VARIABLE_DES_INDEXING
    std::vector<Up<VulkanDescriptorSet>> mVulkanPerImageDescriptorSets;
#else
    Up<VulkanDescriptorSet> mVarDesVulkanDescriptorSet;
#endif

#ifdef JKR_USE_VARIABLE_DES_INDEXING
    uint32_t mCurrentImageIndex;
#endif
};
} // namespace Jkr::Renderer
