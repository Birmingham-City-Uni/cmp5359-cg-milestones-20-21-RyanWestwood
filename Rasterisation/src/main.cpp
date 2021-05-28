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

#define M_PI 3.14159265359
static const float inchToMm = 25.4;
enum FitResolutionGate { kFill = 0, kOverscan };

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const int width = 640;
const int height = 480;
std::unique_ptr<Model> model;
Matrix44f worldToCamera;

const float nearClippingPlane = 1;
const float farClippingPlane = 1000;
float focalLength = 20; // in mm
// 35mm Full Aperture in inches
float filmApertureWidth = 0.980;
float filmApertureHeight = 0.735;


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

// Compute screen coordinates based on a physically-based camera model
// http://www.scratchapixel.com/lessons/3d-basic-rendering/3d-viewing-pinhole-camera
void computeScreenCoordinates(
	const float& filmApertureWidth,
	const float& filmApertureHeight,
	const uint32_t& imageWidth,
	const uint32_t& imageHeight,
	const FitResolutionGate& fitFilm,
	const float& nearClippingPlane,
	const float& focalLength,
	float& top, float& bottom, float& left, float& right
)
{
	float filmAspectRatio = filmApertureWidth / filmApertureHeight;
	float deviceAspectRatio = imageWidth / (float)imageHeight;

	top = ((filmApertureHeight * inchToMm / 2) / focalLength) * nearClippingPlane;
	right = ((filmApertureWidth * inchToMm / 2) / focalLength) * nearClippingPlane;

	// field of view (horizontal)
	float fov = 2 * 180 / M_PI * atan((filmApertureWidth * inchToMm / 2) / focalLength);
	std::cerr << "Field of view " << fov << std::endl;

	float xscale = 1;
	float yscale = 1;

	switch (fitFilm) {
	default:
	case kFill:
		if (filmAspectRatio > deviceAspectRatio) {
			xscale = deviceAspectRatio / filmAspectRatio;
		}
		else {
			yscale = filmAspectRatio / deviceAspectRatio;
		}
		break;
	case kOverscan:
		if (filmAspectRatio > deviceAspectRatio) {
			yscale = filmAspectRatio / deviceAspectRatio;
		}
		else {
			xscale = deviceAspectRatio / filmAspectRatio;
		}
		break;
	}

	right *= xscale;
	top *= yscale;

	bottom = -top;
	left = -right;
}

// Compute vertex raster screen coordinates.
// Vertices are defined in world space. They are then converted to camera space,
// then to NDC space (in the range [-1,1]) and then to raster space.
// The z-coordinates of the vertex in raster space is set with the z-coordinate
// of the vertex in camera space.
void convertToRaster(
	const Vec3f& vertexWorld,
	const Matrix44f& worldToCamera,
	const float& l,
	const float& r,
	const float& t,
	const float& b,
	const float& near,
	const uint32_t& imageWidth,
	const uint32_t& imageHeight,
	Vec3f& vertexRaster
)
{
	Vec3f vertCam;
	worldToCamera.multVecMatrix(vertexWorld, vertCam);

	Vec2f vertS;
	vertS.x = near * vertCam.x / -vertCam.z;
	vertS.y = near * vertCam.y / -vertCam.z;

	Vec2f vertNDC;
	vertNDC.x = 2 * vertS.x / (r - l) - (r + l) / (r - l);
	vertNDC.y = 2 * vertS.y / (t - b) - (t + b) / (t - b);

	vertexRaster.x = (vertNDC.x + 1) / 2 * imageWidth;
	vertexRaster.y = (1 - vertNDC.y) / 2 * imageHeight;
	vertexRaster.z = -vertCam.z;
}

float min3(const float& a, const float& b, const float& c)
{
	return std::min(a, std::min(b, c));
}

float max3(const float& a, const float& b, const float& c)
{
	return std::max(a, std::max(b, c));
}

float edgeFunction(const Vec3f& a, const Vec3f& b, const Vec3f& c)
{
	return (c[0] - a[0]) * (b[1] - a[1]) - (c[1] - a[1]) * (b[0] - a[0]);
}


