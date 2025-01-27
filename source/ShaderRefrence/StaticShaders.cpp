#include "StaticShaders.h"
#include <GLFW/glfw3.h>


bool StaticShaders::haseBeenInit = false;


Shader StaticShaders::infinityGrid;
Shader StaticShaders::phongShader;



const Shader& StaticShaders::GetInfinityGrid() {
	return infinityGrid;
}

const Shader& StaticShaders::GetPhongShader()
{
	return phongShader;
}


void StaticShaders::Init()
{
	if (haseBeenInit)
		return;
	haseBeenInit = true;

	
	infinityGrid = 
	{ 
		"../resource/Shaders/infinityGrid_vert.glsl",
		"../resource/Shaders/infinityGrid_frag.glsl" 
	};

	phongShader = {
		"../resource/Shaders/Phong_vert.glsl",
		"../resource/Shaders/Phong_frag.glsl"
	};
}
