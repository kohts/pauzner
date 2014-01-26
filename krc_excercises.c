#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <errno.h>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <string.h>

#include <limits.h>
#include <float.h>
#include <math.h>

#include <ctype.h>

#define LOWER 0
#define UPPER 120
#define STEP 15

float fahrenheit_to_celcius (int F) {
  return (5.0 / 9.0) * (F - 32.0) ;
}

float celsius_to_fahrenheit (int C) {
  return (9.0 * C / 5.0 + 32.0);
}

int power_local (int base, int n) {
  int total = 1;
  int i;

  for(i = 1; i <= n; ++i)
    total *= base;

  return total;
}

int is_true(int i) {
  if (i) {
    printf("%d: is true\n", i);
  }
  else {
    printf("%d: is false\n", i);
  }
}

#define TRUE 1
#define FALSE 0
#define LENGTHS_SIZE 128

#define MAXLINE 1000 /* maximum input line length */
#define BINARRAY 10000
#define TABSIZE 4

#define MAXOP 100 /* max size of operand or operator */
#define NUMBER '0' /* "signal" that a number was found */
#define MATH_FUNCTION '1'
#define MAXVAL 100 /* maximum depth of val stack */
#define BUFSIZE 100

double val[MAXVAL]; /* value stack */
int sp = 0; /* next free stack position */

char buf[BUFSIZE]; /* buffer for ungetch */
int bufp = 0; /* next free position in buf */

char math_function[BUFSIZE]; /* buffer for math functions */

int DEBUG = FALSE;
void debug(char * msg) {
  if (DEBUG)
    printf("%s\n", msg);
}

int getline_local(char line[], int maxline, int return_truncated_length);
void copy(char to[], char from[]);
void reverse(char line_in[], char line_out[]);

int calc_getop(char []);
void calc_push(double);
double calc_pop(void);
int getch(void);
void ungetch(int);
void ungets(char [], int);


// Exercise 5-7. Rewrite readlines to store lines in an array supplied by main, rather than
// calling alloc to maintain storage. How much faster is the program?

#define MAXLINES 5000 /* max #lines to be sorted */
#define MAXLEN 1000 /* max length of any input line */
#define ALLOCSIZE 10000 /* size of available space */

static char allocbuf[ALLOCSIZE]; /* storage for alloc */
static char *allocp = allocbuf; /* next free position */

char *lineptr[MAXLINES];

/* return pointer to n characters */
char *alloc(int n) {
  if (allocbuf + ALLOCSIZE - allocp >= n) { /* it fits */
    allocp += n;
    return allocp - n; /* old p */
  }
  else /* not enough room */
    return 0;
}

void afree(char *p) { /* free storage pointed to by p */
  if (p >= allocbuf && p < allocbuf + ALLOCSIZE)
    allocp = p;
}

/* swap: interchange v[i] and v[j] */
void swap_char(char *v[], int i, int j) {
  char *temp;
  temp = v[i];
  v[i] = v[j];
  v[j] = temp;
}

/* qsort: sort v[left]...v[right] into increasing order */
void qsort_local(char *v[], int left, int right) {
  int i, last;
  
  if (left >= right) /* do nothing if array contains */
    return; /* fewer than two elements */

  swap_char(v, left, (left + right)/2);
  last = left;

  for (i = left+1; i <= right; i++)
    if (strcmp(v[i], v[left]) < 0)
      swap_char(v, ++last, i);
  
  swap_char(v, left, last);
  qsort_local(v, left, last-1);
  qsort_local(v, last+1, right);
}

/* writelines: write output lines */
void writelines(char *lineptr[], int nlines) {
  int i;
  for (i = 0; i < nlines; i++)
    printf("%s\n", lineptr[i]);
}

/* readlines: read input lines */
int readlines(char *lineptr[], int maxlines) {
  int len, nlines;
  char *p, line[MAXLEN];
  nlines = 0;

  while ((len = getline_local(line, MAXLEN, 1)) > 0)
    if (nlines >= maxlines || (p = alloc(len)) == NULL)
      return -1;
    else {
      line[len-1] = '\0'; /* delete newline */
      strcpy(p, line);
      lineptr[nlines++] = p;
    }
  
  return nlines;
}

int readlines2(char static_array[][MAXLEN], int maxlines) {
  int len, nlines;
  nlines = 0;

  while ((len = getline_local(static_array[nlines++], MAXLEN, 1)) > 0)
    if (nlines >= maxlines)
      return -1;
    else {
      static_array[nlines++][len - 1] = '\0';
    }
  
  return nlines;
}

int main5_7 (int arg, char *argv[]) {
  int nlines; /* number of input lines read */

  char lines[MAXLINES][MAXLEN];

  if ((nlines = readlines(lineptr, MAXLINES)) >= 0) {
    printf("----\n");
    qsort_local(lineptr, 0, nlines-1);
    writelines(lineptr, nlines);
    return 0;
  }
  else {
    printf("error: input too big to sort\n");
    return 1;
  }
}


// Exercise 5-5. Write versions of the library functions strncpy, strncat, and strncmp, which
// operate on at most the first n characters of their argument strings. For example,
// strncpy(s,t,n) copies at most n characters of t to s. Full descriptions are in Appendix B.
int main5_5 (int arg, char *argv[]) {
  char s1[] = "test";
}

// Exercise 5-4. Write the function strend(s,t), which returns 1 if the string t occurs at the
// end of the string s, and zero otherwise.
int strend_local(char *s, char *t) {
  char *s_start;
  char *t_start;

  int eq;

  s_start = s;
  t_start = t;

  // rewind both pointers to the end of the strings
  while (*s != '\0') ++s;
  while (*t != '\0') ++t;

  // compare backwards
  while (*s == *t && t > t_start && s > s_start) {
    --t;
    --s;
  }

  if (*s == *t) {
    return 1;
  }

  return 0;
}

