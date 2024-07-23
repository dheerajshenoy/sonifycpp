#include "sonify.hpp"

int main (int argc, char *argv[]) {
    QApplication a(argc, argv);
    Sonify s;
    a.exec();
    return 0;
}
