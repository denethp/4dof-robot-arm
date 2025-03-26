#include <Adafruit_PWMServoDriver.h>
#include <Wire.h>
#include <math.h>

#define PULSE_OUT_OF_RANGE 655
#define ERR_ANG 656

#define BASE_MOTOR 0
#define LEFT_MOTOR 1
#define RIGHT_MOTOR 2

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

struct ServoData {
    String servo;
    int pin;
    int min;
    int max;
    float minA;
    float maxA;
};

ServoData servos[] = {
    {"b", BASE_MOTOR, 110, 510, -90 * M_PI / 180, 90 * M_PI / 180},
    {"l", LEFT_MOTOR, 340, 510, 80 * M_PI / 180, 155 * M_PI / 180},
    {"r", RIGHT_MOTOR, 160, 440, 130 * M_PI / 180, 10 * M_PI / 180}};

float L1 = 8;
float L2 = 8;

float x;
float y;
float z;

int s_idx;
String u_in;

void setup() {
    Serial.begin(9600);

    pwm.begin();
    pwm.setPWMFreq(50);
}

void loop() {
    u_in = Serial.readString();

    if (u_in.length() == 0)
        return;

    int s_idx1 = u_in.indexOf(' ');
    int s_idx2 = u_in.indexOf(' ', s_idx1 + 1);

    if (s_idx1 == -1 || s_idx2 == -1) {
        Serial.println("[ ERROR ] Enter three space separated values.");
        return;
    }

    x = u_in.substring(0, s_idx1).toFloat();
    y = u_in.substring(s_idx1 + 1, s_idx2).toFloat();
    z = u_in.substring(s_idx2 + 1).toFloat();

    Serial.print("X: ");
    Serial.println(x);
    Serial.print("Y: ");
    Serial.println(y);
    Serial.print("Z: ");
    Serial.println(z);

    float l_ang = get_left_angle(x, y, z, L1, L2);
    float r_ang = get_right_angle(x, y, z, L1, L2);
    float b_ang = get_base_angle(x, z);

    Serial.print("Left: ");
    Serial.println((l_ang / M_PI) * 180);
    Serial.print("Right: ");
    Serial.println((r_ang / M_PI) * 180);
    Serial.print("Base: ");
    Serial.println((b_ang / M_PI) * 180);

    move_servos(b_ang, l_ang, r_ang);
}

void move_servos(float b_ang, float l_ang, float r_ang) {
    if (b_ang == ERR_ANG || l_ang == ERR_ANG || r_ang == ERR_ANG) {
        Serial.println("[ ERROR ] Not reachable. (Angle DNE)");
    } else {
        float b_pulse = angle_to_pulse(BASE_MOTOR, b_ang);
        float l_pulse = angle_to_pulse(LEFT_MOTOR, l_ang);
        float r_pulse = angle_to_pulse(RIGHT_MOTOR, r_ang);

        if (b_pulse == PULSE_OUT_OF_RANGE || l_pulse == PULSE_OUT_OF_RANGE ||
            r_pulse == PULSE_OUT_OF_RANGE) {
            Serial.println("[ ERROR ] Not reachable. (Pulse out of range)");
        } else {
            pwm.setPWM(BASE_MOTOR, 0, b_pulse);
            delay(750);
            pwm.setPWM(LEFT_MOTOR, 0, l_pulse);
            delay(750);
            pwm.setPWM(RIGHT_MOTOR, 0, r_pulse);
        }
    }
}

float get_base_angle(float x, float z) { return atan2(z, x); }

float get_left_angle(float x, float y, float z, float L1, float L2) {
    float y_ = -y;
    float x_ = x / cos(get_base_angle(x, z));
    float R = sqrt(x_ * x_ + y_ * y_);
    float cos_a_plus_theta =
        (L1 * L1 - L2 * L2 - x_ * x_ - y_ * y_) / (2 * L2 * R);
    float a;

    if (cos_a_plus_theta > 1 || cos_a_plus_theta < -1) {
        a = ERR_ANG;
    } else {
        a = acos(cos_a_plus_theta) - atan2(y_, x_);
    }

    if (!isnan(a)) {
        return a;
    } else {
        return ERR_ANG;
    }
}

float get_right_angle(float x, float y, float z, float L1, float L2) {
    float y_ = -y;
    float x_ = x / cos(get_base_angle(x, z));
    float R = sqrt(x_ * x_ + y * y);
    float cos_b_plus_theta =
        (x_ * x_ + y_ * y_ + L1 * L1 - L2 * L2) / (2 * L1 * R);
    float b;

    if (cos_b_plus_theta > 1 || cos_b_plus_theta < -1) {
        b = ERR_ANG;
    } else {
        b = acos(cos_b_plus_theta) - atan2(y_, x_);
    }

    if (!isnan(b)) {
        return b;
    } else {
        return ERR_ANG;
    }
}

float angle_to_pulse(int pin, float angle) {
    float pulse = servos[pin].min + (((servos[pin].max - servos[pin].min) /
                                      (servos[pin].maxA - servos[pin].minA)) *
                                     (angle - servos[pin].minA));
    if (pulse < servos[pin].min || pulse > servos[pin].max) {
        pulse = PULSE_OUT_OF_RANGE;
    }
    return pulse;
}