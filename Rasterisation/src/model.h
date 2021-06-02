#pragma once
#include "geometry.h"
#include "tgaimage.h"
#include "stb_image.h"
#include <vector>
#include <map>
#include <unordered_map>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

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
	Vec3f Tf;
	float Ns;
	float Ni;
	float d;
	int illum;
	std::string map_Kd;
	stbi_uc* image;	
	int w, h, n;

	friend std::ostream& operator<<(std::ostream& os, const Material& m);
	Material() = default;
	Material(std::string exc) : Kd(1.F, 1.F, 1.F) {};

	TGAColor GetPixel(float x, float y) {
		if (x * w >= w || y * h >= h) return { 255,255,255,255 };
		unsigned char* pixelOffset = image + ((int)(x * w) + (int)(y * h) * w) * n;
		return { pixelOffset[0], pixelOffset[1], pixelOffset[2], 255 };
	}
};

class Model {
private:
	std::vector<Vec3f> verts_;
	std::vector<Vec3f> vertNorms_;
	std::vector<Vec2f> texCoords_;
	std::vector<Face> tris_;

	std::unordered_map<std::string, Material> materials_;

	void LoadModel(const std::string& filename);
	void LoadMaterial(const std::string& filename);

public:
	Model(const std::string& filename);
	~Model();

	int nverts();
	int nfaces();

	Vec3f vert(int i);
	Vec3f vertNorm(int i);
	Vec2f textureCoord(int i);

	Face& triangle(int idx);
	std::vector<Face>& faces();
	std::unordered_map<std::string, Material>& mats();
};