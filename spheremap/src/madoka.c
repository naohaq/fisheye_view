/* -*- mode: c; coding: utf-8-unix -*- */
/**
 * @file madoka.c
 * 
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef char char_t;

#include "vector.h"
#include "madoka.h"

#ifndef M_PI
#define M_PI (3.141592653589793238462643)
#endif

typedef struct {
	double x_off;
	double c[17];
} taylor_coeffs_t;

static const taylor_coeffs_t s_taylor_tbl[] = {
	{
		0.0, {
			1.3840984129331592e-08, 9.6040011895449617e-01, 5.1116929413954208e-07, -1.5372656227450809e-01,
			3.1463808838730799e-06, 7.3874280708949285e-03, 7.7467193588505820e-06, -1.5929102292731493e-04,
			1.0217804264595640e-05, 1.2008917954398897e-05, 8.3857681807479094e-06, 6.5321928924754065e-06,
			4.6924181307077838e-06, 3.1023028115384818e-06, 1.9043777517524689e-06, 1.0911275242403207e-06,
			5.8609719152569624e-07,
		}
	},

	{
		4.7123889803846897e-01, {
			4.3666225836665035e-01, 8.5980074833673725e-01, -2.0965549516127452e-01, -1.3754029451287622e-01,
			1.6962370554440245e-02, 6.9191398079565310e-03, -9.2598606744389902e-05, 3.9479720911576448e-04,
			5.9568965763824415e-04, 5.6205907384099252e-04, 4.8122344878440989e-04, 3.7604202710729827e-04,
			2.6933358345024441e-04, 1.7805745403513541e-04, 1.0930643642964934e-04, 6.2627976477660011e-05,
			3.3640488390158518e-05,
		}
	},

	{
		7.8539816339744828e-01, {
			6.8200636167424566e-01, 6.8979009050162021e-01, -3.2705720981036251e-01, -1.0914546169779511e-01,
			2.8897612897409952e-02, 9.9201691081785559e-03, 5.7770706775421322e-03, 8.0018717457296419e-03,
			8.7409955372048844e-03, 8.3349111442441812e-03, 7.1617852350972970e-03, 5.5956522384098964e-03,
			4.0076019392342272e-03, 2.6494456223581753e-03, 1.6264505001483147e-03, 9.3188749543492470e-04,
			5.0056144213808077e-04,
		}
	},

	{
		1.0995574287564276e+00, {
			8.6336784613827089e-01, 4.5623555099462987e-01, -4.0800875178266893e-01, -5.4176394577861477e-02,
			7.3158927132578044e-02, 7.2223875284488231e-02, 9.7384573176770003e-02, 1.2078952179601218e-01,
			1.2986775881245022e-01, 1.2399824797460944e-01, 1.0656753354453427e-01, 8.3262004890021835e-02,
			5.9632026505367593e-02, 3.9423038844387186e-02, 2.4201147186256912e-02, 1.3866235935487720e-02,
			7.4482199705089092e-03,
		}
	},

	{
		1.3744467859455345e+00, {
			9.5741908354882810e-01, 2.2923414377943283e-01, -3.8988456364440061e-01, 1.6346456084311453e-01,
			4.6126009422933228e-01, 7.3136281677684400e-01, 1.0440772724411356e+00, 1.2832892656747656e+00,
			1.3786927687628612e+00, 1.3165358610534650e+00, 1.1314785815346482e+00, 8.8403126371174390e-01,
			6.3314075929446345e-01, 4.1857261451443406e-01, 2.5695475959774822e-01, 1.4722423251210665e-01,
			7.9081192171940554e-02,
		}
	},

	{
		1.6179202165987434e+00, {
			9.9502989110589968e-01, 1.1591025803588524e-01, 8.8478108481667539e-02, 1.6037521692547481e+00,
			3.4785910671208069e+00, 5.9144171074612473e+00, 8.4707289890106878e+00, 1.0401561696078669e+01,
			1.1174372727178536e+01, 1.0670720155506290e+01, 9.1708082470715286e+00, 7.1652084537786438e+00,
			5.1317025480595921e+00, 3.3925949726332343e+00, 2.0826575735196120e+00, 1.1932748913411688e+00,
			6.4096514130735005e-01,
		}
	}
};

static double
taylor_approx(int32_t idx, double x)
{
	double a = s_taylor_tbl[idx].x_off;
	double xx = x - a;
	double w0  = s_taylor_tbl[idx].c[16];
	double w1  = w0  * xx + s_taylor_tbl[idx].c[15];
	double w2  = w1  * xx + s_taylor_tbl[idx].c[14];
	double w3  = w2  * xx + s_taylor_tbl[idx].c[13];
	double w4  = w3  * xx + s_taylor_tbl[idx].c[12];
	double w5  = w4  * xx + s_taylor_tbl[idx].c[11];
	double w6  = w5  * xx + s_taylor_tbl[idx].c[10];
	double w7  = w6  * xx + s_taylor_tbl[idx].c[ 9];
	double w8  = w7  * xx + s_taylor_tbl[idx].c[ 8];
	double w9  = w8  * xx + s_taylor_tbl[idx].c[ 7];
	double w10 = w9  * xx + s_taylor_tbl[idx].c[ 6];
	double w11 = w10 * xx + s_taylor_tbl[idx].c[ 5];
	double w12 = w11 * xx + s_taylor_tbl[idx].c[ 4];
	double w13 = w12 * xx + s_taylor_tbl[idx].c[ 3];
	double w14 = w13 * xx + s_taylor_tbl[idx].c[ 2];
	double w15 = w14 * xx + s_taylor_tbl[idx].c[ 1];
	double w16 = w15 * xx + s_taylor_tbl[idx].c[ 0];

	return w16;
}

double
madoka_theta_to_radius(double th)
{
	double y = 0.0;
	if (th <= 0.3*M_PI) {
		if (th <= 0.085*M_PI) {
			y = taylor_approx(0, th);
		}
		else if (th <= 0.2*M_PI) {
			y = taylor_approx(1, th);
		}
		else {
			y = taylor_approx(2, th);
		}
	}
	else {
		if (th <= 0.4*M_PI) {
			y = taylor_approx(3, th);
		}
		else if (th <= 0.48*M_PI) {
			y = taylor_approx(4, th);
		}
		else {
			y = taylor_approx(5, th);
		}
	}

	return y;
}


/*
 * Local Variables:
 * indent-tabs-mode: t
 * tab-width: 4
 * End:
 */
