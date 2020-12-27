

set(SRC_BUILD_FILES
"FbxImporter.h"
"FbxImporter.cpp"
"main.cpp"
)

add_executable(AssetImporter ${SRC_BUILD_FILES})

target_compile_options(AssetImporter PUBLIC /wd26812 /wd26451 /wd26495)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
         FILES ${SRC_BUILD_FILES})

target_include_directories(AssetImporter PUBLIC 
${PROJECT_SOURCE_DIR}/Source/InceptionEngine
${PROJECT_SOURCE_DIR}/Source/InceptionBase
${PROJECT_SOURCE_DIR}/ThirdParty/ThirdPartyLibraries/fbxsdk/include)

target_link_libraries(AssetImporter 
InceptionBase 
${PROJECT_SOURCE_DIR}/ThirdParty/ThirdPartyLibraries/fbxsdk/lib/x64/debug/libfbxsdk.lib)

add_custom_command(TARGET AssetImporter POST_BUILD 
COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        "${PROJECT_SOURCE_DIR}/ThirdParty/ThirdPartyLibraries/fbxsdk/lib/x64/debug/libfbxsdk.dll"
        "${PROJECT_BINARY_DIR}/Source/AssetImporter/Debug")