#pragma once
#include <Windows.h>
namespace internal {
	class MemoryReader {
	public:
		template <class T>
		T Read(DWORD Address) {
			return *(T*)Address;
		}
		template <class T>
		void Write(DWORD Address, T Value) {
			*(T*)Address = Value;
		}
	};
}
namespace external {
	class MemoryReader {
	public:
		HANDLE Targetprocess;
		template <class T>
		T Read(DWORD Address) {
			T tBuffer;
			if (Targetprocess == nullptr)
				return tBuffer;
			int bytesRead;
			ReadProcessMemory(HANDLE, Address, tBuffer, sizeof(T), bytesRead);
			return tBuffer;
		}
		template <class T>
		void Write(DWORD Address, T Value) {
			if (Targetprocess == nullptr)
				return;
			int bytesWrote;
			WriteProcessMemory(HANDLE, Address, Value, sizeof(Value), bytesWrote);
		}
	};
}