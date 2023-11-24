#include "PlanManager.h"

PlanManager manager;

void testPlanManagerAdd()
{
    manager_msgs::Plan goal1;
    goal1.pose.position.x = 1.1f;
    goal1.pose.position.y = 1.1f;
    goal1.pose.position.z = 0.0f;
    goal1.pose.orientation.z = 0.3f;
    goal1.pose.orientation.w = 0.7f;
    goal1.type.status = manager_msgs::Type::MOVE;
    goal1.action_id = 0;

    manager_msgs::Plan goal2;
    goal2.pose.position.x = 1.1f;
    goal2.pose.position.y = 1.1f;
    goal2.pose.position.z = 0.0f;
    goal2.pose.orientation.z = 0.7f;
    goal2.pose.orientation.w = 0.3f;
    goal2.type.status = manager_msgs::Type::ACTION;
    goal2.action_id = 1;
    
    std::string plan1_name = "plan1";
    std::string plan2_name = "plan2";
    std::string plan3_name = "plan3";
    NavPlan plan1(plan1_name);
    NavPlan plan2(plan2_name);
    NavPlan plan3(plan3_name);
    

    plan1.Addgoal(goal1);
    plan1.Addgoal(goal2);

    plan2.Addgoal(goal1);
    plan2.Addgoal(goal2);
 
    plan3.Addgoal(goal1);
    plan3.Addgoal(goal2);


    manager.AddPlan(plan1);
    manager.AddPlan(plan2);
    manager.AddPlan(plan3);
    manager.pushPlans();
}

void testPlanManagerClear()
{
    manager.pushClearAll();
}

void testPlanManagerPull()
{
    manager.pullPlans();
}