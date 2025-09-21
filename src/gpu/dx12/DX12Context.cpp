#include <dx12/DX12Context.h>

std::string PixelMachine::GPU::DX12Context::GetInfo() const { return "DX12Context initialized"; }

PixelMachine::GPU::Context * PixelMachine::GPU::Context::Get(void *windowHandle) {
	return new DX12Context();
}