#include "voxelpch.h"

#include "Window.h"
#ifdef VX_PLATFORM_WINDOWS
	#include "Platform/Windows/WindowsWindow.h"
#endif

namespace VoxelCore {

	std::unique_ptr<Window> Window::Create()
	{
	#ifdef VX_PLATFORM_WINDOWS
		return std::make_unique<WindowsWindow>();
	#else
		VX_CORE_CRITICAL("Unsupported Platform Declared");
		return nullptr;
	#endif
	}
}