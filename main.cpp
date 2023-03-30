#include <bits/stdc++.h>
#include "GLCanvas.h"
#include "GLSL.h"
using namespace std;

int N = 50;

struct Material {
    vec3 base_color = vec3(1.0f);
    vec3 emission;
    bool is_emit;
    float metallic = 0.6;
    float roughness = 0.6;
    float specular = 0.5;
    float specular_tint = 0.5;
    float sheen;
    float sheen_tint;
    float subsurface;
    float clearcoat;
    float clearcoat_gloss;
    float anisotropic;
    float index_of_refraction = 1.2;
    float spec_trans = 0.8;
};

vec3 to_world(vec3 v, vec3 n) {
    vec3 help = vec3(1, 0, 0);
    if(abs(n.x) > 0.999) help = vec3(0, 0, 1);
    vec3 t = normalize(cross(n, help));
    vec3 b = normalize(cross(n, t));
    return normalize(v.x * t + v.y * b + v.z * n);
}
float pow2(float x) { return x * x; }
float pow5(float x) {
    float t = x * x;
    return t * t * x;
}
float rand01() {
    return rand() * 1.0 / RAND_MAX;
}

vec3 GTR2_sample(Material m, vec2 uv, vec3 nor, float &pdf) {
    float m_roughness = m.roughness;
    float alpha2 = max(0.01f, m_roughness * m_roughness);
    float x = rand01();

    float cos_theta = sqrt((1. - x) / (x * (alpha2 - 1) + 1));
    float cos2 = cos_theta * cos_theta;
    float phi = rand01() * 2 * PI;
    float r = sqrt(1. - cos2);

    pdf = 2. * alpha2 * cos_theta * r / pow2(cos2 * (alpha2 - 1.) + 1.) / (2 * PI);
    vec3 h = normalize(vec3(r * cos(phi), r * sin(phi), cos_theta));
    return to_world(h, nor);
}
float GTR2_sample_pdf(Material m, vec2 uv, vec3 nor, vec3 h) {
    float cos_theta = abs(dot(h, nor));
    if(cos_theta < 0) return 0;
    float m_roughness = m.roughness;
    float alpha2 = max(0.01f, m_roughness * m_roughness);
    float cos2 = cos_theta * cos_theta;
    float pdf = 2. * alpha2 * cos_theta / pow2(cos2 * (alpha2 - 1.) + 1.) / (2 * PI);
    return pdf;
}

// bxdf
// ---------------------------------------------- //

float smithG_GGX(float NdotV, float roughness) {
    float a2 = pow2(pow2(0.5 + roughness / 2));
    float b = NdotV*NdotV;
    return 2.0f * b / (NdotV + sqrt(a2 + b - a2*b));
}
float GTR2(float NdotH, float a) {
	float a2 = a * a;
	float t = 1 + (a2-1)*NdotH*NdotH;
	return a2 / (PI * t*t);
}
vec3 SchlickFresnel(vec3 f0, float c) {
	return f0 + (vec3(1) - f0) * pow5(1 - c);
}
float fresnel(float cosI, float etaI, float etaT) {
	float sinI = sqrt(1.0 - cosI * cosI);
	float sinT = etaI / etaT * sinI;
	float cosT = sqrt(1.0 - sinT * sinT);
	float Rl = pow2((etaI * cosI - etaT * cosT) / (etaI * cosI + etaT * cosT));
	float Rp = pow2((etaI * cosT - etaT * cosI) / (etaI * cosT + etaT * cosI));
	return (Rl + Rp) / 2;
}

// L(in), N(same direction with L), V(out)
vec3 btdf(Material m, vec3 L, vec3 V, vec3 N, vec2 uv, float ior1, float ior2) {
    if(m.spec_trans < 0.01) return vec3(0);
    float NdotL = dot(N, L);
    float NdotV = dot(N, V);
    if(NdotL <= 0 || NdotV >= 0) return vec3(0);
    float eta = ior2 / ior1;
    vec3 H = -normalize(L + eta * V);
    float NdotH = dot(N, H);
    if(NdotH <= 0) H = -H;
    float LdotH = dot(L, H);
    float VdotH = dot(V, H);
    if(LdotH * VdotH >= 0) return vec3(0);

    float m_roughness = m.roughness;
    float m_metallic = m.metallic;

    vec3 m_transmission = sqrt(m.base_color) * m.spec_trans;

    float sqrtDenom = LdotH + eta * VdotH;
    float Ds = GTR2(NdotH, m_roughness);
    float iFs = 1 - fresnel(LdotH, ior1, ior2);
    if(iFs <= 0) return vec3(0); // 全反射
    float Gs = smithG_GGX(abs(NdotL), m_roughness);
    Gs *= smithG_GGX(abs(NdotV), m_roughness);

    vec3 refraction = abs((LdotH * VdotH * pow2(eta) * Gs * iFs * Ds * m_transmission)
                          / (NdotV * NdotL * pow2(sqrtDenom)));

    return refraction;
}

