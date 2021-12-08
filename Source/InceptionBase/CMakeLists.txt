

set(SRC_BUILD_FILES
"EngineBaseGlobals/EngineBaseApiPrefix.h"
"EngineBaseGlobals/EngineBaseConcepts.h"
"Math/Math.h"
"Math/Math.cpp"
"Math/Spatial.h"
"Math/Spatial.cpp"
"Serialization/Serializer.h"
"Serialization/Serializer.cpp"
"Timer.hpp"
"PathHelper.h"
"PathHelper.cpp")

add_library(InceptionBase STATIC ${SRC_BUILD_FILES})

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
         FILES ${SRC_BUILD_FILES})

target_include_directories(InceptionBase PUBLIC ${PROJECT_SOURCE_DIR}/Source/InceptionBase)
target_include_directories(InceptionBase PUBLIC ${PROJECT_SOURCE_DIR}/Source/InceptionBase/External/eigen-3.4.0)

#add_custom_command(TARGET InceptionBase POST_BUILD 
 #COMMAND ${CMAKE_COMMAND} -E copy_if_different 
  #      "${PROJECT_BINARY_DIR}/Source/InceptionBase/Debug/InceptionBase.dll"
   #     "${PROJECT_BINARY_DIR}/Source/StandAlone/Debug")