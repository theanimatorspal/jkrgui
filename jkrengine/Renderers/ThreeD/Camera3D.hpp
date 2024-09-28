#pragma once
#include "Renderers/Generator.hpp"
#include "Renderers/Renderer_base.hpp"
#include "Renderers/ThreeD/Shape3D.hpp"
#include "Renderers/ThreeD/Simple3D.hpp"
#include "Renderers/ThreeD/glTF_Model.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include "glm/trigonometric.hpp"

namespace Jkr::Renderer::_3D {
using Simple3D = Renderer::_3D::Simple3D;
using Shape3D  = Renderer::_3D::Shape;

struct Camera3D {
    GETTER GetMatrix() const { return mProjection * mView; }
    GETTER GetPosition() const { return mEye; }
    GETTER GetView() const { return mView; }
    GETTER GetProjection() const { return mProjection; }
    GETTER GetNearZ() const { return mNearZ; }
    GETTER GetFarZ() const { return mFarZ; }
    void SetAttributes(glm::vec3 inTarget_3f, glm::vec3 inEye_3f);
    void SetPerspective(float inFov, float inAspect, float inNearZ, float inFarZ);
    void SetPerspective();
    void SetPerspectiveTargeted();

    ///@note using Quaternions
    void SetPerspectiveQ();
    void SetPerspectiveQ(float inFov, float inAspect, float inNearZ, float inFarZ);

    /// Controls
    void Yaw(float inDelYaw);
    void Pitch(float inDelPitch);
    void UpdateDirectionByAngles();
    void MoveForward(float inFactor);
    void MoveBackward(float inFactor);
    void MoveLeft(float inFactor);
    void MoveRight(float inFactor);

    private:
    glm::vec3 mTarget     = glm::vec3(0);
    glm::vec3 mRight      = glm::vec3(0);
    glm::vec3 mEye        = glm::vec3(0);
    glm::vec3 mUp         = glm::vec3(0, 1, 0);
    glm::vec3 mCameraUp   = glm::vec3(0);
    glm::vec3 mDirection  = glm::vec3(0);
    float mFov            = 0.45f;
    float mAspect         = 16.0f / 9.0f;
    float mNearZ          = 0.1f;
    float mFarZ           = 100.0f;
    float mYaw            = -90.0f;
    float mPitch          = 0.0f;
    float mRoll           = 0.0f;
    glm::mat4 mProjection = glm::mat4(1.0f);
    glm::mat4 mView       = glm::mat4(1.0f);

    glm::quat mQuaternion = glm::quat();
};
} // namespace Jkr::Renderer::_3D