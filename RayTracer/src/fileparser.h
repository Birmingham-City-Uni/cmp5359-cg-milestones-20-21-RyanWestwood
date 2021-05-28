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
#include "material.h"
#include "enum.h"

void WriteMaterials(std::vector<std::shared_ptr<Material>>& mats, std::string filename);
void WriteNode(std::vector<std::shared_ptr<Hittable>>& objects, std::string filename);

std::vector<Material*> ReadMaterials(std::string filename);
std::vector<Hittable*> ReadNode(std::string filename);