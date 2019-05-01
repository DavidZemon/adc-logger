cmake_minimum_required(VERSION 3.3)
find_package(PropWare REQUIRED
    COMPONENTS PropWare)

project(Scratch)

set(MODEL lmm)
set(BOARD activityboard)
create_executable(adc-logger
    main.cpp
)
