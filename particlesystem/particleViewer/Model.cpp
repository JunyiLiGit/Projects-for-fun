/*
  Model.cpp

  Source File for Geometric Model Class
  Provides for construction of cuboid, cylinder and cone shapes tiled by triangles
  
  BIHE Computer Graphics    Donald H. House     6/22/06
*/

#include "Model.h"
#include <cstdlib>

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

using namespace std;

//
// Bookkeeping, remove all vertices and triangles
//
void Model::Clean(){
  nvertices = ntriangles = 0;
}

//
// Insert a vertex into the vertex table
//
int Model::AddVertex(const Vector3d &v){
  if(nvertices == MAXVERTICES){
    cerr << "Number of vertices exceeds maximum of " << MAXVERTICES << endl;
    exit(1);
  }
  
  vertices[nvertices] = v;
  
  return nvertices++;
}

//
// Insert a triangle and its normal into the triangle tables
//
int Model::AddTriangle(int v0, int v1, int v2){
  if(ntriangles == MAXTRIANGLES){
    cerr << "Number of triangles exceeds maximum of " << MAXTRIANGLES << endl;
    exit(1);
  }
  
  triangles[ntriangles][0] = v0;
  triangles[ntriangles][1] = v1;
  triangles[ntriangles][2] = v2;
  
  Vector3d V0(vertices[v0].x, vertices[v0].y, vertices[v0].z);
  Vector3d V1(vertices[v1].x, vertices[v1].y, vertices[v1].z);
  Vector3d V2(vertices[v2].x, vertices[v2].y, vertices[v2].z);
  
  Vector3d V01 = V1 - V0;
  Vector3d V02 = V2 - V0;
  normals[ntriangles] = (V01 % V02).normalize();
  
  return ntriangles++;
}

//
// Constructor, make sure model is empty
//
Model::Model(){
  Clean();
}

//
// Make a cuboid model
//
void Model::BuildCuboid(float width, float height, float depth){
  int v[8];
  Vector3d vector;
  int i;
  int isign, jsign, ksign;
  int vlist[36] = {0, 2, 3,     0, 3, 1,    // back
                   4, 5, 6,     5, 7, 6,    // front
		   0, 4, 2,     2, 4, 6,    // left
		   1, 7, 5,     1, 3, 7,    // right
		   0, 1, 4,     1, 5, 4,    // bottom
		   2, 7, 3,     2, 6, 7};   // top
		   
  // delete any old data that may have been built previously
  Clean();
  
  // construct the 8 vertices for the cubeoid.
  i = 0;
  for(ksign = -1; ksign <= 1; ksign += 2)
    for(jsign = -1; jsign <= 1; jsign += 2)
      for(isign = -1; isign <= 1; isign += 2){
	vector.set(isign * width / 2, jsign * height / 2, ksign * depth / 2);
	v[i++] = AddVertex(vector);
      }
	
  // construct the 12 triangles that make the 6 faces
  for(i = 0; i < 36; i += 3)
    AddTriangle(v[vlist[i]], v[vlist[i + 1]], v[vlist[i + 2]]);
}

//
// Make a cylinder model
//
void Model::BuildCylinder(float radius, float height){
  const int NUMFACETS = 16;

  int v[NUMFACETS * 2 + 2];
  Vector3d vector;
  int ksign;
  int i, j;
  float theta;
  
  Clean();

  // construct the vertices for the 2 bases of the cylinder
  i = 0;
  for(ksign = -1; ksign <= 1; ksign += 2)
    for(theta = 0; theta < 360; theta += 360.0 / NUMFACETS){
      vector.set(radius * cos(DegToRad(theta)), 
		 radius * sin(DegToRad(theta)),
		 ksign * height / 2);
      v[i++] = AddVertex(vector);
    }
  // construct the two vertices at the centers of the bases
  vector.set(0, 0, -height / 2);
  v[i++] = AddVertex(vector);
  vector.set(0, 0, height / 2);  
  v[i++] = AddVertex(vector);
			 
  // construct the triangles that make the 2 bases
  for(i = 0; i < NUMFACETS; i++){
    j = (i + 1) % NUMFACETS;
    AddTriangle(v[j], v[i], v[2 * NUMFACETS]);
    AddTriangle(v[i + NUMFACETS], v[j + NUMFACETS], v[2 * NUMFACETS + 1]);
  }
  
  // construct the triangles that make the sides
  for(i = 0; i < NUMFACETS; i++){
    j = (i + 1) % NUMFACETS;
    AddTriangle(v[i], v[j], v[j + NUMFACETS]);
    AddTriangle(v[i], v[j + NUMFACETS], v[i + NUMFACETS]);
  }
}

//
// Make a cone model
//
void Model::BuildCone(float radius, float height){
  const int NUMFACETS = 16;

  int v[NUMFACETS + 2];
  Vector3d vector;
  int i, j;
  float theta;
  
  Clean();

  // construct the vertices for the base of the cone
  i = 0;
  for(theta = 0; theta < 360; theta += 360.0 / NUMFACETS){
    vector.set(radius * cos(DegToRad(theta)), 
	       radius * sin(DegToRad(theta)),
	       -height / 2);
    v[i++] = AddVertex(vector);
  }
		 
  // construct the vertex at the center of the base and at the apex of the cone
  vector.set(0, 0, -height / 2);
  v[i++] = AddVertex(vector);
  vector.set(0, 0, height / 2);
  v[i++] = AddVertex(vector);
			 
  // construct the triangles that make the base
  for(i = 0; i < NUMFACETS; i++){
    j = (i + 1) % NUMFACETS;
    AddTriangle(v[j], v[i], v[NUMFACETS]);
  }
  
  // construct the triangles that make the sides
  for(i = 0; i < NUMFACETS; i++){
    j = (i + 1) % NUMFACETS;
    AddTriangle(v[i], v[j], v[NUMFACETS + 1]);
  }
}

//
// Draw the current model in wireframe or shaded
//
void Model::Draw(int wireframe){
  int itri, ivertex;
  int op = (wireframe? GL_LINE_LOOP: GL_POLYGON);
  
  for(itri = 0; itri < ntriangles; itri++){
    glBegin(op);
      if(!wireframe)
	glNormal3f(normals[itri].x, normals[itri].y, normals[itri].z);
      for(int i = 0; i < 3; i++){
	ivertex = triangles[itri][i];
	glVertex3f(vertices[ivertex].x, vertices[ivertex].y, vertices[ivertex].z);
      }
    glEnd();
  }
}
