:: This script returns the machine dependent compile options needed
:: to compile and link applications using the ROOT libraries.

:: Author: Bertrand Bellenot, 28/10/2020

@echo off
setlocal EnableDelayedExpansion

set "usage="
for %%u in (
   "Usage: root-config [--prefix] [--version] [--cflags] [--libs] [--glibs]"
   " [--evelibs] [--bindir] [--libdir] [--incdir] [--etcdir] [--tutdir]"
   " [--has-<feature>] [--arch] [--platform] [--config] [--features] [--ncpu]"
   " [--git-revision] [--python-version] [--python3-version]"
   " [--cc] [--cxx] [--cxxstandard] [--f77] [--ld ] [--help]"
) do set usage=!usage!%%~u^


if "%1"=="" (
   echo !usage! 1>&2
   exit /b 1
)

for %%i in ("%~dp0..") do set "folder=%%~fi"

set ROOTSYS=%folder%
set prefix=%ROOTSYS%
set arch=win64
set platform=Windows-10.0.26200
set bindir=%ROOTSYS%\bin
set libdir=%ROOTSYS%\lib
set incdir=%ROOTSYS%\include
set etcdir=%ROOTSYS%\etc
set tutdir=%ROOTSYS%\tutorials
set all_features= arrow asimage asimage_tiff asserts builtin_cfitsio builtin_clang builtin_cling builtin_civetweb builtin_cppzmq builtin_fftw3 builtin_freetype builtin_ftgl builtin_gif builtin_gl2ps builtin_gsl builtin_gtest builtin_jpeg builtin_llvm builtin_lz4 builtin_lzma builtin_nlohmannjson builtin_openssl builtin_openui5 builtin_pcre builtin_png builtin_tbb builtin_unuran builtin_vdt builtin_xrootd builtin_xxhash builtin_zeromq builtin_zlib builtin_zstd ccache cefweb check_connection clad cocoa coverage cuda curl daos dataframe davix dcache dev distcc experimental_adaptivecpp experimental_genvectorx fcgi fftw3 fitsio fortran gdml geom geombuilder gnuinstall gviz http imt libcxx llvm13_broken_tests macos_native mathmore memory_termination minuit2_mpi minuit2_omp mpi opengl pyroot pythia8 qt6web r roofit roofit_multiprocess root7 runtime_cxxmodules shadowpw shared soversion spectrum sqlite ssl test_distrdf_dask test_distrdf_pyspark testsupport thisroot_scripts tmva tmva-cpu tmva-cudnn tmva-gpu tmva-pymva tmva-rmva tmva-sofie tpython unfold unuran uring use_gsl_cblas vdt vecgeom webgui win_broken_tests winrtdebug x11 xml xrootd
set features=cxx20  asimage builtin_cfitsio builtin_clang builtin_cling builtin_civetweb builtin_freetype builtin_ftgl builtin_gif builtin_gl2ps builtin_gtest builtin_jpeg builtin_llvm builtin_lz4 builtin_lzma builtin_nlohmannjson builtin_openui5 builtin_pcre builtin_png builtin_tbb builtin_unuran builtin_xxhash builtin_zlib builtin_zstd clad curl dataframe fftw3 fitsio gdml geom http imt mathmore opengl pyroot roofit root7 shared spectrum sqlite testsupport thisroot_scripts tmva tmva-cpu tpython unfold unuran use_gsl_cblas webgui xml
set configargs="CLANG_EXECUTABLE_VERSION=20 CMAKE_CXX_STANDARD_LIBRARIES=kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib CMAKE_C_STANDARD_LIBRARIES=kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib FFTW_INCLUDE_DIR=C:/libs/x64/fftw/3.3.5/include FFTW_LIBRARY=C:/libs/x64/fftw/3.3.5/lib/fftw3-3.lib GSL_CBLAS_LIBRARY=C:/libs/x64/GSL/2.7/lib/Release/gslcblas.lib GSL_CBLAS_LIBRARY_DEBUG=C:/libs/x64/GSL/2.7/lib/Debug/gslcblas.lib GSL_INCLUDE_DIR=C:/libs/x64/GSL/2.7/include GSL_LIBRARY=C:/libs/x64/GSL/2.7/lib/Release/gsl.lib GSL_LIBRARY_DEBUG=C:/libs/x64/GSL/2.7/lib/Debug/gsl.lib LIBCLANG_LIBRARY_VERSION=20 LIBXML2_INCLUDE_DIR=C:/libs/x64/libxml2/2.7.8/include LIBXML2_LIBRARY=C:/libs/x64/libxml2/2.7.8/lib/libxml2.lib LLVM_INCLUDE_BENCHMARKS=OFF LLVM_INCLUDE_EXAMPLES=OFF LLVM_INCLUDE_RUNTIMES=ON LLVM_INCLUDE_TOOLS=ON LLVM_INCLUDE_UTILS=ON LLVM_VERSION_PRINTER_SHOW_BUILD_CONFIG=ON LLVM_VERSION_PRINTER_SHOW_HOST_TARGET_INFO=ON OPENGL_gl_LIBRARY=opengl32 OPENGL_glu_LIBRARY=glu32 PCRE_INCLUDE_DIR=C:/ROOT-CI/build/builtins/pcre/PCRE-prefix/src/PCRE-build PCRE_LIBRARIES=C:/ROOT-CI/build/builtins/pcre/PCRE-prefix/src/PCRE-build/Release/pcre.lib PCRE_PCRE_LIBRARY=C:/ROOT-CI/build/builtins/pcre/PCRE-prefix/src/PCRE-build/Release/pcre.lib PCRE_VERSION=8.43 SQLITE_INCLUDE_DIR=C:/libs/x64/sqlite3/3.45.2 SQLITE_LIBRARIES=C:/libs/x64/sqlite3/3.45.2/sqlite3.lib ZSTD_VERSION=1.5.7 xxHash_INCLUDE_DIR=C:/ROOT-CI/src/builtins/xxhash xxHash_INCLUDE_DIRS=C:/ROOT-CI/src/builtins/xxhash xxHash_LIBRARIES=xxHash::xxHash xxHash_LIBRARY=$<TARGET_FILE:xxhash> xxHash_VERSION=0.8.0 xxHash_VERSION_STRING=0.8.0 "
set altcc=cl.exe
set altcxx=cl.exe
set altf77=CMAKE_Fortran_COMPILER-NOTFOUND
set altld=cl.exe

