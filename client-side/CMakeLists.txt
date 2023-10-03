project(CyberLighthouse-client)

add_subdirectory(src bin)

add_executable(clh-clnt clh-clnt.cpp)

target_link_libraries(clh-clnt client_required)