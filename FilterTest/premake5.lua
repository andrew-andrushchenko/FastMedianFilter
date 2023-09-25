project "FilterTest"
    location "../FilterTest"
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
        "../Filter/src/vendor/stb_image",
        "../Filter/src/filter",
        "../vendor/googletest/googletest/include",
        "../vendor/googletest/googletest/"
    }

    links
    {
        "Filter", "googletest"
    }

    filter "system:windows"
        systemversion "latest"