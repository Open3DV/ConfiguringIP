# cmake的最低版本要求
cmake_minimum_required(VERSION 3.1.0)

# 设置项目名称
set(PRO_NAME configuring_ip) 

project(${PRO_NAME}) 
# 在相应的构建目录中查找头文件
set(CMAKE_INCLUDE_CURRENT_DIR ON) 
# 搜索当前目录下的源文件
aux_source_directory(. DIR_SRCS) 

# 设置可执行文件
add_executable(${PRO_NAME} ${DIR_SRCS})
 
