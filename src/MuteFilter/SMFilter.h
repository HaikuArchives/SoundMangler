// SMFilter.h
//
// November 22, 1997
// Vikram Kulkarni
// Be Users Group at UIUC

#pragma once

#pragma export on

typedef int32 filter_id;

class SMFilter : public BHandler {
	public:
		// Constructor and Destructor
		SMFilter(const char* name);
		// SMFilter(BMessage* archive);
		~SMFilter();
		
		// Filter functions
		virtual status_t Initialize();
		virtual void Filter( int16 * left, int16 * right, int32 count ) = 0;

		virtual const char* getName() const;
		
	protected:
		char * _name;
};

extern "C" SMFilter* MakeNewFilter( const char * name );

#pragma export off
