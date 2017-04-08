module seq19(a, b, f, RESET, CLK);

    input a, b, RESET, CLK;
    output f;

    wire c, d, e, g1, g2;

    NAND2_X1 U1 ( .A1(a), .A2(g1), .ZN(c) );
    NAND2_X1 U2 ( .A1(b), .A2(g2), .ZN(d) );
    NOR2_X1 U3 ( .A1(c), .A2(d), .ZN(e) );
    INV_X1 U4 ( .A(e), .ZN(f) );
    DFF_X1 reg_2 ( .D(f), .CK(CLK), .Q(g1), .QN(g2));

endmodule
