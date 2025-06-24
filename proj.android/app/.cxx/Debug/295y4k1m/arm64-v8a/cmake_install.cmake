# Install script for directory: /Users/rightpill-2016/Dev/CocosProjects/CartoonCraftNV

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/Users/rightpill-2016/Dev/Sdks/android-ndk-r21b/toolchains/llvm/prebuilt/darwin-x86_64/bin/llvm-objdump")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/Users/rightpill-2016/Dev/CocosProjects/CartoonCraftNV/proj.android/app/.cxx/Debug/295y4k1m/arm64-v8a/engine/external/recast/cmake_install.cmake")
  include("/Users/rightpill-2016/Dev/CocosProjects/CartoonCraftNV/proj.android/app/.cxx/Debug/295y4k1m/arm64-v8a/engine/external/tinyxml2/cmake_install.cmake")
  include("/Users/rightpill-2016/Dev/CocosProjects/CartoonCraftNV/proj.android/app/.cxx/Debug/295y4k1m/arm64-v8a/engine/external/android-specific/pvmp3dec/cmake_install.cmake")
  include("/Users/rightpill-2016/Dev/CocosProjects/CartoonCraftNV/proj.android/app/.cxx/Debug/295y4k1m/arm64-v8a/engine/external/android-specific/tremolo/cmake_install.cmake")
  include("/Users/rightpill-2016/Dev/CocosProjects/CartoonCraftNV/proj.android/app/.cxx/Debug/295y4k1m/arm64-v8a/engine/external/unzip/cmake_install.cmake")
  include("/Users/rightpill-2016/Dev/CocosProjects/CartoonCraftNV/proj.android/app/.cxx/Debug/295y4k1m/arm64-v8a/engine/external/flatbuffers/cmake_install.cmake")
  include("/Users/rightpill-2016/Dev/CocosProjects/CartoonCraftNV/proj.android/app/.cxx/Debug/295y4k1m/arm64-v8a/engine/external/xxhash/cmake_install.cmake")
  include("/Users/rightpill-2016/Dev/CocosProjects/CartoonCraftNV/proj.android/app/.cxx/Debug/295y4k1m/arm64-v8a/engine/cocos/core/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/Users/rightpill-2016/Dev/CocosProjects/CartoonCraftNV/proj.android/app/.cxx/Debug/295y4k1m/arm64-v8a/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
