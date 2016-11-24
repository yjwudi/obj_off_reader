#include "glwidget.h"  

#include <QtGui>  
#include <QtCore>  
#include <QtOpenGL>  

static void normalizeAngle(double &angle)
{
	while (angle < 0.) angle += 360. * 16;
	while (angle > 360.) angle -= 360. * 16;
}

GLWidget::GLWidget(QWidget *parent): QGLWidget(parent)
{
	fullscreen = false;
	int window_height = 550;
	int window_width = 600;
	ver_num = 0;
	fap_num = 0;
	edge_num = 0;
	color_point_num = 0;
	ver_vec.clear();
	fap_vec.clear();
	fnorm_vec.clear();
	memset(pModelViewMatrix, 0, sizeof(pModelViewMatrix));
	pModelViewMatrix[0] = 1;
	pModelViewMatrix[5] = 1;
	pModelViewMatrix[10] = 1;
	pModelViewMatrix[15] = 1;
	zoom = 1.0;
	my_draw = draw_type::COLOR_TYPE;
	my_color = color_type::DEFAULT_COLOR;
	nb_pointid = 0;
	nb_point_faceid = 0;
	nb_faceid = 0;
	normal_faceid = 0;
	face_flag = NULL;

}

GLWidget::~GLWidget()
{
}

//这是对虚函数，这里是重写该函数  
void GLWidget::initializeGL()
{
	setGeometry(200, 20, window_width, window_height);//设置窗口初始位置和大小
	glShadeModel(GL_FLAT);//设置阴影平滑模式
	//glClearColor(0.5, 1.0, 0.2, 0);//改变窗口的背景颜色，不过我这里貌似设置后并没有什么效果
	glClearDepth(1.0);//设置深度缓存
	glEnable(GL_DEPTH_TEST);//允许深度测试
	glDepthFunc(GL_LEQUAL);//设置深度测试类型
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);//进行透视校正

	glClearColor(0.0, 0.0, 0.0, 0.0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (float)window_width / (float)window_height, 0.01f, 200.0f);

	glClearColor(0, 0, 0, 1);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	

	glEnable(GL_CULL_FACE);

	// Setup other misc features.
	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);
	glShadeModel(GL_SMOOTH);

	// Setup lighting model.
	/*GLfloat light_model_ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat light0_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat light0_direction[] = { 0.0f, 0.0f, 10.0f, 0.0f };
	GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	glLightfv(GL_LIGHT0, GL_POSITION, light0_direction);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_model_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glEnable(GL_LIGHT0);*/
	GLfloat lightPosition[] = {8.5, 5.0, -2.0, 0.0};   // w不为0

	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glEnable(GL_LIGHT0);//光源的颜色为白光

}

void GLWidget::open_file(string file_name)
{
	my_model = MeshModel(file_name);
	ver_vec = my_model.get_vertex();
	fap_vec = my_model.get_face();
	fnorm_vec = my_model.get_facen();
	ver_num = my_model.get_vernum();
	fap_num = my_model.get_fapnum();
	edge_num = my_model.get_edgenum();
	face_flag = new bool[fap_num+10];
	memset(face_flag, 0, sizeof(face_flag));
	this->set_draw_type(draw_type::COLOR_TYPE);
	updateGL();
}
void GLWidget::open_file2(string file_name)
{
	icp_model = MeshModel(file_name);
}
void GLWidget::open_data_file(string file_name)
{
	color_point_set.clear();
	ifstream in_file(file_name);
	streambuf *strmin_buf = cin.rdbuf();
	cin.rdbuf(in_file.rdbuf());

	int a;
	while(cin >> a)
		color_point_set.insert(a);

	cin.rdbuf(strmin_buf);
}
int GLWidget::get_vernum()
{
	return ver_num;
}
int GLWidget::get_fapnum()
{
	return fap_num;
}
int GLWidget::get_edgenum()
{
	return edge_num;
}

void GLWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslated(0.0, 0.0, -5.0);
	glScalef(zoom, zoom, zoom);
	glMultMatrixd(pModelViewMatrix);

	switch(my_draw)
	{
	case draw_type::COLOR_TYPE:
		draw_color();
		break;
	case draw_type::NEIGHBOR_TYPE:
		draw_neighbor();
		break;
	case draw_type::ICP_TYPE:
		draw_icp();
		break;
	case draw_type::REGION_TYPE:
		draw_region();
		break;
	default:
		break;
	}
}

