#include <gpu/RenderContext.h>

namespace PXMGPU = PixelMachine::GPU;

int main() {
    
    PXMGPU::RenderContext::Initialize(nullptr);
    auto renderContextP = PXMGPU::RenderContext::Get();
    renderContextP->BeginPass("");

    PXMGPU::RenderContext::Destroy();

    return 0;
}