#include "common/assert_handler.h"
#include "drivers/mcu_init.h"
static void test_setup(void)
{
    mcu_init();
}
int main(void)
{
    test_setup();
    ASSERT(0);
    return 0;
}