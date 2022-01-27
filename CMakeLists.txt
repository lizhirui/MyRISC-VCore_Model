# CMakeList.txt: MyRISC-VCore_Model_CMake 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.12)

project ("MyRISC-VCore_Model")

include_directories(./src/top_header)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# 将源代码添加到此项目的可执行文件。
file(GLOB_RECURSE src_files CONFIGURE_DEPENDS "./src/*.cpp" "./src/*.h")
add_executable(sim ${src_files})

# TODO: 如有需要，请添加测试并安装目标。
add_custom_target(run COMMAND sim DEPENDS sim WORKING_DIRECTORY ${CMAKE_PROJECT_DIR})
