#include "enginecoreglfw.hpp"
#include "servicelocator.hpp"

// --------------------------------------- TEMP ------------------------------------------------
#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/btBulletCollisionCommon.h>

static btDynamicsWorld *physicsWorld = nullptr;
struct PhysicsPair
{
    Entity *obj;
    btRigidBody *body;
};
static std::vector<PhysicsPair> pObjectPairs;
// =============================================================================================

// ---------------------------------------- TEMP -----------------------------------------------
#include <imgui.h>
#include "imgui_impl_glfw.h"

#ifdef RENDERER_OPENGL
#include "imgui_impl_opengl3.h"
#else
#error Unsupported graphics API
#endif
// =============================================================================================

EngineCoreGLFW *EngineCoreGLFW::corePtr = nullptr;
static const char *MODULE_NAME = "EngineCoreGLFW";

EngineCoreGLFW::EngineCoreGLFW()
    : m_mainWindow(nullptr),
      m_elapsedTime(0), m_deltaTime(0),
      m_windowSize(glm::ivec2(1280, 720))
{
    assert(corePtr == nullptr && "Instance is already running");
    corePtr = this;
}

EngineCoreGLFW::~EngineCoreGLFW()
{
    terminate();
}

void tempKeyProcessor(GLFWwindow *win, int key, int scancode, int action, int mods)
{
    (void)action;
    (void)scancode;
    (void)mods;

    if(key == GLFW_KEY_E && action == GLFW_PRESS)
    {
        StaticMesh *mesh = new StaticMesh();
        mesh->setModel(ServiceLocator::getModelManager().getModel("cube"));
        mesh->setPos(glm::vec3((rand() % 50 - 25) / 10.f, 0, (rand() % 50 - 25) / 10.f));
        ServiceLocator::getSceneManager().getScene("default")->addObject(mesh);
        ServiceLocator::getLogger().info(MODULE_NAME, "Created cube");
    }

    if(key == GLFW_KEY_ESCAPE)
        glfwSetWindowShouldClose(win, true);
}

void EngineCoreGLFW::init()
{
    if(glfwInit() != GLFW_TRUE)
    {
        const char *buff;
        glfwGetError(&buff);
        ServiceLocator::getLogger().info(MODULE_NAME, buff);
        throw std::runtime_error("Failed to init GLFW");
    }

    std::string appName;
#ifdef RENDERER_OPENGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DEPTH_BITS, 32); // or fallback to 24
    glfwWindowHint(GLFW_SAMPLES, 2);
    appName = "CleanEngineGL";

#elif RENDERER_VULKAN
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    appName = "CleanEngineVk";
#else
#error No graphics API specified
#endif
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_mainWindow = glfwCreateWindow(m_windowSize.x, m_windowSize.y,
                                    appName.c_str(), nullptr, nullptr);
    if(m_mainWindow == NULL)
    {
        const char *buff;
        glfwGetError(&buff);
        ServiceLocator::getLogger().info(MODULE_NAME, buff);
        throw std::runtime_error("Failed to create GLFW window");
    }
    glfwMakeContextCurrent(m_mainWindow);
    //

    glfwSetKeyCallback(m_mainWindow, tempKeyProcessor);

    glfwSwapInterval(0); // disable vsync

#ifdef RENDERER_OPENGL
    m_mainRenderer = new OpenGLRenderer();

    // ---------------- Init GUI --------------------
    ImGui::CreateContext();

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(m_mainWindow, true);
    ImGui_ImplOpenGL3_Init("#version 450");
    // ==============================================

    m_mainRenderer->init(VideoMode(m_windowSize.x, m_windowSize.y));
#elif RENDERER_VULKAN
#error Renderer is WIP
#endif
    ServiceLocator::setRenderer(m_mainRenderer);

    // TEMP
    {
        ServiceLocator::getModelManager().loadModel("data/models/cube.obj", "cube");
        ServiceLocator::getModelManager().loadModel("data/models/sphere.obj", "sphere");
        ServiceLocator::getModelManager().loadModel("data/models/gear.glb", "gear");

        Scene3D testScene;
        ServiceLocator::getSceneManager().addScene(std::move(testScene), "default");
    }
}

void EngineCoreGLFW::terminate()
{
    ServiceLocator::clear();
    ImGui::DestroyContext();
    glfwTerminate();
}

