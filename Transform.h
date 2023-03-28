#pragma once

#include <string>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

using glm::vec3;
using glm::mat4;
using std::string;

class Instance;

class Transform {
    string order = "YXZ";       /* rotate order */
public:
    vec3 position;
    vec3 rotation;
    vec3 scale;

    mat4 matrix();

    Transform();
    Transform(vec3 position, vec3 rotation, vec3 scale);

	vec3 direction_x();
	vec3 direction_y();
	vec3 direction_z();

	/**
     * set rotate order for the rotation(dynamic euler angle).
     * \param o must be like "XYZ", otherwise it throws error.
     */
    void set_order(string o);
};


