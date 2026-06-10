#include "Win32MouseCursor.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <algorithm>
#include <cstring>
#include <vector>

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

#include "platform/desktop/CCGLViewImpl-desktop.h"

USING_NS_CC;

namespace {

constexpr int kMaxCursorSize = 64;

HCURSOR s_activeCursor = nullptr;
HWND s_hookedHwnd = nullptr;
WNDPROC s_originalWndProc = nullptr;

LRESULT CALLBACK CursorWndProc(HWND hwnd, UINT msg, WPARAM wParam,
                               LPARAM lParam);

void destroyActiveCursor() {
  if (s_activeCursor != nullptr) {
    DestroyCursor(s_activeCursor);
    s_activeCursor = nullptr;
  }
}

HWND getWin32Hwnd() {
  auto *glview =
      dynamic_cast<GLViewImpl *>(Director::getInstance()->getOpenGLView());
  if (glview == nullptr) {
    return nullptr;
  }
  return glview->getWin32Window();
}

void refreshCursorOverClient(HWND hwnd) {
  if (hwnd == nullptr) {
    return;
  }
  POINT pt = {};
  GetCursorPos(&pt);
  ScreenToClient(hwnd, &pt);
  SendMessage(hwnd, WM_SETCURSOR, reinterpret_cast<WPARAM>(hwnd),
              MAKELPARAM(HTCLIENT, WM_MOUSEMOVE));
}

void uninstallWndProcHook() {
  if (s_hookedHwnd == nullptr || s_originalWndProc == nullptr) {
    s_hookedHwnd = nullptr;
    s_originalWndProc = nullptr;
    return;
  }

  WNDPROC current =
      reinterpret_cast<WNDPROC>(GetWindowLongPtrW(s_hookedHwnd, GWLP_WNDPROC));
  if (current == reinterpret_cast<WNDPROC>(CursorWndProc)) {
    SetWindowLongPtrW(s_hookedHwnd, GWLP_WNDPROC,
                      reinterpret_cast<LONG_PTR>(s_originalWndProc));
  }

  s_hookedHwnd = nullptr;
  s_originalWndProc = nullptr;
}

void installWndProcHook(HWND hwnd) {
  if (hwnd == nullptr) {
    return;
  }

  WNDPROC current =
      reinterpret_cast<WNDPROC>(GetWindowLongPtrW(hwnd, GWLP_WNDPROC));
  if (current == reinterpret_cast<WNDPROC>(CursorWndProc)) {
    s_hookedHwnd = hwnd;
    return;
  }

  if (s_hookedHwnd != nullptr && s_hookedHwnd != hwnd) {
    uninstallWndProcHook();
  }

  s_originalWndProc = current;
  SetWindowLongPtrW(hwnd, GWLP_WNDPROC,
                    reinterpret_cast<LONG_PTR>(CursorWndProc));
  s_hookedHwnd = hwnd;
}

LRESULT CALLBACK CursorWndProc(HWND hwnd, UINT msg, WPARAM wParam,
                               LPARAM lParam) {
  if (msg == WM_SETCURSOR) {
    if (LOWORD(lParam) == HTCLIENT && s_activeCursor != nullptr) {
      SetCursor(s_activeCursor);
      return TRUE;
    }
  }

  if (s_originalWndProc != nullptr) {
    return CallWindowProcW(s_originalWndProc, hwnd, msg, wParam, lParam);
  }
  return DefWindowProcW(hwnd, msg, wParam, lParam);
}

std::vector<unsigned char> resizeRgbaNearest(const unsigned char *src, int srcW,
                                             int srcH, int dstW, int dstH) {
  std::vector<unsigned char> dst(static_cast<size_t>(dstW) * dstH * 4);
  for (int y = 0; y < dstH; ++y) {
    int srcY = y * srcH / dstH;
    for (int x = 0; x < dstW; ++x) {
      int srcX = x * srcW / dstW;
      const int srcIdx = (srcY * srcW + srcX) * 4;
      const int dstIdx = (y * dstW + x) * 4;
      dst[dstIdx + 0] = src[srcIdx + 0];
      dst[dstIdx + 1] = src[srcIdx + 1];
      dst[dstIdx + 2] = src[srcIdx + 2];
      dst[dstIdx + 3] = src[srcIdx + 3];
    }
  }
  return dst;
}

std::vector<unsigned char> fitCursorImage(const unsigned char *pixels,
                                          int width, int height, int &outW,
                                          int &outH) {
  outW = width;
  outH = height;
  if (width <= kMaxCursorSize && height <= kMaxCursorSize) {
    return std::vector<unsigned char>(
        pixels, pixels + static_cast<size_t>(width) * height * 4);
  }

  const float scale = std::min(static_cast<float>(kMaxCursorSize) / width,
                               static_cast<float>(kMaxCursorSize) / height);
  outW = std::max(1, static_cast<int>(width * scale));
  outH = std::max(1, static_cast<int>(height * scale));
  return resizeRgbaNearest(pixels, width, height, outW, outH);
}

HCURSOR createWin32CursorFromRgba(const unsigned char *rgba, int width,
                                  int height, int hotX, int hotY) {
  if (rgba == nullptr || width <= 0 || height <= 0) {
    return nullptr;
  }

  hotX = std::max(0, std::min(hotX, width - 1));
  hotY = std::max(0, std::min(hotY, height - 1));

  BITMAPV5HEADER colorHeader = {};
  colorHeader.bV5Size = sizeof(BITMAPV5HEADER);
  colorHeader.bV5Width = width;
  colorHeader.bV5Height = -height;
  colorHeader.bV5Planes = 1;
  colorHeader.bV5BitCount = 32;
  colorHeader.bV5Compression = BI_BITFIELDS;
  colorHeader.bV5RedMask = 0x00FF0000;
  colorHeader.bV5GreenMask = 0x0000FF00;
  colorHeader.bV5BlueMask = 0x000000FF;
  colorHeader.bV5AlphaMask = 0xFF000000;

  HDC screenDc = GetDC(nullptr);
  void *colorBits = nullptr;
  HBITMAP colorBitmap =
      CreateDIBSection(screenDc, reinterpret_cast<BITMAPINFO *>(&colorHeader),
                       DIB_RGB_COLORS, &colorBits, nullptr, 0);

  const int maskStride = ((width + 31) / 32) * 4;
  std::vector<unsigned char> maskBits(static_cast<size_t>(maskStride) * height,
                                      0xFF);

  if (colorBitmap == nullptr || colorBits == nullptr) {
    ReleaseDC(nullptr, screenDc);
    CCLOG("Win32MouseCursor: CreateDIBSection failed, error=%lu",
          GetLastError());
    return nullptr;
  }

  auto *dst = static_cast<unsigned char *>(colorBits);
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      const int i = y * width + x;
      const unsigned char r = rgba[i * 4 + 0];
      const unsigned char g = rgba[i * 4 + 1];
      const unsigned char b = rgba[i * 4 + 2];
      const unsigned char a = rgba[i * 4 + 3];

      dst[i * 4 + 0] = b;
      dst[i * 4 + 1] = g;
      dst[i * 4 + 2] = r;
      dst[i * 4 + 3] = a;

      const int byteIndex = y * maskStride + (x / 8);
      const int bitIndex = 7 - (x % 8);
      if (a < 128) {
        maskBits[static_cast<size_t>(byteIndex)] |= (1 << bitIndex);
      } else {
        maskBits[static_cast<size_t>(byteIndex)] &= ~(1 << bitIndex);
      }
    }
  }

  ReleaseDC(nullptr, screenDc);

  HBITMAP maskBitmap = CreateBitmap(width, height, 1, 1, maskBits.data());
  if (maskBitmap == nullptr) {
    DeleteObject(colorBitmap);
    CCLOG("Win32MouseCursor: CreateBitmap(mask) failed, error=%lu",
          GetLastError());
    return nullptr;
  }

  ICONINFO iconInfo = {};
  iconInfo.fIcon = FALSE;
  iconInfo.xHotspot = hotX;
  iconInfo.yHotspot = hotY;
  iconInfo.hbmMask = maskBitmap;
  iconInfo.hbmColor = colorBitmap;

  HCURSOR cursor = CreateIconIndirect(&iconInfo);

  DeleteObject(colorBitmap);
  DeleteObject(maskBitmap);

  if (cursor == nullptr) {
    CCLOG("Win32MouseCursor: CreateIconIndirect failed, error=%lu",
          GetLastError());
  }

  return cursor;
}

