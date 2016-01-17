;********************************************************************
;Systemcontrolregister

CustomBase	equ	$dff000

DDFSTRT	equ	$92
DDFSTOP	equ	$94
DIWSTRT	equ	$8e
DIWSTOP	equ	$90
DMACON	equ	$96
DMACONR	equ	$2
INTENA	equ	$9a
INTENAR	equ	$1c
INTREQ	equ	$9c
INTREQR	equ	$1e

;********************************************************************
;Bitplaneregister

BPLCON0	equ	$100
BPLCON1	equ	$102
BPLCON2	equ	$104
BPL1MOD	equ	$108
BPL2MOD	equ	$10a

;********************************************************************
;Copperregister

COPINS1	equ	$80
COPJMP1	equ	$88

;********************************************************************
;Spriteregister

SPR0DAT	equ	$144

;********************************************************************
;Colorregister

COLOR00	equ	$180

;********************************************************************
;Memoryvariablen

MEMF_CHIP	equ	2
MEMF_CLEAR	equ	$10000

;********************************************************************
;exec.library

ExecBase	equ	4
AllocMem	equ	-198
FreeMem		equ	-210
OpenLibrary	equ	-552
CloseLibrary	equ	-414
Forbid		equ	-132
Permit		equ	-138

;********************************************************************
Macros

mouse	macro
1$	btst	#6,$bfe001
	bne.s	1$
	endm

sys	macro
	move.l	ExecBase.w,a6
	jsr	\1(a6)
	endm

;********************************************************************
;Code

	section	"code",code

	bsr	GETPARAM
	bsr	GETPLANES
	bsr	MAKECLIST
	sys	Forbid
	lea	CustomBase,a5
	move.w	DMACONR(a5),_dmacon
	move.w	INTENAR(a5),_intena
	move.w	#$7fff,d0
	move.w	d0,DMACON(a5)
	move.w	d0,INTENA(a5)
	move.w	d0,INTREQ(a5)
	move.l	$6c,_vbi
	move.l	#VBI,$6c
	bsr	INITSYS
	bsr	INITCL
	bsr	SETCOLOR
	move.w	#$8380,DMACON(a5)

	jsr	SQUARE
	tst.l	d0
	beq.s	rstcl
	movea.l	d0,a5
	lea	mandwindow(pc),a0
	movem.w	(a0),d0-d3
	jsr	APFEL2D

	mouse
	lea	$dff000,a5
	jsr	FREE_SQR
rstcl	bsr	RESTORECL
	bsr	FREEPLANES
	move.l	_vbi(pc),$6c
	move.w	_intena(pc),d0
	or.w	#$8000,d0
	move.w	d0,INTENA(a5)
	move.w	_dmacon(pc),d0
	or.w	#$8000,d0
	move.w	d0,DMACON(a5)
	sys	Permit
mn_end	bsr	t_out
	moveq	#0,d0
	rts

_dmacon	dc.w	0
_intena	dc.w	0
_vbi	dc.l	0

;********************************************************************
;Zeit ausgeben

t_out	lea	dosname(pc),a1
	moveq	#33,d0
	sys	OpenLibrary
	move.l	d0,dosbase
	beq.s	t_end
	movea.l	d0,a6
	jsr	-60(a6)
	move.l	d0,-(sp)
	bsr	itoa
	move.l	(sp)+,d1
	move.l	#ascii,d2
	move.l	d0,d3
	jsr	-48(a6)
	movea.l	a6,a1
	sys	CloseLibrary
t_end	rts

dosbase	ds.l	1
dosname	dc.b	"dos.library",0

	even

itoa	lea	ascii(pc),a0
	movea.l	a0,a1
	add.w	#16,a0
	move.b	#0,-(a0)
	move.l	time(pc),d0
ia_loop	cmp.l	#9,d0
	bcs.s	ia_end
	divs	#10,d0
	move.l	d0,d1
	swap	d1
	add.b	#$30,d1
	move.b	d1,-(a0)
	ext.l	d0
	bra.s	ia_loop
