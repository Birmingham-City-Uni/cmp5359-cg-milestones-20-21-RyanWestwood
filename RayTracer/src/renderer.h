#pragma once
#define STB_IMAGE_WRITE_STATIC
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <vector>
#include "stb_image_write.h"
#include "geometry.h"
#include "camera.h"
#include "hittable_list.h"
#include "timer.h"
#include "threadpool.h"
#include "material.h"
#if defined(_WIN32) || defined(_WIN64)
#include <SDL.h>
#define M_PI 3.14159265359
#endif
#if defined(__unix__) || defined(__linux__)
#include <SDL2/SDL.h>
#endif

typedef std::vector<std::vector<Colour>> ColourArr;

constexpr int WIDTH = 1920;
constexpr int HEIGHT = 1080;

void putpixel(SDL_Surface* surface, int x, int y, Uint32 pixel);

Colour Ray_Colour(const Ray& r, const Colour& background, const Hittable& world, int depth);

void Image_Write(ColourArr& image, int spp);

void RenderPixel(SDL_Surface* screen, const int image_width, const int image_height,
	Camera& cam, Hittable_List& world, ColourArr& colours, int x, int y, int spp, int max_depth);

void InteractiveRender(SDL_Surface* screen, const int image_width, const int image_height,
	Camera& cam, Hittable_List& world, ColourArr& colours, int& spp, int max_depth);

void StaticRender(SDL_Surface* screen, const int image_width, const int image_height,
	Camera& cam, Hittable_List& world, ColourArr& colours, int& spp, int max_depth, int quality);