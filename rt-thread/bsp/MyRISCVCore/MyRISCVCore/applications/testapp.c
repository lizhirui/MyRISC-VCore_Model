#include <rtthread.h>

static rt_thread_t thread_a;

void thread_a_entry(void *arg)
{
    int i;

    for(i = 0;i < 10;i++)
    {
        rt_kprintf("[%d]Test rt__kprintf\n",i);
        rt_thread_mdelay(1000);
    }
}

static long testapp()
{
    thread_a = rt_thread_create("thread_a",thread_a_entry,RT_NULL,4096,10,10);
    rt_thread_startup(thread_a);
    return 0;
}
MSH_CMD_EXPORT(testapp, testapp);