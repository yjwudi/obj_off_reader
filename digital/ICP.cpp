/*
 * ICP.cpp
 *
 *  Created on: Dec 24, 2008
 *      Author: a
 */

#include <iostream>
#include <sstream>
#include <fstream>
#include <cassert>
#include <math.h>
#include <time.h>
#include "newmat/newmat.h"
#include "newmat/newmatap.h"
#include "ICP.h"

using namespace std;
//using namespace NEWMAT;

ICP::ICP(MeshModel modelP, MeshModel modelQ)//(int controlnum,double thre,int iter)
{
	cono = min(modelP.get_vernum(), modelQ.get_vernum());
	threshold = 0.001;
	iterate = 20;
	
	VarrP = modelP.get_vertex();
	VarrQ = modelQ.get_vertex();

	contP = new Vertex[cono];
	assert(contP!=NULL);
	contQ = new Vertex[cono];
	assert(contQ!=NULL);
	rmcoP = new Vertex[cono];
	assert(rmcoP!=NULL);
	rmcoQ = new Vertex[cono];
	assert(rmcoQ!=NULL);
	index = new int[cono];
	assert(index!=NULL);
}

ICP::~ICP()
{
	delete [] contP;
	delete [] contQ;
	delete [] rmcoP;
	delete [] rmcoQ;
	delete [] index;
}

void ICP::run()
{
    cout << "run\n";
	initransmat();
	sample();
	//
	err = closest();
	cout<<"initial error = "<<err<<endl;
	//
	for(int i=0;i<iterate;i++)
	{
		getcenter();
		rmcontcenter();
		transform();
		uprotate();
		uptranslate();
		updata();
		double newerr = closest();
		cout<<"iterate times = "<<i<<endl;
		cout<<"error = "<<newerr<<endl;
		double delta = fabs(err-newerr)/cono;
		cout<<"delta = "<<delta<<endl;
		if(delta<threshold)
				break;
		err = newerr;

	}
	printTR();
	printTT();
	calc_result();
	//applyall();
}

void ICP::initransmat()
{
	cout<<"initial translate and rotate matrix"<<endl;
	//
	for(int i=0;i<3;i++)
		TT[i] = 0.0;
	for(int i=0;i<3;i++)
	{
		for(int j=0;j<3;j++)
		{
			if(i!=j)
				TR[i][j]=0.0;
			else
				TR[i][j]=1.0;
		}
	}
}

void ICP::sample()
{
	cout<<"sample control points from P"<<endl;
	//
	int N = VarrP.size();
	Debug(N);
	bool *flag = new bool[N];
	assert(flag!=NULL);
	for(int i=0;i<N;i++)
		flag[i] = false;
	//sample control points index
	srand((unsigned)time(NULL));
	for(int i=0;i<cono;i++)
	{
		while(true)
		{
			int sam =rand()%N;
			if(!flag[sam])
			{
				index[i] = sam;
				flag[sam] = true;
				break;
			}
		}
	}
	Debug(cono);
	//
	//cout<<"store control points into contP"<<endl;
	//
	for(int i=0;i<cono;i++)
	{
		Vertex v = VarrP[index[i]];
		contP[i].x = v.x;
		contP[i].y = v.y;
		contP[i].z = v.z;
	}
	Debug(cono);

	delete [] flag;
}

double ICP::closest()
{
	cout<<"find closest points and error"<<endl;
	//
	double error = 0.0;
	for(int i=0;i<cono;i++)
	{
		double maxdist = 100.0;
		index[i] = 0;
		for(unsigned int j=0;j<VarrQ.size();j++)
		{
			double dist = distance(contP[i],VarrQ[j]);
			if(dist < maxdist)
			{
				maxdist = dist;
				index[i] = j;
			}
		}
		Vertex v = VarrQ[index[i]];
		contQ[i].x = v.x;
		contQ[i].y = v.y;
		contQ[i].z = v.z;
		/*for(int j=0;j<3;j++)
		{
			contQ[i].coord[j] = v.coord[j];
		}*/
		error += maxdist;
	}
	return error;
}

void ICP::getcenter()
{
	//cout<<"get center from two clouds of control points"<<endl;
	//
	/*for(int i=0;i<3;i++)
		meanP.coord[i] = 0.0 ;*/
	meanP.x = 0.0 ;
	meanP.y = 0.0 ;
	meanP.z = 0.0 ;
	for(int i=0;i<cono;i++)
	{
		meanP.x = contP[i].x;
		meanP.y = contP[i].y;
		meanP.z = contP[i].z;
		/*for(int j=0;j<3;j++)
			meanP.coord[j] += contP[i].coord[j];*/
	}
	meanP.x = meanP.x/cono;
	meanP.y = meanP.y/cono;
	meanP.z = meanP.z/cono;
	/*for(int i=0;i<3;i++)
		meanP.coord[i] = meanP.coord[i]/cono;*/
	meanQ.x = 0.0 ;
	meanQ.y = 0.0 ;
	meanQ.z = 0.0 ;
	/*for(int i=0;i<3;i++)
		meanQ.coord[i] = 0.0;*/
	for(int i=0;i<cono;i++)
	{
		meanQ.x = contQ[i].x;
		meanQ.y = contQ[i].y;
		meanQ.z = contQ[i].z;
		/*for(int j=0;j<3;j++)
			meanQ.coord[j] += contQ[i].coord[j];*/
	}
	meanQ.x = meanQ.x/cono;
	meanQ.y = meanQ.y/cono;
	meanQ.z = meanQ.z/cono;
	/*for(int i=0;i<3;i++)
		meanQ.coord[i] = meanQ.coord[i]/cono;*/
}

