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

RUNTIME_FUNCTION(Runtime_DumpObjects) {
	SealHandleScope shs(isolate);
	MaybeObject maybe_object(*args.address_of_arg_at(0));

	StdoutStream os;
	Object obj = maybe_object.GetHeapObjectOrSmi();
	Address  pobj = HeapObject::cast(obj)->address();

	if (obj->IsSmi()) {
		return args[0];
	}
	CONVERT_NUMBER_OPT(uint32_t, pvoid_display_count, Uint32, args[1],10)

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
				if (!obj->IsSmi()) {
					os << "----- [ ";
					os << HeapObject::cast(tmp_obj)->map()->instance_type();
					os << " : 0x" << std::hex << HeapObject::cast(tmp_obj)->Size();
					os << " ] -----";
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
