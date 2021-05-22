//#include <iostream>
//#include <string>
//#include <fstream>
//#include <sstream>
//#include <vector>
//#include "model.h"
//
//Model::Model(const char* filename) : verts_(), faces_() {
//	std::ifstream in;
//	in.open(filename, std::ifstream::in);
//	if (in.fail()) return;
//	std::string line;
//	while (!in.eof()) {
//		std::getline(in, line);
//		std::istringstream iss(line.c_str());
//		char trash;
//		if (!line.compare(0, 2, "v ")) {
//			iss >> trash;
//			Vec3f v;
//			for (int i = 0; i < 3; i++) iss >> v.raw[i];
//			verts_.push_back(v);
//		}
//		else if (!line.compare(0, 2, "f ")) {
//			std::vector<int> f;
//			std::vector<int> vns;
//			std::vector<int> uvs;
//			int itrash, v_idx, vt_idx, vn_idx;
//			iss >> trash;
//			while (iss >> v_idx >> trash >> vt_idx >> trash >> vn_idx) {
//				v_idx--, vt_idx--, vn_idx--; // in wavefront obj all indices start at 1, not zero
//				f.push_back(v_idx);
//				vns.push_back(vn_idx);
//				uvs.push_back(vt_idx);
//			}
//			faces_.push_back(f);
//			vnorms_.push_back(vns);
//			uvs_.push_back(uvs);
//		}
//	}
//	std::cerr << "Model: " << filename << " v# " << verts_.size() << " f# " << faces_.size() << std::endl;
//}
//
//Model::~Model() {
//}
//
//int Model::nverts() {
//	return (int)verts_.size();
//}
//
//int Model::nfaces() {
//	return (int)faces_.size();
//}
//
//std::vector<int> Model::face(int idx) {
//	return faces_[idx];
//}
//
//std::vector<int> Model::vnorms(int idx)
//{
//	return vnorms_[idx];
//}
//
//Vec3f Model::vert(int i) {
//	return verts_[i];
//}
//
//Vec3f Model::vn(int i)
//{
//	return vnorms_[i];
//}
//

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
			tris_.emplace_back(Face{ v, t, n });
		}
	}
	std::cerr << "# v# " << verts_.size() << " f# " << tris_.size() << std::endl;
}

std::ostream& operator<<(std::ostream& os, const Face& f)
{
	os << f.vertexIndex[0] << "/" << f.textureCoordsIndex[0] << "/" << f.vertexNormalsIndex[0] << "\t" <<
		f.vertexIndex[1] << "/" << f.textureCoordsIndex[1] << "/" << f.vertexNormalsIndex[1] << "\t" <<
		f.vertexIndex[2] << "/" << f.textureCoordsIndex[2] << "/" << f.vertexNormalsIndex[2] << "\n";

	return os;
}