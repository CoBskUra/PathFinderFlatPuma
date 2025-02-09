
#include <Main.h>
#include <filesystem>
#include <Helper/ImGuiStyles.h>
#include <ShaderRefrence/StaticShaders.h>
#include <PathFinder/Puma2D.h>
#include <PathFinder/Obsticles.h>

unsigned int width = 1024;
unsigned int height = 1024;
void ResizeCallBack(GLFWwindow* window, int w, int h);

GLFWwindow* Init();

int main()
{
	GLFWwindow* window = Init();
	if (window == NULL)
		return -1;

	//// infinity Grid
	/*VAO vao;
	vao.Bind();
	VBO vboInfinityGrid(infinityGrid, GL_STATIC_DRAW);

	vao.LinkAttrib(0, 3, GL_FLOAT, false, 3 * sizeof(float), 0);

	vao.Unbind(); vboInfinityGrid.Unbind();
	Shader shader("infinityGrid_vert.glsl", "infinityGrid_frag.glsl");*/

	//InfinityGrid ig;
	////////////////////////////////
	StaticShaders::Init();
	Puma2D puma;
	Obsticles obsticles;

	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.00f, 0.00f, 0.00f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		

		glfwPollEvents();

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		obsticles.Inputs(window, width, height);
		puma.Inputs(window, width, height);
		ImGui::Begin("Puma"); {
			puma.UserInterfers(obsticles);
		}
		puma.ErrorMgWindow(width, height);
		ImGui::End();
		obsticles.Draw();
		puma.Draw(true, &obsticles);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}

	// DeleteAll all the objects we've created

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	

	// DeleteAll window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}


GLFWwindow* Init() {
	srand(time(NULL));
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, "MKMG_lab_1", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return NULL;
	}
	glfwMakeContextCurrent(window);
	glfwSetWindowSizeCallback(window, ResizeCallBack);

	gladLoadGL();
	glViewport(0, 0, width, height);


	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 410");

	ImGuiStyles::SetupImGuiStyle(true, 0.75f);



	return window;
}

// PóŸniej tutaj dam resize, mam tylko problem, ¿e po risie Ÿle 
// kursor po klikniêciu ustawia mi siê w z³ym miejscu
// 
void ResizeCallBack(GLFWwindow* window, int w, int h)
{
	width = w;
	height = h;
	glViewport(0, 0, width, height);
}