Matrix44f lookAt(const Vec3f from, const Vec3f to, const Vec3f _tmp = Vec3f(0, 1, 0))
{
	Vec3f forward = (from - to).normalize();

	Vec3f tmp = _tmp;
	Vec3f right = tmp.normalize().crossProduct(forward);

	Vec3f up = forward.crossProduct(right);

	Matrix44f camToWorld;

	camToWorld[0][0] = right.x;
	camToWorld[0][1] = right.y;
	camToWorld[0][2] = right.z;
	camToWorld[1][0] = up.x;
	camToWorld[1][1] = up.y;
	camToWorld[1][2] = up.z;
	camToWorld[2][0] = forward.x;
	camToWorld[2][1] = forward.y;
	camToWorld[2][2] = forward.z;

	camToWorld[3][0] = from.x;
	camToWorld[3][1] = from.y;
	camToWorld[3][2] = from.z;

	return camToWorld;
}

int main(int argc, char** argv) {

	std::string dir = "./res/";
	std::string filename = dir + "tree_palmShort";

	2 == argc ? model = std::make_unique<Model>(Model(argv[1])) : model = std::make_unique<Model>(Model(filename));

	TGAImage image(width, height, TGAImage::RGB);

	Vec3f light_dir(0, 0, -1);
	// compute screen coordinates
	float t, b, l, r;

	// Calculate screen coordinates and store in t, b, l, r
	computeScreenCoordinates(
		filmApertureWidth, filmApertureHeight,
		width, height,
		kOverscan,
		nearClippingPlane,
		focalLength,
		t, b, l, r);

	Vec3f eye(0.F, 1.F, 3.F);
	Vec3f target(0.f, 0.5f, 0.f);
	Vec3f up(0.f, 1.f, 0.f);
	worldToCamera = lookAt(eye, target, up).inverse();

	// define the depth-buffer. Initialize depth buffer to far clipping plane.
	float* depthBuffer = new float[width * height];
	for (uint32_t i = 0; i < width * height; ++i) depthBuffer[i] = farClippingPlane;

	auto mats = model->mats();
	for (auto& face : model->faces()) {
		//	Vertex indexes
		const Vec3f& v0 = model->vert(face.vertexIndex[0]);
		const Vec3f& v1 = model->vert(face.vertexIndex[1]);
		const Vec3f& v2 = model->vert(face.vertexIndex[2]);

		//	Convert vert to raster space
		Vec3f v0Raster, v1Raster, v2Raster;
		convertToRaster(v0, worldToCamera, l, r, t, b, nearClippingPlane, width, height, v0Raster);
		convertToRaster(v1, worldToCamera, l, r, t, b, nearClippingPlane, width, height, v1Raster);
		convertToRaster(v2, worldToCamera, l, r, t, b, nearClippingPlane, width, height, v2Raster);

		// Precompute reciprocal of vertex z-coordinate
		v0Raster.z = 1 / v0Raster.z;
		v1Raster.z = 1 / v1Raster.z;
		v2Raster.z = 1 / v2Raster.z;

		//	Texture attributes
		Vec2f st0 = model->textureCoord(face.textureCoordsIndex[0]);
		Vec2f st1 = model->textureCoord(face.textureCoordsIndex[1]);
		Vec2f st2 = model->textureCoord(face.textureCoordsIndex[2]);
		st0 *= v0Raster.z;
		st1 *= v1Raster.z;
		st2 *= v2Raster.z;

		// Calculate the bounding box of the triangle defined by the vertices
		float xmin = min3(v0Raster.x, v1Raster.x, v2Raster.x);
		float ymin = min3(v0Raster.y, v1Raster.y, v2Raster.y);
		float xmax = max3(v0Raster.x, v1Raster.x, v2Raster.x);
		float ymax = max3(v0Raster.y, v1Raster.y, v2Raster.y);

		// the triangle is out of screen
		if (xmin > width - 1 || xmax < 0 || ymin > height - 1 || ymax < 0) continue;

		// sets the bounds of the rectangle for the raster triangle
		// be careful xmin/xmax/ymin/ymax can be negative. Don't cast to uint32_t
		uint32_t x0 = std::max(int32_t(0), (int32_t)(std::floor(xmin)));
		uint32_t x1 = std::min(int32_t(width) - 1, (int32_t)(std::floor(xmax)));
		uint32_t y0 = std::max(int32_t(0), (int32_t)(std::floor(ymin)));
		uint32_t y1 = std::min(int32_t(height) - 1, (int32_t)(std::floor(ymax)));

		// calculates the area of the triangle, used in determining barycentric coordinates
		float area = edgeFunction(v0Raster, v1Raster, v2Raster);

		// Inner loop - for every pixel of the bounding box enclosing the triangle
		for (uint32_t y = y0; y <= y1; ++y) {
			for (uint32_t x = x0; x <= x1; ++x) {
				Vec3f pixelSample(x + 0.5, y + 0.5, 0); // You could use multiple pixel samples for antialiasing!!
				// Calculate the area of the subtriangles for barycentric coordinates
				float w0 = edgeFunction(v1Raster, v2Raster, pixelSample);
				float w1 = edgeFunction(v2Raster, v0Raster, pixelSample);
				float w2 = edgeFunction(v0Raster, v1Raster, pixelSample);
				if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
					// divide by the area to give us our coefficients
					w0 /= area;
					w1 /= area;
					w2 /= area;
					float oneOverZ = v0Raster.z * w0 + v1Raster.z * w1 + v2Raster.z * w2; // reciprocal for depth testing
					float z = 1 / oneOverZ;
					// Depth-buffer test
					if (z < depthBuffer[y * width + x]) { // is this triangle closer than others previously?
						depthBuffer[y * width + x] = z;

						// Calculate the texture coordinate based on barycentric position of the pixel
						Vec2f st = st0 * w0 + st1 * w1 + st2 * w2;

						// correct for perspective distortion
						st *= z;

						// If you need to compute the actual position of the shaded
						// point in camera space. Proceed like with the other vertex attribute.
						// Divide the point coordinates by the vertex z-coordinate then
						// interpolate using barycentric coordinates and finally multiply
						// by sample depth.
						Vec3f v0Cam, v1Cam, v2Cam;
						worldToCamera.multVecMatrix(v0, v0Cam);
						worldToCamera.multVecMatrix(v1, v1Cam);
						worldToCamera.multVecMatrix(v2, v2Cam);

						float px = (v0Cam.x / -v0Cam.z) * w0 + (v1Cam.x / -v1Cam.z) * w1 + (v2Cam.x / -v2Cam.z) * w2;
						float py = (v0Cam.y / -v0Cam.z) * w0 + (v1Cam.y / -v1Cam.z) * w1 + (v2Cam.y / -v2Cam.z) * w2;

						Vec3f pt(px * z, py * z, -z); // pt is in camera space

						// Compute the face normal which is used for a simple facing ratio.
						// Keep in mind that we are doing all calculation in camera space.
						// Thus the view direction can be computed as the point on the object
						// in camera space minus Vec3f(0), the position of the camera in camera space.
						Vec3f n = (v1Cam - v0Cam).crossProduct(v2Cam - v0Cam);
						n.normalize();
						Vec3f viewDirection = -pt;
						viewDirection.normalize();

						// Calculate shading of the surface based on dot product of the normal and view direction
						float nDotView = std::max(0.f, n.dotProduct(viewDirection));

						//// Set the pixel value
						auto colour = mats.size() != 0 ? mats[face.mat] : Material("Default");
						TGAColor col = TGAColor(nDotView * colour.Kd.x * 255, nDotView * colour.Kd.y * 255, nDotView * colour.Kd.z * 255, 255);
						image.set(x, y, col);
					}
				}
			}
		}
	}
	image.write_tga_file(filename + ".tga");

	return 0;
}