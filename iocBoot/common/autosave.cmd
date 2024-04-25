
set_requestfile_path("$(TOP)/iocBoot/$(IOC)", "")

set_savefile_path("$(TOP)/iocBoot/$(IOC)/autosave")

set_pass0_restoreFile("auto_settings.sav", "P=${IOC}:")
set_pass1_restoreFile("auto_settings.sav", "P=${IOC}:")

save_restoreSet_NumSeqFiles(3)
save_restoreSet_SeqPeriodInSeconds(10)

create_monitor_set("auto_positions.req", 5, "P=${IOC}:")
create_monitor_set("auto_settings.req", 30, "P=${IOC}:")
