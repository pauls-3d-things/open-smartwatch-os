#include "apps/tools/OswAppFlashLight.h"
#include "assets/img/icons/flashlight.png.h"

OswIconProgmem OswAppFlashLight::icon = OswIconProgmem(flashlight_png, flashlight_png_dimensions, rgb565(220, 220, 220));

OswAppFlashLight::OswAppFlashLight(): OswAppV2() {

}

OswAppFlashLight::~OswAppFlashLight() {

}

const char* OswAppFlashLight::getAppId() {
    return "osw.flashlight";
}

const char* OswAppFlashLight::getAppName() {
    return "Flashlight";
}

OswIcon& OswAppFlashLight::getAppIcon() {
    return icon;
}

void OswAppFlashLight::onStart() {
    OswAppV2::onStart();
    this->knownButtonStates[Button::BUTTON_SELECT] = ButtonStateNames::SHORT_PRESS;
    this->knownButtonStates[Button::BUTTON_UP] = ButtonStateNames::SHORT_PRESS;
    this->knownButtonStates[Button::BUTTON_DOWN] = ButtonStateNames::SHORT_PRESS;
}

void OswAppFlashLight::onLoop() {
    OswAppV2::onLoop();

    this->hal->setBrightness(flashlightBrightness, false); // sets the brighntess, but doesn't store it
}

void OswAppFlashLight::onDraw() {
    OswAppV2::onDraw();

    this->hal->gfx()->fillCircle(120, 120, 120, ui->getSuccessColor());
    if (this->on) {
        this->hal->gfx()->fillCircle(120, 120, 115, ui->getForegroundColor());
        this->hal->gfx()->setTextSize(3);
        this->hal->gfx()->setTextCenterAligned();
        this->hal->gfx()->setTextCursor(120, 125);
        this->hal->gfx()->setTextColor(ui->getBackgroundColor());
        this->hal->gfx()->print(int(hal->screenBrightness())); //displays the current brightness
    } else {
        this->hal->setBrightness(OswConfigAllKeys::settingDisplayBrightness.get()); //sets the brighntess to the initial value
        this->hal->gfx()->fillCircle(120, 120, 115, ui->getBackgroundColor());
        this->hal->gfx()->setTextSize(3.5);
        this->hal->gfx()->setTextCenterAligned();
        this->hal->gfx()->setTextCursor(120, 125);
        this->hal->gfx()->setTextColor(ui->getForegroundColor());
        this->hal->gfx()->print("Flashlight");
    }

}

void OswAppFlashLight::onButton(Button id, bool up, OswAppV2::ButtonStateNames state) {
    OswAppV2::onButton(id, up, state); // always make sure to call the base class method!
    if(!up) return;
    this->needsRedraw = true; // we need to redraw the screen, regardless of what happens next
    if(id == Button::BUTTON_SELECT)
        this->on = !this->on;
    else if(id == Button::BUTTON_UP) {
        this->flashlightBrightness = this->flashlightBrightness + 50;
        this->hal->setBrightness(flashlightBrightness, false);
    }
    else if(id == Button::BUTTON_DOWN) {
        this->flashlightBrightness = this->flashlightBrightness - 50;
        this->hal->setBrightness(flashlightBrightness, false);
    }
}

void OswAppFlashLight::onStop() {
    OswAppV2::onStop();
    this->hal->setBrightness(OswConfigAllKeys::settingDisplayBrightness.get(), false); // sets the brighntess to the initial value
}