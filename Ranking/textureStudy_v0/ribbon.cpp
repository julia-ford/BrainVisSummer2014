#include "ribbon.h"
#include <cassert>
#include "myGL.h"
#include "mystdexp.h"
using namespace mystdexp;

Ribbon::Ribbon(void)
{
	Reset();
}


Ribbon::~Ribbon(void)
{
}

void Ribbon::SetPositions(const vector<vec3>* positions){
	m_positions = positions;
}

void Ribbon::SetColors(const vector<rgba>* colors){
	m_colors = colors;
}

void Ribbon::SetSizes(const vector<float>* sizes){
	m_sizes = sizes;
}

void Ribbon::SetUs(const vector<float>* us){
	m_us = us;
}

void Ribbon::Reset(){
	m_positions = NULL;
	m_colors = NULL;
	m_sizes = NULL;
	m_us = NULL;
	m_drawHalo = false;
}

void Ribbon::DrawGeometry(const vec3& normal, const vector<int> &lefts) const{
	const vector<vec3>& positions = *m_positions;
	const vector<rgba>& colors = *m_colors;
	const vector<float>& sizes = *m_sizes;
	const vector<float>& us = *m_us;
	if(positions.size()<3){
		//don't handle now
		return;
	}

	int numSamples = positions.size();
	vector<vec3> points(numSamples*2);

	vector<vec3> normals(numSamples*2);
	for(int i = 0;i<numSamples;i++){
		vec3 leftVec = (normal^getDirection(i)).normalized();
		assert(leftVec.norm()!=0);
		vec3 leftOffset = sizes[i]*leftVec;

		points[i*2] = positions[i]+leftOffset;
		points[i*2+1] = positions[i]-leftOffset;

		vec3 dir1,dir2;
		if(i==0){
			dir1 = dir2 = getDirection(i);
		}
		else{
			dir1 = points[i*2]-points[i*2-2];
			dir2 = points[i*2+1]-points[i*2-1];
		}
		normals[i*2] = dir1^leftVec;
		normals[i*2+1] = dir2^leftVec;
		normals[i*2].normalize();
		normals[i*2+1].normalize();

	}
	
	// the geo
	glEnable(GL_LIGHTING);
	for(int ileft=0;ileft<lefts.size();ileft+=2){
		for(int i = lefts[ileft];i<lefts[ileft+1];i++){
			glBegin(GL_TRIANGLE_FAN);{
				vec3& p1 = points[i*2];
				vec3& p2 = points[i*2+1];
				vec3& p3 = points[i*2+2];
				vec3& p4 = points[i*2+3];
				vec3& norm1 = normals[i*2];
				vec3& norm2 = normals[i*2+1];
				vec3& norm3 = normals[i*2+2];
				vec3& norm4 = normals[i*2+3];
				glColor4f(colors[i].r,colors[i].g,colors[i].b,colors[i].a);
				glTexCoord2f(us[i*2],0);
				glNormal3f(norm1.x,norm1.y,norm1.z);
				glVertex3f(p1.x,p1.y,p1.z);
				glTexCoord2f(us[i*2],1);
				glNormal3f(norm2.x,norm2.y,norm2.z);
				glVertex3f(p2.x,p2.y,p2.z);
				glColor4f(colors[i+1].r,colors[i+1].g,colors[i+1].b,colors[i+1].a);
				glTexCoord2f(us[i*2+1],1);
				glNormal3f(norm4.x,norm4.y,norm4.z);
				glVertex3f(p4.x,p4.y,p4.z);
				glTexCoord2f(us[i*2+1],0);
				glNormal3f(norm3.x,norm3.y,norm3.z);
				glVertex3f(p3.x,p3.y,p3.z);
			}glEnd();
		}
	}
	
	// now the halo lines
	glDisable(GL_LIGHTING);
	if(m_drawHalo){
		for(int ileft=0;ileft<lefts.size();ileft+=2){
			glBegin(GL_LINE_STRIP);{
				glColor4f(m_haloColor.r,m_haloColor.g,m_haloColor.b,m_haloColor.a);
				vec3 &p0 = points[lefts[ileft]*2+1];
				glVertex3f(p0.x,p0.y,p0.z);
				for(int i = lefts[ileft];i<=lefts[ileft+1];i++){
					vec3& p1 = points[i*2];
					glVertex3f(p1.x,p1.y,p1.z);
				}
				for(int i = lefts[ileft+1];i>=lefts[ileft];i--){
					vec3& p2 = points[i*2+1];
					glVertex3f(p2.x,p2.y,p2.z);
				}
			}glEnd();
		}
	}

}

