#include <WebServer.h>
#include <String.h>
#include <WiFi.h>
#include <DNSServer.h>

#define ENHANCED_PAGE

#include "webpage.h"
typedef enum {CONFIG_PAGE, CONNECTING_PAGE, SUCCESS_PAGE, FAILURE_PAGE} Page_t;
typedef enum {RESULT_SUCCESS, RESULT_FAILED, RESULT_NONE} Result_t;
void buildPage(Page_t page);

static WebServer *server;
static bool Done;
static String *pagetoserve;
static Result_t Result;
static String selectedSSID;
static const  String  _emptyString = String("");
/*
 * Function to handle unknown URLs
 */
bool isIp(String str) {
  for (int i = 0; i < str.length(); i++) {
    int c = str.charAt(i);
    if (c != '.' && (c < '0' || c > '9')) {
      return false;
    }
  }
  return true;
}
void waitForEndTransmission(void) {
  while (server->client().connected()) {
    delay(1);
    yield();
  }
}

bool wifisetup_captivePortal() {
  String  hostHeader = server->hostHeader();
  if (!isIp(hostHeader) && (hostHeader != WiFi.localIP().toString()) && (!hostHeader.endsWith(F(".local")))) {
    Serial.print(F("Request redirected to captive portal"));
    server->sendHeader(String(F("Location")), String(F("http://")) + server->client().localIP().toString() + String(F("/")), true);
    server->send ( 302, String(F("text/plain")), _emptyString); // Empty content inhibits Content-length header so we have to close the socket ourselves.
    server->client().stop(); // Stop is needed because we sent no content length
    return true;
  }
  return false;
}

void wifisetup_handleNotFound() {
  Serial.printf("handleNotFound: %s\n", server->uri().c_str());
  if (wifisetup_captivePortal()) { // If captive portal redirect instead of displaying the error page.
    return;
  }
  String message = String(F("File Not Found\n\n"));
  message += String(F("URI: "));
  message += server->uri();
  message += String(F("\nMethod: "));
  message += (server->method() == HTTP_GET) ? String(F("GET")) : String(F("POST"));
  message += String(F("\nArguments: "));
  message += server->args();
  message += String(F("\n"));
  for (uint8_t i = 0; i < server->args(); i++) {
    message += String(F(" ")) + server->argName(i) + String(F(": ")) + server->arg(i) + String(F("\n"));
  }
  server->send(404, String(F("text/plain")), message);
  Serial.print(message);
}
void wifisetup_handleResult() {
  String redirect = String(F("http://"));
  redirect += server->client().localIP().toString();
  switch(Result) {
    case RESULT_NONE:
      redirect += String(F(AUTOCONNECT_URI_RESULT));
      break;
    case RESULT_FAILED:
      redirect += String(F(AUTOCONNECT_URI_FAIL));
    break;
    case RESULT_SUCCESS:
      redirect += String(F(AUTOCONNECT_URI_SUCCESS));
    break;
  }
  // Redirect to result page
  server->sendHeader(String(F("Location")), redirect, true);
  server->send(302, String(F("text/plain")), _emptyString);
  server->client().stop();
  waitForEndTransmission();  // Wait for response transmission complete
}

void wifisetup_handleSuccess() {
  buildPage(SUCCESS_PAGE);
  server->send(200, String(F("text/html")), *pagetoserve);
  waitForEndTransmission();  // Wait for response transmission complete
  Done = true;
}

void wifisetup_handleFail() {
  buildPage(FAILURE_PAGE);
  server->send(200, String(F("text/html")), *pagetoserve);
}

/*
 * Function for handling form
 */
void wifisetup_handleSubmit() {
  int i = 20;
  Serial.println(String(F("Trying to connect to "))+server->arg(F("SSID")));
  WiFi.begin(server->arg(F("SSID")).c_str(), (const char *)server->arg(F("Passphrase")).c_str());
  while ((WiFi.status() != WL_CONNECTED) && (i > 0)){
    delay(500);
    i--;
  }

#ifdef ENHANCED_PAGE
  if (WiFi.status() == WL_CONNECTED) {
    Result = RESULT_SUCCESS;
    //Serial.println(F("Success"));
  } else {
    Result = RESULT_FAILED;
    //Serial.println(F("Failed"));
  }
#else
  if (WiFi.status() == WL_CONNECTED) {
    buildPage(SUCCESS_PAGE);
    server->send(200, String(F("text/html")), *pagetoserve);
    //delay(3000);
    //ESP.restart();
    Done = true;
  } else {
    buildPage(FAILURE_PAGE);
    server->send(200, String(F("text/html")), *pagetoserve);
  }
#endif
}

/*
 * Function for home page
 */
