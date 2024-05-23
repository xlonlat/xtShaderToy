# xtShaderToy Usage

./xtShaderToy -w {window_width} -h {window_height} -i {input_file}

-w The default value is 1280

-h The default value is 720

-i Same as the code of the www.ShaderToy.com,There are several test files in the ./res directory

Currently supports the following uniforms:

uniform vec3 iResolution;

uniform float iTime;

uniform float iTimeDelta;

uniform float iFrame;

uniform vec4 iMouse;


# Linux

-> cd {xtShaderToy directory}

-> mkdir build 

-> cd build 

-> cmake ..

-> make

-> ./xtShaderToy -i {input_file}

# Windows

 Microsoft Visual C++ 2005 or later
	
 open {xtShaderToy directory}

 open ./win

 open xtShaderToy.sln & build

 open {xtShaderToy directory}/bin/{platform directory}

 ./xtShaderToy.exe -i {input_file}
