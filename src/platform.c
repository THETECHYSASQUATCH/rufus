/*
 * Rufus: The Reliable USB Formatting Utility
 * Platform abstraction implementation
 * Copyright © 2025 Rufus Contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "platform.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef PLATFORM_WINDOWS

/* Message box implementation for non-Windows platforms */
void MessageBoxA(HWND hWnd, const char* lpText, const char* lpCaption, unsigned int uType)
{
    printf("[%s] %s\n", lpCaption ? lpCaption : "Rufus", lpText ? lpText : "");
}

/* Volume information stub for non-Windows platforms */
BOOL GetVolumeInformationA(const char* lpRootPathName, char* lpVolumeNameBuffer, 
                          DWORD nVolumeNameSize, DWORD* lpVolumeSerialNumber,
                          DWORD* lpMaximumComponentLength, DWORD* lpFileSystemFlags,
                          char* lpFileSystemNameBuffer, DWORD nFileSystemNameSize)
{
    /* TODO: Implement using platform-specific APIs */
    if (lpVolumeNameBuffer && nVolumeNameSize > 0) {
        strncpy(lpVolumeNameBuffer, "UNKNOWN", nVolumeNameSize - 1);
        lpVolumeNameBuffer[nVolumeNameSize - 1] = '\0';
    }
    
    if (lpFileSystemNameBuffer && nFileSystemNameSize > 0) {
        strncpy(lpFileSystemNameBuffer, "ext4", nFileSystemNameSize - 1);
        lpFileSystemNameBuffer[nFileSystemNameSize - 1] = '\0';
    }
    
    if (lpVolumeSerialNumber) *lpVolumeSerialNumber = 0;
    if (lpMaximumComponentLength) *lpMaximumComponentLength = 255;
    if (lpFileSystemFlags) *lpFileSystemFlags = 0;
    
    return FALSE; /* Stub implementation */
}

/* File creation stub for non-Windows platforms */
HANDLE CreateFileA(const char* lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
                   void* lpSecurityAttributes, DWORD dwCreationDisposition,
                   DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
    /* TODO: Implement using platform-specific APIs (open, etc.) */
    return INVALID_HANDLE_VALUE;
}

/* Handle closing stub for non-Windows platforms */
BOOL CloseHandle(HANDLE hObject)
{
    /* TODO: Implement using platform-specific APIs (close, etc.) */
    return TRUE;
}

#endif /* !PLATFORM_WINDOWS */