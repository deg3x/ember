#ifndef LOGGER_H
#define LOGGER_H

#define EMBER_LOG_INFO(...) fprintf(stdout, __VA_ARGS__)
#define EMBER_LOG_ERROR(...) fprintf(stderr, __VA_ARGS__)

#endif // LOGGER_H
