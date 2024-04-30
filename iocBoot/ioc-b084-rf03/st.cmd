## Example RTEMS startup script

## You may have to change Jupiter to something else
## everywhere it appears in this file


setenv("LOCATION", "B084:RF03")
setenv("IOC_NAME", "IOC:B084:RF03")
setenv("ENGINEER", "Jeremy Lorelli (lorelli)")
cd("../..")

# Load obj file
ld("bin/RTEMS-mvme3100/atlas.obj")
# Let's look at the load addresses
lsmod()

## Register all support components
dbLoadDatabase("dbd/atlas.dbd")
atlas_registerRecordDeviceDriver(pdbbase)

# System Location:
epicsEnvSet("LOCA","B084")

epicsEnvSet("IOC_NAME","IOC:B084:RF03")
epicsEnvSet("EVR_DEV1","EVR:B084:RF03")
epicsEnvSet("UNIT","RF03")
epicsEnvSet("FAC","SYS0")

#=========================================================
# Initialize PMC Type EVR on MVME3100 with PMC Carrier
#=========================================================
#ErConfigure(0,0x000000,0x00,0,1)   #PMC type EVR230
#evrInitialize()
#bspExtVerbosity = 1
#evrTimeFlag=0
#=========================================================

#============================================================================
# Load Timing System databases
#============================================================================
#dbLoadRecords("db/Pattern.db","IOC=IOC:B084:RF03,SYS=SYS0")
## Load EVR record instances
# PMC-Carrier on VME: MRF EVR230
#dbLoadRecords("db/EvrPmc.db","EVR=EVR:B084:RF03,CRD=0,SYS=SYS0")
#dbLoadRecords("db/PMC-trig.db","IOC=IOC:B084:RF03,LOCA=B084,UNIT=03,SYS=SYS0")
#=============================================================================

## Load record instances
#dbLoadRecords("db/dbExample1.db", "user=V4_Axion")
#dbLoadRecords("db/dbExample2.db", "user=V4_Axion, no=1, scan = 1 second")

dbLoadRecords("db/iocAdminRTEMS.db","IOC=IOC:B084:RF03")
dbLoadRecords("db/iocRelease.db"   ,"IOC=IOC:B084:RF03")

dbLoadRecords("db/atlasRecords.db", "P=IOC:B084:RF03")
dbLoadRecords("db/genRecords.db", "P=IOC:B084:RF03")

# Let's load up some waveforms and scalars:
#dbLoadDatabase("db/sinePower.db")
#dbLoadDatabase("db/sineEnergy.db")

# database for BsaCore test case 0 #
#  dbLoadRecords("db/bsaPulseId.db", "IOC=IOC:B084:RF03,SCAN=Event,EVNT=40,N=0")
#  dbLoadRecords("db/Bsa.db", "DEVICE=IOC:B084:RF03:0,ATRB=PULSEID") 

# database for BsaCore test case 1 #
  
#  dbLoadRecords("db/bsaPulseId.db", "IOC=IOC:B084:RF03,SCAN=Event,EVNT=41,N=1")
#  dbLoadRecords("db/Bsa.db", "DEVICE=IOC:B084:RF03:1,ATRB=PULSEID")

#=========================================================
# Init autosave
#=========================================================
. "iocBoot/common/autosave_rtems.cmd"

## Run this to trace the stages of iocInit
#traceIocInit()

iocInit()

## Start any sequence programs
#seq(sncExample, "user=V4_Axion")

