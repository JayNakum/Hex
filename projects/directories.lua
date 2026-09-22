-- DEFINED PATHS --
action = _ACTION or "null"
obj_dir = "%{wks.location}/../obj/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
target_dir = "%{wks.location}/../bin/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

code_path = "%{wks.location}/../../code"
vendors_path = "%{wks.location}/../../vendors"

-- CORE --
prj_src = {}
prj_src["sandbox"] = "%{code_path}/sandbox"
prj_src["hex"] = "%{code_path}/hex"

-- DEPENDENCIES --
dependencies = {}
dependencies["volk"] = "%{vendors_path}/volk/volk-1.4.350"
dependencies["vulkan_headers"] = "%{vendors_path}/Vulkan-Headers/Vulkan-Headers-vulkan-sdk-1.4.350.0"
dependencies["vulkan_profiles"] = "%{vendors_path}/Vulkan-Profiles/Vulkan-Profiles-vulkan-sdk-1.4.350.0"
dependencies["glfw"] = "%{vendors_path}/glfw/glfw-3.5.1.bin.WIN64"
dependencies["slang"] = "%{vendors_path}/slang"
dependencies["imgui"] = "%{vendors_path}/imgui/imgui-1.92.9b-docking"

-- SHADER TOOLCHAIN --
slangc_bin = "%{dependencies.slang}/bin/slangc.exe"
shader_out_dir = "%{target_dir}/Shaders"
