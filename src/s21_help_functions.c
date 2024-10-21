#include "s21_help_functions.h"

#include "s21_decimal.h"

int get_bit(s21_decimal value, int index) {
  int mask = 1u << (index % 32);
  return (value.bits[index / 32] & mask) != 0;
}

int get_big_bit(s21_big_decimal value, int index) {
  int mask = 1u << (index % 32);
  return (value.bits[index / 32] & mask) != 0;
}

int get_scale(s21_decimal value) {
  int mask = 127 << 16;
  int scale = (mask & value.bits[3]) >> 16;
  return scale;
}

int get_sign(s21_decimal value) { return (value.bits[3] & 1u << 31) != 0; }

int get_big_sign(s21_big_decimal value) {
  int sign = value.bits[6] & 2147483648;
  if (sign)
    return 1;
  else
    return 0;
}

void set_bit(s21_decimal *value, int index, int bit) {
  int mask = 1u << (index % 32);
  if (bit == 0)
    value->bits[index / 32] = value->bits[index / 32] & ~mask;
  else
    value->bits[index / 32] = value->bits[index / 32] | mask;
}

void set_big_bit(s21_big_decimal *value, int index, int bit) {
  int mask = 1u << (index % 32);
  if (bit == 0)
    value->bits[index / 32] = value->bits[index / 32] & ~mask;
  else
    value->bits[index / 32] = value->bits[index / 32] | mask;
}

void set_scale(s21_decimal *value, int scale) {
  int sign = get_sign(*value);
  value->bits[3] = 0;
  scale <<= 16;
  value->bits[3] = scale | value->bits[3];
  if (sign) set_sign(value);
}

void set_sign(s21_decimal *value) {
  value->bits[3] = value->bits[3] | 1u << 31;
}

void to_big_decimal(s21_decimal value_1, s21_big_decimal *value_2) {
  value_2->bits[0] = value_1.bits[0];
  value_2->bits[1] = value_1.bits[1];
  value_2->bits[2] = value_1.bits[2];
}

void to_decimal(s21_decimal *value_1, s21_big_decimal value_2) {
  value_1->bits[0] = value_2.bits[0];
  value_1->bits[1] = value_2.bits[1];
  value_1->bits[2] = value_2.bits[2];
}

void shift_big_right(s21_big_decimal *value, int number) {
  int buffer[7] = {0};
  int i = 0;
  while (i < number) {
    int j = 0;
    while (j < 7) {
      buffer[j] = get_big_bit(*value, (j + 1) * 32);
      j++;
    }
    j = 0;
    while (j < 7) {
      value->bits[j] >>= 1;
      set_big_bit(value, (j + 1) * 32 - 1, buffer[j]);
      j++;
    }
    value->bits[7] >>= 1;
    i++;
  }
}

int shift_big_left(s21_big_decimal *value, int number) {
  int error = 0;
  int buffer[8] = {0};
  int i = 0;
  while (i < number) {
    int j = 0;
    while (j < 7) {
      buffer[j] = get_big_bit(*value, (j + 1) * 32 - 1);
      j++;
    }
    j = 7;
    while (j > 0 && !error) {
      if (get_big_bit(*value, 255)) error = 1;
      value->bits[j] <<= 1;
      set_big_bit(value, j * 32, buffer[j - 1]);
      j--;
    }
    value->bits[0] <<= 1;
    i++;
  }
  return error;
}

void normalization(s21_big_decimal *value_1, s21_big_decimal *value_2,
                   int delta) {
  if (delta > 0) {
    increase_scale_big(value_2, delta);
  } else if (delta < 0) {
    increase_scale_big(value_1, -delta);
  }
}

int scale_after_normalization(s21_big_decimal *result, int scale) {
  int dop = 0;
  while ((result->bits[3] || result->bits[4] || result->bits[5] ||
          result->bits[6] || result->bits[7]) &&
         scale > 0) {
    if (scale == 1 && result->bits[3]) dop = 1;
    decrease_scale_big(result, 1);
    scale--;
  }
  if (dop && scale == 0 && result->bits[3] == 0 && get_big_bit(*result, 0))
    set_big_bit(result, 0, 0);
  if ((result->bits[3] || result->bits[4] || result->bits[5] ||
       result->bits[6] || result->bits[7]))
    scale = -1;
  return scale;
}

void set_zero(s21_decimal *value) {
  value->bits[0] = value->bits[1] = value->bits[2] = value->bits[3] = 0;
}

