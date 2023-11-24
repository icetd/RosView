#ifndef APPLICATION_H
#define APPLICATION_H

#include <string>
#include <memory>
#include <vector>
#include <functional>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Layer.h"

class Application 
{
public:
    Application(const char *appName, int w, int h);
    ~Application();

    static Application *GetInstance();
    void Run();

    template <typename T>
    void PushLayer()
    {
        static_assert(std::is_base_of<Layer, T>::value, "Pushed type is not subclass of Layer!");
        m_LayerStack.emplace_back(std::make_shared<T>());
        m_LayerStack.back()->OnAttach();
    }

    void PushLayer(const std::shared_ptr<Layer> &layer);
    void Close();

    float GetTime();
    
private:
    std::string m_AppName;
    int m_Width;
    int m_Height;
    GLFWwindow *m_WindowHandler;

    float m_TimeStep = 0.0f;
    float m_FrameTime = 0.0f;
    float m_LastFrameTime = 0.0f;
    std::vector<std::shared_ptr<Layer>> m_LayerStack;

    void Init();
    void Shutdown();

};

#endif