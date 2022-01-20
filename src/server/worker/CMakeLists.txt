######################################################
#            PROJECT  : IO Catcher                   #
#            VERSION  : 0.0.0                        #
#            DATE     : 10/2020                      #
#            AUTHOR   : Valat Sébastien  - ATOS      #
#            LICENSE  : ????????                     #
######################################################

######################################################
set(SERVER_WORKER_SRC TaskIO.cpp Worker.cpp WorkerManager.cpp TaskScheduler.cpp IORanges TaskRunner.cpp)

######################################################
add_library(ioc-server-worker OBJECT ${SERVER_WORKER_SRC})
SET_TARGET_PROPERTIES(ioc-server-worker PROPERTIES POSITION_INDEPENDENT_CODE ON)

######################################################
if (ENABLE_TESTS)
	add_subdirectory(tests)
endif (ENABLE_TESTS)
