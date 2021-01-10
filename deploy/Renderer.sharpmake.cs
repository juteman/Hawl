using Sharpmake;

[module: Sharpmake.Include("HawlProject.sharpmake.cs")]
[module: Sharpmake.Include("Core.sharpmake.cs")]

namespace Hawl
{
    [Generate]
    class RendererLib : HawlProject
    {
        public RendererLib()
        {
            Name = "Renderer";
            SourceRootPath = @"[project.SharpmakeCsPath]\..\Renderer\";
        }

        public override void ConfigureAll(Configuration configuration, Target target)
        {
            base.ConfigureAll(configuration, target);
            configuration.ProjectPath = @"[project.SharpmakeCsPath]\..\[project.Name]";
            configuration.Defines.Add("D3D12_SUPPORTED");
            
            configuration.IncludePaths.Add(@"[project.SourceRootPath]\interface");
            configuration.IncludePrivatePaths.Add(@"[project.SourceRootPath]\..\ThirdParty\nvapi");
            configuration.IncludePrivatePaths.Add(@"[project.SourceRootPath]\..\ThirdParty\AGS_SDK\ags_lib\inc");
            configuration.LibraryPaths.Add(@"[project.SourceRootPath]\..\ThirdParty\nvapi\amd64");
            configuration.LibraryPaths.Add(@"[project.SourceRootPath]\..\ThirdParty\AGS_SDK\ags_lib\lib");
            configuration.LibraryFiles.Add("nvapi64.lib");
            configuration.LibraryFiles.Add("amd_ags_x64.lib");
            configuration.Output = Configuration.OutputType.Lib;
            configuration.AddPublicDependency<CoreLib>(target);
            configuration.AddPublicDependency<D3D12MemoryAllocator>(target);
        }

        public override void ConfigureDebug(Configuration conf, Target target)
        {
            base.ConfigureDebug(conf, target);
            conf.Defines.Add("GRAPHICS_DEBUG");
            conf.Defines.Add("EA_DEBUG");
        }
    }
}