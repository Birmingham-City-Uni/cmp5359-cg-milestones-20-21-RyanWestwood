// A practical implementation of the ray tracing algorithm.
#include "geometry.h"
#include "ray.h"
#include "common.h"
#include "hittable_list.h"
#include "camera.h"
#include "Sphere.h"
#include "SDL.h" 
#include "material.h"
#include "threadpool.h"
#include "model.h"
#include "Triangle.h"
#include "bvh.h"
#include "fileparser.h"
#include "timer.h"
#include <fstream>
#include <chrono>
#include <thread>
#include <vector>
#include <array>
#include <memory>
#include <queue>

#define M_PI 3.14159265359

typedef std::vector<std::vector<Colour>> ColourArr;

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Surface* screen;

void SDL2init() {
	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* window = SDL_CreateWindow(
		"Software Ray Tracer",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		640,
		480,
		0
	);

	screen = SDL_GetWindowSurface(window);

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
}

void putpixel(SDL_Surface* surface, int x, int y, Uint32 pixel)
{
	int bpp = surface->format->BytesPerPixel;
	/* Here p is the address to the pixel we want to set */
	Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

	switch (bpp) {
	case 1:
		*p = pixel;
		break;

	case 2:
		*(Uint16*)p = pixel;
		break;

	case 3:
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
			p[0] = (pixel >> 16) & 0xff;
			p[1] = (pixel >> 8) & 0xff;
			p[2] = pixel & 0xff;
		}
		else {
			p[0] = pixel & 0xff;
			p[1] = (pixel >> 8) & 0xff;
			p[2] = (pixel >> 16) & 0xff;
		}
		break;

	case 4:
		*(Uint32*)p = pixel;
		break;
	}
}

void clamp255(Vec3f& col) {
	col.x = (col.x > 255) ? 255 : (col.x < 0) ? 0 : col.x;
	col.y = (col.y > 255) ? 255 : (col.y < 0) ? 0 : col.y;
	col.z = (col.z > 255) ? 255 : (col.z < 0) ? 0 : col.z;
}

double Hit_Sphere(const Point3f& centre, double radius, const Ray& r) {
	Vec3f oc = r.Origin() - centre;
	auto a = r.Direction().dotProduct(r.Direction());
	auto b = 2.0 * oc.dotProduct(r.Direction());
	auto c = oc.dotProduct(oc) - radius * radius;
	auto discriminant = b * b - 4 * a * c;
	if (discriminant < 0) return -1.0;
	else return (-b - sqrt(discriminant)) / (2.0 * a);
}

Colour Ray_Colour(const Ray& r, const Colour& background, const Hittable& world, int depth) {
	Hit_Record rec;
	if (depth <= 0) return Colour(0, 0, 0);
	if (!world.Hit(r, 0.001, infinity, rec)) return background;

	Ray scattered;
	Colour attentuation;
	Colour emitted = rec.mat_ptr->Emitted();
	if (!rec.mat_ptr->Scatter(r, rec, attentuation, scattered)) return emitted;

	return emitted + attentuation * Ray_Colour(scattered, background, world, depth - 1);
}

Colour CollectiveColour(Uint32 col, SDL_Surface* screen, int spp) {

	Uint8 red, green, blue;
	SDL_GetRGB(col, screen->format, &red, &green, &blue);

	Colour result(red, green, blue);
	result /= 255;
	result.r = result.r * result.r;
	result.g = result.g * result.g;
	result.b = result.b * result.b;
	result *= 255.f * spp;

	return result;
}

void ResetColours(ColourArr& arr) {
	for (int x = 0; x < screen->w; x++)
	{
		for (int y = 0; y < screen->h; y++)
		{
			arr[x][y] = { 0,0,0 };
		}
	}
}

