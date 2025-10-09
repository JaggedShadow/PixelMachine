#include <gpu/RenderContext.h>

namespace PXMGPU = PixelMachine::GPU;

int main() {
    
    PXMGPU::RenderContext::Initialize();
    auto renderContextP = PXMGPU::RenderContext::Get();
    renderContextP->BeginFrame("");

    PXMGPU::RenderContext::Destroy();

    return 0;
}