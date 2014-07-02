
#include "tubesegment.h"
#include <gl/glut.h>
#include <cmath>

#define PI 3.1415926

TubeSegment::TubeSegment(){
	segments = 6;
	direction = vec3(0,0,1);
	ua=0;
	ub=1;
	bCapHead = false;
	bCapTail = false;
	changed();
}

TubeSegment::~TubeSegment(){
	glDeleteLists(gl_displayList,1);
	glDeleteLists(gl_displayList_halo,1);
	gl_displayList = -1;
	gl_displayList_halo = -1;
}

void TubeSegment::DrawGeometry(){
    vec3 *end_a = new vec3[segments + 1];
    vec3 *end_b = new vec3[segments + 1];

    // each end starts as unit circle in xy plane
    float angle = 0;
    const float dangle = 2 * PI / segments;
    for (int i = 0; i < segments; ++i, angle += dangle)
            end_a[i] = end_b[i] = vec3(cos(angle), sin(angle), 0);

    // rotate to the proper angle (end A first)
    vec3 row[3]; // rows of rotation matrix
    vec3 rot_axis = (direction^da).normalized();
    float rot_angle = acos(direction*da);
    rotation_matrix(rot_axis, rot_angle, row);

    for (int i = 0; i < segments; ++i)
    {
            // rotate unit circle base at origin
            vec3 rotated(  row[0]*end_a[i],
                            row[1]*end_a[i],
                            row[2]*end_a[i]);
            end_a[i] = rotated;
    }

    end_a[segments] = end_a[0];

    // repeat for end B
    rot_axis = (direction^db).normalized();
    rot_angle = acos(direction*db);
    rotation_matrix(rot_axis, rot_angle, row);

    for (int i = 0; i < segments; ++i)
    {
            // rotate unit circle base at origin
            vec3 rotated(  row[0]*end_b[i],
                            row[1]*end_b[i],
                            row[2]*end_b[i]);
            end_b[i] = rotated;
    }

    end_b[segments] = end_b[0];

    // show surface
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= segments; ++i)
    {
            glNormal3f(end_b[i].x,end_b[i].y,end_b[i].z);
            vec3 b = size_b * end_b[i] + pb;
            glColor4f(color_b.r,color_b.g,color_b.b,color_b.a);
			glTexCoord2f(ub,(float)i/segments);
            glVertex3f(b.x,b.y,b.z);

			glNormal3f(end_a[i].x,end_a[i].y,end_a[i].z);
            vec3 a = size_a * end_a[i] + pa;
			glColor4f(color_a.r,color_a.g,color_a.b,color_a.a);
			glTexCoord2f(ua,(float)i/segments);
			glVertex3f(a.x,a.y,a.z);
    }
    glEnd();

	vec3 headToTail = (pb-pa).normalized();
	if(bCapHead){
		glBegin(GL_TRIANGLE_FAN);
        glNormal3f(-headToTail.x,-headToTail.y,-headToTail.z);
        glColor4f(color_a.r,color_a.g,color_a.b,color_a.a);
        glVertex3f(pa.x,pa.y,pa.z);
		for(int i = segments;i>=0;i--){
			vec3 capP = size_a * end_a[i] + pa;
			glNormal3f(-headToTail.x,-headToTail.y,-headToTail.z);
			glColor4f(color_a.r,color_a.g,color_a.b,color_a.a);
			glVertex3f(capP.x,capP.y,capP.z);
		}
		glEnd();
	}
	if(bCapTail){
		glBegin(GL_TRIANGLE_FAN);
        glNormal3f(headToTail.x,headToTail.y,headToTail.z);
        glColor4f(color_b.r,color_b.g,color_b.b,color_b.a);
        glVertex3f(pb.x,pb.y,pb.z);
		for(int i = 0;i<=segments;i++){
			vec3 capP = size_b * end_b[i] + pb;
			glNormal3f(headToTail.x,headToTail.y,headToTail.z);
			glColor4f(color_b.r,color_b.g,color_b.b,color_b.a);
			glVertex3f(capP.x,capP.y,capP.z);
		}
		glEnd();
	}
    delete[] end_a;
    delete[] end_b;
}
void TubeSegment::DrawHere(vector<float> &vertices, vector<float> &texCoords,
		vector<unsigned char> &colors, vector<float> &normals,
		vector<float> &capvertices, vector<float> &captexCoords,
		vector<unsigned char> &capcolors, vector<float> &capnormals) const{
    vec3 *end_a = new vec3[segments + 1];
    vec3 *end_b = new vec3[segments + 1];

    // each end starts as unit circle in xy plane
    float angle = 0;
    const float dangle = 2 * PI / segments;
    for (int i = 0; i < segments; ++i, angle += dangle)
            end_a[i] = end_b[i] = vec3(cos(angle), sin(angle), 0);

    // rotate to the proper angle (end A first)
    vec3 row[3]; // rows of rotation matrix
    vec3 rot_axis = (direction^da).normalized();
    float rot_angle = acos(direction*da);
    rotation_matrix(rot_axis, rot_angle, row);

    for (int i = 0; i < segments; ++i)
    {
            // rotate unit circle base at origin
            vec3 rotated(  row[0]*end_a[i],
                            row[1]*end_a[i],
                            row[2]*end_a[i]);
            end_a[i] = rotated;
    }

    end_a[segments] = end_a[0];

    // repeat for end B
    rot_axis = (direction^db).normalized();
    rot_angle = acos(direction*db);
    rotation_matrix(rot_axis, rot_angle, row);

    for (int i = 0; i < segments; ++i)
    {
            // rotate unit circle base at origin
            vec3 rotated(  row[0]*end_b[i],
                            row[1]*end_b[i],
                            row[2]*end_b[i]);
            end_b[i] = rotated;
    }

    end_b[segments] = end_b[0];

    // show surface
   // glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= segments; ++i)
    {
            vec3 b = size_b * end_b[i] + pb;
            vec3 a = size_a * end_a[i] + pa;
			/*
            glNormal3f(end_b[i].x,end_b[i].y,end_b[i].z);
            glColor4f(color_b.r,color_b.g,color_b.b,color_b.a);
			glTexCoord2f(ub,(float)i/segments);
            glVertex3f(b.x,b.y,b.z);

			glNormal3f(end_a[i].x,end_a[i].y,end_a[i].z);
			glColor4f(color_a.r,color_a.g,color_a.b,color_a.a);
			glTexCoord2f(ua,(float)i/segments);
			glVertex3f(a.x,a.y,a.z);
			*/
			normals.push_back(end_b[i].x);
			normals.push_back(end_b[i].y);
			normals.push_back(end_b[i].z);
			vertices.push_back(b.x);
			vertices.push_back(b.y);
			vertices.push_back(b.z);
			texCoords.push_back(ub);
			texCoords.push_back((float)i/segments);
			colors.push_back(color_b.r*255);
			colors.push_back(color_b.g*255);
			colors.push_back(color_b.b*255);
			colors.push_back(color_b.a*255);

			normals.push_back(end_a[i].x);
			normals.push_back(end_a[i].y);
			normals.push_back(end_a[i].z);
			vertices.push_back(a.x);
			vertices.push_back(a.y);
			vertices.push_back(a.z);
			texCoords.push_back(ua);
			texCoords.push_back((float)i/segments);
			colors.push_back(color_a.r*255);
			colors.push_back(color_a.g*255);
			colors.push_back(color_a.b*255);
			colors.push_back(color_a.a*255);
    }
   // glEnd();

	vec3 headToTail = (pb-pa).normalized();
	if(bCapHead){
		//glBegin(GL_TRIANGLE_FAN);
       // glNormal3f(-headToTail.x,-headToTail.y,-headToTail.z);
        //glColor4f(color_a.r,color_a.g,color_a.b,color_a.a);
        //glVertex3f(pa.x,pa.y,pa.z);
		for(int i = 0;i<=segments;i++){
			vec3 capP = size_a * end_a[i] + pa;
			//glNormal3f(-headToTail.x,-headToTail.y,-headToTail.z);
			//glColor4f(color_a.r,color_a.g,color_a.b,color_a.a);
			//glVertex3f(capP.x,capP.y,capP.z);

			
			capnormals.push_back(-headToTail.x);
			capnormals.push_back(-headToTail.y);
			capnormals.push_back(-headToTail.z);
			capvertices.push_back(capP.x);
			capvertices.push_back(capP.y);
			capvertices.push_back(capP.z);
			//captexCoords.push_back(ub);
			//captexCoords.push_back((float)i/segments);
			capcolors.push_back(color_a.r*255);
			capcolors.push_back(color_a.g*255);
			capcolors.push_back(color_a.b*255);
			capcolors.push_back(color_a.a*255);
		}
		glEnd();
	}
	if(bCapTail){
		
		//glBegin(GL_TRIANGLE_FAN);
       // glNormal3f(headToTail.x,headToTail.y,headToTail.z);
       // glColor4f(color_b.r,color_b.g,color_b.b,color_b.a);
       // glVertex3f(pb.x,pb.y,pb.z);
		for(int i = 0;i<=segments;i++){
			vec3 capP = size_b * end_b[i] + pb;
			//glNormal3f(headToTail.x,headToTail.y,headToTail.z);
			//glColor4f(color_b.r,color_b.g,color_b.b,color_b.a);
			//glVertex3f(capP.x,capP.y,capP.z);
		
			capnormals.push_back(headToTail.x);
			capnormals.push_back(headToTail.y);
			capnormals.push_back(headToTail.z);
			capvertices.push_back(capP.x);
			capvertices.push_back(capP.y);
			capvertices.push_back(capP.z);
			//captexCoords.push_back(ub);
			//captexCoords.push_back((float)i/segments);
			capcolors.push_back(color_b.r*255);
			capcolors.push_back(color_b.g*255);
			capcolors.push_back(color_b.b*255);
			capcolors.push_back(color_b.a*255);
		}
		//glEnd();
	}

	// load data

    delete[] end_a;
    delete[] end_b;
}

