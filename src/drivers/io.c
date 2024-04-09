#include "drivers/io.h" //defines enum for name of register function
#include "common/defines.h" //defines UNUSED MACRO
void io_set_select (io_e io,io_select_e select) {
    UNUSED(io);
    UNUSED(select);
}
void io_set_direction(io_e io,io_dir_e direction) {
    UNUSED(io);
    UNUSED(direction);
}
void io_set_output(io_e io,io_out_e output) {
    UNUSED(io);
    UNUSED(output);
}
void io_set_resistor(io_e io,io_resistor_e resistor){
    UNUSED(io);
    UNUSED(resistor);
}
io_in_e io_get_input(io_e io) {
    UNUSED(io);
    return 0;
}