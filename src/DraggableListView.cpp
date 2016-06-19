#include "DraggableListView.h"
#include "SMMsgDefs.h"

#define SM_DRAG_FILTER '$Mdf'

DraggableListView::DraggableListView(BRect frame, const char *name, bool new_accepts_drags, bool new_remove_on_drag, list_view_type type, uint32 resizingMode, uint32 flags)
	:BListView(frame, name, type, resizingMode, flags),
	remove_on_drag(new_remove_on_drag),
	accepts_drags(new_accepts_drags) {
}

bool DraggableListView::InitiateDrag(BPoint point, int32 index, bool wasSelected)
	{
	snooze(200000);
	BPoint new_point;
	uint32 buttons;
	GetMouse(&new_point, &buttons);
	bool drag = ((new_point != point) && (buttons & B_PRIMARY_MOUSE_BUTTON));
	if (drag)
		{
		if (!wasSelected)
			Select(index);
		BRect drag_rect;
		int32 current_item = CurrentSelection(0);
		int32 item_count = CountItems();
		drag_rect = ItemFrame(index);
		BMessage msg(SM_DRAG_FILTER);
		while (current_item < item_count)
			{
			if (ItemAt(current_item)->IsSelected())
				{
				drag_rect = drag_rect | ItemFrame(current_item);
				if (remove_on_drag) {
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
		if (remove_on_drag)
			while (current_item < item_count)
				{
				filter_in_limbo = true;
				filter_limbo = ((FilterItem*)ItemAt(current_item))->FilterID();
				if (ItemAt(current_item)->IsSelected())
					delete RemoveItem(current_item);
				current_item++;
				}
		return true;
		}
	else
		return false;
	}

void DraggableListView::MessageReceived(BMessage *message)
	{
	if (message->what != B_PULSE) {
		printf("DraggableListView:\n");
		message->PrintToStream();
		printf("\n");
	}
	switch(message->what)
		{
		case SM_DRAG_FILTER:
			{
			if (accepts_drags)
				{
				int32 item_count;
				type_code type_found;
				message->GetInfo("filter_name", &type_found, &item_count);
				int32 current_item = 0;
				char* text = NULL;
				int32 filter_id = 0;
				if (message->HasInt32("filter_id"))
					while (current_item < item_count)
						{
						message->FindString("filter_name", current_item, &text);
						message->FindInt32("filter_id", current_item, &filter_id);
						BStringItem *newItem = new FilterItem(text, filter_id);
						AddItem(newItem);
						// Do move-filter stuff
						BMessage app_msg(SM_MOVE_FILTER);
						app_msg.AddInt32("filter_id", filter_id);
						app_msg.AddInt32("position", IndexOf(newItem));
						be_app_messenger.SendMessage(&app_msg);
						filter_in_limbo = false;
						current_item++;
						}
					else
						{
						message->FindString("filter_name", current_item, &text);
						// Do add-filter stuff
						BMessage app_msg(SM_ADD_FILTER);
						app_msg.AddString("addon_name", text);
						app_msg.AddInt32("position", CountItems());
						BMessage reply;
						be_app_messenger.SendMessage(&app_msg, &reply);
						reply.FindInt32("filter_id", &filter_id);
						if (filter_in_limbo) {
							// Tell app that filter is really really gone
							BMessage app_msg(SM_REMOVE_FILTER);
							app_msg.AddInt32("filter_id", filter_limbo);
							be_app_messenger.SendMessage(&app_msg);
							filter_in_limbo = false;
						}
						BStringItem *newItem = new FilterItem(text, filter_id);
						AddItem(newItem);
						}
				}
			break;
			}
		default:
			{
			BListView::MessageReceived(message);
			break;
			}
		}
	}
