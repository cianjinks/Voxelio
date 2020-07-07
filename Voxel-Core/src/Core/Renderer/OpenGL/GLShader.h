#pragma once
#include "../Shader.h"
#include <glad/glad.h>

namespace VoxelCore {

	class GLShader : public Shader
	{
	private:
		std::string m_Name;
		std::string m_VertFilePath;
		std::string m_FragFilePath;
		GLuint m_ShaderID;
	public:
		GLShader(const std::string& name, const std::string& vertexfilepath, const std::string& fragmentfilepath);
		virtual ~GLShader();

		void AddShader(GLenum type, const std::string& filepath);

		void Bind() override;
		void UnBind() override;

		const std::string& GetName() const override;
		const std::string& GetVertexFilePath() const override;
		const std::string& GetFragmentFilePath() const override;

		// Uniforms
		void SetFloat1(const std::string& name, float value) override;
		void SetFloat2(const std::string& name, glm::vec2& value) override;
		void SetFloat3(const std::string& name, glm::vec3& value) override;
		void SetFloat4(const std::string& name, glm::vec4& value) override;

		void SetMat3(const std::string& name, glm::mat3& value) override;
		void SetMat4(const std::string& name, glm::mat4& value) override;

		void SetInt1(const std::string& name, int value) override;
		void SetInt2(const std::string& name, glm::ivec2& value) override;
		void SetInt3(const std::string& name, glm::ivec3& value) override;
		void SetInt4(const std::string& name, glm::ivec4& value) override;

		static void ErrorHandleShader(GLuint shaderobj, GLuint shaderID);

		static std::string ParseShaderFromFile(const std::string& filepath);

	};
}