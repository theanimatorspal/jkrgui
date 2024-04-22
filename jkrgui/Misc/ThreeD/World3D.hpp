#pragma once
#include "EventManager.hpp"
#include "Instance.hpp"
#include "Renderers/Generator.hpp"
#include "Renderers/Renderer_base.hpp"
#include "Renderers/ThreeD/Shape3D.hpp"
#include "Renderers/ThreeD/glTF_Model.hpp"
#include "Uniform3D.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/geometric.hpp"
#include "glm/trigonometric.hpp"
#include <string_view>

namespace Jkr::Misc::_3D {
using Simple3D = Renderer::_3D::Simple3D;
using Shape3D  = Renderer::_3D::Shape;
struct Light3D {
               glm::vec4 mPosition;
};

struct Camera3D {
               GETTER GetMatrix() { return mMatrix; }
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
                              glm::mat4 lookat = glm::lookAt(mEye, mEye + mDirection, mCameraUp);
                              mMatrix          = glm::perspective(mFov, mAspect, mNearZ, mFarZ) * lookat;
               }

               void SetPerspectiveTargeted() {
                              glm::mat4 lookat = glm::lookAt(mEye, mTarget, mCameraUp);
                              mMatrix          = glm::perspective(mFov, mAspect, mNearZ, mFarZ) * lookat;
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
               glm::mat4 mMatrix     = glm::mat4(1.0f);

               glm::quat mQuaternion = glm::quat(0, 0, 0, -1);
};

struct World3D {
               struct Object3D {
                              int mId;
                              int mAssociatedModel;
                              int mAssociatedUniform;
                              int mAssociatedSimple3D;
                              // glm::mat4 mMatrix; TODO
               };
               GETTER GetCamera(int inId) { return &mCameras[inId]; }
               GETTER GetCurrentCamera() { return &mCameras[mCurrentCamera]; }
               GETTER GetGLTFModel(int inId) { return mGLTFModels[inId].get(); }
               GETTER GetUniform(int inId) { return mUniforms[inId].get(); }
               GETTER GetSimple3D(int inId) { return mSimple3Ds[inId].get(); }
               SETTER SetCurrentCamera(int inId) { mCurrentCamera = inId; }

               static Up<World3D> CreateWorld3D(Shape3D& inShape);
               void BuildBasic();
               void BuildDemo();

               void AddCamera(Camera3D& inCamera) { mCameras.push_back(inCamera); }
               int AddGLTFModel(std::string_view inFileName);
               int AddObject(int inId, int inAssociatedGLTFModel, int inAssociatedUniform, int inAssociatedSimple3D);
               int AddSimple3D(Jkr::Instance& inInstance, Window& inWindow);
               void DrawBackgrounds(Window& inWindow, Renderer::CmdParam inParam);
               void DrawObjects(Window& inWindow, Renderer::CmdParam inParam);
               void Event(Jkr::EventManager& inEvent);
               void Update(Jkr::EventManager& inEvent);

               World3D(Shape3D& inShape) : mShape(inShape) {}

       private:
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
};
} // namespace Jkr::Misc::_3D
