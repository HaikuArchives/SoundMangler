#include "SMWindow.h"


void SMWindow::MessageReceived(BMessage *message)
	{
	switch(message->what)
		{
		case SM_MENU_OPEN:
		case SM_MENU_SAVE:
		case SM_MENU_SAVAS:
			{
			be_app->PostMessage(message);
			break;
			}
		default:
			{
			BWindow::MessageReceived(message);
			break;
			}
		}
	}

//Saves files
void SMWindow::Save()
	{
	BEntry *temp;
	temp = new BEntry(&document_ref, false);
	BFile *file;
	file = new BFile(temp, B_READ_WRITE | B_CREATE_FILE);
	BResources *resrc;
	resrc = new BResources(file, true);
	uint32 filtercount = 0;
	filtercount = ((BListView*)FindView("activeView"))->CountItems();
	int32 error;
	error = resrc->AddResource(B_UINT32_TYPE, 0, &filtercount, sizeof(uint32), "filtercount");
	char message[128];
	sprintf(message, "There was an error writing to the file %s.", document_ref.name);
	if (error == B_ERROR)
		(new BAlert("", message, "OK"))->Go();
	delete resrc;
	delete file;
	delete temp;
	}		

void SMWindow::SettingFunc(entry_ref setter)
	{
	document_ref = setter;
	}

//Opens			
void SMWindow::Open(entry_ref gets)
	{
	document_ref = gets;
	BEntry *temp;
	temp = new BEntry(&document_ref, false);
	BFile *file;
	file = new BFile(temp, B_READ_ONLY);
	BResources *resrc;
	resrc = new BResources(file, false);
	uint32 *filtercount;
	filtercount = new uint32(0);
	int32 error;
	error = resrc->ReadResource(B_UINT32_TYPE, 0, filtercount, 0, sizeof(uint32));
	char message[128];
	sprintf(message, "There was an error reading from the file %s.", document_ref.name);
	if (error == B_ERROR)
		(new BAlert("", message, "OK"))->Go();
	sprintf(message, "%d", *filtercount);
	(new BAlert("", message, "OK"))->Go();
	delete resrc;
	delete file;
	delete temp;
	}
