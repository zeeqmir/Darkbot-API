// DarkBot.cpp : Defines the exported functions for the DLL application.
//

#include <Windows.h>
#include <vector>
#include <TlHelp32.h>
#include <Psapi.h>
#include <memoryapi.h>
#include <iostream>

#include "DarkBot.h"
#include "Utils.h"

typedef struct {
	UINT64 dwBase, dwSize;
} MemModule;


static DWORD dwWritten;
static HANDLE TargetProcess;
static HANDLE hJob;
static DWORD BrowserPID;
static DWORD EnginePID = 0;;
static HWND BrowserWin;
static MemModule maxmod;


void send_command(char *command, bool sync = false) {
	char pipePath[30];
	sprintf(pipePath, "\\\\.\\pipe\\%lu", BrowserPID);
	HANDLE hPipe = CreateFile(TEXT(pipePath),
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);
	if (hPipe != INVALID_HANDLE_VALUE) {
		WriteFile(hPipe,
			command,
			strlen(command)+1,
			&dwWritten,
			NULL);
		/* If call is synchronous, wait for browser to finish. */
		if (sync) {
			bool fSuccess;
			char *chBuf[512];
			DWORD cbRead;
			do {

				fSuccess = ReadFile(
					hPipe,    // pipe handle 
					chBuf,    // buffer to receive reply 
					512 * sizeof(TCHAR),  // size of buffer 
					&cbRead,  // number of bytes read 
					NULL);    // not overlapped 

				if (!fSuccess && GetLastError() != ERROR_MORE_DATA)
					break;
			} while (!fSuccess);  // repeat loop if ERROR_MORE_DATA 
		}
		CloseHandle(hPipe);

	}
}

/* Returns a struct with the base address and size of the module */
MemModule get_module(const char *pname) {
	TCHAR buf[MAX_PATH];
	HMODULE modules[1584];
	DWORD needed = sizeof(HMODULE) * 1024;
	MODULEINFO mInfo;

	// Get process full path
	GetProcessImageFileName(TargetProcess, buf, sizeof(buf) / sizeof(*buf));

	// Enumerate process modules
	if (EnumProcessModules(TargetProcess, modules, sizeof(modules) / sizeof(HMODULE), &needed) != 0) {
		for (HMODULE m_module : modules) {
			TCHAR szModName[MAX_PATH];
			if (GetModuleFileNameEx(TargetProcess, m_module, szModName, sizeof(szModName) / sizeof(TCHAR)) != 0) {

				if (GetModuleInformation(TargetProcess, m_module, &mInfo, sizeof(MODULEINFO)) != 0) {
					if (strstr(szModName, pname) != NULL) {
						return { (DWORD_PTR)mInfo.EntryPoint, (DWORD_PTR)mInfo.SizeOfImage };
					}
				} else {
					break;
				}
			}
		}
	} else {
		std::cout << "EnumProcessModules failed with code: " << GetLastError() << std::endl;
	}
	return { 0 };
}

template<typename T>
UINT64 *query_memory(T value, UINT64 start, UINT64 end, int amount, int *found) {
	MEMORY_BASIC_INFORMATION meminfo = { 0 };
	UINT64 addr, end;
	int count = 0;
	SIZE_T read;
	UINT64 *result = (UINT64 *)calloc(amount, sizeof(UINT64 *));
	*found = 0;

	addr = start;
	end = end;

	/* Iterate over memory areas */
	while (addr < end){
		VirtualQueryEx(TargetProcess, (LPCVOID)addr, &meminfo, sizeof(meminfo));
		addr = (UINT64)meminfo.BaseAddress + meminfo.RegionSize;

		/* Skip Reserved and private pages */
		if (!(meminfo.State & MEM_COMMIT))
			continue;

		/* Skip guarded pages to prevent triggering any tripwire */
		if (meminfo.AllocationProtect & PAGE_GUARD)
			continue;


		BYTE* data = new BYTE[meminfo.RegionSize];
		ReadProcessMemory(TargetProcess, meminfo.BaseAddress, data, meminfo.RegionSize, &read);
		for (UINT64 i = 0; i < meminfo.RegionSize-sizeof(T); i++) {
			T test;
			memcpy(&test, &data[i], sizeof(T));
			if (test == value) {
				result[count] = (UINT64)meminfo.BaseAddress + i;
				count++;
				*found = count;
			}
			if (*found == amount) {
				free(data);
				return result;
			}
		}
		free(data);
	}
	return result;
}

