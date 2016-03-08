#include "mylib.h"
#include "MdbEng.h"

bool MdbEng::loadTables(const std::string& desc)
{
	_ConnectionPtr conn;
	HRESULT hr = conn.CreateInstance(__uuidof(Connection));
	if (FAILED(hr)) {
		return false;
	}

	try {
		hr = conn->Open(desc.c_str(), _T(""), _T(""), adConnectUnspecified);
		if (FAILED(hr)) {

			// log error
			return false;
		}

	} catch (_com_error& ) {

		// log error
		return false;
	}

	_RecordsetPtr pRstSchema;
	pRstSchema = conn->OpenSchema(adSchemaTables);

	_bstr_t tableType("TABLE");

	while(!(pRstSchema->adoEOF)) {

		_bstr_t table_type = pRstSchema->Fields->GetItem("TABLE_TYPE")->Value;
		if (table_type == tableType) {

			std::string table_name = (_bstr_t )pRstSchema->Fields->GetItem("TABLE_NAME")->Value;
			if (isLoadableTable(table_name)) {
				loadTable(conn, table_name);

				_TRACE("Table: %s loaded\n", table_name.c_str());
			}
		}

		pRstSchema->MoveNext();

	}

	return true;
}

_Any _Any2Var(const _variant_t& v)
{
	_Any any;
	switch (v.vt) {

	case VT_EMPTY:
	case VT_NULL:
		break;

	case VT_BSTR:
		any = (const char *)(_bstr_t)v;
		break;

	case VT_I1:
	case VT_I2:
	case VT_I4:
	case VT_I8:
	case VT_BOOL:
		any = (__int64)v;
		break;

	case VT_R4:
	case VT_R8:
		any = (double )v;
		break;

	default:
		assert(false);
		break;
	}

	return any;
}

void MdbEng::loadTable(_ConnectionPtr& conn, const std::string& tableName)
{
	_variant_t affected;
	_RecordsetPtr rs = conn->Execute(tableName.c_str(), &affected, 0);

	Table& table = createTable(tableName);

	if (rs->adoEOF)
		return;

	long fieldCount = rs->Fields->Count;

	std::vector<std::string> fieldNames;
	fieldNames.resize(fieldCount);

	for (long i = 0; i < fieldCount; i ++) {

		FieldPtr field = rs->Fields->GetItem(i);
		fieldNames[i] = field->GetName();
	}

	while (!rs->adoEOF) {

		Row& row = table.createRow();

		FieldsPtr fields = rs->Fields;		
		for (long i = 0; i < fieldCount; i ++) {

			row.set(fieldNames[i], _Any2Var(fields->GetItem(i)->Value));
		}

		rs->MoveNext();
	}

	for (long i = 0; i < fieldCount; i ++) {

		std::string& fieldName = fieldNames[i];

		if (isIndexField(tableName, fieldName)) {
		
			try {
				if (!table.createIndex(fieldName)) {

					_TRACE("cannot create index! table: %s, field: %s", tableName.c_str(), 
						fieldName.c_str());
				}

			} catch (...) {

				_TRACE("cannot create index! table: %s, field: %s", tableName.c_str(), 
					fieldName.c_str());
			}
		}
	}
}
