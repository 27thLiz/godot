#include "register_types.h"
#include "object_type_db.h"
#include "openvr.h"

void register_openvr_types() {
	
	ObjectTypeDB::register_type<Openvr>();
}

void unregister_openvr_types() {
	//nothing to do here
}

