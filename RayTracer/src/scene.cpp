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

	Model* model = new Model("./res/raster-scene");

	Vec3f transform(0, 0.8, 0);
	auto mat_diffuse = std::make_shared<Lambertian>(Colour(0.4, 0.2, 0.1), index);
	model->AddToWorld(world, transform, mat_diffuse, index);
	m.push_back(mat_diffuse);
	index++;

	return Hittable_List(std::make_shared<BVH_Node>(world));
}

Hittable_List My_Scene_Ind(std::vector<std::shared_ptr<Material>>& m) {
	Hittable_List world;
	int index = 1;

	//Model* scene = new Model("./res/raster-scene");
	//Vec3f transform(0, 0, 0);
	//auto wall_diffuse = std::make_shared<Lambertian>(Colour(136.f / 255.f, 133.F / 255.f, 122.f / 255.f), index);
	//scene->AddToWorld(world, transform, wall_diffuse, index);
	//m.push_back(wall_diffuse);
	//index++;

	Vec3f transform(0, 0, 0);

	std::unique_ptr<Model> left_wall = std::make_unique<Model>("./res/left-wall");
	std::unique_ptr<Model> right_wall = std::make_unique<Model>("./res/right-wall");
	std::unique_ptr<Model> floor = std::make_unique<Model>("./res/floor");

	auto wall_diffuse = std::make_shared<Lambertian>(Colour(136.f / 255.f, 133.F / 255.f, 122.f / 255.f), index);

	left_wall->AddToWorld(world, transform, wall_diffuse, index);
	right_wall->AddToWorld(world, transform, wall_diffuse, index);
	m.push_back(wall_diffuse);
	index++;

	auto floor_diffuse = std::make_shared<Lambertian>(Colour(77.f / 255.f, 56.f / 255.f, 33.f / 255.f), index);
	floor->AddToWorld(world, transform, floor_diffuse, index);
	m.push_back(floor_diffuse);
	index++;

	std::unique_ptr<Model> leg_one = std::make_unique<Model>("./res/leg-1");
	std::unique_ptr<Model> leg_two = std::make_unique<Model>("./res/leg-2");
	std::unique_ptr<Model> leg_three = std::make_unique<Model>("./res/leg-3");
	std::unique_ptr<Model> leg_four = std::make_unique<Model>("./res/leg-4");
	auto leg_diffuse = std::make_shared<Lambertian>(Colour(8.f / 255.f, 0.f / 255.f, 8.f / 255.f), index);
	leg_one->AddToWorld(world, transform, leg_diffuse, index);
	leg_two->AddToWorld(world, transform, leg_diffuse, index);
	leg_three->AddToWorld(world, transform, leg_diffuse, index);
	leg_four->AddToWorld(world, transform, leg_diffuse, index);
	m.push_back(leg_diffuse);
	index++;

	std::unique_ptr<Model> table_top = std::make_unique<Model>("./res/table-top");
	auto table_metal = std::make_shared<Metal>(Colour(0.5, 0.6, 0.5), 0.3, index);
	table_top->AddToWorld(world, transform, table_metal, index);
	m.push_back(table_metal);
	index++;

	std::unique_ptr<Model> palm_setup = std::make_unique<Model>("./res/palm-setup");
	std::unique_ptr<Model> palm_tree = std::make_unique<Model>("./res/palm-tree");
	auto picture_frame = std::make_shared<Lambertian>(Colour(86.f / 255.f, 81.f / 255.f, 79.f / 255.f), index);
	palm_setup->AddToWorld(world, transform, picture_frame, index);
	palm_tree->AddToWorld(world, transform, picture_frame, index);
	m.push_back(picture_frame);
	index++;


	std::unique_ptr<Model> book_one_cover = std::make_unique<Model>("./res/book-one-cover");
	auto book_one_cover_mat = std::make_shared<Lambertian>(Colour(125.f / 255.f, 64.f / 255.f, 146.f / 255.f), index);
	book_one_cover->AddToWorld(world, transform, book_one_cover_mat, index);
	m.push_back(book_one_cover_mat);
	index++;

	std::unique_ptr<Model> book_two_cover = std::make_unique<Model>("./res/book-two-cover");
	auto book_two_cover_mat = std::make_shared<Lambertian>(Colour(70.f / 255.f, 139.f / 255.f, 144.f / 255.f), index);
	book_two_cover->AddToWorld(world, transform, book_two_cover_mat, index);
	m.push_back(book_two_cover_mat);
	index++;

	std::unique_ptr<Model> book_three_cover = std::make_unique<Model>("./res/book-three-cover");
	auto book_three_cover_mat = std::make_shared<Lambertian>(Colour(106.f / 255.f, 69.f / 255.f, 35.f / 255.f), index);
	book_three_cover->AddToWorld(world, transform, book_three_cover_mat, index);
	m.push_back(book_three_cover_mat);
	index++;
	
	std::unique_ptr<Model> book_four_cover = std::make_unique<Model>("./res/book-four-cover");
	auto book_four_cover_mat = std::make_shared<Lambertian>(Colour(57.f / 255.f, 123.f / 255.f, 0.f / 255.f), index);
	book_four_cover->AddToWorld(world, transform, book_four_cover_mat, index);
	m.push_back(book_four_cover_mat);
	index++;
	
	std::unique_ptr<Model> book_five_cover = std::make_unique<Model>("./res/book-five-cover");
	auto book_five_cover_mat = std::make_shared<Lambertian>(Colour(6.f / 255.f, 0.f / 255.f, 6.f / 255.f), index);
	book_five_cover->AddToWorld(world, transform, book_five_cover_mat, index);
	m.push_back(book_five_cover_mat);
	index++;

	std::unique_ptr<Model> book_one_pages = std::make_unique<Model>("./res/book-one-pages");
	std::unique_ptr<Model> book_two_pages = std::make_unique<Model>("./res/book-two-pages");
	std::unique_ptr<Model> book_three_pages = std::make_unique<Model>("./res/book-three-pages");
	std::unique_ptr<Model> book_four_pages = std::make_unique<Model>("./res/book-four-pages");
	std::unique_ptr<Model> book_five_pages = std::make_unique<Model>("./res/book-five-pages");
	auto pages = std::make_shared<Lambertian>(Colour(214.f / 255.f, 211.f / 255.f, 171.f / 255.f), index);

	book_one_pages->AddToWorld(world, transform, pages, index);
	book_two_pages->AddToWorld(world, transform, pages, index);
	book_three_pages->AddToWorld(world, transform, pages, index);
	book_four_pages->AddToWorld(world, transform, pages, index);
	book_five_pages->AddToWorld(world, transform, pages, index);
	m.push_back(pages);
	index++;

	std::unique_ptr<Model> plant = std::make_unique<Model>("./res/plant");
	auto plant_mat = std::make_shared<Lambertian>(Colour(178.f / 255.f, 97.f / 255.f, 66.f / 255.f), index);

	plant->AddToWorld(world, transform, plant_mat, index);
	m.push_back(plant_mat);
	index++;

	std::unique_ptr<Model> mug_one = std::make_unique<Model>("./res/mug-one");
	std::unique_ptr<Model> mug_two = std::make_unique<Model>("./res/mug-two");
	auto mug_mat = std::make_shared<Metal>(Colour(52.f / 255.f, 154.f / 255.f, 166.f / 255.f), 0.3f, index);
	mug_one->AddToWorld(world, transform, mug_mat, index);
	mug_two->AddToWorld(world, transform, mug_mat, index);
	m.push_back(mug_mat);
	index++;

	std::unique_ptr<Model> lamp_top = std::make_unique<Model>("./res/lamp-top");
	std::unique_ptr<Model> lamp_bottom = std::make_unique<Model>("./res/lamp-bottom");
	auto lamp_mat = std::make_shared<Metal>(Colour(0.f / 255.f, 50.f / 255.f, 8.f / 255.f), 0.1f, index);
	lamp_top->AddToWorld(world, transform, lamp_mat, index);
	lamp_bottom->AddToWorld(world, transform, lamp_mat, index);
	m.push_back(lamp_mat);
	index++;

	std::unique_ptr<Model> glass_sphere = std::make_unique<Model>("./res/glass-sphere");
	auto glass_sphere_mat = std::make_shared<Dielectric>(1.5, index);
	glass_sphere->AddToWorld(world, transform, glass_sphere_mat, index);
	m.push_back(glass_sphere_mat);
	index++;	
	
	
	return Hittable_List(std::make_shared<BVH_Node>(world));
}