#include "Camera3D.hpp"
namespace Jkr::Renderer::_3D {
using Simple3D = Renderer::_3D::Simple3D;
using Shape3D  = Renderer::_3D::Shape;

void Camera3D::SetAttributes(glm::vec3 inTarget_3f, glm::vec3 inEye_3f) {
    mTarget     = inTarget_3f;
    mEye        = inEye_3f;
    mDirection  = glm::normalize(mTarget - mEye);
    mRight      = glm::normalize(glm::cross(mUp, mDirection));
    mCameraUp   = glm::cross(mDirection, mRight);

    glm::mat4 E = glm::lookAt(mEye, mEye + mDirection, mCameraUp);
    mPitch      = -glm::degrees(std::asin(E[1][2]));
    mYaw        = 180.0f + glm::degrees(std::atan2(E[0][2], E[2][2]));
    mRoll       = glm::degrees(std::atan2(-E[1][0], E[1][1]));

    mQuaternion = glm::quatLookAt(mDirection, mCameraUp);
}
void Camera3D::SetPerspective(float inFov, float inAspect, float inNearZ, float inFarZ) {
    mFov    = inFov;
    mAspect = inAspect;
    mNearZ  = inNearZ;
    mFarZ   = inFarZ;
    SetPerspective();
}
void Camera3D::SetPerspective() {
    UpdateDirectionByAngles();
    mView       = glm::lookAt(mEye, mEye + mDirection, mCameraUp);
    mProjection = glm::perspective(mFov, mAspect, mNearZ, mFarZ);

    glm::mat4 E = mView;
    mPitch      = -glm::degrees(std::asin(E[1][2]));
    mYaw        = 180.0f + glm::degrees(std::atan2(E[0][2], E[2][2]));
    mRoll       = glm::degrees(std::atan2(-E[1][0], E[1][1]));
}

void Camera3D::SetPerspectiveTargeted() {
    mView       = glm::lookAt(mEye, mTarget, mCameraUp);
    mProjection = glm::perspective(mFov, mAspect, mNearZ, mFarZ);

    glm::mat4 E = mView;
    mPitch      = -glm::degrees(std::asin(E[1][2]));
    mYaw        = 180.0f + glm::degrees(std::atan2(E[0][2], E[2][2]));
    mRoll       = glm::degrees(std::atan2(-E[1][0], E[1][1]));
}

void Camera3D::SetPerspectiveQ(float inFov, float inAspect, float inNearZ, float inFarZ) {
    mFov    = inFov;
    mAspect = inAspect;
    mNearZ  = inNearZ;
    mFarZ   = inFarZ;
    SetPerspectiveQ();
}

void Camera3D::SetPerspectiveQ() {
    mDirection  = mQuaternion * glm::vec3(0, 0, -1);
    mCameraUp   = mQuaternion * glm::vec3(0, 1, 0);
    mRight      = glm::normalize(glm::cross(mCameraUp, mDirection));
    mView       = glm::lookAt(mEye, mEye + mDirection, mCameraUp);
    mProjection = glm::perspective(mFov, mAspect, mNearZ, mFarZ);
}

void Camera3D::Yaw(float inDelYaw) {
    mYaw += inDelYaw;
    glm::quat rotation = glm::angleAxis(glm::radians(inDelYaw), mCameraUp);
    mQuaternion        = rotation * mQuaternion;
}
void Camera3D::Pitch(float inDelPitch) {
    mPitch += inDelPitch;
    glm::quat rotation = glm::angleAxis(glm::radians(inDelPitch), mRight);
    mQuaternion        = rotation * mQuaternion;
}

void Camera3D::UpdateDirectionByAngles() {
    ///@note not letting the camera to go round up
    if (mPitch > 89.0f) mPitch = 89.0f;
    if (mPitch < -89.0f) mPitch = -89.0f;
    mDirection.z = cos(glm::radians(mYaw)) * cos(glm::radians(mPitch));
    mDirection.y = sin(glm::radians(mPitch));
    mDirection.x = sin(glm::radians(mYaw)) * cos(glm::radians(mPitch));
    mDirection   = glm::normalize(mDirection);
    mQuaternion  = glm::quatLookAt(mDirection, mCameraUp);
}

void Camera3D::MoveForward(float inFactor) { mEye += mDirection * inFactor; }
void Camera3D::MoveBackward(float inFactor) { mEye -= mDirection * inFactor; }
void Camera3D::MoveUp(float inFactor) { mEye += mUp * inFactor; }
void Camera3D::MoveDown(float inFactor) { mEye -= mUp * inFactor; }

void Camera3D::MoveLeft(float inFactor) {
    mEye += glm::normalize(glm::cross(mCameraUp, mDirection)) * inFactor;
}
void Camera3D::MoveRight(float inFactor) {
    mEye -= glm::normalize(glm::cross(mCameraUp, mDirection)) * inFactor;
}
} // namespace Jkr::Renderer::_3D
