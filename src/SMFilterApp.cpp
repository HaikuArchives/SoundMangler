// SMFilterApp.cpp
//
// November 22, 1997
// Vikram Kulkarni
// Be Users Group at UIUC

#include "SMFilterApp.h"

int main()
{
	SMFilterApp *app = new SMFilterApp();
	if (app)
		app->Run();
	
	delete (app);
	return (1);
}

SMFilterApp::SMFilterApp()
	: BApplication(SM_FILTER_SIG)
{
	BAlert *welcome = new BAlert("", "Press okay to try and auto-install this Sound Mangler filter.", "Okay");
	welcome->Go();
	
	SM_directory = NULL;
	
	TryGuesses();
	
	if (!SM_directory)
	{
		BAlert *alert = new BAlert("", "Error trying to auto install filter.\nCould not find Sound Mangler's folder.\nPlease copy this filter into the add-ons subdirectory in the Sound Mangler directory.", "Okay");
		alert->Go();
	}
	else

	Move();
	
	PostMessage(B_QUIT_REQUESTED);
}

SMFilterApp::~SMFilterApp()
{
	if (SM_directory)
		delete (SM_directory);
}

void SMFilterApp::MessageReceived( BMessage *in_msg )
{
	switch(in_msg->what)
	{
		default:
		{
			BApplication::MessageReceived(in_msg);
			break;
		}
	}
}

void SMFilterApp::TryGuesses()
{
	BDirectory *dir = new BDirectory();
	
	dir->SetTo("/boot/apps/SoundMangler/");
	
	if (dir->InitCheck() == B_OK)
		SM_directory = new BDirectory(*dir);
	else
	{
		dir->Unset();
		dir->SetTo("/boot/home/SoundMangler/");
		
		if (dir->InitCheck() == B_OK)
			SM_directory = new BDirectory(*dir);
	}
	
	delete dir;
}

status_t SMFilterApp::Move()
{
	app_info SMF_info;
	if (GetAppInfo(&SMF_info) != B_OK)
		{
			BAlert *alert = new BAlert("", "GetAppInfo Failed.  Could not auto install filter.", "Sorry");
			alert->Go();
			return B_ERROR;
		}
	BEntry SMF_entry(&(SMF_info.ref), false);
	
	if (SMF_entry.InitCheck() != B_OK)
		{
			BAlert *alert = new BAlert("", "Unable to create BEntry for filter.  Could not auto install filter.", "Sorry");
			alert->Go();
			return B_ERROR;
		}
	char addon_name[B_FILE_NAME_LENGTH];
	SMF_entry.GetName(addon_name);
	char relative_path[10 + B_FILE_NAME_LENGTH];
	sprintf(relative_path, "./add-ons/%s", addon_name);
	
	switch(SMF_entry.MoveTo(SM_directory, relative_path, false))
	{
		case B_NO_ERROR:
			{
				BAlert *alert = new BAlert("", "The filter was installed successtully.\n		Enjoy.", "Cool");
				alert->Go();
				return B_OK;
				break;
			}
			
		case B_FILE_EXISTS:
			{
				BAlert *alert = new BAlert("", "Error, the file exists.", "Sorry");
				alert->Go();
				return B_ERROR;
				break;
			}
		
		default:
			{
				return B_ERROR;
				break;
			}
	}
}

