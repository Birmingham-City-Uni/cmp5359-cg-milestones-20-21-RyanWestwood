#include "scene.h"

Hittable_List Small_Scene(std::vector<std::shared_ptr<Material>>& m) {
	Hittable_List world;
	int index = 1;

	auto material1 = std::make_shared<Dielectric>(1.5, index);
	world.Add(std::make_shared<Sphere>(Point3f(0, 1, 0), 1.0, material1, index));
	m.push_back(material1);
	index++;

	auto material2 = std::make_shared<Lambertian>(Colour(0.4, 0.2, 0.1), index);
	world.Add(std::make_shared<Sphere>(Point3f(-4, 1, 0), 1.0, material2, index));
	m.push_back(material2);
	index++;

	auto material3 = std::make_shared<Metal>(Colour(0.7, 0.6, 0.5), 0.0, index);
	world.Add(std::make_shared<Sphere>(Point3f(4, 1, 0), 1.0, material3, index));
	m.push_back(material3);
	index++;

	auto material4 = std::make_shared<Diffuse_Light>(Colour(255, 255, 255), index);
	world.Add(std::make_shared<Sphere>(Point3f(0, 3, 0), 0.5, material4, index));
	m.push_back(material4);
	index++;

	auto ground_material = std::make_shared<Lambertian>(Colour(0.5, 0.5, 0.5), index);
	world.Add(std::make_shared<Sphere>(Point3f(0, -1000, 0), 1000, ground_material, index));
	m.push_back(ground_material);
	index++;

	return Hittable_List(std::make_shared<BVH_Node>(world));
}

Hittable_List Test_Scene(std::vector<std::shared_ptr<Material>>& m) {
	Hittable_List world;
	int index = 1;

	Model* model = new Model("./res/cc_t");

	Vec3f transform(0, 0.8, 0);
	auto glass = std::make_shared<Dielectric>(1.5, index);
	model->AddToWorld(world, transform, glass, index);
	m.push_back(glass);
	index++;

	transform = Vec3f(1.2, 0.8, 0);
	auto mat_diffuse = std::make_shared<Lambertian>(Colour(0.4, 0.2, 0.1), index);
	model->AddToWorld(world, transform, mat_diffuse, index);
	m.push_back(mat_diffuse);
	index++;

	transform = Vec3f(-1.2, 0.8, 0);
	auto mat_metal = std::make_shared<Metal>(Colour(0.5, 0.6, 0.5), 0.0, index);
	model->AddToWorld(world, transform, mat_metal, index);
	m.push_back(mat_metal);
	index++;

	auto ground_material = std::make_shared<Lambertian>(Colour(0.5, 0.5, 0.5), index);
	world.Add(std::make_shared<Sphere>(Point3f(0, -1000, 0), 1000, ground_material, index));
	m.push_back(ground_material);
	index++;

	auto material4 = std::make_shared<Diffuse_Light>(Colour(255, 255, 255), index);
	world.Add(std::make_shared<Sphere>(Point3f(0, 5, 0), 0.5, material4, index));
	m.push_back(material4);
	index++;

	return Hittable_List(std::make_shared<BVH_Node>(world));
}

Hittable_List My_Scene(std::vector<std::shared_ptr<Material>>& m) {
	Hittable_List world;
	int index = 1;

	Model* model = new Model("./res/Scene");

	return Hittable_List(std::make_shared<BVH_Node>(world));
}