void GLWidget::draw_color()
{
	vector<float> color_vec;
	float color_vr = 0.0, color_vg = 0.1, color_vb = 0.2, color_max = -10;
	if(my_color == color_type::DISCRETE_COLOR)
	{
		color_vec = my_model.get_color();
		for(int i = 0; i < color_vec.size(); i++)
			color_max = max(color_max, color_vec[i]);
	}
	GLfloat matAmbient[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat matDiffuse[]   = { 1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat matAmbDif[]   = {0.5, 0.5, 0.5, 1.0};
	GLfloat matSpecular[] = {0.2, 0.2, 0.2, 1.0};
	GLfloat shine[] = {5.0};
	GLfloat matEmission[] = {0.3, 0.1, 0.1, 1.0};
	GLfloat color_render[]={0.0/255.0, 191.0/255.0, 255.0/255.0};

	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDiffuse);//散射光
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, matAmbDif);  // 将背景颜色和散射颜色设置成同一颜色
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpecular);//镜面光
	glMaterialfv(GL_FRONT, GL_SHININESS, shine);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, matAmbient);
	glEnable(GL_LIGHTING);//允许光源
	glLightfv(GL_LIGHT0, GL_DIFFUSE, matDiffuse); 

	glPushMatrix();
	if(my_color == color_type::CONSECUTIVE_COLOR)
	{
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE,color_render );
	}
	for (int i = 0; i < fap_num; i++)
	{
		if(my_color == color_type::DISCRETE_COLOR)
		{
			if(i < color_vec.size())
			{
				color_vr = 0.0;
				color_vg = 1.0 - color_vec[i]/color_max;
				color_vb = color_vec[i]/color_max;
				color_render[0] = color_vr;
				color_render[1] = color_vg;
				color_render[2] = color_vb;
				glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE,color_render );
			}
		}
		glBegin(GL_TRIANGLES);
		glNormal3f(fnorm_vec[i].x, fnorm_vec[i].y, fnorm_vec[i].z);		
		int count = fap_vec[i].order[0];
		glVertex3f(ver_vec[count].x, ver_vec[count].y, ver_vec[count].z);
		count = fap_vec[i].order[1];
		glVertex3f(ver_vec[count].x, ver_vec[count].y, ver_vec[count].z);
		count = fap_vec[i].order[2];
		glVertex3f(ver_vec[count].x, ver_vec[count].y, ver_vec[count].z);
		glEnd();

	}
	glPopMatrix();
}

void GLWidget::draw_region()
{
	vector<float> color_vec;
	float color_vr = 0.0, color_vg = 0.1, color_vb = 0.2, color_max = -10;

	GLfloat matAmbient[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat matDiffuse[]   = { 1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat matAmbDif[]   = {0.5, 0.5, 0.5, 1.0};
	GLfloat matSpecular[] = {0.2, 0.2, 0.2, 1.0};
	GLfloat shine[] = {5.0};
	GLfloat matEmission[] = {0.3, 0.1, 0.1, 1.0};
	GLfloat color_render[]={0.0/255.0, 191.0/255.0, 255.0/255.0};

	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDiffuse);//散射光
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, matAmbDif);  // 将背景颜色和散射颜色设置成同一颜色
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpecular);//镜面光
	glMaterialfv(GL_FRONT, GL_SHININESS, shine);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, matAmbient);
	glEnable(GL_LIGHTING);//允许光源
	glLightfv(GL_LIGHT0, GL_DIFFUSE, matDiffuse); 

	glPushMatrix();
	int asum = 0;
	//Debug(color_point_set.size());
	for (int i = 0; i < fap_num; i++)
	{
		int a = fap_vec[i].order[0], b = fap_vec[i].order[1], c = fap_vec[i].order[2];
		int sum = 0;
		if(color_point_set.find(a)!=color_point_set.end())
			sum++;
		if(color_point_set.find(b)!=color_point_set.end())
			sum++;
		if(color_point_set.find(c)!=color_point_set.end())
			sum++;
		if(sum >= color_point_num)
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE,color_render );
		else
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, matAmbDif);
		if(sum >= color_point_num)
			asum++;
		glBegin(GL_TRIANGLES);
		glNormal3f(fnorm_vec[i].x, fnorm_vec[i].y, fnorm_vec[i].z);		
		int count = fap_vec[i].order[0];
		glVertex3f(ver_vec[count].x, ver_vec[count].y, ver_vec[count].z);
		count = fap_vec[i].order[1];
		glVertex3f(ver_vec[count].x, ver_vec[count].y, ver_vec[count].z);
		count = fap_vec[i].order[2];
		glVertex3f(ver_vec[count].x, ver_vec[count].y, ver_vec[count].z);
		glEnd();

	}
	//Debug(asum);
	//Debug(color_point_num);
	glPopMatrix();
}

