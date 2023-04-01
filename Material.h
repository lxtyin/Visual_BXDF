//
// Created by 19450 on 2023/3/31.
//

#ifndef OPENGL__MATERIAL_H_
#define OPENGL__MATERIAL_H_

#include "imgui/imgui.h"
#include "GLSL.h"

struct Material {
	vec3 base_color = vec3(1.0f);
	vec3 emission;
	bool is_emit;
	float metallic = 0.7;
	float roughness = 0.01;
	float specular = 0;
	float specular_tint = 1;
	float sheen;
	float sheen_tint;
	float subsurface;
	float clearcoat;
	float clearcoat_gloss;
	float anisotropic;
	float index_of_refraction = 1.2;
	float spec_trans = 0.8;

	void insert_gui() {
		if(ImGui::TreeNode("Material")) {
			ImGui::SliderFloat("Roughness", &roughness, 0.01, 1);
			ImGui::SliderFloat("Metallic", &metallic, 0, 1);
			ImGui::SliderFloat("IndexOfRefraction", &index_of_refraction, 1, 3);
			ImGui::SliderFloat("SpecTrans", &spec_trans, 0, 1);
			ImGui::TreePop();
		}
	}
};

#endif //OPENGL__MATERIAL_H_
