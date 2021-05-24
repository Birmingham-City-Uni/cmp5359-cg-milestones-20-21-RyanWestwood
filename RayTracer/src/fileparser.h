#pragma once
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include "aabb.h"
#include "hittable.h"
#include "bvh.h"
#include "Sphere.h"
#include "Triangle.h"

void Write(std::vector<AABB>& objects, std::string filename);
void WriteNode(std::vector<std::shared_ptr<Hittable>>& objects, std::string filename);

std::vector<AABB> Read(std::string filename);
std::vector<Hittable*> ReadNode(std::string filename);