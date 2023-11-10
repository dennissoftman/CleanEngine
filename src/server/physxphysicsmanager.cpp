#include <PxPhysics.h>
#include <PxPhysicsAPI.h>
#include <thread>

#include <format>
#include <spdlog/spdlog.h>

#include "server/physxphysicsmanager.hpp"
#include "common/servicelocator.hpp"

static const char *MODULE_NAME = "PhysXPhysicsManager";

PhysicsManager *PhysicsManager::create()
{
    return new PhysXPhysicsManager();
}

PhysXPhysicsManager::PhysXPhysicsManager()
    : m_dispatcher(nullptr),
      m_foundation(nullptr),
      m_physics(nullptr),
      m_scene(nullptr),
      m_pvdTransport(nullptr),
      m_pvd(nullptr),
      m_pvdClient(nullptr)
{

}

PhysXPhysicsManager::~PhysXPhysicsManager()
{
    terminate();
}

void PhysXPhysicsManager::init()
{
    // init physx
    m_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_defaultAllocatorCallback, m_defaultErrorCallback);
    if(!m_foundation)
    {
        spdlog::error("Failed to create PhysX foundation");
        return;
    }

#ifndef NDEBUG
    m_pvd = physx::PxCreatePvd(*m_foundation);
    if(m_pvd)
    {
        m_pvdTransport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 100);
        if(!m_pvd->connect(*m_pvdTransport, physx::PxPvdInstrumentationFlag::eALL))
        {
            spdlog::warn("Failed to connect PVD");
        }
    }
    else
    {
        spdlog::error("Failed to create PVD");
    }
#endif

    m_toleranceScale.length = 100; // 1m
    m_toleranceScale.speed = 981; // gravity
    m_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_foundation, m_toleranceScale, true, m_pvd);
    if(!m_physics)
    {
        spdlog::error("Failed to create PhysX physics object");
        return;
    }

    if(!PxInitExtensions(*m_physics, m_pvd))
        spdlog::error("Failed to init PhysX extensions");

    // initialize world
    physx::PxSceneDesc sceneDesc(m_physics->getTolerancesScale());
    sceneDesc.gravity = physx::PxVec3(0.f, -9.81f, 0.f);
    m_dispatcher = physx::PxDefaultCpuDispatcherCreate(std::thread::hardware_concurrency());
    sceneDesc.cpuDispatcher = m_dispatcher;
    sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
    m_scene = m_physics->createScene(sceneDesc);
    if(!m_scene)
    {
        spdlog::error("Failed to create PhysX scene");
        return;
    }

#ifndef NDEBUG
    m_pvdClient = m_scene->getScenePvdClient();
    if(m_pvdClient)
    {
        m_pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
        m_pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
        m_pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
    }
    else
    {
        spdlog::error("PhysX PVD client unsupported");
    }
#endif

    spdlog::debug("Physics init completed");
}

void PhysXPhysicsManager::update(double dt)
{
    if(!m_scene)
        return;

    m_scene->simulate(dt);
    m_scene->fetchResults(true);

    // update object position rotation
    for(size_t i=0; i < m_bodies.size(); i++)
    {
        physx::PxRigidBody *body = m_bodies[i]->is<physx::PxRigidBody>();
        if(body)
        {
            if(body->userData)
            {
                Entity *ent = static_cast<Entity*>(body->userData);
                physx::PxTransform transform = body->getGlobalPose();

                ent->setPosition(glm::vec3(transform.p.x, transform.p.y, transform.p.z));
                ent->setRotation(glm::quat(transform.q.w, transform.q.x, transform.q.y, transform.q.z));
            }
        }
    }
}

void PhysXPhysicsManager::clear()
{
    for(size_t i=0; i < m_bodies.size(); i++)
    {
        physx::PxActor *actor = m_bodies[i];
        if(m_scene)
            m_scene->removeActor(*actor);
    }
    m_bodies.clear();
}

