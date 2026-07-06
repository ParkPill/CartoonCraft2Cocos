#ifndef __MAC_FILE_DIALOG_H__
#define __MAC_FILE_DIALOG_H__

#include <string>

// Native macOS NSSavePanel/NSOpenPanel wrappers for the Map Editor, mirroring
// the win32ShowSaveDialog/win32ShowOpenDialog helpers in MapEditor.cpp so
// Save/Save As/Load behave the same way on Mac as they do on Win32.
// Both are modal (blocking) and return the chosen absolute path, or "" if
// the user cancelled.
std::string macShowSaveDialog(const std::string &defaultName);
std::string macShowOpenDialog();

#endif
