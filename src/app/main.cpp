#include <gpu/RenderContext.h>

int main() {
    
    PixelMachine::GPU::RenderContext::Allocate();
    auto renderContextP = PixelMachine::GPU::RenderContext::Get();
    renderContextP->BeginFrame("");
    PixelMachine::GPU::RenderContext::Deallocate();

    return 0;
}