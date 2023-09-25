project "FilterApp"
    location "../FilterApp"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files 
    { 
        "../%{prj.name}/src/**.h",
        "../%{prj.name}/src/**.cpp" 
    }

    includedirs
    {
        "../Filter/src"
    }

    links
    {
        "Filter"
    }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"