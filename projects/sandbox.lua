project "sandbox"
  location "%{wks.location}/sandbox"
  kind "ConsoleApp"

  debugdir (target_dir)
  objdir (obj_dir)
  targetdir (target_dir)

  files
  {
    "%{prj_src.sandbox}/**.h",
    "%{prj_src.sandbox}/**.cpp"
  }

  includedirs
  {
    "%{prj_src.hex}/",

    "%{prj_src.sandbox}/"
  }

  links
  {
    "core",
    "platform",
    "graphics",
    "engine",
  }
