/*	$OpenBSD: zbus.c,v 1.4 1996/04/21 22:15:50 deraadt Exp $	*/
/*	$NetBSD: zbus.c,v 1.17 1996/03/28 18:41:49 is Exp $	*/

/*
 * Copyright (c) 1994 Christian E. Hopps
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by Christian E. Hopps.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <sys/param.h>
#include <sys/device.h>
#include <sys/systm.h>
#include <machine/cpu.h>
#include <machine/pte.h>
#include <amiga/amiga/cfdev.h>
#include <amiga/amiga/device.h>
#include <amiga/dev/zbusvar.h>

struct aconfdata {
	char *name;
	int manid;
	int prodid;
};

struct preconfdata {
	int manid;
	int prodid;
	caddr_t vaddr;
};


/* 
 * explain the names.. 0123456789 => zothfisven
 */
static struct aconfdata aconftab[] = {
	/* Commodore Amiga */
	{ "atfsc",	514,	84 },
	{ "atzee",	513,	1 },
	{ "atzsc",	514,	3 },
	{ "bah",	514,	9 },	/* A2060 */
	{ "le",		514,	112 },
	{ "ql",		514,	69 },
	{ "ql",		514,	70 },
	/* Ameristar */
	{ "le",		1053,	1 },
	{ "bah",	1053,	9 },	/* A2060 */
	{ "es",		1053,	10 },
	/* University of Lowell */
	{ "grful",	1030,	0 },
	/* Macrosystems */
	{ "grfrt",	18260,	6 },
	{ "grfrh",	18260,	16},	/* Retina BLT Z3 */
	{ "grfrh",	18260,	19},	/* Altais */
	/* Greater valley products */
	{ "gvpbus",	2017,	2 },
	{ "gvpbus",	2017,	11 },
	{ "giv",	2017,	32 },
	{ "gio",	2017,	255 },
	/* progressive perhiperals */
	{ "zssc",	2026,	150 },
	{ "ppia",	2026,	187 },
	{ "ppta",	2026,	105 },
	{ "ppha",	2026,	1 },
	{ "mrsc",	2026,	0 },
	/* CSA */
	{ "mgnsc",	1058,	17 },
	{ "otgsc",	1058,	21 },
	/* Microbotics */
	{ "vhzsc",	1010,	69 },
	/* Supra */
	{ "wstsc",	1056,	12 },
	{ "wstsc",	1056,	13 },
	/* IVS */
	{ "itrmp",	2112,	48 },
	{ "itrmp",	2112,	52 },
	{ "ivasc",	2112,	242 },
	{ "ivsc",	2112,	243 },
	/* Hydra */
	{ "ed",		2121,	1 },
	/* ASDG */
	{ "ed",		9999,	9 },		/* XXXX */
	/* Village Tronic Ariadne */
	{ "ae",		2167,	201},
	/* bsc/Alf Data */
	{ "mfc",	2092,	16 },
	{ "mfc",	2092,	17 },
	{ "mfc",	2092,	18 },
	/* Cirrus CL GD 5426 -> Picasso, Piccolo, EGS Spectrum */
	{ "grfcl",	2167,	11},	/* Picasso-II mem*/
	{ "grfcl",	2167,	12},	/* regs */
	{ "grfcl",	2193,	2},	/* Spectrum mem */
	{ "grfcl",	2193,	1},	/* Spectrum regs */
	{ "grfcl",	2195,	5},	/* Piccolo mem */
	{ "grfcl",	2195,	6},	/* Piccolo regs */
	/* MacroSystemsUS */
	{ "wesc",	2203,	19},	/* Warp engine */
	/* phase 5 digital products */
	{ "flmem",	8512,	10},	/* FastlaneZ3 memory */
	{ "flsc",	8512,	11},	/* FastlaneZ3 */
	{ "grfcv",	8512,	34},	/* CyberVison 64 */
	/* Commodore Amiga */
	{ "afsc",	514,	84},	/* A4091 SCSI HD Controller */
	/* Hacker Inc. */
	{ "mlhsc",	2011,	1 },
	/* Resource Management Force */
	{ "qn",		2011,	2 },	/* QuickNet Ethernet */
	/* ??? */
	{ "empsc",	2171,	21 }	/* Emplant SCSI */
};
static int naconfent = sizeof(aconftab) / sizeof(struct aconfdata);

/*
 * Anything listed in this table is subject to pre-configuration,
 * if autoconf.c:config_console() calls amiga_config_found() on
 * the Zorro III device.
 */
static struct preconfdata preconftab[] = {
	{18260, 6, 0 },	/* Retina Z2 */
	{18260, 16, 0}, /* Retina BLT Z3 */
	{18260, 19, 0}, /* Altais */
	{2167,	11, 0},	/* Picasso-II mem*/
	{2167,	12, 0},	/* regs */
	{2193,	2, 0},	/* Spectrum mem */
	{2193,	1, 0},	/* Spectrum regs */
	{2195,	5, 0},	/* Piccolo mem */
	{2195,	6, 0},	/* Piccolo regs */
	{1030,	0, 0},	/* Ulwl board */
	{8512,	34, 0}	/* Cybervison 64 */
};
static int npreconfent = sizeof(preconftab) / sizeof(struct preconfdata);


