using Sharpmake;

[module: Sharpmake.Include("HawlProject.sharpmake.cs")]

namespace Hawl
{
    [Generate]
    class EASTL : ThirdProject
    {
        public EASTL()
        {
            Name = "EASTL";
            SourceRootPath = @"[project.SharpmakeCsPath]\..\ThirdParty\EASTL\source";
            AddTargets(new Target(
                Platform.win64,
                DevEnv.vs2019,
                Optimization.Debug | Optimization.Release,
                OutputType.Dll | OutputType.Lib
            ));
        }

        [Configure()]
        public void Configure(Configuration configuration, Target target)
        {
            configuration.Defines.Add("_CHAR16T");
            configuration.Defines.Add("_CRT_SECURE_NO_WARNINGS");
            configuration.Defines.Add("_SCL_SECURE_NO_WARNINGS");
            configuration.Defines.Add("EASTL_OPENSOURCE", "1");
            configuration.IncludePaths.Add(@"[project.SharpmakeCsPath]\..\ThirdParty\EASTL\test\packages\EABase\include\Common");
            configuration.IncludePaths.Add(@"[project.SharpmakeCsPath]\..\ThirdParty\EASTL\include");
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


    [Generate]
    class Spdlog : ThirdProject
    {
        public Spdlog()
        {
            Name = "Spdlog";
            AddTargets(new Target(
                Platform.win64,
                DevEnv.vs2019,
                Optimization.Debug | Optimization.Release,
                OutputType.Dll | OutputType.Lib
            ));
        }

        [Configure()]
        public void Configure(Configuration configuration, Target target)
        {
            
            configuration.IncludePaths.Add(@"[project.SharpmakeCsPath]\..\ThirdParty\spdlog\include");
            if (target.OutputType == OutputType.Dll)
            {
                // if output as shared library
                configuration.Output = Configuration.OutputType.Dll;


                // add define micro.
                //configuration.Defines.Add("SPDLOG_SHARED_LIB");
            }

            if (target.OutputType == OutputType.Lib)
            {
                configuration.Output = Configuration.OutputType.Lib;
            }
        }
    }
}