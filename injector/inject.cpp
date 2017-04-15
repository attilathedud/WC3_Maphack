/*!
*   All code related to injection. This should probably be encapsulated in a class, but that's
*   why you don't program as a teenager.
*
*   Originally written 2010/02/16 by attilathedud.
*/

#include <windows.h>

/*!
*   When Blizzard released patch 1.15.1 for Starcraft: Brood War, they introduced a new method
*   of anti-hack protection that was intended as a low-wall to keep out inexperienced hackers.
*   All this new method did was ensure that any process touching a Blizzard process had an elevated 
*   privilege (SE_DEBUG).
*/
void AdjustTokenPrivs( void )
{
    HANDLE token;
    LUID luid;
    TOKEN_PRIVILEGES tp;

    OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &token);
    LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid);

    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    tp.Privileges[0].Attributes = 2;

    AdjustTokenPrivileges(token, FALSE, &tp, 28, NULL, NULL);
}

/*!
*   Our injection function works by the following process:
*
*   1. Open a process handle to Warcraft 3. Note that finding a process via FindWindow is 
*       considered a pretty terrible method.
*   2. Allocate and write our dlls path into the process' memory space.
*   3. Create a thread inside WC3 that will invoke LoadLibrary along with a parameter to our dll's name
*       inside the process.
*/
bool inject(char* dll)
{
    DWORD exitCode, thread;
    void *lpBaseAddress, *process;

    // Adjust our applications privilege to SE_DEBUG.
    AdjustTokenPrivs();
	
    // Get WC3's process handle.
    GetWindowThreadProcessId(FindWindow(NULL, "Warcraft III"), (DWORD*)&process);
    process = OpenProcess(PROCESS_ALL_ACCESS, true, (DWORD)process);

    if(!process)
        return false;

    // Allocate the space for our dll name inside the process.
    if(!(lpBaseAddress =  VirtualAllocEx(process, NULL, strlen(dll) + 1, MEM_COMMIT, PAGE_READWRITE)))
        return false;

    // Write our dll's name inside the process.
    WriteProcessMemory(process, lpBaseAddress, dll, strlen(dll)+1, NULL);

    // Create our thread that will invoke LoadLibrary on our dll name.
    thread = (DWORD)CreateRemoteThread(process, 0, 0, (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA"), lpBaseAddress,0, 0);
    WaitForSingleObject((void*)thread, INFINITE);
    GetExitCodeThread((void*)thread, &exitCode);

    if(!exitCode)
        return false;

    // Free the memory we allocated and close active handles.
    VirtualFreeEx(process, 0, strlen(dll)+1, MEM_DECOMMIT);
    CloseHandle((void*)thread);
    CloseHandle(process);

    return true;
}