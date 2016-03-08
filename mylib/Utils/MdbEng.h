#pragma once

#include "TableEng.h"
#import "C:\Program Files\Common Files\System\ado\msado15.dll" no_namespace rename ( "EOF", "adoEOF" )
// #import "C:\Program Files\Common Files\System\ado\msadox.dll"

class MdbEng: public TableEng {
public:
	MdbEng()
	{
		
	}

	virtual bool loadTables(const std::string& desc);
	void loadTable(_ConnectionPtr& conn, const std::string& tableName);

	virtual bool isLoadableTable(const std::string& tableName)
	{
		return true;
	}

	virtual bool isIndexField(const std::string& tableName, const std::string& fieldName)
	{
		return fieldName[0] == 'I';
	}
};