int main5_4 (int arg, char *argv[]) {
  char s1[] = "test";
  char s2[] = "best is test";

  int r;
  r = strend_local(s2, s1);

  printf("result: %d\n", r);
}

// Exercise 5-3. Write a pointer version of the function strcat that we showed in Chapter 2:
// strcat(s,t) copies the string t to the end of s.
//
void strcat_local(char *s, char *t) {
  while (*s != '\0')
    ++s;

/*
  (1)  *(s++) = 'z'; put 'z' into *s and increment s afterwards

  (2)  (*s)++; // increment *s ('d' -> 'e'), doesn't change value of s
  
  (3)  *s++ = 'z' is equal to (1)
  
  (4)  *s++ = *t++ reads as:
    put 'something' into *s and increment s afterwards,
    while 'something' is calculated as *t (and t is
    incremented after value is read from *t)

*/

  while ((*s++ = *t++) != '\0')
      ;

/*
  while(*t != '\0') {
    *s = *t;
    ++t;
    ++s;
  }
*/

//  *s = '\0';
}

int main5_3 (int arg, char *argv[]) {
  char s1[MAXLINE] = "abc";
  char s2[MAXLINE] = "def";

  strcat_local(s1, s2);
  printf("s1: %s\ns2: %s\n", s1, s2);

  char s3[] = "123";
  strcat_local(s3, "test");
  printf("s3: %s\n", s3);

/*  char *s4;
  s4 = "test";
  strcat_local(s4, "test");
//  printf("s4: %s\n", s4);
*/

}


// Exercise 5-2. Write getfloat, the floating-point analog of getint. What type does getfloat
// return as its function value?
int getfloat(float *pn) {
  int c;
  int sign;

  while (isspace(c = getch())) /* skip white space */
    ;

  if (!isdigit(c) && c != EOF && c != '+' && c != '-') {
    ungetch(c); /* it is not a number */
    return 0;
  }

  sign = (c == '-') ? -1 : 1;

  if (c == '+' || c == '-') {
    c = getch();

    if (!isdigit(c)) {
      ungetch(c);
      return 0;
    }
  }

  for (*pn = 0; isdigit(c); c = getch())
    *pn = 10 * *pn + (c - '0');

  // fractional part
  if (c == '.') {
    int i;
    for (i=1, c = getch(); isdigit(c); c = getch(), ++i)
      *pn = *pn + (float) (c - '0') / power_local(10, i);
  }

  *pn *= sign;
  
  if (c != EOF)
    ungetch(c);
  
  return c;
}

int main5_2 (int arg, char *argv[]) {
  int res;
  float i;

  while ((res = getfloat(&i)) > 0)
    printf("got %.4f\n", i);

}

// Exercise 5-1. As written, getint treats a + or - not followed by a digit as a valid
// representation of zero. Fix it to push such a character back on the input.

/* getint: get next integer from input into *pn */
int getint(int *pn) {
  int c, sign;
  
  while (isspace(c = getch())) /* skip white space */
    ;

  if (!isdigit(c) && c != EOF && c != '+' && c != '-') {
    ungetch(c); /* it is not a number */
    return 0;
  }

  sign = (c == '-') ? -1 : 1;

  if (c == '+' || c == '-') {
    c = getch();

    if (!isdigit(c)) {
      ungetch(c);
//      ungetch((sign == -1) ? '-' : '+');
      return 0;
    }
  }

  for (*pn = 0; isdigit(c); c = getch())
    *pn = 10 * *pn + (c - '0');
  
  *pn *= sign;
  
  if (c != EOF)
    ungetch(c);
  
  return c;
}

int main5_1 (int arg, char *argv[]) {
  int res;
  int i;

  while ((res = getint(&i)) > 0)
    printf("got %d\n", i);

}

void swap (int *x, int *y) {
  int z;
  z = *x;
  *x = *y;
  *y = z;
}

int main5_0 (int arg, char *argv[]) {
  int x, x1;
  int *y;

  x = 1;
  y = &x;

  x1 = x;
  *y = 2;

  ++*y;
  (*y)++;

  swap(&x, &x1);

  int z[5] = {1, 2, 3, 10, 15};

  y = z + 3;

  printf("hello pointers: %d, %d\n", x, x1);
  printf("y: %d\n", *(y + 3));

  char *t = "abc";
  printf("%s\n", t);
}

// Exercise 4-14. Define a macro swap(t,x,y) that interchanges two arguments of type t.
// (Block structure will help.)
# define swap(t,x,y) {t z; z=x; x=y; y=z;}
# define sum_def(t,x,y) t x##y; x##y = x + y;

int main4_14(int arg, char *argv[]) {
  int x = 1;
  int y = 2;

  swap(int, x, y)

  printf("x: %d\n", x);
  printf("y: %d\n", y);

  sum_def(int, x, y);
  printf("xy: %d\n", xy);

  printf("_SYS_TYPES_H: %d\n", _SYS_TYPES_H);

//  printf("%s\n", SYSTEM);
}

// Exercise 4-13. Write a recursive version of the function reverse(s),
// which reverses the string s in place.
void reverse_recursive(char s[], int depth) {
  static int line_length = 0;
  static int strlen_s = 0;
  int tmp;

/*
  if (strlen_s == 0) strlen_s = strlen(s);

  if (depth < strlen_s / 2) {
    reverse_recursive(s, depth + 1);

    tmp = s[depth];
    s[depth] = s[line_length - depth + 1];
    s[line_length - depth + 1] = tmp;
  }
  else {
    if (strlen_s % 2 == 1) {
      line_length = depth * 2 + 1;
    }
    else {
      line_length = depth * 2;

      tmp = s[depth];
      s[depth] = s[line_length - depth + 1];
      s[line_length - depth + 1] = tmp;
    }
    
    printf("final depth: %u\n", depth);
    printf("line length: %u\n", line_length);
//    s[line_length - depth] =    
  }
*/

}

