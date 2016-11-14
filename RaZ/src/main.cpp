
#include <iostream>
#include <array>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

const GLuint WIDTH = 800, HEIGHT = 600;

const GLfloat verticesRectangle[] = {
   0.5f,  0.5f, 0.0f, // Top right
   0.5f, -0.5f, 0.0f, // Bottom right
  -0.5f, -0.5f, 0.0f, // Bottom left
  -0.5f,  0.5f, 0.0f  // Top left
};

const GLfloat verticesDoubleTriangle[] = {
  -0.9f,  0.0f, 0.0f,
  -0.45f, 0.5f, 0.0f,
   0.0f,  0.0f, 0.0f,
   0.45f, 0.5f, 0.0f,
   0.9f,  0.0f, 0.0f
};

GLfloat firstTriangle[] = {
  -0.9f, -0.5f, 0.0f,  // Left
  -0.0f, -0.5f, 0.0f,  // Right
  -0.45f, 0.5f, 0.0f,  // Top
};

GLfloat secondTriangle[] = {
  0.0f, -0.5f, 0.0f,  // Left
  0.9f, -0.5f, 0.0f,  // Right
  0.45f, 0.5f, 0.0f   // Top
};

const GLuint indicesRectangle[] = {
  0, 1, 3,  // First triangle
  1, 2, 3   // Second triangle
};

const GLuint indicesDoubleTriangle[] = {
  0, 1, 2,  // First triangle
  2, 3, 4   // Second triangle
};

const GLchar* vertexShaderSource = {
  "#version 330 core\n"
  "layout (location = 0) in vec3 position;\n"

  "void main() {\n"
  "  gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
  "}"
};

const GLchar* fragmentShaderSourceOrange = {
  "#version 330 core\n"
  "out vec4 color;\n"

  "void main() {\n"
  "	 color = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
  "}"
};

const GLchar* fragmentShaderSourceYellow = {
  "#version 330 core\n"
  "out vec4 color;\n"

  "void main() {\n"
  "	 color = vec4(1.0f, 1.0f, 0.0f, 1.0f);\n"
  "}"
};

// Function which creates the shaders used to render
std::array<GLuint, 2> createShaders() {
  GLint success;
  GLchar infoLog[512];

  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER); // Creates the vertex shader then compiles it
  glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
  glCompileShader(vertexShader);
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) { // If shader creation failed, gets errors then prints it
    glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
    std::cout << "Error: vertex shader compilation failed." << std::endl << infoLog << std::endl;
  }

  std::array<GLuint, 2> fragmentShader;
  fragmentShader[0] = glCreateShader(GL_FRAGMENT_SHADER); // Creates the fragment shader then compiles it
  fragmentShader[1] = glCreateShader(GL_FRAGMENT_SHADER); // Creates the fragment shader then compiles it
  glShaderSource(fragmentShader[0], 1, &fragmentShaderSourceOrange, nullptr);
  glCompileShader(fragmentShader[0]);
  glGetShaderiv(fragmentShader[0], GL_COMPILE_STATUS, &success);
  if (!success) { // If shader creation failed, gets errors then prints it
    glGetShaderInfoLog(fragmentShader[0], 512, nullptr, infoLog);
    std::cout << "Error: fragment shader orange compilation failed." << std::endl << infoLog << std::endl;
  }

  glShaderSource(fragmentShader[1], 1, &fragmentShaderSourceYellow, nullptr);
  glCompileShader(fragmentShader[1]);
  glGetShaderiv(fragmentShader[1], GL_COMPILE_STATUS, &success);
  if (!success) { // If shader creation failed, gets errors then prints it
    glGetShaderInfoLog(fragmentShader[1], 512, nullptr, infoLog);
    std::cout << "Error: fragment shader yellow compilation failed." << std::endl << infoLog << std::endl;
  }

  // Creates the shader program, attaches the compiled shaders, then links it to the program
  std::array<GLuint, 2> shaderProgram;
  shaderProgram[0] = glCreateProgram();
  shaderProgram[1] = glCreateProgram();

  glAttachShader(shaderProgram[0], vertexShader);
  glAttachShader(shaderProgram[0], fragmentShader[0]);
  glLinkProgram(shaderProgram[0]);
  glGetProgramiv(shaderProgram[0], GL_LINK_STATUS, &success);
  if (!success) { // If shader program failed, gets errors then prints it
    glGetProgramInfoLog(shaderProgram[0], 512, nullptr, infoLog);
    std::cout << "Error: shader program link failed." << std::endl << infoLog << std::endl;
  }

  glAttachShader(shaderProgram[1], vertexShader);
  glAttachShader(shaderProgram[1], fragmentShader[1]);
  glLinkProgram(shaderProgram[1]);
  glGetProgramiv(shaderProgram[1], GL_LINK_STATUS, &success);
  if (!success) { // If shader program failed, gets errors then prints it
    glGetProgramInfoLog(shaderProgram[1], 512, nullptr, infoLog);
    std::cout << "Error: shader program link failed." << std::endl << infoLog << std::endl;
  }

  glDeleteShader(vertexShader); // Deletes both shaders now that they're linked to the program
  glDeleteShader(fragmentShader[0]);
  glDeleteShader(fragmentShader[1]);

  return shaderProgram;
}

