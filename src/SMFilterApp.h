// SMFilterApp.h
//
// November 22, 1997
// Vikram Kulkarni
// Be Users Group at UIUC

#pragma once

#ifndef _SOUND_MANGLER_FILTER_APP_H_
#define _SOUND_MANGLER_FILTER_APP_H_

#include <Application.h>

#define SM_FILTER_SIG "application/x-vnd.BUGatUIUC-MuteFilter"

class SMFilterApp : public BApplication
{
	public:
						SMFilterApp();
						~SMFilterApp();
		
		virtual void	MessageReceived( BMessage *in_msg );
		void			TryGuesses();
		status_t		Move();
		
	private:
		BDirectory		*SM_directory;
};

#endif
