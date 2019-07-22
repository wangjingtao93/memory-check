cmake_minimum_required(VERSION 3.2.0)#版本。可以不加，建议加上

#project()#工程名，建议加上,这一行会引入两个变量XXX_BINARY_DIR (二进制文件保存路径)和 XXX_SOURCE_DIR(源代码保存路径)

#add_definitions(-std=c++11)#确定编译语言，也可以用set(CMAKE_CXX_STANDARD 11),默认什么语言都支持
#add_definitions(-g -o2)

#define cuda,opencv,cudnn，设定变量如darknet中代码编译需要define变量GPU，CUDNN，OPENCV等，则用该语句进行定义
ADD_DEFINITIONS( -DGPU -DCUDNN ) 
set(CMAKE_C_FLAGS "-g -O2 -fsanitize=address -fno-omit-frame-pointer -fsanitize=address")

# headers
include_directories(${PROJECT_SOURCE_DIR}/)

#sources，通过设定SRC变量，将源代码路径都给SRC，如果有多个，可以直接在后面继续添加，
set(SRC  
  ${PROJECT_SOURCE_DIR}/main.c
  ${PROJECT_SOURCE_DIR}/fun1.c
  ${PROJECT_SOURCE_DIR}/fun2.c
  )

#build so
link_directories(${PROJECT_SOURCE_DIR})
add_executable(Test ${SRC})

