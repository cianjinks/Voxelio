-- Voxelio
workspace "Voxelio"
	architecture "x64"
	startproject "Voxel-Editor"

	configurations
	{
		"Debug",
		"Release"
	}
	
	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "dependencies/GLFW/include"
IncludeDir["Glad"] = "dependencies/glad/include"
IncludeDir["ImGui"] = "dependencies/imgui"
IncludeDir["glm"] = "dependencies/glm"
IncludeDir["stb_image"] = "dependencies/stb_image"

-- Projects
group "Dependencies"
	include "Voxel-Core/dependencies/GLFW"
	include "Voxel-Core/dependencies/glad"
	include "Voxel-Core/dependencies/imgui"
group ""

include "Voxel-Core"
include "Voxel-Editor"