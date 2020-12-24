using Sharpmake;

namespace Hawl
{
    abstract class HawlLib : Project
    {
        public HawlLib()
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
            configuration.Name = @"[target.Optimization] [target.OutputType]";

            configuration.ProjectPath = @"[project.SharpmakeCsPath]/projects/[project.Name]";
            // set Languages Standard as latest
            configuration.Options.Add(Options.Vc.Compiler.CppLanguageStandard.Latest);

            configuration.Options.Add(Options.Vc.Compiler.MultiProcessorCompilation.Enable);
            // set warnning log
            configuration.Options.Add(Options.Vc.General.WarningLevel.Level4);
            configuration.Options.Add(Options.Vc.General.WindowsTargetPlatformVersion.Latest);
            configuration.Options.Add(Options.Vc.Compiler.Exceptions.Enable);
            configuration.Options.Add(Options.Vc.Compiler.FloatingPointModel.Precise);
            configuration.Options.Add(Options.Vc.General.CharacterSet.Unicode);

            if (target.Optimization == Optimization.Debug)
            {
                configuration.Defines.Add("_DEBUG");
                configuration.Options.Add(Sharpmake.Options.Vc.Compiler.MinimalRebuild.Enable);
                configuration.Options.Add(Sharpmake.Options.Vc.Compiler.RuntimeChecks.Both);
                configuration.Options.Add(Sharpmake.Options.Vc.Compiler.RuntimeLibrary.MultiThreadedDebugDLL);
                configuration.Options.Add(Sharpmake.Options.Vc.Compiler.Optimization.Disable);
                configuration.Options.Add(Sharpmake.Options.Vc.Compiler.BufferSecurityCheck.Enable);
            }

            if (target.Optimization == Optimization.Release)
            {
                configuration.Options.Add(Options.Vc.Compiler.Optimization.FullOptimization);
                configuration.Options.Add(Sharpmake.Options.Vc.General.UseDebugLibraries.Disabled);
                configuration.Options.Add(Sharpmake.Options.Vc.Compiler.RuntimeLibrary.MultiThreadedDLL);
                configuration.Options.Add(Sharpmake.Options.Vc.Compiler.BufferSecurityCheck.Disable);
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


    /// <summary>
    /// This class is used to define the targets fragments used everywhere in this sample
    /// </summary>
    public class SampleTargets
    {
        public static Target[] Targets
        {
            get
            {
                return new Target[]{ new Target(
                    Platform.win64,
                    DevEnv.vs2019,
                    Optimization.Debug | Optimization.Release,
                    buildSystem: BuildSystem.MSBuild | BuildSystem.FastBuild)};
            }
        }
    }

    /// <summary>
    /// This is the base class for projects with the Sharpmake.Export attribute.
    ///
    /// </summary>
    /// <remarks>
    /// ConfigurePriority() attribute on configure methods is to force the order of the calls. Lower numbers have highest priority
    /// </remarks>
    public class ExportProject : Project
    {
        public ExportProject()
        {
            AddTargets(SampleTargets.Targets);
        }

        [Configure(), ConfigurePriority(1)]
        public virtual void ConfigureAll(Configuration conf, Target target)
        {
        }

        [Configure(Optimization.Debug), ConfigurePriority(2)]
        public virtual void ConfigureDebug(Configuration conf, Target target)
        {
        }

        [Configure(Optimization.Release), ConfigurePriority(3)]
        public virtual void ConfigureRelease(Configuration conf, Target target)
        {
        }
    }
}