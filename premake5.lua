workspace "ASCII_convertor"
	architecture "x86_64"

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

project "image_ascii"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/obj/" .. outputdir .. "/%{prj.name}")

    files
    {
        "src/**.h",
        "src/**.cpp",
    }

    includedirs
    {
        "src"
    }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"
        defines 
        { 
            "_DEBUG"
        }
        buildoptions
        {
            "-Wall",
            "-Wextra"
        }

    filter "configurations:Release"
        runtime "Release"
        optimize "on"