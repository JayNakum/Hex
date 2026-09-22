project "engine"
  location "%{wks.location}/hex/engine"
  kind "StaticLib"

  debugdir ("%{target_dir}/%{prj.name}")
  objdir ("%{obj_dir}/%{prj.name}")
  targetdir ("%{target_dir}/%{prj.name}")

  files
  {
    "%{prj_src.hex}/engine/**.h",
    "%{prj_src.hex}/engine/**.cpp",
  }

  includedirs
  {
    "%{prj_src.hex}/",
  }

  externalincludedirs
  {
    -- Engine.cpp includes graphics/Renderer.h (full type, not just forward-declared) to
    -- construct it, so this TU needs Vulkan on its include path even though engine/'s own
    -- public headers (Engine.h) never mention a Vulkan type.
    "%{dependencies.vulkan_headers}/include",
    "%{dependencies.volk}",

    -- Renderer.h holds an ImGuiPass by value, whose ImGUI.h pulls in <imgui.h> when
    -- HEX_IMGUI_ENABLED is defined, so this TU needs it resolvable too.
    "%{dependencies.imgui}",

    -- maths headers use anonymous struct/union swizzles and direct float ==
    -- comparisons by design; treat as external so -Werror doesn't trip on them.
    "%{prj_src.hex}/maths/",
  }

  links
  {
    "core",
    "platform",
    "graphics",
  }
