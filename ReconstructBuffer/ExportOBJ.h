#pragma once
#include <GL\glew.h>
#include <glut.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>

using namespace std;
void exportOBJ(vector<GLfloat> vertices, vector<GLuint> indices, int offset1, int offset2, string filename);

void exportOBJ(vector<GLfloat> vertices, vector<GLuint> indices, int offset1, int offset2, string filename)
{
	ofstream objFile(filename + ".obj");

	if (!objFile)
		return;

	//objFile << "g default" << endl;
	objFile.setf(ios::fixed);
	//写入vertex position
	for (int i = 0; i < offset1; i++) {
		if (i % 3 == 0) {
			objFile <<  "v ";
		}
		objFile << setprecision(5) << vertices[i];
		if (i % 3 == 2) {
			objFile << endl;
		}
		else
			objFile << " ";
	}

	//写入texture coordinate
	for (int i = offset2; i < vertices.size()-1; i++) {
		if ((i - offset2) % 2 == 0) {
			objFile << "vt ";
		}
		objFile << setprecision(5) << vertices[i];
		if ((i-offset2) % 2 == 1) {
			objFile << endl;
		}
		else
			objFile << " ";
	}

	//写入法向量
	for (int i = 0; i < offset1 / 3; i++)
	{
		objFile << "vn 0.000000 0.000000 1.000000" << endl;
	}

	//写入面
	for (int i = 0; i < indices.size() / 3; i++)
	{
		objFile << "f ";
		objFile << indices[3*i] << "/" << indices[3*i] << "/" << indices[3*i] << " ";
		objFile << indices[3*i + 1] << "/" << indices[3*i + 1] << "/" << indices[3*i + 1] << " ";
		objFile << indices[3*i + 2] << "/" << indices[3*i + 2] << "/" << indices[3*i + 2] << " ";
		objFile << endl;
	}

	objFile.close();
}