void wifisetup_handleRoot() {
  if (server->hasArg(F("SSID")) && server->hasArg(F("Passphrase"))) {
    selectedSSID = server->arg(F("SSID"));
#ifdef ENHANCED_PAGE
    buildPage(CONNECTING_PAGE);
    server->send(200, String(F("text/html")), *pagetoserve);
#endif
    wifisetup_handleSubmit();
  }
  else {
    server->sendHeader(String(F("Cache-Control")), String(F("no-cache, no-store, must-revalidate")));
    server->sendHeader(String(F("Pragma")), String(F("no-cache")));
    server->sendHeader(String(F("Expires")), String(F("-1")));
    buildPage(CONFIG_PAGE);
    server->send(200, String(F("text/html")), *pagetoserve);
  }
}

void captivePortalWatchdog_cb(TimerHandle_t xTimer) {
  // if we are stuck in the captive portal, just reboot
  if (!Done) {
    Serial.println(F("Captive portal watchdog: rebooting..."));
    ESP.restart();
  }
}

/*
 * Function for loading captive portal form
 */
void startWiFiManager(void) {
  TimerHandle_t captivePortalWatchdogTimer;
  DNSServer dnsServer;
  WebServer webserver;
  server = &webserver;
  String webpage;
  pagetoserve = &webpage;

  Done = false;
  Result = RESULT_NONE;
  selectedSSID = "<none>";

  WiFi.persistent(true);

  // Setup access point
  const char* ssid     = "ESP32 WiFi Manager";
  const char* password = NULL; // Open Network
  //const char* password = "12345678";
  Serial.println(F("Setting Access Point..."));
  WiFi.softAP(ssid, password);
  IPAddress apIP = WiFi.softAPIP();
  Serial.print(F("AP IP address: "));
  Serial.println(apIP);

  // Setup the DNS server redirecting all the domains to the apIP
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(53, F("*"), apIP);

  // Setup web pages
  server->on(F("/"), wifisetup_handleRoot);
  server->on(F("/result"), wifisetup_handleResult);
  server->on(F("/success"), wifisetup_handleSuccess);
  server->on(F("/fail"), wifisetup_handleFail);
  server->onNotFound(wifisetup_handleNotFound);
  server->begin();

  Serial.println(F("WiFi Manager server started"));
  captivePortalWatchdogTimer = xTimerCreate("captivePortalWatchdog", pdMS_TO_TICKS(5*60e3), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(captivePortalWatchdog_cb));
  xTimerStart(captivePortalWatchdogTimer, 0);

  while(!Done){
    dnsServer.processNextRequest();
    server->handleClient();
    delay(100);
  }
  // Job done, stopping everything
  xTimerStop(captivePortalWatchdogTimer, 0);
  server->stop();
  //delete server;
  dnsServer.stop();
  //delete &dnsServer;
  //delete pagetoserve;
  WiFi.softAPdisconnect(true);
  WiFi.enableAP(false);
}



