#include "DraggableListView.h"

DraggableListView::DraggableListView(BWindow* window, BRect frame, const char *name, bool new_accepts_drags, bool new_remove_on_drag, list_view_type type, uint32 resizingMode, uint32 flags)
	:BListView(frame, name, type, resizingMode, flags),
	remove_on_drag(new_remove_on_drag),
	accepts_drags(new_accepts_drags),
	win(window)
	{
		
	}

bool DraggableListView::InitiateDrag(BPoint point, int32 index, bool wasSelected)
	{
	if (wasSelected)
		{
		BRect drag_rect;
		int32 current_item = CurrentSelection(0);
		int32 item_count = CountItems();
		drag_rect = ItemFrame(index);
		BMessage* msg = new BMessage(SM_DRAG_FILTER);
		while (current_item < item_count)
			{
			if (ItemAt(current_item)->IsSelected())
				{
				drag_rect = drag_rect | ItemFrame(current_item);
				msg->AddString("filter_name", ((BStringItem*)ItemAt(current_item))->Text());
				}
			current_item++;
			}
		DragMessage(msg, drag_rect);
		current_item = CurrentSelection(0);
		if (remove_on_drag)
			while (current_item < item_count)
				{
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
				while (current_item < item_count)
					{
					message->FindString("filter_name", current_item, &text);
					BStringItem *newItem = new BStringItem(text);
					AddItem(newItem);
					current_item++;
					// send a message to the app telling it to Commission the filter
					BMessage *activate = new BMessage(SM_ACTIVATE_FILTER);
					activate->AddString("filter_name", text);
					activate->AddInt32("index", IndexOf(newItem)); 
					//post the message
					Window()->PostMessage(activate);
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
