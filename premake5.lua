workspace "Mandelbrot"
  architecture "x64"

  startproject "Mandelbrot"

  configurations {
    "Debug",
    "Release",
    "Dist"
  }

outdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Mandelbrot"
  location "Mandelbrot"
  kind "ConsoleApp"
  language "C++"
  staticruntime "on"
  cppdialect "C++17"

  targetdir("bin/" ..outdir.. "/%{prj.name}")
  objdir("obj/" ..outdir.. "/%{prj.name}")

  files {
    "%{prj.name}/src/**.h",
    "%{prj.name}/src/**.cpp"
  }

  filter "system:macosx"
    systemversion "latest"
    system "macosx"
  
  filter "system:linux"
    system "linux"
    systemversion "latest"
  
  filter "system:Windows"
    system "Windows"
    systemversion "latest"

  filter "configurations:Debug"
    symbols "On"

  filter "configurations:Release"
    optimize "On"

  filter "configurations:Dist"
    optimize "On"