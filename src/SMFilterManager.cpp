// SMFilterManager definintions
//  Jason E. Luther - jluther@uiuc.edu

#include <support/Locker.h>
#include <support/List.h>
#include <support/Errors.h>
#include <stdio.h>
#include "SMFilter.h"
#include "SMFilterManager.h"
#include <kernel/image.h>

SMFilterManager::SMFilterManager() {
	filters = new BList;
	lock = new BLocker("Spank my booty!");
}

SMFilterManager::~SMFilterManager() {
	// delete filters and unload images
	image_id image;
	SMFilter* f;
	SMFilter* tmp;
	for (long i = 0; f = (SMFilter*)filters->ItemAt(i); i++) {
		unload_add_on(f->ReturnImageID());
		tmp = (SMFilter*)filters->RemoveItem(i);
		delete tmp;
	}

	// delete lists and lock
	delete filters;
	delete lock;
}

BLocker* SMFilterManager::getLock() const {
	return lock;
}

BList* SMFilterManager::getFilterList() const {
	return filters;
}

status_t SMFilterManager::Commission(char* filterName, int32 index) {

	// is add-on loaded? 
	image_id image;
	SMFilter* f;
	int inUse = 0;
	for (long i = 0; f = (SMFilter*)filters->ItemAt(i); i++) {
		if (! strcmp(f->ReturnName(), filterName)) {
			image = f->ReturnImageID();
			inUse++;
			break;
		}
	}
	if (! inUse) {
		image = load_add_on(filterName);
	}		
	// get function pointer to MakeNewFilter
	SMFilter* (*creator)(char* fname, image_id img);
	if (B_ERROR == 
		get_image_symbol(
			image, "MakeNewFilter", 
			B_SYMBOL_TYPE_TEXT, &creator)
		) {
		// upon an error, cry like a baby
		exit(0);
	}
	// create filter
	SMFilter* filter = (*creator)(filterName, image);
	if (NULL == filter) {
		exit(0);
	}
	// call filter's initialize function
	if (filter->Initialize() != B_OK) {
		delete filter;
		unload_add_on(image);
		// BAlert error in filter
		// Tell Matt
		return B_ERROR;
	}
	// add to lists
	lock->Lock();
	filters->AddItem(filter);
	lock->Unlock();
	
	return B_OK;
}

status_t SMFilterManager::Decommission(int32 index) {
	// Remove from the list
	lock->Lock();
	SMFilter* tmp = (SMFilter*)filters->RemoveItem(index);
	lock->Unlock();
	
	// Housekeeping
	//  unload image if no longer in use
	image_id img = tmp->ReturnImageID();
	SMFilter* f; // temp filter
	int inUse = 0;
	for (long i = 0; f = (SMFilter*)filters->ItemAt(i); i++) {
		if (f->ReturnImageID() == img) {
			inUse++;
			break;
		}
	}
	if (inUse == 0) {
		unload_add_on(img);
	}
	//  delete filter
	delete tmp;
	
	return B_OK;
}

SMFilter* SMFilterManager::getFilterByIndex(int32 index) {
	return(SMFilter*)(filters->ItemAt(index));
}