void set_big_zero(s21_big_decimal *value) {
  value->bits[0] = value->bits[1] = value->bits[2] = value->bits[3] =
      value->bits[4] = value->bits[5] = value->bits[6] = value->bits[7] = 0;
}

void increase_scale(s21_decimal *value, int n) {
  s21_decimal tmp = {0}, ten = {{10, 0, 0, 0}};
  int scale = get_scale(*value);
  int i = 0;
  while (i < n) {
    s21_mul(*value, ten, &tmp);
    *value = tmp;
    set_zero(&tmp);
    i++;
  }
  set_scale(value, scale + n);
}

void increase_scale_big(s21_big_decimal *value, int n) {
  s21_big_decimal ten = {{10, 0, 0, 0, 0, 0, 0, 0}}, tmp = {0};
  int i = 0;
  while (i < n) {
    s21_mul_big(*value, ten, &tmp);
    *value = tmp;
    set_big_zero(&tmp);
    i++;
  }
}

void decrease_scale_big(s21_big_decimal *value, int n) {
  s21_big_decimal ten = {{10, 0, 0, 0, 0, 0, 0, 0}}, tmp = {0};
  int i = 0;
  while (i < n) {
    s21_div_big(*value, ten, &tmp);
    *value = tmp;
    set_big_zero(&tmp);
    i++;
  }
}

int is_big_decimal_greater(s21_big_decimal value_1, s21_big_decimal value_2) {
  int result = 0, out = 0;
  int i = 7;
  while (i >= 0 && !result && !out) {
    if (value_1.bits[i] || value_2.bits[i]) {
      if (value_1.bits[i] > value_2.bits[i]) {
        result = 1;
      }
      if (value_1.bits[i] != value_2.bits[i]) out = 1;
    }
    i--;
  }
  return result;
}

int is_big_decimal_greater_or_equal(s21_big_decimal value_1,
                                    s21_big_decimal value_2) {
  int result = 0, out = 0;
  int i = 7;
  while (i >= 0 && !out && !result) {
    if (value_1.bits[i] != 0 || value_2.bits[i] != 0) {
      if (value_1.bits[i] >= value_2.bits[i]) {
        result = 1;
      }
      out = 1;
    }
    i--;
  }
  return result;
}

int is_decimal_empty(s21_decimal value) {
  return value.bits[0] + value.bits[1] + value.bits[2];
}

int is_big_decimal_empty(s21_big_decimal value) {
  return value.bits[0] + value.bits[1] + value.bits[2] + value.bits[3] +
         value.bits[4] + value.bits[5] + value.bits[6] + value.bits[7];
}

void find_max_bit(s21_decimal x, s21_decimal y, int *bit_1, int *bit_2) {
  for (int i = 95; i >= 0 && (!(*bit_1) || !(*bit_2)); i--) {
    if (*bit_1 == 0 && get_bit(x, i)) *bit_1 = i;
    if (*bit_2 == 0 && get_bit(y, i)) *bit_2 = i;
  }
}

void find_max_bit_big(s21_big_decimal x, s21_big_decimal y, int *bit_1,
                      int *bit_2) {
  for (int i = 255; i >= 0 && (!(*bit_1) || !(*bit_2)); i--) {
    if (*bit_1 == 0 && get_big_bit(x, i)) *bit_1 = i;
    if (*bit_2 == 0 && get_big_bit(y, i)) *bit_2 = i;
  }
}

int is_big_equal(s21_big_decimal *value_1, s21_big_decimal *value_2) {
  int scale = 0;
  while (is_big_decimal_greater(*value_2, *value_1)) {
    increase_scale_big(value_1, 1);
    scale++;
  }
  while (is_big_decimal_greater_or_equal(*value_1, *value_2)) {
    shift_big_left(value_2, 1);
  }
  shift_big_right(value_2, 1);
  return scale;
}

void s21_add_big_decimal(s21_big_decimal value_1, s21_big_decimal value_2,
                         s21_big_decimal *result) {
  int res = 0, overflow = 0;
  for (int i = 0; i < 256; i++) {
    res = get_big_bit(value_1, i) + get_big_bit(value_2, i) + overflow;
    overflow = res / 2;
    set_big_bit(result, i, res % 2);
  }
}

