//
// Created by 19450 on 2023/3/28.
//

#ifndef OPENGL__GLCANVAS_H_
#define OPENGL__GLCANVAS_H_

#include <GL/gl.h>
#include <GL/glu.h>
#include "Transform.h"
#include "Gl/glut.h"
#include "GLSL.h"

class GLCanvas {

	static Transform transform;

	// Callback functions for mouse and keyboard events
	static void display();
	static void keyboard(unsigned char key, int x, int y);
    static void (*draw)();
 public:
	static void initialize(int argc, char** argv, void (*draw)());
};

#endif //OPENGL__GLCANVAS_H_
