
module adder ( Co2, sum, A, B );
  output [1:0] sum;
  input [1:0] A;
  input [1:0] B;
  output Co2;
  wire   Co1;

  FullAdder_1 FA0 ( .co(Co1), .sum(sum[0]), .a(A[0]), .b(B[0]), .ci(1'b0) );
  FullAdder_0 FA1 ( .co(Co2), .sum(sum[1]), .a(A[1]), .b(B[1]), .ci(Co1) );
endmodule

module FullAdder_0 ( co, sum, a, b, ci );
  input a, b, ci;
  output co, sum;
  wire   n6, n1, n3, n4, n5;

  INV_X1 U1 ( .A(n6), .ZN(n1) );
  INV_X1 U2 ( .A(n1), .ZN(sum) );
  XOR2_X1 U3 ( .A(a), .B(n3), .Z(n6) );
  OR2_X1 U4 ( .A1(n4), .A2(n5), .ZN(co) );
  AND2_X1 U5 ( .A1(b), .A2(ci), .ZN(n5) );
  AND2_X1 U6 ( .A1(a), .A2(n3), .ZN(n4) );
  XOR2_X1 U7 ( .A(b), .B(ci), .Z(n3) );
endmodule

module FullAdder_1 ( co, sum, a, b, ci );
  input a, b, ci;
  output co, sum;
  wire   n6, n1, n3, n4, n5;

  INV_X1 U1 ( .A(n6), .ZN(n1) );
  INV_X1 U2 ( .A(n1), .ZN(sum) );
  XOR2_X1 U3 ( .A(a), .B(n3), .Z(n6) );
  OR2_X1 U4 ( .A1(n4), .A2(n5), .ZN(co) );
  AND2_X1 U5 ( .A1(b), .A2(ci), .ZN(n5) );
  AND2_X1 U6 ( .A1(a), .A2(n3), .ZN(n4) );
  XOR2_X1 U7 ( .A(b), .B(ci), .Z(n3) );
endmodule
