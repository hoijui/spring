### Generic native Skirmish AI config
#

set(mySourceDirRel         "src") # Common values are "" or "src"
set(additionalSources      "")
set(additionalCompileFlags "")
set(additionalLibraries    cutils ${Cpp_AIWRAPPER_TARGET})

include_directories(BEFORE ${Cpp_AIWRAPPER_GENERATED_SRC_DIR})
ConfigureNativeSkirmishAI(mySourceDirRel additionalSources additionalCompileFlags additionalLibraries)
