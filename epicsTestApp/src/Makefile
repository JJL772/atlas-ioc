TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#==================================================
# build a support library

LIBRARY_IOC += epicsTest

# xxxRecord.h will be created from xxxRecord.dbd
#DBDINC += xxxRecord
# install epicsTest.dbd into <top>/dbd
DBD += epicsTest.dbd

# specify all source files to be compiled and added to the library
epicsTest_SRCS += fileSystemTest.cpp
epicsTest_SRCS += getopt.cpp
epicsTest_SRCS += testUtils.cpp
epicsTest_SRCS += tcploop.cpp

epicsTest_LIBS += $(EPICS_BASE_IOC_LIBS)

USR_CXXFLAGS += -std=c++11

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

