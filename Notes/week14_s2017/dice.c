#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <gmp.h>

typedef struct die {
  unsigned sides;
  unsigned value;
  unsigned *roll_count;
} die_t;

unsigned num_factorials;
mpz_t *factorials;

/*
  1,
  1,
  2,
  6,
  24,
  120,
  720,
  5040,
  40320,
  362880,
  3628800,
  39916800,
  479001600,
  6227020800,
  87178291200,
  1307674368000,
  20922789888000,
  355687428096000,
  6402373705728000,
  121645100408832000,
  2432902008176640000,
*/

void get_factorial(mpz_t *f, unsigned n)
{
  unsigned start;
  unsigned i;

  if (num_factorials == 0) {
    num_factorials = n > 128 ? n + 1 : 128;
    factorials = calloc(num_factorials, sizeof (*factorials));

    mpz_init(factorials[0]);
    mpz_set_ui(factorials[0], 1);
    mpz_init(factorials[1]);
    mpz_set_ui(factorials[1], 1);
    for (i = 2; i < num_factorials; i++) {
      mpz_init(factorials[i]);
      mpz_mul_ui(factorials[i], factorials[i - 1], i);
    }
  }

  if (n >= num_factorials) {
    start = num_factorials;
    while (num_factorials <= n) {
      num_factorials *= 2;
    }
    factorials = realloc(factorials, num_factorials * sizeof (*factorials));
    for (i = start; i < num_factorials; i++) {
      mpz_init(factorials[i]);
      mpz_mul_ui(factorials[i], factorials[i - 1], i);
    }
  }

  mpz_set(*f, factorials[n]);
}

void choose(mpz_t *val, unsigned n, unsigned m)
{
  static mpz_t t1, t2, t3;
  static unsigned initialized;

  if (!initialized) {
    mpz_init(t1);
    mpz_init(t2);
    mpz_init(t3);
  }

  get_factorial(&t1, m);
  get_factorial(&t2, n - m);

  mpz_mul(t3, t2, t1);
  get_factorial(&t2, n);
  mpz_cdiv_q(*val, t2, t3);
}

void die_probability(unsigned dice, unsigned sides, unsigned value,
                     mpz_t *numerator, mpz_t *denominator)
{
  unsigned i;
  mpz_t sum, t1, t2, t3;

  mpz_init(sum);
  mpz_init(t1);
  mpz_init(t2);
  mpz_init(t3);

  for (mpz_set_ui(sum, 0), i = 0; i <= ((value - dice) / sides); i++) {
    choose(&t1, dice, i);
    choose(&t2, value - (sides * i) - 1, dice - 1);
    mpz_mul_si(t3, t2, i & 0x1 ? -1 : 1);
    mpz_mul(t2, t3, t1);
    mpz_set(t3, sum);
    mpz_add(sum, t2, t3);
      /*
    sum += (choose(dice, i) *
            choose(value - (sides * i) - 1, dice - 1) *
            (i & 0x1 ? -1 : 1));
      */
  }

  mpz_set(*numerator, sum);
  mpz_ui_pow_ui(*denominator, sides, dice);

  mpz_clear(t3);
  mpz_clear(t2);
  mpz_clear(t1);
  mpz_clear(sum);
}

void init_die(die_t *die, unsigned sides)
{
  die->sides = sides;
  die->roll_count = calloc(sides, sizeof (*die->roll_count));
}

unsigned roll(die_t *die)
{
  while ((die->value = random()) && (die->value + die->sides) < die->sides)
    ;

  die->value = (die->value % die->sides) + 1;
  die->roll_count[die->value]++;

  return die->value;
}

