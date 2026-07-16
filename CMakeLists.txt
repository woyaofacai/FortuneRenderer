cmake_minimum_required(VERSION 3.15)
project(FortuneRenderer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# minifb 子项目：关闭其自带的 examples 与 version_info，避免被纳入本项目构建
set(MINIFB_BUILD_EXAMPLES OFF CACHE BOOL "Build minifb example programs" FORCE)
set(MINIFB_BUILD_VERSION_INFO OFF CACHE BOOL "Build version info utility" FORCE)
add_subdirectory(minifb)

# 平台相关的系统库
# 说明：opengl32 / winmm 等 Win32 库仅存在于 Windows，且已在 minifb 内部按平台链接；
# 这里只在 Windows 上补充 Chapter 程序可能会用到的 GUI / 系统库，Linux/macOS 保持为空。
if (WIN32)
    set(PLATFORM_LIBS
        opengl32
        winmm
        kernel32
        user32
        gdi32
        winspool
        shell32
        ole32
        oleaut32
        uuid
        comdlg32
        advapi32
    )
else()
    set(PLATFORM_LIBS "")
endif()

# 遍历 source 下的 Chapter* 目录，每个目录生成一个 exe
file(GLOB CHAPTER_DIRS RELATIVE "${CMAKE_SOURCE_DIR}/source"
    "${CMAKE_SOURCE_DIR}/source/Chapter*"
)

foreach(chapter ${CHAPTER_DIRS})
    if(IS_DIRECTORY "${CMAKE_SOURCE_DIR}/source/${chapter}")
        file(GLOB_RECURSE chapter_sources CONFIGURE_DEPENDS
            "${CMAKE_SOURCE_DIR}/source/${chapter}/*.cpp"
            "${CMAKE_SOURCE_DIR}/source/${chapter}/*.h"
            "${CMAKE_SOURCE_DIR}/source/${chapter}/*.hpp"
        )

        # 没有源文件就跳过
        if(NOT chapter_sources)
            message(STATUS "Skip ${chapter}: no source files")
            continue()
        endif()

        # 可执行程序名就用目录名：Chapter01, Chapter02, ...
        add_executable(${chapter} ${chapter_sources})

        target_include_directories(${chapter} PRIVATE
            "${CMAKE_SOURCE_DIR}/minifb/include"
            "${CMAKE_SOURCE_DIR}/glm"
        )

        target_link_libraries(${chapter} PRIVATE
            minifb
            ${PLATFORM_LIBS}
        )
    endif()
endforeach()

