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

void FullRender(SDL_Surface* screen, Hittable_List world, int spp, int max_depth) {
	const float aspect_ratio = 16.0 / 9;
	const int image_width = screen->w;
	const int image_height = static_cast<int>(image_width / aspect_ratio);

	Camera cam;
	const Colour black(0, 0, 0);
	Colour pix_col(black);

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

void PartialRender(SDL_Surface* screen, Hittable_List world, int start, int step, int spp, int max_depth) {
	const float aspect_ratio = 16.0 / 9;
	const int image_width = screen->w;
	const int image_height = static_cast<int>(image_width / aspect_ratio);

	Camera cam;

	const Colour black(0, 0, 0);
	Colour pix_col(black);

	for (int y = start; y >= start - step; --y) {
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

void LineRender(SDL_Surface* screen, Hittable_List world, int y, int spp, int max_depth) {
	const float aspect_ratio = 16.0 / 9;
	const int image_width = screen->w;
	const int image_height = static_cast<int>(image_width / aspect_ratio);

	Camera cam;
	const Colour black(0, 0, 0);
	Colour pix_col(black);

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

#include "threadpool.h"
int main(int argc, char** argv)
{
	// initialise SDL2
	init();

	const int spp = 50;
	const int max_depth = 50;

	//	TODO: remove these and pass to threaded function?
	const float aspect_ratio = 16.0 / 9;
	const int image_width = screen->w;
	const int image_height = static_cast<int>(image_width / aspect_ratio);

	Camera cam;

	Hittable_List world;

	auto material_ground = std::make_shared<Lambertian>(Colour(0.1, 0.6, 0.9));
	auto material_center = std::make_shared<Lambertian>(Colour(0.7, 0.4, 0.3));
	auto material_left = std::make_shared<Metal>(Colour(0.8, 0.4, 0.3), 0.3);
	auto material_right = std::make_shared<Metal>(Colour(0.2, 0.7, 0.5), 1.0);

	auto material_egg = std::make_shared<Lambertian>(Colour(0.96, 0.62, 0.384));

	world.Add(std::make_shared<Sphere>(Point3f(0.0, -100.5, -1.0), 100, material_ground));
	world.Add(std::make_shared<Sphere>(Point3f(0.0, 0.0, -1.0), 0.5, material_egg));
	world.Add(std::make_shared<Sphere>(Point3f(-1.0, 0.0, -1.0), 0.5, material_egg));
	world.Add(std::make_shared<Sphere>(Point3f(1.0, 0.0, -1.0), 0.5, material_egg));

	const Colour white(255, 255, 255);
	const Colour black(0, 0, 0);
	const Colour red(255, 0, 0);

	double t;
	Colour pix_col(black);

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


		////========================= Naive split multi-thread =========================== 
		//auto t_start = std::chrono::high_resolution_clock::now();

		//auto cores = std::thread::hardware_concurrency();
		//std::vector<std::thread> threads;

		//int start = screen->h - 1;
		//int step = screen->h / cores;
		//for (auto i = 0; i < cores; i++)
		//{
		//	if (start - step < 0) step--;
		//	threads.emplace_back(std::thread(PartialRender, screen, world, start, step, spp, max_depth));
		//	start -= step;
		//}

		//for (auto& thread : threads) {
		//	thread.join();
		//}

		//auto t_end = std::chrono::high_resolution_clock::now();
		//auto passedTime = std::chrono::duration<double, std::milli>(t_end - t_start).count();
		//std::cerr << "Naive multi-threaded render time:  " << passedTime << " ms\n";

		//=========================== 100% CPU - multi-thread =========================== 
		auto t_start = std::chrono::high_resolution_clock::now();
		{
			t_start = std::chrono::high_resolution_clock::now();
			ThreadPool pool(std::thread::hardware_concurrency());

			for (int y = screen->h - 1; y >= 0; y--)
					{
						pool.Enqueue(std::bind(LineRender, screen, world, y, spp, max_depth));
					}
		}
		auto t_end = std::chrono::high_resolution_clock::now();
		auto passedTime = std::chrono::duration<double, std::milli>(t_end - t_start).count();
		std::cerr << "Threadpool multi-threaded render time:  " << passedTime << " ms\n";

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

		if (SDL_PollEvent(&e))
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
