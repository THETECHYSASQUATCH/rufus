/*
 * Rufus: The Reliable USB Formatting Utility
 * Platform detection and abstraction layer
 * Copyright © 2025 Rufus Contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#pragma once

/* Platform detection */
#if defined(_WIN32) || defined(_WIN64)
#define PLATFORM_WINDOWS
#elif defined(__APPLE__)
#define PLATFORM_MACOS
#include <TargetConditionals.h>
#elif defined(__linux__)
#define PLATFORM_LINUX
#elif defined(__unix__) || defined(__unix)
#define PLATFORM_UNIX
#else
#define PLATFORM_UNKNOWN
#endif

/* Architecture detection for non-Windows platforms */
#if !defined(PLATFORM_WINDOWS)
#if defined(__x86_64__) || defined(__amd64__)
#define APPLICATION_ARCH "x64"
#elif defined(__i386__)
#define APPLICATION_ARCH "x86"
#elif defined(__aarch64__)
#define APPLICATION_ARCH "ARM64"
#elif defined(__arm__)
#define APPLICATION_ARCH "ARM"
#else
#define APPLICATION_ARCH "(Unknown Arch)"
#endif
#endif

/* Feature availability */
#ifdef PLATFORM_WINDOWS
#define HAS_WINDOWS_UI
#define HAS_DIRECT_DISK_ACCESS
#define HAS_WINDOWS_REGISTRY
#define HAS_WINDOWS_APIS
#endif

#ifdef PLATFORM_MACOS
#define HAS_UNIX_APIS
#define HAS_COCOA_UI
#define HAS_DISK_ARBITRATION
#endif

#ifdef PLATFORM_LINUX
#define HAS_UNIX_APIS
#define HAS_GTK_UI
#define HAS_UDEV
#define HAS_BLOCK_DEVICE_ACCESS
#endif

/* Common includes based on platform */
#ifdef PLATFORM_WINDOWS
#include <windows.h>
#include <windowsx.h>
#include <winioctl.h>
#include <shlobj.h>
#include <process.h>
#include <dwmapi.h>
#include <dbt.h>
#include <io.h>
#endif

#ifdef PLATFORM_MACOS
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#endif

#ifdef PLATFORM_LINUX
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <linux/fs.h>
#endif

/* Type definitions for compatibility */
#ifndef PLATFORM_WINDOWS
typedef int BOOL;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef void* HANDLE;
typedef void* HWND;
typedef void* HINSTANCE;
typedef void* HIMAGELIST;
typedef struct { long left, top, right, bottom; } RECT;

#define TRUE 1
#define FALSE 0
#define INVALID_HANDLE_VALUE ((HANDLE)-1)
#define MAX_PATH 260

/* Common Windows constants */
#define MB_OK                0x00000000L
#define MB_ICONINFORMATION   0x00000040L
#define MB_ICONWARNING       0x00000030L
#define MB_ICONERROR         0x00000010L
#define MB_YESNO             0x00000004L
#define IDYES                6
#define IDNO                 7

/* File attributes */
#define FILE_ATTRIBUTE_HIDDEN     0x00000002
#define FILE_ATTRIBUTE_DIRECTORY  0x00000010
#endif

/* Function declarations for cross-platform abstraction */
#ifndef PLATFORM_WINDOWS
/* These functions need to be implemented for non-Windows platforms */
void MessageBoxA(HWND hWnd, const char* lpText, const char* lpCaption, unsigned int uType);
BOOL GetVolumeInformationA(const char* lpRootPathName, char* lpVolumeNameBuffer, 
                          DWORD nVolumeNameSize, DWORD* lpVolumeSerialNumber,
                          DWORD* lpMaximumComponentLength, DWORD* lpFileSystemFlags,
                          char* lpFileSystemNameBuffer, DWORD nFileSystemNameSize);
HANDLE CreateFileA(const char* lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
                   void* lpSecurityAttributes, DWORD dwCreationDisposition,
                   DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
BOOL CloseHandle(HANDLE hObject);

/* Stub implementations for now - to be replaced with actual platform code */
#define RegOpenKeyExA(...)     FALSE
#define RegQueryValueExA(...)  FALSE
#define RegSetValueExA(...)    FALSE
#define RegCloseKey(...)       
#define RegCreateKeyExA(...)   FALSE
#define RegDeleteValueA(...)   FALSE

#define GetLogicalDrives()     0
#define GetDriveTypeA(...)     0
#define GetVolumePathNameA(...) FALSE

/* UI stubs for command-line interface */
#define SendMessage(...)       0
#define SetWindowTextA(...)    
#define GetWindowTextA(...)    0
#define EnableWindow(...)      
#define ShowWindow(...)        
#define UpdateWindow(...)      
#define PostMessage(...)       
#define SetFocus(...)          
#define GetDlgItem(...)        NULL
#define CheckDlgButton(...)    
#define IsDlgButtonChecked(...) FALSE

#define WM_COMMAND             0x0111
#define WM_DEVICECHANGE        0x0219
#define BN_CLICKED             0

#endif /* !PLATFORM_WINDOWS */