static uint16_t fans[6][2] = {
  {A0, 11},
  {A1, 10},
  {A2, 9},
  {A3, 6},
  {A4, 5},
  {A5, 3},
};
static float matrix[6][7] = {
  {0.40, 0.05, 0.05, 0.07, 0.07, 0.02, -0.04},
  {0.05, 0.40, 0.05, 0.07, 0.07, 0.02, -0.04},
  {0.10, 0.15, 0.50, 0.04, 0.02, 0.00, -0.12},
  {0.05, 0.07, 0.05, 0.45, 0.05, 0.05, -0.25},
  {0.05, 0.07, 0.05, 0.05, 0.45, 0.05, -0.25},
  {0.05, 0.07, 0.05, 0.05, 0.05, 0.45, -0.25},
};
static float min_fan[6] = {0.16, 0.17, 0.17, 0.18, 0.18, 0.18};
float fan_val[6];
// Sample factor * 100 is the sample time.
static int sample_factor = 15;
static uint8_t ave_n = 10;
static uint16_t min_v = 645, max_v = 715;
float volt = 677.0;

void setup() {
  uint8_t i;
  for (i = 0; i < 6; i++) {
    pinMode(fans[i][0], INPUT);
    pinMode(fans[i][1], OUTPUT);
  }
  Serial.begin(115200);
  Serial.println("HP fan proxy");

}

void readPWM(int pin, float* pwm, uint16_t* v) {
  uint32_t total = 0;
  uint16_t ana_in;
  *pwm = 0;
  for (uint8_t i = 0; i < sample_factor; i++){
    for (uint8_t j = 0; j < 100; j++) {
      ana_in = analogRead(pin);
      // Keeping the on voltage.
      if (ana_in > *v) *v = ana_in;
      total += ana_in;
    }
    *pwm += total / 100.0 / sample_factor;
    total = 0;
  }
  // Reversed PWM
  *pwm = (volt-*pwm)/volt;
  return ;
}

void update_volt(uint16_t v) {
  // Prevent wrong reading
  if (v < min_v || v > max_v) return;
  volt += (v - volt) / ave_n;
}

void loop() {
  float out;
  uint8_t pwm_out;
  uint16_t v = 0;
  for (uint8_t i = 0; i < 6; i++) {
    fan_val[i] = out = 0;
    readPWM(fans[i][0], &fan_val[i], &v);
    // Compute using matrix and previous data
    for (uint8_t j = 0; j < 6; j++) {
      out += matrix[i][j] * fan_val[j];
    }
    out += matrix[i][6];
    // Ensure minimal fan speed
    if (out < min_fan[i]) {
      out = min_fan[i];
    }
    pwm_out = 255 - out * 255;
    analogWrite(fans[i][1], pwm_out);
    Serial.print("Fan["); Serial.print(i+1); Serial.print("] iLO:"); Serial.print(fan_val[i] * 100); Serial.print("% Out:"); Serial.print(out*100); Serial.print("% Diff:"); Serial.print((fan_val[i] - out) * 100); Serial.println("%");
  }
  update_volt(v);
  Serial.print("Voltage: ");Serial.print(volt / 204.6);Serial.println("V");
  Serial.println("---");
}
