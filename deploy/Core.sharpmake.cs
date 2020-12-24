using Sharpmake;

[module: Sharpmake.Include("HawlProject.sharpmake.cs")]
[module: Sharpmake.Include("ThirdParty.sharpmake.cs")]

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
            configuration.Defines.Add("SPDLOG_WCHAR_TO_UTF8_SUPPORT");
            configuration.Defines.Add("SPDLOG_WCHAR_FILENAMES");
            configuration.IncludePaths.Add(@"[project.SourceRootPath]/interface");
            configuration.AddPublicDependency<EASTL>(target);
            configuration.AddPublicDependency<Spdlog>(target);
        }
    }
}