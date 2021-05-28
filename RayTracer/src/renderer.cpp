#include "renderer.h"

extern const char* renderFile;

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

Colour Ray_Colour(const Ray& r, const Colour& background, const Hittable& world, int depth) {
	Hit_Record rec;
	if (depth <= 0) {
		return {0, 0, 0};
	}
	if (!world.Hit(r, 0.001, infinity, rec)) {
		return background;
	}	
	Ray scattered;
	Colour attentuation;
	Colour emitted = rec.mat_ptr->Emitted();
	if (!rec.mat_ptr->Scatter(r, rec, attentuation, scattered)) {
		return emitted;
	}
	return emitted + attentuation * Ray_Colour(scattered, background, world, depth - 1);
}

void Image_Write(ColourArr& image, int spp)
{
	unsigned char* img1920x1080_rgb = new unsigned char[WIDTH * HEIGHT * 3];

	for (int i = 0; i < WIDTH * HEIGHT; i++) {
		auto x = i % WIDTH;
		auto y = i / WIDTH;
		auto col = image[x][y];		

		col /= 255.f * spp - 1;
		col.r = sqrt(col.r);
		col.g = sqrt(col.g);
		col.b = sqrt(col.b);
		col *= 255;

		img1920x1080_rgb[i * 3 + 0] = col.r;
		img1920x1080_rgb[i * 3 + 1] = col.g;
		img1920x1080_rgb[i * 3 + 2] = col.b;
	}
	stbi_flip_vertically_on_write(1);
	stbi_write_tga(renderFile, WIDTH, HEIGHT, 3, img1920x1080_rgb);
}

void RenderPixel(SDL_Surface* screen, const int image_width, const int image_height,
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

void InteractiveRender(SDL_Surface* screen, const int image_width, const int image_height,
	Camera& cam, Hittable_List& world, ColourArr& colours, int& spp, int max_depth) {
		{
			Timer t("Frame render time: ");
			ThreadPool pool(std::thread::hardware_concurrency());
			for (int x = screen->w- 1; x >= 0; x--) {
				for (int y = screen->h - 1; y >= 0; y--)
				{
					pool.Enqueue(std::bind(RenderPixel, screen, image_width, image_height, std::ref(cam), std::ref(world), std::ref(colours), x, y, spp, max_depth));
				}
			}
			spp++;
		}
}

void StaticRender(SDL_Surface* screen, const int image_width, const int image_height,
	Camera& cam, Hittable_List& world, ColourArr& colours, int& spp, int max_depth, int renderQuality) {
		{
			Timer t("Scene render time: ");
			for (int i = 0; i < renderQuality; i++)
			{
				ThreadPool pool(std::thread::hardware_concurrency());
				for (int x = screen->w - 1; x >= 0; x--) {
					for (int y = screen->h - 1; y >= 0; y--)
					{
						pool.Enqueue(std::bind(RenderPixel, screen, image_width, image_height, std::ref(cam), std::ref(world), std::ref(colours), x, y, spp, max_depth));
					}
				}
				spp++;
			}
			Image_Write(colours, spp);
		}
}