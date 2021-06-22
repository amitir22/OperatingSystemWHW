#include <unistd.h>
#include <cmath>

const int SBRK_ERROR_CODE = -1;

void *smalloc(size_t size) {
    if (size == 0 || size > pow(10.0, 8.0)) {
        return nullptr;
    } else {
        void *memory = sbrk(size);

        return memory != (void *) SBRK_ERROR_CODE ? memory : nullptr;
    }
}