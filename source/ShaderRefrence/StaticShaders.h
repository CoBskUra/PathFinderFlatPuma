#pragma once
#include "ShaderManadement/Shader.h"

static struct StaticShaders{
	static void Init(); // u¿yj przed u¿yciem shaderów
	static const Shader& GetInfinityGrid();
	static const Shader& GetPhongShader();
private:
	static bool haseBeenInit;
		   
	static Shader infinityGrid;
	static Shader phongShader;
};