#version 330 core

// Positions/Coordinates
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;

// Outputs the color for the Fragment Shader
// out vec4 o_color;

// Imports the camera matrix from the main function
uniform mat4 MODEL_MATRIX;
uniform mat4 CAM_MATRIX;

out vec3 Normal;
out vec3 FragPos;

void main()
{
	// Outputs the positions/coordinates of all vertices
	// CAM_MATRIX *
	gl_Position = CAM_MATRIX * MODEL_MATRIX *  vec4(aPos, 1.0);
	Normal = normalize( vec3(MODEL_MATRIX * vec4(aNorm, 0)));
	FragPos = vec3( MODEL_MATRIX *  vec4(aPos, 1.0));
}