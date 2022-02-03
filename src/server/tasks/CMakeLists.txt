######################################################
#  PROJECT  : IO Catcher                             #
#  LICENSE  : Apache 2.0                             #
#  COPYRIGHT: 2022 Sebastien Valat                   #
######################################################

######################################################
set(SERVER_TASKS_SRC TaskDeferredOps.cpp
                     TaskObjectFlush.cpp
                     TaskObjectCow.cpp
)

######################################################
add_library(ioc-server-tasks OBJECT ${SERVER_TASKS_SRC})
SET_TARGET_PROPERTIES(ioc-server-tasks PROPERTIES POSITION_INDEPENDENT_CODE ON)

######################################################
#if (ENABLE_TESTS)
#	add_subdirectory(tests)
#endif (ENABLE_TESTS)
