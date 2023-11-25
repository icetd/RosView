#include "AppNode.h"
#include <windows.h>
#include <map>
#include <vector>
#include <iostream>

AppNode *AppNode::instance = nullptr;
AppNode *AppNode::GetInstance()
{
    if (instance == nullptr) {
        instance = new AppNode();
    }
    return instance;
}

AppNode::AppNode() :
    isClient(false)
{
}

AppNode::~AppNode() 
{
}

void amcl_pose_callback(const geometry_msgs::PoseWithCovarianceStamped &pose)
{
    AppNode *appNode = AppNode::GetInstance();
    appNode->OnAmclPoseCallback(pose);
}

void map_meta_callback(const nav_msgs::MapMetaData &mapMeta)
{
    AppNode *appNode = AppNode::GetInstance();
    appNode->OnMapMetaCallback(mapMeta);
}

void AppNode::setOnAmclPoseCallback(std::function<void (const geometry_msgs::PoseWithCovarianceStamped & pose)> callback)
{
    OnAmclPoseCallback = std::move(callback);
}

void AppNode::setOnMapMetaCallback(std::function<void (const nav_msgs::MapMetaData &mapMeta)> callback)
{
    OnMapMetaCallback = std::move(callback);   
}

bool AppNode::init(char *master_url)
{
    bool ret;
    if (!isClient) {
        isClient = true;
        nh.initNode(master_url);
        nh.setSpinTimeout(10000);

        m_cmd_vel_pub = new ros::Publisher("cmd_vel", &twist_msg);
        nh.advertise(*m_cmd_vel_pub);
        
        m_amcl_pose_sub = new ros::Subscriber<geometry_msgs::PoseWithCovarianceStamped>("/amcl_pose", &amcl_pose_callback);
        nh.subscribe(*m_amcl_pose_sub);

        m_map_metaData_sub = new ros::Subscriber<nav_msgs::MapMetaData> ("/map_metadata", &map_meta_callback);
        nh.subscribe(*m_map_metaData_sub);

    }
    return true;
}

bool AppNode::destroy()
{
    if (isClient) {
        isClient = false;
    }
    return true;
}

void AppNode::move(char key, float speed_linear, float speed_angular)
{
    if (!isClient)
        return;
    std::map<char, std::vector<float>> moveBases {
        {'Q', { 1, 0, 0, 1}}, {'W', { 1, 0, 0, 0}}, {'E', { 1, 0, 0,-1}},
        {'A', { 0, 0, 0, 1}}, {'S', { 0, 0, 0, 0}}, {'D', { 0, 0, 0,-1}},
        {'Z', {-1, 0, 0,-1}}, {'X', {-1, 0, 0, 0}}, {'C', {-1, 0, 0, 1}},
    };

    float direction_x = moveBases[key][0];
    float direction_y = moveBases[key][1];
    float direction_z = moveBases[key][2];
    float direction_yaw = moveBases[key][3];

    twist_msg.linear.x = direction_x * speed_linear;
    twist_msg.linear.y = direction_y * speed_linear;
    twist_msg.linear.z = direction_z * speed_linear;
    twist_msg.angular.x = 0.0;
    twist_msg.angular.y = 0.0;
    twist_msg.angular.z = direction_yaw * speed_angular;

    m_cmd_vel_pub->publish(&twist_msg);
    nh.spinOnce(); 
}

void AppNode::run()
{   
    int ret = 0;
    while (!isStoped())
    {
        if (isClient) {
           ret = nh.spinOnce();
           Sleep(100);
        } else {
            Sleep(500);
        }
    }
}
