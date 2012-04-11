// mingw32-make
#include "header.h"

int main(int argc, char *argv[])
{
     QApplication app(argc, argv);
     app.setFont(QFont("Times", 12, QFont::Bold));
     Header h;
    
     h.show();
     return app.exec();
}

