#pragma once
#include <memory>
#include <vector>
#include "hittable.h"
#include "bvh.h"
#include "enum.h"
#include "Triangle.h"
#include "Sphere.h"
#include "material.h"

void Traverse_Tree(std::shared_ptr<Hittable> n, std::vector<std::shared_ptr<Hittable>>& arr);

std::shared_ptr<Hittable> Create_Tree(std::vector<Hittable*>& objs, std::vector<std::shared_ptr<Material>>& mtl);
std::vector<std::shared_ptr<Material>> Create_Materials(std::vector<Material*>& mtls);