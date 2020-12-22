using System;
using Sharpmake;

namespace Hawl
{
    abstract class HawlLibProject : Project
    {
        public HawlLibProject()
        {
            AddTargets(new Target(
                Platform.win64,
                DevEnv.vs2019,
                Optimization.Debug | Optimization.Release,
                OutputType.Dll | OutputType.Lib
            ));
        }
    }
}
