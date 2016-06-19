// SMFilterApp.h
//
// November 22, 1997
// Vikram Kulkarni
// Be Users Group at UIUC

#pragma once

#define SM_FILTER_SIG "application/x-vnd.BUGatUIUC-MuteFilter"

class SMFilterApp : public BApplication {
	public:
		SMFilterApp();
		~SMFilterApp();
		virtual void MessageReceived(BMessage *in_msg);
		virtual void Pulse();
		void TryGuesses(BDirectory* dir);
		void Move(BDirectory* dir);
};
