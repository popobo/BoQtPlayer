#ifndef BODATA_H
#define BODATA_H

struct BoData {
    unsigned char *data = nullptr;
    int size = 0;
    void drop();
};

#endif // BODATA_H
