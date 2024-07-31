// #include<rtthread.h>
// #include<aht10.h>

// struct AHT20
// {
//     float humidity;
//     float temperature;
// }aht20;

// aht10_device_t aht20_init(const char *i2c_bus_name)
// {
//     aht10_device_t dev ;


//     int count=0;

//     rt_thread_mdelay(1000);

//     dev = aht10_init(i2c_bus_name);
//     if (dev == RT_NULL)
//     {
//         rt_kprintf("aht10 init failed\n");
//         return;
//     }

//     return dev;
// }
// void AHT20_THREAD(void)
// {

//     aht10_device_t dev = aht20_init("i2c3");
//     while(1)
//     {
//         aht20.humidity = aht10_read_humidity(dev);
//         aht20.temperature = aht10_read_temperature(dev);

//         // rt_vsnprintf(RT_NULL, 128, "humidity: %.2f%%\n temperature: %.2fC\n", humidity, temperature);

//         rt_kprintf("humidity: %.2f%%\n temperature: %.2fC\n", aht20.humidity, aht20.temperature);
//         char buf[50];
//         rt_sprintf(buf,"{\"params\":{\"temperature\":%.0f}}",aht20.temperature);
//         rt_kprintf(buf);
//         rt_thread_mdelay(1000);
//     }

// } 

// static int ath_app(void)
// {
//     rt_thread_t thread;
//     thread = rt_thread_create("aht10", AHT20_THREAD, RT_NULL, 1024, 25, 10); 
//     if (thread == RT_NULL)
//     {
//         return -RT_ERROR;
//     }
//     rt_thread_startup(thread);
//     return RT_EOK;
// }

// MSH_CMD_EXPORT(ath_app, aht10 test);

