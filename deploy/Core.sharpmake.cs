using Sharpmake;

[module: Sharpmake.Include("HawlProject.sharpmake.cs")]
[module: Sharpmake.Include("ThirdParty.sharpmake.cs")]

namespace Hawl
{

    [Generate]
    class CoreLib : HawlProject
    {
        public  CoreLib()
        {
            Name = "Core";
            SourceRootPath = @"[project.SharpmakeCsPath]\..\Core";
        }

        public override void ConfigureAll(Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);
            conf.ProjectPath = @"[project.SharpmakeCsPath]\..\[project.Name]";
            conf.Output = Configuration.OutputType.Lib;
            conf.Options.Add(Options.Vc.Compiler.Exceptions.Enable);
            conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\..\Core\interface");
			conf.AddPublicDependency<EASTL>(target);
            conf.AddPublicDependency<EAAssert>(target);
			conf.AddPublicDependency<Spdlog>(target);
            conf.AddPublicDependency<Tbb>(target);
            conf.AddPublicDependency<MiMalloc>(target);
        }

        public override void ConfigureDebug(Configuration conf, Target target)
        {
            base.ConfigureDebug(conf, target);
            conf.Defines.Add("EA_DEBUG");
        }   
    }
}