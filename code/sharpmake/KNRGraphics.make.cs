using System.IO; // For Path.Combine
using Sharpmake; // Contains the entire Sharpmake object library.


public enum GraphicsPlatform 
{
    DIRECTX12,
    DIRECTX11,
    VULKAN,
}
[Generate]
public class KNRGraphics : Sharpmake.Project
{
    public GraphicsPlatform m_GraphicsAPI = GraphicsPlatform.DIRECTX12;

    public string[] GetIncludeDirectoriesByGraphicsAPI(GraphicsPlatform platform)
    {
        switch (platform)
        {
            case DIRECTX12:
                return new[] {
                    "platform/directx12",
                };
            case DIRECTX11:
                return new[] {
                    "platform/directx11",
                };
            case VULKAN:
                return new[] {
                    "platform/vulkan",
                };
                case OPENGL:
                return new[] {
                    "platform/opengl",
                };         
        }
    }

    public string[] GetExcludeDirectoriesByGraphicsAPI(GraphicsPlatform platform)
    {
        switch (platform)
        {
            case DIRECTX12:
                return new[] {
                    "platform/directx11",
                    "platform/vulkan",
                    "platform/opengl",
                };
            case DIRECTX11:
                return new[] {
                    "platform/directx12",
                    "platform/vulkan",
                    "platform/opengl",
                };
            case VULKAN:
                return new[] {
                    "platform/directx12",
                    "platform/directx11",
                    "platform/opengl",
                };
            case OPENGL:
                return new[] {
                    "platform/directx12",
                    "platform/directx11",
                    "platform/vulkan",
                    "platform/opengl",
                };       
        }

        return null;
    }

   
    public string[] GetDefinesByGraphicsAPI(GraphicsPlatform platform)
    {
        switch (platform)
        {
            case DIRECTX12:
                return new[] {
                    "PLATFORM_DX12",
                };
            case DIRECTX11:
                return new[] {
                    "PLATFORM_DX11",
                };
            case VULKAN:
                return new[] {
                    "PLATFORM_VULKAN",
                };
            case OPENGL:
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

        //SourceFilesExtension.AddRange(new string[]{
        //  ".cpp",
        //    ".h"
        //};);
    }

    [Configure]
    public void ConfigureAll(Configuration conf, Target target)
    {
        conf.ProjectFileName = "[project.Name]";
        conf.ProjectPath = Path.Combine(Globals.BuildDir, "[project.Name]");

        conf.TargetPath = Path.Combine(Globals.BuildDir, "[project.Name]", "output", "[target.Platform]");
        conf.TargetFileName = @"KNRGraphics";
        conf.Output = Configuration.OutputType.Lib;

    }
}