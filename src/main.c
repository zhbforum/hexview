#include "hexview.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#if defined(_WIN32)
#include <io.h>
#define ISATTY _isatty
#define FILENO _fileno
#define FSEEK _fseeki64
#define FTELL _ftelli64
#else
#include <unistd.h>
#define ISATTY isatty
#define FILENO fileno
#define FSEEK fseeko
#define FTELL ftello
#endif

static void print_usage(const char *prog)
{
    fprintf(stderr,
            "Usage: %s <file|-> [options]\n"
            "  <file>              Path to binary file or '-' for stdin\n"
            "Options:\n"
            "  -w, --width  N      Bytes per line (default: %d, max: 64)\n"
            "  -o, --offset N      Start offset (dec or 0xHEX)\n"
            "  -l, --length N      Max bytes to read\n"
            "      --no-ascii      Do not print ASCII panel\n"
            "      --compact       Compact hex (no spaces)\n"
            "      --color[=MODE]  Colorize output: auto|always|never\n"
            "  -v, --version       Print version and exit\n",
            prog, DEFAULT_WIDTH);
}

static int stdin_is_piped(void)
{
    return !ISATTY(FILENO(stdin));
}

static int stdout_is_tty(void)
{
    return ISATTY(FILENO(stdout));
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        print_usage(argv[0]);
        return 1;
    }

    const char *path = NULL;
    hv_config_t cfg = {
        .width = DEFAULT_WIDTH,
        .offset = 0,
        .length = U64_MAX,
        .show_ascii = 1,
        .compact = 0,
        .use_color = 0,
        .offset_width = 8};

    int color_mode = 0;
    int use_stdin = 0;

    for (int i = 1; i < argc; ++i)
    {
        if (strcmp(argv[i], "-") == 0)
        {
            path = "-";
            continue;
        }
        else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0)
        {
            printf("hexview %s\n", HEXVIEW_VERSION);
            return 0;
        }
        else if (strcmp(argv[i], "--no-ascii") == 0)
            cfg.show_ascii = 0;
        else if (strcmp(argv[i], "--compact") == 0)
            cfg.compact = 1;
        else if (strncmp(argv[i], "--color", 7) == 0)
        {
            const char *val = NULL;
            if (argv[i][7] == '=')
                val = argv[i] + 8;
            else if (i + 1 < argc && argv[i + 1][0] != '-')
                val = argv[++i];
            if (!val)
                color_mode = 0;
            else if (strcmp(val, "always") == 0)
                color_mode = 1;
            else if (strcmp(val, "never") == 0)
                color_mode = 2;
            else if (strcmp(val, "auto") == 0)
                color_mode = 0;
            else
            {
                fprintf(stderr, "error: --color must be auto|always|never\n");
                return 1;
            }
        }
        else if (strcmp(argv[i], "-w") == 0 || strcmp(argv[i], "--width") == 0)
        {
            if (++i >= argc)
            {
                fprintf(stderr, "error: missing value for width\n");
                return 1;
            }
            cfg.width = atoi(argv[i]);
            if (cfg.width <= 0 || cfg.width > 64)
            {
                fprintf(stderr, "error: width must be between 1 and 64\n");
                return 1;
            }
        }
        else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--offset") == 0)
        {
            if (++i >= argc)
            {
                fprintf(stderr, "error: missing value for offset\n");
                return 1;
            }
            if (!hv_parse_u64(argv[i], &cfg.offset))
            {
                fprintf(stderr, "error: invalid offset '%s'\n", argv[i]);
                return 1;
            }
        }
        else if (strcmp(argv[i], "-l") == 0 || strcmp(argv[i], "--length") == 0)
        {
            if (++i >= argc)
            {
                fprintf(stderr, "error: missing value for length\n");
                return 1;
            }
            if (!hv_parse_u64(argv[i], &cfg.length))
            {
                fprintf(stderr, "error: invalid length '%s'\n", argv[i]);
                return 1;
            }
        }
        else if (argv[i][0] == '-')
        {
            fprintf(stderr, "error: unknown option: %s\n", argv[i]);
            print_usage(argv[0]);
            return 1;
        }
        else
        {
            path = argv[i];
        }
    }

    if (!path)
    {
        use_stdin = stdin_is_piped();
        if (!use_stdin)
        {
            fprintf(stderr, "error: file path required (or use '-' for stdin)\n");
            print_usage(argv[0]);
            return 1;
        }
    }
    else if (strcmp(path, "-") == 0)
    {
        use_stdin = 1;
    }

    if (use_stdin && cfg.offset != 0)
    {
        fprintf(stderr, "error: --offset is not supported for stdin\n");
        return 1;
    }

    int is_tty = stdout_is_tty();
    cfg.use_color = (color_mode == 1) || (color_mode == 0 && is_tty);

    if (!use_stdin)
    {
        FILE *f = fopen(path, "rb");
        if (!f)
        {
            fprintf(stderr, "error: cannot open '%s': %s\n", path, strerror(errno));
            return 1;
        }

        if (FSEEK(f, 0, SEEK_END) != 0)
        {
            fprintf(stderr, "error: seek failed: %s\n", strerror(errno));
            fclose(f);
            return 1;
        }

        long long size = FTELL(f);
        if (size < 0)
        {
            fprintf(stderr, "error: ftell failed: %s\n", strerror(errno));
            fclose(f);
            return 1;
        }

        if ((uint64_t)size <= cfg.offset)
        {
            fprintf(stderr, "note: offset 0x%llX >= file size 0x%llX\n",
                    (unsigned long long)cfg.offset, (unsigned long long)size);
            fclose(f);
            return 0;
        }

        uint64_t maxoff = (uint64_t)size - 1;
        cfg.offset_width = hv_hex_digits_u64(maxoff);
        if (cfg.offset_width < 8)
            cfg.offset_width = 8;
        if (cfg.offset_width > 16)
            cfg.offset_width = 16;

        fclose(f);
    }

    if (use_stdin)
        hv_dump_stdin(&cfg);
    else
        hv_dump_file(path, &cfg);

    return 0;
}
