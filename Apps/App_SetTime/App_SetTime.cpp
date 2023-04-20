#if 1
#include "App_SetTime.h"
#include "../../../ChappieBsp/Chappie.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiMulti.h>

#define USE_SMART_CONF 0

static WiFiMulti wifiMulti;
static HTTPClient http;
static I2C_BM8563_TimeTypeDef rtc_time;
static I2C_BM8563_DateTypeDef rtc_date;
static std::string app_name = "SetTime";
static CHAPPIE* device;

LV_IMG_DECLARE(ui_img_icon_settime_png);

static int extract_ints(char* str, int* nums, int max_count) {
    int len = strlen(str);
    int count = 0;
    int num = 0;
    for (int i = 0; i < len && count < max_count; i++) {
        if (i == 4 || i == 6 || i == 8 || i == 10 || i == 12) {
            nums[count++] = num;
            num = 0;
        }
        num = num * 10 + (str[i] - '0');
    }
    if (count < max_count) {
        nums[count++] = num;
    }
    return count;
}

namespace App {

    /**
     * @brief Return the App name laucnher, which will be show on launcher App list
     * 
     * @return std::string 
     */
    std::string App_SetTime_appName()
    {
        return app_name;
    }


    /**
     * @brief Return the App Icon laucnher, NULL for default
     * 
     * @return void* 
     */
    void* App_SetTime_appIcon()
    {
        return (void*)&ui_img_icon_settime_png;

    }


    /**
     * @brief Called when App is on create
     * 
     */
    void App_SetTime_onCreate()
    {
        uint8_t i;
        device->Lcd.fillScreen(TFT_BLACK);
        device->Lcd.setCursor(0, 30);
        device->Lcd.setTextSize(1.5);
        device->Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
        WiFi.mode(WIFI_AP_STA);

        #if USE_SMART_CONF
            WiFi.beginSmartConfig();
            device->Lcd.printf("Waiting for SmartConfig...\n");
            while (!WiFi.smartConfigDone()) { delay(500); }
            device->Lcd.printf("SmartConfig received, connecting WiFi...\n");
            while (WiFi.status() != WL_CONNECTED) { delay(500); }
            device->Lcd.printf("Connected. IP: %s\n", WiFi.localIP().toString().c_str());
        #else
            /*Add your wifiAP as wifiMulti.addAP("Your SSID", "Your Passwd");*/
            wifiMulti.addAP("T", "66668888");
            wifiMulti.addAP("E", "66668888");
            WiFi.begin();
            device->Lcd.printf("Connecting WiFi");
            while (wifiMulti.run() != WL_CONNECTED) { 
                delay(500);
                i++;
                device->Lcd.printf(".");
                if(i == 30){
                    i = 0;
                    device->Lcd.printf("\nConnect Faild...\n");
                    WiFi.disconnect();
                    break;
                }
            }
        #endif
        device->Lcd.printf("\nConnected. IP: %s\n", WiFi.localIP().toString().c_str());
        http.begin("http://quan.suning.com/getSysTime.do"); //HTTP begin
    	int httpCode = http.GET();
        if (httpCode > 0)
        {
            // httpCode will be negative on error
            if (httpCode == HTTP_CODE_OK) // 收到正确的内容
            {
            String resBuff = http.getString();
            char charArray[100];
            char str[30];
            resBuff.toCharArray(charArray, 61);
            for(i=0;i<=15;i++){str[i]=charArray[i+46];}
            device->Lcd.printf("Get time: %s\n", String(str).c_str());
            int nums[6];
            extract_ints(str, nums, 6);
            rtc_date.year = nums[0];
            rtc_date.month = nums[1];
            rtc_date.date = nums[2];
            rtc_time.hours = nums[3];
            rtc_time.minutes = nums[4];
            rtc_time.seconds = nums[5]+2;//补偿数据处理的时间            
            uint16_t y = nums[0];
            uint8_t m = nums[1];
            uint8_t d = nums[2];
            if(m<3)
            {
                m+=12;
                y-=1;
            }
            rtc_date.weekDay=(d+2*m+3*(m+1)/5+y+y/4-y/100+y/400+1)%7;
            device->Rtc.setDate(&rtc_date);
            device->Rtc.setTime(&rtc_time);
            }
        }
        http.end();
        WiFi.disconnect();
        device->Lcd.printf("Battery Voltage:%d % \n",(device->Pow.readBatPercentage() - 86)*10);
        device->Lcd.printf("Exit in ");
        i = 5;
        while(i){
            device->Lcd.printf("%d.",i);
            vTaskDelay(1000);
            i--;
        }
        lv_obj_t * label = lv_label_create(lv_scr_act());
        lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
        lv_label_set_text(label, "Press B to quit");

    }


    /**
     * @brief Called repeatedly, end this function ASAP! or the App management will be affected
     * If the thing you want to do takes time, try create a taak or lvgl timer to handle them.
     * Try use millis() instead of delay() here
     * 
     */
    void App_SetTime_onLoop()
    {

        
    }


    /**
     * @brief Called when App is about to be destroy
     * Please remember to release the resourse like lvgl timers in this function
     * 
     */
    void App_SetTime_onDestroy()
    {
        UI_LOG("[%s] onDestroy\n", App_SetTime_appName().c_str());
    }


    /**
     * @brief Launcher will pass the BSP pointer through this function before onCreate
     * 
     */
    void App_SetTime_getBsp(void* bsp)
    {
        device = (CHAPPIE*)bsp;
    }
    
}

#endif
