#include "Init.h"
#include <GLFW/glfw3.h>
#include "engine/graphics/Renderer.h"

class STDGLRenderer { public: static std::shared_ptr<Renderer> Make(); };

static void exitfunc() {
	glfwTerminate();
}

void Engine::Init() {
    glfwInit();
	std::atexit(&exitfunc);

    Renderer::AddRenderer("STDGLRenderer", &STDGLRenderer::Make);
}