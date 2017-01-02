#include "stdafx.h"
#include "Object.h"


Object::Object()
{
}

Object::Object(std::string name) : name(name)
{
}


Object::~Object()
{
}

std::string Object::ToString()
{
	return typeid(this).name();
}
