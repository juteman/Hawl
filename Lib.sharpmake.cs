using Sharpmake;

namespace Hawl
{
    abstract class LibProject : Project
    {
        public LibProject()
        {
            IsFileNameToLower = false;
            IsTargetFileNameToLower = false;
            AddTargets(new Target(
                Platform.win64,
                DevEnv.vs2019,
                Optimization.Debug | Optimization.Release,
                OutputType.Dll | OutputType.Lib
            ));
        }

        [Configure]
        public virtual void Configure(Configuration configuration, Target target)
        {
            // Configuration Project name and path setting
            configuration.ProjectFileName = "[project.Name]_[target.DevEnv]_[target.Platform]";
            configuration.ProjectPath = @"[project.SharpmakeCsPath]\projects";
            // set Languages Standard as latest
            configuration.Options.Add(Options.Vc.Compiler.CppLanguageStandard.Latest);
            configuration.Options.Add(Options.Vc.Compiler.MultiProcessorCompilation.Enable);
            // set warnning log
            configuration.Options.Add(Options.Vc.General.WarningLevel.Level4);
            configuration.Options.Add(Options.Vc.Compiler.Exceptions.Enable);
            configuration.Options.Add(Options.Vc.Compiler.FloatingPointModel.Precise);
            configuration.TargetLibraryPath = @"[project.SharpmakeCsPath]/lib";

            if (target.Optimization == Optimization.Debug)
            {
                configuration.Options.Add(Options.Vc.General.WholeProgramOptimization.Disable);
            }

            if (target.Optimization == Optimization.Release)
            {
                configuration.Options.Add(Options.Vc.General.WholeProgramOptimization.Optimize);
            }

            if (target.OutputType == OutputType.Dll)
            {
                // if output as shared library
                configuration.Output = Configuration.OutputType.Dll;

                // expects HAWL_DLL symbol to be defined when use as DLL
                configuration.ExportDefines.Add("HAWL_DLL");

                // add define micro.
                configuration.Defines.Add("HAWL_DLL");
            }

            if (target.OutputType == OutputType.Lib)
            {
                configuration.Output = Configuration.OutputType.Lib;
            }
        }
    }
}