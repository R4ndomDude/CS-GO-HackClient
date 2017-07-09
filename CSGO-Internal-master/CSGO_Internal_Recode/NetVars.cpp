#include "NetVars.h"

CNetVars::CNetVars()
{
	m_tables.clear();

	ClientClass* clientClass = Interfaces.Client->GetAllClasses();
	if(!clientClass)
		return;

	while(clientClass)
	{
		RecvTable* recvTable = clientClass->rtTable;

		m_tables.push_back(recvTable);

		clientClass = clientClass->pNextClass;
	}
}

CNetVars::~CNetVars()
{
	m_tables.clear();
}

void CNetVars::GetOffset(const char *tableName, const char *propName, unsigned long* pOffset)
{
	int offset = GetProp(tableName, propName);
	if(offset)
		*pOffset = offset;
}

RecvVarProxyFn CNetVars::GetProxyFunction(const char *tableName, const char *propName)
{
	RecvProp* recvProp = nullptr;
	GetProp(tableName, propName, &recvProp);
	if(!recvProp)
		return nullptr;

	return recvProp->GetProxyFn();
}

void CNetVars::HookProp(const char *tableName, const char *propName, RecvVarProxyFn function)
{
	RecvProp* recvProp = nullptr;
	GetProp(tableName, propName, &recvProp);
	if(!recvProp)
		return;

	recvProp->SetProxyFn(function); //recvProp->m_ProxyFn = function;
}

int CNetVars::GetProp(const char *tableName, const char *propName, RecvProp **prop)
{
	RecvTable* recvTable = GetTable(tableName);
	if(!recvTable)
		return 0;

	int offset = GetProp(recvTable, propName, prop);
	if(!offset)
		return 0;

	return offset;
}

int CNetVars::GetProp(RecvTable *recvTable, const char *propName, RecvProp **prop)
{
	int extraOffset = 0;

	for(int i = 0; i < recvTable->m_nProps; ++i)
	{
		RecvProp* recvProp = &recvTable->m_pProps[i];
		RecvTable* child = recvProp->GetDataTable();

		if(child && (child->m_nProps > 0))
		{
			int tmp = GetProp(child, propName, prop);

			if(tmp)
				extraOffset += (recvProp->GetOffset() + tmp);
		}

		if(_stricmp(recvProp->m_pVarName, propName))
			continue;

		if(prop)
			*prop = recvProp;

		return (recvProp->GetOffset() + extraOffset);
	}

	return extraOffset;
}

RecvTable* CNetVars::GetTable(const char *tableName)
{
	if(m_tables.empty())
		return 0;

	for each (RecvTable* table in m_tables)
	{
		if(!table)
			continue;

		if(_stricmp(table->m_pNetTableName, tableName) == 0)
			return table;
	}

	return 0;
}