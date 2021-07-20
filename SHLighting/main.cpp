#include <iostream>
#include "SHLighting.h"

int main()
{
    SHLighting app;

    //app.save_sh_texture();

    app.setWindowSize(800, 600);
    app.setWindowTitle("SHLight");
    app.run();

    return 0;
}
