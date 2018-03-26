
void copy_to_RAM()
{
    extern int kernel_code_LMA, kernel_end_LMA, MOVE_ADDR;
    int *src = (int *)kernel_code_LMA;
    int *dest = (int *)kernel_end_LMA;
    int i = kernel_code_LMA;
    int j = MOVE_ADDR;
    while(i < kernel_end_LMA) 
    {
        MOVE_ADDR
    }
}
