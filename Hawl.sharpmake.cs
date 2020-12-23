using System;
using Sharpmake;

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
               Optimization.Debug | Optimization.Release,
               OutputType.Dll | OutputType.Lib
           ));
        }

        [Configure()]
        public void ConfigureAll(Configuration conf, Target target)
        {
            conf.SolutionFileName = "[solution.Name]_[target.DevEnv]_[target.Platform]";
            conf.SolutionPath = @"[solution.SharpmakeCsPath]\projects";
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
