#include <Context.h>

namespace PixelMachine {
    namespace GPU {
        class VulkanContext : public Context {
        public:
            std::string GetInfo() const override;
        };
    }
}
