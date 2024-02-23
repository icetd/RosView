#ifndef LAYER_H
#define LAYER_H

#include "imgui.h"
#include "imgui_internal.h"
#include "implot.h"
#include "implot_internal.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Layer
{
public:
    virtual ~Layer() = default;

    virtual void OnAttach() {}
    virtual void OnDetach() {}
    virtual void OnUpdate(float ts) {};
    virtual void OnUIRender() {}
};

#endif