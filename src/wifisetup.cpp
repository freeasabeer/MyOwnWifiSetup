#include <WebServer.h>
#include <String.h>
#include <WiFi.h>
#include <DNSServer.h>
#include "wifisetup.h"
#include <Preferences.h>

#define URI_RESULT "/result"
#define URI_SUCCESS "/success"
#define URI_FAIL "/fail"
#define RESPONSE_WAITTIME "2000"

//#define BASIC_PAGE
#define ENHANCED_PAGE

#ifdef BASIC_PAGE
  #ifdef ENHANCED_PAGE
    #undef ENHANCED_PAGE
  #endif
#endif

#include "webpage.h"

MOWM::MOWM() {
  this->Done = false;
  this->Result = RESULT_NONE;
  this->selectedSSID = "<none>";
  this->cb = nullptr;
}

MOWM::MOWM(void (*cb)(const char* param)) {
  this->Done = false;
  this->Result = RESULT_NONE;
  this->selectedSSID = "<none>";
  this->cb = cb;
}

/*
 * Function to handle unknown URLs
 */
bool MOWM::isIp(String str) {
  for (int i = 0; i < str.length(); i++) {
    int c = str.charAt(i);
    if (c != '.' && (c < '0' || c > '9')) {
      return false;
    }
  }
  return true;
}
void MOWM::waitForEndTransmission(void) {
  while (this->server->client().connected()) {
    delay(1);
    yield();
  }
}
String MOWM::decodeWiFiStatus(wl_status_t wl_status) {
  const char *statusTable [] PROGMEM = {
    PSTR("IDLE"),
    PSTR("NO_SSID_AVAIL"),
    PSTR("SCAN_COMPLETED"),
    PSTR("CONNECTED"),
    PSTR("CONNECT_FAILED"),
    PSTR("CONNECTION_LOST"),
    PSTR("DISCONNECTED"),
    PSTR("NO_SHIELD")
  };
  const char *status = nullptr;
  switch (wl_status) {
    case WL_IDLE_STATUS:
      status = statusTable[0];
      break;
    case WL_NO_SSID_AVAIL:
      status = statusTable[1];
      break;
    case WL_SCAN_COMPLETED:
      status = statusTable[2];
      break;
    case WL_CONNECTED:
      status = statusTable[3];
      break;
    case WL_CONNECT_FAILED:
      status = statusTable[4];
      break;
    case WL_CONNECTION_LOST:
      status = statusTable[5];
      break;
    case WL_DISCONNECTED:
      status = statusTable[6];
      break;
    case WL_NO_SHIELD:
      status = statusTable[7];
      break;
  }
  return String(status);
}

String MOWM::get_WIFI_MODE(void) {
  String wifiMode;

  switch (WiFi.getMode()) {
  case WIFI_OFF:
    wifiMode = PSTR("OFF");
    break;
  case WIFI_STA:
    wifiMode = PSTR("STA");
    break;
  case WIFI_AP:
    wifiMode = PSTR("AP");
    break;
  case WIFI_AP_STA:
    wifiMode = PSTR("AP_STA");
    break;
#ifdef ARDUINO_ARCH_ESP32
  case WIFI_MODE_MAX:
    wifiMode = PSTR("MAX");
    break;
#endif
  default:
    wifiMode = PSTR("experimental");
  }
  return wifiMode;
}

bool MOWM::captivePortal() {
  String  hostHeader = this->server->hostHeader();
  if (!isIp(hostHeader) && (hostHeader != WiFi.localIP().toString()) && (!hostHeader.endsWith(F(".local")))) {
    Serial.print(F("Request redirected to captive portal"));
    this->server->sendHeader(String(F("Location")), String(F("http://")) + this->server->client().localIP().toString() + String(F("/")), true);
    this->server->send( 302, String(F("text/plain")), this->_emptyString); // Empty content inhibits Content-length header so we have to close the socket ourselves.
    this->server->client().stop(); // Stop is needed because we sent no content length
    return true;
  }
  return false;
}

