#include <Borek.h>

class Test : public Borek::Application
{
public:
        Test() : Borek::Application()
        {
        }

        ~Test()
        {
        }

        void OnUpdate(float delta) override
        {
        }
};

BOREK_APPLICATION(Test)
