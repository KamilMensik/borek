// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <vector>
#include <iostream>

#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

#include "Include/Graphics/OpenGL/OpenGLShader.h"
#include "Include/Core.h"
#include "Include/Debug/Log.h"
#include "Include/Debug/Assert.h"
#include "Include/Graphics/ShaderPreprocessor.h"

namespace Borek {
namespace Graphics {

static void GenerateBinary(GLuint program_id, const std::string& path,
                           const ShaderOptions& options)
{
        GLsizei length;
        GLenum format;
        glGetProgramiv(program_id, GL_PROGRAM_BINARY_LENGTH, &length);

        uint8_t* data = new uint8_t[length];
        glGetProgramBinary(program_id, length, &length, &format, data);

        uint32_t name_size = options.name.size();
        std::cout << "Name: " << options.name << " with size: " << name_size << "\n";

        std::ofstream file(path, std::ios::out | std::ios::binary);
        file.write(RCAST<char*>(&length), sizeof(length))
            .write(RCAST<char*>(&format), sizeof(format))
            .write(RCAST<char*>(data), length)
            .write(RCAST<char*>(&name_size), sizeof(name_size))
            .write(options.name.data(), name_size);
}

static uint32_t LoadBinary(std::ifstream& file)
{
        GLsizei length;
        GLenum format;

        file.read(RCAST<char*>(&length), sizeof(length));
        file.read(RCAST<char*>(&format), sizeof(format));

        char *data = new char[length];
        file.read(data, length);

        uint32_t id = glCreateProgram();
        glProgramBinary(id, format, data, length);

        delete[] data;

        return id;
}

void OpenGLShader::LoadShaderOptions(std::ifstream& file)
{
        // Read Name
        uint32_t name_length;
        file.read(RCAST<char*>(&name_length), sizeof(name_length));
        m_Name.resize(name_length);
        file.read(m_Name.data(), name_length);

}

OpenGLShader::OpenGLShader(const std::string& vertex_source,
                           const std::string& fragment_source)
{
        bool result = Compile(vertex_source, fragment_source);
        BOREK_ENGINE_ASSERT(result, "Could not compile shader.");
}

OpenGLShader::OpenGLShader(const std::string& path)
{
        namespace fs = std::filesystem;
        std::string bin_path = path.substr(0, path.find_last_of('.')) + ".shdr";

        if (fs::exists(bin_path) &&
            (fs::last_write_time(path) < fs::last_write_time(bin_path))) {
                std::ifstream program(bin_path,
                                      std::ios::in | std::ios::binary);
                m_Id = LoadBinary(program);
                LoadShaderOptions(program);

                return;
        }

        std::stringstream vertex_source, fragment_source;
        ShaderPreprocessor::Preprocess(path, vertex_source, fragment_source);

        bool result = Compile(vertex_source.str(), fragment_source.str());
        BOREK_ENGINE_ASSERT(result, "Could not compile shader.");

        GenerateBinary(m_Id, bin_path, ShaderPreprocessor::GetShaderOptions());
}

OpenGLShader::~OpenGLShader()
{
        glad_glDeleteProgram(m_Id);
}

void OpenGLShader::Bind() const
{
        glUseProgram(m_Id);
}

void OpenGLShader::SetUniform(const std::string name, const glm::mat4& mat)
{
        this->Bind();
        glUniformMatrix4fv(glGetUniformLocation(m_Id, name.c_str()),
                           1, GL_FALSE, glm::value_ptr(mat));
}

void OpenGLShader::SetUniform(const std::string name, const glm::vec4& mat)
{
        this->Bind();
        glUniform4f(glGetUniformLocation(m_Id, name.c_str()),
                    mat.x, mat.y, mat.z, mat.w);
}

void OpenGLShader::SetUniform(const std::string name, int num)
{
        this->Bind();
        glUniform1i(glGetUniformLocation(m_Id, name.c_str()), num);
}

void OpenGLShader::SetUniform(const std::string name, const int* nums,
                              uint32_t count)
{
        this->Bind();
        glUniform1iv(glGetUniformLocation(m_Id, name.c_str()), count, nums);
}

/*
 *
 * Example shader compilation code taken from
 * https://www.khronos.org/opengl/wiki/Shader_Compilation#Example
 * Slightly edited to be better suited for use within engine.
 *
 */
static bool CompilationError(GLuint shader, const std::string& type)
{
        GLint max_length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &max_length);
        std::vector<GLchar> infoLog(max_length);
        glGetShaderInfoLog(shader, max_length,
                           &max_length, &infoLog[0]);
        
        glDeleteShader(shader);

        BOREK_ENGINE_FATAL("Could not compile {} shader. {}",
                           type, infoLog.data());
        return false;
}

bool OpenGLShader::Compile(const std::string& vertex_source,
                           const std::string& fragment_source)
{
        GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        GLint is_compiled = 0;

        const GLchar *source = (const GLchar *)vertex_source.c_str();
        glShaderSource(vertex_shader, 1, &source, 0);
        glCompileShader(vertex_shader);
        glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &is_compiled);

        if(is_compiled == GL_FALSE)
                return CompilationError(vertex_shader, "vertex");

        GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        source = (const GLchar *)fragment_source.c_str();

        glShaderSource(fragment_shader, 1, &source, 0);
        glCompileShader(fragment_shader);
        glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &is_compiled);

        if (is_compiled == GL_FALSE)
                return CompilationError(fragment_shader, "fragment");

        m_Id = glCreateProgram();

        glAttachShader(m_Id, vertex_shader);
        glAttachShader(m_Id, fragment_shader);

        glLinkProgram(m_Id);

        GLint is_linked = 0;
        glGetProgramiv(m_Id, GL_LINK_STATUS, (int *)&is_linked);
        if (is_linked == GL_FALSE)
        {
                GLint max_length = 0;
                glGetProgramiv(m_Id, GL_INFO_LOG_LENGTH, &max_length);

                std::vector<GLchar> infoLog(max_length);
                glGetProgramInfoLog(m_Id, max_length,
                                    &max_length, &infoLog[0]);
                
                glDeleteProgram(m_Id);
                glDeleteShader(vertex_shader);
                glDeleteShader(fragment_shader);
                
                BOREK_ENGINE_FATAL("Could not link shader. {}",
                                   infoLog.data());
                return false;
        }

        glDetachShader(m_Id, vertex_shader);
        glDetachShader(m_Id, fragment_shader);

        return true;
}

}  // namespace Graphics
}  // namespace Borek
