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
                OutputType.Dll
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
            configuration.Output = Configuration.OutputType.Lib;
        }
    }


    [Generate]
    class Spdlog : ThirdProject
    {
        public Spdlog()
        {
            Name = "Spdlog";
            SourceRootPath = @"[project.SharpmakeCsPath]\..\ThirdParty\D3D12MemoryAllocator";
            AddTargets(new Target(
                Platform.win64,
                DevEnv.vs2019,
                Optimization.Debug | Optimization.Release,
                OutputType.Dll
            ));
        }

        [Configure()]
        public void Configure(Configuration configuration, Target target)
        {

            configuration.IncludePaths.Add(@"[project.SharpmakeCsPath]\..\ThirdParty\spdlog\include");
            configuration.Output = Configuration.OutputType.Lib;

        }
    }

    [Generate]
    class D3D12MemoryAllocator : ThirdProject
    {
        public D3D12MemoryAllocator()
        {
            Name = "D3D12MemoryAllocator";
            SourceRootPath = @"[project.SharpmakeCsPath]\..\ThirdParty\D3D12MemoryAllocator";
            AddTargets(new Target(
                Platform.win64,
                DevEnv.vs2019,
                Optimization.Debug | Optimization.Release,
                OutputType.Dll
            ));
        }

        [Configure()]
        public void Configure(Configuration configuration, Target target)
        {
            configuration.IncludePaths.Add(@"[project.SharpmakeCsPath]\..\ThirdParty\D3D12MemoryAllocator"); 
          
            configuration.Output = Configuration.OutputType.Lib;
            
        }
    }

    [Generate]
    class Tbb : ThirdProject
    {
        public Tbb()
        {
            Name = "OneTbb";
            SourceRootPath = @"[project.SharpmakeCsPath]\..\ThirdParty\oneTbb\src\tbb";
            AddTargets(new Target(
                Platform.win64,
                DevEnv.vs2019,
                Optimization.Debug | Optimization.Release,
                OutputType.Dll
            ));
        }

        [Configure()]
        public void Configure(Configuration configuration, Target target)
        {
            configuration.IncludePaths.Add(@"[project.SharpmakeCsPath]\..\ThirdParty\oneTbb\include");
            configuration.Output = Configuration.OutputType.Lib;
            configuration.Defines.Add("__TBB_BUILD");
        }
    }
}