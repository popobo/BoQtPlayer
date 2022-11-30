#ifndef BODATA_H
#define BODATA_H

class BoData {
  public:
    unsigned char *getData() const;

    int getSize() const;

    void drop();

    void setData(unsigned char *newData);

    void setSize(int newSize);

    BoData();

    BoData(const BoData &boData);

    BoData &operator=(const BoData &boData);

    bool isAudio() const;
    void setIsAudio(bool newIsAudio);

  private:
    unsigned char *m_data = nullptr;
    int m_size = 0;
    bool m_isAudio = false;
};

#endif // BODATA_H
