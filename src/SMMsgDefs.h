// Message Definitions
// Be Users Group @ UIUC - SoundMangler Project
// 2/16/1998

#pragma once

enum {
	// Message Open
	// Source: SMWindow
	// Target: SMApp
	// Data: none
	// Reply: none
	// Tells application to open new filter list
	SM_OPEN = '$Mop',
	
	// Message Save
	// Source: SMWindow
	// Target: SMApp
	// Data: none
	// Reply: none
	// Tells application to save filters to file
	SM_SAVE = '$Msv',
	
	// Message Save As
	// Source: SMWindow
	// Target: SMApp
	// Data: none
	// Reply: none
	// Tells application to save filters to a new file
	SM_SAVE_AS = '$Msa',
	
	// Message New
	// Source: SMWindow
	// Target: SMApp
	// Data: none
	// Reply: none
	// Tells application to create an empty filter list
	SM_NEW = '$Mnw',
	
	// Message Start
	// Source: SMWindow SMApp
	// Target: SMApp SMFilterMaster
	// Data: none
	// Reply: none
	// Tells object to start operations
	SM_START = '$Mgo',
	
	// Message Stop
	// Source: SMWindow SMApp
	// Target: SMApp SMFilterMaster
	// Data: none
	// Reply: none
	// Tells object to stop operations
	SM_STOP = '$Mst',
	
	// Message Open Sound Prefs
	// Source: SMWindow(Sound Prefs Button)
	// Target: SMWindow
	// Data: none
	// Reply: none
	// Tells window to open sound preferneces panel
	SM_SOUND_PREFS = '$Msp',
	
	// Message Show Filter About
	// Source: SMWindow(Available List)
	// Target: SMApp
	// Data: filter_id
	// Reply: SM_ARCHIVED_VIEW
	// Tells filter to create about view
	SM_ABOUT_VIEW = '$Mav',
	
	// Message Show Filter Prefs
	// Source: SMWindow(In-Use List)
	// Target: SMApp
	// Data: filter_id
	// Reply: SM_ARCHIVED_VIEW
	// Tells filter to create preferences view
	SM_PREFS_VIEW = '$Mpv',
	
	// Message Remove Add-on
	// Source: SMAddOnManager
	// Target: SMApp
	// Data: entry_ref
	// Reply: none
	// Tells application to remove add-on
	SM_REMOVE_ADDON = '$Mra',
	
	// Message Add Add-on
	// Source: SMAddOnManager
	// Target: SMApp
	// Data: entry_ref
	// Reply: none
	// Tells application to add add-on
	SM_ADD_ADDON = '$Maa',
	
	// Message Remove Filter
	// Source: SMWindow(In-Use List)
	// Target: SMApp
	// Data: filter_id
	// Reply: none
	// Tells application to remove filter
	SM_REMOVE_FILTER = '$Mrf',
	
	// Message Move Filter
	// Source: SMWindow(In-Use List)
	// Target: SMApp
	// Data: filter_id, list_position
	// Reply: none
	// Tells application to move filter
	SM_MOVE_FILTER = '$Mmf',
	
	// Message Add Filter
	// Source: SMWindow(In-Use List)
	// Target: SMApp
	// Data: addon_name, list_position
	// Reply: SM_NEW_FILTER
	// Tells application to add filter
	SM_ADD_FILTER = '$Maf',
	
	// Message New Filter
	// Source: SMApp
	// Target: SMWindow(In-Use List)
	// Data: filter_id
	// Reply: none
	// Passes filter id to list
	SM_NEW_FILTER = '$Mnf',
	
	// Message Filter List
	// Source: SMApp
	// Target: SMWindow
	// Data: filter_name, filter_id
	// Reply: none
	// Tells window to add filters to list
	SM_FILTER_LIST = '$Mfl',
	
	// Message View
	// Source: SMApp
	// Target: SMWindow
	// Data: view
	// Reply: none
	// Sends a view to the window as a control panel
	SM_VIEW = '$Mar',
	
	// Message Update Available Add-ons
	// Source: SMApp
	// Target: SMWindow
	// Data: addon_name
	// Reply: none
	// Tells window to update available add-ons list
	SM_UPDATE_ADDONS = '$Mua',
	
	// Message Help Requested
	// Source: SMWindow
	// Target: SMWindow
	// Data: none
	// Reply: none
	// Tells window to open help file
	SM_HELP_REQUESTED = '$Mhr',
	
	// Message Get Add-on List
	// Source: SMApp
	// Target: SMAddOnManager
	// Data: none
	// Reply: SM_ADDON_LIST
	// Requests an list of add-ons from the add-on manager
	SM_GET_ADDON_LIST = '$Mga',
	
	// Message Add-on List
	// Source: SMAddOnManager
	// Target: SMApp
	// Data: entry_refs
	// Reply: none
	// Sends a list of add-ons to the application
	SM_ADDON_LIST = '$Mna',
	
	// Message Toggle Play
	// Source: SMWindow(Play Button)
	// Target: SMWindow
	// Data: none
	// Reply: none
	// Toggles filtering on and off
	SM_TOGGLE_PLAY = '$Mtp'
};