set srcdir=C:/ROOT-CI/src
rem convert forward slashes to backslashes
set srcdir=!srcdir:/=\!

set cflags=-nologo  -FIw32pragma.h -FIsehmap.h -GR -MD -EHsc -wd4267 -DNOMINMAX 
set cxxflags=-nologo  -Zc:__cplusplus -std:c++20 -GR -FIw32pragma.h -FIsehmap.h -MD -EHsc -wd4141 -wd4291 -wd4244 -wd4049 -wd4146 -wd4250 -wd4624 -wd4267 -D_XKEYCHECK_H -DNOMINMAX -D_CRT_SECURE_NO_WARNINGS -D_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING -D_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING 
if "-MD"=="-MDd" (
    set cflags=!cflags! -Od -Z7
    set cxxflags=!cxxflags! -Od -Z7
) else (
    set cflags=!cflags! -O2
    set cxxflags=!cxxflags! -O2
)
rem add double-quotes to allow PowerShell usage like: Invoke-Expression "cl test.cpp $(root-config --cflags --libs)"
set cxxflags=!cxxflags:-FIw32pragma.h=-FI"w32pragma.h"!
set cxxflags=!cxxflags:-FIsehmap.h=-FI"sehmap.h"!

set rootglibs=libGui.lib
set rootevelibs=libEve.lib libEG.lib libGeom.lib libGed.lib libRGL.lib
set rootlibs=libCore.lib libImt.lib libRIO.lib libNet.lib libHist.lib libGraf.lib libGraf3d.lib libGpad.lib libROOTVecOps.lib libTree.lib libTreePlayer.lib libRint.lib libPostscript.lib libMatrix.lib libPhysics.lib libMathCore.lib libThread.lib

set out=
set err=0