void zbusattach __P((struct device *, struct device *, void *));
int zbusprint __P((void *, char *));
int zbusmatch __P((struct device *, void *, void *));
caddr_t zbusmap __P((caddr_t, u_int));
static char *aconflookup __P((int, int));

/*
 * given a manufacturer id and product id, find the name
 * that describes this board.
 */
static char *
aconflookup(mid, pid)
	int mid, pid;
{
	struct aconfdata *adp, *eadp;

	eadp = &aconftab[naconfent];
	for (adp = aconftab; adp < eadp; adp++)
		if (adp->manid == mid && adp->prodid == pid)
			return(adp->name);
	return("board");
}

/* 
 * mainbus driver 
 */

struct cfattach zbus_ca = {
	sizeof(struct device), zbusmatch, zbusattach
};

struct cfdriver zbus_cd = {
	NULL, "zbus", DV_DULL, NULL, 0
};

static struct cfdata *early_cfdata;

/*ARGSUSED*/
int
zbusmatch(pdp, match, auxp)
	struct device *pdp;
	void *match, *auxp;
{
	struct cfdata *cfp = match;

	if (matchname(auxp, "zbus") == 0)
		return(0);
	if (amiga_realconfig == 0)
		early_cfdata = cfp;
	return(1);
}

/*
 * called to attach bus, we probe, i.e., scan configdev structs passed
 * in, for each found name call config_found() which will do this again
 * with that driver if matched else print a diag.
 */
void
zbusattach(pdp, dp, auxp)
	struct device *pdp, *dp;
	void *auxp;
{
	struct zbus_args za;
	struct preconfdata *pcp, *epcp;
	struct cfdev *cdp, *ecdp;

	epcp = &preconftab[npreconfent];
	ecdp = &cfdev[ncfdev];
	if (amiga_realconfig) {
		if (ZTWOMEMADDR)
			printf(": mem 0x%08lx-0x%08lx",
			    ZTWOMEMADDR, ZTWOMEMADDR + NBPG * NZTWOMEMPG - 1);
		if (ZBUSAVAIL)
			printf (": i/o size 0x%08x", ZBUSAVAIL);
		printf("\n");
	}
	for (cdp = cfdev; cdp < ecdp; cdp++) {
		for (pcp = preconftab; pcp < epcp; pcp++) {
			if (pcp->manid == cdp->rom.manid && 
			    pcp->prodid == cdp->rom.prodid)
				break;
		}
		if (amiga_realconfig == 0 && pcp >= epcp)
			continue;

		/*
		 * check if it's a Zorro II or III board and not linked into
		 * MemList (i.e. not a memory board)
		 */
		if ((cdp->rom.type & 0xe0) != 0xc0 &&
		    (cdp->rom.type & 0xe0) != 0x80)
			continue;	/* er_Type != Zorro I/O */

		za.pa = cdp->addr;
		za.size = cdp->size;
		if (amiga_realconfig && pcp < epcp && pcp->vaddr)
			za.va = pcp->vaddr;
		else {
			za.va = (void *) (isztwopa(za.pa) ? ztwomap(za.pa) :
			    zbusmap(za.pa, za.size));
/*                          ??????? */
			/*
			 * save value if early console init 
			 */
			if (amiga_realconfig == 0)
				pcp->vaddr = za.va;
		}
		za.manid = cdp->rom.manid;
		za.prodid = cdp->rom.prodid;
		za.serno = cdp->rom.serno;
		za.slot = (((u_long)za.pa >> 16) & 0xF) - 0x9;
		amiga_config_found(early_cfdata, dp, &za, zbusprint);
	}
}

/*
 * print configuration info.
 */
int
zbusprint(auxp, pnp)
	void *auxp;
	char *pnp;
{
	struct zbus_args *zap;
	int rv;

	rv = UNCONF;
	zap = auxp;

	if (pnp) {
		printf("%s at %s:", aconflookup(zap->manid, zap->prodid),
		    pnp);
		if (zap->manid == -1)
			rv = UNSUPP;
	}
	printf(" rom %p man/pro %d/%d", zap->pa, zap->manid, zap->prodid);
	return(rv);
}

/*
 * this function is used to map Zorro physical I/O addresses into kernel
 * virtual addresses. We don't keep track which address we map where, we don't
 * NEED to know this. We made sure in amiga_init.c (by scanning all available
 * Zorro devices) to have enough kva-space available, so there is no extra
 * range check done here.
 */
caddr_t
zbusmap (pa, size)
	caddr_t pa;
	u_int size;
{
	static vm_offset_t nextkva = 0;
	vm_offset_t kva;

	if (nextkva == 0)
		nextkva = ZBUSADDR;

	if (nextkva > ZBUSADDR + ZBUSAVAIL)
		return 0;

	/* size better be an integral multiple of the page size... */
	kva = nextkva;
	nextkva += size;
	if (nextkva > ZBUSADDR + ZBUSAVAIL)
		panic("allocating too much Zorro I/O address space");
	physaccess((caddr_t)kva, (caddr_t)pa, size, PG_RW|PG_CI);
	return((caddr_t)kva);
}
