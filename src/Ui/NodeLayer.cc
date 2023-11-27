#include "NodeLayer.h"
#include "TransForm.h"

extern bool show_node_settings_layout;
extern bool show_tool_log;

void NodeLayer::OnAttach()
{
    m_realrobotPos = {0.0f, 0.0f};
    m_robotAngle = 0.0f;
    m_Texture_Map = std::make_unique<Texture>();
    m_Texture_Map->bind("../res/textures/nav.pgm");
    
    m_Texture_Robot = std::make_unique<Texture>();
    m_Texture_Robot->bind("../res/textures/nav.png");

    m_PlanManager = std::make_unique<PlanManager>();
    m_PlanManager->pullPlans();
    m_current_goal = -1;
    std::string default_name = "default";
    m_make_plan =  std::make_unique<NavPlan>(default_name);
    m_make_plan->Clear();
    m_make_goal_num = 1;
    m_make_plan_num = 1;

    isCliented = false;
    m_AppNode = AppNode::GetInstance();
    m_AppNode->setOnAmclPoseCallback(std::bind(&NodeLayer::OnAmclPoseCallback, this, std::placeholders::_1));
    m_AppNode->setOnMapMetaCallback(std::bind(&NodeLayer::OnMapMeatCallback, this, std::placeholders::_1));
    m_AppNode->setOnPlanCallback(std::bind(&NodeLayer::OnPlanCallback, this, std::placeholders::_1));
    m_AppNode->start();
}

void NodeLayer::OnUpdate(float ts)
{   
    if (show_node_settings_layout) 
        Show_Node_Layout(&show_node_settings_layout);

    if (show_tool_log)
        Show_Tool_Log(&show_tool_log);
}

void NodeLayer::Show_Tool_Log(bool *p_open)
{
    ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
    ImGui::Begin(u8"日志", p_open);
    ImGui::End();
    m_Log.Draw(u8"日志", p_open);
}

