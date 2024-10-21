#include "s21_decimal.h"
#include "s21_help_functions.h"

int s21_from_int_to_decimal(int src, s21_decimal *dst) {
  unsigned int mask = 2147483648;
  mask = src & mask;
  decimal_zeroing(dst);
  if (src < 0) src *= -1;
  dst->bits[0] = src;
  if (mask) set_sign(dst);
  return 0;
}
int s21_from_float_to_decimal(float src, s21_decimal *dst) {
  if (isinf(src) || isnan(src)) {
    return 1;
  }
  decimal_zeroing(dst);
  int sign = 0;
  if (src < 0) {
    sign = 1;
    src = -src;
  }
  long long int integerPart = (long long int)src;
  int exp = 0;
  while (src - ((float)integerPart / (long long int)(pow(10, exp))) != 0) {
    exp++;
    integerPart = src * (long long int)(pow(10, exp));
  }
  s21_from_int_to_decimal((int)integerPart, dst);
  if (sign) {
    s21_negate(*dst, dst);
  }
  dst->bits[3] += exp << 16;
  return 0;
}

int s21_from_decimal_to_int(s21_decimal src, int *dst) {
  int error = 0;
  s21_truncate(src, &src);
  int scale = get_scale(src);
  if (src.bits[2] == 0 && src.bits[1] == 0 && dst) {
    *dst = src.bits[0];
    if (get_sign(src)) {
      *dst *= -1;
    }
    for (int i = 0; i < scale; i++) {
      *dst /= 10;
    }
  } else {
    error = 1;
  }
  return error;
}

int s21_from_decimal_to_float(s21_decimal src, float *dst) {
  double decimalValue = 0;
  if (dst == NULL || isnan(decimalValue)) {
    return 1;
  }
  unsigned int i = 0;
  double multiplier = 1.0;
  while (i < 96) {
    if ((src.bits[i / 32] & (1U << i % 32)) != 0) {
      decimalValue += multiplier;
    }
    i++;
    multiplier *= 2.0;
  }
  int off = (src.bits[3] & ~0x80000000) >> 16;
  while (off > 0) {
    decimalValue /= 10.0;
    off--;
  }
  *dst = (float)decimalValue;
  if (src.bits[3] & 0x80000000) {
    *dst = -(*dst);
  }
  return 0;
}