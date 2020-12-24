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
            SourceRootPath = @"[project.SharpmakeCsPath]/../Renderer/";
        }

        public override void Configure(Configuration configuration, Target target)
        {
            base.Configure(configuration, target);
            configuration.IncludePaths.Add(@"[project.SourceRootPath]/interface");
            configuration.AddPublicDependency<CoreLib>(target);
        }
    }
}