#include <rtthread.h>
#include<board.h>
#include<drv_gpio.h>
#include<drv_lcd.h>
#ifdef RT_USING_NANO
#include<rtdevice.h>
#endif

static char thread2_stack[1024];
static struct rt_thread thread2;

#define THREAD_PRIORITY         25
#define THREAD_STACK_SIZE       512
#define THREAD_TIMESLICE        5

static void thread2_entry(void *param)
{
    char string1[25];
    char string2[25];
    lcd_clear(WHITE);
    lcd_set_color(WHITE,BLACK);
    int temp =1;
    int humidity =50;
    rt_sprintf(string1, "temperature:%d",temp);

    while(1)
    {
        rt_sprintf(string1, "temperature:%d",temp);
        rt_sprintf(string2, "humidity:%d",humidity);
        lcd_show_string(10,69+16+24,32,string1);
        lcd_show_string(10,69,32,string2);
        rt_thread_mdelay(500);
        temp++;
        humidity++;
    }

    /* 线程2运行结束后也将自动被系统脱离 */
}

int lcd_led(void)
{


    /* 初始化线程2，名称是thread2，入口是thread2_entry */
    rt_thread_init(&thread2,
                   "thread2",
                   thread2_entry,
                   RT_NULL,
                   &thread2_stack[0],
                   sizeof(thread2_stack),
                   THREAD_PRIORITY - 1, THREAD_TIMESLICE);
#ifdef RT_USING_SMP
    /* 绑定线程到同一个核上，避免启用多核时的输出混乱 */
    rt_thread_control(&thread2, RT_THREAD_CTRL_BIND_CPU, (void*)0);
#endif
    rt_thread_startup(&thread2);

    return 0;
}

/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(lcd_led, lcdandled);