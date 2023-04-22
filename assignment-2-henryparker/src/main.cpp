#include <utils.h>
#include <camera.h>
#include "enum.h"
#include <object.h>
#include <shader.h>
#include <bezier.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

void processInput(GLFWwindow *window);

void mouse_callback(GLFWwindow *window, double x, double y);

const int WIDTH = 800;
const int HEIGHT = 600;

// carmra
glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  1.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

bool firstMouse = true;
float yaw   = -90.0f;
float pitch =  0.0f;
float lastX =  800.0f / 2.0;
float lastY =  600.0f / 2.0;
float fov   =  45.0f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

// light
glm::vec3 lightPos(0.1f, 0.1f, 0.5f);

GLFWwindow *window;

BETTER_ENUM(RenderCase, int,
            uniform_single_bezier_surface,
            uniform_multi_bezier_surface,
            single_spline_surface)
//            adaptive_single_bezier_surface,
//            adaptive_multi_bezier_surface,
//            single_spline_surface
/**
 * BETTER_ENUM is from https://aantron.github.io/better-enums/
 * You can add any render case as you like,
 * this is to allow you to demonstrate multi task in single executable file.
 * */

RenderCase choice = RenderCase::_values()[0];

void Print_Vertex(Vertex& vertex) {
    std::cout << "position:" << std::endl;
    std::cout << vertex.position[0] << std::endl;
    std::cout << vertex.position[1] << std::endl;
    std::cout << vertex.position[2] << std::endl;
    std::cout << "normal:" << std::endl;
    std::cout << vertex.normal[0] << std::endl;
    std::cout << vertex.normal[1] << std::endl;
    std::cout << vertex.normal[2] << std::endl;
    
}

