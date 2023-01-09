#ifndef ENHANCED_PAGE
static const char INDEX_HTML1[] PROGMEM =
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

static const char INDEX_HTML2[] PROGMEM =
"</p><p>"
"<label>SSID:&nbsp;</label>"
"<input maxlength=\"30\" name=\"SSID\"><br>"
"<label>Key:&nbsp;&nbsp;&nbsp;&nbsp;</label><input type=\"password\" maxlength=\"30\" name=\"Passphrase\"><br>"
"<input type=\"submit\" value=\"Save\">"
"</p>"
"</form>"
"</body>"
"</html>";

#else  //#ifndef ENHANCED_PAGE

// Got a lof of inspiration from https://github.com/Hieromon/AutoConnect

#define AUTOCONNECT_PAGETITLE_CONFIG      "My Own Wifi Manager"
#define AUTOCONNECT_URI_CONNECT           "/"
#define AUTOCONNECT_PAGECONFIG_TOTAL      "Total:"
#define AUTOCONNECT_PAGECONFIG_HIDDEN     "Hidden:"
#define AUTOCONNECT_PAGECONFIG_SSID       "SSID"
#define AUTOCONNECT_PARAMID_SSID          "SSID"
#define AUTOCONNECT_PAGECONFIG_MQTT       "MQTT"
#define AUTOCONNECT_PARAMID_MQTT          "MQTT"
#define AUTOCONNECT_PAGECONFIG_CITY       "CITY"
#define AUTOCONNECT_PARAMID_CITY          "CITY"
#define AUTOCONNECT_PAGECONFIG_LAT        "LAT"
#define AUTOCONNECT_PARAMID_LAT           "LAT"
#define AUTOCONNECT_PAGECONFIG_LON        "LON"
#define AUTOCONNECT_PARAMID_LON           "LON"
#define AUTOCONNECT_PAGECONFIG_SLEEPDURATION "SLEEPDURATION"
#define AUTOCONNECT_PARAMID_SLEEPDURATION    "SLEEPDURATION"
#define AUTOCONNECT_PAGECONFIG_WAKEUPHOUR    "WAKEUPHOUR"
#define AUTOCONNECT_PARAMID_WAKEUPHOUR       "WAKEUPHOUR"
#define AUTOCONNECT_PAGECONFIG_SLEEPHOUR     "SLEEPHOUR"
#define AUTOCONNECT_PARAMID_SLEEPHOUR        "SLEEPHOUR"
#define AUTOCONNECT_PAGECONFIG_PASSPHRASE "Passphrase"
#define AUTOCONNECT_PARAMID_PASS          "Passphrase"
#define AUTOCONNECT_PAGECONFIG_ENABLEDHCP "Enable DHCP"
#define AUTOCONNECT_PAGECONFIG_APPLY      "Apply"
#define AUTOCONNECT_PARAMID_STAIP         "sip"
#define AUTOCONNECT_PARAMID_GTWAY         "gw"
#define AUTOCONNECT_PARAMID_NTMSK         "nm"
#define AUTOCONNECT_PARAMID_DNS1          "ns1"
#define AUTOCONNECT_PARAMID_DNS2          "ns2"
#define BOOT_URI "/"
#define MENU_TITLE "MyOwnWifiManager"
#define AUTOCONNECT_MENUCOLOR_BACKGROUND  "#263238"
#define AUTOCONNECT_MENUCOLOR_TEXT        "#fffacd"
#define AUTOCONNECT_MENUCOLOR_ACTIVE      "#37474f"
#define AUTOCONNECT_RESPONSE_WAITTIME     RESPONSE_WAITTIME
#define AUTOCONNECT_URI_RESULT            URI_RESULT
#define AUTOCONNECT_URI_SUCCESS           URI_SUCCESS
#define AUTOCONNECT_URI_FAIL              URI_FAIL
#define AUTOCONNECT_PAGETITLE_CONNECTING  "Connection tentative"
#define AUTOCONNECT_STRING_DEPLOY(s) #s
#define AUTOCONNECT_PAGESTATS_ESTABLISHEDCONNECTION "Established connection"
#define AUTOCONNECT_PAGESTATS_MODE        "Mode"
#define AUTOCONNECT_PAGESTATS_IP          "IP"
#define AUTOCONNECT_PAGESTATS_GATEWAY     "GW"
#define AUTOCONNECT_PAGESTATS_SUBNETMASK  "Subnet mask"
#define AUTOCONNECT_PAGESTATS_CHANNEL     "Channel"
#define AUTOCONNECT_PAGESTATS_DBM         "dBm"
#define AUTOCONNECT_PAGETITLE_STATISTICS "Statistics"
#define AUTOCONNECT_PAGETITLE_CONNECTIONFAILED "Connection failed"
#define AUTOCONNECT_PAGECONNECTIONFAILED_CONNECTIONFAILED "Connection Failed"

