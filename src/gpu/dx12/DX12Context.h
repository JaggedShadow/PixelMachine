#include <Context.h>

namespace PixelMachine {
    namespace GPU {
        class DX12Context : public Context {
        public:
            std::string GetInfo() const override;
        };
    }
}