void FullRender(SDL_Surface* screen, const float aspect_ratio, const int image_width, const int image_height,
	Camera& cam, Hittable_List& world, int spp, int max_depth)
{
	Colour background(0, 0, 0);

	Colour pix_col(0, 0, 0);
	for (int y = screen->h - 1; y >= 0; --y) {
		for (int x = 0; x < screen->w; ++x) {
			pix_col = { 0,0,0 };
			for (int s = 0; s < spp; s++) {
				auto u = double(x + Random_Double()) / (image_width - 1);
				auto v = double(y + Random_Double()) / (image_height - 1);
				Ray ray = cam.Get_Ray(u, v);
				Vec3f unit_direction = ray.Direction().normalize();
				auto t = 0.5 * (unit_direction.y + 1.0);
				background = (1.0 - t) * Colour(1.0, 1.0, 1.0) + t * Colour(0.5, 0.7, 1.0) * 255;
				pix_col = pix_col + Ray_Colour(ray, background, world, max_depth);
			}
			pix_col /= 255.f * spp;
			pix_col.x = sqrt(pix_col.x);
			pix_col.y = sqrt(pix_col.y);
			pix_col.z = sqrt(pix_col.z);
			pix_col *= 255;
			Uint32 colour = SDL_MapRGB(screen->format, pix_col.x, pix_col.y, pix_col.z);
			putpixel(screen, x, y, colour);
		}
	}
}

void RenderPixel(SDL_Surface* screen, const float aspect_ratio, const int image_width, const int image_height,
	Camera& cam, Hittable_List& world, ColourArr& colours, int x, int y, int spp, int max_depth)
{
	Colour background(0, 0, 0);
	Colour pix_col = colours[x][y];

	auto u = double(x + Random_Double()) / (image_width - 1);
	auto v = double(y + Random_Double()) / (image_height - 1);
	Ray ray = cam.Get_Ray(u, v);
	Vec3f unit_direction = ray.Direction().normalize();
	auto t = 0.5 * (unit_direction.y + 1.0);
	background = (1.0 - t) * Colour(1.0, 1.0, 1.0) + t * Colour(0.5, 0.7, 1.0) * 255;
	pix_col = pix_col + Ray_Colour(ray, background, world, max_depth);

	colours[x][y] = pix_col;

	pix_col /= 255.f * spp;
	pix_col.r = sqrt(pix_col.r);
	pix_col.g = sqrt(pix_col.g);
	pix_col.b = sqrt(pix_col.b);
	pix_col *= 255;
	Uint32 colour = SDL_MapRGB(screen->format, pix_col.r, pix_col.g, pix_col.b);
	putpixel(screen, x, y, colour);
}

void Movement(ColourArr& tc, int& spp, Point3f& lf, Camera& cam, Vec3f dir) {
	ResetColours(tc);
	spp = 1;
	lf += dir;
	cam.LookFrom(lf);
}

Hittable_List Random_Scene() {
	Hittable_List world;
	auto ground_material = std::make_shared<Lambertian>(Colour(0.5, 0.5, 0.5));
	world.Add(std::make_shared<Sphere>(Point3f(0, -1000, 0), 1000, ground_material));

	for (int a = -11; a < 11; a++)
	{
		for (int b = -11; b < 11; b++)
		{
			auto choose_mat = Random_Double();
			Point3f centre(a + 0.9 * Random_Double(), 0.2, b + 0.9 * Random_Double());
			if ((centre - Point3f(4, 0.2, 0)).length() > 0.9) {
				std::shared_ptr<Material> sphere_material;
				if (choose_mat < 0.8) {
					//	Diffuse
					auto albedo = Colour::Random() * Colour::Random();
					sphere_material = std::make_shared<Lambertian>(albedo);
					world.Add(std::make_shared<Sphere>(centre, 0.2, sphere_material));
				}
				else if (choose_mat < 0.90) {
					//	Metal
					auto albedo = Colour::Random(0.5, 1);
					auto fuzz = Random_Double(0, 0.5);
					sphere_material = std::make_shared<Metal>(albedo, fuzz);
					world.Add(std::make_shared<Sphere>(centre, 0.2, sphere_material));
				}
				else {
					//	Glass
					sphere_material = std::make_shared<Dielectric>(1.5);
					world.Add(std::make_shared<Sphere>(centre, 0.2, sphere_material));
				}
			}
		}
	}
	auto material1 = std::make_shared<Dielectric>(1.5);
	world.Add(std::make_shared<Sphere>(Point3f(0, 1, 0), 1.0, material1));
	auto material2 = std::make_shared<Lambertian>(Colour(0.4, 0.2, 0.1));
	world.Add(std::make_shared<Sphere>(Point3f(-4, 1, 0), 1.0, material2));
	auto material3 = std::make_shared<Metal>(Colour(0.7, 0.6, 0.5), 0.0);
	world.Add(std::make_shared<Sphere>(Point3f(4, 1, 0), 1.0, material3));
	auto material4 = std::make_shared<Diffuse_Light>(Colour(255, 255, 255));
	world.Add(std::make_shared<Sphere>(Point3f(0, 3, 0), 0.5, material4));

	return Hittable_List(std::make_shared<BVH_Node>(world));
}