void buildPage(Page_t page) {
#ifndef ENHANCED_PAGE
  switch (page) {
    case CONFIG_PAGE:
      *pagetoserve = INDEX_HTML1;
      // Scanning available Wi-Fi networks
      int n = WiFi.scanNetworks();
      for (int i = 0; i < n; ++i) {
        *pagetoserve += String(F("SSID: "))+WiFi.SSID(i)+String(F(", RSSI: "))+String(WiFi.RSSI(i))+String(F(", Channel: "))+String(WiFi.channel(i));
        *pagetoserve += (WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?String(F(" ")):String(F(" *"));
        *pagetoserve += String(F("<br />"));
      }
      *pagetoserve += INDEX_HTML2;
      break;

    case CONNECTING_PAGE:
      *pagetoserve =
      "<h1>Connection attempt</h1>"
      "<h2>Device is trying to connect to the network...</h2>";
      break;

    case SUCCESS_PAGE:
      *pagetoserve =
      "<h1>Success</h1>"
      "<h2>Device is now connected to " + selectedSSID + "</h2>";
      break;

    case FAILURE_PAGE:
      *pagetoserve =
      "<h1>Error</h1>"
      "<h2>Decice could not connect to "+ selectedSSID + "<br /><a href='/'>Go back</a> and try again</h2>";
      break;
  }
#else
  switch (page) {
    case CONFIG_PAGE: {
/*
      *pagetoserve = MOWM_PAGE_CONFIGNEW_1;
      // Scanning available Wi-Fi networks
      int hidden = 0;
      int n = WiFi.scanNetworks();
      for (int i = 0; i < n; ++i) {
        String ssid = WiFi.SSID(i);
        *pagetoserve += "<input type=\"button\" onClick=\"onFocus(this.getAttribute('value'))\" value=\"" + ssid + "\">";
        *pagetoserve += "<label class=\"slist\">" + String(WiFi.RSSI(i)) + "&#037;&ensp;Ch." + String(WiFi.channel(i)) + "</label>";
        *pagetoserve += (WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"<span class=\"img-lock\"></span>";
        *pagetoserve +="<br>";
        if (ssid.length() == 0)
          hidden++;
      }
      *pagetoserve += "<div style=\"margin:16px 0 8px 0;border-bottom:solid 1px #263238;\">" AUTOCONNECT_PAGECONFIG_TOTAL  + String(n) + " " + AUTOCONNECT_PAGECONFIG_HIDDEN + String(hidden) + "</div>";
      *pagetoserve += MOWM_PAGE_CONFIGNEW_2;
*/
      *pagetoserve =  MOWM_ELM_HTML_HEAD;
      *pagetoserve += MOWM_PAGE_CONFIGNEW_1;
      *pagetoserve += MOWM_CSS_BASE;
      *pagetoserve += MOWM_CSS_ICON_LOCK;
      *pagetoserve += MOWM_CSS_UL;
      *pagetoserve += MOWM_CSS_INPUT_BUTTON;
      *pagetoserve += MOWM_CSS_INPUT_TEXT;
      *pagetoserve += MOWM_CSS_LUXBAR;
      *pagetoserve += MOWM_PAGE_CONFIGNEW_2;
      *pagetoserve += MOWM_ELM_MENU_PRE;
      *pagetoserve += MOWM_ELM_MENU_POST;
      *pagetoserve += MOWM_PAGE_CONFIGNEW_3;
      // Scanning available Wi-Fi networks
      int hidden = 0;
      int n = WiFi.scanNetworks();
      for (int i = 0; i < n; ++i) {
        String ssid = WiFi.SSID(i);
        *pagetoserve += "<input type=\"button\" onClick=\"onFocus(this.getAttribute('value'))\" value=\"" + ssid + "\">";
        *pagetoserve += "<label class=\"slist\">" + String(WiFi.RSSI(i)) + "&#037;&ensp;Ch." + String(WiFi.channel(i)) + "</label>";
        *pagetoserve += (WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"<span class=\"img-lock\"></span>";
        *pagetoserve +="<br>";
        if (ssid.length() == 0)
          hidden++;
      }
      *pagetoserve += "<div style=\"margin:16px 0 8px 0;border-bottom:solid 1px #263238;\">" AUTOCONNECT_PAGECONFIG_TOTAL  + String(n) + " " + AUTOCONNECT_PAGECONFIG_HIDDEN + String(hidden) + "</div>";
      *pagetoserve += MOWM_PAGE_CONFIGNEW_4;
      }
      break;

    case CONNECTING_PAGE:
      *pagetoserve =  " ";// {{REQ}};
      *pagetoserve += MOWM_ELM_HTML_HEAD;
      *pagetoserve +=
      "<title>" AUTOCONNECT_PAGETITLE_CONNECTING "</title>"
      "<style type=\"text/css\">";
      *pagetoserve += MOWM_CSS_BASE;
      *pagetoserve += MOWM_CSS_SPINNER;
      *pagetoserve += MOWM_CSS_LUXBAR;
      *pagetoserve +=
      "</style>"
      "</head>"
      "<body style=\"padding-top:58px;\">"
      "<div class=\"container\">";
      *pagetoserve += MOWM_ELM_MENU_PRE;
      *pagetoserve += MOWM_ELM_MENU_POST;
      *pagetoserve +=
        "<div class=\"spinner\">"
          "<div class=\"dbl-bounce1\"></div>"
          "<div class=\"dbl-bounce2\"></div>"
          "<div style=\"position:absolute;left:-100%;right:-100%;text-align:center;margin:10px auto;font-weight:bold;color:#0b0b33;\">" + selectedSSID +"</div>"
        "</div>"
      "</div>"
      "<script type=\"text/javascript\">"
        "setTimeout(\"link()\"," AUTOCONNECT_RESPONSE_WAITTIME ");"
        "function link(){location.href='" AUTOCONNECT_URI_RESULT "';}"
      "</script>"
      "</body>"
      "</html>";
      break;

    case SUCCESS_PAGE:
      *pagetoserve =
      "<h1>Success</h1>"
      "<h2>Device is now connected to " + selectedSSID + "</h2>";
      break;

    case FAILURE_PAGE:
      *pagetoserve =
      "<h1>Error</h1>"
      "<h2>Decice could not connect to "+ selectedSSID + "<br /><a href='/'>Go back</a> and try again</h2>";
      break;
  }

#endif

}