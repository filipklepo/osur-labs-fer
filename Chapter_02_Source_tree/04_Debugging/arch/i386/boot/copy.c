#include <types/basic.h>

void copy_to_RAM()
{
    extern char data_LMA, data_VMA, kernel_end_addr;
    char *src = (char *)&data_LMA;
    char *dest = (char *)&data_VMA;
    int i = 0;
    int size = &kernel_end_addr - &data_LMA;
    while(i < size) 
    {
        dest[i] = src[i];
        i++;
    }
}
