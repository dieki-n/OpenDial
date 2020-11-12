#define KEY_UNPRESS_EVENT 1
#define KEY_PRESS_EVENT   2

const int COL_PINS = 3;
const int ROW_PINS = 4;
const int PIN_ROW[4] = {32, 33, 25, 26};
const int PIN_COL[3] = {23, 14, 12};


class KeyboardMatrix {
  bool initialized = false;
                       
  public:
    int key_pressed[4][3] = {};  
    void scan(int *buff);
    void init();
};
