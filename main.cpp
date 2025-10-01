#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader.hpp>
#include <shader_m.h>
#include <camera.h>
#include <iostream>
#include <objloader.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 5.0f, 30.0f)); // Adjusted for closer view
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(0.0f, 20.0f, 20.0f);

// Posições para os hangares e as naves
glm::vec3 hangar1Pos(-15.0f, -5.0f, -15.0f);
glm::vec3 hangar2Pos(15.0f, -5.0f, -15.0f); 
bool isMoving = false;
float moveProgress = 0.0f; 
glm::vec3 targetPos1 = hangar2Pos; 
glm::vec3 targetPos2 = hangar1Pos; 


bool spacePressed = false;

// Ajusta as posições consoante a posição da câmara
glm::vec3 forwardOffset = camera.Front * 4.0f; 
glm::vec3 rightOffset = camera.Right * 2.0f;   
// posicão inicial das naves
glm::vec3 nave1Pos = hangar1Pos - forwardOffset - rightOffset;
glm::vec3 nave2Pos = hangar2Pos - forwardOffset + rightOffset;

int main() {
    // Carregar os objetos  
    std::vector<glm::vec3> hangarVertices, nave1Vertices, nave2Vertices, normals;
    std::vector<glm::vec2> uvs;
    const char* hangarFile = "./hangar2.obj";
    const char* nave1File = "./t65fighter.obj";
    const char* nave2File = "./nave5.obj";

    if (!loadOBJ(hangarFile, hangarVertices, uvs, normals)) {
        std::cerr << "Erro ao carregar o OBJ hangar" << std::endl;
        return -1;
    }

    if (!loadOBJ(nave1File, nave1Vertices, uvs, normals)) {
        std::cerr << "Erro ao carregar o OBJ nave1" << std::endl;
        return -1;
    }

    if (!loadOBJ(nave2File, nave2Vertices, uvs, normals)) {
        std::cerr << "Erro ao carregar o OBJ nave2" << std::endl;
        return -1;
    }

    std::cout << "Todos os objetos carregados com sucesso" << std::endl;

    // Init GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Star Wars 3D", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // OpenGL setup
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader program
    Shader lightingShader("/home/passwd/CG/Projeto2/shaders/2.1.basic_lighting.vs", "/home/passwd/CG/Projeto2/shaders/2.1.basic_lighting.fs");
    Shader lampShader("/home/passwd/CG/Projeto2/shaders/2.1.lamp.vs", "/home/passwd/CG/Projeto2/shaders/2.1.lamp.fs");
    
    //Para hangar
    unsigned int hangarVBO, hangarVAO;
    glGenVertexArrays(1, &hangarVAO);
    glGenBuffers(1, &hangarVBO);
    glBindVertexArray(hangarVAO);

    std::vector<float> hangarData;
    for (size_t i = 0; i < hangarVertices.size(); ++i) {
        hangarData.push_back(hangarVertices[i].x);
        hangarData.push_back(hangarVertices[i].y);
        hangarData.push_back(hangarVertices[i].z);
        hangarData.push_back(normals[i].x);
        hangarData.push_back(normals[i].y);
        hangarData.push_back(normals[i].z);
    }

    glBindBuffer(GL_ARRAY_BUFFER, hangarVBO);
    glBufferData(GL_ARRAY_BUFFER, hangarData.size() * sizeof(float), hangarData.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0); // Unbind
    
    //Para nave1
    unsigned int fighter1VBO, fighter1VAO;
    glGenVertexArrays(1, &fighter1VAO);
    glGenBuffers(1, &fighter1VBO);
    glBindVertexArray(fighter1VAO);

    std::vector<float> fighter1Data;
    for (size_t i = 0; i < nave1Vertices.size(); ++i) {
        fighter1Data.push_back(nave1Vertices[i].x);
        fighter1Data.push_back(nave1Vertices[i].y);
        fighter1Data.push_back(nave1Vertices[i].z);
        fighter1Data.push_back(normals[i].x);
        fighter1Data.push_back(normals[i].y);
        fighter1Data.push_back(normals[i].z);
    }

    glBindBuffer(GL_ARRAY_BUFFER, fighter1VBO);
    glBufferData(GL_ARRAY_BUFFER, fighter1Data.size() * sizeof(float), fighter1Data.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0); // Unbind

    // Para nave2
    unsigned int fighter2VBO, fighter2VAO;
    glGenVertexArrays(1, &fighter2VAO);
    glGenBuffers(1, &fighter2VBO);
    glBindVertexArray(fighter2VAO);

    std::vector<float> fighter2Data;
    for (size_t i = 0; i < nave2Vertices.size(); ++i) {
        fighter2Data.push_back(nave2Vertices[i].x);
        fighter2Data.push_back(nave2Vertices[i].y);
        fighter2Data.push_back(nave2Vertices[i].z);
        fighter2Data.push_back(normals[i].x);
        fighter2Data.push_back(normals[i].y);
        fighter2Data.push_back(normals[i].z);
    }

    glBindBuffer(GL_ARRAY_BUFFER, fighter2VBO);
    glBufferData(GL_ARRAY_BUFFER, fighter2Data.size() * sizeof(float), fighter2Data.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0); // Unbind


    // Render loop
    while (!glfwWindowShouldClose(window)) {
        // Per-frame time logic
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Input
        processInput(window);

        // Render
        glClearColor(0.0f, 0.0f, 0.2f, 1.0f); // Dark blue background
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lightingShader.use();

        // Light setup 
        lightingShader.setVec3("light.position", lightPos);
        lightingShader.setVec3("viewPos", camera.Position);
       
        lightingShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
        lightingShader.setVec3("light.diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        
        lightingShader.setVec3("material.ambient", 0.4f, 0.4f, 0.4f);
        lightingShader.setVec3("material.diffuse", 0.5f, 0.5f, 0.8f);
        lightingShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
        lightingShader.setFloat("material.shininess", 16.0f);

        // Projection and view
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        // função de movimento
        if (isMoving) {
            moveProgress += deltaTime * 0.1f; // Aumenta a velocidade do movimento
            if (moveProgress >= 1.0f) {
                moveProgress = 0.0f;  
                isMoving = false;  // As naves voltam ao iníco no fim
            }
        }

        // Render hangar 1
        glm::mat4 model = glm::translate(glm::mat4(1.0f), hangar1Pos);
        model = glm::scale(model, glm::vec3(0.05f));
        lightingShader.setMat4("model", model);
        glBindVertexArray(hangarVAO);
        glDrawArrays(GL_TRIANGLES, 0, hangarVertices.size());
        glBindVertexArray(0);

        // Render hangar 2
        model = glm::translate(glm::mat4(1.0f), hangar2Pos);
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.05f));
        lightingShader.setMat4("model", model);
        glBindVertexArray(hangarVAO);
        glDrawArrays(GL_TRIANGLES, 0, hangarVertices.size());
        glBindVertexArray(0);

       
        if (isMoving) {
            glm::vec3 forwardOffset = camera.Front * 2.0f; // Reduz a velocidade
            glm::vec3 rightOffset = camera.Right * 3.0f;   // a distância horizontal é maior entre as naves
            float yAdjustment = -0.025f;                     // A altura das naves é ajustada para que fiquem um pouco acima do chão

            // Ajusta as posições
            glm::vec3 adjustedTargetPos1 = targetPos1 - forwardOffset - rightOffset;
            adjustedTargetPos1.y += yAdjustment;

            glm::vec3 adjustedTargetPos2 = targetPos2 - forwardOffset + rightOffset;
            adjustedTargetPos2.y += yAdjustment;

            nave1Pos
         = glm::mix(hangar1Pos, adjustedTargetPos1, moveProgress);
            nave2Pos = glm::mix(hangar2Pos, adjustedTargetPos2, moveProgress);
        }


        // Render nave 1
        model = glm::translate(glm::mat4(1.0f), nave1Pos);
        model = glm::scale(model, glm::vec3(0.0105f));
        model = glm::translate(model, glm::vec3(0.0f, 100.0f, 0.0f)); // Levanta a nave
        lightingShader.setMat4("model", model);
        glBindVertexArray(fighter1VAO);
        glDrawArrays(GL_TRIANGLES, 0, nave1Vertices.size());
        glBindVertexArray(0);

        // render nave 2
        glm::mat4 model2 = glm::translate(glm::mat4(1.0f), nave2Pos);
        model2 = glm::translate(model2, glm::vec3(0.0f, -0.5f, 0.0f));// Levanta a nave
        lightingShader.setMat4("model", model2);
        glBindVertexArray(fighter2VAO);
        glDrawArrays(GL_TRIANGLES, 0, nave2Vertices.size());
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Move camera
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    // movimento das naves quando clicamos no espaço
   if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        isMoving = true; 
    } else {
        isMoving = false;
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

float mouseSensitivity = 0.05f;

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    // para reduzir a sensibilidade do rato
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}

