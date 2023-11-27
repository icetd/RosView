#define STB_IMAGE_IMPLEMENTATION
#include <Windows.h>
#include <stb_image.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <glm/glm.hpp>
#include "Application.h"
#include "log.h"

static Application *Instance = nullptr;

static void glfw_error_callback(int error, const char *description)
{
    LOG(ERRO, "GLFW Error %d:%s", error, description);
}

Application::Application(const char *appName, int w, int h) :
    m_AppName(appName),
    m_Width(w),
    m_Height(h),
    m_WindowHandler(nullptr)
{
    Instance = this;
    Init();
}

Application::~Application()
{

}

void Application::Init()
{   
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) {
        LOG(ERRO, "Could not Initialie GLFW!");
        return;
    }

    const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    m_WindowHandler = glfwCreateWindow(m_Width, m_Height, m_AppName.c_str(), NULL, NULL);

    GLFWimage icon;
    icon.width = 860;
    icon.height = 940;
    int channels = 4;
    icon.pixels = stbi_load("../res/textures/robot.png", &icon.width, &icon.height, &channels, 4);
    glfwSetWindowIcon(m_WindowHandler, 1, &icon);
	stbi_image_free(icon.pixels);

    glfwMakeContextCurrent(m_WindowHandler);
    glfwSetWindowUserPointer(m_WindowHandler, this);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        LOG(ERRO, "Failed to initialize GLAD");
        exit(EXIT_FAILURE);
    }


    LOG(NOTICE, "%s\n", glGetString(GL_VERSION));
	ImGui::CreateContext();
	IMGUI_CHECKVERSION();
	ImGuiIO& io = ImGui::GetIO();

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
	
    // fonts set
	io.Fonts->AddFontFromFileTTF("../res/fonts/YaHei.ttf", 16.0f, nullptr, io.Fonts->GetGlyphRangesChineseFull());

	ImGui::StyleColorsClassic();

	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	ImGui_ImplGlfw_InitForOpenGL(m_WindowHandler, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
}

void Application::Shutdown()
{
    for (auto & layer : m_LayerStack)
        layer->OnDetach();

    m_LayerStack.clear();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(m_WindowHandler);
    glfwTerminate();
}

Application *Application::GetInstance()
{
    return Instance;
}

void Application::Run()
{

    ImVec4 clear_color = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);

	while (!glfwWindowShouldClose(m_WindowHandler)) {

		glfwPollEvents();
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Show Layers
		for (auto& layer : m_LayerStack)
			layer->OnUpdate(m_TimeStep);

		// Redering
		ImGui::Render();

		int display_w, display_h;
		glfwGetFramebufferSize(m_WindowHandler, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);

		// Update and Render additional Platform Windows
		// (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
		//  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)

	    ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(m_WindowHandler);

        float time = GetTime();
        m_FrameTime = time - m_LastFrameTime;
        m_TimeStep = glm::min<float>(m_FrameTime, 0.03333);
        m_LastFrameTime = time;
    }
}

float Application::GetTime()
{
    return (float) glfwGetTime();
}


void Application::PushLayer(const std::shared_ptr<Layer> &layer)
{
    m_LayerStack.emplace_back(layer);
    layer->OnAttach();
}
