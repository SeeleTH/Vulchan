#pragma once

#define WFHALT() __debugbreak()
#define WFASSERT_UNUSED(cond, msg) do{(void)sizeof(cond);}while(0)

#ifdef _DEBUG
#define WFASSERT_MSG(cond, msg) \
do \
{ \
	if (!(cond)) \
	{ \
		if(wfAssert::ReportFailure(#cond, #msg, __FILE__, __LINE__) == wfAssert::HALT) \
		{ \
			WFHALT(); \
		} \
	} \
} while(0)
#define WFASSERT(cond) WFASSERT_MSG(cond,)
#else
#define WFASSERT_MSG(cond, msg) do{WFASSERT_UNUSED(cond, msg);}while(0)
#define WFASSERT(cond) WFASSERT_MSG(cond,)
#endif

namespace wfAssert
{
	enum FailBehavior
	{
		HALT,
		CONTINUE,
	};


	typedef FailBehavior(*Handler)(const char* condition,
		const char* msg,
		const char* file,
		const int line);

	Handler GetHandler();
	void SetHandler(Handler newHandler);

	FailBehavior ReportFailure(const char* condition,
		const char* msg,
		const char* file,
		const int line);
}