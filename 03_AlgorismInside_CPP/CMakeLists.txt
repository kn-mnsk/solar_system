cmake_minimum_required (VERSION 3.1)
cmake_policy(VERSION 3.1)

get_filename_component(ProjectID ${CMAKE_CURRENT_LIST_DIR} NAME)
string(REPLACE " " "_" ProjectID ${ProjectID})
project (${ProjectID})

##### From CmakeLists of freeglut
# for multiarch LIBDIR support (requires cmake>=2.8.8)
INCLUDE(GNUInstallDirs)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}" CACHE PATH "..." FORCE)
endif()

### Folder Structures
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

### Define directories -Configuration types are either Debug or Release only!
set(ConfigDir $<$<CONFIG:Release>:Release>$<$<CONFIG:Debug>:Debug>)
set(COMMONSDK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../.common_sdk")
#set(DllDir "${CMAKE_CURRENT_SOURCE_DIR}/../.common/common_cpp/dll")
#set(LibDir "${CMAKE_CURRENT_SOURCE_DIR}/../.common/common_cpp/lib")
#set(IncludeDir "${CMAKE_CURRENT_SOURCE_DIR}/../.common/common_cpp/include")
set(CurrDir "${CMAKE_CURRENT_SOURCE_DIR}")

# NOTE: On Windows and Cygwin, the dll's are placed in the
# CMAKE_RUNTIME_OUTPUT_DIRECTORY, while their corresponding import
# libraries end up in CMAKE_ARCHIVE_OUTPUT_DIRECTORY. On other
# platforms, such as Linux, the shared libraries are put in
# CMAKE_ARCHIVE_OUTPUT_DIRECTORY instead.
# Static libraries end up in CMAKE_ARCHIVE_OUTPUT_DIRECTORY on all
# platforms.
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
##### From CmakeLists of freeglut


IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${COMMONSDK_DIR}/cmake/modules/")

include_directories(
    "${COMMONSDK_DIR}/assimp/assimp-5.0.1/include" 
    "${COMMONSDK_DIR}/freeglut/freeglut-3.2.1/include" 
    "${COMMONSDK_DIR}/glad/glad-4.3-compatibility/include" 
    "${COMMONSDK_DIR}/glew/glew-2.2.0/include" 
    "${COMMONSDK_DIR}/glfw/glfw-3.3.2/include" 
    "${COMMONSDK_DIR}/glm/glm-0.9.9.7" 
    "${COMMONSDK_DIR}/stb/stb_image.h_2.26/include" 
    "${COMMONSDK_DIR}/glimg/include" 
    "${COMMONSDK_DIR}/glimg/source" 
    "${COMMONSDK_DIR}/boost/boost_1_72_0" 
    "${COMMONSDK_DIR}/FreeType/FreeType-2.10.4/FreeType/include" 
    "${CurrDir}/localtool/configuration" 
    "${CurrDir}/../.globaltool//mydevelop" 
    "${CurrDir}/../.globaltool/learnopengl" )

### find the required packages
# OpenGL
find_package(OPENGL REQUIRED)
set(LIBS ${OPENGL_LIBRARIES})

add_definitions(-D_CRT_SECURE_NO_WARNINGS)  # to disable deprication

configure_file("${CurrDir}/configuration/root_directory.h.in" "${CurrDir}/localtool/configuration/configuration/root_directory.h")
include_directories("${CurrDir}/localtool/configuration/configuration")


### first create relevant static libraries requried for other projects
add_library(STB_IMAGE "${COMMONSDK_DIR}/stb/stb_image.h_2.26/src/stb_image.cpp")
set_target_properties(STB_IMAGE PROPERTIES FOLDER "Utility")
set(LIBS ${LIBS} STB_IMAGE)

add_library(GLAD "${COMMONSDK_DIR}/glad/glad-4.3-compatibility/src/glad.c")
set_target_properties(GLAD PROPERTIES FOLDER "Utility")
set(LIBS ${LIBS} GLAD)

### then create a project file
file(GLOB_RECURSE Header_LearnOpenGL 
    "${CurrDir}/../.globaltool/learnopengl/learnopengl/original/*.h" 
    "${CurrDir}/../.globaltool/learnopengl/learnopengl/original/*.hPP" )
file(GLOB_RECURSE Header_Custom
    "${CurrDir}/../.globaltool/learnopengl/learnopengl/custom/*.h"
    "${CurrDir}/../.globaltool/learnopengl/learnopengl/custom/*.hpp."
    "${CurrDir}/../.globaltool/learnopengl/learnopengl/custom/*.cpp"
    "${CurrDir}/../.globaltool/mydevelop/mydevelop/*.h"
    "${CurrDir}/../.globaltool/mydevelop/mydevelop/*.hpp"
    "${CurrDir}/../.globaltool/mydevelop/mydevelop/*.cpp"
    "${CurrDir}/localtool/configuration/configuration/*.h")
file(GLOB_RECURSE Source 
    "${CurrDir}/src/*.cpp" )
file(GLOB_RECURSE Shader 
    "${CurrDir}/shader/*.vert"
    "${CurrDir}/shader/*.frag"
    "${CurrDir}/shader/*.geom"
    "${CurrDir}/shader/*.tesc"
    "${CurrDir}/shader/*.tese"
    "${CurrDir}/shader/*.glsl")

add_executable(${ProjectID} ${Source} 
    ${Shader} ${Header_LearnOpenGL} ${Header_Custom} )
source_group(Shader FILES ${Shader})
source_group(Header_Custom FILES ${Header_Custom})
source_group(Header_LearnOpenGL FILES ${Header_LearnOpenGL})

target_link_libraries(${ProjectID} 
    "${LIBS}" 
    "${COMMONSDK_DIR}/assimp/assimp-5.0.1/.build/code/${ConfigDir}/assimp-vc142-mt$<$<CONFIG:Debug>:d>.lib" 
    "${COMMONSDK_DIR}/freeglut/freeglut-3.2.1/.build/lib/${ConfigDir}/freeglut$<$<CONFIG:Debug>:d>.lib" 
    "${COMMONSDK_DIR}/freeglut/freeglut-3.2.1/.build/lib/${ConfigDir}/freeglut_static$<$<CONFIG:Debug>:d>.lib" 
    "${COMMONSDK_DIR}/glew/glew-2.2.0/.build/lib/${ConfigDir}/glew32$<$<CONFIG:Debug>:d>.lib" 
    "${COMMONSDK_DIR}/glfw/glfw-3.3.2/.build/src/${ConfigDir}/glfw3dll.lib" 
    "${COMMONSDK_DIR}/SOIL/${ConfigDir}/SOIL.lib"
    "${COMMONSDK_DIR}/glimg/.build/lib/${ConfigDir}/GLIMG.lib" 
    "${COMMONSDK_DIR}/FreeType/FreeType-2.10.4/FreeType/release dll/win64/freetype.lib"  )

### copy dlls    
add_custom_command(TARGET ${ProjectID} POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy "${COMMONSDK_DIR}/assimp/assimp-5.0.1/.build/code/${ConfigDir}/assimp-vc142-mt$<$<CONFIG:Debug>:d>.dll" $<TARGET_FILE_DIR:${ProjectID}> 
    COMMAND ${CMAKE_COMMAND} -E copy "${COMMONSDK_DIR}/freeglut/freeglut-3.2.1/.build/bin/${ConfigDir}/freeglut$<$<CONFIG:Debug>:d>.dll" $<TARGET_FILE_DIR:${ProjectID}> 
    COMMAND ${CMAKE_COMMAND} -E copy "${COMMONSDK_DIR}/glew/glew-2.2.0/.build/bin/${ConfigDir}/glew32$<$<CONFIG:Debug>:d>.dll" $<TARGET_FILE_DIR:${ProjectID}> 
    COMMAND ${CMAKE_COMMAND} -E copy "${COMMONSDK_DIR}/glfw/glfw-3.3.2/.build/src/${ConfigDir}/glfw3.dll" $<TARGET_FILE_DIR:${ProjectID}> 
    COMMAND ${CMAKE_COMMAND} -E copy "${COMMONSDK_DIR}/FreeType/FreeType-2.10.4/FreeType/release dll/win64/freetype.dll" $<TARGET_FILE_DIR:${ProjectID}> )

### if compiling for visual studio, also use configure file for each project (specifically to set up working directory)
if(MSVC)
    configure_file(${CMAKE_SOURCE_DIR}/configuration/visualstudio.vcxproj.user.in ${CMAKE_CURRENT_BINARY_DIR}/${ProjectID}.vcxproj.user @ONLY)
endif(MSVC)

unset(Source)
unset(Shader)
unset(Header_Custom)
unset(Header_Learnopengl)