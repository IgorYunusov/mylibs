#pragma once 

class IniFile {
public: 
	IniFile(std::string fileName = std::string());
	~IniFile();

	class Section {

	public:
		Section()
		{

		}

		Section(std::string* fileName, std::string secionName): _fileName(fileName), 
			_sectionName(secionName)
		{

		}

		long getLong(std::string name, long def = 0);
		std::string getString(std::string name, std::string def = std::string());
		double getDouble(std::string name, double def = 0.0l);

		bool setLong(std::string name, long val);
		bool setString(std::string name, std::string val);
		bool setDouble(std::string name, double val);

	protected:

		std::string*		_fileName;
		std::string			_sectionName;
	};

	Section& operator[](std::string name)
	{
		Sections::iterator it = _sections.find(name);
		if (it == _sections.end()) {
			Sections::_Pairib r;
			r = _sections.insert(Sections::value_type(name, Section(&_iniFile, name)));
			it = r.first;
		}

		return it->second;
	}

protected:

	std::string			_iniFile;

	typedef std::map<std::string, Section> Sections;
	Sections			_sections;
};
