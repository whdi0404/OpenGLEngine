#pragma once

//그냥 넣을 수 있도록 할 수 있을듯.
class AttributeBase
{
public:
	void Begin(GLuint vertexBuffer);
	void End();

	virtual void SetAttribute() = 0;
};

template<typename Class>
class AttributeSingleton : public Singleton<Class>, public AttributeBase
{
};

class VertexAttrib : public AttributeSingleton<VertexAttrib>
{
	// AttributeBase을(를) 통해 상속됨
	virtual void SetAttribute() override;
};

