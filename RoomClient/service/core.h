#pragma once

namespace vi {

class Core
{
public:
    Core();

    static void init();

    static void destroy();

private:
    static void registerServices();
};

}
