#include <gl/glew.h>
#include "textureTest.h"
#include "ColorConverter.h"
#include "colortotexturemap.h"

#define NUMTEXTURES 10
#define TEXTURESIZE 128

ColorToTextureMap::ColorToTextureMap(void)
{
}


ColorToTextureMap::~ColorToTextureMap(void)
{
	deleteTextures();
}

void ColorToTextureMap::BuildTextures(){
	deleteTextures();
	m_textureNames.resize(NUMTEXTURES,-1);
	for(int i = 0;i<NUMTEXTURES;i++){
		unsigned char data[TEXTURESIZE][TEXTURESIZE][4];
		generateTextureData(data,i);
		// make texutre
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glGenTextures(1, &m_textureNames[i]);
		glBindTexture(GL_TEXTURE_2D, m_textureNames[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TEXTURESIZE, 
					TEXTURESIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, 
					data);
		glGenerateMipmap(GL_TEXTURE_2D); 

		//GLenum error = glGetError();

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 
		//				GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
		//				GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 
						GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
						GL_NEAREST);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void ColorToTextureMap::BuildScaleTexture(){
	glDeleteTextures(1,&m_scaleTextureName);
	unsigned char data[TEXTURESIZE][TEXTURESIZE*8][4];
	for(int i = 0;i<8;i++){
		int it;
		if(i<=3) it=i;
		else it=i+2;
		unsigned char tdata[TEXTURESIZE][TEXTURESIZE][4];
		generateTextureData(tdata,it);

		// copy partial to whole
		for(int x = 0;x<TEXTURESIZE;x++){
			for(int y = 0;y<TEXTURESIZE;y++){
				for(int z = 0;z<4;z++){
					data[x][TEXTURESIZE*i+y][z]=tdata[x][y][z];
				}
			}
		}

		// make texutre
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glGenTextures(1, &m_scaleTextureName);
		glBindTexture(GL_TEXTURE_2D, m_scaleTextureName);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TEXTURESIZE*8, 
					TEXTURESIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, 
					data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glGenerateMipmap(GL_TEXTURE_2D); 
		
		//GLenum error = glGetError();

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 
						GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
						GL_NEAREST_MIPMAP_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

GLuint ColorToTextureMap::GetTextureNameByColor(const rgba& color) const{
	int getIdxByBoysColor(const rgba& color);
	int texIdx = ColorConverter::getIdxByBoysColor(color);
	return m_textureNames[texIdx];
}
void ColorToTextureMap::deleteTextures(){
	for(int i = 0;i<m_textureNames.size();i++){
		if(glIsTexture(m_textureNames[i]) == GL_TRUE){
			glDeleteTextures(1,&m_textureNames[i]);
		}
	}
}

GLuint ColorToTextureMap::GetTextureNameByIndex(const int idx) const{
	return m_textureNames[idx];
}

GLuint ColorToTextureMap::GetTextureNameByInt(const int i) const{
	// do not use 4 or 5
	if(i<=3){
		return m_textureNames[i];
	}
	else{
		return m_textureNames[i+2];
	}
}