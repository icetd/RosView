#ifndef PLAN_H
#define PLAN_H

#include <string>
#include <manager_msgs/Plan.h>
#include <map>
#include "MSqlite.h"


class NavPlan 
{
public:
    NavPlan(std::string &plan_name);
    ~NavPlan();

    int Addgoal(const manager_msgs::Plan &goal);
    int Deletegoal(int goalId);
    int Clear();

    int pushPlan();
    int pullPlan();

    int GetCurrentGoalId() { return m_cur_goal_id; }
    int GetGoalNum() { return m_goal_num; }
    
    std::map <int, manager_msgs::Plan> GetGoalList() { return m_GoalList; }

    void SetName(std::string &name) { m_plan_name = name; }
    std::string GetName() { return m_plan_name; }

private:
    MSqlite m_plan;
    std::string m_plan_name;
    std::map <int, manager_msgs::Plan> m_GoalList;
    int m_goal_num;
    int m_cur_goal_id;
};

#endif