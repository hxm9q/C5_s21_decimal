#ifndef S21_HELP_DECIMAL_H
#define S21_HELP_DECIMAL_H
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "s21_decimal.h"

typedef struct {
  unsigned int bits[8];
} s21_big_decimal;

int get_bit(s21_decimal value, int index);
int get_scale(s21_decimal value);
int get_sign(s21_decimal value);
int get_big_sign(s21_big_decimal value);
void set_bit(s21_decimal *value, int index, int bit);
void set_big_bit(s21_big_decimal *value, int index, int bit);
void set_scale(s21_decimal *value, int scale);
void set_sign(s21_decimal *value);
void to_big_decimal(s21_decimal value_1, s21_big_decimal *value_2);
void to_decimal(s21_decimal *value_1, s21_big_decimal value_2);
void shift_big_right(s21_big_decimal *value, int number);
int shift_big_left(s21_big_decimal *value, int number);
void normalization(s21_big_decimal *value_1, s21_big_decimal *value_2,
                   int delta);
int scale_after_normalization(s21_big_decimal *result, int scale);
void set_zero(s21_decimal *value);
void set_big_zero(s21_big_decimal *value);
void increase_scale(s21_decimal *value, int n);
void increase_scale_big(s21_big_decimal *value, int n);
void decrease_scale_big(s21_big_decimal *value, int n);
int is_big_decimal_greater(s21_big_decimal value_1, s21_big_decimal value_2);
int is_big_decimal_greater_or_equal(s21_big_decimal value_1,
                                    s21_big_decimal value_2);
int is_decimal_empty(s21_decimal value);
int is_big_empty(s21_big_decimal value);
void find_max_bit(s21_decimal x, s21_decimal y, int *bit_1, int *bit_2);
void find_max_bit_big(s21_big_decimal x, s21_big_decimal y, int *bit_1,
                      int *bit_2);
int is_big_equal(s21_big_decimal *value_1, s21_big_decimal *value_2);

int s21_mul_big(s21_big_decimal value_1, s21_big_decimal value_2,
                s21_big_decimal *result);
void s21_add_big_decimal(s21_big_decimal value_1, s21_big_decimal value_2,
                         s21_big_decimal *result);
int s21_div_big(s21_big_decimal value_1, s21_big_decimal value_2,
                s21_big_decimal *result);
void s21_sub_big(s21_big_decimal value_1, s21_big_decimal value_2,
                 s21_big_decimal *result);

int get_big_scale(s21_big_decimal value);
int big_decimal_shift_left(s21_big_decimal *value);
int s21_mul_big_10(s21_big_decimal *result);
int s21_div_big_10(s21_big_decimal *result);
void set_big_sign(s21_big_decimal *value);
void set_big_scale(s21_big_decimal *value, int scale);
void s21_normalization(s21_big_decimal *value_1, s21_big_decimal *value_2);
int s21_all_big_zero(s21_big_decimal value);
int s21_big_is_less(s21_big_decimal value1, s21_big_decimal value2);
int s21_big_is_equal(s21_big_decimal big_value_1, s21_big_decimal big_value_2);
int bits_all_one(s21_big_decimal *bits, int index, int count_bit);
int bits_diff(s21_big_decimal *bits, int index, int count_bit);
int bits_all_zero(s21_big_decimal *bits, int index, int count_bit);
int s21_add_big(s21_big_decimal value_1, s21_big_decimal value_2,
                s21_big_decimal *result);
void to_big_decimal_comparison(s21_decimal small, s21_big_decimal *big);

int mult10DecimalSimple(s21_decimal *num, int howMany);
s21_decimal div10DecimalSimple(s21_decimal *var, int howMany);
s21_decimal divDecimalAndGetFractional(s21_decimal *res, int i, int exp);
int shiftLeft3BitDecimal(s21_decimal *num, unsigned howMany, int whatInsert);

void decimal_zeroing(s21_decimal *value);

#endif