set(SRC_BUILD_FILES
"main.cpp"
"Milia/MiliaScript.h"
"Milia/MiliaScript.cpp"
"Milia/MiliaASM.h"
"Milia/MiliaASM.cpp"
"Sice/SiceScript.h"
"Sice/SiceScript.cpp"
"Sice/SiceASM.h"
"Sice/SiceASM.cpp"
"Interfaces/IHitable.h")

add_executable(StandAlone ${SRC_BUILD_FILES})

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
         FILES ${SRC_BUILD_FILES})

target_include_directories(StandAlone PUBLIC ${PROJECT_SOURCE_DIR}/Source/InceptionEngine ${PROJECT_SOURCE_DIR}/Source/StandAlone)

target_link_libraries(StandAlone InceptionEngine)
IF (CMAKE_SYSTEM_NAME MATCHES "Windows")
add_custom_command(TARGET StandAlone POST_BUILD 
 COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        "${PROJECT_BINARY_DIR}/Source/InceptionEngine/Debug/InceptionEngine.dll"
        "${PROJECT_BINARY_DIR}/Source/StandAlone/Debug")
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Windows")
