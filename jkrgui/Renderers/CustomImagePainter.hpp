#include "../Painter.hpp"
#include "../PainterParameter.hpp"

namespace Jkr::Renderer {
using Image = PainterParameter<Jkr::PainterParameterContext::StorageImage>;
class CustomImagePainter;
class Shape;

struct CustomPainterImage {
    CustomPainterImage(const Instance& inInstance, const Window_base& inWindow, ui inWidth, ui inHeight);
    void Register(const Instance& inInstance, CustomImagePainter& inPainterCache);
    std::vector<int> GetImageToVector(const Instance& inInstance, const Window_base& inWindow);

    GETTER& GetPainterParam() { return *mPainterParam; }
    GETTER& GetDescriptorSet() { return *mVulkanDescriptorSet; }
    up<VulkanDescriptorSet> mVulkanDescriptorSet;
    up<Image> mPainterParam;
};

struct CustomImagePainter {
    using ComPar = Jkr::Window_base::ParameterContext;
    CustomImagePainter(
         sv inName, sv inComputeShaderFunction, sv inPushConstantSignature, ui inX, ui inY, ui inZ);
    CustomImagePainter(sv inName, sv inComputeShader);
    GETTER& GetPainter() { return *mPainter; }
    void Load(const Instance& inInstance, Window_base& inWindow);
    void Store(const Instance& inInstance, Window_base& inWindow);
    void RegisterImage(const Instance& inInstance, Window_base& inWindow, CustomPainterImage& inImage) {
        VulkanDescriptorUpdateHandler Handler(inInstance.GetDevice());
        inImage.GetPainterParam().Register(0, 0, 0, inImage.GetDescriptorSet());
    }
    void RegisterImageExternal(const Instance& inInstance,
                               Window_base& inWindow,
                               Shape& inShape,
                               CustomPainterImage& inImage,
                               int inImageIndex,
                               int inDstBinding);
    void Bind(const Window_base& inWindow, ComPar inPar = ComPar::None) {
        mPainter->BindComputePipeline(inWindow, inPar);
    }

    void BindImageFromImage(const Window_base& inWindow,
                            CustomPainterImage& inImage,
                            ComPar inPar = ComPar::None);
    template <class T>
    void
    Draw(Window_base& inWindow, T inPushConstant, ui inX, ui inY, ui inZ, ComPar inPar = ComPar::None) {
        mPainter->Dispatch_EXT<T>(inWindow, inPushConstant, inX, inY, inZ, inPar);
    }

    GETTER& GetPainterCache() { return *mCustomPainterCache; }

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
