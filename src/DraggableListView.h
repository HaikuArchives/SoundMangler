#pragma once

#define SM_DRAG_FILTER 'snot'

class DraggableListView : public BListView
	{
	public:
		DraggableListView(BRect frame, const char *name, bool new_accepts_drags, bool new_remove_on_drag, list_view_type type = B_SINGLE_SELECTION_LIST, uint32 resizingMode = B_FOLLOW_LEFT | B_FOLLOW_TOP, uint32 flags = B_WILL_DRAW | B_NAVIGABLE | B_FRAME_EVENTS);
		virtual bool InitiateDrag(BPoint point, int32 index, bool wasSelected);
		void MessageReceived(BMessage *message);
	protected:
		bool remove_on_drag;
		bool accepts_drags;
	};
