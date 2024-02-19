#ifndef PLAN_H
#define PLAN_H

#include <string>
#include <manager_msgs/Plan.h>
#include <map>
#include "MSqlite.h"


class NavPlan
{
public:
	NavPlan(std::string& plan_name);
	~NavPlan();

	int Addgoal(const manager_msgs::Plan& goal, std::string& goalName);
	int Deletegoal(int goalId);
	int Clear();

	int pushPlan();

	int GetCurrentGoalId() { return m_cur_goal_id; }
	int GetGoalNum() { return m_goal_num; }

	std::map <int, manager_msgs::Plan> GetGoalList() { return m_GoalList; }
	std::map <int, std::string> GetGoalNameList() { return m_GoalNameList; }

	void SetName(std::string &name) { m_plan_name = name; }
	void SetShowName(std::string &showname) { m_plan_show_name = showname; }
	std::string GetName() { return m_plan_name; }
	std::string GetShowName() { return m_plan_show_name; }

private:
	MSqlite m_plan;
	std::string m_plan_name;
	std::string m_plan_show_name;
	std::map <int, manager_msgs::Plan> m_GoalList;
	std::map <int, std::string> m_GoalNameList;
	int m_goal_num;
	int m_cur_goal_id;
};

#endif