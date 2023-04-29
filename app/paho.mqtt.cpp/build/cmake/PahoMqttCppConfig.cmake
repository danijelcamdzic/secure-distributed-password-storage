# save build-time options
set(PAHO_BUILD_STATIC FALSE)
set(PAHO_BUILD_SHARED ON)
set(PAHO_WITH_SSL TRUE)

include(CMakeFindDependencyMacro)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})
find_dependency(PahoMqttC REQUIRED)
list(REMOVE_AT CMAKE_MODULE_PATH -1)
find_dependency(Threads REQUIRED)

include("${CMAKE_CURRENT_LIST_DIR}/PahoMqttCppTargets.cmake")
