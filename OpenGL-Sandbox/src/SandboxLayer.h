#pragma once

#include <GLCore.h>
#include <GLCoreUtils.h>

struct Vec2
{
	float x, y;

};

struct Vec3
{
	float x, y, z;

};

struct Vec4
{
	float x, y, z, w;

};

struct Vec6
{
	unsigned int a, b, c, d, e, f;

};

struct Vertex
{
	Vec3 Position;
	Vec4 Color;
	Vec2 TexCoords;
	float TexID;
};

struct Quad
{
	Vertex v0, v1, v2, v3;
};

struct IndexBuffer
{
	Vec6 QuadIndices;

};

struct QuadIndex
{
	IndexBuffer i0, i1;

};

class SandboxLayer : public GLCore::Layer
{
public:
	SandboxLayer();
	virtual ~SandboxLayer();

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnEvent(GLCore::Event& event) override;
	virtual void OnUpdate(GLCore::Timestep ts) override;
	virtual void OnImGuiRender() override;
private:

	GLCore::Utils::Shader* m_Shader;
	GLCore::Utils::OrthographicCameraController m_CameraController;

	GLuint m_QuadVA, m_QuadVB, m_QuadIB, m_Cyberpunk, m_Tom, m_NoQuads;

	Vec2 m_PosQ1, m_PosQ2;

	glm::vec4 m_SquareBaseColor = { 0.8f, 0.2f, 0.3f, 1.0f };
	
};