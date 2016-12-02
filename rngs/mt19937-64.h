#ifndef MT19937_64_H_
#define MT19937_64_H_

void init_genrand64(unsigned long long seed);

/* generates a random number on [0, 2^64-1]-interval */
unsigned long long genrand64_int64(void);

/* generates a random number on [0, 2^63-1]-interval */
long long genrand64_int63(void);

/* generates a random number on [0,1]-real-interval */
double genrand64_real1(void);

/* generates a random number on [0,1)-real-interval */
double genrand64_real2(void);

/* generates a random number on (0,1)-real-interval */
double genrand64_real3(void);

#endif
