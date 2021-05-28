#include "fileparser.h"

typedef char Byte;
std::string dir = "./res/binary/";

//	MATERIALS
const std::vector<uint64_t> materialSizes = {
	sizeof(Material),
	sizeof(Lambertian),
	sizeof(Metal),
	sizeof(Dielectric),
	sizeof(Diffuse_Light)
};

void WriteMaterials(std::vector<std::shared_ptr<Material>>& mats, std::string filename){
	std::cout << "Writing...\n";

	uint32_t bytes = 0;
	for(auto& mat : mats){
		bytes += materialSizes[mat->id];
	}

	std::ofstream file;
	file.open(dir + filename, std::ios::out | std::ios::binary);
	if (!file) {
		std::cout << "Error file not found!\n";
	}
	for (auto& mat : mats) {
		if (mat->id == MATERIAL){
			file.write(reinterpret_cast<char*>(dynamic_cast<Material*>(mat.get())), materialSizes[mat->id]);
		}
		else if (mat->id == LAMBERTIAN){
			file.write(reinterpret_cast<char*>(dynamic_cast<Lambertian*>(mat.get())), materialSizes[mat->id]);
		}
		else if (mat->id == METAL){
			file.write(reinterpret_cast<char*>(dynamic_cast<Metal*>(mat.get())), materialSizes[mat->id]);
		}
		else if (mat->id == DIELECTRIC){
			file.write(reinterpret_cast<char*>(dynamic_cast<Dielectric*>(mat.get())), materialSizes[mat->id]);
		}
		else if (mat->id == DIFFUSE_LIGHT){
			file.write(reinterpret_cast<char*>(dynamic_cast<Diffuse_Light*>(mat.get())), materialSizes[mat->id]);
		}
		else {
			std::cout << "Unknown material!\n";
		}
	}
	file.close();

	std::cout << "Finished!\n";
}

std::vector<Material*> ReadMaterials(std::string filename) {
	std::cout << "Reading...\n";

	std::ifstream infile(dir + filename, std::ifstream::binary);
	if (!infile){
		std::cout << "Error file not found!\n";
	}
	infile.seekg(0, std::ifstream::end);
	int bytes = infile.tellg();
	infile.seekg(0);

	Byte* buffer = new char[bytes];
	infile.read(buffer, bytes);
	infile.close();

	std::vector<Material*> results;
	int progresscount = 0;
	for (int bytePosition = 0; bytePosition < bytes; bytePosition += progresscount)
	{
		Byte* material = new Byte[materialSizes[MATERIAL]];
		for (int bit = 0; bit < materialSizes[MATERIAL]; bit++)
		{
			material[bit] = buffer[bytePosition + bit];
		}
		auto base = reinterpret_cast<Material*>(material);

		if (base->id == LAMBERTIAN) {
			Byte* lambert = new Byte[materialSizes[LAMBERTIAN]];
			for (int bit = 0; bit < materialSizes[LAMBERTIAN]; bit++)
			{
				lambert[bit] = buffer[bytePosition + bit];
			}
			progresscount = materialSizes[LAMBERTIAN];
			results.push_back(reinterpret_cast<Lambertian*>(lambert));
		}
		else if (base->id == METAL) {
			Byte* metal = new Byte[materialSizes[METAL]];
			for (int bit = 0; bit < materialSizes[METAL]; bit++)
			{
				metal[bit] = buffer[bytePosition + bit];
			}
			progresscount = materialSizes[METAL];
			results.push_back(reinterpret_cast<Metal*>(metal));
		}
		else if (base->id == DIELECTRIC) {
			Byte* dielectric = new Byte[materialSizes[DIELECTRIC]];
			for (int bit = 0; bit < materialSizes[DIELECTRIC]; bit++)
			{
				dielectric[bit] = buffer[bytePosition + bit];
			}
			progresscount = materialSizes[DIELECTRIC];
			results.push_back(reinterpret_cast<Dielectric*>(dielectric));
		}
		else if (base->id == DIFFUSE_LIGHT) {
			Byte* diffuse_light = new Byte[materialSizes[DIFFUSE_LIGHT]];
			for (int bit = 0; bit < materialSizes[DIFFUSE_LIGHT]; bit++)
			{
				diffuse_light[bit] = buffer[bytePosition + bit];
			}
			progresscount = materialSizes[DIFFUSE_LIGHT];
			results.push_back(reinterpret_cast<Diffuse_Light*>(diffuse_light));
		}

		delete[] material;
	}

	delete[] buffer;

	std::cout << "Finished!\n";

	return results;
}

