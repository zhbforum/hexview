#pragma once
#include <stdint.h>

int hv_hex_digits_u64(uint64_t v);
int hv_parse_u64(const char *s, uint64_t *out);
