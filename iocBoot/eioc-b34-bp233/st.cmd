## Example RTEMS startup script

## You may have to change Jupiter to something else
## everywhere it appears in this file

setenv("LOCATION", "B34:BP233")
setenv("IOC_NAME", "EIOC:B34:BP233")
setenv("ENGINEER", "Jeremy Lorelli (lorelli)")
cd("../..")

# Start the debugger
#ld("rtems-gdb-stub.obj")
#rtems_gdb_start(200,0)

# Load obj file
ld("bin/RTEMS-uC5282/atlas.obj")
# Let's look at the load addresses
lsmod()

## Register all support components
dbLoadDatabase("dbd/atlas.dbd")
atlas_registerRecordDeviceDriver(pdbbase)

# System Location:
epicsEnvSet("LOCA","B34")

epicsEnvSet("IOC_NAME","EIOC:B34:BP233")
epicsEnvSet("UNIT","RD02")
epicsEnvSet("FAC","SYS0")

#===========================================================
# Autosave 9p mounting setup. nfsMount.cmd and friends
# will configure out mounts, but we don't want NFS! Unmount
# and use 9P instead.
#===========================================================

unmount("/data")
unmount("/dat")

# Mount with 9p
p9Mount("16626.2211@134.79.217.70", "/scratch/lorelli/dummy-diod-fs/eioc-b34-bp233", "/data", "msize=4096")

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
# Loading records
#===========================================================

dbLoadRecords("db/iocAdminRTEMS.db","IOC=EIOC:B34:BP233")
#dbLoadRecords("db/iocRelease.db"   ,"IOC=IOC:B084:RF03")

dbLoadRecords("db/atlasRecords.db", "P=EIOC:B34:BP233")
dbLoadRecords("db/genRecordsSmall.db", "P=EIOC:B34:BP233:")

#=========================================================
# Init autosave
#=========================================================
cexpsh("iocBoot/common/autosave_rtems.cmd")

# For testing
save_restoreSet_SeqPeriodInSeconds(20)

iocInit()

# Start autosave
cexpsh("iocBoot/common/start_restore.cmd")

# vim: syn=csh
