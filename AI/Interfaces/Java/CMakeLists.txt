### Java AI Interface
#
# Global variables set in this file:
# * BUILD_Java_AIINTERFACE
# * Java_AIINTERFACE_VERS
# * Java_AIINTERFACE_TARGET
#
# Functions and macros defined in this file:
# * ConfigureJavaSkirmishAI
#

#enable_language(Java)


################################################################################
### BEGINN: MACROS_AND_FUNCTIONS
# Define macros and functions to be used in this file and by Java Skirmish AIs

# includes rts/build/cmake/UtilJava.cmake
INCLUDE(UtilJava)

# Java Skirmish AI configuration macro.
# This will be called from Java AIs at AI/Skirmish/*/CMakeLists.txt.
# If Maven is installed, and
macro    (ConfigureJavaSkirmishAI wrapperNames)
	# Assemble meta data
	set(myDir            "${CMAKE_CURRENT_SOURCE_DIR}")
	GetLastPathPart(dirName ${myDir})
	set(myName          "${dirName}")
	set(myJarFile       "SkirmishAI")
	set(myBinJarFile    "${myJarFile}.jar")
	set(mySrcJarFile    "${myJarFile}-src.jar")
	set(myJLibDir       "${myDir}/data/jlib")
	GetVersionPlusDepFile(myVersion myVersionDepFile)
	set(myTarget        "${myName}")
	set(myInstLibsDir   "${SKIRMISH_AI_LIBS}/${myName}/${myVersion}")
	set(myInstDataDir   "${SKIRMISH_AI_DATA}/${myName}/${myVersion}")
	# CMAKE_CURRENT_BINARY_DIR: .../spring-build-dir/AI/Skirmish/${myName}
	set(myBuildDir      "${CMAKE_CURRENT_BINARY_DIR}")

	# Check if the user wants to compile the AI
	if    (BUILD_Java_AIINTERFACE AND NOT myName MATCHES "${AI_EXCLUDE_REGEX}")
		set(BUILD_THIS_SKIRMISHAI TRUE)
	else  (BUILD_Java_AIINTERFACE AND NOT myName MATCHES "${AI_EXCLUDE_REGEX}")
		set(BUILD_THIS_SKIRMISHAI FALSE)
		message("warning: ${myName} Skirmish AI will not be built!")
	endif (BUILD_Java_AIINTERFACE AND NOT myName MATCHES "${AI_EXCLUDE_REGEX}")

	# Compile and install
	if    (BUILD_THIS_SKIRMISHAI)
		file(MAKE_DIRECTORY "${myBuildDir}/jlib")

		IsMavenInstalled(isMavenInstalled)
		IsMavenProject(${myDir} isMavenProject)
		set(configureMavenProject FALSE)
		if    (isMavenProject)
			if    (isMavenInstalled)
				set(configureMavenProject TRUE)
			else  (isMavenInstalled)
				message("Warning: Java AI with Maven Project file found, but Maven 2 not installed: Using default configuration.")
			endif (isMavenInstalled)
		endif (isMavenProject)

		set(configString "default")
		if    (configureMavenProject)
			set(configString "Maven")
		endif (configureMavenProject)
		SkirmishAIMessage(STATUS "Found Skirmish AI: ${myName} ${myVersion} (config: ${configString})")

		# Assemble project generated targets (and their libraries) we depend on
		set(myDependTargets  "${Java_AIINTERFACE_TARGET}")
		set(myDependLibFiles "${Java_AIINTERFACE_JAR_BIN}")
		foreach    (wrapperName ${wrapperNames})
			set(myDependTargets  ${myDependTargets}  "${${wrapperName}_AIWRAPPER_TARGET}")
			set(myDependLibFiles ${myDependLibFiles} "${${wrapperName}_AIWRAPPER_JAR_BIN}")
		endforeach (wrapperName)
		set_source_files_properties(${myDependLibFiles} PROPERTIES GENERATED TRUE)

		# find java source root
		if     (EXISTS "${myDir}/src/main/java")
			# default Maven source dir
			set(mySourceDir          "${myDir}/src/main/java")
		elseif (EXISTS "${myDir}/src")
			# simple java source dir path
			set(mySourceDir          "${myDir}/src")
		else   (EXISTS "${myDir}/src/main/java")
			message(SEND_ERROR "No sources dir found for Skirmish AI: ${myName}")
		endif  (EXISTS "${myDir}/src/main/java")

		# Create a list of all the AIs source files (for compiling)
		file(GLOB_RECURSE mySources RELATIVE "${mySourceDir}" FOLLOW_SYMLINKS "${mySourceDir}/*.java")
		# Create a list of all the AIs source files (for dependency tracking)
		file(GLOB_RECURSE mySourcesDep FOLLOW_SYMLINKS "${mySourceDir}/*.java")

		set_source_files_properties("${myBuildDir}/${myBinJarFile}" PROPERTIES GENERATED TRUE)
		set_source_files_properties("${myBuildDir}/${mySrcJarFile}" PROPERTIES GENERATED TRUE)


		if    (configureMavenProject)
			# The Maven Project has to make sure these files get created:
			# ${my.build.dir}/SkirmishAI.jar
			# ${my.build.dir}/SkirmishAI-src.jar (optional)
			file(RELATIVE_PATH myBuildDirRel "${myDir}" "${myBuildDir}")
			set(MVN_ATTRIBUTES
				"-P" "as-spring-subproject" # enable special profile
				"-Dmy.build.dir=${myBuildDirRel}"
				"-Dmy.version=${myVersion}"
				"--batch-mode" # Run in non-interactive (batch) mode
				)
			if    (NOT CMAKE_VERBOSE_MAKEFILE)
				# Quiet output - only show errors
				set(MVN_ATTRIBUTES ${MVN_ATTRIBUTES} "--quiet")
			endif (NOT CMAKE_VERBOSE_MAKEFILE)
			set(MVN_PHASE "package")
			#set(MVN_PHASE "help:effective-pom") # use this for debug purposes
			add_custom_command(
				OUTPUT
					"${myBuildDir}/${myBinJarFile}"
				COMMAND "mvn"
					"${MVN_PHASE}" ${MVN_ATTRIBUTES}
				DEPENDS
					${myDependLibFiles}
					${mySourcesDep}
				WORKING_DIRECTORY
					"${myDir}"
				COMMENT
					"  ${myTarget}: Running Maven ${MVN_PHASE} phase" VERBATIM
				)
			add_custom_target(${myTarget} ALL DEPENDS "${myBuildDir}/${myBinJarFile}" ${myVersionDepFile})
			add_dependencies(${myTarget} ${myDependTargets})
		else  (configureMavenProject)
			# If main Java package is "my.ai.pkg", this has to be set to "my".
			GetFirstSubDirName(firstSrcSubDir ${mySourceDir})
			set(myJavaPkgFirstPart "${firstSrcSubDir}")

			# Assemble additional meta data
			set(myBinaryTarget "${myTarget}-BIN")
			set(mySourceTarget "${myTarget}-SRC")
			set(myJavaBuildDir "${myBuildDir}/classes")

			add_custom_target(${myBinaryTarget}
				DEPENDS "${myBuildDir}/${myBinJarFile}")
			add_dependencies(${myBinaryTarget} ${myDependTargets})

			add_custom_target(${mySourceTarget}
				DEPENDS "${myBuildDir}/${mySrcJarFile}")

			add_custom_target(${myTarget} ALL DEPENDS ${myVersionDepFile})
			add_dependencies(${myTarget} ${myBinaryTarget} ${mySourceTarget})

			# Create our full Java class-path
			CreateClasspath(myJavaLibs ${myJLibDir})
			ConcatClasspaths(myClassPath "${CLASSPATH_Java_AIINTERFACE}" "${myJavaLibs}")
			foreach    (wrapperName ${wrapperNames})
				ConcatClasspaths(myClassPath "${myClassPath}" "${${wrapperName}_AIWRAPPER_JAR_CLASSPATH}")
			endforeach (wrapperName)

			# Locate the manifest file
			FindManifestFile("${myDir}" myManifestFile)
			if    (myManifestFile)
				set(myBinJarArgs "cmf" "${myManifestFile}")
			else  (myManifestFile)
				set(myBinJarArgs "cf")
			endif (myManifestFile)

			# Write list of source files to an arg-file
			set(mySrcArgFile "${myBuildDir}/sourceFiles.txt")
			if    (EXISTS "${mySrcArgFile}")
				file(REMOVE "${mySrcArgFile}")
			endif (EXISTS "${mySrcArgFile}")
			set(mySrcArgFile "${myBuildDir}/sourceFiles.txt")
			foreach    (srcFile ${mySources})
				file(APPEND "${mySrcArgFile}" "${srcFile}\n")
			endforeach (srcFile)

			# Compile and pack the library
			add_custom_command(
				OUTPUT
					"${myBuildDir}/${myBinJarFile}"
				DEPENDS
					${myDependLibFiles}
					${mySourcesDep}
				COMMAND "${CMAKE_COMMAND}"
					"-E" "remove_directory" "${myJavaBuildDir}"
				COMMAND "${CMAKE_COMMAND}"
					"-E" "make_directory" "${myJavaBuildDir}"
				COMMAND "${JAVA_COMPILE}"
					"${JAVA_COMPILE_FLAG_CONDITIONAL}"
					"-cp" "${myClassPath}"
					"-d" "${myJavaBuildDir}"
					"@${mySrcArgFile}"
				COMMAND "${JAVA_ARCHIVE}"
					${myBinJarArgs} "${myBuildDir}/${myBinJarFile}"
					"-C" "${myJavaBuildDir}" "${myJavaPkgFirstPart}"
				WORKING_DIRECTORY
					"${mySourceDir}"
				COMMENT
					"  ${myTarget}: Compiling sources and packing library ${myBinJarFile}" VERBATIM
				)

			# Pack the sources
			add_custom_command(
				OUTPUT
					"${myBuildDir}/${mySrcJarFile}"
				COMMAND "${JAVA_ARCHIVE}"
					"cf" "${myBuildDir}/${mySrcJarFile}"
					"-C" "${mySourceDir}" "${myJavaPkgFirstPart}"
				DEPENDS
					${mySourcesDeps}
				WORKING_DIRECTORY
					"${mySourceDir}"
				COMMENT
					"  ${myTarget}: Creating sources archive ${mySrcJarFile}" VERBATIM
				)
		endif (configureMavenProject)

		# Install the data files
		install(DIRECTORY "${myDir}/data/"                 DESTINATION ${myInstDataDir})
		# Install the library
		install(FILES    "${myBuildDir}/${myBinJarFile}"   DESTINATION ${myInstDataDir})
		# Install the sources archive (optional)
		install(FILES    "${myBuildDir}/${mySrcJarFile}"   DESTINATION ${myInstDataDir}/jlib OPTIONAL)

		# Install files generated/downloaded during buildtime
		install(DIRECTORY "${myBuildDir}/jlib/"            DESTINATION ${myInstDataDir}/jlib)
		if    (EXISTS "${myBuildDir}/resources")
			install(DIRECTORY "${myBuildDir}/resources/"   DESTINATION ${myInstDataDir}/resources)
		endif (EXISTS "${myBuildDir}/resources")
		if    (EXISTS "${myBuildDir}/config")
			install(DIRECTORY "${myBuildDir}/config/"      DESTINATION ${myInstDataDir}/config)
		endif (EXISTS "${myBuildDir}/config")
		if    (EXISTS "${myBuildDir}/script")
			install(DIRECTORY "${myBuildDir}/script/"      DESTINATION ${myInstDataDir}/script)
		endif (EXISTS "${myBuildDir}/script")

		# Install special script and config files
		if    (EXISTS "${myDir}/src/main/groovy")
			install(DIRECTORY "${myDir}/src/main/groovy/"  DESTINATION ${myInstDataDir}/script)
		endif (EXISTS "${myDir}/src/main/groovy")
		if    (EXISTS "${myDir}/src/main/ruby")
			install(DIRECTORY "${myDir}/src/main/ruby/"    DESTINATION ${myInstDataDir}/script)
		endif (EXISTS "${myDir}/src/main/ruby")
		if    (EXISTS "${myDir}/src/main/bsh")
			install(DIRECTORY "${myDir}/src/main/bsh/"     DESTINATION ${myInstDataDir}/script)
		endif (EXISTS "${myDir}/src/main/bsh")
		if    (EXISTS "${myDir}/src/main/config")
			install(DIRECTORY "${myDir}/src/main/config/"  DESTINATION ${myInstDataDir}/config)
		endif (EXISTS "${myDir}/src/main/config")

		if    (NOT configureMavenProject)
			# Install jars of wrappers
			foreach    (wrapperName ${wrapperNames})
				# Install the wrappers main jar
				install(FILES "${${wrapperName}_AIWRAPPER_JAR_BIN}" DESTINATION ${myInstDataDir}/jlib)
				set(wrapperJLibDir "${CMAKE_SOURCE_DIR}/AI/Wrappers/${wrapperName}/jlib")
				# Install the wrappers java libs, if there are any
				if    (EXISTS ${wrapperJLibDir})
					install(DIRECTORY "${wrapperJLibDir}/" DESTINATION ${myInstDataDir}/jlib)
				endif (EXISTS ${wrapperJLibDir})
			endforeach (wrapperName)
		endif (NOT configureMavenProject)
	endif (BUILD_THIS_SKIRMISHAI)
