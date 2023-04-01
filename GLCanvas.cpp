//
// Created by 19450 on 2023/3/28.
//

#include "GLCanvas.h"
#include "imgui/imgui.h"
#include "imgui/backend/imgui_impl_glut.h"
#include "imgui/backend/imgui_impl_opengl2.h"

Transform GLCanvas::transform;
void(*GLCanvas::draw)() = nullptr;
int GLCanvas::mouseButton = 0;
int GLCanvas::mouseX = 0;
int GLCanvas::mouseY = 0;

void GLCanvas::display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplGLUT_NewFrame();

    draw();

	ImGui::Render();
	ImGuiIO& io = ImGui::GetIO();
	glViewport(0, 0, (GLsizei)io.DisplaySize.x, (GLsizei)io.DisplaySize.y);
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
    glutSwapBuffers();
	glutPostRedisplay();
}

// x, y: mouse location
void GLCanvas::keyboard(unsigned char key, int x, int y) {

	float speed = 10, dt = 0.01;
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
	if(key == 'q') exit(0);

	update_camera();
	ImGui_ImplGLUT_KeyboardFunc(key, x, y);
}

void GLCanvas::mouse(int button, int state, int x, int y) {
	// Save the current state of the mouse.  This will be
	// used by the 'motion' function
	mouseButton = button;
	mouseX = x;
	mouseY = y;
	ImGui_ImplGLUT_MouseFunc(button, state, x, y);
}

void GLCanvas::motion(int x, int y) {
	if(mouseButton == GLUT_RIGHT_BUTTON) {
		transform.rotation += vec3(0, (mouseX - x) * -0.01, 0);
		transform.rotation += vec3((mouseY - y) * -0.01, 0, 0);
		mouseX = x;
		mouseY = y;
		update_camera();
	}
	ImGui_ImplGLUT_MotionFunc(x, y);
}

void GLCanvas::update_camera() {
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

    transform.position = vec3(0, 0, 5);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	ImGui_ImplGLUT_Init();
	ImGui_ImplOpenGL2_Init();
	ImGui_ImplGLUT_InstallFuncs();

	// 输入函数中手动调用ImGui的输入函数
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);

	keyboard('w', 0, 0);
	glutMainLoop();

	// Cleanup
	ImGui_ImplOpenGL2_Shutdown();
	ImGui_ImplGLUT_Shutdown();
	ImGui::DestroyContext();
}
