#if 1
#pragma once
#include "../AppTypedef.h"
#include "../../ChappieUIConfigs.h"

/**
 * @brief Create an App in name space 
 * 
 */
namespace App {

    std::string App_SetTime_appName();
    void* App_SetTime_appIcon();
    void App_SetTime_onCreate();
    void App_SetTime_onLoop();
    void App_SetTime_onDestroy();
    void App_SetTime_getBsp(void* bsp);

    /**
     * @brief Declace your App like this 
     * 
     */
    App_Declare(SetTime);
}

#endif
