#include "../Painter.hpp"
#include "../PainterParameter.hpp"


namespace Jkr::Renderer {

class CustomImagePainter {
    using Image = PainterParameter<Jkr::PainterParameterContext::StorageImage>;

public:
    CustomImagePainter(const Instance& inInstance,
        std::string_view inName,
        std::string_view inComputeShaderFunction,
        std::string_view inPushConstantSignature,
        uint32_t inX,
        uint32_t inY,
        uint32_t inZ);

    explicit CustomImagePainter(CustomImagePainter &inPainter,
                                sv inName,
                                sv inComputeShaderFunction,
                                sv inPushConstantSignature);
    void Load(Window& inWindow);
    void Store(Window &inWindow);

    void RegisterImageToBeDrawnTo(Window& inWindow, uint32_t inWidth, uint32_t inHeight);
    void RegisterImageToBeDrawnTo(Window &inWindow, CustomImagePainter &inExisting);

    template <class T>
    void Draw(Window& inWindow, T inPushConstant, uint32_t inX, uint32_t inY, uint32_t inZ)
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
    GETTER &GetImagePainterParameter() { return mPainterParam; }

private:
    void Make() { mCustomPainterCache = MakeUp<PainterCache>(mInstance); }
    const Instance& mInstance;
    std::string mCustomPainterFileName = "customPainter.bin";
    std::ostringstream mComputeStream;
    std::ostringstream mVertexStream;
    std::ostringstream mFragmentStream;
    glm::vec3 mThreads;
    Up<PainterCache> mCustomPainterCache;
    Up<Painter> mPainter;
    Up<Image> mPainterParam;
};

} // namespace Jkr::Renderer
