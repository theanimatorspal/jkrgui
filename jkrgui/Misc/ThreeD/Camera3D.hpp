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

namespace Jkr::Misc::_3D {
using Simple3D = Renderer::_3D::Simple3D;
using Shape3D  = Renderer::_3D::Shape;

struct Camera3D {
               GETTER GetMatrix() const { return mProjection * mView; }
               GETTER GetPosition() const { return mEye; }
               GETTER GetView() const { return mView; }
               GETTER GetProjection() const { return mProjection; }
               void SetAttributes(glm::vec3 inTarget_3f, glm::vec3 inEye_3f) {
                              mTarget     = inTarget_3f;
                              mEye        = inEye_3f;
                              mDirection  = glm::normalize(mTarget - mEye);
                              mRight      = glm::normalize(glm::cross(mUp, mDirection));
                              mCameraUp   = glm::cross(mDirection, mRight);

                              glm::mat4 E = glm::lookAt(mEye, mEye + mDirection, mCameraUp);
                              mPitch      = -glm::degrees(std::asin(E[1][2]));
                              mYaw        = 180.0f + glm::degrees(std::atan2(E[0][2], E[2][2]));
                              mRoll       = glm::degrees(std::atan2(-E[1][0], E[1][1]));
                              // TODO when cos(mPitch) = 0
                              // TODO Quaternions
               }
               void SetPerspective(float inFov, float inAspect, float inNearZ, float inFarZ) {
                              mFov    = inFov;
                              mAspect = inAspect;
                              mNearZ  = inNearZ;
                              mFarZ   = inFarZ;
                              SetPerspective(); // TODO Remove This
               }
               void SetPerspective() {
                              mView       = glm::lookAt(mEye, mEye + mDirection, mCameraUp);
                              mProjection = glm::perspective(mFov, mAspect, mNearZ, mFarZ);
               }

               void SetPerspectiveTargeted() {
                              mView       = glm::lookAt(mEye, mTarget, mCameraUp);
                              mProjection = glm::perspective(mFov, mAspect, mNearZ, mFarZ);
               }

               void Yaw(float inDelYaw) { mYaw += inDelYaw; }
               void Pitch(float inDelPitch) { mPitch += inDelPitch; }
               void UpdateDirectionByAngles() {
                              if (mPitch > 89.0f) mPitch = 89.0f;
                              if (mPitch < -89.0f) mPitch = -89.0f;
                              mDirection.x = cos(glm::radians(mYaw)) * cos(glm::radians(mPitch));
                              mDirection.y = sin(glm::radians(mPitch));
                              mDirection.z = sin(glm::radians(mYaw)) * cos(glm::radians(mPitch));
                              mDirection   = glm::normalize(mDirection);
               }
               void MoveForward(float inFactor) { mEye += mDirection * inFactor; }
               void MoveBackward(float inFactor) { mEye -= mDirection * inFactor; }
               void MoveLeft(float inFactor) { mEye += glm::normalize(glm::cross(mCameraUp, mDirection)) * inFactor; }
               void MoveRight(float inFactor) { mEye -= glm::normalize(glm::cross(mCameraUp, mDirection)) * inFactor; }

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

               glm::quat mQuaternion = glm::quat(0, 0, 0, -1);
};
} // namespace Jkr::Misc::_3D