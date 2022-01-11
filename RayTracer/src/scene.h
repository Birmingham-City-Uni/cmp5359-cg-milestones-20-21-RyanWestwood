#pragma once
#include "hittable_list.h"
#include "material.h"
#include "Sphere.h"
#include "bvh.h"
#include "model.h"

Hittable_List Ball_Scene(std::vector<std::shared_ptr<Material>>& m);
Hittable_List Test_Scene(std::vector<std::shared_ptr<Material>>& m);
Hittable_List My_Scene(std::vector<std::shared_ptr<Material>>& m);