vec3 brdf(Material m, vec3 L, vec3 V, vec3 N, vec2 uv) {
	vec3 H = normalize(L + V);
	float NdotL = dot(N, L);
	float NdotV = dot(N, V);
	float NdotH = dot(N, H);
	float LdotH = dot(L, H);
	if(NdotL <= 0 || NdotV <= 0) return vec3(0); // assert!

	float m_roughness = m.roughness;
	float m_metallic = m.metallic;

	vec3 Cdlin = m.base_color;  // 线性空间下的漫反射颜色
	float Cdlum = .3*Cdlin.x + .6*Cdlin.y  + .1*Cdlin.z; // 计算亮度
	vec3 Ctint = Cdlum > 0 ? Cdlin/Cdlum : vec3(1);     // normalize lum. to isolate hue+sat
	vec3 Cspec = m.specular * mix(vec3(1.0f), Ctint, m.specular_tint); // 插值得到镜面反射颜色
	vec3 Cspec0 = mix(0.08 * Cspec, Cdlin, m_metallic);     // 0° 镜面反射颜色, 即菲涅尔项中的F0

	// diffuse
	float FL = pow5(1 - NdotL), FV = pow5(1 - NdotV);
	float Fd90 = 0.5 + 2 * LdotH*LdotH * m_roughness;
	float Fd = mix(1.0f, Fd90, FL) * mix(1.0f, Fd90, FV);
	vec3 diffuse = IVPI * Fd * Cdlin * (1 - m.spec_trans);

	// subsurface
//    float Fss90 = LdotH * LdotH * m_roughness;
//    float Fss = mix(1.0, Fss90, FL) * mix(1.0, Fss90, FV);
//    float ss = 1.25 * (Fss * (1 / (NdotL + NdotV) - .5) + .5);
//    vec3 diffuse = IVPI * mix(Fd, ss, m.subsurface) * Cdlin;

	// specular
	float Ds = GTR2(NdotH, m_roughness);
	vec3 Fs = SchlickFresnel(Cspec0, LdotH);
	float Gs = smithG_GGX(NdotL, m_roughness);
	Gs *= smithG_GGX(NdotV, m_roughness);
	vec3 specular = Gs * Fs * Ds / (4 * NdotV * NdotL);

	// 菲涅尔项已经蕴含了金属度
	return diffuse * (1 - m_metallic) + specular;
//	return mix(diffuse, specular, m_metallic);
}

// 在m上，L方向入射，V方向出射，面法向为N（指向外），纹理坐标uv
vec3 bxdf(Material m, vec3 L, vec3 V, vec3 N, vec2 uv) {
	vec3 LN = dot(L, N) > 0 ? N : -N;
	if(dot(V, LN) >= 0) {
		return brdf(m, L, V, LN, uv);
	} else {
		float ior1 = 1, ior2 = m.index_of_refraction;
		if(LN != N) { // swap
			ior1 += ior2;
			ior2 = ior1 - ior2;
			ior1 -= ior2;
		}
		return btdf(m, L, V, LN, uv, ior1, ior2);
	}
}


void draw() {

    vec3 L = normalize(vec3(-1, 1, 0));
    Material mat;

    // 坐标系
    {
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

    glLineWidth(5);
    glBegin(GL_LINES);
    glColor3f(1, 0, 1);
    glVertex3f(0, 0, 0);
    glVertex3f(L.x * 10, L.y * 10, L.z * 10);
    glEnd();

    glLineWidth(1);
    glBegin(GL_LINES);

    bxdf(mat, L, normalize(vec3(-1, 0.2, 0)), vec3(0, 1, 0), vec2(0, 0));

    srand(0);
    for(int i = 0;i < 500;i++) {

//        float pdf;
//        vec3 V = GTR2_sample(mat, vec2(0, 0), vec3(0, 1, 0), pdf);
//        pdf = GTR2_sample_pdf(mat, vec2(0, 0), vec3(0, 1, 0), V);
//        glColor3f(1, 1, 1);
//        glVertex3f(0, 0, 0);
////        V = V * GTR2(dot(V, vec3(0, 0, 1)), mat.roughness);
////        V = V * pdf;
//        glVertex3f(V.x, V.y, V.z);

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