void s21_sub_big(s21_big_decimal value_1, s21_big_decimal value_2,
                 s21_big_decimal *result) {
  int tmp = 0, res = 0;
  for (int i = 0; i < 256; i++) {
    res = get_big_bit(value_1, i) - get_big_bit(value_2, i) - tmp;
    tmp = res < 0;
    res = abs(res);
    set_big_bit(result, i, res % 2);
  }
}

int s21_mul_big(s21_big_decimal value_1, s21_big_decimal value_2,
                s21_big_decimal *result) {
  int error = 0, count = 0;
  for (int i = 0; i < 256 && !error; i++) {
    if (get_big_bit(value_2, i)) {
      error = shift_big_left(&value_1, i - count);
      s21_add_big(value_1, *result, result);
      count = i;
    }
  }
  return error;
}

int s21_div_big(s21_big_decimal value_1, s21_big_decimal value_2,
                s21_big_decimal *result) {
  int b_1 = 0, b_2 = 0, bit_2 = 0, scale = 0, delta = 0, save_scale = 0;
  s21_big_decimal tmp = {0};
  find_max_bit_big(value_1, value_2, &b_1, &b_2);
  bit_2 = b_2;
  int i = 0;
  while (i < 96 && is_big_decimal_empty(value_1)) {
    if (i > 0) {
      shift_big_left(&value_2, 1);
      increase_scale_big(result, 1);
      increase_scale_big(&value_1, 1);
      save_scale++;
    }
    scale = is_big_equal(&value_1, &value_2);
    save_scale += scale;
    b_1 = b_2 = 0;
    find_max_bit_big(value_1, value_2, &b_1, &b_2);
    delta = b_2 - bit_2;
    if (delta < 0) delta = 0;
    while (delta >= 0 && is_big_decimal_empty(value_1)) {
      if (is_big_decimal_greater(value_2, value_1)) {
        set_big_bit(&tmp, 0, 0);
      } else {
        s21_sub_big(value_1, value_2, &value_1);
        set_big_bit(&tmp, 0, 1);
      }
      i++;
      delta--;
      if (delta >= 0) shift_big_right(&value_2, 1);
      shift_big_left(&tmp, 1);
    }
    if (delta >= 0) shift_big_left(&tmp, delta + 1);
    shift_big_right(&tmp, 1);
    s21_add_big(*result, tmp, result);
    set_big_zero(&tmp);
    i++;
  }
  return save_scale;
}

int get_big_scale(s21_big_decimal value) {
  int mask = 255 << 16;
  int scale = (value.bits[6] & mask) >> 16;
  return scale;
}

int big_decimal_shift_left(s21_big_decimal *value) {
  int bit_31 = get_big_bit(*value, 31);
  int bit_63 = get_big_bit(*value, 63);
  int bit_95 = get_big_bit(*value, 95);
  int bit_127 = get_big_bit(*value, 127);
  int bit_159 = get_big_bit(*value, 159);
  int bit_191 = get_big_bit(*value, 191);
  value->bits[0] = value->bits[0] << 1;
  value->bits[1] = value->bits[1] << 1;
  value->bits[2] = value->bits[2] << 1;
  value->bits[3] = value->bits[3] << 1;
  value->bits[4] = value->bits[4] << 1;
  value->bits[5] = value->bits[5] << 1;
  if (bit_31) set_big_bit(value, 32, 1);
  if (bit_63) set_big_bit(value, 64, 1);
  if (bit_95) set_big_bit(value, 96, 1);
  if (bit_127) set_big_bit(value, 128, 1);
  if (bit_159) set_big_bit(value, 160, 1);
  return bit_191;
}

int s21_mul_big_10(s21_big_decimal *result) {
  s21_big_decimal res_cpy;
  res_cpy = *result;
  int error = 0;
  result->bits[0] = 0b0;
  result->bits[1] = 0b0;
  result->bits[2] = 0b0;
  result->bits[3] = 0b0;
  result->bits[4] = 0b0;
  result->bits[5] = 0b0;
  for (int i = 0; i < 192; ++i) {
    if (get_big_bit(res_cpy, i)) {
      s21_big_decimal mask = {{10, 0, 0, 0, 0, 0, res_cpy.bits[6]}};
      for (int j = 0; j < i; j++) {
        big_decimal_shift_left(&mask);
      }
      error = s21_add_big(*result, mask, result);
      if (error) i = 192;
    }
  }
  return error;
}
int s21_div_big_10(s21_big_decimal *result) {
  int remainder = 0;
  for (int i = 191; i >= 0; --i) {
    int bit = get_big_bit(*result, i);
    int res = (bit + remainder * 2) / 10;
    remainder = (bit + remainder * 2) % 10;
    set_big_bit(result, i, res);
  }
  return remainder;
}

