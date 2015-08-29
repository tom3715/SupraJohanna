#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdio.h>
#include <string>
#include <math.h>

#include "shader.h"
#include "shell.h"
//#include "maths.h"
#include "camera.h"

int main() {
    GLuint vao;
    GLuint vbo_points;
    GLuint vbo_colors;

    GLfloat points[] = {
        0.0f, 0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f
    };
    GLfloat colors[] = {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f
    };

    shell.title = "OpenGL";

    glfwSetErrorCallback(glfw_error_callback);

    glfwSetWindowSizeCallback(shell.window, glfw_window_size_callback);
    glfwSetKeyCallback(shell.window, key_callback);
    glfwSetCursorPosCallback(shell.window, mouse_callback);
    glfwSetMouseButtonCallback(shell.window, mouse_button_callback);
    Shader testShader("res/shader.vert", "res/shader.frag");

    glGenBuffers(1, &vbo_points);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_points);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

    glGenBuffers(1, &vbo_colors);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_colors);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_points);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_colors);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    int model_view_location = glGetUniformLocation(testShader.Program, "view");
    int model_proj_location = glGetUniformLocation(testShader.Program, "proj");

    while (!glfwWindowShouldClose(shell.window)) {
        double elapsed_seconds = shell.update_fps_counter(shell.window);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, shell.screenWidth, shell.screenHeight);

		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 3);

        glm::mat4 view;
        view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(camera.Zoom, (float)shell.screenWidth/(float)shell.screenHeight, 0.1f, 100.0f);
        glUniformMatrix4fv(model_view_location, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(model_proj_location, 1, GL_FALSE, glm::value_ptr(projection));
		testShader.Use();
		glfwPollEvents();
        camera.do_movement(elapsed_seconds);
		glfwSwapBuffers(shell.window);
	}

    testShader.print_all();
	glfwTerminate();
	return 0;
}
