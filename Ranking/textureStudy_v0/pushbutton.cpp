#include "pushbutton.h"
#include "myGL.h"
#include "myGLexp.h"
#include <cassert>
using namespace myGLexp;

PushButton::PushButton(void)
{
	m_hovered = false;
	m_selected = false;
}


PushButton::~PushButton(void)
{
}

void output(int x, int y, const char *string){
	int len, i;
	glRasterPos2f(x, y);
	len = (int) strlen(string);
	for (i = 0; i < len; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[i]);
	}
}

void PushButton::Draw()
{
	glViewport(m_bottomLeft.x,m_bottomLeft.y,m_range.x,m_range.y);
	//glViewport(100,100,20,20);
	if(m_selected){
		glColor3f(1,1,0);
		glLineWidth(5);
	}
	else if(m_hovered){
		glColor3f(0.5,0.5,0);
		glLineWidth(4);
	}
	else{
		glColor3f(m_color.r,m_color.g,m_color.b);
		glLineWidth(3);
	}
	
	// draw the frame box
	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();{
		glLoadIdentity();
		gluPerspective(60,1,1,1000);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();{
			glLoadIdentity();
			gluLookAt(0,0,1,0,0,0,0,1,0);
			glBegin(GL_POLYGON);{
				glVertex3f(-1,-1,-1);
				glVertex3f(1,-1,-1);
				glVertex3f(1,1,-1);
				glVertex3f(-1,1,-1);
			}glEnd();
		}glPopMatrix();
	}glPopMatrix();
	
	// draw text
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();{
		glLoadIdentity();
		gluPerspective(60,(float)m_range.x/m_range.y,1,1000);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();{
			glLoadIdentity();
			gluLookAt(0,0,1,0,0,0,0,1,0);
			glPushMatrix();{
				glColor3f(1,1,1);
				glScalef(0.003,0.003,0.003);
				int ns = m_content.size();
				glTranslatef(-m_range.x/2-(ns-1)*25,-m_range.y/2,0);
				for(int i = 0;i<ns;i++){
					glutStrokeCharacter(GLUT_STROKE_ROMAN,m_content[i]);
				}
			} glPopMatrix();

		}glPopMatrix();
	}glPopMatrix();
}

bool PushButton::IsIn(const vec3 &pos) const{
	return m_bottomLeft <= pos && pos <= (m_bottomLeft+m_range);
}

bool PushButton::IsIn(float x, float y) const{
	float tz = m_bottomLeft.z + m_range.z*0.5;
	return IsIn(vec3(x,y,tz));
}

void PushButton::SetPosition(const vec3 &pos,const vec3 &range){
	m_bottomLeft = pos;
	m_range = range;
}
void PushButton::SetPosition(const float x, const float y,const float width, const float height){
	m_bottomLeft.set(x,y,0);
	m_range.set(width,height,1);
}