#include "SHLighting.h"

SHLighting::SHLighting()
{
    width = 800;
    height = 600;

    title = "SHLighting";

    camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));
    lastX = width / 2.0f;
    lastY = height / 2.0f;
    firstMouse = true;

    deltaTime = 0.0f;
    lastFrame = 0.0f;
}

void SHLighting::setWindowSize(unsigned int width_, unsigned int height_)
{
	width = width_;
	height = height_;
}

void SHLighting::setWindowTitle(const std::string& str)
{
	title = str;
}

void SHLighting::save_sh_texture()
{
    harmonics m_harmonics;
    texture text("assets/cal_light.jpg");

    m_harmonics.set_sh_order(4);
    m_harmonics.set_sample_num(100);

    m_harmonics.SH_setup_spherical_samples();
    m_harmonics.generate_coefficients(texture::cal_test_light, text); 

    m_harmonics.print_coeff();

    save_vector_txt("assets/cal_light_coeff.txt", m_harmonics.sh_coefficents);

    unsigned char* sh_texture_data = new unsigned char[text.width * text.height * text.channel];

    int index = 0;
    for (int j = 100 - 1; j >= 0; j--)
    {
        for (int i = 0; i < 100; i++)
        {
            double theta = (double)j / 100 * PI;
            double phi = (double)i / 100 * 2 * PI;

            double x = sin(theta) * cos(phi);
            double y = sin(theta) * sin(phi);
            double z = cos(theta);

            std::vector<double> Y;
            m_harmonics.generate_basis(x, y, z, Y);

            double f_r = 0, f_g = 0, f_b = 0;

            for (int l = 0; l < Y.size(); l++)
            {
                f_r += m_harmonics.sh_coefficents[l][0] * Y[l];
                f_g += m_harmonics.sh_coefficents[l][1] * Y[l];
                f_b += m_harmonics.sh_coefficents[l][2] * Y[l];
            }

            sh_texture_data[index++] = static_cast<unsigned char>(f_r * 255);
            sh_texture_data[index++] = static_cast<unsigned char>(f_g * 255);
            sh_texture_data[index++] = static_cast<unsigned char>(f_b * 255);
        }
    }

    text.write_to_jpg("assets/cal_light_sh_4.jpg", 100, 100, 3, sh_texture_data);

    delete[] sh_texture_data;

}

void SHLighting::run()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
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
        static_cast<SHLighting*>(glfwGetWindowUserPointer(w))->framebuffer_size_callback(w, width, height);
    };
    auto mousecallback = [](GLFWwindow* w, double xpos, double ypos)
    {
        static_cast<SHLighting*>(glfwGetWindowUserPointer(w))->mouse_callback(w, xpos, ypos);
    };
    auto scrollcallback = [](GLFWwindow* w, double xoffset, double yoffset)
    {
        static_cast<SHLighting*>(glfwGetWindowUserPointer(w))->scroll_callback(w, xoffset, yoffset);
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


    int lat = 40, lon = 81;
    int ball_vertex_num = lat * lon + 2;
    int ball_tri_num = lon * lat * 2;
    double radius = 2.0;

    generate_ball_mesh("assets/ball_mesh.txt", lat, lon, radius);

    float* ball_mesh_vertex = new float[ball_vertex_num * 8];
    unsigned int* ball_vertex_index = new unsigned int[ball_tri_num * 3];

    load_mesh("assets/ball_mesh.txt", ball_mesh_vertex, ball_vertex_index);

    unsigned int ball_VAO, ball_VBO, ball_EBO;
    glGenVertexArrays(1, &ball_VAO);
    
    glGenBuffers(1, &ball_VBO);
    glGenBuffers(1, &ball_EBO);

    glBindVertexArray(ball_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, ball_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * ball_vertex_num * 8, ball_mesh_vertex, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ball_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * ball_tri_num * 3, ball_vertex_index, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    unsigned int earthmap_texture;
    glGenTextures(1, &earthmap_texture);

    glBindTexture(GL_TEXTURE_2D, earthmap_texture);
    //set wrap and filter mode
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    texture earth_map("assets/earthmap.jpg");

    if (earth_map.texture_data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, earth_map.width, earth_map.height, 0, GL_RGB, GL_UNSIGNED_BYTE, earth_map.texture_data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texutre" << std::endl;
    }

    Shader rabbitShader = Shader("shaders/rabbit_vert.vs", "shaders/rabbit_frag.fs");

    Shader calLightShader = Shader("shaders/cal_light_vert.vs", "shaders/cal_light_frag.fs");

    camera.setInitialStatus(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window, deltaTime);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        calLightShader.use();
        
        glm::mat4 model = glm::mat4(1.0);
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::translate(model, glm::vec3(-2.0f, 0.0f, 0.0f));
        calLightShader.setMat4("model", model);

        glm::mat4 view = camera.GetViewMatrix();
        calLightShader.setMat4("view", view);

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)width / (float)height, 0.1f, 100.0f);
        calLightShader.setMat4("projection", projection);

        glBindTexture(GL_TEXTURE_2D, earthmap_texture);

        glBindVertexArray(ball_VAO);
        glDrawElements(GL_TRIANGLES, ball_tri_num * 3, GL_UNSIGNED_INT, 0);

        model = glm::translate(model, glm::vec3(5.0f, 0.0f, 0.0f));
        calLightShader.setMat4("model", model);
        glDrawElements(GL_TRIANGLES, ball_tri_num * 3, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &ball_VAO);
    glDeleteBuffers(1, &ball_VBO);
    glDeleteBuffers(1, &ball_EBO);

    glfwTerminate();

}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void SHLighting::processInput(GLFWwindow* window, float deltaTime)
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
void SHLighting::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void SHLighting::mouse_callback(GLFWwindow* window, double xpos, double ypos)
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
void SHLighting::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}