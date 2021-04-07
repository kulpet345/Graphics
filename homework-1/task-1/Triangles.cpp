// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <iostream>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/shader.hpp>

int main( void )
{
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 1024, 768, "Two triangles", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	
	
	// Enable depth test
    glEnable(GL_DEPTH_TEST);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  
    // enable alpha channel
    //glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS); 

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint GreenID = LoadShaders("GreenTriangle.vertexshader", "GreenTriangle.fragmentshader");
	GLuint PurpleID = LoadShaders("GreenTriangle.vertexshader", "PurpleTriangle.fragmentshader");
	
	GLuint MatrixGreenID = glGetUniformLocation(GreenID, "MVP");
	GLuint MatrixPurpleID = glGetUniformLocation(PurpleID, "MVP");

	
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	
	//glm::mat4 View   = glm::lookAt(
	//							glm::vec3(4,3,3), // Camera is at (4,3,3), in World Space
	//							glm::vec3(0,0,0), // and looks at the origin
	//							glm::vec3(0,-1,0)  // Head is up (set to 0,-1,0 to look upside-down)
	//);
	
	glm::mat4 Model = glm::mat4(1.0f);


	static const GLfloat g_vertex_buffer_data[] = { 
		-0.5f, -0.5f, 0.3f,
		 0.5f, -0.5f, 0.3f,
		 0.0f,  0.5f, 0.3f,
		 
		 0.0f, -0.7f, 0.5f,
		 -0.5f,  0.0f, 0.5f,
		 0.5f,  0.0f, 0.1f
	};

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
	
	glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	
	const int cnt = 1000;
	
	int i = 0;
	
	const double pi = 2 * std::acos(0);

	do {
		
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		
		//GLuint MatrixID = glGetUniformLocation(programID, "MVP");

		// Use our shader
		glUseProgram(GreenID);
		// glUseProgram(PurpleID);
		glm::mat4 View = glm::lookAt(
		        //glm::vec3(0,0,3),
				glm::vec3(3 * std::cos(2 * pi * i / cnt), 0, 3 * std::sin(2 * pi * i / cnt)), 
				glm::vec3(0,0,0), // and looks at the origin
				glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
	    );
		glm::mat4 MVP = Projection * View * Model;
		glUniformMatrix4fv(MatrixGreenID, 1, GL_FALSE, &MVP[0][0]);
		
		
		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
		

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, 3); // 3 indices starting at 0 -> 1 triangle
		
		glUseProgram(PurpleID);
		
		
		glUniformMatrix4fv(MatrixPurpleID, 1, GL_FALSE, &MVP[0][0]);

		
		glDrawArrays(GL_TRIANGLES, 3, 3);

		glDisableVertexAttribArray(0);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
		
		++i;
		i %= cnt;

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Cleanup VBO
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
	//glDeleteProgram(programID);
	glDeleteProgram(GreenID);
	glDeleteProgram(PurpleID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

