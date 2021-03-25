#include <vector>
#include <cmath>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
Model* model = nullptr;
const int width = 1080;
const int height = 1080;

void Line(Vec2i a, Vec2i b, TGAImage& image, TGAColor color) {

	bool steep = false;
	if (abs(a.x - b.x) < abs(a.y - b.y)) {
		std::swap(a.x, a.y);
		std::swap(b.x, b.y);
		steep = true;
	}

	if (a.x > b.x) {
		std::swap(a.x, b.x);
		std::swap(a.y, b.y);
	}

	for (int x = a.x; x <= b.x; x++)
	{
		float t = (x - float(a.x)) / (b.x - a.x);
		float y = a.y * (1.0f - t) + (b.y * t);
		if (steep)
		{
			image.set(y, x, color);
		}
		else {
			image.set(x, y, color);
		}
	}
}

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

void Find() {

}

std::pair<Vec2i, Vec2i> TriangleBoundingBox(std::vector<Vec2i> points, TGAImage& image) {

	auto minXIter = std::min_element(begin(points), end(points), [](auto& a, auto& b)-> bool { return a.x < b.x; });
	int minX = Vec2i(*minXIter).x;
	auto minYIter = std::min_element(begin(points), end(points), [](auto& a, auto& b)-> bool { return a.y < b.y; });
	int minY = Vec2i(*minYIter).y;

	auto maxXIter = std::max_element(begin(points), end(points), [](auto& a, auto& b)-> bool { return a.x < b.x; });
	int maxX = Vec2i(*maxXIter).x;
	auto maxYIter = std::max_element(begin(points), end(points), [](auto& a, auto& b)-> bool { return a.y < b.y; });
	int maxY = Vec2i(*maxYIter).y;

	//Line(minX, minY, maxX, minY, image, red);
	//Line(maxX, maxY, minX, maxY, image, red);
	//Line(minX, minY, minX, maxY, image, red);
	//Line(maxX, minY, maxX, maxY, image, red);

	return { {minX, minY}, {maxX, maxY} };
}

bool InTriangle(Vec2i a, Vec2i b, Vec2i c, Vec2i p) {

	float denominator = ((b.y - c.y) * (a.x - c.x) + (c.x - b.x) * (a.y - c.y));
	float aa = ((b.y - c.y) * (p.x - c.x) + (c.x - b.x) * (p.y - c.y)) / denominator;
	float bb = ((c.y - a.y) * (p.x - c.x) + (a.x - c.x) * (p.y - c.y)) / denominator;
	float cc = 1 - aa - bb;

	return 0 <= aa && aa <= 1 && 0 <= bb && bb <= 1 && 0 <= cc && cc <= 1;
}

void Triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage& image, TGAColor color) {

	std::pair<Vec2i, Vec2i> bbox = TriangleBoundingBox({ t0, t1, t2 }, image);

	for (int x = bbox.first.x; x < bbox.second.x; x++)
	{
		for (int y = bbox.first.y; y < bbox.second.y; y++)
		{
			if (InTriangle(t0, t1, t2, { x,y })) {
				image.set(x, y, color);
			}
		}
	}

	//if (t0.y == t1.y && t0.y == t2.y) return;

	//if (t0.y > t1.y) std::swap(t0, t1);
	//if (t0.y > t2.y) std::swap(t0, t2);
	//if (t1.y > t2.y) std::swap(t1, t2);

	//float totalHeight = t2.y - t0.y;

	//for (int i = t0.y; i < t1.y; i++)
	//{
	//	float segment_height = t1.y - t0.y + 1;
	//	float alpha = (i - t0.y) / totalHeight;
	//	float beta = (i - t0.y) / segment_height;
	//	Vec2i A = t0 + (t2 - t0) * alpha;
	//	Vec2i B = t0 + (t1 - t0) * beta;

	//	if (A.x > B.x) std::swap(A, B);
	//	for (int j = A.x; j < B.x; j++)
	//	{
	//		image.set(j, i, color);
	//	}
	//}

	//for (int i = t1.y; i < t2.y; i++)
	//{
	//	float segment_height = t2.y - t1.y + 1;
	//	float alpha = (i - t0.y) / totalHeight;
	//	float beta = (i - t1.y) / segment_height;
	//	Vec2i A = t0 + (t2 - t0) * alpha;
	//	Vec2i B = t1 + (t2 - t1) * beta;

	//	if (A.x > B.x) std::swap(A, B);
	//	for (int j = A.x; j < B.x; j++)
	//	{
	//		image.set(j, i, color);
	//	}
	//}
}

int main(int argc, char** argv) {

	if (2 == argc) {
		model = new Model(argv[1]);
	}
	else {
		model = new Model("cc.obj");
	}

	TGAImage image(width, height, TGAImage::RGB);
	Vec3f light_dir(0, 0, -1);
	for (int i = 0; i < model->nfaces(); i++) {
		std::vector<int> face = model->face(i);
		Vec2i screen_coords[3];
		Vec3f world_coords[3];
		for (int j = 0; j < 3; j++) {
			Vec3f v = model->vert(face[j]);
			screen_coords[j] = Vec2i((v.x + 1) * width / 2, (v.y + 1) * height / 2);
			world_coords[j] = v;
		}
		Vec3f n = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);
		n.normalize();
		float intensity = n * light_dir;
		if (intensity > 0) {
			Triangle(screen_coords[0], screen_coords[1], screen_coords[2], image, TGAColor(intensity * 255, intensity * 255, intensity * 255, 255));
		}
	}
	image.flip_vertically();
	image.write_tga_file("output1.tga");

	delete model;

	return 0;
}