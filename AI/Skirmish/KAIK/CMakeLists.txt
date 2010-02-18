### Generic native Skirmish AI config
#

set(mySourceDirRel         "") # Common values are "" or "src"
set(additionalSources      "")
set(additionalCompileFlags "-DUSING_CREG")
set(additionalLibraries    cutils ${LegacyCpp_Creg_AIWRAPPER_TARGET})

ConfigureNativeSkirmishAI(mySourceDirRel additionalSources additionalCompileFlags additionalLibraries)
