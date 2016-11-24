#include <QtOpenGL>  
#include <QWidget>  
#include <QtCore/qglobal.h>
#include <QMouseEvent>

#include <vector>
#include <cstring>
#include <iostream>
#include <sstream>
#include <set>

#include "ICP.h"

using namespace std;
#define Debug(x) cout<<#x<<"="<<(x)<<endl;

#define GLEW_STATIC
#define GLUT_DISABLE_ATEXIT_HACK
#include <Gl/glut.h>
#include <Gl/glaux.h>

#include "MeshModel.h"
#include "trackball.h"

enum color_type
{
	DEFAULT_COLOR = 0,
	CONSECUTIVE_COLOR,
	DISCRETE_COLOR
};
enum draw_type
{
	COLOR_TYPE = 0,
	NEIGHBOR_TYPE,
	ICP_TYPE,
	REGION_TYPE
};

class GLWidget : public QGLWidget
{
	Q_OBJECT

public:
	GLWidget(QWidget *parent = 0);
	~GLWidget();

	void open_file(string file_name);
	void open_file2(string file_name);
	void open_data_file(string file_name);
	int get_vernum();
	int get_fapnum();
	int get_edgenum();
	void set_color_type(color_type new_color);
	void set_draw_type(draw_type new_draw);
	void set_neighbor_pointid(int point_id);
	void set_neighbor_point_faceid(int point_id);
	void set_neighbor_faceid(int face_id);
	void set_normal_faceid(int face_id);
	void set_color_point_num(int new_num);
	void draw_color();
	void draw_neighbor();
	void draw_icp();
	void draw_region();
	void clear_neighbors();
	vector<int> get_point_nbp();
	vector<int> get_point_nbf();
	vector<int> get_face_nbf();
	vector<double> icp();

protected:
	void initializeGL();  //初始化OpenGL窗口部件  
	void paintGL();  //绘制整个OpenGL窗口，只要有更新发生，这个函数就会被调用  
	void resizeGL(int width, int height); //处理窗口大小变化事件的，参数是新状态下的宽和高  
	void keyPressEvent(QKeyEvent *e);  //鼠标处理函数  

	void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
	void wheelEvent(QWheelEvent *event);


protected:
	bool fullscreen;  //判断是否全屏的变量  
	int window_width;
	int window_height;
	MeshModel my_model;
	MeshModel icp_model;
	vector<Vertex> ver_vec;
	vector<Facep> fap_vec;
	vector<FaceNorm> fnorm_vec;
	vector<float> color_vec;
	vector<double> icp_ans;
	bool *face_flag;
	int ver_num;
	int fap_num;
	int edge_num;
	double pModelViewMatrix[16];
	draw_type my_draw;
	color_type my_color;
	int nb_pointid;
	int nb_point_faceid;
	int nb_faceid;
	int normal_faceid;
	set<int> color_point_set;
	int color_point_num;

	int OldX;
	int OldY;
	float zoom;
};