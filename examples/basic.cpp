#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "esp_wifi.h"
#include <wifisetup.h>


WebServer server(80);

static const char *sEnable = "enable";
void handle_settings()
{
    String Message = "";

    for (uint8_t i = 0; i < server.args(); i++) {
        // connect to http://<esp32 IP address>:/settings?wificonf=reset
        // to actually reset the internal wi-fi data stored by the esp32
        // this can be usefull to make some tests on the captive portal :)
        if (server.argName(i) == "wificonf") {
            if (server.arg(i) == "reset") {
                wifi_config_t wfconf;
                memset(&wfconf, 0, sizeof(wifi_config_t));
                esp_wifi_set_config(WIFI_IF_STA, &wfconf);
                Message += "Reset of wifi configuration done.\n";
            } else {
                Message += "Not a valid wificonf argument.\n";
            }
        }

    }
    server.send(200, "text/plain", Message);
    Serial.printf("%s\n", Message.c_str());

}

void handleNotFound()
{
    String message = "Server is running!\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    server.send(200, "text/plain", message);
}

void setup()
{
    Serial.begin(115200);
    Serial.printf("\n");

    server.on("/settings", HTTP_GET, handle_settings);
    server.on("/favicon.ico", HTTP_GET,[]() {
        server.sendHeader("Connection", "close");
        server.send(200, "text/html", "<link rel=\"icon\" href=\"data:;base64,iVBORw0KGgo=\">");
        });
    server.onNotFound(handleNotFound);

    // Try to connect for 10 seconds, if not successfull, switch to Wifi Manager captive portal
    Serial.printf("Connecting to Wi-Fi...\n");
    WiFi.begin();
    int i = 10;
    while((WiFi.status() != WL_CONNECTED) && (i>0)) {
        delay(1000);
        i--;
    }
    if (WiFi.status() != WL_CONNECTED)
        startWiFiManager();

    // If we reached that point, then we got connected to an access point as a client !

    debug("WiFi connected\n");
    debug("IP address: %s\n", WiFi.localIP().toString().c_str());
    server.begin();
    MDNS.begin("basic");
    MDNS.addService("http", "tcp", 80);
    debug("HTTP server started.\n");
}


void loop()
{
    server.handleClient();
    delay(100);
}