int main4_13(int arg, char *argv[]) {
  char buf[BUFSIZE] = "1234test";
  printf("original string: %s\n", buf);
  reverse_recursive(buf, 0);
  printf("reversed string: %s\n", buf);
}

// Exercise 4-12. Adapt the ideas of printd to write a recursive version of itoa; that is, convert
// an integer into a string by calling a recursive routine.
/* printd: print n in decimal */
void printd(int n) {
  if (n < 0) {
    putchar('-');
    n = -n;
  }
  if (n / 10)
    printd(n / 10);
  
  putchar(n % 10 + '0');
}

void itoa_recursive(int n, char s[], int depth) {
  static int line_length = 0;

  if (n < 0) {
    line_length = 1;
    s[depth++] = '-';
    n = -n;
  }

  if (n / 10) {
    itoa_recursive(n / 10, s, depth + 1);
    s[line_length - depth] = n % 10 + '0';
  }
  else {
    line_length += depth;
    s[line_length - depth] = n % 10 + '0';
    s[depth+1] = '\0';
  }
}

int main4_12(int arg, char *argv[]) {
  printd(123);
  putchar('\n');
  
  char buf[BUFSIZE];
  itoa_recursive(5123, buf, 0);

  printf("%s\n", buf);

  return;
}

// Exercise 4-5. Add access to library functions like sin, exp, and pow. See <math.h> in
// Appendix B, Section 4.
int main4_5(int type) {
  double op;

  if (strcmp(math_function, "sin") == 0) {
    calc_push(sin(calc_pop()));
  }
  if (strcmp(math_function, "exp") == 0) {
    calc_push(exp(calc_pop()));
  }
  if (strcmp(math_function, "pow") == 0) {
    op = calc_pop();
    calc_push(pow(op, calc_pop()));
  }

  return;
}

// Exercise 4-4. Add the commands to print the top elements of the stack without popping, to
// duplicate it, and to swap the top two elements. Add a command to clear the stack.
void swap_top_elements() {
  int i;

  if (sp < 3) {
    printf("not enough elements in stack to swap\n");
    return;
  }

  i = val[sp - 1];
  val[sp - 1] = val[sp - 2];
  val[sp - 2] = i;
}

// show n top elements in stack
double calc_show_stack_top(int n) {
  int i;

  if (sp > 0 && n > 0) {
    for (i = 0; i < n; ++i)
      if (sp - i > 0) {
        printf("%d: %g\n", sp - i, val[sp - i - 1]);
      }
      else {
        printf("no more elements in stack\n");
        break;
      }
  }
  else
    printf("stack empty\n");
}

void clear_stack() {
  while (sp > 0)
    calc_pop();
}

int main4_4(int arg, char *argv[]) {
  calc_push(11.1);  
  calc_push(31);  
  calc_push(3);
  swap_top_elements();
  clear_stack();
  calc_show_stack_top(5);
}

// Exercise 4-3. Given the basic framework, it's straightforward to extend the calculator. Add the
// modulus (%) operator and provisions for negative numbers.
//
int main4_3(int arg, char *argv[]) {
  int type;
  double op2;
  char s[MAXOP];

  while ((type = calc_getop(s)) != EOF) {
//    printf("got %d\n", type);
    
    switch (type) {
      case NUMBER:
        calc_push(atof(s));
        break;
      case MATH_FUNCTION:
        main4_5(type);
        break;
      case '+':
        calc_push(calc_pop() + calc_pop());
        break;
      case '*':
        calc_push(calc_pop() * calc_pop());
        break;
      case '-':
        op2 = calc_pop();
        calc_push(calc_pop() - op2);
        break;
      case '/':
        op2 = calc_pop();
        if (op2 != 0.0)
          calc_push(calc_pop() / op2);
        else
          printf("error: zero divisor\n");
        break;
      case '%':
        op2 = calc_pop();
        if (op2 != 0.0)
          calc_push((int) calc_pop() % (int) op2);
        else
          printf("error: zero divisor\n");
        break;
      case '\n':
        printf("\t%.8g\n", calc_pop());
        break;
      default:
        printf("error: unknown command %s\n", s);
        break;
    }
  }

  return 0;
}

/* push: push f onto value stack */
void calc_push(double f) {
  if (sp < MAXVAL)
    val[sp++] = f;
  else
    printf("error: stack full, can't push %g\n", f);
}

/* pop: pop and return top value from stack */
double calc_pop(void) {
  if (sp > 0)
    return val[--sp];
  else {
    printf("error: stack empty\n");
    return 0.0;
  }
}

/* getop: get next character or numeric operand */
int calc_getop(char s[]) {
  int i, c, d;
  
  while ((s[0] = c = getch()) == ' ' || c == '\t')
      ;
  s[1] = '\0';

  // sin, exp, pow
  if (c == 's' || c == 'e' || c == 'p') {
    math_function[0] = c;
    i = 1;
    while ((d = getch()) != ' ' && d != '\t' && d != '\n' && d != EOF) 
      math_function[i++] = d;

    math_function[i] = '\0';
    ungetch(d);

    if (strcmp(math_function, "sin") == 0 ||
      strcmp(math_function, "exp") == 0 ||
      strcmp(math_function, "pow") == 0) {
      return MATH_FUNCTION;
    }

    ungets(math_function, strlen(math_function) - 1);
  }
  
  if (!isdigit(c) && c != '.' && c != '-')
    return c;

  // account for negative numbers
  if (c == '-') {
    if (isdigit(i = getch()))
      ungetch(i);
    else {
      ungetch(i);
      return c; /* not a negative number */   
    }
  }
          
  i = 0;
  if (isdigit(c) || c == '-') /* collect integer part */
    while (isdigit(s[++i] = c = getch()))
      ;

  if (c == '.') /* collect fraction part */
    while (isdigit(s[++i] = c = getch()))
      ;

  s[i] = '\0';

// i don't understand why this check is needed, see no difference
//  if (c != EOF)
    ungetch(c);

  return NUMBER;
}

