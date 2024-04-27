#include "World3D.hpp"
#include "EventManager.hpp"
#include "Global/Standards.hpp"
#include "Misc/ThreeD/Uniform3D.hpp"
#include "SDL2/SDL_scancode.h"
using namespace Jkr::Misc::_3D;
using namespace Jkr;

// TODO Bad name, change this
void World3D::BuildBasic() {
    Camera3D Cam;
    Cam.SetAttributes(glm::vec3(0, 0, 0), glm::vec3(10, 10, 10));
    Cam.SetPerspective(0.45, 16.0f / 9.0f, 0.1, 100);
    AddCamera(Cam);
}

void World3D::BuildDemo() {
    Jkr::Generator Gen(Shapes::Cube3D, glm::vec3(1, 0.1, 1));
    int Id                = mShape.AddEXT(Gen, glm::vec3(0, 0, 0));
    mObjectToSimpleMap[0] = mObjects.size() - 1;
    AddObject(Id, -1, -1, 0);
}

Up<World3D> World3D::CreateWorld3D(Shape3D& inShape) {
    auto O = mu<World3D>(inShape);
    return O;
}

int World3D::AddGLTFModel(std::string_view inFileName) {
    mGLTFModels.emplace_back(mu<Renderer::_3D::glTF_Model>(inFileName));
    return mGLTFModels.size() - 1;
}

int World3D::AddObject(int inId,
                       int inAssociatedGLTFModel,
                       int inAssociatedUniform,
                       int inAssociatedSimple3D) {
    mObjects.push_back(Object3D{.mId                 = inId,
                                .mAssociatedModel    = inAssociatedGLTFModel,
                                .mAssociatedUniform  = inAssociatedUniform,
                                .mAssociatedSimple3D = inAssociatedSimple3D});
    mObjectToSimpleMap[mObjects.size() - 1]  = inAssociatedSimple3D;
    mObjectToUniformMap[mObjects.size() - 1] = inAssociatedUniform;
    return mObjects.size() - 1;
}

