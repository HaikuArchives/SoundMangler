// Class SMApp
// Be Users Group @ UIUC - SoundMangler Project
// 2/16/1998

#include "SMAddOnDirectory.h"
#include "SMApp.h"
#include "SMFilterManager.h"
#include "SMSoundEngine.h"
#include "SMMsgDefs.h"
#include "SMRefFilter.h"
#include "SMWindow.h"

int main() {	
	SMApp myApplication;
	myApplication.Run();
	return 0;
}

SMApp::SMApp() :
	BApplication(SM_APP_SIGNATURE)
{
	SetPulseRate(50000);
	open_panel = new BFilePanel(B_OPEN_PANEL, new BMessenger(this), new entry_ref, B_FILE_NODE, false, NULL, new SMRefFilter);
	save_panel = new BFilePanel(B_SAVE_PANEL, new BMessenger(this), new entry_ref, B_FILE_NODE, false);
	filter_manager = new SMFilterManager;
	main_window = BMessenger(new SMWindow);
	sound_engine = BMessenger(new SMSoundEngine(adc, filter_manager->getFilterList(), filter_manager->getLock()));
	addon_directory = BMessenger(new SMAddOnDirectory);
}

SMApp::~SMApp() {
	main_window.SendMessage(B_QUIT_REQUESTED);
	sound_engine.SendMessage(B_QUIT_REQUESTED);
	addon_directory.SendMessage(B_QUIT_REQUESTED);
	delete open_panel;
	delete save_panel;
	delete filter_manager;
}

void SMApp::ReadyToRun() {
	addon_directory.SendMessage(SM_GET_ADDON_LIST);
	// update the list of add-ons in use
//	main_window.SendMessage(filter_manager->getIDMapList());
	main_window.SendMessage(filter_manager->getAvailNames());
}

void SMApp::AboutRequested() {
	(new BAlert("","SoundMangler 1.0beta\nEngineering Open House 1998\
\n© 1998 BUG@UIUC\
\nthe Be Users Group at University of Illinois\n\
http://www.acm.uiuc.edu/bug/projects/soundmangler/\n","OK"))->Go();
}

void SMApp::MessageReceived(BMessage *msg) {
	if (msg->what != B_PULSE) {
		printf("SMApp:\n");
		msg->PrintToStream();
		printf("\n");
	}
	switch(msg->what) {
		case SM_NEW:
			// Clear in-use filter list and unset save file
			break;
		case SM_OPEN:
			// Allow user to select file to open
			open_panel->Show();
			break;
		case SM_SAVE: {
			// Save to currently opened file
			BMessage numsg(B_REFS_RECEIVED);
			numsg.AddRef("refs", &filter_document);
			PostMessage(&numsg);
			break;
		}
		case SM_SAVE_AS:
			// Allow user to select file to save to
			save_panel->Show();
			break;
		case SM_START:
			// Tell the sound filters to start
			sound_engine.SendMessage(msg);
			break;
		case SM_STOP:
			// Tell the sound filters to stop
			sound_engine.SendMessage(msg);
			break;
		case SM_ABOUT_VIEW:
			// Get the about view for the selected filter and return it
			break;
		case SM_PREFS_VIEW:
			// Get the prefs view for the selected filter and return it
			break;
		case SM_REMOVE_ADDON:
			// Remove the addon from the available list
			break;
		case SM_ADD_ADDON:
			// Add the addon from the available list
			break;
		case SM_REMOVE_FILTER:
			// The filter is no longer in-use
			break;
		case SM_MOVE_FILTER:
			// The filter previously removed has been added to the in-use list
			break;
		case SM_ADD_FILTER: {
			// An add-on has been added to the in-use list
			int32 position;
			char *name;
			msg->FindString("addon_name", &name);
			msg->FindInt32("list_position", &position);
			int fID;
			filter_manager->Activate(name, position, fID);
			BMessage reply(SM_NEW_FILTER);
			reply.AddInt32("filter_id", fID);
			msg->SendReply(&reply);

			break;
		}
		case SM_ADDON_LIST:
			// Update list of add-ons
			break;
		default:
			BApplication::MessageReceived(msg);
			break;
	}
}

void SMApp::Pulse() {
	if (!main_window.IsValid())
		PostMessage(B_QUIT_REQUESTED);
}

bool SMApp::QuitRequested() {
	return BApplication::QuitRequested();
}

void SMApp::RefsReceived(BMessage *message) {
	entry_ref gets;
	status_t error;
	error = message->FindRef("refs", &gets);
	if (error != B_OK)
		(new BAlert("", "App: File Reference not Found in SMApp::RefsReceived" , "OK"))->Go();
	Open(gets);
}

void SMApp::Save() {
}

void SMApp::Open(entry_ref gets) {
}

void SMApp::SettingFunc(entry_ref setter) {
}
