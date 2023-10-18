using System.IO; // For Path.Combine
using Sharpmake; // Contains the entire Sharpmake object library.

[Generate]
public class KNRLibBase : Sharpmake.Project
{
    public KNRLibBase(string projectName)
    {
        Name = projectName;

        AddTargets(new Target(
                Platform.win32 | Platform.win64,
                DevEnv.vs2019,
                Optimization.Debug | Optimization.Release, OutputType.Lib
        ));
    }

    
    public virtual void SetUpVisualStudioOptions(Configuration conf, Target target)
    {
        
        //General
        //Platform setup
        conf.Options.Add(Sharpmake.Options.Vc.General.WindowsTargetPlatformVersion.v10_0_19041_0);
        conf.Options.Add(Sharpmake.Options.Vc.General.PlatformToolset.v142);
        conf.Options.Add(Sharpmake.Options.Vc.General.PreferredToolArchitecture.x64);

        //Unicode character set
        conf.Options.Add(Sharpmake.Options.Vc.General.CharacterSet.Unicode);

        //Warnings
        //conf.Options.Add(Sharpmake.Options.Vc.General.TreatWarningsAsErrors.Enable);
        conf.Options.Add(Sharpmake.Options.Vc.General.WarningLevel.Level4);

        //Compiler
        conf.Options.Add(Sharpmake.Options.Vc.Compiler.CppLanguageStandard.CPP17);
        conf.Options.Add(Sharpmake.Options.Vc.Compiler.FavorSizeOrSpeed.FastCode);
        conf.Options.Add(Sharpmake.Options.Vc.Compiler.MultiProcessorCompilation.Enable);
        conf.Options.Add(Sharpmake.Options.Vc.Compiler.Optimization.FullOptimization);
        conf.Options.Add(Sharpmake.Options.Vc.Compiler.RTTI.Enable);
    }
}