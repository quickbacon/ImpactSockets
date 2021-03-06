cmake_minimum_required (VERSION 3.0.2)
project(SocketsLibrary)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - #
#                 COMPILER CONFIGURATION                  #
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - #

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR
	CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Werror -Wextra")
endif()

# Set install directory to project root if building on Windows
if(MSVC)
	set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/Install" CACHE PATH
		"Prefix prepended to install directories" FORCE)
endif()

set(LIBRARIES_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Libraries)
set(EVENTS_INCLUDE_DIR ${LIBRARIES_PATH}/Events/Include)
set(SOCKETS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Sockets)
set(SOCKETS_INCLUDE_DIR ${SOCKETS_PATH}/Include)
set(EXAMPLES_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Examples)

option(BUILD_SHARED "Build shared libraries instead of static ones." OFF)
if(${BUILD_SHARED})
	set(BUILD_MODE SHARED)
	set(BUILD_FLAGS -DDLL_EXPORTS)
else()
	set(BUILD_MODE STATIC)
	set(BUILD_FLAGS -DSTATIC)
endif()

set(LIBRARY_NAME ImpactSockets)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - #
#                     PROJECT MODULES                     #
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - #

include_directories(
  ${EVENTS_INCLUDE_DIR}
  ${SOCKETS_INCLUDE_DIR}
)
file(GLOB_RECURSE SOURCE_FILES "${SOCKETS_PATH}/Source/*.cpp")
add_library(${LIBRARY_NAME} ${BUILD_MODE} ${SOURCE_FILES})
target_compile_definitions(${LIBRARY_NAME} PUBLIC ${BUILD_FLAGS})

set(DEMO_TCP_CLIENT DemoTCPClient)
add_executable(${DEMO_TCP_CLIENT} "${EXAMPLES_PATH}/${DEMO_TCP_CLIENT}.cpp")
target_link_libraries(${DEMO_TCP_CLIENT} ${LIBRARY_NAME})

set(DEMO_TCP_SERVER DemoTCPServer)
add_executable(${DEMO_TCP_SERVER} "${EXAMPLES_PATH}/${DEMO_TCP_SERVER}.cpp")
target_link_libraries(${DEMO_TCP_SERVER} ${LIBRARY_NAME})

install(
  TARGETS ${LIBRARY_NAME} ${DEMO_TCP_CLIENT} ${DEMO_TCP_SERVER}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
INSTALL (
  DIRECTORY ${SOCKETS_INCLUDE_DIR}/
  DESTINATION include
  FILES_MATCHING PATTERN "*.h*"
)
INSTALL (
  DIRECTORY ${EVENTS_INCLUDE_DIR}/
  DESTINATION include
  FILES_MATCHING PATTERN "*.h"
)