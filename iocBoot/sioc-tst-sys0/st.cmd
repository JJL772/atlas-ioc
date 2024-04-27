#!../../bin/rhel7-x86_64/atlas

#- You may have to change atlas to something else
#- everywhere it appears in this file

< envPaths

cd "${TOP}"

## Register all support components
dbLoadDatabase "dbd/atlas.dbd"
atlas_registerRecordDeviceDriver pdbbase

dbLoadRecords("db/test-records.db", "P=${IOC}")
dbLoadRecords("db/genRecords.db", "P=${IOC}")

cd "${TOP}/iocBoot/${IOC}"

< "${TOP}/iocBoot/common/autosave.cmd"

save_restoreSet_CAReconnect(1)

iocInit

## Start any sequence programs
#seq sncxxx,"user=lorelli"
