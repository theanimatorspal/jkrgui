#include "World3D.hpp"
#include "EventManager.hpp"
#include "Global/Standards.hpp"
#include "Renderers/ThreeD/Uniform3D.hpp"
using namespace Jkr::Renderer::_3D;
using namespace Jkr;
using namespace ksai::kstd;

constexpr int WorldInfoDescriptorSetIndex = 0;

// TODO Bad name, change this
void World3D::BuildBasic() {
    Camera3D Cam;
    Cam.SetAttributes(glm::vec3(0, 0, 0), glm::vec3(10, 10, 10));
    Cam.SetPerspective(0.45, 16.0f / 9.0f, 0.1, 100);
    AddCamera(Cam);
}

Up<World3D> World3D::CreateWorld3D(Shape3D &inShape) {
    auto O = mu<World3D>(inShape);
    return O;
}
static std::mutex AddGltfMutex;
int World3D::AddGLTFModel(std::string_view inFileName) {
    std::scoped_lock<std::mutex> Guard(AddGltfMutex);
    mGLTFModels.emplace_back(mu<Renderer::_3D::glTF_Model>(inFileName));
    return mGLTFModels.size() - 1;
}

int World3D::AddSimple3D(Jkr::Instance &inInstance, Window_base &inWindow) {
    mSimple3Ds.emplace_back(mu<Simple3D>(inInstance, inWindow));
    return mSimple3Ds.size() - 1;
}
int World3D::AddUniform3D(Jkr::Instance &inInstance) {
    mUniforms.emplace_back(mu<Uniform3D>(inInstance));
    return mUniforms.size() - 1;
}

int World3D::AddLight3D(glm::vec4 inPosition, glm::vec4 inDirection) {
    Light3D Light;
    Light.mDirection = inDirection;
    Light.mPosition  = inPosition;
    mLights.push_back(Light);
    return mLights.size() - 1;
}

void World3D::AddSkyboxToUniform3D(Instance &inInstance, sv inFolderPath, int inId, int inSet) {
    auto SkyboxImage         = mu<SkyboxImageType>(inInstance);
    std::vector<s> FileNames = {"px", "nx", "py", "ny", "pz", "nz"}; // TODO Make this better
    for (auto &st : FileNames) {
        st = s(inFolderPath) + st + ".png";
    }
    SkyboxImage->Setup(FileNames);
    mUniforms[inId]->AddSkyboxImage(*SkyboxImage, kstd::BindingIndex::Uniform::CubeMapImage, inSet);
    mSkyboxImages.push_back(mv(SkyboxImage));
}

void World3D::AddShadowMapToUniform3D(Window &inWindow, int inId, int inSet) {
    auto &DesSet   = mUniforms[inId]->GetVulkanDescriptorSet();
    auto &ImgParam = inWindow.GetShadowPass().GetDepthImagePainterParameter();
    ImgParam.RegisterDepth(0, kstd::BindingIndex::Uniform::Images, 0, DesSet, inSet);
}

struct PushConstantDefault {
    glm::mat4 m1;
    glm::mat4 m2;
};

void World3D::DrawObjectsExplicit(Window_base &inWindow,
                                  v<Object3D *> &inExplicitObjects,
                                  Renderer::CmdParam inParam) {
    if (not inExplicitObjects.empty()) {
        mShape.Bind(inWindow, inParam);
        mUniforms.front()->Bind(
             inWindow, *mSimple3Ds.front(), WorldInfoDescriptorSetIndex, inParam);

        int PreviousSimpleIndex = -1;
        for (int i = 0; i < inExplicitObjects.size(); ++i) {
            auto ExplicitObject = *inExplicitObjects[i];
            int simpleIndex     = ExplicitObject.mAssociatedSimple3D;
            if (simpleIndex == -1) continue;
            if (ExplicitObject.mId == -1) continue;
            int uniformIndex = ExplicitObject.mAssociatedUniform;
            int indicesCount = ExplicitObject.mIndexCount == -1
                                    ? mShape.GetIndexCount(ExplicitObject.mId)
                                    : ExplicitObject.mIndexCount;

            if (ExplicitObject.mDrawable) {
                if (not(simpleIndex == PreviousSimpleIndex)) {
                    mSimple3Ds[simpleIndex]->Bind(inWindow, inParam);
                    PreviousSimpleIndex = simpleIndex;
                }
                if (uniformIndex != -1) {
                    mUniforms[uniformIndex]->Bind(
                         inWindow, *mSimple3Ds[PreviousSimpleIndex], 1, inParam);
                }
                PushConstantDefault Push;
                Push.m1 = mWorldMatrix * ExplicitObject.GetLocalMatrix();
                Push.m2 = ExplicitObject.mMatrix2;

                int Offset =
                     mShape.GetIndexOffsetAbsolute(ExplicitObject.mId) + ExplicitObject.mFirstIndex;
                mSimple3Ds[simpleIndex]->Draw<PushConstantDefault>(
                     inWindow, mShape, Push, Offset, indicesCount, 1, inParam);
            }
        }
    }
}

