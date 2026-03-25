@echo off

set program=ember.exe
set sources=./engine/build_engine.c
set depends=user32.lib shell32.lib vulkan-1.lib

:::::::: DEBUG CONFIGURATION ::::::::
set bin_dir_dbg=.\bin\dbg\
set lnk_dir_dbg=.\obj\dbg\
set defines_dbg=/DBUILD_DEBUG=1 /DEMBER_PROFILING_ENABLED=1 /DEMBER_BUILD_CONSOLE=1 /DPLATFORM_WINDOWS=1 /DEMBER_ASSERT_ENABLED=1 /DRHI_VULKAN=1 /DRHI_VK_VALIDATIONS_ENABLED=1
set cmp_opt_dbg=/W4 /Od /wd4996 /Zi /Fe%bin_dir_dbg%%program% /Fo:%lnk_dir_dbg% /I%VULKAN_SDK%\Include %defines_dbg%
set lnk_opt_dbg=/link /SUBSYSTEM:WINDOWS /INCREMENTAL:NO /LIBPATH:%VULKAN_SDK%\Lib

:::::::: RELEASE CONFIGURATION ::::::::
set bin_dir_rel=.\bin\rel\
set lnk_dir_rel=.\obj\rel\
set defines_rel=/DBUILD_DEBUG=0 /DEMBER_PROFILING_ENABLED=0 /DEMBER_BUILD_CONSOLE=0 /DPLATFORM_WINDOWS=1 /DEMBER_ASSERT_ENABLED=0 /DRHI_VULKAN=1 /DRHI_VK_VALIDATIONS_ENABLED=0
set cmp_opt_rel=/W4 /02 /wd4996 /Fe%bin_dir_rel%%program% /Fo:%lnk_dir_rel% /I%VULKAN_SDK%\Include %defines_rel%
set lnk_opt_rel=/link /SUBSYSTEM:WINDOWS /INCREMENTAL:NO /LIBPATH:%VULKAN_SDK%\Lib

if /i "%~1"=="release" (
    if not exist %bin_dir_rel% mkdir %bin_dir_rel%
    if not exist %lnk_dir_rel% mkdir %lnk_dir_rel%

    echo Building in RELEASE mode
    cl %cmp_opt_rel% %sources% %depends% %lnk_opt_rel%
) else (
    if not exist %bin_dir_dbg% mkdir %bin_dir_dbg%
    if not exist %lnk_dir_dbg% mkdir %lnk_dir_dbg%

    echo Building in DEBUG mode
    cl %cmp_opt_dbg% %sources% %depends% %lnk_opt_dbg%
)

call compile_shaders.bat
