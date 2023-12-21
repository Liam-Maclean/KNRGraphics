using System.IO; // For Path.Combine
using Sharpmake; // Contains the entire Sharpmake object library.
using System.Collections;
using System.Linq;
using ProjConfig = Sharpmake.Project.Configuration;

[Generate]
public class KNRShaderCompiler : KNRExeBase
{
    public KNRShaderCompiler()
        : base("KNRShaderCompiler")
    {
        SourceRootPath = Globals.ShaderCompilerDir;
        SourceFilesFiltersRegex.AddRange(Globals.SourceIncludeFileExtensions);
    }

    [Configure]
    public void ConfigureAll(ProjConfig conf, Target target)
    {
        //Platform specific set-up
        conf.IncludePaths.Add(Globals.ShaderCompilerDir);

        //Include Bison and Flex
        conf.IncludePaths.Add(Path.Combine(Globals.ExternalDir, "winflexbison"));

        //Project include and output
        conf.ProjectFileName = "[project.Name]";
        conf.ProjectPath = Path.Combine(Globals.BuildDir, "[project.Name]");

        conf.TargetPath = Path.Combine(Globals.BuildDir, "[project.Name]", "output", "[target.Platform]");
        conf.TargetFileName = @"KNRShaderCompiler";
        //conf.Output = Configuration.OutputType.Exe;
    }
}

