#include <WebServer.h>
#include <String.h>
#include <WiFi.h>
#include <DNSServer.h>

static const char INDEX_HTML1[] =
"<!DOCTYPE HTML>"
"<html>"
"<head>"
"<meta content=\"text/html; charset=ISO-8859-1\""
" http-equiv=\"content-type\">"
"<meta name = \"viewport\" content = \"width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0\">"
"<title>Wi-Fi Manager</title>"
"<style>"
"\"body { background-color: #808080; font-family: Arial, Helvetica, Sans-Serif; Color: #000000; text-align:center;}\""
"</style>"
"</head>"
"<body>"
"<h3>Enter your Wi-Fi credentials</h3>"
"<form action=\"/\" method=\"post\">"
"<p>";

static const char INDEX_HTML2[] =
"</p><p>"
"<label>SSID:&nbsp;</label>"
"<input maxlength=\"30\" name=\"ssid\"><br>"
"<label>Key:&nbsp;&nbsp;&nbsp;&nbsp;</label><input type=\"password\" maxlength=\"30\" name=\"password\"><br>"
"<input type=\"submit\" value=\"Save\">"
"</p>"
"</form>"
"</body>"
"</html>";

static WebServer *server;
static bool Done = false;
static String availableNetworks = "";

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

bool wifisetup_captivePortal() {
  String  hostHeader = server->hostHeader();
  if (!isIp(hostHeader) && (hostHeader != WiFi.localIP().toString()) && (!hostHeader.endsWith(F(".local")))) {
    Serial.print("Request redirected to captive portal");
    server->sendHeader("Location", String("http://") + server->client().localIP().toString() + String("/"), true);
    server->send ( 302, "text/plain", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.
    server->client().stop(); // Stop is needed because we sent no content length
    return true;
  }
  return false;
}

void wifisetup_handleNotFound() {
  if (wifisetup_captivePortal()) { // If captive portal redirect instead of displaying the error page.
    return;
  }
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server->uri();
  message += "\nMethod: ";
  message += (server->method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server->args();
  message += "\n";
  for (uint8_t i = 0; i < server->args(); i++) {
    message += " " + server->argName(i) + ": " + server->arg(i) + "\n";
  }
  server->send(404, "text/plain", message);
  Serial.printf(message.c_str());
}

/*
 * Function for handling form
 */
void wifisetup_handleSubmit() {
  String response_success=
  "<h1>Success</h1>"
  "<h2>Device will switch to configured network in 3 seconds</h2>";

  String response_error=
  "<h1>Error</h1>"
  "<h2><a href='/'>Go back</a>and try again";

  int i = 20;
  WiFi.begin(server->arg("ssid").c_str(), (const char *)server->arg("password").c_str());
  while ((WiFi.status() != WL_CONNECTED) && (i > 0)){
    delay(500);
    i--;
  }

  if (WiFi.status() == WL_CONNECTED) {
      server->send(200, "text/html", response_success);
      delay(3000);
      //ESP.restart();
      Done = true;
  } else {
      server->send(200, "text/html", response_error);
  }
}

/*
 * Function for home page
 */
void wifisetup_handleRoot() {
  if (server->hasArg("ssid") && server->hasArg("password")) {
    wifisetup_handleSubmit();
  }
  else {
    server->sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    server->sendHeader("Pragma", "no-cache");
    server->sendHeader("Expires", "-1");
    server->send(200, "text/html", INDEX_HTML1 + availableNetworks + INDEX_HTML2);
  }
}

/*
 * Function for loading form
 * Returns: false if no WiFi creds in EEPROM
 */
void startWiFiManager(void) {
  DNSServer dnsServer;
  WebServer webserver;
  server = &webserver;

  WiFi.persistent(true);

  // Scanning available Wi-Fi networks
  int n = WiFi.scanNetworks();
  for (int i = 0; i < n; ++i) {
    availableNetworks += "SSID: "+WiFi.SSID(i)+", RSSI: "+String(WiFi.RSSI(i))+", Channel: "+String(WiFi.channel(i));
    availableNetworks += (WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":" *";
    availableNetworks += "<br />";
  }
  Serial.println(availableNetworks);

  // Setup access point
  const char* ssid     = "ESP32 WiFi Manager";
  const char* password = NULL; // Open Network
  //const char* password = "12345678";
  Serial.println("Setting Access Point...");
  WiFi.softAP(ssid, password);
  IPAddress apIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(apIP);

  // Setup the DNS server redirecting all the domains to the apIP
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(53, "*", apIP);

  // Setup web pages
  server->on("/", wifisetup_handleRoot);
  server->onNotFound(wifisetup_handleNotFound);
  server->begin();

  Serial.println("WiFi Manager server started");

  while(!Done){
    dnsServer.processNextRequest();
    server->handleClient();
    delay(100);
  }
  // Job done, stopping everything
  server->stop();
  delete server;
  dnsServer.stop();
  delete &dnsServer;
  WiFi.softAPdisconnect();
}