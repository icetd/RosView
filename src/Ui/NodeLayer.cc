#include "NodeLayer.h"
#include "TransForm.h"
#include "VoltageCalculate.h"
#include <string>
#include <iostream>

extern bool show_node_settings_layout;
extern bool show_tool_log;

void NodeLayer::OnAttach()
{
	m_realrobotPos = { 0.0f, 0.0f };
	m_robotAngle = 0.0f;
	m_pose_list.resize(5000);
	m_isShowTrajectory = false;

	m_Texture_Map = std::make_unique<Texture>();
	m_Texture_Map->bind("./res/textures/nav.pgm");

	m_Texture_Robot = std::make_unique<Texture>();
	m_Texture_Robot->bind("./res/textures/nav.png");

	m_PlanManager = std::make_unique<PlanManager>();
	m_PlanManager->pullPlans();
	m_current_goal = -1;
	std::string default_name = "default";
	m_make_plan = std::make_unique<NavPlan>(default_name);
	m_make_plan->Clear();
	m_make_goal_num = 1;
	m_make_plan_num = 1;


	isPowerControlCliented = false;
	m_power_run = false;
	m_power_ctrl = false;
	m_power_xyz = false;
	m_power_oil = false;
	m_power_led_far = false;
	m_power_led_near = false;
	m_power_led_oil = false;
	m_robot_total_current = 0;


	isRosCliented = false;
	

	m_config = new INIReader("./configs/url.ini");
	m_master_url = m_config->Get("ROS", "URL_MASTER", "192.168.2.162:11411");
	m_powerControl_url = m_config->Get("ROSPOWERCONTROL", "URL", "192.168.2.200");
	m_powerControl_port = m_config->GetInteger("ROSPOWERCONTROL", "PORT", 23);



	m_AppNode = AppNode::GetInstance();
	m_AppNode->setOnAmclPoseCallback(std::bind(&NodeLayer::OnAmclPoseCallback, this, std::placeholders::_1));
	m_AppNode->setOnMapMetaCallback(std::bind(&NodeLayer::OnMapMeatCallback, this, std::placeholders::_1));
	m_AppNode->setOnPlanCallback(std::bind(&NodeLayer::OnPlanCallback, this, std::placeholders::_1));
	m_AppNode->start();
}

void NodeLayer::OnDetach()
{	
	m_AppNode->destroy();

	if (m_AppNode)
		delete m_AppNode;
	if (m_config)
		delete m_config;

	if (m_timer) 
		delete m_timer;

	if (m_powerControl_client)
		delete m_powerControl_client;
}

void NodeLayer::OnUpdate(float ts)
{
	if (show_node_settings_layout)
		Show_Node_Layout(&show_node_settings_layout);

	if (show_tool_log)
		Show_Tool_Log(&show_tool_log);
}

void NodeLayer::Show_Tool_Log(bool* p_open)
{
	ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
	ImGui::Begin(u8"日志", p_open);
	ImGui::End();
	m_Log.Draw(u8"日志", p_open);
}

