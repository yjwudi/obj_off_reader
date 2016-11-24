#include "MeshModel.h"


MeshModel::MeshModel()
{
}


MeshModel::~MeshModel()
{
}
 
MeshModel::MeshModel(string file)
{
	file_name = file;
	read_color_flag = false;
	
	if (file[file.length() - 1] == 'f')
		read_off();
	else
		read_obj();
	calc_norm();
}

void MeshModel::read_off()
{
	ifstream in_file(file_name);
	streambuf *strmin_buf = cin.rdbuf();
	cin.rdbuf(in_file.rdbuf());

	string rab;
	int i, j;
	Vertex tmp_v;
	Facep tmp_f;
	cin >> rab;
	cin >> ver_num >> fap_num >> edge_num;
	Debug(ver_num);
	for (i = 0; i < ver_num; i++)
	{
		cin >> tmp_v.x >> tmp_v.y >> tmp_v.z;
		ver_vec.push_back(tmp_v);
	}
	for (i = 0; i < fap_num; i++)
	{
		cin >> tmp_f.num >> tmp_f.order[0] >> tmp_f.order[1] >> tmp_f.order[2];
		fap_vec.push_back(tmp_f);
	}

	cin.rdbuf(strmin_buf);
}

void MeshModel::read_obj()
{
	ifstream in_file(file_name);
	streambuf *strmin_buf = cin.rdbuf();
	cin.rdbuf(in_file.rdbuf());

	string rab;
	int i, j;
	Vertex tmp_v;
	Facep tmp_f;
	ver_num = 0;
	fap_num = 0;
	edge_num = 0;
	while (cin >> rab)
	{
		if (rab[0] == '#')
		{
			getline(cin, rab);
			cout << rab << endl;
			continue;
		}
		else if (rab[0] == 'v')
		{
			cin >> tmp_v.x >> tmp_v.y >> tmp_v.z;
			ver_vec.push_back(tmp_v);
		}
		else if (rab[0] == 'f')
		{
			tmp_f.num = 3;
			cin >> tmp_f.order[0] >> tmp_f.order[1] >> tmp_f.order[2];
			edge_num += 3;
			tmp_f.order[0]--;
			tmp_f.order[1]--;
			tmp_f.order[2]--;
			fap_vec.push_back(tmp_f);
		}
	}
	ver_num = ver_vec.size();
	fap_num = fap_vec.size();
	edge_num = edge_num/2;
	Debug(ver_num);
	Debug(edge_num);
	cin.rdbuf(strmin_buf);
}


FaceNorm MeshModel::_glmCross(GLfloat* u, GLfloat* v)
{
	FaceNorm n;
	int X = 0, Y = 1, Z = 2;
	n.x = u[Y] * v[Z] - u[Z] * v[Y];
	n.y = u[Z] * v[X] - u[X] * v[Z];
	n.z = u[X] * v[Y] - u[Y] * v[X];
	return n;
}


FaceNorm MeshModel::_glmNormalize(FaceNorm n)
{
	FaceNorm nn = n;
	GLfloat l;
	/* normalize */
	l = (GLfloat)sqrt(n.x * n.x + n.y * n.y + n.z * n.z);
	nn.x /= l;
	nn.y /= l;
	nn.z /= l;
	return nn;
}
void MeshModel::calc_norm()
{
	int  i;
	GLfloat u[3];
	GLfloat v[3];
	FaceNorm tmp_norm;
	Vertex v1, v2, v3;
	Facep face;
	for (i = 0; i < fap_num; i++)
	{
		face = fap_vec[i];
		v1 = ver_vec[face.order[0]];
		v2 = ver_vec[face.order[1]];
		v3 = ver_vec[face.order[2]];
		u[0] = v2.x - v1.x;
		u[1] = v2.y - v1.y;
		u[2] = v2.z - v1.z;
		v[0] = v3.x - v1.x;
		v[1] = v3.y - v1.y;
		v[2] = v3.z - v1.z;
		tmp_norm = _glmCross(u, v);
		tmp_norm = _glmNormalize(tmp_norm);
		fnorm_vec.push_back(tmp_norm);
	}
	
}

vector<FaceNorm> MeshModel::get_facen()
{
	return fnorm_vec;
}

vector<Vertex> MeshModel::get_vertex()
{
	return ver_vec;
}
vector<Facep> MeshModel::get_face()
{
	return fap_vec;
}
int MeshModel::get_vernum()
{
	return ver_num;
}
int MeshModel::get_fapnum()
{
	return fap_num;
}
int MeshModel::get_edgenum()
{
	return edge_num;
}
vector<float> MeshModel::get_color()
{
	return color_vec;
}

void MeshModel::read_color()
{
	if(read_color_flag)
		return;
	string cfile_name = file_name.substr(0, file_name.length()-4);
	cfile_name += "_color.txt";
	Debug(cfile_name);
	FILE *fp = fopen(cfile_name.c_str(), "r");
	if(fp==NULL)
	{
		cout << "NULL\n";
		return ;
	}
	fclose(fp);
	ifstream in_file(cfile_name);
	streambuf *strmin_buf = cin.rdbuf();
	cin.rdbuf(in_file.rdbuf());
	float colorv;
	while(cin >> colorv)
	{
		color_vec.push_back(colorv);
	}

	cin.rdbuf(strmin_buf);
	read_color_flag = true;
}


