#pragma once

#include <imgui.h>
#include <string_view>
#include <fmt/format.h>

namespace ImGuiHelper
{

template <typename ...Param>
static void Text(std::string_view fmt, Param &&...param)
{
	ImGui::TextUnformatted(fmt::format(fmt, std::forward<Param>(param)...).c_str());
}

}
