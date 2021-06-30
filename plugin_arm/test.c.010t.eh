
;; Function iowrite_example (iowrite_example, funcdef_no=1, decl_uid=5148, cgraph_uid=1, symbol_order=5)

__attribute__((irqbox_test))
iowrite_example ()
{
  struct UART * uart_2;
  struct UART * uart_1.0;

  uart_2 = 266403840B;
  uart_2->DR = 1;
  uart_1.0 = uart_1;
  uart_1.0->DR = 1;
  return;
}



;; Function main (main, funcdef_no=2, decl_uid=5152, cgraph_uid=2, symbol_order=6)

main ()
{
  int D.5158;

  iowrite_example ();
  D.5158 = 0;
  goto <D.5159>;
  D.5158 = 0;
  goto <D.5159>;
  <D.5159>:
  return D.5158;
}