void EngineCoreGLFW::mainLoop()
{
    Scene3D *currentScene = ServiceLocator::getSceneManager().getScene("default");

    btCollisionConfiguration* pConfig = nullptr;
    btCollisionDispatcher *pDispatch = nullptr;
    btBroadphaseInterface *pCache = nullptr;
    btConstraintSolver *pSolver = nullptr;

    {
        { // create world
            pConfig = new btDefaultCollisionConfiguration();
            pDispatch = new btCollisionDispatcher(pConfig);
            pCache = new btDbvtBroadphase();
            pSolver = new btSequentialImpulseConstraintSolver();
            physicsWorld = new btDiscreteDynamicsWorld(pDispatch, pCache, pSolver, pConfig);

            if(!physicsWorld)
                throw std::runtime_error("Failed to init physics world");

            physicsWorld->setGravity(btVector3(0, -9.8f, 0));
        }

        const Model3D *cubeMdl = ServiceLocator::getModelManager().getModel("cube");
        // floor
        {
            StaticMesh *ent = new StaticMesh();
            ent->setModel(cubeMdl);
            ent->setScale(glm::vec3(100, 2, 100));
            currentScene->addObject(ent, "floor");

            // shape
            btCollisionShape *rbCollisionShape = new btBoxShape(btVector3(50.f, 1.f, 50.f));

            // transform
            btVector3 rbInertia;
            btTransform t;
            t.setIdentity();
            t.setOrigin(btVector3(0, -1, 0));

            // construct body
            btMotionState *rbMotionState = new btDefaultMotionState(t);
            btRigidBody::btRigidBodyConstructionInfo rbInfo(0.f, rbMotionState, rbCollisionShape, rbInertia);
            btRigidBody *body = new btRigidBody(rbInfo);
            physicsWorld->addRigidBody(body);
            pObjectPairs.push_back(PhysicsPair{.obj = ent, .body = body});
        }

        // -X wall
        {
            StaticMesh *ent = new StaticMesh();
            ent->setModel(cubeMdl);
            ent->setScale(glm::vec3(2, 20, 100));
            currentScene->addObject(ent, "wall_left");

            // shape
            btCollisionShape *rbCollisionShape = new btBoxShape(btVector3(1.f, 10.f, 50.f));

            // transform
            btVector3 rbInertia;
            btTransform t;
            t.setIdentity();
            t.setOrigin(btVector3(-50.f, -1, 0));

            // construct body
            btMotionState *rbMotionState = new btDefaultMotionState(t);
            btRigidBody::btRigidBodyConstructionInfo rbInfo(0.f, rbMotionState, rbCollisionShape, rbInertia);
            btRigidBody *body = new btRigidBody(rbInfo);
            physicsWorld->addRigidBody(body);
            pObjectPairs.push_back(PhysicsPair{.obj = ent, .body = body});
        }

        // +X wall
        {
            StaticMesh *ent = new StaticMesh();
            ent->setModel(cubeMdl);
            ent->setScale(glm::vec3(2, 20, 100));
            currentScene->addObject(ent, "wall_right");

            // shape
            btCollisionShape *rbCollisionShape = new btBoxShape(btVector3(1.f, 10.f, 50.f));

            // transform
            btVector3 rbInertia;
            btTransform t;
            t.setIdentity();
            t.setOrigin(btVector3(50.f, -1, 0));

            // construct body
            btMotionState *rbMotionState = new btDefaultMotionState(t);
            btRigidBody::btRigidBodyConstructionInfo rbInfo(0.f, rbMotionState, rbCollisionShape, rbInertia);
            btRigidBody *body = new btRigidBody(rbInfo);
            physicsWorld->addRigidBody(body);
            pObjectPairs.push_back(PhysicsPair{.obj = ent, .body = body});
        }

        // +Z wall (near)
        {
            StaticMesh *ent = new StaticMesh();
            ent->setModel(cubeMdl);
            ent->setScale(glm::vec3(100, 20, 2));
            currentScene->addObject(ent, "wall_near");

            // shape
            btCollisionShape *rbCollisionShape = new btBoxShape(btVector3(50.f, 10.f, 1.f));

            // transform
            btVector3 rbInertia;
            btTransform t;
            t.setIdentity();
            t.setOrigin(btVector3(0, -1, 50.f));

            // construct body
            btMotionState *rbMotionState = new btDefaultMotionState(t);
            btRigidBody::btRigidBodyConstructionInfo rbInfo(0.f, rbMotionState, rbCollisionShape, rbInertia);
            btRigidBody *body = new btRigidBody(rbInfo);
            physicsWorld->addRigidBody(body);
            pObjectPairs.push_back(PhysicsPair{.obj = ent, .body = body});
        }

        // -Z wall
        {
            StaticMesh *ent = new StaticMesh();
            ent->setModel(cubeMdl);
            ent->setScale(glm::vec3(100, 20, 2));
            currentScene->addObject(ent, "wall_far");

            // shape
            btCollisionShape *rbCollisionShape = new btBoxShape(btVector3(50.f, 10.f, 1.f));

            // transform
            btVector3 rbInertia;
            btTransform t;
            t.setIdentity();
            t.setOrigin(btVector3(0, -1, -50.f));

            // construct body
            btMotionState *rbMotionState = new btDefaultMotionState(t);
            btRigidBody::btRigidBodyConstructionInfo rbInfo(0.f, rbMotionState, rbCollisionShape, rbInertia);
            btRigidBody *body = new btRigidBody(rbInfo);
            physicsWorld->addRigidBody(body);
            pObjectPairs.push_back(PhysicsPair{.obj = ent, .body = body});
        }
    }
    // =========================================================================================

    glm::mat4 _projMatrix, _viewMatrix;
    _projMatrix = glm::perspective(90.f, (float)m_windowSize.x/(float)m_windowSize.y, 0.1f, 1000.f);
    _viewMatrix = glm::lookAt(
                glm::vec3(0, 50.f, 40.f),
                glm::vec3(0, 0, 0),
                glm::vec3(0, 1, 0));

    m_mainRenderer->setViewMatrix(_viewMatrix);
    m_mainRenderer->setProjectionMatrix(_projMatrix);

    while(!glfwWindowShouldClose(m_mainWindow))
    {
        m_deltaTime = glfwGetTime() - m_elapsedTime;
        m_elapsedTime = glfwGetTime();
        glfwPollEvents();

        // ------------------------------- UPDATE ----------------------------------------------
        // physics
        physicsWorld->stepSimulation(m_deltaTime);
        for(auto &kv : pObjectPairs)
        {
            const btTransform &t = kv.body->getWorldTransform();
            const btVector3 &pos = t.getOrigin();
            const btQuaternion &rot = t.getRotation();

            kv.obj->setPos(glm::vec3(pos.x(), pos.y(), pos.z()));
            kv.obj->setRotation(glm::quat(rot.w(), rot.x(), rot.y(), rot.z()));
        }
        // objects

        double fps = 1.0 / (m_deltaTime > 0 ? m_deltaTime : 1);
        if(fps > 100.0)
        {
            {
                StaticMesh *ent = new StaticMesh();
                { // entity
                    ent->setModel(ServiceLocator::getModelManager().getModel("sphere"));
                    ent->setPos(glm::vec3((rand() % 100 - 50) / 10.f, 10.f, (rand() % 100 - 50)/10.f));
                    ent->setRotation(glm::vec3(glm::degrees((float)(rand() % 360)),
                                               glm::degrees((float)(rand() % 360)),
                                               glm::degrees((float)(rand() % 360))));
                    ServiceLocator::getSceneManager().getScene("default")->addObject(ent);
                }
                { // rigidbody
                    btScalar rbMass = 10.f;
                    // shape, mass and inertia
                    btCollisionShape *rbCollisionShape = new btSphereShape(1.f);
                    btVector3 rbInertia;
                    rbCollisionShape->calculateLocalInertia(rbMass, rbInertia);

                    // transform
                    btTransform t;
                    t.setIdentity();
                    const glm::vec3 &pos = ent->getPosition();
                    t.setOrigin(btVector3(pos.x, pos.y, pos.z));
                    const glm::quat &rot = ent->getRotation();
                    t.setRotation(btQuaternion(rot.x, rot.y, rot.z, rot.w));

                    // construct body
                    btMotionState *rbMotionState = new btDefaultMotionState(t);
                    btRigidBody::btRigidBodyConstructionInfo rbInfo(rbMass, rbMotionState, rbCollisionShape, rbInertia);
                    btRigidBody *body0 = new btRigidBody(rbInfo);

                    // add to world
                    physicsWorld->addRigidBody(body0);
                    pObjectPairs.push_back(PhysicsPair{.obj = ent, .body = body0});
                }
            }
        }

        currentScene->update(m_deltaTime);
        // gui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Text("Cubes: %lu", pObjectPairs.size() - 5);
        // =====================================================================================

        // -------------------------------- DRAW -----------------------------------------------
        currentScene->draw(m_mainRenderer);

        // Draw objects on screen
        m_mainRenderer->draw();

        // GUI
        ImGui::Render();
        // =====================================================================================
#ifdef RENDERER_OPENGL
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(m_mainWindow);
#endif
    }

    // --------------------------------------- TEMP --------------------------------------------
    delete physicsWorld;
    delete pSolver;
    delete pCache;
    delete pDispatch;
    delete pConfig;
    // =========================================================================================
}