/* get a (possibly pushed-back) character */
int getch(void) {
  return (bufp > 0) ? buf[--bufp] : getchar();
}

/* push character back on input */
void ungetch(int c) {
  if (bufp >= BUFSIZE)
    printf("ungetch: too many characters\n");
  else
    buf[bufp++] = c;
}

/*
  Exercise 4-7. Write a routine ungets(s) that will push back an entire string onto the input.
  Should ungets know about buf and bufp, or should it just use ungetch?

  push string 's' back on input
    does check that string fits into unget buffer
    start with the end of the string
    allows limit (1-based)
*/
void ungets(char s[], int limit) {
  int i;
  int s_len;

  s_len = strlen(s);
  if (limit == -1) limit = s_len + 1;

  if (bufp + s_len - (limit + 1) >= BUFSIZE) {
    printf("ungets: string too long to fit in unget buffer");
  }
  else {
    i = 0;
    while (i < (limit - 1) && i < s_len) {
      ungetch(s[s_len - (i + 1)]);
      ++i;
    }
  }
}

// Exercise 4-2. Extend atof to handle scientific notation of the form
// 123.45e-6
// where a floating-point number may be followed by e or E and an optionally signed exponent.
/* atof: convert string s to double */
double atof_local(char s[]) {
  double val, power;
  int i, sign, e;

  for (i = 0; isspace(s[i]); i++) /* skip white space */
    ;

  sign = (s[i] == '-') ? -1 : 1;

  if (s[i] == '+' || s[i] == '-')
    i++;

  for (val = 0.0; isdigit(s[i]); i++)
    val = 10.0 * val + (s[i] - '0');

  if (s[i] == '.')
    i++;

  for (power = 1.0; isdigit(s[i]); i++) {
    val = 10.0 * val + (s[i] - '0');
    power *= 10;
  }

  if (s[i++] == 'e' && s[i++] == '-') {
    e = 0;
    while(isdigit(s[i])) {
      e = e * 10 + (s[i] - '0');
      ++i;
    }
    val *= power_local(10, e);
  }

  return sign * val / power;
}

int main4_2(int arg, char *argv[]) {
  double sum;
  double atof_local(char []);
  char line[MAXLINE];
  sum = 0;
  
    while (getline_local(line, MAXLINE, 0) > 0)
      printf("\t%g\n", sum += atof_local(line));
  
  return 0;  
}

// Exercise 4-1. Write the function strindex(s,t) which returns the position of the rightmost
// occurrence of t in s, or -1 if there is none.
int strrindex(char s[], char t[]) {
  int i;
  int j;
  
  int s_length = strlen(s);
  int t_length = strlen(t);

  if (t_length < 1) return -1;
  if (s_length < 1) return -1;
  if (s_length < t_length) return -1;
  
  for (i = s_length - t_length; i >= 0 ; --i) {
    for (j = 0; s[i + j] == t[j]; ++j)
      ;

    if (j == t_length) {
      return i;
    }
  }
}

int strlindex(char s[], char t[]) {
  int i;
  int j;
  
  if (strlen(t) < 1) return -1;
  if (strlen(s) < 1) return -1;
  if (strlen(s) < strlen(t)) return -1;

  for (i = 0; i <= strlen(s) - strlen(t); ++i) {
    for (j = 0; s[i + j] == t[j]; ++j)
      ;

    if (j == strlen(t)) {
      return i;
    }
  }
  
  return -1;  
}

int main4_1(int arg, char *argv[]) {
  char where_to_search[] = "this is a test";
  char what_to_search[] = "s";
  int lpos, rpos;

  printf("%s\n", where_to_search);
  printf("%s\n", what_to_search);

  lpos = strlindex(where_to_search, what_to_search);
  rpos = strrindex(where_to_search, what_to_search);

  if (lpos > 0) {
    printf("most left 0-based position: %d\n", lpos);
    printf("most right 0-based position: %d\n", rpos);
  }
  else {
    printf("[%s] is not contained in: %s\n", what_to_search, where_to_search);
  }
}

// Exercise 3-4. In a two's complement number representation, our version of itoa does not
// handle the largest negative number, that is, the value of n equal to -(2wordsize-1). Explain why not.
// Modify it to print that value correctly, regardless of the machine on which it runs.

void reverse_inline(char s[]) {
  int c, i, j;
  for (i = 0, j = strlen(s)-1; i < j; i++, j--) {
    c = s[i];
    s[i] = s[j];
    s[j] = c;
  }
}
/* itoa: convert n to characters in s */
void itoa(int n, char s[]) {
  int i, sign, j;

  sign = n;
  
  i = 0;
  
  do { /* generate digits in reverse order */
    printf("n: %d\n", n);

    j = n % 10;
    if (j < 0) j = -j;
    s[i++] = j + '0'; /* get next digit */

  } while ((n /= 10) != 0); /* delete it */

  if (sign < 0)
    s[i++] = '-';
  
  s[i] = '\0';
  reverse_inline(s);
}

int main3_4(int arg, char *argv[]) {
  int l = INT32_MIN;
  char l_str[MAXLINE];

  itoa(l, l_str);
  printf("%d is %s\n", l, l_str);
}

int find_int_size() {
  int i;
  int j;

  for (i=1; i > 0; ++i) j=i;
  printf("max int: %d\n", j);
  printf("overflow int: %d\n", i);

  for (i=-1; i < 0; --i) j=i;
  printf("min int: %d\n", j);
  printf("overflow int: %d\n", i);
}

