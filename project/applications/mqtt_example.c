
#include<math.h>
#include <board.h>
#include "rtthread.h"
#include "dev_sign_api.h"
#include "mqtt_api.h"
#include "aht10.h"
#include <drv_gpio.h>
#include <dfs_posix.h>
#include<rtdevice.h>
#include<drv_lcd.h>
#include <wlan_mgnt.h>
#include <wlan_prot.h>
#include <wlan_cfg.h>
#include"D:\RT-Thread\project\project\board\ports\led_matrix\drv_matrix_led.h"

char DEMO_PRODUCT_KEY[IOTX_PRODUCT_KEY_LEN + 1] = {0};
char DEMO_DEVICE_NAME[IOTX_DEVICE_NAME_LEN + 1] = {0};
char DEMO_DEVICE_SECRET[IOTX_DEVICE_SECRET_LEN + 1] = {0};

#define PIN_BEEP        16 

int count =1;
char String[100];
char string1[25];
char string2[25];
char string3[10];

aht10_device_t dev ;
// static struct rt_messagequeue mq;
// /* 消息队列中用到的放置消息的内存池 */
// static rt_uint8_t msg_pool[2048];

ALIGN(RT_ALIGN_SIZE)
static char thread1_stack[4096];
static struct rt_thread thread1;

ALIGN(RT_ALIGN_SIZE)
static char thread2_stack[2048];
static struct rt_thread thread2;

void *HAL_Malloc(uint32_t size);
void HAL_Free(void *ptr);
void HAL_Printf(const char *fmt, ...);
int HAL_GetProductKey(char product_key[IOTX_PRODUCT_KEY_LEN + 1]);
int HAL_GetDeviceName(char device_name[IOTX_DEVICE_NAME_LEN + 1]);
int HAL_GetDeviceSecret(char device_secret[IOTX_DEVICE_SECRET_LEN]);
uint64_t HAL_UptimeMs(void);
int HAL_Snprintf(char *str, const int len, const char *fmt, ...);

