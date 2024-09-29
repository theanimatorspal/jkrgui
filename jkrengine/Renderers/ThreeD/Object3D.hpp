#include "Renderers/ThreeD/glTF_Model.hpp"

namespace Jkr::Renderer::_3D {
using namespace glm;
struct Object3D {
    using BoundingBox = Jkr::Renderer::_3D::glTF_Model::BoundingBox;
    mat4 GetMatrix();
    mat4 GetTransforms();
    mat4 GetLocalMatrix();
    vec3 GetContactPoint(Object3D &inObject3D);
    vec3 GetContactNormal(Object3D &inObject3D);
    void ApplyTransforms();
    void SetParent(Object3D *inParent);
    bool IsCollidingWith(Object3D &inObject);
    float GetCollisionThreashold(Object3D &inObject);
    vec3 GetOverlap(Object3D &inObject);

    int mId                 = -1;
    int mAssociatedModel    = -1;
    int mAssociatedUniform  = -1;
    int mAssociatedSimple3D = -1;
    int mIndexCount         = -1;
    int mFirstIndex         = 0;

    ///@note The following are for other usage,
    /// I am using it to store glTF's node and mesh
    /// indices
    int mP1 = -1;
    int mP2 = -1;
    int mP3 = -1;

    vec3 mTranslation{};
    vec3 mScale{1.0f};
    quat mRotation = quat(1.0f, 0.0f, 0.0f, 0.0f);
    mat4 mMatrix   = glm::identity<mat4>();
    vec4 mColor    = vec4(1.0f);
    BoundingBox mBoundingBox;

    float mMass = 1;
    vec3 mMomentum{0.0f};
    mat3 mIBody = glm::identity<mat3>();
    vec3 mVelocity{0.0f};
    vec3 mAngularVelocity{0.0f};
    vec3 mAngularMomentum{0.0f};
    vec3 mForce{0.0f};
    vec3 mTorque{0.0f};

    mat4 mMatrix2 = glm::identity<mat4>();

    private:
    Object3D *mParent;
};
} // namespace Jkr::Renderer::_3D