void NodeLayer::Show_Node_Layout(bool *p_open)
{
    {
        ImGuiContext &g = *GImGui;

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
        ImGui::InputTextWithHint("URL", "set master url here", m_master_url, IM_ARRAYSIZE(m_master_url));
        static int radio_master = 0;
        ImGui::RadioButton(u8"连接机器人", &radio_master, 1); ImGui::SameLine(0, 20);
        ImGui::RadioButton(u8"断开机器人", &radio_master, 0); ImGui::SameLine(0, 20);

        if (radio_master) {
            m_AppNode->init(m_master_url);
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Start");
            isCliented = true;
        } else {
            m_AppNode->destroy();
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Stop");
            isCliented = false;
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
        

    }
}

void NodeLayer::OnRenderView()
{
    /*get view info for all child*/
    m_viewStartPos = ImGui::GetCursorScreenPos();
    m_viewportSize = ImGui::GetContentRegionAvail();
    if (!isCliented)
        return;
    ViewMap();
    ViewRobot();
    ViewPlanPoint();
}

void NodeLayer::ViewMap()
{
    ImGui::BeginChild("Map");
    ImGui::Image((ImTextureID)(intptr_t)m_Texture_Map->getId(), m_viewportSize, ImVec2(0, 1), ImVec2(1, 0));
    ImGui::EndChild();
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
    robotSize.x = 1.0 / resolution ;
    robotSize.y = 1.0 / resolution ;

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
    for (auto &plan : m_PlanManager->GetPlanList().find(m_current_plan + 1)->second.GetGoalList()) {
        realGoalPosx = plan.second.pose.position.x;
        realGoalPosy = plan.second.pose.position.y;
        GoalPos[i].x = m_viewStartPos.x + (realGoalPosx - originPosx) / resolution * m_viewportSize.x / map_width;
        GoalPos[i].y = m_viewStartPos.y + m_viewportSize.y - (realGoalPosy - originPosy) / resolution * m_viewportSize.y / map_height;
        if (plan.second.id > 1)
            ImGui::GetWindowDrawList()->AddLine(GoalPos[i], GoalPos[!i], IM_COL32(0, 255, 255, 100), 2.0f);

        ImGui::GetWindowDrawList()->AddText(GoalPos[i], IM_COL32(128, 0, 255, 255), std::to_string(plan.second.id).c_str());
        ImGui::GetWindowDrawList()->AddCircleFilled(GoalPos[i], 3.0f, IM_COL32(72, 181, 33, 255));
        i = !i;
    }
    ImGui::EndChild();
}

void NodeLayer::OnRenderNav()
{
    if (!isCliented)
        return;
    NavShowPlan();
    NavMessage();
}

void NodeLayer::NavShowPlan()
{
    ImGui::SeparatorText(u8"线路选择");
    ImGui::NewLine();
    
    // 尝试将c_str() 返回的指针添加到 std::vector<const char*> 中，
    // 但这可能导致问题。c_str() 返回的指针是指向字符串内部缓冲区的指针，而这个缓冲区是临时的，当 GetName() 的结果超出作用域时就会被销毁，从而导致 planNameList 中的指针成为悬空指针。
    // 将字符串复制到 std::vector<std::string> 中，然后使用 std::vector<const char*> 作为 ImGui Combo 的参数。这样可以确保字符串的生命周期足够长，避免悬空指针的问题。

    if (m_PlanManager->GetPlanNum() ==  0)
        return;

    std::vector<std::string> planNameList;
    std::map<int, NavPlan> planList = m_PlanManager->GetPlanList();
    m_make_plan_num = planList.size() + 1;
    for (auto &plan : planList) {   
        planNameList.push_back(plan.second.GetName());   
    }
    std::vector<const char*> planNamesArray;
    for (const auto &str : planNameList) {
        planNamesArray.push_back(str.c_str());
    }
    ImGui::Combo(u8"线路选择", &m_current_plan, planNamesArray.data(), static_cast<int>(planNamesArray.size()));

    ImGui::NewLine();

    if (m_current_plan >= m_make_plan_num -1)
        return;
    std::vector<std::string> goalNameList;
    std::map<int, manager_msgs::Plan> goalList = m_PlanManager->GetPlanList().find(m_current_plan + 1)->second.GetGoalList();
    for (auto &goal : goalList) {
        goalNameList.push_back("goal_" + std::to_string(goal.second.id));
    }
    std::vector<const char*> goalNamesArray;
    for (const auto &str : goalNameList) {
        goalNamesArray.push_back(str.c_str());
    }
    
    static int item_current_idx = 0; 
    if (ImGui::BeginListBox(u8"目标点"))
    {
        for (int n = 0; n < static_cast<int>(goalNamesArray.size()); n++)
        {   
            if (n == m_current_goal) {
                ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "%s", goalNamesArray[n]);
            } else {
                ImGui::Text("%s", goalNamesArray[n]);
            }
        }
        ImGui::EndListBox();
    }
    ImGui::NewLine();
    if(ImGui::Button(u8"发布路线", ImVec2(110, 30)) && m_PlanManager->GetCurrentPlanId() > 0) {

        manager_msgs::Status status;
        status.status = status.DELETEALL;
        m_AppNode->pubCmdPlan(status);
        m_current_goal = -1;
        
        for(auto &plan : m_PlanManager->GetPlanList().at(m_current_plan + 1).GetGoalList()) {
            m_AppNode->pubPlan(plan.second);
        }
    }

    ImGui::SameLine(0, 10);
    if(ImGui::Button(u8"开始执行", ImVec2(110, 30)) && m_PlanManager->GetCurrentPlanId() > 0) {
        m_current_goal = -1;
        manager_msgs::Status status;
        status.status = status.START;
        m_AppNode->pubCmdPlan(status);
    }

    if(ImGui::Button(u8"暂停导航", ImVec2(110, 30)) && m_PlanManager->GetCurrentPlanId() > 0) {
        manager_msgs::Status status;
        status.status = status.CANCEL;
        m_AppNode->pubCmdPlan(status);
    }

    ImGui::SameLine(0, 10);
    if(ImGui::Button(u8"继续导航", ImVec2(110, 30)) && m_PlanManager->GetCurrentPlanId() > 0) {
        manager_msgs::Status status;
        status.status = status.CONTINUE;
        m_AppNode->pubCmdPlan(status);
    }


}

void NodeLayer::NavMessage()
{
    ImGui::NewLine();
    ImGui::SeparatorText(u8"消息");
    ImGui::Text(u8"当前位置:"); ImGui::SameLine(0, 20);
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "X: %.4fm", m_realrobotPos.x); ImGui::SameLine(0, 20);
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Y: %.4fm", m_realrobotPos.y); ImGui::SameLine(0, 20);
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Yaw: %.4f", m_robotAngle);

    ImGui::Text(u8"执行消息"); ImGui::SameLine(0, 20);
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "%s", m_plan_back_msg.c_str());
 
    HelpMarker(u8"1.选择路线后，可以在目标点以及左侧视窗中查看具体目标点。\n" \
                 "2.目标点中按顺序列出了所有当前线路途经的目标，机器人导航过程中最近途经的目标点高亮显示。\n" \
                 "3.点击发布路线，可以将当前路线发布给机器。\n" \
                 "4.点击开始执行，当前发布的线路被激活，机器人开始执行计划路线。\n" \
                 "5.点击暂停导航，机器人暂停当前路线。\n" \
                 "6.点击继续导航，机器人继续当前路线。\n" \
                 "7.最后一个目标点高亮时表示当前路线执行完毕。");
}

void NodeLayer::OnRenderMake()
{
    if (!isCliented)
        return;
    MakePlan();
}

