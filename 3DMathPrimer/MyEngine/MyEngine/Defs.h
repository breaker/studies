// Defs.h
//

#pragma once

#define MYENGINE_NS_BEGIN   namespace MyEngine {
#define MYENGINE_NS_END     }

#ifdef MYENGINE_EXPORTS
#define MYENGINE_API __declspec(dllexport)
#else
#define MYENGINE_API __declspec(dllimport)
#endif
