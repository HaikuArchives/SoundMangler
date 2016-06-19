// SMFilterManager definintions
//  Jason E. Luther - jluther@uiuc.edu

#include <support/Locker.h>
#include <support/List.h>
#include <support/Errors.h>
#include <stdio.h>
#include "SMFilter.h"
#include "SMFilterManager.h"
#include <kernel/image.h>
#include <storage/Directory.h>

SMFilterManager::SMFilterManager() :
	fList(BMessage(SM_FILTER_LIST)),
	fNames(BMessage(SM_UPDATE_ADDONS))
{
	filters = new BList;
	lock = new BLocker("Filter List Lock");
	
	// load add-ons
	// this is ugly - should search other places
	char * path="/boot/home/config/add-ons/SoundMangler/";
	BDirectory dir(path);
	image_id image;
	char name[B_FILE_NAME_LENGTH];
	BEntry entry;
	while (B_ENTRY_NOT_FOUND != dir.GetNextEntry(&entry)) {
		entry.GetName(name);
		//entry.GetRef(&tmpRef);
		char * filterName = new char[strlen(name) + 1];
		strcpy(filterName, name);
		char * filterFileName = new char[strlen(path) + strlen(filterName) + 1];
		strcpy(filterFileName, path);
		strcat(filterFileName, filterName);
		fprintf(stdout, "trying %s\n", filterFileName); fflush(stdout);
		image = load_add_on(filterFileName);
		fprintf(stdout, "image_id = %d\n", image); fflush(stdout);
		if (image < 0) {
			fprintf(stdout, "couldn't load %s image\n", filterName); fflush(stdout); 
		}
		images[filterName]=image;  // add to map
		fNames.AddString("addon_name", filterName);
	}	
}

SMFilterManager::~SMFilterManager() {

	// delete filters and unload images

	for (image_id_map::iterator p=images.begin(); p!=images.end(); ++p) {
		fprintf(stdout, "Unloading %s, id: %d. ", (*p).first.c_str(), (*p).second); fflush(stdout);
		unload_add_on((*p).second);
		fprintf(stdout, "done.\n"); fflush(stdout);
	}	

	fprintf(stdout, "Deleting filters. "); fflush(stdout);
	SMFilter* f;
	SMFilter* tmp;		
	for (long i = 0; f = (SMFilter*)filters->ItemAt(i); i++) {
		tmp = (SMFilter*)filters->RemoveItem(i);
		delete tmp;
	}
	fprintf(stdout, "done.\n"); fflush(stdout);

	// delete lists and lock
	// (make sure we're not filtering!)
	fprintf(stdout, "Deleting filters. "); fflush(stdout);
	delete filters;
	fprintf(stdout, "done.\n"); fflush(stdout);
	fprintf(stdout, "Deleting lock. "); fflush(stdout);
	delete lock;
	fprintf(stdout, "done.\n"); fflush(stdout);
}

BLocker* SMFilterManager::getLock() const {
	return lock;
}

BList* SMFilterManager::getFilterList() const {
	return filters;
}

status_t SMFilterManager::Activate(char* filterName, int pos, filter_id& fID) {

	// find addon image
	image_id image;
	image_id_map::iterator p = images.find(filterName);
	if (p!=images.end()) {
		image = (*p).second;
		fprintf(stdout, "image for filter %s is %d\n", 
			filterName, image); fflush(stdout);
	}
	else {
		fprintf(stdout, "couldn't find %s's image\n", filterName); fflush(stdout); 
		(new BAlert("", "couldn't find image", "Damn"))->Go();
		return B_ERROR;
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
		fprintf(stdout, "couldn't get creator function!\n"); fflush(stdout);
		exit(0);
	}
	// create filter
	fprintf(stdout, "creating the filter with %s, %d\n", filterName, image); fflush(stdout);
	SMFilter* filter = (*creator)(filterName, image);
	if (NULL == filter) {
		(new BAlert("", "filter is NULL", "Damn"))->Go();
		fprintf(stdout, "filter is NULL!\n"); fflush(stdout);
		exit(0);
	}
	// call filter's initialize function
	fprintf(stdout, "initializing filter\n"); fflush(stdout);
	if (filter->Initialize() != B_OK) {
		fprintf(stdout, "Problem initializing filter.\n"); fflush(stdout);
		(new BAlert("", "Problem initializing filter.", "OK"))->Go();
		delete filter;
		unload_add_on(image);
		// BAlert error in filter
		// Tell Matt
		return B_ERROR;
	}
	// add to lists
	fprintf(stdout, "adding the filter to the list\n"); fflush(stdout);
	// filter id
	filter_id id = (filter_id)filter;
	ids[id] = filter;
	// filter* list
	lock->Lock();
	filters->AddItem(filter, pos);
	lock->Unlock();
		
	// set the filter id
	fID = id;
	return B_OK;
}

status_t SMFilterManager::Deactivate(filter_id id) {
	// get the filter
	SMFilter* tmpf;
	filter_id_map::iterator p = ids.find(id);
	if (p!=ids.end()) {
		tmpf = (*p).second;
		fprintf(stdout, "found filter by id %d\n",  id); fflush(stdout);
	}
	else
		fprintf(stdout, "Damn! find filter by id %d failed!\a\n",  id); fflush(stdout);
	
	// remove the filter from the list	
	lock->Lock();
	if (!(SMFilter*)filters->RemoveItem(tmpf)) {
		fprintf(stdout, "FilterManager: Couldn't remove the filter!\n"); fflush(stdout);
		return B_ERROR;
	}
	lock->Unlock();

	//  delete filter
	delete tmpf;
	return B_OK;	
}

BView* SMFilterManager::PrefsView(filter_id id) { return NULL; } 
BView* SMFilterManager::AboutView(filter_id id) { return NULL; } 

BMessage* SMFilterManager::getIDMapList() { 
	// shove the map contents into a message
	for (filter_id_map::iterator p=ids.begin(); p!=ids.end(); ++p) {
		fList.AddInt32("filter_id", (*p).first);
		fList.AddString("filter_name", ((*p).second)->getName());
	}
	return &fList;
}

BMessage* SMFilterManager::getAvailNames() {
	return &fNames;
}