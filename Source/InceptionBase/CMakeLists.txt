

set(SRC_BUILD_FILES
"EngineBaseGlobals/EngineBaseApiPrefix.h"
"EngineBaseGlobals/EngineBaseConcepts.h"
"Math/Math.h"
"Math/Math.cpp"
"Serialization/Serializer.h"
"Serialization/Serializer.cpp"
"Timer.hpp"
"PathHelper.h"
"PathHelper.cpp")
IF (CMAKE_SYSTEM_NAME MATCHES "Windows")
add_library(InceptionBase STATIC ${SRC_BUILD_FILES})
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Linux")
add_library(InceptionBase SHARED ${SRC_BUILD_FILES})
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Windows")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
         FILES ${SRC_BUILD_FILES})

target_include_directories(InceptionBase PUBLIC ${PROJECT_SOURCE_DIR}/Source/InceptionBase)

#add_custom_command(TARGET InceptionBase POST_BUILD 
 #COMMAND ${CMAKE_COMMAND} -E copy_if_different 
  #      "${PROJECT_BINARY_DIR}/Source/InceptionBase/Debug/InceptionBase.dll"
   #     "${PROJECT_BINARY_DIR}/Source/StandAlone/Debug")
