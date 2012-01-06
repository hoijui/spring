/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#include "Misc.h"

#ifdef linux
#include <unistd.h>
#include <dlfcn.h> // for dladdr(), dlopen()

#elif WIN32
#include <io.h>
#include <process.h>
#include <shlobj.h>
#include <shlwapi.h>
#ifndef SHGFP_TYPE_CURRENT
	#define SHGFP_TYPE_CURRENT 0
#endif
#include "System/Platform/Win/WinVersion.h"

#elif __APPLE__
#include <unistd.h>
#include <mach-o/dyld.h>
#include <stdlib.h>
#include <dlfcn.h> // for dladdr(), dlopen()
#include <climits> // for PATH_MAX

#else

#endif

#if !defined(WIN32)
#include <sys/utsname.h> // for uname()
#include <sys/types.h> // for getpw
#include <pwd.h> // for getpw
#endif

#include <cstring>
#include <cerrno>

#include "lib/cutils/Util.h"
#include "System/Util.h"
#include "System/SafeCStrings.h"
#include "System/Log/ILog.h"
#include "System/FileSystem/FileSystem.h"

#if       defined WIN32
/**
 * Returns a handle to the currently loaded module.
 * Note: requires at least Windows 2000
 * @return handle to the currently loaded module, or NULL if an error occures
 */
static HMODULE GetCurrentModule() {

	HMODULE hModule = NULL;

	// both solutions use the address of this function
	// both found at:
	// http://stackoverflow.com/questions/557081/how-do-i-get-the-hmodule-for-the-currently-executing-code/557774

	// Win 2000+ solution
	MEMORY_BASIC_INFORMATION mbi = {0};
	::VirtualQuery((void*)GetCurrentModule, &mbi, sizeof(mbi));
	hModule = reinterpret_cast<HMODULE>(mbi.AllocationBase);

	// Win XP+ solution (cleaner)
	//::GetModuleHandleEx(
	//		GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
	//		(LPCTSTR)GetCurrentModule,
	//		&hModule);

	return hModule;
}
#endif // defined WIN32

/**
 * The user might want to define the user dir manually,
 * to locate spring related data in a non-default location.
 * @link http://en.wikipedia.org/wiki/Environment_variable#Synopsis
 */
static std::string GetUserDirFromEnvVar()
{
#ifdef _WIN32
	#define HOME_ENV_VAR "LOCALAPPDATA"
#else
	#define HOME_ENV_VAR "HOME"
#endif
	char* home = getenv(HOME_ENV_VAR);
#undef HOME_ENV_VAR

	return (home == NULL) ? "" : home;
}

static std::string GetUserDirFromSystemApi()
{
#ifdef _WIN32
	TCHAR strPath[MAX_PATH + 1];
	SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA, NULL, SHGFP_TYPE_CURRENT, strPath);
	return strPath;
#else
	struct passwd* pw = getpwuid(getuid());
	return pw->pw_dir;
#endif
}


namespace Platform
{

std::string GetUserDir()
{
	std::string userDir = GetUserDirFromEnvVar();

	if (userDir.empty()) {
		// In some cases, the env var is not set,
		// for example for non-human user accounts,
		// or when starting through the UI on OS X.
		// It is unset by default on windows.
		userDir = GetUserDirFromSystemApi();
	}

	return userDir;
}

// Mac OS X:        _NSGetExecutablePath() (man 3 dyld)
// Linux:           readlink /proc/self/exe
// Solaris:         getexecname()
// FreeBSD:         sysctl CTL_KERN KERN_PROC KERN_PROC_PATHNAME -1
// BSD with procfs: readlink /proc/curproc/file
// Windows:         GetModuleFileName() with hModule = NULL
std::string GetProcessExecutableFile()
{
	std::string procExeFilePath = "";

	char path[2048];
	const bool pathFetchOk = util_getProcessExecutableFile(path, sizeof(path));
	if (!pathFetchOk && util_hasError()) {
		char errorMsg[2048];
		util_getError(errorMsg, sizeof(errorMsg));
		LOG_L(L_WARNING, "Failed to get file path of the process executable, reason: %s", errorMsg);
	} else {
		procExeFilePath = path;
	}

	return procExeFilePath;
}

std::string GetProcessExecutablePath()
{
	return FileSystem::GetDirectory(GetProcessExecutableFile());
}

std::string GetModuleFile(std::string moduleName)
{
	std::string moduleFilePath = "";

	char path[2048];
	const bool pathFetchOk = util_getModuleFile(moduleName.c_str(), path, sizeof(path));
	if (!pathFetchOk && util_hasError()) {
		char errorMsg[2048];
		util_getError(errorMsg, sizeof(errorMsg));
		LOG_L(L_WARNING, "Failed to get file path of the module \"%s\", reason: %s", moduleName.c_str(), errorMsg);
	} else {
		moduleFilePath = path;
	}

	return moduleFilePath;
}
std::string GetModulePath(const std::string& moduleName)
{
	return FileSystem::GetDirectory(GetModuleFile(moduleName));
}

std::string GetOS()
{
#if defined(WIN32)
	return "Microsoft Windows\n" +
		GetOSDisplayString() + "\n" +
		GetHardwareInfoString();
#else
	struct utsname info;
	if (uname(&info) == 0) {
		return std::string(info.sysname) + " "
				+ info.release + " "
				+ info.version + " "
				+ info.machine;
	} else {
#if defined(__linux__)
		return "Linux";
#elif defined(__FreeBSD__)
		return "FreeBSD";
#elif defined(__APPLE__)
		return "Mac OS X";
#else
		#warning improve this
		return "unknown OS";
#endif
	}
#endif
}

bool Is64Bit()
{
	return (sizeof(void*) == 8);
}

#ifdef WIN32
typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

LPFN_ISWOW64PROCESS fnIsWow64Process;

/** @brief checks if the current process is running in 32bit emulation mode
    @return FALSE, TRUE, -1 on error (usually no permissions) */
bool Is32BitEmulation()
{
	BOOL bIsWow64 = FALSE;

	fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
		GetModuleHandle(TEXT("kernel32")),"IsWow64Process");

	if (NULL != fnIsWow64Process)
	{
		if (!fnIsWow64Process(GetCurrentProcess(),&bIsWow64))
		{
			return false;
		}
	}
	return bIsWow64;
}
#else
// simply assume other OS doesn't need 32bit emulation
bool Is32BitEmulation()
{
	return false;
}
#endif

std::string ExecuteProcess(const std::string& file, std::vector<std::string> args)
{
	std::string execError = "";

	// "The first argument, by convention, should point to
	// the filename associated with the file being executed."
	args.insert(args.begin(), Quote(file));

	char** processArgs = new char*[args.size() + 1];

	for (size_t a = 0; a < args.size(); ++a) {
		const std::string& arg = args.at(a);
		const size_t arg_size = arg.length() + 1;
		processArgs[a] = new char[arg_size];
		STRCPY_T(processArgs[a], arg_size, arg.c_str());
	}

	// "The array of pointers must be terminated by a NULL pointer."
	processArgs[args.size()] = NULL;

	{
		// Execute
#ifdef WIN32
	#define EXECVP _execvp
#else
	#define EXECVP execvp
#endif
		const int ret = EXECVP(file.c_str(), processArgs);

		if (ret == -1) {
			execError = strerror(errno);
		}
	}

	for (size_t a = 0; a < args.size(); ++a) {
		delete[] processArgs[a];
	}
	delete[] processArgs;

	return execError;
}

} // namespace Platform
