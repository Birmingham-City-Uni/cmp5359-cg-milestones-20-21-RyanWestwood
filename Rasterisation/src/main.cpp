#include <vector>
#include <cmath>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <memory>

#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const int width = 1080;
const int height = 1080;
std::unique_ptr<Model> model;

void Line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {
	double dx = abs(x1 - x0);
	double sx = x0 < x1 ? 1 : -1;
	double dy = -abs(y1 - y0);
	double sy = y0 < y1 ? 1 : -1;
	double err = dx + dy;

	while (true) {
		image.set(x0, y0, color);
		if (x0 == x1 && y0 == y1) break;
		double e2 = 2 * err;
		if (e2 >= dy) {
			err += dy;
			x0 += sx;
		}
		if (e2 <= dx) {
			err += dx;
			y0 += sy;
		}
	}
}

std::pair<Vec2i, Vec2i> TriangleBoundingBox(std::vector<Vec2i> points, TGAImage& image) {

	int minX = std::min(points[0].x, std::min(points[1].x, points[2].x));
	int maxX = std::max(points[0].x, std::max(points[1].x, points[2].x));

	int minY = std::min(points[0].y, std::min(points[1].y, points[2].y));
	int maxY = std::max(points[0].y, std::max(points[1].y, points[2].y));

#ifdef _DEBUG
	Line(minX, minY, maxX, minY, image, red);
	Line(maxX, maxY, minX, maxY, image, red);
	Line(minX, minY, minX, maxY, image, red);
	Line(maxX, minY, maxX, maxY, image, red);
#endif

	return { {minX,minY},{maxX, maxY} };
}

inline bool InTriangle(Vec2i a, Vec2i b, Vec2i c, Vec2i p) {

	float denominator = ((b.y - c.y) * (a.x - c.x) + (c.x - b.x) * (a.y - c.y));
	float aa = ((b.y - c.y) * (p.x - c.x) + (c.x - b.x) * (p.y - c.y)) / denominator;
	float bb = ((c.y - a.y) * (p.x - c.x) + (a.x - c.x) * (p.y - c.y)) / denominator;
	float cc = 1 - aa - bb;

	return 0 <= aa && aa <= 1 && 0 <= bb && bb <= 1 && 0 <= cc && cc <= 1;
}

void Triangle(std::vector<Vec2i> p, TGAImage& image, TGAColor color) {

	std::pair<Vec2i, Vec2i> bbox = TriangleBoundingBox(p, image);

	for (int x = bbox.first.x; x < bbox.second.x; x++)
		for (int y = bbox.first.y; y < bbox.second.y; y++)
			if (InTriangle(p[0], p[1], p[2], { x,y }))
				image.set(x, y, color);
}

int main(int argc, char** argv) {

	2 == argc ? model = std::make_unique<Model>(Model(argv[1])) : model = std::make_unique<Model>(Model("res/cc.obj"));

	TGAImage image(width, height, TGAImage::RGB);
	Vec3f light_dir(0, 0, -1);
	for (int i = 0; i < model->nfaces(); i++) {
		std::vector<int> face = model->face(i);
		std::vector<Vec2i> screen_coords(3, Vec2i());
		std::vector<Vec3f> world_coords(3, Vec3f());
		for (int j = 0; j < 3; j++) {
			Vec3f v = model->vert(face[j]);
			screen_coords[j] = Vec2i((v.x + 1) * width / 2, (v.y + 1) * height / 2);
			world_coords[j] = v;
		}
		Vec3f n = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);
		n.normalize();
		float intensity = n * light_dir;
		if (intensity > 0) {
			Triangle(screen_coords, image, TGAColor(intensity * 255, intensity * 255, intensity * 255, 255));
		}
	}
	image.flip_vertically();
	image.write_tga_file("res/carlo.tga");

	return 0;
}