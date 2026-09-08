cmake_minimum_required(VERSION 3.20)

project(option_pricing_engine VERSION 0.1.2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(option_pricing
    src/asian.cpp
    src/closed_form.cpp
    src/math_functions.cpp
    src/monte_carlo.cpp
    src/payoff.cpp
)
target_include_directories(option_pricing PUBLIC include)

add_executable(option-pricer main.cpp src/input.cpp src/output.cpp)
target_link_libraries(option-pricer PRIVATE option_pricing)

include(CTest)
if(BUILD_TESTING)
    add_executable(option-pricing-tests tests/test_engine.cpp)
    target_link_libraries(option-pricing-tests PRIVATE option_pricing)
    add_test(NAME option-pricing-tests COMMAND option-pricing-tests)
endif()
