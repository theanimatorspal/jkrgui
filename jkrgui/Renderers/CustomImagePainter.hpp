#include "../Painter.hpp"
#include "../PainterParameter.hpp"

namespace Jkr::Renderer {
using Image = PainterParameter<Jkr::PainterParameterContext::StorageImage>;

class CustomImagePainter;

struct CustomPainterImage {
    CustomPainterImage(const Instance& inInstance, const Window& inWindow, ui inWidth, ui inHeight)
    {
        mPainterParam = mu<Image>(inInstance);
        mPainterParam->Setup(inWidth, inHeight);
        Painter::OptimizeParameter(inInstance, *mPainterParam, inWindow);
    }
    void Register(const Instance& inInstance, CustomImagePainter& inPainterCache);

    std::vector<int> GetImageToVector(const Instance& inInstance, const Window& inWindow)
    {
		ui ImageChannels = 4;
        auto ImageExtent = mPainterParam->GetStorageImage().GetImageExtent();
		auto Size = ImageChannels * ImageExtent.width * ImageExtent.height;
        uint32_t size = ImageChannels * ImageExtent.width * ImageExtent.height;
        VulkanBufferVMA<BufferContext::Storage, MemoryType::HostVisibleAndCoherenet> Buffer(inInstance.GetVMA(), inInstance.GetDevice(), size);
        Buffer.SubmitImmediateCmdCopyFromImage(inInstance.GetGraphicsQueue(), inWindow.GetCommandBuffers(Jkr::Window::None)[inWindow.GetCurrentFrame()], mPainterParam->GetStorageImage());
        void* MemoryRegion;
        Buffer.MapMemoryRegion(&MemoryRegion);
        std::vector<uint8_t> OutImage;
        std::vector<int> OutImage_i;
        OutImage.resize(size);
        OutImage_i.reserve(size);
        std::memcpy(OutImage.data(), MemoryRegion, size);

        for (int i = 0; i < OutImage.size(); i++)
        {
            OutImage_i.push_back(OutImage[i]);
        }
        return OutImage_i;
    }

    GETTER& GetPainterParam() { return *mPainterParam; }
    up<VulkanDescriptorSet> mVulkanDescriptorSet;
    up<Image> mPainterParam;
};


class CustomImagePainter {
     using ComPar = Jkr::Window::ParameterContext;
public:
    CustomImagePainter(
        sv inName,
        sv inComputeShaderFunction,
        sv inPushConstantSignature,
        ui inX,
        ui inY,
        ui inZ);
    GETTER& GetPainter() { return *mPainter; }

    void Load(const Instance& inInstance, Window& inWindow);
    void Store(const Instance& inInstance, Window& inWindow);

    void RegisterImage(const Instance& inInstance, Window& inWindow, CustomPainterImage& inImage)
    {
        mPainter->RegisterPainterParameter(inImage.GetPainterParam(), 0, 0, 0, Jkr::RegisterMode::ComputeOnly);
    }

    void Bind(const Window& inWindow, ComPar inPar = ComPar::None)
    {
        mPainter->BindDispatchParametersPipelineOnly_EXT(inWindow, inPar);
    }

    // TODO To be removed
    void BindImage(const Window& inWindow, ComPar inPar = ComPar::None)
    {
        mPainter->BindDispatchParametersDescriptorsOnly_EXT(inWindow, inPar);
    }

    void BindImageFromImage (const Window &inWindow, CustomPainterImage& inImage, ComPar inPar = ComPar::None)
    {
         auto& Cmd = inWindow.GetCommandBuffers(inPar)[inWindow.GetCurrentFrame()];
         Cmd.GetCommandBufferHandle().bindDescriptorSets(
	   vk::PipelineBindPoint::eCompute,
	   mCustomPainterCache->GetComputePipelineLayout().GetPipelineLayoutHandle(),
	   0,
	   inImage.mVulkanDescriptorSet->GetDescriptorSetHandle(),
	   {});
    }

    template <class T>
    void Draw(Window& inWindow, T inPushConstant, ui inX, ui inY, ui inZ, ComPar inPar = ComPar::None)
    {
        mPainter->Dispatch_EXT<T>(inPushConstant, inX, inY, inZ, inPar);
    }

    template <class T>
    void Draw(Window& inWindow, T inPushConstant, ComPar inPar = ComPar::None)
    {
        mPainter->Dispatch_EXT<T>(inPushConstant, mThreads.x, mThreads.y, mThreads.z, inPar);
    }

    GETTER& GetPainterCache() { return *mCustomPainterCache; }

private:
    s mCustomPainterFileName = "customPainter.bin";
    std::ostringstream mComputeStream;
    std::ostringstream mVertexStream;
    std::ostringstream mFragmentStream;
    glm::vec3 mThreads;
    Up<PainterCache> mCustomPainterCache;
    Up<Painter> mPainter;
};

inline void Jkr::Renderer::CustomPainterImage::Register(const Instance& inInstance, CustomImagePainter& inPainter)
{
     mVulkanDescriptorSet = MakeUp<VulkanDescriptorSet>(inInstance.GetDevice(), inInstance.GetDescriptorPool(), inPainter.GetPainterCache().GetComputePipelineDescriptorSetLayout());
     mPainterParam->Register(0, 0, 0, *mVulkanDescriptorSet);
}
} // namespace Jkr::Renderer
