add_executable(StandAlone main.cpp)

target_include_directories(StandAlone PUBLIC ${PROJECT_SOURCE_DIR}/Source/InceptionEngine)

target_link_libraries(StandAlone InceptionEngine)

add_custom_command(TARGET StandAlone POST_BUILD 
 COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        "${PROJECT_BINARY_DIR}/Source/InceptionEngine/Debug/InceptionEngine.dll"
        "${PROJECT_BINARY_DIR}/Source/StandAlone/Debug")

 add_custom_command(TARGET StandAlone POST_BUILD 
 COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        "${PROJECT_SOURCE_DIR}/Source/InceptionEngine/External/fbxsdk/lib/x64/debug/libfbxsdk.dll"
        "${PROJECT_BINARY_DIR}/Source/StandAlone/Debug")