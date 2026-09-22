project "graphics"
  location "%{wks.location}/hex/graphics/"
  kind "StaticLib"

  debugdir ("%{target_dir}/%{prj.name}")
  objdir ("%{obj_dir}/%{prj.name}")
  targetdir ("%{target_dir}/%{prj.name}")
  
  files
  {
    "%{prj_src.hex}/graphics/**.h",
    "%{prj_src.hex}/graphics/**.hpp",
    "%{prj_src.hex}/graphics/**.cpp",
    "%{prj_src.hex}/graphics/**.slang",
    "%{prj_src.hex}/graphics/**.json",
    "%{dependencies.volk}/volk.c",
    "%{dependencies.imgui}/imgui.cpp",
    "%{dependencies.imgui}/imgui_draw.cpp",
    "%{dependencies.imgui}/imgui_tables.cpp",
    "%{dependencies.imgui}/imgui_widgets.cpp",
    "%{dependencies.imgui}/imgui_demo.cpp",
    "%{dependencies.imgui}/backends/imgui_impl_vulkan.cpp",
    "%{dependencies.imgui}/backends/imgui_impl_glfw.cpp",
  }

  includedirs
  {
    "%{prj_src.hex}/",
  }

  externalincludedirs
  {
    "%{dependencies.vulkan_headers}/include",
    "%{dependencies.volk}",
    "%{dependencies.glfw}/include",
    "%{dependencies.imgui}",
    "%{dependencies.imgui}/backends",

    -- maths headers use anonymous struct/union swizzles and direct float ==
    -- comparisons by design; treat as external so -Werror doesn't trip on them.
    -- Needed here too: passes precompute values (matrices, vectors) on the CPU
    -- and upload them, rather than computing them in-shader.
    "%{prj_src.hex}/maths/",
  }

  defines { "IMGUI_IMPL_VULKAN_USE_VOLK" }

  links
  {
    "core",
    "platform",
  }

  -- VOLK
  filter "files:**/volk.c"
    warnings "Off"
  filter {}

  -- IMGUI
  filter "files:**/imgui-1.92.9b-docking/**.cpp"
    warnings "Off"
  filter {}

  -- VULKAN PROFILES: --output-library-src is intentionally omitted so the generator only emits
  -- the header-only profiles.hpp (VPAPI_ATTR = inline, included via vkCommon.h) and skips the
  -- split profiles.h/.cpp library variant, whose VPAPI_ATTR symbols have external linkage and
  -- would otherwise ODR-clash with the inline definitions pulled into every TU via profiles.hpp.
  filter "files:**/profiles/hex_vulkan_profiles.json"
    buildmessage "Generating Vulkan Profiles"

    buildinputs {
      "%{dependencies.vulkan_headers}/registry/vk.xml",
      "%{prj_src.hex}/graphics/vulkan/profiles/*.json"
    }

    buildoutputs {
      "%{prj_src.hex}/graphics/vulkan/profiles.hpp",
    }

    buildcommands {
      "python \"%{dependencies.vulkan_profiles}/scripts/gen_profiles_solution.py\" -r \"%{dependencies.vulkan_headers}/registry/vk.xml\" -i \"%{prj_src.hex}/graphics/vulkan/profiles\" --output-library-inc \"%{prj_src.hex}/graphics/vulkan\" --output-library-filename \"profiles\""
    }

  filter {}

  -- SHADERS
  prebuildcommands
  {
    "{MKDIR} %{shader_out_dir}"
  }

  filter "files:**.slang"
    buildmessage "Compiling Shader: %{file.name}"
    buildoutputs { "%{shader_out_dir}/%{file.basename}.spv" }
  filter { "files:**.slang", "configurations:Debug" }
    buildcommands { '"%{slangc_bin}" "%{file.relpath}" -target spirv -fvk-use-entrypoint-name -O0 -g2 -o "%{shader_out_dir}/%{file.basename}.spv"' }
  filter { "files:**.slang", "configurations:Profile or configurations:Release" }
    buildcommands { '"%{slangc_bin}" "%{file.relpath}" -target spirv -fvk-use-entrypoint-name -O3 -o "%{shader_out_dir}/%{file.basename}.spv"' }
  filter {}