void NodeLayer::Show_Node_Layout(bool* p_open)
{
	{
		ImGuiContext& g = *GImGui;

		if (!(g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSize))
			ImGui::SetNextWindowSize(ImVec2(0.0f, ImGui::GetFontSize() * 12.0f), ImGuiCond_FirstUseEver);
		if (!ImGui::Begin(u8"机器人", p_open) || ImGui::GetCurrentWindow()->BeginCount > 1)
		{
			ImGui::End();
			return;
		}

		ImGui::PushItemWidth(-ImGui::GetWindowWidth() * 0.2f);
		ImGui::AlignTextToFramePadding();

		ImGui::SeparatorText(u8"连接");
		ImGui::Text(u8"\t\t机器人地址:\t%s", m_master_url.c_str());
		static int radio_master = 0;
		ImGui::RadioButton(u8"连接机器人", &radio_master, 1); ImGui::SameLine(0, 20);
		ImGui::RadioButton(u8"断开机器人", &radio_master, 0); ImGui::SameLine(0, 20);

		if (radio_master) {
			m_AppNode->init((char *)m_master_url.c_str());
			ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Start");
			isRosCliented = true;
		}
		else {
			m_AppNode->destroy();
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Stop");
			isRosCliented = false;
		}

		HelpMarker(u8"输入机器人地址后，点击连接机器人");
		ImGui::PopItemWidth();
		ImGui::End();
		// add map show view
		{
			ImGui::Begin(u8"地图");
			OnRenderView();
			ImGui::End();
		}

		// add maker plan view
		{
			if (!(g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSize))
				ImGui::SetNextWindowSize(ImVec2(0.0f, ImGui::GetFontSize() * 12.0f), ImGuiCond_FirstUseEver);

			ImGui::Begin(u8"路线制作");
			ImGui::PushItemWidth(-ImGui::GetWindowWidth() * 0.2f);
			ImGui::AlignTextToFramePadding();

			OnRenderMake();

			ImGui::End();
		}

		// add nav plan view
		{
			if (!(g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSize))
				ImGui::SetNextWindowSize(ImVec2(0.0f, ImGui::GetFontSize() * 12.0f), ImGuiCond_FirstUseEver);

			ImGui::Begin(u8"导航");
			ImGui::PushItemWidth(-ImGui::GetWindowWidth() * 0.2f);
			ImGui::AlignTextToFramePadding();

			OnRenderNav();

			ImGui::PopItemWidth();
			ImGui::End();
		}

		// add power message view
		{
			if (!(g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSize))
				ImGui::SetNextWindowSize(ImVec2(0.0f, ImGui::GetFontSize() * 12.0f), ImGuiCond_FirstUseEver);

			ImGui::Begin(u8"电源状态");
			ImGui::PushItemWidth(-ImGui::GetWindowWidth() * 0.2f);
			ImGui::AlignTextToFramePadding();

			OnMessagePowerView();

			ImGui::PopItemWidth();
			ImGui::End();
		}

		// add oil needle status view
		{
			if (!(g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSize))
				ImGui::SetNextWindowSize(ImVec2(0.0f, ImGui::GetFontSize() * 12.0f), ImGuiCond_FirstUseEver);

			ImGui::Begin(u8"油管状态");
			ImGui::PushItemWidth(-ImGui::GetWindowWidth() * 0.2f);
			ImGui::AlignTextToFramePadding();

			OnMessageOilNeedleView();

			ImGui::PopItemWidth();
			ImGui::End();
		}

		// add powerControl view
		{
			if (!(g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSize))
				ImGui::SetNextWindowSize(ImVec2(0.0f, ImGui::GetFontSize() * 12.0f), ImGuiCond_FirstUseEver);

			ImGui::Begin(u8"电源控制");
			ImGui::PushItemWidth(-ImGui::GetWindowWidth() * 0.2f);
			ImGui::AlignTextToFramePadding();
			
			OnPowerControlView();
			
			ImGui::PopItemWidth();
			ImGui::End();
		}
	}
}

void NodeLayer::OnRenderView()
{
	/*get view info for all child*/
	m_viewStartPos = ImGui::GetCursorScreenPos();
	m_viewportSize = ImGui::GetContentRegionAvail();
	if (!isRosCliented)
		return;
	ViewMap();
	ViewRobot();
	ViewPlanPoint();
	if (m_isShowTrajectory)
		ViewTrajectory();
}

void NodeLayer::ViewMap()
{
	ImGui::BeginChild("Map");
	ImGui::Image((ImTextureID)(intptr_t)m_Texture_Map->getId(), m_viewportSize, ImVec2(0, 1), ImVec2(1, 0));
	ImGui::EndChild();
	    // 处理相机控制
}

void NodeLayer::ViewRobot()
{
	float resolution = m_mapMetaData.resolution;
	double map_width = m_mapMetaData.width;
	double map_height = m_mapMetaData.height;

	double realPosx = m_pose.pose.pose.position.x;
	double realPosy = m_pose.pose.pose.position.y;
	double originPosx = m_mapMetaData.origin.position.x;    // 左下角原点在 map坐标系中的坐标
	double originPosy = m_mapMetaData.origin.position.y;    // Textue 加载时上下颠倒
	m_realrobotPos.x = realPosx;
	m_realrobotPos.y = realPosy;

	Quaternion quaterion;
	quaterion.w = m_pose.pose.pose.orientation.w;
	quaterion.x = m_pose.pose.pose.orientation.x;
	quaterion.y = m_pose.pose.pose.orientation.y;
	quaterion.z = m_pose.pose.pose.orientation.z;

	EulerAngles eulerAngles;
	eulerAngles = ToEulerAngles(quaterion);
	m_robotAngle = -eulerAngles.yaw;

	ImVec2 robotPos;
	robotPos.x = m_viewStartPos.x + (m_realrobotPos.x - originPosx) / resolution * m_viewportSize.x / map_width;
	robotPos.y = m_viewStartPos.y + m_viewportSize.y - (m_realrobotPos.y - originPosy) / resolution * m_viewportSize.y / map_height;

	ImVec2 robotSize;
	robotSize.x = 1.0 / resolution;
	robotSize.y = 1.0 / resolution;

	ImGui::BeginChild("Map");
	ImageRotated((ImTextureID)(intptr_t)m_Texture_Robot->getId(), robotPos, robotSize, m_robotAngle);
	ImGui::EndChild();
}

