# CMakeList.txt: dxf_reader 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

# 如果支持，请为 MSVC 编译器启用热重载。
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("dxf_reader")

 # 源文件列表
set(PROJECT_SOURCES
    main.cpp
    dxfreader.cpp
    dxfreader.h
    dxflib/dl_attributes.h
    dxflib/dl_codes.h
    dxflib/dl_creationadapter.h
    dxflib/dl_creationinterface.h
    dxflib/dl_dxf.cpp
    dxflib/dl_dxf.h
    dxflib/dl_entities.h
    dxflib/dl_exception.h
    dxflib/dl_extrusion.h
    dxflib/dl_global.h
    dxflib/dl_writer.h
    dxflib/dl_writer_ascii.cpp
    dxflib/dl_writer_ascii.h
)

# 将源代码添加到此项目的可执行文件。
add_executable (dxf_reader ${PROJECT_SOURCES} "main.cpp" "dxfreader.cpp" "dxfreader.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET dxf_reader PROPERTY CXX_STANDARD 20)
endif()


add_custom_command(
    TARGET dxf_reader POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/test.dxf"
        "$<TARGET_FILE_DIR:dxf_reader>"
    COMMENT "Copying test.dxf to executable directory"
)