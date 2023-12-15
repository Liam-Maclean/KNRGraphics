using System.IO; // For Path.Combine
using Sharpmake; // Contains the entire Sharpmake object library.
using System.Collections;
using System.Linq;
using ProjConfig = Sharpmake.Project.Configuration;

[Generate]
public class KNRShaders : KNRLibBase
{
    public enum GraphicsPlatform 
    {
        DIRECTX12,
        DIRECTX11,
        VULKAN,
        OPENGL,
    }

    public static GraphicsPlatform m_GraphicsAPI = GraphicsPlatform.DIRECTX12;

    public KNRShaders()
        : base("KNRShaders")
    {
        SourceRootPath = Globals.ShadersDir;
    }

    protected override void ExcludeOutputFiles()
    {
        base.ExcludeOutputFiles();
        ShaderCompiler.CreateShaderBuildSteps((Project)this, m_GraphicsAPI);
    }

    [Configure]
    public void ConfigureAll(ProjConfig conf, Target target)
    {
        //Platform specific set-up
        conf.IncludePaths.Add(Globals.ShadersDir);
        conf.IncludePaths.Add(Globals.ShaderAssetsDir);

        //Project include and output
        conf.ProjectFileName = "[project.Name]";
        conf.ProjectPath = Path.Combine(Globals.BuildDir, "[project.Name]");

        conf.TargetPath = Path.Combine(Globals.BuildDir, "[project.Name]", "output", "[target.Platform]");
        conf.TargetFileName = @"KNRShaders";
        conf.Output = Configuration.OutputType.Lib;
    }

    public class ShaderCompiler : ProjConfig.CustomFileBuildStep
    {
        private static string[] GetShaderDefinesByGraphicsPlatform(GraphicsPlatform platform)
        {
            switch (platform)
            {
                case GraphicsPlatform.DIRECTX12:
                    return new[]{
                        "PLATFORM_DIRECTX12",
                    };
                case GraphicsPlatform.DIRECTX11:
                    return new[]{
                        "PLATFORM_DIRECTX11",
                    };
                case GraphicsPlatform.VULKAN:
                    return new[]{
                        "PLATFORM_VULKAN",
                    };
                case GraphicsPlatform.OPENGL:
                    return new[]{
                        "PLATFORM_OPENGL",
                    };
            }
            return null;
        }


        public static string[] GetIncludesFromFile(string file)
        {
             return new[] {
                   "",
            }; 
        }

        public static string[] GetDefines()
        {
            return new[] {
                   "SM5",
            }; 
        }

        private static string GetShaderExecutableByGraphicsPlatform(GraphicsPlatform platform)
        {
            switch (platform)
            {
                case GraphicsPlatform.DIRECTX12:
                case GraphicsPlatform.DIRECTX11:
                case GraphicsPlatform.VULKAN:
                case GraphicsPlatform.OPENGL:
                    return "dxc.exe";
            }
            return null;
        }

        private static string GetOutputExtensionByGraphicsPlatform(GraphicsPlatform platform)
        {
            switch (platform)
            {
            case GraphicsPlatform.DIRECTX12:
            case GraphicsPlatform.DIRECTX11:
                return ".dxc";
            case GraphicsPlatform.VULKAN:
            case GraphicsPlatform.OPENGL:
                return ".spirv";
            } 
            return "";
        }

        private static bool IsSpirvPlatform(GraphicsPlatform platform)
        {
            switch (platform)
            {
            case GraphicsPlatform.DIRECTX12:
            case GraphicsPlatform.DIRECTX11:
                return false;
            case GraphicsPlatform.VULKAN:
            case GraphicsPlatform.OPENGL:
                return true;
            } 
            return false;
        }

