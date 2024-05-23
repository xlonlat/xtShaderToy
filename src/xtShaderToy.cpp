#include "xtShaderToy.h"
#include <iostream>
#include <regex>

namespace xlonlat
{
	xtShaderToy::xtShaderToy()
	{
		m_uniforms = { 0 };
	}

	xtShaderToy::~xtShaderToy()
	{
		if (m_shader > 0)
			glDeleteProgram(m_shader);
		m_shader = 0;
	}

	void xtShaderToy::Compile(const char* code)
	{
		std::string vs =
#pragma region
			"	#version 130										\n"
			"														\n"
			"	out vec2 vCanvasCoord;								\n"
			"														\n"
			"	void main()											\n"
			"	{													\n"
			"		vec4 verteics[6];								\n"
			"		verteics[0] = vec4(-1.0, 1.0, 0.0, 1.0);		\n"
			"		verteics[1] = vec4(-1.0, -1.0, 0.0, 1.0);		\n"
			"		verteics[2] = vec4(1.0,  1.0, 0.0, 1.0);		\n"
			"		verteics[3] = vec4(1.0,  1.0, 0.0, 1.0);		\n"
			"		verteics[4] = vec4(-1.0, -1.0, 0.0, 1.0);		\n"
			"		verteics[5] = vec4(1.0, -1.0, 0.0, 1.0)	;		\n"
			"														\n"
			"		gl_Position = verteics[gl_VertexID];			\n"
			"														\n"
			"		vCanvasCoord.x = (gl_Position.x + 1.0) / 2;		\n"
			"		vCanvasCoord.y = (gl_Position.y + 1.0) / 2;		\n"
			"	}													\n"
			"														\n";
#pragma endregion 

		std::string head =
#pragma region
			"	#version 130																						\n"
			"																										\n"
			"	in vec2				vCanvasCoord;																	\n"
			"																										\n"
			"	out vec4 fragColor;																					\n"
			"																										\n"
			"	uniform vec3		iResolution;			// viewport resolution (in pixels)						\n"
			"	uniform float		iTime;					// shader playback time (in seconds)					\n"
			"	uniform float		iTimeDelta;				// render time (in seconds)								\n"
			"	uniform int			iFrame;					// shader playback frame								\n"
			"	uniform float		iChannelTime[4];		// channel playback time (in seconds)					\n"
			"	uniform vec3		iChannelResolution[4];	// channel resolution (in pixels)						\n"
			"	uniform vec4		iMouse;					// mouse pixel coords. xy: current (if MLB down), zw: click		\n"
			"	uniform vec4		iDate;                  // (year, month, day, time in seconds)					\n"
			"	uniform float		iSampleRate;			// sound sample rate (i.e., 44100)						\n"
			"	uniform sampler2D	iChannel0;				// input channel. XX = 2D/Cube							\n"
			"	uniform sampler2D	iChannel1;				// input channel. XX = 2D/Cube							\n"
			"	uniform sampler2D	iChannel2;				// input channel. XX = 2D/Cube							\n"
			"	uniform sampler2D	iChannel3;				// input channel. XX = 2D/Cube							\n"
			"																										\n"
			"	vec2 fragCoord = vCanvasCoord * iResolution.xy;														\n"
			"																										\n";
#pragma endregion 

		std::string combine = head + code;
		std::regex pattern("mainImage(.*?)\\)");
		std::string fs = std::regex_replace(combine, pattern, "main()");

		Load(vs.c_str(), fs.c_str());
	}

	void xtShaderToy::Render(const xtDrawArgs& args)
	{
		if (m_shader == 0)
			return;

		{
			m_uniforms.iResolution[0] = (float)args.width;
			m_uniforms.iResolution[1] = (float)args.height;
			m_uniforms.iResolution[2] = 1.f;
			m_uniforms.iTime += (float)args.delta;
			m_uniforms.iTimeDelta = (float)args.delta;
			m_uniforms.iFrame++;
			m_uniforms.iMouse[0] = (float)args.mouse[0];
			m_uniforms.iMouse[1] = (float)args.mouse[1];
			m_uniforms.iMouse[2] = (float)args.mouse[2];
			m_uniforms.iMouse[3] = 1.f;
		}

		glUseProgram(m_shader);
		{
			GLint iResolutionLoc = glGetUniformLocation(m_shader, "iResolution");
			GLint iTimeLoc = glGetUniformLocation(m_shader, "iTime");
			GLint iTimeDeltaLoc = glGetUniformLocation(m_shader, "iTimeDelta");
			GLint iFrameLoc = glGetUniformLocation(m_shader, "iFrame");
			GLint iMouseLoc = glGetUniformLocation(m_shader, "iMouse");

			glUniform3f(iResolutionLoc, m_uniforms.iResolution[0], m_uniforms.iResolution[1], m_uniforms.iResolution[2]);
			glUniform1f(iTimeLoc, m_uniforms.iTime);
			glUniform1f(iTimeDeltaLoc, m_uniforms.iTimeDelta);
			glUniform1i(iFrameLoc, m_uniforms.iFrame);
			glUniform4f(iMouseLoc, m_uniforms.iMouse[0], m_uniforms.iMouse[1], m_uniforms.iMouse[2], m_uniforms.iMouse[3]);
		}
		glFrontFace(GL_CCW);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glUseProgram(0);
	}

	bool xtShaderToy::Load(const char* vs, const char* fs)
	{
		char error_info[4096] = { 0 };
		GLuint vert_shader = 0;
		GLuint frag_shader = 0;
		GLint result = GL_FALSE;

		do 
		{
			if (m_shader > 0)
				glDeleteProgram(m_shader);
			m_shader = 0;

			if (vs == nullptr || fs == nullptr)
				break;

			vert_shader = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vert_shader, 1, &vs, nullptr);
			glCompileShader(vert_shader);

			glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &result);
			if (result == GL_FALSE)
			{
				glGetShaderInfoLog(vert_shader, sizeof(error_info), nullptr, error_info);
				std::cout << error_info << std::endl;
				break;
			}

			frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(frag_shader, 1, &fs, nullptr);
			glCompileShader(frag_shader);

			glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &result);
			if (result == GL_FALSE)
			{
				glGetShaderInfoLog(frag_shader, sizeof(error_info), nullptr, error_info);
				std::cout << error_info << std::endl;
				break;
			}

			m_shader = glCreateProgram();
			glAttachShader(m_shader, vert_shader);
			glAttachShader(m_shader, frag_shader);
			glLinkProgram(m_shader);

			glGetProgramiv(m_shader, GL_LINK_STATUS, &result);
			if (result == GL_FALSE)
			{
				glGetProgramInfoLog(m_shader, sizeof(error_info), nullptr, error_info);
				std::cout << error_info << std::endl;
				break;
			}

			std::cout << "Compile success" << std::endl;

		} while (0);

		if (vert_shader > 0) glDeleteShader(vert_shader);
		if (frag_shader > 0) glDeleteShader(frag_shader);

		std::cout << "----------------------------------------" << std::endl;

		return result;
	}
}