void TubeSegment::DrawHalo(){
	vec3 *end_a = new vec3[segments + 1];
	vec3 *end_b = new vec3[segments + 1];

	// each end starts as unit circle in xy plane
	float angle = 0;
	const float dangle = 2 * PI / segments;
	for (int i = 0; i < segments; ++i, angle += dangle)
		end_a[i] = end_b[i] = vec3(cos(angle), sin(angle), 0);

	// rotate to the proper angle (end A first)
	vec3 row[3]; // rows of rotation matrix
	vec3 rot_axis = (direction^da).normalized();
	float rot_angle = acos(direction*da);
	rotation_matrix(rot_axis, rot_angle, row);

	for (int i = 0; i < segments; ++i)
	{
		// rotate unit circle base at origin
		vec3 rotated(  row[0]*end_a[i],
			row[1]*end_a[i],
			row[2]*end_a[i]);
		end_a[i] = rotated;
	}

	end_a[segments] = end_a[0];

	// repeat for end B
	rot_axis = (direction^db).normalized();
	rot_angle = acos(direction*db);
	rotation_matrix(rot_axis, rot_angle, row);

	for (int i = 0; i < segments; ++i)
	{
		// rotate unit circle base at origin
		vec3 rotated(  row[0]*end_b[i],
			row[1]*end_b[i],
			row[2]*end_b[i]);
		end_b[i] = rotated;
	}

	end_b[segments] = end_b[0];

	// show surface
	glBegin(GL_QUAD_STRIP);
	for (int i = 0; i <= segments; ++i)
	{
		glNormal3f(end_b[i].x,end_b[i].y,end_b[i].z);
		vec3 b = size_b * end_b[i] + pb;
		glColor3f(0,0,0);
		glVertex3f(b.x,b.y,b.z);

		glNormal3f(end_a[i].x,end_a[i].y,end_a[i].z);
		vec3 a = size_a * end_a[i] + pa;
		glColor3f(0,0,0);
		glVertex3f(a.x,a.y,a.z);
	}
	glEnd();
	delete[] end_a;
	delete[] end_b;
}