/**< Common html document header. */
//static const char  MOWM_PAGE_CONFIGNEW_1[] PROGMEM = {
static const char MOWM_ELM_HTML_HEAD[] PROGMEM = {
  "<!DOCTYPE html>"
  "<html>"
  "<head>"
  "<meta charset=\"UTF-8\" name=\"viewport\" content=\"width=device-width,initial-scale=1\">"
};
static const char  MOWM_PAGE_CONFIGNEW_1[] PROGMEM = {
    "<title>" AUTOCONNECT_PAGETITLE_CONFIG "</title>"
    "<style type=\"text/css\">"
};
/**< Basic CSS common to all pages */
static const char MOWM_CSS_BASE[] PROGMEM = {
  "html{"
    "font-family:Helvetica,Arial,sans-serif;"
    "font-size:16px;"
    "-ms-text-size-adjust:100%;"
    "-webkit-text-size-adjust:100%;"
    "-moz-osx-font-smoothing:grayscale;"
    "-webkit-font-smoothing:antialiased"
  "}"
  "body{"
    "margin:0;"
    "padding:0"
  "}"
  ".base-panel{"
    "margin:0 22px 0 22px"
  "}"
  ".base-panel * label :not(.bins){"
    "display:inline-block;"
    "width:3.0em;"
    "text-align:right"
  "}"
  ".base-panel * .slist{"
    "width:auto;"
    "font-size:0.9em;"
    "margin-left:10px;"
    "text-align:left"
  "}"
  "input{"
    "-moz-appearance:none;"
    "-webkit-appearance:none;"
    "font-size:0.9em;"
    "margin:8px 0 auto"
  "}"
  ".lap{"
    "visibility:collapse"
  "}"
  ".lap:target{"
    "visibility:visible"
  "}"
  ".lap:target .overlap{"
    "opacity:0.7;"
    "transition:0.3s"
  "}"
  ".lap:target .modal_button{"
    "opacity:1;"
    "transition:0.3s"
  "}"
  ".overlap{"
    "top:0;"
    "left:0;"
    "width:100%;"
    "height:100%;"
    "position:fixed;"
    "opacity:0;"
    "background:#000;"
    "z-index:1000"
  "}"
  ".modal_button{"
    "border-radius:13px;"
    "background:#660033;"
    "color:#ffffcc;"
    "padding:20px 30px;"
    "text-align:center;"
    "text-decoration:none;"
    "letter-spacing:1px;"
    "font-weight:bold;"
    "display:inline-block;"
    "top:40%;"
    "left:40%;"
    "width:20%;"
    "position:fixed;"
    "opacity:0;"
    "z-index:1001"
  "}"
};
/**< Image icon for inline expansion, the lock mark. */
const char MOWM_CSS_ICON_LOCK[] PROGMEM = {
  ".img-lock{"
    "width:22px;"
    "height:22px;"
    "margin-top:14px;"
    "float:right;"
    "background: url(data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABgAAAAYCAYAAADgdz34AAAACXBIWXMAAAsTAAALEwEAmpwYAAAB1ElEQVRIibWVu0scURTGf3d2drBQFAWbbRQVCwuVLIZdi2gnWIiF/4GtKyuJGAJh8mgTcU0T8T8ICC6kiIVu44gvtFEQQWwsbExQJGHXmZtiZsOyzCN3Vz+4cDjfvec7j7l3QAF95onRZ54YKmdE1IbnS0c9mnAyAjkBxDy3LRHrjtRyu7OD52HntTAyvbw/HxP2hkCearrRb2WSCSuTTGi60S+QpzFhbwznDl/VVMHw0sF7hEjFbW2qkB38lfp8nNDipWcATil+uDM3cDWyeNRSijnfkHJnezb5Vkkgvbg3IOXD2e1ts93S+icnkZOAVaalZK3YQMa4L+pC6L1WduhYSeCf0PLBdxzOjZ93Lwvm6APAiLmlF1ubPiHotmaS41ExQjH0ZbfNM1NAFpgD0lVcICIrANqAVaAd+AFIYAy4BqaBG+Wsq5AH3vgk8xpYrzf4KLAZwhe8PYEIvQe4vc6H8Hnc2dQs0AFchvAXQGdEDF8s4A5TZS34BQqqQNaS1WMI3KD4WUbNoBJfce9CO7BSr4BfBe8A21vmUwh0VdjdTyHwscL+UK+AHxoD7FDoAX6/Cnpxn4ay/egCjcCL/w1chkqLakLQ/6ABhT57uAd+Vzv/Ara3iY6fK4WxAAAAAElFTkSuQmCC) no-repeat"
  "}"
};
/**< non-marked list for UL */
static const char MOWM_CSS_UL[] PROGMEM = {
  ".noorder,.exp{"
    "padding:0;"
    "list-style:none;"
    "display:table"
  "}"
  ".noorder li,.exp{"
    "display:table-row-group"
  "}"
  ".noorder li label, .exp li{"
    "display:table-cell;"
    "width:auto;"
    "text-align:right;"
    "padding:10px 0.5em"
  "}"
  ".noorder input[type=\"checkbox\"]{"
    "-moz-appearance:checkbox;"
    "-webkit-appearance:checkbox"
  "}"
  ".noorder input[type=\"radio\"]{"
    "margin-right:0.5em;"
    "-moz-appearance:radio;"
    "-webkit-appearance:radio"
  "}"
  ".noorder input[type=\"text\"],.noorder input[type=\"number\"]{"
    "width:auto"
  "}"
  ".noorder input[type=\"text\"]:invalid{"
    "background:#fce4d6"
  "}"
};
/**< INPUT button and submit style */
static const char MOWM_CSS_INPUT_BUTTON[] PROGMEM = {
  "input[type=\"button\"],input[type=\"submit\"],button[type=\"submit\"],button[type=\"button\"]{"
    "padding:8px 0.5em;"
    "font-weight:bold;"
    "letter-spacing:0.8px;"
    "color:#fff;"
    "border:1px solid;"
    "border-radius:2px;"
    "margin-top:12px"
  "}"
  "input[type=\"button\"],button[type=\"button\"]{"
    "background-color:#1b5e20;"
    "border-color:#1b5e20;"
    "width:15em"
  "}"
  ".aux-page input[type=\"button\"],.aux-page button[type=\"button\"]{"
    "cursor:pointer;"
    "font-weight:normal;"
    "padding:8px 14px;"
    "margin:12px;"
    "width:auto"
  "}"
  "#sb[type=\"submit\"]{"
    "width:15em"
  "}"
  "input[type=\"submit\"],button[type=\"submit\"]{"
    "padding:8px 30px;"
    "background-color:#006064;"
    "border-color:#006064"
  "}"
  "input[type=\"button\"],input[type=\"submit\"],button[type=\"submit\"]:focus,"
  "input[type=\"button\"],input[type=\"submit\"],button[type=\"submit\"]:active{"
    "outline:none;"
    "text-decoration:none"
  "}"
};
/**< INPUT text style */
static const char MOWM_CSS_INPUT_TEXT[] PROGMEM = {
  "input[type=\"text\"],input[type=\"password\"],input[type=\"number\"],.aux-page select{"
    "background-color:#fff;"
    "border:1px solid #ccc;"
    "border-radius:2px;"
    "color:#444;"
    "margin:8px 0 8px 0;"
    "padding:10px"
  "}"
  "input[type=\"text\"],input[type=\"password\"],input[type=\"number\"]{"
    "font-weight:300;"
    "width:auto;"
    "-webkit-transition:all 0.20s ease-in;"
    "-moz-transition:all 0.20s ease-in;"
    "-o-transition:all 0.20s ease-in;"
    "-ms-transition:all 0.20s ease-in;"
    "transition:all 0.20s ease-in"
  "}"
  "input[type=\"text\"]:focus,input[type=\"password\"]:focus,input[type=\"number\"]:focus{"
    "outline:none;"
    "border-color:#5C9DED;"
    "box-shadow:0 0 3px #4B8CDC"
  "}"
  "input.error,input.error:focus{"
    "border-color:#ED5564;"
    "color:#D9434E;"
    "box-shadow:0 0 3px #D9434E"
  "}"
  "input:disabled{"
    "opacity:0.6;"
    "background-color:#f7f7f7"
  "}"
  "input:disabled:hover{"
    "cursor:not-allowed"
  "}"
  "input.error::-webkit-input-placeholder,"
  "input.error::-moz-placeholder,"
  "input.error::-ms-input-placeholder{"
    "color:#D9434E"
  "}"
  ".aux-page label{"
    "display:inline;"
    "padding:10px 0.5em;"
  "}"
};
/**< Common menu bar. This style quotes LuxBar. */
/**< balzss/luxbar is licensed under the MIT License https://github.com/balzss/luxbar */
const char MOWM_CSS_LUXBAR[] PROGMEM = {
  ".lb-fixed{"
    "width:100%;"
    "position:fixed;"
    "top:0;"
    "left:0;"
    "z-index:1000;"
    "box-shadow:0 1px 3px rgba(0, 0, 0, 0.12), 0 1px 2px rgba(0, 0, 0, 0.24)"
  "}"
  ".lb-burger span,"
  ".lb-burger span::before,"
  ".lb-burger span::after{"
    "display:block;"
    "height:2px;"
    "width:26px;"
    "transition:0.6s ease"
  "}"
  ".lb-cb:checked~.lb-menu li .lb-burger span{"
    "background-color:transparent"
  "}"
  ".lb-cb:checked~.lb-menu li .lb-burger span::before,"
  ".lb-cb:checked~.lb-menu li .lb-burger span::after{"
    "margin-top:0"
  "}"
  ".lb-header{"
    "display:flex;"
    "flex-direction:row;"
    "justify-content:space-between;"
    "align-items:center;"
    "height:58px"
  "}"
  ".lb-menu-right .lb-burger{"
    "margin-left:auto"
  "}"
  ".lb-brand{"
    "font-size:1.6em;"
    "padding:18px 24px 18px 24px"
  "}"
  ".lb-menu{"
    "min-height:58px;"
    "transition:0.6s ease;"
    "width:100%"
  "}"
  ".lb-navigation{"
    "display:flex;"
    "flex-direction:column;"
    "list-style:none;"
    "padding-left:0;"
    "margin:0"
  "}"
  ".lb-menu a,"
  ".lb-item a{"
    "text-decoration:none;"
    "color:inherit;"
    "cursor:pointer"
  "}"
  ".lb-item{"
    "height:58px"
  "}"
  ".lb-item a{"
    "padding:18px 24px 18px 24px;"
    "display:block"
  "}"
  ".lb-burger{"
    "padding:18px 24px 18px 24px;"
    "position:relative;"
    "cursor:pointer"
  "}"
  ".lb-burger span::before,"
  ".lb-burger span::after{"
    "content:'';"
    "position:absolute"
  "}"
  ".lb-burger span::before{"
    "margin-top:-8px"
  "}"
  ".lb-burger span::after{"
    "margin-top:8px"
  "}"
  ".lb-cb{"
    "display:none"
  "}"
  ".lb-cb:not(:checked)~.lb-menu{"
    "overflow:hidden;"
    "height:58px"
  "}"
  ".lb-cb:checked~.lb-menu{"
    "transition:height 0.6s ease;"
    "height:100vh;"
    "overflow:auto"
  "}"
  ".dropdown{"
    "position:relative;"
    "height:auto;"
    "min-height:58px"
  "}"
  ".dropdown:hover>ul{"
    "position:relative;"
    "display:block;"
    "min-width:100%"
  "}"
  ".dropdown>a::after{"
    "position:absolute;"
    "content:'';"
    "right:10px;"
    "top:25px;"
    "border-width:5px 5px 0;"
    "border-color:transparent;"
    "border-style:solid"
  "}"
  ".dropdown>ul{"
    "display:block;"
    "overflow-x:hidden;"
    "list-style:none;"
    "padding:0"
  "}"
  ".dropdown>ul .lb-item{"
    "min-width:100%;"
    "height:29px;"
    "padding:5px 10px 5px 40px"
  "}"
  ".dropdown>ul .lb-item a{"
    "min-height:29px;"
    "line-height:29px;"
    "padding:0"
  "}"
  "@media screen and (min-width:768px){"
    ".lb-navigation{"
      "flex-flow:row;"
      "justify-content:flex-end;"
    "}"
    ".lb-burger{"
      "display:none;"
    "}"
    ".lb-cb:not(:checked)~.lb-menu{"
      "overflow:visible;"
    "}"
    ".lb-cb:checked~.lb-menu{"
      "height:58px;"
    "}"
    ".lb-menu .lb-item{"
      "border-top:0;"
    "}"
    ".lb-menu-right .lb-header{"
      "margin-right:auto;"
    "}"
    ".dropdown{"
      "height:58px;"
    "}"
    ".dropdown:hover>ul{"
      "position:absolute;"
      "left:0;"
      "top:58px;"
      "padding:0;"
    "}"
    ".dropdown>ul{"
      "display:none;"
    "}"
    ".dropdown>ul .lb-item{"
      "padding:5px 10px;"
    "}"
    ".dropdown>ul .lb-item a{"
      "white-space:nowrap;"
    "}"
  "}"
  ".lb-cb:checked+.lb-menu .lb-burger-dblspin span::before{"
    "transform:rotate(225deg)"
  "}"
  ".lb-cb:checked+.lb-menu .lb-burger-dblspin span::after{"
    "transform:rotate(-225deg)"
  "}"
  ".lb-menu-material,"
  ".lb-menu-material .dropdown ul{"
    "background-color:" AUTOCONNECT_MENUCOLOR_BACKGROUND ";"
    "color:" AUTOCONNECT_MENUCOLOR_TEXT
  "}"
  ".lb-menu-material .active,"
  ".lb-menu-material .lb-item:hover{"
    "background-color:" AUTOCONNECT_MENUCOLOR_ACTIVE
  "}"
  ".lb-menu-material .lb-burger span,"
  ".lb-menu-material .lb-burger span::before,"
  ".lb-menu-material .lb-burger span::after{"
    "background-color:" AUTOCONNECT_MENUCOLOR_TEXT
  "}"
};
/**< END OF balzss/luxbar is licensed under the MIT License https://github.com/balzss/luxbar */

