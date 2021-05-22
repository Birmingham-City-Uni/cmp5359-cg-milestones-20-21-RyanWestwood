//#pragma once
//#include <vector>
//#include "geometry.h"
//#include <map>

//class Model {
//private:
//	std::vector<Vec3f> verts_;
//	std::vector<std::vector<int>> vnorms_;
//	std::vector<std::vector<int>> uvs_;
//	std::vector<std::vector<int>> faces_;
//public:
//	Model(const char* filename);
//	~Model();
//	int nverts();
//	int nfaces();
//	Vec3f vert(int i);
//	Vec3f vn(int i);
//	std::vector<int> face(int idx);
//	std::vector<int> vnorms(int idx);
//};

#pragma once

#include <vector>
#include "geometry.h"
#include <map>

struct Face {
	std::vector<int> vertexIndex, textureCoordsIndex, vertexNormalsIndex;

	friend std::ostream& operator<<(std::ostream& os, const Face& f);
};

class Model {
private:
	std::vector<Vec3f> verts_;
	std::vector<Vec3f> vertNorms_;
	std::vector<Vec3f> texCoords_;
	std::vector<Face> tris_;

	void LoadModel(std::string filename);

public:
	Model(std::string filename);
	~Model();

	int nverts();
	int nfaces();

	Vec3f vert(int i);
	Vec3f vertNorm(int i);
	Vec3f textureCoord(int i);

	Face& triangle(int idx);
	std::vector<Face>& faces();
};
