#include "SandboxLayer.h"
#include "stb_image/stb_image.h"

#include <iostream>
#include <string>

using namespace GLCore;
using namespace GLCore::Utils;

void LoadTexture(const char* name, unsigned int & id)
{
	stbi_set_flip_vertically_on_load(1);
	int width, height, bpp;
	unsigned char* pixels;

	pixels = stbi_load(name, &width, &height, &bpp, 4);

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	glBindTexture(GL_TEXTURE_2D, 0);
	if (pixels)
	{
		stbi_image_free(pixels);
	}
}

SandboxLayer::SandboxLayer()
	: m_CameraController(16.0f / 9.0f), m_Cyberpunk(0), m_PosQ1{-1.5f, -0.5f}, m_PosQ2{ 0.5f, -0.5f }, m_NoQuads(1)
{
	
}

SandboxLayer::~SandboxLayer()
{
}

static Quad CreateQuad(const float& x, const float& y, const float& size, const float& texID)
{
	Vertex v0;
	v0.Position = { x, y, 0.0f };
	v0.Color = { 0.7f, 0.3f, 0.1f, 1.0f };
	v0.TexCoords = { 0.0f, 0.0f };
	v0.TexID = texID;

	Vertex v1;
	v1.Position = { x + size, y, 0.0f };
	v1.Color = { 0.7f, 0.3f, 0.1f, 1.0f };
	v1.TexCoords = { 1.0f, 0.0f };
	v1.TexID = texID;

	Vertex v2;
	v2.Position = { x + size,  y + size, 0.0f };
	v2.Color = { 0.7f, 0.3f, 0.1f, 1.0f };
	v2.TexCoords = { 1.0f, 1.0f };
	v2.TexID = texID;

	Vertex v3;
	v3.Position = {x,  y + size, 0.0f };
	v3.Color = { 0.7f, 0.3f, 0.1f, 1.0f };
	v3.TexCoords = { 0.0f, 1.0f };
	v3.TexID = texID;

	Quad outp = { v0, v1, v2, v3 };

	return outp;
}

static IndexBuffer CreateQuadIB(const unsigned int& noIndex)
{
	unsigned offset = 4;
	unsigned shift = offset * noIndex;
	IndexBuffer i0;
	i0.QuadIndices = { 0 + shift, 1 + shift, 2 + shift, 2 + shift, 3 + shift, 0 + shift};

	
	return i0;
}

void SandboxLayer::OnAttach()
{
	EnableGLDebugging();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_Shader = Shader::FromGLSLTextFiles(
		"assets/shaders/test.vert.glsl",
		"assets/shaders/test.frag.glsl"
	);

	glCreateVertexArrays(1, &m_QuadVA);
	glBindVertexArray(m_QuadVA);

	glUseProgram(m_Shader->GetRendererID());

	int samplers[2] = { 0, 1 };
	auto loc = glGetUniformLocation(m_Shader->GetRendererID(), "u_Textures");
	glUniform1iv(loc, 2, samplers);

	glCreateBuffers(1, &m_QuadVB);
	glBindBuffer(GL_ARRAY_BUFFER, m_QuadVB);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 1000, nullptr, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, Position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, Color));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, TexCoords));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, TexID));

	LoadTexture("assets/textures/cyberpunk_tex.png", m_Cyberpunk);
	LoadTexture("assets/textures/tawm.png", m_Tom);

	glCreateBuffers(1, &m_QuadIB);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_QuadIB);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(IndexBuffer) * 1000, nullptr, GL_DYNAMIC_DRAW);

}

void SandboxLayer::OnDetach()
{
	glDeleteVertexArrays(1, &m_QuadVA);
	glDeleteBuffers(1, &m_QuadVB);
	glDeleteBuffers(1, &m_QuadIB);
}

void SandboxLayer::OnEvent(Event& event)
{
	// zoom
	m_CameraController.OnEvent(event);
}

void SandboxLayer::OnUpdate(Timestep ts)
{
	m_CameraController.OnUpdate(ts);

	Quad* quad = new Quad[m_NoQuads];
	IndexBuffer* ibuf = new IndexBuffer[m_NoQuads];

	for (int i = 0; i < m_NoQuads; i++)
	{
		Quad q = CreateQuad(m_PosQ1.x + 1.2f * i, m_PosQ1.y, 1.0f, 0.0f);
		IndexBuffer ind = CreateQuadIB(i);

		quad[i] = q;
		ibuf[i] = ind;
	}

	glBindBuffer(GL_ARRAY_BUFFER, m_QuadVB);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_NoQuads * sizeof(Quad), quad);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_QuadIB);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, m_NoQuads * sizeof(IndexBuffer), ibuf);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_Shader->GetRendererID());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_Cyberpunk);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_Tom);

	int location = glGetUniformLocation(m_Shader->GetRendererID(), "u_ViewProjection");
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(m_CameraController.GetCamera().GetViewProjectionMatrix()));

	location = glGetUniformLocation(m_Shader->GetRendererID(), "u_Transform");
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f))));

	location = glGetUniformLocation(m_Shader->GetRendererID(), "u_Color");
	glUniform4fv(location, 1, glm::value_ptr(m_SquareBaseColor));

	glBindVertexArray(m_QuadVA);
	glDrawElements(GL_TRIANGLES, 6 * m_NoQuads, GL_UNSIGNED_INT, nullptr);

	delete[] quad;
	delete[] ibuf;
}

void SandboxLayer::OnImGuiRender()
{
	// ImGui here
	static bool numQuads = true;
	ImGui::Begin("Controls");
	ImGui::DragFloat2("Position CP", &m_PosQ1.x, 0.01f);
	ImGui::DragFloat2("Position Tom", &m_PosQ2.x, 0.01f);
	ImGui::SliderInt("Number of Quads", (int*)&m_NoQuads, 0, 10);
	ImGui::End();
}
