#pragma once

typedef int rotary_id_t;
typedef int rotary_movement_t;

typedef void (*rotary_callback_t)(rotary_id_t, rotary_movement_t);

void nl_rotary_register_callback(rotary_callback_t cb);
rotary_movement_t nl_rotary_query_resolution(rotary_id_t id);
int nl_rotary_query_number_of_rotaries();

void nl_rotary_test();
