#pragma once

#ifdef DLL1_EXPORTS
#define MYDLL extern "C" __declspec(dllexport)
#else
#define MYDLL extern "C" __declspec(dllimport)
#endif

MYDLL int Mul(int a, int b);