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
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp> 
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <IL/il.h>

#include <fbxsdk.h>

#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>

#include <iostream>
#include "SingletonBase.h"

#include "MyGLUtil.h"
#include "math.h"
#include "Renderer.h"

//#pragma comment( lib, "winmm.lib" )
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glfw3dll.lib")
#pragma comment(lib, "libfbxsdk.lib")
#pragma comment(lib, "DevIL.lib")

extern Renderer* g_Renderer;
extern GLFWwindow* g_Window;
extern FbxManager* g_FbxSdkManager;