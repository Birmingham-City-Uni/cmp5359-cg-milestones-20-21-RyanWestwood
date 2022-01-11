#include "model.h"

Model::Model(const std::string& filename) {
	LoadModel(filename + ".obj");
	LoadMaterial(filename + ".mtl");
}

Model::~Model() {
}

int Model::nverts() {
	return static_cast<int>(verts_.size());
}

int Model::nfaces() {
	return static_cast<int>(tris_.size());
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

Vec2f Model::textureCoord(int i) {
	return texCoords_[i];
}

std::vector<Face>& Model::faces()
{
	return tris_;
}

std::unordered_map<std::string, Material>& Model::mats()
{
	return materials_;
}

void Model::LoadModel(const std::string& filename) {
	std::ifstream in;
	in.open(filename, std::ifstream::in);
	if (in.fail()){
		return;
	}
	std::string line;
	std::string mat;
	while (!in.eof()) {
		std::getline(in, line);
		std::istringstream iss(line);
		char trash;
		if (line.compare(0, 2, "v ") == 0) { // Vertex
			iss >> trash;
			Vec3f v;
			for(float& i : v.raw){
				iss>>i;
			}
			verts_.push_back(v);
		}
		if (line.compare(0, 3, "vn ") == 0) { // Vertex normals
			iss >> trash;
			Vec3f v;
			for(float& i : v.raw){
				iss>>i;
			}
			vertNorms_.push_back(v);
		}
		if (line.compare(0, 3, "vt ") == 0) { // Texture coordinates
			std::string str;
			iss >> str;
			Vec2f v;
			for(float& i : v.raw){
				iss>>i;
			}
			texCoords_.push_back(v);
		}
		if (line.compare(0, 7, "usemtl ") == 0) {
			std::string str;
			iss >> str;
			iss >> str;
			mat = str;
		}
		if (line.compare(0, 2, "f ") == 0) { // face polygons
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
	//std::cerr << "# v# " << verts_.size() << " f# " << tris_.size() << std::endl;
}

void Model::LoadMaterial(const std::string& filename) {
	std::ifstream in;
	in.open(filename, std::ifstream::in);
	if (in.fail()){
		return;
	} 
	std::string line;

	std::unique_ptr<Material> mat;

	while (!in.eof()) {
		std::getline(in, line);
		std::istringstream iss(line);
		std::string trash;
		if (line.compare(0, 7, "newmtl ") == 0) { // Material name 
			if (mat != nullptr)	{
				materials_.insert(std::make_pair(std::string(mat->name), Material(*mat)));
			}
			iss >> trash;
			std::string str;
			iss >> str;

			mat = std::make_unique<Material>();
			mat->name = str;
		}
		if (line.compare(0, 3, "Ka ") == 0) { // Ambient colour
			iss >> trash;
			Vec3f v;
			for(float& i : v.raw){
				iss>>i;
			}

			mat->Ka = v;
		}
		if (line.compare(0, 3, "Kd ") == 0) { // Diffuse colour
			iss >> trash;
			Vec3f v;
			for(float& i : v.raw){
				iss>>i;
			}

			mat->Kd = v;
		}
		if (line.compare(0, 3, "Ks ") == 0) { // Specular colour
			iss >> trash;
			Vec3f v;
			for(float& i : v.raw){
				iss>>i;
			}

			mat->Ks = v;
		}
		if (line.compare(0, 3, "Ns ") == 0) { // Specular highlights 
			iss >> trash;
			float f;
			iss >> f;

			mat->Ns = f;
		}
		if (line.compare(0, 3, "Ni ") == 0) { // Optical density
			iss >> trash;
			float f;
			iss >> f;

			mat->Ni = f;
		}
		if (line.compare(0, 3, "Tf ") == 0) { // Transmission filter
			iss >> trash;
			Vec3f v;
			for(float& i : v.raw){
				iss>>i;
			}

			mat->Tf = v;
		}
		if (line.compare(0, 2, "d ") == 0) { // Dissolve
			iss >> trash;
			float f;
			iss >> f;

			mat->d = f;
		}
		if (line.compare(0, 6, "illum ") == 0) { // Illumination model
			iss >> trash;
			int i;
			iss >> i;

			mat->illum = i;
		}
		if (line.compare(0, 7, "map_Kd ") == 0) { // Colour texture file
			iss >> trash;
			std::string str;
			iss >> str;

			mat->map_Kd = str;

			unsigned bytePerPixel = 4;
			mat->image = stbi_load(("./res/" + str).c_str(), &mat->w, &mat->h, &mat->n, bytePerPixel);
		}
	}
	materials_.insert(std::make_pair(std::string(mat->name), Material(*mat)));

	//std::cerr << "## " << materials_.size() << "size of materials" << std::endl;
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
		"Tf: " << m.Tf.x << ", " << m.Tf.y << ", " << m.Tf.z << "\n" <<
		"d: " << m.d << "\n" <<
		"illum: " << m.illum << "\n" <<
		"map_Kd: " << m.map_Kd << "\n";

	return os;
}
