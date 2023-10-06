using System.IO; // For Path.Combine
using Sharpmake;

public static class Globals
{
    //Root directories
    public static readonly string ProjectRoot = @"S:\github\KNR\KNRGraphics";
    public static readonly string CodeRoot = Path.Combine(ProjectRoot, @"code");
    public static readonly string BuildDir = Path.Combine(ProjectRoot, @"build");

    //Code directories
    public static readonly string EngineDir = Path.Combine(CodeRoot, @"engine");

    //Engine directories
    public static readonly string GraphicsDir = Path.Combine(EngineDir, @"graphics");
    public static readonly string CoreDir = Path.Combine(EngineDir, @"core");
}