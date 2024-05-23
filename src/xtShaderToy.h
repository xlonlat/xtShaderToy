#ifndef XT_SHADERTOY_HEADER
#define XT_SHADERTOY_HEADER

#include <glad/glad.h>

namespace xlonlat
{
	struct xtDrawArgs
	{
		int		width = 0;
		int		height = 0;
		int		mouse[3] = { 0 };
		float	delta = 0.0f;
	};

	class xtShaderToy
	{
	public:
		xtShaderToy();
		virtual ~xtShaderToy();
		void	Compile(const char* code);
		void	Render(const xtDrawArgs& args);
		bool	Load(const char* vs, const char* fs);

	private:
		GLuint			m_shader = 0;
		struct
		{
			float		iResolution[3];			// viewport resolution (in pixels)
			float		iTime;					// shader playback time (in seconds)
			float		iTimeDelta;				// render time (in seconds)
			int			iFrame;					// shader playback frame
			float		iMouse[4];				// mouse pixel coords. xy: current (if MLB down), zw: click
			//float		iSampleRate;			// sound sample rate (i.e., 44100)
			//float		iDate[4];               // (year, month, day, time in seconds)
			//float		iChannelTime[4];		// channel playback time (in seconds)
			//float		iChannelResolution[4];	// channel resolution (in pixels)
			//int		iChannel0;				// input channel. XX = 2D/Cube
			//int		iChannel1;				// input channel. XX = 2D/Cube
			//int		iChannel2;				// input channel. XX = 2D/Cube
			//int		iChannel3;				// input channel. XX = 2D/Cube
		} m_uniforms;
	};
}

#endif //XT_SHADERTOY_HEADER