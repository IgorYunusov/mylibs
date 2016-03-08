#pragma once 

#include "Any.h"

class Item: public _Any {
public:

	static Item& bad()
	{
		static Item BadItem;
		return BadItem;
	}

	bool isValid() const 
	{
		return this != &bad();
	}

	Item& operator = (const _Any& val)
	{
		*(_Any *)this = val;
		return *this;
	}
};

class Row {
public:
	
	const Item& operator[](const std::string& fieldName) const
	{
		Items::const_iterator it = _items.find(fieldName);
		if (it == _items.end()) {
			assert(false);
			return Item::bad();
		}

		return it->second;
	}

	static Row& bad()
	{
		static Row BadRow;
		return BadRow;
	}

	bool isValid() const 
	{
		return this != &bad();
	}

	bool set(const std::string& fieldName, const _Any& val)
	{
		if (!isValid())
			return false;

		_items[fieldName] = val;
		return true;
	}

protected:
	typedef std::map<std::string, Item> Items;
	Items		_items;
};

class BadIndex
{

};

class Index {

public:
	Row& operator[](const _Any& key);

	typedef std::map<_Any, Row*> IndexMap;

	std::map<_Any, Row*>	_index;
};

class Table {

public:
	Table();

	size_t getRowCount();
	Row& getRow(size_t n);

	Index& operator[](const std::string& field);

	static Table& bad()
	{
		static Table badTable;
		return badTable;
	}

	bool isValid() const
	{
		return this != &bad();
	}

	Row& createRow()
	{
		_rows.push_back(Row());
		return _rows.back();
	}

	bool createIndex(const std::string& fieldName);
	void deleteIndex(const std::string& fieldName)
	{
		_indexes.erase(fieldName);
	}

protected:

	typedef std::vector<Row> Rows;
	Rows			_rows;

	typedef std::map<std::string, Index> Indexes;
	Indexes			_indexes;
};

class TableEng {
public:
	TableEng();

	virtual bool loadTables(const std::string& desc);
	bool loadIndexes(const std::string& tableName, const std::string& field);

	Table& getTable(const std::string& tableName)
	{
		Tables::iterator it = _tables.find(tableName);
		if (it == _tables.end())
			return Table::bad();

		return it->second;

	}

	bool isTable(const std::string& tableName);

	Table& createTable(const std::string& tableName);
	bool deleteTable(const std::string& tableName);

protected:
	
	typedef std::map<std::string, Table> Tables;
	Tables		_tables;
};
