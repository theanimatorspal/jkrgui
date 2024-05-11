#include "Object3D.hpp"

using namespace Jkr::Misc::_3D;

bool Object3D::IsCollidingWith(Object3D& inObject) {
    const auto AABB1 = inObject.mBoundingBox.GetAABB(inObject.GetLocalMatrix());
    const auto AABB2 = this->mBoundingBox.GetAABB(this->GetLocalMatrix()); // TODO Looks Fishy
    return (AABB1.min.x <= AABB2.max.x and AABB1.max.x >= AABB2.min.x and
            AABB1.min.y <= AABB2.max.y and AABB1.max.y >= AABB2.min.y and
            AABB1.min.z <= AABB2.max.z and AABB1.max.z >= AABB2.min.z);
}

float Object3D::GetCollisionThreashold(Object3D& inObject) {
    const auto AABB1    = inObject.mBoundingBox.GetAABB(inObject.GetLocalMatrix());
    const auto AABB2    = this->mBoundingBox.GetAABB(this->GetLocalMatrix());

    float overlapX      = std::min(AABB1.max.x, AABB2.max.x) - std::max(AABB1.min.x, AABB2.min.x);
    float overlapY      = std::min(AABB1.max.y, AABB2.max.y) - std::max(AABB1.min.y, AABB2.min.y);
    float overlapZ      = std::min(AABB1.max.z, AABB2.max.z) - std::max(AABB1.min.z, AABB2.min.z);

    float overlapVolume = overlapX * overlapY * overlapZ;

    if (overlapX < 0 or overlapY < 0 or overlapZ < 0 or overlapVolume <= 0) {
        return 0.0f;
    } else {
        float volume1 = (AABB1.max.x - AABB1.min.x) * (AABB1.max.y - AABB1.min.y) *
                        (AABB1.max.z - AABB1.min.z);
        float volume2 = (AABB2.max.x - AABB2.min.x) * (AABB2.max.y - AABB2.min.y) *
                        (AABB2.max.z - AABB2.min.z);
        float collisionThreshold = overlapVolume / std::min(volume1, volume2);
        return collisionThreshold;
    }
}

vec3 Object3D::GetContactPoint(Object3D& inObject) {
    return (inObject.mTranslation + this->mTranslation) / 2.0f;
}

vec3 Object3D::GetContactNormal(Object3D& inObject) {
    return normalize(this->mTranslation - inObject.mTranslation);
}

vec3 Object3D::GetOverlap(Object3D& inObject) {

    const auto AABB1 = inObject.mBoundingBox.GetAABB(inObject.GetLocalMatrix());
    const auto AABB2 = this->mBoundingBox.GetAABB(this->GetLocalMatrix());

    float overlapX   = std::min(AABB1.max.x, AABB2.max.x) - std::max(AABB1.min.x, AABB2.min.x);
    float overlapY   = std::min(AABB1.max.y, AABB2.max.y) - std::max(AABB1.min.y, AABB2.min.y);
    float overlapZ   = std::min(AABB1.max.z, AABB2.max.z) - std::max(AABB1.min.z, AABB2.min.z);

    return vec3(overlapX, overlapY, overlapZ);
}

void Object3D::SetParent(Object3D* inParent) { mParent = inParent; }

glm::mat4 Object3D::GetMatrix() { return glm::mat4(mMatrix); }

glm::mat4 Object3D::GetTransforms() {
    return glm::translate(glm::mat4(1.0f), mTranslation) * glm::mat4(mRotation) *
           glm::scale(glm::mat4(1.0f), mScale);
}

glm::mat4 Object3D::GetLocalMatrix() {
    if (not mParent) {
        return glm::translate(glm::mat4(1.0f), mTranslation) * glm::mat4(mRotation) *
               glm::scale(glm::mat4(1.0f), mScale) * mMatrix;
    } else {
        return mParent->GetTransforms() *
               (glm::translate(glm::mat4(1.0f), mTranslation) * glm::mat4(mRotation) *
                glm::scale(glm::mat4(1.0f), mScale) * mMatrix);
    }
}

void Object3D::ApplyTransforms() {
    mMatrix      = GetLocalMatrix();
    mTranslation = {};
    mScale       = vec3{1.0f};
    mRotation    = quat(1.0f, 0.0f, 0.0f, 0.0f);
}