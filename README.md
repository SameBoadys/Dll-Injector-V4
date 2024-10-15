###  DLL Injector Cheat C++ 


## Compile

1. Clone the project to your computer or download it as a ZIP file.
2. Open the solution file (.sln).
3. Select **Build Solution** from the **Build** menu or press `Ctrl+Shift+B` to compile the project.

## Usage

The injection module will download PDB files for `ntdll.dll` and its wow64 version on x64 systems to resolve symbol addresses. To start the download, use the `StartDownload` function. The injector can only operate after downloads are complete. Use `GetSymbolState` and `GetImportState` to check if downloads and address resolution are finished `(returning INJ_ERROR_SUCCESS (0))`. You can track download progress with `GetDownloadProgress`. If unloading the injection module during download, use `InterruptDownload` to avoid process deadlock.


## Features


<summary>Injection Methods</summary>

- **LoadLibraryExW**
  Description: Injects an external DLL file into the target process.

- **LdrLoadDll**
  Description: Loads and associates a DLL file into the target process.

- **LdrpLoadDll**
  Description: Manages DLL loading operations.

- **LdrpLoadDllInternal**
  Description: Manages DLL loading operations as an internal function.

- **ManualMapping**
  Description: Manually loads DLL code into the target process.



<summary>Shellcode Execution Methods</summary>

- **NtCreateThreadEx**
  Description: Injects code by creating a new thread.

- **Thread Hijacking**
  Description: Injects code by using an existing thread as the target.

- **SetWindowsHookEx**
  Description: Injects code using Windows event hooks.

- **QueueUserAPC**
  Description: Injects user-mode code into a thread.

- **KernelCallback**
  Description: Injects code using kernel callbacks.

- **FakeVEH**
  Description: Injects code by manipulating Virtual Exit Handlers.




<summary>Manual Mapping Features</summary>

- **Section Mapping**
  Description: Maps DLL sections into the target process's memory.

- **Base Relocation**
  Description: Places DLL code in the target process's memory appropriately.

- **Imports**
  Description: Properly resolves dependent DLLs.

- **Delayed Imports**
  Description: Handles dependencies in a delayed manner.

- **SEH Support**
  Description: Provides Structured Exception Handler support.

- **TLS Initialization**
  Description: Initializes Thread Local Storage.

- **Security Cookie Initialization**
  Description: Initializes the security cookie.

- **Loader Lock**
  Description: Provides synchronization using the loader lock.

- **Shift Image**
  Description: Injects code by redirecting image loading.

- **Clean Data Directories**
  Description: Cleans data directories, removing unnecessary information.
  


## Preview 

![image](https://user-images.githubusercontent.com/105746452/169072886-9292af6d-f26a-42ae-b23b-282f18b19255.png)

## Disclaimer 

This source code is for educational purposes only. It's created to explore reverse engineering without impacting other gamers' experiences.

## License

This project is licensed under the MIT. For more information, see the [License](LICENSE).