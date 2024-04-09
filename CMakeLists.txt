cmake_minimum_required( VERSION 3.9 )
project( j.sacks )

set( CMAKE_CXX_STANDARD 11 )
set( OpenGL_GL_PREFERENCE GLVND)

find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
find_package( glfw3 REQUIRED )
find_package( OpenGL REQUIRED )
find_package( GLEW REQUIRED )

set( TARGET_NAME geom )

add_executable( ${TARGET_NAME}
        ./src/main.cpp
        ./src/genericShape.cpp
        ./src/scene.cpp
        ./src/testScene.cpp
        ./src/shader.cpp
        ./src/shapeManager.cpp
        ./src/cameraScene.cpp
        ./src/point.cpp
        ./src/genShapeModels.cpp
        ./src/pfd.cpp
        
        ./imgui/imgui_single_file.cpp )

if (UNIX)
    add_compile_options( -g )
endif (UNIX)
	
if (WIN32)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${TARGET_NAME})
    #set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY )
endif (WIN32)


target_include_directories( ${TARGET_NAME} PUBLIC ${OPENGL_INCLUDE_DIR} )
target_include_directories( ${TARGET_NAME} PUBLIC ${GLFW_INCLUDE_DIRS} )
target_include_directories( ${TARGET_NAME} PUBLIC "./imgui/" )
target_include_directories( ${TARGET_NAME} PUBLIC "./imgui/backends/" )
target_include_directories( ${TARGET_NAME} PUBLIC /usr/local/include/ )


target_link_libraries( ${TARGET_NAME} ${GLEW_LIBRARIES}  ${GLFW_LIBRARIES} )
target_link_libraries( ${TARGET_NAME} ${OPENGL_opengl_LIBRARY}
        ${OPENGL_glu_LIBRARY} ${OPENGL_glx_LIBRARY} )
target_link_libraries( ${TARGET_NAME} ${CMAKE_DL_LIBS} )


message( "GLFW Include : " ${GLFW_INCLUDE_DIRS} )
message( "GLFW Libs : " ${GLFW_LIBRARIES} )
message( "GLEW Include : " ${GLEW_INCLUDE_DIRS} )
message( "GLEW Libs : " ${GLEW_LIBRARIES} )
message( "OpenGL includes: " ${OPENGL_INCLUDE_DIR} )
message( "OpenGL libs: " ${OPENGL_opengl_LIBRARY} ", "
        ${OPENGL_glu_LIBRARY} ", "
        ${OPENGL_glx_LIBRARY} )
