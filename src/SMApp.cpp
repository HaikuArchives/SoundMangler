#include "SMApp.h"
#include "SMWindow.h"


main() {	
	SMApp *myApplication;
	status_t err;			
	
	// Get the application started
	myApplication = new SMApp();
	myApplication->Run();
	
	// Delete application object
	delete(myApplication);
	return(0);
}

SMApp::SMApp()
	:BApplication(APP_SIGNATURE)
{
	open_panel = new BFilePanel(B_OPEN_PANEL, new BMessenger(this), new entry_ref, B_FILE_NODE, false);
	//open_panel->Show();
	save_panel = new BFilePanel(B_SAVE_PANEL, new BMessenger(this), new entry_ref, B_FILE_NODE, false);
	//save_panel->Show();
	MainWindow = new SMWindow();
	
	
	// make window visible
	MainWindow->Show();
}

SMApp::~SMApp()
	{
		delete open_panel;
	}

void SMApp::RefsReceived(BMessage *message)
	{
		entry_ref gets;
		status_t error;
		error = message->FindRef("refs", &gets);
		if (error != B_OK)
			{
				(new BAlert("", "Debug: File Reference not Found in SMApp::RefsReceived" , "OK"))->Go();
			}
		MainWindow->Open(gets);
	}

void SMApp::MessageReceived(BMessage *message)	
	{
		switch(message->what)
			{
				case B_SAVE_REQUESTED:
					{
					entry_ref dir_ref;
					message->FindRef("directory", &dir_ref);
					const char* name = message->FindString("name");
					BEntry* entry = new BEntry(new BDirectory(&dir_ref), name);
					entry_ref file;				
					entry->GetRef(&file);
					MainWindow->SettingFunc(file);
					MainWindow->Save();
					delete entry;
					break;
					}
				case SM_MENU_OPEN:
					{
					open_panel->Show();
					break;
					}
				case SM_MENU_SAVE:
					{
					MainWindow->Save();
					break;
					}
				case SM_MENU_SAVAS:
					{
					save_panel->Show();
					break;
					}
				default:
					{
					BApplication::MessageReceived(message);
					break;
					}
				}
		}
		