void GLWidget::draw_icp()
{
	vector<float> color_vec;
	float color_vr = 0.0, color_vg = 0.1, color_vb = 0.2, color_max = -10;

	GLfloat matAmbient[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat matDiffuse[]   = { 1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat matAmbDif[]   = {0.5, 0.5, 0.5, 1.0};
	GLfloat matSpecular[] = {0.2, 0.2, 0.2, 1.0};
	GLfloat shine[] = {5.0};
	GLfloat matEmission[] = {0.3, 0.1, 0.1, 1.0};
	GLfloat color_render[]={0.0/255.0, 191.0/255.0, 255.0/255.0};

	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDiffuse);//散射光
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, matAmbDif);  // 将背景颜色和散射颜色设置成同一颜色
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpecular);//镜面光
	glMaterialfv(GL_FRONT, GL_SHININESS, shine);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, matAmbient);
	glEnable(GL_LIGHTING);//允许光源
	glLightfv(GL_LIGHT0, GL_DIFFUSE, matDiffuse); 

	double icp_mat[4][4];
	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
			icp_mat[i][j] = icp_ans[i*4+j];
	}
	/*cout << "icp_mat:\n";
	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
			cout << icp_mat[i][j] << " ";
		cout << endl;
	}*/
	int ifap_num = icp_model.get_fapnum();
	int ivec_num = icp_model.get_vernum();
	vector<Vertex> iver_vec, ner_ver_vec;
	iver_vec = icp_model.get_vertex();
	vector<Facep> ifap_vec = icp_model.get_face();
	vector<FaceNorm> ifnorm_vec = icp_model.get_facen();
	for(int i = 0; i < ivec_num; i++)
	{
		Vertex v = iver_vec[i];
		Vertex new_v;
		float vmat[3], new_vmat[3];
		vmat[0] = v.x, vmat[1] = v.y, vmat[2] = v.z;
		for(int j = 0; j < 3; j++)
		{
			new_vmat[j] = 0;
			for(int k = 0; k < 3; k++)
				new_vmat[j] += icp_mat[j][k]*vmat[k];
			new_vmat[j] += icp_mat[j][3];
		}
		new_v.x = new_vmat[0];
		new_v.y = new_vmat[1];
		new_v.z = new_vmat[2];
		ner_ver_vec.push_back(new_v);
	}

	glPushMatrix();
	for (int i = 0; i < fap_num; i++)
	{
		glBegin(GL_TRIANGLES);
		glNormal3f(fnorm_vec[i].x, fnorm_vec[i].y, fnorm_vec[i].z);		
		int count = fap_vec[i].order[0];
		glVertex3f(ver_vec[count].x, ver_vec[count].y, ver_vec[count].z);
		count = fap_vec[i].order[1];
		glVertex3f(ver_vec[count].x, ver_vec[count].y, ver_vec[count].z);
		count = fap_vec[i].order[2];
		glVertex3f(ver_vec[count].x, ver_vec[count].y, ver_vec[count].z);
		glEnd();

	}

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE,color_render );
	//Debug(ifap_num);
	for (int i = 0; i < ifap_num; i++)
	{
		glBegin(GL_TRIANGLES);
		glNormal3f(ifnorm_vec[i].x, ifnorm_vec[i].y, ifnorm_vec[i].z);		
		int count = ifap_vec[i].order[0];
		glVertex3f(ner_ver_vec[count].x, ner_ver_vec[count].y, ner_ver_vec[count].z);
		count = ifap_vec[i].order[1];
		glVertex3f(ner_ver_vec[count].x, ner_ver_vec[count].y, ner_ver_vec[count].z);
		count = ifap_vec[i].order[2];
		glVertex3f(ner_ver_vec[count].x, ner_ver_vec[count].y, ner_ver_vec[count].z);
		glEnd();

	}
	glPopMatrix();
}
void GLWidget::draw_neighbor()
{
	glScalef(8,8,8);
	glDisable(GL_LIGHTING);//这个可以删？
	glPushMatrix();
	vector<int> nb_faces;
	if(nb_point_faceid!=0)
	{Debug(nb_point_faceid);
		nb_faces = get_point_nbf();
		glPointSize(15.0f);
		glColor3f(1, 0, 0);
		glBegin(GL_POINTS);
		glVertex3f(ver_vec[nb_point_faceid].x, ver_vec[nb_point_faceid].y, ver_vec[nb_point_faceid].z);
		glEnd();
		Debug(nb_faces.size());
		for(int i = 0; i < nb_faces.size(); i++)
		{
			int idx = nb_faces[i];
			glBegin(GL_TRIANGLES);
			glColor3f(0, 0, 1);
			glNormal3f(fnorm_vec[idx].x, fnorm_vec[idx].y, fnorm_vec[idx].z);		
			int count = fap_vec[idx].order[0];
			glVertex3f(ver_vec[count].x, ver_vec[count].y, ver_vec[count].z);
			count = fap_vec[idx].order[1];
			glVertex3f(ver_vec[count].x, ver_vec[count].y, ver_vec[count].z);
			count = fap_vec[idx].order[2];
			glVertex3f(ver_vec[count].x, ver_vec[count].y, ver_vec[count].z);
			glEnd();
			glBegin (GL_LINE_LOOP);
			glColor3f(0.0f, 0.0f, 0.0f);
			glNormal3f(fnorm_vec[idx].x, fnorm_vec[idx].y, fnorm_vec[idx].z);	
			count = fap_vec[idx].order[0];
			glVertex3f(ver_vec[count].x, ver_vec[count].y, ver_vec[count].z);
			count = fap_vec[idx].order[1];
			glVertex3f(ver_vec[count].x, ver_vec[count].y, ver_vec[count].z);
			count = fap_vec[idx].order[2];
			glVertex3f(ver_vec[count].x, ver_vec[count].y, ver_vec[count].z);
			glEnd (); 
		}
		
	}

	if(nb_faceid!=0)
	{
		nb_faces = get_face_nbf();
		int idx = nb_faceid;
		//Debug(idx);
		glBegin(GL_TRIANGLES);
		glColor3f(1, 0, 1);
		glNormal3f(fnorm_vec[idx].x, fnorm_vec[idx].y, fnorm_vec[idx].z);		
		int count = fap_vec[idx].order[0];
		glVertex3f(ver_vec[count].x, ver_vec[count].y, ver_vec[count].z);
		count = fap_vec[idx].order[1];
		glVertex3f(ver_vec[count].x, ver_vec[count].y, ver_vec[count].z);
		count = fap_vec[idx].order[2];
		glVertex3f(ver_vec[count].x, ver_vec[count].y, ver_vec[count].z);
		glEnd();
		glBegin (GL_LINE_LOOP);
		glColor3f(0.0f, 0.0f, 0.0f);
		glNormal3f(fnorm_vec[idx].x, fnorm_vec[idx].y, fnorm_vec[idx].z);	
		count = fap_vec[idx].order[0];
		glVertex3f(ver_vec[count].x, ver_vec[count].y, ver_vec[count].z);
		count = fap_vec[idx].order[1];
		glVertex3f(ver_vec[count].x, ver_vec[count].y, ver_vec[count].z);
		count = fap_vec[idx].order[2];
		glVertex3f(ver_vec[count].x, ver_vec[count].y, ver_vec[count].z);
		glEnd (); 
		for(int i = 0; i < nb_faces.size(); i++)
		{
			idx = nb_faces[i];
			//Debug(idx);
			glBegin(GL_TRIANGLES);
			glColor3f(0, 0, 1);
			glNormal3f(fnorm_vec[idx].x, fnorm_vec[idx].y, fnorm_vec[idx].z);		
			count = fap_vec[idx].order[0];
			glVertex3f(ver_vec[count].x, ver_vec[count].y, ver_vec[count].z);
			count = fap_vec[idx].order[1];
			glVertex3f(ver_vec[count].x, ver_vec[count].y, ver_vec[count].z);
			count = fap_vec[idx].order[2];
			glVertex3f(ver_vec[count].x, ver_vec[count].y, ver_vec[count].z);
			glEnd();
			glBegin (GL_LINE_LOOP);
			glColor3f(0.0f, 0.0f, 0.0f);
			glNormal3f(fnorm_vec[idx].x, fnorm_vec[idx].y, fnorm_vec[idx].z);	
			count = fap_vec[idx].order[0];
			glVertex3f(ver_vec[count].x, ver_vec[count].y, ver_vec[count].z);
			count = fap_vec[idx].order[1];
			glVertex3f(ver_vec[count].x, ver_vec[count].y, ver_vec[count].z);
			count = fap_vec[idx].order[2];
			glVertex3f(ver_vec[count].x, ver_vec[count].y, ver_vec[count].z);
			glEnd (); 
		}

	}

	
	for (int i = 0; i < fap_num; i++)
	{
		bool flag = false;
		for(int s = 0; s < nb_faces.size(); s++)
			if(i==nb_faces[s])
			{
				flag = true;
				break;
			}
		if(flag)
			continue;
		if(nb_faceid!=0&&i==nb_faceid)
			continue;
		glBegin(GL_TRIANGLES);
		glColor3f(0, 1, 0);
		glNormal3f(fnorm_vec[i].x, fnorm_vec[i].y, fnorm_vec[i].z);		
		int count = fap_vec[i].order[0];
		glVertex3f(ver_vec[count].x, ver_vec[count].y, ver_vec[count].z);
		count = fap_vec[i].order[1];
		glVertex3f(ver_vec[count].x, ver_vec[count].y, ver_vec[count].z);
		count = fap_vec[i].order[2];
		glVertex3f(ver_vec[count].x, ver_vec[count].y, ver_vec[count].z);
		glEnd();

		glBegin (GL_LINE_LOOP);
		glColor3f(0.0f, 0.0f, 0.0f);
		glNormal3f(fnorm_vec[i].x, fnorm_vec[i].y, fnorm_vec[i].z);	
		count = fap_vec[i].order[0];
		glVertex3f(ver_vec[count].x, ver_vec[count].y, ver_vec[count].z);
		count = fap_vec[i].order[1];
		glVertex3f(ver_vec[count].x, ver_vec[count].y, ver_vec[count].z);
		count = fap_vec[i].order[2];
		glVertex3f(ver_vec[count].x, ver_vec[count].y, ver_vec[count].z);
		glEnd (); 
	}
	if(normal_faceid!=0)
	{
		Facep tmp = fap_vec[normal_faceid];
		Vertex a = ver_vec[tmp.order[0]], b = ver_vec[tmp.order[1]], c = ver_vec[tmp.order[2]];
		float cx = (a.x+b.x+c.x)/3.0, cy = (a.y+b.y+c.y)/3.0, cz = (a.z+b.z+c.z)/3.0;
		FaceNorm tmpn = fnorm_vec[normal_faceid];
		float down = sqrt(tmpn.x*tmpn.x+tmpn.y*tmpn.y+tmpn.z*tmpn.z);
		tmpn.x = tmpn.x/down, tmpn.y = tmpn.y/down, tmpn.z = tmpn.z/down;
		glColor3f(1.0f,0.0f,0.0f);
		//glLineWidth(4.0f);
		glBegin(GL_LINES);
		glVertex3f(cx,cy,cz);
		glVertex3f(cx+tmpn.x*0.02,cy+tmpn.y*0.02,cz+tmpn.z*0.02);
		glEnd();

	}

	if(nb_pointid!=0)
	{
		vector<int> nb_points = get_point_nbp();
		glPointSize(15.0f);
		glColor3f(1, 0, 0);
		glBegin(GL_POINTS);
		glVertex3f(ver_vec[nb_pointid].x, ver_vec[nb_pointid].y, ver_vec[nb_pointid].z);
		glEnd();
		glPointSize(10.0f);
		glColor3f(0, 0, 1);
		for(int i = 0; i < nb_points.size(); i++)
		{
			int idx = nb_points[i];
			glBegin(GL_POINTS);
			glVertex3f(ver_vec[idx].x, ver_vec[idx].y, ver_vec[idx].z);
			glEnd();
		}
	}
	

	glPopMatrix();
}



