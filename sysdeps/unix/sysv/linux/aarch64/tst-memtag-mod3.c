/* Unmarked module with a dependency (tst-memtag-mod2.so) that requires
   MTE stack protection.  */

extern void run_mte_test (void *);

void
mod3_func (void *closure)
{
  run_mte_test (closure);
}
