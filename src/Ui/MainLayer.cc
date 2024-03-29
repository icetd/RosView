#include "MainLayer.h"
#include "Utils.h"
#include "StyleManager.h"
#include "Application.h"

bool show_video_layout = true;
bool show_node_settings_layout = true;
bool show_tool_log = true;
bool isExit;

void MainLayer::OnUpdate(float ts)
{
	static bool opt_fullscreen = true;
	static bool opt_padding = false;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen) {
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}
	else {
		dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
	}

	// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
	// and handle the pass-thru hole, so we ask Begin() to not render a background.
	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;


	// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
	// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
	// all active windows docked into it will lose their parent and become undocked.
	// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
	// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
	if (!opt_padding)
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	ImGui::Begin("Main", nullptr, window_flags);
	if (!opt_padding)
		ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	// Submit the DockSpace
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu(u8"开始"))
		{
			ImGui::MenuItem(u8"退出", NULL, &isExit);
			if (isExit)
				Application::GetInstance()->Close();
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu(u8"视窗"))
		{
			ImGui::MenuItem(u8"视频", NULL, &show_video_layout);
			ImGui::MenuItem(u8"机器人", NULL, &show_node_settings_layout);
			ImGui::MenuItem(u8"日志", NULL, &show_tool_log);
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu(u8"设置"))
		{
			// Disabling fullscreen would allow the window to be moved to the front of other windows,
			// which we can't undo at the moment without finer window depth/z control.
			// ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
			ImGui::MenuItem("Padding", NULL, &opt_padding);
			ImGui::Separator();

			if (ImGui::MenuItem("Flag: NoDockingOverCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingOverCentralNode) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoDockingOverCentralNode; }
			if (ImGui::MenuItem("Flag: NoDockingSplit", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingSplit) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoDockingSplit; }
			if (ImGui::MenuItem("Flag: NoUndocking", "", (dockspace_flags & ImGuiDockNodeFlags_NoUndocking) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoUndocking; }
			if (ImGui::MenuItem("Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoResize; }
			if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar; }
			if (ImGui::MenuItem("Flag: PassthruCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0, opt_fullscreen)) { dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode; }
			ImGui::Separator();
			StyleManager::ShowStyleSelector(u8"选择主题");
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu(u8"帮助"))
		{
			HelpMarker(
			  u8"菜单说明:\n"
				"\t开始--用于退出程序 (键盘ESC也可退出程序)\n"
				"\t视窗--控制界面是否显示\n"
				"\t设置--界面以及主题设置\n"
				"\t帮助--帮助信息\n\n"
				"界面说明:\n" \
				"\t地图--显示地图、导航路线、以及机器人位置\n" \
				"\t电源控制--用于控制机器人各个电源以及灯光\n" \
				"\t导航--用于路线的选择、发布\n" \
				"\t路线制作--用于路线的制作\n\n" \
				"\t机器人--用于连接机器人\n" \
				"\t视频--用于控制视频流的开关\n" \
				"\t\t前摄像头--机器人前方视频流\n" \
				"\t\t对接摄像头--对接扫码视频流\n" \
				"\t\t取油室摄像头--取油针管观察视频流\n\n" \
				"\t日志--机器人返回信息\n" \
				"\t电源状态--显示电量、电流\n" \
				"\t油管状态--显示油管油量\n\n" \
				"操作步骤：\n" \
				"\t1: 在右侧电源控制栏连接电源控制板，打开所有电源，等待机器人系统启动\n" \
				"\t2: 在右侧视频栏窗口点击打开视频\n" \
				"\t3: 在右侧机器人栏窗口点击连接机器人\n"\
				"\t4: 在右侧导航栏窗口选择好路线后发布路线\n\n" \
				"特殊说明：\n" \
				"\t 路线制作功能谨慎使用!!!");

			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
	ImGui::End();
}