void set_big_sign(s21_big_decimal *value) {
  value->bits[6] = value->bits[6] | 2147483648;
}

void set_big_scale(s21_big_decimal *value, int scale) {
  int sign = get_big_sign(*value);
  value->bits[6] = 0;
  int mask = scale << 16;
  value->bits[6] = value->bits[6] | mask;
  if (sign) set_big_sign(value);
}

void s21_normalization(s21_big_decimal *value_1, s21_big_decimal *value_2) {
  int scale_1 = get_big_scale(*value_1);
  int scale_2 = get_big_scale(*value_2);
  int sign_1 = get_big_sign(*value_1);
  int sign_2 = get_big_sign(*value_2);
  if (scale_1 > 28) {
    int diff_1 = scale_1 - 28;
    for (int i = 0; i < diff_1; i++) {
      s21_div_big_10(value_1);
    }
    scale_1 = 28;
    set_big_scale(value_1, 28);
  }
  if (scale_2 > 28) {
    int diff_2 = scale_2 - 28;
    for (int i = 0; i < diff_2; i++) {
      s21_div_big_10(value_2);
    }
    scale_2 = 28;
    set_big_scale(value_2, 28);
  }

  if (scale_1 > scale_2) {
    int diff = scale_1 - scale_2;
    set_big_scale(value_2, scale_1);
    for (int i = 0; i < diff; i++) {
      s21_mul_big_10(value_2);
    }
  } else {
    int diff = scale_2 - scale_1;
    set_big_scale(value_1, scale_2);
    for (int i = 0; i < diff; i++) {
      s21_mul_big_10(value_1);
    }
  }
  if (sign_1) set_big_sign(value_1);
  if (sign_2) set_big_sign(value_2);
}

int s21_all_big_zero(s21_big_decimal value) {
  int zero = 0;
  for (int i = 0; i < 6; ++i) {
    if (value.bits[i] == 0) zero++;
  }
  return zero == 6 ? 1 : 0;
}

int s21_big_is_less(s21_big_decimal value1, s21_big_decimal value2) {
  int result = 0;
  int sign_value1 = get_big_sign(value1);
  int sign_value2 = get_big_sign(value2);
  if (!(s21_all_big_zero(value1) && s21_all_big_zero(value2))) {
    s21_normalization(&value1, &value2);
    if (sign_value1 && sign_value2 == 0) result = 1;
    if (sign_value1 && sign_value2) {
      set_big_bit(&value1, 223, 0);
      set_big_bit(&value2, 223, 0);
      result = s21_big_is_less(value2, value1);
    }
    if (sign_value1 == 0 && sign_value2 == 0) {
      for (int i = 191; 0 <= i; i--) {
        int bit_value1 = get_big_bit(value1, i);
        int bit_value2 = get_big_bit(value2, i);
        if (bit_value1 == 0 && bit_value2) {
          result = 1;
          i = 0;
        }
        if (bit_value1 && bit_value2 == 0) {
          result = 0;
          i = 0;
        }
      }
    }
  }
  return result;
}

int s21_big_is_equal(s21_big_decimal big_value_1, s21_big_decimal big_value_2) {
  int mean = 0;
  if (s21_all_big_zero(big_value_1) && s21_all_big_zero(big_value_2)) {
    mean = 1;
  }
  s21_normalization(&big_value_1, &big_value_2);
  if (big_value_1.bits[0] == big_value_2.bits[0] &&
      big_value_1.bits[1] == big_value_2.bits[1] &&
      big_value_1.bits[2] == big_value_2.bits[2] &&
      big_value_1.bits[3] == big_value_2.bits[3] &&
      big_value_1.bits[4] == big_value_2.bits[4] &&
      big_value_1.bits[5] == big_value_2.bits[5] &&
      get_big_sign(big_value_1) == get_big_sign(big_value_2)) {
    mean = 1;
  }
  return mean;
}

int bits_all_one(s21_big_decimal *bits, int index, int count_bit) {
  if (count_bit == 0) {
    set_big_bit(bits, index, 0);
    count_bit = 1;
  } else {
    set_big_bit(bits, index, 1);
  }
  return count_bit;
}

