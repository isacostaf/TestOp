cmake_minimum_required(VERSION 3.25)
project(
  "TP1"
  VERSION 1.0.0
  DESCRIPTION "Proyecto TP1 - Monitoreo básico y observabilidad"
  LANGUAGES C)

# Enable testing
include(CTest)

# Flags for compiling
set(CMAKE_C_STANDARD 17)

find_package(cJSON REQUIRED)
find_package(unity REQUIRED)

# Executável principal
add_executable(TP1 src/main.c src/metrics.c)

# Linka todas as bibliotecas que você quer usar
target_link_libraries(TP1 cjson::cjson unity::unity)

# Incluir diretório de headers
target_include_directories(TP1 PRIVATE include)

# Executável de testes
add_executable(test_metrics src/test_metrics.c src/metrics.c)
target_link_libraries(test_metrics cjson::cjson unity::unity)
target_include_directories(test_metrics PRIVATE include)

# Configurar CTest
enable_testing()
add_test(NAME metrics_tests COMMAND test_metrics)
