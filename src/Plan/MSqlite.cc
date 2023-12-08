#include "MSqlite.h"
#include "log.h"

MSqlite::MSqlite() :
	m_db(nullptr)
{

}

MSqlite::~MSqlite()
{
	Destroy();
}

int MSqlite::CreateDb(const std::string& path)
{
	int re = 0;
	re = sqlite3_open(path.c_str(), &m_db);
	if (re != SQLITE_OK)
		LOG(ERRO, "create sqlite3 db %s falied.", path.c_str());
	return re;
}

int MSqlite::CreateTable(const std::string& sqlCreatetable)
{
	int re = 0;
	char* errMsg = nullptr;
	re = sqlite3_exec(m_db, sqlCreatetable.c_str(), NULL, NULL, &errMsg);
	if (re != SQLITE_OK) {
		LOG(WARN, "create table failed: %s", errMsg);
		if (errMsg)
			free(errMsg);
	}
	return re;
}

int MSqlite::DeleteTable(const std::string& sqlDeletetable)
{
	int re = 0;
	char* errMsg = nullptr;
	re = sqlite3_exec(m_db, sqlDeletetable.c_str(), NULL, NULL, &errMsg);
	if (re != SQLITE_OK) {
		LOG(WARN, "delete table failed: %s", errMsg);
		if (errMsg)
			sqlite3_free(errMsg);
	}
	return re;
}

int MSqlite::OpenDb(const std::string& path)
{
	int re = 0;
	re = sqlite3_open(path.c_str(), &m_db);
	if (re != SQLITE_OK)
		LOG(ERRO, "failed open db %s", path.c_str());
	return re;
}

int MSqlite::Insert(const std::string& sqlInsert)
{
	if (sqlInsert.empty())
		return -1;

	int re = 0;
	char* errMsg = nullptr;
	re = sqlite3_exec(m_db, sqlInsert.c_str(), NULL, NULL, &errMsg);
	if (re != SQLITE_OK) {
		LOG(ERRO, "falied insert msg case: %s", errMsg);
		if (errMsg)
			free(errMsg);
	}
}

int MSqlite::Delete(const std::string& sqlDelete)
{
	int nCols = 0;
	int nRows = 0;
	char** pazResult = 0;
	char* errMsg = nullptr;

	int re = sqlite3_get_table(m_db, sqlDelete.c_str(), &pazResult, &nRows, &nCols, &errMsg);
	if (re != SQLITE_OK) {
		LOG(ERRO, "delete error %s", errMsg);
		if (errMsg)
			sqlite3_free(errMsg);

	}
	if (pazResult)
		sqlite3_free_table(pazResult);
	return re;
}

int MSqlite::Update(const std::string& sqlUpdate)
{
	char* errMsg = nullptr;
	int re = sqlite3_exec(m_db, sqlUpdate.c_str(), NULL, NULL, &errMsg);
	if (re != SQLITE_OK) {
		LOG(ERRO, "Update error %s", errMsg);
		if (errMsg)
			sqlite3_free(errMsg);
	}

	return re;
}

int MSqlite::QueryData(const std::string& sqlQuery, std::vector<std::string>& arrKey, std::vector<std::vector<std::string>>& arrValue)
{
	int re = 0;
	if (sqlQuery.empty())
		return -1;

	int nCols = -1;
	int nRows = -1;
	char** pazResult = nullptr;
	char* errMsg = nullptr;
	re = sqlite3_get_table(m_db, sqlQuery.c_str(), &pazResult, &nRows, &nCols, &errMsg);

	if (re != SQLITE_OK) {
		return -1;
		LOG(ERRO, "get_table_failed:%s", errMsg);
		if (errMsg)
			sqlite3_free(errMsg);
	}

	int index = nCols;
	arrKey.clear();
	arrKey.resize(nCols);
	arrValue.clear();
	arrValue.reserve(nRows);

	bool bKeyCaptured = false;
	for (int i = 0; i < nRows; i++) {
		std::vector<std::string> temp;
		for (int j = 0; j < nCols; j++) {
			if (!bKeyCaptured) {
				arrKey.push_back(pazResult[j]);
			}
			temp.push_back(pazResult[index]);
			index++;
		}
		bKeyCaptured = true;
		arrValue.push_back(temp);
	}

	if (errMsg)
		sqlite3_free(errMsg);
	if (pazResult)
		sqlite3_free_table(pazResult);

	return re;
}

void MSqlite::Destroy()
{
	if (m_db) {
		sqlite3_close(m_db);
		m_db = nullptr;
	}
}