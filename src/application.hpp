
#pragma once

// glm
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


// project
#include "opengl.hpp"
#include "cgra/cgra_mesh.hpp"
#include "skeleton_model.hpp"


// Basic model that holds the shader, mesh and transform for drawing.
// Can be copied and modified for adding in extra information for drawing
// including textures for texture mapping etc.
struct basic_model {
	GLuint shader = 0;
	cgra::gl_mesh mesh;
	glm::vec3 color{0.7};
	glm::mat4 modelTransform{1.0};
	GLuint texture;
	int timelineVal = 0;
	glm::vec3 timelinePoint = glm::vec3(0,0,0);



	void draw(const glm::mat4 &view, const glm::mat4 proj);
};

struct basic_spline {
	GLuint shader = 0;
	cgra::gl_mesh mesh;
	glm::vec3 color{0.7};
	glm::mat4 modelTransform{1.0};
	GLuint texture;
	std::vector<glm::vec3> splinePointsRaw;
	std::vector<glm::vec3> splinePointsInterpolated;
	int splineDivisionSteps = 10;
	cgra::gl_mesh gl_spline;

	void draw(const glm::mat4 &view, const glm::mat4 proj);
	void setPoints();
	glm::vec3 calculateSplineInterpolation(glm::vec3 P0, glm::vec3 P1, glm::vec3 P2, glm::vec3 P3, float t);

	void initSpline_01();


};


// Main application class
//
class Application {
private:
	// window
	glm::vec2 m_windowsize;
	GLFWwindow *m_window;

	// oribital camera
	float m_pitch = .86;
	float m_yaw = -.86;
	float m_distance = 20;
	glm::vec2 m_cam_pos;

	// last input
	bool m_leftMouseDown = false;
	bool m_rightMouseDown = false;
	bool m_middleMouseDown = false;
	glm::vec2 m_mousePosition;

	// drawing flags
	bool m_show_axis = false;
	bool m_show_grid = false;
	bool m_showWireframe = false;
	bool animateModel = false;

	// geometry
	basic_model m_model;
	basic_spline m_spline;
	

public:
	// setup
	Application(GLFWwindow *);

	// disable copy constructors (for safety)
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;

	// rendering callbacks (every frame)
	void render();
	void renderGUI();

	// input callbacks
	void cursorPosCallback(double xpos, double ypos);
	void mouseButtonCallback(int button, int action, int mods);
	void scrollCallback(double xoffset, double yoffset);
	void keyCallback(int key, int scancode, int action, int mods);
	void charCallback(unsigned int c);
};