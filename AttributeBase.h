#pragma once

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
	// AttributeBase��(��) ���� ��ӵ�
	virtual void SetAttribute() override;
};