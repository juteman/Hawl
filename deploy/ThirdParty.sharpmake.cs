using Sharpmake;

[module: Sharpmake.Include("HawlProject.sharpmake.cs")]

namespace Hawl
{
    [Generate]
    class EASTL : ThirdProject
    {
        public EASTL()
        {
            SourceRootPath = @"[project.SharpmakeCsPath]\..\ThirdParty\EASTL\source";
        }

        [Configure]
        public void Configure(Configuration configuration, Target target)
        {
            configuration.Defines.Add("_CHAR16T");
            configuration.Defines.Add("_CRT_SECURE_NO_WARNINGS");
            configuration.Defines.Add("_SCL_SECURE_NO_WARNINGS");
            configuration.Defines.Add("EASTL_OPENSOURCE", "1");
            configuration.IncludePaths.Add(@"[project.SharpmakeCsPath]\..\ThirdParty\EASTL\test\EABase\include");
            configuration.IncludePaths.Add(@"[project.SharpmakeCsPath]\..\ThirdParty\EASTL\include");
            configuration.Output = Configuration.OutputType.Lib;
        }
    }
}