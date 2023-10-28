using System.IO; // For Path.Combine
using Sharpmake; // Contains the entire Sharpmake object library.

[Generate]
public class KNRGraphics : KNRLibBase
{
    public enum GraphicsPlatform 
    {
        DIRECTX12,
        DIRECTX11,
        VULKAN,
        OPENGL,
    }

    public GraphicsPlatform m_GraphicsAPI = GraphicsPlatform.DIRECTX12;

    public string[] GetLibraryPathsByGraphicsAPI(GraphicsPlatform platform)
    {
        switch (platform)
        {
            case GraphicsPlatform.DIRECTX12:
                return new[] {
                    Globals.GraphicsDir,
                };
            case GraphicsPlatform.DIRECTX11:
                return new[] {
                    Globals.GraphicsDir,
                };
            case GraphicsPlatform.VULKAN:
                return new[] { 
                    Path.Combine(Globals.ExternalDir, "vulkan", "lib"), 
                };
            case GraphicsPlatform.OPENGL:
                return new[] {
                    Globals.GraphicsDir,
                };         
        }

        return null;
    }

    public string[] GetIncludeDirectoriesByGraphicsAPI(GraphicsPlatform platform)
    {
        switch (platform)
        {
            case GraphicsPlatform.DIRECTX12:
                return new[] {
                    Globals.GraphicsDir,
                };
            case GraphicsPlatform.DIRECTX11:
                return new[] {
                    Globals.GraphicsDir,
                };
            case GraphicsPlatform.VULKAN:
                return new[] { 
                    Path.Combine(Globals.ExternalDir,"vulkan", "include"),
                    Path.Combine(Globals.ExternalDir, "vulkan", "lib"),
                    Globals.GraphicsDir,
                  
                };
            case GraphicsPlatform.OPENGL:
                return new[] {
                    Globals.GraphicsDir,
                };         
        }

        return null;
    }

    public string[] GetExcludeDirectoriesByGraphicsAPI(GraphicsPlatform platform)
    {
        switch (platform)
        {
            case GraphicsPlatform.DIRECTX12:
                return new[] {
                    "directx11",
                    "vulkan",
                    "opengl",
                };
            case GraphicsPlatform.DIRECTX11:
                return new[] {
                    "directx12",
                    "vulkan",
                    "opengl",
                };
            case GraphicsPlatform.VULKAN:
                return new[] {
                    "directx12",
                    "directx11",
                    "opengl",
                };
            case GraphicsPlatform.OPENGL:
                return new[] {
                    "directx12",
                    "directx11",
                    "vulkan",
                };       
        }

        return null;
    }
    
    public string[] GetDefinesByGraphicsAPI(GraphicsPlatform platform)
    {
        switch (platform)
        {
            case GraphicsPlatform.DIRECTX12:
                return new[] {
                    "PLATFORM_DX12",
                    "NOMINMAX",
                };
            case GraphicsPlatform.DIRECTX11:
                return new[] {
                    "PLATFORM_DX11",
                    "NOMINMAX",
                };
            case GraphicsPlatform.VULKAN:
                return new[] {
                    "PLATFORM_VULKAN",
                    "NOMINMAX",
                };
            case GraphicsPlatform.OPENGL:
                return new[] {
                    "PLATFORM_OPENGL",
                    "NOMINMAX",
                };       
        }

        return null;
    }

    public string[] GetLibrariesByGraphicsAPI(GraphicsPlatform platform)
    {
        switch (platform)
        {
            case GraphicsPlatform.DIRECTX12:
                return new[] {
                    "d3d12.lib",
                    "dxgi.lib",
                };
            case GraphicsPlatform.DIRECTX11:
                return new[] {
                    "d3d11.lib",
                    "dxgi.lib",
                };
            case GraphicsPlatform.VULKAN:
                return new[] {
                    "vulkan-1.lib",
                };
            case GraphicsPlatform.OPENGL:
                return new[] {
                    "opengl32.lib",
                };       
        }
        return null;
    }

    public KNRGraphics()
        : base("KNRGraphics")
    {
        SourceRootPath = Globals.GraphicsDir;
        SourceFilesExcludeRegex.AddRange(GetExcludeDirectoriesByGraphicsAPI(m_GraphicsAPI));
    }

    [Configure]
    public void ConfigureAll(Configuration conf, Target target)
    {
        SetUpVisualStudioOptions(conf, target);

        //Platform specific set-up
        conf.Defines.AddRange(GetDefinesByGraphicsAPI(m_GraphicsAPI));

        //Libs
        conf.LibraryFiles.AddRange(GetLibrariesByGraphicsAPI(m_GraphicsAPI));
        conf.LibraryPaths.AddRange(GetLibraryPathsByGraphicsAPI(m_GraphicsAPI));

        //Include files
        conf.IncludePaths.Add(Globals.GraphicsDir);
        conf.IncludePaths.AddRange(GetIncludeDirectoriesByGraphicsAPI(m_GraphicsAPI));  
        
        //Spdlog (we should make this a library at some point)
        conf.IncludePaths.Add(Path.Combine(Globals.ExternalDir, "spdlog", "include"));

        //PCH
        conf.PrecompHeader = "knrpch.h";
        conf.PrecompSource = "knrpch.cpp";

        conf.ForcedIncludes.Add("knrpch.h");
        //Project include and output
        conf.ProjectFileName = "[project.Name]";
        conf.ProjectPath = Path.Combine(Globals.BuildDir, "[project.Name]");

        conf.TargetPath = Path.Combine(Globals.BuildDir, "[project.Name]", "output", "[target.Platform]");
        conf.TargetFileName = @"KNRGraphics";
        conf.Output = Configuration.OutputType.Lib;

    }
}