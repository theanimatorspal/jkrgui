require "JkrGUIv2.Basic"
require "JkrGUIv2.Widgets"
require "JkrGUIv2.Threed"
require "JkrGUIv2.Multit"
require "JkrGUIv2.ShaderFactory"

Engine = {}
Engine.Load = function(self, inEnableValidation)
    self.i = Jkr.CreateInstance(inEnableValidation)
    self.e = Jkr.CreateEventManager()
    self.mt = Jkr.MultiThreading(self.i)
end

Engine.GravitationalForce = 10

Engine.MakeRigidBody = function(inObject, inType)
    local o = {}
    local object = inObject
    o.object = inObject
    local scale = object.mScale
    local y = scale.y
    local x = scale.x
    local z = scale.z
    object.mIBody = mat3(
        vec3(1 / 12 * (y * y + z * z), 0, 0),
        vec3(0, 1 / 12 * (x * x + z * z), 0),
        vec3(0, 0, 1 / 12 * (x * x + y * y))
    )
    object.mMomentum = vec3(0)
    object.mVelocity = object.mMomentum / object.mMass
    object.mAngularVelocity = vec3(0)
    object.mAngularMomentum = object.mIBody * object.mAngularVelocity
    object.mForce = vec3(0.0, -object.mMass * Engine.GravitationalForce, 0.0)
    object.mTorque = vec3(0)

    local IWorld = mat3(0)
    local IWorld_inv = mat3(0)

    -- local ComputeAuxiliary = function()
    --           local Rmat = object.mRotation:GetMatrix3x3()
    --           IWorld = Rmat * object.mIBody * Jmath.Transpose(Rmat)
    --           IWorld_inv = Rmat * Jmath.Inverse(object.mIBody) * Jmath.Transpose(Rmat)
    --           object.mAngularVelocity = IWorld_inv * object.mAngularMomentum
    -- end

    -- local CalculateTorque = function()

    -- end

    o.ResetForces = function()
        object.mForce = vec3(0, -object.mMass * Engine.GravitationalForce, 0)
        object.mTorque = vec3(0)
    end

    if inType == "STATIC" then
        o.mStaticTranslation = vec3(object.mTranslation)
        o.mStatic = true
        object.mMass = 10
        o.Simulate = function(dt)
            object.mTranslation = object.mTranslation + object.mVelocity * dt
            object.mTranslation = o.mStaticTranslation
        end
    else
        o.Simulate = function(dt)
            object.mVelocity = object.mVelocity + object.mForce / object.mMass * dt
            object.mTranslation = object.mTranslation + object.mVelocity * dt
        end
    end
    return o
end

local GetVelocityAfterCollision = function(inObject1, inObject2, e, inStatic)
    if inStatic then
        return vec3(0)
    end
    local m1 = inObject1.mMass
    local m2 = inObject2.mMass
    local v1 = inObject1.mVelocity
    local v2 = inObject2.mVelocity
    return (m1 * v1 + m2 * v2 - m2 * (v1 - v2) * e) / (m1 + m2)
end

local abs = math.abs
Engine.SimulateRigidBody = function(inObjectsTable, dt, e)
    local ObjectsSize = #inObjectsTable
    for i = 1, ObjectsSize, 1 do
        local O1 = inObjectsTable[i].object
        inObjectsTable[i].Simulate(dt)
        for j = i + 1, ObjectsSize, 1 do
            local O2 = inObjectsTable[j].object
            local CollisionThreasold = O1:GetCollisionThreashold(O2)
            if CollisionThreasold > 0.0 then
                O1.mVelocity = GetVelocityAfterCollision(O1, O2, e,
                    inObjectsTable[i].mStatic)
                O2.mVelocity = GetVelocityAfterCollision(O2, O1, e,
                    inObjectsTable[j].mStatic)
                if CollisionThreasold > 0.0001 then
                    local ContactNormal = O1:GetContactNormal(O2)
                    local absX = math.abs(ContactNormal.x)
                    local absY = math.abs(ContactNormal.y)
                    local absZ = math.abs(ContactNormal.z)

                    if absX > absY and absX > absZ then
                        O1.mTranslation.x = O1.mTranslation.x +
                            ContactNormal.x * 0.1
                    elseif absY > absZ then
                        O1.mTranslation.y = O1.mTranslation.y +
                            ContactNormal.y * 0.1
                    else
                        O1.mTranslation.z = O1.mTranslation.z +
                            ContactNormal.z * 0.1
                    end
                end
            end
        end
    end
end

Engine.SimulateRigidBodySubSteps = function(inObjectsTable, dt, inSubsteps, e)
    local SubSteps = 10
    if inSubsteps then
        SubSteps = inSubsteps
    end
    local newdt = dt / SubSteps
    for i = 1, SubSteps, 1 do
        Engine.SimulateRigidBody(inObjectsTable, newdt, e)
    end
end
