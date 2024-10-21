#include "s21_decimal.h"
#include "s21_help_functions.h"

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int error = 0, get = 0, result_sign = 0;
  set_zero(result);
  if (get_sign(value_1) && get_sign(value_2)) result_sign = 1;

  if (get_sign(value_1) != get_sign(value_2)) {
    int sign = 0;
    sign = get_sign(value_1);
    set_bit(&value_1, 127, 0);
    set_bit(&value_2, 127, 0);
    if (sign) {
      error = s21_sub(value_2, value_1, result);
    } else {
      error = s21_sub(value_1, value_2, result);
    }
  } else {
    s21_big_decimal x = {0}, y = {0}, r = {0};
    int scale = 0;
    to_big_decimal(value_1, &x);
    to_big_decimal(value_2, &y);
    int delta = get_scale(value_1) - get_scale(value_2);
    if (delta > 0) {
      get = get_scale(value_1);
      set_scale(&value_2, get);
    } else {
      get = get_scale(value_2);
      set_scale(&value_1, get);
    }
    normalization(&x, &y, delta);
    s21_add_big_decimal(x, y, &r);
    scale = scale_after_normalization(&r, get_scale(value_1));
    if (scale >= 0) {
      to_decimal(result, r);
      set_scale(result, scale);
    } else {
      error = 1;
    }
  }
  if (result_sign == 1) set_sign(result);

  if (error == 1 && get_sign(*result)) error = 2;
  if (error) set_zero(result);
  return error;
}

int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int error = 0, scale = 0, result_sign = 0;
  set_zero(result);
  if (get_sign(value_1) && get_sign(value_2)) {
    s21_decimal temp_1 = value_1;
    value_1 = value_2;
    value_2 = temp_1;
    set_bit(&value_1, 127, 0);
    set_bit(&value_2, 127, 0);
  }
  if (get_sign(value_1) != get_sign(value_2)) {
    if (get_sign(value_1)) result_sign = 1;
    set_bit(&value_1, 127, 0);
    set_bit(&value_2, 127, 0);
    error = s21_add(value_1, value_2, result);
  } else {
    s21_big_decimal x = {0}, y = {0}, r = {0};
    to_big_decimal(value_1, &x);
    to_big_decimal(value_2, &y);
    int delta = get_scale(value_1) - get_scale(value_2);
    delta > 0 ? set_scale(&value_2, get_scale(value_2) + delta)
              : set_scale(&value_1, get_scale(value_1) - delta);
    normalization(&x, &y, delta);
    if (is_big_decimal_greater(y, x)) {
      s21_big_decimal temp_2 = x;
      x = y;
      y = temp_2;
      set_sign(result);
    }
    s21_sub_big(x, y, &r);
    scale = scale_after_normalization(&r, get_scale(value_1));
    if (scale >= 0) {
      to_decimal(result, r);
      set_scale(result, scale);
    } else {
      error = 1;
    }
  }
  if (result_sign == 1) set_sign(result);

  if (error == 1 && get_sign(*result)) error = 2;
  if (error) set_zero(result);
  return error;
}

int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int error = 0, scale = 0;
  s21_big_decimal x = {0}, y = {0}, r = {0};
  to_big_decimal(value_1, &x);
  to_big_decimal(value_2, &y);
  if (get_sign(value_1) != get_sign(value_2)) set_sign(result);
  scale = get_scale(value_1) + get_scale(value_2);
  error = s21_mul_big(x, y, &r);
  scale = scale_after_normalization(&r, scale);
  if (scale >= 0) {
    set_scale(result, scale);
    to_decimal(result, r);
  } else {
    error = 1;
  }
  if (error == 1 && get_sign(*result)) error = 2;
  if (error) set_zero(result);
  return error;
}

int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  s21_decimal result_init = {0};
  int error = 0;
  *result = result_init;
  if (is_decimal_empty(value_2)) {
    int scale = 0, res_scale = 0;
    s21_big_decimal x = {0}, y = {0}, r = {0};
    to_big_decimal(value_1, &x);
    to_big_decimal(value_2, &y);
    if (get_sign(value_1) != get_sign(value_2)) set_sign(result);
    scale = s21_div_big(x, y, &r);
    set_scale(&value_1, get_scale(value_1) + scale);
    res_scale = get_scale(value_1) - get_scale(value_2);
    if (res_scale > 0) {
      res_scale = scale_after_normalization(&r, res_scale);
    } else if (res_scale < 0) {
      increase_scale_big(&r, abs(res_scale));
      res_scale = scale_after_normalization(&r, 0);
    }
    if (res_scale >= 0) {
      to_decimal(result, r);
      set_scale(result, res_scale);
    } else {
      error = 1;
    }
  } else {
    error = 3;
  }
  if (error == 1 && get_sign(*result)) error = 2;
  if (error) set_zero(result);
  return error;
}