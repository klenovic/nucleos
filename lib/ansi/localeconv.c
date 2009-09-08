/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*
 * localeconv - set components of a struct according to current locale
 */
#include	<nucleos/kernel.h>
#include	<locale.h>

extern struct lconv _lc;

struct lconv *
localeconv(void)
{
	register struct lconv *lcp = &_lc;

	lcp->decimal_point = ".";
	lcp->thousands_sep = "";
	lcp->grouping = "";
	lcp->int_curr_symbol = "";
	lcp->currency_symbol = "";
	lcp->mon_decimal_point = "";
	lcp->mon_thousands_sep = "";
	lcp->mon_grouping = "";
	lcp->positive_sign = "";
	lcp->negative_sign = "";
	lcp->int_frac_digits = CHAR_MAX;
	lcp->frac_digits = CHAR_MAX;
	lcp->p_cs_precedes = CHAR_MAX;
	lcp->p_sep_by_space = CHAR_MAX;
	lcp->n_cs_precedes = CHAR_MAX;
	lcp->n_sep_by_space = CHAR_MAX;
	lcp->p_sign_posn = CHAR_MAX;
	lcp->n_sign_posn = CHAR_MAX;

	return lcp;
}
