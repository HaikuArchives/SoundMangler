// populateFilterList
//  Jason E. Luther - jluther@uiuc.edu
#include <Directory.h>
#include <support/List.h>

void populateFilterList(BList*&);

void populateFilterList(BList*& l) {

	BDirectory* d = new BDirectory("/boot/apps/SoundMangler/add-ons");
	l->MakeEmpty();
	BEntry* entry = new BEntry;
	char name[B_FILE_NAME_LENGTH];

	while (B_ENTRY_NOT_FOUND != d->GetNextEntry(entry)) {
		entry->GetName(name);
		l->AddItem(name);
	}
	
	delete d;
	delete entry;
	return;
}