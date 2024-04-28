#pragma once
#include "Camera3D.hpp"
#include "EventManager.hpp"
#include "Global/Standards.hpp"
#include "Instance.hpp"
#include "PainterParameter.hpp"
#include "PainterParameter_base.hpp"
#include "Renderers/Renderer_base.hpp"
#include "Uniform3D.hpp"
#include "WindowMulT.hpp"

// TODO Refactor, Make a world base (Or however) with Object, Lights and Cameras Only
namespace Jkr::Misc::_3D {
using Simple3D = Renderer::_3D::Simple3D;
using Shape3D  = Renderer::_3D::Shape;

struct World3D {
    struct WorldInfoUniform {
        alignas(16) glm::mat4 mView;
        alignas(16) glm::mat4 mProjection;
        glm::vec3 mCameraPosition;
        alignas(16) glm::vec4 mNearFar;
        alignas(16) glm::vec4 mLights[8];
        alignas(16) glm::mat4 mShadowMatrix;
    };
    struct Light3D {
        glm::vec4 mPosition{0.0f};
        glm::vec4 mDirection{0.0f};
    };
    struct Object3D {
        int mId;
        int mAssociatedModel;
        int mAssociatedUniform;
        int mAssociatedSimple3D;
        glm::vec3 mTranslation{};
        glm::vec3 mScale{1.0f};
        glm::quat mRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        glm::mat4 mMatrix   = glm::identity<glm::mat4>();
        glm::mat4 GetLocalMatrix() {
            return glm::translate(glm::mat4(1.0f), mTranslation) * glm::mat4(mRotation) *
                   glm::scale(glm::mat4(1.0f), mScale) * mMatrix;
        }
    };
    GETTER GetExplicitShadowCastingObjects() { return &mExplicitShadowCastingObjects; }
    GETTER GetExplicitObjects() { return &mObjects; }
    GETTER GetCamera(int inId) { return &mCameras[inId]; }
    GETTER GetCurrentCamera() { return &mCameras[mCurrentCamera]; }
    GETTER GetGLTFModel(int inId) { return mGLTFModels[inId].get(); }
    GETTER GetUniform3D(int inId) { return mUniforms[inId].get(); }
    GETTER GetSimple3D(int inId) { return mSimple3Ds[inId].get(); }
    SETTER SetCurrentCamera(int inId) { mCurrentCamera = inId; }

    static Up<World3D> CreateWorld3D(Shape3D& inShape);
    void BuildBasic();
    void BuildDemo();

    void AddCamera(Camera3D& inCamera) { mCameras.push_back(inCamera); }
    int AddGLTFModel(std::string_view inFileName);
    int AddObject(int inId,
                  int inAssociatedGLTFModel,
                  int inAssociatedUniform,
                  int inAssociatedSimple3D);
    int AddSimple3D(Jkr::Instance& inInstance, Window& inWindow);
    int AddUniform3D(Jkr::Instance& inInstance);
    int AddLight3D(glm::vec4 inPosition, glm::vec4 inDirection);
    void DrawBackgrounds(Window& inWindow, Renderer::CmdParam inParam);
    void DrawObjects(Window& inWindow, Renderer::CmdParam inParam);
    void DrawObjectsUniformed3D(Window& inWindow, Renderer::CmdParam inParam);
    void DrawObjectsExplicit(Window& inWindow,
                             v<Object3D>& inExplicitObjectIds,
                             Renderer::CmdParam inParam);
    void Event(Jkr::EventManager& inEvent);
    void Update(Jkr::EventManager& inEvent);
    void AddWorldInfoToUniform3D(int inId);
    void AddSkyboxToUniform3D(Instance& inInstance, sv inFolderPath, int inId, int inSet);
    void AddShadowMapToUniform3D(WindowMulT& inWindow, int inId, int inSet);

    World3D(Shape3D& inShape) : mShape(inShape) {}

    private:
    using SkyboxImageType = Jkr::PainterParameter<PainterParameterContext::SkyboxImage>;
    void UpdateWorldInfoToUniform3D(int inId);
    Shape3D& mShape;
    int mCurrentCamera = 0;
    v<Camera3D> mCameras;
    v<Up<Renderer::_3D::glTF_Model>> mGLTFModels;
    v<Up<Uniform3D>> mUniforms;
    v<Up<Simple3D>> mSimple3Ds;
    v<Object3D> mObjects;
    v<Object3D> mBackgroundObjects;
    v<Light3D> mLights;

    umap<int, int> mObjectToSimpleMap;
    umap<int, int> mObjectToUniformMap;
    const int invalid                = -1;
    float mCameraMovementSensitivity = 0.1f;
    float mCameraRotateSensitivity   = 0.1f;
    v<Object3D> mExplicitShadowCastingObjects;
    Up<SkyboxImageType> mSkyboxImage;
};
} // namespace Jkr::Misc::_3D