UINT64 find_pattern(unsigned char *query, UINT64 start, UINT64 end, int size) {
	MEMORY_BASIC_INFORMATION meminfo = { 0 };
	UINT64 addr, end;
	UINT64 total = 0;
	SIZE_T read;

	addr = start;
	end = end;
	/* Iterate over memory areas */
	while (addr < end){
		VirtualQueryEx(TargetProcess, (LPCVOID)addr, &meminfo, sizeof(meminfo));
		addr = (DWORD_PTR)meminfo.BaseAddress + meminfo.RegionSize;

		/* Skip Reserved and private pages */
		if (!(meminfo.State & MEM_COMMIT))
			continue;

		/* Skip guarded pages to prevent triggering any tripwire */
		if (meminfo.AllocationProtect & PAGE_GUARD)
			continue;

		/* Patterns for executable pages */

		BYTE* data = new BYTE[meminfo.RegionSize];
		ReadProcessMemory(TargetProcess, meminfo.BaseAddress, data, meminfo.RegionSize, &read);
		int counter = 0;
		for (DWORD_PTR i = 0; i < meminfo.RegionSize - size; i++) {
			while (data[i + counter] == query[counter]) {
				if (counter == size-1) {
					return (DWORD_PTR)meminfo.BaseAddress + (i * sizeof(BYTE));
				}
				counter++;
			}
			counter = 0;
		}
		delete(data);
	}
	return 0;
}

template <typename T>
T read_memory(LPCVOID base) {
	T out;
	ReadProcessMemory(TargetProcess, base, &out, sizeof(T), NULL);
	return out;
}

template <typename var>
bool write_memory(UINT64 Address, var Value) {
	return WriteProcessMemory(TargetProcess, (LPVOID)Address, &Value, sizeof(var), 0);
}

unsigned char* read_memory_bytes(LPCVOID addr, int size) {
	unsigned char* stuff = (unsigned char *)malloc(size);
	SIZE_T read;
	ReadProcessMemory(TargetProcess, addr, stuff, size, &read);

	return stuff;
}

bool get_module(HANDLE TargetProcess, const char *modname) {
	TCHAR buf[MAX_PATH];
	HMODULE modules[1584];
	DWORD needed = sizeof(HMODULE) * 1024;
	MODULEINFO mInfo;

	// Get process full path
	GetProcessImageFileName(TargetProcess, buf, sizeof(buf) / sizeof(*buf));

	// Enumerate process modules
	if (EnumProcessModules(TargetProcess, modules, sizeof(modules) / sizeof(HMODULE), &needed) != 0) {
		for (HMODULE m_module : modules) {
			TCHAR szModName[MAX_PATH];
			if (GetModuleFileNameEx(TargetProcess, m_module, szModName, sizeof(szModName) / sizeof(TCHAR)) != 0) {
				if (GetModuleInformation(TargetProcess, m_module, &mInfo, sizeof(MODULEINFO)) != 0) {
					if (strstr(szModName, modname)) {
						return true;
					}
				}
			}
		}
	}
	return false;
}

int find_process(const char *pattern) {
	int PID = 0;
	HANDLE handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	HANDLE process_handle = nullptr;
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(entry);

	while (Process32Next(handle, &entry)) {
		/* If parent of process is the browser we executed. */
		// && entry.th32ParentProcessID == BrowserPID
		if (strstr(entry.szExeFile, pattern)) {
			PID = entry.th32ProcessID;
			process_handle = OpenProcess(PROCESS_ALL_ACCESS, false, PID);
			/* Check if WebEngine process has the flashplayer dll */
			if (get_module(process_handle, "flashplayer")) {
				CloseHandle(process_handle);
				break;
			}
			PID = 0;
			CloseHandle(process_handle);
		}
	};
	return PID;
}


