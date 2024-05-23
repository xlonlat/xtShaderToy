#include "xtShaderToy.h"
#include <GLFW/glfw3.h>
#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <sys/stat.h>

int			m_width = 1280;
int			m_height = 720;
std::string	m_file = "";
time_t		m_filetime;

bool IsFileModified()
{
	struct stat info;
	int ret = stat(m_file.c_str(), &info);
	if (ret != 0)
		return false;

	if (m_filetime != info.st_mtime)
	{
		m_filetime = info.st_mtime;
		return true;
	}
	return false;
}

void ReadFile(std::string& byte)
{
	std::ifstream ifile;
	ifile.open(m_file, std::ios_base::in);
	if (ifile.is_open())
	{
		std::stringstream stream;
		stream << ifile.rdbuf();
		byte = stream.str();
		ifile.close();
	}
}

void ShowHelpInfo()
{
	std::ostringstream oss;
	oss << "Options:" << std::endl
		<< "-w Window width" << std::endl
		<< "-h Window height" << std::endl
		<< "-i Input file path" << std::endl
		<< "----------------------------------------" << std::endl;
	std::cout << oss.str();
}

void ParseOptions(int argc, char* argv[])
{
	if (argc == 1)
	{
		ShowHelpInfo();
		return;
	}

	std::cout << "Options:" << std::endl;
	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "-w") == 0)
		{
			m_width = atoi(argv[++i]);
			if (m_width <= 0) m_width = 1280;
		}
		else if (strcmp(argv[i], "-h") == 0)
		{
			m_height = atoi(argv[++i]);
			if (m_height <= 0) m_height = 720;
		}
		else if (strcmp(argv[i], "-i") == 0)
		{
			m_file = argv[++i];
		}
	}

	std::cout << "-w " << m_width << std::endl;
	std::cout << "-h " << m_height << std::endl;
	std::cout << "-i " << m_file << std::endl;
	std::cout << "----------------------------------------" << std::endl;
}

void OnError(int error, const char* description)
{
	std::cout << "[GLFW-Error]:" << std::endl;
	std::cout << description << std::endl;
	std::cout << "----------------------------------------" << std::endl;
}

int main(int argc, char* argv[])
{
	ParseOptions(argc, argv);

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	glfwSetErrorCallback(OnError);

	GLFWwindow* window = glfwCreateWindow(m_width, m_height, "xtShaderToy", nullptr, nullptr);
	if (window == nullptr)
	{
		glfwTerminate();
		return 1;
	}

	glfwMakeContextCurrent(window);
	glfwSetWindowUserPointer(window, (void*)window);
	glfwSwapInterval(0);

	{
		gladLoadGL();
	}

	{
		typedef std::chrono::high_resolution_clock::time_point TimePoint;

		xlonlat::xtShaderToy toy;
		xlonlat::xtDrawArgs args;
		TimePoint prev = std::chrono::high_resolution_clock::now();

		while (window && !glfwWindowShouldClose(window))
		{
			if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
				glfwSetWindowShouldClose(window, true);

			{
				TimePoint curr = std::chrono::high_resolution_clock::now();
				std::chrono::duration<double, std::deci> time_span = curr - prev;
				prev = curr;

				double mouse[2] = { 0 };
				glfwGetCursorPos(window, &mouse[0], &mouse[1]);
				glfwGetWindowSize(window, &args.width, &args.height);

				args.mouse[0] = (int)mouse[0];
				args.mouse[1] = (int)mouse[1];
				args.mouse[2] = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
				args.delta = (float)time_span.count() * 0.1f;
			}

			glViewport(0, 0, args.width, args.height);
			glScissor(0, 0, args.width, args.height);

			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();

			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			glClearDepth(1.0f);
			glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			if (IsFileModified())
			{
				std::string byte; ReadFile(byte);
				toy.Compile(byte.c_str());
			}

			toy.Render(args);

			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
