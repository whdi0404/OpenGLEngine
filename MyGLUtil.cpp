#include "stdafx.h"
#include "MyGLUtil.h"

#if _WIN32 || _WIN64
#if _WIN64
#define ENVIRONMENT64
#else
#define ENVIRONMENT32
#endif
#endif


glm::mat4x4 GetGLMMatrixFromFBXMatrix(FbxAMatrix fbxMatrix)
{
	glm::mat4x4 mat;
	auto double44 = fbxMatrix.Double44();
	for (int col = 0; col < 4; ++col)
	{
		for (int row = 0; row < 4; ++row)
		{
			float single = static_cast<float>(double44[col][row]);
			mat[col][row] = single;
		}
	}
	return mat;
}

fbxsdk::FbxAMatrix GetFBXMatrixFromGLMMatrix(glm::mat4x4 glMatrix)
{
	fbxsdk::FbxAMatrix fbxMatrix;
	fbxMatrix.SetRow(0, FbxVector4(glMatrix[0][0], glMatrix[0][1], glMatrix[0][2], glMatrix[0][3]));
	fbxMatrix.SetRow(1, FbxVector4(glMatrix[1][0], glMatrix[1][1], glMatrix[1][2], glMatrix[1][3]));
	fbxMatrix.SetRow(2, FbxVector4(glMatrix[2][0], glMatrix[2][1], glMatrix[2][2], glMatrix[2][3]));
	fbxMatrix.SetRow(3, FbxVector4(glMatrix[3][0], glMatrix[3][1], glMatrix[3][2], glMatrix[3][3]));

	//fbxMatrix.SetRow(0, FbxVector4(glMatrix[0][0], glMatrix[1][0], glMatrix[2][0], glMatrix[3][0]));
	//fbxMatrix.SetRow(1, FbxVector4(glMatrix[0][1], glMatrix[1][1], glMatrix[2][1], glMatrix[3][1]));
	//fbxMatrix.SetRow(2, FbxVector4(glMatrix[0][2], glMatrix[1][2], glMatrix[2][2], glMatrix[3][2]));
	//fbxMatrix.SetRow(3, FbxVector4(glMatrix[0][3], glMatrix[1][3], glMatrix[2][3], glMatrix[3][3]));

	return fbxMatrix;
}

