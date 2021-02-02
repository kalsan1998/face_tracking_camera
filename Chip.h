#ifndef __CHIP_H__
#define __CHIP_H__

// See this datasheet: http://www.nxp.com/docs/en/data-sheet/PCA9685.pdf
class Chip {
  public:
    Chip();
    ~Chip();
    void write_buffer(char *buffer, int length);

  private:
    void init();
    int i2c_fd;
};

#endif // __CHIP_H__
