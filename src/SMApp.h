// Class SMApp
// Be Users Group @ UIUC - SoundMangler Project
// 2/16/1998

#pragma once

// The MIME type application signature
#define SM_APP_SIGNATURE "application/x-vnd.BUGatUIUC-SoundMangler"
#define SM_FILE_SIGNATURE "application/x-vnd.BUGatUIUC-SoundManglerPrefs"

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
		void Open(entry_ref gets);
		void SettingFunc(entry_ref setter);
		
		// Member Data
		entry_ref filter_document;
		BFilePanel* open_panel;
		BFilePanel* save_panel;
		SMFilterManager* filter_manager;
		BMessenger main_window;
		BMessenger sound_engine;
		BMessenger addon_directory;
};
