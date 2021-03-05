#include <vector>
#include <cmath>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
Model* model = NULL;
const int width = 800;
const int height = 800;

struct Point
{
	int x, y;
};

void Line(Point a, Point b, TGAImage& image, TGAColor color) {

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

//void Line(double x0, double y0, double x1, double y1, TGAImage& image, TGAColor color) {
//	double deltaX = x1 - x0;
//	double deltaY = y1 - y0;
//
//	double deltaErr = abs(deltaY / deltaX);
//
//	double error = 0.0;
//
//	int y = y0;
//	for (int x = x0; x < x1; x++)
//	{
//		image.set(x, y, color);
//		error += deltaErr;
//		if (error >= 0.5) {
//			y += sin(deltaY);
//			error -= 1.0;
//		}
//	}
//}

//void Line(double x0, double y0, double x1, double y1, TGAImage& image, TGAColor color) {
//
//	double dx = x1 - x0;
//	double dy = y1 - y0;
//	double yi = 1;
//
//	if (dy < 0) {
//		yi = -1;
//		dy = -dy;
//	}
//	double D = (2 * dy) - dx;
//	double y = y0;
//
//	for (int x = x0; x < x1; x++)
//	{
//		image.set(x, y, color);
//		if (D > 0) {
//			y = y + yi;
//			D = D + (2 * (dy - dx));
//		}
//		else {
//			D = D + 2 * dy;
//		}
//	}
//}

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

int main(int argc, char** argv) {

	if (2 == argc) {
		model = new Model(argv[1]);
	}
	else {
		model = new Model("./res/cc.obj");
	}

	TGAImage image(width, height, TGAImage::RGB);
	for (int i = 0; i < model->nfaces(); i++) {
		std::vector<int> face = model->face(i);
		for (int j = 0; j < 3; j++) {
			Vec3f v0 = model->vert(face[j]);
			Vec3f v1 = model->vert(face[(j + 1) % 3]);

			int x0 = (v0.x + 1.) * width / 2;
			int y0 = (v0.y + 1.) * height / 2;
			int x1 = (v1.x + 1.) * width / 2;
			int y1 = (v1.y + 1.) * height / 2;

			Line(x0, y0, x1, y1, image, white);
		}
	}
	image.flip_vertically();
	image.write_tga_file("./res/output.tga");

	delete model;

	return 0;
}