ia_end	add.b	#$30,d0
	move.b	d0,-(a0)
	moveq	#0,d0
ia_copy	addq.w	#1,d0
	move.b	(a0)+,(a1)+
	bne.s	ia_copy
	move.b	#10,-1(a1)
	move.b	#0,(a1)
	rts

ascii	ds.b	16

;********************************************************************
;Zeitzähler

VBI	addq.l	#1,time
	move.w	#$20,INTREQ+CustomBase
	rte

time	dc.l	0

;********************************************************************

GETPARAM
	subq.w	#1,d0
	beq	pm_org
	move.w	d0,d1
	lea	linea0_d0(pc),a1
ln_ad	move.b	(a0)+,(a1)+
	dbf	d1,ln_ad
	lea	linea0_d0(pc),a0
	move.b	#0,(a0,d0.w)
	lea	strp(pc),a1
	move.l	a0,(a1)+
nextlt	move.b	(a0)+,d1
	subq.w	#1,d0
	bmi.s	ln_end
	cmp.b	#$20,d1
	bne.s	nextlt
	move.b	#0,-1(a0)
	move.l	a0,(a1)+
	bra.s	nextlt
ln_end	moveq	#3,d2
	lea	strp(pc),a1
	lea	mandwindow(pc),a2
p_l	movea.l	(a1)+,a0
	bsr	pack
	move.w	d0,(a2)+
	dbf	d2,p_l
	move.w	d2,d0
	rts

linea0_d0	ds.b	256

strp		ds.l	4
mandwindow	ds.w	4

pm_org	lea	mandwindow(pc),a0
	move.w	#-9216,(a0)+
	move.w	#3072,(a0)+
	move.w	#6144,(a0)+
	move.w	#-6144,(a0)+
	rts

pack	movem.l	d2-d5,-(sp)
	moveq	#0,d2
	move.b	(a0),d0
	cmp.b	#$2d,d0
	bne.s	not_neg
	moveq	#1,d2
	addq.w	#1,a0
not_neg	cmp.b	#$2b,d0
	bne.s	not_pos
	addq.w	#1,a0
not_pos	cmp.b	#$2e,d0
	bne.s	nkma
	addq.w	#1,a0
nkma	moveq	#-1,d3
nkomma	addq.w	#1,d3
	move.b	(a0)+,d0
	bne.s	nkomma
	subq.w	#1,a0
	moveq	#1,d4
	moveq	#0,d5
pck_l	subq.w	#1,d3
	bmi.s	p_end
	move.b	-(a0),d0
	sub.b	#$30,d0
	bmi.s	komma
	and.w	#$ff,d0
	mulu	d4,d0
	mulu	#10,d4
	add.l	d0,d5
	bra.s	pck_l
komma	move.l	d4,d1
	bra.s	pck_l
p_end	mulu	#4096,d5
	divu	d1,d5
	move.w	d2,d0
	beq.s	ppos
	neg.w	d5
ppos	move.w	d5,d0
	movem.l	(sp)+,d2-d5
	rts
	

;********************************************************************
;Speicher für Bitplanes besorgen

GETPLANES
	move.l	#40*256*5,d0
	move.l	#MEMF_CLEAR+MEMF_CHIP,d1
	sys	AllocMem
	move.l	d0,screen
	rts

screen	dc.l	0

;********************************************************************
;Speicher für Bitplanes freigeben

FREEPLANES
	movea.l	screen(pc),a1
	move.l	#40*256*5,d0
	sys	FreeMem
	rts

;********************************************************************
;CList erstellen

MAKECLIST
	lea	clist,a0
	move.l	screen(pc),d0
	move.w	#$e0,d1
	moveq	#4,d2
2$	move.w	d1,(a0)+
	addq.w	#2,d1
	swap	d0
	move.w	d0,(a0)+
	move.w	d1,(a0)+
	addq.w	#2,d1
	swap	d0
	move.w	d0,(a0)+
	add.l	#40*256,d0
	dbf	d2,2$
	move.l	#$fffffffe,(a0)
	rts

