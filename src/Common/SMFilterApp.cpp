// SMFilterApp.cpp
//
// November 22, 1997
// Vikram Kulkarni
// Be Users Group at UIUC

#include "SMFilterApp.h"
#include "StatusWindow.h"

int main()
{
	SMFilterApp app;
	app.Run();
	return 0;
}

SMFilterApp::SMFilterApp()
	:BApplication(SM_FILTER_SIG) {
	SetPulseRate(500000);
	
	int32 response = (new BAlert("", "Do you want to install this SoundMangler filter?", "Cancel", "OK"))->Go();
	if (response == 0) {
		return;
	}
	
	BRect rect(0,0,256,80);
	rect.OffsetBy(80,90);
	StatusWindow* status = new StatusWindow(rect, "Installing");
	status->Show();
	
	status->UpdateStatus(0, "Locating add-ons directory");
	BDirectory dir;
	TryGuesses(&dir);
	snooze(100000);
	status->UpdateStatus(50, "Installing filter");
	
	if (dir.InitCheck() == B_OK) {
		Move(&dir);
		snooze(100000);
		status->UpdateStatus(100, "Completing installation");
	}
	else {
		(new BAlert("", "The installation was unsuccessful. Please move this filter into the SoundMangler add-on directory manually.", "OK", NULL, NULL, B_WIDTH_AS_USUAL, B_STOP_ALERT))->Go();
		snooze(100000);
		status->UpdateStatus(100, "Cancelling installation");
	}
	snooze(100000);
	status->PostMessage(B_QUIT_REQUESTED);
}

SMFilterApp::~SMFilterApp() {
}

void SMFilterApp::MessageReceived( BMessage *in_msg ) {
	switch(in_msg->what) {
		default: {
			BApplication::MessageReceived(in_msg);
			break;
		}
	}
}

void SMFilterApp::Pulse() {
	if (CountWindows() == 0)
		PostMessage(B_QUIT_REQUESTED);
}

void SMFilterApp::TryGuesses(BDirectory* dir) {
	char addons_path[128];
	int32 addons_path_length = 128;
	status_t err = find_directory(B_USER_ADDONS_DIRECTORY, NULL, false, addons_path, addons_path_length);
	char sm_addons_path[128];
	sprintf(sm_addons_path, "%s/SoundMangler", addons_path);
	create_directory(sm_addons_path, 777);
	dir->SetTo(sm_addons_path);
}

void SMFilterApp::Move(BDirectory* dir)
{
	app_info SMF_info;
	if (GetAppInfo(&SMF_info) != B_OK) {
		(new BAlert("", "The installation was unsuccessful. Please move this filter into the SoundMangler add-on directory manually.", "OK", NULL, NULL, B_WIDTH_AS_USUAL, B_STOP_ALERT))->Go();
		return;
	}
	BEntry SMF_entry(&(SMF_info.ref), false);
	
	if (SMF_entry.InitCheck() != B_OK) {
		(new BAlert("", "The installation was unsuccessful. Please move this filter into the SoundMangler add-on directory manually.", "OK", NULL, NULL, B_WIDTH_AS_USUAL, B_STOP_ALERT))->Go();
		return;
	}
	char addon_name[B_FILE_NAME_LENGTH];
	SMF_entry.GetName(addon_name);
	
	switch (SMF_entry.MoveTo(dir, addon_name, false)) {
		case B_OK:
			break;
		case B_FILE_EXISTS: {
			int32 response = (new BAlert("", "A version of this filter has already been installed. Replace it with this version?", "Cancel", "Replace"))->Go();
			if (response == 1) {
				SMF_entry.MoveTo(dir, addon_name, true);
			}
			break;
		}
		default:
			break;
	}
}