// Exercise 3-3. Write a function expand(s1,s2) that expands shorthand notations like a-z in
// the string s1 into the equivalent complete list abc...xyz in s2. Allow for letters of either case
// and digits, and be prepared to handle cases like a-b-c and a-z0-9 and -a-z. Arrange that a
// leading or trailing - is taken literally.
void expand(char s1[], char s2[]) {
  int i = 0;
  int j = 0;
  int k;

  // skip trailing spaces
  for(; i < strlen(s1); ++i)
    if (s1[i] != ' ')
      break;

  // trailing - taken literally
  if (s1[i] == '-')
    s2[j++] = s1[i++];

  for(; i < strlen(s1); ++i) {
    if (s1[i] == '-' && (i + 1) < strlen(s1)) {

      if (s2[j-1] < s1[i+1]) {
        for(k = s2[j-1] + 1; k < s1[i+1]; ++k)
          s2[j++] = k;
      }
      else {
        s2[j++] = s1[i];
      }
    }
    else {
      s2[j++] = s1[i];
    }
  }
      
}

int main3_3(int arg, char *argv[]) {
  char source[MAXLINE];
  char destination[MAXLINE];
  
  getline_local(source, MAXLINE, 0);
  expand(source, destination);

  printf("source: %s\n", source);
  printf("expanded: %s\n", destination);
}

// Exercise 3-2. Write a function escape(s,t) that converts characters like newline and tab into
// visible escape sequences like \n and \t as it copies the string t to s. Use a switch. Write a
// function for the other direction as well, converting escape sequences into the real characters.
void unescape(char s[], char t[]) {
  int i = 0;
  int j = 0;
  int current_char;

  while(i < strlen(s)) {
    current_char = s[i];

    switch(current_char) {
      case '\\':
        switch(s[i+1]) {
          case 't':
            t[j++] = '\t';
            ++i;
            break;
          case 'n':
            t[j++] = '\n';
            ++i;
            break;
          default:
            t[j++] = current_char;
            break;
        }
        break;
      default:
        t[j++] = current_char;
        break;
    }
    ++i;
  }
}

void escape(char s[], char t[]) {
  int i = 0;
  int j = 0;
  int current_char;

  while(i < strlen(s)) {
    current_char = s[i];

    switch(current_char) {
      case '\t':
        t[j++] = '\\'; 
        t[j++] = 't'; 
        break;
      case '\n':
        t[j++] = '\\'; 
        t[j++] = 'n'; 
        break;
      default:
        t[j++] = current_char;
        break;
    }
    
    ++i;
  }
}

int main3_2(int arg, char *argv[]) {
  char source[MAXLINE] = "test\tt\\est\ntest";
  char destination[MAXLINE] = "";

  escape(source, destination);

  printf("source: %s\n", source);
  printf("destination: %s\n", destination);

  unescape(destination, source);

  printf("unescaped destination: %s\n", source);
}

// Exercise 3-1. Our binary search makes two tests inside the loop, when one would suffice (at
// the price of more tests outside.) Write a version with only one test inside the loop and measure
// the difference in run-time.

int binsearch_oneif(int x, int v[], int n) {
  register int low, high, mid;
  low = 0;
  high = n - 1;

  while (low < high) {
    //printf("%d - %d\n", low, high);
    mid = (low+high)/2;
    
    if (x <= v[mid])
      high = mid;
    else
      low = mid + 1;
  }
  
  //printf("%d - %d\n", low, high);
  if (x == high) {
    return high;
  }

  return -1; /* no match */
}

/* binsearch: find x in v[0] <= v[1] <= ... <= v[n-1] */
int binsearch(int x, int v[], int n) {
  register int low, high, mid;
  low = 0;
  high = n - 1;
  
  while (low <= high) {
    //printf("%d - %d\n", low, high);
    mid = (low+high)/2;
    
    if (x < v[mid])
      high = mid - 1;
    else if (x > v[mid])
      low = mid + 1;
    else /* found match */
      return mid;
  }
  //printf("%d - %d\n", low, high);

  return -1; /* no match */
}

int main3_1(int arg, char *argv[]) {
  int v [BINARRAY];
  int i,j;

  for (i=0; i<BINARRAY; ++i) v[i] = i;

  for (i = 0; i < 1000; ++i)
    for (j = 0; j < 100000; ++j)
      binsearch_oneif(9999, v, BINARRAY);
      // binsearch(9999, v, BINARRAY);

  /*
    binsearch_oneif:
petya@petya-desktop:~/cron/petya$ time ./cron
result: 1000

real    0m3.701s
user    0m3.700s
sys     0m0.000s

    binsearch:
petya@petya-desktop:~/cron/petya$ time ./cron
result: 1000

real    0m4.702s
user    0m4.700s
sys     0m0.000s
  */

  printf("result: %d\n", i);
}

int main_comma_test () {
  printf("in here\n");

  for (printf("1\n"), sleep (1); printf("2\n"), sleep (1), 1; printf("3\n"), sleep (1)) {
    printf("body\n"), sleep(1);
  }
}

// Exercise 2-5. Write the function any(s1,s2), which returns the first location in a string s1
// where any character from the string s2 occurs, or -1 if s1 contains no characters from s2.
// (The standard library function strpbrk does the same job but returns a pointer to the
// location.)
int any(char s1[], char s2[]) {
  int i,j;
  for(i = 0; i < strlen(s1); ++i) {
    for(j = 0; j < strlen(s2); ++j) {
      if (s1[i] == s2[j])
        return i + 1;
    }
  }

  return -1;
}

int main2_5() {
  int res;
  res = any("test", "te");  
  printf("%d\n", res);
}

void squeeze (char s1[], char s2[]) {
  int i,j,k;
  for(i=0; i<strlen(s2); ++i) {
    for(j = k = 0; s1[j] != '\0'; ++j) {
      if (s1[j] != s2[i])
        s1[k++] = s1[j];
    }
    s1[k]='\0';
  }
}

