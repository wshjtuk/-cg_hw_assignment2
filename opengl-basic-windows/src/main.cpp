
#include <windef.h>
#include <glad/glad.h>  
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

#include <iostream>
#include "../3rdLibs/glm/glm/glm.hpp"
#include "../3rdLibs/glm/glm/gtc/matrix_transform.hpp"
#include "../3rdLibs/glm/glm/gtc/type_ptr.hpp"


#include "../inc/my_texture.h"
#include "../inc/shader_m.h"
#include "tiny_obj_loader.h"
#define PI 3.1415926

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The .obj .mtl and images are in Dir "model".                                                                  //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*-----------------------------------------------------------------------*/
//Here are some mouse and keyboard function. You can change that.
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
float degreeX = (360 * lastX / 400);
float degreeY = (360 * lastY / 300);
bool firstMouse = true;
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;
float OX = 0;//should be update to a new coordinate
float OY = 0;
float OZ = 0;
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
float currentFrame;
float yaw = -90.0;
float pitch = 0;


void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;
	float sensitivity = 0.01f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;
	yaw += xoffset;
	pitch += yoffset;
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;
	glm::vec3 front;//why not in global 
	front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	front.y = sin(glm::radians(pitch));
	front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
	cameraFront = glm::normalize(front);
	//std::cout << yaw << " " << pitch << std::endl;
}

