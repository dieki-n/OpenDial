const int PIN_ENC_A = 16;
const int PIN_ENC_B = 4;
class RotaryEncoder{
  int16_t last_encoder_count = 0;
  uint8_t forward_key = '[';
  uint8_t reverse_key = ']';
  public:
    void init();
    int16_t getPosition();
};
