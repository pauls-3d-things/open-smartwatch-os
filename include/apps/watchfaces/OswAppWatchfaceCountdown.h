#pragma once

#include <OswAppV2.h>
#include <osw_hal.h>
#include <osw_ui.h>

class OswAppWatchfaceCountdown : public OswAppV2 {
 public:
  constexpr static const char* APP_ID = "osw.wf.cdown";

  const char* getAppId() override;
  const char* getAppName() override;

  void onStart() override;
  void onLoop() override;
  void onDraw() override;
  void onButton(Button id, bool up, ButtonStateNames state) override;

 private:
  static uint8_t dateFormatCache;
  time_t lastTime = 0;
};