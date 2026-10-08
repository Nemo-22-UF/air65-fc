cmake_minimum_required(VERSION 3.16)
project(air65_fc C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
if(NOT MSVC)
  add_compile_options(-Wall -Wextra)
endif()

add_library(fc_core STATIC
  src/control/pid.c
  src/control/rates.c
  src/control/mixer.c
  src/safety/arming.c
)
target_include_directories(fc_core PUBLIC include src)

add_library(fc_sim STATIC
  sim/time_sim.c
  sim/imu_sim.c
  sim/receiver_sim.c
  sim/motors_sim.c
  sim/physics.c
)
target_include_directories(fc_sim PUBLIC include sim)
if(NOT MSVC)
  target_link_libraries(fc_sim PUBLIC m)
endif()

add_executable(air65_sim src/main.c)
target_link_libraries(air65_sim fc_core fc_sim)

enable_testing()
add_executable(air65_tests
  tests/test_main.c
  tests/test_mixer.c
)
target_include_directories(air65_tests PRIVATE tests)
target_link_libraries(air65_tests fc_core)
add_test(NAME all_tests COMMAND air65_tests)
