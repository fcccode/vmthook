#ifndef VMTHOOK_H_
#define VMTHOOK_H_

#include <memory.h>
#include <stdint.h>

class VMTHook {
	private:
		uintptr_t** baseclass = nullptr;
		uintptr_t* current_vft = nullptr;
		uintptr_t* original_vft = nullptr;
		size_t total_functions = 0;
	public:
		VMTHook(void* baseclass) {
			this->baseclass = reinterpret_cast<uintptr_t**>(baseclass);

			while (reinterpret_cast<uintptr_t*>(*this->baseclass)[this->total_functions])
				this->total_functions++;

			size_t table_size = this->total_functions * sizeof(uintptr_t);

			this->original_vft = *this->baseclass;
			this->current_vft = new uintptr_t[table_size];

			memcpy(this->current_vft, this->original_vft, table_size);

			*this->baseclass = this->current_vft;
		};

		~VMTHook() {
			*this->baseclass = this->original_vft;
			delete this->current_vft;
		};

		template <typename Function> inline Function GetOriginalFunction(size_t function_index) {
			return reinterpret_cast<Function>(this->original_vft[function_index]);
		}

		inline bool HookFunction(void* new_function, size_t function_index) {
			if (function_index > this->total_functions)
				return false;

			this->current_vft[function_index] = reinterpret_cast<uintptr_t>(new_function);

			return true;
		}

		inline bool UnhookFunction(size_t function_index) {
			if (function_index > this->total_functions)
				return false;

			this->current_vft[function_index] = this->original_vft[function_index];

			return true;
		}

		inline size_t GetTotalFunctions() {
			return this->total_functions;
		}
};

#endif