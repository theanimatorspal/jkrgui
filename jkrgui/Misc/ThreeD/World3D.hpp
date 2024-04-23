#pragma once
#include "Camera3D.hpp"
#include "EventManager.hpp"
#include "Instance.hpp"
#include "Uniform3D.hpp"

namespace Jkr::Misc::_3D {
using Simple3D = Renderer::_3D::Simple3D;
using Shape3D  = Renderer::_3D::Shape;

struct World3D {
               struct WorldInfoUniform {
                              alignas(16) glm::mat4 mView;
                              alignas(16) glm::mat4 mProjection;
                              glm::vec3 mCameraPosition;
                              glm::vec4 mLights[8];
               };
               struct Object3D {
                              int mId;
                              int mAssociatedModel;
                              int mAssociatedUniform;
                              int mAssociatedSimple3D;
                              glm::mat4 mMatrix = glm::identity<glm::mat4>();
               };
               GETTER GetCamera(int inId) { return &mCameras[inId]; }
               GETTER GetCurrentCamera() { return &mCameras[mCurrentCamera]; }
               GETTER GetGLTFModel(int inId) { return mGLTFModels[inId].get(); }
               GETTER GetUniform3D(int inId) { return mUniforms[inId].get(); }
               GETTER GetSimple3D(int inId) { return mSimple3Ds[inId].get(); }
               SETTER SetCurrentCamera(int inId) { mCurrentCamera = inId; }
               SETTER SetObjectMatrix(int inId, glm::mat4 inMatrix) { mObjects[inId].mMatrix = inMatrix; }

               static Up<World3D> CreateWorld3D(Shape3D& inShape);
               void BuildBasic();
               void BuildDemo();

               void AddCamera(Camera3D& inCamera) { mCameras.push_back(inCamera); }
               int AddGLTFModel(std::string_view inFileName);
               int AddObject(int inId, int inAssociatedGLTFModel, int inAssociatedUniform, int inAssociatedSimple3D);
               int AddSimple3D(Jkr::Instance& inInstance, Window& inWindow);
               int AddUniform3D(Jkr::Instance& inInstance);
               void DrawBackgrounds(Window& inWindow, Renderer::CmdParam inParam);
               void DrawObjects(Window& inWindow, Renderer::CmdParam inParam);
               void DrawObjectsUniformed3D(Window& inWindow, Renderer::CmdParam inParam);
               void Event(Jkr::EventManager& inEvent);
               void Update(Jkr::EventManager& inEvent);
               void AddWorldInfoToUniform3D(int inId);

               World3D(Shape3D& inShape) : mShape(inShape) {}

       private:
               void UpdateWorldInfoToUniform3D(int inId);
               Shape3D& mShape;
               int mCurrentCamera = 0;
               v<Camera3D> mCameras;
               v<Up<Renderer::_3D::glTF_Model>> mGLTFModels;
               v<Up<Uniform3D>> mUniforms;
               v<Up<Simple3D>> mSimple3Ds;
               v<Object3D> mObjects;
               v<Object3D> mBackgroundObjects;
               std::array<glm::vec4, 8> mLights;

               umap<int, int> mObjectToSimpleMap;
               umap<int, int> mObjectToUniformMap;
               const int invalid                = -1;
               float mCameraMovementSensitivity = 0.1f;
               float mCameraRotateSensitivity   = 0.1f;
};
} // namespace Jkr::Misc::_3D
