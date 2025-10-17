#include "hexview.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>

static void hv_print_hex_byte(unsigned char b, int use_color)
{
    if (use_color && b == 0x00)
        printf("\x1b[90m");
    printf("%02X", b);
    if (use_color && b == 0x00)
        printf("\x1b[0m");
}

void hv_print_line(uint64_t offset,
                   const unsigned char *buf,
                   size_t n,
                   const hv_config_t *cfg)
{
    printf("%0*llX: ", cfg->offset_width, (unsigned long long)offset);

    for (int i = 0; i < cfg->width; ++i)
    {
        if ((size_t)i < n)
            hv_print_hex_byte(buf[i], cfg->use_color);
        else
            printf("  ");
        if (!cfg->compact && i != cfg->width - 1)
            printf(" ");
    }

    if (cfg->show_ascii)
    {
        printf("  |");
        for (size_t i = 0; i < n; ++i)
        {
            unsigned char c = buf[i];
            printf("%c", isprint(c) ? (char)c : '.');
        }
        for (int i = (int)n; i < cfg->width; ++i)
            printf(" ");
        printf("|");
    }

    printf("\n");
}

#if defined(_WIN32)
#define FSEEK _fseeki64
#else
#define _FILE_OFFSET_BITS 64
#define FSEEK fseeko
#endif

static void hv_dump_stream(FILE *f, uint64_t start, uint64_t maxlen, const hv_config_t *cfg)
{
    unsigned char *buf = (unsigned char *)malloc((size_t)cfg->width);
    if (!buf)
    {
        fprintf(stderr, "error: out of memory\n");
        return;
    }

    if (!f)
    {
        free(buf);
        return;
    }

    if (start > 0)
    {
        if (FSEEK(f, (long long)start, SEEK_SET) != 0)
        {
            fprintf(stderr, "error: seek to 0x%llX failed\n", (unsigned long long)start);
            free(buf);
            return;
        }
    }

    uint64_t printed_offset = start;
    uint64_t remaining = maxlen;

    for (;;)
    {
        size_t toread = (size_t)cfg->width;
        if (remaining != U64_MAX)
        {
            if (remaining == 0)
                break;
            if (remaining < (uint64_t)cfg->width)
                toread = (size_t)remaining;
        }

        size_t n = fread(buf, 1, toread, f);
        if (n == 0)
        {
            if (ferror(f))
                fprintf(stderr, "error: read failed\n");
            break;
        }

        hv_print_line(printed_offset, buf, n, cfg);
        printed_offset += (uint64_t)n;

        if (remaining != U64_MAX)
        {
            if (remaining <= (uint64_t)n)
                break;
            remaining -= (uint64_t)n;
        }
    }

    free(buf);
}

void hv_dump_file(const char *path, const hv_config_t *cfg)
{
    FILE *f = fopen(path, "rb");
    if (!f)
    {
        fprintf(stderr, "error: cannot open '%s': %s\n", path, strerror(errno));
        return;
    }
    hv_dump_stream(f, cfg->offset, cfg->length, cfg);
    fclose(f);
}

void hv_dump_stdin(const hv_config_t *cfg)
{
    hv_dump_stream(stdin, 0, cfg->length, cfg);
}