void NodeLayer::ViewPlanPoint()
{
	if (m_current_plan >= m_make_plan_num - 1)
		return;
	ImGui::BeginChild("Map");
	float resolution = m_mapMetaData.resolution;
	double map_width = m_mapMetaData.width;
	double map_height = m_mapMetaData.height;

	double originPosx = m_mapMetaData.origin.position.x;    // 左下角原点在 map坐标系中的坐标
	double originPosy = m_mapMetaData.origin.position.y;    // Textue 加载时上下颠倒

	double realGoalPosx;
	double realGoalPosy;
	ImVec2 GoalPos[2];

	bool i = 0;
	for (auto& plan : m_PlanManager->GetPlanList().find(m_current_plan + 1)->second.GetGoalList()) {
		realGoalPosx = plan.second.pose.position.x;
		realGoalPosy = plan.second.pose.position.y;
		GoalPos[i].x = m_viewStartPos.x + (realGoalPosx - originPosx) / resolution * m_viewportSize.x / map_width;
		GoalPos[i].y = m_viewStartPos.y + m_viewportSize.y - (realGoalPosy - originPosy) / resolution * m_viewportSize.y / map_height;
		if (plan.second.id > 1)
			ImGui::GetWindowDrawList()->AddLine(GoalPos[i], GoalPos[!i], IM_COL32(0, 255, 255, 100), 2.0f);

		ImGui::GetWindowDrawList()->AddText(GoalPos[i], IM_COL32(128, 0, 255, 255), std::to_string(plan.second.id).c_str());
		ImGui::GetWindowDrawList()->AddCircleFilled(GoalPos[i], 3.0f, IM_COL32(33, 181, 33, 255));
		i = !i;
	}
	ImGui::EndChild();
}

void NodeLayer::ViewTrajectory()
{
	ImGui::BeginChild("Map");
	float resolution = m_mapMetaData.resolution;
	double map_width = m_mapMetaData.width;
	double map_height = m_mapMetaData.height;

	double originPosx = m_mapMetaData.origin.position.x;    // 左下角原点在 map坐标系中的坐标
	double originPosy = m_mapMetaData.origin.position.y;    // Textue 加载时上下颠倒

	double realGoalPosx;
	double realGoalPosy;
	ImVec2 showPoint;

	for (auto& curpose : m_pose_list) {
		realGoalPosx = curpose.pose.pose.position.x;
		realGoalPosy = curpose.pose.pose.position.y;
		showPoint.x = m_viewStartPos.x + (realGoalPosx - originPosx) / resolution * m_viewportSize.x / map_width;
		showPoint.y = m_viewStartPos.y + m_viewportSize.y - (realGoalPosy - originPosy) / resolution * m_viewportSize.y / map_height;
		ImGui::GetWindowDrawList()->AddCircleFilled(showPoint, 2.0f, IM_COL32(255, 0, 0, 50));
	}

	ImGui::EndChild();
}

void NodeLayer::OnRenderNav()
{
	if (!isRosCliented)
		return;
	NavShowPlan();
}

