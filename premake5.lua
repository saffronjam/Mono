project "mono"
    kind "StaticLib"
    language "C++"
    staticruntime "on"
	cppdialect "C++17"
	
    files
    {
        "include/monopp/**.*",
        "src/**.*",
    }

    includedirs
    {
        "include"
    }
    
    filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		runtime "Release"
		optimize "on"