bool applyWin32Cursor(HCURSOR cursor) {
  if (cursor == nullptr) {
    return false;
  }

  HWND hwnd = getWin32Hwnd();
  if (hwnd == nullptr) {
    DestroyCursor(cursor);
    return false;
  }

  installWndProcHook(hwnd);
  destroyActiveCursor();
  s_activeCursor = cursor;
  SetCursor(cursor);
  refreshCursorOverClient(hwnd);
  return true;
}

bool setCustomCursor(const std::string &filename, int hotX, int hotY) {
  std::string fullPath =
      FileUtils::getInstance()->fullPathForFilename(filename);
  if (fullPath.empty()) {
    CCLOG("Win32MouseCursor: file not found: %s", filename.c_str());
    return false;
  }

  int srcW = 0;
  int srcH = 0;
  int channels = 0;
  unsigned char *pixels =
      stbi_load(fullPath.c_str(), &srcW, &srcH, &channels, 4);
  if (pixels == nullptr) {
    CCLOG("Win32MouseCursor: failed to load image: %s", fullPath.c_str());
    return false;
  }

  int cursorW = 0;
  int cursorH = 0;
  std::vector<unsigned char> rgba =
      fitCursorImage(pixels, srcW, srcH, cursorW, cursorH);
  stbi_image_free(pixels);

  if (srcW != cursorW || srcH != cursorH) {
    hotX = hotX * cursorW / srcW;
    hotY = hotY * cursorH / srcH;
  }

  HCURSOR cursor =
      createWin32CursorFromRgba(rgba.data(), cursorW, cursorH, hotX, hotY);
  if (!applyWin32Cursor(cursor)) {
    if (cursor != nullptr) {
      DestroyCursor(cursor);
    }
    return false;
  }

  CCLOG("Win32MouseCursor: applied %s (%dx%d -> %dx%d) hotspot (%d,%d)",
        fullPath.c_str(), srcW, srcH, cursorW, cursorH, hotX, hotY);
  return true;
}

} // namespace

bool Win32MouseCursor::setCursor(const std::string &filename, int hotspotX,
                                 int hotspotY) {
  return setCustomCursor(filename, hotspotX, hotspotY);
}

void Win32MouseCursor::setDefaultCursor() {
  if (!setCursor("cursor.png", 4, 4)) {
    restoreSystemCursor();
  }
}

void Win32MouseCursor::setAttackMoveCursor() {
  if (!setCursor("cursor_attack.png", 4, 4)) {
    setCursor("swordIcon.png", 0, 0);
  }
}

void Win32MouseCursor::restoreSystemCursor() {
  HWND hwnd = getWin32Hwnd();
  destroyActiveCursor();
  if (hwnd != nullptr) {
    refreshCursorOverClient(hwnd);
  }
}

#endif
