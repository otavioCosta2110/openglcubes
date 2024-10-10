/* Este programa cria um cenário simples com um terreno plano verde usando OpenGL moderno com GLEW, GLFW e GLM. O terreno é renderizado como um quadrado plano no plano XZ, e a câmera está posicionada acima e atrás do terreno, olhando para o centro.*/

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;
void criarEsfera(float radius, int sectors, int stacks, std::vector<float>& vertices, std::vector<unsigned int>& indices);

// Função de callback para redimensionamento da janela
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}

// Função para processar entrada do usuário
void processInput(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

int main() {
  // Inicialização do GLFW
  if (!glfwInit()) {
    std::cerr << "Falha ao inicializar GLFW" << std::endl;
    return -1;
  }

  // Configuração do GLFW
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Criação da janela
  GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Terreno Plano OpenGL", NULL, NULL);
  if (window == NULL) {
    std::cerr << "Falha ao criar janela GLFW" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  // Inicialização do GLEW
  if (glewInit() != GLEW_OK) {
    std::cerr << "Falha ao inicializar GLEW" << std::endl;
    return -1;
  }

  // Vertex Shader
  const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aColor;
    out vec3 ourColor;
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;
    void main() {
        gl_Position = projection * view * model * vec4(aPos, 1.0);
        ourColor = aColor;
    }
)";

  // Fragment Shader
  const char* fragmentShaderSource = R"(
    #version 330 core
    in vec3 ourColor;
    out vec4 FragColor;
    void main() {
        FragColor = vec4(ourColor, 1.0);
    }
)";

  // Compilação e link dos shaders
  unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);

  unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);

  unsigned int shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  // Criação do terreno plano
  std::vector<float> vertices = {
    // Posições            // Cores
    -10.0f, 0.0f, -10.0f,  0.0f, 0.8f, 0.0f,
    10.0f, 0.0f, -10.0f,  0.0f, 0.8f, 0.0f,
    10.0f, 0.0f,  10.0f,  0.0f, 0.8f, 0.0f,
    -10.0f, 0.0f,  10.0f,  0.0f, 0.8f, 0.0f
  };

  std::vector<unsigned int> indices = {
    0, 1, 2,
    2, 3, 0
  };

  unsigned int VBO, VAO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  // Configuração da matriz de projeção
  glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);

  // Configuração da matriz de visualização
  glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 5.0f, 15.0f),
      glm::vec3(0.0f, 0.0f, 0.0f),
      glm::vec3(0.0f, 1.0f, 0.0f));

  // Configuração da matriz de modelo
  glm::mat4 model = glm::mat4(1.0f);

  // Habilitação do teste de profundidade
  glEnable(GL_DEPTH_TEST);


  std::vector<float> sphereVertices;
  std::vector<unsigned int> sphereIndices;

  criarEsfera(1.0f, 30, 30, sphereVertices, sphereIndices);

  unsigned int sphereVAO, sphereVBO, sphereEBO;
  glGenVertexArrays(1, &sphereVAO);
  glGenBuffers(1, &sphereVBO);
  glGenBuffers(1, &sphereEBO);

  glBindVertexArray(sphereVAO);

  glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
  glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(float), sphereVertices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(unsigned int), sphereIndices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  const int NUM_ESFERAS = 5; 
  std::vector<glm::vec3> esferaPosicoes = {
    glm::vec3(6.0f, 2.0f, -1.0f), 
    glm::vec3(1.0f, 1.0f, 2.0f),
    glm::vec3(-3.0f, 4.0f, 2.0f),
    glm::vec3(3.0f, 3.0f, 4.0f),
    glm::vec3(-5.0f, 2.0f, 5.0f) 
  };

  // Loop principal
  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgram);

    unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
    unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
    unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");

    glBindVertexArray(VAO);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    for (int i = 0; i < NUM_ESFERAS; ++i) {
      glm::mat4 sphereModel = glm::mat4(1.0f);
      sphereModel = glm::translate(sphereModel, esferaPosicoes[i]);
      glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(sphereModel));

      glBindVertexArray(sphereVAO);
      glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT, 0);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // Limpeza e encerramento
  glfwTerminate();

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  glDeleteVertexArrays(1, &sphereVAO);
  glDeleteBuffers(1, &sphereVBO);
  glDeleteBuffers(1, &sphereEBO);

  return 0;
}


void criarEsfera(float radius, int sectors, int stacks, std::vector<float>& vertices, std::vector<unsigned int>& indices) {
  float x, y, z, xy;                            
  float nx, ny, nz, lengthInv = 1.0f / radius;  
  float s, t;                                   

  float sectorStep = 2 * M_PI / sectors;
  float stackStep = M_PI / stacks;
  float sectorAngle, stackAngle;

  for (int i = 0; i <= stacks; ++i) {
    stackAngle = M_PI / 2 - i * stackStep;     
    xy = radius * cosf(stackAngle);            
    z = radius * sinf(stackAngle);             

    for (int j = 0; j <= sectors; ++j) {
      sectorAngle = j * sectorStep;            

      x = xy * cosf(sectorAngle);             
      y = xy * sinf(sectorAngle);             
      vertices.push_back(x);
      vertices.push_back(y);
      vertices.push_back(z);

      nx = x * lengthInv;
      ny = y * lengthInv;
      nz = z * lengthInv;
      vertices.push_back(nx);
      vertices.push_back(ny);
      vertices.push_back(nz);

      s = (float)j / sectors;
      t = (float)i / stacks;
      vertices.push_back(s);
      vertices.push_back(t);
    }
  }

  unsigned int k1, k2;
  for (int i = 0; i < stacks; ++i) {
    k1 = i * (sectors + 1);     
    k2 = k1 + sectors + 1;      

    for (int j = 0; j < sectors; ++j, ++k1, ++k2) {
      if (i != 0) {
        indices.push_back(k1);
        indices.push_back(k2);
        indices.push_back(k1 + 1);
      }
      if (i != (stacks - 1)) {
        indices.push_back(k2);
        indices.push_back(k2 + 1);
        indices.push_back(k1 + 1);
      }
    }
  }
}

