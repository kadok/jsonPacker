cmake_minimum_required(VERSION 3.19)
project(jsonPacker C)

set(CMAKE_C_STANDARD 11)
include_directories(/usr/local/apr/include/apr-1/ /usr/local/include/cjson/)
link_directories(/usr/local/apr/lib/ /usr/local/lib/)

#set ( PROJECT_LINK_LIBS libcjson.so )

add_executable(jsonPacker main.c nxlog.h nxlog.c tlv_box.h tlv_box.c key_list.h key_list.c log.c log.h)
#target_link_libraries(jsonPacker ${PROJECT_LINK_LIBS} )
target_link_libraries(jsonPacker -lapr-1 -lpthread -lcjson)
