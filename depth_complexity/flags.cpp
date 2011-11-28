#include "flags.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

const char* option(const char *const name, const int argc, const char *const *const argv,
                   const char *const defaut, const char *const usage, const bool reset_static) {
  static bool first = true, visu = false;
  if (reset_static) { first = true; return 0; }
  const char *res = 0;
  if (first) {
    first = false;
    visu = option("-h", argc, argv, (char*)0, (char*)0, false) != 0;
    visu |= option("-help", argc, argv, (char*)0, (char*)0, false) != 0;
    visu |= option("--help", argc, argv, (char*)0, (char*)0, false) != 0;
  }
  if (!name && visu) {
    if (usage) {
      std::printf("\n %s", argv[0]);
      std::printf(" : %s",usage);
      std::printf(" (%s, %s)\n\n",__DATE__,__TIME__);
    }
    if (defaut)
      std::printf("%s\n",defaut);
  }
  if (name) {
    if (argc>0) {
      int k = 0;
      while (k<argc && std::strcmp(argv[k],name)) ++k;
      res = (k++==argc? defaut: (k==argc? argv[--k]: argv[k]));
    } else {
      res = defaut;
    }
    if (visu && usage)
      std::printf("    %-16s = %-24s %s\n", name, res?res:"0", usage);
    //    std::printf("    %s%-16s%s %-24s %s%s%s\n",
    //                cimg::t_bold,name,cimg::t_normal,res?res:"0",cimg::t_green,usage,cimg::t_normal);
  }
  return res;
}

const char* option(const char *const name, const int argc, const char *const *const argv,
                   const char *const defaut, const char *const usage) {
  return option(name, argc, argv, defaut, usage, false);
}

bool option(const char *const name, const int argc, const char *const *const argv,
            const bool defaut, const char *const usage) {
  const char *const s = option(name,argc,argv,(char*)0);
  const bool res = s?(strcmp(s,"false") && strcmp(s,"off") && strcmp(s,"0")):defaut;
  option(name,0,0,res?"true":"false",usage);
  return res;
}

int option(const char *const name, const int argc, const char *const *const argv,
           const int defaut, const char *const usage) {
  const char *const s = option(name, argc, argv, (char* )0);
  const int res = s? std::atoi(s): defaut;
  char tmp[256] = { 0 };
  snprintf(tmp, sizeof(tmp), "%d", res);
  option(name, 0, 0, tmp, usage);
  return res;
}

char option(const char *const name, const int argc, const char *const *const argv,
            const char defaut, const char *const usage) {
  const char *const s = option(name,argc,argv,(char*)0);
  const char res = s?*s:defaut;
  char tmp[8] = { 0 };
  *tmp = res;
  option(name,0,0,tmp,usage);
  return res;
}

float option(const char *const name, const int argc, const char *const *const argv,
             const float defaut, const char *const usage) {
  const char *const s = option(name,argc,argv,(char*)0);
  const float res = s?(float)atof(s):defaut;
  char tmp[256] = { 0 };
  snprintf(tmp,sizeof(tmp),"%g",res);
  option(name,0,0,tmp,usage);
  return res;
}

double option(const char *const name, const int argc, const char *const *const argv,
              const double defaut, const char *const usage) {
  const char *const s = option(name,argc,argv,(char*)0);
  const double res = s?atof(s):defaut;
  char tmp[256] = { 0 };
  snprintf(tmp,sizeof(tmp),"%g",res);
  option(name,0,0,tmp,usage);
  return res;
}
