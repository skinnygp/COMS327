#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>

int main(int argc, char *argv[])
{
  mpz_t val;
  mpz_t op1;
  mpz_t op2;
  int i;

  mpz_init(val);
  mpz_init(op1);
  mpz_init(op2);

  gmp_sscanf(argv[1], "%Zd", val);

  mpz_set_ui(op2, 1);

  if (mpz_cmp_ui(val, 1)) {
    for (i = 1; mpz_cmp_ui(val, 1); i++) {
      mpz_and(op1, val, op2);
      if (mpz_get_ui(op1)) {
        //odd
        mpz_mul_ui(val, val, 3);
        mpz_add_ui(val, val, 1);
      } else {
        //even
        mpz_tdiv_q_ui(val, val, 2);
      }
      gmp_printf("%Zd\n", val);
    }
    printf("%d\n", i - 1);
  }

  return 0;
}