int bits_diff(s21_big_decimal *bits, int index, int count_bit) {
  if (count_bit == 0) {
    set_big_bit(bits, index, 1);
  } else {
    set_big_bit(bits, index, 0);
  }
  return count_bit;
}

int bits_all_zero(s21_big_decimal *bits, int index, int count_bit) {
  if (count_bit == 0) {
    set_big_bit(bits, index, 0);
  } else {
    set_big_bit(bits, index, 1);
  }
  return 0;
}

int s21_add_big(s21_big_decimal value_1, s21_big_decimal value_2,
                s21_big_decimal *result) {
  int error = 0, index = 0, count_bit = 0;
  for (; index <= 191; ++index) {
    int count_1 = get_big_bit(value_1, index);
    int count_2 = get_big_bit(value_2, index);
    if (count_1 && count_2) {
      count_bit = bits_all_one(result, index, count_bit);
    }
    if ((count_1 == 0 && count_2) || (count_1 && count_2 == 0)) {
      count_bit = bits_diff(result, index, count_bit);
    }
    if (count_2 == 0 && count_1 == 0) {
      count_bit = bits_all_zero(result, index, count_bit);
    }
  }
  if (count_bit && index == 192) {
    error = 1;
    count_bit = 0;
  }
  if (count_bit) {
    bits_all_one(result, index, count_bit);
  }
  set_big_scale(result, get_big_scale(value_1));
  return error;
}

void to_big_decimal_comparison(s21_decimal small, s21_big_decimal *big) {
  big->bits[0] = small.bits[0];
  big->bits[1] = small.bits[1];
  big->bits[2] = small.bits[2];
  big->bits[3] = 0;
  big->bits[4] = 0;
  big->bits[5] = 0;
  big->bits[6] = small.bits[3];
}

int mult10DecimalSimple(s21_decimal *num, int howMany) {
  s21_decimal numCpy = *num;
  int flag = 0;
  int i = 0;
  for (; flag == 0 && i < howMany; i++) {
    if ((numCpy.bits[2] & 0x80000000)) {
      flag = 1;
    } else if (numCpy.bits[2] & (0x80000000 >> 1)) {
      flag = 1;
    } else if (numCpy.bits[2] & (0x80000000 >> 2)) {
      flag = 1;
    } else {
      shiftLeft3BitDecimal(&numCpy, 3, 0);
      flag = s21_add(*num, numCpy, &numCpy);
    }
    if (flag == 0) *num = numCpy;
  }
  return 0;
}

s21_decimal div10DecimalSimple(s21_decimal *var, int howMany) {
  unsigned long temp = 0;
  for (int i = 0; i < howMany; i++) {
    temp = (*var).bits[2];
    (*var).bits[2] /= 10;

    temp = temp % 10;
    temp <<= 32;
    temp += (*var).bits[1];
    (*var).bits[1] = (unsigned)(temp / 10);

    temp %= 10;
    temp <<= 32;
    temp += (*var).bits[0];
    (*var).bits[0] = (unsigned)(temp / 10);
  }
  s21_decimal fraction = {{temp % 10, 0, 0, 0}};
  return fraction;
}

s21_decimal divDecimalAndGetFractional(s21_decimal *res, int i, int exp) {
  int j = i;
  s21_decimal fraction = {{0, 0, 0, 0}};
  if (exp != 0) {
    fraction = div10DecimalSimple(res, 1);
    s21_add(fraction, divDecimalAndGetFractional(res, ++i, exp - 1), &fraction);
  }
  if (j != 0)
    mult10DecimalSimple(&fraction, 1);
  else
    set_scale(&fraction, exp);
  return fraction;
}

int shiftLeft3BitDecimal(s21_decimal *num, unsigned howMany, int whatInsert) {
  int ret = 0;
  for (unsigned i = 0; i < howMany && ret == 0; i++) {
    if ((num->bits[2] & 0x80000000) == 0x80000000) ret = 1;

    num->bits[2] <<= 1;
    if ((num->bits[1] & 0x80000000) == 0x80000000) num->bits[2] |= 1;
    num->bits[1] <<= 1;
    if ((num->bits[0] & 0x80000000) == 0x80000000) num->bits[1] |= 1;
    num->bits[0] <<= 1;
    num->bits[0] |= whatInsert;
  }
  return ret;
}

void decimal_zeroing(s21_decimal *value) {
  value->bits[0] = 0;
  value->bits[1] = 0;
  value->bits[2] = 0;
  value->bits[3] = 0;
}