void ICP::rmcontcenter()
{
	cout<<"move clouds of control points to their correspond points center"<<endl;
	//
	for(int i=0;i<cono;i++)
	{
		rmcoP[i].x = contP[i].x - meanP.x;
		rmcoP[i].y = contP[i].y - meanP.y;
		rmcoP[i].z = contP[i].z - meanP.z;
		/*for(int j=0;j<3;j++)
		{
			rmcoP[i].coord[j] = contP[i].coord[j] - meanP.coord[j];
		}*/
	}
	//
	for(int i=0;i<cono;i++)
	{
		rmcoQ[i].x = contQ[i].x - meanQ.x;
		rmcoQ[i].y = contQ[i].y - meanQ.y;
		rmcoQ[i].z = contQ[i].z - meanQ.z;
		/*for(int j=0;j<3;j++)
		{
			rmcoQ[i].coord[j] = contQ[i].coord[j] - meanQ.coord[j];
		}*/
	}
}

void ICP::transform()
{
	cout<<"get transform matrix"<<endl;
	//
	Matrix B(4,4);
	B = 0;
	double u[3]; //di+di'
	double d[3]; //di-di'
	for(int i=0;i<cono;i++)
	{
		/*for(int j=0;j<3;j++)
		{
			u[j] = rmcoP[i].coord[j]+rmcoQ[i].coord[j];
			d[j] = rmcoP[i].coord[j]-rmcoQ[i].coord[j];
		}*/
		u[0] = rmcoP[i].x+rmcoQ[i].x;
		d[0] = rmcoP[i].x-rmcoQ[i].x;
		u[1] = rmcoP[i].y+rmcoQ[i].y;
		d[1] = rmcoP[i].y-rmcoQ[i].y;
		u[2] = rmcoP[i].z+rmcoQ[i].z;
		d[2] = rmcoP[i].z-rmcoQ[i].z;
		double uM[16] = {0,    -d[0],  -d[1],  -d[2],
						 d[0], 0,      -u[2],  -u[1],
						 d[1], -u[2],  0,      u[0],
						 d[2], u[1],   -u[0],  0   };
		/*
		double uM[16] = {0,    -u[2] , -u[1], d[0],
						-u[2], 0    ,  u[0], d[1],
						u[1] , -u[0],   0  , d[2],
						-d[0], -d[1], -d[2],  0 };*/
		Matrix Ai(4,4) ;
		Ai << uM ;
		B += Ai*Ai.t();
	}
	//
	Matrix U;
	Matrix V;
	DiagonalMatrix D;
	SVD(B,D,U,V);
	//

	for(int i=0;i<4;i++)
	{
		quad[i] = V.element(i,3);
	}
	//
	B.Release();
	U.Release();
	V.Release();
	D.Release();
}

void ICP::uprotate()
{
	//cout<<"change quadternious to 3*3 rotate matrix"<<endl;
	//
	Rw[0][0] = quad[0]*quad[0]+quad[1]*quad[1]-quad[2]*quad[2]-quad[3]*quad[3];
	Rw[0][1] = 2*(-quad[0]*quad[3]+quad[1]*quad[2]);
	Rw[0][2] = 2*(quad[0]*quad[2]+quad[1]*quad[3]);
	Rw[1][0] = 2*(quad[0]*quad[3]+quad[1]*quad[2]);
	Rw[1][1] = quad[0]*quad[0]-quad[1]*quad[1]+quad[2]*quad[2]-quad[3]*quad[3];
	Rw[1][2] = 2*(-quad[0]*quad[1]+quad[2]*quad[3]);
	Rw[2][0] = 2*(-quad[0]*quad[2]+quad[1]*quad[3]);
	Rw[2][1] = 2*(quad[0]*quad[1]+quad[2]*quad[3]);
	Rw[2][2] = quad[0]*quad[0]-quad[1]*quad[1]-quad[2]*quad[2]+quad[3]*quad[3];
	//
	//Rn+1 = R * Rn
	double tmp[3][3];
	for(int i=0;i<3;i++)
		for(int j=0;j<3;j++)
			tmp[i][j] = 0.0;
	for(int i=0;i<3;i++)
	{
		for(int j=0;j<3;j++)
		{
			for(int k=0;k<3;k++)
			{
				tmp[i][j] += Rw[i][k]*TR[k][j];
			}
		}
	}
	for(int i=0;i<3;i++)
		for(int j=0;j<3;j++)
			TR[i][j] = tmp[i][j];
}

