workspace "Mandelbrot"
  architecture "x64"

  startproject "Mandelbrot"

  configurations {
    "Debug",
    "Release",
    "Dist"
  }

outdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
indirs = {}
indirs["GLFW"] = "Mandelbrot/vendor/GLFW/include/"

group "Denpendencies"
  include "Mandelbrot/vendor/premake5.glfw.lua"
group ""

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
    "%{prj.name}/src/**.cpp",
    "%{prj.name}/vendor/stb/**.h",
    "%{prj.name}/vendor/stb/**.cpp"
  }

  includedirs {
    "%{prj.name}/src",
    "%{prj.name}/vendor",
    "%{indirs.GLFW}"
  }

  links {
    "GLFW"
  }
  
  filter "system:macosx"
    systemversion "latest"
    system "macosx"

    defines {
      "GL_SILENCE_DEPRECATION"
    }

    links {
      "Cocoa.framework",
      "IOKit.framework",
      "CoreVideo.framework",
      "OpenGL.framework"
    }
  
  filter "system:linux"
    system "linux"
    systemversion "latest"

    links {
      "pthread",
      "dl",
      "m",
      "GL",
      "X11"
    }
  
  filter "system:Windows"
    system "Windows"
    systemversion "latest"

    links {
      "OpenGL32.lib",
    }

  filter "configurations:Debug"
    symbols "On"

  filter "configurations:Release"
    optimize "On"

  filter "configurations:Dist"
    optimize "On"