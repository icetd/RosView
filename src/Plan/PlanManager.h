#ifndef PLAN_MANAGER_H
#define PLAN_MANAGER_H

#include <map>
#include "NavPlan.h"

class PlanManager
{
public:
	PlanManager();
	~PlanManager();

	int AddPlan(NavPlan& plan);
	int DeletePlan(int plan_id);
	int ClearPlan();

	int GetCurrentPlanId() { return m_cur_plan_id; }
	int GetPlanNum() { return m_plan_num; }
	std::map <int, NavPlan>& GetPlanList() { return m_planList; }

	int pushPlans();
	int pushClearAll();
	int pullPlans();

private:
	MSqlite m_plans;
	std::map<int, NavPlan> m_planList;
	int m_plan_num;
	int m_cur_plan_id;
	bool isPullAlready;
};

#endif
