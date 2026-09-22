project "core"
  location "%{wks.location}/hex/core"
  kind "StaticLib"

  debugdir ("%{target_dir}/%{prj.name}")
  objdir ("%{obj_dir}/%{prj.name}")
  targetdir ("%{target_dir}/%{prj.name}")
  
  files
  {
    "%{prj_src.hex}/core/**.h",
    "%{prj_src.hex}/core/**.cpp"
  }

  includedirs
  {
    "%{prj_src.hex}/"
  }