for %%w in (%*) do (
   set arg=%%w
   if "!arg!"=="--arch" (
      rem Output the architecture
      set out=!out! !arch!
   )
   if "!arg!"=="--platform" (
      rem Output the platform (OS)
      set out=!out! !platform!
   )
   if "!arg:~0,6!"=="--has-" (
      rem Check for feature
      set feature=!arg:~6!
      set res=""
      set known=""
      for %%f in (%features%) do (
         if %%f==!feature! (
            set res=%%f
            if not "!out!"=="" (
               set out=!out! yes
            ) else (
               set out=yes
            )
         )
      )
      if !res!=="" (
         for %%f in (%all_features%) do (
            if %%f==!feature! (
               set known=%%f
            )
         )
         if !known!=="" (
            set out=!out! --has-!feature!: unknown feature^^!
            set err=1
         ) else (
            if not "!out!"=="" (
               set out=!out! no
            ) else (
               set out=no
            )
         )
      )
   )
   if "!arg!"=="--prefix" (
      rem Output the prefix
      set out=!out! !prefix!
   )
   if "!arg!"=="--version" (
      set out=!out! 6.40.02
   )
   if "!arg!"=="--git-revision" (
      if exist !etcdir!\gitinfo.txt (
         for /f "skip=1" %%l in (!etcdir!\gitinfo.txt) do (
            set ROOT_GIT_COMMIT=%%~l
            goto COMMITSET
         )
      ) else (
         echo "cannot read !etcdir!\gitinfo.txt"
         exit /b 1
      )
      :COMMITSET
      set out=!out! !ROOT_GIT_COMMIT!
   )
   if "!arg!"=="--python-version" (
      set out=!out! 3.14.2
   )
   if "!arg!"=="--python3-version" (
      set out=!$out! 3.14.2
   )
   if "!arg!"=="--cxxstandard" (
      set out=!out! 20
   )
   if "!arg!"=="--cflags" (
      set out=!out! !cxxflags! -I!incdir!
   )
   if "!arg!"=="--libs" (
      set out=!out! /link -LIBPATH:!libdir! !rootlibs!
   )
   if "!arg!"=="--glibs" (
      set out=!out! /link -LIBPATH:!libdir! !rootlibs! !rootglibs!
   )
   if "!arg!"=="--evelibs" (
      set out=!out! /link -LIBPATH:!libdir! !rootlibs! !rootglibs! !rootevelibs!
   )
   if "!arg!"=="--bindir" (
      set out=!out! !bindir!
   )
   if "!arg!"=="--libdir" (
      set out=!out! !libdir!
   )
   if "!arg!"=="--incdir" (
      set out=!out! !incdir!
   )
   if "!arg!"=="--etcdir" (
      set out=!out! !etcdir!
   )
   if "!arg!"=="--tutdir" (
      set out=!out! !tutdir!
   )
   if "!arg!"=="--srcdir" (
      rem output the src directory
      set out=!out! !srcdir!
   )
   if "!arg!"=="--config" (
      rem output the configure arguments
      set out=!out! !configargs!
   )
   if "!arg!"=="--features" (
      rem output all supported features
      set out=!out!!features!
   )
   if "!arg!"=="--ncpu" (
      rem number of available cores
      set out=!out! %NUMBER_OF_PROCESSORS%
   )
   if "!arg!"=="--cc" (
      rem output used C compiler
      set out=!out! !altcc!
   )
   if "!arg!"=="--cxx" (
      rem output used C++ compiler
      set out=!out! !altcxx!
   )
   if "!arg!"=="--f77" (
      rem output used Fortran compiler
      set out=!out! !altf77!
   )
   if "!arg!"=="--ld" (
      rem output used Linker
      set out=!out! !altld!
   )
   if "!arg!"=="--help" (
      rem Print a help message
      goto print_help
   )
)

if "!out!"=="" (
   set err=1
   echo !arg!: Unknown option or argument^^!
   echo.
   goto print_help
)
echo|set /p=!out!

exit /b !err!

:print_help

echo Usage: %0 [options]
echo.
echo   --arch                Print the architecture (compiler/OS)
echo   --platform            Print the platform (OS)
echo   --prefix              Print the prefix
echo   --libs                Print regular ROOT libraries
echo   --glibs               Print regular + GUI ROOT libraries
echo   --evelibs             Print regular + GUI + Eve libraries
echo   --cflags              Print compiler flags and header path
echo   --bindir              Print the executable directory
echo   --libdir              Print the library directory
echo   --incdir              Print the header directory
echo   --etcdir              Print the configuration directory
echo   --tutdir              Print the tutorials directory
echo   --srcdir              Print the top of the original source directory
echo   --auxlibs             Print auxiliary libraries
echo   --config              Print arguments used for configuration with CMake
echo   --features            Print list of all supported features
echo   --has-^<feature^>       Test if ^<feature^> is compiled in
echo   --version             Print the ROOT version
echo   --git-revision        Print the ROOT git revision number
echo   --python-version      Print the Python version used by ROOT
echo   --python3-version     Print the Python3 version used by PyROOT
echo   --ncpu                Print number of available (hyperthreaded) cores
echo   --cc                  Print alternative C compiler specified when ROOT was built
echo   --cxx                 Print alternative C++ compiler specified when ROOT was built
echo   --f77                 Print alternative Fortran compiler specified when ROOT was built
echo   --ld                  Print alternative Linker specified when ROOT was built
echo   --help                Print this message

exit /b !err!
