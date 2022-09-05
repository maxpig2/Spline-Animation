
// std
#include <iostream>
#include <string>
#include <chrono>

#define GLM_ENABLE_EXPERIMENTAL

// glm
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/spline.hpp>

// project
#include "application.hpp"
#include "cgra/cgra_geometry.hpp"
#include "cgra/cgra_gui.hpp"
#include "cgra/cgra_image.hpp"
#include "cgra/cgra_shader.hpp"
#include "cgra/cgra_wavefront.hpp"


using namespace std;
using namespace cgra;
using namespace glm;


void basic_model::draw(const glm::mat4 &view, const glm::mat4 proj) {
	mat4 modelview = view * modelTransform;
	
	mat4 transformation = mat4(1);
	

	glUseProgram(shader); // load shader and variables
	glUniformMatrix4fv(glGetUniformLocation(shader, "uProjectionMatrix"), 1, false, value_ptr(proj));
	glUniform3fv(glGetUniformLocation(shader, "uColor"), 1, value_ptr(color));

	float scaleDivider = 20;
	transformation = scale(transformation, vec3(1/scaleDivider));
	transformation = translate(transformation, timelinePoint * vec3(scaleDivider));
	glUniformMatrix4fv(glGetUniformLocation(shader, "uModelViewMatrix"), 1, false, value_ptr(modelview * transformation));


	//drawCylinder();
	mesh.draw(); // draw
}


//Spline Reference 
// https://www.mvps.org/directx/articles/catmull/
void basic_spline::draw(const glm::mat4 &view, const glm::mat4 proj) {
	mat4 modelview = view * modelTransform;
	
	glUseProgram(shader); // load shader and variables
	glUniformMatrix4fv(glGetUniformLocation(shader, "uProjectionMatrix"), 1, false, value_ptr(proj));
	glUniformMatrix4fv(glGetUniformLocation(shader, "uModelViewMatrix"), 1, false, value_ptr(modelview));
	glUniform3fv(glGetUniformLocation(shader, "uColor"), 1, value_ptr(color));

	//drawCylinder();
	//drawSphere();
	gl_spline.mode = 1; //1 for GL_LINES 2 for GL_LINE_STRIPS
	gl_spline.draw();

	int noSpline = splinePointsRaw.size();
	for (int i = 1; i < noSpline; i ++) {
		color = vec3(0.3,0.3,0.3);
		if(i == 1) {
			color = vec3(0,1,0);
			
		} else if(i == noSpline-1) {
			color = vec3(1,0,0);
			
		}
	glUniform3fv(glGetUniformLocation(shader, "uColor"), 1, value_ptr(color));
	mat4 transformation = mat4(1);
	float scaleDivider = 15;
	transformation = scale(transformation, vec3(1/scaleDivider));
	transformation = translate(transformation,splinePointsRaw[i]*vec3(scaleDivider));
	glUniformMatrix4fv(glGetUniformLocation(shader, "uModelViewMatrix"), 1, false, value_ptr(modelview * transformation));

	drawSphere();
	}


	//gl_spline.mode(GL_LINE);
}

vec3 basic_spline::calculateSplineInterpolation(vec3 P0,vec3 P1, vec3 P2, vec3 P3, float t) {
//	for (int i = 0; i < splineDivisionSteps; i++){
	vec3 point = 0.5f * ( (2.0f * P1)+ (-P0 + P2)*t + (2.0f * P0 - 5.0f *P1 + 4.0f * P2 - P3 ) * pow(t,2.0f) + (-P0 + 3.0f*P1 - 3.0f*P2 + P3) * pow(t,3.0f));
	splinePointsInterpolated.push_back(point);
	
	//t += 1/splineDivisionSteps;
	cout << "Interpolated Spline 	X: " << point.x << "	Y: " << point.y << "	Z: " << point.z << endl;
	//}

	return point;
}

