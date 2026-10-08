include "directories.lua"

workspace "Hex"
  location "../build/%{action}"
  startproject "sandbox"
  multiprocessorcompile "On"

  language "C++"
  toolset "clang"
  cppdialect "C++20"
  staticruntime "on"

  filter { "toolset:clang" }
    warnings "Extra"
    fatalwarnings { "all" }
    enablewarnings {
      "all",
      "extra",
      "pedantic"
    }
    disablewarnings {
      "c++98-compat-pedantic",
      "gnu-anonymous-struct",
      "nested-anon-types",
      "covered-switch-default",
      "language-extension-token",
      "missing-field-initializers",
      "global-constructors",
      "exit-time-destructors",
      "unsafe-buffer-usage"
    }

  defines { 'HEX_ASSETS_DIR="' .. assets_path .. '"' }

  filter "system:Windows"
    architecture "x64"
    defines { "HEX_WINDOWS" }

  configurations { "Debug", "Profile", "Release" }

  filter "configurations:Debug"
    runtime "Debug"
    symbols "On"
    defines { "HEX_DEBUG" }

  filter "configurations:Profile"
    runtime "Release"
    optimize "On"
    defines { "HEX_PROFILE" }

  filter "configurations:Release"
    runtime "Release"
    optimize "On"
    defines { "HEX_RELEASE" }

  filter {}

group "hex"
  include "hex/core"
  include "hex/platform"
  include "hex/graphics"
  include "hex/maths"
  include "hex/engine"

group "projects"
  include "sandbox"

group ""
