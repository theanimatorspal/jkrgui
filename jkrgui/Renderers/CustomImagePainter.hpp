#include "../Painter.hpp"
#include "../PainterParameter.hpp"

namespace Jkr::Renderer {

class CustomImagePainter {
    using Image = PainterParameter<Jkr::PainterParameterContext::StorageImage>;

public:
    CustomImagePainter(
        sv inName,
        sv inComputeShaderFunction,
        sv inPushConstantSignature,
        ui inX,
        ui inY,
        ui inZ);

    explicit CustomImagePainter(CustomImagePainter& inPainter,
        sv inName,
        sv inComputeShaderFunction,
        sv inPushConstantSignature);
    void Load(const Instance& inInstance, Window& inWindow);
    void Store(const Instance& inInstance, Window& inWindow);
    void RegisterImageToBeDrawnTo(const Instance& inInstance, Window& inWindow, ui inWidth, ui inHeight);
    void RegisterImageToBeDrawnTo(const Instance& inInstance, Window& inWindow);
    template <class T>
    void Draw(Window& inWindow, T inPushConstant, ui inX, ui inY, ui inZ)
    {
        mPainter->Dispatch<T>(inPushConstant, inX, inY, inZ);
    }
    template <class T>
    void Draw(Window& inWindow, T inPushConstant)
    {
        Draw<T>(inWindow, inPushConstant, mThreads.x, mThreads.y, mThreads.z);
    }
    GETTER& GetImage() { return mPainterParam->GetStorageImage(); }
    GETTER GetImagePtr() { return mPainterParam->GetStorageImagePtr(); }
    GETTER& GetImagePainterParameter() { return mPainterParam; }

private:
    s mCustomPainterFileName = "customPainter.bin";
    std::ostringstream mComputeStream;
    std::ostringstream mVertexStream;
    std::ostringstream mFragmentStream;
    glm::vec3 mThreads;
    Up<PainterCache> mCustomPainterCache;
    Up<Painter> mPainter;
    sp<Image> mPainterParam;
};

} // namespace Jkr::Renderer