endmacro (ConfigureJavaSkirmishAI wrapperNames)


### END: MACROS_AND_FUNCTIONS
################################################################################


set(myName               "Java")
set(myDir                "${CMAKE_CURRENT_SOURCE_DIR}")
set(myNativeSourceDirRel "src/main/native")
set(myJavaSourceDirRel   "src/main/java")
set(myPkgFirstPart       "com")
set(myPkg                "${myPkgFirstPart}/springrts/ai")


# Check if the user wants to compile the interface
if    ("${AI_TYPES}" STREQUAL "ALL" OR "${AI_TYPES}" STREQUAL "JAVA")
	set(AI_TYPES_JAVA TRUE)
else  ("${AI_TYPES}" STREQUAL "ALL" OR "${AI_TYPES}" STREQUAL "JAVA")
	set(AI_TYPES_JAVA FALSE)
endif ("${AI_TYPES}" STREQUAL "ALL" OR "${AI_TYPES}" STREQUAL "JAVA")

if    (AI_TYPES_JAVA AND myName MATCHES "${AI_EXCLUDE_REGEX}")
	set(AI_TYPES_JAVA FALSE)
endif (AI_TYPES_JAVA AND myName MATCHES "${AI_EXCLUDE_REGEX}")


# Look for dependencies, but only if the user wants to build the interface
if    (AI_TYPES_JAVA)
	if    (NOT JAVA_FOUND)
		set(JAVA_FIND_QUIETLY TRUE)
		FIND_PACKAGE(JavaExtended) # Note: this is not the CMake built-in FindJava
		MakeGlobal(
			JAVA_FOUND
			JAVA_COMPILE
			JAVA_ARCHIVE
			JAVA_COMPILE_FLAG_CONDITIONAL)
	endif (NOT JAVA_FOUND)
	if    (MINGW)
		set (JNI_FOUND TRUE)
	else  (MINGW)
		# this hack is needed for FindJNI.cmake to use the JDK we want it to use,
		# as otherwise it might not find one at all (eg. in the case of OpenJDK)
		if    ( NOT ENV{JAVA_HOME} AND JAVA_HOME )
			SET(ENV{JAVA_HOME} "${JAVA_HOME}")
		endif ( NOT ENV{JAVA_HOME} AND JAVA_HOME )
		FIND_PACKAGE(JNI)
		if (JAVA_INCLUDE_PATH)
			set (JNI_FOUND TRUE)
			include_directories(${JAVA_INCLUDE_PATH} ${JAVA_INCLUDE_PATH2} ${JNI_INCLUDE_DIRS})
		else (JAVA_INCLUDE_PATH)
			set (JNI_FOUND FALSE)
			message ("warning: No Java includes found!")
		endif (JAVA_INCLUDE_PATH)
	endif (MINGW)
