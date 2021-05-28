#include "ray.h"
#include "common.h"
#include "fileparser.h"
#include "timer.h"
#include "enum.h"
#include "scene.h"
#include "renderer.h"
#include "tree.h"
#if defined(_WIN32) || defined(_WIN64)
#include <SDL.h>
#define M_PI 3.14159265359
std::string file = "win-scene";
const char* renderFile = "./res/output/render-win.tga";
#endif
#if defined(__unix__) || defined(__linux__)
#include <SDL2/SDL.h>
std::string file = "linux-scene";
const char* renderFile = "./res/output/render-linux.tga";
#endif
#include <fstream>
#include <vector>
#include <memory>

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Surface* screen;

void SDL2init() {
	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* window = SDL_CreateWindow(
		"Software Ray Tracer",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		WIDTH,
		HEIGHT,
		0
	);

	screen = SDL_GetWindowSurface(window);

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
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

void Movement(ColourArr& tc, int& spp, Point3f& lf, Camera& cam, Vec3f dir) {
	ResetColours(tc);
	spp = 1;
	lf += dir;
	cam.LookFrom(lf);
}

int main(int argc, char** argv)
{
	SDL2init();

	const float aspect_ratio = 16.0 / 9;
	const int image_width = screen->w;
	const int image_height = static_cast<int>(image_width / aspect_ratio);
	int spp = 1;
	const int max_depth = 10;

	Point3f lookfrom(0, 4, 15);
	Point3f lookat(0, 0, 0);
	Vec3f vup(0, 1, 0);
	auto dist_to_focus = 15.0;
	auto aperature = 0.15;

	Camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperature, dist_to_focus);

	ColourArr totalColour;
	for (int i = 0; i < screen->w; i++)
	{
		std::vector<Colour> arr(screen->h);
		totalColour.push_back(arr);
	}
	ResetColours(totalColour);

	Hittable_List world;
	std::ifstream bvhfile("./res/binary/" + file + ".bvh", std::ifstream::binary);
	std::ifstream matfile("./res/binary/" + file + ".mtls", std::ifstream::binary);
	if (!bvhfile || !matfile) {
		std::vector<std::shared_ptr<Hittable>> nodes;
		std::vector<std::shared_ptr<Material>> mats;
		world = Test_Scene(mats);
		Traverse_Tree(world.objects.front(), nodes);
		WriteNode(nodes, file + ".bvh");
		WriteMaterials(mats, file + ".mtls");
	}
	else {
		std::vector<Material*> mtls = ReadMaterials(file + ".mtls");
		std::vector<Hittable*> nodes = ReadNode(file + ".bvh");
		auto materials = Create_Materials(mtls);
		auto root = Create_Tree(nodes, materials);
		world.Add(root);
	}

	SDL_Event e;
	bool running = true;
	while (running) {
		SDL_FillRect(screen, nullptr, SDL_MapRGB(screen->format, 0, 0, 0));
		SDL_RenderClear(renderer);

		InteractiveRender(screen, image_width, image_height, std::ref(cam), std::ref(world), std::ref(totalColour), spp, max_depth);
		//StaticRender(screen, image_width, image_height, std::ref(cam), std::ref(world), std::ref(totalColour), spp, max_depth, 10);

		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, screen);
		if (texture == nullptr) {
			fprintf(stderr, "CreateTextureFromSurface failed: %s\n", SDL_GetError());
			exit(1);
		}
		SDL_FreeSurface(screen);
		SDL_RenderCopyEx(renderer, texture, nullptr, nullptr, 0, 0, SDL_FLIP_VERTICAL);
		SDL_RenderPresent(renderer);

		SDL_DestroyTexture(texture);

		while (SDL_PollEvent(&e) != 0)
		{
			switch (e.type) {
			case SDL_KEYDOWN:
				switch (e.key.keysym.sym) {
				case SDLK_ESCAPE:
					// image_write(totalColour, spp);
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
