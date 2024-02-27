#ifndef NODE_LAYER_H
#define NODE_LAYER_H

#include <memory>
#include <functional>
#include "Layer.h"
#include "AppNode.h"
#include "Texuture.h"
#include "PlanManager.h"
#include "TcpClient.h"
#include "MTimer.h"
#include "Utils.h"
#include "INIReader.h"


class NodeLayer : public Layer
{
protected:
	virtual void OnAttach() override;
	virtual void OnUpdate(float ts) override;
	virtual void OnDetach() override;

private:
	MTimer *m_timer;
	void TimerSendToPowerControl();

	typedef enum {
		UREF_BOARD = 0,
		IREF_CHG,
		IREF_RUN,
		IREF_POWR0,
		IREF_CTRL,
		IREF_XYZ,
		IREF_OIL
	} CurrentType_t;

	std::string m_powerControl_message;
	std::string m_powerControl_url;
	uint16_t m_powerControl_port;
	bool isPowerControlCliented;
	TcpClient *m_powerControl_client;
	float m_robot_voltage;
	float m_robot_power;
	float m_robot_current[7];
	float m_robot_total_current;

	int16_t m_current_index;
	uint16_t m_voltage;
	void OnPowerControlMessage(std::string message);

	std::string m_master_url;
	AppNode* m_AppNode = nullptr;
	bool isRosCliented;

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
	float m_viewScale;
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

	
	void OnPowerControlView();
	void OnMessagePowerView();
	void OnMessageOilNeedleView();

	/*Ros Mav Make Plan View*/

	std::unique_ptr<NavPlan> m_make_plan;
	int m_make_goal_num;
	int m_make_plan_num;
	void OnRenderMake();
	void MakePlan();

	INIReader *m_config;
};

#endif