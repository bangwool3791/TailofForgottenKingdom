#pragma once

#pragma warning(push, 0)
#include <FBXLoader/fbxsdk/scene/geometry/fbxnurbs.h>
#include <FBXLoader/fbxsdk.h>
#include <xstring>
#include <DirectXTex/DirectXTex.h>
#pragma warning(pop)

#pragma warning(push)
#pragma warning( disable : X4717 C26813 C26495)
#pragma warning(pop)

#include <Windows.h>
#include <minwindef.h>
#include <thread>
#include <stack>
#include <mutex>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <string>
#include <array>
#include <iterator>
#include <algorithm>
#include <queue>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cstdint>
#include <process.h>
#include <filesystem>
#include <stdint.h>
#include "precision.h"
#include "tinyxml.h"
#include <sys\stat.h>
#include <sys\types.h>

using std::vector;
using std::list;
using std::map;
using std::make_pair;
using std::back_insert_iterator;
using std::stack;
using std::unordered_map;
using std::pair;
using std::string;
using std::wstring;

using std::array;
using std::queue;

using std::begin;
using std::cbegin;

using std::end;
using std::cend;

using std::cin;
using std::cout;
using std::endl;

namespace fs = std::filesystem;

using std::thread;
#include <typeinfo>

// FileSystem
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
using namespace std::experimental;
using namespace std::experimental::filesystem;

#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgidebug.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <wrl.h>
#include <random>
//#include "ScreenGrab11.h"

#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")
#pragma comment( lib, "dxguid.lib")

using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace Microsoft::WRL;


#include "SimpleMath.h"
using namespace DirectX::SimpleMath;

#include <d3dcommon.h>

// Fbx Loader
#include <FBXLoader/fbxsdk.h>

#ifdef _DEBUG
#pragma comment(lib, "FBXLoader/x64/debug/libfbxsdk-md.lib")
#else
#pragma comment(lib, "FBXLoader/x64/release/libfbxsdk-md.lib")
#endif

#include "singleton.h"
#include "define.h"
#include "struct.h"
#include "func.h"
#include "Base.h"
#include "particle.h"
