

set(SRC_BUILD_FILES
"AssetImporter.h"
"Protocal/ImportScene.h"
"FbxImporter/FbxImporter.h"
"AssetImporter.cpp"
"Protocal/ImportScene.cpp"
"FbxImporter/FbxImporter.cpp")

add_executable(AssetImporter ${SRC_BUILD_FILES})

 source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
         FILES ${SRC_BUILD_FILES})