void World3D::Event(Jkr::EventManager &inEvent) {
    Camera3D &Currentcamera = mCameras[mCurrentCamera];
    bool ShouldUpdate       = false;
}

// TODO Remove this
void World3D::Update(Jkr::EventManager &inEvent) { UpdateWorldInfoToUniform3D(0); }

void World3D::AddWorldInfoToUniform3D(int inId) {
    mUniforms[inId]->AddUniformBuffer(kstd::BindingIndex::Uniform::WorldInfo,
                                      sizeof(WorldInfoUniform),
                                      WorldInfoDescriptorSetIndex);
}

void World3D::AddWorldInfoToUniform3DEXT(Uniform3D &inUniform) {
    inUniform.AddUniformBuffer(kstd::BindingIndex::Uniform::WorldInfo,
                               sizeof(WorldInfoUniform),
                               WorldInfoDescriptorSetIndex);
}

WorldInfoUniform World3D::GetWorldInfo() {
    WorldInfoUniform Uniform;
    Camera3D LightCamera;
    auto CurrentCamera = GetCurrentCamera();
    LightCamera.SetAttributes(mLights[0].mDirection + mLights[0].mPosition, mLights[0].mPosition);
    LightCamera.SetPerspectiveTargeted();

    Uniform.mView           = GetCurrentCamera()->GetView();
    Uniform.mProjection     = GetCurrentCamera()->GetProjection();
    Uniform.mCameraPosition = GetCurrentCamera()->GetPosition();

    for (int i = 0; i < LightCount; i++) {
        Uniform.mLights[i]           = glm::vec4(0.0f);
        Uniform.mLightsDirections[i] = glm::vec4(0.0f);
    }

    for (int i = 0; i < mLights.size(); i++) {
        Uniform.mLights[i]           = mLights[i].mPosition;
        Uniform.mLightsDirections[i] = mLights[i].mDirection;
    }

    Uniform.mNearFar      = glm::vec4(CurrentCamera->GetNearZ(),
                                 CurrentCamera->GetFarZ(),
                                 CurrentCamera->GetNearZ(),
                                 CurrentCamera->GetFarZ());
    Uniform.mShadowMatrix = LightCamera.GetView();
    return Uniform;
}

void World3D::UpdateWorldInfoToUniform3D(int inId) {
    mUniforms[inId]->UpdateUniformBuffer(kstd::BindingIndex::Uniform::WorldInfo, GetWorldInfo());
}

void World3D::UpdateWorldInfoToUniform3D(Uniform3D &inUniform) {
    inUniform.UpdateUniformBuffer(kstd::BindingIndex::Uniform::WorldInfo, GetWorldInfo());
}

void World3D::AddWorldPrimitiveToUniform3D(Instance &inInstance, Uniform3D &inUniform3D, int inId) {
    VulkanDescriptorUpdateHandler Handler(inInstance.GetDevice());
    Handler.RW(BufferContext::Storage,
               inUniform3D.GetVulkanDescriptorSet(),
               *mShape.GetPrimitive().GetVertexBufferPtr(),
               0,
               kstd::BindingIndex::Storage::WorldVertexBuffer,
               0,
               1);
    Handler.RW(BufferContext::Storage,
               inUniform3D.GetVulkanDescriptorSet(),
               *mShape.GetPrimitive().GetIndexBufferPtr(),
               0,
               kstd::BindingIndex::Storage::WorldIndexBuffer,
               0,
               1);
}