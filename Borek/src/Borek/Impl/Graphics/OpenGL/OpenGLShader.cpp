// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <iostream>
#include <vector>

#include <glad/glad.h>

#include "Include/Graphics/OpenGL/OpenGLShader.h"
#include "Include/Core.h"
#include "Include/Log.h"

namespace Borek {
namespace Graphics {

OpenGLShader::OpenGLShader(const std::string& vertex_source,
                           const std::string& fragment_source)
{
        bool result = Compile(vertex_source, fragment_source);
        BOREK_ENGINE_ASSERT(result, "Could not compile shader.");
}

OpenGLShader::~OpenGLShader()
{
        glad_glDeleteProgram(m_Id);
}

void OpenGLShader::Bind()
{
        glUseProgram(m_Id);
}

/*
 *
 * Example shader compilation code taken from
 * https://www.khronos.org/opengl/wiki/Shader_Compilation#Example
 * Slightly edited to be better suited for use within engine.
 *
 */
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
        {
                GLint max_length = 0;
                glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &max_length);
                std::vector<GLchar> infoLog(max_length);
                glGetShaderInfoLog(vertex_shader, max_length,
                                   &max_length, &infoLog[0]);
                
                glDeleteShader(vertex_shader);

                BOREK_ENGINE_FATAL("Could not compile vertex shader. {}",
                                   infoLog.data());
                return false;
        }

        GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        source = (const GLchar *)fragment_source.c_str();

        glShaderSource(fragment_shader, 1, &source, 0);
        glCompileShader(fragment_shader);
        glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &is_compiled);

        if (is_compiled == GL_FALSE)
        {
                GLint max_length = 0;
                glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &max_length);

                std::vector<GLchar> infoLog(max_length);
                glGetShaderInfoLog(fragment_shader, max_length,
                                   &max_length, &infoLog[0]);
                glDeleteShader(fragment_shader);
                glDeleteShader(vertex_shader);

                BOREK_ENGINE_FATAL("Could not compile fragment shader. {}",
                                   infoLog.data());

                return false;
        }

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
