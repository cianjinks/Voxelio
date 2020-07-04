project "Voxel-Editor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
	objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	includedirs
	{
		"../Voxel-Core/src",
		"../Voxel-Core/dependencies",
		"../Voxel-Core/%{IncludeDir.glm}",
		"../Voxel-Core/%{IncludeDir.Glad}",
		"../Voxel-Core/%{IncludeDir.ImGui}",
		"../Voxel-Core/%{IncludeDir.GLFW}",
		"../Voxel-Core/%{IncludeDir.stb_image}",
	}

	links
	{
		"Voxel-Core"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"VX_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "VX_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "VX_RELEASE"
		runtime "Release"
        optimize "on"