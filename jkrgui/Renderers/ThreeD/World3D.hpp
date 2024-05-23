#pragma once
#include "Camera3D.hpp"
#include "EventManager.hpp"
#include "Global/Standards.hpp"
#include "Instance.hpp"
#include "Object3D.hpp"
#include "PainterParameter.hpp"
#include "PainterParameter_base.hpp"
#include "Renderers/Renderer_base.hpp"
#include "Uniform3D.hpp"
#include "WindowMulT.hpp"

// TODO Refactor, Make a world base (Or however) with Object, Lights and Cameras Only
namespace Jkr::Renderer::_3D {
using Simple3D = Renderer::_3D::Simple3D;
using Shape3D  = Renderer::_3D::Shape;
using namespace ksai::kstd;

struct World3D {
    using Object3D    = Jkr::Renderer::_3D::Object3D;
    using BoundingBox = Renderer::_3D::glTF_Model::BoundingBox;
    struct Light3D {
        glm::vec4 mPosition{0.0f};
        glm::vec4 mDirection{0.0f};
    };

    /* ============================================================

        GETTERs

    ============================================================== */
    GETTER MakeExplicitObjectsVector() -> v<Object3D> { return {}; }
    GETTER GetCamera3D(int inId) { return &mCameras[inId]; }
    GETTER GetCurrentCamera() { return &mCameras[mCurrentCamera]; }
    GETTER GetGLTFModel(int inId) { return mGLTFModels[inId].get(); }
    GETTER GetUniform3D(int inId) { return mUniforms[inId].get(); }
    GETTER GetSimple3D(int inId) { return mSimple3Ds[inId].get(); }
    SETTER SetCurrentCamera(int inId) { mCurrentCamera = inId; }
    GETTER GetSkyboxImageBase(int inId) -> VulkanImageBase& {
        return mSkyboxImages[inId]->GetUniformImage();
    }
    WorldInfoUniform GetWorldInfo();

    /* ============================================================

        BuildBasic

    ============================================================== */

    static Up<World3D> CreateWorld3D(Shape3D& inShape);
    void BuildBasic();

    /* ============================================================

        Modification Routines

    ============================================================== */

    void AddCamera(Camera3D& inCamera) { mCameras.push_back(inCamera); }
    int AddGLTFModel(std::string_view inFileName);
    int AddSimple3D(Jkr::Instance& inInstance, Window& inWindow);
    int AddUniform3D(Jkr::Instance& inInstance);
    int AddLight3D(glm::vec4 inPosition, glm::vec4 inDirection);
    void DrawObjectsExplicit(Window& inWindow,
                             v<Object3D>& inExplicitObjectIds,
                             Renderer::CmdParam inParam);
    void Event(Jkr::EventManager& inEvent);
    void Update(Jkr::EventManager& inEvent);

    /* ============================================================

        Utility Routines

    ============================================================== */
    void AddWorldInfoToUniform3D(int inId);
    void AddSkyboxToUniform3D(Instance& inInstance, sv inFolderPath, int inId, int inSet);
    void AddShadowMapToUniform3D(WindowMulT& inWindow, int inId, int inSet);
    void AddWorldPrimitiveToUniform3D(Instance& inInstance, Uniform3D& inUniform3D, int inId);

    /* PBR Routines */
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
    v<Light3D> mLights;

    const int invalid                = -1;
    float mCameraMovementSensitivity = 0.1f;
    float mCameraRotateSensitivity   = 0.1f;
    std::vector<Up<SkyboxImageType>> mSkyboxImages;
};
} // namespace Jkr::Renderer::_3D