void TubeSegment::buildTubeSegmentDisplayList(){
	glDeleteLists(gl_displayList,1);
	gl_displayList = glGenLists(1);
	glPushMatrix();{
		glNewList(gl_displayList,GL_COMPILE);{
			DrawGeometry();
		}glEndList();
	}glPopMatrix();
}

void TubeSegment::buildHaloDisplayList(){
	glDeleteLists(gl_displayList_halo,1);
	gl_displayList_halo = glGenLists(1);
	glNewList(gl_displayList_halo,GL_COMPILE);{
		glPushMatrix();{
			DrawHalo();
		}glPopMatrix();
	}glEndList();
}

void TubeSegment::RenderTubeSegment(){
	glCallList(gl_displayList);
}

void TubeSegment::RenderHalo(){
	glCallList(gl_displayList_halo);
}

void TubeSegment::SetSegments(const float numSegs){
	segments = numSegs;
	changed();
}

void TubeSegment::SetDirections(const vec3& d1, const vec3& d2){
	da = d1;
	db = d2;
	changed();
}

void TubeSegment::SetColors(const rgba& color1, const rgba& color2){
	color_a = color1;
	color_b = color2;
	changed();
}

void TubeSegment::SetSizes(float size1, float size2){
	size_a = size1;
	size_b = size2;
	changed();
}