static const char  MOWM_PAGE_CONFIGNEW_2[] PROGMEM = {
    "</style>"
  "</head>"
  "<body style=\"padding-top:58px;\">"
    "<div class=\"container\">"
};
/**< LuxBar menu element. {{MENU_PRE}} */
static const char  MOWM_ELM_MENU_PRE[] PROGMEM = {
  "<header id=\"lb\" class=\"lb-fixed\">"
    "<input type=\"checkbox\" class=\"lb-cb\" id=\"lb-cb\"/>"
    "<div class=\"lb-menu lb-menu-right lb-menu-material\">"
      "<ul class=\"lb-navigation\">"
        "<li class=\"lb-header\">"
          "<a href=\"" BOOT_URI "\" class=\"lb-brand\">" MENU_TITLE "</a>"
          "<label class=\"lb-burger lb-burger-dblspin\" id=\"lb-burger\" for=\"lb-cb\"><span></span></label>"
        "</li>"
//        "MENU_LIST"
};
/**< END OF LuxBar menu element. {{MENU_PRE}} */

//static const char  MOWM_ELM_MENU_AUX[] PROGMEM = {
//        "{{AUX_MENU}}"
//};

/**< LuxBar menu element. {{MENU_POST}} */
static const char  MOWM_ELM_MENU_POST[] PROGMEM = {
//        "MENU_HOME"
//        "MENU_DEVINFO"
      "</ul>"
    "</div>"
//    "<div class=\"lap\" id=\"rdlg\"><a href=\"#reset\" class=\"overlap\"></a>"
//      "<div class=\"modal_button\"><h2><a href=\"" AUTOCONNECT_URI_RESET "\" class=\"modal_button\">" AUTOCONNECT_BUTTONLABEL_RESET "</a></h2></div>"
//    "</div>"
  "</header>"
};
/**< END OF LuxBar menu element. {{MENU_POST}} */
static const char  MOWM_PAGE_CONFIGNEW_3[] PROGMEM = {
      "<div class=\"base-panel\">"
        "<form action=\"" AUTOCONNECT_URI_CONNECT "\" method=\"post\">"
          "<button style=\"width:0;height:0;padding:0;border:0;margin:0\" aria-hidden=\"true\" tabindex=\"-1\" type=\"submit\" name=\"apply\" value=\"apply\"></button>"
};
// -->          "{{LIST_SSID}}"
// -->          "<div style=\"margin:16px 0 8px 0;border-bottom:solid 1px #263238;\">" AUTOCONNECT_PAGECONFIG_TOTAL "{{SSID_COUNT}} " AUTOCONNECT_PAGECONFIG_HIDDEN "{{HIDDEN_COUNT}}</div>"
static const char  MOWM_PAGE_CONFIGNEW_4[] PROGMEM = {
          "<ul class=\"noorder\">"
            "<li>"
              "<label for=\"ssid\">" AUTOCONNECT_PAGECONFIG_SSID "</label>"
              "<input id=\"ssid\" type=\"text\" name=\"" AUTOCONNECT_PARAMID_SSID "\" placeholder=\"" AUTOCONNECT_PAGECONFIG_SSID "\">"
            "</li>"
            "<li>"
              "<label for=\"passphrase\">" AUTOCONNECT_PAGECONFIG_PASSPHRASE "</label>"
              "<input id=\"passphrase\" type=\"password\" name=\"" AUTOCONNECT_PARAMID_PASS "\" placeholder=\"" AUTOCONNECT_PAGECONFIG_PASSPHRASE "\">"
            "</li>"
//            "<li>"
//              "<label for=\"dhcp\">" AUTOCONNECT_PAGECONFIG_ENABLEDHCP "</label>"
//              "<input id=\"dhcp\" type=\"checkbox\" name=\"dhcp\" value=\"en\" checked onclick=\"vsw(this.checked);\">"
//            "</li>"
//            "{{CONFIG_IP}}"
            "<li><input type=\"submit\" name=\"apply\" value=\"" AUTOCONNECT_PAGECONFIG_APPLY "\"></li>"
          "</ul>"
        "</form>"
      "</div>"
    "</div>"
  "<script type=\"text/javascript\">"
//    "window.onload=function(){"
//      "['" AUTOCONNECT_PARAMID_STAIP "','" AUTOCONNECT_PARAMID_GTWAY "','" AUTOCONNECT_PARAMID_NTMSK "','" AUTOCONNECT_PARAMID_DNS1 "','" AUTOCONNECT_PARAMID_DNS2 "'].forEach(function(n,o,t){"
//        "io=document.getElementById(n),io.placeholder='0.0.0.0',io.pattern='^(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$'});"
//      "vsw(true)};"
    "function onFocus(e){"
      "document.getElementById('ssid').value=e,document.getElementById('passphrase').focus()"
    "}"
//    "function vsw(e){"
//      "var t;t=e?'none':'table-row';for(const n of document.getElementsByClassName('exp'))n.style.display=t,n.getElementsByTagName('input')[0].disabled=e;e||document.getElementById('sip').focus()"
//    "}"
  "</script>"
  "</body>"
  "</html>"
};