void PhysXPhysicsManager::createBody(const PhysicsBodyCreateInfo &cInfo, Entity *parent)
{
    if(!m_physics)
        return;
    if(!m_scene)
        return;

    const auto &shapeInfo = cInfo.getShapeInfo();
    const PhysicsBodyProperties &props = cInfo.getBodyProperties();
    physx::PxMaterial* bodyMat = m_physics->createMaterial(props.getFriction(), props.getFriction(), props.getRestitution());
    physx::PxShape *bodyShape = nullptr;
    float density = 0.f;
    switch(shapeInfo.getShapeType())
    {
    case PhysicsShape::eBox:
    {
        glm::vec3 extents = std::get<glm::vec3>(shapeInfo.getShapeData());
        bodyShape = m_physics->createShape(physx::PxBoxGeometry(extents.x, extents.y, extents.z), *bodyMat);
        density = cInfo.getMass() / (extents.x*2 * extents.y*2 * extents.z*2); // a * b * c
        break;
    }
    case PhysicsShape::eSphere:
    {
        float radius = std::get<float>(shapeInfo.getShapeData());
        bodyShape = m_physics->createShape(physx::PxSphereGeometry(radius), *bodyMat);
        density = cInfo.getMass() / ((4/3) * glm::pi<float>() * glm::pow<float>(radius, 3)); // 4/3 * pi * r^3
        break;
    }
    case PhysicsShape::eTriangleMesh:
    {
        spdlog::error("TriangleMesh shapes unsupported");
        return;
        break;
    }
    default:
        return;
    }

    glm::vec3 pos;
    glm::quat rot;
    if(parent)
    {
        pos = parent->getPosition();
        rot = parent->getRotation();
    }
    physx::PxTransform bodyTransform = physx::PxTransform(pos.x, pos.y, pos.z, physx::PxQuat(rot.x, rot.y, rot.z, rot.w));

    physx::PxActor *body;
    if(cInfo.getMass() > 0)
    {
        physx::PxRigidDynamic *dyn = physx::PxCreateDynamic(*m_physics, bodyTransform, *bodyShape, density);
        dyn->setMass(cInfo.getMass());
        dyn->setWakeCounter(15.f);
        dyn->setSleepThreshold(0.0001f * m_toleranceScale.speed * m_toleranceScale.speed);
        if(glm::length(cInfo.getImpulse()) > 0.f)
        {
            glm::vec3 impulse = cInfo.getImpulse();
            dyn->setLinearVelocity(physx::PxVec3(impulse.x, impulse.y, impulse.z));
        }
        body = dyn;
    }
    else
    {
        body = physx::PxCreateStatic(*m_physics, bodyTransform, *bodyShape);
    }

    if(parent)
        body->userData = parent;

    m_scene->addActor(*body);
    m_bodies.push_back(body);
}

void PhysXPhysicsManager::terminate()
{
    m_bodies.clear();

    if(m_scene)
    {
        m_scene->release();
        m_scene = nullptr;
    }

    if(m_physics)
    {
        m_physics->release();
        m_physics = nullptr;
    }

#ifndef NDEBUG
    if(m_pvd)
    {
        m_pvd->release();
        m_pvd = nullptr;
    }

    if(m_pvdTransport)
    {
        m_pvdTransport->release();
        m_pvdTransport = nullptr;
    }
#endif

    if(m_foundation)
    {
        PxCloseExtensions();
        m_foundation->release();
        m_foundation = nullptr;
    }
    spdlog::debug("Physics cleanup completed");
}

void PhysXPhysicsManager::setRaycastCallback(OnRaycastHitCallback callb)
{

}

bool PhysXPhysicsManager::raycast(const glm::vec3 &pos, const glm::vec3 &dir, float len)
{
    return false;
}

void PhysXPhysicsManager::explode(const glm::vec3 &pos, float radius, float power)
{

}

