#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>     
#include<string.h>
#include<stdbool.h>
#include<math.h>

#define SIZE 100

int mCnt;

struct hash_t {
  unsigned long hash;
  double value;
};

struct hash_t varHash[SIZE];
int vhCnt = 0;

unsigned long strToHash(char *s)
{
    unsigned long hash = 5432;
    int c;
    while ((c = (unsigned char)*s++))
        hash = ((hash << 5) + hash) + c;
    return hash;
}

double valueByHash(unsigned long hash) {
  for (int i = 0; i < vhCnt; i++) {
    if (varHash[i].hash == hash) {
      return varHash[i].value;
    }
  }
  return 0;
}

int varHashExists(unsigned long hash) {
  for (int i = 0; i < vhCnt; i++) {
    if (varHash[i].hash == hash) {
      return i;
    }
  }
  return -1;
}

void setVarValue(char *s, double v) {
  unsigned long h = strToHash(s);
  int hi = varHashExists(h);
  if (hi == -1) {
    varHash[vhCnt].hash = h;
    varHash[vhCnt].value = v;
    vhCnt++;
  } else {
    varHash[hi].value = v;
  }
}

double parseExpr(char **s, int pr);

void skipBlanks(char **s)
{
    while(**s == ' ' || **s == '\n') (*s)++;
}

void na()
{
    fprintf(stderr, "?\n");
}

void getMember(char *r, char **s, bool asNumber) {
  int i = 1;
  while ((asNumber && (isdigit(**s) || (**s == '.'))) ||
         (!asNumber && isalpha(**s))) {
    r[i] = *(*s)++;
    i++;
  }
  r[i] = '\0';
}

bool isFunc(char *s) {
  return strcmp(s, "sin") == 0 ||
         strcmp(s, "cos") == 0 ||
         strcmp(s, "abs") == 0 ||
         strcmp(s, "pow") == 0;
}

double to_radians(double degrees) {
  return degrees / (180.0 / 3.1415926535);
}

double parseMember(char **s) {
    skipBlanks(s);
    mCnt++;
    char sn[SIZE];
    int c = *(*s)++;
    if(isalpha(c)) {
        sn[0] = c;
        getMember(sn, s, false);
        if (strcmp(sn, "pi") == 0) {
          return 3.141593;
        } else if (strcmp(sn, "e") == 0) {
          return 2.718282;
        } else {
          c = *(*s)++;
          if (isFunc(sn) && (c == '(')) {
            double tf = parseExpr(s, 0);
            if (strcmp(sn, "sin") == 0) {
              tf = sin(to_radians(tf));
            } else if (strcmp(sn, "cos") == 0) {
              tf = cos(to_radians(tf));
            } else if (strcmp(sn, "abs") == 0) {
              tf = fabs(tf);
            } else if (strcmp(sn, "pow") == 0) {
              skipBlanks(s);
              if(*(*s)++ != ',') {
                na();
                (*s)--;
              }
              double tf2 = parseExpr(s, 0);
              tf = pow(tf, tf2);
            }
            skipBlanks(s);
            if(*(*s)++ != ')') {
              na();
              (*s)--;
            }
            return tf;
          } else if (!isFunc(sn) && (c == '=')) {
            double vv = parseExpr(s, 0);
            setVarValue(sn, vv);
            return vv;
          } else {
            (*s)--;
            unsigned long h = strToHash(sn);
            if (varHashExists(h) >= 0) {
              return valueByHash(h);
            } else {
              na();
              return 0;
            }
          }
        }
        return 0;
    } else if(isdigit(c) || (c == '.')) {
        sn[0] = c;
        getMember(sn, s, true);
        return atof(sn);
    } else if(c == '-') {
        return -parseMember(s);
    } else if(c == '(') {
        double r = parseExpr(s, 0);
        skipBlanks(s);
        if(*(*s)++ != ')') {
            na();
            (*s)--;
        }
        return r;
    } else {
        na();
        return 0;
    }
}

double parseExpr(char **s, int pr) {
  if (pr == 3) 
    return parseMember(s);
  else {
    double res = parseExpr(s, pr + 1);
    while(1) {
        skipBlanks(s);
        switch(pr) {
          case 2:
            switch(*(*s)++) {
              case '^': res = pow(res, parseExpr(s, pr)); break;
              default: (*s)--; return res;
            }
            break;
          case 1:
            switch(*(*s)++) {
              case '*': res *= parseExpr(s, pr); break;
              case '/': res /= parseExpr(s, pr); break;
              default: (*s)--; return res;
            }
            break;
          case 0:
            switch(*(*s)++) {
              case '+': res += parseExpr(s, pr); break;
              case '-': res -= parseExpr(s, pr); break;
              default: (*s)--; return res;
            }
            break;
        }
    }
  }
}

int main(void) {
  char expr[SIZE];
  char *e;

  while(fgets(expr, sizeof(expr), stdin)) {
    e = expr;
    mCnt = 0;
    printf("%f\n", parseExpr(&e, 0));
  }
}
