#pragma once

#ifdef ENGINE_EXPORTS
#define ENGINE_MODULE
#define ENGINE_API __declspec(dllexport)
#define ENGINE_API_EXTERN extern __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllimport)
#define ENGINE_API_EXTERN extern __declspec(dllimport)
#endif
