workspace "MyTinyRenderer"
    configurations {"Debug", "Release"}
    conformancemode "On"

    language "C++"
    cppdialect "C++20"
    
    flags { "MultiProcessorCompile" }

    filter "configurations:Debug"
		optimize "Off"
		symbols "On"

    filter "configurations:Release"
		optimize "On"
		symbols "Default"


project "MyTinyRenderer"
    kind "ConsoleApp"

    targetdir ("bin/".."%{cfg.buildcfg}".."/%{prj.name}")
    objdir ("bin-int/".."%{cfg.buildcfg}".."/%{prj.name}")


    
    files {
        "src/**.h",
        "src/**.c",
        "src/**.hpp",
        "src/**.cpp",
        "vendor/**.h",
        "vendor/**.cpp"
    }

    includedirs {
        "src/",
        "vendor/"
    }

    filter ""