;********************************************************************
;Farben

SETCOLOR
	movem.l	d2-d3,-(sp)
	lea	$180(a5),a0
	moveq	#0,d2
	moveq	#15,d0
f1_lp	move.w	d0,d1
	lsl.w	#4,d1
	or.w	d0,d1
	move.w	d2,d3
	lsl.w	#8,d3
	or.w	d3,d1
	move.w	d1,(a0)+
	move.w	d2,d1
	lsl.w	#4,d1
	or.w	#$f00,d1
	move.w	d1,$1e(a0)
	addq.w	#1,d2
	dbf	d0,f1_lp
	move.w	#0,$180(a5)
	movem.l	(sp)+,d2-d3
	rts

;********************************************************************
;System initialisieren

INITSYS
	move.w	#$38,DDFSTRT(a5)
	move.w	#$d0,DDFSTOP(a5)
	move.w	#$2c81,DIWSTRT(a5)
	move.w	#$2cc1,DIWSTOP(a5)
	move.w	#$5200,BPLCON0(a5)
	move.w	#0,BPLCON1(a5)
	move.w	#0,BPLCON2(a5)
	move.w	#0,BPL1MOD(a5)
	move.w	#0,BPL2MOD(a5)
	move.w	#0,SPR0DAT(a5)
	rts

;********************************************************************
;Clist reinstallieren

RESTORECL
	lea	gfxname(pc),a1
	moveq	#33,d0
	sys	OpenLibrary
	movea.l	d0,a1
	move.l	38(a1),COPINS1(a5)
	move.w	#0,COPJMP1(a5)
	sys	CloseLibrary
	rts

gfxname	dc.b	"graphics.library",0

	even

;********************************************************************
;Clist installieren

INITCL
	lea	clist,a0
	move.l	a0,COPINS1(a5)
	move.w	#0,COPJMP1(a5)
	rts

;********************************************************************
;Copperlist

	section	"clist",data,chip

clist	ds.w	10*4
	dc.l	$fffffffe

;********************************************************************
;Bildschirmzeile zeichnen

DRAWLN	macro
	moveq	#19,d0			;Wortzähler
w_loop	moveq	#1,d3
	subq.w	#2,a6
	moveq	#15,d1			;Bitzähler
b_loop	movea.l	a6,a3
	move.w	-(a7),d2
	bmi.s	mandf
	and.w	#$1f,d2
bpl_lp	lsr.w	#1,d2
	bcc.s	no_bit
	or.w	d3,(a3)
no_bit	adda.w	#40*256,a3
	bne.s	bpl_lp
mandf	add.w	d3,d3
	dbf	d1,b_loop
	dbf	d0,w_loop
	endm

;********************************************************************
;Iterationsschleife

ITERATE	macro
	move.w	#$4000,d4
2$	move.w	d0,d2
	add.w	d2,d2
	move.w	(a5,d2.w),d2
	move.w	d1,d3
	add.w	d3,d3
	move.w	(a5,d3.w),d3
	add.w	d3,d2
	cmp.l	d4,d2
	bgt.s	3$
	muls	d0,d1
	asr.l	#8,d1
	asr.l	#3,d1
	add.w	a1,d1
	add.w	d3,d3
	sub.w	d3,d2
	move.w	d2,d0
	add.w	a0,d0
	dbf	d5,2$
3$
	endm

;********************************************************************
;Routine für Apfelmännchen 2d
;d0 = rmin
;d1 = rmax
;d2 = imin
;d3 = imax
;a5 = Quadrattabelle

;In der Berechnung:
;d7 = h_kord, d6 = v_kord, d5 = itschritt
;d0 = real,  d1 = imag
;d2 = realq, d3 = imagq
;a0 = real0, a1 = imag0
;a2 = dreal
;a4 = reall
;a6 = Screenadresse
;a7 = Farbadresse

ITMAX		equ	49

	section	"apfel2d",code

