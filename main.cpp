#include <bits/stdc++.h>
#include "GLCanvas.h"
#include "GLSL.h"
#include "imgui/imgui.h"
#include "Material.h"
#include "Shader.h"
using namespace std;

vec3 L = normalize(vec3(-11, 1, 0));
Material mat;

void drawAxes() {
	glLineWidth(10);
	glBegin(GL_LINES);
	glColor3f(1, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(10, 0, 0);

	glColor3f(0, 1, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 10, 0);

	glColor3f(0, 0, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 10);
	glEnd();
}

void draw() {
	float v[3] = {L.x, L.y, L.z};
	ImGui::DragFloat3("L IN", v, 0.01, -1, 1);
	L = normalize(vec3(v[0], v[1], v[2]));

	mat.insert_gui();

    // 坐标系
	drawAxes();

    glLineWidth(5);
    glBegin(GL_LINES);
    glColor3f(1, 0, 1);
    glVertex3f(0, 0, 0);
    glVertex3f(L.x * 10, L.y * 10, L.z * 10);
    glEnd();

    glLineWidth(1);
    glBegin(GL_LINES);

//    bxdf(mat, L, normalize(vec3(10, 1, 0)), vec3(0, 1, 0), vec2(0, 0));

    srand(0);
    for(int i = 0;i < 2000;i++) {

        float theta = rand01() * 2 * PI - PI;
        float phi = rand01() * PI - PI / 2;

        float r = cos(phi);
        vec3 V = normalize(vec3(cos(theta) * r, sin(phi), sin(theta) * r));
        vec3 f_r = bxdf(mat, L, V, vec3(0, 1, 0), vec2(0, 0));

        if(_isnan(f_r.x) || _isnan(f_r.y) || _isnan(f_r.z)) {
            glColor3f(1, 0, 0);
            glVertex3f(0, 0, 0);
            glVertex3f(V.x, V.y, V.z);
        } else {
            glColor3f(1, 1, 1);
            V = V * length(f_r);
            glVertex3f(0, 0, 0);
            glVertex3f(V.x, V.y, V.z);
        }
    }
    glEnd();
}

int main(int argc, char** argv) {

	GLCanvas::initialize(argc, argv, draw);
    return 0;
}
