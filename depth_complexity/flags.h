#ifndef FLAGS_H
#define FLAGS_H

#define cmd_usage(usage) option((char*)0, argc, argv, (char*)0, usage, false)
#define cmd_help(str) option((char*)0, argc, argv, str, (char*)0)
#define cmd_option(name, defaut, usage) option(name, argc, argv, defaut, usage)

// INTERNAL FUNCTIONS
const char* option(const char *const name, const int argc, const char *const *const argv,
                   const char *const defaut, const char *const usage, const bool reset_static);

const char* option(const char *const name, const int argc, const char *const *const argv,
                   const char *const defaut, const char *const usage=0);

bool option(const char *const name, const int argc, const char *const *const argv,
            const bool defaut, const char *const usage=0);

int option(const char *const name, const int argc, const char *const *const argv,
           const int defaut, const char *const usage=0);

char option(const char *const name, const int argc, const char *const *const argv,
            const char defaut, const char *const usage=0);

float option(const char *const name, const int argc, const char *const *const argv,
             const float defaut, const char *const usage=0);

double option(const char *const name, const int argc, const char *const *const argv,
              const double defaut, const char *const usage=0);

#endif // FLAGS_H
