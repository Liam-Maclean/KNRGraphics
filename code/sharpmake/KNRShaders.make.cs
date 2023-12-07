using System.IO; // For Path.Combine
using Sharpmake; // Contains the entire Sharpmake object library.
using System.Collections;
using System.Linq;

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

    public GraphicsPlatform m_GraphicsAPI = GraphicsPlatform.DIRECTX12;

    public string GetShaderModelByShaderType(string filenameEnding)
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

    public string[] GetIncludesFromFile(string file)
    {
         return new[] {
               "",
        }; 
    }

    public string[] GetDefines()
    {
        return new[] {
               "SM5",
        }; 
    }

    public KNRShaders()
        : base("KNRGraphics")
    {
        SourceRootPath = Globals.ShadersDir;
    }

    protected override void ExcludeOutputFiles()
    {
        base.ExcludeOutputFiles();
        CreateShaderBuildSteps(this);
    }

    [Configure]
    public void ConfigureAll(Configuration conf, Target target)
    {
        //Platform specific set-up
        conf.IncludePaths.Add(Globals.ShadersDir);

        //Project include and output
        conf.ProjectFileName = "[project.Name]";
        conf.ProjectPath = Path.Combine(Globals.BuildDir, "[project.Name]");

        conf.TargetPath = Path.Combine(Globals.BuildDir, "[project.Name]", "output", "[target.Platform]");
        conf.TargetFileName = @"KNRShaders";
        conf.Output = Configuration.OutputType.Lib;
    }

    public void CreateShaderBuildSteps(Project project)
    {
        CreateShaderCustomBuildSteps(project, "VS.hlsl", GetShaderModelByShaderType("VS.hlsl"));
        CreateShaderCustomBuildSteps(project, "PS.hlsl", GetShaderModelByShaderType("PS.hlsl"));
        CreateShaderCustomBuildSteps(project, "CS.hlsl", GetShaderModelByShaderType("CS.hlsl"));
    }

    public void CreateShaderCustomBuildSteps(Project project, string filenameEnding, string shaderModelFlag)
    {
        Strings shaderFiles = new Strings(project.ResolvedSourceFiles.Where(file => file.EndsWith(filenameEnding, System.StringComparison.InvariantCultureIgnoreCase)));
        foreach(Configuration conf in project.Configurations)
        {
            foreach (string shader in shaderFiles)
            {
                ShaderCompiler shaderCompiler = new ShaderCompiler(conf.Target, shader, shaderModelFlag, GetIncludesFromFile(shader), GetDefines(), m_GraphicsAPI);
                conf.CustomFileBuildSteps.Add(shaderCompiler);
            }
        }
    }

    public class ShaderCompiler : Configuration.CustomFileBuildStep
    {
        private string[] GetShaderDefinesByGraphicsPlatform(GraphicsPlatform platform)
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

        private string GetShaderExecutableByGraphicsPlatform(GraphicsPlatform platform)
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

        private string GetOutputExtensionByGraphicsPlatform(GraphicsPlatform platform)
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

        private bool IsSpirvPlatform(GraphicsPlatform platform)
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


        public ShaderCompiler(Target target, string file, string shaderModelFlag, string[] includes, string[] defines, GraphicsPlatform graphicsPlatform)
        {
            string fileWithoutExt = Path.GetFileNameWithoutExtension(file);
            string outputExtension = GetOutputExtensionByGraphicsPlatform(graphicsPlatform);
            string shaderCompiler = GetShaderExecutableByGraphicsPlatform(graphicsPlatform);
            
            string outputFile = $"{fileWithoutExt}{outputExtension}";
            string outputDir = Path.Combine(Globals.ShadersDir, $"Shaders_{target.Platform.ToString()}_{target.Optimization.ToString()}");
            string outputFileAbsolutePath = Path.Combine(outputDir, outputFile);
            string outputLog = Path.Combine(outputDir,  $"{fileWithoutExt}.log");
            string outputReflection = Path.Combine(outputDir, $"{fileWithoutExt}.refl");
            string outputPDB = Path.Combine(outputDir, $"{fileWithoutExt}.pdb");

            //Debug
            bool disableWarnings = false;
            bool treatWarningsAsErrors = (target.Optimization == Optimization.Release);
            bool debugInfo = (target.Optimization == Optimization.Debug);
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