using System.IO; // For Path.Combine
using Sharpmake; // Contains the entire Sharpmake object library.

[Generate]
public class KNRShaders : Sharpmake.Project
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

    public KNRShaders()
    {
        Name = "KNRShaders";

        AddTargets(new Target(
                Platform.win32 | Platform.win64,
                DevEnv.vs2019,
                Optimization.Debug | Optimization.Release, OutputType.Lib
        ));

        SourceRootPath = Globals.GraphicsDir;
        SourceFilesExcludeRegex.AddRange(GetExcludeDirectoriesByGraphicsAPI(m_GraphicsAPI));
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
        conf.Defines.AddRange(GetDefinesByGraphicsAPI(m_GraphicsAPI));
        conf.LibraryFiles.AddRange(GetLibrariesByGraphicsAPI(m_GraphicsAPI));
        conf.IncludePaths.Add(Globals.GraphicsDir);

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

    public static void CreateShaderBuildSteps(Project project)
    {
        CreateShaderCustomBuildSteps(project, "VS.hlsl", GetShaderModelByShaderType("VS.hlsl"));
        CreateShaderCustomBuildSteps(project, "PS.hlsl", GetShaderModelByShaderType("PS.hlsl"));
        CreateShaderCustomBuildSteps(project, "CS.hlsl", GetShaderModelByShaderType("CS.hlsl"));
    }

    private string GetIncludesFromFile(string file)
    {
        
    }

    public void CreateShaderCustomBuildSteps(Project project, string filenameEnding, string shaderModelFlag)
    {
        string[] shaderFiles = new string[](project.ResolvedSourceFiles.Where(file => file.EndsWith(filenameEnding, StringComparison.InvariantCultureIgnoreCase)));
        foreach(ProjConfig conf in project.Configurations)
        {
            foreach (string shader in shaderFiles)
            {
                ShaderCompiler shaderCompiler = new ShaderCompiler(conf.Target, shader, shaderModelFlag, "", "", m_GraphicsAPI);
                conf.CustomFileBuildSteps.Add(shaderCompiler);
            }
        }
    }

    public class ShaderCompiler : Configuration.CustomFileBuildStepData
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

        private string[] GetShaderExecutableByGraphicsPlatform(GraphicsPlatform platform)
        {
            switch (platform)
            {
                case GraphicsPlatform.DIRECTX12:
                case GraphicsPlatform.DIRECTX11:
                case GraphicsPlatform.VULKAN:
                case GraphicsPlatform.OPENGL:
                    return new[] {
                        "dxc.exe",
                    };
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


        public ShaderCompiler(Target target, string file, string shaderModelFlag, Strings includes, Strings defines, GraphicsPlatform graphicsPlatform)
        {
            string fileWithoutExt = Path.GetFileNameWithoutExtension(file);
            string outputExtension = GetOutputExtensionByGraphicsPlatform(GraphicsPlatform);
            string shaderCompiler = GetShaderExecutableByGraphicsPlatform(GraphicsPlatform);
            
            string outputFile = $"{fileWithoutExt}{outputExtension}";
            string outputFileAbsolutePath = Path.Combine(outputDir, outputFile);
            string outputDir = Path.Combine(Globals.AssetsDir, $"Shaders_{target.Platform.ToString()}_{target.Optimization.ToString()}");
            string outputLog = Path.Combine(outputDir,  $"{fileWithoutExt}.log");
            string outputReflection = Path.Combine(outputDir, $"{fileWithoutExt}.refl");
            string outputPDB = Path.Combine(outputDir, $"{fileWithoutExt}.pdb");

            //Debug
            bool disableWarnings = false;
            bool treatWarningsAsErrors = (Target.Optimization.Release);
            bool debugInfo = (Target.Optimization.Debug);
            bool spirv = IsSpirvPlatform(GraphicsPlatform);

            KeyInput = file,
            Output = outputFileAbsolutePath;
            Description = $"Shader Compilation step for {file}",
            Executable = shaderCompiler;
            ExecutableArguments = 
                    $" -Fo {outputFileAbsolutePath}"                //Output file (.dxc, .o etc)
                +   $" -Fre {outputReflection}"                     //DXC shader reflection file
                +   $" -Fd {outputPDB}"                             //PDB output file

                //Defines and includes
                +   $" -D {String.Join(" ", defines.ToArray())}"    //Shader defines
                +   $" -I {String.Join(" ", includes.ToArray())}"   //Shader includes
                +   $" -T {shaderModelFlag}"                        //Shader model flag
                +   spirv ? $"-spirv " : $""                        //Output spirv

                //Debug
                +   $" -Fe {outputLog}"                             //Output log
                +   treatWarningsAsErrors ? $" -WX " : $""          //treat warnings as errors
                +   debugInfo ? $" -Zi " : ""                       //Debug info
                +   disableWarnings ? $"-no-warnings " : $"";       //Disabling warnings
        }
    }
}