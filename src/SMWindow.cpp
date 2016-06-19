/*
	SMWindow.cpp
		Bryan wrote this, I think

*/

#include "SMApp.h"
#include "SMWindow.h"
#include "DraggableListView.h"
#include "BitmapButton.h"
#include "SMFilterManager.h"
#include <app/Roster.h>

SMWindow::SMWindow()
		: BWindow(BRect(100,100,600,400), "Sound Mangler", B_TITLED_WINDOW, B_NOT_H_RESIZABLE|B_NOT_ZOOMABLE)
{	
	DraggableListView *availableList, *activeList;	// Filter lists
	BScrollView *availableScroll, *activeScroll;	// Filter lists
	BButton *soundPrefsButton; 						// Open Sound Prefs...
	BitmapButton *toggleButton;						// Start/stop filtering
	BRect frame;
	frame = Bounds();
	frame.OffsetTo(B_ORIGIN);
	float frame_w = frame.Width();
	float frame_h = frame.Height();	
	BMenuBar *super = new BMenuBar(frame,"super");
	BMenu *menu;
	BMenuItem *item;
	
	// Set up Menu
	menu = new BMenu("File");
	item = new BMenuItem("New",NULL,'N');
	menu->AddItem(item);
	item = new BMenuItem("Open",new BMessage(SM_MENU_OPEN),'O');
	menu->AddItem(item);
	item = new BMenuItem("Save",new BMessage(SM_MENU_SAVE),'S');
	menu->AddItem(item);
	item = new BMenuItem("Save As...",new BMessage(SM_MENU_SAVAS));
	menu->AddItem(item);
	item = new BMenuItem("Close",NULL);
	menu->AddItem(item);
	menu->AddSeparatorItem();
	item = new BMenuItem("Refresh Add-On List",NULL,'R');
	menu->AddItem(item);
	menu->AddSeparatorItem();
	item = new BMenuItem("Quit",new BMessage('quit'),'Q');
	item->SetTarget(be_app);
	menu->AddItem(item);
	super->AddItem(menu);
	
	menu = new BMenu("Edit");
	item = new BMenuItem("Cut",NULL,'X');
	menu->AddItem(item);
	item = new BMenuItem("Copy",NULL,'C');
	menu->AddItem(item);
	item = new BMenuItem("Paste",NULL,'V');
	menu->AddItem(item);
	item = new BMenuItem("Clear",NULL);
	menu->AddItem(item);
	item = new BMenuItem("Select All",NULL,'A');
	menu->AddItem(item);
	super->AddItem(menu);

	menu = new BMenu("Help");
	item = new BMenuItem("About Sound Mangler", new BMessage(B_ABOUT_REQUESTED));
	item->SetTarget(be_app);
	menu->AddItem(item);
	item = new BMenuItem("Using Sound Mangler",NULL);
	menu->AddItem(item);
	item = new BMenuItem("Add-On Help",NULL);
	menu->AddItem(item);
	super->AddItem(menu);

	AddChild(super);
	SetKeyMenuBar(super);

	// Set up background
	SetSizeLimits(frame_w,frame_w,frame_h,9999);

	float offset = (super->Bounds()).Height();
	frame = Bounds();
	frame.OffsetTo(B_ORIGIN);	
	frame.Set(0,offset,frame_w,frame_h);
	BView *background = new BView(frame, "background", B_FOLLOW_ALL_SIDES, B_WILL_DRAW|B_NAVIGABLE);		
	background->SetViewColor(220,220,220);

	// Set up filter lists	
	frame_h -= (offset+5);
	// Available filters
	frame.Set(5,5,(frame_w/4)-B_V_SCROLL_BAR_WIDTH,frame_h);
	availableList = new DraggableListView(this, frame, "availableView", false, false, B_SINGLE_SELECTION_LIST, B_FOLLOW_TOP_BOTTOM);
	availableScroll = new BScrollView("availableScrollView",availableList,B_FOLLOW_TOP_BOTTOM,0,false,true);
	background->AddChild(availableScroll);
	// Active filters
	frame.Set((frame_w/4)+5,5,(frame_w/2)-B_V_SCROLL_BAR_WIDTH,frame_h);
	activeList = new DraggableListView(this, frame, "activeView", true, true, B_SINGLE_SELECTION_LIST, B_FOLLOW_TOP_BOTTOM);
	activeScroll = new BScrollView("activeScrollView",activeList,B_FOLLOW_TOP_BOTTOM,0,false,true);
//	activeList->SetSelectionMessage(new BMessage(SM_FILTER_SELECTED));
	background->AddChild(activeScroll);

// move this 
	// populate availableView
	BList* l = new BList;
	// change this to get %A/add-ons
	BDirectory* d = new BDirectory("/boot/apps/SoundMangler/add-ons");
	BEntry* entry = new BEntry;
	char name[B_FILE_NAME_LENGTH];
	while (B_ENTRY_NOT_FOUND != d->GetNextEntry(entry)) {
		entry->GetName(name);
		char * tmp = new char[strlen(name) + 1];
		strcpy(tmp, name);
		l->AddItem(new BStringItem(tmp));
	}
	delete d;
	delete entry;
	availableList->AddList(l);
	delete l;
	// ---
	
	// set up button to open sound prefs
	frame.Set((frame_w/2)+5,frame_h - 22,3*frame_w/4,frame_h);
	soundPrefsButton = new BButton(frame, "soundPrefs", "Sound Preferences",
		new BMessage(SM_OPEN_SOUND_PREFS), B_FOLLOW_LEFT | B_FOLLOW_TOP);
	background->AddChild(soundPrefsButton);
	
	// set up start/stop button
	BBitmap *buttonBitmap;
	BRect tempRect;
	
	frame.Set((3*frame_w/4)+5,frame_h - 20,frame_w-5,frame_h);
//	toggleButton = new BitmapButton(frame, "toggleButton", NULL, B_TWO_STATE_BUTTON, B_FOLLOW_V_CENTER);
	tempRect.Set(0, 0, ((BUTTON_WIDTH + 7) & 0xfff8) - 1, BUTTON_HEIGHT);
	toggleButton = new BitmapButton(BRect((3*frame_w/4)+5,frame_h-(BUTTON_HEIGHT),(3*frame_w/4)+5+BUTTON_WIDTH,frame_h),"toggleButton",NULL,B_TWO_STATE_BUTTON);
	buttonBitmap = new BBitmap(tempRect,B_COLOR_8_BIT);
	buttonBitmap->SetBits(play_button,buttonBitmap->BitsLength(),0,B_COLOR_8_BIT);
	toggleButton->SetEnabledOff(buttonBitmap);
	buttonBitmap = new BBitmap(tempRect,B_COLOR_8_BIT);
	buttonBitmap->SetBits(pause_button,buttonBitmap->BitsLength(),0,B_COLOR_8_BIT);
	toggleButton->SetEnabledOn(buttonBitmap);
	buttonBitmap = new BBitmap(tempRect,B_COLOR_8_BIT);
	buttonBitmap->SetBits(play_select_button,buttonBitmap->BitsLength(),0,B_COLOR_8_BIT);
	toggleButton->SetSelectedOff(buttonBitmap);
	buttonBitmap = new BBitmap(tempRect,B_COLOR_8_BIT);
	buttonBitmap->SetBits(pause_select_button,buttonBitmap->BitsLength(),0,B_COLOR_8_BIT);
	toggleButton->SetSelectedOn(buttonBitmap);
	buttonBitmap = new BBitmap(tempRect,B_COLOR_8_BIT);
	buttonBitmap->SetBits(play_disable_button,buttonBitmap->BitsLength(),0,B_COLOR_8_BIT);
	toggleButton->SetDisabledOff(buttonBitmap);
	buttonBitmap = new BBitmap(tempRect,B_COLOR_8_BIT);
	buttonBitmap->SetBits(play_disable_button,buttonBitmap->BitsLength(),0,B_COLOR_8_BIT);
	toggleButton->SetDisabledOn(buttonBitmap);
	toggleButton->SetTarget(this);
	background->AddChild(toggleButton);
	
	AddChild(background);
	
	// Create the filter manager.
	m = new SMFilterManager();
}

