#pragma once

class FilterItem : public BStringItem {
	public:
		FilterItem(const char *text, int32 new_filter_id, uint32 level = 0, bool expanded = true)
			:BStringItem(text, level, expanded) {
			filter_id = new_filter_id;
		}
		int32 FilterID() {
			return filter_id;
		}
	protected:
		int32 filter_id;
};

class DraggableListView : public BListView {
	public:
		DraggableListView(BRect frame, const char *name, bool new_accepts_drags, bool new_remove_on_drag, list_view_type type = B_SINGLE_SELECTION_LIST, uint32 resizingMode = B_FOLLOW_LEFT | B_FOLLOW_TOP, uint32 flags = B_WILL_DRAW | B_NAVIGABLE | B_FRAME_EVENTS);
		virtual bool InitiateDrag(BPoint point, int32 index, bool wasSelected);
		void MessageReceived(BMessage *message);
	protected:
		bool remove_on_drag;
		bool accepts_drags;
		int32 filter_limbo;
		bool filter_in_limbo;
};
