#include <stdio.h>
#include <unistd.h>

#define usedMemory syscall(354)
#define freeMemorySpace syscall(353)

int main() {
    int x;
    float fragment;
    for (x = 0; x < 3; x++) {
        fragment = (float)freeMemorySpace / (float)usedMemory;
        printf("Fragment-> %f\n", fragment);
        printf("Used Memory-> %lu\n", usedMemory);
        printf("Free Memory Space-> %lu\n", freeMemorySpace);
        sleep(1);
    }
}
