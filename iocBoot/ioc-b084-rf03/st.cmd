## Example RTEMS startup script

## You may have to change Jupiter to something else
## everywhere it appears in this file

setenv("LOCATION", "B084:RF03")
setenv("IOC_NAME", "IOC:B084:RF03")
setenv("ENGINEER", "Jeremy Lorelli (lorelli)")
cd("../..")

# Start the debugger
ld("rtems-gdb-stub.obj")
rtems_gdb_start(200,0)

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

dbLoadRecords("db/iocAdminRTEMS.db","IOC=IOC:B084:RF03")
#dbLoadRecords("db/iocRelease.db"   ,"IOC=IOC:B084:RF03")

dbLoadRecords("db/save_restoreStatus.db", "P=IOC:B084:RF03:")

dbLoadRecords("db/atlasRecords.db", "P=IOC:B084:RF03")
dbLoadRecords("db/genRecords.db", "P=IOC:B084:RF03")

save_restoreSet_SeqPeriodInSeconds(20)

#=========================================================
# Init autosave
#=========================================================
. "iocBoot/common/autosave_rtems.cmd"
. "iocBoot/common/start_restore.cmd"

## Run this to trace the stages of iocInit
#traceIocInit()

iocInit()

# Mount with 9p
p9Mount("16626.2211@134.79.217.70", "/scratch/lorelli/dummy-diod-fs", "/test")

# vim: syn=csh
