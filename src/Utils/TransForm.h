#ifndef TRANS_FORM_H
#define TRANS_FORM_H

#include <cmath>
#include <imgui.h>
#include <imgui_internal.h>
#include <implot.h>
#include <implot_internal.h>

#define M_PI 3.141592653589793

struct Quaternion {
	double w, x, y, z;
};

struct EulerAngles {
	double roll, pitch, yaw;
};

EulerAngles ToEulerAngles(Quaternion q);
void ImageRotated(ImTextureID tex_id, ImVec2 center, ImVec2 size, float angle);

#endif