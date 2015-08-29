#include <iostream>
using namespace std;

#include <GLFW/glfw3.h>
#include <emscripten/emscripten.h>

/* Version 0.0.1 matthias -> Frist builds */

GLFWwindow *window;


static void error_callback(int error, const char* description) {
    cout << description << endl;
}


static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
        cout << "Time: " << glfwGetTime() << endl;
}


void renderGame() {
	glClear(GL_COLOR_BUFFER_BIT);

    float ratio;
	int width, height;
	
	ratio = width / (float) height;

	glfwGetFramebufferSize(window, &width, &height);
	ratio = width / (float) height;

    glViewport(0, 0, width, height);
    

	glfwSwapBuffers(window);
	glfwPollEvents();
}


int main() {
    if(!glfwInit())
        cout << "Starting GLFW failed!" << endl;
    cout << "Started GLFW successfully!" << endl;
    
    glfwSetErrorCallback(error_callback);
    
    window = glfwCreateWindow(640, 480, "Emscripten", NULL, NULL);

	if (!window) {
		cout << "Failed to create GLFW3 window!" << endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

    glfwMakeContextCurrent(window);
    emscripten_set_main_loop(renderGame, 0, 0);
    
    glfwSwapInterval(1);
    glfwSetKeyCallback(window, key_callback);
    
    
    glClearColor(0.0f, 1.0f, 0.0f, 1.0f);     
}