Hittable_List Test_Scene() {
	Hittable_List world;

	Model* model = new Model("res/cc_t");

	Vec3f transform(0, 0.8, 0);
	auto glass = std::make_shared<Dielectric>(1.5);
	model->AddToWorld(world, transform, glass);

	transform = Vec3f(1.2, 0.8, 0);
	auto mat_diffuse = std::make_shared<Lambertian>(Colour(0.4, 0.2, 0.1));
	model->AddToWorld(world, transform, mat_diffuse);

	transform = Vec3f(-1.2, 0.8, 0);
	auto mat_metal = std::make_shared<Metal>(Colour(0.5, 0.6, 0.5), 0.0);
	model->AddToWorld(world, transform, mat_metal);

	auto ground_material = std::make_shared<Lambertian>(Colour(0.5, 0.5, 0.5));
	world.Add(std::make_shared<Sphere>(Point3f(0, -1000, 0), 1000, ground_material));

	auto material4 = std::make_shared<Diffuse_Light>(Colour(255, 255, 255));
	world.Add(std::make_shared<Sphere>(Point3f(0, 5, 0), 0.5, material4));

	return Hittable_List(std::make_shared<BVH_Node>(world));
}

Hittable_List Small_Scene() {
	Hittable_List world;
	auto material1 = std::make_shared<Dielectric>(1.5);
	world.Add(std::make_shared<Sphere>(Point3f(0, 1, 0), 1.0, material1));
	auto material2 = std::make_shared<Lambertian>(Colour(0.4, 0.2, 0.1));
	world.Add(std::make_shared<Sphere>(Point3f(-4, 1, 0), 1.0, material2));
	auto material3 = std::make_shared<Metal>(Colour(0.7, 0.6, 0.5), 0.0);
	world.Add(std::make_shared<Sphere>(Point3f(4, 1, 0), 1.0, material3));
	auto material4 = std::make_shared<Diffuse_Light>(Colour(255, 255, 255));
	world.Add(std::make_shared<Sphere>(Point3f(0, 3, 0), 0.5, material4));
	auto ground_material = std::make_shared<Lambertian>(Colour(0.5, 0.5, 0.5));
	world.Add(std::make_shared<Sphere>(Point3f(0, -1000, 0), 1000, ground_material));
	return Hittable_List(std::make_shared<BVH_Node>(world));
}

//TODO pass this as a reference to make it more readable?
std::vector<std::shared_ptr<Hittable>> hittableNodes;
void Traverse_Tree(std::shared_ptr<Hittable> n) {
	if (n == nullptr) return;

	hittableNodes.push_back(n);
	std::cout << n->id << "\n";

	Traverse_Tree(n->Left());
	Traverse_Tree(n->Right());
}

std::shared_ptr<Hittable> Create_Tree(std::vector<Hittable*>& objs)
{
	if (objs.size() == 0) return nullptr;

	int idx = objs.front()->id;

	std::shared_ptr<Hittable> node;
	if (idx == 1) { // BVH node
		BVH_Node* bvh = (BVH_Node*)objs.front();
		node = std::make_shared<BVH_Node>(bvh->box);
		objs.erase(begin(objs));
	}
	else if (idx == 2) { // Triangle
		Triangle* tri = (Triangle*)objs.front();
		std::shared_ptr<Hittable> n = std::make_shared<Triangle>(
			tri->v0, tri->v1, tri->v2,
			tri->v0n, tri->v1n, tri->v2n,
			nullptr
			);
		objs.erase(begin(objs));
		return n;
	}
	else if (idx == 3) { //	sphere
		Sphere* sph = (Sphere*)objs.front();
		std::shared_ptr<Hittable> m = std::make_shared<Sphere>(sph->centre, sph->radius, nullptr);
		objs.erase(begin(objs));
		return m;
	}

	node->Left(Create_Tree(objs));
	node->Right(Create_Tree(objs));

	return node;
}

