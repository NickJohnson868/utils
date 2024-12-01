cmake_minimum_required(VERSION 3.22)
project(utils)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 20)

# 添加可执行目标
add_executable(utils src/main.cpp
        src/util/util.cpp
        src/util/_string.h
        src/util/_string.cpp
        src/util/util.h
        src/util/_math.h
        src/util/_math.cpp
        src/util/_time.h
        src/util/_time.cpp
        src/util/_file.h
        src/util/_file.cpp
        src/3rd/cpp-base64/base64.cpp
        src/util/_proc.h
        src/util/_proc.cpp
        src/util/_registry.h
        src/util/_registry.cpp
        src/util/_inject.h
        src/util/_inject.cpp
        src/test.h
        src/test_daffodils.cpp
        src/test_json.cpp
        src/rename.cpp
        src/rename.h
        src/pixel_on_cmd.cpp
        src/image.hpp
        src/pixel_on_cmd.h
        src/util/macro.h)

set_target_properties(utils PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

# 添加头文件搜索路径
target_include_directories(utils PRIVATE
        ${CMAKE_SOURCE_DIR}/src/3rd/cpp-base64
        ${CMAKE_SOURCE_DIR}/src/3rd/nlohmann/include
        ${CMAKE_SOURCE_DIR}/src/3rd/OpenXLSX/include
        ${CMAKE_SOURCE_DIR}/src/3rd/OpenXLSX/include/headers)

# 判断架构类型
if (CMAKE_CXX_FLAGS MATCHES "-m32")
    set(ARCH_DIR "x86")  # 如果编译器标志中包含 -m32，则设置为 x86 架构
elseif (CMAKE_CXX_FLAGS MATCHES "-m64")
    set(ARCH_DIR "x64")  # 如果编译器标志中包含 -m64，则设置为 x64 架构
else ()
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(ARCH_DIR "x64")  # 默认情况下，如果指针大小为 8 字节，设置为 x64
    else ()
        set(ARCH_DIR "x86")  # 如果指针大小为 4 字节，设置为 x86
    endif ()
endif ()

# 设置库文件路径
set(LIBRARY_DIR "${CMAKE_SOURCE_DIR}/src/3rd/OpenXLSX/lib/${ARCH_DIR}")

# 添加库文件路径
target_link_directories(utils PRIVATE ${LIBRARY_DIR})

# 根据操作系统选择静态库文件
if (WIN32)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(LIBRARY_PATH "${LIBRARY_DIR}/libOpenXLSXd_win.a")
    else ()
        set(LIBRARY_PATH "${LIBRARY_DIR}/libOpenXLSX_win.a")  # Release 版本的 .lib 文件
    endif ()
else ()
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(LIBRARY_PATH "${LIBRARY_DIR}/libOpenXLSXd.a")  # Debug 版本的 .a 文件
    else ()
        set(LIBRARY_PATH "${LIBRARY_DIR}/libOpenXLSX.a")  # Release 版本的 .a 文件
    endif ()
endif ()

# 链接静态库（指定完整路径）
target_link_libraries(utils PRIVATE ${LIBRARY_PATH})

if (WIN32)
    target_link_libraries(utils PRIVATE winmm)
endif ()