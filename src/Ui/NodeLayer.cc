#include "NodeLayer.h"
#include "TransForm.h"

extern bool show_node_settings_layout;


void NodeLayer::OnAttach()
{
    m_realrobotPos = {0.0f, 0.0f};
    m_robotAngle = 0.0f;
    m_Texture_Map = std::make_unique<Texture>();
    m_Texture_Map->bind("../res/textures/nav.pgm");
    
    m_Texture_Robot = std::make_unique<Texture>();
    m_Texture_Robot->bind("../res/textures/nav.png");

    m_AppNode = AppNode::GetInstance();
    m_AppNode->setOnAmclPoseCallback(std::bind(&NodeLayer::OnAmclPoseCallback, this, std::placeholders::_1));
    m_AppNode->setOnMapMetaCallback(std::bind(&NodeLayer::OnMapMeatCallback, this, std::placeholders::_1));
    m_AppNode->start();
}

void NodeLayer::OnUpdate(float ts)
{   
    if (show_node_settings_layout) 
        Show_Node_Layout(&show_node_settings_layout);
}

void NodeLayer::Show_Node_Layout(bool *p_open)
{
    {
        ImGuiContext &g = *GImGui;

        if (!(g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSize))
            ImGui::SetNextWindowSize(ImVec2(0.0f, ImGui::GetFontSize() * 12.0f), ImGuiCond_FirstUseEver);
        if (!ImGui::Begin(u8"ROS", p_open) || ImGui::GetCurrentWindow()->BeginCount > 1)
        {
            ImGui::End();
            return;
        }

        ImGui::PushItemWidth(-ImGui::GetWindowWidth() * 0.2f);
        ImGui::AlignTextToFramePadding();

        ImGui::NewLine();
        ImGui::InputTextWithHint("master", "set master url here", m_master_url, IM_ARRAYSIZE(m_master_url));
        static int radio_master = 0;
        ImGui::RadioButton(u8"连接 master", &radio_master, 1);
        ImGui::SameLine(0, 0);
        ImGui::RadioButton(u8"断开 master", &radio_master, 0);
        ImGui::SameLine(0, 20);

        if (radio_master) {
            m_AppNode->init(m_master_url);
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Start");
        } else {
            m_AppNode->destroy();
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Stop");
        }
 
        
        ImGui::Text(u8"手动控制:");
        ImGuiKey start_key = (ImGuiKey)0;
        ImGui::SameLine(0, 20);
        static std::string moveInfo;
        for (ImGuiKey key = start_key; key < ImGuiKey_NamedKey_END; key = (ImGuiKey)(key + 1))
        {
            if (!ImGui::IsKeyDown(key)) 
                continue;
            if (key < ImGuiKey_NamedKey_BEGIN) {
                switch(key) {
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
                        m_AppNode->move('S', 0.5, 0.5); //32 space
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

        if(moveInfo == u8"停止") 
            ImGui::TextColored(ImVec4(0.9f, 0.0f, 0.0f, 1.0f), moveInfo.c_str());
         else 
            ImGui::TextColored(ImVec4(0.0f, 0.9f, 0.0f, 1.0f), moveInfo.c_str());
        

		ImGui::Separator();
        ImGui::PopItemWidth();
        ImGui::End();

        {
            ImGui::Begin("View");
            OnRenderView();
            ImGui::End();
        }

        {
            ImGui::Begin("Info");
            OnRenderInfo();
            ImGui::End();
        }
    }
}


void NodeLayer::OnRenderView()
{
    ImGui::BeginChild("Map");
    ImVec2 viewStartPos = ImGui::GetCursorScreenPos();
	ImVec2 viewportSize = ImGui::GetContentRegionAvail();


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
    robotPos.x = viewStartPos.x + (m_realrobotPos.x - originPosx) / resolution * viewportSize.x / map_width;
    robotPos.y = viewStartPos.y + viewportSize.y - (m_realrobotPos.y - originPosy) / resolution * viewportSize.y / map_height;

    ImVec2 robotSize;
    robotSize.x = 1.0 / resolution ;
    robotSize.y = 1.0 / resolution ;


    ImGui::Image((ImTextureID)(intptr_t)m_Texture_Map->getId(), viewportSize, ImVec2(0, 1), ImVec2(1, 0));
    
    ImGui::EndChild();

    ImGui::BeginChild("Map");
    
    ImGui::GetWindowDrawList()->AddCircleFilled(robotPos, 2, ImColor(1.0f, 0.0f, 0.0f),  3.0f);
    ImageRotated((ImTextureID)(intptr_t)m_Texture_Robot->getId(), robotPos, robotSize, m_robotAngle);

    ImGui::EndChild();
}

void NodeLayer::OnRenderInfo()
{
    ImGui::NewLine(); 
    ImGui::Text(u8"当前位置:"); ImGui::SameLine(0, 20);
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "X: %.4fm", m_realrobotPos.x); ImGui::SameLine(0, 20);
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Y: %.4fm", m_realrobotPos.y); ImGui::SameLine(0, 20);
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Yaw: %.4f", m_robotAngle);
}

void NodeLayer::OnAmclPoseCallback(const geometry_msgs::PoseWithCovarianceStamped& pose)
{
   m_pose = pose;
}

void NodeLayer::OnMapMeatCallback(const nav_msgs::MapMetaData &mapMeta)
{
    m_mapMetaData = mapMeta;
}
