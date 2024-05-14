#ifndef MHSERIES_H
#define MHSERIES_H
#include <stdint.h>
struct mhseries_voltages {
    uint16_t front_left;
    uint16_t front_right;
    uint16_t back_left;
    uint16_t back_right;
};
void mhseries_init(void);
void mhseries_get_voltages(struct mhseries_voltages *voltages);
#endif // MHSERIES_H