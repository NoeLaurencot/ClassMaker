#ifndef SCAN_INPUT_H
#define SCAN_INPUT_H

int scan_unsigned_int(void);

int scan_min_max_int(int n_min, int n_max);

int scan_n_unsigned_int(int n_max);

int scan_int(void);

int scan_boolean(void);

char *scan_string(int size);

#endif
