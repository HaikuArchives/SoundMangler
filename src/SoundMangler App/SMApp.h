// Class SMApp
// Be Users Group @ UIUC - SoundMangler Project
// 2/16/1998

#pragma once

class SMFilterManager;

// The application coordinates the add-ons and filters with the
//  filter master, the window, and the add-on manager.
class SMApp : public BApplication {
	public:
		SMApp();
		~SMApp();
		virtual void AboutRequested();
		virtual void MessageReceived(BMessage *msg);
		virtual void Pulse();
		virtual bool QuitRequested();
		virtual void RefsReceived(BMessage *msg);
		virtual void ReadyToRun();
		
	protected:
		// File Functions
		void Save();
		void Open();
		bool New();
		
		// Member Data
		entry_ref filter_document;
		BFilePanel* open_panel;
		BFilePanel* save_panel;
		SMFilterManager* filter_manager;
		BMessenger main_window;
		BMessenger sound_engine;
		BMessenger addon_directory;
};