// Exercise 2-4. Write an alternative version of squeeze(s1,s2) that deletes each character in
// s1 that matches any character in the string s2.
int main2_4(int arg, char *argv[]) {
  char line[MAXLINE] = "test is best";
  squeeze(line, "e");
  printf("%s\n", line);
}

// Exercise 2-3. Write a function htoi(s), which converts a string of hexadecimal digits
// (including an optional 0x or 0X) into its equivalent integer value. The allowable digits are 0
// through 9, a through f, and A through F.
int htoi(char in[]) {
  int i;
  int j = 0;

  int start_offset = 0;

  if (strlen(in) > 2 && in[0] == '0' && (in[1] == 'x' || in[1] == 'X')) {
    start_offset = 2;
  }

  for (i = 0 + start_offset; i < strlen(in); ++i) {
    if (isdigit(in[i])) {
      j = j * 16 + (in[i] - '0');
    }
    else if (isxdigit(in[i])) {
      j = j * 16 + (tolower(in[i]) - 'a' + 10);
    }
  }
  return j;
}

int main2_3(int argc, char *argv []) {
  char line[] = "DEAD";
  printf("%s is: %d\n", argv[1], htoi(argv[1]));
}

int main2_2a(char line []) {
  int i;
  for (i=0; line[i] != 0; ++i) {
    putchar(tolower(line[i]));
  }
  putchar('\n');
}

// for (i=0; i < lim-1 && (c=getchar()) != '\n' && c != EOF; ++i)
//   s[i] = c;
//
// Exercise 2-2. Write a loop equivalent to the for loop above without using && or ||.
int main2_2() {
  int i,c;
  int done=0;
  char line[MAXLINE];

  for (i=0; done == 0; ++i) {
    if (i < MAXLINE - 1) {
      if ( (c = getchar()) != '\n') {
        if (c != EOF) {
          line[i] = c;
          continue;
        }
      }
    }

    // exit
    done = 1;
  }
  line[i - 1] = 0;

  printf("got [%s]\n", line);
  main2_2a(line);
}


// Exercise 2-1. Write a program to determine the ranges of char, short, int, and long
// variables, both signed and unsigned, by printing appropriate values from standard headers
// and by direct computation. Harder if you compute them: determine the ranges of the various
// floating-point types.
int main2_1() {
  printf("signed char min: %d\n", SCHAR_MIN);  
  printf("signed char max: %d\n", SCHAR_MAX);  
  printf("unsigned char max: %d\n", UCHAR_MAX);

}

// Exercise 1-23. Write a program to remove all comments from a C program. Don't forget to
// handle quoted strings and character constants properly. C comments don't nest.
int main1_23() {
  int i, c;

  int possible_comment;
  int one_line_comment;
  int multiline_comment;
  int possible_multiline_end;

  int escaped;
  int in_single_quotes;
  int in_double_quotes;

  i = 0;

  while((c=getchar()) != EOF) {
    if (c == '\n')
      i = 0;
    else
      ++i;

    if (c == '\\' && escaped != 1) {
      escaped = 1;
    }
    else {
      escaped = 0;
    }

    if (in_single_quotes == 1) {
      if (c == '\'' && escaped != 1) in_single_quotes = 0;
      putchar(c);
      continue;
    }
    if (in_double_quotes == 1) {
      if (c == '"' && escaped != 1) in_double_quotes = 0;
      putchar(c);
      continue;
    }
    if (one_line_comment == 1) {
      if (c == '\n') {
        one_line_comment = 0;
        putchar(c);
      }
      continue;
    }
    if (multiline_comment == 1) {
      if (possible_multiline_end == 1) {
        possible_multiline_end = 0;
        if (c == '/') {
          multiline_comment = 0;
        }
        continue;
      }
      if (c == '*') {
        possible_multiline_end = 1;
        continue;
      }
      continue;
    }

    if (possible_comment == 1) {
      possible_comment = 0;

      if (c == '/') {
        one_line_comment = 1;
        continue;
      }
      
      if (c == '*') {
        multiline_comment = 1;
        continue;
      }

      putchar('/');
      putchar(c);
    }
    else {
      if (c == '/') {
        possible_comment = 1;
        continue;
      }
      if (c == '\'') {
        in_single_quotes = 1;
      }
      if (c == '"') {
        in_double_quotes = 1;
      }

      putchar(c);
    }
  }
}

// Exercise 1-21. Write a program entab that replaces strings of blanks by the minimum number
// of tabs and blanks to achieve the same spacing. Use the same tab stops as for detab. When
// either a tab or a single blank would suffice to reach a tab stop, which should be given
// preference?
int main1_21() {
  int c, i;
  int in_spaces;
  in_spaces = 0;

  for (i=0; (c=getchar()) != EOF; ++i) {
    if (c == '\n') i=-1;

    if (c == ' ') {
      ++in_spaces;
    }
    else if (c == '\t') {
      in_spaces = in_spaces + TABSIZE;
    }
    else {
      if (in_spaces > 0) {
        // do entab
        while (in_spaces >= TABSIZE) {
          putchar('\t');
          in_spaces = in_spaces - TABSIZE;
        }
        while (in_spaces > 0) {
          putchar(' ');
          --in_spaces;
        }
      }
      putchar(c);
    }
  }
}

// Exercise 1-20. Write a program detab that replaces tabs in the input with the proper number
// of blanks to space to the next tab stop. Assume a fixed set of tab stops, say every n columns.
// Should n be a variable or a symbolic parameter?
int main1_20() {
  int c, i;
  int spaces_needed, j;

  i = 0;
  while((c=getchar()) != EOF) {
    if (c == '\n')
      i = 0;
    else
      ++i;

    if (c == '\t') {
      spaces_needed = TABSIZE - ((i - 1) % TABSIZE);
      for (j=0; j < spaces_needed; ++j) putchar('.');
    }
    else {
      putchar(c);
    }
  }
}

