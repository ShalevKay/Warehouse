cmake_minimum_required(VERSION 3.27)
project(SPL_Ass1_Warehouse)

set(CMAKE_CXX_STANDARD 11)

include_directories(include)

add_executable(SPL_Ass1_Warehouse
        include/Action.h
        include/Customer.h
        include/Order.h
        include/Volunteer.h
        include/WareHouse.h
        src/main.cpp
        src/Action.cpp
        src/Customer.cpp
        src/Order.cpp
        src/Volunteer.cpp
        src/WareHouse.cpp
)