void TubeSegment::ScaleSizes(float scale1, float scale2){
	size_a *= scale1;
	size_b *= scale2;
	changed();
}

void TubeSegment::SetPositions(const vec3& p1, const vec3& p2){
	pa=p1;
	pb=p2;
	changed();
}
void TubeSegment::SetU(const float startU,const float endU){
	ua=startU;
	ub=endU;
	changed();
}

void TubeSegment::SetCaps(const bool capHead, const bool capTail){
	bCapHead = capHead;
	bCapTail = capTail;
	changed();
}

void TubeSegment::UpdateTubeSegment(){
	if(m_needRebuildTubeSegment){
		buildTubeSegmentDisplayList();
	}
	m_needRebuildTubeSegment = false;
}

void TubeSegment::UpdateHalo(){
	if(m_needRebuildHalo){
		buildHaloDisplayList();
	}
	m_needRebuildHalo = false;
}

void TubeSegment::changed(){
	m_needRebuildTubeSegment = true;
	m_needRebuildHalo = true;
}

void TubeSegment::WriteGeometry( vector<vec3>& vertices,vector<vec3>& normals,vector<rgba>& colors, vector<int4>& meshes){
	vec3 *end_a = new vec3[segments + 1];
	vec3 *end_b = new vec3[segments + 1];

	// each end starts as unit circle in xy plane
	float angle = 0;
	const float dangle = 2 * PI / segments;
	for (int i = 0; i < segments; ++i, angle += dangle)
		end_a[i] = end_b[i] = vec3(cos(angle), sin(angle), 0);

	// rotate to the proper angle (end A first)
	vec3 row[3]; // rows of rotation matrix
	vec3 rot_axis = (direction^da).normalized();
	float rot_angle = acos(direction*da);
	rotation_matrix(rot_axis, rot_angle, row);

	for (int i = 0; i < segments; ++i)
	{
		// rotate unit circle base at origin
		vec3 rotated(  row[0]*end_a[i],
			row[1]*end_a[i],
			row[2]*end_a[i]);
		end_a[i] = rotated;
	}

	end_a[segments] = end_a[0];

	// repeat for end B
	rot_axis = (direction^db).normalized();
	rot_angle = acos(direction*db);
	rotation_matrix(rot_axis, rot_angle, row);

	for (int i = 0; i < segments; ++i)
	{
		// rotate unit circle base at origin
		vec3 rotated(  row[0]*end_b[i],
			row[1]*end_b[i],
			row[2]*end_b[i]);
		end_b[i] = rotated;
	}

	end_b[segments] = end_b[0];

	// show surface
	int currentVerticeIdx = vertices.size();
	int currentNormalIdx = normals.size();

	for (int i = 0; i <= segments; ++i){
		normals.push_back(end_b[i]);
		vec3 b = size_b * end_b[i] + pb;
		colors.push_back(color_b);
		vertices.push_back(b);

		normals.push_back(end_a[i]);
		vec3 a = size_a * end_a[i] + pa;
		colors.push_back(color_a);
		vertices.push_back(a);
	}

	// write TubeSegment
	for (int i = currentVerticeIdx;i<normals.size()-2;i++){
		int4 t;
		if ((i-currentVerticeIdx)%2 == 0){
			t = int4(i,i+1,i+2);
		}
		else t = int4(i,i+2,i+1);
		
		meshes.push_back(t);
	}

	//write caps
	currentVerticeIdx = vertices.size();
	vec3 headToTail = (pb-pa).normalized();
	if(bCapHead){
		normals.push_back(-headToTail);
		colors.push_back(color_a);
		vertices.push_back(pa);
		for(int i = 0;i<=segments;i++){
			vec3 capP = size_a * end_a[i] + pa;
			normals.push_back(-headToTail);
			colors.push_back(color_a);
			vertices.push_back(capP);
			if(i<segments){
				int4 t(currentVerticeIdx,currentVerticeIdx+i+1,currentVerticeIdx+i+2);
				meshes.push_back(t);
			}
		}
	}
	currentVerticeIdx = vertices.size();
	if(bCapTail){
		normals.push_back(headToTail);
		colors.push_back(color_b);
		vertices.push_back(pb);
		for(int i = 0;i<=segments;i++){
			vec3 capP = size_b * end_b[i] + pb;
			normals.push_back(headToTail);
			colors.push_back(color_b);
			vertices.push_back(capP);
			if(i<segments){
				int4 t(currentVerticeIdx,currentVerticeIdx+i+2,currentVerticeIdx+i+1);
				meshes.push_back(t);
			}
		}
	}
	
	delete[] end_a;
	delete[] end_b;
}

void TubeSegment::setGLContext( int _mode ){
	if (1 == _mode){
		glEnable(GL_LIGHTING);
		glPolygonMode(GL_FRONT, GL_FILL);
		glCullFace(GL_BACK);
	}
	else{
		glDisable(GL_LIGHTING);
		glPolygonMode(GL_BACK, GL_LINE);
		glCullFace(GL_FRONT);
		glEnable (GL_BLEND);
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable (GL_LINE_SMOOTH);
		glHint (GL_LINE_SMOOTH_HINT, GL_NICEST);
		glLineWidth(5);
	}
}

void TubeSegment::rotation_matrix(const vec3& axis, const float angle, vec3 matrix_row[3]) const{
        float x = axis.x,
                  y = axis.y,
                  z = axis.z,
                  s = sin(angle),
                  c = cos(angle),
                  t = 1 - c;

        matrix_row[0] = vec3( t*x*x+c,   t*x*y-s*z, t*x*z+s*y );
        matrix_row[1] = vec3( t*x*y+s*z, t*y*y+c,   t*y*z-s*x );
        matrix_row[2] = vec3( t*x*z-s*y, t*y*z+s*x, t*z*z+c );
}


#undef PI