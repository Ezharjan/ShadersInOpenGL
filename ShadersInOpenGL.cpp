#include<GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
using namespace std;

//by Alexander

struct ShaderProgramSource
{
	string VertexSource;
	string FragmentSource;
};

static ShaderProgramSource ParseShader(const string filePath){
	
	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	ifstream stream(filePath);
	ShaderType type = ShaderType::NONE;
	string line;
	stringstream ss[2]; // One for vertex shader while the other is for the fragment shader
	while (getline(stream, line))
	{
		if (line.find("#shader") != string :: npos )
		{
			if (line.find("vertex") != string::npos)
			{
				//set vertex mode
				type = ShaderType::VERTEX;
			}
			else{
				//set fragment mode
				type = ShaderType::FRAGMENT;
			}
		}
		else
		{
			ss[(int)type] << line << '\n'; // Add line to the stream string
		}
	}
	return {ss[0].str(), ss[1].str()};
}

static int CompilerShader(unsigned int type, const string& source) {
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str(); 
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) 
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length *sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex " : "fragment ") << "shader" <<   endl;
		cout << message << endl;
		glDeleteShader(id);
		return 0;
	}
	return id;
}

static unsigned int CreateShader(const string& vertexShader, const string& fragmentShader) { 
	unsigned int programe = glCreateProgram();  // "unsigned int" equals to "GLuint"
	unsigned int vShder = CompilerShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fShder = CompilerShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(programe, vShder);
	glAttachShader(programe, fShder);
	glLinkProgram(programe);
	glValidateProgram(programe);

	glDeleteShader(vShder); 
	glDeleteShader(fShder);

	return programe;
}

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	if (glewInit() != GLEW_OK)
	{
		std::cout << "Error(Don't worry for this is just a test!)" << std::endl; 
	}

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Shaders in OpenGL", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK)
	{
		std::cout << "Error Again!!!" << std::endl;  // Will not an Error printing 
	}
	std::cout << "Your current GL version is : " << glGetString(GL_VERSION) << std::endl;

	float position[6] = {
		-0.5f, 0.5f,
		0.5f, 0.5f,
		0, -0.5f
	};
	unsigned int buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), position, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8, 0);

	ShaderProgramSource source = ParseShader("Basics.shader");

	cout << "Vertex Shader Code Below" << endl;
	cout << source.VertexSource << endl;
	cout << "Fragment Shader Code Below" << endl;
	cout << source.FragmentSource << endl;

	unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
	glUseProgram(shader);

	glBindBuffer(GL_ARRAY_BUFFER, 0);


	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		/*Draw here*/
		glDrawArrays(GL_TRIANGLES, 0, 3);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}
	glDeleteProgram(shader);
	glfwTerminate();
	return 0;
}