void ICP::uptranslate()
{
	//Tw = P' -Rw*P
	double tmp[3] = {0,0,0};
	for(int i=0;i<3;i++)
	{
		/*for(int j=0;j<3;j++)
		{
			tmp[i] += Rw[i][j]*meanP.coord[j];
		}*/
		tmp[i] += Rw[i][0]*meanP.x;
		tmp[i] += Rw[i][1]*meanP.y;
		tmp[i] += Rw[i][2]*meanP.z;
	}
	Tw[0] = meanQ.x - tmp[0];
	Tw[1] = meanQ.y - tmp[1];
	Tw[2] = meanQ.z - tmp[2];
	/*for(int i=0;i<3;i++)
	{
	Tw[i] = meanQ.coord[i] - tmp[i];
	}*/
	//Tn+1 = R*Tn + Tw
	double temp[3] = {0,0,0};
	for(int i=0;i<3;i++)
	{
		for(int j=0;j<3;j++)
		{
			temp[i] += Rw[i][j]*TT[j];
		}
	}
	for(int i=0;i<3;i++)
	{
		TT[i] = temp[i] + Tw[i];
	}

}

void ICP::updata()
{
	//cout<<"update control points in P"<<endl;
	//
	//rotate + translate
	for(int i=0;i<cono;i++)
	{
		double tmp[3] = {0,0,0};
		for(int j=0;j<3;j++)
		{
			/*for(int k=0;k<3;k++)
			{
				tmp[j] += Rw[j][k]*contP[i].coord[k];
			}*/
			tmp[j] += Rw[j][0]*contP[i].x;
			tmp[j] += Rw[j][1]*contP[i].y;
			tmp[j] += Rw[j][2]*contP[i].z;
		}
		contP[i].x = tmp[0] + Tw[0];
		contP[i].y = tmp[1] + Tw[1];
		contP[i].z = tmp[2] + Tw[2];
		/*for(int j=0;j<3;j++)
			contP[i].coord[j] = tmp[j] + Tw[j];*/

	}
}

void ICP::applyall()
{
	cout<<"transform to all data in P"<<endl;
	// make rotate
	for(vector<Vertex>::iterator p=VarrP.begin();p!=VarrP.end();p++)
	{
		Vertex v = *p;
		double tmp[3] = {0,0,0};
		for(int i=0;i<3;i++)
		{
			tmp[i] += TR[i][0]*v.x;
			tmp[i] += TR[i][1]*v.y;
			tmp[i] += TR[i][2]*v.z;
			/*for(int k=0;k<3;k++)
			{
				tmp[i] += TR[i][k]*v.coord[k];
			}*/
		}
		v.x = tmp[0] + TT[0];
		v.y = tmp[1] + TT[1];
		v.z = tmp[2] + TT[2];
		/*for(int i=0;i<3;i++)
		{
		v.coord[i] = tmp[i] + TT[i];
		}*/
		*p = v;
	}
	//
}

void ICP::writefile(string name)
{
	cout<<"output clouds of points P after transform"<<endl;
	//
	ofstream outobj;
	outobj.open(name.c_str());
	outobj<<"# Geomagic Studio"<<endl;
	int num = 1;
	for(vector<Vertex>::const_iterator p=VarrP.begin();p!=VarrP.end();p++)
	{
		Vertex v;
		v = *p;
		outobj<<"v "<<v.x<<" "<<v.y<<" "<<v.z<<endl;
		//outobj<<"v "<<v.coord[0]<<" "<<v.coord[1]<<" "<<v.coord[2]<<endl;
		outobj<<"p "<<num++<<endl;
	}
	//
	outobj.close();
}

double ICP::distance(Vertex a,Vertex b)
{
	double dist = 0.0;
	dist += (a.x-b.x)*(a.x-b.x);
	dist += (a.y-b.y)*(a.y-b.y);
	dist += (a.z-b.z)*(a.z-b.z);
	/*for(int i=0;i<3;i++)
	{
		dist += (a.coord[i]-b.coord[i])*(a.coord[i]-b.coord[i]);
	}*/
	return dist;
}

void ICP::printTT()
{
	cout<<"Translate Matrix = "<<endl;
	for(int i=0;i<3;i++)
	{
		cout<<TT[i]<<"  ";
	}
	cout<<endl;
}

void ICP::printTR()
{
	cout<<"Rotate Matrix = "<<endl;
	for(int i=0;i<3;i++)
	{
		for(int j=0;j<3;j++)
		{
			cout<<TR[i][j]<<" ";
		}
		cout<<endl;
	}
}

void ICP::calc_result()
{
	ans.clear();
	for(int i = 0; i < 3; i++)
	{
		for(int j = 0; j < 3; j++)
			ans.push_back(TR[i][j]);
		ans.push_back(TT[i]);
	}
	for(int i = 0; i < 3; i++)
		ans.push_back(0.0);
	ans.push_back(1.0);
	ans.push_back(err);
}

vector<double> ICP::result()
{
	return ans;
}
