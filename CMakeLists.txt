# CMakeList.txt: MyRISC-VCore_Model_CMake 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.12)

project ("MyRISC-VCore_Model")

include_directories(./src/top_header)

# 将源代码添加到此项目的可执行文件。
file(GLOB_RECURSE src_files CONFIGURE_DEPENDS "*.cpp" "*.h")
add_executable(sim ${src_files})

# TODO: 如有需要，请添加测试并安装目标。