endif (AI_TYPES_JAVA)


# Check if dependencies of the interface are met
if    (AI_TYPES_JAVA AND JNI_FOUND AND JAVA_FOUND AND EXISTS ${myDir} AND EXISTS ${myDir}/bin AND AWK_FOUND AND NOT myName MATCHES "${AI_EXCLUDE_REGEX}")
	SetGlobal(BUILD_${myName}_AIINTERFACE TRUE)
else  (AI_TYPES_JAVA AND JNI_FOUND AND JAVA_FOUND AND EXISTS ${myDir} AND EXISTS ${myDir}/bin AND AWK_FOUND AND NOT myName MATCHES "${AI_EXCLUDE_REGEX}")
	SetGlobal(BUILD_${myName}_AIINTERFACE FALSE)
	message("warning: Java AI Interface will not be built!")
endif (AI_TYPES_JAVA AND JNI_FOUND AND JAVA_FOUND AND EXISTS ${myDir} AND EXISTS ${myDir}/bin AND AWK_FOUND AND NOT myName MATCHES "${AI_EXCLUDE_REGEX}")


# Build
if    (BUILD_${myName}_AIINTERFACE)
	GetVersionPlusDepFile(myVersion myVersionDepFile)
	set(myTarget      "${myName}-AIInterface")
	set(myInstLibsDir ${AI_INTERFACES_LIBS}/${myName}/${myVersion})
	set(myInstDataDir ${AI_INTERFACES_DATA}/${myName}/${myVersion})
	MakeAbsolute(myNativeSourceDir "${myDir}" "${myNativeSourceDirRel}")
	MakeAbsolute(myJavaSourceDir   "${myDir}" "${myJavaSourceDirRel}")

	AIInterfaceMessage(STATUS "Found AI Interface: ${myTarget} ${myVersion}")

	SetGlobal(${myName}_AIINTERFACE_VERS   ${myVersion})
	SetGlobal(${myName}_AIINTERFACE_TARGET ${myTarget})


	# Init some vars
	# --------------
	set(myAwkScriptsDir            "${myDir}/bin")
	set(commonAwkScriptsDir        "${CMAKE_SOURCE_DIR}/AI/Wrappers/CUtils/bin")
	set(myBuildDir                 "${CMAKE_CURRENT_BINARY_DIR}")
	set(springSourceDir            "${PROJECT_SOURCE_DIR}")
	set(springAIInterfaceSourceDir "${springSourceDir}/rts/ExternalAI/Interface")
	set(myJavaBuildDir             "${myBuildDir}/classes")
	set(myJarFile                  "AIInterface")
	set(myBinJarFile               "${myJarFile}.jar")
	set(mySrcJarFile               "${myJarFile}-src.jar")
	set(myGeneratedSourceDir       "${myBuildDir}/src-generated/main")
	set(myJavaGeneratedSourceDir   "${myGeneratedSourceDir}/java")
	set(myNativeGeneratedSourceDir "${myGeneratedSourceDir}/native")
	set(myJLibDir                  "${myDir}/data/jlib")
	CreateClasspath(myJavaLibs ${myJLibDir})
	set(myClassPath                ".${PATH_DELIM_H}${myJavaLibs}${PATH_DELIM_H}${myJavaSourceDir}")

	# Used by Java Skirmish AIs
	SetGlobal(SOURCE_ROOT_${myName}_AIINTERFACE      "${myDir}")
	SetGlobal(BUILD_ROOT_${myName}_AIINTERFACE       "${myBuildDir}")
	SetGlobal(${myName}_AIINTERFACE_JAR_BIN          "${myBuildDir}/${myBinJarFile}")
	SetGlobal(${myName}_AIINTERFACE_JAR_SRC          "${myBuildDir}/${mySrcJarFile}")
	SetGlobal(CLASSPATH_${myName}_AIINTERFACE        "${myJavaLibs}${PATH_DELIM_H}${myBuildDir}/${myBinJarFile}")
	SetGlobal(JAVA_SRC_DIR_${myName}_AIINTERFACE     "${myJavaSourceDir}")
	SetGlobal(JAVA_GEN_SRC_DIR_${myName}_AIINTERFACE "${myJavaGeneratedSourceDir}")


	# Generate sources
	# ----------------

	set(commonAwkScriptArgs
		"-v" "SPRING_SOURCE_DIR=${springSourceDir}"
		"-v" "INTERFACE_SOURCE_DIR=${myJavaSourceDir}"
		"-v" "GENERATED_SOURCE_DIR=${myGeneratedSourceDir}"
		"-v" "NATIVE_GENERATED_SOURCE_DIR=${myNativeGeneratedSourceDir}"
		"-v" "JAVA_GENERATED_SOURCE_DIR=${myJavaGeneratedSourceDir}"
		"-f" "${commonAwkScriptsDir}/common.awk"
		"-f" "${commonAwkScriptsDir}/commonDoc.awk"
		)

	# A CMake Custom Target will allways be built.
	# from CMake docu:
	# "add_custom_target is ALWAYS CONSIDERED OUT OF DATE"

	# Stub file for dependency tracking
	set(myGeneratedSourceDirStubFile "${CMAKE_CURRENT_BINARY_DIR}/myGeneratedSourceDir.stub")
	set_source_files_properties(${myGeneratedSourceDirStubFile} PROPERTIES GENERATED TRUE)


	# source file lists (static and generated)

	set(myNativeSources
		"${myNativeSourceDir}/InterfaceExport.c"
		"${myNativeSourceDir}/JavaBridge.c"
		"${myNativeSourceDir}/JniUtil.c"
		"${myNativeSourceDir}/JvmLocater_common.c"
		"${myNativeSourceDir}/JvmLocater_solaris.c"
		"${myNativeSourceDir}/JvmLocater_windows.c"
		)
	set(myNativeGeneratedSources
		"${myNativeGeneratedSourceDir}/CallbackFunctionPointerBridge.c"
		"${myNativeGeneratedSourceDir}/CallbackJNIBridge.c"
		"${myNativeGeneratedSourceDir}/EventsJNIBridge.c"
		)
	set(myNativeGeneratedHeaders
		"${myNativeGeneratedSourceDir}/CallbackFunctionPointerBridge.h"
		"${myNativeGeneratedSourceDir}/CallbackJNIBridge.h"
		"${myNativeGeneratedSourceDir}/EventsJNIBridge.h"
		)

	set(myJavaSources
		"${myJavaSourceDir}/${myPkg}/Util.java"
		)
	set(myJavaGeneratedSources
		"${myJavaGeneratedSourceDir}/${myPkg}/AI.java"
		"${myJavaGeneratedSourceDir}/${myPkg}/AbstractAI.java"
		"${myJavaGeneratedSourceDir}/${myPkg}/AICallback.java"
		"${myJavaGeneratedSourceDir}/${myPkg}/JniAICallback.java"
		)

	set(myGeneratedSources
		${myNativeGeneratedSources}
		${myNativeGeneratedHeaders}
		${myJavaGeneratedSources}
		)
	set_source_files_properties(${myGeneratedSources} PROPERTIES GENERATED TRUE)


	# remove all files in the generates sources dir,
	# that are not generated sources (of this build)
	file(GLOB_RECURSE allGeneratedSrcFiles "${myGeneratedSourceDir}/*")
	foreach    (genFile ${allGeneratedSrcFiles})
		list(FIND myGeneratedSources "${genFile}" isInList)
		if    (${isInList} EQUAL -1)
			file(REMOVE "${genFile}")
		endif ()
	endforeach (genFile)


	add_custom_command(
		OUTPUT
			${myGeneratedSourceDirStubFile}
		COMMAND "${CMAKE_COMMAND}"
			"-E" "remove_directory" "${myGeneratedSourceDir}"
		COMMAND "${CMAKE_COMMAND}"
			"-E" "make_directory"   "${myNativeGeneratedSourceDir}"
		COMMAND "${CMAKE_COMMAND}"
			"-E" "make_directory"   "${myJavaGeneratedSourceDir}/${myPkg}"
		COMMAND "${CMAKE_COMMAND}"
			"-E" "touch" "${myGeneratedSourceDirStubFile}"
		DEPENDS
			"${commonAwkScriptsDir}/common.awk"
			"${commonAwkScriptsDir}/commonDoc.awk"
			"${myAwkScriptsDir}/native_wrappCallback.awk"
			"${myAwkScriptsDir}/native_wrappCommands.awk"
			"${myAwkScriptsDir}/jni_wrappCallback.awk"
			"${myAwkScriptsDir}/jni_wrappEvents.awk"
			"${springAIInterfaceSourceDir}/SSkirmishAICallback.h"
			"${springAIInterfaceSourceDir}/AISCommands.h"
			"${springAIInterfaceSourceDir}/AISEvents.h"
		WORKING_DIRECTORY
			"${myAwkScriptsDir}"
		COMMENT
			"  ${myTarget}: Cleanup & create generated source directories" VERBATIM
		)

	# 1. & 2. Wrapp Callback (native->native)
	add_custom_command(
		OUTPUT
			"${myNativeGeneratedSourceDir}/CallbackFunctionPointerBridge.h"
			"${myNativeGeneratedSourceDir}/CallbackFunctionPointerBridge.c"
		COMMAND "${AWK_BIN}"
			${commonAwkScriptArgs}
			"-f" "${myAwkScriptsDir}/native_wrappCallback.awk"
			"${springAIInterfaceSourceDir}/SSkirmishAICallback.h"
		COMMAND "${AWK_BIN}"
			${commonAwkScriptArgs}
			"-f" "${myAwkScriptsDir}/native_wrappCommands.awk"
			"${springAIInterfaceSourceDir}/AISCommands.h"
		DEPENDS
			"${myGeneratedSourceDirStubFile}"
			"${commonAwkScriptsDir}/common.awk"
			"${commonAwkScriptsDir}/commonDoc.awk"
			"${myAwkScriptsDir}/native_wrappCallback.awk"
			"${myAwkScriptsDir}/native_wrappCommands.awk"
			"${springAIInterfaceSourceDir}/SSkirmishAICallback.h"
			"${springAIInterfaceSourceDir}/AISCommands.h"
		WORKING_DIRECTORY
			"${myAwkScriptsDir}"
		COMMENT
			"  ${myTarget}: Generating native callback wrapper sources" VERBATIM
		)

	# 3. Wrapp AI Callback (native-JNI->Java)
	add_custom_command(
		OUTPUT
			"${myNativeGeneratedSourceDir}/CallbackJNIBridge.h"
			"${myNativeGeneratedSourceDir}/CallbackJNIBridge.c"
			"${myJavaGeneratedSourceDir}/${myPkg}/AICallback.java"
			"${myJavaGeneratedSourceDir}/${myPkg}/JniAICallback.java"
		COMMAND "${AWK_BIN}"
			${commonAwkScriptArgs}
			"-f" "${myAwkScriptsDir}/jni_wrappCallback.awk"
			"${myNativeGeneratedSourceDir}/CallbackFunctionPointerBridge.h"
		DEPENDS
			"${myGeneratedSourceDirStubFile}"
			"${commonAwkScriptsDir}/common.awk"
			"${commonAwkScriptsDir}/commonDoc.awk"
			"${myAwkScriptsDir}/jni_wrappCallback.awk"
			"${myNativeGeneratedSourceDir}/CallbackFunctionPointerBridge.h"
		WORKING_DIRECTORY
			"${myAwkScriptsDir}"
		COMMENT
			"  ${myTarget}: Generating JNI callback wrapper sources" VERBATIM
		)

	# 4. Wrapp AI Events (native-JNI->Java)
	add_custom_command(
		OUTPUT
			"${myNativeGeneratedSourceDir}/EventsJNIBridge.h"
			"${myNativeGeneratedSourceDir}/EventsJNIBridge.c"
			"${myJavaGeneratedSourceDir}/${myPkg}/AI.java"
			"${myJavaGeneratedSourceDir}/${myPkg}/AbstractAI.java"
		COMMAND "${AWK_BIN}"
			${commonAwkScriptArgs}
			"-f" "${myAwkScriptsDir}/jni_wrappEvents.awk"
			"${springAIInterfaceSourceDir}/AISEvents.h"
		DEPENDS
			"${myGeneratedSourceDirStubFile}"
			"${commonAwkScriptsDir}/common.awk"
			"${commonAwkScriptsDir}/commonDoc.awk"
			"${myAwkScriptsDir}/jni_wrappEvents.awk"
			"${springAIInterfaceSourceDir}/AISEvents.h"
		WORKING_DIRECTORY
			"${myAwkScriptsDir}"
		COMMENT
			"  ${myTarget}: Generating JNI events wrapper sources" VERBATIM
		)

	# Locate the manifest file
	FindManifestFile("${myDir}" myManifestFile)
	if    (myManifestFile)
		set(myBinJarArgs "cmf" "${myManifestFile}")
	else  (myManifestFile)
		set(myBinJarArgs "cf")
	endif (myManifestFile)

	# Build the native part
	# ---------------------
	if    (MINGW)
		# It is important that this is used instead of the one
		# from the installed JDK, as the jni_md.h is in here too,
		# and this file contains OS (win32) specific information.
		include_directories(BEFORE ${MINGWLIBS}/include/java)
	endif (MINGW)
	include_directories(BEFORE "${rts}/lib/streflop" "${myNativeSourceDir}" "${myNativeGeneratedSourceDir}")
	add_library(${myTarget} MODULE ${myNativeSources} ${myNativeGeneratedSources} ${ai_common_SRC} ${CUtils_SRC} ${myVersionDepFile})
	target_link_libraries(${myTarget} streflop)
	set_target_properties(${myTarget} PROPERTIES COMPILE_FLAGS "-DUSING_STREFLOP")
	set_target_properties(${myTarget} PROPERTIES OUTPUT_NAME   "AIInterface")
	FixLibName(${myTarget})


	# Build the java part
	# -------------------

	# Write list of source files to an arg-file
	set(mySrcArgFile "${myBuildDir}/sourceFiles.txt")
	if    (EXISTS "${mySrcArgFile}")
		file(REMOVE "${mySrcArgFile}")
	endif (EXISTS "${mySrcArgFile}")
	foreach    (srcFile ${myJavaSources} ${myJavaGeneratedSources})
		file(APPEND "${mySrcArgFile}" "${srcFile}\n")
	endforeach (srcFile)

	add_custom_command(
		OUTPUT
			"${myBuildDir}/${myBinJarFile}"
		COMMAND "${CMAKE_COMMAND}"
			"-E" "remove_directory" "${myJavaBuildDir}"
		COMMAND "${CMAKE_COMMAND}"
			"-E" "make_directory" "${myJavaBuildDir}"
		COMMAND "${JAVA_COMPILE}" ARGS
			"${JAVA_COMPILE_FLAG_CONDITIONAL}"
			"-cp" "${myClassPath}"
			"-d" "${myJavaBuildDir}"
			"@${mySrcArgFile}"
		COMMAND "${JAVA_ARCHIVE}" ARGS
			${myBinJarArgs} "${myBuildDir}/${myBinJarFile}"
			"-C" "${myJavaBuildDir}" "${myPkgFirstPart}"
		DEPENDS
			${myJavaSources}
			${myJavaGeneratedSources}
		WORKING_DIRECTORY
			"${myJavaGeneratedSourceDir}"
		COMMENT
			"  ${myTarget}: Compiling sources and packing library ${myBinJarFile}" VERBATIM
		)

	add_custom_command(
		OUTPUT
			"${myBuildDir}/${mySrcJarFile}"
		COMMAND "${JAVA_ARCHIVE}" ARGS
			"cf" "${myBuildDir}/${mySrcJarFile}"
			"-C" "${myJavaSourceDir}" "${myPkgFirstPart}"
		COMMAND "${JAVA_ARCHIVE}" ARGS
			"uf" "${myBuildDir}/${mySrcJarFile}"
			"-C" "${myJavaGeneratedSourceDir}" "${myPkgFirstPart}"
		DEPENDS
			${myJavaSources}
			${myJavaGeneratedSources}
		WORKING_DIRECTORY
			"${myBuildDir}"
		COMMENT
			"  ${myTarget}: Creating sources archive ${mySrcJarFile}" VERBATIM
		)

	IsMavenInstalled(isMavenInstalled)
	if    (isMavenInstalled)
		add_custom_command(TARGET ${myTarget} POST_BUILD
			COMMAND "${CMAKE_COMMAND}"
				"-Dfile.in=${myDir}/pom.xml"
				"-Dfile.out=${myBuildDir}/pom.xml"
				"-DmyVersion=${myVersion}"
				"-P" "${CMAKE_MODULES_SPRING}/ConfigureFile.cmake"
			COMMAND "mvn"
				"install:install-file"
				"-DpomFile=${myBuildDir}/pom.xml"
				"-Dfile=${myBuildDir}/${myBinJarFile}"
				"-Dsources=${myBuildDir}/${mySrcJarFile}"
			DEPENDS
				"${myDir}/pom.xml"
				"${myBuildDir}/${myBinJarFile}"
				"${myBuildDir}/${mySrcJarFile}"
			WORKING_DIRECTORY
				"${myDir}"
			COMMENT
				"  ${myTarget}: Installing to local maven repository" VERBATIM
			)
	endif (isMavenInstalled)

	add_custom_target(${myTarget}-java ALL
		DEPENDS
			"${myBuildDir}/${myBinJarFile}"
			"${myBuildDir}/${mySrcJarFile}")
	add_dependencies(${myTarget} ${myTarget}-java)

	# Install the native library
	install(TARGETS   ${myTarget} DESTINATION ${myInstLibsDir})
	# Install the data files
	install(DIRECTORY "${myDir}/data/" DESTINATION ${myInstLibsDir} FILES_MATCHING PATTERN REGEX "InterfaceInfo\\.lua$")
	install(DIRECTORY "${myDir}/data/" DESTINATION ${myInstDataDir} FILES_MATCHING PATTERN REGEX "InterfaceInfo\\.lua$" EXCLUDE PATTERN "*")
	# Install the library
	install(FILES     "${myBuildDir}/${myBinJarFile}"  DESTINATION ${myInstDataDir})
	# Install the sources archive
	install(FILES     "${myBuildDir}/${mySrcJarFile}"  DESTINATION ${myInstDataDir}/jlib)
endif (BUILD_${myName}_AIINTERFACE)
