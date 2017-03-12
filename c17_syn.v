/////////////////////////////////////////////////////////////
// Created by: Synopsys DC Expert(TM) in wire load mode
// Version   : K-2015.06-SP1
// Date      : Tue Mar  7 19:38:31 2017
/////////////////////////////////////////////////////////////


module c17 ( N1, N2, N3, N6, N7, N22, N23 );
  input N1, N2, N3, N6, N7;
  output N22, N23;
  wire   n13, n6, n8, n9, n10, n11, n12;

  INV_X1 U8 ( .A(n13), .ZN(n6) );
  INV_X1 U9 ( .A(n6), .ZN(N23) );
  NOR2_X1 U10 ( .A1(n8), .A2(n9), .ZN(n13) );
  NOR2_X1 U11 ( .A1(N2), .A2(N7), .ZN(n9) );
  INV_X1 U12 ( .A(n10), .ZN(n8) );
  NAND2_X1 U13 ( .A1(n11), .A2(n12), .ZN(N22) );
  NAND2_X1 U14 ( .A1(N2), .A2(n10), .ZN(n12) );
  NAND2_X1 U15 ( .A1(N6), .A2(N3), .ZN(n10) );
  NAND2_X1 U16 ( .A1(N1), .A2(N3), .ZN(n11) );
endmodule
