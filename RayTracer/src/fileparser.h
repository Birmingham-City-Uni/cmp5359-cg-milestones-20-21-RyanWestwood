#pragma once
#include <fstream>
#include <sstream>
#include <iostream>
#include <bitset>

//	TODO: have this cast in a void and typecast it back?
typedef char Byte;
std::string dir = "res/binary/";

//void GenerateFileFromObject(std::vector<std::shared_ptr<Hittable>>& objects, const char* filename) {
//
//	int bytes = objects.size() * sizeof(BVH_Node);
//
//	std::vector<BVH_Node> test;
//	for (auto& obj : objects) {
//		if (obj == nullptr) test.push_back(BVH_Node());
//		else test.push_back(static_cast<BVH_Node>(obj));
//	}
//
//	std::ofstream file;
//	file.open(dir + filename, std::ios::out | std::ios::binary);
//	if (!file) std::cout << "ERROR 404\n";
//
//	file.write(reinterpret_cast<char*>(&test.front()), bytes);
//	file.close();
//
//	std::cout << "file created!\n";
//}

void GenerateFileFromObject(std::vector<AABB>& objects, const char* filename) {
	std::cout << "Writing\n";

	uint32_t bytes = objects.size() * sizeof(objects.front());

	std::ofstream file;
	file.open(dir + filename, std::ios::out | std::ios::binary);
	if (!file) std::cout << "ERROR 404\n";

	file.write(reinterpret_cast<char*>(&objects.front()), bytes);
	file.close();
}
std::vector<AABB*> ReadObjectFromFile(const char* filename) {
	std::cout << "Reading\n";

	std::ifstream infile(dir + filename, std::ifstream::binary);
	if (!infile) std::cout << "ERROR 404\n";

	infile.seekg(0, infile.end);
	int bytes = infile.tellg();
	infile.seekg(0);

	Byte* buffer = new char[bytes];
	infile.read(buffer, bytes);
	infile.close();

	std::vector<AABB*> results;
	for (int bytePosition = 0; bytePosition < bytes; bytePosition += 24)
	{
		Byte* obj = new Byte[24];
		for (int bit = 0; bit < 24; bit++)
		{
			obj[bit] = buffer[bytePosition + bit];
		}
		results.push_back((AABB*)obj);
		std::cout << *(AABB*)obj << std::endl;
	}

	//delete[] buffer;
	return results;
}