GLuint CreateShader(const char* file_path, GLenum shaderType)
{
	GLuint shaderID = glCreateShader(shaderType);

	std::string shaderCode;
	std::ifstream VertexShaderStream(file_path, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::string Line = "";
		while (getline(VertexShaderStream, Line))
			shaderCode += "\n" + Line;
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ?!\n", file_path);
		getchar();
		return 0;
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", file_path);
	char const * VertexSourcePointer = shaderCode.c_str();
	glShaderSource(shaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(shaderID);

	// Check Vertex Shader
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(shaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	return shaderID;
}

GLuint AttachShaderToProgram(GLuint programID, GLuint count, ...)
{
	va_list args;
	va_start(args, count);
	for (int i = 0; i < count; ++i)
	{
		GLuint shaderID= va_arg(args, GLuint);
		glAttachShader(programID, shaderID);
	}
	va_end(args);

	glLinkProgram(programID);

	GLint Result = GL_FALSE;
	int InfoLogLength;
	// Check the program
	glGetProgramiv(programID, GL_LINK_STATUS, &Result);
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(programID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}
	va_start(args, count);
	for (int i = 0; i < count; ++i)
	{
		GLuint shaderID = va_arg(args, GLuint);
		glDetachShader(programID, shaderID);
		glDeleteShader(shaderID);
	}
	va_end(args);

	return programID;
}

GLuint LoadDefaultShaders(const char * vertex_file_path, const char * fragment_file_path, const char * tessellation_control_file_path, const char * tessellation_evaluate_file_path) {

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	
	GLuint TessCtrlShaderID = -1;
	if (tessellation_control_file_path != nullptr)
		TessCtrlShaderID = glCreateShader(GL_TESS_CONTROL_SHADER);
	GLuint TessEvelShaderID = -1;
	if (tessellation_evaluate_file_path != nullptr)
		TessEvelShaderID = glCreateShader(GL_TESS_EVALUATION_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::string Line = "";
		while (getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if (FragmentShaderStream.is_open()) {
		std::string Line = "";
		while (getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}

	
	std::string TessCtrlShaderCode;
	if (TessCtrlShaderID != -1)
	{
		std::ifstream TessCtrlShaderStream(tessellation_control_file_path, std::ios::in);
		if (TessCtrlShaderStream.is_open()) {
			std::string Line = "";
			while (getline(TessCtrlShaderStream, Line))
				TessCtrlShaderCode += "\n" + Line;
			TessCtrlShaderStream.close();
		}
	}

	
	std::string TessEvelShaderCode;
	if (TessEvelShaderID != -1)
	{
		std::ifstream TessEvelShaderStream(tessellation_evaluate_file_path, std::ios::in);
		if (TessEvelShaderStream.is_open()) {
			std::string Line = "";
			while (getline(TessEvelShaderStream, Line))
				TessEvelShaderCode += "\n" + Line;
			TessEvelShaderStream.close();
		}
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;


	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}



	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	if (TessCtrlShaderID != -1)
	{
		// Compile Fragment Shader
		printf("Compiling shader : %s\n", tessellation_control_file_path);
		char const * TessCtrlSourcePointer = TessCtrlShaderCode.c_str();
		glShaderSource(TessCtrlShaderID, 1, &TessCtrlSourcePointer, NULL);
		glCompileShader(TessCtrlShaderID);

		// Check Fragment Shader
		glGetShaderiv(TessCtrlShaderID, GL_COMPILE_STATUS, &Result);
		glGetShaderiv(TessCtrlShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0) {
			std::vector<char> TessCtrlShaderErrorMessage(InfoLogLength + 1);
			glGetShaderInfoLog(TessCtrlShaderID, InfoLogLength, NULL, &TessCtrlShaderErrorMessage[0]);
			printf("%s\n", &TessCtrlShaderErrorMessage[0]);
		}
	}
	

	if (TessEvelShaderID != -1)
	{
		// Compile Fragment Shader
		printf("Compiling shader : %s\n", tessellation_evaluate_file_path);
		char const * TessEvelSourcePointer = TessEvelShaderCode.c_str();
		glShaderSource(TessEvelShaderID, 1, &TessEvelSourcePointer, NULL);
		glCompileShader(TessEvelShaderID);

		// Check Fragment Shader
		glGetShaderiv(TessEvelShaderID, GL_COMPILE_STATUS, &Result);
		glGetShaderiv(TessEvelShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0) {
			std::vector<char> TessEvelShaderErrorMessage(InfoLogLength + 1);
			glGetShaderInfoLog(TessEvelShaderID, InfoLogLength, NULL, &TessEvelShaderErrorMessage[0]);
			printf("%s\n", &TessEvelShaderErrorMessage[0]);
		}
	}

	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	if (TessCtrlShaderID != -1)
	{
		glAttachShader(ProgramID, TessCtrlShaderID);
	}
	if (TessEvelShaderID != -1)
	{
		glAttachShader(ProgramID, TessEvelShaderID);
	}
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}


	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);
	if (TessCtrlShaderID != -1)
	{
		glDetachShader(ProgramID, TessCtrlShaderID);
	}
	if (TessEvelShaderID != -1)
	{
		glDetachShader(ProgramID, TessEvelShaderID);
	}
	

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);
	if (TessCtrlShaderID != -1)
	{
		glDeleteShader(TessCtrlShaderID);
	}
	if (TessEvelShaderID != -1)
	{
		glDeleteShader(TessEvelShaderID);
	}

	return ProgramID;
}