#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#include <iostream>
#include <ros.h>
#include <Windows.h>
#include "Application.h"
#include "log.h"
#include "MainLayer.h"
#include "VideoLayer.h"
#include "NodeLayer.h"

#define VLD 0

#if VLD
#include <vld.h>
#endif

int main(int argc, char **argv)
{
    initLogger(INFO);
    Application *app = new Application("ROSVIEW", 1920, 1080);
    app->PushLayer<MainLayer>();
    app->PushLayer<VideoLayer>();
    app->PushLayer<NodeLayer>();
    app->Run();
    
    delete app;
    return 0;
}