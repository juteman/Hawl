using Sharpmake;

[module: Sharpmake.Include("Lib.sharpmake.cs")]

namespace Hawl
{

    [Generate]
    class CoreLib : LibProject
    {
        public  CoreLib()
        {
            Name = "CoreLib";
            SourceRootPath = @"[project.SharpmakeCsPath]/Core/src";
        }

        public override void Configure(Configuration configuration, Target target)
        {
            configuration.IncludePaths.Add(@"[project.SourceRootPath]/../interface");
        }
    }
}