#include <types/basic.h>

void copy_to_RAM()
{
    extern char kernel_code_LMA, kernel_code_VMA, kernel_end_LMA;
    char *src = (char *)&kernel_code_LMA;
    char *dest = (char *)&kernel_code_VMA;
    int i = 0;
    int size = &kernel_end_LMA - &kernel_code_LMA;
    while(i < size) 
    {
        dest[i] = src[i];
        i++;
    }
}
