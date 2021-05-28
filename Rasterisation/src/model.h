#pragma once

#include <vector>
#include "geometry.h"
#include <map>

struct Face {
	std::vector<int> vertexIndex, textureCoordsIndex, vertexNormalsIndex;
	std::string mat;

	friend std::ostream& operator<<(std::ostream& os, const Face& f);
};

//	https://www.loc.gov/preservation/digital/formats/fdd/fdd000508.shtml#notes
struct Material {
	std::string name;
	Vec3f Ka;
	Vec3f Kd;
	Vec3f Ks;
	float Ns;
	float Ni;
	float d;
	int illum;
	std::string map_Kd;

	friend std::ostream& operator<<(std::ostream& os, const Material& m);
	Material() = default;
	Material(std::string exc) : Kd(1.F, 1.F, 1.F) {};
};

class Model {
private:
	std::vector<Vec3f> verts_;
	std::vector<Vec3f> vertNorms_;
	std::vector<Vec2f> texCoords_;
	std::vector<Face> tris_;

	std::map<std::string, Material> materials_;

	void LoadModel(std::string filename);
	void LoadMaterial(std::string filename);

public:
	Model(std::string filename);
	~Model();

	int nverts();
	int nfaces();

	Vec3f vert(int i);
	Vec3f vertNorm(int i);
	Vec2f textureCoord(int i);

	Face& triangle(int idx);
	std::vector<Face>& faces();
	std::map<std::string, Material>& mats();
};