//该程序是设置opengl场景透视图，程序中至少被执行一次(程序启动时).  
void GLWidget::resizeGL(int width, int height)
{
	// Update the window's width and height
	window_width = width;
	window_height = height;

	// Reset the viewport
	glViewport(0, 0, window_width, window_height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (float)window_width / (float)window_height, 0.01f, 200.0f);

	//glutPostRedisplay();
}

void GLWidget::keyPressEvent(QKeyEvent *e)
{
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
	 //lastPos = event->pos();
	OldX = event->x();
	OldY = event->y();
}
void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
	int tx = event->x();
	int ty = event->y();
	

	if(event->buttons() & Qt::LeftButton) {
		float fOldX = 2.0f*OldX / (float)window_width - 1.0f;
		float fOldY = -2.0f*OldY / (float)window_height + 1.0f;
		float fNewX = 2.0f*tx / (float)window_width - 1.0f;
		float fNewY = -2.0f*ty / (float)window_height + 1.0f;

		double pMatrix[16];
		trackball_opengl_matrix(pMatrix, fOldX, fOldY, fNewX, fNewY);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glLoadMatrixd(pMatrix);
		glMultMatrixd(pModelViewMatrix);
		glGetDoublev(GL_MODELVIEW_MATRIX, pModelViewMatrix);

		OldX = tx;
		OldY = ty;
		updateGL();
		//glutPostRedisplay();
	}

}

