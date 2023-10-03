project(CyberLighthouse-server)

add_subdirectory(src bin)

add_executable(clh clh.cpp)

target_link_libraries(clh server_required)