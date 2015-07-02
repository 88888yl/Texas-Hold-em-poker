#include <stdio.h>
 
int checkIP(const char* p)
{
  int n[4];
  char c[4];
  if (sscanf(p, "%d%c%d%c%d%c%d%c",
             &n[0], &c[0], &n[1], &c[1],
             &n[2], &c[2], &n[3], &c[3])
      == 7)
  {
    int i;
    for(i = 0; i < 3; ++i)
      if (c[i] != '.')
        return 0;
    for(i = 0; i < 4; ++i)
      if (n[i] > 255 || n[i] < 0)
        return 0;
    return 1;
  } else
    return 0;
}
 
int main()
{
  const char* x[] =
  {
    "192.168.1.1",
    "10.0.0.1.",
    "127.256.0.1",
    "iugerjiogjioe",
    "172.16,0.1",
    0
  };
  const char* m[] = 
  {
    "illegal",
    "good"
  };
  int i = 0;
 
  while(x[i] != 0) {
    printf("%s %s\n", x[i], m[checkIP(x[i])]);
    ++i;
  }
  return 0;
}