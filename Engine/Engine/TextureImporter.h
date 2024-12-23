#pragma once

#include "Resource.h"
#include "Texture.h"

#include <GL/glew.h>

class TextureImporter
{
public:
	TextureImporter();
	~TextureImporter();

	void SaveTextureFile(Resource* resource);
	Texture* LoadTextureImage(Resource* resource);

	GLuint LoadIconImage(const std::string& filePath);
};