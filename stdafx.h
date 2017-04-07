#pragma once
#include <iostream>
#include <sstream>

#include <stdio.h>
#include <string>
#include <fstream>
#include <algorithm>
#include <stdlib.h>
#include <time.h>
#include <cmath>
//#include <MMSystem.h>
#include <stdio.h>
#include <Windows.h>

#define GLFW_INCLUDE_GLU
#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS
#define KEYVALUE(keyType, valueType) std::unordered_map<keyType, valueType>::value_type
#define KFBX_DLLINFO
#define FBXSDK_NEW_API
#define FBXSDK_SHARED
#define NDEBUG 1
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

#include <iostream>
#include "SingletonBase.h"

#include "MyGLUtil.h"
#include "Delegate.h"
#include "FBXHelper.h"
#include "math.h"
#include "Renderer.h"
#include "VertexAttributeDeclarative.h"

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

extern Renderer* g_Renderer;
extern GLFWwindow* g_Window;
extern FbxManager* g_FbxSdkManager;

using namespace physx;

#define STARTSCENE_CLASSNAME BoneScene