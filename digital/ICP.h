/*
 * ICP.h
 *
 *  Created on: Dec 24, 2008
 *      Author: a
 */

#ifndef ICP_H_
#define ICP_H_

#include <vector>
#include <string>
#include <cstdio>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <cstring>
#include <cmath>
#include "MeshModel.h"


using namespace std;
#define Debug(x) cout<<#x<<"="<<(x)<<endl;


class ICP
{
public:
	ICP(MeshModel modelP, MeshModel modelQ);
	~ICP();
	void run();
	void writefile(string name);
	vector<double> result();

private:
	void initransmat(); //init translate and rotate matrix
	void sample(); //sample control points
	double closest(); // find corresponding points and return error
	void getcenter(); //get center from two control points
	void rmcontcenter(); //remove center from two control points
	void transform(); //get transform (rotate) matrix
	void uprotate(); //change quaternious number to matrix and update whole rotate matrix
	void uptranslate(); // update whole translate matrix
	void updata();  // update control points coordinate
	void applyall();
	void calc_result();

private:
	double distance(Vertex a,Vertex b);
	void printTT();
	void printTR();

private:
	int cono; // control points number
	int iterate; //iterate number
	double threshold; //stop threshold
	vector<Vertex> VarrP; //original points
	vector<Vertex> VarrQ;
	Vertex meanP; // control points center
	Vertex meanQ;
	Vertex *contP; //control points in P
	Vertex *contQ;
	Vertex *rmcoP; //control points after removing center
	Vertex *rmcoQ;
	int *index;	//use when sampling control points and in finding corresponding points index
	double TT[3]; //translate
	double TR[3][3]; //rotate
	double Rw[3][3];//step rotate
	double Tw[3]; //step translate
	double quad[4];
	double err;
	vector<double> ans;



};

#endif /* ICP_H_ */
