
#set_requestfile_path("$(TOP)/iocBoot/$(IOC)", "")

#set_savefile_path("$(TOP)/iocBoot/$(IOC)/autosave")

#set_pass0_restoreFile("auto_settings.sav", "P=${IOC}:")
#set_pass1_restoreFile("auto_settings.sav", "P=${IOC}:")

save_restoreSet_status_prefix("autosave:")
save_restoreSet_Debug(1)

# Bad CA connections are OK, we can save 'most' of the file
save_restoreSet_IncompleteSetsOk(1)

save_restoreSet_NumSeqFiles(10)
save_restoreSet_SeqPeriodInSeconds(5)

# Configure save/request paths
set_savefile_path("${IOC_DATA}/${IOC}/autosave")
#set_requestfile_path("${TOP}/iocBoot/${IOC}/autosave-req")

set_pass0_restoreFile("info_positions.sav", "P=${IOC}")
set_pass1_restoreFile("info_positions.sav", "P=${IOC}")

#set_requestfile_path("${TOP}/iocBoot/${IOC}", "")
set_requestfile_path("${TOP}/db")

dbLoadRecords("$(AUTOSAVE)/asApp/Db/save_restoreStatus.db", "P=${IOC}:")

makeAutosaveFiles()

save_restoreSet_NumSeqFiles(3)
save_restoreSet_SeqPeriodInSeconds(10)

create_monitor_set("info_positions.req", 5, "P=${IOC}")
create_monitor_set("info_settings.req", 30, "P=${IOC}")

# vim: syntax=bash