void basic_spline::setPoints(){
	mesh_builder spline;

	initSpline_01();

	int j = 0;
	for  (int k = 0; k < 5; k++) {
	for (float i = 0.0; i <= 1.0; i = i + 0.10, j++) {
		spline.push_index(j);
		
		spline.push_vertex(mesh_vertex{calculateSplineInterpolation(splinePointsRaw[0+k],splinePointsRaw[1+k],splinePointsRaw[2+k],splinePointsRaw[3+k],i)   });
		//spline.push_vertex(mesh_vertex{catmullRom(splinePointsRaw[0+k],splinePointsRaw[1+k],splinePointsRaw[2+k],splinePointsRaw[3+k],i)});
		
	}
	}
	gl_spline = spline.build();

}

void basic_spline::initSpline_01 () {
	splinePointsRaw.clear();
	splinePointsInterpolated.clear();
	//Spline Points
	splinePointsRaw.push_back(vec3(0,0,0));
	///**
	splinePointsRaw.push_back(vec3(-0.25,0,0));
	splinePointsRaw.push_back(vec3(0.5,0.5,0.5));
	splinePointsRaw.push_back(vec3(0.75,0.75,0));
	splinePointsRaw.push_back(vec3(1,1,0));
	splinePointsRaw.push_back(vec3(1.5,1.5,0.5));
	splinePointsRaw.push_back(vec3(2,2,0.5));
	//*/

	/**
	splinePointsRaw.push_back(vec3(0.4,0.8,0));
	splinePointsRaw.push_back(vec3(2.0,1.7,1.0));
	splinePointsRaw.push_back(vec3(3.4,2.6,4.0));
	splinePointsRaw.push_back(vec3(4.4,3.1,4.1));
	splinePointsRaw.push_back(vec3(-1,3.0,3.0));
	splinePointsRaw.push_back(vec3(5.0,2.0,5.0));
	*/


}


Application::Application(GLFWwindow *window) : m_window(window) {
	
	shader_builder sb;
    sb.set_shader(GL_VERTEX_SHADER, CGRA_SRCDIR + std::string("//res//shaders//color_vert.glsl"));
	sb.set_shader(GL_FRAGMENT_SHADER, CGRA_SRCDIR + std::string("//res//shaders//color_frag.glsl"));
	GLuint shader = sb.build();

	m_model.shader = shader;
	m_model.mesh = load_wavefront_data(CGRA_SRCDIR + std::string("/res//assets//teapot.obj")).build();
	m_model.color = vec3(1, 0, 0);
	m_cam_pos = vec2( 0, 0 );

	m_spline.shader = shader;
	m_spline.color = vec3(0,1,0);
	//m_spline.mesh.mode = GL_LINES;
	//m_spline.initSpline_01();
	
	//gl_spline.initSpl
	m_spline.setPoints();

}


