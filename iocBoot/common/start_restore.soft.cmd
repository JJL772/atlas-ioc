#=========================================================#
# Startup autosave for soft IOCs
#=========================================================#

cd "${IOC_DATA}/${IOC}/autosave-req"

makeAutosaveFiles()

# Start the save_restore task 
# save change on monitor, but no faster
# than every 5 seconds. 
# Note: You cannot set the last arg to 0.
create_monitor_set("info_positions.req", 5)
create_monitor_set("info_settings.req" , 5)

cd "${IOC_DATA}/${IOC}"

# End of script
