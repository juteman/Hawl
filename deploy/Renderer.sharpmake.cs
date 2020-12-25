using Sharpmake;

[module: Sharpmake.Include("HawlProject.sharpmake.cs")]
[module: Sharpmake.Include("Core.sharpmake.cs")]

namespace Hawl
{
    [Generate]
    class RendererLib : HawlLib
    {
        public RendererLib()
        {
            Name = "RendererLib";
            SourceRootPath = @"[project.SharpmakeCsPath]\..\Renderer\";
        }

        public override void Configure(Configuration configuration, Target target)
        {
            base.Configure(configuration, target);
            configuration.IncludePaths.Add(@"[project.SourceRootPath]\interface");
            configuration.IncludePrivatePaths.Add(@"[project.SourceRootPath]\..\ThirdParty\nvapi");
            configuration.IncludePrivatePaths.Add(@"[project.SourceRootPath]\..\ThirdParty\AGS_SDK\ags_lib\inc");

            configuration.LibraryPaths.Add(@"[project.SourceRootPath]\..\ThirdParty\nvapi\amd64");
            configuration.LibraryPaths.Add(@"[project.SourceRootPath]\..\ThirdParty\AGS_SDK\ags_lib\lib");

            configuration.LibraryFiles.Add("nvapi64");
            configuration.LibraryFiles.Add("amd_ags_x64");
            configuration.AddPublicDependency<CoreLib>(target);
        }
    }
}