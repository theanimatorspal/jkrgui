#include "../Painter.hpp"
#include "../PainterParameter.hpp"

namespace Jkr::Renderer {
using Image = PainterParameter<Jkr::PainterParameterContext::StorageImage>;
class CustomImagePainter;
class Shape;
using ComPar = Jkr::Window_base::ParameterContext;

struct CustomPainterImage {
    CustomPainterImage(Instance &inInstance, Window_base &inWindow, ui inWidth, ui inHeight);
    void SyncBefore(Window_base &inWindow, ComPar inPar = ComPar::None);
    void SyncAfter(Window_base &inWindow, ComPar inPar = ComPar::None);
    void Register(Instance &inInstance, CustomImagePainter &inPainterCache, int inIndex = 0);
    v<char> GetVector(Instance &inInstance);

    GETTER &GetPainterParam() { return *mPainterParam; }
    GETTER &GetDescriptorSet() { return *mVulkanDescriptorSet; }
    up<VulkanDescriptorSet> mVulkanDescriptorSet;
    up<Image> mPainterParam;
};

struct CustomImagePainter {
    CustomImagePainter(
         sv inName, sv inComputeShaderFunction, sv inPushConstantSignature, ui inX, ui inY, ui inZ);
    CustomImagePainter(sv inName, sv inComputeShader);
    GETTER &GetPainter() { return *mPainter; }
    void Store(Instance &inInstance, Window_base &inWindow, Jkr::Misc::File& inFile);
    void Bind(Window_base &inWindow, ComPar inPar = ComPar::None) {
        mPainter->BindComputePipeline(inWindow, inPar);
    }
    ///@warning this is nonsensical design, this resides here because
    /// PipelineLayout is required for binding descriptor sets (from the CustomPainterImage)
    ///@note Any CustomImagePainter's layout that has a single image will work here, so no
    /// need to rebind CustomImagePainter
    void BindImageFromImage(Window_base &inWindow,
                            CustomPainterImage &inImage,
                            ComPar inPar = ComPar::None);
    template <class T>
    void Draw(Window_base &inWindow,
              T inPushConstant,
              ui inX,
              ui inY,
              ui inZ,
              ComPar inPar = ComPar::None) {
        mPainter->Dispatch_EXT<T>(inWindow, inPushConstant, inX, inY, inZ, inPar);
    }

    GETTER &GetPainterCache() { return *mCustomPainterCache; }

    private:
    s mCustomPainterFileName = "customPainter.bin";
    std::ostringstream mComputeStream;
    std::ostringstream mVertexStream;
    std::ostringstream mFragmentStream;
    glm::vec3 mThreads; // TODO Remove this
    Up<PainterCache> mCustomPainterCache;
    Up<Painter> mPainter;
};
} // namespace Jkr::Renderer