#define EXAMPLE_TRACE(fmt, ...)  \
    do { \
        HAL_Printf("%s|%03d :: ", __func__, __LINE__); \
        HAL_Printf(fmt, ##__VA_ARGS__); \
        HAL_Printf("%s", "\r\n"); \
    } while(0)


struct AHT20
{
    float humidity;
    float temperature;
}aht20;

aht10_device_t aht20_init(const char *i2c_bus_name)
{
    aht10_device_t dev ;


    int count=0;

    rt_thread_mdelay(1000);

    dev = aht10_init(i2c_bus_name);
    if (dev == RT_NULL)
    {
        rt_kprintf("aht10 init failed\n");
        return;
    }

    return dev;
}

static void example_message_arrive(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    iotx_mqtt_topic_info_t     *topic_info = (iotx_mqtt_topic_info_pt) msg->msg;

    switch (msg->event_type) {
        case IOTX_MQTT_EVENT_PUBLISH_RECEIVED:
            /* print topic name and topic message */
            EXAMPLE_TRACE("Message Arrived:");
            EXAMPLE_TRACE("Topic  : %.*s", topic_info->topic_len, topic_info->ptopic);
            EXAMPLE_TRACE("Payload: %.*s", topic_info->payload_len, topic_info->payload);
            EXAMPLE_TRACE("\n");
            break;
        default:
            break;
    }
}

static int example_subscribe(void *handle)
{
    int res = 0;
    const char *fmt = "/%s/%s/user/get";
    char *topic = NULL;
    int topic_len = 0;

    topic_len = strlen(fmt) + strlen(DEMO_PRODUCT_KEY) + strlen(DEMO_DEVICE_NAME) + 1;
    topic = HAL_Malloc(topic_len);
    if (topic == NULL) {
        EXAMPLE_TRACE("memory not enough");
        return -1;
    }
    memset(topic, 0, topic_len);
    HAL_Snprintf(topic, topic_len, fmt, DEMO_PRODUCT_KEY, DEMO_DEVICE_NAME);

    res = IOT_MQTT_Subscribe(handle, topic, IOTX_MQTT_QOS0, example_message_arrive, NULL);
    if (res < 0) {
        EXAMPLE_TRACE("subscribe failed");
        HAL_Free(topic);
        return -1;
    }

    HAL_Free(topic);
    return 0;
}

static int example_publish_temperature(void *handle)
{

    char payload[50];
    rt_sprintf(payload,"{\"params\":{\"temperature\":%.1f}}",aht20.temperature);



    int             res = 0;
    const char     *fmt = "/sys/%s/%s/thing/event/property/post";
    char           *topic = NULL;
    int             topic_len = 0;


    topic_len = strlen(fmt) + strlen(DEMO_PRODUCT_KEY) + strlen(DEMO_DEVICE_NAME) + 1;
    topic = HAL_Malloc(topic_len);
    if (topic == NULL) {
        EXAMPLE_TRACE("memory not enough");
        return -1;
    }
    memset(topic, 0, topic_len);
    HAL_Snprintf(topic, topic_len, fmt, DEMO_PRODUCT_KEY, DEMO_DEVICE_NAME);

    res = IOT_MQTT_Publish_Simple(0, topic, IOTX_MQTT_QOS0, payload, strlen(payload));
    if (res < 0) {
        EXAMPLE_TRACE("publish failed, res = %d", res);
        HAL_Free(topic);
        return -1;
    }

    HAL_Free(topic);
    return 0;
}

static void example_event_handle(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    EXAMPLE_TRACE("msg->event_type : %d", msg->event_type);
}
static int example_publish_Humidity(void *handle)
{


    char payload[50];
    rt_sprintf(payload,"{\"params\":{\"Humidity\":%.1f}}",aht20.humidity);



    int             res = 0;
    const char     *fmt = "/sys/%s/%s/thing/event/property/post";
    char           *topic = NULL;
    int             topic_len = 0;


    topic_len = strlen(fmt) + strlen(DEMO_PRODUCT_KEY) + strlen(DEMO_DEVICE_NAME) + 1;
    topic = HAL_Malloc(topic_len);
    if (topic == NULL) {
        EXAMPLE_TRACE("memory not enough");
        return -1;
    }
    memset(topic, 0, topic_len);
    HAL_Snprintf(topic, topic_len, fmt, DEMO_PRODUCT_KEY, DEMO_DEVICE_NAME);

    res = IOT_MQTT_Publish_Simple(0, topic, IOTX_MQTT_QOS0, payload, strlen(payload));
    if (res < 0) {
        EXAMPLE_TRACE("publish failed, res = %d", res);
        HAL_Free(topic);
        return -1;
    }

    HAL_Free(topic);
    return 0;
}

void wifi_link_nok  (void)
{
    const char *ssid = "--------t";
    const char *key = "-----------";
    rt_thread_mdelay(1000);
    rt_wlan_scan();
    rt_wlan_connect(ssid,key);
    rt_thread_mdelay(1000);
}

void open_file_save(void)
{
    rt_sprintf(String, "Temp:%.2f Humi:%.2f,count=%d",aht20.temperature,aht20.humidity,count);
    int fd = open("/fal/Data.txt", O_WRONLY | O_CREAT);

            //如果打开成功
    if (fd >= 0)
    {
            //写入文件
        write(fd, String, sizeof(String));

        rt_kprintf("Write done.\n");

            //关闭文件
        close(fd);
    }
    else
    {
        rt_kprintf("File Open Fail.\n");
    }
}

void lcd_display_aht21(void)
{
    rt_sprintf(string1, "temperature:%.2f",aht20.temperature);
    rt_sprintf(string2, "humidity:%.2f",aht20.humidity);
    rt_sprintf(string3, "count:%d",count);

    lcd_show_string(10,69+16+24,32,string1);
    lcd_show_string(10,69,32,string2);
    lcd_show_string(10,69-16-24,32,string3);
}

static int work_main(void)
{
    wifi_link_nok();

    void                   *pclient = NULL;
    int                     res = 0;
    int                     loop_cnt = 0;
    iotx_mqtt_param_t       mqtt_params;


    lcd_clear(WHITE);
    lcd_set_color(WHITE,BLACK);

    HAL_GetProductKey(DEMO_PRODUCT_KEY);
    HAL_GetDeviceName(DEMO_DEVICE_NAME);
    HAL_GetDeviceSecret(DEMO_DEVICE_SECRET);

    EXAMPLE_TRACE("mqtt example");

    memset(&mqtt_params, 0x0, sizeof(mqtt_params));

    mqtt_params.handle_event.h_fp = example_event_handle;

    pclient = IOT_MQTT_Construct(&mqtt_params);
    if (NULL == pclient) {
        EXAMPLE_TRACE("MQTT construct failed");
        return -1;
    }

    res = example_subscribe(pclient);
    if (res < 0) {
        IOT_MQTT_Destroy(&pclient);
        return -1;
    }
    while (1) {
        aht20.temperature = aht10_read_temperature(dev);
        aht20.humidity = aht10_read_humidity(dev);


        if (0 == loop_cnt % 20) {
            example_publish_temperature(pclient);
            example_publish_Humidity(pclient);
        }
        IOT_MQTT_Yield(pclient, 200);

        // 显示数据
        lcd_display_aht21();
        
        // 存取数据
        open_file_save();

        loop_cnt += 1;
        count++;
        rt_thread_mdelay(1000);
    }

    return 0;
}


static int led_matrix(void)
{
    int temp =0 ;

    do
    {
        aht20.temperature = aht10_read_humidity(dev);
        temp = (int)aht20.temperature;

        led_matrix_Humidity(temp);

        RGB_Reflash();

        if(temp > 35)
        {
            rt_pin_write(PIN_BEEP,PIN_HIGH);
        }
        else{
            rt_pin_write(PIN_BEEP,PIN_LOW);
        }

        rt_thread_mdelay(1000);
    } while (1);
    return 0;
}





int Smart_Agriculture(void)
{
    dev = aht20_init("i2c3");
    rt_pin_mode(PIN_BEEP, PIN_MODE_OUTPUT);


    rt_thread_init(&thread1,
                   "thread1",
                   work_main,
                   RT_NULL,
                   &thread1_stack[0],
                   sizeof(thread1_stack), 11, 5);
    rt_thread_startup(&thread1);

    rt_thread_init(&thread2,
                   "thread2",
                   led_matrix,
                   RT_NULL,
                   &thread2_stack[0],
                   sizeof(thread2_stack), 11, 5);
    rt_thread_startup(&thread2);

    return 0;
}
MSH_CMD_EXPORT(Smart_Agriculture,SmartA griculture);