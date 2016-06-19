// SMFilter.cpp
//
// November 22, 1997
// Vikram Kulkarni
// Be Users Group at UIUC

#include "SMFilter.h"

SMFilter::SMFilter(const char* name)
	:BLooper(name) {
}

SMFilter::SMFilter(BMessage* archive)
	:BLooper(archive) {
}

// This is for any initialization that you need to do when you app is
// loaded.
status_t SMFilter::Initialize() {
	return B_NO_ERROR;
}
