TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#=============================
# Build the IOC application

PROD_IOC = atlas
# atlas.dbd will be created and installed
DBD += atlas.dbd

# atlas.dbd will be made up from these files:
atlas_DBD += base.dbd
atlas_DBD += iocAdmin.dbd
atlas_DBD += epicsTest.dbd
ifeq ($(EPICS_V7),YES)
atlas_DBD += asSupport.dbd
#atlas_DBD += atlasPva.dbd
else
atlas_DBD += autosaveSupport.dbd
endif
atlas_DBD += atlasDev.dbd

ifeq ($(CXX11),YES)
USR_CXXFLAGS=-std=c++11
endif

# Include dbd files from all support applications:
#atlas_DBD += xxx.dbd

# Add all the support libraries needed by this IOC
#atlas_LIBS += xxx

# atlas_registerRecordDeviceDriver.cpp derives from atlas.dbd
atlas_SRCS += atlas_registerRecordDeviceDriver.cpp
atlas_SRCS += atlasDevSup.c
ifeq ($(EPICS_V7),YES)
#atlas_SRCS += atlasPva.cpp
endif

# Build the main IOC entry point on workstation OSs.
atlas_SRCS_DEFAULT += atlasMain.cpp
atlas_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#atlas_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Finally link to the EPICS Base libraries
atlas_LIBS += devIocStats
atlas_LIBS += autosave
atlas_LIBS += epicsTest
atlas_LIBS += $(EPICS_BASE_IOC_LIBS) 
ifeq ($(EPICS_V7),YES)
atlas_LIBS += $(EPICS_BASE_PVA_CORE_LIBS)
endif

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

