#include "mylib.h"  
#include "IniFile.h"  

IniFile::IniFile(std::string fileName)
{
	if (fileName.empty() || fileName.find('\\') == std::string::npos) {

		CHAR modName[MAX_PATH + 1];
		DWORD n = GetModuleFileNameA(NULL, modName, sizeof(modName));
		CHAR* c = modName + n - 1;
		while (c >= modName) {
			if (*c == _T('\\')) {
				*c = 0;
				break;
			}

			c --;
		}

		if (fileName.empty()) {
			fileName = modName;
			fileName += "\\config.ini";		
		} else {

			fileName = modName + fileName;
		}
	}

	_iniFile = fileName;
}

IniFile::~IniFile()
{

}

//////////////////////////////////////////////////////////////////////////
long IniFile::Section::getLong(std::string name, long def /* = 0 */)
{
	return GetPrivateProfileIntA(_sectionName.c_str(), name.c_str(), def, _fileName->c_str());
}

std::string IniFile::Section::getString(std::string name, std::string def /* = 0 */)
{
	char val[512];
	GetPrivateProfileStringA(_sectionName.c_str(), name.c_str(), def.c_str(), val, 
		sizeof(val) - 1, _fileName->c_str());
	return val;
}

double IniFile::Section::getDouble(std::string name, double def /* = 0.0l */)
{
	char val[128];
	char defVal[128];
	sprintf(defVal, "%lf", def);
	GetPrivateProfileStringA(_sectionName.c_str(), name.c_str(), defVal, val, sizeof(val) - 1, 
		_fileName->c_str());
	return atof(val);	
}

bool IniFile::Section::setLong(std::string name, long val)
{
	char valStr[128];
	sprintf(valStr, "%ld", val);
	return WritePrivateProfileStringA(_sectionName.c_str(), name.c_str(), valStr, _fileName->c_str()) == TRUE;
}

bool IniFile::Section::setString(std::string name, std::string val)
{
	return WritePrivateProfileStringA(_sectionName.c_str(), name.c_str(), val.c_str(), _fileName->c_str()) == TRUE;
}

bool IniFile::Section::setDouble(std::string name, double val)
{
	char valStr[128];
	sprintf(valStr, "%lf", val);
	return WritePrivateProfileStringA(_sectionName.c_str(), name.c_str(), valStr, _fileName->c_str()) == TRUE;
}

#if 0
void IniFileUnitTest()
{
	IniFile ini;
	ini["System"].setLong("LogLevel", 1);
	long l = ini["System"].getLong("LogLevel");
}

#endif
