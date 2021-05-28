#include "tree.h"

void Traverse_Tree(std::shared_ptr<Hittable> n, std::vector<std::shared_ptr<Hittable>>& arr) {
	if (n == nullptr) return;

	arr.push_back(n);

	Traverse_Tree(n->Left(), arr);
	Traverse_Tree(n->Right(), arr);
}

std::shared_ptr<Hittable> Create_Tree(std::vector<Hittable*>& objs, std::vector<std::shared_ptr<Material>>& mtl) {
	if (objs.size() == 0) return nullptr;

	int idx = objs.front()->id;

	std::shared_ptr<Hittable> node;
	if (idx == BVH_NODE) {
		BVH_Node* bvh = (BVH_Node*)objs.front();
		node = std::make_shared<BVH_Node>(bvh->box);
		objs.erase(begin(objs));
	}
	else if (idx == TRIANGLE) {
		Triangle* tri = (Triangle*)objs.front();
		std::shared_ptr<Hittable> n = std::make_shared<Triangle>(
			tri->v0, tri->v1, tri->v2,
			tri->v0n, tri->v1n, tri->v2n,
			mtl[tri->mat_index - 1],
			tri->mat_index
			);
		objs.erase(begin(objs));
		return n;
	}
	else if (idx == SPHERE) {
		Sphere* sph = (Sphere*)objs.front();
		std::shared_ptr<Hittable> m = std::make_shared<Sphere>(sph->centre, sph->radius, mtl[sph->mat_index - 1], sph->mat_index);
		objs.erase(begin(objs));
		return m;
	}

	node->Left(Create_Tree(objs, mtl));
	node->Right(Create_Tree(objs, mtl));

	return node;
}

std::vector<std::shared_ptr<Material>> Create_Materials(std::vector<Material*>& mtls) {
	std::vector<std::shared_ptr<Material>> result;
	for (auto& material : mtls) {
		int id = material->id;

		if (id == LAMBERTIAN) {
			Lambertian* lambert = (Lambertian*)material;
			std::shared_ptr<Material> n = std::make_shared<Lambertian>(
				lambert->albedo,
				lambert->index
				);
			result.push_back(n);
		}
		else if (id == METAL) {
			Metal* metal = (Metal*)material;
			std::shared_ptr<Material> n = std::make_shared<Metal>(
				metal->albedo,
				metal->fuzz,
				metal->index
				);
			result.push_back(n);
		}
		else if (id == DIELECTRIC) {
			Dielectric* dielectric = (Dielectric*)material;
			std::shared_ptr<Material> n = std::make_shared<Dielectric>(
				dielectric->ir,
				dielectric->index
				);
			result.push_back(n);
		}
		else if (id == DIFFUSE_LIGHT) {
			Diffuse_Light* diffuse_light = (Diffuse_Light*)material;
			std::shared_ptr<Material> n = std::make_shared<Diffuse_Light>(
				diffuse_light->colour,
				diffuse_light->index
				);
			result.push_back(n);
		}
	}

	return result;
}