#include "Offscreen.hpp"
using namespace Jkr;

ShadowPass::ShadowPass(const Instance& inInstance, ui inWidth, ui inHeight)
    : mInstance(inInstance) {
    mImage = mu<DepthImageType>(inInstance);
    mImage->SetupDepth(inWidth, inHeight);
    mRenderpass = mu<RenderPassType>(inInstance.GetDevice(), mImage->GetDepthImage());
    mFrameBuffer =
         mu<FrameBufferType>(inInstance.GetDevice(), *mRenderpass, mImage->GetDepthImage());
}