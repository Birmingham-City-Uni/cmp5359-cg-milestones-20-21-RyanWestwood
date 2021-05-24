#include "fileparser.h"

typedef char Byte;
std::string dir = "res/binary/";

void Write(std::vector<AABB>& objects, std::string filename) {
	std::cout << "Writing...\n";

	uint32_t bytes = objects.size() * sizeof(objects.front());

	std::ofstream file;
	file.open(dir + filename, std::ios::out | std::ios::binary);
	if (!file) std::cout << "Error file not found!\n";

	file.write(reinterpret_cast<char*>(&objects.front()), bytes);
	file.close();

	std::cout << "Finished!\n";
}

std::vector<AABB> Read(std::string filename) {
	std::cout << "Reading...\n";

	std::ifstream infile(dir + filename, std::ifstream::binary);
	if (!infile) std::cout << "Error file not found!\n";

	infile.seekg(0, infile.end);
	int bytes = infile.tellg();
	infile.seekg(0);

	Byte* buffer = new char[bytes];
	infile.read(buffer, bytes);
	infile.close();

	std::vector<AABB> results;
	for (int bytePosition = 0; bytePosition < bytes; bytePosition += 24)
	{
		Byte* obj = new Byte[24];
		for (int bit = 0; bit < 24; bit++)
		{
			obj[bit] = buffer[bytePosition + bit];
		}
		results.push_back(*(AABB*)obj);
	}

	delete[] buffer;

	std::cout << "Finished!\n";

	return results;
}

enum {
	HITTABLE = 0,	//16 Bytes
	BVH_NODE,		//72 Bytes
	TRIANGLE,		//104 Bytes
	SPHERE			//56 Bytes
};

std::vector<unsigned long long> sizes = {
	sizeof(Hittable),
	sizeof(BVH_Node),
	sizeof(Triangle),
	sizeof(Sphere),
};

void WriteNode(std::vector<std::shared_ptr<Hittable>>& objects, std::string filename) {
	std::cout << "Writing...\n";

	uint32_t bytes = 0;
	for (uint32_t i = 0; i < objects.size(); i++)
		bytes += sizes[objects[i]->id];

	std::ofstream file;
	file.open(dir + filename, std::ios::out | std::ios::binary);
	if (!file) std::cout << "Error file not found!\n";

	for (auto& obj : objects) {
		if (obj->id == HITTABLE)
			file.write(reinterpret_cast<char*>(static_cast<Hittable*>(obj.get())), sizes[obj->id]);
		if (obj->id == BVH_NODE)
			file.write(reinterpret_cast<char*>(static_cast<BVH_Node*>(obj.get())), sizes[obj->id]);
		if (obj->id == TRIANGLE)
			file.write(reinterpret_cast<char*>(static_cast<Triangle*>(obj.get())), sizes[obj->id]);
		if (obj->id == SPHERE)
			file.write(reinterpret_cast<char*>(static_cast<Sphere*>(obj.get())), sizes[obj->id]);
	}
	file.close();

	std::cout << "Finished!\n";
}

std::vector<Hittable*> ReadNode(std::string filename) {
	std::cout << "Reading...\n";

	std::ifstream infile(dir + filename, std::ifstream::binary);
	if (!infile) std::cout << "Error file not found!\n";

	infile.seekg(0, infile.end);
	int bytes = infile.tellg();
	infile.seekg(0);

	Byte* buffer = new char[bytes];
	infile.read(buffer, bytes);
	infile.close();

	std::vector<Hittable*> results;
	int progresscount = 0;
	for (int bytePosition = 0; bytePosition < bytes; bytePosition += progresscount)
	{
		Byte* hittable = new Byte[sizes[HITTABLE]];
		for (int bit = 0; bit < 12; bit++)
		{
			hittable[bit] = buffer[bytePosition + bit];
		}
		Hittable* a = (Hittable*)hittable;

		if (a->id == BVH_NODE) {
			Byte* bvh = new Byte[sizes[BVH_NODE]];
			for (int bit = 0; bit < sizes[BVH_NODE]; bit++)
			{
				bvh[bit] = buffer[bytePosition + bit];
			}
			progresscount = sizes[BVH_NODE];
			results.push_back((BVH_Node*)bvh);
		}
		if (a->id == TRIANGLE) {
			Byte* tri = new Byte[sizes[TRIANGLE]];
			for (int bit = 0; bit < sizes[TRIANGLE]; bit++)
			{
				tri[bit] = buffer[bytePosition + bit];
			}
			progresscount = sizes[TRIANGLE];
			results.push_back((Triangle*)tri);
		}
		if (a->id == SPHERE) {
			Byte* sph = new Byte[sizes[SPHERE]];
			for (int bit = 0; bit < sizes[SPHERE]; bit++)
			{
				sph[bit] = buffer[bytePosition + bit];
			}
			progresscount = sizes[SPHERE];
			results.push_back((Sphere*)sph);
		}

		delete[] hittable;
	}

	delete[] buffer;

	std::cout << "Finished!\n";

	return results;
}