void Ribbon::DrawGeometry(const vector<vec3>& normals, const vector<int> &lefts) const{
	const vector<vec3>& positions = *m_positions;
	const vector<rgba>& colors = *m_colors;
	const vector<float>& sizes = *m_sizes;
	const vector<float>& us = *m_us;

	if(positions.size()<3){
		//don't handle now
		return;
	}

	int numSamples = positions.size();
	vector<vec3> points(numSamples*2);
	vector<vec3> norms(numSamples*2);
	for(int i = 0;i<numSamples;i++){
		vec3 leftVec = (normals[i]^getDirection(i)).normalized();
		assert(leftVec.norm()!=0);
		vec3 leftOffset = sizes[i]*leftVec;

		points[i*2] = positions[i]+leftOffset;
		points[i*2+1] = positions[i]-leftOffset;
		
		vec3 dir1,dir2;
		if(i==0){
			dir1 = dir2 = getDirection(i);
		}
		else{
			dir1 = points[i*2]-points[i*2-2];
			dir2 = points[i*2+1]-points[i*2-1];
		}
		norms[i*2] = dir1^leftVec;
		norms[i*2+1] = dir2^leftVec;
		norms[i*2].normalize();
		norms[i*2+1].normalize();
	}
	
	// the geo
	glEnable(GL_LIGHTING);
	for(int ileft=0;ileft<lefts.size();ileft+=2){
		for(int i = lefts[ileft];i<lefts[ileft+1];i++){
			glBegin(GL_TRIANGLE_FAN);{
				vec3& p1 = points[i*2];
				vec3& p2 = points[i*2+1];
				vec3& p3 = points[i*2+2];
				vec3& p4 = points[i*2+3];
				const vec3& norm1 = norms[i*2];
				const vec3& norm2 = norms[i*2+1];
				const vec3& norm3 = norms[i*2+2];
				const vec3& norm4 = norms[i*2+3];
				glColor4f(colors[i].r,colors[i].g,colors[i].b,colors[i].a);
				if(m_us != NULL){
					glTexCoord2f(us[i*2],0);
				}
				glNormal3f(norm1.x,norm1.y,norm1.z);
				glVertex3f(p1.x,p1.y,p1.z);
				if(m_us != NULL){
					glTexCoord2f(us[i*2],1);
				}
				glNormal3f(norm2.x,norm2.y,norm2.z);
				glVertex3f(p2.x,p2.y,p2.z);
				glColor4f(colors[i+1].r,colors[i+1].g,colors[i+1].b,colors[i+1].a);
				if(m_us != NULL){
					glTexCoord2f(us[i*2+1],1);
				}
				glNormal3f(norm4.x,norm4.y,norm4.z);
				glVertex3f(p4.x,p4.y,p4.z);
				if(m_us != NULL){
					glTexCoord2f(us[i*2+1],0);
				}
				glNormal3f(norm3.x,norm3.y,norm3.z);
				glVertex3f(p3.x,p3.y,p3.z);
			}glEnd();
		}
	}
	

	// now the halo lines
	glDisable(GL_LIGHTING);
	if(m_drawHalo){
		for(int ileft=0;ileft<lefts.size();ileft+=2){
			glBegin(GL_LINE_STRIP);{
				glColor4f(m_haloColor.r,m_haloColor.g,m_haloColor.b,m_haloColor.a);
				vec3 &p0 = points[lefts[ileft]*2+1];
				glVertex3f(p0.x,p0.y,p0.z);
				for(int i = lefts[ileft];i<=lefts[ileft+1];i++){
					vec3& p1 = points[i*2];
					glVertex3f(p1.x,p1.y,p1.z);
				}
				for(int i = lefts[ileft+1];i>=lefts[ileft];i--){
					vec3& p2 = points[i*2+1];
					glVertex3f(p2.x,p2.y,p2.z);
				}
			}glEnd();
		}
	}
}

vec3 Ribbon::getDirection(int idx) const{
	const vector<vec3>& positions = *m_positions;
	assert(positions.size()>1);
	if(idx == positions.size()-1){
		return positions[idx]-positions[idx-1];
	}
	else{
		return positions[idx+1]-positions[idx];
	}
}