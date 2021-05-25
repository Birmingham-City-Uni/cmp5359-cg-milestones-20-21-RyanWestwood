#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"

Model::Model(std::string filename) : verts_(), tris_() {
	LoadModel(filename + ".obj");
	//LoadMaterial(filename + ".mtl");
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

void Model::LoadModel(std::string filename) {
	std::ifstream in;
	in.open(filename, std::ifstream::in);
	if (in.fail()) return;
	std::string line;
	while (!in.eof()) {
		std::getline(in, line);
		std::istringstream iss(line.c_str());
		std::string trash;
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
		if (!line.compare(0, 2, "f ")) { // face polygons
			std::vector<int> v, t, n;
			int vIndex, tcIndex, vnIndex;
			char slash;
			iss >> trash;
			while (iss >> vIndex >> slash >> tcIndex >> slash >> vnIndex) {
				// in wavefront obj all indices start at 1, not zero
				vIndex--;
				tcIndex--;
				vnIndex--;
				v.push_back(vIndex);
				t.push_back(tcIndex);
				n.push_back(vnIndex);
			}
			tris_.emplace_back(Face{ v, t, n });
		}
	}
	std::cerr << "# v# " << verts_.size() << " f# " << tris_.size() << std::endl;
}

void Model::AddToWorld(Hittable_List& world, Vec3f transform, std::shared_ptr<Material> mat, int index)
{
	for (uint32_t i = 0; i < tris_.size(); i++)
	{
		const Vec3f v0 = verts_[tris_[i].vertexIndex[0]];
		const Vec3f v1 = verts_[tris_[i].vertexIndex[1]];
		const Vec3f v2 = verts_[tris_[i].vertexIndex[2]];

		const Vec3f v0n = vertNorms_[tris_[i].vertexNormalsIndex[0]];
		const Vec3f v1n = vertNorms_[tris_[i].vertexNormalsIndex[1]];
		const Vec3f v2n = vertNorms_[tris_[i].vertexNormalsIndex[2]];

		world.Add(std::make_shared<Triangle>(v0 + transform, v1 + transform, v2 + transform, v0n, v1n, v2n, mat, index));
	}
}

std::ostream& operator<<(std::ostream& os, const Face& f)
{
	os << f.vertexIndex[0] << "/" << f.textureCoordsIndex[0] << "/" << f.vertexNormalsIndex[0] << "\t" <<
		f.vertexIndex[1] << "/" << f.textureCoordsIndex[1] << "/" << f.vertexNormalsIndex[1] << "\t" <<
		f.vertexIndex[2] << "/" << f.textureCoordsIndex[2] << "/" << f.vertexNormalsIndex[2] << "\n";

	return os;
}