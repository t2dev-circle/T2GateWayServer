cmake_minimum_required(VERSION 2.8)

project(T2App-T2GateWayServer)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/t2sdk/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/t2sdk/lib)

gen_server(T2App T2GateWayServer)

#target_link_libraries(mysqlclient ssl crypto nghttp2-static)
target_link_libraries(T2GateWayServer t2sdk)