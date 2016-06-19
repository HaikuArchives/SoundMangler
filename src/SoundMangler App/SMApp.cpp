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

SMApp::SMApp():
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
		case SM_VIEW:
			// send the message to the window
			main_window.SendMessage(msg);
			break;
		case SM_NEW:
			// Clear in-use filter list and unset save file
			break;
		case SM_OPEN:
			// Allow user to select file to open
			open_panel->Show();
			break;
		case SM_SAVE: {
			// Save to currently opened file
			Save();
			break;
		}
		case SM_SAVE_AS:
			// Allow user to select file to save to
			save_panel->Show();
			break;
		case B_SAVE_REQUESTED: {
			entry_ref dir_ref;
			char* file_name;
			msg->FindRef("directory", &dir_ref);
			msg->FindString("name", &file_name);
			BDirectory dir(&dir_ref);
			BEntry entry(&dir, file_name);
			entry.GetRef(&filter_document);
			Save();
			break;
		}
		case SM_START:
			// Tell the sound filters to start
			sound_engine.SendMessage(msg);
			break;
		case SM_STOP:
			// Tell the sound filters to stop
			sound_engine.SendMessage(msg);
			break;
		case SM_ABOUT_VIEW: {
			// Get the about view for the selected filter and return it
			BView* v;
			char * name;
			msg->FindString("addon_name", &name);
			v = filter_manager->AboutView(name);
			BMessage reply(SM_VIEW);
			reply.AddPointer("view",v);
			msg->SendReply(&reply);
			break;
		}
		case SM_PREFS_VIEW: {
			// Get the about view for the selected filter and return it
			BView* v;
			filter_id fID;
			msg->FindInt32("filter_id", &fID);
			v = filter_manager->PrefsView(fID);
			BMessage reply(SM_VIEW);
			reply.AddPointer("view",v);
			msg->SendReply(&reply);
			break;
		}
		case SM_REMOVE_ADDON:
			// Remove the addon from the available list
			break;
		case SM_ADD_ADDON:
			// Add the addon from the available list
			break;
		case SM_REMOVE_FILTER: {
			// The filter is no longer in-use
			filter_id fID;
			msg->FindInt32("filter_id",&fID);
			filter_manager->Deactivate(fID);
			break;
		}
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
	status_t error;
	error = message->FindRef("refs", &filter_document);
	if (error != B_OK)
		(new BAlert("", "App: File reference not found in SMApp::RefsReceived." , "OK"))->Go();
	Open();
}

void SMApp::Save() {
	BList filter_list(*filter_manager->getFilterList());
	int32 current_filter = 0;
	int32 filter_count = filter_list.CountItems();
	BMessage filter_data;
	BMessage prefs_data;
	while (current_filter < filter_count) {
		((BArchivable*)filter_list.ItemAt(current_filter))->Archive(&filter_data);
		prefs_data.AddMessage("filter_data", &filter_data);
		current_filter++;
	}
	const ssize_t prefs_size = prefs_data.FlattenedSize();
	char* buffer = new char[prefs_size];
	prefs_data.Flatten(buffer, prefs_size);
	BFile file(&filter_document, B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
	file.Write(buffer, prefs_size);
	delete[] buffer;
	BNodeInfo ni(&file);
	ni.SetType(SM_FILE_SIGNATURE);
}

void SMApp::Open() {
	New();
	BFile file(&filter_document, B_READ_ONLY);
	off_t prefs_size = 0;
	file.GetSize(&prefs_size);
	char* buffer = new char[prefs_size];
	file.Read(buffer,prefs_size);
	BMessage prefs_data;
	prefs_data.Unflatten(buffer);
	delete[] buffer;
	BMessage filter_data;
	BList* filter_list = filter_manager->getFilterList();
	BLocker* filter_list_lock = filter_manager->getLock();
	int32 current_filter = 0;
	int32 filter_count = 0;
	type_code type_found;
	prefs_data.GetInfo("filter_data", &type_found, &filter_count);
	while (current_filter < filter_count) {
		prefs_data.FindMessage("filter_data", current_filter, &filter_data);
		filter_list_lock->Lock();
		filter_list->AddItem(cast_as(instantiate_object(&filter_data),SMFilter));
		filter_list_lock->Unlock();
		current_filter++;
	}
	main_window.SendMessage(filter_manager->getIDMapList());
}

bool SMApp::New() {
	BList* filter_list = filter_manager->getFilterList();
	BLocker* filter_list_lock = filter_manager->getLock();
	int32 response = 1;
	if (filter_list->CountItems() > 0)
		response = (new BAlert("","Discard current settings?","Cancel","Discard"))->Go();
	if (response == 1) {
		return true;
	} else
		return false;
}