void NodeLayer::NavShowPlan()
{
	ImGui::SeparatorText(u8"线路选择");
	// 尝试将c_str() 返回的指针添加到 std::vector<const char*> 中，
	// 但这可能导致问题。c_str() 返回的指针是指向字符串内部缓冲区的指针，而这个缓冲区是临时的，当 GetName() 的结果超出作用域时就会被销毁，从而导致 planNameList 中的指针成为悬空指针。
	// 将字符串复制到 std::vector<std::string> 中，然后使用 std::vector<const char*> 作为 ImGui Combo 的参数。这样可以确保字符串的生命周期足够长，避免悬空指针的问题。

	if (m_PlanManager->GetPlanNum() == 0)
		return;

	std::vector<std::string> planNameList;
	std::map<int, NavPlan> planList = m_PlanManager->GetPlanList();
	m_make_plan_num = planList.size() + 1;
	for (auto& plan : planList) {
		planNameList.push_back(plan.second.GetShowName());
	}
	std::vector<const char*> planNamesArray;
	for (const auto& str : planNameList) {
		planNamesArray.push_back(str.c_str());
	}
	ImGui::Combo(u8"线路选择", &m_current_plan, planNamesArray.data(), static_cast<int>(planNamesArray.size()));

	if (m_current_plan >= m_make_plan_num - 1)
		return;

	std::vector<std::string> goalNameList;
	std::map<int, std::string> goalList = m_PlanManager->GetPlanList().find(m_current_plan + 1)->second.GetGoalNameList();
	for (auto& goalname : goalList) {
		goalNameList.push_back(goalname.second);
	}
	std::vector<const char*> goalNamesArray;
	for (const auto& str : goalNameList) {
		goalNamesArray.push_back(str.c_str());
	}

	static int item_current_idx = 0;
	if (ImGui::BeginListBox(u8"目标点")) {
		for (int n = 0; n < static_cast<int>(goalNamesArray.size()); n++) {
			if (n <= m_current_goal) {
				ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "%s", goalNamesArray[n]);
			}
			else {
				ImGui::Text("%s", goalNamesArray[n]);
			}
		}
		ImGui::EndListBox();
	}
	ImGui::RadioButton(u8"显示轨迹", &m_isShowTrajectory, 1); ImGui::SameLine(0, 20);
	ImGui::RadioButton(u8"不显示轨迹", &m_isShowTrajectory, 0); ImGui::SameLine(0, 20);
	if (ImGui::Button(u8"清空轨迹", ImVec2(80, 22))) {
		m_pose_list.clear();
	}
	ImGui::NewLine();
	
	if (ImGui::Button(u8"发布路线", ImVec2(110, 30)) && m_PlanManager->GetCurrentPlanId() > 0) {

		manager_msgs::Status status;
		status.status = status.DELETEALL;
		m_AppNode->pubCmdPlan(status);
		m_current_goal = -1;
		m_pose_list.clear();

		for (auto& plan : m_PlanManager->GetPlanList().at(m_current_plan + 1).GetGoalList()) {
			m_AppNode->pubPlan(plan.second);
		}
	}

	ImGui::SameLine(0, 10);
	if (ImGui::Button(u8"开始执行", ImVec2(110, 30)) && m_PlanManager->GetCurrentPlanId() > 0) {
		m_current_goal = -1;
		m_pose_list.clear();
		manager_msgs::Status status;
		status.status = status.START;
		m_AppNode->pubCmdPlan(status);
	}

	if (ImGui::Button(u8"暂停导航", ImVec2(110, 30)) && m_PlanManager->GetCurrentPlanId() > 0) {
		manager_msgs::Status status;
		status.status = status.CANCEL;
		m_AppNode->pubCmdPlan(status);
	}

	ImGui::SameLine(0, 10);
	if (ImGui::Button(u8"继续导航", ImVec2(110, 30)) && m_PlanManager->GetCurrentPlanId() > 0) {
		manager_msgs::Status status;
		status.status = status.CONTINUE;
		m_AppNode->pubCmdPlan(status);
	}
	ImGui::SeparatorText(u8"消息");
	ImGui::Text(u8"当前位置:"); ImGui::SameLine(0, 20);
	ImGui::TextColored(ImVec4(0.9f, 0.9f, 0.1f, 1.0f), "X: %.4fm", m_realrobotPos.x); ImGui::SameLine(0, 20);
	ImGui::TextColored(ImVec4(0.9f, 0.9f, 0.1f, 1.0f), "Y: %.4fm", m_realrobotPos.y); ImGui::SameLine(0, 20);
	ImGui::TextColored(ImVec4(0.9f, 0.9f, 0.1f, 1.0f), "Yaw: %.4f", m_robotAngle);

	ImGui::Text(u8"执行消息"); ImGui::SameLine(0, 20);
	ImGui::TextColored(ImVec4(0.1f, 0.6f, 0.1f, 1.0f), "%s", m_plan_back_msg.c_str());
	
	HelpMarker(u8"1.选择路线后，可以在目标点以及左侧视窗中查看具体目标点。\n"
			   "2.目标点中按顺序列出了所有当前线路途经的目标，机器人导航过程中最近途经的目标点高亮显示。\n"
			   "3.点击发布路线，可以将当前路线发布给机器。\n"
			   "4.点击开始执行，当前发布的线路被激活，机器人开始执行计划路线。\n"
			   "5.点击暂停导航，机器人暂停当前路线。\n"
			   "6.点击继续导航，机器人继续当前路线。\n"
			   "7.最后一个目标点高亮时表示当前路线执行完毕。");
}

