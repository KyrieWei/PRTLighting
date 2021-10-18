#include "PRTLighting.h"

float PRTLighting::deltaTime = 0.0f;
float PRTLighting::lastFrame = 0.0f;
float PRTLighting::lastX = 0.0f;
float PRTLighting::lastY = 0.0f;
bool PRTLighting::firstMouse = true;
template<> PRTLighting::ptr Singleton<PRTLighting>::_instance = nullptr;

PRTLighting::ptr PRTLighting::getSingleton()
{
    if (!_instance)
        _instance = std::make_shared<PRTLighting>();
    return _instance;
}


void PRTLighting::setWindowSize(unsigned int width_, unsigned int height_)
{
	width = width_;
	height = height_;
}

void PRTLighting::setWindowTitle(const std::string& str)
{
	title = str;
}

void PRTLighting::init()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    glfwWindowHint(GLFW_SAMPLES, 4);

    window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
    if (window == NULL)
    {
        std::cout << "failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    simpleScene.init(width, height);

    renderer.init(simpleScene, width, height);
}

void PRTLighting::run()
{

    glEnable(GL_DEPTH_TEST);

    //SHLighting shLighting;

    //shLighting.cal_SH_coeff(skyboxID);

    ////send sh_coeff to fragment shader
    //buddha_shader.use();
    //unsigned int sh_coeffUBI = glGetUniformBlockIndex(buddha_shader.ID, "sh_coeff_buffer");
    //glUniformBlockBinding(buddha_shader.ID, sh_coeffUBI, 0);

    //unsigned int sh_coeff_uboBlock;
    //glGenBuffers(1, &sh_coeff_uboBlock);
    //glBindBuffer(GL_UNIFORM_BUFFER, sh_coeff_uboBlock);
    //glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::vec4)* shLighting.coeff_num, NULL, GL_STATIC_READ);
    //glBindBuffer(GL_UNIFORM_BUFFER, 0);

    //glBindBufferRange(GL_UNIFORM_BUFFER, 0, sh_coeff_uboBlock, 0, sizeof(glm::vec4)* shLighting.coeff_num);
    //glBindBuffer(GL_UNIFORM_BUFFER, sh_coeff_uboBlock);
    //glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::vec4)* shLighting.coeff_num, shLighting.sh_coeff);

    //glBindBuffer(GL_UNIFORM_BUFFER, 0);
 
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window, deltaTime);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        renderer.render(simpleScene);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void PRTLighting::processInput(GLFWwindow* window, float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        getSingleton()->simpleScene.camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        getSingleton()->simpleScene.camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        getSingleton()->simpleScene.camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        getSingleton()->simpleScene.camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void PRTLighting::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void PRTLighting::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    getSingleton()->simpleScene.camera.ProcessMouseMovement(xoffset, yoffset);
}


// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void PRTLighting::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    getSingleton()->simpleScene.camera.ProcessMouseScroll(yoffset);
}