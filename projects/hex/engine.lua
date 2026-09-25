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
    "%{dependencies.vulkan_headers}/include",
    "%{dependencies.volk}",
    "%{dependencies.imgui}",
    "%{prj_src.hex}/maths/",
  }

  links
  {
    "core",
    "platform",
    "graphics",
  }
