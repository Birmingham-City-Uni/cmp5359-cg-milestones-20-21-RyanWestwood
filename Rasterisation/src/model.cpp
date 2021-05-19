#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"

Model::Model(std::string filename) : verts_(), tris_() {
	LoadModel(filename + ".obj");
	LoadMaterial(filename + ".mtl");
}

Model::~Model() {
}

int Model::nverts() {
	return (int)verts_.size();
}

int Model::nfaces() {
	return (int)tris_.size();
}

Face& Model::triangle(int idx)
{
	return tris_[idx];
}

Vec3f Model::vert(int i) {
	return verts_[i];
}

Vec3f Model::vertNorm(int i) {
	return vertNorms_[i];
}

Vec3f Model::textureCoord(int i) {
	return texCoords_[i];
}

std::vector<Face>& Model::faces()
{
	return tris_;
}

std::map<std::string, Material>& Model::mats()
{
	return materials_;
}

void Model::LoadModel(std::string filename) {
	std::ifstream in;
	in.open(filename, std::ifstream::in);
	if (in.fail()) return;
	std::string line;
	std::string mat;
	while (!in.eof()) {
		std::getline(in, line);
		std::istringstream iss(line.c_str());
		char trash;
		if (!line.compare(0, 2, "v ")) { // Vertex
			iss >> trash;
			Vec3f v;
			for (int i = 0; i < 3; i++) iss >> v.raw[i];
			verts_.push_back(v);
		}
		if (!line.compare(0, 3, "vn ")) { // Vertex normals
			iss >> trash;
			Vec3f v;
			for (int i = 0; i < 3; i++) iss >> v.raw[i];
			vertNorms_.push_back(v);
		}
		if (!line.compare(0, 3, "vt ")) { // Texture coordinates
			iss >> trash;
			Vec3f v;
			for (int i = 0; i < 2; i++) iss >> v.raw[i];
			texCoords_.push_back(v);
		}
		if (!line.compare(0, 7, "usemtl ")) {
			std::string str;
			iss >> str;
			iss >> str;
			mat = str;
		}
		if (!line.compare(0, 2, "f ")) { // face polygons
			std::vector<int> v;
			std::vector<int> t;
			std::vector<int> n;
			int slash, vIndex, tcIndex, vnIndex;
			iss >> trash;
			while (iss >> vIndex >> trash >> tcIndex >> trash >> vnIndex) {
				// in wavefront obj all indices start at 1, not zero
				vIndex--;
				tcIndex--;
				vnIndex--;
				v.push_back(vIndex);
				t.push_back(tcIndex);
				n.push_back(vnIndex);
			}
			tris_.emplace_back(Face{ v, t, n, mat });
		}
	}
	std::cerr << "# v# " << verts_.size() << " f# " << tris_.size() << std::endl;
}

void Model::LoadMaterial(std::string filename) {
	std::ifstream in;
	in.open(filename, std::ifstream::in);
	if (in.fail()) return;
	std::string line;

	std::unique_ptr<Material> mat;

	while (!in.eof()) {
		std::getline(in, line);
		std::istringstream iss(line.c_str());
		std::string trash;
		if (!line.compare(0, 7, "newmtl ")) { // Material name 
			if (mat != nullptr)	materials_.insert(std::make_pair(std::string(mat->name), Material(*mat)));
			iss >> trash;
			std::string str;
			iss >> str;

			mat = std::make_unique<Material>();
			mat->name = str;
		}
		if (!line.compare(0, 3, "Ka ")) { // Ambient colour
			iss >> trash;
			Vec3f v;
			for (int i = 0; i < 3; i++) iss >> v.raw[i];

			mat->Ka = v;
		}
		if (!line.compare(0, 3, "Kd ")) { // Diffuse colour
			iss >> trash;
			Vec3f v;
			for (int i = 0; i < 3; i++) iss >> v.raw[i];

			mat->Kd = v;
		}
		if (!line.compare(0, 3, "Ks ")) { // Specular colour
			iss >> trash;
			Vec3f v;
			for (int i = 0; i < 3; i++) iss >> v.raw[i];

			mat->Ks = v;
		}
		if (!line.compare(0, 3, "Ns ")) { // Specular highlights 
			iss >> trash;
			float f;
			iss >> f;

			mat->Ns = f;
		}
		if (!line.compare(0, 3, "Ni ")) { // Optical density
			iss >> trash;
			float f;
			iss >> f;

			mat->Ni = f;
		}
		if (!line.compare(0, 2, "d ")) { // Dissolve
			iss >> trash;
			float f;
			iss >> f;

			mat->d = f;
		}
		if (!line.compare(0, 6, "illum ")) { // Illumination model
			iss >> trash;
			int i;
			iss >> i;

			mat->illum = i;
		}
		if (!line.compare(0, 7, "map_Kd ")) { // Colour texture file
			iss >> trash;
			std::string str;
			iss >> str;

			mat->map_Kd = str;

			//	TODO: load texture here?
		}
	}
	materials_.insert(std::make_pair(std::string(mat->name), Material(*mat)));

	std::cerr << "## " << materials_.size() << "size of materials" << std::endl;
}

std::ostream& operator<<(std::ostream& os, const Face& f)
{
	os << f.vertexIndex[0] << "/" << f.textureCoordsIndex[0] << "/" << f.vertexNormalsIndex[0] << "\t" <<
		f.vertexIndex[1] << "/" << f.textureCoordsIndex[1] << "/" << f.vertexNormalsIndex[1] << "\t" <<
		f.vertexIndex[2] << "/" << f.textureCoordsIndex[2] << "/" << f.vertexNormalsIndex[2] << "\n";

	return os;
}

std::ostream& operator<<(std::ostream& os, const Material& m)
{
	os << "Name: " << m.name << "\n" <<
		"Ka: " << m.Ka.x << ", " << m.Ka.y << ", " << m.Ka.z << "\n" <<
		"Kd: " << m.Kd.x << ", " << m.Kd.y << ", " << m.Kd.z << "\n" <<
		"Ks: " << m.Ks.x << ", " << m.Ks.y << ", " << m.Ks.z << "\n" <<
		"Ns: " << m.Ns << "\n" <<
		"Ni: " << m.Ni << "\n" <<
		"d: " << m.d << "\n" <<
		"illum: " << m.illum << "\n" <<
		"map_Kd: " << m.map_Kd << "\n";

	return os;
}
