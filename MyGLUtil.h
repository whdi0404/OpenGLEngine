#pragma once

#define GetMacro(type, fnName, varName)\
private: \
	type varName; \
public: \
	type& Get##fnName()\
	{\
		return varName;\
	}

#define GetMacroProtected(type, fnName, varName)\
protected: \
	type varName; \
public: \
	type& Get##fnName()\
	{\
		return varName;\
	}

#define SetMacro(type, fnName, varName)\
private: \
	type varName; \
public: \
	void Set##fnName(type value)\
	{\
		varName = value;\
	}

#define SetMacroProtected(type, fnName, varName)\
protected: \
	type varName; \
public: \
	void Set##fnName(type value)\
	{\
		varName = value;\
	}

#define GetSetMacro(type, fnName, varName)\
GetMacro(type, fnName, varName)\
	void Set##fnName(type value)\
	{\
		varName = value;\
	}

#define GetSetMacroProtected(type, fnName, varName)\
GetMacroProtected(type, fnName, varName)\
	void Set##fnName(type value)\
	{\
		varName = value;\
	}

#define Get_NonRefMacro(type, fnName, varName)\
private: \
	type varName; \
public: \
	type Get##fnName()\
	{\
		return varName;\
	}

#define Execute(instanceName) (instanceName.target.*instanceName.func)
template <class Class, typename Func>
class Delegate
{
public:
	Delegate(Class& trg, Func op)
		: target(trg),
		func(op)
	{
	}
private:
	Delegate();
	Delegate(const Delegate&);

public:
	Class& target;
	const Func func;
};

//Example
/*
Delegate<Component, void(Component::*)()> del(*this, &Component::Update);
Execute(del)();
*/
glm::mat4x4 GetGLMMatrixFromFBXMatrix(FbxAMatrix fbxMatrix);

FbxAMatrix GetFBXMatrixFromGLMMatrix(glm::mat4x4 glMatrix);

physx::PxVec2 GetPxVec2FromGLMVec2(glm::vec2 glVec2);

physx::PxVec3 GetPxVec3FromGLMVec3(glm::vec3 glVec3);

physx::PxVec4 GetPxVec4FromGLMVec4(glm::vec4 glVec4);

glm::vec2 GetGLMVec2FromPxVec2(physx::PxVec2 pxVec2);

glm::vec3 GetGLMVec3FromPxVec3(physx::PxVec3 pxVec3);

glm::vec4 GetGLMVec4FromPxVec4(physx::PxVec4 pxVec4);

GLuint CreateShader(const char* file_path, GLenum shaderType);

GLuint AttachShaderToProgram(GLuint programID, GLuint count, ...);

GLuint LoadDefaultShaders(const char * vertex_file_path, const char * fragment_file_path, const char * tessellation_control_file_path = nullptr, const char * tessellation_evaluate_file_path = nullptr);

template<typename T>

T inverseLerp(T a, T b, T value)
{
	if (b - a == 0)
		return 0;
	return (value - a) / (b - a);
}