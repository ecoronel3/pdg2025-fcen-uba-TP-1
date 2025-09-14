cmake_minimum_required(VERSION 3.16)

# list of source files
set(dgpTest1_files dgpTest1.cpp)

# define the executable
if(WIN32)
  add_executable(dgpTest1 WIN32 ${dgpTest1_files})
else()
  add_executable(dgpTest1 ${dgpTest1_files})
endif()

# in Windows + Visual Studio we need this to make it a console application
if(WIN32)
  if(MSVC)
    set_target_properties(dgpTest1 PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")
  endif(MSVC)
endif(WIN32)

# install application
set(BIN_DIR ${CMAKE_INSTALL_PREFIX}/bin)

# target_link_libraries(dgpTest1 core io util wrl)
target_link_libraries(dgpTest1 ${LIB_LIST})

install(TARGETS dgpTest1 DESTINATION ${BIN_DIR})

