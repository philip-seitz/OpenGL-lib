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
	: m_CameraController(16.0f / 9.0f), m_Cyberpunk(0)
{
	
}

SandboxLayer::~SandboxLayer()
{
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

	float vertices[] = {
		-1.5f, -0.5f, 0.0f,		0.7f, 0.3f, 0.1f, 1.0f,		0.0f, 0.0f,		0.0f, 
		 -0.5f, -0.5f, 0.0f,	0.7f, 0.3f, 0.1f, 1.0f,		1.0f, 0.0f,		0.0f,
		 -0.5f,  0.5f, 0.0f,	0.7f, 0.3f, 0.1f, 1.0f,		1.0f, 1.0f,		0.0f,
		-1.5f,  0.5f, 0.0f,		0.7f, 0.3f, 0.1f, 1.0f,		0.0f, 1.0f,		0.0f,

		 0.5f, -0.5f, 0.0f,		0.1f, 0.3f, 0.6f, 1.0f,		0.0f, 0.0f,		1.0f,
		 1.5f, -0.5f, 0.0f,		0.1f, 0.3f, 0.6f, 1.0f,		1.0f, 0.0f,		1.0f,
		 1.5f,  0.5f, 0.0f,		0.1f, 0.3f, 0.6f, 1.0f,		1.0f, 1.0f,		1.0f,
		 0.5f,  0.5f, 0.0f,		0.1f, 0.3f, 0.6f, 1.0f,		0.0f, 1.0f,		1.0f
	};

	glCreateBuffers(1, &m_QuadVB);
	glBindBuffer(GL_ARRAY_BUFFER, m_QuadVB);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 10, 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 10, (const void*)12);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 10, (const void*)28);

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 10, (const void*)36);

	LoadTexture("assets/textures/cyberpunk_tex.png", m_Cyberpunk);
	LoadTexture("assets/textures/tawm.png", m_Tom);

	uint32_t indices[] =
	{ 
		0, 1, 2, 2, 3, 0,
		4, 5, 6, 6, 7, 4
	};
	glCreateBuffers(1, &m_QuadIB);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_QuadIB);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

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
	glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr);
}

void SandboxLayer::OnImGuiRender()
{
	// ImGui here
}
