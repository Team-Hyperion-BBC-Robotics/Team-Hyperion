#include <Kicker.h>

/**
 * @brief Initialise the kicker hardware interfaces and internal timers.
 *
 * This function configures the kicker output pin, initialises the voltage
 * divider used for capacitor monitoring, and synchronises both timing systems
 * responsible for pulse duration and recharge lockout.
 *
 * @note This must be called before any attempt to fire the kicker.
 */
void Kicker::init() {
    pinMode(KICKER_PIN, OUTPUT);
    digitalWrite(KICKER_PIN, LOW);

    kickerVoltage.init();
    pulseDuration.update();
    rechargeDelay.update();
}

/**
 * @brief Executes one update cycle of the kicker state machine.
 *
 * This function handles both **ending an existing kick pulse** and
 * **initiating a new one** if safety constraints allow.
 *
 * @note This function should be called frequently inside the main control loop.
 */
void Kicker::fire() {
    kickerVoltage.update();

    // ------ END KICK WHEN TIME IS UP ------
    if (pulseActive && pulseDuration.time_has_passed_no_update()) {
        pulseActive = false;
        digitalWrite(KICKER_PIN, LOW);
        rechargeDelay.update();   // cooldown starts after pulse ends
        return;
    }

    // ------ KICK CONDITIONS ------
    if (!pulseActive && (kickerVoltage.get_lvl() >= KICKER_REQUIRED_VOLT) && 
        rechargeDelay.time_has_passed_no_update()) {
        digitalWrite(KICKER_PIN, HIGH);
        pulseActive = true;
        pulseDuration.update();  // start pulse timer
    }
}
