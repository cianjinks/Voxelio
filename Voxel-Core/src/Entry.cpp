#include "Log/Log.h"
#include "Core/Application.h"

extern VoxelCore::Application* VoxelCore::CreateApplication();

#ifdef VX_PLATFORM_WINDOWS
int main()
{
	VoxelCore::Log::InitLogger();
	VX_CORE_WARN("Initialised Core Logger");
	VX_EDITOR_WARN("Initialised Editor Logger");

	VoxelCore::Application* app = VoxelCore::CreateApplication();
	app->Run();
	delete app;

	return 0;
}
#endif