SMWindow::~SMWindow() {

}

void SMWindow::AboutRequested() {
}

bool SMWindow::QuitRequested()
{
	be_app->PostMessage(B_QUIT_REQUESTED);
	return(TRUE);
}

// added from manglerwindowsave.cpp 1/4/98
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

		case SM_OPEN_SOUND_PREFS:
		{
			// open up the sound preferences
			status_t r = (be_roster->Launch("application/x-vnd.Be-SOND"));
			if (B_ALREADY_RUNNING == r)
				(new BAlert("", "'Sound Preferences' is already open!", "OK"))->Go();
			else if (r != B_OK)
				(new BAlert("", "Unable to open Sound Preferences!", "OK"))->Go();
			break;
		}

		case SM_ACTIVATE_FILTER:
		{
			char *fName = NULL;
			message->FindString("filter_name", &fName);
			int32 fIndex;
			message->FindInt32("index", &fIndex);
			fprintf(stdout, "%s: %d\n", fName, fIndex); fflush(stdout);
			m->Commission(fName, fIndex);
			break;
		}

		case SM_FILTER_SELECTED:
		{
			fprintf(stdout, "SM_FILTER_SELECTED\n"); fflush(stdout);
			BListView* l;
			status_t er = message->FindPointer("source", &l);
			if (er != B_OK) {
			}
			int index = l->CurrentSelection();
			BStringItem * item = (BStringItem*)(l->ItemAt(index)); 
			fprintf(stdout, "displaying %s\n", item->Text()); fflush(stdout);		
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

