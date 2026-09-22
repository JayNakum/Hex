project "maths"
  location "%{wks.location}/hex/maths"
  kind "None"

  files
  {
    "%{prj_src.hex}/maths/**.h",
    "%{prj_src.hex}/maths/**.cpp"
  }

  includedirs
  {
    "%{prj_src.hex}/",
  }
