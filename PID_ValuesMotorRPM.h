int rpmLow = 102;
int rpmMid = 120;
int rpmHigh = 158;
//                 PID CONSTANTS                       //
/////////////////////////////////////////////////////////
float KpL = 1.0;
float KpR = 1.0;
float KiL = 1.0;
float KiR = 1.0;
float KdL = 1.0;
float KdR = 1.0;
/////////////////////////////////////////////////////////
float KpLowShooting = 0.0675;
float KiLowShooting = 0.00675;//0.002103;//0.002103;//0.0007
float KdLowShooting = 0.6753;//0.7572413800000;//0.7572413800000;//0.0021
float KpLow = 0.01900500;
float KiLow = 0.00000200;
float KdLow = 0.05000000;
/////////////////////////////////////////////////////////
float KpMid = 0.018;
float KiMid = 0.00350;//0.00050;
float KdMid = 0.4;//0.008;
/////////////////////////////////////////////////////////
float KpHighRS = 0.01600000; // 0.10
float KiHighRS = 0.0024000; // 0.0024
float KdHighRS  = 0.002000; // 0.0048
float KpHighLS = KpHighRS;
float KiHighLS = KiHighRS;
float KdHighLS = KdHighRS;
float KpHighL = KpHighRS;
float KiHighL = KiHighRS;
float KdHighL = KdHighRS;
float KpHighR = KpHighRS;
float KiHighR = KiHighRS;
float KdHighR = KdHighRS;
/////////////////////////////////////////////////////////

//             PID CONSTANTS SOFT                      //
/////////////////////////////////////////////////////////
float KpLowSoft = 0.01192500;
float KiLowSoft = 0.00000900;
float KdLowSoft = 0.92000000;
float KpLowShootingSoft = 0.01292500;
float KiLowShootingSoft = 0.000000900;
float KdLowShootingSoft = 0.92000000;
/////////////////////////////////////////////////////////
float KpMidSoft = 0.01240000;
float KiMidSoft = 0.00050;//0.00050;
float KdMidSoft = 0.5;//0.008;
/////////////////////////////////////////////////////////
float KpHighRSSoft = 0.01600000; // 0.10
float KiHighRSSoft = 0.0024000; // 0.0024
float KdHighRSSoft  = 0.002000; // 0.0048
float KpHighLSSoft = KpHighRSSoft;
float KiHighLSSoft = KiHighRSSoft;
float KdHighLSSoft = KdHighRSSoft;
float KpHighLSoft = KpHighRSSoft;
float KiHighLSoft = KiHighRSSoft;
float KdHighLSoft = KdHighRSSoft;
float KpHighRSoft = KpHighRSSoft;
float KiHighRSoft = KiHighRSSoft;
float KdHighRSoft = KdHighRSSoft;
/////////////////////////////////////////////////////////
