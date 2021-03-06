# Project name
#project(tdd VERSION 0.1.0 LANGUAGES CXX)


#include(GNUInstallDirs)
#FILE(GLOB HDRS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
FILE(GLOB SRCS *.cpp)
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)
SET(TargetName Rem.App)
#target_compile_definitions(${PROJECT_NAME} PUBLIC "${PROJECT_NAME}_DEBUG=$<CONFIG:Debug>")
#target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

#include(GenerateExportHeader)
#add_dependencies(tdd journal grammar lexer utils)

add_executable(${TargetName} ${SRCS})
add_dependencies(${TargetName} Rem ) # Diagnostics ) #sqlitedb logbook lexer)

list(
    APPEND LSC_INCLUDE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
#    "${LSC_ROOT}/Rem/include" "${LSC_ROOT}/Diagnostics/include"
#    "${LSC_ROOT}/Base/SQLite/include" 
#    "${LSC_ROOT}/Scripture/include")
)

message ("App/LSC_INCLUDES: " ${LSC_INCLUDE_PATH})

target_include_directories(${TargetName} PUBLIC
        "$<BUILD_INTERFACE:${REM_INCLUDE_PATH}>"
        )

target_link_libraries(${TargetName} ${CMAKE_DL_LIBS} Rem)
target_compile_features(${TargetName} PUBLIC cxx_std_20)

IF (EXISTS "${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json")
    EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json
            ${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json
            )
ENDIF ()


install(TARGETS ${TargetName}
        RUNTIME DESTINATION bin/${PROJECT_NAME}
        )

#SET(EXPR "\\'A = sin pi/2\;\\'")
#
#
#add_custom_command(TARGET ${TargetName} POST_BUILD
#    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${TargetName}
#
#        "--Test=\\'Util::String::Words\\' ${CMAKE_INSTALL_PREFIX}"
#        "--Test=\\'Util::App\\' --File=/home/bretzel/Applications/bin/logs/App-Test.log"
#        "--Test=\\'Lexer::TokenScan\\' ${EXPR}"
#
#    WORKING_DIRECTORY ${CMAKE_CURRENT_BIN_DIR}
#    COMMENT "Running ${TargetName}..."
#)


