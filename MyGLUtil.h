#pragma once

#define Get(type, fnName, varName)\
private: \
	type varName; \
public: \
	type& Get##fnName()\
	{\
		return varName;\
	}

#define Set(type, fnName, varName)\
private: \
	type varName; \
public: \
	void Set##fnName(type value)\
	{\
		varName = value;\
	}

#define GetSet(type, fnName, varName)\
Get(type, fnName, varName)\
	void Set##fnName(type value)\
	{\
		varName = value;\
	}

#define Get_NonRef(type, fnName, varName)\
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
GLuint CreateShader(const char* file_path, GLenum shaderType);

GLuint AttachShaderToProgram(GLuint programID, GLuint count, ...);

GLuint LoadDefaultShaders(const char * vertex_file_path, const char * fragment_file_path, const char * tessellation_control_file_path = nullptr, const char * tessellation_evaluate_file_path = nullptr);