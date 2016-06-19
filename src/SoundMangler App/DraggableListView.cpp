#include "DraggableListView.h"
#include "SMMsgDefs.h"

#define SM_DRAG_FILTER '$Mdf'

DraggableListView::DraggableListView(BRect frame, const char *name, SMListType new_list_type, bool new_accepts_drags, bool new_remove_on_drag, list_view_type type, uint32 resizingMode, uint32 flags)
	:BListView(frame, name, type, resizingMode, flags),
	remove_on_drag(new_remove_on_drag),
	accepts_drags(new_accepts_drags),
	list_type(new_list_type) {
}

bool DraggableListView::InitiateDrag(BPoint point, int32 index, bool wasSelected) {
	uint32 buttons = Looper()->CurrentMessage()->FindInt32("buttons");
	if (buttons & B_PRIMARY_MOUSE_BUTTON) {
		snooze(200000);
		BPoint new_point;
		uint32 buttons;
		GetMouse(&new_point, &buttons);
		bool drag = ((new_point != point) && (buttons & B_PRIMARY_MOUSE_BUTTON));
		if (drag && list_type == addon) {
			if (!wasSelected)
				Select(index);
			BRect drag_rect;
			int32 current_item = CurrentSelection(0);
			int32 item_count = CountItems();
			drag_rect = ItemFrame(index);
			BMessage msg(SM_DRAG_FILTER);
			while (current_item < item_count) {
				if (ItemAt(current_item)->IsSelected()) {
					drag_rect = drag_rect | ItemFrame(current_item);
					if (list_type == filter) {
						msg.AddString("filter_name", ((FilterItem*)ItemAt(current_item))->Text());
						msg.AddInt32("filter_id", ((FilterItem*)ItemAt(current_item))->FilterID());
					} else {
						msg.AddString("filter_name", ((BStringItem*)ItemAt(current_item))->Text());
					}
				}
				current_item++;
			}
			DragMessage(&msg, drag_rect);
			current_item = CurrentSelection(0);
			return true;
		} else
				if (list_type == filter) {
					BMessage app_msg(SM_PREFS_VIEW);
					app_msg.AddInt32("filter_id", ((FilterItem*)ItemAt(index))->FilterID());
					be_app_messenger.SendMessage(&app_msg, Window());
				} else {
					BMessage app_msg(SM_ABOUT_VIEW);
					app_msg.AddString("addon_name", ((BStringItem*)ItemAt(index))->Text());
					be_app_messenger.SendMessage(&app_msg, Window());
				}
			return false;
	} else
		return false;
}

void DraggableListView::KeyDown(const char* bytes, int32 numBytes) {
	if (numBytes == 1) {
		switch (bytes[0]) {
			case B_BACKSPACE: {
				if (list_type == filter) {
					BMessage msg(SM_REMOVE_FILTER);
					msg.AddInt32("filter_id", ((FilterItem*)ItemAt(CurrentSelection()))->FilterID());
					be_app_messenger.SendMessage(&msg);
					delete RemoveItem(CurrentSelection());
				}
				break;
			}
			default: {
				BListView::KeyDown(bytes, numBytes);
				break;
			}
		}
	}
}

void DraggableListView::MessageReceived(BMessage *message) {
	if (message->what != B_PULSE) {
		printf("DraggableListView:\n");
		message->PrintToStream();
		printf("\n");
	}
	switch(message->what) {
		case SM_DRAG_FILTER: {
			if (accepts_drags) {
				int32 item_count;
				type_code type_found;
				message->GetInfo("filter_name", &type_found, &item_count);
				int32 current_item = 0;
				char* text = NULL;
				int32 filter_id = 0;
				message->FindString("filter_name", current_item, &text);
				// Do add-filter stuff
				BMessage app_msg(SM_ADD_FILTER);
				app_msg.AddString("addon_name", text);
				app_msg.AddInt32("position", CountItems());
				BMessage reply;
				be_app_messenger.SendMessage(&app_msg, &reply);
				reply.FindInt32("filter_id", &filter_id);
				BStringItem *newItem = new FilterItem(text, filter_id);
				AddItem(newItem);
			}
			break;
		}
		default: {
			BListView::MessageReceived(message);
			break;
		}
	}
}

void DraggableListView::MouseDown(BPoint point) {
	uint32 buttons = Looper()->CurrentMessage()->FindInt32("buttons");
	int32 index = IndexOf(point);
	if (buttons & B_SECONDARY_MOUSE_BUTTON && index >= 0) {
		Select(index);
		if (list_type == filter) {
			BPopUpMenu popup_menu("popup_menu", false, false);
			popup_menu.SetFont(be_plain_font);
			BMessage* del_msg = new BMessage(SM_REMOVE_FILTER);
			del_msg->AddInt32("filter_id", ((FilterItem*)ItemAt(index))->FilterID());
			BMessage* help_msg = new BMessage(SM_HELP_REQUESTED);
			help_msg->AddString("addon_name", ((FilterItem*)ItemAt(index))->Text());
			popup_menu.AddItem(new BMenuItem("Delete", del_msg));
			popup_menu.AddItem(new BMenuItem("Add-on Help"B_UTF8_ELLIPSIS, help_msg));
			BMenuItem* item = popup_menu.Go(ConvertToScreen(point));
			if (item != NULL) {
				if (strcmp(item->Label(),"Delete") == 0) {
					be_app_messenger.SendMessage(item->Message());
					delete RemoveItem(index);
				if (strcmp(item->Label(),"Add-on Help"B_UTF8_ELLIPSIS) == 0)
					Window()->PostMessage(item->Message());
				}
			}
		} else {
			BView* active_list = Window()->FindView("active_list");
			if (active_list != NULL) {
				BPopUpMenu popup_menu("popup_menu", false, false);
				popup_menu.SetFont(be_plain_font);
				BMessage* add_msg = new BMessage(SM_DRAG_FILTER);
				add_msg->AddString("filter_name", ((BStringItem*)ItemAt(index))->Text());
				BMessage* help_msg = new BMessage(SM_HELP_REQUESTED);
				help_msg->AddString("addon_name", ((BStringItem*)ItemAt(index))->Text());
				popup_menu.AddItem(new BMenuItem("Add", add_msg));
				popup_menu.AddItem(new BMenuItem("Add-on Help"B_UTF8_ELLIPSIS, help_msg));
				BMenuItem* item = popup_menu.Go(ConvertToScreen(point));
				if (item != NULL) {
					if (strcmp(item->Label(),"Add") == 0)
						active_list->MessageReceived(item->Message());
					if (strcmp(item->Label(),"Add-on Help"B_UTF8_ELLIPSIS) == 0)
						Window()->PostMessage(item->Message());
				}
			}
		}
	}
	BListView::MouseDown(point);
}
