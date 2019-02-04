#define portOfPin(P)\
  (((P)>=0&&(P)<8)?&PORTD:(((P)>7&&(P)<14)?&PORTB:&PORTC))
#define ddrOfPin(P)\
  (((P)>=0&&(P)<8)?&DDRD:(((P)>7&&(P)<14)?&DDRB:&DDRC))
#define pinOfPin(P)\
  (((P)>=0&&(P)<8)?&PIND:(((P)>7&&(P)<14)?&PINB:&PINC))
#define pinIndex(P)((uint8_t)(P>13?P-14:P&7))
#define pinMask(P)((uint8_t)(1<<pinIndex(P)))

#define pinAsInput(P) *(ddrOfPin(P))&=~pinMask(P)
#define pinAsInputPullUp(P) *(ddrOfPin(P))&=~pinMask(P);digitalHigh(P)
#define pinAsOutput(P) *(ddrOfPin(P))|=pinMask(P)
#define digitalLow(P) *(portOfPin(P))&=~pinMask(P)
#define digitalHigh(P) *(portOfPin(P))|=pinMask(P)
#define isHigh(P)((*(pinOfPin(P))& pinMask(P))>0)
#define isLow(P)((*(pinOfPin(P))& pinMask(P))==0)
#define digitalState(P)((uint8_t)isHigh(P))

#define fixTime(t)((uint16_t)t*64)

const uint8_t fans[6][2] = {
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
const int sample = 25000;

void setup() {
  uint8_t i;
  for (i = 0; i < 6; i++) {
    pinAsInput(fans[i][0]);
    pinAsOutput(fans[i][1]);
  }
  // Set to 31.25kHz to reach 25kHz requirement
  TCCR0B = _BV(CS00);
  TCCR1B = _BV(CS00);
  TCCR2B = _BV(CS00);
  TCCR0A = _BV(COM0A1) | _BV(COM0B1) | _BV(WGM00); 
  Serial.begin(115200);
  Serial.println("HP fan proxy");

}

float readPWM(int pin) {
  uint32_t total = 0;
  uint32_t low = 0;
  for (uint16_t i = 0; i < sample; i++) {
    low += isLow(pin);
    total ++;
  }
  // Inversed PWM
  return (float) low / total;
}

void update_volt(uint16_t v) {
  // Prevent wrong reading
  if (v < min_v || v > max_v) return;
  volt += (v - volt) / ave_n;
}

void loop() {
  float out;
  uint8_t pwm_out;
  for (uint8_t i = 0; i < 6; i++) {
    fan_val[i] = readPWM(fans[i][0]);
  }
  for (uint8_t i = 0; i < 6; i++) {
    out = 0;
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
    Serial.print("Fan["); Serial.print(i + 1); Serial.print("] iLO:"); Serial.print(fan_val[i] * 100); Serial.print("% Out:"); Serial.print(out * 100); Serial.print("% Diff:"); Serial.print((fan_val[i] - out) * 100); Serial.println("%");
  }
  Serial.println("---");
  delay(fixTime(1000));
}

