#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include "stb_image.h"
#include "particle_system.h"
#include "shader.h"

/* Rectangle for snowflower texture */
float vertices[] = {
/*   x       y       z      u     v	*/
	-0.01f, -0.01f, -0.01f, 0.0f, 0.0f,
	 0.01f, -0.01f, -0.01f, 1.0f, 0.0f,
	 0.01f,  0.01f, -0.01f, 1.0f, 1.0f,
	 0.01f,  0.01f, -0.01f, 1.0f, 1.0f,
	-0.01f,  0.01f, -0.01f, 0.0f, 1.0f,
	-0.01f, -0.01f, -0.01f, 0.0f, 0.0f
};

Shader shader;
ParticleSystem pSystem;
GLFWwindow *window;
GLuint VAO, VBO, texture;

void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
	glViewport(0, 0, width, height);
}


void init() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "SnowSimulator", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		exit(-1);
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		exit(-1);
	}
	
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	shader.init(VS_PATH, FS_PATH);
	
	/* glVertexAttribPointer(index, size, type, normalized, stride, pointer) */
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (const GLvoid*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	/* Init snowflower texture */
	int width, height, channelNum;
	unsigned char *imageDate;
	imageDate = stbi_load(TEXTURE_PATH.c_str(), &width, &height, &channelNum, 0);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageDate);
	glGenerateMipmap(GL_TEXTURE_2D);

	glEnable(GL_DEPTH_TEST);

	
}

void render() {
	/* Clear */
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shader.use();

	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 proj = glm::mat4(1.0f);
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
	proj = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
	glUniformMatrix4fv(glGetUniformLocation(shader.getId(), "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shader.getId(), "proj"), 1, GL_FALSE, glm::value_ptr(proj));

	pSystem.update();
	for (auto it = pSystem.getParticles().begin(); it != pSystem.getParticles().end(); it++) {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, it->getPosition());
		glUniform1f(glGetUniformLocation(shader.getId(), "a"), (float)it->getLife() / (float)it->getMinLife());
		glUniformMatrix4fv(glGetUniformLocation(shader.getId(), "model"), 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
}

int main() {
	init();

	/* Loop */
	while (!glfwWindowShouldClose(window)) {
		render();
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glfwTerminate();
	return 0;
}