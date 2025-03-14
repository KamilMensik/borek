#include <Borek.h>

class Test : public Borek::Application
{
public:
        Test()
        {
        }

        ~Test()
        {
        }
};

Borek::Application* Borek::CreateApplication()
{
        return new Test();
}