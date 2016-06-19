// Class SMWindow
// Be Users Group @ UIUC - SoundMangler Project
// 2/17/1998

#include <AppKit.h>

#include "Colors.h"
#include "DraggableListView.h"
#include "SMMsgDefs.h"
#include "SMPlayButton.h"
#include "SMWindow.h"

SMWindow::SMWindow()
	:BWindow(BRect(100,100,600,400), "Sound Mangler", B_TITLED_WINDOW, B_NOT_H_RESIZABLE|B_NOT_ZOOMABLE) {	
	is_filtering = false;
	
	SetSizeLimits(Bounds().Width(),Bounds().Width(),Bounds().Height(),9999);
	
	BMenuBar* super = new BMenuBar(Bounds(),"super");
	
	BMenu* menu = new BMenu("File");
	menu->AddItem(new BMenuItem("New",new BMessage(SM_NEW),'N'));
	menu->AddItem(new BMenuItem("Open",new BMessage(SM_OPEN),'O'));
	menu->AddSeparatorItem();
	menu->AddItem(new BMenuItem("Save",new BMessage(SM_SAVE),'S'));
	menu->AddItem(new BMenuItem("Save As"B_UTF8_ELLIPSIS,new BMessage(SM_SAVE_AS),'S',B_SHIFT_KEY));
	menu->SetTargetForItems(be_app);
	menu->AddSeparatorItem();
	menu->AddItem(new BMenuItem("Quit",new BMessage(B_QUIT_REQUESTED),'Q'));
	super->AddItem(menu);
	
	menu = new BMenu("Edit");
	menu->AddItem(new BMenuItem("Cut",NULL,'X'));
	menu->AddItem(new BMenuItem("Copy",NULL,'C'));
	menu->AddItem(new BMenuItem("Paste",NULL,'V'));
	menu->AddItem(new BMenuItem("Clear",NULL));
	menu->AddItem(new BMenuItem("Select All",NULL,'A'));
	super->AddItem(menu);

	menu = new BMenu("Help");
	menu->AddItem(new BMenuItem("About Sound Mangler", new BMessage(B_ABOUT_REQUESTED)));
	menu->SetTargetForItems(be_app);
	menu->AddItem(new BMenuItem("Using Sound Mangler", new BMessage(SM_HELP_REQUESTED)));
	menu->AddItem(new BMenuItem("Add-On Help",NULL));
	super->AddItem(menu);
	
	AddChild(super);
	
	int32 menu_bar_height = super->Bounds().Height();
	BRect back_frame = Bounds();
	back_frame.top += menu_bar_height;
	BView* back = new BView(back_frame, "background", B_FOLLOW_ALL_SIDES, B_WILL_DRAW|B_NAVIGABLE);
	back->SetViewColor(BeBackgroundGrey);
	
	DraggableListView* available_list = new DraggableListView(BRect(5,5,110,277), "available_list", false, false, B_SINGLE_SELECTION_LIST, B_FOLLOW_TOP_BOTTOM);
	back->AddChild(new BScrollView("available_scroll_view",available_list,B_FOLLOW_TOP_BOTTOM,0,false,true));
//	available_list->AddItem(new BStringItem("foo filter"));
	
	DraggableListView* active_list = new DraggableListView(BRect(130,5,235,277), "active_list", true, true, B_SINGLE_SELECTION_LIST, B_FOLLOW_TOP_BOTTOM);
	back->AddChild(new BScrollView("active_scroll_view",active_list,B_FOLLOW_TOP_BOTTOM,0,false,true));
	
	back->AddChild(new BButton(BRect(255,255,375,277), "soundPrefs", "Sound Preferences", new BMessage(SM_SOUND_PREFS), B_FOLLOW_LEFT | B_FOLLOW_BOTTOM));
	back->AddChild(new SMPlayButton(BRect(380,256,420,278), "play_button", new BMessage(SM_TOGGLE_PLAY), B_FOLLOW_LEFT | B_FOLLOW_BOTTOM));
	
	AddChild(back);
	Show();
}

SMWindow::~SMWindow() {
}

void SMWindow::MessageReceived(BMessage *msg) {
	if (msg->what != B_PULSE) {
		printf("SMWindow:\n");
		msg->PrintToStream();
		printf("\n");
	}
	switch(msg->what) {
		case SM_SOUND_PREFS: {
			// Open Sound Preferences
			status_t err = (be_roster->Launch("application/x-vnd.Be-SOND"));
			if (err == B_ALREADY_RUNNING)
				be_roster->ActivateApp(be_roster->TeamFor("application/x-vnd.Be-SOND"));
			else if (err != B_OK)
				(new BAlert("", "Unable to open Sound Preferences!", "OK"))->Go();
			break;
		}
		case SM_VIEW:
			// Add view to the filter prefs panel
			break;
		case SM_UPDATE_ADDONS: {
			// Delete contents of add-on list and add new set
			BListView* available_list = cast_as(FindView("available_list"), BListView);
			int32 index = 0;
			while (index < available_list->CountItems()) {
				delete available_list->RemoveItem(index);
				index++;
			}
			index = 0;
			int32 addon_count = 0;
			type_code type_found;
			msg->GetInfo("addon_name", &type_found, &addon_count);
			while (index < addon_count) {
				char* name;
				msg->FindString("addon_name", index, &name);
				available_list->AddItem(new BStringItem(name));
				index++;
			}
			break;
		}
		case SM_FILTER_LIST: {
			// Delete contents of add-on list and add new set
			BListView* active_list = cast_as(FindView("active_list"), BListView);
			int32 index = 0;
			int32 addon_count = 0;
			type_code type_found;
			msg->GetInfo("filter_name", &type_found, &addon_count);
			while (index < addon_count) {
				char* name;
				int32 id;
				msg->FindString("filter_name", index, &name);
				msg->FindInt32("filter_id", index, &id);
				active_list->AddItem(new FilterItem(name, id));
				index++;
			}
			break;
		}
		case SM_HELP_REQUESTED:
			// Open help window
			break;
		case SM_TOGGLE_PLAY:
			// Toggle filtering on and off
			if (is_filtering) {
				be_app->PostMessage(SM_STOP);
				is_filtering = false;
			} else {
				be_app->PostMessage(SM_START);
				is_filtering = true;
			}
			break;
		default:
			BWindow::MessageReceived(msg);
			break;
	}
}
