using Sharpmake;

namespace Hawl
{
    public class HawlProject : Project
    {
        public HawlProject()
        {
            IsFileNameToLower = false;
            IsTargetFileNameToLower = false;
            AddTargets(new Target(
                Platform.win64,
                DevEnv.vs2019,
                Optimization.Debug | Optimization.Release
            ));
        }

        [Configure(), ConfigurePriority(1)]
        public virtual void ConfigureAll(Configuration conf, Target target)
        {
            // Configuration Project name and path setting
            conf.Name = @"[target.Optimization]";
            conf.TargetFileName = Name;
            conf.ProjectPath = @"[project.SharpmakeCsPath]\projects\[project.Name]";
            // set Languages Standard as latest
            conf.Options.Add(Options.Vc.Compiler.CppLanguageStandard.Latest);

            conf.Options.Add(Options.Vc.Compiler.MultiProcessorCompilation.Enable);
            // set warnning log
            conf.Options.Add(Options.Vc.General.WarningLevel.Level4);
            conf.Options.Add(Options.Vc.General.WindowsTargetPlatformVersion.Latest);
            conf.Options.Add(Options.Vc.Compiler.Exceptions.Enable);
            conf.Options.Add(Options.Vc.General.CharacterSet.Unicode);
        }

        [Configure(Optimization.Debug), ConfigurePriority(2)]
        public virtual void ConfigureDebug(Configuration conf, Target target)
        {
            conf.Options.Add(Sharpmake.Options.Vc.Compiler.Inline.Disable);
            conf.Defines.Add("_DEBUG");
            conf.Defines.Add("DEBUG");
            conf.Options.Add(Sharpmake.Options.Vc.Compiler.Optimization.Disable);
            conf.Options.Add(Sharpmake.Options.Vc.Compiler.BufferSecurityCheck.Enable);
        }

        [Configure(Optimization.Release), ConfigurePriority(3)]
        public virtual void ConfigureRelease(Configuration conf, Target target)
        {
            conf.Options.Add(Sharpmake.Options.Vc.Compiler.Optimization.MaximizeSpeed);
            conf.Options.Add(Sharpmake.Options.Vc.General.WholeProgramOptimization.LinkTime);
            conf.Options.Add(Sharpmake.Options.Vc.Linker.LinkTimeCodeGeneration.UseLinkTimeCodeGeneration);
            conf.Options.Add(Sharpmake.Options.Vc.Linker.EnableCOMDATFolding.RemoveRedundantCOMDATs);
            conf.Options.Add(Sharpmake.Options.Vc.Linker.Reference.EliminateUnreferencedData);
            conf.Options.Add(Sharpmake.Options.Vc.General.UseDebugLibraries.Disabled);
            conf.Options.Add(Sharpmake.Options.Vc.Compiler.BufferSecurityCheck.Disable);
        }
    }

    abstract public class ThirdProject : Project
    {
        public ThirdProject()
        {
            IsFileNameToLower = false;
            IsTargetFileNameToLower = false;
            AddTargets(new Target(
                    Platform.win64,
                    DevEnv.vs2019,
                    Optimization.Debug | Optimization.Release));
        }

        [Configure(), ConfigurePriority(1)]
        public virtual void ConfigureAll(Configuration configuration, Target target)
        {
        }

        [Configure(Optimization.Debug), ConfigurePriority(2)]
        public virtual void ConfigureDebug(Configuration configuration, Target target)
        {
        }

        [Configure(Optimization.Release), ConfigurePriority(3)]
        public virtual void ConfigureRelease(Configuration configuration, Target target)
        {
        }
    }
}




