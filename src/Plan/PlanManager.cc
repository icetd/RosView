#include "PlanManager.h"
#include "log.h"
#include <iostream>

PlanManager::PlanManager() :
	m_cur_plan_id(0),
	m_plan_num(0),
	isPullAlready(false)
{
	ClearPlan();
}

PlanManager::~PlanManager()
{
}

int PlanManager::AddPlan(NavPlan& plan)
{
	m_cur_plan_id++;
	m_plan_num = m_cur_plan_id;
	m_planList.insert(std::make_pair(m_cur_plan_id, plan));
	return 1;
}

int PlanManager::DeletePlan(int plan_id)
{
	if (plan_id <= m_plan_num) {
		m_planList.erase(plan_id);
		m_cur_plan_id--;
		m_plan_num = m_cur_plan_id;
	}
	return 1;
}

int PlanManager::ClearPlan()
{
	m_planList.clear();
	m_cur_plan_id = 0;
	m_plan_num = 0;
	return 1;
}

int PlanManager::pushClearAll()
{
	if (m_planList.empty())
		return 0;
	std::string sql;
	char temp[256];

	int re = m_plans.OpenDb("../res/db/navDataBase.db");
	if (re == SQLITE_OK) {
		for (auto iter = m_planList.begin(); iter != m_planList.end(); ++iter) {

			sprintf(temp, "DROP TABLE main.%s", iter->second.GetName().c_str());
			sql = temp;
			re = m_plans.DeleteTable(sql);
			//iter->second.Clear();

			sprintf(temp, "delete from nav_plans where plan_name='%s'", iter->second.GetName().c_str());
			sql = temp;
			re = m_plans.Delete(sql);
		}
	}

	//ClearPlan();
	return 1;
}

int PlanManager::pushPlans()
{
	if (!isPullAlready) {
		LOG(NOTICE, "Please pull plan from database First.");
		return 0;
	}


	std::string sql;
	char temp[256];

	int re = m_plans.OpenDb("../res/db/navDataBase.db");
	if (re == SQLITE_OK) {
		for (auto iter = m_planList.begin(); iter != m_planList.end(); ++iter) {
			sprintf(temp, "insert into nav_plans(id, plan_id, plan_name) values('%d', '%d', '%s')",
				iter->first, iter->first, iter->second.GetName().c_str());
			sql = temp;
			re = m_plans.Insert(sql);
			iter->second.pushPlan();
		}
	}

	return 1;
}

int PlanManager::pullPlans()
{
	std::string sql;
	char temp[256];
	std::vector<std::string> arrKey;
	std::vector<std::vector<std::string>> arrValue;

	arrKey.clear();
	arrValue.clear();

	int re = m_plans.OpenDb("../res/db/navDataBase.db");
	if (re == SQLITE_OK) {
		sprintf(temp, "select * from nav_plans");
		sql = temp;
		re = m_plans.QueryData(sql, arrKey, arrValue);

		if (re == SQLITE_OK && !arrKey.empty() && !arrValue.empty()) {
			int plan_num = arrValue.size();
			for (int i = 0; i < plan_num; ++i) {
				int plan_id = atoi(arrValue[i][1].c_str());
				std::string plan_name = arrValue[i][2];
				LOG(INFO, "add %s from database.", plan_name.c_str());
				NavPlan plan(plan_name);
				this->AddPlan(plan);
			}

			for (auto iter = m_planList.begin(); iter != m_planList.end(); ++iter) {
				sprintf(temp, "select * from %s", iter->second.GetName().c_str());
				sql = temp;
				re = m_plans.QueryData(sql, arrKey, arrValue);
				if (re == SQLITE_OK && !arrKey.empty() && !arrValue.empty()) {
					int goal_num = arrValue.size();
					for (int i = 0; i < goal_num; ++i) {
						manager_msgs::Plan goal;
						goal.id = atoi(arrValue[i][1].c_str());
						goal.type.status = atoi(arrValue[i][2].c_str());
						goal.action_id = atoi(arrValue[i][3].c_str());
						goal.pose.position.x = atof(arrValue[i][4].c_str());
						goal.pose.position.y = atof(arrValue[i][5].c_str());
						goal.pose.position.z = 0.0f;
						goal.pose.orientation.z = atof(arrValue[i][6].c_str());
						goal.pose.orientation.w = atof(arrValue[i][7].c_str());
						iter->second.Addgoal(goal);
					}
				}
				else {
					LOG(WARN, "query data failed. Please Clear && Initizlize the database.");
					return 0;
				}
			}
		}
		else {
			LOG(NOTICE, "query data failed. Please Initizlize the database.");
		}
	}
	isPullAlready = true;
	return 1;
}