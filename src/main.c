#include "common/assert_handler.h"
#include "drivers/mcu_init.h"
SUPPRESS_UNUSED
static void test_setup(void)
{
    mcu_init();
}
int main(void)
{
    ASSERT(0);
    return 0;
}