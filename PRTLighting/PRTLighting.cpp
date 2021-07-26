#include "PRTLighting.h"

PRTLighting::PRTLighting()
{
	width = 1280;
	height = 720;

	title = "PRTLighting";

	camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));

	lastX = width / 2.0f;
	lastY = height / 2.0f;
	firstMouse = true;

	deltaTime = 0.0f;
	lastFrame = 0.0f;
}

void PRTLighting::setWindowSize(unsigned int width_, unsigned int height_)
{
	width = width_;
	height = height_;

    lastX = width / 2.0f;
    lastY = height / 2.0f;
}

void PRTLighting::setWindowTitle(const std::string& str)
{
	title = str;
}

void PRTLighting::run()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow* window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
    if (window == NULL)
    {
        std::cout << "failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);

    glfwSetWindowUserPointer(window, this);
    auto framebuffersizecallback = [](GLFWwindow* w, int width, int height)
    {
        static_cast<PRTLighting*>(glfwGetWindowUserPointer(w))->framebuffer_size_callback(w, width, height);
    };
    auto mousecallback = [](GLFWwindow* w, double xpos, double ypos)
    {
        static_cast<PRTLighting*>(glfwGetWindowUserPointer(w))->mouse_callback(w, xpos, ypos);
    };
    auto scrollcallback = [](GLFWwindow* w, double xoffset, double yoffset)
    {
        static_cast<PRTLighting*>(glfwGetWindowUserPointer(w))->scroll_callback(w, xoffset, yoffset);
    };

    glfwSetFramebufferSizeCallback(window, framebuffersizecallback);
    glfwSetCursorPosCallback(window, mousecallback);
    glfwSetScrollCallback(window, scrollcallback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    glEnable(GL_DEPTH_TEST);

#pragma region skybox

    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };


    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);

    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    std::vector<std::string> skybox = { "PRTLighting/assets/cubemaps/skybox/right.jpg",
                                       "PRTLighting/assets/cubemaps/skybox/left.jpg",
                                       "PRTLighting/assets/cubemaps/skybox/top.jpg",
                                       "PRTLighting/assets/cubemaps/skybox/bottom.jpg",
                                       "PRTLighting/assets/cubemaps/skybox/front.jpg",
                                       "PRTLighting/assets/cubemaps/skybox/back.jpg" };

    std::vector<std::string> grace = { "PRTLighting/assets/cubemaps/grace/right.bmp",
                                       "PRTLighting/assets/cubemaps/grace/left.bmp",
                                       "PRTLighting/assets/cubemaps/grace/top.bmp",
                                       "PRTLighting/assets/cubemaps/grace/bottom.bmp",
                                       "PRTLighting/assets/cubemaps/grace/front.bmp",
                                       "PRTLighting/assets/cubemaps/grace/back.bmp"};

    texture skybox_cubemap;

    unsigned int skyboxID = skybox_cubemap.load_bmp_cube_map(grace);

    Shader skyboxShader("PRTLighting/shaders/skybox_vert.vs", "PRTLighting/shaders/skybox_frag.fs");

    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

#pragma endregion

#pragma region buddha

    const GLuint buddha_vertex_num = 49990;
    const GLuint buddha_tri_num = 100000;
    const GLuint buddha_stride = 8;
    float* buddha_vertex = new float[buddha_vertex_num * buddha_stride];
    GLuint* buddha_index = new GLuint[buddha_tri_num * 3];

    load_mesh("PRTLighting/assets/models/buddha.obj", buddha_vertex, buddha_index);

    unsigned int buddha_VAO, buddha_VBO, buddha_EBO;

    glGenVertexArrays(1, &buddha_VAO);
    glGenBuffers(1, &buddha_VBO);
    glGenBuffers(1, &buddha_EBO);

    glBindVertexArray(buddha_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, buddha_VBO);
    glBufferData(GL_ARRAY_BUFFER, buddha_vertex_num* buddha_stride * sizeof(float), buddha_vertex, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buddha_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, buddha_tri_num * 3 * sizeof(unsigned int), buddha_index, GL_STATIC_DRAW);

    //normal
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, buddha_stride * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //texcoord
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, buddha_stride * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //vertex
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, buddha_stride * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    Shader buddha_shader("PRTLighting/shaders/buddha_vert.vs", "PRTLighting/shaders/buddha_frag.fs");

#pragma endregion

#pragma region SH_Coeff

    SHLighting shLighting;

    shLighting.cal_SH_coeff(skyboxID);

    //send sh_coeff to fragment shader
    buddha_shader.use();
    unsigned int sh_coeffUBI = glGetUniformBlockIndex(buddha_shader.ID, "sh_coeff_buffer");
    glUniformBlockBinding(buddha_shader.ID, sh_coeffUBI, 0);

    unsigned int sh_coeff_uboBlock;
    glGenBuffers(1, &sh_coeff_uboBlock);
    glBindBuffer(GL_UNIFORM_BUFFER, sh_coeff_uboBlock);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::vec4)* shLighting.coeff_num, NULL, GL_STATIC_READ);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferRange(GL_UNIFORM_BUFFER, 0, sh_coeff_uboBlock, 0, sizeof(glm::vec4)* shLighting.coeff_num);
    glBindBuffer(GL_UNIFORM_BUFFER, sh_coeff_uboBlock);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::vec4)* shLighting.coeff_num, shLighting.sh_coeff);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);

#pragma endregion

    glm::vec3 lightPos = glm::vec3(-2.0, 2.0, -2.0);

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window, deltaTime);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //buddha
        glm::mat4 model = glm::mat4(1.0);

        glm::mat4 view = camera.GetViewMatrix();

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)width / (float)height, 0.1f, 100.0f);

        buddha_shader.use();
        buddha_shader.setMat4("model", model);
        buddha_shader.setMat4("view", view);
        buddha_shader.setMat4("projection", projection);

        buddha_shader.setVec3("lightPos", lightPos);

        glBindVertexArray(buddha_VAO);
        glDrawElements(GL_TRIANGLES, buddha_tri_num * 3, GL_UNSIGNED_INT, 0);

        //skybox
        glDepthFunc(GL_LEQUAL);

        skyboxShader.use();

        view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxID);

        glBindVertexArray(skyboxVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glDepthFunc(GL_LESS);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &buddha_VAO);
    glDeleteBuffers(1, &buddha_VBO);
    glDeleteBuffers(1, &buddha_EBO);

    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);

    delete[] buddha_vertex;
    delete[] buddha_index;

    glfwTerminate();

}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void PRTLighting::processInput(GLFWwindow* window, float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
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

    camera.ProcessMouseMovement(xoffset, yoffset);
}


// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void PRTLighting::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}