void print_expected_distribution(unsigned dice, unsigned sides)
{
  unsigned roll_counts[dice * sides + 1];
  unsigned die[dice];
  unsigned inc;
  unsigned i, j;
  unsigned sum;
  unsigned logmaxsum;
  unsigned rolls;
  mpz_t numerator, denominator;
  double r;

  mpz_init(numerator);
  mpz_init(denominator);

  for (i = dice; i <= sides * dice; i++) {
    roll_counts[i] = 0;
  }
  for (j = 0; j < dice; j++) {
    die[j] = 1;
  }

  rolls = (r = pow(sides, dice));
  if (r > UINT_MAX) {
    fprintf(stderr, "Sorry\n");
    exit(1);
  }

  for (inc = i = 0; i < rolls; i++) {
    for (sum = j = 0; j < dice; j++) {
      /*      printf("%u ", die[j]);*/
      sum += die[j]; 
    }
    /*    printf("= %u\n", sum);*/
    roll_counts[sum]++;
    if (++die[0] > sides) {
      inc = 1;
    }
    for (j = 1; inc && j < dice; j++) {
      die[j - 1] = 1;
      if (++die[j] != sides + 1) {
        inc = 0;
      }
    }
  }

  logmaxsum = log10(sides * dice) + 1;

  for (i = dice; i <= sides * dice; i++) {
    printf("%u\n", roll_counts[i]);
  }

  printf("Theoretical Distribution:\n");
  for (i = dice, j = 0; i <= dice * sides; i++) {
    sum += (i * roll_counts[i]);
    j += roll_counts[i];
    printf("%*u %4.1f %5.1f: %*s", logmaxsum, i,
           100.0 * (((float) roll_counts[i]) / ((float) rolls)),
           100.0 * (((float) j) / ((float) rolls)),
           (int) ((((float) roll_counts[i]) /
                   ((float) roll_counts[(int) ((((float) sides) + 1.0) *
                                               ((float) dice) / 2.0)])) *
                  (80.0 - (logmaxsum + 12))),
           ">\n");
  }

  printf("Theoretical Mean:   %f\n",
         (((float) sides) + 1.0) * ((float) dice) / 2.0);
  if ((((float) sides) + 1.0) * ((float) dice) / 2.0 ==
      floor((((float) sides) + 1.0) * ((float) dice) / 2.0)) {
    printf("Theoretical Median: %d\n",
           (int) ((((float) sides) + 1.0) * ((float) dice) / 2.0));
    printf("Theoretical Mode:   %d\n",
           (int) ((((float) sides) + 1.0) * ((float) dice) / 2.0));
  } else {
    printf("Theoretical Median: %d and %d\n",
           (int) ((((float) sides) + 1.0) * ((float) dice) / 2.0),
           (int) ((((float) sides) + 1.0) * ((float) dice) / 2.0 + 1));
    printf("Theoretical Mode:   %d and %d\n\n",
           (int) ((((float) sides) + 1.0) * ((float) dice) / 2.0),
           (int) ((((float) sides) + 1.0) * ((float) dice) / 2.0 + 1));
  }

  for (i = dice; i <= sides * dice; i++) {


    die_probability(dice, sides, i,
                    &numerator, &denominator);
    gmp_printf("%Zd/%Zd ", numerator, denominator);
  }
  printf("\n");
}

int main(int argc, char *argv[])
{
  unsigned sides, dice, rolls;
  unsigned i, j;
  die_t *dieptr;
  unsigned *roll_counts;
  unsigned sum = 0;
  unsigned logrolls, logmaxsum;
  unsigned maxcount;
  unsigned mode, median;

  if (argc != 4) {
    fprintf(stderr, "usage: %s <dice> <sides> <rolls>\n", argv[0]);
    exit(1);
  }

  srandom(time(NULL));

  dice  = atoi(argv[1]);
  sides = atoi(argv[2]);
  rolls = atoi(argv[3]);

  print_expected_distribution(dice, sides);

  dieptr = calloc(dice, sizeof (*dieptr));

  for (j = 0; j < dice; j++) {
    init_die(&dieptr[j], sides);
  }

  roll_counts = calloc(dice * sides + 1, sizeof (*roll_counts));

  for (i = 0; i < rolls; i++) {
    for (j = sum = 0; j < dice; j++) {
      sum += roll(&dieptr[j]);
    }
    roll_counts[sum]++;
  }

  logrolls = log10(rolls) + 1;
  logmaxsum = log10(sides * dice) + 1;

  for (maxcount = 0, i = dice; i <= dice * sides; i++) {
    if (roll_counts[i] > roll_counts[maxcount]) {
      maxcount = i;
    }
  }
  mode = maxcount;
  maxcount = roll_counts[maxcount];

  printf("Actual Distribution:\n");
  for (i = dice, j = median = 0; i <= dice * sides; i++) {
    sum += (i * roll_counts[i]);
    j += roll_counts[i];
    printf("%*u %*u %4.1f %5.1f: %*s", logmaxsum, i, logrolls, roll_counts[i],
           100.0 * (((float) roll_counts[i]) / ((float) rolls)),
           100.0 * (((float) j) / ((float) rolls)),
           (int) ((((float) roll_counts[i]) / ((float) maxcount)) *
                  (80.0 - (logrolls + logmaxsum + 13))),
           ">\n");
    if (!median && j >= rolls / 2) {
      median = i;
    }
  }

  printf("Actual Mean:   %f\n", ((float) sum) / ((float) rolls));
  printf("Actual Median: %d\n", median);
  printf("Actual Mode:   %d\n", mode);

  return 0;
}
