//
// Created by 19450 on 2023/3/28.
//

#include "GLCanvas.h"

Transform GLCanvas::transform;
void (*GLCanvas::draw)() = nullptr;
void GLCanvas::display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    draw();

    glutSwapBuffers();
}

// x, y: mouse location
void GLCanvas::keyboard(unsigned char key, int x, int y) {

	float speed = 5, dt = 0.01;
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
	if(key == 'l') transform.rotation += vec3(0, dt, 0);
	if(key == 'j') transform.rotation += vec3(0, -dt, 0);
	if(key == 'i') transform.rotation += vec3(-dt, 0, 0);
	if(key == 'k') transform.rotation += vec3(dt, 0, 0);
	if(key == 'q') exit(0);

    if(key == 't') return;

	vec3 tar = transform.position - transform.direction_z();
    vec3 up = transform.direction_y();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); // 指定当前矩阵为单位矩阵
    gluLookAt(transform.position.x, transform.position.y, transform.position.z,
              tar.x, tar.y, tar.z, up.x, up.y, up.z);
	glutPostRedisplay();
}

void GLCanvas::initialize(int argc, char** argv, void (*dr)()) {
    draw = dr;
	glutInit(&argc, argv);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glutInitDisplayMode(GLUT_DOUBLE| GLUT_DEPTH |GLUT_RGBA);
	glutInitWindowSize(800, 800);
	glutCreateWindow("hello window");

    glMatrixMode(GL_PROJECTION);//设置当前矩阵为投影矩阵.
    glLoadIdentity(); // 指定当前矩阵为单位矩阵
    gluPerspective(90, 1, 0.01, 500);

    transform.position = vec3(0, 0, 10);

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);

    keyboard('w', 0, 0);
	glutMainLoop();
}
