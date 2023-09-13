workspace "FastMedianFilter"
   architecture "x86_64"
   configurations
   { 
      "Debug",
      "Release" 
   }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "MedianFilter"
   location "MedianFilter"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++17"

   targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

   files 
   { 
       "%{prj.name}/src/**.h",
       "%{prj.name}/src/**.cpp" 
   }

   includedirs
   {
      "%{prj.name}/src/vendor/stb_image"
   }

   filter "system:windows"
      systemversion "latest"

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"