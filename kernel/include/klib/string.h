/* SPDX-License-Identifier: GPL-2.0 */
#ifndef STRING_H
#define STRING_H

void memcpy(void *src, void *dest, int size);
void memset(void *src, unsigned char val, int size);
void strcpy(char *str1, char *str2);
int strcmp(char *str1, char *str2);



char *itoa(int num);
char *htoa(unsigned int num);
int atoi(char *str);

#endif