void processInput(GLFWwindow* window)
{
	/*currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;*/
	float cameraSpeed = 2.5f * deltaTime; // adjust accordingly
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void drawSphere(GLfloat x1,GLfloat y1,GLfloat z1,GLfloat r) {
	glEnable(GL_NORMALIZE);
	GLfloat x, y, z, u, v; // Storage for coordinates and angles        
	int gradation = 200;
	for (u = 0.0; u < PI; u += PI / gradation)
	{
		glBegin(GL_TRIANGLE_STRIP);
		for (v = 0.0; v <= 2.1 * PI; v += PI / gradation)
		{
			x = r * cos(v) * sin(u) + x1;
			y = r * sin(v) * sin(u) + y1;
			z = r * cos(u) + z1;
			glNormal3f(0, 0, 0);
			glVertex3f(x, y, z);
			x = r * cos(v) * sin(u + PI / gradation) + x1;
			y = r * sin(v) * sin(u + PI / gradation) + y1;
			z = r * cos(u + PI / gradation) + z1;
			glVertex3f(x, y, z);
		}
		glEnd();
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void initPMV()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(60, SCR_WIDTH / SCR_HEIGHT, 0.1, 1000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt
	(
		3, 3, 3,
		0, 0, 0,
		0, 1, 0
	);

}

void changePMV()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(60, SCR_WIDTH / SCR_HEIGHT, 0.1, 1000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt
	(
		cameraPos.x, cameraPos.y,cameraPos.z,
		(cameraPos + cameraFront).x, (cameraPos + cameraFront).y, (cameraPos + cameraFront).z,
		cameraUp.x, cameraUp.y, cameraUp.z
	);
}
/*-----------------------------------------------------------------------*/



//This an function to get v, vt and vn. 
bool make_face(std::vector<float> v, std::vector<float> vt, std::vector<float> vn, std::vector<unsigned int> f,
	std::vector<glm::vec3>& points, std::vector<glm::vec3>& normals, std::vector<glm::vec2>& uvs)
{
	if (f.size() % 3 != 0)
		return false;
	for (int i = 0; i < f.size()/3; i += 1)
	{
		int k = i * 3;
		for (int j = 0; j < 3; j++)
		{
			points.push_back(glm::vec3(v[f[k + j] * 3], v[f[k + j] * 3 + 1], v[f[k + j] * 3 + 2]));
			normals.push_back(glm::vec3(vn[f[k + j] * 3], vn[f[k + j] * 3 + 1], vn[f[k + j] * 3 + 2]));
			uvs.push_back(glm::vec2(vt[f[k + j] * 2], vt[f[k + j] * 2 + 1]));
		}
		
	}
}
void get_vec3(std::vector<float> list, std::vector<glm::vec3> &vec)
{
	int n = list.size() / 3;
	for (int i = 0; i < n; i++)
	{
		vec.push_back(glm::vec3(list[i], list[i + 1], list[i + 2]));
	}
}
void get_vec2(std::vector<float> list, std::vector<glm::vec2>& vec)
{
	int n = list.size() / 2;
	for (int i = 0; i < n; i++)
	{
		vec.push_back(glm::vec2(list[i], list[i + 1]));
	}
}



int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	
    gladLoadGL();  
	initPMV();
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glEnable(GL_DEPTH_TEST);
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Here you need to fill construct function of class Shader. And you need to understand other funtions in Shader.//
	// Then, write code in shader_m.vs, shader_m.fs and shader_m.gs to finish the tasks.                             //
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Shader my_shader(
		"../src/shader_m.vs", 
		"../src/shader_m.fs"
	);
	//A shader for light visiable source
	Shader lampShader("../src/lamp.vs", "../src/lamp.fs");


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// tiny::LoadObj is a function to load obj file. The output is shape_t and material_t.                         //
	// "shape.mesh" is a mesh struct. "mesh.positions", "mesh.normals", "mesh.texcoords" corresponds to v,vn,vt.   //
	// For example:                                                                                                //
	// positions[0],positions[1],positions[2] -> v 0,0,1                                                           //
	// positions[3],positions[4],positions[5] -> v 0,1,0                                                           //
	// "mesh.indice" corresponds to f, but it is different from f. Each element is an index for all of v,vn,vt.    //
	// positions[0],positions[1],positions[2] -> v 0,0,1  positions[0],positions[1],positions[2] -> v 0,0,1        //
	// You can read tiny_obj_loader.h to get more specific information.                                            //
	//                                                                                                             //
	// I have write make_face for you.  It will return v, vt, vn in vec form (each element if for one point).      //
	// These informations can help you to do normal mapping.  (You can calculate tangent here)                     //
	// Since i did not assign uv for noraml map, you just need use vt as uv for normal map, but you will find it is//
	//  ugly. So please render a box to show a nice normal mapping. (Do normal mapping on obj and box)             //
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string if_load_succeed = tinyobj::LoadObj(shapes, materials,
		"C:/Users/wjk13/Desktop/cg_hw/cs171-assignment2-wshjtuk/model/p.obj"
	);
	std::vector<unsigned int> obj_VBO_l, obj_VAO_l, obj_light_l;
	std::cout << shapes.size() << std::endl;
	int num;
	for (int i = 0; i < shapes.size(); i++)
	{
		unsigned int vbo;
		unsigned int vao;
		int clock=0;

		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		std::vector < glm::vec3 > out_vertices;
		std::vector < glm::vec2 > out_uvs;
		std::vector < glm::vec3 > out_normals;
		// out_vertices, out_uvs, out_normals will get v, vt and vn.
		make_face(shapes[i].mesh.positions, shapes[i].mesh.texcoords, shapes[i].mesh.normals, shapes[i].mesh.indices,
			out_vertices, out_normals, out_uvs);
		num = out_vertices.size();
		float* vertices = (float*)malloc(sizeof(float) * 8 * num);
		for (int t = 0; t < num; t++) {
			vertices[clock] = out_vertices[t][0];
			clock++;
			vertices[clock] = out_vertices[t][1];
			clock++;
			vertices[clock] = out_vertices[t][2];
			clock++;
			vertices[clock] = out_normals[t][0];
			clock++;
			vertices[clock] = out_normals[t][1];
			clock++;
			vertices[clock] = out_normals[t][2];
			clock++;
			vertices[clock] = out_uvs[t][0];
			clock++;
			vertices[clock] = out_uvs[t][1];
			clock++;
		}
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8 * num, vertices, GL_STATIC_DRAW);
		glBindVertexArray(vao);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
		unsigned int light;
		glGenVertexArrays(1, &light);
		glBindVertexArray(light);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		// note that we update the lamp's position attribute's stride to reflect the updated buffer data
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		obj_VBO_l.push_back(vbo);
		obj_VAO_l.push_back(vao);
		obj_light_l.push_back(light);
		// second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
		
		/*glGenVertexArrays(1, &pVAO);
		glGenBuffers(1, &VBO1);
		int t = 0;
		std::cout << out_vertices.size() << std::endl;
		float* p_vertices = (float*)malloc(sizeof(float) * 8 * out_vertices.size());
		for (int m = 0; m < out_vertices.size(); m++) {
			p_vertices[t] = out_vertices[m].x;
			t++;
			p_vertices[t] = out_vertices[m].y;
			t++;
			p_vertices[t] = out_vertices[m].z;
			t++;
			p_vertices[t] = out_uvs[m].x;
			t++;
			p_vertices[t] = out_uvs[m].y;
			t++;
			p_vertices[t] = out_normals[m].x;
			t++;
			p_vertices[t] = out_normals[m].y;
			t++;
			p_vertices[t] = out_normals[m].z;
			t++;
		}
		
		glBindBuffer(GL_ARRAY_BUFFER, VBO1);
		glBufferData(GL_ARRAY_BUFFER, sizeof(p_vertices), p_vertices, GL_STATIC_DRAW);
		glBindVertexArray(pVAO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
		
		obj_VBO_l[i] = VBO1;
		obj_VAO_l[i] = pVAO;*/
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Render a box to show nice normal mapping.                                                                   //
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	float vertices_cube_0[] = {

		// positions          // normals           // texture coords

		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f

	};
	// first, configure the cube's VAO (and VBO)
	unsigned int VBO, cubeVAO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_cube_0), vertices_cube_0, GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// note that we update the lamp's position attribute's stride to reflect the updated buffer data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// You need to fill this function which is defined in my_texture.h. The parameter is the path of your image.   //
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	unsigned int textureForCube = loadTexture("C:/Users/wjk13/Desktop/cg_hw/cs171-assignment2-wshjtuk/model/2.jpg");
	unsigned int textureForP = loadTexture("C:/Users/wjk13/Desktop/cg_hw/cs171-assignment2-wshjtuk/model/p_r.jpg");
	my_shader.use();
	my_shader.setInt("material.diffuse", 0);
	my_shader.setInt("material.specular", 1);
	my_shader.setFloat("material.shininess", 64.0f);
	my_shader.setVec3("viewPos", cameraPos);


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Here we defined pointlights in shader and passed some parameter for you. You can take this as an example.   //
	// Or you can change it if you like.                                                                           //
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	glm::vec3 pointLightPositions[] = {
		glm::vec3(5.7f,  5.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};
	my_shader.use();
	my_shader.setVec3("dirLight.direction", glm::vec3(1.01f, 1.01f, 1.01f));
	my_shader.setVec3("dirLight.ambient", glm::vec3(0.01f, 0.01f, 0.02f));
	my_shader.setVec3("dirLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
	my_shader.setVec3("dirLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
	// point light 1
	my_shader.setVec3("pointLights[0].position", pointLightPositions[0]);
	drawSphere(5.7f, 5.2f, 2.0f, 1.0f);
	my_shader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
	my_shader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
	my_shader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
	my_shader.setFloat("pointLights[0].constant", 1.0f);
	my_shader.setFloat("pointLights[0].linear", 0.09);
	my_shader.setFloat("pointLights[0].quadratic", 0.032);
	// point light 2
	my_shader.setVec3("pointLights[1].position", pointLightPositions[1]);
	drawSphere(2.3f, -3.3f, -4.0f, 1.0f);
	my_shader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
	my_shader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
	my_shader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
	my_shader.setFloat("pointLights[1].constant", 1.0f);
	my_shader.setFloat("pointLights[1].linear", 0.09);
	my_shader.setFloat("pointLights[1].quadratic", 0.032);
	// point light 3
	my_shader.setVec3("pointLights[2].position", pointLightPositions[2]);
	drawSphere(-4.0f, 2.0f, -12.0f, 1.0f);
	my_shader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
	my_shader.setVec3("pointLights[2].diffuse", 0.6f, 0.1f, 0.8f);
	my_shader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
	my_shader.setFloat("pointLights[2].constant", 1.0f);
	my_shader.setFloat("pointLights[2].linear", 0.09);
	my_shader.setFloat("pointLights[2].quadratic", 0.032);
	// point light 4
	my_shader.setVec3("pointLights[3].position", pointLightPositions[3]);
	drawSphere(0.0f, 0.0f, -3.0f, 1.0f);
	my_shader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
	my_shader.setVec3("pointLights[3].diffuse", 0.1f, 1.1f, 0.8f);
	my_shader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
	my_shader.setFloat("pointLights[3].constant", 1.0f);
	my_shader.setFloat("pointLights[3].linear", 0.09);
	my_shader.setFloat("pointLights[3].quadratic", 0.032);
	
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
		//Update Camera Matrix
		glFlush();
		glEnable(GL_MULTISAMPLE);
		glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
		glLightModeli(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		changePMV();
		
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//  Render an object using texture and normal map.                                                             //
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		glm::mat4 projection = glm::perspective((float)glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 viewmatrix = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		my_shader.setMat4("projection", projection);
		my_shader.setMat4("view", viewmatrix);

		// world transformation
		glm::mat4 model = glm::mat4(1.0f);
		my_shader.setMat4("model", model);

		// bind diffuse map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureForCube);
		// bind specular map
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textureForCube);

		// render the cube
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		/*for (int i = 0; i < shapes.size(); i++)
		{
			glBindVertexArray(obj_VAO_l[i]);
		}*/

		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);






		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//  Render the object in .obj file. You need to set materials and wrap texture for objects.                    //
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureForP);
		// bind specular map
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textureForP);
		for (int p=0; p < shapes.size(); p++) {
			glBindVertexArray(obj_VAO_l[p]);
			glDrawArrays(GL_TRIANGLES, 0, num);

			glBindVertexArray(obj_light_l[p]);
			glDrawArrays(GL_TRIANGLES, 0, num);
		}
		







		
		/////////////////////////////////////////////////////////////////////
		
		/////////////////////////////end/////////////////////////////////////

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