APFEL2D	movem.l	d2-d7/a2-a6,-(sp)	;Register retten
	move.l	a7,stack		;SP retten
	movea.l	screen,a6		;a6 = Screenbase
	adda.w	#40*256,a6
	lea	color(pc),a7		;a7 = Farbtabelle
	move.w	d0,r_lo			;rl      = r- (d0)
	movea.w	d3,a1			;i0 (a1) = i+ (d3)
	sub.w	d0,d1
	ext.l	d1
	divs	#320,d1
	movea.w	d1,a2			;dr (a2) = (r+ (d1) - r- (d0)) / HRES
	sub.w	d2,d3
	ext.l	d3
	divs	#256,d3
	move.w	d3,di			;di      = (i+ (d3) - i- (d2)) / VRES
	moveq	#0,d2
	moveq	#0,d3

	clr.l	time
	move.w	#$c020,INTENA+CustomBase;Zeitzähler ein

	move.w	#255,d7			;d7 = vcount
v_loop	movea.w	r_lo(pc),a0		;r0 (a0) = rl
	swap	d7
	suba.w	a2,a0
	move.w	a0,d0			;r  (d0) = r0 (a0)
	move.w	a1,d1			;i  (d1) = i0 (a1)
	moveq	#ITMAX,d5		;d5 = itcount
	ITERATE
	move.w	d5,d7
	moveq	#39,d6			;d6 = hcount
h_loop	move.w	a2,d0
	asl.w	#3,d0
	add.w	d0,a0
	move.w	a0,d0
	move.w	a1,d1
	moveq	#ITMAX,d5
	ITERATE
	cmp.w	d5,d7
	bne.s	norm
	moveq	#7,d0
loop_8	move.w	d5,(a7)+
	dbf	d0,loop_8
	move.w	d5,d7
	dbf	d6,h_loop
	bra.s	__x
norm	move.w	d5,14(a7)
	move.w	a2,d0
	asl.w	#3,d0
	suba.w	d0,a0
	adda.w	a2,a0
	move.w	#6,d7
norm_lp	move.w	a0,d0
	move.w	a1,d1
	moveq	#ITMAX,d5
	ITERATE
	move.w	d5,(a7)+
	adda.w	a2,a0
	dbf	d7,norm_lp
	move.w	(a7)+,d7
	dbf	d6,h_loop

__x	swap	d7
	DRAWLN
	suba.w	di(pc),a1		;i0 (a1) -= di
	move.b	$bfe001,d4
	and.b	#$40,d4
	beq.s	a_end
	dbf	d7,v_loop

	move.w	#$20,INTENA+CustomBase	;Zeitzähler aus

a_end	move.l	stack(pc),a7
	movem.l	(sp)+,d2-d7/a2-a6
	rts

di	dc.w	0
r_lo	dc.w	0
stack	dc.l	0
color	ds.w	320

SQUARE
	move.l	#256*1024,d0
	moveq	#0,d1
	sys	AllocMem
	move.l	d0,sqrmem
	beq.s	sqr_end
	movea.l	d0,a0
	movea.l	d0,a1
	move.l	#131071,d0
sqr_fl	move.w	#$7fff,(a0)+
	subq.l	#1,d0
	bpl.s	sqr_fl
	movea.l	a1,a0
	adda.l	#128*1024,a0
	move.l	a0,sqr
	movea.l	a0,a1
	addq.w	#2,a1
	moveq	#0,d0
sqr_lp	move.w	d0,d1
	muls	d1,d1
	and.w	#$f000,d1
	swap	d1
	rol.l	#4,d1
	cmp.l	#$7fff,d1
	bcc.s	sqr_lpe
	move.w	d1,(a0)+
	move.w	d1,-(a1)
	addq.w	#1,d0
	bra.s	sqr_lp
sqr_lpe	move.l	sqr(pc),d0
sqr_end	rts

FREE_SQR
	move.l	#256*1024,d0
	movea.l	sqrmem(pc),a1
	sys	FreeMem
	rts

sqrmem	dc.l	0
sqr	dc.l	0

	end
