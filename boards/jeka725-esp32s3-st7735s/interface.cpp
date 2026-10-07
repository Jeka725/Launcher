#include "hal/bright/bright.h"
#include "hal/device.h"
#include "hal/inputs/buttons.h"
#include "idf/launcher_platform.h"
#include "powerSave.h"
#include <interface.h>

static constexpr int8_t BTN_LEFT = 12;
static constexpr int8_t BTN_RIGHT = 13;
static constexpr int8_t BTN_UP = 9;
static constexpr int8_t BTN_DOWN = 11;
static constexpr int8_t BTN_SELECT = 14;

static DeviceButtons buttonsCfg() {
    DeviceButtons cfg;
    cfg.btn1 = BTN_LEFT;
    cfg.btn2 = BTN_RIGHT;
    cfg.btn3 = BTN_UP;
    cfg.btn4 = BTN_DOWN;
    cfg.btn5 = BTN_SELECT;
    cfg.pullup = true;
    cfg.activeHigh = false;
    return cfg;
}

void _setup_gpio() {
    pinMode(TFT_CS, OUTPUT);
    digitalWrite(TFT_CS, HIGH);
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, LOW);
    hal_buttons_init(buttonsCfg(), 5);
}

void _post_setup_gpio() {
    hal_bright_attach(TFT_BL);
    hal_bright_set(TFT_BL, bright);
}

void _setBrightness(uint8_t brightval) {
    hal_bright_set(TFT_BL, brightval);
}

void InputHandler(void) {
    static unsigned long tm = 0;
    static bool selectWasDown = false;
    static unsigned long selectDownAt = 0;
    static bool longBackSent = false;
    static unsigned long lockoutUntil = 0;

    constexpr unsigned long debounceMs = 60;
    constexpr unsigned long backHoldMs = 2000;
    constexpr unsigned long backLockoutMs = 500;

    if (launcherMillis() - tm < debounceMs && !LongPress) return;
    tm = launcherMillis();

    checkPowerSaveTime();

    PrevPress = false;
    NextPress = false;
    UpPress = false;
    DownPress = false;
    SelPress = false;
    EscPress = false;
    AnyKeyPress = false;

    const bool left = launcherGpioRead(BTN_LEFT) == LOW;
    const bool right = launcherGpioRead(BTN_RIGHT) == LOW;
    const bool up = launcherGpioRead(BTN_UP) == LOW;
    const bool down = launcherGpioRead(BTN_DOWN) == LOW;
    const bool select = launcherGpioRead(BTN_SELECT) == LOW;

    if (!(left || right || up || down || select)) {
        if (selectWasDown) {
            const unsigned long heldMs = launcherMillis() - selectDownAt;
            selectWasDown = false;
            if (!longBackSent && launcherMillis() >= lockoutUntil && heldMs < backHoldMs) {
                SelPress = true;
                AnyKeyPress = true;
            }
            longBackSent = false;
        }
        return;
    }

    if (launcherMillis() < lockoutUntil) return;

    if (select && !selectWasDown) {
        selectWasDown = true;
        selectDownAt = launcherMillis();
        longBackSent = false;
        AnyKeyPress = true;
        if (wakeUpScreen()) return;
    }

    if (select && selectWasDown) {
        const unsigned long heldMs = launcherMillis() - selectDownAt;
        AnyKeyPress = true;
        if (heldMs >= backHoldMs && !longBackSent) {
            EscPress = true;
            longBackSent = true;
            lockoutUntil = launcherMillis() + backLockoutMs;
        }
        return;
    }

    if (selectWasDown && !select) {
        selectWasDown = false;
        const unsigned long heldMs = launcherMillis() - selectDownAt;
        if (!longBackSent && heldMs < backHoldMs) {
            SelPress = true;
            AnyKeyPress = true;
        }
        longBackSent = false;
        return;
    }

    if (left && right) {
        EscPress = true;
        AnyKeyPress = true;
        return;
    }
    if (left) PrevPress = true;
    if (right) NextPress = true;
    if (up) UpPress = true;
    if (down) DownPress = true;
    AnyKeyPress = true;
}

void powerOff() {
    esp_deep_sleep_start();
}

void reboot() {
    ESP.restart();
}