/**< SVG animation for spinner */
const char MOWM_CSS_SPINNER[] PROGMEM = {
  ".spinner{"
    "width:40px;"
    "height:40px;"
    "position:relative;"
    "margin:100px auto"
  "}"
  ".dbl-bounce1, .dbl-bounce2{"
    "width:100%;"
    "height:100%;"
    "border-radius:50%;"
    "background-color:#a3cccc;"
    "opacity:0.6;"
    "position:absolute;"
    "top:0;"
    "left:0;"
    "-webkit-animation:sk-bounce 2.0s infinite ease-in-out;"
    "animation:sk-bounce 2.0s infinite ease-in-out"
  "}"
  ".dbl-bounce2{"
    "-webkit-animation-delay:-1.0s;"
    "animation-delay:-1.0s"
  "}"
  "@-webkit-keyframes sk-bounce{"
    "0%, 100%{-webkit-transform:scale(0.0)}"
    "50%{-webkit-transform:scale(1.0)}"
  "}"
  "@keyframes sk-bounce{"
    "0%,100%{"
      "transform:scale(0.0);"
      "-webkit-transform:scale(0.0);"
    "}50%{"
      "transform:scale(1.0);"
      "-webkit-transform:scale(1.0);"
    "}"
  "}"
};

/**< TABLE style */
static const char MOWM_CSS_TABLE[] PROGMEM = {
  "table{"
    "border-collapse:collapse;"
    "border-spacing:0;"
    "border:1px solid #ddd;"
    "color:#444;"
    "background-color:#fff;"
    "margin-bottom:20px"
  "}"
  "table.info,"
  "table.info>tfoot,"
  "table.info>thead{"
    "width:100%;"
    "border-color:#5C9DED"
  "}"
  "table.info>thead{"
    "background-color:#5C9DED"
  "}"
  "table.info>thead>tr>th{"
    "color:#fff"
  "}"
  "td,"
  "th{"
    "padding:10px 22px"
  "}"
  "thead{"
    "background-color:#f3f3f3;"
    "border-bottom:1px solid #ddd"
  "}"
  "thead>tr>th{"
    "font-weight:400;"
    "text-align:left"
  "}"
  "tfoot{"
    "border-top:1px solid #ddd"
  "}"
  "tbody,"
  "tbody>tr:nth-child(odd){"
    "background-color:#fff"
  "}"
  "tbody>tr>td,"
  "tfoot>tr>td{"
    "font-weight:300;"
    "font-size:.88em"
  "}"
  "tbody>tr:nth-child(even){"
    "background-color:#f7f7f7"
  "}"
    "table.info tbody>tr:nth-child(even){"
    "background-color:#EFF5FD"
  "}"
};


#endif //#ifndef ENHANCED_PAGE