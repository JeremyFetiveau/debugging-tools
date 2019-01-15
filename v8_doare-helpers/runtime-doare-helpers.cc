//
// Jeremy '@__x86' Fetiveau
//

#include "src/runtime/runtime-utils.h"

#include <memory>
#include <sstream>
#include <iomanip>

#include "src/api-inl.h"
#include "src/arguments-inl.h"
#include "src/assembler-inl.h"
#include "src/counters.h"

#include "src/wasm/wasm-objects.h"
#include "src/wasm/wasm-objects-inl.h"

namespace v8 {
namespace internal {

#define MIN_PTR_ADDR 0x0000010000000000
#define MAX_PTR_ADDR 0x00007fffffffffff
#define PTR_HI_MASK 0xFFFFFFFF00000000

#define CONVERT_NUMBER_OPT(type, name, Type, obj, default_value)    \
type name = default_value;                                          \
if(obj->IsNumber()) name = NumberTo##Type(obj);                     \

bool isTaggedPointer(unsigned long ptr) {
	return (ptr & 1) && (ptr > MIN_PTR_ADDR) && (ptr < MAX_PTR_ADDR) && IsAligned(ptr-1, kTaggedSize);
}

enum DumpFlag {
	OBJECT_MODE = 0,
	ADDRESS_MODE = 1
};

void usage() {
	StdoutStream os;
	os << "\%DumpObjects(js_object, lines|0);" << std::endl;
	os << "\%DumpObjects(address, lines|1);" << std::endl;
	os << "\%DumpObjects(taggedAddress, lines|1);" << std::endl;
}

RUNTIME_FUNCTION(Runtime_DumpObjects) {
	SealHandleScope shs(isolate);

	StdoutStream os;

	CONVERT_NUMBER_OPT(uint32_t, pvoid_display_count, Uint32, args[1],10)

	DumpFlag mode = OBJECT_MODE;

	if (pvoid_display_count & 1) {
		mode = ADDRESS_MODE;
	}

	Address pobj;

	if (mode == OBJECT_MODE) {
		MaybeObject maybe_object(*args.address_of_arg_at(0));
		Object obj = maybe_object.GetHeapObjectOrSmi();
		pobj = HeapObject::cast(obj)->address();
		if (obj->IsSmi()) {
			os << "[!] error using OBJECT_MODE with smi parameter" << std::endl;
			usage();
			return args[0];
		}
	}
	else if (mode == ADDRESS_MODE) {
		CONVERT_NUMBER_OPT(int64_t, obj_addr, Int64, args[0], 0);
		obj_addr &= ~1; // untag if tagged
		if (obj_addr == 0) {
			os << "[!] error using ADDRESS_MODE with object parameter" << std::endl;
			usage();
			return args[0];
		}
		pobj = static_cast<Address>(obj_addr);
	}

	bool dumping_wasm_instance = false;
	bool dumping_string = false;

	for (unsigned int i = 0; i < pvoid_display_count; ++i) {
		uintptr_t ptr = pobj + i * sizeof(uintptr_t);
		unsigned long val = *reinterpret_cast<unsigned long*>(ptr);
		// this is totally unsafe and might crash sometimes
		// if val is incorrectly considered as a pointer
		if (isTaggedPointer(val)) {
			HeapObject* heapobj = reinterpret_cast<HeapObject*>(ptr)	;
			InstanceType heapobj_instance_type = heapobj->map()->instance_type();
			if (heapobj_instance_type == MAP_TYPE) {
				MaybeObject maybe_tmp_object(reinterpret_cast<Address>(ptr+1));
				Object tmp_obj = maybe_tmp_object.GetHeapObjectOrSmi();
				if (!tmp_obj->IsSmi()) {
					os << "----- [ ";
					os << HeapObject::cast(tmp_obj)->map()->instance_type();
					os << " : 0x" << std::hex << HeapObject::cast(tmp_obj)->Size();
					dumping_wasm_instance = false;
					dumping_string = false;
					switch (HeapObject::cast(tmp_obj)->map()->instance_type()) {
						case WASM_INSTANCE_TYPE:
							dumping_wasm_instance = true;
							break;
						case ONE_BYTE_STRING_TYPE:
						case ONE_BYTE_INTERNALIZED_STRING_TYPE:
							dumping_string = true;
							break;
						default:
							break;
					}
					os << ((dumping_wasm_instance) ? " : REFERENCES RWX MEMORY]" : " ]");
					os << " -----";
					os << std::endl;
				}
			}
			os << std::hex << "0x" << std::setfill('0') << std::setw(sizeof(uintptr_t)*2) << ptr;
			os << "    ";
			os << std::hex << "0x" << std::setfill('0') << std::setw(sizeof(uintptr_t)*2) << val;
			os << "    ";
			os << heapobj_instance_type;
			os << "    ";
			os << std::endl;
		}
		else {
			os << std::hex << "0x" << std::setfill('0') << std::setw(sizeof(uintptr_t)*2) << ptr;
			os << "    ";
			os << std::hex << "0x" << std::setfill('0') << std::setw(sizeof(uintptr_t)*2) << val;
			os << "    ";
			if (dumping_wasm_instance) {
				if (i == WasmInstanceObject::kJumpTableStartOffset / sizeof(uintptr_t))
					os << "JumpTableStart [RWX]";
			}
			else if (dumping_string) {
				std::string str = std::string((const char*)&val).substr(0, sizeof(uintptr_t));
				str.erase(remove_if(str.begin(),str.end(), [](char c) { return c < '!'; }), str.end());  
				os << str;
			}
			os << std::endl;
		}
	}

	
	return args[0];
}

RUNTIME_FUNCTION(Runtime_DescribeObjectLayout) {
	return args[0];
}

}  // namespace internal
}  // namespace v8
