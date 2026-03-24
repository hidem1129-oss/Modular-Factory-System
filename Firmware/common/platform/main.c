#include <stdio.h>
#include "pico/stdlib.h"
#include "node_core.h"
#include "node_app.h"

int main(void) {
    stdio_init_all();
    node_core_init(node_app_get_callbacks());

    uint8_t tick10 = 0;
    while (true) {
        node_core_poll_1ms();

        tick10++;
        if (tick10 >= 10) {
            tick10 = 0;
            node_core_tick_10ms();
        }

        sleep_ms(1);
    }
}