//	NODES
const std::vector<uint64_t> hittableSizes = {
	sizeof(Hittable),
	sizeof(BVH_Node),
	sizeof(Triangle),
	sizeof(Sphere),
};

void WriteNode(std::vector<std::shared_ptr<Hittable>>& objects, std::string filename) {
	std::cout << "Writing...\n";

	uint32_t bytes = 0;
	for(auto& obj : objects){
		bytes += materialSizes[obj->id];
	}

	std::ofstream file;
	file.open(dir + filename, std::ios::out | std::ios::binary);
	if (!file){
		std::cout << "Error file not found!\n";
	}
	for (auto& obj : objects) {
		if (obj->id == HITTABLE){
			file.write(reinterpret_cast<char*>(dynamic_cast<Hittable*>(obj.get())), hittableSizes[obj->id]);
		}
		else if (obj->id == BVH_NODE){
			file.write(reinterpret_cast<char*>(dynamic_cast<BVH_Node*>(obj.get())), hittableSizes[obj->id]);
		}
		else if (obj->id == TRIANGLE){
			file.write(reinterpret_cast<char*>(dynamic_cast<Triangle*>(obj.get())), hittableSizes[obj->id]);
		}
		else if (obj->id == SPHERE){
			file.write(reinterpret_cast<char*>(dynamic_cast<Sphere*>(obj.get())), hittableSizes[obj->id]);
		}
		else{
			std::cout << "Unknown node!\n";
		}
	}
	file.close();

	std::cout << "Finished!\n";
}

std::vector<Hittable*> ReadNode(std::string filename) {
	std::cout << "Reading...\n";

	std::ifstream infile(dir + filename, std::ifstream::binary);
	if (!infile) {
		std::cout << "Error file not found!\n";
	}
	infile.seekg(0, std::ifstream::end);
	int bytes = infile.tellg();
	infile.seekg(0);

	Byte* buffer = new char[bytes];
	infile.read(buffer, bytes);
	infile.close();

	std::vector<Hittable*> results;
	int progresscount = 0;
	for (int bytePosition = 0; bytePosition < bytes; bytePosition += progresscount)
	{
		Byte* hittable = new Byte[hittableSizes[HITTABLE]];
		for (int bit = 0; bit < hittableSizes[HITTABLE]; bit++)
		{
			hittable[bit] = buffer[bytePosition + bit];
		}
		auto base = reinterpret_cast<Hittable*>(hittable);

		if (base->id == BVH_NODE) {
			Byte* bvh = new Byte[hittableSizes[BVH_NODE]];
			for (int bit = 0; bit < hittableSizes[BVH_NODE]; bit++)
			{
				bvh[bit] = buffer[bytePosition + bit];
			}
			progresscount = hittableSizes[BVH_NODE];
			results.push_back(reinterpret_cast<BVH_Node*>(bvh));
		}
		else if (base->id == TRIANGLE) {
			Byte* tri = new Byte[hittableSizes[TRIANGLE]];
			for (int bit = 0; bit < hittableSizes[TRIANGLE]; bit++)
			{
				tri[bit] = buffer[bytePosition + bit];
			}
			progresscount = hittableSizes[TRIANGLE];
			results.push_back(reinterpret_cast<Triangle*>(tri));
		}
		else if (base->id == SPHERE) {
			Byte* sph = new Byte[hittableSizes[SPHERE]];
			for (int bit = 0; bit < hittableSizes[SPHERE]; bit++)
			{
				sph[bit] = buffer[bytePosition + bit];
			}
			progresscount = hittableSizes[SPHERE];
			results.push_back(reinterpret_cast<Sphere*>(sph));
		}
		delete[] hittable;
	}

	delete[] buffer;

	std::cout << "Finished!\n";

	return results;
}