using Sharpmake;

[module: Sharpmake.Include("HawlProject.sharpmake.cs")]

namespace Hawl
{
    // Add a dependency to this project to be able to use vcpkg packages in a project.
    //
    // This project then setup the necessary include and library paths to be able to reference any installed vcpkg package in
    // our local vcpackage installation.
    //
    // Note: The required vcpkg packages are installed by bootstrap-sample.bat
    //
    [Export]
    public class VCPKG : ThirdProject
    {
        public override void ConfigureRelease(Configuration conf, Target target)
        {
            base.ConfigureRelease(conf, target);

            // Add root include path for vcpkg packages.
            conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\..\vcpkg\installed\x64-windows\include");

            // Add root lib path for vcpkg packages.
            conf.LibraryPaths.Add(@"[project.SharpmakeCsPath]\..\vcpkg\installed\x64-windows\lib");
        }

        public override void ConfigureDebug(Configuration conf, Target target)
        {
            base.ConfigureDebug(conf, target);

            // Add root include path for vcpkg packages.
            conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\..\vcpkg\installed\x64-windows\include");

            // Add root lib path for vcpkg packages.
            conf.LibraryPaths.Add(@"[project.SharpmakeCsPath]\..\vcpkg\installed\x64-windows\debug\lib");
        }
    }

    [Export]
    class EASTL: VCPKG
    {
        public override void ConfigureAll(Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);
        }

        public override void ConfigureDebug(Configuration conf, Target target)
        {
            base.ConfigureDebug(conf, target);
            conf.LibraryFiles.Add("EASTL.lib");
        }

        public override void ConfigureRelease(Configuration conf, Target target)
        {
            base.ConfigureRelease(conf, target);
            conf.LibraryFiles.Add("EASTL.lib");
        }
    }



    [Export]
    class MiMalloc: VCPKG
    {
        public override void ConfigureAll(Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);
        }

        public override void ConfigureDebug(Configuration conf, Target target)
        {
            base.ConfigureDebug(conf, target);
            conf.LibraryFiles.Add("mimalloc-debug.lib");
        }

        public override void ConfigureRelease(Configuration conf, Target target)
        {
            base.ConfigureRelease(conf, target);
            conf.LibraryFiles.Add("mimalloc.lib");
        }
    }

    [Export]
    class Spdlog : VCPKG
    {
        public override void ConfigureAll(Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);
        }

        public override void ConfigureDebug(Configuration conf, Target target)
        {
            base.ConfigureDebug(conf, target);
            conf.LibraryFiles.Add(@"fmtd.lib");
            conf.LibraryFiles.Add(@"spdlogd.lib");
        }

        public override void ConfigureRelease(Configuration conf, Target target)
        {
            base.ConfigureRelease(conf, target);
            conf.LibraryFiles.Add(@"fmt.lib");
            conf.LibraryFiles.Add(@"spdlog.lib");
        }
    }

    [Generate]
    class D3D12MemoryAllocator : HawlProject
    {
        public D3D12MemoryAllocator()
        {
            Name = "D3D12MemoryAllocator";
            SourceRootPath = @"[project.SharpmakeCsPath]\..\ThirdParty\D3D12MemoryAllocator";
        }

        public override void ConfigureAll(Configuration configuration, Target target)
        {
            base.ConfigureAll(configuration, target);
            configuration.IncludePaths.Add(@"[project.SharpmakeCsPath]\..\ThirdParty\D3D12MemoryAllocator");
            configuration.Output = Configuration.OutputType.Lib;
        }
    }

    [Export]
    class Tbb : VCPKG
    {
        public override void ConfigureAll(Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);
        }

        public override void ConfigureDebug(Configuration conf, Target target)
        {
            base.ConfigureDebug(conf, target);
            conf.LibraryFiles.Add(@"tbb_debug.lib");
            conf.LibraryFiles.Add(@"tbbmalloc_debug.lib");
            conf.LibraryFiles.Add(@"tbbmalloc_proxy_debug");
        }

        public override void ConfigureRelease(Configuration conf, Target target)
        {
            base.ConfigureRelease(conf, target);
            conf.LibraryFiles.Add(@"tbb.lib");
            conf.LibraryFiles.Add(@"tbbmalloc.lib");
            conf.LibraryFiles.Add(@"tbbmalloc_proxy");
        }
    }
}