void MOWM::handleNotFound() {
  Serial.printf("handleNotFound: %s\n", this->server->uri().c_str());
  if (captivePortal()) { // If captive portal redirect instead of displaying the error page.
    return;
  }
  String message = String(F("File Not Found\n\n"));
  message += String(F("URI: "));
  message += this->server->uri();
  message += String(F("\nMethod: "));
  message += (this->server->method() == HTTP_GET) ? String(F("GET")) : String(F("POST"));
  message += String(F("\nArguments: "));
  message += this->server->args();
  message += String(F("\n"));
  for (uint8_t i = 0; i < this->server->args(); i++) {
    message += String(F(" ")) + this->server->argName(i) + String(F(": ")) + this->server->arg(i) + String(F("\n"));
  }
  this->server->send(404, String(F("text/plain")), message);
  Serial.print(message);
}
void MOWM::handleResult() {
  String redirect = String(F("http://"));
  redirect += this->server->client().localIP().toString();
  switch(this->Result) {
    case RESULT_NONE:
      redirect += String(F(URI_RESULT));
      break;
    case RESULT_FAILED:
      redirect += String(F(URI_FAIL));
    break;
    case RESULT_SUCCESS:
      redirect += String(F(URI_SUCCESS));
    break;
  }
  // Redirect to result page
  this->server->sendHeader(String(F("Location")), redirect, true);
  this->server->send(302, String(F("text/plain")), this->_emptyString);
  this->server->client().stop();
  waitForEndTransmission();  // Wait for response transmission complete
}

void MOWM::handleSuccess() {
  buildPage(SUCCESS_PAGE);
  this->server->send(200, String(F("text/html")), *this->pagetoserve);
  waitForEndTransmission();  // Wait for response transmission complete
  this->Done = true;
}

void MOWM::handleFail() {
  buildPage(FAILURE_PAGE);
  this->server->send(200, String(F("text/html")), *this->pagetoserve);
}

/*
 * Function for handling form
 */
void MOWM::handleSubmit() {
  int i = 20;
  if (this->mqtt_ip != this->server->arg(F("MQTT"))) {
    this->mqtt_ip = this->server->arg(F("MQTT"));
    Preferences pref;
    pref.begin("mows", false);
    pref.putString("mqtt", this->mqtt_ip);
    pref.end();
  }
  Serial.println(String(F("Trying to connect to "))+this->server->arg(F("SSID")));
  WiFi.begin(this->server->arg(F("SSID")).c_str(), (const char *)this->server->arg(F("Passphrase")).c_str());
  while ((WiFi.status() != WL_CONNECTED) && (i > 0)){
    delay(500);
    i--;
  }

  if (WiFi.status() == WL_CONNECTED) {
    this->Result = RESULT_SUCCESS;
    //Serial.println(F("Success"));
  } else {
    this->Result = RESULT_FAILED;
    //Serial.println(F("Failed"));
  }
}

/*
 * Function for home page
 */
void MOWM::handleRoot() {
  if (this->server->hasArg(F("SSID")) && this->server->hasArg(F("Passphrase"))) {
    this->selectedSSID = this->server->arg(F("SSID"));
    buildPage(CONNECTING_PAGE);
    this->server->send(200, String(F("text/html")), *this->pagetoserve);
    handleSubmit();
  }
  else {
    this->server->sendHeader(String(F("Cache-Control")), String(F("no-cache, no-store, must-revalidate")));
    this->server->sendHeader(String(F("Pragma")), String(F("no-cache")));
    this->server->sendHeader(String(F("Expires")), String(F("-1")));
    buildPage(CONFIG_PAGE);
    this->server->send(200, String(F("text/html")), *this->pagetoserve);
  }
}

static void static_captivePortalWatchdog_cb(TimerHandle_t xTimer) {
  MOWM *me;
  me = static_cast<MOWM*>(pvTimerGetTimerID(xTimer));
  me->captivePortalWatchdog_cb(xTimer);
}

