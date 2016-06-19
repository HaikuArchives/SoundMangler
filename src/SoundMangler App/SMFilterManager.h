// Class SMFilterManager 
// Jason E. Luther - jluther@uiuc.edu 
// Be Users Group @ UIUC - SoundMangler Project 
// 2/17/1998 

#pragma once 

#include <support/Errors.h> 
#include <support/List.h> 
#include <support/Locker.h> 
#include <string>
#include <map>
#include "SMFilter.h"
#include <app/Message.h>
#include "SMMsgDefs.h"

typedef map< string, image_id, less<string>, allocator<image_id> > image_id_map;
typedef map< string, BView*, less<string>, allocator<BView*> > about_view_map;
typedef map< filter_id, SMFilter*, less<filter_id>, allocator<SMFilter*> > filter_id_map;
typedef map< filter_id, BView*, less<filter_id>, allocator<BView*> > prefs_view_map;

// The filter manager controls the loading/unloading of add-ons and 
//  the creation and destruction of filters. 
class SMFilterManager { 
        public: 
			SMFilterManager(); 
			~SMFilterManager(); 
			BLocker* getLock() const; 
			BList* getFilterList() const; 
			status_t Activate(char* filterName, int listPosition, filter_id& fID); 
			status_t Deactivate(filter_id id); 
			BView* PrefsView(filter_id id);
			BView* AboutView(char * fiterName); 
			SMFilter* getFilterByID(filter_id id); 
			BMessage* getIDMapList();
			BMessage* getAvailNames();
                
        private: 
			BList* filters;			// list of pointers to filters 
			BLocker* lock;			// lock that controls access to filters
			image_id_map images;	// filter name/image_id map
			filter_id_map ids;		// filter_id/SMFilter* map
			BMessage fList;			// BMessage with list of filter_id/names
			BMessage fNames;		// BMessage with list of available filters
			about_view_map avmap;	// AboutViews that are created when an image is loaded
			prefs_view_map pvmap;	// PrefsViews that are created when a filter is instantiated			
};
