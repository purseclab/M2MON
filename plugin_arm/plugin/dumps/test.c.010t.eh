
;; Function foo (foo, funcdef_no=1, decl_uid=5135, cgraph_uid=1, symbol_order=1)

foo ()
{
  return;
}



;; Function fun (fun, funcdef_no=2, decl_uid=5143, cgraph_uid=2, symbol_order=3)

fun (int a)
{
  return;
}



;; Function testup_svcal (testup_svcal, funcdef_no=4, decl_uid=5141, cgraph_uid=4, symbol_order=5)

__attribute__((irqbox))
testup_svcal ()
{
  void (*<Te27>) (int) fun_ptr;
  unsigned int * shouldNot;
  int b;
  unsigned int * simple;
  volatile unsigned int * mem_ref2;
  volatile unsigned int * mem_ref;
  struct a * pst;
  struct a st;
  int a;
  volatile unsigned int * D.5180;
  int a.1;
  unsigned char D.5178;
  volatile unsigned char * D.5177;
  volatile unsigned char * D.5176;
  int a.0;
  unsigned int D.5174;
  volatile unsigned int * D.5173;
  volatile unsigned int * D.5172;

  a = 0;
  a = 2;
  D.5172 = 268308480B;
  *D.5172 = 0;
  D.5173 = 268308480B;
  D.5174 = *D.5173;
  a.0 = (int) D.5174;
  a = a.0;
  D.5176 = 268308480B;
  *D.5176 = 0;
  D.5177 = 268308480B;
  D.5178 = *D.5177;
  a.1 = (int) D.5178;
  a = a.1;
  st.a = 0;
  pst = &st;
  pst->a = 0;
  mem_ref = 268308480B;
  mem_ref2 = mem_ref;
  *mem_ref = 0;
  simple = &a;
  *simple = 5;
  b = 0;
  shouldNot = &b;
  *shouldNot = 10;
  D.5180 = 250482688B;
  *D.5180 = 0;
  global = 2;
  fun_ptr = fun;
  fun_ptr (10);
  fun (10);
  a = {CLOBBER};
  st = {CLOBBER};
  b = {CLOBBER};
  return;
}



;; Function test2 (test2, funcdef_no=5, decl_uid=5160, cgraph_uid=5, symbol_order=6)

test2 (int c)
{
  int D.5181;

  D.5181 = 1;
  goto <D.5182>;
  <D.5182>:
  return D.5181;
}



;; Function test3 (test3, funcdef_no=6, decl_uid=5163, cgraph_uid=6, symbol_order=7)

test3 (int d)
{
  testup_svcal ();
  return;
}



;; Function main (main, funcdef_no=7, decl_uid=5165, cgraph_uid=7, symbol_order=8)

main ()
{
  int b;
  int c;
  int D.5183;

  printf ("Crap");
  c = 100;
  b = test2 (c);
  b = b + 1;
  test3 (b);
  testup_svcal ();
  testup_svcal ();
  D.5183 = 0;
  goto <D.5184>;
  D.5183 = 0;
  goto <D.5184>;
  <D.5184>:
  return D.5183;
}