void MOWM::captivePortalWatchdog_cb(TimerHandle_t xTimer) {
  // if we are stuck in the captive portal, just reboot
  if (!this->Done) {
    Serial.println(F("Captive portal watchdog: rebooting..."));
    ESP.restart();
  }
}

/*
 * Function for loading captive portal form
 */
void MOWM::startWiFiManager(bool doReboot) {
  TimerHandle_t captivePortalWatchdogTimer;
  DNSServer dnsServer;
  WebServer webserver;
  this->server = &webserver;
  String webpage;
  this->pagetoserve = &webpage;


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
  this->server->on(F("/"), std::bind(&MOWM::handleRoot, this));
  this->server->on(F(URI_RESULT), std::bind(&MOWM::handleResult, this));
  this->server->on(F(URI_SUCCESS), std::bind(&MOWM::handleSuccess, this));
  this->server->on(F(URI_FAIL), std::bind(&MOWM::handleFail, this));
  this->server->onNotFound(std::bind(&MOWM::handleNotFound, this));
  this->server->begin();

  Serial.println(F("WiFi Manager server started"));
  captivePortalWatchdogTimer = xTimerCreate("captivePortalWatchdog", pdMS_TO_TICKS(5*60e3), pdFALSE, this, reinterpret_cast<TimerCallbackFunction_t>(static_captivePortalWatchdog_cb));
  xTimerStart(captivePortalWatchdogTimer, 0);
  if (this->cb)
    (*this->cb)(apIP.toString().c_str());

  while(!this->Done){
    dnsServer.processNextRequest();
    this->server->handleClient();
    delay(100);
  }
  // Job done, stopping everything
  xTimerStop(captivePortalWatchdogTimer, 0);
  this->server->stop();
  //delete server;
  dnsServer.stop();
  //delete &dnsServer;
  //delete this->pagetoserve;
  WiFi.softAPdisconnect(true);
  WiFi.enableAP(false);

  if (doReboot)
    ESP.restart();
}

void MOWM::begin(bool doReboot, unsigned long msec_try, bool force_activation) {
  WiFi.begin();
  unsigned long i = msec_try/500;
  while((WiFi.status() != WL_CONNECTED) && (i>0)) {
    delay(500);
    i--;
  }
  if (force_activation) {
    startWiFiManager(doReboot);
  } else {
  if (WiFi.status() != WL_CONNECTED)
    startWiFiManager(doReboot);
  }
}


