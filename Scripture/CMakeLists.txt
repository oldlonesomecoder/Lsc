cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
# Project name

project(Scripture VERSION 0.1.0 LANGUAGES CXX)


include(GNUInstallDirs)
#SET(REM_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include)
message("SCRIPTURE_INCLUDE_DIR:" ${SCRIPTURE_INCLUDE_DIR})
FILE(GLOB HDRS ${SCRIPTURE_INCLUDE_DIR}/* ${SCRIPTURE_INCLUDE_DIR}/Interpreter/* ${SCRIPTURE_INCLUDE_DIR}/Interpreter/Compiler/*) # ${SCRIPTURE_INCLUDE_DIR}/Interpreter/Compiler/*
FILE(GLOB SRCS Src/* Src/Interpreter/* Src/Interpreter/Compiler/*) # ... Src/Interpreter/Compiler/* Src/Interpreter/Compiler/*
SET( CMAKE_EXPORT_COMPILE_COMMANDS ON )




add_library(
        ${PROJECT_NAME}
        SHARED
        ${HDRS}
        ${SRCS}
		Src/Interpreter/Compiler/Ast.cpp include/Lsc/Scripture/Interpreter/Compiler/Ast.h)

list(
    APPEND INCLUDE_PATHS
    "${REM_INCLUDE_DIR}"
    "${SCRIPTURE_INCLUDE_DIR}"

)


target_compile_definitions(${PROJECT_NAME} PUBLIC "${PROJECT_NAME}_DEBUG=$<CONFIG:Debug>")
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

include(GenerateExportHeader)
#generate_export_header(${PROJECT_NAME} EXPORT_FILE_NAME ${${PROJECT_NAME}_export})

IF( EXISTS "${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json" )
	EXECUTE_PROCESS( COMMAND ${CMAKE_COMMAND} -E copy_if_different
			${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json
			)
ENDIF()


message("Scripture::INCLUDE_PATHS:" ${INCLUDE_PATHS})
add_dependencies(${PROJECT_NAME} Rem)

target_include_directories(${PROJECT_NAME} PUBLIC
    "$<BUILD_INTERFACE:${INCLUDE_PATHS}>"
        $<INSTALL_INTERFACE:include/${FrameWorkName}/${PROJECT_NAME}>  # <prefix>/include/mylib
        )

target_link_libraries(${PROJECT_NAME} ${CMAKE_DL_LIBS} Rem )


install(DIRECTORY
        include/${FrameWorkName}/${PROJECT_NAME}/
        DESTINATION "${CMAKE_INSTALL_PREFIX}/include/${FrameWorkName}/${PROJECT_NAME}"
#        FILES_MATCHING PATTERN "*.h"
)

install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}Targets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
)


string(REPLACE "++" "" NS PROJECTNAME)


add_subdirectory(App)
