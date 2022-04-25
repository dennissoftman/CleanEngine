#ifdef RENDERER_VULKAN
    #define GLFW_INCLUDE_VULKAN
    #ifdef __linux__
        #define GLFW_EXPOSE_NATIVE_X11
        #include <X11/Xlib-xcb.h>
    #elif _WIN32
        #define GLFW_EXPOSE_NATIVE_WIN32
    #endif
    #include "vulkanrenderer.hpp"
#elif RENDERER_OPENGL
    #include "openglrenderer.hpp"
#endif

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <string>
#include <stdexcept>
#include <cstdio>

#include "servicelocator.hpp"

#ifndef NDEBUG
#include "debuglogger.hpp"
#endif

#ifdef RENDERER_OPENGL
#include "glshader.hpp"
#include "glmaterial.hpp"
#elif RENDERER_VULKAN
#include "vkshader.hpp"
#include "vkmaterial.hpp"
#endif

#include <glm/gtx/transform.hpp>

// TEMP
#include <IL/il.h>
#include <IL/ilu.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//

#include "camera3d.hpp"

#include "scriptengine.hpp"
#include "luascriptengine.hpp"

const std::string MODULE_NAME = "Main";

static GLFWwindow *mainWindow = nullptr;
static int scrWidth = 1280, scrHeight = 720;

int initWindow()
{
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

    mainWindow = glfwCreateWindow(scrWidth, scrHeight, appName.c_str(), nullptr, nullptr);
    if(mainWindow == NULL)
    {
        const char *buff;
        glfwGetError(&buff);
        ServiceLocator::getLogger().info(MODULE_NAME, buff);
        throw std::runtime_error("Failed to create GLFW window");
    }
    glfwMakeContextCurrent(mainWindow);
    //

    glfwSwapInterval(0); // disable vsync

    return 0;
}

static double lastDeltaTime = 0;
static glm::mat4 cameraMatrix = glm::mat4(1);

void keyProcessor(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    (void)window;
    (void)scancode;
    (void)action;
    (void)mods;

    // SCALE * ROTATE * TRANSLATE
    const float cameraSpeed = 100;
    bool camAltered = false; // moved/rotated

    if(key == GLFW_KEY_UP)
    {
        cameraMatrix = glm::rotate(cameraMatrix, 5.f, glm::vec3(1, 0, 0));
        camAltered = true;
    }
    else if(key == GLFW_KEY_DOWN)
    {
        cameraMatrix = glm::rotate(cameraMatrix, 5.f, glm::vec3(-1, 0, 0));
        camAltered = true;
    }

    if(key == GLFW_KEY_W)
    {
        cameraMatrix = glm::translate(cameraMatrix, glm::vec3(0, 0, -1) * cameraSpeed * (float)lastDeltaTime);
        camAltered = true;
    }
    else if(key == GLFW_KEY_S)
    {
        cameraMatrix = glm::translate(cameraMatrix, glm::vec3(0, 0, 1) * cameraSpeed * (float)lastDeltaTime);
        camAltered = true;
    }

    if(key == GLFW_KEY_A)
    {
        cameraMatrix = glm::translate(cameraMatrix, glm::vec3(1, 0, 0) * cameraSpeed * (float)lastDeltaTime);
        camAltered = true;
    }
    else if(key == GLFW_KEY_D)
    {
        cameraMatrix = glm::translate(cameraMatrix, glm::vec3(-1, 0, 0) * cameraSpeed * (float)lastDeltaTime);
        camAltered = true;
    }

    if(camAltered)
        ServiceLocator::getRenderer().setProjectionMatrix(cameraMatrix);

    if(key == GLFW_KEY_ESCAPE)
        glfwSetWindowShouldClose(window, true);
}

