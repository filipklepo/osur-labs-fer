#include <kernel/kprint.h>
#include <kernel/net.h>

char *net_prefix;

void net_init(char *prefix) 
{
    for(int i = 0; prefix[i]; ++i)
    {
        if(i == 20) {
            prefix[i] = '\0';
            break;
        }
    }
    net_prefix = prefix;
}

void net_print(char *msg)
{
    kprintf("%s: %s", net_prefix, msg);
}
