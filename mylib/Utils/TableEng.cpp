#include "mylib.h" 
#include "TableEng.h" 

//////////////////////////////////////////////////////////////////////////
Row& Index::operator[](const _Any& key)
{
	std::map<_Any, Row*>::iterator it = _index.find(key);
	if (it == _index.end())
		return Row::bad();

	return *it->second;
}

//////////////////////////////////////////////////////////////////////////

Table::Table()
{

}
size_t Table::getRowCount()
{
	return _rows.size();
}

Row& Table::getRow(size_t n)
{
	if (n >= _rows.size()) {
		assert(false);

		return Row::bad();
	}

	return _rows[n];
}

Index& Table::operator[](const std::string& field)
{
	Indexes::iterator it = _indexes.find(field);
	if (it == _indexes.end())
		throw BadIndex();

	return it->second;
}

bool Table::createIndex(const std::string& fieldName)
{
	if (!isValid())
		return false;

	Rows::iterator it;
	Index& index = _indexes[fieldName];

	for (it = _rows.begin(); it != _rows.end(); it ++) {
		Row& row = *it;
		const Item& item = row[fieldName.c_str()];
		if (!item.isValid())
			return false;

		try {
			Index::IndexMap::_Pairib r = index._index.insert(Index::IndexMap::value_type(item, &row));

			if (!r.second) {
				_indexes.erase(fieldName);
				return false;
			}

		} catch (BadAnyType& ) {
			_indexes.erase(fieldName);
			return false;
		}
	}
	
	return true;
}

//////////////////////////////////////////////////////////////////////////

TableEng::TableEng()
{

}

Table& TableEng::createTable(const std::string& tableName)
{
	return _tables[tableName];
}

bool TableEng::deleteTable(const std::string& tableName)
{
	_tables.erase(tableName);
	return true;
}

bool TableEng::isTable(const std::string& tableName)
{
	return _tables.find(tableName) != _tables.end();
}

bool TableEng::loadIndexes(const std::string& tableName, const std::string& field)
{
	Table& table = getTable(tableName);
	if (!table.isValid()) {
		return false;
	}
	
	return table.createIndex(field);	
}

bool TableEng::loadTables(const std::string& desc)
{		
	/*
	// for testing
	Table& table = createTable("test1");
	Row& row = table.createRow();
	row.set("ID", 1);
	row.set("Name", "Xiao");

	Row& row2 = table.createRow();
	row2.set("ID", 2);
	row2.set("Name", "Li");

	table.createIndex("ID");

	Row& row3 = table["ID"][1];
	std::string name = row3["Name"];
	*/
	return true;
}

#if 0
void TestTableEng()
{
	TableEng tableEng;
	tableEng.loadTables("");

}

#endif
