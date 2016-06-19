// Class SMAddOnDirectory
// Be Users Group @ UIUC - SoundMangler Project
// 2/17/1998

#include "SMAddOnDirectory.h"
#include "SMMsgDefs.h"

class AddOnRecord {
	public:
		entry_ref ref;
		node_ref nref;
};

SMAddOnDirectory::SMAddOnDirectory()
	:BLooper("add-on manager") {
	ref_list = new BList;
	char addons_path[128];
	int32 addons_path_length = 128;
	status_t err = find_directory(B_USER_ADDONS_DIRECTORY, NULL, false, addons_path, addons_path_length);
	char sm_addons_path[128];
	sprintf(sm_addons_path, "%s/SoundMangler", addons_path);
	create_directory(sm_addons_path, 777);
	BNode sm_addons_node(sm_addons_path);
	sm_addons_node.GetNodeRef(&addons_ref);
	
	BDirectory directory(&addons_ref);
	int32 current_entry = 0;
	while (current_entry < directory.CountEntries()) {
		AddOnRecord* record = new AddOnRecord;
		directory.GetNextRef(&record->ref);
		BNode node(&record->ref);
		node.GetNodeRef(&record->nref);
		ref_list->AddItem(record);
		current_entry++;
	}
	
	watch_node(&addons_ref, B_WATCH_DIRECTORY, BMessenger(this));
	
	Run();
}

SMAddOnDirectory::~SMAddOnDirectory() {
	stop_watching(BMessenger(this));
	int32 index = 0;
	while (index < ref_list->CountItems()) {
		delete ref_list->ItemAt(index);
		index++;
	}
	delete ref_list;
}

void SMAddOnDirectory::MessageReceived(BMessage *msg) {
	if (msg->what != B_PULSE) {
		printf("SMAddOnDirectory:\n");
		msg->PrintToStream();
		printf("\n");
	}
	switch(msg->what) {
		case SM_GET_ADDON_LIST: {
			// Get list of add-ons and send to the be_app
			BMessage reply(SM_ADDON_LIST);
			BDirectory directory(&addons_ref);
			entry_ref ref;
			int32 current_entry = 0;
			while (current_entry < directory.CountEntries()) {
				directory.GetNextRef(&ref);
				reply.AddRef("addon_ref", &ref);
				current_entry++;
			}
			msg->SendReply(&reply);
			break;
		}
		case B_ENTRY_CREATED: {
			// Add new add-on to the available list
			AddOnRecord* record = new AddOnRecord;
			char* name;
			msg->FindInt32("device", &record->ref.device);
			msg->FindInt64("directory", &record->ref.directory);
			msg->FindString("name", &name);
			record->ref.set_name(name);
			msg->FindInt32("device", &record->nref.device);
			msg->FindInt64("node", &record->nref.node);
			ref_list->AddItem(record);
			BMessage notify(SM_ADD_ADDON);
			notify.AddRef("addon_ref", &record->ref);
			be_app_messenger.SendMessage(&notify);
			break;
		}
		case B_ENTRY_REMOVED: {
			// Remove add-on from the available list
			node_ref nref;
			msg->FindInt32("device", &nref.device);
			msg->FindInt64("node", &nref.node);
			BMessage notify(SM_REMOVE_ADDON);
			int32 index = 0;
			while (index < ref_list->CountItems()) {
				if (((AddOnRecord*)ref_list->ItemAt(index))->nref == nref) {
					notify.AddRef("addon_ref", &((AddOnRecord*)ref_list->ItemAt(index))->ref);
					delete ref_list->ItemAt(index);
				}
				index++;
			}
			be_app_messenger.SendMessage(&notify);
			break;
		}
		case 'NDMN': {
			// Remove add-on from the available list
			ino_t dnode = 0;
			msg->FindInt64("from directory", &dnode);
			if (dnode == addons_ref.node) {
				msg->what = B_ENTRY_REMOVED;
				PostMessage(msg);
			} else {
				msg->AddInt64("directory", dnode);
				msg->what = B_ENTRY_CREATED;
				PostMessage(msg);
			}
			break;
		}
		default:
			BLooper::MessageReceived(msg);
			break;
	}
}
