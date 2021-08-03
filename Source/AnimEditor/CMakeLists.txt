

set(SRC_BUILD_FILES
"MatchingDatabaseBuilder.h"
"MatchingDatabaseBuilder.cpp"
"main.cpp"
)

add_executable(AnimEditor ${SRC_BUILD_FILES})

target_compile_options(AnimEditor PUBLIC /wd26812 /wd26451 /wd26495)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
         FILES ${SRC_BUILD_FILES})

target_include_directories(AnimEditor PUBLIC 
${PROJECT_SOURCE_DIR}/Source/InceptionEngine
${PROJECT_SOURCE_DIR}/Source/InceptionBase)

target_link_libraries(AnimEditor 
InceptionBase)