#include "Log.h"

namespace VoxelCore {

	std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
	std::shared_ptr<spdlog::logger> Log::s_EditorLogger;

	void Log::InitLogger()
	{
		spdlog::set_pattern("%^[%T][%n] %v %$");
		s_CoreLogger = spdlog::stdout_color_mt("Voxel Core");
		s_EditorLogger = spdlog::stdout_color_mt("Voxel Editor");
	}
}