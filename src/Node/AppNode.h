#ifndef APP_NODE_H
#define APP_NODE_H

#include <ros.h>
#include <string>
#include <manager_msgs/Status.h>
#include <manager_msgs/Plan.h>
#include <geometry_msgs/PoseWithCovarianceStamped.h>
#include <geometry_msgs/Twist.h>
#include <nav_msgs/Odometry.h>
#include <nav_msgs/MapMetaData.h>
#include <functional>

#include "MThread.h"

class AppNode : public MThread
{
public:
    AppNode(AppNode &other) = delete;
    void operator = ( const AppNode &) = delete;

    static AppNode *GetInstance();
    bool init(char *master_url);
    bool destroy();
    void move(char key, float speed_linear, float speed_angular);

    void setClient(bool is_client) { isClient = is_client; }
    bool getClient() { return isClient; }

    std::function<void (const geometry_msgs::PoseWithCovarianceStamped &)> OnAmclPoseCallback;
    void setOnAmclPoseCallback(std::function<void (const geometry_msgs::PoseWithCovarianceStamped & pose)> callback);

    std::function<void (const nav_msgs::MapMetaData &)> OnMapMetaCallback;
    void setOnMapMetaCallback(std::function<void (const nav_msgs::MapMetaData &mapMeta)> callback);
    
protected:
    AppNode();
    virtual ~AppNode();
    virtual void run() override;

private:
    static AppNode *instance;

    ros::NodeHandle nh;
    std::string url;
 
    geometry_msgs::Twist twist_msg;

    ros::Publisher *m_cmd_vel_pub;
    ros::Subscriber<geometry_msgs::PoseWithCovarianceStamped> *m_amcl_pose_sub;
    ros::Subscriber<nav_msgs::MapMetaData> *m_map_metaData_sub;

    bool isClient;
};


#endif