void NodeLayer::OnPowerControlView()
{
	
	ImGui::SeparatorText(u8"电源控制");
	ImGui::Text(u8"\t\t电源控制地址:\t%s:%d", m_powerControl_url.c_str(), m_powerControl_port);
	static int radio_power;
	ImGui::RadioButton(u8"连接电源控制", &radio_power, 1); ImGui::SameLine(0, 20);
	ImGui::RadioButton(u8"断开电源控制", &radio_power, 0); ImGui::SameLine(0, 20);

	if (radio_power) {
		ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Start");
		if (!isPowerControlCliented) {
			m_powerControl_client = new TcpClient(m_powerControl_url, m_powerControl_port);
			if (m_powerControl_client->Init() != 0)
				exit(1);
			m_powerControl_client->SetMessageCallback(std::bind(&NodeLayer::OnPowerControlMessage, this, std::placeholders::_1));
			m_powerControl_client->Start();

			m_timer = new MTimer(166);
			m_timer->SetTimerCallback(std::bind(&NodeLayer::TimerSendToPowerControl, this));
			m_timer->Start();

			isPowerControlCliented = true;
		}
	} else {
		if (isPowerControlCliented) {
			m_timer->stop();
			m_powerControl_client->stop();
		}
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Stop");
		isPowerControlCliented = false;
	}

	if (!isPowerControlCliented)
		return;

	ImGui::NewLine();
	ImGui::SeparatorText(u8"电源开关");
	static bool btn_power_run;
	ImGui::Text(u8"行走电源"); ImGui::SameLine(0, 20);
	MToggleButton(u8"行走电源_", &btn_power_run);
	if (btn_power_run && !m_power_run) {
		m_powerControl_client->SendData("s1", 2);
		m_power_run = true;
	} else if (!btn_power_run && m_power_run){
		m_powerControl_client->SendData("r1", 2);
		m_power_run = false;
	}

	ImGui::SameLine(0, 40);
	static bool btn_power_ctrl;
	ImGui::Text(u8"控制电源"); ImGui::SameLine(0, 20);
	MToggleButton(u8"控制电源_", &btn_power_ctrl);
	if (btn_power_ctrl && !m_power_ctrl) {
		m_powerControl_client->SendData("s2", 2);
		m_power_ctrl = true;
	} else if (!btn_power_ctrl && m_power_ctrl){
		m_powerControl_client->SendData("r2", 2);
		m_power_ctrl = false;
	}

	static bool btn_power_xyz;
	ImGui::Text(u8"对接电源"); ImGui::SameLine(0, 20);
	MToggleButton(u8"对接电源_", &btn_power_xyz);
	if (btn_power_xyz && !m_power_xyz) {
		m_powerControl_client->SendData("s3", 2);
		m_power_xyz = true;
	} else if (!btn_power_xyz && m_power_xyz){
		m_powerControl_client->SendData("r3", 2);
		m_power_xyz = false;
	}
	
	ImGui::SameLine(0, 40);
	static bool btn_power_oil;
	ImGui::Text(u8"取油电源"); ImGui::SameLine(0, 20);
	MToggleButton(u8"取油电源_", &btn_power_oil);
	if (btn_power_oil && !m_power_oil) {
		m_powerControl_client->SendData("s4", 2);
		m_power_oil = true;
	} else if (!btn_power_oil && m_power_oil){
		m_powerControl_client->SendData("r4", 2);
		m_power_oil = false;
	}

	ImGui::NewLine();
	ImGui::SeparatorText(u8"灯光开关");
	static bool btn_power_led_near;
	ImGui::Text(u8"车近光灯"); ImGui::SameLine(0, 20);
	MToggleButton(u8"车近光灯_", &btn_power_led_near);
	if (btn_power_led_near && !m_power_led_near) {
		m_powerControl_client->SendData("o7", 2);
		m_power_led_near = true;
	} else if (!btn_power_led_near && m_power_led_near){
		m_powerControl_client->SendData("c7", 2);
		m_power_led_near = false;
	}

	ImGui::SameLine(0, 40);
	static bool btn_power_led_far;
	ImGui::Text(u8"车远光灯"); ImGui::SameLine(0, 20);
	MToggleButton(u8"车远光灯_", &btn_power_led_far);
	if (btn_power_led_far && !m_power_led_far) {
		m_powerControl_client->SendData("o8", 2);
		m_power_led_far = true;
	} else if (!btn_power_led_far && m_power_led_far){
		m_powerControl_client->SendData("c8", 2);
		m_power_led_far = false;
	}

	static bool btn_power_led_oil;
	ImGui::Text(u8"取油室灯"); ImGui::SameLine(0, 20);
	MToggleButton(u8"取油室灯_", &btn_power_led_oil);
	if (btn_power_led_oil && !m_power_led_oil) {
		m_powerControl_client->SendData("o5", 2);
		m_power_led_oil = true;
	} else if (!btn_power_led_oil && m_power_led_oil){
		m_powerControl_client->SendData("c5", 2);
		m_power_led_oil = false;
	}
}

