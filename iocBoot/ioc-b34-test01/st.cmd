# RTEMS startup script for PROD test stand in B34

setenv("LOCATION", "B34:TEST01")
setenv("IOC_NAME", "IOC:B34:TEST01")
setenv("ENGINEER", "Jeremy Lorelli (lorelli)")
cd("../..")

# Start the debugger
ld("rtems-gdb-stub.obj")
rtems_gdb_start(200,0)

# Load obj file
ld("bin/RTEMS-beatnik/atlas.obj")
# Let's look at the load addresses
lsmod()

## Register all support components
dbLoadDatabase("dbd/atlas.dbd")
atlas_registerRecordDeviceDriver(pdbbase)

# System Location:
epicsEnvSet("LOCA","B34")

epicsEnvSet("IOC_NAME","IOC:B34:TEST01")
#epicsEnvSet("EVR_DEV1","EVR:B084:RF03")
#epicsEnvSet("UNIT","RF03")
#epicsEnvSet("FAC","SYS0")

#===========================================================
# Autosave NFS mounting setup. The following code mounts
# /data through autosave, giving it management control over
# the mount. This allows autosave to automatically remount
# /data if saves start failing.
# NOTE 09/05/24: DISABLED on S3DF for now
#===========================================================
# Get rid of /data and /dat
#unmount("/data")
#unmount("/dat")

# Let autosave manage the mount. Syntax is user@host, host, nfsServerPath[:mountpoint]
# The second parameter seems to be unused...
#save_restoreSet_NFSHost("8412.2211@172.23.20.118", "172.23.20.118", "/vol/vol1/g.lcls/epics/ioc/data/ioc-b084-rf03:/data")

# Wait for saves to fail 10 times before attempting a remount (this is the default)
#save_restoreRemountThreshold = 10

#===========================================================

dbLoadRecords("db/iocAdminRTEMS.db","IOC=IOC:B34:TEST01")
#dbLoadRecords("db/iocRelease.db"   ,"IOC=IOC:B084:RF03")

dbLoadRecords("db/save_restoreStatus.db", "P=IOC:B34:TEST01:")

#dbLoadRecords("db/atlasRecords.db", "P=IOC:B34:TEST01")
#dbLoadRecords("db/genRecords.db", "P=IOC:B34:TEST01")

#save_restoreSet_SeqPeriodInSeconds(60)

#=========================================================
# Init autosave
#=========================================================
#. "iocBoot/common/autosave_rtems.cmd"

## Run this to trace the stages of iocInit
#traceIocInit()

iocInit()

# vim: syn=csh
