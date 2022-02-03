######################################################
#  PROJECT  : IO Catcher                             #
#  LICENSE  : Apache 2.0                             #
#  COPYRIGHT: 2022 Sebastien Valat                   #
######################################################

######################################################
set(SERVER_WORKER_SRC Worker.cpp
                      IORanges.cpp
                      ObjectRanges.cpp
                      IOTask.cpp
                      TaskRunner.cpp
                      TaskScheduler.cpp
                      WorkerManager.cpp
)

######################################################
add_library(ioc-server-worker OBJECT ${SERVER_WORKER_SRC})
SET_TARGET_PROPERTIES(ioc-server-worker PROPERTIES POSITION_INDEPENDENT_CODE ON)

######################################################
if (ENABLE_TESTS)
	add_subdirectory(tests)
endif (ENABLE_TESTS)
