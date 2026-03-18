#!../../bin/rhel7-x86_64/atlas

#- You may have to change atlas to something else
#- everywhere it appears in this file

< envPaths

cd "${TOP}"

## Register all support components
dbLoadDatabase "dbd/atlas.dbd"
atlas_registerRecordDeviceDriver pdbbase

dbLoadRecords("db/genRecords.db", "P=${IOC}:")
dbLoadRecords("db/iocAdminSoft.db", "IOC=SIOC:TST:SYS0")

< "${TOP}/iocBoot/common/autosave.cmd"

cd "${TOP}/iocBoot/${IOC}"

save_restoreSet_CAReconnect(1)

#atlasPvaInit("${IOC}")

iocInit

< "${TOP}/iocBoot/common/start_restore.soft.cmd"
