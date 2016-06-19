// Class SMWindow
// Be Users Group @ UIUC - SoundMangler Project
// 2/16/1998

#pragma once

// The window provides an interface to the filter system. The user
//  can change the list of filters in use, save or load them on
//  the disk, change the sound preferences, and turn filtering
//  on or off.
class SMWindow : public BWindow {
	public:
		SMWindow();
		~SMWindow();
		virtual void MessageReceived(BMessage* msg);
	protected:
		bool is_filtering;
		BView* default_view;
};
