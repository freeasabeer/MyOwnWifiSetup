#ifndef _WIFISETUP_H
#define _WIFISETUP_H

#include <WebServer.h>
#include <String.h>

class MOWM /*: public WebServer*/
{
  public:
    MOWM();
    void begin(bool doReboot, unsigned long msec_try = 10000, bool force_activation = false);
    void captivePortalWatchdog_cb(TimerHandle_t xTimer);


  private:
    typedef enum {CONFIG_PAGE, CONNECTING_PAGE, SUCCESS_PAGE, FAILURE_PAGE} Page_t;
    typedef enum {RESULT_SUCCESS, RESULT_FAILED, RESULT_NONE} Result_t;

    void buildPage(Page_t page);
    bool captivePortal();
    String decodeWiFiStatus(wl_status_t wl_status);
    String get_WIFI_MODE(void);
    bool isIp(String str);
    void waitForEndTransmission(void);
    void startWiFiManager(bool doReboot);
    void handleFail();
    void handleNotFound();
    void handleResult();
    void handleRoot();
    void handleSubmit();
    void handleSuccess();

    WebServer *server;
    bool Done;
    String *pagetoserve;
    Result_t Result;
    String selectedSSID;
    const  String  _emptyString = String("");
    String mqtt_ip;
    void (*cb)(const char* param);
};
#endif