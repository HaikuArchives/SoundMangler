// MuteFilter.h
// VERY Stupid Demo Filter that Mutes everything...
// © 1997 Vikram Kulkarni
// © 1997 Be Users Group at UIUC

#ifndef _MUTE_FILTER_H_
#define _MUTE_FILTER_H_

#include "SMFilter.h"

class MuteFilter;
class MuteFilterView;

#pragma export on

extern "C" {
SMFilter* MakeNewFilter( const char * name, image_id id );
}

class MuteFilter : public SMFilter
{
	friend class MuteFilterView;
	public:
							MuteFilter( const char * name, image_id id );
							~MuteFilter();

		virtual void		Filter( int16 * left, int16 * right, int32 count);

		virtual BView		* ReturnView( BRect rect );
		
	private:
		bool				mute;
};

class MuteFilterView : public BView
{
	public:
							MuteFilterView( BRect rect );
							~MuteFilterView();
		virtual void		MessageReceived( BMessage *in_msg );
	
	private:
		MuteFilter			* filter;
};
#pragma export off

#endif
