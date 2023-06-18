#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}


void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

char* read_shader(std::string file_path) {
	std::ifstream file(file_path);
	std::stringstream buffer;
	buffer << file.rdbuf();
	file.close();
	std::string content = buffer.str();
	char* char_content = new char[content.length() + 1];
	strcpy_s(char_content, content.length() + 1, content.c_str());

	return char_content;
}

void check_shader_compilation(int shader, GLenum shader_type) {

	int success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::" << shader_type << "::COMPILATION_FAILED\n" <<
			infoLog << std::endl;
	}
}

int load_shader(std::string program_path, GLenum shader_type) {
	unsigned int shader = glCreateShader(shader_type);
	const char* shader_source = read_shader(program_path);
	glShaderSource(shader, 1, &shader_source, NULL);
	glCompileShader(shader);
	check_shader_compilation(shader, shader_type);

	return shader;
}

int create_program(std::string vert_path, std::string frag_path) {
	unsigned int vertexShader = load_shader(vert_path, GL_VERTEX_SHADER);
	unsigned int fragmentShader = load_shader(frag_path, GL_FRAGMENT_SHADER);
	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	int success;
	char infoLog[512];

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	return shaderProgram;
}

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "Beginning", NULL, NULL);

	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to init GLAD" << std::endl;
		return -1;
	}

	unsigned int programs[2] = { 
		create_program("./shader.vert", "./shader.frag"),
		create_program("./shader.vert", "./shader1.frag")};

	
	

	glViewport(0, 0, 800, 600);
	

	float vertices[] = {
		0.0f, 0.0f, 0.0f,
		0.5f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f,
		0.0f, 0.0f, 0.0f,
		-0.5f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f
	};

	unsigned int VBOs[2], VAOs[2];
	glGenBuffers(2, VBOs);
	glGenVertexArrays(2, VAOs);


	for (int i = 0; i < sizeof(VAOs) / sizeof(VAOs[0]); i++) {
		glBindVertexArray(VAOs[i]);
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

	}


	
	while (!glfwWindowShouldClose(window)) {
		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);


		for (int i = 0; i < sizeof(VAOs) / sizeof(VAOs[0]); i++) {
			glUseProgram(programs[i]);
			glBindVertexArray(VAOs[i]);
			glDrawArrays(GL_TRIANGLES, i * 3, 3);
		}
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	for (int i = 0; i < sizeof(VAOs) / sizeof(VAOs[0]); i++) {
		glDeleteVertexArrays(1, &VAOs[i]);
		glDeleteBuffers(1, &VBOs[i]);
		glDeleteProgram(programs[i]);

	}

	glfwTerminate();
	return 0;
}