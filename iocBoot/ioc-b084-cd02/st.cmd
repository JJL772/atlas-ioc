## Example RTEMS startup script

## You may have to change Jupiter to something else
## everywhere it appears in this file


setenv("LOCATION", "B84:CD02")
setenv("IOC_NAME", "IOC:B84:CD02")
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
epicsEnvSet("LOCA","B84")

epicsEnvSet("IOC_NAME","IOC:B84:CD02")
epicsEnvSet("EVR_DEV1","EVR:B84:CD02")
epicsEnvSet("UNIT","CD02")
epicsEnvSet("FAC","SYS0")

# =========================================================
# Initialize PMC Type EVR on MVME3100 with PMC Carrier
# =========================================================
#ErConfigure(0,0x000000,0x00,0,1)   #PMC type EVR230
#evrInitialize()
#bspExtVerbosity = 1
#evrTimeFlag=0
# =========================================================

# ============================================================================
# Load Timing System databases
# ============================================================================
#dbLoadRecords("db/Pattern.db","IOC=IOC:B84:CD02,SYS=SYS0")
## Load EVR record instances
# PMC-Carrier on VME: MRF EVR230
#dbLoadRecords("db/EvrPmc.db","EVR=EVR:B84:CD02,CRD=0,SYS=SYS0")
#dbLoadRecords("db/PMC-trig.db","IOC=IOC:B84:CD02,LOCA=B84,UNIT=02,SYS=SYS0")
# =============================================================================

## Load record instances
#dbLoadRecords("db/dbExample1.db", "user=V4_Axion")
#dbLoadRecords("db/dbExample2.db", "user=V4_Axion, no=1, scan = 1 second")

dbLoadRecords("db/iocAdminRTEMS.db","IOC=IOC:B84:CD02")
#dbLoadRecords("db/iocRelease.db"   ,"IOC=IOC:B84:CD02")

# Let's load up some waveforms and scalars:
#dbLoadDatabase("db/sinePower.db")
#dbLoadDatabase("db/sineEnergy.db")

# database for BsaCore test case 0 #
#  dbLoadRecords("db/bsaPulseId.db", "IOC=IOC:B84:CD02,SCAN=Event,EVNT=40,N=0")
#  dbLoadRecords("db/Bsa.db", "DEVICE=IOC:B84:CD02:0,ATRB=PULSEID") 

# database for BsaCore test case 1 #
  
#  dbLoadRecords("db/bsaPulseId.db", "IOC=IOC:B84:CD02,SCAN=Event,EVNT=41,N=1")
#  dbLoadRecords("db/Bsa.db", "DEVICE=IOC:B84:CD02:1,ATRB=PULSEID")

## Run this to trace the stages of iocInit
traceIocInit()

iocInit()

## Start any sequence programs
#seq(sncExample, "user=V4_Axion")