JNIEXPORT void JNICALL Java_com_github_manolo8_darkbot_core_DarkBotAPI_createWindow0(JNIEnv *env, jobject obj, jstring jpath) {
	const char *path;
	path = env->GetStringUTFChars(jpath, NULL);
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	hJob = CreateJobObject(NULL, NULL);
	BOOL bSuccess;
	JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli = { 0 };
	jeli.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
	bSuccess = SetInformationJobObject(hJob, JobObjectExtendedLimitInformation, &jeli, sizeof(jeli));

	if (AssignProcessToJobObject(hJob, GetCurrentProcess()) == FALSE) {
		DWORD Error = GetLastError();
	}

	CreateProcess
	(
		TEXT(path),
		NULL, NULL, NULL, FALSE,
		CREATE_NEW_CONSOLE,
		NULL, NULL,
		&si,
		&pi
	);
	BrowserPID = pi.dwProcessId;

	AssignProcessToJobObject(hJob, pi.hProcess);
	
}


JNIEXPORT jboolean JNICALL Java_com_github_manolo8_darkbot_core_DarkBotAPI_isValid(JNIEnv *env, jobject obj) {
	if (!BrowserWin) {
		BrowserWin = find_main_window(BrowserPID);
		return false;
	}
	if (!EnginePID) {
		EnginePID = find_process("QtWebEngineProcess.exe");
		TargetProcess = OpenProcess(PROCESS_ALL_ACCESS, false, EnginePID);
		return false;
	}

	maxmod = get_module("KERNELBASE.dll");
	puts("Ready!");
	printf("%llx\n", maxmod.dwBase);

	return true;
}


JNIEXPORT void JNICALL Java_com_github_manolo8_darkbot_core_DarkBotAPI_mousePress(JNIEnv *env, jobject obj, jint jx, jint jy) {
	char cmd[30];
	sprintf(cmd, "mousePress|%d|%d", (int)jx, (int)jy);
	send_command(cmd);
}


JNIEXPORT void JNICALL Java_com_github_manolo8_darkbot_core_DarkBotAPI_mouseMove(JNIEnv *env, jobject obj, jint jx, jint jy) {
	char cmd[30];
	sprintf(cmd, "mouseMove|%d|%d", (int)jx, (int)jy);
	send_command(cmd);
}


JNIEXPORT void JNICALL Java_com_github_manolo8_darkbot_core_DarkBotAPI_mouseRelease(JNIEnv *env, jobject obj, jint jx, jint jy) {
	char cmd[30];
	sprintf(cmd, "mouseRelease|%d|%d", (int)jx, (int)jy);
	send_command(cmd);
}


JNIEXPORT void JNICALL Java_com_github_manolo8_darkbot_core_DarkBotAPI_mouseClick(JNIEnv *env, jobject obj, jint jx, jint jy) {
	SendMessage(BrowserWin, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(jx, jy));
	SendMessage(BrowserWin, WM_LBUTTONUP, MK_LBUTTON, MAKELPARAM(jx, jy));
	Sleep(25);
}


JNIEXPORT void JNICALL Java_com_github_manolo8_darkbot_core_DarkBotAPI_keyboardClick(JNIEnv *env, jobject obj, jchar jkey) {
	char cmd[30];
	sprintf(cmd, "keyClick|%d", (int)jkey);
	send_command(cmd);
}


JNIEXPORT jdouble JNICALL Java_com_github_manolo8_darkbot_core_DarkBotAPI_readMemoryDouble(JNIEnv *env, jobject obj, jlong jaddr) {
	double val = read_memory<double>((LPCVOID)jaddr);
	return (jdouble)val;
}


JNIEXPORT jlong JNICALL Java_com_github_manolo8_darkbot_core_DarkBotAPI_readMemoryLong(JNIEnv *env, jobject obj, jlong jaddr) {
	return read_memory<UINT64>((LPCVOID)jaddr);
}


