#include <string>

namespace PixelMachine {
    namespace GPU {
        class Context {
            public:
                Context() {};
                ~Context() {};
                static Context *Get(void *windowHandle);
                virtual std::string GetInfo() const = 0;
            private:
        };
    }
}

