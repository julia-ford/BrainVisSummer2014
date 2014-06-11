#pragma once
#include "rgba.h"
#include <vector>
#include "myGL.h"
using namespace std;

class ColorToTextureMap
{
public:
	ColorToTextureMap(void);
	~ColorToTextureMap(void);
	
	void BuildTextures();
	void BuildScaleTexture();
	GLuint GetTextureNameByColor(const rgba& dir) const;
	GLuint GetTextureNameByIndex(const int idx) const;
	GLuint GetTextureNameByInt(const int i) const;

	GLuint GetScaleTextureName() const{return m_scaleTextureName;};

protected:
	vector<GLuint> m_textureNames;
	GLuint m_scaleTextureName;
	void deleteTextures();
};

