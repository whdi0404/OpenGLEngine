#pragma once
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <algorithm>
#include <stdlib.h>
#include <time.h>
//#include <cmath.h>
#include <functional>
//#include <MMSystem.h>
#include <Windows.h>

#define GLFW_INCLUDE_GLU
#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS
#define KEYVALUE(keyType, valueType) std::unordered_map<keyType, valueType>::value_type
#define KFBX_DLLINFO
#define FBXSDK_NEW_API
#define FBXSDK_SHARED
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/ext.hpp>
#include <IL/il.h>

#include <fbxsdk.h>

#include <PxPhysicsAPI.h>
#include <PxPhysXConfig.h>
#include <foundation/PxMath.h>
#include <foundation/PxMathUtils.h>
#include <extensions/PxExtensionsAPI.h>
#include <foundation/PxAssert.h>
#include <foundation/PxMemory.h>
#include <foundation/PxTransform.h>
#include <foundation/PxFoundationVersion.h>
#include <cudamanager/PxCudaContextManager.h>

#include <vector>
#include <map>
#include <queue>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <filesystem>

#include <stdlib.h>

#include "SingletonBase.h"
#include "MyGLUtil.h"
#include "Delegate.h"
#include "FBXHelper.h"
#include "math.h"
#include "Renderer.h"
#include "VertexAttributeDeclarative.h"
#include "PhysXManager.h"

template <class Output, class Input>
inline Output horrible_cast(const Input input) {
	union { Output output; Input input; }u;

	typedef int ERROR_CantHorrible_cast[sizeof(Input) == sizeof(u) &&
		sizeof(Input) == sizeof(Output) ? 1 : -1];

	u.input = input;
	return u.output;
}
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glfw3dll.lib")
#pragma comment(lib, "libfbxsdk.lib")
#pragma comment(lib, "DevIL.lib")

using namespace std::experimental;
using namespace std::experimental::filesystem;

extern GLFWwindow* g_Window;
extern Renderer* g_Renderer;
extern PhysXManager* g_PhysXManager;
extern FbxManager* g_FbxSdkManager;

#define STARTSCENE_CLASSNAME BoneScene