// Function setting an event on a keyboard action
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
/*	std::cout << "key = " << key << std::endl;
  std::cout << "scancode = " << scancode << std::endl;
  std::cout << "action = " << action << std::endl;
  std::cout << "mode = " << mode << std::endl;*/

  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) // If ESC key has been pressed, set the window to be closed
    glfwSetWindowShouldClose(window, GL_TRUE);
}

int main() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // Defining OpenGL version to 3.3 (<major>.<minor>)
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Defining OpenGL to run on core profile instead of immediate mode
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); // The window won't be resizable by the user

  // Creating a 800x600 window, with the title 'Yolo', with no particular monitor to display in (no fullscreen) and no process to share data with
  GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Yolo", nullptr, nullptr);
  if (window == nullptr) { // If the window failed to be created, exit program
    std::cout << "Failed to create GLFW Window." << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window); // Defining the created window as the current context
  glfwSetKeyCallback(window, key_callback); // Adds the key_callback function to the window

  glewExperimental = GL_TRUE; // Setting GLEW to force modern OpenGL to be executed
  if (glewInit() != GLEW_OK) {
    std::cout << "Failed to initialize GLEW." << std::endl;
    return -1;
  }

  int width, height;
  glfwGetFramebufferSize(window, &width, &height); // Recovers the window's width & height in variables
  // Set the viewport to the same size as the window's, beginning at [0;0] (defines the inside of the window as "drawable")
  glViewport(0, 0, width, height);

  std::array<GLuint, 2> shaderProgram = createShaders(); // Creates the shaders used to render the scene

  // VAO == Vertex Array Object; VBO == Vertex Buffer Object[; EBO == Element Buffer Object]
  std::array<GLuint, 2> VAOs, VBOs; // Creating VAO, VBO [& EBO]
  glGenVertexArrays(2, VAOs.data());
  glGenBuffers(2, VBOs.data());

  glBindVertexArray(VAOs[0]);                                 // Using VAO
  glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);                     // Binds & fills VBO
  glBufferData(GL_ARRAY_BUFFER, sizeof(firstTriangle), firstTriangle, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), static_cast<GLvoid*>(0));
  glEnableVertexAttribArray(0);
  glBindVertexArray(0);

  glBindVertexArray(VAOs[1]);                                 // Using VAO
  glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);                     // Binds & fills VBO
  glBufferData(GL_ARRAY_BUFFER, sizeof(secondTriangle), secondTriangle, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), static_cast<GLvoid*>(0));
  glEnableVertexAttribArray(0);
  glBindVertexArray(0);

  // Uncommenting this call will result in wireframe polygons
  //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  while (!glfwWindowShouldClose(window)) { // Game loop: while the window hasn't been told to close itself
    glfwPollEvents(); // Calls all the events that might occur in this loop (keyboard, mouse, ...)

    // The game is ON.
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw triangle
    glUseProgram(shaderProgram[0]);
    glBindVertexArray(VAOs[0]);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glUseProgram(shaderProgram[1]);
    glBindVertexArray(VAOs[1]);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glBindVertexArray(0);

    glfwSwapBuffers(window); // Swaps the processed frame from the background with the front one
  }

  // De-allocates
  glDeleteVertexArrays(2, VAOs.data());
  glDeleteBuffers(2, VBOs.data());
  glfwTerminate(); // Clears any resource allocated by GLFW

  return 0;
}
