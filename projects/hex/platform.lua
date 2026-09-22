project "platform"
  location "%{wks.location}/hex/platform/"
  kind "StaticLib"

  debugdir ("%{target_dir}/%{prj.name}")
  objdir ("%{obj_dir}/%{prj.name}")
  targetdir ("%{target_dir}/%{prj.name}")
  
  files
  {
    "%{prj_src.hex}/platform/**.h",
    "%{prj_src.hex}/platform/**.cpp",
  }

  includedirs
  {
    "%{prj_src.hex}/",
  }
  externalincludedirs
  {
    "%{dependencies.glfw}/include",
    "%{dependencies.vulkan_headers}/include",
  }

  libdirs
  {
    "%{dependencies.glfw}/lib-vc2022",
  }

  links
  {
    "core",
    "glfw3_mt.lib",
  }
