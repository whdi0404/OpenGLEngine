#pragma once

class Mesh;
class Object;
namespace FBXHelper
{
	Delegate(Object*, FBXCreateAction, (FbxNode* node), (node));
	std::vector<Object*> GetResourcesFromFile(std::string filePath, FBXCreateAction createAction);
}