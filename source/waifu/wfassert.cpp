#include "wfassert.h"
#include <iostream>

namespace wfAssert
{
	static Handler curHandler = nullptr;

	Handler GetHandler()
	{
		return curHandler;
	}

	void SetHandler(Handler newHandler)
	{
		curHandler = newHandler;
	}

	FailBehavior ReportFailure(const char* condition,
		const char* msg,
		const char* file,
		const int line)
	{
		if (curHandler)
			return curHandler(condition, msg, file, line);
		std::cout << "[ASSERTION FAILURE: " << file << "(" << line << ")]" << msg << std::endl;
		return HALT;
	}
}