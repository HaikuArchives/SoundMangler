/*
	SMWindow.cpp
*/

#include "SMApp.h"
#include "SMWindow.h"
#include "DraggableListView.h"
#include "BitmapButton.h"

SMWindow::SMWindow()
		: BWindow(BRect(100,100,600,400), "Sound Mangler", B_TITLED_WINDOW, B_NOT_H_RESIZABLE|B_NOT_ZOOMABLE)
{	
	DraggableListView *availableList, *activeList;
	BScrollView *availableScroll, *activeScroll;
	BButton *soundPrefsButton; 
	BitmapButton *toggleButton;
	BRect frame;
	frame = Bounds();
	frame.OffsetTo(B_ORIGIN);
	float frame_w = frame.Width();
	float frame_h = frame.Height();	
	BMenuBar *super = new BMenuBar(frame,"super");
	BMenu *menu;
	BMenuItem *item;

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
	
	SetSizeLimits(frame_w,frame_w,frame_h,9999);

	float offset = (super->Bounds()).Height();
	frame = Bounds();
	frame.OffsetTo(B_ORIGIN);	
	frame.Set(0,offset,frame_w,frame_h);
	BView *background = new BView(frame, "background", B_FOLLOW_ALL_SIDES, B_WILL_DRAW|B_NAVIGABLE);		
	background->SetViewColor(220,220,220);
	
	frame_h -= (offset+5);
	
	frame.Set(5,5,(frame_w/4)-B_V_SCROLL_BAR_WIDTH,frame_h);
	availableList = new DraggableListView(frame, "availableView", false, B_SINGLE_SELECTION_LIST, B_FOLLOW_TOP_BOTTOM);
	availableScroll = new BScrollView("availableScrollView",availableList,B_FOLLOW_TOP_BOTTOM,0,false,true);
	background->AddChild(availableScroll);
	
	frame.Set((frame_w/4)+5,5,(frame_w/2)-B_V_SCROLL_BAR_WIDTH,frame_h);
	activeList = new DraggableListView(frame, "activeView", true, B_SINGLE_SELECTION_LIST, B_FOLLOW_TOP_BOTTOM);
	activeScroll = new BScrollView("activeScrollView",activeList,B_FOLLOW_TOP_BOTTOM,0,false,true);
	background->AddChild(activeScroll);
	
	// populate availableView
	BList* l = new BList;
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
	
	frame.Set((frame_w/2)+5,frame_h - 20,3*frame_w/4,frame_h);
	soundPrefsButton = new BButton(frame, "soundPrefs", "Sound Preferences",NULL, B_FOLLOW_V_CENTER);
	background->AddChild(soundPrefsButton);
	
	BBitmap *buttonBitmap;
	BRect tempRect;
	
	frame.Set((3*frame_w/4)+5,frame_h - 20,frame_w-5,frame_h);
	//toggleButton = new BitmapButton(frame, "toggleButton", NULL, B_TWO_STATE_BUTTON, B_FOLLOW_V_CENTER);
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
