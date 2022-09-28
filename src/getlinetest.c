#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
int main(int argc, char *argv[]) {
    /* char line[100];  //pointer to the heap allocated buffer */
    float f;
    ssize_t linesize =0;
    // this allocates line
    scanf("%f",&f);
    printf("Entered: %f\n",f);
    return 0;
}
