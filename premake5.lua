workspace "furtivehook-t7"
    startproject "furtivehook-t7"
    location "build/"

    targetdir "%{wks.location}/bin/%{cfg.buildcfg}/"
    objdir "%{wks.location}/obj/%{cfg.buildcfg}/%{prj.name}/"
    buildlog "%{wks.location}/obj/%{cfg.buildcfg}/%{prj.name}.log"

    systemversion "latest"
    architecture "x64"
    characterset "mbcs"
    warnings "off"
    editandcontinue "off"
    staticruntime "on"

    disablewarnings {
        "4005",
        "4099",
        "5054",
        "26812"
    }
    
    includedirs {
        "src/",
        "$(DXSDK_DIR)include/",
    }

    buildoptions {
        "/Zm200",
        "/utf-8",
        "/std:c++latest"
    }

    flags {
        "shadowedvariables",
        "noincrementallink",
        "undefinedidentifiers",
        "multiprocessorcompile",
    }

    defines {
        "NOMINMAX",
        "WIN32_LEAN_AND_MEAN",
        "_CRT_SECURE_NO_WARNINGS",
        "ASMJIT_STATIC",
    }

    platforms {
        "x64",
    }

    configurations {
        "debug",
        "release",
    }

    configuration "debug"
        defines {
             "DEBUG",
             "STEAM"
        }

        optimize "debug"
        runtime "debug"
        symbols "on"

        targetname "furtivehook-t7-debug"

    configuration "release"
        defines {
            "NDEBUG",
            "STEAM"
        }
        
        optimize "off"
        runtime "release"
        symbols "on"

        targetname "furtivehook-t7-release"

project "furtivehook-t7"
    kind "sharedlib"
    language "c++"
    
	dependson {
        "imgui",
        "steam_api",
    }
    
    files {
		"src/**.h",
		"src/**.hpp",
		"src/**.cpp",
        "src/**.asm",
    }
    
	links {
        "imgui",
        "steam_api.lib",
        "MinHook.lib",
        "ws2_32.lib",
    }
    
    includedirs {
        "src/",
        "$(DXSDK_DIR)include/",
    }

    libdirs {
        "thirdparty/minhook/",
        "thirdparty/steam_api/",
        "$(DXSDK_DIR)lib/x64/",
    }

project "imgui"
    targetname "imgui"
    warnings "off"

    language "c++"
    kind "staticlib"

    files {
        "thirdparty/imgui/*.h",
        "thirdparty/imgui/*.hpp",
        "thirdparty/imgui/*.cpp",
    }

    includedirs {
        "thirdparty/imgui",
    }

project "steam_api"
    targetname "steam_api"
    warnings "off"

    language "c++"
    kind "staticlib"

    files {
        "thirdparty/steam_api/**.hpp",
        "thirdparty/steam_api/**.lib",
    }

    includedirs {
        "thirdparty/steam_api",
    }