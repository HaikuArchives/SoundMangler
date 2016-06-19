// MuteFilter.h
// VERY Stupid Demo Filter that Mutes everything...
// © 1997 Vikram Kulkarni
// © 1997 Be Users Group at UIUC

#pragma once

#include "SMFilter.h"


#pragma export on

extern "C" {
	SMFilter* MakeNewFilter( const char* name );
}

class MuteFilterView;

class MuteFilter : public SMFilter {
	public:
		MuteFilter( const char* name );
		virtual void MessageReceived( BMessage* msg );
		virtual void Filter( int16* left, int16* right, int32 count);
		virtual BView* PrefsView( BRect rect );
	protected:
		bool mute;
};

#pragma export off
