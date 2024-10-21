#include "s21_decimal.h"
#include "s21_help_functions.h"

int s21_floor(s21_decimal value, s21_decimal *result) {
  if (result == NULL) return 1;
  *result = value;
  int exp = get_scale(*result);
  if (exp != 0) {
    s21_decimal fractional = divDecimalAndGetFractional(result, 0, exp);
    s21_decimal zero = {{0, 0, 0, 0}};
    if (get_sign(*result) == 1 && s21_is_greater(fractional, zero)) {
      s21_decimal one = {{1, 0, 0, 0}};
      s21_add(*result, one, result);
    }
    set_scale(result, 0);
  }
  return 0;
}

int s21_round(s21_decimal value, s21_decimal *result) {
  if (result == NULL) return 1;
  *result = value;
  int exp = get_scale(*result);
  if (exp != 0) {
    s21_decimal fractional = divDecimalAndGetFractional(result, 0, exp);

    s21_decimal half = {{5, 0, 0, 0}};
    set_scale(&half, 1);
    s21_decimal one = {{1, 0, 0, 0}};
    if (s21_is_equal(half, fractional)) {
      if ((result->bits[0] & 1) == 1) s21_add(one, *result, result);
    } else if (s21_is_greater(fractional, half)) {
      s21_add(one, *result, result);
    }
    set_scale(result, 0);
  }
  return 0;
}

int s21_truncate(s21_decimal value, s21_decimal *result) {
  if (result == NULL) return 1;
  *result = value;
  div10DecimalSimple(result, get_scale(*result));
  set_scale(result, 0);
  return 0;
}

int s21_negate(s21_decimal value, s21_decimal *result) {
  if (result == NULL) return 1;
  *result = value;
  if ((result->bits[3] & 0x80000000) == 0x80000000)
    result->bits[3] ^= 0x80000000;
  else
    result->bits[3] |= 0x80000000;
  return 0;
}
