#include "SimpleScene.h"

void SimpleScene::init(int viewportwidth, int viewportheight)
{
    //camera
    camera.setInitialStatus(glm::vec3(520.0f, 210.0f, -40.0f), glm::vec3(0.0f, 1.0f, 0.0f), 175, 8);
    camera.setMovementSpeed(200.0f);
    camera.setPerspectiveProject((float)viewportwidth / (float)viewportheight, 0.1f, 3000.0f);

    //sponza
    Model sponza("PRTLighting/assets/models/sponza_pbr/sponza_pbr.obj");
    //sponza.transform.scale = glm::vec3(0.5, 0.5, 1.0);
    //sponza.transform.updateTransformMatrix();

    models.push_back(sponza);


    //directional light
    dirLightPos = glm::vec3(-50.0f, 1800.0f, -324.0f);
    glm::vec3 direction = glm::normalize(glm::vec3(1, 1, 1));
    glm::vec3 radiance = glm::vec3(5.0f, 5.0f, 5.0f);
    directionalLight.init(direction, radiance);

    //sky box
    std::vector<std::string> skybox_texture = { "PRTLighting/assets/cubemaps/skybox/right.jpg",
                                   "PRTLighting/assets/cubemaps/skybox/left.jpg",
                                   "PRTLighting/assets/cubemaps/skybox/top.jpg",
                                   "PRTLighting/assets/cubemaps/skybox/bottom.jpg",
                                   "PRTLighting/assets/cubemaps/skybox/front.jpg",
                                   "PRTLighting/assets/cubemaps/skybox/back.jpg" };
    skybox.init(skybox_texture);
}

void SimpleScene::draw(Shader& shader)
{
    for (auto& elem : models)
        elem.Draw(shader);
}

void SimpleScene::draw_skybox(Shader& shader)
{
    skybox.bindTexture(shader, 0);
    skybox.Draw(shader);
}

void SimpleScene::uploadShadowInfo(Shader& shader)
{
    shader.use();

    GLfloat near_plane = 1.0f, far_plane = 3000.0f;
    glm::mat4 lightProjection = glm::ortho(-1500.0f, 1500.0f, -1000.0f, 1000.0f, near_plane, far_plane);
    glm::mat4 lightView = glm::lookAt(dirLightPos, glm::vec3(-30.0f, 40.0f, -20.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;

    shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
}

void SimpleScene::uploadCameraInfo(Shader& shader)
{
    shader.use();

    shader.setMat4("view", camera.GetViewMatrix());
    shader.setMat4("projection", camera.GetProjectMatrix());
    shader.setVec3("viewPos", camera.Position);
}

void SimpleScene::uploadLightInfo(Shader& shader)
{
    shader.use();

    //directional light
    directionalLight.Upload(shader);


}