void GLWidget::wheelEvent(QWheelEvent *event)
{
	int numDegrees = event->delta() / 8;
	int numSteps = numDegrees / 15; // Usually numSteps here is declared outside the function and is used for tracking the number of steps
	// the wheel has been rolled. However here we use it only to determine the direction of the scroll of the wheel

	// TODO After the settings dialog is added make the zoom factor changable
	if(numSteps == 1)
		zoom += .02;
	else if (numSteps == -1)
		zoom -= .02;
	if(zoom < 0)
		zoom = 0;
	updateGL();
}

void GLWidget::set_draw_type(draw_type new_draw)
{
	my_draw = new_draw;
	updateGL();
}

void GLWidget::set_color_type(color_type new_color)
{
	my_color = new_color;
	if(new_color != color_type::DEFAULT_COLOR)
	{
		my_model.read_color();
	}
	updateGL();
}
void GLWidget::set_neighbor_pointid(int point_id)
{
	if(point_id>ver_num)
		return ;
	nb_pointid = point_id;
}

void GLWidget::set_neighbor_point_faceid(int point_id)
{
	if(point_id>ver_num)
		return ;
	nb_point_faceid = point_id;
}

void GLWidget::set_neighbor_faceid(int face_id)
{
	if(face_id > fap_num)
		return ;
	nb_faceid = face_id;
}

