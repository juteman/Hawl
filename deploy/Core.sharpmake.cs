using Sharpmake;

[module: Sharpmake.Include("HawlProject.sharpmake.cs")]

namespace Hawl
{

    [Generate]
    class CoreLib : HawlLib
    {
        public  CoreLib()
        {
            Name = "CoreLib";
            SourceRootPath = @"[project.SharpmakeCsPath]/../Core";
        }

        public override void Configure(Configuration configuration, Target target)
        {
            base.Configure(configuration, target);
            configuration.ProjectFileName = @"[project.Name]_[target.DevEnv]_[target.Platform]";
            configuration.IncludePaths.Add(@"[project.SourceRootPath]/interface");
        }
    }
}