void mainLoop()
{
    Renderer *rend = nullptr;
#ifdef RENDERER_OPENGL
    rend = new OpenGLRenderer();

    rend->init(VideoMode(scrWidth, scrHeight));
#elif RENDERER_VULKAN
    rend = new VulkanRenderer();

    NativeSurfaceProps props;
#ifdef __linux__
    props.connection = XGetXCBConnection(glfwGetX11Display());
    if(props.connection.value() == NULL)
        throw std::runtime_error("XCB connection is NULL");
    props.window = glfwGetX11Window(mainWindow);
    if(props.window.value() == None)
        throw std::runtime_error("XCB window is NULL");
#elif _WIN32
    props.hwnd = glfwGetWin32Window(mainWindow);
    props.hInstance = GetModuleHandle(NULL);
#endif

#ifndef NDEBUG
    ((VulkanRenderer*)rend)->addInstanceExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    ((VulkanRenderer*)rend)->addValidationLayer("VK_LAYER_KHRONOS_validation");
#endif

    ((VulkanRenderer*)rend)->setSurfaceProps(NativeSurfaceProps(props));

    uint32_t requiredExtCount = 0;
    const char **requiredExts = glfwGetRequiredInstanceExtensions(&requiredExtCount);
    for(uint32_t i=0; i < requiredExtCount; i++)
        ((VulkanRenderer*)rend)->addInstanceExtension(requiredExts[i]);

    ((VulkanRenderer*)rend)->addDeviceExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

    rend->init(VideoMode(scrWidth, scrHeight));
#else
#error No graphics API specified
#endif
    ServiceLocator::setRenderer(rend);

    // -----------------------------------------------------------------------------------------
    glfwSetKeyCallback(mainWindow, keyProcessor);
    std::vector<Model3D*> objectModels;
    std::vector<std::string> objMaterials;

    // init DevIL
    ilInit();

    // LOADING TEXTURES (MATERIALS)
    {
        const std::unordered_map<std::string, std::string> mat2tex = {
            {"uv", "data/textures/uv.png"},
            {"utcampfire", "data/textures/utcampfire.png"},
            {"ztoiltower", "data/textures/ztoiltower.png"},
            {"utslab04", "data/textures/utslab04.png"},
            {"utslab02", "data/textures/utslab02.png"},
            {"sufactionlogopage_glauserinterface", "data/textures/sufactionlogopage_glauserinterface.png"},
            {"pmcontainer1", "data/textures/pmcontainer1.png"},
            {"utbarreltop", "data/textures/utbarreltop.png"},
            {"housecolor2", "data/textures/housecolor2.png"},
            {"suuserinterface512_003", "data/textures/suuserinterface512_003.png"},
            {"utcloth", "data/textures/utcloth.png"},
            {"atcanon", "data/textures/atcanon.png"},
            {"grass01", "data/textures/grass01.png"},
            {"utcolumnb", "data/textures/utcolumnb.png"},
            {"utwoodstake", "data/textures/utwoodstake.png"},
            {"atventwall01", "data/textures/atventwall01.png"},
            {"utcloths", "data/textures/utcloths.png"},
            {"utdoor03", "data/textures/utdoor03.png"},
            {"utslab", "data/textures/utslab.png"},
            {"utwallg", "data/textures/utwallg.png"},
            {"utcolumnp", "data/textures/utcolumnp.png"},
            {"utlilwall", "data/textures/utlilwall.png"},
            {"suuserinterface512_002", "data/textures/suuserinterface512_002.png"},
            {"ztslab01", "data/textures/ztslab01.png"},
            {"utbarrelside", "data/textures/utbarrelside.png"},
            {"utconcroof", "data/textures/utconcroof.png"},
            {"pmgaldrum", "data/textures/pmgaldrum.png"},
            {"subuttons", "data/textures/subuttons.png"},
            {"uiworker", "data/textures/uiworker.png"},
            {"utcloth2", "data/textures/utcloth2.png"},
            {"utredmetal", "data/textures/utredmetal.png"},
            {"ctcrateboxes", "data/textures/ctcrateboxes.png"},
            {"ubdome", "data/textures/ubdome.png"},
            {"pmcontainer3", "data/textures/pmcontainer3.png"},
            {"sand02", "data/textures/sand02.png"},
            {"grass02", "data/textures/grass02.png"},
            {"utgrill", "data/textures/utgrill.png"},
            {"suuserinterface512_001", "data/textures/suuserinterface512_001.png"},
            {"utmetroof", "data/textures/utmetroof.png"},
            {"zbsupplydk", "data/textures/zbsupplydk.png"},
            {"utdrkwall", "data/textures/utdrkwall.png"},
            {"utwall2", "data/textures/utwall2.png"},
            {"utcrate", "data/textures/utcrate.png"},
            {"pmcontainer2", "data/textures/pmcontainer2.png"},
            {"sucommandbar", "data/textures/sucommandbar.png"},
            {"sucontrolbar512_001", "data/textures/sucontrolbar512_001.png"},
            {"sand01", "data/textures/sand01.png"},
            {"utslab03", "data/textures/utslab03.png"},
            {"ztoilpump", "data/textures/ztoilpump.png"},
            {"utwall", "data/textures/utwall.png"},
            {"utdoor", "data/textures/utdoor.png"},
            {"utcolumn", "data/textures/utcolumn.png"}
        };

        for(auto &kv : mat2tex)
        {
            Material *mat = nullptr;
#ifdef RENDERER_OPENGL
            mat = new GLMaterial();
#elif RENDERER_VULKAN
            mat = new VkMaterial();
#endif
            try
            {
                mat->setImage(kv.first, kv.second);
                mat->init();
                ServiceLocator::getMatManager().addMaterial(kv.first, mat);
            }
            catch (const std::runtime_error &err)
            {
                ServiceLocator::getLogger().error(MODULE_NAME, "Failed to import material: " + kv.first);
                delete mat;
                break;
            }
        }
        ServiceLocator::getLogger().info(MODULE_NAME, "Material import finished");
    }
    //

    std::vector<int> materialIndexes;
    {
        Assimp::Importer mdlLoader;

        const aiScene *cubeModel = mdlLoader.ReadFile("data/models/oil_derrick.obj",
                                                      aiProcess_Triangulate | aiProcess_ValidateDataStructure);

        for(size_t i=0; i < cubeModel->mNumMaterials; i++)
        {
            const aiMaterial *mat = cubeModel->mMaterials[i];

            objMaterials.push_back(mat->GetName().C_Str());
        }

        for(size_t i=0; i < cubeModel->mNumMeshes; i++)
        {
            Model3D *mdl = new Model3D();
            const aiMesh *mesh = cubeModel->mMeshes[i];
            materialIndexes.push_back(mesh->mMaterialIndex);
            for(size_t j=0; j < mesh->mNumFaces; j++)
            {
                aiFace face = mesh->mFaces[j];
                assert(face.mNumIndices == 3 && "Mesh is not triangulated"); // Triangulated mesh

                aiVector3D   v0 = mesh->mVertices[face.mIndices[0]],
                             v1 = mesh->mVertices[face.mIndices[1]],
                             v2 = mesh->mVertices[face.mIndices[2]];


                aiVector3D   t0, t1, t2;

                if(mesh->mTextureCoords[0])
                {
                    t0 = mesh->mTextureCoords[0][face.mIndices[0]];
                    t1 = mesh->mTextureCoords[0][face.mIndices[1]];
                    t2 = mesh->mTextureCoords[0][face.mIndices[2]];
                }

                mdl->tris.push_back(Triangle3D{
                                            glm::vec3(v0.x, v0.y, v0.z), glm::vec2(t0.x, t0.y),
                                            glm::vec3(v1.x, v1.y, v1.z), glm::vec2(t1.x, t1.y),
                                            glm::vec3(v2.x, v2.y, v2.z), glm::vec2(t2.x, t2.y)
                                        });
            }
            objectModels.push_back(mdl);
        }
    }

    std::vector<RenderObject> objects;
    {
        int i=0;
        for(const Model3D *objMdl : objectModels)
        {
            Material *mat = ServiceLocator::getMatManager().get(objMaterials[materialIndexes[i]]);
            if(mat == nullptr)
            {
#ifdef RENDERER_OPENGL
                mat = new GLMaterial();
#elif RENDERER_VULKAN
                mat = new VkMaterial();
#endif
            }

            objects.push_back(RenderObject{.model = objMdl, .mat = mat, .modelMatrix = glm::mat4(1)});
            i++;
        }
    }

    // =========================================================================================

    // Camera3D mainCamera(90.f, (float)scrWidth/(float)scrHeight, 0.1f, 1000.f);
    {
        glm::mat4 Projection = glm::perspective(glm::radians(90.0f), (float)scrWidth / (float)scrHeight, 0.1f, 1000.0f);

        glm::mat4 View       = glm::lookAt(
            glm::vec3(0,50.f,50.f),
            glm::vec3(0,0,0),
            glm::vec3(0,1,0)
        );

        rend->setViewMatrix(View);
        rend->setProjectionMatrix(Projection);
    }

    // Scripts
    ScriptEngine *scEngine = new LuaScriptEngine();
    scEngine->init();
    //

    double lastTime;
    while(!glfwWindowShouldClose(mainWindow))
    {
        lastDeltaTime = glfwGetTime() - lastTime;
        lastTime = glfwGetTime();
        glfwPollEvents();

        //
        for(RenderObject &obj : objects)
        {
            obj.modelMatrix = glm::translate(glm::mat4(1), glm::vec3(0, 0, -sinf(lastTime)));
            obj.modelMatrix = glm::rotate(obj.modelMatrix, glm::radians(30.f*(float)lastTime), glm::vec3(0, 1, 0));

            rend->queueRenderObject(&obj);
        }
        //

        rend->draw();
#ifdef RENDERER_OPENGL
        glfwSwapBuffers(mainWindow);
#endif
    }

    delete scEngine;
}

void cleanup()
{
    glfwTerminate();
    ServiceLocator::clear();
}

int main()
{
    ServiceLocator::init();
    {
//        FILE *debugFP = fopen("debug.log", "a");
        DebugLogger *logger = new DebugLogger();
//        logger->setInfoFP(debugFP);
//        logger->setWarningFP(debugFP);
//        logger->setErrorFP(debugFP);
        ServiceLocator::setLogger(logger);
    }
    ServiceLocator::getLogger().info(MODULE_NAME, "Started logging");

    // init GLFW
    if(glfwInit() != GLFW_TRUE)
    {
        const char *buff;
        glfwGetError(&buff);
        ServiceLocator::getLogger().info(MODULE_NAME, buff);
        throw std::runtime_error("Failed to init GLFW");
    }
    // -----------------------------------------------------------

    initWindow();
    mainLoop();
    cleanup();

    return 0;
}
