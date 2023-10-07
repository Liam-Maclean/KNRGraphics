using System.IO; // For Path.Combine
using Sharpmake; // Contains the entire Sharpmake object library.

[Generate]
public class KNRGraphics : Sharpmake.Project
{

    public enum GraphicsPlatform 
    {
        DIRECTX12,
        DIRECTX11,
        VULKAN,
        OPENGL,
    }

    public GraphicsPlatform m_GraphicsAPI = GraphicsPlatform.DIRECTX12;

    public string[] GetIncludeDirectoriesByGraphicsAPI(GraphicsPlatform platform)
    {
        switch (platform)
        {
            case GraphicsPlatform.DIRECTX12:
                return new[] {
                    "platform/directx12",
                };
            case GraphicsPlatform.DIRECTX11:
                return new[] {
                    "platform/directx11",
                };
            case GraphicsPlatform.VULKAN:
                return new[] {
                    "platform/vulkan",
                };
            case GraphicsPlatform.OPENGL:
                return new[] {
                    "platform/opengl",
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
                };
            case GraphicsPlatform.DIRECTX11:
                return new[] {
                    "PLATFORM_DX11",
                };
            case GraphicsPlatform.VULKAN:
                return new[] {
                    "PLATFORM_VULKAN",
                };
            case GraphicsPlatform.OPENGL:
                return new[] {
                    "PLATFORM_OPENGL",
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
                    "PLATFORM_DX11",
                };
            case GraphicsPlatform.VULKAN:
                return new[] {
                    "PLATFORM_VULKAN",
                };
            case GraphicsPlatform.OPENGL:
                return new[] {
                    "PLATFORM_OPENGL",
                };       
        }
        return null;
    }

    public KNRGraphics()
    {
        Name = "KNRGraphics";

        AddTargets(new Target(
                Platform.win32 | Platform.win64,
                DevEnv.vs2019,
                Optimization.Debug | Optimization.Release, OutputType.Lib
        ));

        SourceRootPath = Globals.GraphicsDir;
        //AdditionalSourceRootPaths.AddRange(new[]{
        //    Path.Combine(Globals.GraphicsDir, ),
        //};);
        SourceFilesExcludeRegex.AddRange(GetExcludeDirectoriesByGraphicsAPI(m_GraphicsAPI));
    }

    void SetupStaticLibraryPaths(Configuration configuration, DependencySetting dependencySetting, Configuration dependency)
    {

    }

    [Configure]
    public void ConfigureAll(Configuration conf, Target target)
    {
        //Platform specific set-up
        conf.Defines.AddRange(GetDefinesByGraphicsAPI(m_GraphicsAPI));
        conf.LibraryFiles.AddRange(GetLibrariesByGraphicsAPI(m_GraphicsAPI));
        conf.IncludePaths.Add(Globals.GraphicsDir);

        //PCH
        conf.PrecompHeader = "knrpch.h";//Path.Combine(Globals.GraphicsDir, "knrpch.h");
        conf.PrecompSource = "knrpch.cpp";//Path.Combine(Globals.GraphicsDir, "knrpch.cpp");
        conf.ForcedIncludes.Add("knrpch.h");
        //Project include and output
        conf.ProjectFileName = "[project.Name]";
        conf.ProjectPath = Path.Combine(Globals.BuildDir, "[project.Name]");

        conf.TargetPath = Path.Combine(Globals.BuildDir, "[project.Name]", "output", "[target.Platform]");
        conf.TargetFileName = @"KNRGraphics";
        conf.Output = Configuration.OutputType.Lib;

    }
}