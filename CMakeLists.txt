cmake_minimum_required(VERSION 3.25)
project(
  "TP0"
  VERSION 1.0.0
  DESCRIPTION "Proyecto TP0 Sistemas Operativos"
  LANGUAGES C)

# Enable testing
include(CTest)

# Flags for compiling
set(CMAKE_C_STANDARD 17)

find_package(cJSON REQUIRED)
find_package(unity REQUIRED)

add_executable(TP0 src/main.c)

# Linka todas as bibliotecas que você quer usar
target_link_libraries(TP0 cjson::cjson unity::unity)
