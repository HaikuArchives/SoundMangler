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
	lock = new BLocker("FilterManager Lock");
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
			fprintf(stdout, "image for filter %s is already loaded\n", 
				filterName); fflush(stdout);
			break;
		}
	}
	if (! inUse) {
		fprintf(stdout, "loading %s image\n", filterName); fflush(stdout);
		char * path = "/boot/apps/SoundMangler/add-ons/";
		char * filterFileName = new char(strlen(path) + strlen(filterName) + 1);
		strcpy(filterFileName, path);
		strcat(filterFileName, filterName);
		fprintf(stdout, "trying %s\n", filterFileName); fflush(stdout);
		image = load_add_on(filterFileName);
		fprintf(stdout, "image_id = %d\n", image); fflush(stdout);
		if (image < 0) {
			fprintf(stdout, "couldn't load %s image\n", filterName); fflush(stdout); 
			(new BAlert("", "couldn't load image", "Damn"))->Go();
			exit(0);
		}
	}
	// get function pointer to MakeNewFilter
	fprintf(stdout, "Getting creator function\n"); fflush(stdout);
	SMFilter* (*creator)(char* fname, image_id img);
	if (B_OK != 
		get_image_symbol(
			image, "MakeNewFilter", 
			B_SYMBOL_TYPE_TEXT, &creator)
		) {
		// upon an error, cry like a baby
		(new BAlert("", "couldn't get creator function!", "Damn"))->Go();
		exit(0);
	}
	// create filter
	fprintf(stdout, "creating the filter with %s, %d\n", filterName, image); fflush(stdout);
	SMFilter* filter = (*creator)(filterName, image);
	if (NULL == filter) {
		(new BAlert("", "filter is NULL", "Damn"))->Go();
		exit(0);
	}
	// call filter's initialize function
	fprintf(stdout, "initializing filter\n"); fflush(stdout);
	if (filter->Initialize() != B_OK) {
		(new BAlert("", "Problem initializing filter.", "OK"))->Go();
		fprintf(stdout, "Problem initializing filter.\n"); fflush(stdout);
		delete filter;
		unload_add_on(image);
		// BAlert error in filter
		// Tell Matt
		return B_ERROR;
	}
	// add to lists
	fprintf(stdout, "adding the filter to the list\n"); fflush(stdout);
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