        public static string GetShaderModelByShaderType(string filenameEnding)
        {
            if (filenameEnding == "VS.hlsl")
            {
                return "vs_6_0";
            }
            else if (filenameEnding == "PS.hlsl")
            {
                return "ps_6_0";
            }
            else if(filenameEnding == "CS.hlsl")
            {
                return "cs_6_0";
            }
            return "";
        }

        public static void CreateShaderBuildSteps(Project project, GraphicsPlatform graphicsPlatform)
        {
            CreateShaderCustomBuildSteps(project, "VS.hlsl", GetShaderModelByShaderType("VS.hlsl"), graphicsPlatform);
            CreateShaderCustomBuildSteps(project, "PS.hlsl", GetShaderModelByShaderType("PS.hlsl"), graphicsPlatform);
            CreateShaderCustomBuildSteps(project, "CS.hlsl", GetShaderModelByShaderType("CS.hlsl"), graphicsPlatform);
        }

        public static void CreateShaderCustomBuildSteps(Project project, string filenameEnding, string shaderModelFlag, GraphicsPlatform graphicsPlatform)
        {
            Strings shaderFiles = new Strings(project.ResolvedSourceFiles.Where(file => file.EndsWith(filenameEnding, System.StringComparison.InvariantCultureIgnoreCase)));
            foreach(ProjConfig conf in project.Configurations)
            {
                foreach (string shader in shaderFiles)
                {
                    ShaderCompiler shaderCompiler = new ShaderCompiler(conf, shader, shaderModelFlag, GetIncludesFromFile(shader), GetDefines(), graphicsPlatform);
                    conf.CustomFileBuildSteps.Add(shaderCompiler);
                }
            }
        }

        public ShaderCompiler(ProjConfig conf, string file, string shaderModelFlag, string[] includes, string[] defines, GraphicsPlatform graphicsPlatform)
        {
            string fileWithoutExt = Path.GetFileNameWithoutExtension(file);
            string outputExtension = GetOutputExtensionByGraphicsPlatform(graphicsPlatform);
            string shaderCompiler = GetShaderExecutableByGraphicsPlatform(graphicsPlatform);
            
            string outputFile = $"{fileWithoutExt}{outputExtension}";
            string outputDir = Path.Combine(Globals.ShaderAssetsDir, $"Shaders_built");
            string outputFileAbsolutePath = Path.Combine(outputDir, outputFile);
            string outputLog = Path.Combine(outputDir,  $"{fileWithoutExt}.log");
            string outputReflection = Path.Combine(outputDir, $"{fileWithoutExt}.refl");
            string outputPDB = Path.Combine(outputDir, $"{fileWithoutExt}.pdb");

            //Debug
            bool disableWarnings = false;
            bool treatWarningsAsErrors = false; // (conf.Target.Optimization == Optimization.Release);
            bool debugInfo = false; // (conf.Target.Optimization == Optimization.Debug);
            bool spirv = IsSpirvPlatform(graphicsPlatform);

            KeyInput = file;
            Output = outputFileAbsolutePath;
            Description = $"Shader Compilation step for {file}";
            Executable = shaderCompiler;
            ExecutableArguments =
                    $" -Fo {outputFileAbsolutePath}"                    //Output file (.dxc, .o etc)
                +   $" -Fre {outputReflection}"                         //DXC shader reflection file
                +   $" -Fd {outputPDB}"                                 //PDB output file

                    //Defines and includes
                +   $" -D {string.Join(" ", defines)}"        //Shader defines
                +   $" -I {string.Join(" ", includes)}"       //Shader includes
                +   $" -T {shaderModelFlag}"                            //Shader model flag
                +   (spirv ? $"-spirv " : $"")                          //Output spirv

                    //Debug
                +    $" -Fe {outputLog}"                                //Output log
                +   (treatWarningsAsErrors ? $" -WX " : $"")            //treat warnings as errors
                +   (debugInfo ? $" -Zi " : "")                         //Debug info
                +   (disableWarnings ? $"-no-warnings " : $"");         //Disabling warnings
        }
    }
}

