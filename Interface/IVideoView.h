#ifndef IVIDEOVIEW_H
#define IVIDEOVIEW_H

#include "IObserver.h"

// struct ViewRect {
//     ViewRect(unsigned int posXIn, unsigned int posYIn, unsigned int widthIn,
//              unsigned int heightIn)
//         : posX(posXIn), posY(posYIn), width(widthIn), height(heightIn) {}

//    unsigned int posX = 0;
//    unsigned int posY = 0;
//    unsigned int width = 0;
//    unsigned int height = 0;
//};

class IVideoView : public IObserver {
  public:
    IVideoView();

    ~IVideoView(){};

    virtual bool open() = 0;

    virtual bool start() = 0;

    virtual void stop() = 0;

    virtual void initView(void *win) = 0;

    virtual void update(const std::shared_ptr<IBoData> &boData) override {}
};

#endif // IVIDEOVIEW_H
