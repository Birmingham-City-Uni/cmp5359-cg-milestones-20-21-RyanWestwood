#pragma once
#include <vector>
#include <map>
#include "geometry.h"
#include "hittable_list.h"
#include "Triangle.h"

struct Face {
	std::vector<int> vertexIndex;
	std::vector<int> textureCoordsIndex;
	std::vector<int> vertexNormalsIndex;

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
	~Model() = default;

	int nverts();
	int nfaces();

	Vec3f vert(int i);
	Vec3f vertNorm(int i);
	Vec3f textureCoord(int i);

	Face& triangle(int idx);
	std::vector<Face>& faces();

	void AddToWorld(Hittable_List& world, Vec3f transform, const std::shared_ptr<Material>& mat, int index);
};
