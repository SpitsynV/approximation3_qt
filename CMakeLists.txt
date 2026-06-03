cmake_minimum_required(VERSION 3.16)
project(approx2d LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets)

find_package(Threads REQUIRED)

add_executable(approx3
    main.cpp

    func.cpp       func.h
    task29.cpp     task29.h
    
    approximator2d.cpp  approximator2d.h
    plotwidget2d.cpp    plotwidget2d.h
    error.cpp           error.h
)

target_link_libraries(approx3
    Qt5::Widgets
    Threads::Threads
)