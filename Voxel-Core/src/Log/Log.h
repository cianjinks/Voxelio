#pragma once
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace VoxelCore {

	class Log
	{
	public:
		static void InitLogger();
		static inline std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger;  }
		static inline std::shared_ptr<spdlog::logger>& GetEditorLogger() { return s_EditorLogger;  }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_EditorLogger;
	};
}

#ifdef VX_DEBUG
	#define VX_CORE_ERROR(...) ::VoxelCore::Log::GetCoreLogger()->error(__VA_ARGS__)
	#define VX_CORE_WARN(...) ::VoxelCore::Log::GetCoreLogger()->warn(__VA_ARGS__)
	#define VX_CORE_TRACE(...) ::VoxelCore::Log::GetCoreLogger()->trace(__VA_ARGS__)
	#define VX_CORE_CRITICAL(...) ::VoxelCore::Log::GetCoreLogger()->critical(__VA_ARGS__)
	#define VX_CORE_INFO(...) ::VoxelCore::Log::GetCoreLogger()->info(__VA_ARGS__)

	#define VX_EDITOR_ERROR(...) ::VoxelCore::Log::GetEditorLogger()->error(__VA_ARGS__)
	#define VX_EDITOR_WARN(...) ::VoxelCore::Log::GetEditorLogger()->warn(__VA_ARGS__)
	#define VX_EDITOR_TRACE(...) ::VoxelCore::Log::GetEditorLogger()->trace(__VA_ARGS__)
	#define VX_EDITOR_CRITICAL(...) ::VoxelCore::Log::GetEditorLogger()->critical(__VA_ARGS__)
	#define VX_EDITOR_INFO(...) ::VoxelCore::Log::GetEditorLogger()->info(__VA_ARGS__)
#else
	#define VX_CORE_ERROR
	#define VX_CORE_WARN
	#define VX_CORE_TRACE
	#define VX_CORE_CRITICAL
	#define VX_CORE_INFO 

	#define VX_EDITOR_ERROR
	#define VX_EDITOR_WARN
	#define VX_EDITOR_TRACE
	#define VX_EDITOR_CRITICAL
	#define VX_EDITOR_INFO
#endif