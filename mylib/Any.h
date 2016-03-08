#pragma once 

#define _ANY_STRING_SIZE				256

enum _AnyType {
	AnyNone, 
	AnyInt, 
	AnyStr, 
	AnyDouble, 
	AnyPointer, 
};

class BadAnyType {

};

struct _Any {

	_Any()
	{
		type = AnyNone;
	}

	_AnyType getType() const 
	{
		return type;
	}

	_Any(__int64 n)
	{
		type = AnyInt;
		lVal = n;
	}

	_Any(long n)
	{
		type = AnyInt;
		lVal = n;
	}

	_Any(int n)
	{
		type = AnyInt;
		lVal = n;
	}

	_Any(short n)
	{
		type = AnyInt;
		lVal = n;
	}

	_Any(const char* str)
	{
		type = AnyStr;
		strncpy(sVal, str, sizeof(sVal) - 1);
		sVal[sizeof(sVal) - 1] = 0;
	}


	_Any(const std::string& str)
	{
		type = AnyStr;
		strncpy(sVal, str.c_str(), sizeof(sVal) - 1);
		sVal[sizeof(sVal) - 1] = 0;
	}

	_Any(double d)
	{
		type = AnyDouble;
		lfVal = d;
	}

	_Any(float d)
	{
		type = AnyDouble;
		lfVal = d;
	}

	_Any(bool b)
	{
		type = AnyInt;
		lfVal = b ? 1 : 0;
	}

	_Any(void* p) 
	{
		type = AnyPointer;
		ptr = p;
	}

	operator __int64() const
	{
		if (type != AnyInt)
			throw BadAnyType();

		return lVal;
	}

	operator long() const
	{
		if (type != AnyInt)
			throw BadAnyType();

		return (long )lVal;
	}

	operator int() const
	{
		if (type != AnyInt)
			throw BadAnyType();

		return (int )lVal;
	}

	operator short() const
	{
		if (type != AnyInt)
			throw BadAnyType();

		return (short )lVal;
	}

	operator bool() const
	{
		if (type != AnyInt)
			throw BadAnyType();

		return lVal != 0;
	}

	operator const char* () const
	{
		if (type != AnyStr)
			throw BadAnyType();

		return sVal;
	}

	operator std::string() const
	{
		if (type != AnyStr)
			throw BadAnyType();

		return std::string(sVal);
	}

	operator double() const
	{
		if (type != AnyDouble)
			throw BadAnyType();

		return lfVal;
	}

	operator float() const
	{
		if (type != AnyDouble)
			throw BadAnyType();

		return (float )lfVal;
	}

	operator void*() const 
	{
		if (type != AnyPointer)
			throw BadAnyType();

		return ptr;
	}

	bool operator < (const _Any& b) const
	{
		if (type != b.type)
			return false;

		switch (type) {
		case AnyInt:
			return lVal < b.lVal;
			break;

		case AnyDouble:
			return lfVal < b.lfVal;
			break;

		case AnyStr:
			return strcmp(sVal, b.sVal) < 0;
			break;

		case AnyPointer:
			return ptr < b.ptr;
			break;

		default:
			throw BadAnyType();
			break;
		}
	}

	static _Any& bad()
	{
		static _Any v;
		return v;
	}

	bool isValid() const 
	{
		return this != &bad();
	}

	_AnyType		type;
	union {
		__int64		lVal;
		char		sVal[_ANY_STRING_SIZE];
		double		lfVal;
		void*		ptr;
	};
};
