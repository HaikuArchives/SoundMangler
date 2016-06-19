#ifndef SM_FILTER_MANAGER
#define SM_FILTER_MANAGER

// SMFilterManager
//  Jason E. Luther - jluther@uiuc.edu

#include <support/Errors.h>
#include <support/List.h>
#include <support/Locker.h>

// The SMFilter controls the loading/unloading of add-ons and the
//  creation and destruction of SMFilters. 

class SMFilterManager
{
public:
	SMFilterManager();
	~SMFilterManager();
	BLocker* getLock() const;
	BList* getFilterList() const;
	status_t Commission(char* filterName, int32 index);
	status_t Decommission(int32 index);
	SMFilter* getFilterByIndex(int32 index);
	
private:
	BList* filters;
	BLocker* lock;
};

#endif // SM_FILTER_MANAGER

