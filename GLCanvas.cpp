//
// Created by 19450 on 2023/3/28.
//

#include "GLCanvas.h"

void GLCanvas::display() {
	glColor3f(0, 1, 0);
	glBegin(GL_LINES);
	glVertex3d(0, 1, 0);
	glVertex3d(1, 0, 1);
	glEnd();
}

// x, y: mouse location
void GLCanvas::keyboard(unsigned char key, int x, int y) {

	float speed = 50, dt = 0.01;
	if(key == 'w'){
		transform.position += transform.direction_z() * -speed * dt;
	}
	if(key == 's'){
		transform.position += transform.direction_z() * speed * dt;
	}
	if(key == 'd'){
		transform.position += transform.direction_x() * speed * dt;
	}
	if(key == 'a'){
		transform.position += transform.direction_x() * -speed * dt;
	}
	if(key == 'j') transform.rotation += vec3(0, dt, 0);
	if(key == 'l') transform.rotation += vec3(0, -dt, 0);
	if(key == 'i') transform.rotation += vec3(-dt, 0, 0);
	if(key == 'k') transform.rotation += vec3(dt, 0, 0);
	if(key == 'q') exit(0);

	vec3 tar = transform.position + transform.direction_z();
	gluLookAt(transform.position.x, transform.position.y, transform.position.z,
		tar.x, tar.y, tar.z, 0, 1, 0);
	glutPostRedisplay();
}

void GLCanvas::initialize(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitWindowSize(800, 800);
	glutCreateWindow("hello window");

	glClearColor(0, 0, 0, 0);
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0, 100, 0, 100); // 视口大小

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);

	glutMainLoop();
}
