#include <iostream>
#include "PRTLighting.h"

int main()
{
    PRTLighting::ptr app = PRTLighting::getSingleton();

    app->setWindowSize(1280, 720);
    app->setWindowTitle("PRTLighting");
    app->init();
    app->run();

    return 0;
}