int World3D::AddSimple3D(Jkr::Instance& inInstance, Window& inWindow) {
    mSimple3Ds.emplace_back(mu<Simple3D>(inInstance, inWindow));
    return mSimple3Ds.size() - 1;
}
int World3D::AddUniform3D(Jkr::Instance& inInstance) {
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

void World3D::AddSkyboxToUniform3D(Instance& inInstance, sv inFolderPath, int inId) {
    mSkyboxImage             = mu<SkyboxImageType>(inInstance);
    std::vector<s> FileNames = {"px", "nx", "py", "ny", "pz", "nz"}; // TODO Make this better
    for (auto& st : FileNames) {
        st = s(inFolderPath) + st + ".png";
    }
    mSkyboxImage->Setup(FileNames);
    mUniforms[inId]->AddSkyboxImage(*mSkyboxImage);
}

void World3D::AddShadowMapToUniform3D(WindowMulT& inWindow, int inId) {
    auto& DesSet   = mUniforms[inId]->GetVulkanDescriptorSet();
    auto& ImgParam = inWindow.GetShadowPass().GetDepthImagePainterParameter();
    ImgParam.RegisterDepth(0, kstd::BindingIndex::Uniform::DiffuseImage, 0, DesSet);
}

void World3D::DrawBackgrounds(Window& inWindow, Renderer::CmdParam inParam) {}

struct PushConstantDefault {
    glm::mat4 m1;
    glm::mat4 m2;
};

// TODO: Remove this
// TODO: Take an Span of object Ids and render those
void World3D::DrawObjects(Window& inWindow, Renderer::CmdParam inParam) {
    PushConstantDefault Push;
    Push.m1         = GetCurrentCamera()->GetMatrix(); // TODO Change This
    Push.m2         = GetCurrentCamera()->GetMatrix(); // TODO Change This
    int SimpleIndex = 0;
    mShape.Bind(inWindow, inParam);
    for (auto& simple : mSimple3Ds) {
        simple->Bind(inWindow, inParam);
        for (auto& uniform : mUniforms) {
            uniform->Bind(inWindow, *simple, 0, inParam);
            for (int i = 0; i < mObjects.size(); ++i) {
                int simple  = mObjectToSimpleMap[i];
                int uniform = mObjectToUniformMap[i];
                if (not(uniform == invalid) and not(simple == SimpleIndex)) {
                    mSimple3Ds[simple]->Draw<PushConstantDefault>(
                         inWindow,
                         mShape,
                         Push,
                         0,
                         mShape.GetIndexCount(mObjects[i].mId),
                         1,
                         inParam);
                }
            }
        }
        if (mUniforms.empty()) {
            for (int i = 0; i < mObjects.size(); ++i) {
                int simple = mObjectToSimpleMap[i];
                mSimple3Ds[simple]->Draw<PushConstantDefault>(
                     inWindow, mShape, Push, 0, mShape.GetIndexCount(mObjects[i].mId), 1, inParam);
            }
        }
        ++SimpleIndex;
    }
}

void World3D::DrawObjectsUniformed3D(Window& inWindow, Renderer::CmdParam inParam) {
    mShape.Bind(inWindow, inParam);
    int SimpleIndex = 0;
    for (auto& simple : mSimple3Ds) {
        simple->Bind(inWindow, inParam);
        for (int i = 0; i < mObjects.size(); ++i) {
            int simpleIndex  = mObjectToSimpleMap[i];
            int uniformIndex = mObjectToUniformMap[i];
            if (simpleIndex == SimpleIndex) {
                mUniforms[uniformIndex]->Bind(inWindow, *simple, 0, inParam);
                PushConstantDefault Push;
                Push.m1 = mObjects[i].GetLocalMatrix();
                mSimple3Ds[simpleIndex]->Draw<PushConstantDefault>(
                     inWindow,
                     mShape,
                     Push,
                     mShape.GetIndexOffsetAbsolute(mObjects[i].mId),
                     mShape.GetIndexCount(mObjects[i].mId),
                     1,
                     inParam);
            }
        }
        ++SimpleIndex;
    }
}

void World3D::DrawObjectsExplicit(Window& inWindow,
                                  v<Object3D>& inExplicitObjects,
                                  Renderer::CmdParam inParam) {
    mUniforms.front()->Bind(inWindow, *mSimple3Ds.front(), 1, inParam);
    mShape.Bind(inWindow, inParam);
    int SimpleIndex = 0;
    for (auto& simple : mSimple3Ds) {
        simple->Bind(inWindow, inParam);
        for (int i = 0; i < inExplicitObjects.size(); ++i) {
            int index        = i;
            int simpleIndex  = inExplicitObjects[i].mAssociatedSimple3D;
            int uniformIndex = inExplicitObjects[i].mAssociatedUniform;
            if (simpleIndex == SimpleIndex) {
                mUniforms[uniformIndex]->Bind(inWindow, *simple, 0, inParam);
                PushConstantDefault Push;
                Push.m1 = mObjects[index].GetLocalMatrix();
                mSimple3Ds[simpleIndex]->Draw<PushConstantDefault>(
                     inWindow,
                     mShape,
                     Push,
                     mShape.GetIndexOffsetAbsolute(mObjects[i].mId),
                     mShape.GetIndexCount(mObjects[i].mId),
                     1,
                     inParam);
            }
        }
        ++SimpleIndex;
    }
}

void World3D::Event(Jkr::EventManager& inEvent) {
    Camera3D& Currentcamera = mCameras[mCurrentCamera];
    bool ShouldUpdate       = false;
}

void World3D::Update(Jkr::EventManager& inEvent) {
    Camera3D& Currentcamera = mCameras[mCurrentCamera];
    if (inEvent.IsKeyPressedContinous(SDL_SCANCODE_W)) {
        Currentcamera.MoveForward(mCameraMovementSensitivity);
    } else if (inEvent.IsKeyPressedContinous(SDL_SCANCODE_S)) {
        Currentcamera.MoveBackward(mCameraMovementSensitivity);
    } else if (inEvent.IsKeyPressedContinous(SDL_SCANCODE_A)) {
        Currentcamera.MoveLeft(mCameraMovementSensitivity);
    } else if (inEvent.IsKeyPressedContinous(SDL_SCANCODE_D)) {
        Currentcamera.MoveRight(mCameraMovementSensitivity);
    }
    if (inEvent.IsLeftButtonPressed()) {
        auto RelMousePos = inEvent.GetRelativeMousePos();
        glm::vec2 Pos    = glm::vec2(RelMousePos);
        if (glm::abs(Pos.x) > 2.0f or glm::abs(Pos.y) > 2.0f) {
            Currentcamera.Yaw(-static_cast<float>(RelMousePos.x) * mCameraRotateSensitivity);
            Currentcamera.Pitch(static_cast<float>(RelMousePos.y) * mCameraRotateSensitivity);
        }
        Currentcamera.UpdateDirectionByAngles();
    }
    Currentcamera.SetPerspective();
    UpdateWorldInfoToUniform3D(0);
}
void World3D::AddWorldInfoToUniform3D(int inId) {
    mUniforms[inId]->AddUniformBuffer(kstd::BindingIndex::Uniform::WorldInfo,
                                      sizeof(WorldInfoUniform));
    {};
}
void World3D::UpdateWorldInfoToUniform3D(int inId) {
    WorldInfoUniform Uniform;
    Camera3D LightCamera;
    LightCamera.SetAttributes(mLights[0].mDirection + mLights[0].mPosition, mLights[0].mPosition);
    LightCamera.SetPerspectiveTargeted();
    Uniform.mView           = GetCurrentCamera()->GetView();
    Uniform.mProjection     = GetCurrentCamera()->GetProjection();
    Uniform.mCameraPosition = GetCurrentCamera()->GetPosition();
    Uniform.mLights[0]      = mLights[0].mPosition;
    Uniform.mShadowMatrix   = LightCamera.GetView();
    mUniforms[inId]->UpdateUniformBuffer(kstd::BindingIndex::Uniform::WorldInfo, Uniform);
}
