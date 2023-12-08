#ifndef MSQLITE_H
#define MSQLITE_H

#include <string>
#include <vector>
#include "sqlite3.h"


class MSqlite {
public:
	MSqlite();
	~MSqlite();

	int CreateDb(const std::string& path);
	int CreateTable(const std::string& sqlCreatetable);
	int DeleteTable(const std::string& sqlDeletetable);
	int OpenDb(const std::string& path);
	int Insert(const std::string& sqlInsert);
	int Delete(const std::string& sqlDelete);
	int Update(const std::string& sqlUpdate);
	int QueryData(const std::string& sqlQuery, std::vector<std::string>& arrKey, std::vector<std::vector<std::string>>& arrValue);

private:
	sqlite3* m_db;
	void Destroy();
};

#endif