void NodeLayer::MakePlan()
{
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

    static char temp[128];
    sprintf(temp, "plan_%d", m_make_plan_num);
    ImGui::InputTextWithHint(u8"路线名", "input plan name here", temp, IM_ARRAYSIZE(temp));

    const char *goal_type_items[] = {"STOP", "PAUSE", "MOVE", "ACTION"};
    static int goal_type = 2;
    ImGui::Combo(u8"目标点类型", &goal_type, goal_type_items, IM_ARRAYSIZE(goal_type_items));

    const char *action_id_items[] = {"NULL", u8"取油", u8"扫码", u8"过桥"};
    static int action_id = 0;
    ImGui::Combo("ACTION_ID", &action_id, action_id_items, IM_ARRAYSIZE(action_id_items));


    std::string plan_name = temp;
    if(ImGui::Button(u8"添加goal", ImVec2(110, 30))) {
        manager_msgs::Plan goal;
        goal.pose.position.x = m_pose.pose.pose.position.x;
        goal.pose.position.y = m_pose.pose.pose.position.y;
        goal.pose.position.z = m_pose.pose.pose.position.z;
        goal.pose.orientation.z = m_pose.pose.pose.orientation.z;
        goal.pose.orientation.w = m_pose.pose.pose.orientation.w;
        goal.type.status = goal_type;
        goal.action_id = action_id;
        goal.id = m_make_goal_num;
        m_make_plan->Addgoal(goal);
        m_make_goal_num++;
    }
    ImGui::SameLine(0, 20);
    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "goal_%d", m_make_goal_num);
 
    if(ImGui::Button(u8"删除goal", ImVec2(110, 30)) && m_make_goal_num > 1) {
        m_make_goal_num--;
        m_make_plan->Deletegoal(m_make_goal_num);
    }
    ImGui::SameLine(0, 20);
    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "goal_%d", m_make_goal_num-1);

    ImGui::NewLine();
    if(ImGui::Button(u8"添加Plan", ImVec2(110, 30)) && m_make_goal_num > 1) {
        m_make_plan->SetName(plan_name);
        m_PlanManager->AddPlan(*m_make_plan);
        m_make_plan->Clear();
        m_make_goal_num = 1;
        m_make_plan_num++;
    }
    ImGui::SameLine(0, 20);
    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "plan_%d", m_make_plan_num);
    
    if(ImGui::Button(u8"删除Plan", ImVec2(110, 30)) && m_make_plan_num > 1) {
        m_make_plan_num--;
        m_PlanManager->DeletePlan(m_make_plan_num);
        m_make_plan->Clear();
        m_make_goal_num = 1;
    }
    ImGui::SameLine(0, 20);
    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "plan_%d", m_make_plan_num-1);

    ImGui::NewLine();
    if(ImGui::Button(u8"提交路线到数据库", ImVec2(110, 30)) && m_make_plan_num > 1) {
        m_PlanManager->pushPlans();
    }

    ImGui::SameLine(0, 20);
    if(ImGui::Button(u8"从数据库更新路线", ImVec2(110, 30))) {
        int ret = 0;
        m_make_plan->Clear();
        m_make_plan_num = 1;
        m_make_goal_num = 1;
        m_PlanManager->ClearPlan();
        ret = m_PlanManager->pullPlans();
    }

    if(ImGui::Button(u8"清空本地路线", ImVec2(110, 30))) {
        m_make_plan->Clear();
        m_make_plan_num = 1;
        m_make_goal_num = 1;
        m_PlanManager->ClearPlan();
    }

    ImGui::SameLine(0, 20);
    if(ImGui::Button(u8"清空数据库", ImVec2(110, 30))) {
        m_PlanManager->pushClearAll();
    }

    ImGui::NewLine();
    if(ImGui::Button(u8"保存地图", ImVec2(110, 30)) && m_PlanManager->GetCurrentPlanId() > 0) {
        manager_msgs::Status status;
        status.status = status.MAPSAVE;
        m_AppNode->pubCmdPlan(status);
    }

    HelpMarker(u8"手动控制机器人到达指定位置后添加目标点到路线");
}


void NodeLayer::OnAmclPoseCallback(const geometry_msgs::PoseWithCovarianceStamped& pose)
{
   m_pose = pose;
}

void NodeLayer::OnMapMeatCallback(const nav_msgs::MapMetaData &mapMeta)
{
    m_mapMetaData = mapMeta;
}

void NodeLayer::OnPlanCallback(const std_msgs::String &str)
{
    m_planBackString = str;
    m_plan_back_msg = str.data;
    int temp = 0;
    
    m_Log.AddLog("%s\n", m_plan_back_msg.c_str());

    int ret = sscanf(m_planBackString.data, "Plan MOVE [%d] successed", &temp);
    if(ret)
        m_current_goal = temp - 1;
    
    ret = sscanf(m_planBackString.data, "Plan TASK [%d] successed", &temp);
    if(ret)
        m_current_goal = temp - 1;

    ret = sscanf(m_planBackString.data, "Plan STOP [%d] successed", &temp);
    if(ret)
        m_current_goal = temp - 1;
}