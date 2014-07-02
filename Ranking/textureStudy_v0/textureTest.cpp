#include "myGL.h"

#define textureSize 128

void setWhite(unsigned char color[4]){
	color[0] = 255;
	color[1] = 255;
	color[2] = 255;
	color[3] = 255;
}

void setBlack(unsigned char color[4]){
	color[0] = 0;
	color[1] = 0;
	color[2] = 0;
	color[3] = 255;
}

void setColor(unsigned char color[4]){
	color[0] = 200;
	color[1] = 200;
	color[2] = 200;
	color[3] = 255;
}

bool isBoundary(int i,int j){
	if(i==0 || j==0){
		return true;
	}
	if(i==textureSize-1 || j == textureSize-1){
		return true;
	}
	return false;
}
void generateTextureData1( unsigned char tex[textureSize][textureSize][4]){
	for(int i = 0;i<textureSize;i++){
		for(int j = 0;j<textureSize;j++){
			if(i+j>textureSize/2+2
				&& i+j<textureSize*1.5-2
				&& j-i<textureSize/2-2
				&& j-i>-textureSize/2+2
				&& !isBoundary(i,j)){
				// within the square
				// black
				setBlack(tex[i][j]);
			}
			else setColor(tex[i][j]);
		}
	}
}

void generateTextureData2( unsigned char tex[textureSize][textureSize][4]){
	for(int i = 0;i<textureSize;i++){
		for(int j = 0;j<textureSize;j++){
			int c = 0;
			int r = textureSize/3;
			int center = textureSize/2;
			int d1 = center-i;
			int d2 = center-j;
			if(d1*d1+d2*d2<r*r){
				// within the square
				// black
				setBlack(tex[i][j]);
			}
			else setColor(tex[i][j]);
		}
	}
}

void generateTextureData3( unsigned char tex[textureSize][textureSize][4]){
	for(int i = 0;i<textureSize;i++){
		for(int j = 0;j<textureSize;j++){
			int c = 0;
			int r1 = textureSize/3;
			int r2 = textureSize/5;
			int center = textureSize/2;
			int d1 = center-i;
			int d2 = center-j;
			int dis = d1*d1+d2*d2;
			if( dis<r1*r1 && dis > r2*r2){
				// within the square
				// black
				setBlack(tex[i][j]);
			}
			else setColor(tex[i][j]);
		}
	}
}


void generateTextureData4( unsigned char tex[textureSize][textureSize][4]){
	for(int i = 0;i<textureSize;i++){
		for(int j = 0;j<textureSize;j++){
			int c = 0;
			int length = textureSize/4;
			int thickness = textureSize/8;
			int center = textureSize/2;
			if( j>center-length && j <center+length
				&& i>center-thickness && i <center+thickness){
				// within the square
				// black
				setBlack(tex[i][j]);
			}
			else setColor(tex[i][j]);
		}
	}
}

void generateTextureData5( unsigned char tex[textureSize][textureSize][4]){
	for(int i = 0;i<textureSize;i++){
		for(int j = 0;j<textureSize;j++){
			int c = 0;
			int thickness = textureSize/8;
			int center = textureSize/2;
			if( j>center-thickness && j <center+thickness
				&& i>center-thickness && i <center+thickness){
				// within the square
				// black
				setBlack(tex[i][j]);
			}
			else setColor(tex[i][j]);
		}
	}
}

void generateTextureData6( unsigned char tex[textureSize][textureSize][4]){
	for(int i = 0;i<textureSize;i++){
		for(int j = 0;j<textureSize;j++){
			int c = 0;
			int thickness = textureSize/8;
			int center = textureSize/2;
			if( j>center-thickness && j <center+thickness
				&& i>center-thickness && i <center+thickness){
				// within the square
				// white
				setWhite(tex[i][j]);
			}
			else setColor(tex[i][j]);
		}
	}
}

void generateTextureData7( unsigned char tex[textureSize][textureSize][4]){
	for(int i = 0;i<textureSize;i++){
		for(int j = 0;j<textureSize;j++){
			int c = 255;
			int length = textureSize/4;
			int thickness = textureSize/8;
			int center = textureSize/2;
			if( j>center-length && j <center+length
				&& i>center-thickness && i <center+thickness){
				// within the square
				// white
				setWhite(tex[i][j]);
			}
			else setColor(tex[i][j]);
		}
	}
}

void generateTextureData8( unsigned char tex[textureSize][textureSize][4]){
	for(int i = 0;i<textureSize;i++){
		for(int j = 0;j<textureSize;j++){
			int c = 255;
			int r1 = textureSize/3;
			int r2 = textureSize/5;
			int center = textureSize/2;
			int d1 = center-i;
			int d2 = center-j;
			int dis = d1*d1+d2*d2;
			if( dis<r1*r1 && dis > r2*r2){
				// within the square
				// white
				setWhite(tex[i][j]);
			}
			else setColor(tex[i][j]);
		}
	}
}

void generateTextureData9( unsigned char tex[textureSize][textureSize][4]){
	for(int i = 0;i<textureSize;i++){
		for(int j = 0;j<textureSize;j++){
			int c = 255;
			int r = textureSize/3;
			int center = textureSize/2;
			int d1 = center-i;
			int d2 = center-j;
			if(d1*d1+d2*d2<r*r){
				// within the square
				// white
				setWhite(tex[i][j]);
			}
			else setColor(tex[i][j]);
		}
	}
}

void generateTextureData10( unsigned char tex[textureSize][textureSize][4]){
	for(int i = 0;i<textureSize;i++){
		for(int j = 0;j<textureSize;j++){
			int c = 255;
			if(i+j>textureSize/2+2
				&& i+j<textureSize*1.5-2
				&& j-i<textureSize/2-2
				&& j-i>-textureSize/2+2
				&& !isBoundary(i,j)){
				// within the square
				// white
				setWhite(tex[i][j]);
			}
			else setColor(tex[i][j]);
		}
	}
}

void generateTextureData(unsigned char tex[textureSize][textureSize][4], int idx){
	
	switch(idx){
	case 0:
		generateTextureData1(tex);
		break;
	case 1:
		generateTextureData2(tex);
		break;
	case 2:
		generateTextureData3(tex);
		break;
	case 3:
		generateTextureData4(tex);
		break;
	case 4:
		generateTextureData5(tex);
		break;
	case 5:
		generateTextureData6(tex);
		break;
	case 6:
		generateTextureData7(tex);
		break;
	case 7:
		generateTextureData8(tex);
		break;
	case 8:
		generateTextureData9(tex);
		break;
	case 9:
		generateTextureData10(tex);
		break;
	default:
		break;
	}
	
/*
	for(int i = 0;i<textureSize;i++){
		for(int j = 0;j<textureSize;j++){
			int c = 0;
			int length = textureSize/8;
			int thickness = (idx+1)/11.f*textureSize/2;
			int center = textureSize/2;
			if( j>center-length && j <center+length
				&& i>center-thickness && i <center+thickness){
				// within the square
				// black
				setBlack(tex[i][j]);
			}
			else setColor(tex[i][j]);
		}
	}
	*/
}