void MOWM::buildPage(Page_t page) {
#ifndef ENHANCED_PAGE
  switch (page) {
    case CONFIG_PAGE: {
      *this->pagetoserve = INDEX_HTML1;
      // Scanning available Wi-Fi networks
      int n = WiFi.scanNetworks();
      for (int i = 0; i < n; ++i) {
        *this->pagetoserve += String(F("SSID: "))+WiFi.SSID(i)+String(F(", RSSI: "))+String(WiFi.RSSI(i))+String(F(", Channel: "))+String(WiFi.channel(i));
        *this->pagetoserve += (WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?String(F(" ")):String(F(" *"));
        *this->pagetoserve += String(F("<br />"));
      }
      //*this->pagetoserve += INDEX_HTML2;
      *this->pagetoserve += "</p><p>";
      *this->pagetoserve += "<label>SSID:&nbsp;</label>";
      *this->pagetoserve += "<input maxlength=\"30\" name=\"SSID\"><br>";
      *this->pagetoserve += "<label>Key:&nbsp;&nbsp;&nbsp;&nbsp;</label><input type=\"password\" maxlength=\"30\" name=\"Passphrase\"><br>";
      Preferences pref;
      pref.begin("mows", true);
      String mqtt_ip = pref.getString("mqtt", F("192.168.100.24"));
      pref.end();
      *this->pagetoserve += "<label>MQTT:&nbsp;</label>";
      *this->pagetoserve += "<input maxlength=\"16\" name=\"MQTT\" value=\""+mqtt_ip+"\"><br>";
      *this->pagetoserve += "<input type=\"submit\" value=\"Save\">";
      *this->pagetoserve += "</p>";
      *this->pagetoserve += "</form>";
      *this->pagetoserve += "</body>";
      *this->pagetoserve += "</html>";
      }
      break;

    case CONNECTING_PAGE:
      *this->pagetoserve =
      "<!DOCTYPE html>"
      "<html>"
      "<head>"
      "<meta charset=\"UTF-8\" name=\"viewport\" content=\"width=device-width,initial-scale=1\">"
      "<title>Connect</title>"
      "</head>"
      "<body>"
      "<h1>Connection attempt</h1>"
      "<h2>Device is trying to connect to the network...</h2>"
      "<script type=\"text/javascript\">"
        "setTimeout(\"link()\"," RESPONSE_WAITTIME ");"
        "function link(){location.href='" URI_RESULT "';}"
      "</script>"
      "</body>"
      "</html>";
      break;

    case SUCCESS_PAGE:
      *this->pagetoserve =
      "<!DOCTYPE html>"
      "<html>"
      "<head>"
      "<meta charset=\"UTF-8\" name=\"viewport\" content=\"width=device-width,initial-scale=1\">"
      "<title>Success</title>"
      "</head>"
      "<body>"
      "<h1>Success</h1>"
      "<h2>Device is now connected to " + this->selectedSSID + " ("+WiFi.localIP().toString()+")</h2>"
<<<<<<< HEAD
      "<h2>MQTT server set to " + this->mqtt_ip + "</h2>"
=======
>>>>>>> main
      "</body>"
      "</html>";
      break;

    case FAILURE_PAGE:
      *this->pagetoserve =
      "<!DOCTYPE html>"
      "<html>"
      "<head>"
      "<meta charset=\"UTF-8\" name=\"viewport\" content=\"width=device-width,initial-scale=1\">"
      "<title>Failure</title>"
      "</head>"
      "<body>"
      "<h1>Error</h1>"
      "<h2>Decice could not connect to "+ this->selectedSSID + "<br /><a href='/'>Go back</a> and try again</h2>"
      "</body>"
      "</html>";
      break;
  }
#else
  switch (page) {
    case CONFIG_PAGE: {
      *this->pagetoserve =  MOWM_ELM_HTML_HEAD;
      *this->pagetoserve += MOWM_PAGE_CONFIGNEW_1;
      *this->pagetoserve += MOWM_CSS_BASE;
      *this->pagetoserve += MOWM_CSS_ICON_LOCK;
      *this->pagetoserve += MOWM_CSS_UL;
      *this->pagetoserve += MOWM_CSS_INPUT_BUTTON;
      *this->pagetoserve += MOWM_CSS_INPUT_TEXT;
      *this->pagetoserve += MOWM_CSS_LUXBAR;
      *this->pagetoserve += MOWM_PAGE_CONFIGNEW_2;
      *this->pagetoserve += MOWM_ELM_MENU_PRE;
      *this->pagetoserve += MOWM_ELM_MENU_POST;
      *this->pagetoserve += MOWM_PAGE_CONFIGNEW_3;
      // Scanning available Wi-Fi networks
      int hidden = 0;
      int n = WiFi.scanNetworks();
      for (int i = 0; i < n; ++i) {
        String ssid = WiFi.SSID(i);
        *this->pagetoserve += "<input type=\"button\" onClick=\"onFocus(this.getAttribute('value'))\" value=\"" + ssid + "\">";
        *this->pagetoserve += "<label class=\"slist\">" + String(WiFi.RSSI(i)) + "&#037;&ensp;Ch." + String(WiFi.channel(i)) + "</label>";
        *this->pagetoserve += (WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"<span class=\"img-lock\"></span>";
        *this->pagetoserve +="<br>";
        if (ssid.length() == 0)
          hidden++;
      }
      *this->pagetoserve += "<div style=\"margin:16px 0 8px 0;border-bottom:solid 1px #263238;\">" AUTOCONNECT_PAGECONFIG_TOTAL  + String(n) + " " + AUTOCONNECT_PAGECONFIG_HIDDEN + String(hidden) + "</div>";
      //*this->pagetoserve += MOWM_PAGE_CONFIGNEW_4;
      *this->pagetoserve += "<ul class=\"noorder\">";
      *this->pagetoserve +=   "<li>";
      *this->pagetoserve +=     "<label for=\"ssid\">" AUTOCONNECT_PAGECONFIG_SSID "</label>";
      *this->pagetoserve +=     "<input id=\"ssid\" type=\"text\" name=\"" AUTOCONNECT_PARAMID_SSID "\" placeholder=\"" AUTOCONNECT_PAGECONFIG_SSID "\">";
      *this->pagetoserve +=   "</li>";
      *this->pagetoserve +=   "<li>";
      *this->pagetoserve +=     "<label for=\"passphrase\">" AUTOCONNECT_PAGECONFIG_PASSPHRASE "</label>";
      *this->pagetoserve +=     "<input id=\"passphrase\" type=\"password\" name=\"" AUTOCONNECT_PARAMID_PASS "\" placeholder=\"" AUTOCONNECT_PAGECONFIG_PASSPHRASE "\">";
      *this->pagetoserve +=   "</li>";
      /* champ pour MQTT */
      Preferences pref;
      pref.begin("mows", true);
      String mqtt_ip = pref.getString("mqtt", "192.168.100.24");
      pref.end();
      *this->pagetoserve +=   "<li>";
      *this->pagetoserve +=     "<label for=\"mqtt\">" AUTOCONNECT_PAGECONFIG_MQTT "</label>";
      String default_mqtt_ip = "192.168.100.24";
      *this->pagetoserve +=     "<input id=\"mqtt\" type=\"text\" name=\"" AUTOCONNECT_PARAMID_MQTT "\" placeholder=\"" AUTOCONNECT_PAGECONFIG_MQTT "\" value=\"" + mqtt_ip + "\">";
      *this->pagetoserve +=   "</li>";

       *this->pagetoserve +=   "<li><input type=\"submit\" name=\"apply\" value=\"" AUTOCONNECT_PAGECONFIG_APPLY "\"></li>";
      *this->pagetoserve +=  "</ul>";
      *this->pagetoserve +=  "</form>";
      *this->pagetoserve +=  "</div>";
      *this->pagetoserve +=  "</div>";
      *this->pagetoserve +=  "<script type=\"text/javascript\">";
      *this->pagetoserve +=     "function onFocus(e){";
      *this->pagetoserve +=       "document.getElementById('ssid').value=e,document.getElementById('passphrase').focus()";
      *this->pagetoserve +=     "}";
      *this->pagetoserve +=   "</script>";
      *this->pagetoserve +=   "</body>";
      *this->pagetoserve +=   "</html>";
      }
      break;

    case CONNECTING_PAGE:
      *this->pagetoserve =  " ";// {{REQ}};
      *this->pagetoserve += MOWM_ELM_HTML_HEAD;
      *this->pagetoserve +=
      "<title>" AUTOCONNECT_PAGETITLE_CONNECTING "</title>"
      "<style type=\"text/css\">";
      *this->pagetoserve += MOWM_CSS_BASE;
      *this->pagetoserve += MOWM_CSS_SPINNER;
      *this->pagetoserve += MOWM_CSS_LUXBAR;
      *this->pagetoserve +=
      "</style>"
      "</head>"
      "<body style=\"padding-top:58px;\">"
      "<div class=\"container\">";
      *this->pagetoserve += MOWM_ELM_MENU_PRE;
      *this->pagetoserve += MOWM_ELM_MENU_POST;
      *this->pagetoserve +=
        "<div class=\"spinner\">"
          "<div class=\"dbl-bounce1\"></div>"
          "<div class=\"dbl-bounce2\"></div>"
          "<div style=\"position:absolute;left:-100%;right:-100%;text-align:center;margin:10px auto;font-weight:bold;color:#0b0b33;\">" + this->selectedSSID +"</div>"
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
      *this->pagetoserve =  MOWM_ELM_HTML_HEAD;
      *this->pagetoserve +=
        "<title>" AUTOCONNECT_PAGETITLE_STATISTICS "</title>"
        "<style type=\"text/css\">";
      *this->pagetoserve += MOWM_CSS_BASE;
      *this->pagetoserve += MOWM_CSS_TABLE;
      *this->pagetoserve += MOWM_CSS_LUXBAR;
      *this->pagetoserve +=
        "</style>"
      "</head>"
      "<body style=\"padding-top:58px;\">"
      "<div class=\"container\">";
      *this->pagetoserve += MOWM_ELM_MENU_PRE;
      *this->pagetoserve += MOWM_ELM_MENU_POST;
      *this->pagetoserve +=
        "<div>"
        "<table class=\"info\" style=\"border:none;\">"
          "<tbody>"
          "<tr>"
            "<td>" AUTOCONNECT_PAGESTATS_ESTABLISHEDCONNECTION "</td>"
            "<td>" + WiFi.SSID() + "</td>"
          "</tr>"
          "<tr>"
            "<td>" AUTOCONNECT_PAGESTATS_MODE "</td>"
            "<td>" + get_WIFI_MODE() + "(" + WiFi.status() + ")</td>"
          "</tr>"
          "<tr>"
            "<td>" AUTOCONNECT_PAGESTATS_IP "</td>"
            "<td>" + WiFi.localIP().toString() + "</td>"
          "</tr>"
            "<td>" AUTOCONNECT_PAGESTATS_GATEWAY "</td>"
            "<td>" + WiFi.gatewayIP().toString() + "</td>"
          "</tr>"
          "<tr>"
            "<td>" AUTOCONNECT_PAGESTATS_SUBNETMASK "</td>"
            "<td>" + WiFi.subnetMask().toString() + "</td>"
          "</tr>"
          "<tr>"
            "<td>" AUTOCONNECT_PAGESTATS_CHANNEL "</td>"
            "<td>" + WiFi.channel() + "</td>"
          "</tr>"
          "<tr>"
            "<td>" AUTOCONNECT_PAGESTATS_DBM "</td>"
            "<td>" + WiFi.RSSI() + "</td>"
          "</tr>"
          "<tr>"
            "<td> MQTT server </td>"
            "<td>" + this->mqtt_ip + "</td>"
          "</tr>"
          "</tbody>"
        "</table>"
      "</div>"
    "</div>"
  "</body>"
  "</html>";
      break;

    case FAILURE_PAGE:
      *this->pagetoserve =  MOWM_ELM_HTML_HEAD;
      *this->pagetoserve +=
  "<title>" AUTOCONNECT_PAGETITLE_CONNECTIONFAILED "</title>"
    "<style type=\"text/css\">";
      *this->pagetoserve += MOWM_CSS_BASE;
      *this->pagetoserve += MOWM_CSS_TABLE;
      *this->pagetoserve += MOWM_CSS_LUXBAR;
      *this->pagetoserve +=
    "</style>"
  "</head>"
  "<body style=\"padding-top:58px;\">"
    "<div class=\"container\">";
      *this->pagetoserve += MOWM_ELM_MENU_PRE;
      *this->pagetoserve += MOWM_ELM_MENU_POST;
      *this->pagetoserve +=
      "<div>"
        "<table class=\"info\" style=\"border:none;\">"
          "<tbody>"
          "<tr>"
            "<td>" AUTOCONNECT_PAGECONNECTIONFAILED_CONNECTIONFAILED "</td>"
            "<td>(" + String(WiFi.status()) + ") " + decodeWiFiStatus(WiFi.status()) +"</td>"
          "</tr>"
          "</tbody>"
        "</table>"
      "</div>"
    "</div>"
  "</body>"
  "</html>";
      break;
  }

#endif
}
