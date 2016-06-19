// SMFilter.h
//
// November 22, 1997
// Vikram Kulkarni
// Be Users Group at UIUC

#pragma once

#pragma export on

typedef int32 filter_id;

class SMFilter : public BLooper {
	public:
		// Constructor and Destructor
		SMFilter(const char* name);
		SMFilter(BMessage* archive);
		
		// Filter functions
		virtual status_t Initialize();
		virtual void Filter( int16 * left, int16 * right, int32 count ) = 0;
		virtual BView* PrefsView() = 0;
};

extern "C" SMFilter* MakeNewFilter( const char * name );
extern "C" BView* AboutView(); // a view to be displayed when a filter is selected

#pragma export off