void Application::render() {
	
	// retrieve the window height
	int width, height;
	glfwGetFramebufferSize(m_window, &width, &height); 

	m_windowsize = vec2(width, height); // update window size
	glViewport(0, 0, width, height); // set the viewport to draw to the entire window

	// clear the back-buffer
	glClearColor(0.3f, 0.3f, 0.4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

	// enable flags for normal/forward rendering
	glEnable(GL_DEPTH_TEST); 
	glDepthFunc(GL_LESS);

	// projection matrix
	mat4 proj = perspective(1.f, float(width) / height, 0.1f, 1000.f);

	// view matrix
	mat4 view = translate(mat4(1), vec3( m_cam_pos.x, m_cam_pos.y, -m_distance))
		* rotate(mat4(1), m_pitch, vec3(1, 0, 0))
		* rotate(mat4(1), m_yaw,   vec3(0, 1, 0));


	// helpful draw options
	if (m_show_grid) drawGrid(view, proj);
	if (m_show_axis) drawAxis(view, proj);
	glPolygonMode(GL_FRONT_AND_BACK, (m_showWireframe) ? GL_LINE : GL_FILL);

	m_model.timelinePoint = m_spline.splinePointsInterpolated[m_model.timelineVal];

	// draw the model
	m_model.draw(view, proj);
	m_spline.draw(view,proj);
	if (animateModel) {
	m_model.timelineVal++;
	if(m_model.timelineVal > m_spline.splinePointsInterpolated.size()) {
		m_model.timelineVal = 0;
	}
	}
	//cout<<m_model.timelineVal<<endl;
	//cout << "TP	X: 	"<< m_model.timelinePoint.x << " Y: 	"<< m_model.timelinePoint.y << "Z: 	"<< m_model.timelinePoint.z << endl ;
}


void Application::renderGUI() {

	// setup window
	ImGui::SetNextWindowPos(ImVec2(5, 5), ImGuiSetCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiSetCond_Once);
	ImGui::Begin("Options", 0);

	// display current camera parameters
	ImGui::Text("Application %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::SliderFloat("Pitch", &m_pitch, -pi<float>() / 2, pi<float>() / 2, "%.2f");
	ImGui::SliderFloat("Yaw", &m_yaw, -pi<float>(), pi<float>(), "%.2f");
	ImGui::SliderFloat("Distance", &m_distance, 0, 100, "%.2f", 2.0f);

	// helpful drawing options
	ImGui::Checkbox("Show axis", &m_show_axis);
	ImGui::SameLine();
	ImGui::Checkbox("Show grid", &m_show_grid);
	ImGui::Checkbox("Wireframe", &m_showWireframe);
	ImGui::SameLine();
	if (ImGui::Button("Screenshot")) rgba_image::screenshot(true);
	ImGui::Checkbox("Animate", &animateModel);
	
	ImGui::Separator();

	// example of how to use input boxes
	static float exampleInput;
	if (ImGui::InputFloat("example input", &exampleInput)) {
		cout << "example input changed to " << exampleInput << endl;
	}

	// finish creating window
	ImGui::End();
}


void Application::cursorPosCallback(double xpos, double ypos) {
	vec2 whsize = m_windowsize / 2.0f;

	double y0 = glm::clamp((m_mousePosition.y - whsize.y) / whsize.y, -1.0f, 1.0f);
	double y = glm::clamp((float(ypos) - whsize.y) / whsize.y, -1.0f, 1.0f);
	double dy = -( y - y0 );

	double x0 = glm::clamp((m_mousePosition.x - whsize.x) / whsize.x, -1.0f, 1.0f);
	double x = glm::clamp((float(xpos) - whsize.x) / whsize.x, -1.0f, 1.0f);
	double dx = x - x0;

	if (m_leftMouseDown) {
		// clamp the pitch to [-pi/2, pi/2]
		m_pitch += float( acos(y0) - acos(y) );
		m_pitch = float(glm::clamp(m_pitch, -pi<float>() / 2, pi<float>() / 2));

		// wrap the yaw to [-pi, pi]
		m_yaw += float( acos(x0) - acos(x) );
		if (m_yaw > pi<float>()) 
			m_yaw -= float(2 * pi<float>());
		else if (m_yaw < -pi<float>()) 
			m_yaw += float(2 * pi<float>());
	} else if ( m_rightMouseDown ) {
		m_distance += dy * 10;
	} else if ( m_middleMouseDown ) {
		m_cam_pos += vec2( dx, dy ) * 10.f;
	}

	// updated mouse position
	m_mousePosition = vec2(xpos, ypos);
}


void Application::mouseButtonCallback(int button, int action, int mods) {
	(void)mods; // currently un-used

	// capture is left-mouse down
	if (button == GLFW_MOUSE_BUTTON_LEFT)
		m_leftMouseDown = (action == GLFW_PRESS); // only other option is GLFW_RELEASE
	else if (button == GLFW_MOUSE_BUTTON_RIGHT)
		m_rightMouseDown = (action == GLFW_PRESS);
	else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
		m_middleMouseDown = (action == GLFW_PRESS);
}


void Application::scrollCallback(double xoffset, double yoffset) {
	(void)xoffset; // currently un-used
	m_distance *= pow(1.1f, -yoffset);
}


void Application::keyCallback(int key, int scancode, int action, int mods) {
	(void)key, (void)scancode, (void)action, (void)mods; // currently un-used
}


void Application::charCallback(unsigned int c) {
	(void)c; // currently un-used
}
