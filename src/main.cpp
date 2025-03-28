#include <vector>
#include <cmath>
#include <random>
#include <glm/vec2.hpp>
#include <glm/geometric.hpp>
#include "tgaimage.h"
#include "model.h"


const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red	 = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
Model* model = nullptr;
int width = 800;
int height = 800;
float* zbuffer = nullptr;

void line(int x0, int y0, int x1, int y1, TGAImage& tgaImage, const TGAColor& tgaColor)
{
	bool steep = false;
	if (std::abs(x1 - x0) < std::abs(y1 - y0))
	{
		std::swap(x0, y0);
		std::swap(x1, y1);
		steep = true;
	}

	if (x0 > x1)
	{
		std::swap(x0, x1);
		std::swap(y0, y1);
	}

	int dx = x1 - x0;
	int dy = y1 - y0;
	int derror2 = std::abs(dy) * 2;
	int error2 = 0;
	int y = y0;
	for (int x = x0; x <= x1; x++) {
		if (steep) {
			tgaImage.set(y, x, tgaColor);
		}
		else {
			tgaImage.set(x, y, tgaColor);
		}
		error2 += derror2;
		if (error2 > dx) {
			y += (y1 > y0 ? 1 : -1);
			error2 -= dx * 2;
		}
	}
}

void line(glm::vec2 v0, glm::vec2 v1, TGAImage& tgaImage, const TGAColor& tgaColor)
{
	line(v0.x, v0.y, v1.x, v1.y, tgaImage, tgaColor);
}

glm::vec3 barycentric(glm::vec3 A, glm::vec3 B, glm::vec3 C, glm::vec3 P)
{
	glm::vec3 s[2];
	for (int i = 1; i >= 0; i--) {
		s[i][0] = C[i] - A[i];
		s[i][1] = B[i] - A[i];
		s[i][2] = A[i] - P[i];
	}
	glm::vec3 u = cross(s[0], s[1]);
	if (std::abs(u[2]) > 1e-2) // dont forget that u[2] is integer. If it is zero then triangle ABC is degenerate
		return glm::vec3(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
	return glm::vec3(-1, 1, 1); // in this case generate negative coordinates, it will be thrown away by the rasterizator
}

void triangle(glm::vec3* pts, TGAImage& image, TGAColor color)
{
	glm::vec2 bboxmin(image.get_width() - 1, image.get_height() - 1);
	glm::vec2 bboxmax(0, 0);
	glm::vec2 clamp(image.get_width() - 1, image.get_height() - 1);
	for (int i = 0; i < 3; i++) {
		bboxmin.x = std::max(0.f, std::min(bboxmin.x, pts[i].x));
		bboxmin.y = std::max(0.f, std::min(bboxmin.y, pts[i].y));

		bboxmax.x = std::min(clamp.x, std::max(bboxmax.x, pts[i].x));
		bboxmax.y = std::min(clamp.y, std::max(bboxmax.y, pts[i].y));
	}
	glm::vec3 P;
	for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) {
		for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) {
			glm::vec3 bc_screen = barycentric(pts[0], pts[1], pts[2], P);
			if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) continue;
			P.z = 0;
			for (int i = 0; i < 3; i++) P.z += pts[i][2] * bc_screen[i];
			if (zbuffer[int(P.x + P.y * width)] < P.z) {
				zbuffer[int(P.x + P.y * width)] = P.z;
				image.set(P.x, P.y, color);
			}
		}
	}	
}

void triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, TGAImage& image, TGAColor color)
{
	glm::vec3 pts[3] = { v0, v1, v2 };
	triangle(pts, image, color);
}

glm::vec3 world2screen(glm::vec3 v)
{
	return glm::vec3(int((v.x + 1.) * width / 2. + .5), int((v.y + 1.) * height / 2. + .5), v.z);
}

int main(int argc, char** argv) {
	if (argc == 2)
	{
		model = new Model(argv[1]);
	}
	else
	{
		model = new Model("../res/obj/african_head.obj");
	}

	TGAImage image(width, height, TGAImage::RGB);

	zbuffer = new float[width * height];
	for (int i = width * height; i--; zbuffer[i] = -std::numeric_limits<float>::max());
	glm::vec3 light_dir(0, 0, -1); // define light_dir

	for (int i = 0; i < model->nfaces(); i++) {
		std::vector<int> face = model->face(i);
		glm::vec3 screen_coords[3];
		glm::vec3 world_coords[3];
		for (int j = 0; j < 3; j++) {
			glm::vec3 v = model->vert(face[j]);
			screen_coords[j] = world2screen(v);
			world_coords[j] = v;
		}
		glm::vec3 n = glm::normalize(glm::cross((world_coords[2] - world_coords[0]) , (world_coords[1] - world_coords[0])));
		float intensity = glm::dot(n, light_dir);
		triangle(screen_coords[0], screen_coords[1], screen_coords[2], image, TGAColor(intensity * 255, intensity * 255, intensity * 255, 255));
	}

	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	return 0;
}

