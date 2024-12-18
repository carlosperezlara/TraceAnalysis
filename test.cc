#include "generic.h"

int main() {
  float ti[502];
  float ch[502];
  PulseMeasure a(502);
  a.AddCF(0.2);

  a.SetData(ti,ch);

  return 0;
}
