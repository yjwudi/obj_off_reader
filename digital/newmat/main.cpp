/*
 * main.cpp
 *
 *  Created on: Dec 24, 2008
 *      Author: a
 */

#include "ICP.h"


int main()
{
	ICP myicp(5500,0.001,50);
	myicp.readfile("62.off","63.off");
	myicp.run();
	myicp.writefile("out.obj");
	return 0;
}
