
#include <iostream>

#define GLEW_STATIC
#include <GL\glew.h>
#include <GLFW\glfw3.h>

const GLuint WIDTH = 800, HEIGHT = 600;

const GLfloat vertices[] = {
	-0.5f, -0.5f, 0.0f,
	 0.5f, -0.5f, 0.0f,
	 0.0f,  0.5f, 0.0f
};

// Function setting an event on a keyboard action
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	std::cout << "key = " << key << std::endl;
	std::cout << "scancode = " << scancode << std::endl;
	std::cout << "action = " << action << std::endl;
	std::cout << "mode = " << mode << std::endl;

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) // If ESC key has been pressed, set the window to be closed
		glfwSetWindowShouldClose(window, GL_TRUE);
}

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // Defining OpenGL version to 3.3 (<major>.<minor>)
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Defining OpenGL to run on core profile instead of immediate mode
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); // The window won't be resizable by the user

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Yolo", nullptr, nullptr); // Creating a 800x600 window, with the title 'Yolo', with no particular monitor to display in (no fullscreen) and no process to share data with
	if (window == nullptr) { // If the window failed to be created, exit program
		std::cout << "Failed to create GLFW Window." << std::endl;
		glfwTerminate();
		return (EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window); // Defining the created window as the current context
	glfwSetKeyCallback(window, key_callback); // Adds the key_callback function to the window

	glewExperimental = GL_TRUE; // Setting GLEW to force modern OpenGL to be executed
	if (glewInit() != GLEW_OK) {
		std::cout << "Failed to initialize GLEW." << std::endl;
		return (EXIT_FAILURE);
	}

	int width, height;
	glfwGetFramebufferSize(window, &width, &height); // Recovers the window's width & height in variables
	glViewport(0, 0, width, height); // Set the viewport to the same size as the window's, beginning at [0;0] (defines the inside of the window as "drawable")

	while (!glfwWindowShouldClose(window)) { // Game loop: while the window hasn't been told to close itself
		glfwPollEvents(); // Calls all the events that might occur in this loop (keyboard, mouse, ...)

		// The game is ON.
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(window); // Swaps the processed frame from the background with the front one
	}

    return (EXIT_SUCCESS);
}