#=========================================================#
# Startup autosave for hard IOCs
#=========================================================#

# Wait before building autosave files
epicsThreadSleep(1)

# Generate the autosave PV list (Takes FOREVER!!!)
chdir("/data/autosave-req")
iocshCmd("makeAutosaveFiles()")

# Start the save_restore task 
# save change on monitor, but no faster
# than every 5 seconds. 
# Note: You cannot set the last arg to 0. 
create_monitor_set("info_positions.req", 5 )
create_monitor_set("info_settings.req" , 5 )

# Change directory to the TOP of the Application
chdir(getenv("IOC_TOP"))

# End of script
