#include "s21_decimal.h"
#include "s21_help_functions.h"

int s21_is_less(s21_decimal dec_1, s21_decimal dec_2) {
  s21_big_decimal value_big_first = {0}, value_big_second = {0};
  to_big_decimal_comparison(dec_1, &value_big_first);
  to_big_decimal_comparison(dec_2, &value_big_second);
  int result = s21_big_is_less(value_big_first, value_big_second);
  return result;
}

int s21_is_not_equal(s21_decimal dec_1, s21_decimal dec_2) {
  return (!s21_is_equal(dec_1, dec_2));
}

int s21_is_less_or_equal(s21_decimal dec_1, s21_decimal dec_2) {
  int result;
  if (s21_is_less(dec_1, dec_2) || s21_is_equal(dec_1, dec_2))
    result = 1;
  else
    result = 0;
  return result;
}
int s21_is_greater(s21_decimal dec_1, s21_decimal dec_2) {
  int result;
  if (s21_is_less(dec_2, dec_1))
    result = 1;
  else
    result = 0;
  return result;
}
int s21_is_greater_or_equal(s21_decimal dec_1, s21_decimal dec_2) {
  int result;
  if (s21_is_less(dec_2, dec_1) || s21_is_equal(dec_1, dec_2))
    result = 1;
  else
    result = 0;
  return result;
}
int s21_is_equal(s21_decimal dec_1, s21_decimal dec_2) {
  s21_big_decimal value_big_first = {0}, value_big_second = {0};
  to_big_decimal_comparison(dec_1, &value_big_first);
  to_big_decimal_comparison(dec_2, &value_big_second);
  int result = s21_big_is_equal(value_big_first, value_big_second);
  return result;
}
