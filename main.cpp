#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "glm/glm.hpp"

#include "glm/gtc/matrix_transform.hpp"
#include "primitives/Vertex.h"
#include "ShapeGenerator.h"


static bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "OPENGL ERROR: " << error << " Function: " << function << " File: " << file << " Line : " << line << std::endl;
        return false;
    }
    return true;
}

static void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};

static ShaderProgramSource parseShader(const std::string& filepath)
{
    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1,
    };

    ShaderType type = ShaderType::NONE;
    std::ifstream stream(filepath);
    std::string line;
    std::stringstream ss[2];

    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
            {
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos)
            {
                type = ShaderType::FRAGMENT;
            }
        }
        else
        {
            ss[(int)type] << line << '\n';
        }
    }

    return { ss[0].str(), ss[1].str() };

}

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
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
        char* message = (char*)(alloca(length * sizeof(char)));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << ": " << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}


int main(void)
{
    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    window = glfwCreateWindow(1000, 1000, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW cannot be initialized." << std::endl;
    }



    ShapeData cube = ShapeGenerator::makeCube();

    glEnable(GL_DEPTH_TEST);

    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, cube.vertexBufferSize(), cube.vertices, GL_STATIC_DRAW);

 

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

    unsigned int ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cube.indexBufferSize(), cube.indices, GL_STATIC_DRAW);

    ShaderProgramSource source = parseShader("src/res/shaders/basic.shader");
    std::cout << "Vertex: \n" << source.VertexSource << std::endl;
    std::cout << "Fragment: \n" << source.FragmentSource << std::endl;
    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
    glUseProgram(shader);


    int fullTransLocation = glGetUniformLocation(shader, "fullTransformMatrix");

   
    float r = 0.0f;
    double prevTime = glfwGetTime();
    float dir = 1.0f;
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        glViewport(0, 0, 1000, 1000);

        double crntTime = glfwGetTime();
        if (crntTime = prevTime >= 1 / 60)
        {
            r += 0.50f;
            prevTime = crntTime;
        }

        glm::mat4 projectionMatrix = glm::perspective(glm::radians(60.0f), 1.0f, 0.1f, 10.0f);
        glm::mat4 projTranslationMatrix = glm::translate(projectionMatrix, glm::vec3(0.0f, 0.0f, -5.0f));
        glm::mat4 fullTransfromMatrix = glm::rotate(projTranslationMatrix, glm::radians(r), glm::vec3(dir, dir, 0.0f));
        glUniformMatrix4fv(fullTransLocation, 1, GL_FALSE, &fullTransfromMatrix[0][0]);

        glDrawElements(GL_TRIANGLES, cube.numIndices, GL_UNSIGNED_SHORT, nullptr);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

