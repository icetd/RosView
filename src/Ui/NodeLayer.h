#ifndef NODE_LAYER_H
#define NODE_LAYER_H

#include <memory>
#include <functional>
#include "Layer.h"
#include "AppNode.h"
#include "Texuture.h"
#include "PlanManager.h"
#include "Utils.h"


class NodeLayer : public Layer
{
protected:
	virtual void OnAttach() override;
	virtual void OnUpdate(float ts) override;
	virtual void OnDetach() override;

private:
	char m_master_url[128] = "192.168.2.162:11411";
	AppNode* m_AppNode = nullptr;
	bool isCliented;

	geometry_msgs::PoseWithCovarianceStamped m_pose = {};
	std::vector<geometry_msgs::PoseWithCovarianceStamped> m_pose_list;
	void OnAmclPoseCallback(const geometry_msgs::PoseWithCovarianceStamped& pose);

	nav_msgs::MapMetaData m_mapMetaData = {};
	void OnMapMeatCallback(const nav_msgs::MapMetaData& mapMeta);

	std_msgs::String m_planBackString = {};
	std::string m_plan_back_msg;
	void OnPlanCallback(const std_msgs::String& str);

	/*Ros all View*/
	AppLog m_Log;
	void Show_Tool_Log(bool* p_open);
	void Show_Node_Layout(bool* p_open);


	/*Ros Map View*/
	std::unique_ptr<Texture> m_Texture_Map;
	std::unique_ptr<Texture> m_Texture_Robot;
	ImVec2 m_viewStartPos; // start pos for all child
	ImVec2 m_viewportSize; // view size for all child
	ImVec2 m_realrobotPos;
	float m_robotAngle;
	void OnRenderView();
	void ViewMap();
	void ViewRobot();
	void ViewPlanPoint();

	int m_isShowTrajectory;
	void ViewTrajectory();


	/*Ros Nav Plan View*/
	std::unique_ptr<PlanManager> m_PlanManager;
	int m_current_plan;
	int m_current_goal;
	void OnRenderNav();
	void NavShowPlan();

	void NavMessage();

	/*Ros Mav Make Plan View*/

	std::unique_ptr<NavPlan> m_make_plan;
	int m_make_goal_num;
	int m_make_plan_num;
	void OnRenderMake();
	void MakePlan();

};

#endif