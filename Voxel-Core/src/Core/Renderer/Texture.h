#pragma once

namespace VoxelCore {

	class Texture
	{
	public:
		virtual ~Texture() = default;

		virtual void Bind() = 0;
		virtual void UnBind() = 0;
		
		virtual const std::string& GetName() const = 0;
		virtual const std::string& GetFilePath() const = 0;

		static std::shared_ptr<Texture> CreateTexture(const std::string& name, const std::string& filepath);
	};
}