// Exercise 1-19. Write a function reverse(s) that reverses the character string s. Use it to
// write a program that reverses its input a line at a time.
void reverse(char line_in[], char line_out[]) {
  int i,j,k;

  // find length of line_in
  for (i=0; i < MAXLINE && line_in[i] != '\0'; ++i)
    ;

  // reverse
  k=0;
  for (j = i - 1; j >= 0; --j) {
    line_out[k] = line_in[j];
    ++k;
  }
  line_out[k] = '\0';
}

int main1_19_1() {
  char line[MAXLINE];
  char line_reversed[MAXLINE];
  int current_line_length;

  while ((current_line_length = getline_local(line, MAXLINE, 1)) != 0) {
    reverse(line, line_reversed);
    printf("%s\n", line_reversed);    
  }
}

int main1_19() {
  char line[MAXLINE];
  char line_reversed[MAXLINE];
  int i;

  for(i=0; i < MAXLINE; ++i) {
    line[i] = '\0';
    line_reversed[i] = '\0';
  }

  line[0] = 'a';
  line[1] = '2';
  line[2] = 'b';
  line[3] = '3';
  line[4] = 'v';
  line[5] = '3';
  line[6] = '\0';

  reverse(line, line_reversed);

  printf("%s\n", line);
  printf("%s\n", line_reversed);
}

// Exercise 1-18. Write a program to remove trailing blanks and tabs from each line of input, and
// to delete entirely blank lines.
int main1_18() {
  int current_line_length;
  char line[MAXLINE];
  int removed;
  
  int in_space;
  int i;

  while ((current_line_length = getline_local(line, MAXLINE, 1)) != 0) {
    if (current_line_length == -1) continue;

    printf("%d\n", current_line_length);

    removed = 0;

    for(i = current_line_length - 2; i >= 0; --i) {
      // putchar(line[i]);
      // putchar('\n');
      if (removed != 1 && line[i] != ' ' && line[i] != '\t') {
        // printf("BOO\n");
        line[i+1] = '\0';
        removed = 1;
      }
    }

    putchar('-');
    for(i=0; line[i] != '\0'; ++i)
      putchar(line[i]);
    putchar('-');
    putchar('\n');    
  }
}

// Exercise 1-17. Write a program to print all input lines that are longer than 80 characters.
int main1_17() {
  int current_line_length;
  char line[MAXLINE];

  while ((current_line_length = getline_local(line, MAXLINE, 0)) > 0) {
    if (current_line_length > 8)
      printf("%s\n", line);
  }
}

// Exercise 1-16. Revise the main routine of the longest-line program so it will correctly print the
// length of arbitrary long input lines, and as much as possible of the text.

/* print the longest input line */
int main1_16() {
  int len; /* current line length */
  int max; /* maximum length seen so far */
  char line[MAXLINE]; /* current input line */
  char longest[MAXLINE]; /* longest line saved here */
  
  max = 0;
  while ((len = getline_local(line, MAXLINE, 0)) > 0 || len == -1)
    if (len > max) {
      max = len;
      copy(longest, line);
    }
  
  if (max > MAXLINE) {
    printf("length %d (truncated): %s\n", max, longest);
  }
  else if (max > 0) /* there was a line */
    printf("length %d: %s\n", max, longest);
  else
    printf("no input\n");

  return 0;
}

/* 
  getline_local:
    reads no more than `lim` characters (up to \n or EOF)
    from STDIN into `s`, always builds \0 terminated line.

  OUTPUT:
    if number of characters read is less than `lim`
    simply returns length of line including last \0 character;
    otherwise returns either real line length
    or `lim` (depends on the `return_truncated_length` var)

    if read only `EOF` returns `0`
    if read only `\n` returns `-1`
*/
int getline_local(char s[], int lim, int return_truncated_length) {
  int c, i;
  for (i=0; (c=getchar()) != EOF && c != '\n'; ++i) {
    if (i < lim - 1) {
      s[i] = c;
    }
  }

  if (i == 0)
    s[0] = '\0';
  else if (i <= lim)
    s[i] = '\0';
  else
    s[lim] = '\0';

  if (i == 0 && c == '\n')
    return -1;
  else if (i == 0 && c == EOF)
    return 0;
  else if (return_truncated_length == 1 && i > lim) {
    return lim;
  }
  else
    return i + 1;
}
/* copy: copy 'from' into 'to'; assume to is big enough */
void copy(char to[], char from[])
{
  int i;
  i = 0;
  while ((to[i] = from[i]) != '\0')
  ++i;
}

// Exercise 1.15. Rewrite the temperature conversion program of Section 1.2 to use a function
// for conversion.
int main1_15() {
  int i;

  printf("%4s %6s\n", "F", "C");
  printf("---- ------\n");
  i = LOWER;
  while(i < UPPER) {
    printf("%3d: %6.1f\n", i, fahrenheit_to_celcius(i));
    i = i + STEP;
  }
  printf("\n\n");

  printf("%4s %6s\n", "C", "F");
  printf("---- ------\n");
  for (i = UPPER; i >= LOWER; i = i - STEP)
    printf("%3d: %6.1f\n", i, celsius_to_fahrenheit(i));
  printf("\n\n");
}

// Exercise 1-14. Write a program to print a histogram of the frequencies of different characters
// in its input.
int main9() {
  int chars [LENGTHS_SIZE];
  int i,j,k, current_char, current_length;
  
  for (i=0; i < LENGTHS_SIZE; ++i)
    chars[i]=0;

  while ((current_char = getchar()) != EOF) {
    ++chars[current_char];
  }

// horizontal
  for (i=0; i < LENGTHS_SIZE; ++i) {
    if (chars[i] > 0) {
      printf("%03d", i);
      if (i>20) {
        putchar(' ');
        putchar(i);
        putchar(' ');
      }
      else {
        printf("   ");
      }
      printf(": %03d ", chars[i]);



      for (j=0; j < chars[i] && j < 70; ++j) {
        printf("*");
      }

      printf("\n");
    }
  }
}

