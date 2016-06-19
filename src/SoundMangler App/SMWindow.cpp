// Class SMWindow
// Be Users Group @ UIUC - SoundMangler Project
// 2/17/1998

#include "Colors.h"
#include "DraggableListView.h"
#include "SMMsgDefs.h"
#include "SMPlayButton.h"
#include "SMWindow.h"

class BitmapView : public BView {
	public:
		BitmapView(BRect rect, const char* name, BMessage* bm) :
			BView(rect, name, B_FOLLOW_ALL_SIDES, B_WILL_DRAW | B_NAVIGABLE) {
			bitmap = new BBitmap(bm);
		}
		~BitmapView() {
			delete bitmap;
		}
		virtual void Draw(BRect rect) {
			DrawBitmap(bitmap);
		}
	protected:
		BBitmap* bitmap;
};

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
	
	DraggableListView* available_list = new DraggableListView(BRect(5,5,110,277), "available_list", addon, false, false, B_SINGLE_SELECTION_LIST, B_FOLLOW_TOP_BOTTOM);
	back->AddChild(new BScrollView("available_scroll_view",available_list,B_FOLLOW_TOP_BOTTOM,0,false,true));
	available_list->AddItem(new BStringItem("foo filter"));
	
	DraggableListView* active_list = new DraggableListView(BRect(130,5,235,277), "active_list", filter, true, true, B_SINGLE_SELECTION_LIST, B_FOLLOW_TOP_BOTTOM);
	back->AddChild(new BScrollView("active_scroll_view",active_list,B_FOLLOW_TOP_BOTTOM,0,false,true));
	
	back->AddChild(new BButton(BRect(255,255,375,277), "sound_prefs", "Sound Preferences", new BMessage(SM_SOUND_PREFS), B_FOLLOW_LEFT | B_FOLLOW_BOTTOM));
	back->AddChild(new SMPlayButton(BRect(380,256,420,278), "play_button", new BMessage(SM_TOGGLE_PLAY), B_FOLLOW_LEFT | B_FOLLOW_BOTTOM));
	
	app_info info;
	be_app->GetAppInfo(&info);
	BFile app_file(&info.ref, B_READ_ONLY);
	BResources rsrcs(&app_file);
	size_t len;
	int32 res_id;
	rsrcs.GetResourceInfo('ARCV', "Splash Screen", &res_id, &len);
	status_t err;
	char* data = new char[len];
	rsrcs.ReadResource('ARCV', res_id, data, 0, len);
	BMessage archive;
	err = archive.Unflatten(data);
	delete[] data;
	BBitmap splash_screen(&archive);
	default_view = new BitmapView(BRect(255,5,495,250),"default_view", &archive);
	back->AddChild(new BScrollView("filter_prefs_view",default_view,B_FOLLOW_TOP_BOTTOM,0,false,false));
	
	AddChild(back);
	Show();
}

SMWindow::~SMWindow() {
	default_view->RemoveSelf();
	delete default_view;
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
		case SM_VIEW: {
			// Remove existing view
			BView* container_view = FindView("filter_prefs_view");
			BView* prefs_view;
			if (container_view->CountChildren() > 0) {
				prefs_view = container_view->ChildAt(0);
				prefs_view->RemoveSelf();
			}
			// Add view to the filter prefs panel
			msg->FindPointer("view", &prefs_view);
			container_view->AddChild(prefs_view);
			break;
		}
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
		case B_SIMPLE_DATA: {
			entry_ref ref;
			msg->FindRef("refs", &ref);
			BNode node(&ref);
			BNodeInfo ni(&node);
			char file_type[B_MIME_TYPE_LENGTH];
			ni.GetType(file_type);
			if (strcmp(file_type, SM_FILE_SIGNATURE) == 0) {
				BMessage open_msg(B_REFS_RECEIVED);
				open_msg.AddRef("refs", &ref);
				be_app_messenger.SendMessage(&open_msg);
			}
			break;
		}
		default:
			BWindow::MessageReceived(msg);
			break;
	}
}
