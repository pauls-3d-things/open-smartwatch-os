#include "./apps/watchfaces/OswAppWatchfaceCountdown.h"

#include <OswLogger.h>
#include <gfx_util.h>
#include <osw_config.h>
#include <osw_config_keys.h>
#include <osw_hal.h>

#include "./apps/watchfaces/OswAppWatchface.h"

static void drawDaysRemaining(uint8_t years, uint8_t months, uint8_t days, uint8_t fontSize, uint8_t yOffset) {
  OswHal *hal = OswHal::getInstance();

  hal->gfx()->setTextSize(fontSize);
  hal->gfx()->setTextMiddleAligned();
  hal->gfx()->setTextLeftAligned();
  hal->gfx()->setTextCursor(120 - hal->gfx()->getTextOfsetColumns(5), yOffset);

  hal->gfx()->printDecimal(years, 3);
  hal->gfx()->print("y");
  hal->gfx()->printDecimal(months, 2);
  hal->gfx()->print("m");
  hal->gfx()->printDecimal(days, 2);
  hal->gfx()->print("d");
}

static void drawTimeRemaining(uint8_t hour, uint8_t minute, uint8_t second, uint8_t fontSize, uint8_t yOffset) {
  OswHal *hal = OswHal::getInstance();

  hal->gfx()->setTextSize(fontSize);
  hal->gfx()->setTextMiddleAligned();
  hal->gfx()->setTextLeftAligned();
  hal->gfx()->setTextCursor(120 - hal->gfx()->getTextOfsetColumns(4), yOffset);

  hal->gfx()->printDecimal(hour, 2);
  hal->gfx()->print(":");
  hal->gfx()->printDecimal(minute, 2);
  hal->gfx()->print(":");
  hal->gfx()->printDecimal(second, 2);
}

static void drawLabel(String lbl, uint8_t fontSize, uint8_t CoordY) {
  OswHal *hal = OswHal::getInstance();
  hal->gfx()->setTextSize(fontSize);
  hal->gfx()->setTextMiddleAligned();
  hal->gfx()->setTextLeftAligned();
  hal->gfx()->setTextCursor(120 - hal->gfx()->getTextOfsetColumns(lbl.length() / 2), CoordY);
  hal->gfx()->print(lbl);
}

const char *OswAppWatchfaceCountdown::getAppId() { return OswAppWatchfaceCountdown::APP_ID; }

const char *OswAppWatchfaceCountdown::getAppName() { return LANG_COUNTDOWN; }

void OswAppWatchfaceCountdown::onStart() {
  OswAppV2::onStart();
  OswAppWatchface::addButtonDefaults(this->knownButtonStates);
}

void OswAppWatchfaceCountdown::onLoop() {
  OswAppV2::onLoop();

  this->needsRedraw = this->needsRedraw or time(nullptr) != this->lastTime;  // redraw every second
}

const uint8_t daysInMonth[12] PROGMEM = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
bool isLeapYear(uint32_t year) { return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)); }
uint16_t getDaysInMonth(uint16_t year, uint16_t month) {
  uint16_t days = daysInMonth[month - 1];

  if (month == 2 && isLeapYear(year)) {
    days++;
  }

  return days;
}

void OswAppWatchfaceCountdown::onDraw() {
  OswAppV2::onDraw();

  String label = "EVENT X";
  RtcDateTime dCountDown = RtcDateTime(2050, 01, 01, 0, 0, 0);  // UTC TIME

  OswHal *hal = OswHal::getInstance();
  time_t timeZone = OswHal::getInstance()->getTimezoneOffsetPrimary();

  time_t now = hal->getUTCTime();

  RtcDateTime dNow = RtcDateTime();
  dNow.InitWithUnix64Time(now);

  // easiest case
  time_t yearsRemain = dCountDown.Year() - dNow.Year();

  int8_t deltaMonth = dCountDown.Month() - dNow.Month();
  time_t monthsRemain = deltaMonth > 0 ? deltaMonth : (12 - dNow.Month()) + dCountDown.Month();

  int8_t deltaDay = dCountDown.Day() - dNow.Day();
  time_t daysRemain =
      deltaDay > 0 ? deltaDay : (getDaysInMonth(dNow.Year(), dNow.Month()) - dNow.Day()) + dCountDown.Day();

  int8_t deltaHours = dCountDown.Hour() - dNow.Hour();
  time_t hoursRemain = deltaHours > 0 ? deltaHours : (24 - dNow.Hour()) + dCountDown.Hour();

  int8_t deltaMinutes = dCountDown.Minute() - dNow.Minute();
  time_t minutesRemain = deltaMinutes > 0 ? deltaMinutes : (60 - dNow.Minute()) + dCountDown.Minute();

  int8_t deltaSeconds = dCountDown.Second() - dNow.Second();
  time_t secondsRemain = deltaSeconds > 0 ? deltaSeconds : (60 - dNow.Second()) + dCountDown.Second();

  drawDaysRemaining(yearsRemain, monthsRemain, daysRemain, 2, 80);
  drawTimeRemaining(hoursRemain, minutesRemain, secondsRemain, 4, 120);
  drawLabel(label, 2, 160);
  this->lastTime = time(nullptr);
}

void OswAppWatchfaceCountdown::onButton(Button id, bool up, OswAppV2::ButtonStateNames state) {
  OswAppV2::onButton(id, up, state);
  if (OswAppWatchface::onButtonDefaults(*this, id, up, state))
    return;  // if the button was handled by the defaults, we are done here
}