int main() {
    for (size_t index = 0; index < RenderCase::_size(); ++index) {
        RenderCase render_case = RenderCase::_values()[index];
        std::cout << index << ". " << +render_case << "\n";
    }
    while (true) {
        std::cout << "choose a rendering case from [0:" << RenderCase::_size() - 1 << "]" << "\n";
        std::string input;
        std::cin >> input;
        if (isNumber(input) &&
            std::stoi(input) >= 0 &&
            std::stoi(input) < RenderCase::_size()) {
            choice = RenderCase::_values()[std::stoi(input)];
            break;
        } else {
            std::cout << "Wrong input.\n";
        }
    }
    /**
     * Choose a rendering case in the terminal.
     * */
    WindowGuard windowGuard(window, WIDTH, HEIGHT, "CS171 hw2");
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glEnable(GL_DEPTH_TEST);

    Shader teaShader("../../src/shader.vs", "../../src/shader.fs");
    Shader easyShader("../../src/easyshader.vs", "../../src/easyshader.fs");

    BezierSurface oursurface(0, 0);
    std::vector<vec3> line1 = {
        vec3(0.0f, 0.0f, 0.0f), 
        vec3(0.1f, 0.0f, 0.2f), 
        vec3(0.2f, 0.0f, 0.0f)
    };
    std::vector<vec3> line2 = {
        vec3(0.0f, 0.1f, 0.2f), 
        vec3(0.1f, 0.1f, -0.1f), 
        vec3(0.2f, 0.1f, 0.2f)
    };
    std::vector<vec3> line3 = {
        vec3(0.0f, 0.2f, 0.0f), 
        vec3(0.1f, 0.2f, 0.2f), 
        vec3(0.2f, 0.2f, 0.0f)
    };
    std::vector<std::vector<vec3>> control_points = {
        line1,
        line2,
        line3
    };
    std::vector<Vertex> actual_points;
    for (float u = 0; u <= 1.01; u += 0.1)
        for (float v = 0; v <= 1.01; v += 0.1)
            actual_points.push_back(oursurface.evaluate(control_points, u, v));

    // std::cout << "size is " << actual_points.size() << std::endl;
    // for (int i = 0; i < actual_points.size(); i++ ) {
    //     std::cout << "position: " << std::endl;
    //     std::cout << actual_points[i].position[0] << std::endl;
    //     std::cout << actual_points[i].position[1] << std::endl;
    //     std::cout << actual_points[i].position[2] << std::endl;
    //     std::cout << "normal: " << std::endl;
    //     std::cout << actual_points[i].normal[0] << std::endl;
    //     std::cout << actual_points[i].normal[1] << std::endl;
    //     std::cout << actual_points[i].normal[2] << std::endl;
    // }
    
    // indices generating
    int row_column = 11;
    std::vector<GLuint> indices;
    for (int i = 0; i < row_column - 1; i++ ) {
        for (int j = 0; j < row_column - 1; j++ ) {
            indices.push_back(row_column*i+j);
            indices.push_back(row_column*i+j+1);
            indices.push_back(row_column*(i+1)+j);

            indices.push_back(row_column*i+j+1);
            indices.push_back(row_column*(i+1)+j);
            indices.push_back(row_column*(i+1)+j+1);
        }
    }
    Object single_object;
    single_object.initialize(actual_points, indices);

    // tea
    std::vector<BezierSurface> tea;
    tea = read("../../assets/tea.bzs");
    std::vector<Object> surface_list;
    for (int i = 0; i < tea.size(); i++ ) {
        Object current_surface;
        current_surface.initialize(tea[i].actual_points, tea[i].indices);
        surface_list.push_back(current_surface);
    }

    // Optional : NURBS surfaces
    std::vector<vec4> NURBS_line1 = {
        vec4(0.0f, 0.0f, 0.0f, 1.0f),
        vec4(0.1f, 0.0f, 0.2f, 0.1f),
        vec4(0.2f, 0.0f, 0.0f, 1.0f)
    };
    std::vector<vec4> NURBS_line2 = {
        vec4(0.0f, 0.1f, 0.2f, 1.0f),
        vec4(0.1f, 0.1f, -0.1f, 1.0f),
        vec4(0.2f, 0.1f, 0.2f, 1.0f)
    };
    std::vector<vec4> NURBS_line3 = {
        vec4(0.0f, 0.2f, 0.0f, 1.0f),
        vec4(0.1f, 0.2f, 0.2f, 1.0f),
        vec4(0.2f, 0.2f, 0.0f, 1.0f)
    };
    std::vector<std::vector<vec4>> NURBS_poinits = {
        NURBS_line1,
        NURBS_line2,
        NURBS_line3
    };
    std::vector<Vertex> NURBS_actual_points;
    for (float u = 0; u <= 1.01; u += 0.1)
        for (float v = 0; v <= 1.01; v += 0.1)
            NURBS_actual_points.push_back(oursurface.NURBS_evaluate(NURBS_poinits, u, v));
    
    // std::cout << "size is " << NURBS_actual_points.size() << std::endl;
    // for (int i = 0; i < NURBS_actual_points.size(); i++ ) {
    //     std::cout << "position: " << std::endl;
    //     std::cout << NURBS_actual_points[i].position[0] << std::endl;
    //     std::cout << NURBS_actual_points[i].position[1] << std::endl;
    //     std::cout << NURBS_actual_points[i].position[2] << std::endl;
    //     std::cout << "normal: " << std::endl;
    //     std::cout << NURBS_actual_points[i].normal[0] << std::endl;
    //     std::cout << NURBS_actual_points[i].normal[1] << std::endl;
    //     std::cout << NURBS_actual_points[i].normal[2] << std::endl;
    // }
    Object NURBS_single_object;
    NURBS_single_object.initialize(NURBS_actual_points, indices);

    // test
    


    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame; 
        
        processInput(window);

        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        switch (choice) {
            case RenderCase::uniform_single_bezier_surface: {
                easyShader.use();
                glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
                easyShader.setMat4("projection", projection);

                // camera/view transformation
                glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
                easyShader.setMat4("view", view);

                // model transformation
                glm::mat4 model = glm::mat4(1.0f);
                easyShader.setMat4("model", model);

                single_object.drawElements(easyShader);
                break;
            }
            case RenderCase::uniform_multi_bezier_surface: {
                teaShader.use();
                teaShader.setVec3("objectColor", vec3(1.0f, 0.5f, 0.3f));
                teaShader.setVec3("lightColor",  vec3(1.0f));
                teaShader.setVec3("viewPos", cameraPos);
                teaShader.setVec3("Directionlight", lightPos);
                // pass projection matrix to shader
                glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
                teaShader.setMat4("projection", projection);

                // camera/view transformation
                glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
                teaShader.setMat4("view", view);

                // model transformation
                glm::mat4 model = glm::mat4(1.0f);
                teaShader.setMat4("model", model);
                for (int i = 0; i < surface_list.size(); i++ ) {
                    surface_list[i].drawElements(teaShader);
                }
                break;
            }
            case RenderCase::single_spline_surface: {
                easyShader.use();
                glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
                easyShader.setMat4("projection", projection);

                // camera/view transformation
                glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
                easyShader.setMat4("view", view);

                // model transformation
                glm::mat4 model = glm::mat4(1.0f);
                easyShader.setMat4("model", model);

                NURBS_single_object.drawElements(easyShader);
                break;
            }
        }

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    const float cameraSpeed = 1.0f * deltaTime; // adjust accordingly
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

