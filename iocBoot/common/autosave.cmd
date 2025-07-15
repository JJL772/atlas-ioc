#=========================================================#
# Common autosave setup code for LinuxRT and other flavors
# of soft IOCs
#=========================================================#

save_restoreSet_Debug(0)

# Ok to restore a save set that has missing values (no CA connection to PV)?
# Ok to save a file if some CA connections are bad?
save_restoreSet_IncompleteSetsOk(1)

# In the restore operation, a copy of the save file will be written.  The
# file name can look like "auto_settings.sav.bu", and be overwritten every
# reboot, or it can look like "auto_settings.sav_020306-083522" (this is what
# is meant by a dated backup file) and every reboot will write a new copy.
#save_restoreSet_DatedBackupFiles(1)

# Specify where request and save files can be located (If this is a mount, we need to omit the /data prefix)
set_requestfile_path("${IOC_DATA}/${IOC}/autosave-req")
set_savefile_path("${IOC_DATA}/${IOC}/autosave")
save_restoreSet_status_prefix("${IOC_NAME}")
dbLoadRecords("db/save_restoreStatus.db", "P=${IOC}:")

# Specify what save files should be restored when.
# Up to eight files can be specified for each pass.
set_pass0_restoreFile("info_positions.sav")
set_pass0_restoreFile("info_settings.sav")
set_pass1_restoreFile("info_settings.sav")
 
# Number of sequenced backup files (e.g., 'info_settings.sav0') to write
save_restoreSet_NumSeqFiles(3)

# Time interval between sequenced backups
save_restoreSet_SeqPeriodInSeconds(600)

# Time between failed .sav-file write and the retry.
save_restoreSet_RetrySeconds(60)

# vim: syntax=csh
# End of script
