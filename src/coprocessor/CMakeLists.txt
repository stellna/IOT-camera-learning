cmake_minimum_required(VERSION  3.24)

# set(EXTRA_COMPONENT_DIRS    ../components)  // DL 상관 X, 코드 오류로 인해 주석 달기
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(stargazer-coprocessor)