void NodeLayer::OnMessagePowerView()
{
	ImGui::SeparatorText(u8"电源状态");
    static ScrollingBuffer dataV;
    static ScrollingBuffer dataC1, dataC2, dataC3, dataC4, dataC5, dataC6, dataCAll;
	
	static float t = 0;
    t += ImGui::GetIO().DeltaTime;
	dataV.AddPoint(t, m_robot_power);
    dataC1.AddPoint(t, m_robot_current[1]);
    dataC2.AddPoint(t, m_robot_current[2]);
    dataC3.AddPoint(t, m_robot_current[3]);
    dataC4.AddPoint(t, m_robot_current[4]);
    dataC5.AddPoint(t, m_robot_current[5]);
    dataC6.AddPoint(t, m_robot_current[6]);
    dataCAll.AddPoint(t, m_robot_total_current);
    
	static float history = 10.0f;

    if (ImPlot::BeginPlot("##ScrollVoltage", ImVec2(-1, 150) )) {
        ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_NoTickLabels, 0);
        ImPlot::SetupAxisLimits(ImAxis_X1, t - history, t, ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1, -1, 100);
        ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.5f);
		ImPlot::PushStyleColor(ImPlotCol_Fill, ImVec4(0.5f, 0.1f, 0.1f, 0.9f));
		ImPlot::PlotShaded(u8"电量(%)", &dataV.Data[0].x, &dataV.Data[0].y, dataV.Data.size(), -INFINITY, 0, dataV.Offset, 2 * sizeof(float));
		ImPlot::PopStyleColor();
        ImPlot::EndPlot();
    }

    if (ImPlot::BeginPlot("##ScrollCurrent", ImVec2(-1, 250) )) {
        ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_NoTickLabels, ImPlotAxisFlags_AutoFit);
        ImPlot::SetupAxisLimits(ImAxis_X1, t - history, t, ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1, -0.5, 10);
        ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.5f);
			
		ImPlot::PlotLine(u8"充电电流(A)", &dataC1.Data[0].x, &dataC1.Data[0].y, dataC1.Data.size(), 0, dataC1.Offset, 2 * sizeof(float));
        ImPlot::PlotLine(u8"行走电流(A)", &dataC2.Data[0].x, &dataC2.Data[0].y, dataC2.Data.size(), 0, dataC2.Offset, 2 * sizeof(float));
        ImPlot::PlotLine(u8"待机电流(A)", &dataC3.Data[0].x, &dataC3.Data[0].y, dataC3.Data.size(), 0, dataC3.Offset, 2 * sizeof(float));
        ImPlot::PlotLine(u8"控制电流(A)", &dataC4.Data[0].x, &dataC4.Data[0].y, dataC4.Data.size(), 0, dataC4.Offset, 2 * sizeof(float));
        ImPlot::PlotLine(u8"取油电流(A)", &dataC5.Data[0].x, &dataC5.Data[0].y, dataC5.Data.size(), 0, dataC5.Offset, 2 * sizeof(float));
        ImPlot::PlotLine(u8"对接电流(A)", &dataC6.Data[0].x, &dataC6.Data[0].y, dataC6.Data.size(), 0, dataC6.Offset, 2 * sizeof(float));
        ImPlot::PlotLine(u8"总电流(A)",   &dataCAll.Data[0].x, &dataCAll.Data[0].y, dataCAll.Data.size(), 0, dataCAll.Offset, 2 * sizeof(float));

		ImPlot::EndPlot();
    }

    ImGui::SliderFloat(u8"历史时间", &history, 1, 30, "%.1f s");
}

void NodeLayer::OnMessageOilNeedleView()
{
	ImGui::SeparatorText(u8"油管状态");
	ImGui::NewLine();
	if (ImPlot::BeginPlot(u8"针管油量", ImVec2(-1, 400)))
	{
		ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 90);
		ImPlot::PushStyleColor(ImPlotCol_Fill, ImVec4(0.5f, 0.5f, 0.1f, 0.8f));
		ImPlot::PlotBars(u8"油量(ml)", m_oil_value, 6, 0.7, 1);
		ImPlot::PopStyleColor();
		ImPlot::EndPlot();
	}
}

void NodeLayer::OnRenderMake()
{
	if (!isRosCliented)
		return;
	MakePlan();
}

