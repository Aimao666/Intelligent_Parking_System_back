#include "CBaseOperation.h"

CBaseOperation::CBaseOperation()
{
	this->connection = DBConnection::getInstance();
	this->conn = connection->getConnection();
}

CBaseOperation::~CBaseOperation()
{
}


string CBaseOperation::getTablename() const
{
	return tablename;
}