// Exercise 1-13. Write a program to print a histogram of the lengths of words in its input. It is
// easy to draw the histogram with the bars horizontal; a vertical orientation is more challenging.
int main8() {
  int lengths [LENGTHS_SIZE];
  int i,j,k, current_char, current_length;
  
  for (i=0; i < LENGTHS_SIZE; ++i)
    lengths[i]=0;

  current_length = 0;

  while ((current_char = getchar()) != EOF) {
    // end of previous word, beginning of the next
    if (current_char == ' ' || current_char == '\t' || current_char == '\n' || current_char == ',') {
      if (current_length > 0) {
        ++lengths[current_length];
      }
      current_length = 0;
    }
    else {
      ++current_length;
    }
  }

// horizontal
  for (i=0; i < LENGTHS_SIZE; ++i) {
    if (lengths[i] > 0) {
      printf("%02d (%d): ", i, lengths[i]);

      for (j=0; j < lengths[i] && j < 70; ++j) {
        printf("-");
      }

      printf("\n");
    }
  }
  printf("\n\n");

// vertical
  int max_length = 0;
  for (i=0; i < LENGTHS_SIZE; ++i) {
    if (lengths[i] > max_length)
      max_length = lengths[i];
  }

  if (max_length > 30) {
    if (DEBUG) printf("real max_length: %d\n\n", max_length);
    max_length = 30;
  }

  for (k=0; k < max_length; ++k) {
    for (j=0; j < LENGTHS_SIZE; ++j) {
      if (lengths[j] > 0) {
        if (lengths[j] >= (max_length - k)) {
          printf("  *  ");
        }
        else {
          printf("     ");
        }
      }    
    }
    printf("\n");
  }
  
  // legend
  for (j=0; j < LENGTHS_SIZE; ++j) if (lengths[j] > 0) printf("---- ");
  printf("\n");
  for (j=0; j < LENGTHS_SIZE; ++j) if (lengths[j] > 0) printf("%03d  ", j);
  printf("\n");
  for (j=0; j < LENGTHS_SIZE; ++j) if (lengths[j] > 0) printf("%03d  ", lengths[j]);
  printf("\n");
}

// Exercise 1-12. Write a program that prints its input one word per line.
// (converting multiple spaces into one new-line
int main7() {
  int c;
  int in_spaces;
  
  while ((c = getchar()) != EOF) {
    if (c == ' ' || c == '\t' || c == '\n') {
      if (in_spaces == FALSE) {
        in_spaces = TRUE;
        putchar('\n');
      }
      continue;
    }
    else {
      in_spaces = FALSE;
    }

    putchar(c);
  }
}

// Exercise 1-10. Write a program to copy its input to its output, replacing each tab by \t, each
// backspace by \b, and each backslash by \\. This makes tabs and backspaces visible in an
// unambiguous way.
int main6() {
  int c;
  while ((c = getchar()) != EOF) {
    if (c == '\t') {
      printf("\\t");
      continue;
    }
    
    if (c == '\b') {
      printf("\\b");
      continue;
    }

    if (c == '\\') {
      printf("\\\\");
      continue;
    }

    putchar(c);
  }
}
  
// Exercise 1-9. Write a program to copy its input to its output,
// replacing each string of one or more blanks by a single blank.
int main5() {
  int c;
  int had_spaces;
  while ((c = getchar()) != EOF) {
    if (c == ' ') {
      had_spaces = 1;
    }
    else {
      if (had_spaces) {
        printf(" ");
        had_spaces = 0;
      }
      putchar(c);
    }
  }
  return 0;
}

// Exercise 1-8. Write a program to count blanks, tabs, and newlines.
int main4() {
  int c;
  int count [3];
  count[0] = 0;
  count[1] = 0;
  count[2] = 0;
  while ((c = getchar()) != EOF) {
    if (c == ' ') ++count[0];
    if (c == '\t') ++count[1];
    if (c == '\n') ++count[2];
  }
  printf("spaces: %d\n", count[0]);
  printf("tabs: %d\n", count[1]);
  printf("newlines: %d\n", count[2]);
  return 0;
}

// count new lines
int main3 () {
  int c, nc;
  nc = 0;
  while((c = getchar()) != EOF) {
    if (c == '\n')
      ++nc;
  }
  printf("%d\n", nc);
  return 0;
}

// count all input characters
int main2 () {
  double nc;
  for (nc = 0; getchar() != EOF; ++nc)
    ;
  printf("%.0f\n", nc);
  return 0;
}

// first test
int main1(int argc, char *argv[]) {
//  printf("hello world\n\n");

  if (argc < 2) {
    printf("\nusage: ./cron <COMMAND>\n\n");
    return 0;
  }

//  printf("got: -%s-\n", argv[1]);

/*
  if (strcmp(argv[1], "true")) {
    printf("\n");
    is_true(0);
    is_true(1);
    is_true(-1);
    printf("\n");
  }
  else {
*/

/*  else {
    int i;
    for(i=0; i<argc; i++) {
      printf("%u: %s\n", i, (char *)argv[i]);
    }
  }*/

  int i;

//  int * p;
//  int k;
//  p = &k;
//  *p = 1;

  struct stat stat_buf;
  char * boo;
  boo = "/etc/passwd1";

  printf("stating file [%s]\n\n", boo);
  if (stat(boo, &stat_buf)) {
    printf("error stating file: %d\n", errno);
  }
  else {
    printf("file size is: %d\n", (int) stat_buf.st_size);
  }

  int n_arr[10];
  n_arr[0] = 1;
  n_arr[11] = 2;

//  printf("%i\n", 'a' - 'b');
//  }

  return 0;
}

int main(int argc, char *argv[]) {
  if (argc > 1) DEBUG=TRUE;

  int j;
  j = main5_7(argc, argv);
  return 0;
}
