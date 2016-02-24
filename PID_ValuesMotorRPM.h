int rpmLow = 104;
int rpmMid = 140;
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
/*PID Tuner
p = .0235;
i = .000007
d= .42
*/
float KpLowShooting = 0.0205454545;
float KiLowShooting = 0.00115;//0.002103;//0.002103;//0.0007
float KdLowShooting = 0.5434;//0.7572413800000;//0.7572413800000;//0.0021
float KpLow = 0.000900500;
float KiLow = 0.000000200;
float KdLow = 0.005000000;
/////////////////////////////////////////////////////////
float KpMid = 0.0160000;
float KiMid = 0.00240;//0.00050;
float KdMid = 0.002;//0.008;
/////////////////////////////////////////////////////////
float KpHighRS = 0.01600000; // 0.10
float KiHighRS = 0.0024000; // 0.0024
float KdHighRS  = 0.002000; // 0.0048

/* final mateo values
float KpHighRS = 0.010600000;
float KiHighRS = 0.00240;
float KdHighRS  = 0.0030000;

/*
most bad shots shoot just a bit too strong, but every once in a while there is a short shot
raise the d a little more to correct overshot, and eitehr raise the p or i to fix the every once in a while short shots
/*
BEST
float KpHighRS = 0.021000000;//.008
float KiHighRS = 0.00202;//.0007
float KdHighRS  = 0.0042000;// .009//.00155

float KpHighRS = 0.018000000;//.008
float KiHighRS = 0.000200;//.00202
float KdHighRS  = 0.004040000;// .009//.00155*/

/* good with 1575/1582
float KpHighRS = 0.020000000;//.008
float KiHighRS = 0.00202;//.0007
float KdHighRS  = 0.000042000;// .009//.00155
*/

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

/* final mateo values
float KpHighRS = 0.010600000;
float KiHighRS = 0.00240;
float KdHighRS  = 0.0030000;

/*
most bad shots shoot just a bit too strong, but every once in a while there is a short shot
raise the d a little more to correct overshot, and eitehr raise the p or i to fix the every once in a while short shots
/*
BEST
float KpHighRS = 0.021000000;//.008
float KiHighRS = 0.00202;//.0007
float KdHighRS  = 0.0042000;// .009//.00155

float KpHighRS = 0.018000000;//.008
float KiHighRS = 0.000200;//.00202
float KdHighRS  = 0.004040000;// .009//.00155*/

/* good with 1575/1582
float KpHighRS = 0.020000000;//.008
float KiHighRS = 0.00202;//.0007
float KdHighRS  = 0.000042000;// .009//.00155
*/

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
