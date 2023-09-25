workspace "FastMedianFilter"
   architecture "x86_64"
   startproject "FilterApp"
   configurations
   { 
      "Debug",
      "Release" 
   }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "googletest"
   kind "StaticLib"
   language "C++"
   cppdialect "C++17"

   targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

   files
   {
      "vendor/googletest/googletest/**.h",
      "vendor/googletest/googletest/**.hpp",
      "vendor/googletest/googletest/src/**.cc"
   }

   includedirs
   {
      "vendor/googletest/googletest/include",
      "vendor/googletest/googletest/"
   }

   filter "system:windows"
      systemversion "latest"

include "Filter/premake5.lua"
include "FilterApp/premake5.lua"
include "FilterTest/premake5.lua"