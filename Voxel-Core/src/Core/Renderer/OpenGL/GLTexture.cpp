#include "voxelpch.h"
#include "GLTexture.h"
#include "stb_image.h"


namespace VoxelCore {

	int GLTexture::m_ActiveTexture = 0;

	GLTexture::GLTexture(const std::string& name, const std::string& filepath)
		: m_Name(name), m_FilePath(filepath)
	{
		glGenTextures(1, &m_TextureID);
		LoadJPGImage(filepath);
	}

	GLTexture::~GLTexture()
	{
		UnBind();
		glDeleteTextures(1, &m_TextureID);
	}

	void GLTexture::Bind()
	{
		if (!m_Bound) {
			if (m_ActiveTexture < 16)
			{
				glActiveTexture(GL_TEXTURE0 + m_ActiveTexture);
				m_ActiveTexture++;
				m_Bound = true;
				glBindTexture(GL_TEXTURE_2D, m_TextureID);
			}
			else
			{
				VX_CORE_WARN("Failed to bind texture. Too many texture's bound");
			}
		}
	}

	void GLTexture::UnBind()
	{
		if (m_Bound) {
			glBindTexture(GL_TEXTURE_2D, 0);
			m_ActiveTexture--;
			m_Bound = false;
		}
	}

	void GLTexture::LoadJPGImage(const std::string& filepath)
	{
		m_ImageData = stbi_load(filepath.c_str(), &m_ImageWidth, &m_ImageHeight, &m_ImageChannels, 0);
		if (m_ImageData)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_ImageWidth, m_ImageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, m_ImageData);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else {
			VX_CORE_WARN("Failed to load texture from: {0}", filepath);
		}
		stbi_image_free(m_ImageData);
	}
}