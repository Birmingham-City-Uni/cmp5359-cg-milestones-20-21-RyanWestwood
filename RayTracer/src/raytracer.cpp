// A practical implementation of the ray tracing algorithm.
#include "geometry.h"
#include "ray.h"
#include "common.h"
#include "hittable_list.h"
#include "camera.h"
#include "Sphere.h"
#include "SDL.h" 
#include <fstream>
#include <chrono>
#include "material.h"
#include <thread>
#include "threadpool.h"
#include "model.h"
#include "Triangle.h"

#define M_PI 3.14159265359

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Surface* screen;

void init() {
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

// method to ensure colours don’t go out of 8 bit range in RGB​
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

Colour Ray_Colour(const Ray& r, const Hittable& world, int depth) {
	Hit_Record rec;
	if (depth <= 0) return Colour(0, 0, 0);
	if (world.Hit(r, 0.001, infinity, rec)) {
		Ray scattered;
		Colour attentuation;
		if (rec.mat_ptr->Scatter(r, rec, attentuation, scattered))
			return attentuation * Ray_Colour(scattered, world, depth - 1);
		return Colour(0, 0, 0);
	}
	Vec3f unit_direction = r.Direction().normalize();
	auto t = 0.5 * (unit_direction.y + 1.0);
	return (1.0 - t) * Colour(1.0, 1.0, 1.0) + t * Colour(0.5, 0.7, 1.0) * 255;
}

//	TODO: Use this if passing screen is thread unsafe.
struct Result {
	int x, y;
	Uint32 colour;
};

void FullRender(SDL_Surface* screen, const float aspect_ratio, const int image_width, const int image_height,
	Camera& cam, Hittable_List& world, int spp, int max_depth)
{
	const Colour black(0, 0, 0);
	Colour pix_col(black);

	pix_col = black;

	for (int y = screen->h - 1; y >= 0; --y) {
		for (int x = 0; x < screen->w; ++x) {
			pix_col = black;
			for (int s = 0; s < spp; s++) {
				auto u = double(x + Random_Double()) / (image_width - 1);
				auto v = double(y + Random_Double()) / (image_height - 1);
				Ray ray = cam.Get_Ray(u, v);
				pix_col = pix_col + Ray_Colour(ray, world, max_depth);
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
	Camera& cam, Hittable_List& world, int x, int y, int spp, int max_depth)
{
	const Colour black(0, 0, 0);
	Colour pix_col(black);

	pix_col = black;
	for (int s = 0; s < spp; s++) {
		auto u = double(x + Random_Double()) / (image_width - 1);
		auto v = double(y + Random_Double()) / (image_height - 1);
		Ray ray = cam.Get_Ray(u, v);
		pix_col = pix_col + Ray_Colour(ray, world, max_depth);
	}
	pix_col /= 255.f * spp;
	pix_col.x = sqrt(pix_col.x);
	pix_col.y = sqrt(pix_col.y);
	pix_col.z = sqrt(pix_col.z);
	pix_col *= 255;
	Uint32 colour = SDL_MapRGB(screen->format, pix_col.x, pix_col.y, pix_col.z);
	putpixel(screen, x, y, colour);
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
				else if (choose_mat < 0.95) {
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
	return world;
}

Hittable_List Test_Scene() {
	Hittable_List world;

	Model* model = new Model("res/cc.obj");

	Vec3f transform(0, 0.8, 0);
	auto glass = std::make_shared<Dielectric>(1.5);

	for (uint32_t i = 0; i < model->nfaces(); i++)
	{
		const Vec3f& v0 = model->vert(model->face(i)[0]);
		const Vec3f& v1 = model->vert(model->face(i)[1]);
		const Vec3f& v2 = model->vert(model->face(i)[2]);

		world.Add(std::make_shared<Triangle>(v0 + transform, v1 + transform, v2 + transform, glass));
	}

	transform = Vec3f(1.2, 0.8, 0);
	auto mat_diffuse = std::make_shared<Lambertian>(Colour(0.4, 0.2, 0.1));
	for (uint32_t i = 0; i < model->nfaces(); ++i) {
		const Vec3f& v0 = model->vert(model->face(i)[0]);
		const Vec3f& v1 = model->vert(model->face(i)[1]);
		const Vec3f& v2 = model->vert(model->face(i)[2]);
		world.Add(std::make_shared<Triangle>(v0 + transform, v1 + transform, v2 + transform, mat_diffuse));
	}

	transform = Vec3f(-1.2, 0.8, 0);
	auto mat_metal = std::make_shared<Metal>(Colour(0.7, 0.6, 0.5), 0.0);
	mat_diffuse = std::make_shared<Lambertian>(Colour(0.8, 0.4, 0.7));
	for (uint32_t i = 0; i < model->nfaces(); ++i) {
		const Vec3f& v0 = model->vert(model->face(i)[0]);
		const Vec3f& v1 = model->vert(model->face(i)[1]);
		const Vec3f& v2 = model->vert(model->face(i)[2]);
		world.Add(std::make_shared<Triangle>(v0 + transform, v1 + transform, v2 + transform, mat_metal));
	}

	auto ground_material = std::make_shared<Lambertian>(Colour(0.5, 0.5, 0.5));
	world.Add(std::make_shared<Sphere>(Point3f(0, -1000, 0), 1000, ground_material));

	return world;
}

int main(int argc, char** argv)
{
	// initialise SDL2
	init();

	const float aspect_ratio = 16.0 / 9;
	const int image_width = screen->w;
	const int image_height = static_cast<int>(image_width / aspect_ratio);
	const int spp = 1;
	const int max_depth = 50;

	auto world = Random_Scene();
	std::cout << "Scene Created: \n";

	Point3f lookfrom(13, 2, 3);
	Point3f lookat(0, 0, 0);
	Vec3f vup(0, 1, 0);
	auto dist_to_focus = 10.0;
	auto aperature = 0.15;

	Camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperature, dist_to_focus);

	SDL_Event e;
	bool running = true;
	while (running) {
		SDL_FillRect(screen, nullptr, SDL_MapRGB(screen->format, 0, 0, 0));
		SDL_RenderClear(renderer);

		//=========================== START ===========================  

		////=========================== Single thread =========================== 
		//auto t_start = std::chrono::high_resolution_clock::now();

		//FullRender(screen, world, spp, max_depth);

		//auto t_end = std::chrono::high_resolution_clock::now();
		//auto passedTime = std::chrono::duration<double, std::milli>(t_end - t_start).count();
		//std::cerr << "Single-thread render time:  " << passedTime << " ms\n";


		//=========================== 100% CPU - multi-thread =========================== 
		auto t_start = std::chrono::high_resolution_clock::now();
		{
			t_start = std::chrono::high_resolution_clock::now();
			ThreadPool pool(std::thread::hardware_concurrency());

			for (int x = screen->w - 1; x >= 0; x--) {
				for (int y = screen->h - 1; y >= 0; y--)
				{
					pool.Enqueue(std::bind(RenderPixel, screen, aspect_ratio, image_width, image_height, std::ref(cam), std::ref(world), x, y, spp, max_depth));
				}
			}
		}
		auto t_end = std::chrono::high_resolution_clock::now();
		auto passedTime = std::chrono::duration<double, std::milli>(t_end - t_start).count();
		std::cerr << "\tThreadpool multi-threaded render time:  " << passedTime / 1000 << " seconds\n";

		//=========================== END ===========================  

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
				}
				break;
			}
		}
	}

	return 0;
}
