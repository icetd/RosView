#include <iostream>
#include <ros.h>
#include <Windows.h>
#include "Application.h"
#include "log.h"

#include "MainLayer.h"
#include "VideoLayer.h"
#include "NodeLayer.h"
#include "log.h"

int main(int argc, char **argv)
{
    initLogger(INFO);
    Application *app = new Application("ROSVIEW", 1920, 1080);
    app->PushLayer<MainLayer>();
    app->PushLayer<VideoLayer>();
    app->PushLayer<NodeLayer>();
    app->Run();
    return 0;
}