void GLWidget::set_normal_faceid(int face_id)
{
	if(face_id > fap_num)
		return ;
	normal_faceid = face_id;
}

void GLWidget::clear_neighbors()
{
	nb_pointid = 0;
	nb_point_faceid = 0;
	nb_faceid = 0;
	normal_faceid = 0;
	delete [] face_flag;
	face_flag = new bool[fap_num+10];
	memset(face_flag, 0, sizeof(face_flag));
}

vector<int> GLWidget::get_point_nbp()
{
	vector<int> ans;
	for(int i = 0; i < fap_num; i++)
	{
		Facep tmpf = fap_vec[i];
		if(tmpf.order[0]==nb_pointid)
		{
			ans.push_back(tmpf.order[1]);
			ans.push_back(tmpf.order[2]);
		}
		else if(tmpf.order[1]==nb_pointid)
		{
			ans.push_back(tmpf.order[0]);
			ans.push_back(tmpf.order[2]);
		}
		else if(tmpf.order[2]==nb_pointid)
		{
			ans.push_back(tmpf.order[1]);
			ans.push_back(tmpf.order[0]);
		}
	}
	return ans;
}

vector<int> GLWidget::get_point_nbf()
{
	vector<int> ans;
	for(int i = 0; i < fap_num; i++)
	{
		Facep tmpf = fap_vec[i];
		if(tmpf.order[0]==nb_point_faceid||tmpf.order[1]==nb_point_faceid||tmpf.order[2]==nb_point_faceid)
		{
			ans.push_back(i);
			face_flag[i] = true;
		}
	}
	return ans;
}

vector<int> GLWidget::get_face_nbf()
{
	vector<int> ans;
	Facep f = fap_vec[nb_faceid];
	int a = f.order[0], b = f.order[1], c = f.order[2];
	face_flag[nb_faceid] = true;
	for(int i = 0; i < fap_num; i++)
	{
		Facep tmpf = fap_vec[i];
		int d = tmpf.order[0], e = tmpf.order[1], f = tmpf.order[2];
		int sum = 0;
		if(a==d||a==e||a==f)
			sum++;
		if(b==d||b==e||b==f)
			sum++;
		if(c==d||c==e||c==f)
			sum++;
		if(sum==2)
		{
				ans.push_back(i);
				face_flag[i] = true;
		}
	}
	return ans;
}

vector<double> GLWidget::icp()
{
	ICP my_icp(icp_model, my_model);
	my_icp.run();
	icp_ans = my_icp.result();
	this->set_draw_type(draw_type::ICP_TYPE);
	return icp_ans;
}

void GLWidget::set_color_point_num(int new_num)
{
	color_point_num = new_num;
}

