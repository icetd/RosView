#ifndef UTILS_H
#define UTILS_H
#include "Layer.h"

static void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(help)");
    if (ImGui::BeginItemTooltip())
    {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

#endif