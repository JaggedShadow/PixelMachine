#include <iostream>
#include <gpu/Context.h>

int main() {

    PixelMachine::GPU::Context* pContext = PixelMachine::GPU::Context::Get(nullptr);
    std::cout << pContext->GetInfo();

    std::cin.get();
    return 0;
}