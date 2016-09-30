#pragma once

#ifndef OS_WIN32
#define OS_WIN32
#endif

#if defined(_DEBUG)
#define WFNEW  new//new(_NORMAL_BLOCK,__FILE__, __LINE__)
#else
#define WFNEW  new
#endif

#if !defined(WFDEL)
#define WFDEL(x) if(x) delete x; x=NULL;
#endif

#if !defined(WFDEL_SIZE)
#define WFDEL_SIZE(x,y) if(x) delete(x,y); x=NULL;
#endif

#if !defined(WFDEL_ARRAY)
#define WFDEL_ARRAY(x) if (x) delete [] x; x=NULL; 
#endif

#if !defined(WFREL)
#define WFREL(x) if(x) x->Release(); x=NULL;
#endif

#if !defined(WFDEB_COUT)
#if _DEBUG
#define WFDEB_COUT(MSG) std::cout << "[" << __FILE__ <<":" << __LINE__ << "] " << std::endl << MSG << std::endl;
#else
#define WFDEB_COUT(MSG)
#endif
#endif

typedef unsigned __int32 uint32;
typedef __int32 int32;