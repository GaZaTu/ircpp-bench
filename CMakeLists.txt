cmake_minimum_required(VERSION 3.12.0 FATAL_ERROR)
project(cpptest VERSION 0.0.1 LANGUAGES CXX)

set(PROJECT_ORGANIZATION gazatu.xyz)

cmake_policy(SET CMP0074 NEW)

add_definitions(
  -DPROJECT_ORGANIZATION="${PROJECT_ORGANIZATION}"
  -DPROJECT_NAME="${PROJECT_NAME}"
  -DPROJECT_VERSION="${PROJECT_VERSION}"
  -DPROJECT_SOURCE_DIR="${PROJECT_SOURCE_DIR}"
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")

include_directories(
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_SOURCE_DIR}/include
)

set(SRC_FILES
  src/main.cpp
  src/irc/clearchat.cpp
  src/irc/common.cpp
  src/irc/join.cpp
  src/irc/out.cpp
  src/irc/parse.cpp
  src/irc/ping.cpp
  src/irc/privmsg.cpp
  src/irc/reconnect.cpp
  src/irc/roomstate.cpp
  src/irc/unknown.cpp
  src/irc/usernotice.cpp
)

add_executable(${PROJECT_NAME} ${SRC_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
)

set_property(TARGET ${PROJECT_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
