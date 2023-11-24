#ifndef NODE_LAYER_H
#define NODE_LAYER_H

#include "Layer.h"
#include "AppNode.h"
#include <memory>
#include <functional>
#include "Texuture.h"

class NodeLayer : public Layer
{
protected:
    virtual void OnAttach() override;
    virtual void OnUpdate(float ts) override;

private:
    void Show_Node_Layout(bool *p_open);
    char m_master_url[128] = "192.168.2.162:11411";
    AppNode *m_AppNode;
    
    geometry_msgs::PoseWithCovarianceStamped m_pose = {};
    void OnAmclPoseCallback(const geometry_msgs::PoseWithCovarianceStamped &pose);

    nav_msgs::MapMetaData m_mapMetaData = {};
    void OnMapMeatCallback(const nav_msgs::MapMetaData &mapMeta);
    
    std::unique_ptr<Texture> m_Texture_Map;
    std::unique_ptr<Texture> m_Texture_Robot;

    ImVec2 m_realrobotPos;
    float m_robotAngle;

    void OnRenderView();
    void OnRenderInfo();
};

#endif