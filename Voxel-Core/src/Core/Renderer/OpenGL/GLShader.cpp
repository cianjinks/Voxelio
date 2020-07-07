#include "voxelpch.h"
#include "GLShader.h"

#include <fstream>
#include <glm/gtc/type_ptr.hpp>

namespace VoxelCore {

	GLShader::GLShader(const std::string& name, const std::string& vertexfilepath, const std::string& fragmentfilepath)
		: m_Name(name), m_VertFilePath(vertexfilepath), m_FragFilePath(fragmentfilepath)
	{
		m_ShaderID = glCreateProgram();
        AddShader(GL_VERTEX_SHADER, vertexfilepath);
        AddShader(GL_FRAGMENT_SHADER, fragmentfilepath);
        glLinkProgram(m_ShaderID);
        glValidateProgram(m_ShaderID);
        glUseProgram(m_ShaderID);
	}

	GLShader::~GLShader()
	{
        glDeleteProgram(m_ShaderID);
	}

    void GLShader::Bind()
    {
        glUseProgram(m_ShaderID);
    }

    void GLShader::UnBind()
    {
        glUseProgram(0);
    }

    const std::string& GLShader::GetName() const
    {
        return m_Name;
    }

    const std::string& GLShader::GetVertexFilePath() const
    {
        return m_VertFilePath;
    }

    const std::string& GLShader::GetFragmentFilePath() const
    {
        return m_FragFilePath;
    }

	void GLShader::AddShader(GLenum type, const std::string& filepath)
	{
        std::string source = ParseShaderFromFile(filepath);
        const char* rawsource = source.c_str();
        unsigned int shaderObj = glCreateShader(type);
        glShaderSource(shaderObj, 1, &rawsource, nullptr);
        glCompileShader(shaderObj);
        ErrorHandleShader(shaderObj, m_ShaderID);
        glAttachShader(m_ShaderID, shaderObj);
        glDeleteShader(shaderObj);
	}

    void GLShader::ErrorHandleShader(GLuint shaderobj, GLuint shaderid)
    {
        GLint result;
        glGetShaderiv(shaderobj, GL_COMPILE_STATUS, &result);
        if (result == GL_FALSE)
        {
            GLint length;
            glGetShaderiv(shaderobj, GL_INFO_LOG_LENGTH, &length);
            GLchar* log = (GLchar*)malloc(length);
            glGetShaderInfoLog(shaderobj, length, &length, log);
            VX_CORE_CRITICAL("Failed to compile shader: {}", log);
            free(log);
            glDeleteShader(shaderobj);
            glDeleteProgram(shaderid);
        }
    }

    std::string GLShader::ParseShaderFromFile(const std::string& filepath)
    {
        std::string result;
        std::ifstream in(filepath, std::ios::in | std::ios::binary);
        if (in)
        {
            in.seekg(0, std::ios::end);
            size_t size = in.tellg();
            if (size != -1)
            {
                result.resize(size);
                in.seekg(0, std::ios::beg);
                in.read(&result[0], size);
                in.close();
            }
            else
            {
                VX_CORE_ERROR("Could not read from file: {}", filepath);
            }
        }
        else
        {
            VX_CORE_ERROR("Could not open file: {}", filepath);
            return "";
        }

        return result;
    }

    // UNIFORMS

    void GLShader::SetFloat1(const std::string& name, float value)
    {
        unsigned int location = glGetUniformLocation(m_ShaderID, name.c_str());
        glUniform1f(location, value);
    }

    void GLShader::SetFloat2(const std::string& name, glm::vec2& value)
    {
        unsigned int location = glGetUniformLocation(m_ShaderID, name.c_str());
        glUniform2f(location, value.x, value.y);
    }

    void GLShader::SetFloat3(const std::string& name, glm::vec3& value)
    {
        unsigned int location = glGetUniformLocation(m_ShaderID, name.c_str());
        glUniform3f(location, value.x, value.y, value.z);
    }

    void GLShader::SetFloat4(const std::string& name, glm::vec4& value)
    {
        unsigned int location = glGetUniformLocation(m_ShaderID, name.c_str());
        glUniform4f(location, value.x, value.y, value.z, value.w);
    }

    void GLShader::SetMat3(const std::string& name, glm::mat3& value)
    {
        unsigned int location = glGetUniformLocation(m_ShaderID, name.c_str());
        glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void GLShader::SetMat4(const std::string& name, glm::mat4& value)
    {
        unsigned int location = glGetUniformLocation(m_ShaderID, name.c_str());
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void GLShader::SetInt1(const std::string& name, int value)
    {
        unsigned int location = glGetUniformLocation(m_ShaderID, name.c_str());
        glUniform1i(location, value);
    }

    void GLShader::SetInt2(const std::string& name, glm::ivec2& value)
    {
        unsigned int location = glGetUniformLocation(m_ShaderID, name.c_str());
        glUniform2i(location, value.x, value.y);
    }

    void GLShader::SetInt3(const std::string& name, glm::ivec3& value)
    {
        unsigned int location = glGetUniformLocation(m_ShaderID, name.c_str());
        glUniform3i(location, value.x, value.y, value.z);
    }

    void GLShader::SetInt4(const std::string& name, glm::ivec4& value)
    {
        unsigned int location = glGetUniformLocation(m_ShaderID, name.c_str());
        glUniform4i(location, value.x, value.y, value.z, value.w);
    }
}