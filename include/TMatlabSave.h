#ifndef TMATLABSAVE_H
#define TMATLABSAVE_H

#include "TTms32Reader.h"

class TMatlabSave
{
    public:
        static int DoSave(TTms32Reader::TTms32Signals& signals,const char* matFileName);
    protected:
    private:
};

#endif // TMATLABSAVE_H
