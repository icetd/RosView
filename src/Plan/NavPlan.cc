#include "NavPlan.h"

NavPlan::NavPlan(std::string &plan_name) :
    m_cur_goal_id(0),
    m_plan_name(plan_name),
    m_goal_num(0)
{
    Clear();
}

NavPlan::~NavPlan()
{
}

int NavPlan::Addgoal(const manager_msgs::Plan &goal)
{   
    m_cur_goal_id++;
    m_goal_num = m_cur_goal_id;
    m_GoalList.insert(std::make_pair(m_cur_goal_id, goal));
    return 1;
}

int NavPlan::Deletegoal(int goalId)
{
    if (goalId <= m_goal_num) {
        m_GoalList.erase(goalId);
        m_cur_goal_id--;
    }
    return 1;
}

int NavPlan::Clear()
{
    m_GoalList.clear();
    m_cur_goal_id = 0;
    m_goal_num = 0;

    return 1;
}

int NavPlan::pushPlan()
{
    std::string sql;
    char temp[512];
    memset(temp, 0, 512);
    int re = m_plan.OpenDb("../res/db/navDataBase.db");
    if (re != SQLITE_OK)
        return 0;
     
    sprintf(temp, "create table %s(id integer primary key autoincrement,goal_id integer,goal_type integer,action_id intefer, px REAL,py REAL,oz REAL,ow REAL)", 
                                    m_plan_name.c_str());
    sql = temp;
    m_plan.CreateTable(sql);
    if (re != SQLITE_OK)
        return 0;
    
    for(auto iter = m_GoalList.begin(); iter != m_GoalList.end(); ++iter) {
        sprintf(temp, "insert into %s(id, goal_id, goal_type, action_id, px, py, oz, ow) values('%d', '%d', '%d', %d, '%f', '%f', '%f', '%f')",
                       m_plan_name.c_str(),
                       iter->first, iter->second.id, iter->second.type.status, iter->second.action_id,
                       iter->second.pose.position.x, iter->second.pose.position.y,
                       iter->second.pose.orientation.z, iter->second.pose.orientation.w);
        sql = temp;
        m_plan.Insert(sql);
    }
    return 1;
}