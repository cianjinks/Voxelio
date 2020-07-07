#pragma once
#include <glm/glm.hpp>

namespace VoxelCore {

	class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void Bind() = 0;
		virtual void UnBind() = 0;

		virtual const std::string& GetName() const = 0;
		virtual const std::string& GetVertexFilePath() const = 0;
		virtual const std::string& GetFragmentFilePath() const = 0;

		// Uniforms
		virtual void SetFloat1(const std::string& name, float value) = 0;
		virtual void SetFloat2(const std::string& name, glm::vec2& value) = 0;
		virtual void SetFloat3(const std::string& name, glm::vec3& value) = 0;
		virtual void SetFloat4(const std::string& name, glm::vec4& value) = 0;

		virtual void SetMat3(const std::string& name, glm::mat3& value) = 0;
		virtual void SetMat4(const std::string& name, glm::mat4& value) = 0;

		virtual void SetInt1(const std::string& name, int value) = 0;
		virtual void SetInt2(const std::string& name, glm::ivec2& value) = 0;
		virtual void SetInt3(const std::string& name, glm::ivec3& value) = 0;
		virtual void SetInt4(const std::string& name, glm::ivec4& value) = 0;

		static std::shared_ptr<Shader> CreateBasicShader(const std::string& name, const std::string& vertexfilepath, const std::string& fragmentfilepath);
	};
}