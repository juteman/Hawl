using System;
using Sharpmake;

[module: Sharpmake.Include("ThirdParty.sharpmake.cs")]
[module: Sharpmake.Include("Core.sharpmake.cs")]
[module: Sharpmake.Include("Renderer.sharpmake.cs")]

namespace Hawl
{
    [Sharpmake.Generate]
    public class HawlSolution : Sharpmake.Solution
    {
        public HawlSolution()
        {
            Name = "Hawl";

            AddTargets(new Target(
               Platform.win64,
               DevEnv.vs2019,
               Optimization.Debug | Optimization.Release
           ));

            ExtraItems.Add("Markdown", new Strings()
            {
                @"[solution.SharpmakeCsPath]/../README.md",
            });
        }

        [Configure()]
        public void ConfigureAll(Configuration conf, Target target)
        {
            conf.Name = @"[target.Optimization]_[target.OutputType]";
            conf.SolutionPath = @"[solution.SharpmakeCsPath]\projects";
            conf.Options.Add(Options.Vc.General.WindowsTargetPlatformVersion.Latest);
            conf.AddProject<CoreLib>(target);
            conf.AddProject<RendererLib>(target);
        }

        [Sharpmake.Main]
        public static void SharpmakeMain(Sharpmake.Arguments arguments)
        {
            arguments.Generate<HawlSolution>();
        }
    }
}