int main(int argc, char** argv)
{
	SDL2init();

	const float aspect_ratio = 16.0 / 9;
	const int image_width = screen->w;
	const int image_height = static_cast<int>(image_width / aspect_ratio);
	int spp = 1;
	const int max_depth = 50;

	Point3f lookfrom(0, 4, 15);
	Point3f lookat(0, 0, 0);
	Vec3f vup(0, 1, 0);
	auto dist_to_focus = 15.0;
	auto aperature = 0.15;

	//	TODO: Use lookfrom as pointer so only needs to be passed once?
	Camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperature, dist_to_focus);

	ColourArr totalColour;
	for (int i = 0; i < screen->w; i++)
	{
		std::vector<Colour> arr;
		for (int b = 0; b < screen->h; b++)
		{
			arr.push_back(Colour(0, 0, 0));
		}
		totalColour.emplace_back(arr);
	}
	ResetColours(totalColour);

	//Hittable_List world;
	//std::string filenamebvh = "mat-scene.bvh";
	//std::ifstream infile("res/binary/" + filenamebvh, std::ifstream::binary);
	//if (!infile) {
	//	Timer t("BVH file generation: ");
	//	world = Small_Scene();
	//	Traverse_Tree(world.objects.front());
	//	Write(aabbNodes, filenamebvh);
	//}
	//else {
	//	Timer t("BVH file read: ");
	//	std::vector<AABB> readObject = Read(filenamebvh);
	//	std::shared_ptr<Hittable> bvh;
	//	bvh = Create_Tree(readObject);
	//	world = Test_Scene_BVH(bvh);
	//}

	const char* file = "jesusplease.bvh";
	//auto world = Small_Scene();
	//Traverse_Tree(world.objects.front());
	//WriteNode(hittableNodes, file);
	std::vector<Hittable*> a = ReadNode(file);
	auto root = Create_Tree(a);
	Hittable_List world;
	world.Add(root);

	SDL_Event e;
	bool running = true;
	while (running) {
		SDL_FillRect(screen, nullptr, SDL_MapRGB(screen->format, 0, 0, 0));
		SDL_RenderClear(renderer);
		std::cout << "\r          \rSPP: " << spp - 1;

		//=========================== Single thread =========================== 
		{
			//Timer t("Single-thread frame render time: ");
			//FullRender(screen, aspect_ratio, image_width, image_height, cam, world, spp, max_depth);
		}

		//=========================== 100% CPU - multi-thread =========================== 
		{
			ThreadPool pool(std::thread::hardware_concurrency());
			//Timer t("Frame render time: ");
			for (int x = screen->w - 1; x >= 0; x--) {
				for (int y = screen->h - 1; y >= 0; y--)
				{
					pool.Enqueue(std::bind(RenderPixel, screen, aspect_ratio, image_width, image_height, std::ref(cam), std::ref(world), std::ref(totalColour), x, y, spp, max_depth));
				}
			}
		}
		spp++;

		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, screen);
		if (texture == NULL) {
			fprintf(stderr, "CreateTextureFromSurface failed: %s\n", SDL_GetError());
			exit(1);
		}
		SDL_FreeSurface(screen);
		SDL_RenderCopyEx(renderer, texture, NULL, NULL, 0, 0, SDL_FLIP_VERTICAL);
		SDL_RenderPresent(renderer);

		SDL_DestroyTexture(texture);

		while (SDL_PollEvent(&e))
		{
			switch (e.type) {
			case SDL_KEYDOWN:
				switch (e.key.keysym.sym) {
				case SDLK_ESCAPE:
					running = false;
					break;
				case SDLK_LEFT:
				case SDLK_a:
					Movement(totalColour, spp, lookfrom, cam, { -1,0,0 });
					break;

				case SDLK_RIGHT:
				case SDLK_d:
					Movement(totalColour, spp, lookfrom, cam, { 1,0,0 });
					break;

				case SDLK_UP:
				case SDLK_w:
					Movement(totalColour, spp, lookfrom, cam, { 0,0,-1 });
					break;

				case SDLK_DOWN:
				case SDLK_s:
					Movement(totalColour, spp, lookfrom, cam, { 0,0,1 });
					break;
				}
			}
		}
	}

	return 0;
}
