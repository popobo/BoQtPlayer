#ifndef IBODATA_H
#define IBODATA_H

#include <vector>

class IBoData {
  public:
    IBoData();

    virtual ~IBoData();

    virtual bool alloc(int size, const uint8_t *dataIn) { return false; }

    virtual void drop() = 0;

    virtual void *structDataPtr() const;
    virtual void setStructDataPtr(void *newStructDataPtr);

    virtual uint8_t *data() const;
    virtual void setData(uint8_t *newData);

    virtual std::vector<uint8_t *> datas() const;
    virtual void addDatas(uint8_t *data);
    virtual void setDatas(const std::vector<uint8_t *> &datas);

    virtual int width() const;
    virtual void setWidth(int newWidth);

    virtual int height() const;
    virtual void setHeight(int newHeight);

    virtual int format() const;
    virtual void setFormat(int newFormat);

    virtual bool isAudio() const;
    virtual void setIsAudio(bool newIsAudio);

    virtual int size() const;
    virtual void setSize(int newSize);

    virtual void copyBasicAttributes(const IBoData &boData);

    int64_t pts() const;
    void setPts(int64_t newPts);

    double timeBase() const;
    void setTimeBase(double newTimeBase);

  protected:
    // 存储(char*)AVPacket, (char*)AVFrame等结构体数据的指针
    void *m_structDataPtr{nullptr};
    // IBoData本身alloc的数据
    uint8_t *m_data{nullptr};
    // 存储AVFrame中的帧数据
    // unsigned char *m_datas[AV_NUM_DATA_POINTERS]{nullptr};
    std::vector<uint8_t *> m_datas;
    // 数据大小，指AVPacket中包的大小，AVFrame的帧数据的大小，IBoData
    // alloc的数据大小
    int m_size{0};
    int m_width{0};
    int m_height{0};
    int m_format{0};
    int64_t m_pts{0};
    double m_timeBase{0};
    bool m_isAudio{false};
};

#endif // IBODATA_H
