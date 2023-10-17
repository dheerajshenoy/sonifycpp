#ifndef __MAIN_CPP
#define __MAIN_CPP

class sonify;
sonify *s;

#include "sonify.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    s = new sonify();
    s->show();
    app.exec();
}

#endif
