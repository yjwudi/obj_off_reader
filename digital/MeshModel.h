#pragma once
#include <cstdio>
#include <string>
#include <vector>
#include <iostream>
#include <fstream> 
#include <cstring>

#define GLEW_STATIC
#define GLUT_DISABLE_ATEXIT_HACK
//#include <GL/glew.h> 
#include <Gl/glut.h>
#include <Gl/glaux.h>

using namespace std;
#define Debug(x) cout<<#x<<"="<<(x)<<endl;

typedef struct Vertex
{
	float x, y, z;
}Vertex;


typedef struct Facep
{
	int num;
	int order[3];
}Facep;


typedef struct FaceNorm
{
	GLfloat x, y, z;
} FaceNorm;

class MeshModel
{
public:
	MeshModel();
	MeshModel(string file);
	~MeshModel();

	vector<Vertex> get_vertex();
	vector<Facep> get_face();
	vector<FaceNorm> get_facen();
	vector<float> get_color();
	FaceNorm _glmNormalize(FaceNorm n);
	FaceNorm _glmCross(GLfloat* u, GLfloat* v);
	int get_vernum();
	int get_fapnum();
	int get_edgenum();
	void read_off();
	void read_obj();
	void calc_norm();
	void read_color();
	

private:
	string file_name;
	vector<Vertex> ver_vec;
	vector<Facep> fap_vec;
	vector<FaceNorm> fnorm_vec;
	vector<float> color_vec;
	int ver_num;
	int fap_num;
	int edge_num;
	bool read_color_flag;
	
};

