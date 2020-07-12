#pragma once
#include "Core/Renderer/Texture.h"
#include <glad/glad.h>

namespace VoxelCore {

	class GLTexture : public Texture
	{
	private:
		std::string m_Name;
		std::string m_FilePath;
		GLuint m_TextureID;
		int m_ImageWidth;
		int m_ImageHeight;
		int m_ImageChannels;
		unsigned char* m_ImageData;

		bool m_Bound = false;
		static int m_ActiveTexture;

	public:
		GLTexture(const std::string& name, const std::string& filepath);
		virtual ~GLTexture();

		void Bind() override;
		void UnBind() override;

		const std::string& GetName() const override { return m_Name; };
		const std::string& GetFilePath() const override { return m_FilePath; };

		void LoadJPGImage(const std::string& filepath);
	};
}