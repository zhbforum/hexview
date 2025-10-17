#pragma once
#include <stdint.h>
#include <stddef.h>

#define HEXVIEW_VERSION "1.0.0"
#define DEFAULT_WIDTH 16
#define U64_MAX ((uint64_t)(~(uint64_t)0))

typedef struct
{
    int width;
    uint64_t offset;
    uint64_t length;
    int show_ascii;
    int compact;
    int use_color;
    int offset_width;
} hv_config_t;

void hv_print_line(uint64_t offset,
                   const unsigned char *buf,
                   size_t n,
                   const hv_config_t *cfg);

void hv_dump_file(const char *path, const hv_config_t *cfg);
void hv_dump_stdin(const hv_config_t *cfg);

int hv_parse_u64(const char *s, uint64_t *out);
