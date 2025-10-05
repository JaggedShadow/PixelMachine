#include <gpu/RenderContext.h>

int main() {
    
    PixelMachine::GPU::RenderContext::Initialize();
    auto renderContextP = PixelMachine::GPU::RenderContext::Get();
    renderContextP->BeginFrame("");
    PixelMachine::GPU::RenderContext::Destroy();

    return 0;
}