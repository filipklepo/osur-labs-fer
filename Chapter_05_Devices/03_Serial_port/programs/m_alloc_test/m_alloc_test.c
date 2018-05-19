#include <stdio.h>
#include <malloc.h>

void m_alloc_test() 
{
    void *a, *b, *c, *d, *new_a;
    size_t size_a, size_b, size_c, size_d; 
    size_a = 16 * 16 + 10;
    size_b = 16 * 64 + 100;
    size_c = 16 * 128 + 1000;
    size_d = 16 * 256 + 10000;
    printf("===== z: A (%d bytes)\n", size_a);
    a = malloc(size_a);
    printf("##### Adresa A: %d\n", a);
    printf("===== z: B (%d bytes)\n", size_b);
    b = malloc(size_b);
    printf("##### Adresa B: %d\n", b);
    printf("===== z: C (%d bytes)\n", size_c);
    c = malloc(size_c);
    printf("##### Adresa C: %d\n", c);
    printf("===== z: D (%d bytes)\n", size_d);
    d = malloc(size_d);
    printf("##### Adresa D: %d\n", d);
    
    printf("\n\n\n===== o: A (%d bytes)\n", size_a);
    free(a);

    printf("===== z: A (%d bytes)\n", size_a);
    new_a = malloc(size_a);
    printf("---- new A location same as previous? %s\n", (new_a == a) ? "YES": "NO");
    printf("##### Adresa NEW_A %d\n", new_a);

    printf("===== o: C (%d bytes)\n", size_c);
    free(c);
    printf("===== o: B (%d bytes)\n", size_b);
    free(b);
    printf("===== o: D (%d bytes)\n", size_d);
    free(d);
    printf("===== o: A (%d bytes)\n", size_a);
    free(new_a);
}
