cmake_minimum_required(VERSION 3.16)

set(EXE_NAME dgpTestFaces)

# list of source files
set(dgpTestFaces_files dgpTestFaces.cpp)

# define the executable
if(WIN32)
    add_executable(${EXE_NAME} WIN32 ${dgpTestFaces_files})
else()
    add_executable(${EXE_NAME} ${dgpTestFaces_files})
endif()

# in Windows + Visual Studio we need this to make it a console application
if(WIN32)
    if(MSVC)
        set_target_properties(${EXE_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")
    endif(MSVC)
endif(WIN32)

# install application
set(BIN_DIR ${CMAKE_INSTALL_PREFIX}/bin)

target_link_libraries(${EXE_NAME} ${LIB_LIST})

install(TARGETS ${EXE_NAME} DESTINATION ${BIN_DIR})

