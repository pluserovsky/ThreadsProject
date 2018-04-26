#include <iostream>

class Pitstop
{

    int id;
    bool available = true;

  public:
    bool getAvailable();
    void setAvailable(bool _available);
    Pitstop();
    Pitstop(int _id);
    int getID();
};