JNIEXPORT jint JNICALL Java_com_github_manolo8_darkbot_core_DarkBotAPI_readMemoryInt(JNIEnv *env, jobject obj, jlong jaddr) {
	return read_memory<int>((LPCVOID)jaddr);

}


JNIEXPORT jboolean JNICALL Java_com_github_manolo8_darkbot_core_DarkBotAPI_readMemoryBoolean(JNIEnv *env, jobject obj, jlong jaddr) {
	return read_memory<bool>((LPCVOID)jaddr);

}


JNIEXPORT jbyteArray JNICALL Java_com_github_manolo8_darkbot_core_DarkBotAPI_readMemory(JNIEnv *env, jobject obj, jlong jaddr, jint jsize) {
	unsigned char* stuff = read_memory_bytes((LPCVOID)jaddr, (int)jsize);
	jbyteArray barray = env->NewByteArray(jsize);
	env->SetByteArrayRegion(barray, 0, jsize, (jbyte*)(stuff));

	free(stuff);
	return barray;
}


JNIEXPORT void JNICALL Java_com_github_manolo8_darkbot_core_DarkBotAPI_writeMemoryDouble(JNIEnv *env, jobject obj, jlong jaddr, jdouble jvalue) {
	write_memory<double>(jaddr, jvalue);
}


JNIEXPORT void JNICALL Java_com_github_manolo8_darkbot_core_DarkBotAPI_writeMemoryLong(JNIEnv *env, jobject obj, jlong jaddr, jlong jvalue) {
	write_memory<long>(jaddr, jvalue);
}


JNIEXPORT void JNICALL Java_com_github_manolo8_darkbot_core_DarkBotAPI_writeMemoryInt(JNIEnv *env, jobject obj, jlong jaddr, jint jvalue) {
	write_memory<int>(jaddr, jvalue);
}


JNIEXPORT jlongArray JNICALL Java_com_github_manolo8_darkbot_core_DarkBotAPI_queryMemoryInt(JNIEnv *env, jobject obj, jint jvalue, jint jamount) {
	int found = 0;
	UINT64 *stuff;
	stuff = query_memory<int>(jvalue, 0, maxmod.dwBase, jamount, &found);
	
	jlongArray jarr = env->NewLongArray(found);
	env->SetLongArrayRegion(jarr, (jsize)0, (jsize)found, reinterpret_cast<jlong*>(stuff));
	free(stuff);
	return jarr;
}


JNIEXPORT jlongArray JNICALL Java_com_github_manolo8_darkbot_core_DarkBotAPI_queryMemoryLong(JNIEnv *env, jobject obj, jlong jvalue, jint jamount) {
	int found = 0;
	UINT64 *stuff;
	stuff = query_memory<UINT64>((UINT64)jvalue, 0, maxmod.dwBase, jamount, &found);

	jlongArray jarr = env->NewLongArray(found);
	env->SetLongArrayRegion(jarr, (jsize)0, (jsize)found, reinterpret_cast<jlong*>(stuff));

	free(stuff);
	return jarr;
}


JNIEXPORT jlongArray JNICALL Java_com_github_manolo8_darkbot_core_DarkBotAPI_queryMemory(JNIEnv *env, jobject obj, jbyteArray jvalue, jint jamount) {
	jlong arr[1];
	int len = env->GetArrayLength(jvalue);
	unsigned char* buf = new unsigned char[len];
	env->GetByteArrayRegion(jvalue, 0, len, reinterpret_cast<jbyte*>(buf));

	arr[0] = find_pattern(buf,0, maxmod.dwBase, len);
	int s = 1;
	if (!arr[0]) {
		s = 0;
	}
	jlongArray jarr = env->NewLongArray(s);

	env->SetLongArrayRegion(jarr, (jsize)0, (jsize)s, arr);

	delete(buf);
	return jarr;
}


JNIEXPORT void JNICALL Java_com_github_manolo8_darkbot_core_DarkBotAPI_refresh(JNIEnv *env, jobject obj) {
	char cmd[] = "refresh";
	send_command(cmd);
}