void NodeLayer::MakePlan()
{

	static char text[128] = {}; // 设置输入框默认参数
	ImGui::Text(u8"制作路线请输入密码:");
	ImGui::SameLine(0, 20);
	ImGui::InputText(u8"输入密码",text, IM_ARRAYSIZE(text), ImGuiInputTextFlags_Password, 0, text);
	if (strcmp((char*)text, "DeepCloud123") != 0)
		return;

	ImGui::NewLine();
	ImGui::SeparatorText(u8"制作路线");
	ImGui::Text(u8"手动控制:");
	ImGuiKey start_key = (ImGuiKey)0;
	ImGui::SameLine(0, 20);
	static std::string moveInfo;
	for (ImGuiKey key = start_key; key < ImGuiKey_NamedKey_END; key = (ImGuiKey)(key + 1)) {
		if (!ImGui::IsKeyDown(key))
			continue;
		if (key < ImGuiKey_NamedKey_BEGIN) {
			switch (key) {
			case 81:
				m_AppNode->move('Q', 0.5, 0.5);
				moveInfo = u8"左前";
				break;
			case 87:
				m_AppNode->move('W', 0.5, 0.5);
				moveInfo = u8"前";
				break;
			case 69:
				m_AppNode->move('E', 0.5, 0.5);
				moveInfo = u8"右前";
				break;
			case 65:
				m_AppNode->move('A', 0.5, 0.5);
				moveInfo = u8"左转";
				break;
			case 32:
				m_AppNode->move('S', 0.5, 0.5); // 32 space
				moveInfo = u8"停止";
				break;
			case 68:
				m_AppNode->move('D', 0.5, 0.5);
				moveInfo = u8"右转";
				break;
			case 90:
				m_AppNode->move('Z', 0.5, 0.5);
				moveInfo = u8"左后";
				break;
			case 83:
				m_AppNode->move('X', 0.5, 0.5); // 83 S
				moveInfo = u8"后";
				break;
			case 67:
				m_AppNode->move('C', 0.5, 0.5);
				moveInfo = u8"右后";
				break;
			default:
				break;
			}
		}
	}

	if (moveInfo == u8"停止")
		ImGui::TextColored(ImVec4(0.9f, 0.0f, 0.0f, 1.0f), moveInfo.c_str());
	else
		ImGui::TextColored(ImVec4(0.0f, 0.9f, 0.0f, 1.0f), moveInfo.c_str());

	static char temp_name[128];
	sprintf(temp_name, "plan_%d", m_make_plan_num);
	ImGui::InputTextWithHint(u8"路线编号", "input plan name here", temp_name, IM_ARRAYSIZE(temp_name));

	static char temp_show_name[128];
	ImGui::InputTextWithHint(u8"路线名", "input plan name here", temp_show_name, IM_ARRAYSIZE(temp_show_name));

	const char* goal_type_items[] = { "STOP", "PAUSE", "MOVE", "ACTION" };
	static int goal_type = 2;
	ImGui::Combo(u8"目标点类型", &goal_type, goal_type_items, IM_ARRAYSIZE(goal_type_items));

	const char* action_id_items[] = { "NULL", u8"OIL1", u8"OIL2", u8"OIL3", u8"OIL4", u8"OIL5", u8"OIL6"};
	static int action_id = 0;
	ImGui::Combo("ACTION_ID", &action_id, action_id_items, IM_ARRAYSIZE(action_id_items));
	
	static char temp_goal_name[128] = {};
	ImGui::InputTextWithHint(u8"目标点名称", u8"input plan name here", temp_goal_name, IM_ARRAYSIZE(temp_goal_name));

	static char needle_capacity[128] = {};
	static char alignment_offset[128] = {};

	ImGui::InputTextWithHint(u8"针管容量", u8"input needle capacity (ml)", needle_capacity, IM_ARRAYSIZE(needle_capacity));
	ImGui::InputTextWithHint(u8"对准偏移", u8"input alignment offset (mm)", alignment_offset, IM_ARRAYSIZE(alignment_offset));

	std::string plan_name = temp_name;
	std::string plan_name_show = temp_show_name;
	std::string goal_name = temp_goal_name;

	
	if (ImGui::Button(u8"添加goal", ImVec2(110, 30))) {
		manager_msgs::Plan goal;
		goal.pose.position.x = m_pose.pose.pose.position.x;
		goal.pose.position.y = m_pose.pose.pose.position.y;
		goal.pose.position.z = m_pose.pose.pose.position.z;
		goal.pose.orientation.z = m_pose.pose.pose.orientation.z;
		goal.pose.orientation.w = m_pose.pose.pose.orientation.w;
		goal.type.status = goal_type;
		goal.id = m_make_goal_num;
		goal.action_id = action_id;
		goal.needle_capacity = atoi(needle_capacity);
		goal.alignment_offset = atoi(alignment_offset);

		m_make_plan->Addgoal(goal, goal_name);
		m_make_goal_num++;
	}
	ImGui::SameLine(0, 20);
	ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "goal_%d", m_make_goal_num);

	ImGui::SameLine(0, 20);
	if (ImGui::Button(u8"删除goal", ImVec2(110, 30)) && m_make_goal_num > 1) {
		m_make_goal_num--;
		m_make_plan->Deletegoal(m_make_goal_num);
	}
	ImGui::SameLine(0, 20);
	ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "goal_%d", m_make_goal_num - 1);
	
	if (ImGui::Button(u8"添加Plan", ImVec2(110, 30)) && m_make_goal_num > 1) {
		m_make_plan->SetName(plan_name);
		m_make_plan->SetShowName(plan_name_show);
		m_PlanManager->AddPlan(*m_make_plan);
		m_make_plan->Clear();
		m_make_goal_num = 1;
		m_make_plan_num++;
	}
	ImGui::SameLine(0, 20);
	ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "plan_%d", m_make_plan_num);

	ImGui::SameLine(0, 20);
	if (ImGui::Button(u8"删除Plan", ImVec2(110, 30)) && m_make_plan_num > 1) {
		m_make_plan_num--;
		m_PlanManager->DeletePlan(m_make_plan_num);
		m_make_plan->Clear();
		m_make_goal_num = 1;
	}
	ImGui::SameLine(0, 20);
	ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "plan_%d", m_make_plan_num - 1);
	
	ImGui::NewLine();
	if (ImGui::Button(u8"提交路线到数据库", ImVec2(110, 30)) && m_make_plan_num > 1) {
		m_PlanManager->pushPlans();
	}

	ImGui::SameLine(0, 20);
	if (ImGui::Button(u8"从数据库更新路线", ImVec2(110, 30))) {
		int ret = 0;
		m_make_plan->Clear();
		m_make_plan_num = 1;
		m_make_goal_num = 1;
		m_PlanManager->ClearPlan();
		ret = m_PlanManager->pullPlans();
	}

	if (ImGui::Button(u8"清空本地路线", ImVec2(110, 30))) {
		m_make_plan->Clear();
		m_make_plan_num = 1;
		m_make_goal_num = 1;
		m_PlanManager->ClearPlan();
	}

	ImGui::SameLine(0, 20);
	if (ImGui::Button(u8"清空数据库", ImVec2(110, 30))) {
		m_PlanManager->pushClearAll();
	}

	ImGui::NewLine();
	if (ImGui::Button(u8"保存地图", ImVec2(110, 30)) && m_PlanManager->GetCurrentPlanId() > 0) {
		manager_msgs::Status status;
		status.status = status.MAPSAVE;
		m_AppNode->pubCmdPlan(status);
	}

	HelpMarker(u8"手动控制机器人到达指定位置后添加目标点到路线");
}

