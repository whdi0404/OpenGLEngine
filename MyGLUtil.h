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

GLuint CreateShader(const char* file_path, GLenum shaderType);

GLuint AttachShaderToProgram(GLuint programID, GLuint count, ...);

GLuint LoadDefaultShaders(const char * vertex_file_path, const char * fragment_file_path, const char * tessellation_control_file_path = nullptr, const char * tessellation_evaluate_file_path = nullptr);