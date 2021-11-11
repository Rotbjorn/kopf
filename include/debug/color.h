#pragma once

#define KOPF_USE_COLORS

#ifdef KOPF_USE_COLORS

#define BOLD "\x1b[1m"

#define WHITE "\x1b[97m"
#define GRAY "\x1b[37m"
#define BLACK "\x1b[30m"
#define DARK_GRAY "\x1b[90m"

#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define BLUE "\x1b[34m"
#define PURPLE "\x1b[35m"
#define AQUA "\x1b[36m"


#define RESET "\x1b[0m"

#else

#define RED
#define GREEN
#define YELLOW
#define BLUE
#define PURPLE
#define AQUA

#define RESET
#endif
