PRAGMA foreign_keys=OFF;
BEGIN TRANSACTION;
CREATE TABLE 'VCALibs' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"DB_TBL" TEXT DEFAULT '' ,"ICO" TEXT DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO "VCALibs" VALUES('ElectroEls','','','','','wlb_ElectroEls','');
CREATE TABLE 'wlb_ElectroEls' ("ID" TEXT DEFAULT '' ,"ICO" TEXT DEFAULT '' ,"PARENT" TEXT DEFAULT '' ,"PROC" TEXT DEFAULT '' ,"en#PROC" TEXT DEFAULT '' ,"ru#PROC" TEXT DEFAULT '' ,"uk#PROC" TEXT DEFAULT '' ,"PROC_PER" INTEGER DEFAULT '-1' ,"ATTRS" TEXT DEFAULT '*' ,"DBV" INTEGER DEFAULT '1' , PRIMARY KEY ("ID"));
INSERT INTO "wlb_ElectroEls" VALUES('El_Accu','iVBORw0KGgoAAAANSUhEUgAAAEAAAAAVCAIAAAB5SH/NAAAACXBIWXMAAA1hAAANsAGkB5ZwAAAC
IklEQVRIid2XvYrqQBSAz6opBA0haAj+ISpGS1HESkEsHPEJ1MKHsLD0ZSwFg5aCKAgipLJQ0iio
CNOIWqiFZIuAhGw2d/beC0v8isCcnPnOHOakyAfHcdlsFqzJYrFwpNPpwWDw2yf5SyqViu23z/Cv
GDfQ7XYJgy/O5zP5TZqrdrvdZDIh9P+3Bi6Xy3A4NEkgV+33++l0Suh/0xGyEA7dut/vY4x1QUmS
RFE0sbTb7Xw+T1LP0K+l0WjUajVyP9ENKIqiKIp5AomHsNaP/G83QioY406n81oej0eO4+73e6/X
A4DT6RSJRCRJKpVKoijW6/XZbFYoFCRJ0u4yZL1eF4tFjPFoNNrtdi6Xi6IojLEgCPP5HCG0XC4B
YDweP5/P1y7V/4MG3G53tVp9LVerlSzLFEWlUikAeDweNE2zLBsIBKrVKsdx0WgUAEKhkHbXd2b1
mUgkgsGgw+Gw2Wy3241hGJqm/X6/3+8HgHg8rlVtNpvvhMYNOJ3OTCajjciybLfbI5HIK6JW8ng8
AMDzPAB4vV7drq/s93uMsdPpDAQCulc+nw8AWJZV5VqV6jfE8t+AvgGe50OhkC7IMIwgCCaWZDKp
zsYfMfRrSaVSFEWR+/UN5HK5crmsC8ZisUajYVK12WyGw2GTBHO/llar5XK5yP1vN0LWAyGkfEGW
ZcLgi8fjsd1uTRLIVdfr9XA4kPgRQsYNWAWE0IfV/4k/AVDqMy0I1X7BAAAAAElFTkSuQmCC','/wlb_originals/wdg_ElFigure','','','','',-1,'name;geomW;geomH;elLst;',2);
INSERT INTO "wlb_ElectroEls" VALUES('El_Converter','iVBORw0KGgoAAAANSUhEUgAAAEAAAABACAIAAAAlC+aJAAAACXBIWXMAAA1hAAANsAGkB5ZwAAAC
hklEQVRogdWaMY7CMBBFzYILGlqEqFJScIEUUCHR5QAggYSIBLeg5BqIjgYaakA4B6CKopyChnK3
yCq76zWxx7Ed+3UkTjLDnxl/BWqe5/m+j9wkiiI0mUw+3eR+v3ue16j6S5Tker0mSeL7/kfVkchA
CEmSZD6fI4TcU+ByuaRpulwus4+OKUAISdN0NpvlR1xS4Ha7xXEchuHvg84oQAiJ43ixWFDH3UiA
EPJ4PMIwrNfr1CkHSiirnNVqxTxruwLvKifHagWyiUl1LYW9CvyfmEwsVSBzCvluVYCNCvx2Clys
UyCbmO9mzn/4CZxOp8PhAApiu912u13QJRnQ6JFIAkEQBEEgEQ0UieiRPSWUdS00emRJE4O6lqJ6
BSh/D6ViBQR3qwL4CpzP5+PxCLrpZrPpdDrcZUx/D4WfwGg0Gg6HoJs2m03uGq5LE4SfAMYYY1zy
MRRyE5NJBU1c7O+hmG5iVZWTY1QBEX8PxZwC5ScmE0MKiPt7KCYUKOMUuGhXQOHEZKJXAd3RI60J
GIge6SshaX8PRYsCWruWQr0CJf09FMUKaNqtClCpgBJ/D0WZAspdmiBqEih4f68bBSXE9fev12u3
24Hu2e/3BX9+L5uASOU0Go1erwe6bbvdFlxZKgFBf48xHgwGZR5UgHwPmJ+YTCQV0OfvocgoYNIp
cAErYMZjigNLQC765/O5Xq9Bl4zH4+l0KrISkID0d99qtfb7PfQqQUQTMObvoQg1sVVdS8FXwLC/
h8JRwJLdqoAiBSrx91DeKlCVv4fCTqBCfw+FUUJq39/rhlbAlcrJ+aOAjvf3uvlRwP6JyeRbAXv8
PZQPZLdT4FLzPA8h5Og/8KMo+gIgvk7LK0/PowAAAABJRU5ErkJggg==','/wlb_originals/wdg_ElFigure','','','','',-1,'name;dscr;geomW;geomH;elLst;',2);
INSERT INTO "wlb_ElectroEls" VALUES('El_Converter_1','iVBORw0KGgoAAAANSUhEUgAAAEAAAABACAIAAAAlC+aJAAAACXBIWXMAAA1hAAANsAGkB5ZwAAAD
fElEQVRogdWav2vyQBjH07exFofSISKlQ3EqDu0fYEsFp0xC1xpQUE/8QSkUundsh06F0sFFXNSh
Q/cShauLk2ANIq4lW6miZLJDQPvGNLlLcvnxmWJyF5/H73PPfYnZCIfD0WiU8ibv7+90NBqt1WpO
R2IQjuP+OR2DWejlUSaTkSTJwVDQGY/H5XL54uKC+p2AJEmeqCWe5+v1uiiK8kePlRCEcDQapVKp
5RkvJdBut/v9fjabpelV4XgmAQihIAiZTEZx3hsJQAh7vR4AYHNzU3GJVp3gKtrttiAIhUJB9arb
Ffircpa4WgGe50ejEQBAY4x7FVjvmKq4VIFWqzUcDnO5nO5INyoAIRwOh+l0GmWw6xSQO+ZfPWcd
jASazaYkSeFw+OTkxFBs+uBGT6GU0Gw2AwBwHLe1tcUwTLFYTCaT0+nURJzqGIie0lWg0WgMBoPD
w8Pr62v5DMuyz8/P9/f3pVIpFAoZDHYNedXiRk9pJ/D29ra9vZ3NZvf393+fz+fzoihWKhUAAMMw
6xMhhJPJhGVZxCCwVq0CrRKKx+OJREIRvUwoFAIAPD4+fn19rV99enpCj57n+cFgkMvlfD4f4pTf
GG+jDMNcXV1dXl7e3t5+fHxQFPX5+fny8nJ6enpzc4N4E8TdSgNTbXR3d/fu7u719fX8/DwWiwWD
wYODg4eHh+PjY5TpskvTdgq6mN0H9vb25B61WCxomvb7/YgTdV0aItZsZIFAAGu8sY6pigM7sba/
x8VuL2RV5SyxVQEUf4+LfQqY75iq2KQAur/HxQ4FzDgFXYgrYGHHVIWsAqSjp4gmYEP0FLkSMuzv
cSGiANFVq8B6BeTdikTHVMViBQjtVhpYqYAl/h4XyxSw3KUhYk0CGs/vSWNBCen6+/l8Xq1Wse55
dHSE+Pe72QRQKoem6UgkgnVb9CdOphJA9Pc+n+/s7MzMF2lgfA3Y3zFVMagAOX+PixEF7HQKumAr
YI/HRAcvAWPRf39/F4tFrCksy3IchzISIwHDv/3Ozg6510hQE7DN3+OCtIhdtWoV6Ctgs7/HRUcB
l+xWGmgp4Ii/x+VPBZzy97ioJ+Cgv8dFpYSsfX5PGqUCXqmcJf8pQOL5PWlWCYzH43q9nkqlut2u
gwGhIAjC8niVQLlcFkWx0+k4ERI2sVhMPlglIL/J6zk2vP76/Q9XUeETTlMz6QAAAABJRU5ErkJg
gg==','/wlb_originals/wdg_ElFigure','','','','',-1,'name;dscr;geomW;geomH;elLst;',2);
INSERT INTO "wlb_ElectroEls" VALUES('El_Duga_edv','iVBORw0KGgoAAAANSUhEUgAAAEAAAABACAIAAAAlC+aJAAAACXBIWXMAAA06AAANOgEDIh6FAAAD
z0lEQVRoge2az0sqXRjHzxV/MLRoomAkMGaaoGD+gGAQBlrFbMSFko6zKVrFkES2anBRRLSoTHIR
NosYXNimaOk+iiiEEJHQqBZhRBEoLhx438WF94Vu93LOcYZJrp+VOOeZ8/04Z9Tn6A+GYXieB93J
+fm5k+d5XdftToJJLBZz2J2hU3oCdtMTsJuegN30BOym6wWcVpy02WyWy+VPTxIEwXGc6XOZLJBO
p2u1mtvt9nq9nw612+1sNgsACIVCJn59NEegUCikUimSJBVFmZmZcTqdAwMDn8YYhvH+/g4AyOfz
yWRSEIRgMGjCNZEk6Z/OODw8nJyczOVy8CUPDw/ZbHZsbKxcLncytSRJHd3ExWJxfn6epumLi4tI
JAJfODIyMjc3d3d3d3x8nEwmPz4+sDNgLqFWq5VIJGia3tvbIwgCe3pVVavV6srKit/vlyTJ4UB+
QXEEWq1WJBJhGGZ5eRmj/BMsy66urgYCAQCALMuo5TgCiUSCYZj19XWM2i/x+Xynp6fhcHhwcFAU
RaRa5EtWLBZpmt7Z2enr60Ot/QM+n0/X9cvLS9RCZIH9/f2FhQXUKhhYllUUJR6PI1WhCWiaFolE
Orlr/8zQ0BBFUaVSCb4EQaBQKBwcHExNTaEHQ4DneaRbGUEglUotLi6iR0JDEASWZTVNgxyPIECS
JNKnFTaZTOb29hZyMKxAOp1WFAU3EhokSVIUValUYAbDCtRqtdHR0Q5SIeByuTweT6PRgBkMJdBs
Nt1ut9NpSfPwJRRF1et1mJFQAuVy2ev1/voN2Tqi0Wgul4MZ2fUtZdcL/L+sVVW9v7//ctDb29vL
y8v19TXeHHjb9wzDxGKx3x2NxWLT09MAwHVkV1dX29vb2E0TXiHkybt+CUEJEATRbrcNw7A6DQZQ
AhzHPT8//9xT+G78HUsIABAKhfL5vKVR8IAV4Hn+5OTk8fHR0jQYICwhQRAKhYJ1UfBAEAgGg5ub
m9ZFwQNBgOO4s7OztbU169JggPYuNDExYRhGtVq1KA0GyG+jS0tLW1tbT09PVqTBAFmgv7/f7/cH
AoFv4oDTZEmSBAAIh8O6rrMsa3YkNHA+iR0OhyzLqqoeHR29vr6angkJ/DZXFEVRFOPxOEVRPM8L
gmBiLHg67dN3d3dLpZIsyyzLZjIZkiRdLpcpySAxYaOB47ibmxtN0zY2NiiK8ng8FEVFo9HOzwyD
aTsls7OzAIBKpdJoNOr1+n/d4PDwsFlTfInJWz3j4+M/H6D+ToHNX9MPfFt6AnbTE7CbnoDd9ATs
5ke3//3+Xynjc8QP+iJXAAAAAElFTkSuQmCC','/wlb_originals/wdg_ElFigure','','','','',-1,'owner;name;dscr;geomW;geomH;lineClr;elLst;',2);
INSERT INTO "wlb_ElectroEls" VALUES('El_Duo','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','/wlb_originals/wdg_ElFigure','','','','',-1,'owner;name;dscr;geomW;geomH;lineClr;elLst;',2);
INSERT INTO "wlb_ElectroEls" VALUES('El_KeySqr_1','iVBORw0KGgoAAAANSUhEUgAAAEAAAABACAIAAAAlC+aJAAAACXBIWXMAAA1hAAANsAGkB5ZwAAAC
F0lEQVRoge2asariUBCGz66ilUGF4GMYQjCQ4sQXEEQRFN/ASkUQK0GwFiL4DvoWJxYikagPYCpN
mghaKshuEclmvcVl2bkcvMxXZQaZP5+cEAjzo1qt6rpOoPF9fzQahWWxWKxUKuApjDFiGMavL+Bw
OEST2u32V6QYhhEPAmzbZowB/jfn8zla2rY9mUwA5+u6LssyIeQpwBhbr9eUUqiARCIRLWOxWDKZ
hBpumiYh5C8BQgiltNVqQWU4jjMcDsMyn88DDieE3G634OIn4FAuoABvUIA38AKbzYZSWq/Xo83F
YkEpHQwG4HHxz3/yj1wul+Vy+dJ0Xdd13UwmAx6HR4g3KMAbFODN2wvAvwcKhYJlWafTqVwuh81G
o9HtdtPpNHgcvIAgCIqiZLPZaDOXyymKAp5FvsERQgHeoABvUIA3KMAbFOANCvAGBXiDArxBAd6g
AG9QgDco8AHLsjRNq9Vq0eZ8Ptc0rd/vg8fBf5m7Xq+r1eql6Xme53miKILH4RHiDQrwBgV48/YC
8O8BVVW32+3xeCyVSmGz2Wz2ej1BEMDj4AVSqZQkSS/3KoqiJEngWeQbHCEU4M3bC/x5iINtUih8
34+W+/1+NptBDTdNU1XV4PopEOx/h9uk/8/9fo+Wj8cDcLiqquHC+lNAluVgkxcKx3HG43FYyrLc
6XQA54fEYXe+Q16O0G63m06n4CmMsd+wBUHpMBdF9wAAAABJRU5ErkJggg==','/wlb_originals/wdg_ElFigure','JavaLikeCalc.JavaScript
if( st && !st.isEVal() )
{
  p1x=76; p1y=46;
  p2x=86; p2y=36;
  p3x=126; p3y=76;
  p4x=116; p4y=86;
  c1 = "red"; fillColor = lineClr = "black";
}
else if( !val && !val.isEVal() )
{
  p1x=76; p1y=46;
  p2x=86; p2y=36;
  p3x=126; p3y=76;
  p4x=116; p4y=86;
  c1 = "white"; fillColor = lineClr = "black";
}
else if( val && !val.isEVal() )
{
  p1x=70; p1y=45;
  p2x=80; p2y=45;
  p3x=80; p3y=105;
  p4x=70; p4y=105;
  c1 = "lime"; fillColor = lineClr = "black";
}
else if( val.isEVal() )
{ c1 = fillColor = lineClr = "darkgrey"; }
','JavaLikeCalc.JavaScript
if( st && !st.isEVal() )
{
  p1x=76; p1y=46;
  p2x=86; p2y=36;
  p3x=126; p3y=76;
  p4x=116; p4y=86;
  c1 = "red"; fillColor = lineClr = "black";
}
else if( !val && !val.isEVal() )
{
  p1x=76; p1y=46;
  p2x=86; p2y=36;
  p3x=126; p3y=76;
  p4x=116; p4y=86;
  c1 = "white"; fillColor = lineClr = "black";
}
else if( val && !val.isEVal() )
{
  p1x=70; p1y=45;
  p2x=80; p2y=45;
  p3x=80; p3y=105;
  p4x=70; p4y=105;
  c1 = "lime"; fillColor = lineClr = "black";
}
else if( val.isEVal() )
{ c1 = fillColor = lineClr = "darkgrey"; }
','','',-1,'name;dscr;geomW;geomH;lineClr;fillColor;elLst;p1x;p1y;p2x;p2y;p3x;p3y;p4x;p4y;c1;s1;',2);
INSERT INTO "wlb_ElectroEls" VALUES('El_KeySqr_1_tst','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
TkSuQmCC','/wlb_originals/wdg_ElFigure','JavaLikeCalc.JavaScript
if( !val && !val.isEVal() )
{
  p1x=85; p1y=60;
  p2x=85; p2y=90;
  p3x=15; p3y=90;
  p4x=15; p4y=60;
}
else if( val && !val.isEVal() )
{
  p1x=35; p1y=40;
  p2x=65; p2y=40;
  p3x=65; p3y=110;
  p4x=35; p4y=110;
}','','','',-1,'name;dscr;geomW;geomH;lineClr;elLst;p1x;p1y;p2x;p2y;p3x;p3y;p4x;p4y;c1;s1;',2);
INSERT INTO "wlb_ElectroEls" VALUES('El_KeySqr_2','iVBORw0KGgoAAAANSUhEUgAAAEAAAABACAIAAAAlC+aJAAAACXBIWXMAAA1hAAANsAGkB5ZwAAAE
rklEQVRogd1azUsyXRS/juMVkoJEyWUlCAWCVDz2YdkiwogkLBSshetW0Z/Qpp1F4V4oDEUkjEgi
CCspo1EjaFFE1CKKhj4WRY7T+CzucJ957e0hbHrtvr/VOdc7c87Pe87MvXOOYmRkxG63A7nBsuz0
9DRWe3t7XS6X7FaSySSYn58vfgPOz8+lliYnJ7/Dyvz8PI0MZDKZZDKJZJ1O19HRsbq6ilQI4ejo
aCgUwt74fL5gMIjV8fHxcDhcKBSQ6nQ6d3d3z87OpAQymczs7CwAoLW1leO44+NjNG4ymfR6fSqV
QqrBYLBYLIlEAqkajWZwcDASibw3bbfbW1paAAA0Xot0Ot3T0wMAUKlUFEWp1WpMQKFQYBUA8F6F
EFIUhVSKoiCEEEIpAaVSiS6haVoQBHw5TdP4J2SrRP1X09vb2wAAREAMIb/fHwgEZFzcbw2hQCDg
9/uL0hDy+XwKhQIQCHHdg8GgNMp/OLRarU6nQzJdWVfKw69fvwRBQDJVWVfKQyKRwA9JIglIIYaQ
2+2WK4kPDw+npqZeX1+lg5FIhGGYrq6umZkZWaxgiATW1tYghD6f7+t3fHx83NnZKRm8vr6+vr6u
ra39+v0BABDCYrGIZJHA8/Mzz/Oy3P0/gNPpxASIzIFoNIof+kQSkEIMIYfDoVQqK+tKeRAJ5HI5
CKHJZKqsN2VAJHBzcyPd9P1weL1espM4FArhA4mcBGKxWFtb28TExEcTkslkW1ubx+OR0agYQl1d
XV9PYpZlGYb5y4SnpyeGYfL5/BcNSSESuLu7o2kid6ai06enpwQlsdvtJjuJY7FYOBxGMpEEeJ7H
H0HEEDKbzWTnAISQbAIMw6jV6s7Ozsp680kMDAzgMzGR//r+/j7HcUajERBK4OHhAb8NRQL19fUq
lapyLpUPkUBDQwPZ54GtrS21Wm02myvrzSfR3d399vaGZCJfZJeXlxcXF0gmMomvrq5Kk1ir1ZZ8
0ScFIgGbzYYrFGRBJBCPx9Vq9V8OUz8KFosFf4YjMgc4jsMEiAybk5MTXCYUCahUKrJ3ox6Ph9Aa
mUhgaWmJoCQ2Go1kJ7FOpyN7K5FOp3FxX04CTqczlUrNzc19NMFms6VSqcXFRRmNylnoNhgMBoPh
5eXlowlarVb2UyuRhe66ujqDwYBkIpPYbDbjJCaSwObmZj6fb2pqAoQ+haQQV8Dr9RL6JhZXIBqN
xuPxyrryeVRVVWk0GiSLK8BxHEEr0N/f/42f161Wazabxd0kCGNjY9lsFrXNfR0rKyu4kU7+p1B1
dbXFYqmpqZEO6vV6i8Uiuy2ACQwNDZF9Jt7b21OpVI2NjZX1pgyIBFiWJahG9r8qdP9JYtRN6vV6
pbu64eHhjY0NvMHs6+s7Pj6+vb1FqtVqZVkWt4g2NzdDCHO5HACAZVmpyaOjo+Xl5fb29vX1ddEw
TbtcLmlXbonp0dHReDzOcRxSHQ7HwcHB/f09ctVqtf6DAOr/zufzxWJRWoguFoscx+GRt7e3EpXn
eazyPE9RFFKxYemFgiDgyYIgvLf1XsX3EQShUCigCVar9U/DOvHN37jnW16UhFAul1tYWJDdSjKZ
/A24me+pnGj7iwAAAABJRU5ErkJggg==','/wlb_originals/wdg_ElFigure','JavaLikeCalc.JavaScript
if( st && !st.isEVal() )
{
  p1x=72; p1y=55;
  p2x=91; p2y=36;
  p3x=135; p3y=80;
  p4x=116; p4y=99;
  c1 = "red"; fillColor = lineClr = "black";
}
else if( !val && !val.isEVal() )
{
  p1x=72; p1y=55;
  p2x=91; p2y=36;
  p3x=135; p3y=80;
  p4x=116; p4y=99;
  c1 = "white"; fillColor = lineClr = "black";
}
else if( val && !val.isEVal() )
{
  p1x=60; p1y=45;
  p2x=90; p2y=45;
  p3x=90; p3y=105;
  p4x=60; p4y=105;
  c1 = "lime"; fillColor = lineClr = "black";
}
else if( val.isEVal() )
{ c1 = fillColor = lineClr = "darkgrey"; }
','JavaLikeCalc.JavaScript
if( st && !st.isEVal() )
{
  p1x=72; p1y=55;
  p2x=91; p2y=36;
  p3x=135; p3y=80;
  p4x=116; p4y=99;
  c1 = "red"; fillColor = lineClr = "black";
}
else if( !val && !val.isEVal() )
{
  p1x=72; p1y=55;
  p2x=91; p2y=36;
  p3x=135; p3y=80;
  p4x=116; p4y=99;
  c1 = "white"; fillColor = lineClr = "black";
}
else if( val && !val.isEVal() )
{
  p1x=60; p1y=45;
  p2x=90; p2y=45;
  p3x=90; p3y=105;
  p4x=60; p4y=105;
  c1 = "lime"; fillColor = lineClr = "black";
}
else if( val.isEVal() )
{ c1 = fillColor = lineClr = "darkgrey"; }
','','',-1,'name;dscr;geomW;geomH;lineClr;fillColor;elLst;p1x;p1y;p2x;p2y;p3x;p3y;p4x;p4y;c1;s1;',2);
INSERT INTO "wlb_ElectroEls" VALUES('El_KeySqr_2_tst','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','/wlb_originals/wdg_ElFigure','JavaLikeCalc.JavaScript
if( !val && !val.isEVal() )
{
  p1x=85; p1y=60;
  p2x=85; p2y=90;
  p3x=15; p3y=90;
  p4x=15; p4y=60;
}
else if( val && !val.isEVal() )
{
  p1x=35; p1y=40;
  p2x=65; p2y=40;
  p3x=65; p3y=110;
  p4x=35; p4y=110;
}','','','',-1,'name;geomW;geomH;lineClr;elLst;p1x;p1y;p2x;p2y;p3x;p3y;p4x;p4y;c1;s1;',2);
INSERT INTO "wlb_ElectroEls" VALUES('El_KeySqr_3','iVBORw0KGgoAAAANSUhEUgAAAEAAAABACAIAAAAlC+aJAAAACXBIWXMAAA1hAAANsAGkB5ZwAAAE
8klEQVRogd1aS0hyTRgeT8cRkqJEsVWUgVAoSEWntLJFhBGJlBVZC9etok37Nu0MDPctojBCwogk
hLALXcxLBC2KiIKsSLosijza8VvMYf7z27/4sOPnN/+zet/xnHnex5l3ztwkg4ODZrMZiI1kMjkz
M4Pdrq6ugYEB0VlCoRBwu93ZAuDq6krINDk5WQgWt9tNI4JoNBoKhZCtVCrb2trW19eRCyG02+1L
S0s4GqfTubCwgN3x8XGv15tOp5FrtVr39vYuLy+FAqLR6NzcHACgqamJZdmzszNUrtVqVSrV/v4+
cquqqgwGQyAQQK5cLu/r61tZWflObTabGxsbAQA0boujo6POzk4AgFQqpShKJpNhARKJBLsAgO8u
hJCiKORSFAUhhBAKBZSUlKBXaJrmOA6/TtM0/glx5bj/Sb2zswMAQAL4LuRyuTwej4iNW9Au5PF4
XC5XVtiFnE6nRCIBBIJv94WFBWEv/8uhUCiUSiWy6eKGkh9aWlo4jkM2VdxQ8kMgEMCDJJEChOC7
0PDwsFhJfHJyMjU19fn5KSxcWVmJRCImk2l2dlYUFgxewMbGBoTQ6XT+vMbX19fd3d2cwkQikUgk
Kisrf14/AABCmM1mkc0LeH9/z2QyotT+B2C1WrEAInNgdXUVD/pEChCC70IWi6WkpKS4oeQHXkA8
HocQarXa4kaTB3gBDw8PwknfXw6Hw0F2Ei8tLeEFifhzoZaWlnA4fHd3Z7PZcOHo6OjU1FRFRYXo
dLwAk8kkVhKXl5c3NzcrFAphoVqtbm5uFqX+HPACnp6eaJrImSkf9MXFBUFJPDw8THYS+3w+r9eL
bCIFZDIZvAnCdyG9Xk92DkAIyRYQiURkMpnRaCwQze3tbTAYRLZcLm9ra/tJbb29vXhN/If+dZ/P
5/P5kK3T6fDOXH44PDxkWbaurg4Quivx8vKSSqWQzQuoqamRSqXFCyl/8AJqa2vJXg9sb2/LZDK9
Xl/caH4THR0dX19fyCbyQ3Zzc3N9fY1sIpP49vY2N4kVCkXOjj4p4AW0t7fjE4pCQKPR6HQ6ZFdX
V4tYMy/A7/fLZLKJiQkRqxbCarWiIyZRYDAY8DYckTnAsiwWQOQodH5+jicjvACpVEr2bHRkZITQ
MzJewOLiYkGTWFzU1dWRncRKpZLsqcTR0RE+3BdfQDgcNhqNQ0NDwkKv12s0Gqenp0WnE/+g++3t
7eDgIKfw/v7+/v5epVKJQiEEkQfdarW6qqoK2UQmsV6vx0lMpIBgMJhKperr6wGho5AQfAs4HA5C
v8R8C6yurvr9/uKG8vsoLS2Vy+XI5luAZVmCWqCnp6eA2+sMw8RiMXybBGFsbCwWi4m1pllbW8MX
6cQfhcrKygwGQ3l5ubBQpVIZDAbRuQAW0N/fX9A1ceHACzg4OJBKpRqNprjR5AFeQDKZJOiM7H96
0I1ukzocDuGszmazbW1tfXx8ILe7u/vs7Ozx8RG5DMMkk0l8RbShoQFCGI/HAQDJZFJIeXp6ury8
3Nraurm5yRPT9MDAgPBWbg613W73+/0syyLXYrEcHx8/Pz+jUBmG+ZcAdP87lUpls1m8awcAyGaz
LMvikq+vrxw3k8lgN5PJUBSFXEwsfJHjOPwwx3Hfub67uB6O49LpNHqAYZh/LqwTf/kb3/kWFzld
KB6Pz8/Pi84SCoV+AY8g+verurPrAAAAAElFTkSuQmCC','/wlb_originals/wdg_ElFigure','JavaLikeCalc.JavaScript
if( st && !st.isEVal() )
{
  p1x=76; p1y=46;
  p2x=86; p2y=36;
  p3x=99; p3y=49;
  p4x=108; p4y=40;
  p5x=124; p5y=56;
  p6x=115; p6y=65;
  p7x=129; p7y=79;
  p8x=119; p8y=89;
  c1 = "red"; fillColor = lineClr = "black";
}
else if( !val && !val.isEVal() )
{
  p1x=76; p1y=46;
  p2x=86; p2y=36;
  p3x=99; p3y=49;
  p4x=108; p4y=40;
  p5x=124; p5y=56;
  p6x=115; p6y=65;
  p7x=129; p7y=79;
  p8x=119; p8y=89;
  c1 = "white"; fillColor = lineClr = "black";
}
else if( val && !val.isEVal() )
{
  p1x=70; p1y=45;
  p2x=80; p2y=45;
  p3x=80; p3y=165;
  p4x=90; p4y=65;
  p5x=90; p5y=85;
  p6x=80; p6y=85;
  p7x=80; p7y=105;
  p8x=70; p8y=105;
  c1 = "lime"; fillColor = lineClr = "black";
}
else if( val.isEVal() )
{ c1 = fillColor = lineClr = "darkgrey"; }','JavaLikeCalc.JavaScript
if( st && !st.isEVal() )
{
  p1x=76; p1y=46;
  p2x=86; p2y=36;
  p3x=99; p3y=49;
  p4x=108; p4y=40;
  p5x=124; p5y=56;
  p6x=115; p6y=65;
  p7x=129; p7y=79;
  p8x=119; p8y=89;
  c1 = "red"; fillColor = lineClr = "black";
}
else if( !val && !val.isEVal() )
{
  p1x=76; p1y=46;
  p2x=86; p2y=36;
  p3x=99; p3y=49;
  p4x=108; p4y=40;
  p5x=124; p5y=56;
  p6x=115; p6y=65;
  p7x=129; p7y=79;
  p8x=119; p8y=89;
  c1 = "white"; fillColor = lineClr = "black";
}
else if( val && !val.isEVal() )
{
  p1x=70; p1y=45;
  p2x=80; p2y=45;
  p3x=80; p3y=165;
  p4x=90; p4y=65;
  p5x=90; p5y=85;
  p6x=80; p6y=85;
  p7x=80; p7y=105;
  p8x=70; p8y=105;
  c1 = "lime"; fillColor = lineClr = "black";
}
else if( val.isEVal() )
{ c1 = fillColor = lineClr = "darkgrey"; }','JavaLikeCalc.JavaScript
if( st && !st.isEVal() )
{
  p1x=76; p1y=46;
  p2x=86; p2y=36;
  p3x=99; p3y=49;
  p4x=108; p4y=40;
  p5x=124; p5y=56;
  p6x=115; p6y=65;
  p7x=129; p7y=79;
  p8x=119; p8y=89;
  c1 = "red"; fillColor = lineClr = "black";
}
else if( !val && !val.isEVal() )
{
  p1x=76; p1y=46;
  p2x=86; p2y=36;
  p3x=99; p3y=49;
  p4x=108; p4y=40;
  p5x=124; p5y=56;
  p6x=115; p6y=65;
  p7x=129; p7y=79;
  p8x=119; p8y=89;
  c1 = "white"; fillColor = lineClr = "black";
}
else if( val && !val.isEVal() )
{
  p1x=70; p1y=45;
  p2x=80; p2y=45;
  p3x=80; p3y=165;
  p4x=90; p4y=65;
  p5x=90; p5y=85;
  p6x=80; p6y=85;
  p7x=80; p7y=105;
  p8x=70; p8y=105;
  c1 = "lime"; fillColor = lineClr = "black";
}
else if( val.isEVal() )
{ c1 = fillColor = lineClr = "darkgrey"; }','',-1,'name;dscr;geomW;geomH;lineClr;fillColor;elLst;p1x;p1y;p2x;p2y;p3x;p3y;p4x;p4y;p5x;p5y;p6x;p6y;p7x;p7y;p8x;p8y;c1;s1;',2);
INSERT INTO "wlb_ElectroEls" VALUES('El_KeySqr_3_tst','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','/wlb_originals/wdg_ElFigure','JavaLikeCalc.JavaScript
if( !val && !val.isEVal() )
{
  p1x=76; p1y=46;
  p2x=86; p2y=36;
  p3x=126; p3y=76;
  p4x=116; p4y=86;
}
else if( val && !val.isEVal() )
{
  p1x=70; p1y=45;
  p2x=80; p2y=45;
  p3x=80; p3y=105;
  p4x=70; p4y=105;
}','','','',-1,'name;dscr;geomW;geomH;lineClr;elLst;p1x;p1y;p2x;p2y;p3x;p3y;p4x;p4y;p5x;p5y;p6x;p6y;p7x;p7y;p8x;p8y;c1;s1;',2);
INSERT INTO "wlb_ElectroEls" VALUES('El_KeySqr_4','iVBORw0KGgoAAAANSUhEUgAAAEAAAAAgCAIAAAAt/+nTAAAACXBIWXMAAA1hAAANsAGkB5ZwAAAC
yUlEQVRYhe1YvWoqQRQ+2V2MkoiIiiIYiH+IC6ZJYWkjxM43sPIZfI1gSCPYiQ9gI1pZLpbC+osJ
qIh/RIxZkeCaW5xkvVjI7LBEvdyvOkfnfPud2Tkzs+cim83yPA8EWC6X0WgUAB4fH8PhMEmIKlDw
i6LI8TwvCALDMKFQqFarsSwbCAREUUQXANDw+XztdhvDWq2W0+mUZXk6neK/V1dXDoej2+3uRe25
t7e3s9lsuVyia7VadTrdcDhEd7VaIf/l5WW1WlWigsFgu93ebDboulyuj4+P+Xy+3W7D4TAIglCp
VL4I8Pb2hg8oFosk49VCLX86nRYEgQEAnKSzQygUAgDm2DLogfPOwE8qh9Hr9ZRhiUSiUChoq4aC
n2VZwARIltBmsxkMBmhPJhNJkujFasQfCAQAE3C73dqq+R2IogiYwHg8PrYYGuyKmOR9GY3GRCKB
diwW83g82qqh4P9e+f/PgaNht4RsNtuxxdAA552Dnw2VHKPR6OXlRXNBi8Xib36LxWIymQ6M9/l8
AACCIKTTaVVr9Hfw/Px8WE8mk/muAZKT+ASxXq+B/CQ+QeyKWK/XH1sMDXZF7HK5VEWmUqm7uzvN
BUmSlEwmFf77+/vD4x0OB2ACnU7n4eGB/EmRSETVeELM53NMgJBflmUgv06fIKbTKfwjRUyC9/f3
XC6HdqlU6na72qqh4Fd3mdtjzOfzJGcfOSj4s9nseV/mcBdiAICwsXVqwJfGAECz2Ty2GBrsihg3
1MO4ublpNBpo5/P5eDyurRoKfnVtFY7j7HY72maz2WAw0IvVlP+MixjnnQOA7Xb79PRkNpuvr6/7
/T72UDmO8/v99XpdadB6vV6MLBQKnU4Hf3c6nZ+fn7PZDF2j0Wi1Wl9fXw93ed1u93g8liQJXZvN
xrKs8pFULpcBQHmEEsXzfLPZlGUZXWzuXpx7e/0PVUTDDgGJdOIAAAAASUVORK5CYII=','/wlb_originals/wdg_ElFigure','JavaLikeCalc.JavaScript
if( st && !st.isEVal() )
{
  p1x=226; p1y=46;
  p2x=236; p2y=36;
  p3x=249; p3y=49;
  p4x=259; p4y=39;
  p5x=276; p5y=56;
  p6x=266; p6y=66;
  p7x=279; p7y=79;
  p8x=269; p8y=89;
  p9x=76; p9y=46;
  p10x=86; p10y=36;
  p11x=108; p11y=58;
  p14x=120; p14y=70;
  p15x=129; p15y=79;
  p16x=119; p16y=89;
  p17x=238; p17y=58;
  p18x=250; p18y=70;
  c1 = "red"; fillColor = lineClr = "black";
}
else if( !val && !val.isEVal() )
{
  p1x=226; p1y=46;
  p2x=236; p2y=36;
  p3x=249; p3y=49;
  p4x=259; p4y=39;
  p5x=276; p5y=56;
  p6x=266; p6y=66;
  p7x=279; p7y=79;
  p8x=269; p8y=89;
  p9x=76; p9y=46;
  p10x=86; p10y=36;
  p11x=108; p11y=58;
  p14x=120; p14y=70;
  p15x=129; p15y=79;
  p16x=119; p16y=89;
  p17x=238; p17y=58;
  p18x=250; p18y=70;
  c1 = "white"; fillColor = lineClr = "black";
}
else if( val && !val.isEVal() )
{
  p1x=220; p1y=45;
  p2x=230; p2y=45;
  p3x=230; p3y=65;
  p4x=240; p4y=65;
  p5x=240; p5y=85;
  p6x=230; p6y=85;
  p7x=230; p7y=105;
  p8x=220; p8y=105;
  p9x=70; p9y=45;
  p10x=80; p10y=45;
  p11x=80; p11y=65;
  p14x=80; p14y=85;
  p15x=80; p15y=105;
  p16x=70; p16y=105;
  p17x=220; p17y=65;
  p18x=220; p18y=85;
  c1 = "lime"; fillColor = lineClr = "black";
}
else if( val.isEVal() )
{ c1 = fillColor = lineClr = "darkgrey"; }','JavaLikeCalc.JavaScript
if( st && !st.isEVal() )
{
  p1x=226; p1y=46;
  p2x=236; p2y=36;
  p3x=249; p3y=49;
  p4x=259; p4y=39;
  p5x=276; p5y=56;
  p6x=266; p6y=66;
  p7x=279; p7y=79;
  p8x=269; p8y=89;
  p9x=76; p9y=46;
  p10x=86; p10y=36;
  p11x=108; p11y=58;
  p14x=120; p14y=70;
  p15x=129; p15y=79;
  p16x=119; p16y=89;
  p17x=238; p17y=58;
  p18x=250; p18y=70;
  c1 = "red"; fillColor = lineClr = "black";
}
else if( !val && !val.isEVal() )
{
  p1x=226; p1y=46;
  p2x=236; p2y=36;
  p3x=249; p3y=49;
  p4x=259; p4y=39;
  p5x=276; p5y=56;
  p6x=266; p6y=66;
  p7x=279; p7y=79;
  p8x=269; p8y=89;
  p9x=76; p9y=46;
  p10x=86; p10y=36;
  p11x=108; p11y=58;
  p14x=120; p14y=70;
  p15x=129; p15y=79;
  p16x=119; p16y=89;
  p17x=238; p17y=58;
  p18x=250; p18y=70;
  c1 = "white"; fillColor = lineClr = "black";
}
else if( val && !val.isEVal() )
{
  p1x=220; p1y=45;
  p2x=230; p2y=45;
  p3x=230; p3y=65;
  p4x=240; p4y=65;
  p5x=240; p5y=85;
  p6x=230; p6y=85;
  p7x=230; p7y=105;
  p8x=220; p8y=105;
  p9x=70; p9y=45;
  p10x=80; p10y=45;
  p11x=80; p11y=65;
  p14x=80; p14y=85;
  p15x=80; p15y=105;
  p16x=70; p16y=105;
  p17x=220; p17y=65;
  p18x=220; p18y=85;
  c1 = "lime"; fillColor = lineClr = "black";
}
else if( val.isEVal() )
{ c1 = fillColor = lineClr = "darkgrey"; }','','',-1,'name;dscr;geomW;geomH;lineClr;fillColor;elLst;p1x;p1y;p2x;p2y;p3x;p3y;p4x;p4y;p5x;p5y;p6x;p6y;p7x;p7y;p8x;p8y;p9x;p9y;p10x;p10y;p11x;p11y;p14x;p14y;p15x;p15y;p16x;p16y;p17x;p17y;p18x;p18y;c1;s1;',2);
INSERT INTO "wlb_ElectroEls" VALUES('El_KeySqr_4_tst','iVBORw0KGgoAAAANSUhEUgAAAEAAAAAgCAIAAAAt/+nTAAAACXBIWXMAAA1hAAANsAGkB5ZwAAAD
y0lEQVRYhdVYzUsqbRQ/d2bQQiMELTEKtC9JsEVQLnURRLqoReEm2vgvVP9FhEG0aevCKGgMhBZm
u4eWkuUHWmRUpqRo2gcz897F6U4X75uM48S9/VbPmXnO7znnzJlzHs6PnZ0dm80GElCtVqenpwFg
Y2PD4XBIUWkJMvjj8Thjs9kIIRRF2e32WCxG07TVao3H4ygCAC6Gh4dTqRSqJZNJk8nE83yhUMC3
Go3GaDRmMpkGrQbRbDYXi8VqtYqiXq9XqVS3t7co1ut15Fer1aenp6LW2NhYKpXiOA7F/v7+p6en
UqkkCILD4QBCSDQa/U8CHh8f8YBwOCxlf6told/v9xNCKADAIH072O12AKD+thnygXGn4JcrzXF9
fS1uW15eZllWWWtk8NM0DeiAlBTiOO7m5gbXDw8PtVpNvrEK8VutVkAHLBZLq+fl8/lWVRTnj8fj
gA7IsGZtbS0UCsmwTAZ/pVLZ2toSBKFhD6Yc7fP5rq6upqammjNyHFculycmJkqlUrlc5nl+f39/
fHx8dHRUEYs/4zeZTLOzs9vb28/Pz9jmRIRCob6+vhb6ACISiWi1WqRQq9UHBwdy674k/t8LzMrK
iiAIDX0ACCF+v7/VMzQajXgGy7KK+yDyN2B1dZXnedwWjUYJIQwAGAyGlj63y+U6PDx0u931ev31
9XVhYWFxcZGilGwpZrP57Ozsz+d3d3eCIOBZsVhscnISCCHBYFBGnI6Pjz+L0xdhaWmJ4zjRgHA4
/H6VuL+/l0HndDpDoZCYr18NiqLm5+exeSFyuRwAMCCtE/8vXC4Xy7Iej0e8SK6vr7vd7rat/UAy
mfR6vfV6XRAEr9e7t7fn8Xjw1cvLCwDI+Ymb5JJKpVK8Ln3Gjz8xBQAdHR3tBOn3XFKr1d3d3e2w
Sed/vwERQhS530cikZ6enpOTk/apJPIHg8H3MppOp2dmZtqMk8vlymazX1eX/uTneR6kX6el4Kur
agN/oVAA6dfpfxAYd0bi7kqlsru7K4pOp3NwcFBBa2TwYyeW2geKxaLP5xPFQCCgrAMy+DGjvnEK
GY1GQAckDrb+NWQyGUAHEonE3zZGDj7GKlhQm2NgYODi4gLXgUBgbm5OWWtk8Lc2VmEYpre3F9c6
na6zs1O+sYryf+Of+KMPCIKwubmp0+m0Wm0ul8MZKsMwIyMj5+fn4oB2aGgINVmWTafT+NxkMr29
vRWLRRS7urr0ev3l5WXzKa/FYsnn87VaDUWDwUDTdDabRf6joyMAEI8QtWw2WyKR4HkeRRzu/vju
4/WfVPRN8kqLrcsAAAAASUVORK5CYII=','/wlb_originals/wdg_ElFigure','JavaLikeCalc.JavaScript
if( !val && !val.isEVal() )
{
  p1x=76; p1y=46;
  p2x=86; p2y=36;
  p3x=99; p3y=49;
  p4x=108; p4y=40;
  p4x=124; p4y=56;
  p4x=115; p4y=65;
  p4x=129; p4y=79;
  p4x=119; p4y=89;
}
else if( val && !val.isEVal() )
{
  p1x=70; p1y=45;
  p2x=80; p2y=45;
  p3x=80; p3y=165;
  p4x=90; p4y=65;
  p4x=90; p4y=85;
  p4x=80; p4y=85;
  p4x=80; p4y=105;
  p4x=70; p4y=105;
}','','','',-1,'name;dscr;geomW;geomH;lineClr;elLst;p1x;p1y;p2x;p2y;p3x;p3y;p4x;p4y;p5x;p5y;p6x;p6y;p7x;p7y;p8x;p8y;p9x;p9y;p10x;p10y;p11x;p11y;p14x;p14y;p15x;p15y;p16x;p16y;p17x;p17y;p18x;p18y;s1;',2);
INSERT INTO "wlb_ElectroEls" VALUES('El_KeySqr_5','iVBORw0KGgoAAAANSUhEUgAAAEAAAABACAIAAAAlC+aJAAAACXBIWXMAAA1hAAANsAGkB5ZwAAAF
FklEQVRoge1aTUhySxgeT8cRDCJFUQj6paBCkJLsz2oVWSRhYSQtXLRqEdGmfS3aFVhGixZCECgW
/ZJUEPZDv1YQtChaFCRFUm2SPOrxLuYw91z7uZe+02fzcZ/V+8w5Z973aea1mXlH1NraWltbC4RG
MBgcGBjAtK6uzmw2C+7F5/MBu90e/wZcXV3xPfX29n6HF7vdTiMHx8fHPp8P2QqFoqKiYnFxEVEI
YVtb2/T0NI7GZrM5nU5MOzs7XS5XJBJB1GQybW9vX15e8gUcHx+PjIwAAEpLSxmGOTs7Q+0FBQVK
pXJnZwdRtVqt1Wq9Xi+iqampTU1Nbrf7reva2tqSkhIAAI3HYn9/v6amBgAgFospipJIJFiASCTC
FADwlkIIKYpClKIoCCGEkC8gJSUFfULTNMuy+HOapvEj5CuBvut6c3MTAIAEcFNoeHjY4XAIOLjf
OoUcDsfw8HCcP4VsNptIJAIEght3p9PJn+U/HHK5XKFQIJtObihfQ1lZGcuyyKaSG8rX4PV68Y8k
kQL44KaQxWIRKomPjo76+vpeX1/5jW632+/3V1VVDQ0NCeIFgxOwvLwMIbTZbL/e4/Pz89bWVkJj
IBAIBAIymezX+wcAQAjj8TiyOQEvLy/RaFSQ3n8DTCYTFkBkDng8HvyjT6QAPrgp1NDQkJKSktxQ
vgZOwOnpKYSwoKAgudF8AZyAu7s7/qLvh8NqtZKdxNPT03hDIvxaqKys7PDw8Pb2tqWlBTd2dHT0
9fWlp6cL7o4TUFVVJVQSp6Wl6XQ6uVzOb1SpVDqdTpD+E8AJeHh4oGkiV6Zc0BcXFwQlscViITuJ
Z2dnXS4XsokUEI1G8SEIN4U0Gg3ZOQAhJFuA3++XSCSVlZXJjeY/wmg04j0xkX/1vb09hmHy8vIA
oQKenp7C4TCyOQHZ2dlisTh5IX0dnICcnByy9wMbGxsSiUSj0eAHa2trCwsL737T2NhoNBoFDyUU
CvX397/7KD09fXBwEFODwRCLxZD9YQ74/f6xsbF3HymVyu8QEA6HP/KYkZHBF3B9fR2JRLRaLSA0
iW9ubhKTWC6XJ5zokwJOQHV1Na5QCAKpVGoymTAtLi4WsHM+OAELCwsSiaS7u1uoftVq9fz8vFC9
JUCr1eJjOCJzgGEYLIDI5fT5+TkuE3ICxGIx2avR9vb2hON1o9GoVCqPjo4mJiZwY1dXV3l5OVce
FBpSqXRychIA0NPTEwqFUGN+fn5/f79UKv3ws8+rlPwaLQBgampKwGLjR+CfvhgMhrcveL3epaWl
OL9KSRYUCgVeShCZxPv7+7i4T6QAPv4vdCcDKpVKrVYjm8gk1mg0/74f+MlYX18Ph8OFhYXgz0li
q9VKdhJ7PJ6PdsA/EFKpNDU1FdncCDAMQ9AI1NfXx4k+Xp+bm8OLNCIF8MFNoebmZmH3xL8NnIDd
3V2xWJybm5vcaL4ATkAwGCSoRvaHFrrRbVKr1cpf1eFSFMbMzMz9/T2y9Xp9MBjEV0SLiooghKen
p4hmZmZmZWXhy08ymay8vHxlZYVzTNNms5m/48Ou8akbACAQCIyPjwMAGhoaDg4OHh8fUah6vf4f
AtD973A4HI/H+d+//efAMAx+IRaLRaNRTKPRKEVRmEYikVgshinDMCzLYsqybIKvBJrQyLJsJBJB
tl6v//vC+ueXv3/mnhjDbrfT+M73uzg5OeHT1dXVp6enT94XBAlTaHR09KM3fT7fX2OgdJfhy1Vx
AAAAAElFTkSuQmCC','/wlb_originals/wdg_ElFigure','JavaLikeCalc.JavaScript
if( st && !st.isEVal() )
{
  p1x=70; p1y=45;
  p2x=80; p2y=45;
  p3x=80; p3y=85;
  p4x=70; p4y=85;
  c1 = "red"; fillColor = lineClr = "black";
}
else if( !val && !val.isEVal() )
{
  p1x=64; p1y=36;
  p2x=74; p2y=46;
  p3x=34; p3y=86;
  p4x=24; p4y=76;
  c1 = "white"; fillColor = lineClr = "black";
}
else if( val && !val.isEVal() )
{
  p1x=76; p1y=46;
  p2x=86; p2y=36;
  p3x=126; p3y=76;
  p4x=116; p4y=86;
  c1 = "lime"; fillColor = lineClr = "black";
}
else if( val.isEVal() )
{ c1 = fillColor = lineClr = "darkgrey"; }','JavaLikeCalc.JavaScript
if( st && !st.isEVal() )
{
  p1x=70; p1y=45;
  p2x=80; p2y=45;
  p3x=80; p3y=85;
  p4x=70; p4y=85;
  c1 = "red"; fillColor = lineClr = "black";
}
else if( !val && !val.isEVal() )
{
  p1x=64; p1y=36;
  p2x=74; p2y=46;
  p3x=34; p3y=86;
  p4x=24; p4y=76;
  c1 = "white"; fillColor = lineClr = "black";
}
else if( val && !val.isEVal() )
{
  p1x=76; p1y=46;
  p2x=86; p2y=36;
  p3x=126; p3y=76;
  p4x=116; p4y=86;
  c1 = "lime"; fillColor = lineClr = "black";
}
else if( val.isEVal() )
{ c1 = fillColor = lineClr = "darkgrey"; }','','',-1,'name;dscr;geomW;geomH;lineClr;fillColor;elLst;p1x;p1y;p2x;p2y;p3x;p3y;p4x;p4y;c1;s1;',2);
INSERT INTO "wlb_ElectroEls" VALUES('El_KeySqr_5_tst','iVBORw0KGgoAAAANSUhEUgAAAEAAAABACAIAAAAlC+aJAAAACXBIWXMAAA1hAAANsAGkB5ZwAAAF
4UlEQVRoge1aS0gbXRS+GWfuhLioSqSC4KNSoUIkVDG+dSVKqQsrCqmFLHTTWBFB3NtFdxbiA18B
QREUFYktDbVQoi3VYqwgdFF1oYtQMdRuFOeRSRf3cv/54yTaZBKdn/9b3XNn5p7zzT3nvs7VPXny
pKamBqgNv9/f399PxNra2qamJtW1eDwe4HA4gnHAwcGBXFN3d3c8tDgcDhop2N7e9ng8qGw0GsvK
ylZWVpAIIWxubp6dnSXW2Gy2qakpIra1tc3NzQmCgMTGxsZPnz7t7e3JCWxvb79+/RoAUFRUxPP8
7u4uqs/Pz09PT//8+TMSMzIyzGaz2+1GYnJy8qNHj+bn5y+rrqmpefjwIQCAJn2xublZXV0NAGAY
hqIolmUJAZ1OR0QAwGURQkhRFBIpioIQQgjlBJKSktAnNE1LkkQ+p2maPEK6QkRF1WtrawAARAC7
0MDAwPDwsIqdG1cXGh4eHhgYCMpdyGaz6XQ6oEHgfp+ampJ7+S1HWlqa0WhEZfpmTYkOJSUlkiSh
MnWzpkQHt9tNBklNEpADu1BLS4taQby1tdXT03NxcSGvnJ+f93q9FRUVr169UkULASbw9u1bCKHN
Zou9xd+/f6+vr4dU+nw+n8+Xmpoae/sAAAhhMBhEZUzg7OxMFEVVWk8AGhsbCQFNxsDCwgIZ9DVJ
QA7sQvX19UlJSbE3x3Hc3Nxc7O1cH5jAzs4OhDA/Pz/G5ux2u9PpjNmqvwAm8PPnT/miLwrwPN/Z
2ZkY661Wq8pBzHGc3W6fmJhQpbUrMTs7SzYk6qyFOjs7JycnL9cXFhbK+yQlJUUVdXJgAhUVFdEF
Mc/zdrtd0XoIYW9vb3FxcUwGXgVM4OTkhKb/ujciWK/X68fGxtra2mI18Cpgo3/8+BFFEIezHgAw
OTn59OnTmEwLj5aWlliDmOO4jo4ORetZlp2enrZardEbeBWWlpbIbBNNEKMRM5z14+Pj8fYcURTJ
IQgmYDKZrh8D4awHADidzvh5jiKwC10+CFEEx3Ht7e2K4z3LsjMzM3H1HEXgv+71elmWLS8vj/Cq
IAjh5lrkOQn79w0NDWRP/BcxEHnMScCISbCxscHzfF5eHrjmKBTBc/R6/czMTIL9/vT09NevX6iM
eyAnJ4dhGMW3RVEM5zkQwrGxsQRbHwJMIDc3N9xSIoLnOJ3ORHqOIjCBjx8/sixrMpnIg9XVVZfL
tb+/T86K5dDr9U6nU90x5/z8vK+vT/FRSkrKy5cviVhVVRUIBFA5bBB7vd6hoSHFRwzDjI+Pqz5i
chwXTmNmZqacwOHhoSAIZrMZRDcTv3jx4tmzZ9FZqQqOjo44jkNlTCAtLe06ExnCnTt34mJXVMAE
KisrSYZCW8AEXC4Xy7LPnz+/WWuuCbPZTI7hNHm8zvM8IaBJt/n+/TtJE2ICDMNEsaW8DcBGt7a2
hhyvNzQ0pKenb21tjY6Oksr29vbS0lKcHlQbBoMBTfldXV3n5+eo8v79+319fQaDIexnkbOU8hwt
AGB6elrFZGM4yE9fqqqqLr/gdrvfvHkTlGcptQWj0UiWEpoM4s3NTZLc1yQBOf5PdN8E7t69m5GR
gcqaDGKTyXT1fuA248OHDxzHPXjwAPx3gthqtWo7iBcWFlwu182acn0YDIbk5GRUxj3A87yGeqCu
ri6o6UT38vIyWaRpkoAc2IUeP36s7T3xly9fGIa5d+/ezVoTBTABv98fY6I7kVA/0Z1gKCe60W1S
q9UqX9WRVBTB4uLi8fExKlssFr/fT66IFhQUQAh3dnaQmJWVlZ2dTS4/paamlpaWvnv3Dium6aam
JvmOj6gmp24AAJ/PNzIyAgCor6//+vUrOlVfW1uzWCz/IoDuf3McFwwG5d9fnhx4nicvBAIBURSJ
KIoiRVFEFAQhEAgQked5SZKIKElSiK4QMaRSkiRBEFDZYrH8c2E98uXv27knJnA4HDS5862Ib9++
ycX379+fnp5GeF8VhLjQ4OBguDc9Hs8fq5/oeDM8RskAAAAASUVORK5CYII=','/wlb_originals/wdg_ElFigure','JavaLikeCalc.JavaScript
if( !val && !val.isEVal() )
{
  p1x=64; p1y=36;
  p2x=74; p2y=46;
  p3x=34; p3y=86;
  p4x=24; p4y=76;
}
else if( val && !val.isEVal() )
{
  p1x=76; p1y=46;
  p2x=86; p2y=36;
  p3x=126; p3y=76;
  p4x=116; p4y=86;
}','','','',-1,'name;geomW;geomH;lineClr;elLst;p1x;p1y;p2x;p2y;p3x;p3y;p4x;p4y;c1;s1;',2);
INSERT INTO "wlb_ElectroEls" VALUES('El_Key_1','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','/wlb_originals/wdg_ElFigure','JavaLikeCalc.JavaScript
if( !val && !val.isEVal() )
{
  p1x=76; p1y=46;
  p2x=86; p2y=36;
  p3x=126; p3y=76;
  p4x=116; p4y=86;
}
else if( val && !val.isEVal() )
{
  p1x=70; p1y=45;
  p2x=80; p2y=45;
  p3x=80; p3y=105;
  p4x=70; p4y=105;
}','JavaLikeCalc.JavaScript
if( !val && !val.isEVal() )
{
  p1x=76; p1y=46;
  p2x=86; p2y=36;
  p3x=126; p3y=76;
  p4x=116; p4y=86;
}
else if( val && !val.isEVal() )
{
  p1x=70; p1y=45;
  p2x=80; p2y=45;
  p3x=80; p3y=105;
  p4x=70; p4y=105;
}','','',-1,'name;dscr;geomW;geomH;lineClr;elLst;p1x;p1y;p2x;p2y;p3x;p3y;p4x;p4y;c1;s1;',2);
INSERT INTO "wlb_ElectroEls" VALUES('El_Key_1_tst','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','/wlb_originals/wdg_ElFigure','JavaLikeCalc.JavaScript
if( !val && !val.isEVal() )
{
  p1x=85; p1y=60;
  p2x=85; p2y=90;
  p3x=15; p3y=90;
  p4x=15; p4y=60;
}
else if( val && !val.isEVal() )
{
  p1x=35; p1y=40;
  p2x=65; p2y=40;
  p3x=65; p3y=110;
  p4x=35; p4y=110;
}','','','',-1,'name;dscr;geomW;geomH;lineClr;elLst;p1x;p1y;p2x;p2y;p3x;p3y;p4x;p4y;c1;s1;',2);
INSERT INTO "wlb_ElectroEls" VALUES('El_Key_2','iVBORw0KGgoAAAANSUhEUgAAAEAAAABACAIAAAAlC+aJAAAACXBIWXMAAA1hAAANsAGkB5ZwAAAF
n0lEQVRogdVaT2gTTxSebHY30EDUkmIuYmqKoDUlKG3qn5oeRCulS2nTlC4p5FSoFPFQBa9Fe6wS
CeKth5CSEopExFqEklKxiU1I6Z+DpWo8SNRtpVFrs9luPMwwv/0lPYidte53em82O998mXmzb/eN
rrOz0+VyAdIQBGFoaAi7zc3NHR0dxFlisRjw+/1FFbC2tqZkunHjhhosfr+fhgSpVCoWi0HbbDaf
PXv2yZMn0GVZ1u12h0IhPBqfzzc6Oopdr9cbDocLhQJ0OY6bnZ1dXV1VCkilUvfu3QMAnDlzRhTF
xcVF2H78+PGqqqqXL19C12KxOByOyclJ6BqNxtbW1vHx8XJql8t1+vRpAACN5yIej1+8eBEAwDAM
RVEGgwEL0Ol02AUAlLssy1IUBV2KoliWZVlWKUCv18NbaJqWZRnfTtM0vgS5StxdqWdmZgAAUABa
QiMjI4FAgODkqrqEAoHAyMhIUbmEfD6fTqcDGgSa99HRUeUq/8dRWVlpNpuhTe/vUP4MDQ0NsixD
m9rfofwZJicn8SapSQFKoCXk8XjIBrHJZOI4LhqNAgCOHj3a1NREsHMl0Aw8ffp0amqKVKfJZLK2
thaOHgCQyWR6e3sHBgZI9a98ziABP3782NraIkVw9+7dz58/K1u2traCweDy8jKR/jmOc7vd0CYf
A69fv8a5gBKbm5t+v58IRSQSwZs+eQGbm5s/f/7c9VI2myVOh4K4paVFr9cT7/0vAM1AOp3GGeIe
UV9fb7Vad73U1dVFhEIJNAPZbFaZ9O0FBw4cuHXr1rVr10ra6+rquru7iVDwPF8sFqGtyoOsv7//
0aNHJ06cgO7Bgwf7+/sjkQjDMET6D4VC+IWEZC40MTExPDyMXfyKI8tyIpHo6emBrs1mC4fDpEiR
gPPnz+89iAVBSCaT5e25XE7Zns/n90ikBBLw5csXmtZkZooG/ebNG1JB/Bfg8XjUDWK1MTExgaNI
kwIkScI7BFpCdrtd2zHAsqy2BSSTSYPBcO7cuf0dzW/i6tWr+J1Yk//63NycKIo2mw1oVMDXr1/x
0xAJsFqtpBKVvwwkoLq6WqPvA0jA9PS0wWCw2+37O5rfRFNT087ODrQ1+SDLZDLv3r2DtiaD+MOH
D6VBXFlZWfJFXytAAi5cuIArFNoCEhCNRg0GQ/mL7L8Jh8MhSRK0NRkDoihiAZpcNisrK/gjEBLA
MIy2s9Hu7m6N1siQgGAwqKEgttls2g5is9ms7VQiHo/j4j7JGeA47tSpU9D+/v17IBDAJabBwUFU
WAegoqKCICnJQrfFYrFYLACA+fl5nufX19dheyaTuXnzps/ne/jw4R4pyqFKofvOnTt49BDb29uh
UGhpaYlI/4cPH4b/FFAjBhKJxPPnz8vbc7kcqRKT3W53OBzQJr8L5XK57e3tXS99+vSJCMWLFy/y
+Tz8fK/JXUgJNAM8z5N6Ejc0NFit1vfv35dfIlWhUQLNQCQSwXXpPcJkMt2+fbu83eFweDweIhQV
FRVGoxHaaAZEUSSYC/X19en1+vv378Nt59ChQ16v9/r166TyxcuXL6v+ef3KlSu49C9J0pEjR2pq
akh1/vjxY3yQTi0Boii+ffsW2t++ffv48aNKRGhO29ratP1O/OrVK4Zhjh07tr+j+QMgAYIgaKhG
pnqhW23sXuiGp0l5nldmde3t7VNTU3g/uXTp0uLiIs4InE6nIAj4iOjJkydZlk2n0wAAQRCUlAsL
C2NjY42Njc+ePUPENN3R0aE8lVtC7Xa7o9GoKIrQbWlpSSQSGxsbcKhOp/N/AuD573w+XywWlYXo
YrEoiiJu2dnZKXElScKuJEkURUEXEytvlGUZ/1iW5XKuchf3I8tyoVCAP3A6nf8dWNf84W985pss
SpZQOp1+8OABcZZYLPYLbOpRFA4TyZwAAAAASUVORK5CYII=','/wlb_originals/wdg_ElFigure','JavaLikeCalc.JavaScript
if( !val && !val.isEVal() )
{
  p1x=72; p1y=55;
  p2x=91; p2y=36;
  p3x=135; p3y=80;
  p4x=116; p4y=99;
}
else if( val && !val.isEVal() )
{
  p1x=60; p1y=45;
  p2x=90; p2y=45;
  p3x=90; p3y=105;
  p4x=60; p4y=105;
}','JavaLikeCalc.JavaScript
if( !val && !val.isEVal() )
{
  p1x=72; p1y=55;
  p2x=91; p2y=36;
  p3x=135; p3y=80;
  p4x=116; p4y=99;
}
else if( val && !val.isEVal() )
{
  p1x=60; p1y=45;
  p2x=90; p2y=45;
  p3x=90; p3y=105;
  p4x=60; p4y=105;
}','','',-1,'name;dscr;geomW;geomH;lineClr;elLst;p1x;p1y;p2x;p2y;p3x;p3y;p4x;p4y;c1;s1;',2);
INSERT INTO "wlb_ElectroEls" VALUES('El_Key_2_tst','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','/wlb_originals/wdg_ElFigure','JavaLikeCalc.JavaScript
if( !val && !val.isEVal() )
{
  p1x=85; p1y=60;
  p2x=85; p2y=90;
  p3x=15; p3y=90;
  p4x=15; p4y=60;
}
else if( val && !val.isEVal() )
{
  p1x=35; p1y=40;
  p2x=65; p2y=40;
  p3x=65; p3y=110;
  p4x=35; p4y=110;
}','','','',-1,'name;geomW;geomH;lineClr;elLst;p1x;p1y;p2x;p2y;p3x;p3y;p4x;p4y;c1;s1;',2);
INSERT INTO "wlb_ElectroEls" VALUES('El_Key_3','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','/wlb_originals/wdg_ElFigure','JavaLikeCalc.JavaScript
if( !val && !val.isEVal() )
{
  p1x=76; p1y=46;
  p2x=86; p2y=36;
  p3x=126; p3y=76;
  p4x=116; p4y=86;
}
else if( val && !val.isEVal() )
{
  p1x=70; p1y=45;
  p2x=80; p2y=45;
  p3x=80; p3y=105;
  p4x=70; p4y=105;
}','JavaLikeCalc.JavaScript
if( !val && !val.isEVal() )
{
  p1x=76; p1y=46;
  p2x=86; p2y=36;
  p3x=126; p3y=76;
  p4x=116; p4y=86;
}
else if( val && !val.isEVal() )
{
  p1x=70; p1y=45;
  p2x=80; p2y=45;
  p3x=80; p3y=105;
  p4x=70; p4y=105;
}','','',-1,'name;dscr;geomW;geomH;lineClr;elLst;p1x;p1y;p2x;p2y;p3x;p3y;p4x;p4y;p5x;p5y;p6x;p6y;p7x;p7y;p8x;p8y;c1;s1;',2);
INSERT INTO "wlb_ElectroEls" VALUES('El_Key_h','iVBORw0KGgoAAAANSUhEUgAAAEAAAAAqCAIAAACMZMq1AAAACXBIWXMAAA06AAANOgEDIh6FAAAC
/klEQVRYhe2Wz0vycBzHv2rNiBFSXpR+XLp4ySxSkEhjURQdIioxUoiGIkieBMH/wKNI0G1pkAhe
9JCElwk2O3QbneowgiGiIOVhWrTnMFlWtqfnSRvGXqfvPn4+H94v8Msm29raslgsoDfBcbxvYWHB
6/WKneQ/YVlWLnaG7yIJiI0kIDaSgNhIAmIjCYhNzwv0fSw9PDzgOM6dtVrt7Ozsu4ZMJvP09NSN
NAaDYXR0tLVSqVQuLy+588TExNTU1LuRNwLPz88Yhj0+PhqNRm5RJpMhCGJ+fn56eppvs9vt1Wq1
GwKxWGxvb48Pc3x8zDCM1WpVq9UAgHQ6jeP40tKSTqdrI0DT9OnpqdVqNRqNfNHtdjcajWg0SpIk
v/oHuL29TaVSCILo9Xq+6PV6a7VaPB4nSXJ7e5srNu/Ay8vL0dGRy+VqTc8BQRCKoizLXl1d/Ux6
hmGi0ajL5WpNzwHDMIqipVLp5uaGqzQFCIJYW1tTqVSfLbXZbIVCoUuJ35HP5zc2NmAY/qwBRdFs
NsudmwIYhpnNZoGlEAQNDw/zl7urxOPxmZkZgQalUglB0PX1NeAFPt7uj6yvr9/f33ckojBfCbO8
vFwsFsEveA80Be7u7v7a+pWejvBPYZoC4+Pj9XpdeCafz29ubn4z3FcYGxtrNBrCPSRJIggCeAGf
z+fz+QQGCIKAYXhwcLBTKQU4ODgIBoMCDdlsVqPRDAwMAF5AoVDs7+9jGNZ2oFwuX1xcOJ3OTkdt
j0qlWllZSSaTbX+labpQKNhsNu7x9RLPzc2ZTCaPx0NRFMMwXLFYLJ6fn0cikUAg0N/f3+3oPIuL
i5OTk4eHhxRF8X8nmqYTicTZ2Znf75fJZFzx9VNCLpfrdLpIJBKLxSiKGhkZAQCUSqXd3d3V1dXW
7QiC1Gq1buTWaDTcQaFQ6PX6UCh0cnJSqVSGhoYAAOVy2eFw7OzsvJkJh8NszxIOh3/Le6B3kQTE
RhIQG0lAbCQBsZEExKYvl8vJ5b2qkcvl/gBF1skqfQ6NXAAAAABJRU5ErkJggg==','/wlb_originals/wdg_ElFigure','JavaLikeCalc.JavaScript
if( !val )
{
  p1x=90; p1y=15;
  p2x=60; p2y=15;
  p3x=60; p3y=85;
  p4x=90; p4y=85;
}
else
{
  p1x=40; p1y=35;
  p2x=40; p2y=65;
  p3x=110; p3y=65;
  p4x=110; p4y=35;
}','JavaLikeCalc.JavaScript
if( !val )
{
  p1x=90; p1y=15;
  p2x=60; p2y=15;
  p3x=60; p3y=85;
  p4x=90; p4y=85;
}
else
{
  p1x=40; p1y=35;
  p2x=40; p2y=65;
  p3x=110; p3y=65;
  p4x=110; p4y=35;
}','','JavaLikeCalc.JavaScript
if( !val )
{
  p1x=90; p1y=15;
  p2x=60; p2y=15;
  p3x=60; p3y=85;
  p4x=90; p4y=85;
}
else
{
  p1x=40; p1y=35;
  p2x=40; p2y=65;
  p3x=110; p3y=65;
  p4x=110; p4y=35;
}',-1,'name;dscr;geomW;geomH;lineWdth;lineClr;elLst;p1x;p1y;p2x;p2y;p3x;p3y;p4x;p4y;',2);
INSERT INTO "wlb_ElectroEls" VALUES('El_Trio','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
7cfv/wMBpyH3FPzQ8AAAAABJRU5ErkJggg==','/wlb_originals/wdg_ElFigure','','','','',-1,'owner;name;dscr;geomW;geomH;lineClr;elLst;',2);
INSERT INTO "wlb_ElectroEls" VALUES('El_Zemlia','iVBORw0KGgoAAAANSUhEUgAAAEAAAABACAIAAAAlC+aJAAAACXBIWXMAAA06AAANOgEDIh6FAAAD
/0lEQVRoge2aXSh7YRzHz/+MsItZQiJtkpeMC3nLlptFbWu4OBEZxQoXy4atIfKS3GwXViLjjiVW
S1pJKUOtxkqx2s1KJDNNVou2kP+F2pZs9nKePUftc7Wd8zzf8/v0nD3nt7Z/BQUFTCYT+ZsYjcYk
JpO5ubkJu5IoEQgEKOwaYiUhAJuEAGwSArDBU8BqtdrtdhwDwwFPgZOTEw6Ho1Qq397ecIwNDc63
0OXlpUwmk8lkZ2dn+CYHA8hnQKVSNTc3Hx0dPT09gcgPBNSH+PHxkc1md3V1Acr3AXYXOjg4yMrK
WlxcBHcJ4Nuo0+kcGxsTCATX19cg8uPxHPB6vRqNhsfjgQiH8CBzu92fn594pUEQGB0dVSgUNpsN
lzQ4rYRcLm9pabFara+vrzFGQeuFrFZrWVmZXC6PMQdyM7eyskKj0fR6fdQJkAU+Pj5ub297e3v7
+/vdbncUCYRop51O59raWlNTk06ni3QuIQS+MJlMGIapVKqIZhFI4Ivz8/OIxhNOwMf09PTNzc37
+3voYcQV0Ov1dDp9fHz84uIixLDvAldXV2az2Ww2Pz8/gywvXJRKJY/HMxqNwerxCzgcjuHh4e7u
7tPT0+Xl5Z6enp2dnXjVGYqHhwcWixWsniTfK4PBQCaTDQYDlUr1er0Oh2NwcLCxsTEjIyOO1QZF
r9cfHh7a7XYul1tcXOw77l8BjUbT0dFBpVIRBElJScnNzc3JybFYLBCKDYLH45FIJBiGTU1N+Q76
V4BCoVRUVPhPJCXV1dXNz89nZ2eHeQG8GszQWCyW+/t7FovF4XCQQAGXy7W1tdXZ2fn11uPxqNXq
paWl+vr6MKNXV1dNJhPuFQeSnp6OYZhQKPT9KOMXUCgUUqmURCK1t7fbbDadTlddXR1+9XGgvLx8
d3e3sLAw8KBfgE6nz83NTU5O7u3tuVwuPp8/MTER9yJ/pqSkRCKRcLlcGo327ZRfIC0traqqan9/
P761/U5ra6tWq01OTv7xLHGfxGQyuaGhwWw2b2xsBKseCVwBojE7O8tms38dRtwVCKd6hFArgKJo
amqqWCyObBagaqJgYWHh5eWlpqYmolmEEGAymcfHxyMjI1HMhXwLUSgUkUgkFovDb1i+AU0ARdHK
ysqZmRk+nx9LDjQBoVCoVqtjz4EgwGAw7u7uMjMzcUmDIBDpRhma+O1CLBYLRGw8ViAvL299fR1Q
Zw5cQCKRDAwMlJaWAsoHKFBUVCQSiYaGhsBdAgEnwOPxtFotmUwGlO8Df4H8/Pzt7W0GgxGH6hHc
BaRSaVtbW21tLb6xIcBToK+vD0VREomEY+av4CkQ4osfOAjRTsdCQgA2CQHY/HmBf3/97/f/ATsh
O/ND++OYAAAAAElFTkSuQmCC','/wlb_originals/wdg_ElFigure','','','','',-1,'owner;name;dscr;geomW;geomH;lineClr;fillColor;elLst;',2);
INSERT INTO "wlb_ElectroEls" VALUES('Induct','iVBORw0KGgoAAAANSUhEUgAAAEAAAABACAIAAAAlC+aJAAAACXBIWXMAAA1hAAANsAGkB5ZwAAAE
dElEQVRoge2aT0gibxjHZ0VkVu1Q1BIVVhIEgVAUQUYDEV1UhDT7g1anRDKhDEIQOwRBIXWSJMIS
TIKMgkgKhLBDIQUhJdZJyqLonwaViYn9DrMrsvzWHN4Hl2A/p+f18OH5Wu8784zzrby8nM/nY1+T
/f19Op/PX1pagvW+v78/PT2RdUFBAaw8FYVCQQOXHhwc6HS6H78A9/8GHdAVDocHBwfdbvf19TWg
Nj1gAex2u91u39raghJmCEyA7e3t/v7+t7c3cslms2tqang83ujoKIg/DTABTCZTsnsMwyYnJ9Vq
NYj5UwACaDQap9NJ1m1tbTab7fv37+jaDEEN4Pf7Nzc3f7ro9JGRERaLhdwVBVCPUYfDcX5+jmEY
nU5fXFxsbGwEaIoKqAEWFhbIgsFgKBQK5H4oA3YhE4vFUCpKgAWQyWRQKkrA30pkGbAAx8fHUCpK
gAUwGo1QKkqgBkiem/F43OPxIPdDGdQAGo0Gx3EMw2KxmEwmOzk5geiKAqgBGhoaenp6yPrq6mp5
eRm5JWoA7AG9Xs/lcsl6enq6trZ2b28PXZshAAFKS0uTOzgWix0dHbW3t+v1eqvVii7/HLlc/oHM
6+vr8PBwRUVFqhnHcQ6Hgy5Pg1wuhzlGmUzmzMzMxsZGaoZoNBoMBkH8aQAbKT0ej91uDwQCUMIM
gQng8XikUmlylqfRaDiOf6WR0m63pz6J0Gq1WbswAwQwGo2zs7NkXVVVZTab6+rq0LUZghogGAw6
HI5EIkEuzWYzQRDIXVEANYDb7T48PMQwjEajabXabH73JKgBDAYDWeA4/lduSMFup3k8HpSKEmAB
snBi/i//RspfPD8/Q6koARZgYGAASkUJ1ABFRUVkkUgkbm5ukPuhDGqAqakpsohGowKBgHzMmE1Q
A9TX13d3d5O11+t1uVzILVEDNQCO4yqVisFgkEutVqtUKrP5vwSwiQmCSE6PLy8v8/PzLS0ta2tr
2ZmMYU4hgiAIgkj+HU5PT6VSqUQikUgkIP40wAQoLi7e3d11uVyFhYXJD+/u7tbX10H8aQC7Dtzf
3wcCgUgkAiXMEJiJ7OzsTCqV+v3+1A+5XG5XVxeIPw0wAZxOZ2r3QqHQaDSyWCwOhwPiTwNAgJWV
FZ1OR9a5ubkGg6Gvry8vLw/dnAmoAUKhkNVqjcfj5NJmswmFQuSuKIC6iX0+X/L1gubm5q83UiYf
TTOZzJ2dHeR+KAN2jObk5ECpKAEWIPloKMv8Gyn/NmABhoaGoFSUAAvw+PgIpaIEagCLxUIWkUhE
JBI9PDwgt0QN1ADV1dWtra1k7XQ6vV4vckvUQA2Qn5/f29tLp/+8IHZ2ds7NzYXDYeTGMgVgDygU
ivHxcbIOhUIqlUqpVF5cXNze3qLLPwVmE4vF4srKyuRydXW1rKysqalpYmICxJ8OkJ9ZPz4+Li8v
TSYTm83+zQ8i/xNgP7NiGFZSUqJWqy0Wi0AggHJmAuSrxxiGdXR0iEQin883NjYGa/4T37766/f/
AfiLqHTo2SlUAAAAAElFTkSuQmCC','/wlb_originals/wdg_ElFigure','','','','',-1,'owner;name;dscr;geomW;geomH;geomXsc;geomYsc;lineWdth;elLst;',2);
INSERT INTO "wlb_ElectroEls" VALUES('El_Krug','iVBORw0KGgoAAAANSUhEUgAAAEAAAABACAIAAAAlC+aJAAAACXBIWXMAAA06AAANOgEDIh6FAAAF
GUlEQVRoge2aS0gyXRzGJ3slLQoMwqiQimAQKTDMhS0qRZIWQRfLGEOEJLpsoyhq0yKoEEPJIBKM
JqhILMhVF6EMrKBapBC0iEoC0RSm6SLyfYvgW3SZOTNO+fXibyX6/GeeZ+Z45sw5J62kpEQmk0G/
k8PDwz8ymWxpaSnZTmii1WpZyfaQKKkAySYVINmkAiSbP99x0GAw6HA43n3J4/Ha2toYPxczAXAc
DwaDe3t7W1tbGRkZeXl5TU1N7zSRSESr1UIQFA6HTSYTh8MRCASJn5qBAC6XC0XRl5cXGIanpqaK
i4u/UjY2NkIQ5HQ67Xb75uamRqPp7OxMNAaCIP/QAsfx4+NjlUqlUChOT0+j0Sil8pubG4vF0tzc
PDMz4/P56HlAEIR+gOHh4YaGhpWVFXrl/+H1esvLy91uN41aBEHo9EKhUEgikbS2tq6trSX+v5RK
pW63+/LyUqVSBQIBquWUA3g8noGBgbm5ObFYnJmZSbX8U3Jzcw0GQ1VVlVqtplxMqQkdHByUlJSc
n5/TuN0gzM7OymSyu7s7QD21JhQKhRYWFpxOZ0VFBeXrBEZPT49cLvd6vRiGAZZQCFBfX9/b2/t9
7t8YHx+/vr4eHR0F1AMFeHp6GhkZmZ+fl0gkCXgDpa+vLz8/f2dnB0QMFODi4uLs7AyG4cSMgcJm
s+vq6iYmJkDEQAFGR0d1Oh1TfQ4IUqm0qKjIYrGQKskDuFyuWCz2HeMwYgYHBxcXF0OhELGMJACO
4yiKTk9PM2cMFBiGa2tr3W43sYwkQDAYfHl5KS0tZcwXMCwWS6fTLS8vk8iIf97b24NhOCcnhzlj
FBCJRNFo1O/3E2hIAmxtbRkMBkZdUaO9vX1/f59AQBIgIyODYHz/A5D23UQBgsFgXl4eo34ok52d
/fj4GI/HvxIQBXA4HB/fDH8YsVjs9/ufn5+/Evz6WYlUgGSTCpBsiALweLxIJPJjVj4Fx3Eul8ti
femTKEBbW9vq6uo3uKLAycmJSCTicrlfCf7qJgRBUDgcdjqdP2PlU2w2G7GAJIDJZDo+PmbOD2Ve
X1/1ej2BgCQAh8PZ3Ny8vb1l1BUoKIpiGMZmswk0JAEEAoFGo9nY2GDUGBAPDw8oiprNZmIZ+Z+4
s7Nzd3eXIVcUuL+/Lyws5PP5xDLyAAKBoKam5ujoiCFjoHg8HgRBOBwOsQyoG1UqlV1dXeFwmAlj
QNjtdpfLVVtbS6oECiAUCs1m8/r6eqK+wMAw7Orqymq1gohBH2Q1NTXr6+vgU5a0CQQCLS0tIK3/
DQpPYpvNtru7C3hhaKNWq6VSaXd3N6CewiJfQUGBx+Oprq4OBALj4+O07BERCATUarVWq+3p6QGv
ojwWWltbq6ysNJlMsViMai0BGIbp9Xq5XE7JPUQjQEFBgVKpvL6+NhqNTPWtdrt9cnKyubmZzo2l
vUq5vb2tUCh0Op3P54vH4zSOEA6HfT7f/Px8U1PT/f09jSPQXKV8Q6FQbG9vSyQSnU43NDR0cXFB
qRxFUQRBjEZjWVmZw+EA7HM+kuhKfX9/f0dHh9vtHhsbi0aj7e3tMAxnZ2eLxeJ3ShzHT05OIAiy
2Wyvr68YhlmtVj6fT/qsJSYNQRCmNv35/f63eczHx8ePM7JcLlckEr191uv1xGNMQLRaLZO7VYRC
oVAohCAoHo9/nEtjsVgEb4a0+ZbtNunp6VlZWd9x5I/87e/E/39SAZJNKkCySfvt2+//BUIQVkGk
skutAAAAAElFTkSuQmCC','/wlb_originals/wdg_ElFigure','','','','',-1,'name;geomW;geomH;lineWdth;lineClr;elLst;',2);
INSERT INTO "wlb_ElectroEls" VALUES('El_lhor','iVBORw0KGgoAAAANSUhEUgAAAEAAAAAGCAIAAAD8Cg/4AAAACXBIWXMAAA1hAAANsAGkB5ZwAAAA
EklEQVQ4jWNgGAWjYBSMgoEEAASGAAFT99QhAAAAAElFTkSuQmCC','/wlb_originals/wdg_ElFigure','','','','',-1,'name;geomH;lineWdth;lineClr;elLst;',2);
INSERT INTO "wlb_ElectroEls" VALUES('El_lvert','iVBORw0KGgoAAAANSUhEUgAAAAYAAABACAIAAAC2r5FuAAAACXBIWXMAAA1hAAANsAGkB5ZwAAAA
EklEQVQ4jWNgGAWjYBSMgqEJAATAAAFUww8RAAAAAElFTkSuQmCC','/wlb_originals/wdg_ElFigure','','','','',-1,'name;geomW;lineWdth;lineClr;elLst;',2);
INSERT INTO "wlb_ElectroEls" VALUES('Sharp','iVBORw0KGgoAAAANSUhEUgAAAEAAAABACAIAAAAlC+aJAAAACXBIWXMAAA1hAAANsAGkB5ZwAAAI
vElEQVRoge1aX0xS7xt/QVP+mPwVOIAQuLpIkcXmYrU1Ny+8cSlIXrjW1my6ORtoXlmutVZulHlR
d9Vdfy7owpq1WvOi5pYbGXOCf2byx6Aj4gQBOcEBzu/ibOz98v19U/CYa/Nz58PnPO/z4TzP+7zP
iwD85aABAK5du6bX6w86koIxPT09OjpaCgDQ6/Umk+mg4ykS9IMOYK84FHDQOBRw0DgUcNCgUsDE
xER/f38mk6HQ544opcrR+Pj406dPv337xuPxLl++XF1dTZXn34MCAalUym63P3r0aHJyEgBw8+bN
yspKg8GgVCr37nxHUJBCsVist7f306dPOUt/f/+zZ88wDNu78x1BgQAOh2Oz2Zqbm2Hjw4cPr1+/
vnfnO6IEANDR0XHy5MmiXdDpdIFAIJVKU6mU0+kkjdvb22tra4FAoKGhgclkUhPsPzE/P2+z2Yqv
gXfv3iEIcurUKfLPxsbGbDabSCTevHlDEAQAwOv1PnnyRCQSmUwmlUpFTdT/FzabjSgEGIa5XC69
Xt/X1+f1euGPnE6nVqtlsViw/9HRURRFC1piN7DZbEUKcLlcYrG4pKSkrKzMZDLBH6XT6Vgsljce
MRiMoaEhSoMniKIFvH37Vq/Xl5SUkM8KhUKj0ej3+2GO3W7Pm5AUCoXZbN4PAQUXcTweDwaDDoeD
7LiJRMLr9cbjcZlMJpFISI5UKuXz+TiO52p6a2srEAhEIpETJ05UVlbucq3fgyziggUgCFJbW+vz
+dbX1xOJBAAgnU7b7XY2my2TyUQiEUlTq9UIgvh8PhRF0+k0ACAajU5NTclkMgRBuFwuVQIA2EUK
JZPJjY0NHMdho9FozNsfu7q6wuFwNpvNcQKBgEajKSsrg2kjIyPRaJSqFNqVgKmpqfr6+qWlJdjo
9/t7e3vhyDgcTltbG6wTx3GPx5PX44RC4cDAAFUCdk6h8fFxq9X69evXlZUVqVR67Ngx0l5ZWSmT
ydhs9pcvX0hLMplcX193Op11dXVCoRAAQKfTuVyuQqFIJpO5ekgkEiiK/vjx48yZM+Xl5QWlDYxd
1cDExMTjx48/fPiQzWaXl5dTqRSPx8t1JYlEIpPJUqmU2+1OJpMAAAzD5ubmSktLxWIxgiAkTalU
crnceDy+sLBAWiKRyPLycmlpqVwuL7oedlUDFotFKpXCj7W2tjqdznQ6neOEw+G2tjY+nw/TzGbz
9+/fYVcOh0On0+X1uAcPHuRtwYWm0A4C0un0rVu34CVpNJpWq43FYjlONpvFcbyzsxOm0en0jo4O
2FU2m8UwTKfT5dGGh4f3UQBBEKurq2NjY/CqLBZLr9fb7XaYtri4aLFYYJpAIDAYDMFgENbgcDiM
RiNMk8vlxfW43RYxh8NBEEQoFLpcru3tbQAAjuN+vz8cDvP5fLVaTdKEQqFYLGYwGKQwAACGYX6/
f2NjQ6lUisVi8u1JJBKhUAj3uGg0iqLoxsZGbW1tRUXFf4XxbxTQyLhcbkNDQygUWltbi0Qiuedx
HEcQRKFQkBYEQY4fP46i6NraGjnNpFKpmZkZNpstkUhIDQAAtVpdVVVFbkRkO9/a2rLb7SKRSCKR
8Hi8ggQAUMhZaHBwMG/TaGpqCgQCeT3OYDDkfZfd3d1wLhEE4fF4NBoNg8GAaVardXNzs6AUKkzA
5uam1WqFl2QwGBqNxuPxwLRgMNjd3Q3TKioqDAYDzMFxPBAINDU15b3qwcHBggQUdhZiMplSqVSh
UHz+/Jk84aTT6Ugk4nK5FApFbopns9lKpRLucalUKhQKzczM6HQ6csOl0+lHjx6tqalJJBK5evj1
69fPnz9XV1fPnTt35MiR3wdT5GGObGQlJSU+n29rawsAkMlkVlZWkskkl8vN1bRYLJZIJNlsdmlp
KZVKAQAwDCMbGdzjFAoFl8vFMGx+fp60RCIRj8eTzWbJ9rejAAAKH2hImM1muVwOezQajQ6HAz7M
BYNBg8EgEAhgmsViWVxchF3Z7Xa9Xp/X48bGxlZXV3dMoeIFEAQxPDxMp//jXkOn02EYBmsgCKKj
oyOP1tnZieM4TIvFYlqtlkajwbTbt2/vKGBP1yo9PT3379+HLYuLi2fPnp2dnYWNd+/evXr1Kmx5
//79hQsXyAwkwWQynz9/fv78+UJj2NO1SmVlJYIgEolkdnaWHG5wHEdRNBQKCYXCXD3w+XyxWMxk
Mqenp0kLhmEoivr9frVaTc5AdDpdJBKJRKJcj+vr62tvb5fJZP+1epFFnAcul6vVasPhMIqiuR63
sLCA43hVVVVuX0IQRKVSbW5uBgIBssclk0mHw8FkMslyJ2kqlUogEKTT6fr6eovFkruz+Y0AAPZQ
AzAGBgbIGSCH5uZmj8eTN9+0tbVxOByY1tvbW9yBlIIihhGNRkdGRuDIysrKNBpNIBDIcbLZbDgc
7urqgmlMJtNoNBYtgIKrRRLl5eVyuVypVH78+JEgCABAJpOJRqMzMzM1NTXkeYlGozEYDLVazWQy
cz0unU6HQiG73X769OmChhtqagAGOT2yWCy32x2NRsngPB5PIpGAe5xIJCLvxcjBCACQSCR8Pt+l
S5dylxq7FwAARSkEw2w2546oJEwmU9784Pf7jUYjWTY8Hq+lpcXtdhe0CsU1kIehoaG8k6Zer4/F
YvAsShCEyWRis9ktLS1FLLG/AlAUHR0dhQWwWCytVut0OmGa1+udm5sr9Lsnsb8CCIJwu91WqxWu
SxqN1traOjk5SYl/UgBlP/L9GyqV6sqVK+vr669evfJ6vQAAgiBev37NYrHodHpjYyMlq+yjAAAA
j8e7d+9eJpN58eJFMBgkjS9fvqRQwJ/4ofvOnTt5hzkKsb9vgASTybx48SKbze7v7wcA9PX19fT0
UOX8TwgAACiVyvb29ng8DgBobm6uq6ujyvMfEgAAqK6uvnHjBuVuD//Z46BxKOCgcSjgoHEo4KBR
CgDIXdf8XfhLw87H/wBslqSSSmyDdgAAAABJRU5ErkJggg==','/wlb_originals/wdg_ElFigure','','','','',-1,'name;geomX;geomY;geomW;geomH;geomZ;lineWdth;elLst;',2);
INSERT INTO "wlb_ElectroEls" VALUES('El_KeySqr_6','iVBORw0KGgoAAAANSUhEUgAAAEAAAABACAIAAAAlC+aJAAAACXBIWXMAAA1hAAANsAGkB5ZwAAAF
KklEQVRogdVaXyh0TRifPc7OiijajVLClqK2tpcs1r8ikUhs1HKx167k1h0XygW12juJkm2FtBJJ
sf6T/SP/iqR1ocUJbyJ77J79Lmaa77Te70v2rH3nd/U8s+fM7/ntzHPOnHlG1traWllZCaQGx3F9
fX3EraqqamlpkZzF4XAAs9kcigKurq7ETN3d3dFgMZvNLCJwuVwOhwPZSqWypKRkYWEBuRBCg8Ew
NTVFojGZTOPj48Tt7Oy02WwfHx/IbWpq2traury8FAtwuVzDw8MAgIKCAp7nj4+PUXtubq5Kpdre
3kZuenq6VqtdXl5GbmJiYkNDw/T09GfqysrKX79+AQBYMhb7+/sVFRUAALlczjCMQqEgAmQyGXEB
AJ9dCCHDMMhlGAZCCCEUC4iLi0O3sCwrCAK5nWVZ8hPiCnP/SL2xsQEAQALwFBoaGrJYLBIOblSn
kMViGRoaComnkMlkkslkgELgcR8fHxfP8r8cqampSqUS2WxsQ/keioqKBEFANhPbUL6H5eVl8pCk
UoAYeAq1tbVJlcSHh4c9PT3v7+/ixunpaafTqdfrBwYGJGEhwAIWFxchhCaTKfIen5+fNzc3wxpv
b29vb29TUlIi7x8AACEMhULIxgJeX18DgYAkvf8AmpqaiAAqc2BmZoY89KkUIAaeQnV1dXFxcbEN
5XvAAjweD4QwNzc3ttF8A1iAz+cTL/r+chiNxqgksc/n29nZOT09/a8LHh8fd3Z2fD5fhERTU1Pk
g0TKtZDRaFxbW/ufC7a2tvR6fWZm5uzsbGFhoSSkeAT0er1Op4uwr4eHh/7+/t8iDA4OqtVqccv9
/f3Nzc3Ly0vEkWOwhJtlJRiN+Pj45ORkscswjLjF7/dHziIGDvri4oKiJG5ra6P7TTw3N2ez2ZBN
pYBAIEA2QfAU0mg0kuTAzwMHDSGkW4DT6VQoFKWlpbGN5ouor68n38RU/ut7e3s8z6vVakCpgKen
J/I+wQKysrLkcnnsQvo+sIDs7Gy6vwfW1tYUCoVGo4ltNF9EeXl5MBhENpUvMq/Xe319jWwqk/jm
5iY8iVNTU8N29GkBFlBWVkYqFHQBC7Db7QqFoqurK7bRfBFarZZsw1GZAzzPEwFUTpuzszNSJsQC
5HI53avR9vZ2SmtkWMDk5CRFSaxWq+lOYqVSSZYSUgpQqVQ2m+3k5IS0nJ+f+3w+cd0kGAxmZmYm
JSVFQrS/v+/3+4uKioC0Anp7e0dHR71e7/r6urh9YmICGRkZGeXl5WNjY1JtywEiQJJCd3V1dXV1
9erqapgAgoKCAqvVGiFLGKgsdKelpaWnpyObyiTWaDRRSeIfw+rqqt/vz8vLA5QuJcTAI2A0Gil9
E+MRmJmZsdvtsQ3l60hISEhMTEQ2HgGe5ykagdra2ihur+t0OrfbTU6TIHR0dLjdbnRsLnLMz8+T
g3TSP4WSkpK0Wq24KgMAUKlUWq1Wci5ABDQ2NtL9Tby7uyuXy3NycmIbzTeABXAcR1GNLFqF7h/D
nwvd6DSp0WgUr+qam5tXVlbe3t6QW1NTc3x8fHd3h1ydTsdxHDkimp+fDyH0eDwAAI7jxJRHR0dW
q7W4uHhpaQkTs2xLS4v4VG4YtcFgsNvtPM8jt66u7uDg4PHxEYVKqtpYADr/7ff7Q6GQuJQbCoV4
nictwWAwzA0EAsQNBAIMwyCXEItvFASBXCwIwmeuzy7pRxCEj48PdIFOp/v3wDr1h7/JmW9pETaF
PB7PyMiI5CwOh+Mf3mo0OI3AkzIAAAAASUVORK5CYII=','/wlb_originals/wdg_ElFigure','JavaLikeCalc.JavaScript
if( f_start ) errPresent=false;
if( st && !st.isEVal() )
{
  p1x=72; p1y=55;
  p2x=91; p2y=36;
  p3x=135; p3y=80;
  p4x=116; p4y=99;
  p5x=81; p5y=46;
  p6x=125; p6y=90;
  c1 = "red"; fillColor = lineClr = "black";
  if( !errPresent.isEVal() && !errPresent ) { Special.FLibSYS.messPut("Alarm_"+id,-4, id+" : "+DESCR+" неисправен!"); errPresent=true; }
}
else
{
  if( !errPresent.isEVal() && errPresent && !val.isEVal() ) { Special.FLibSYS.messPut("Alarm_"+id,2, id+" : "+DESCR+" :  "+" исправен."); errPresent=false; }
  if( !val && !val.isEVal() )
  {
    p1x=72; p1y=55;
    p2x=91; p2y=36;
    p3x=135; p3y=80;
    p4x=116; p4y=99;
    p5x=81; p5y=46;
    p6x=125; p6y=90;
    c1 = "white"; fillColor = lineClr = "black";
  }
  else if( val && !val.isEVal() )
  {
    p1x=60; p1y=45;
    p2x=90; p2y=45;
    p3x=90; p3y=105;
    p4x=60; p4y=105;
    p5x=75; p5y=45;
    p6x=75; p6y=105;
    c1 = "lime"; fillColor = lineClr = "black";
  }
  else if( val.isEVal() ) c1 = fillColor = lineClr = "darkgrey";
}
','JavaLikeCalc.JavaScript
if( f_start ) errPresent=false;
if( st && !st.isEVal() )
{
  p1x=72; p1y=55;
  p2x=91; p2y=36;
  p3x=135; p3y=80;
  p4x=116; p4y=99;
  p5x=81; p5y=46;
  p6x=125; p6y=90;
  c1 = "red"; fillColor = lineClr = "black";
  if( !errPresent.isEVal() && !errPresent ) { Special.FLibSYS.messPut("Alarm_"+id,-4, id+" : "+DESCR+" неисправен!"); errPresent=true; }
}
else
{
  if( !errPresent.isEVal() && errPresent && !val.isEVal() ) { Special.FLibSYS.messPut("Alarm_"+id,2, id+" : "+DESCR+" :  "+" исправен."); errPresent=false; }
  if( !val && !val.isEVal() )
  {
    p1x=72; p1y=55;
    p2x=91; p2y=36;
    p3x=135; p3y=80;
    p4x=116; p4y=99;
    p5x=81; p5y=46;
    p6x=125; p6y=90;
    c1 = "white"; fillColor = lineClr = "black";
  }
  else if( val && !val.isEVal() )
  {
    p1x=60; p1y=45;
    p2x=90; p2y=45;
    p3x=90; p3y=105;
    p4x=60; p4y=105;
    p5x=75; p5y=45;
    p6x=75; p6y=105;
    c1 = "lime"; fillColor = lineClr = "black";
  }
  else if( val.isEVal() ) c1 = fillColor = lineClr = "darkgrey";
}
','','',-1,'name;dscr;geomW;geomH;lineClr;fillColor;elLst;p1x;p1y;p2x;p2y;p3x;p3y;p4x;p4y;p5x;p5y;p6x;p6y;c1;s1;',2);
INSERT INTO "wlb_ElectroEls" VALUES('El_KeySqr_6_tst','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
8T8yUKQt2iUmhQAAAABJRU5ErkJggg==','/wlb_originals/wdg_ElFigure','JavaLikeCalc.JavaScript
if( !val && !val.isEVal() )
{
  p1x=85; p1y=60;
  p2x=85; p2y=90;
  p3x=15; p3y=90;
  p4x=15; p4y=60;
}
else if( val && !val.isEVal() )
{
  p1x=35; p1y=40;
  p2x=65; p2y=40;
  p3x=65; p3y=110;
  p4x=35; p4y=110;
}','','','',-1,'name;dscr;geomW;geomH;lineClr;elLst;p1x;p1y;p2x;p2y;p3x;p3y;p4x;p4y;p7x;p7y;p8x;p8y;c1;s1;',2);
INSERT INTO "wlb_ElectroEls" VALUES('El_Key_3_tst','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
cBJVrQIAAAAASUVORK5CYII=','/wlb_originals/wdg_ElFigure','JavaLikeCalc.JavaScript
if( !val && !val.isEVal() )
{
  p1x=76; p1y=46;
  p2x=86; p2y=36;
  p3x=126; p3y=76;
  p4x=116; p4y=86;
}
else if( val && !val.isEVal() )
{
  p1x=70; p1y=45;
  p2x=80; p2y=45;
  p3x=80; p3y=105;
  p4x=70; p4y=105;
}','','','',-1,'name;dscr;geomW;geomH;lineClr;elLst;p1x;p1y;p2x;p2y;p3x;p3y;p4x;p4y;p5x;p5y;p6x;p6y;p7x;p7y;p8x;p8y;c1;s1;',2);
CREATE TABLE 'wlb_ElectroEls_io' ("IDW" TEXT DEFAULT '' ,"IDC" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"IO_VAL" TEXT DEFAULT '' ,"en#IO_VAL" TEXT DEFAULT '' ,"ru#IO_VAL" TEXT DEFAULT '' ,"uk#IO_VAL" TEXT DEFAULT '' ,"SELF_FLG" INTEGER DEFAULT '' ,"CFG_TMPL" TEXT DEFAULT '' ,"en#CFG_TMPL" TEXT DEFAULT '' ,"ru#CFG_TMPL" TEXT DEFAULT '' ,"uk#CFG_TMPL" TEXT DEFAULT '' ,"CFG_VAL" TEXT DEFAULT '' ,"en#CFG_VAL" TEXT DEFAULT '' ,"ru#CFG_VAL" TEXT DEFAULT '' ,"uk#CFG_VAL" TEXT DEFAULT '' , PRIMARY KEY ("IDW","IDC","ID"));
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Accu','','name','Battery','Battery','Батарея','Батарея',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Accu','','geomW','90','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Accu','','geomH','30','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Accu','','elLst','line:(10|5):(10|25):::::
line:(8|10):(8|20):::::
line:(10|15):(18|15):::::
line:(20|5):(20|25):::::
line:(18|10):(18|20):::::
line:(20|15):(33|15):::::2
line:(35|5):(35|25):::::
line:(33|10):(33|20):::::
line:(35|15):(53|15):::::
line:(55|5):(55|25):::::
line:(53|10):(53|20):::::
line:(55|15):(68|15):::::2
line:(70|5):(70|25):::::
line:(68|10):(68|20):::::
line:(70|15):(78|15):::::
line:(80|5):(80|25):::::
line:(78|10):(78|20):::::','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Converter','','name','Direct current converter','Direct current converter','Преобразователь постоянного тока','Перетворювач постійного струму',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Converter','','geomW','30','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Converter','','geomH','30','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Converter','','elLst','line:(5|5):(25|5):::::
line:(25|5):(25|25):::::
line:(25|25):(5|25):::::
line:(5|25):(5|5):::::
line:(5|25):(25|5):::::
line:(15|18):(22|18):::::
line:(15|21):(22|21):::::
line:(8|9):(15|9):::::
line:(8|12):(15|12):::::','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Converter_1','','name','Rectifier','Rectifier','Выпрямитель','Випрямляч',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Converter_1','','geomW','30','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Converter_1','','geomH','30','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Converter_1','','elLst','line:(5|5):(25|5):::::
line:(25|5):(25|25):::::
line:(25|25):(5|25):::::
line:(5|25):(5|5):::::
line:(5|25):(25|5):::::
line:(15|18):(22|18):::::
line:(15|21):(22|21):::::
bezier:(7|11):(16|11):(9|5):(11|17):::::
','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Duga_edv','','owner','root:UI','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Duga_edv','','name','Reactor','Reactor','Реактор','Реактор',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Duga_edv','','geomW','100','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Duga_edv','','geomH','100','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Duga_edv','','lineClr','black','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Duga_edv','','elLst','arc:(50|80):(80|50):(50|50):(20|50):(50|80):::::
line:(50|80):(50|50):::::
line:(50|50):(5|50):::::
line:(80|50):(95|50):::::','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Duo','','owner','root:UI','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Duo','','name','Transformer','Transformer','Трансформатор','Трансформатор',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Duo','','geomW','100','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Duo','','geomH','100','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Duo','','lineClr','black','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Duo','','elLst','arc:(25|70):(25|70):(50|70):(50|45):(25|70):::::
arc:(25|30):(25|30):(50|30):(50|5):(25|30):::::
','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1','','name','Switch plank','Switch plank','Рубильник однополосный','Рубильник односмуговий',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1','','geomW','150','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1','','geomH','150','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1','','lineClr','#000000','','','',40,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1','','fillColor','','','','',40,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1','','elLst','line:(7|7):(143|7):2:c1:::s1
line:(143|143):(7|143):2:c1:::s1
line:(143|7):(143|143):2:c1:::s1
line:(7|143):(7|7):2:c1:::s1
line:1:2:::::
line:2:3:::::
line:3:4:::::
line:4:1:::::
line:(65|24):(85|24):::::
line:(85|24):(85|44):::::
line:(85|44):(65|44):::::
line:(65|44):(65|24):::::
line:(65|106):(85|106):::::
line:(85|106):(85|126):::::
line:(85|126):(65|126):::::
line:(65|126):(65|106):::::
line:(70|1):(70|23):::::
line:(70|1):(80|1):::::
line:(80|1):(80|23):::::
line:(70|23):(80|23):::::
line:(80|127):(80|149):::::
line:(70|127):(80|127):::::
line:(70|149):(80|149):::::
line:(70|127):(70|149):::::
fill:(70|1):(70|23):(80|23):(80|1)::
fill:(70|127):(70|149):(80|149):(80|127)::
fill:(65|24):(85|24):(85|44):(65|44)::
fill:(65|106):(85|106):(85|126):(65|126)::
fill:1:2:3:4::
','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1','','p1x','70','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1','','p1y','45','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1','','p2x','80','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1','','p2y','45','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1','','p3x','80','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1','','p3y','105','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1','','p4x','70','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1','','p4y','105','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1','','c1','#000000','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1','','s1','1','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1_tst','','name','El_KeySqr_1_tst','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1_tst','','geomW','150','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1_tst','','geomH','150','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1_tst','','lineClr','','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1_tst','','elLst','line:1:2:::::
line:2:3:::::
line:3:4:::::
line:4:1:::::
line:(65|24):(85|24):::::
line:(85|24):(85|44):::::
line:(85|44):(65|44):::::
line:(65|44):(65|24):::::
line:(65|106):(85|106):::::
line:(85|106):(85|126):::::
line:(85|126):(65|126):::::
line:(65|126):(65|106):::::
line:(70|1):(70|23):::::
line:(70|1):(80|1):::::
line:(80|1):(80|23):::::
line:(70|23):(80|23):::::
line:(80|127):(80|149):::::
line:(70|127):(80|127):::::
line:(70|149):(80|149):::::
line:(70|127):(70|149):::::
line:(7|143):(7|7):2:c1:::s1
line:(7|7):(143|7):2:c1:::s1
line:(143|143):(7|143):2:c1:::s1
line:(143|7):(143|143):2:c1:::s1
fill:(70|1):(70|23):(80|23):(80|1)::
fill:(70|127):(70|149):(80|149):(80|127)::
fill:(65|24):(85|24):(85|44):(65|44)::
fill:(65|106):(85|106):(85|126):(65|126)::
fill:1:2:3:4::
','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1_tst','','p1x','76','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1_tst','','p1y','46','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1_tst','','p2x','86','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1_tst','','p2y','36','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1_tst','','p3x','126','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1_tst','','p3y','76','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1_tst','','p4x','116','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1_tst','','p4y','86','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1_tst','','c1','#000000','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1_tst','','s1','1','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2','','name','Fuse-switch','Fuse-switch','Выключатель-предохранитель','Вимикач-запобіжник',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2','','geomW','150','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2','','geomH','150','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2','','lineClr','#000000','','','',40,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2','','fillColor','','','','',40,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2','','elLst','line:(7|143):(7|7):2:c1:::s1
line:(143|143):(7|143):2:c1:::s1
line:(7|7):(143|7):2:c1:::s1
line:(143|7):(143|143):2:c1:::s1
line:1:2:::::
line:2:3:::::
line:3:4:::::
line:4:1:::::
line:(65|24):(85|24):::::
line:(85|24):(85|44):::::
line:(85|44):(65|44):::::
line:(65|44):(65|24):::::
line:(65|106):(85|106):::::
line:(85|106):(85|126):::::
line:(85|126):(65|126):::::
line:(65|126):(65|106):::::
line:(70|1):(70|23):::::
line:(70|1):(80|1):::::
line:(80|1):(80|23):::::
line:(70|23):(80|23):::::
line:(80|127):(80|149):::::
line:(70|127):(80|127):::::
line:(70|149):(80|149):::::
line:(70|127):(70|149):::::
fill:(70|1):(70|23):(80|23):(80|1)::
fill:(70|127):(70|149):(80|149):(80|127)::
fill:(65|24):(85|24):(85|44):(65|44)::
fill:(65|106):(85|106):(85|126):(65|126)::
fill:1:2:3:4::
','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2','','p1x','60','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2','','p1y','45','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2','','p2x','90','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2','','p2y','45','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2','','p3x','90','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2','','p3y','105','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2','','p4x','60','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2','','p4y','105','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2','','c1','#000000','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2','','s1','1','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2_tst','','name','El_KeySqr_2_tst','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2_tst','','geomW','150','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2_tst','','geomH','150','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2_tst','','lineClr','','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2_tst','','elLst','line:1:2:::::
line:2:3:::::
line:3:4:::::
line:4:1:::::
line:(65|24):(85|24):::::
line:(85|24):(85|44):::::
line:(85|44):(65|44):::::
line:(65|44):(65|24):::::
line:(65|106):(85|106):::::
line:(85|106):(85|126):::::
line:(85|126):(65|126):::::
line:(65|126):(65|106):::::
line:(70|1):(70|23):::::
line:(70|1):(80|1):::::
line:(80|1):(80|23):::::
line:(70|23):(80|23):::::
line:(80|127):(80|149):::::
line:(70|127):(80|127):::::
line:(70|149):(80|149):::::
line:(70|127):(70|149):::::
line:(7|143):(7|7):2:c1:::s1
line:(7|7):(143|7):2:c1:::s1
line:(143|143):(7|143):2:c1:::s1
line:(143|7):(143|143):2:c1:::s1
fill:(70|1):(70|23):(80|23):(80|1)::
fill:(70|127):(70|149):(80|149):(80|127)::
fill:1:2:3:4::
fill:(65|24):(85|24):(85|44):(65|44)::
fill:(65|106):(85|106):(85|126):(65|126)::','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2_tst','','p1x','72','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2_tst','','p1y','55','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2_tst','','p2x','91','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2_tst','','p2y','36','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2_tst','','p3x','135','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2_tst','','p3y','80','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2_tst','','p4x','116','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2_tst','','p4y','99','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2_tst','','c1','#000000','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2_tst','','s1','1','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3','','name','Automatic switch plank','Automatic switch plank','Рубильник однополосный автоматический','Рубильник односмуговий автоматичний',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3','','geomW','150','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3','','geomH','150','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3','','lineClr','','','','',40,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3','','fillColor','','','','',40,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3','','elLst','line:(7|143):(7|7):2:c1:::s1
line:(143|143):(7|143):2:c1:::s1
line:(7|7):(143|7):2:c1:::s1
line:(143|7):(143|143):2:c1:::s1
line:1:2:::::
line:2:3:::::
line:3:4:::::
line:4:5:::::
line:5:6:::::
line:6:7:::::
line:7:8:::::
line:8:1:::::
line:(65|24):(85|24):::::
line:(85|24):(85|44):::::
line:(85|44):(65|44):::::
line:(65|44):(65|24):::::
line:(65|106):(85|106):::::
line:(85|106):(85|126):::::
line:(85|126):(65|126):::::
line:(65|126):(65|106):::::
line:(70|1):(70|23):::::
line:(70|1):(80|1):::::
line:(80|1):(80|23):::::
line:(70|23):(80|23):::::
line:(80|127):(80|149):::::
line:(70|127):(80|127):::::
line:(70|149):(80|149):::::
line:(70|127):(70|149):::::
fill:(70|1):(70|23):(80|23):(80|1)::
fill:(70|127):(70|149):(80|149):(80|127)::
fill:(65|24):(85|24):(85|44):(65|44)::
fill:(65|106):(85|106):(85|126):(65|126)::
fill:1:2:3:4:5:6:7:8::
','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3','','p1x','70','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3','','p1y','45','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3','','p2x','80','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3','','p2y','45','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3','','p3x','80','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3','','p3y','65','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3','','p4x','90','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3','','p4y','65','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3','','p5x','90','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3','','p5y','85','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3','','p6x','80','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3','','p6y','85','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3','','p7x','80','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3','','p7y','105','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3','','p8x','70','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3','','p8y','105','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3','','c1','#000000','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3','','s1','1','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3_tst','','name','El_KeySqr_3_tst','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3_tst','','geomW','150','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3_tst','','geomH','150','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3_tst','','lineClr','','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3_tst','','elLst','line:1:2:::::
line:2:3:::::
line:3:4:::::
line:4:5:::::
line:5:6:::::
line:6:7:::::
line:7:8:::::
line:8:1:::::
line:(65|24):(85|24):::::
line:(85|24):(85|44):::::
line:(85|44):(65|44):::::
line:(65|44):(65|24):::::
line:(65|106):(85|106):::::
line:(85|106):(85|126):::::
line:(85|126):(65|126):::::
line:(65|126):(65|106):::::
line:(70|1):(70|23):::::
line:(70|1):(80|1):::::
line:(80|1):(80|23):::::
line:(70|23):(80|23):::::
line:(80|127):(80|149):::::
line:(70|127):(80|127):::::
line:(70|149):(80|149):::::
line:(70|127):(70|149):::::
line:(7|143):(7|7):2:c1:::s1
line:(7|7):(143|7):2:c1:::s1
line:(143|143):(7|143):2:c1:::s1
line:(143|7):(143|143):2:c1:::s1
fill:(70|1):(70|23):(80|23):(80|1)::
fill:(70|127):(70|149):(80|149):(80|127)::
fill:(65|24):(85|24):(85|44):(65|44)::
fill:(65|106):(85|106):(85|126):(65|126)::
fill:1:2:3:4:5:6:7:8::
','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3_tst','','p1x','76','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3_tst','','p1y','46','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3_tst','','p2x','86','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3_tst','','p2y','36','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3_tst','','p3x','99','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3_tst','','p3y','49','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3_tst','','p4x','108','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3_tst','','p4y','40','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3_tst','','p5x','124','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3_tst','','p5y','56','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3_tst','','p6x','115','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3_tst','','p6y','65','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3_tst','','p7x','129','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3_tst','','p7y','79','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3_tst','','p8x','119','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3_tst','','p8y','89','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3_tst','','c1','#000000','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3_tst','','s1','1','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','','name','Automatic dual band switch','Automatic dual band switch','Рубильник двухполосный автоматический','Рубильник двохсмуговий автоматичний',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','','geomW','300','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','','geomH','150','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','','lineClr','','','','',40,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','','fillColor','','','','',40,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','','elLst','line:(7|143):(7|7):2:c1:::s1
line:(293|143):(7|143):2:c1:::s1
line:(293|7):(293|143):2:c1:::s1
line:(7|7):(293|7):2:c1:::s1
line:(215|24):(235|24):::::
line:(235|24):(235|44):::::
line:(235|44):(215|44):::::
line:(215|44):(215|24):::::
line:(215|106):(235|106):::::
line:(235|106):(235|126):::::
line:(235|126):(215|126):::::
line:(215|126):(215|106):::::
line:(220|1):(220|23):::::
line:(220|1):(230|1):::::
line:(230|1):(230|23):::::
line:(220|23):(230|23):::::
line:(230|127):(230|149):::::
line:(220|127):(230|127):::::
line:(220|149):(230|149):::::
line:(220|127):(220|149):::::
line:9:10:::::
line:10:11:::::
line:14:15:::::
line:15:16:::::
line:16:9:::::
line:11:17:::::
line:14:18:::::
line:1:2:::::
line:2:3:::::
line:3:4:::::
line:4:5:::::
line:5:6:::::
line:6:7:::::
line:7:8:::::
line:8:18:::::
line:17:1:::::
line:(65|24):(85|24):::::
line:(85|24):(85|44):::::
line:(85|44):(65|44):::::
line:(65|44):(65|24):::::
line:(65|106):(85|106):::::
line:(85|106):(85|126):::::
line:(85|126):(65|126):::::
line:(65|126):(65|106):::::
line:(70|1):(70|23):::::
line:(70|1):(80|1):::::
line:(80|1):(80|23):::::
line:(70|23):(80|23):::::
line:(80|127):(80|149):::::
line:(70|127):(80|127):::::
line:(70|149):(80|149):::::
line:(70|127):(70|149):::::
fill:(220|1):(220|23):(230|23):(230|1)::
fill:(220|127):(220|149):(230|149):(230|127)::
fill:(215|24):(235|24):(235|44):(215|44)::
fill:(215|106):(235|106):(235|126):(215|126)::
fill:(70|1):(70|23):(80|23):(80|1)::
fill:(70|127):(70|149):(80|149):(80|127)::
fill:(65|24):(85|24):(85|44):(65|44)::
fill:(65|106):(85|106):(85|126):(65|126)::
fill:9:10:11:17:1:2:3:4:5:6:7:8:18:14:15:16::
','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','','p1x','220','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','','p1y','45','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','','p2x','230','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','','p2y','45','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','','p3x','230','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','','p3y','65','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','','p4x','240','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','','p4y','65','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','','p5x','240','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','','p5y','85','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','','p6x','230','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','','p6y','85','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','','p7x','230','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','','p7y','105','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','','p8x','220','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','','p8y','105','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','','p9x','70','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','','p9y','45','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','','p10x','80','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','','p10y','45','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','','p11x','80','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','','p11y','65','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','','p14x','80','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','','p14y','85','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','','p15x','80','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','','p15y','105','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','','p16x','70','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','','p16y','105','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','','p17x','220','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','','p17y','65','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','','p18x','220','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','','p18y','85','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','','c1','#000000','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','','s1','1','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','','name','El_KeySqr_4_tst','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','','geomW','300','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','','geomH','150','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','','lineClr','','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','','elLst','line:(7|7):(293|7):2:c1:::s1
line:(293|7):(293|143):2:c1:::s1
line:(7|143):(7|7):2:c1:::s1
line:(293|143):(7|143):2:c1:::s1
line:(215|24):(235|24):::::
line:(235|24):(235|44):::::
line:(235|44):(215|44):::::
line:(215|44):(215|24):::::
line:(215|106):(235|106):::::
line:(235|106):(235|126):::::
line:(235|126):(215|126):::::
line:(215|126):(215|106):::::
line:(220|1):(220|23):::::
line:(220|1):(230|1):::::
line:(230|1):(230|23):::::
line:(220|23):(230|23):::::
line:(230|127):(230|149):::::
line:(220|127):(230|127):::::
line:(220|149):(230|149):::::
line:(220|127):(220|149):::::
line:9:10:::::
line:10:11:::::
line:14:15:::::
line:15:16:::::
line:16:9:::::
line:11:17:::::
line:14:18:::::
line:1:2:::::
line:2:3:::::
line:3:4:::::
line:4:5:::::
line:5:6:::::
line:6:7:::::
line:7:8:::::
line:8:18:::::
line:17:1:::::
line:(65|24):(85|24):::::
line:(85|24):(85|44):::::
line:(85|44):(65|44):::::
line:(65|44):(65|24):::::
line:(65|106):(85|106):::::
line:(85|106):(85|126):::::
line:(85|126):(65|126):::::
line:(65|126):(65|106):::::
line:(70|1):(70|23):::::
line:(70|1):(80|1):::::
line:(80|1):(80|23):::::
line:(70|23):(80|23):::::
line:(80|127):(80|149):::::
line:(70|127):(80|127):::::
line:(70|149):(80|149):::::
line:(70|127):(70|149):::::
fill:(220|1):(220|23):(230|23):(230|1)::
fill:(220|127):(220|149):(230|149):(230|127)::
fill:(215|24):(235|24):(235|44):(215|44)::
fill:(215|106):(235|106):(235|126):(215|126)::
fill:(70|1):(70|23):(80|23):(80|1)::
fill:(70|127):(70|149):(80|149):(80|127)::
fill:(65|24):(85|24):(85|44):(65|44)::
fill:(65|106):(85|106):(85|126):(65|126)::
fill:9:10:11:17:1:2:3:4:5:6:7:8:18:14:15:16::
','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','','p1x','226','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','','p1y','46','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','','p2x','236','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','','p2y','36','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','','p3x','249','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','','p3y','49','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','','p4x','259','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','','p4y','39','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','','p5x','276','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','','p5y','56','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','','p6x','266','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','','p6y','66','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','','p7x','279','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','','p7y','79','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','','p8x','269','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','','p8y','89','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','','p9x','76','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','','p9y','46','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','','p10x','86','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','','p10y','36','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','','p11x','108','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','','p11y','58','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','','p14x','120','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','','p14y','70','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','','p15x','129','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','','p15y','79','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','','p16x','119','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','','p16y','89','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','','p17x','238','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','','p17y','58','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','','p18x','250','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','','p18y','70','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','','s1','1','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5','','name','Switch with the neutral central position','Switch with the neutral central position','Переключатель с нейтральным центральным положением','Перемикач з нейтральним центральним положенням',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5','','geomW','150','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5','','geomH','150','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5','','lineClr','','','','',40,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5','','fillColor','','','','',40,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5','','elLst','line:(7|143):(7|7):2:c1:::s1
line:(143|143):(7|143):2:c1:::s1
line:(7|7):(143|7):2:c1:::s1
line:(143|7):(143|143):2:c1:::s1
line:1:2:::::
line:2:3:::::
line:3:4:::::
line:4:1:::::
line:(65|24):(85|24):::::
line:(85|24):(85|44):::::
line:(85|44):(65|44):::::
line:(65|44):(65|24):::::
line:(110|76):(130|76):::::
line:(130|76):(130|96):::::
line:(130|96):(110|96):::::
line:(110|96):(110|76):::::
line:(70|1):(70|23):::::
line:(70|1):(80|1):::::
line:(80|1):(80|23):::::
line:(70|23):(80|23):::::
line:(125|95):(125|149):::::
line:(115|95):(125|95):::::
line:(115|149):(125|149):::::
line:(115|95):(115|149):::::
line:(20|76):(40|76):::::
line:(40|76):(40|96):::::
line:(40|96):(20|96):::::
line:(20|96):(20|76):::::
line:(35|95):(35|149):::::
line:(25|95):(35|95):::::
line:(25|149):(35|149):::::
line:(25|95):(25|149):::::
fill:(70|1):(70|23):(80|23):(80|1)::
fill:(115|95):(115|149):(125|149):(125|95)::
fill:(65|24):(85|24):(85|44):(65|44)::
fill:(110|76):(130|76):(130|96):(110|96)::
fill:1:2:3:4::
fill:(25|95):(25|149):(35|149):(35|95)::
fill:(20|76):(40|76):(40|96):(20|96)::
','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5','','p1x','70','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5','','p1y','45','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5','','p2x','80','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5','','p2y','45','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5','','p3x','80','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5','','p3y','85','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5','','p4x','70','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5','','p4y','85','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5','','c1','','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5','','s1','1','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5_tst','','name','El_KeySqr_5_tst','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5_tst','','geomW','150','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5_tst','','geomH','150','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5_tst','','lineClr','','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5_tst','','elLst','line:1:2:::::
line:2:3:::::
line:3:4:::::
line:4:1:::::
line:(65|24):(85|24):::::
line:(85|24):(85|44):::::
line:(85|44):(65|44):::::
line:(65|44):(65|24):::::
line:(110|76):(130|76):::::
line:(130|76):(130|96):::::
line:(130|96):(110|96):::::
line:(110|96):(110|76):::::
line:(70|1):(70|23):::::
line:(70|1):(80|1):::::
line:(80|1):(80|23):::::
line:(70|23):(80|23):::::
line:(125|95):(125|149):::::
line:(115|95):(125|95):::::
line:(115|149):(125|149):::::
line:(115|95):(115|149):::::
line:(7|143):(7|7):2:c1:::s1
line:(7|7):(143|7):2:c1:::s1
line:(143|143):(7|143):2:c1:::s1
line:(143|7):(143|143):2:c1:::s1
line:(20|76):(40|76):::::
line:(40|76):(40|96):::::
line:(40|96):(20|96):::::
line:(20|96):(20|76):::::
line:(35|95):(35|149):::::
line:(25|95):(35|95):::::
line:(25|149):(35|149):::::
line:(25|95):(25|149):::::
fill:(70|1):(70|23):(80|23):(80|1)::
fill:(115|95):(115|149):(125|149):(125|95)::
fill:(65|24):(85|24):(85|44):(65|44)::
fill:(110|76):(130|76):(130|96):(110|96)::
fill:1:2:3:4::
fill:(25|95):(25|149):(35|149):(35|95)::
fill:(20|76):(40|76):(40|96):(20|96)::
','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5_tst','','p1x','64','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5_tst','','p1y','36','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5_tst','','p2x','74','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5_tst','','p2y','46','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5_tst','','p3x','34','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5_tst','','p3y','86','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5_tst','','p4x','24','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5_tst','','p4y','76','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5_tst','','c1','#000000','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5_tst','','s1','1','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1','','name','Switch plank(circle)','Switch plank(circle)','Рубильник однополосный(круг)','Рубильник односмуговий (коло)',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1','','geomW','150','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1','','geomH','150','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1','','lineClr','','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1','','elLst','line:1:2:::::
line:2:3:::::
line:3:4:::::
line:4:1:::::
arc:(65|34):(65|34):(75|34):(85|34):(75|24):::::
arc:(65|116):(65|116):(75|116):(75|106):(65|116):::::
line:(70|1):(70|23):::::
line:(70|1):(80|1):::::
line:(80|1):(80|23):::::
line:(70|23):(80|23):::::
line:(80|127):(80|149):::::
line:(70|127):(80|127):::::
line:(70|149):(80|149):::::
line:(70|127):(70|149):::::
line:(7|143):(7|7):2:c1:::s1
line:(7|7):(143|7):2:c1:::s1
line:(143|143):(7|143):2:c1:::s1
line:(143|7):(143|143):2:c1:::s1
fill:1:2:3:4::
fill:(70|1):(70|23):(80|23):(80|1)::
fill:(70|127):(70|149):(80|149):(80|127)::
fill:(65|34):(65|34)::
fill:(65|116):(65|116)::
','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1','','p1x','70','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1','','p1y','45','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1','','p2x','80','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1','','p2y','45','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1','','p3x','80','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1','','p3y','105','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1','','p4x','70','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1','','p4y','105','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1','','c1','#000000','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1','','s1','1','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1_tst','','name','El_KeyRnd_1_tst','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1_tst','','geomW','150','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1_tst','','geomH','150','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1_tst','','lineClr','','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1_tst','','elLst','line:1:2:::::
line:2:3:::::
line:3:4:::::
line:4:1:::::
arc:(65|34):(65|34):(75|34):(85|34):(75|24):::::
arc:(65|116):(65|116):(75|116):(75|106):(65|116):::::
line:(70|1):(70|23):::::
line:(70|1):(80|1):::::
line:(80|1):(80|23):::::
line:(70|23):(80|23):::::
line:(80|127):(80|149):::::
line:(70|127):(80|127):::::
line:(70|149):(80|149):::::
line:(70|127):(70|149):::::
line:(7|143):(7|7):2:c1:::s1
line:(7|7):(143|7):2:c1:::s1
line:(143|143):(7|143):2:c1:::s1
line:(143|7):(143|143):2:c1:::s1
fill:1:2:3:4::
fill:(70|1):(70|23):(80|23):(80|1)::
fill:(70|127):(70|149):(80|149):(80|127)::
fill:(65|34):(65|34)::
fill:(65|116):(65|116)::
','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1_tst','','p1x','76','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1_tst','','p1y','46','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1_tst','','p2x','86','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1_tst','','p2y','36','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1_tst','','p3x','126','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1_tst','','p3y','76','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1_tst','','p4x','116','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1_tst','','p4y','86','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1_tst','','c1','#000000','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1_tst','','s1','1','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2','','name','Fuse-switch(circle)','Fuse-switch(circle)','Выключатель-предохранитель(круг)','Вимикач-запобіжник (коло)',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2','','geomW','150','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2','','geomH','150','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2','','lineClr','','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2','','elLst','line:1:2:::::
line:2:3:::::
line:3:4:::::
line:4:1:::::
arc:(65|34):(65|34):(75|34):(85|34):(75|24):::::
arc:(65|116):(65|116):(75|116):(75|106):(65|116):::::
line:(70|1):(70|23):::::
line:(70|1):(80|1):::::
line:(80|1):(80|23):::::
line:(70|23):(80|23):::::
line:(80|127):(80|149):::::
line:(70|127):(80|127):::::
line:(70|149):(80|149):::::
line:(70|127):(70|149):::::
line:(7|143):(7|7):2:c1:::s1
line:(7|7):(143|7):2:c1:::s1
line:(143|143):(7|143):2:c1:::s1
line:(143|7):(143|143):2:c1:::s1
fill:1:2:3:4::
fill:(70|1):(70|23):(80|23):(80|1)::
fill:(70|127):(70|149):(80|149):(80|127)::
fill:(65|34):(65|34)::
fill:(65|116):(65|116)::
','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2','','p1x','60','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2','','p1y','45','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2','','p2x','90','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2','','p2y','45','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2','','p3x','90','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2','','p3y','105','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2','','p4x','60','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2','','p4y','105','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2','','c1','#000000','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2','','s1','1','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2_tst','','name','El_KeyRnd_2_tst','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2_tst','','geomW','150','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2_tst','','geomH','150','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2_tst','','lineClr','','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2_tst','','elLst','line:1:2:::::
line:2:3:::::
line:3:4:::::
line:4:1:::::
arc:(65|34):(65|34):(75|34):(85|34):(75|24):::::
arc:(65|116):(65|116):(75|116):(75|106):(65|116):::::
line:(70|1):(70|23):::::
line:(70|1):(80|1):::::
line:(80|1):(80|23):::::
line:(70|23):(80|23):::::
line:(80|127):(80|149):::::
line:(70|127):(80|127):::::
line:(70|149):(80|149):::::
line:(70|127):(70|149):::::
line:(7|143):(7|7):2:c1:::s1
line:(7|7):(143|7):2:c1:::s1
line:(143|143):(7|143):2:c1:::s1
line:(143|7):(143|143):2:c1:::s1
fill:1:2:3:4::
fill:(70|1):(70|23):(80|23):(80|1)::
fill:(70|127):(70|149):(80|149):(80|127)::
fill:(65|34):(65|34)::
fill:(65|116):(65|116)::
','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2_tst','','p1x','72','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2_tst','','p1y','55','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2_tst','','p2x','91','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2_tst','','p2y','36','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2_tst','','p3x','135','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2_tst','','p3y','80','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2_tst','','p4x','116','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2_tst','','p4y','99','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2_tst','','c1','#000000','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2_tst','','s1','1','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3','','name','Automatic switch plank(circle)','Automatic switch plank(circle)','Рубильник однополосный автоматический(круг)','Рубильник односмуговий автоматичний (коло)',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3','','geomW','150','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3','','geomH','150','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3','','lineClr','','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3','','elLst','line:1:2:::::
line:2:3:::::
line:3:4:::::
line:4:5:::::
line:5:6:::::
line:6:7:::::
line:7:8:::::
line:8:1:::::
arc:(65|34):(65|34):(75|34):(85|34):(75|24):::::
arc:(65|116):(65|116):(75|116):(75|106):(65|116):::::
line:(70|1):(70|23):::::
line:(70|1):(80|1):::::
line:(80|1):(80|23):::::
line:(70|23):(80|23):::::
line:(80|127):(80|149):::::
line:(70|127):(80|127):::::
line:(70|149):(80|149):::::
line:(70|127):(70|149):::::
line:(7|143):(7|7):2:c1:::s1
line:(7|7):(143|7):2:c1:::s1
line:(143|143):(7|143):2:c1:::s1
line:(143|7):(143|143):2:c1:::s1
fill:1:2:3:4:5:6:7:8::
fill:(70|1):(70|23):(80|23):(80|1)::
fill:(70|127):(70|149):(80|149):(80|127)::
fill:(65|34):(65|34)::
fill:(65|116):(65|116)::
','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3','','p1x','70','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3','','p1y','45','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3','','p2x','80','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3','','p2y','45','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3','','p3x','80','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3','','p3y','65','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3','','p4x','90','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3','','p4y','65','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3','','p5x','90','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3','','p5y','85','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3','','p6x','80','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3','','p6y','85','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3','','p7x','80','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3','','p7y','105','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3','','p8x','70','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3','','p8y','105','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3','','c1','#000000','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3','','s1','1','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_h','','name','Switch','Switch','Выключатель','Вимикач',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_h','','geomW','150','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_h','','geomH','100','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_h','','lineWdth','0','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_h','','lineClr','','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_h','','elLst','line:1:2:::::
line:2:3:::::
line:3:4:::::
line:4:1:::::
arc:(20|35):(20|35):(20|50):(35|50):(20|35):::::
arc:(130|35):(130|35):(130|50):(145|50):(130|35):::::
fill:1:2:3:4::
','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_h','','p1x','40','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_h','','p1y','35','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_h','','p2x','40','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_h','','p2y','65','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_h','','p3x','110','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_h','','p3y','65','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_h','','p4x','110','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_h','','p4y','35','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Trio','','owner','root:UI','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Trio','','name','Transformer with two secondary windings','Transformer with two secondary windings','Трансформатор с двумя вторичными обмотками','Трансформатор з двома вторинними обмотками',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Trio','','geomW','100','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Trio','','geomH','100','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Trio','','lineClr','black','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Trio','','elLst','arc:(5|65):(5|65):(30|65):(30|40):(5|65):::::
arc:(25|30):(25|30):(50|30):(50|5):(25|30):::::
arc:(45|65):(45|65):(70|65):(70|40):(45|65):::::
','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Zemlia','','owner','root:UI','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Zemlia','','name','Ground','Ground','Заземление','Заземлення',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Zemlia','','geomW','100','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Zemlia','','geomH','100','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Zemlia','','lineClr','black','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Zemlia','','fillColor','black','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Zemlia','','elLst','line:(16|50):(31|50):::::
line:(31|15):(41|25):::::
line:(41|25):(41|75):::::
line:(41|75):(31|85):::::
line:(31|85):(31|15):::::
line:(51|30):(61|40):::::
line:(61|40):(61|60):::::
line:(61|60):(51|70):::::
line:(51|70):(51|30):::::
line:(71|45):(71|55):::::
line:(71|55):(81|50):::::
line:(71|45):(81|50):::::
line:(72|46):(72|54):::::
line:(73|47):(73|53):::::
line:(74|48):(74|52):::::
line:(75|48):(75|52):::::
line:(76|49):(76|51):::::
line:(77|49):(77|51):::::
line:(81|50):(71|50):::::
fill:(31|15):(41|25):(41|75):(31|85)::
fill:(51|30):(61|40):(61|60):(51|70)::','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('Induct','','owner','root:UI','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('Induct','','name','Coil','Coil','Индуктивность','Індуктивність',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('Induct','','geomW','100','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('Induct','','geomH','100','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('Induct','','geomXsc','1','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('Induct','','geomYsc','1','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('Induct','','lineWdth','5','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('Induct','','elLst','arc:(50|35):(50|5):(50|20):(50|5):(35|20)
arc:(50|95):(50|65):(50|80):(50|65):(35|80)
arc:(50|65):(50|35):(50|50):(50|35):(35|50)
','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Krug','','name','El_Krug','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Krug','','geomW','30','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Krug','','geomH','30','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Krug','','lineWdth','2','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Krug','','lineClr','','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Krug','','elLst','arc:(5|15):(5|15):(15|15):(15|5):(5|15):::::
','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_lhor','','name','El_lhor','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_lhor','','geomH','10','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_lhor','','lineWdth','0','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_lhor','','lineClr','','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_lhor','','elLst','line:(0|0):(100|0):::::
line:(100|0):(100|10):::::
line:(100|10):(0|10):::::
line:(0|10):(0|0):::::
fill:(0|0):(100|0):(100|10):(0|10)::
','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_lvert','','name','El_lvert','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_lvert','','geomW','25','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_lvert','','lineWdth','0','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_lvert','','lineClr','','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_lvert','','elLst','line:(0|0):(10|0):::::
line:(10|0):(10|100):::::
line:(10|100):(0|100):::::
line:(0|100):(0|0):::::
fill:(0|0):(10|0):(10|100):(0|100)::
','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('Sharp','','name','','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('Sharp','','geomX','827','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('Sharp','','geomY','412','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('Sharp','','geomW','50','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('Sharp','','geomH','50','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('Sharp','','geomZ','7','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('Sharp','','lineWdth','4','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('Sharp','','elLst','line:(19|9):(40|30):::::
line:(14|14):(35|35):::::
line:(9|19):(30|40):::::
','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Trio','','dscr','Transformer with two secondary windings','Transformer with two secondary windings','ГОСТ 2.723-68','ГОСТ 2.723-68',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Duga_edv','','dscr','Reactor','Reactor','ГОСТ 2.723-68','ГОСТ 2.723-68',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Duo','','dscr','Transformer','Transformer','ГОСТ 2.723-68','ГОСТ 2.723-68',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Zemlia','','dscr','Ground','Ground','ГОСТ 2.721-74','ГОСТ 2.721-74',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('Induct','','dscr','Coil','Coil','ГОСТ 2.723-68','ГОСТ 2.723-68',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1','','dscr','Switch plank
','Switch plank
','ГОСТ 2.755-87','ГОСТ 2.755-87',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1_tst','','dscr','','','ГОСТ 2.755-87','ГОСТ 2.755-87',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3','','dscr','Automatic switch plank','Automatic switch plank','ГОСТ 2.755-87','ГОСТ 2.755-87',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3_tst','','dscr','','','ГОСТ 2.755-87','ГОСТ 2.755-87',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','','dscr','Automatic dual band switch','Automatic dual band switch','ГОСТ 2.755-87','ГОСТ 2.755-87',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','','dscr','','','ГОСТ 2.755-87','ГОСТ 2.755-87',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1','','dscr','Switch plank(circle)','Switch plank(circle)','ГОСТ 2.755-87','ГОСТ 2.755-87',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1_tst','','dscr','','','ГОСТ 2.755-87','ГОСТ 2.755-87',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3','','dscr','Automatic switch plank(circle)','Automatic switch plank(circle)','ГОСТ 2.755-87','ГОСТ 2.755-87',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Converter','','dscr','Direct current converter','Direct current converter','ГОСТ 2.737-68','ГОСТ 2.737-68',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Converter_1','','dscr','Rectifier','Rectifier','ГОСТ 2.737-68','ГОСТ 2.737-68',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6','','name','Fuse-switch 2','Fuse-switch 2','Предохранитель-выключатель 2','Запобіжник-вимикач 2',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6','','geomW','150','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6','','geomH','150','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6','','lineClr','#000000','','','',40,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6','','fillColor','','','','',40,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6','','elLst','line:(7|143):(7|7):2:c1:::s1
line:(143|143):(7|143):2:c1:::s1
line:(7|7):(143|7):2:c1:::s1
line:(143|7):(143|143):2:c1:::s1
line:1:2:3::::
line:2:3:3::::
line:3:4:3::::
line:4:1:3::::
line:(65|24):(85|24):::::
line:(85|24):(85|44):::::
line:(85|44):(65|44):::::
line:(65|44):(65|24):::::
line:(65|106):(85|106):::::
line:(85|106):(85|126):::::
line:(85|126):(65|126):::::
line:(65|126):(65|106):::::
line:(70|1):(70|23):::::
line:(70|1):(80|1):::::
line:(80|1):(80|23):::::
line:(70|23):(80|23):::::
line:(80|127):(80|149):::::
line:(70|127):(80|127):::::
line:(70|149):(80|149):::::
line:(70|127):(70|149):::::
line:5:6:3::::
fill:(70|1):(70|23):(80|23):(80|1)::
fill:(70|127):(70|149):(80|149):(80|127)::
fill:(65|24):(85|24):(85|44):(65|44)::
fill:(65|106):(85|106):(85|126):(65|126)::
','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6','','p1x','60','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6','','p1y','45','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6','','p2x','90','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6','','p2y','45','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6','','p3x','90','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6','','p3y','105','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6','','p4x','60','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6','','p4y','105','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6','','p5x','75','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6','','p5y','45','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6','','p6x','75','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6','','p6y','105','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6','','c1','#000000','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6','','s1','1','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6_tst','','name','El_KeySqr_6_tst','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6_tst','','geomW','150','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6_tst','','geomH','150','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6_tst','','lineClr','','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6_tst','','elLst','line:1:2:3::::
line:2:3:3::::
line:3:4:3::::
line:4:1:3::::
line:(65|24):(85|24):::::
line:(85|24):(85|44):::::
line:(85|44):(65|44):::::
line:(65|44):(65|24):::::
line:(65|106):(85|106):::::
line:(85|106):(85|126):::::
line:(85|126):(65|126):::::
line:(65|126):(65|106):::::
line:(70|1):(70|23):::::
line:(70|1):(80|1):::::
line:(80|1):(80|23):::::
line:(70|23):(80|23):::::
line:(80|127):(80|149):::::
line:(70|127):(80|127):::::
line:(70|149):(80|149):::::
line:(70|127):(70|149):::::
line:(7|143):(7|7):2:c1:::s1
line:(7|7):(143|7):2:c1:::s1
line:(143|143):(7|143):2:c1:::s1
line:(143|7):(143|143):2:c1:::s1
line:7:8:3::::
fill:(70|1):(70|23):(80|23):(80|1)::
fill:(70|127):(70|149):(80|149):(80|127)::
fill:(65|24):(85|24):(85|44):(65|44)::
fill:(65|106):(85|106):(85|126):(65|126)::
','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6_tst','','p1x','72','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6_tst','','p1y','55','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6_tst','','p2x','91','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6_tst','','p2y','36','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6_tst','','p3x','135','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6_tst','','p3y','80','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6_tst','','p4x','116','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6_tst','','p4y','99','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6_tst','','p7x','81','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6_tst','','p7y','46','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6_tst','','p8x','125','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6_tst','','p8y','90','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6_tst','','c1','#000000','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6_tst','','s1','1','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6_tst','','dscr','','','ГОСТ 2.755-87','ГОСТ 2.755-87',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6','','dscr','Fuse-switch 2','Fuse-switch 2','ГОСТ 2.755-87','ГОСТ 2.755-87',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3_tst','','name','El_KeyRnd_3_tst','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3_tst','','dscr','','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3_tst','','geomW','150','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3_tst','','geomH','150','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3_tst','','lineClr','','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3_tst','','elLst','line:1:2:::::
line:2:3:::::
line:3:4:::::
line:4:5:::::
line:5:6:::::
line:6:7:::::
line:7:8:::::
line:8:1:::::
arc:(65|34):(65|34):(75|34):(85|34):(75|24):::::
arc:(65|116):(65|116):(75|116):(75|106):(65|116):::::
line:(70|1):(70|23):::::
line:(70|1):(80|1):::::
line:(80|1):(80|23):::::
line:(70|23):(80|23):::::
line:(80|127):(80|149):::::
line:(70|127):(80|127):::::
line:(70|149):(80|149):::::
line:(70|127):(70|149):::::
line:(7|143):(7|7):2:c1:::s1
line:(7|7):(143|7):2:c1:::s1
line:(143|143):(7|143):2:c1:::s1
line:(143|7):(143|143):2:c1:::s1
fill:1:2:3:4:5:6:7:8::
fill:(70|1):(70|23):(80|23):(80|1)::
fill:(70|127):(70|149):(80|149):(80|127)::
fill:(65|34):(65|34)::
fill:(65|116):(65|116)::
','','','',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3_tst','','p1x','76','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3_tst','','p1y','46','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3_tst','','p2x','86','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3_tst','','p2y','36','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3_tst','','p3x','99','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3_tst','','p3y','49','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3_tst','','p4x','108','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3_tst','','p4y','40','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3_tst','','p5x','124','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3_tst','','p5y','56','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3_tst','','p6x','115','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3_tst','','p6y','65','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3_tst','','p7x','129','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3_tst','','p7y','79','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3_tst','','p8x','119','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3_tst','','p8y','89','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3_tst','','c1','#000000','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3_tst','','s1','1','','','',0,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2','','dscr','Fuse-switch','Fuse-switch','Предохранитель-выключатель','Вимикач-запобіжник',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5','','dscr','Switch with the neutral central position','Switch with the neutral central position','Переключатель с нейтральным центральным положением','Перемикач з нейтральним центральним становищем',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2','','dscr','Fuse-switch(circle)','Fuse-switch(circle)','Предохранитель-выключатель(круг)','Вимикач-запобіжник (коло)',32,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_h','','dscr','Switch','Switch','Выключатель','Вимикач',32,'','','','','','','','');
CREATE TABLE 'wlb_ElectroEls_uio' ("IDW" TEXT DEFAULT '' ,"IDC" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"en#NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"IO_TYPE" INTEGER DEFAULT '' ,"IO_VAL" TEXT DEFAULT '' ,"en#IO_VAL" TEXT DEFAULT '' ,"ru#IO_VAL" TEXT DEFAULT '' ,"uk#IO_VAL" TEXT DEFAULT '' ,"SELF_FLG" INTEGER DEFAULT '' ,"CFG_TMPL" TEXT DEFAULT '' ,"en#CFG_TMPL" TEXT DEFAULT '' ,"ru#CFG_TMPL" TEXT DEFAULT '' ,"uk#CFG_TMPL" TEXT DEFAULT '' ,"CFG_VAL" TEXT DEFAULT '' ,"en#CFG_VAL" TEXT DEFAULT '' ,"ru#CFG_VAL" TEXT DEFAULT '' ,"uk#CFG_VAL" TEXT DEFAULT '' , PRIMARY KEY ("IDW","IDC","ID"));
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_KeySqr_1','','st','Error state','Error state','Статус ошибки','Статус помилки',131072,'<EVAL>||','','','',10,'Parameter|st','Parameter|st','','','','','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_KeySqr_1','','val','Value','Value','Значение','Значення',131072,'<EVAL>||','','','',10,'Parameter|var','Parameter|var','','','','','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_KeySqr_1_tst','','val','�������','','','',131072,'<EVAL>||','','','',10,'��������|val','','','','','','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_KeySqr_2','','st','Error state','Error state','Статус ошибки','Статус помилки',131072,'<EVAL>||','','','',10,'Parameter|st','Parameter|st','','','','','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_KeySqr_2','','val','Value','Value','Значение','Значення',131072,'<EVAL>||','','','',10,'Parameter|var','Parameter|var','','','','','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_KeySqr_2_tst','','val','�������','','','',131072,'<EVAL>||','','','',10,'��������|val','','','','','','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_KeySqr_3','','st','Error state','Error state','Статус ошибки','Статус помилки',131072,'<EVAL>||','','','',10,'Parameter|st','Parameter|st','','','','','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_KeySqr_3','','val','Value','Value','Значение','Значення',131072,'<EVAL>||','','','',10,'Parameter|var','Parameter|var','Parameter|var','','','','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_KeySqr_3_tst','','val','�������','','','',131072,'<EVAL>||','','','',10,'��������|val','','','','','','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_KeySqr_4','','st','Error state','Error state','Статус ошибки','Статус помилки',131072,'<EVAL>||','','','',10,'Parameter|st','Parameter|st','','','','','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_KeySqr_4','','val','Value','Value','Значение','Значення',131072,'<EVAL>||','','','',10,'Parameter|var','Parameter|var','','','','','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_KeySqr_4_tst','','val','�������','','','',131072,'<EVAL>||','','','',10,'��������|val','','','','','','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_KeySqr_5','','st','Error state','Error state','Статус ошибки','Статус помилки',131072,'<EVAL>||','','','',10,'Parameter|st','Parameter|st','','','','','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_KeySqr_5','','val','Value','Value','Значение','Значення',131072,'<EVAL>||','','','',10,'Parameter|var','Parameter|var','','','','','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_KeySqr_5_tst','','val','�������','','','',131072,'<EVAL>||','','','',10,'��������|val','','','','','','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_Key_1','','val',' Value',' Value','Значение','Значення',131072,'<EVAL>||','','','',10,'Parameter|val','Parameter|val','','','','','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_Key_1_tst','','val','�������','','','',131072,'<EVAL>||','','','',10,'��������|val','','','','','','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_Key_2','','val',' Value',' Value','Значение','Значення',131072,'<EVAL>||','','','',10,'Parameter|val','Parameter|val','','','','','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_Key_2_tst','','val','�������','','','',131072,'<EVAL>||','','','',10,'��������|val','','','','','','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_Key_3','','val',' Value',' Value','Значение','Значення',131072,'<EVAL>||','','','',10,'Parameter|val','Parameter|val','','','','','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_Key_h','','val',' Value',' Value','Значение','Значення',131072,'<EVAL>||','','','',10,'Parameter|val','Parameter|val','','Parameter|val','','','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_KeySqr_1','','DESCR','Description','Description','Описание','Опис',131077,'<EVAL>||','<EVAL>||','','',10,'Parameter|DESCR','Parameter|DESCR','','','','','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_KeySqr_1','','errPresent','Error presence','Error presence','Наличие ошибки','Наявність помилки',131072,'<EVAL>||','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_KeySqr_2','','DESCR','Description','Description','Описание','Опис',131077,'<EVAL>||','<EVAL>||','','',10,'Parameter|DESCR','Parameter|DESCR','','','','','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_KeySqr_2','','errPresent','Error presence','Error presence','Наличие ошибки','Наявність помилки',131072,'<EVAL>||','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_KeySqr_3','','DESCR','Description','Description','Описание','Опис',131077,'<EVAL>||','<EVAL>||','','',10,'Parameter|DESCR','Parameter|DESCR','','','','','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_KeySqr_3','','errPresent','Error presence','Error presence','Наличие ошибки','Наявність помилки',131072,'<EVAL>||','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_KeySqr_4','','DESCR','Description','Description','Описание','Опис',131077,'<EVAL>||','<EVAL>||','','',10,'Parameter|DESCR','Parameter|DESCR','','','','','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_KeySqr_4','','errPresent','Error presence','Error presence','Наличие ошибки','Наявність помилки',131072,'<EVAL>||','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_KeySqr_5','','val1','Value 1','Value 1','Значение 1','Значення 1',131072,'<EVAL>||','','','',10,'Parameter1|var','Parameter1|var','','','','','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_KeySqr_6','','DESCR','Description','Description','Описание','Опис',131077,'<EVAL>||','<EVAL>||','','',10,'Parameter|DESCR','Parameter|DESCR','','','','','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_KeySqr_6','','errPresent','Error presence','Error presence','Наличие ошибки','Наявність помилки',131072,'<EVAL>||','','','',8,'','','','','','','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_KeySqr_6','','st','Error state','Error state','Статус ошибки','Статус помилки',131072,'<EVAL>||','','','',10,'Parameter|st','Parameter|st','','','','','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_KeySqr_6','','val','Value','Value','Значение','Значення',131072,'<EVAL>||','','','',10,'Parameter|var','Parameter|var','','','','','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_KeySqr_6_tst','','val','�������','','','',131072,'<EVAL>||','','','',10,'��������|val','','','','','','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_Key_3_tst','','val','�������','','','',131072,'<EVAL>||','','','',10,'��������|val','','','','','','','');
COMMIT;