void NodeLayer::TimerSendToPowerControl()
{
	static int channel = 0;
	
	char tempbuf[8];
	if (channel > 6) {
		channel = 0;
	}

	sprintf(tempbuf, "g%d", channel);
	channel ++;

    m_powerControl_client->SendData(tempbuf, 8);
}

void NodeLayer::OnPowerControlMessage(std::string message)
{
	sscanf(message.c_str(), "U%hd=%hd", &m_current_index, &m_voltage);
	static float temp;
	switch (m_current_index) {
	case CurrentType_t::UREF_BOARD:
		temp = (float)m_voltage / 4096.0f * 3.0 * 11.39;
		m_robot_voltage = calcaulateVoltage(temp);
		m_robot_power = calculatePower(m_robot_voltage);
		break;
	case CurrentType_t::IREF_CHG:
		m_robot_current[m_current_index] = (float)m_voltage / 4096.0f * 3.0 / 0.4 * 1.04; // 1.04 is test error
		break;
	case CurrentType_t::IREF_RUN:
		m_robot_current[m_current_index] = (float)m_voltage / 4096.0f * 3.0 / 0.2 * 1.23;  // 1.23 is test error
		break;
	case CurrentType_t::IREF_POWR0:
		m_robot_current[m_current_index] = (float)m_voltage / 4096.0f * 3.0 / 0.84 * 1.03; // 1.03 is test error
		break;
	case CurrentType_t::IREF_CTRL:
		m_robot_current[m_current_index] = (float)m_voltage / 4096.0f * 3.0 / 0.84 * 1.03;
		break;
	case CurrentType_t::IREF_OIL:
		m_robot_current[m_current_index] = (float)m_voltage / 4096.0f * 3.0 / 0.51 * 1.03;
		break;
	case CurrentType_t::IREF_XYZ:
		m_robot_current[m_current_index] = (float)m_voltage / 4096.0f * 3.0 / 0.51 * 1.03;
		break;
	}

	m_robot_total_current = m_robot_current[2] + 
							m_robot_current[3] + 
							m_robot_current[4] + 
							m_robot_current[5] + 
							m_robot_current[6];
}

void NodeLayer::OnAmclPoseCallback(const geometry_msgs::PoseWithCovarianceStamped& pose)
{

	if (m_pose.pose.pose.position.x != pose.pose.pose.position.x && 
		m_pose.pose.pose.position.y != pose.pose.pose.position.y)
		m_pose_list.push_back(pose);

	m_pose = pose;
}

void NodeLayer::OnMapMeatCallback(const nav_msgs::MapMetaData& mapMeta)
{
	m_mapMetaData = mapMeta;
}

void NodeLayer::OnPlanCallback(const std_msgs::String& str)
{
	m_planBackString = str;
	m_plan_back_msg = str.data;
	int temp = 0;
	static uint16_t oil_id;
	static float oil_value;


	int ret = sscanf(m_planBackString.data, "Plan MOVE [%d] successed", &temp);
	if (ret)
		m_current_goal = temp - 1;

	ret = sscanf(m_planBackString.data, "Plan TASK [%d] successed", &temp);
	if (ret)
		m_current_goal = temp - 1;

	ret = sscanf(m_planBackString.data, "Plan STOP [%d] successed", &temp);
	if (ret)
		m_current_goal = temp - 1;

	ret = sscanf(m_planBackString.data, "OIL [%d] [%f]", &oil_id, &oil_value);
	if (ret == 2)
		m_oil_value[oil_id - 1] = oil_value;

	if (ret != 2)
		m_Log.AddLog("%s\n", m_plan_back_msg.c_str());
}