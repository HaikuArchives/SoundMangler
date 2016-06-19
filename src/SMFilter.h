// SMFilter.h
//
// November 22, 1997
// Vikram Kulkarni
// Be Users Group at UIUC

#pragma once

#ifndef _SOUND_MANGLER_FILTER_H_
#define _SOUND_MANGLER_FILTER_H_

#define HELP_REQUESTED 'help'

#pragma export on
class SMFilter
{
	public:
							SMFilter( const char * name, image_id id );
		virtual 			~SMFilter();
						
		virtual status_t	Initialize();
		virtual void		Filter( int16 * left, int16 * right, int32 count ) = 0;
						
		virtual void		LoadPreferences( BMessage *msg );
		virtual BMessage	* ReturnPreferences();
		
		virtual BView		* ReturnView( BRect rect );
		
		char				* ReturnName() const { return MyName; }
		thread_id			ReturnImageID() const { return MyID; }
	
	private:
		char				*MyName;
		image_id			MyID;
};

extern "C" SMFilter * MakeNewFilter( const char * name, image_id id );
#pragma export off

#endif
