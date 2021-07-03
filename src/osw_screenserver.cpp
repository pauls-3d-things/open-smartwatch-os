#include "./osw_screenserver.h"

#include <WebServer.h>
#include <services/OswServiceTaskWiFi.h>
#include <services/OswServiceTasks.h>

#include "osw_hal.h"

// this tool can not be implemented as a background service, as we need to
// block further screenwrites to get an atomic screenshot
#ifdef RAW_SCREEN_SERVER

WebServer* rawScreenServer;
Graphics2DPrint* rawScreenGfx;

void handleRawScreen(void) {
  long contentLength = DISP_W * DISP_H * 3;
  uint8_t buf[3 * DISP_W];
  // curl: (27) Rejected 102977 bytes header (max is 102400)!
  //
  rawScreenServer->client().write("HTTP/1.1 200 OK");
  rawScreenServer->client().write((String("\r\nContent-Length: ") + String(contentLength)).c_str());
  rawScreenServer->client().write("\r\nContent-Type: application/octet-stream");
  rawScreenServer->client().write("\r\nConnection: close");
  rawScreenServer->client().write("\r\n\r\n");  // empty line for header<->body delimiter
  Serial.println("ost-rss: header");
  for (int y = 0; y < DISP_H; y++) {
    for (int x = 0; x < DISP_W; x++) {
      uint16_t rgb = rawScreenGfx->getPixel(x, y);
      buf[x * 3 + 0] = rgb565_red(rgb);
      buf[x * 3 + 1] = rgb565_green(rgb);
      buf[x * 3 + 2] = rgb565_blue(rgb);
    }
    rawScreenServer->client().write(buf, 3 * DISP_W);
    yield();
    Serial.print("y: ");
    Serial.println(y);
  }
  Serial.println("ost-rss: done");
}

void screenserver_setup(OswHal* hal) {
  if (!OswServiceAllTasks::wifi.isConnected()) {
    OswServiceAllTasks::wifi.connectWiFi();
  }
  rawScreenGfx = hal->gfx();
}

void screenserver_loop(OswHal* hal) {
  if (!rawScreenServer && OswServiceAllTasks::wifi.isConnected()) {
    rawScreenServer = new WebServer(8080);
    rawScreenServer->on("/screen.raw", handleRawScreen);
    rawScreenServer->begin();
#ifdef DEBUG
    Serial.println("Started Raw ScreenServer");
    Serial.print("http://");
    Serial.print(OswServiceAllTasks::wifi.getIP().toString());
    Serial.println(":8080/screen.raw");
#endif
  }

  if (rawScreenServer) {
    rawScreenServer->handleClient();
  }
}
#endif
