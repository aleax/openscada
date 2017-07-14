PRAGMA foreign_keys=OFF;
BEGIN TRANSACTION;
CREATE TABLE 'VCALibs' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"DB_TBL" TEXT DEFAULT '' ,"ICO" TEXT DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO "VCALibs" VALUES('ElectroEls','Elements of the electric','Элементы электрики','Library of elements of the electic schemes of the user interface.
Author: Roman Savochenko <rom_as@oscada.org>, Maksim Lisenko (2010)
Version: 1.0.0','Библиотека элементов электрических схем пользовательского интерфейса.
Автор: Роман Савоченко <rom_as@oscada.org>, Максим Лысенко (2010)
Версия: 1.0.0','wlb_ElectroEls','');
CREATE TABLE 'wlb_ElectroEls_io' ("IDW" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"IDC" TEXT DEFAULT '' ,"IO_VAL" TEXT DEFAULT '' ,"ru#IO_VAL" TEXT DEFAULT '' ,"SELF_FLG" INTEGER DEFAULT '0' ,"CFG_TMPL" TEXT DEFAULT '' ,"ru#CFG_TMPL" TEXT DEFAULT '' ,"CFG_VAL" TEXT DEFAULT '' ,"ru#CFG_VAL" TEXT DEFAULT '' , PRIMARY KEY ("IDW","ID","IDC"));
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1','name','','Single-side switch','Рубильник однополосный',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1','dscr','','ГОСТ 2.755-87','ГОСТ 2.755-87',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1','geomW','','150','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1','geomH','','150','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1','lineClr','','#000000','',40,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1','fillColor','','','',40,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1','elLst','','line:(7|7):(143|7):2:c1:::s1
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
','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1','p1x','','70','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1','p1y','','45','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1','p2x','','80','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1','p2y','','45','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1','p3x','','80','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1','p3y','','105','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1','p4x','','70','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1','p4y','','105','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1','c1','','#000000','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1','s1','','1','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1_tst','name','','El_KeySqr_1_tst','El_KeySqr_1_tst',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1_tst','dscr','','ГОСТ 2.755-87','ГОСТ 2.755-87',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1_tst','geomW','','150','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1_tst','geomH','','150','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1_tst','lineClr','','','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1_tst','elLst','','line:1:2:::::
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
','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1_tst','p1x','','76','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1_tst','p1y','','46','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1_tst','p2x','','86','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1_tst','p2y','','36','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1_tst','p3x','','126','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1_tst','p3y','','76','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1_tst','p4x','','116','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1_tst','p4y','','86','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1_tst','c1','','#000000','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_1_tst','s1','','1','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2','name','','Safety switch','Выключатель-предохранитель',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2','dscr','','Safety switch','Предохранитель-выключатель',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2','geomW','','150','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2','geomH','','150','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2','lineClr','','#000000','',40,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2','fillColor','','','',40,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2','elLst','','line:(7|143):(7|7):2:c1:::s1
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
','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2','p1x','','60','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2','p1y','','45','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2','p2x','','90','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2','p2y','','45','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2','p3x','','90','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2','p3y','','105','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2','p4x','','60','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2','p4y','','105','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2','c1','','#000000','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2','s1','','1','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2_tst','name','','El_KeySqr_2_tst','El_KeySqr_2_tst',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2_tst','geomW','','150','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2_tst','geomH','','150','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2_tst','lineClr','','','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2_tst','elLst','','line:1:2:::::
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
fill:(65|106):(85|106):(85|126):(65|126)::','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2_tst','p1x','','72','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2_tst','p1y','','55','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2_tst','p2x','','91','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2_tst','p2y','','36','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2_tst','p3x','','135','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2_tst','p3y','','80','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2_tst','p4x','','116','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2_tst','p4y','','99','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2_tst','c1','','#000000','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_2_tst','s1','','1','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3','name','','Circuit-breaker automatic','Рубильник однополосный автоматический',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3','dscr','','ГОСТ 2.755-87','ГОСТ 2.755-87',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3','geomW','','150','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3','geomH','','150','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3','lineClr','','','',40,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3','fillColor','','','',40,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3','elLst','','line:(7|143):(7|7):2:c1:::s1
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
','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3','p1x','','70','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3','p1y','','45','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3','p2x','','80','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3','p2y','','45','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3','p3x','','80','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3','p3y','','65','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3','p4x','','90','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3','p4y','','65','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3','p5x','','90','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3','p5y','','85','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3','p6x','','80','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3','p6y','','85','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3','p7x','','80','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3','p7y','','105','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3','p8x','','70','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3','p8y','','105','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3','c1','','#000000','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3','s1','','1','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3_tst','name','','El_KeySqr_3_tst','El_KeySqr_3_tst',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3_tst','dscr','','ГОСТ 2.755-87','ГОСТ 2.755-87',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3_tst','geomW','','150','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3_tst','geomH','','150','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3_tst','lineClr','','','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3_tst','elLst','','line:1:2:::::
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
','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3_tst','p1x','','76','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3_tst','p1y','','46','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3_tst','p2x','','86','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3_tst','p2y','','36','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3_tst','p3x','','99','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3_tst','p3y','','49','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3_tst','p4x','','108','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3_tst','p4y','','40','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3_tst','p5x','','124','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3_tst','p5y','','56','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3_tst','p6x','','115','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3_tst','p6y','','65','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3_tst','p7x','','129','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3_tst','p7y','','79','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3_tst','p8x','','119','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3_tst','p8y','','89','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3_tst','c1','','#000000','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_3_tst','s1','','1','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','name','','Two-way automatic switch','Рубильник двухполосный автоматический',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','dscr','','ГОСТ 2.755-87','ГОСТ 2.755-87',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','geomW','','300','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','geomH','','150','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','lineClr','','','',40,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','fillColor','','','',40,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','elLst','','line:(7|143):(7|7):2:c1:::s1
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
','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','p1x','','220','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','p1y','','45','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','p2x','','230','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','p2y','','45','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','p3x','','230','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','p3y','','65','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','p4x','','240','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','p4y','','65','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','p5x','','240','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','p5y','','85','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','p6x','','230','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','p6y','','85','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','p7x','','230','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','p7y','','105','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','p8x','','220','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','p8y','','105','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','p9x','','70','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','p9y','','45','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','p10x','','80','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','p10y','','45','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','p11x','','80','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','p11y','','65','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','p14x','','80','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','p14y','','85','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','p15x','','80','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','p15y','','105','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','p16x','','70','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','p16y','','105','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','p17x','','220','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','p17y','','65','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','p18x','','220','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','p18y','','85','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','c1','','#000000','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4','s1','','1','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','name','','El_KeySqr_4_tst','El_KeySqr_4_tst',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','dscr','','ГОСТ 2.755-87','ГОСТ 2.755-87',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','geomW','','300','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','geomH','','150','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','lineClr','','','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','elLst','','line:(7|7):(293|7):2:c1:::s1
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
','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','p1x','','226','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','p1y','','46','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','p2x','','236','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','p2y','','36','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','p3x','','249','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','p3y','','49','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','p4x','','259','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','p4y','','39','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','p5x','','276','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','p5y','','56','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','p6x','','266','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','p6y','','66','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','p7x','','279','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','p7y','','79','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','p8x','','269','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','p8y','','89','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','p9x','','76','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','p9y','','46','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','p10x','','86','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','p10y','','36','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','p11x','','108','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','p11y','','58','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','p14x','','120','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','p14y','','70','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','p15x','','129','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','p15y','','79','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','p16x','','119','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','p16y','','89','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','p17x','','238','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','p17y','','58','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','p18x','','250','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','p18y','','70','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_4_tst','s1','','1','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5','name','','Переключатель с нейтральным центральным положением','Переключатель с нейтральным центральным положением',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5','dscr','','Переключатель с нейтральным центральным положением','Переключатель с нейтральным центральным положением',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5','geomW','','150','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5','geomH','','150','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5','lineClr','','','',40,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5','fillColor','','','',40,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5','elLst','','line:(7|143):(7|7):2:c1:::s1
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
','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5','p1x','','70','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5','p1y','','45','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5','p2x','','80','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5','p2y','','45','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5','p3x','','80','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5','p3y','','85','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5','p4x','','70','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5','p4y','','85','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5','c1','','','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5','s1','','1','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5_tst','name','','El_KeySqr_5_tst','El_KeySqr_5_tst',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5_tst','geomW','','150','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5_tst','geomH','','150','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5_tst','lineClr','','','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5_tst','elLst','','line:1:2:::::
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
','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5_tst','p1x','','64','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5_tst','p1y','','36','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5_tst','p2x','','74','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5_tst','p2y','','46','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5_tst','p3x','','34','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5_tst','p3y','','86','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5_tst','p4x','','24','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5_tst','p4y','','76','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5_tst','c1','','#000000','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_5_tst','s1','','1','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6','name','','Предохранитель-выключатель 2','Предохранитель-выключатель 2',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6','dscr','','ГОСТ 2.755-87','ГОСТ 2.755-87',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6','geomW','','150','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6','geomH','','150','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6','lineClr','','#000000','',40,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6','fillColor','','','',40,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6','elLst','','line:(7|143):(7|7):2:c1:::s1
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
','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6','p1x','','60','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6','p1y','','45','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6','p2x','','90','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6','p2y','','45','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6','p3x','','90','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6','p3y','','105','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6','p4x','','60','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6','p4y','','105','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6','p5x','','75','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6','p5y','','45','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6','p6x','','75','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6','p6y','','105','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6','c1','','#000000','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6','s1','','1','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6_tst','name','','El_KeySqr_6_tst','El_KeySqr_6_tst',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6_tst','dscr','','ГОСТ 2.755-87','ГОСТ 2.755-87',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6_tst','geomW','','150','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6_tst','geomH','','150','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6_tst','lineClr','','','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6_tst','elLst','','line:1:2:3::::
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
','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6_tst','p1x','','72','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6_tst','p1y','','55','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6_tst','p2x','','91','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6_tst','p2y','','36','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6_tst','p3x','','135','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6_tst','p3y','','80','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6_tst','p4x','','116','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6_tst','p4y','','99','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6_tst','p7x','','81','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6_tst','p7y','','46','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6_tst','p8x','','125','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6_tst','p8y','','90','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6_tst','c1','','#000000','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_KeySqr_6_tst','s1','','1','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1','name','','Рубильник однополосный(круг)','Рубильник однополосный(круг)',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1','dscr','','ГОСТ 2.755-87','ГОСТ 2.755-87',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1','geomW','','150','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1','geomH','','150','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1','lineClr','','','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1','elLst','','line:1:2:::::
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
','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1','p1x','','70','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1','p1y','','45','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1','p2x','','80','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1','p2y','','45','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1','p3x','','80','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1','p3y','','105','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1','p4x','','70','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1','p4y','','105','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1','c1','','#000000','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1','s1','','1','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1_tst','name','','El_KeyRnd_1_tst','El_KeyRnd_1_tst',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1_tst','dscr','','ГОСТ 2.755-87','ГОСТ 2.755-87',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1_tst','geomW','','150','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1_tst','geomH','','150','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1_tst','lineClr','','','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1_tst','elLst','','line:1:2:::::
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
','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1_tst','p1x','','76','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1_tst','p1y','','46','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1_tst','p2x','','86','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1_tst','p2y','','36','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1_tst','p3x','','126','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1_tst','p3y','','76','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1_tst','p4x','','116','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1_tst','p4y','','86','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1_tst','c1','','#000000','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_1_tst','s1','','1','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2','name','','Выключатель-предохранитель(круг)','Выключатель-предохранитель(круг)',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2','dscr','','Предохранитель-выключатель(круг)','Предохранитель-выключатель(круг)',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2','geomW','','150','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2','geomH','','150','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2','lineClr','','','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2','elLst','','line:1:2:::::
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
','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2','p1x','','60','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2','p1y','','45','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2','p2x','','90','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2','p2y','','45','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2','p3x','','90','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2','p3y','','105','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2','p4x','','60','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2','p4y','','105','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2','c1','','#000000','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2','s1','','1','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2_tst','name','','El_KeyRnd_2_tst','El_KeyRnd_2_tst',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2_tst','geomW','','150','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2_tst','geomH','','150','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2_tst','lineClr','','','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2_tst','elLst','','line:1:2:::::
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
','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2_tst','p1x','','72','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2_tst','p1y','','55','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2_tst','p2x','','91','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2_tst','p2y','','36','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2_tst','p3x','','135','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2_tst','p3y','','80','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2_tst','p4x','','116','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2_tst','p4y','','99','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2_tst','c1','','#000000','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_2_tst','s1','','1','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3','name','','Рубильник однополосный автоматический(круг)','Рубильник однополосный автоматический(круг)',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3','dscr','','ГОСТ 2.755-87','ГОСТ 2.755-87',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3','geomW','','150','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3','geomH','','150','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3','lineClr','','','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3','elLst','','line:1:2:::::
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
','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3','p1x','','70','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3','p1y','','45','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3','p2x','','80','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3','p2y','','45','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3','p3x','','80','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3','p3y','','65','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3','p4x','','90','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3','p4y','','65','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3','p5x','','90','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3','p5y','','85','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3','p6x','','80','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3','p6y','','85','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3','p7x','','80','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3','p7y','','105','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3','p8x','','70','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3','p8y','','105','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3','c1','','#000000','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3','s1','','1','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3_tst','name','','El_KeyRnd_3_tst','El_KeyRnd_3_tst',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3_tst','dscr','','','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3_tst','geomW','','150','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3_tst','geomH','','150','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3_tst','lineClr','','','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3_tst','elLst','','line:1:2:::::
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
','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3_tst','p1x','','76','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3_tst','p1y','','46','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3_tst','p2x','','86','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3_tst','p2y','','36','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3_tst','p3x','','99','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3_tst','p3y','','49','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3_tst','p4x','','108','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3_tst','p4y','','40','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3_tst','p5x','','124','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3_tst','p5y','','56','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3_tst','p6x','','115','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3_tst','p6y','','65','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3_tst','p7x','','129','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3_tst','p7y','','79','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3_tst','p8x','','119','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3_tst','p8y','','89','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3_tst','c1','','#000000','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_3_tst','s1','','1','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_h','name','','Выключатель','Выключатель',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_h','dscr','','Выключатель','Выключатель',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_h','geomW','','150','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_h','geomH','','100','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_h','lineWdth','','0','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_h','lineClr','','','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_h','elLst','','line:1:2:::::
line:2:3:::::
line:3:4:::::
line:4:1:::::
arc:(20|35):(20|35):(20|50):(35|50):(20|35):::::
arc:(130|35):(130|35):(130|50):(145|50):(130|35):::::
fill:1:2:3:4::
','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_h','p1x','','40','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_h','p1y','','35','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_h','p2x','','40','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_h','p2y','','65','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_h','p3x','','110','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_h','p3y','','65','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_h','p4x','','110','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Key_h','p4y','','35','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Krug','name','','Окружность','Окружность',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Krug','dscr','','Окружность
Автор: Максим Лысенко
Версия: 1.0.0','Окружность
Автор: Максим Лысенко
Версия: 1.0.0',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Krug','geomW','','100','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Krug','geomH','','100','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Krug','lineWdth','','4','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Krug','lineClr','','','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Krug','elLst','','arc:(5|50):(5|50):(50|50):(50|5):(5|50):::::','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Zemlia','owner','','root:UI','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Zemlia','name','','Заземление','Заземление',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Zemlia','dscr','','Заземление, по ГОСТ 2.721-74
Автор: Максим Лысенко
Версия: 1.0.0','Заземление, по ГОСТ 2.721-74
Автор: Максим Лысенко
Версия: 1.0.0',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Zemlia','geomW','','100','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Zemlia','geomH','','100','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Zemlia','lineClr','','black','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Zemlia','fillColor','','black','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_Zemlia','elLst','','line:(31|15):(39.214|21.428):::::
line:(39.214|21.428):(39.214|78.571):::::
line:(39.214|78.571):(31|85):::::
line:(31|85):(31|15):::::
line:(51|30):(59.214|36.429):::::
line:(59.214|36.429):(59.214|63.571):::::
line:(59.214|63.571):(51|70):::::
line:(51|70):(51|30):::::
line:(71|45):(71|55):::::
line:(71|55):(79.214|50):::::
line:(71|45):(79.214|50):::::
line:(72|46):(72|54):::::
line:(73|47):(73|53):::::
line:(74|48):(74|52):::::
line:(75|48):(75|52):::::
line:(76|49):(76|51):::::
line:(77|49):(77|51):::::
line:(79.214|50):(71|50):::::
fill:(31|15):(39.214|21.428):(39.214|78.571):(31|85)::
fill:(51|30):(59.214|36.429):(59.214|63.571):(51|70)::
','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_lhor','name','','El_lhor','El_lhor',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_lhor','dscr','','Токопроводящая связь, линия или шина, горизонтальная, по ГОСТ 2.723-68
Автор: Максим Лысенко
Версия: 1.0.0','Токопроводящая связь, линия или шина, горизонтальная, по ГОСТ 2.723-68
Автор: Максим Лысенко
Версия: 1.0.0',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_lhor','geomH','','100','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_lhor','lineWdth','','1','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_lhor','lineClr','','black','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_lhor','elLst','','line:(0|45):(100|45):::::
line:(100|45):(100|55):::::
line:(100|55):(0|55):::::
line:(0|55):(0|45):::::
fill:(0|45):(100|45):(100|55):(0|55)::','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_lvert','name','','El_lvert','El_lvert',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_lvert','dscr','','Токопроводящая связь, линия или шина, вертикальная, по ГОСТ 2.723-68
Автор: Максим Лысенко
Версия: 1.0.0','Токопроводящая связь, линия или шина, вертикальная, по ГОСТ 2.723-68
Автор: Максим Лысенко
Версия: 1.0.0',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_lvert','geomW','','100','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_lvert','lineWdth','','1','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_lvert','lineClr','','black','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('El_lvert','elLst','','line:(45|0):(55|0):::::
line:(55|0):(55|100):::::
line:(55|100):(45|100):::::
line:(45|100):(45|0):::::
fill:(45|0):(55|0):(55|100):(45|100)::','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('Induct','owner','','root:UI','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('Induct','name','','Индуктивность','Индуктивность',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('Induct','dscr','','Индуктивность, по ГОСТ 2.723-68
Автор: Максим Лысенко
Версия: 1.0.0','Индуктивность, по ГОСТ 2.723-68
Автор: Максим Лысенко
Версия: 1.0.0',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('Induct','geomW','','100','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('Induct','geomH','','100','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('Induct','geomXsc','','1','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('Induct','geomYsc','','1','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('Induct','lineWdth','','5','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('Induct','elLst','','arc:(50|35):(50|5):(50|20):(50|5):(35|20)
arc:(50|95):(50|65):(50|80):(50|65):(35|80)
arc:(50|65):(50|35):(50|50):(50|35):(35|50)
','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('Sharp','name','','Три полосы','Три полосы',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('Sharp','dscr','','Три полосы, обозначение трёхфазной линии электропередачи
Автор: Максим Лысенко
Версия: 1.0.0','Три полосы, обозначение трёхфазной линии электропередачи
Автор: Максим Лысенко
Версия: 1.0.0',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('Sharp','geomX','','827','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('Sharp','geomY','','412','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('Sharp','geomW','','100','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('Sharp','geomH','','100','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('Sharp','geomZ','','7','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('Sharp','lineWdth','','4','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('Sharp','elLst','','line:(28|0):(100|72):::::
line:(14|14):(86|86):::::
line:(0|29):(72|100):::::
','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('arrowSharp','name','','Двойная стрелка','Двойная стрелка',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('arrowSharp','dscr','','Двойная стрелка, обозначение выездного оборудования
Автор: Роман Савоченко <rom_as@oscada.org>
Версия: 1.0.0','Двойная стрелка, обозначение выездного оборудования
Автор: Роман Савоченко <rom_as@oscada.org>
Версия: 1.0.0',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('arrowSharp','geomX','','827','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('arrowSharp','geomY','','412','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('arrowSharp','geomW','','100','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('arrowSharp','geomH','','100','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('arrowSharp','geomZ','','7','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('arrowSharp','lineWdth','','4','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('arrowSharp','elLst','','line:(50|5):(95|50):::::
line:(50|5):(5|50):::::
line:(50|20):(95|65):::::
line:(50|20):(5|65):::::
','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('battery','name','','Батарея','Батарея',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('battery','dscr','','Батарея
Автор: Максим Лысенко
Версия: 1.0.0','Батарея
Автор: Максим Лысенко
Версия: 1.0.0',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('battery','geomW','','100','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('battery','geomH','','100','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('battery','lineWdth','','1','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('battery','elLst','','line:(15|40):(15|60):::::
line:(13|45):(13|55):::::
line:(15|50):(23|50):::::
line:(25|40):(25|60):::::
line:(23|45):(23|55):::::
line:(25|50):(38|50):::::2
line:(40|40):(40|60):::::
line:(38|45):(38|55):::::
line:(40|50):(58|50):::::
line:(60|40):(60|60):::::
line:(58|45):(58|55):::::
line:(60|50):(73|50):::::2
line:(75|40):(75|60):::::
line:(73|45):(73|55):::::
line:(75|50):(83|50):::::
line:(85|40):(85|60):::::
line:(83|45):(83|55):::::
','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('conBus','owner','','root:UI','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('conBus','name','','Распределительная шина','Распределительная шина',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('conBus','dscr','','Распределительная шина
Автор: Роман Савоченко <rom_as@oscada.org>
Версия: 1.0.0','Распределительная шина
Автор: Роман Савоченко <rom_as@oscada.org>
Версия: 1.0.0',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('conBus','lineWdth','','2','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('conBus','lineClr','','','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('conBus','elLst','','line:(5|45):(95|45):::::
line:(95|45):(95|55):::::
line:(95|55):(5|55):::::
line:(5|55):(5|45):::::
','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('converterAC_DC','name','','Преобр.: переменный в постоянный','Преобр.: переменный в постоянный',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('converterAC_DC','dscr','','Преобразователь переменного тока в постоянный, по ГОСТ 2.723-68
Автор: Максим Лысенко
Версия: 1.0.0','Преобразователь переменного тока в постоянный, по ГОСТ 2.723-68
Автор: Максим Лысенко
Версия: 1.0.0',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('converterAC_DC','geomW','','100','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('converterAC_DC','geomH','','100','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('converterAC_DC','lineWdth','','3','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('converterAC_DC','elLst','','line:(5|5):(95|5):::::
line:(95|5):(95|95):::::
line:(95|95):(5|95):::::
line:(5|95):(5|5):::::
line:(5|95):(95|5):::::
line:(50|68):(81|68):::::
line:(50|81):(81|81):::::
bezier:(14|30):(50|30):(30|10):(30|50):::::
','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('converterDC_DC','name','','Преобр.: постоянный в постоянный','Преобр.: постоянный в постоянный',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('converterDC_DC','dscr','','Преобразователь постоянного тока в постоянный ток другого уровня напряжения, по ГОСТ 2.723-68
Автор: Максим Лысенко
Версия: 1.0.0','Преобразователь постоянного тока в постоянный ток другого уровня напряжения, по ГОСТ 2.723-68
Автор: Максим Лысенко
Версия: 1.0.0',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('converterDC_DC','lineWdth','','3','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('converterDC_DC','elLst','','line:(5|5):(95|5):::::
line:(95|5):(95|95):::::
line:(95|95):(5|95):::::
line:(5|95):(5|5):::::
line:(5|95):(95|5):::::
line:(50|68):(81|68):::::
line:(50|81):(81|81):::::
line:(15|18):(46|18):::::
line:(15|31):(46|31):::::
','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('fuse1','name','','Предохранитель','Предохранитель',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('fuse1','dscr','','Предохранитель
Автор: Роман Савоченко <rom_as@oscada.org>
Версия: 1.0.0','Предохранитель
Автор: Роман Савоченко <rom_as@oscada.org>
Версия: 1.0.0',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('fuse1','lineWdth','','2','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('fuse1','lineClr','','','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('fuse1','elLst','','line:(15|35):(15|65):::::
line:(15|65):(85|65):::::
line:(85|65):(85|35):::::
line:(85|35):(15|35):::::
line:(15|50):(84|50):::::
','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('key','name','','Ключ','Ключ',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('key','dscr','','Простой ключ.
Автор: Роман Савоченко <rom_as@oscada.org>
Версия: 1.0.0
Свойства:
- var=true - замыкание;
- var=false - размыкание;
- var=EVAL - ключ пунктирный;
- рамка конфигурируется.','Простой ключ.
Автор: Роман Савоченко <rom_as@oscada.org>
Версия: 1.0.0
Свойства:
- var=true - замыкание;
- var=false - размыкание;
- var=EVAL - ключ пунктирный;
- рамка конфигурируется.',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('key','lineClr','','','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('key','elLst','','line:(42|8):(58|8):::::
line:(58|8):(58|23):::::
line:(58|23):(42|23):::::
line:(42|23):(42|8):::::
line:(42|77):(58|77):::::
line:(58|77):(58|92):::::
line:(58|92):(42|92):::::
line:(42|92):(42|77):::::
line:(50|15):1:8::::s1
line:(5|5):(95|5):2:c1:::s2
line:(95|95):(5|95):2:c1:::s2
line:(95|5):(95|95):2:c1:::s2
line:(5|95):(5|5):2:c1:::s2
fill:(42|8):(58|8):(58|23):(42|23)::
fill:(42|77):(58|77):(58|92):(42|92)::
','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('key','p1x','','90','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('key','p1y','','65','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('key','c1','','#808080','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('key','s1','','0','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('key','s2','','2','',0,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('limU','name','','Огранич. напряжения','Огранич. напряжения',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('limU','dscr','','Ограничитель напряжения
Автор: Роман Савоченко <rom_as@oscada.org>
Версия: 1.0.0','Ограничитель напряжения
Автор: Роман Савоченко <rom_as@oscada.org>
Версия: 1.0.0',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('limU','lineWdth','','2','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('limU','lineClr','','','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('limU','elLst','','line:(15|35):(15|65):::::
line:(15|65):(85|65):::::
line:(85|65):(85|35):::::
line:(85|35):(15|35):::::
line:(60|23):(84|23):::::
line:(17|86):(60|23):::::
','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('load1','name','','Нагрузка 1','Нагрузка 1',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('load1','dscr','','Нагрузка 1
Автор: Роман Савоченко <rom_as@oscada.org>
Версия: 1.0.0','Нагрузка 1
Автор: Роман Савоченко <rom_as@oscada.org>
Версия: 1.0.0',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('load1','geomX','','827','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('load1','geomY','','412','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('load1','geomZ','','7','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('load1','lineWdth','','3','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('load1','elLst','','line:(50|5):(95|83):::::
line:(50|5):(5|83):::::
line:(5|83):(95|83):::::
line:(79|38):(65|47):::::
line:(82|43):(68|52):::::
line:(37|46):(21|36):::::
line:(34|51):(18|41):::::
line:(48|90):(48|75):::::
line:(54|90):(54|75):::::
','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('load2','name','','Нагрузка 2','Нагрузка 2',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('load2','dscr','','Нагрузка 2
Автор: Роман Савоченко <rom_as@oscada.org>
Версия: 1.0.0','Нагрузка 2
Автор: Роман Савоченко <rom_as@oscada.org>
Версия: 1.0.0',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('load2','lineWdth','','2','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('load2','lineClr','','black','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('load2','elLst','','arc:(5|70):(5|70):(30|70):(30|45):(5|70):::::
arc:(5|30):(5|30):(30|30):(30|5):(5|30):::::
line:(70|29.063):(52.5|60.938):::::
line:(52.5|60.938):(88.125|60.625):::::
line:(70|29.063):(88.125|60.625):::::
line:(30|70.312):(12.5|84.375):::::
line:(30|47.5):(30|70.312):::::
line:(46.875|84.375):(30|70.312):::::
arc:(45|50):(45|50):(70|50):(70|25):(45|50):::::
line:(51|70):(30|70.312):::::
line:(30|7.5):(30|30.312):::::
line:(30|30.312):(12.5|44.375):::::
line:(46.875|44.375):(30|30.312):::::
line:(51|30):(30|30.312):::::
','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('reactor','name','','Реактор','Реактор',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('reactor','dscr','','Реактор, по ГОСТ 2.723-68
Автор: Максим Лысенко
Версия: 1.0.0','Реактор, по ГОСТ 2.723-68
Автор: Максим Лысенко
Версия: 1.0.0',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('reactor','lineWdth','','3','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('reactor','lineClr','','black','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('reactor','elLst','','arc:(50|80):(80|50):(50|50):(20|50):(50|80):::::
line:(50|80):(50|50):::::
line:(50|50):(5|50):::::
line:(80|50):(95|50):::::','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('srcAC','owner','','root:UI','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('srcAC','name','','Источник: переменный ток','Источник: переменный ток',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('srcAC','dscr','','Источник переменного тока, по ГОСТ 2.723-68
Автор: Максим Лысенко
Версия: 1.0.0','Источник переменного тока, по ГОСТ 2.723-68
Автор: Максим Лысенко
Версия: 1.0.0',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('srcAC','geomW','','100','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('srcAC','geomH','','100','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('srcAC','lineWdth','','3','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('srcAC','lineClr','','','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('srcAC','elLst','','arc:(5|50):(5|50):(50|50):(50|5):(5|50):::::
arc:(10|50):(50|50):(30|50):(30|30):(10|50):::::
arc:(90|50):(50|50):(70|50):(70|70):(90|50):::::
','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('stGen','owner','','root:UI','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('stGen','name','','Состояние общее','Состояние общее',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('stGen','dscr','','Общее состояние, обычно для контакторов
Автор: Роман Савоченко <rom_as@oscada.org>
Версия: 1.0.0
Свойства:
- var=true - заполнение красным
- var=false - заполнение зелёным
- var=EVAL - заполнение прозрачным','Общее состояние, обычно для контакторов
Автор: Роман Савоченко <rom_as@oscada.org>
Версия: 1.0.0
Свойства:
- var=true - заполнение красным
- var=false - заполнение зелёным
- var=EVAL - заполнение прозрачным',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('stGen','lineWdth','','2','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('stGen','lineClr','','','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('stGen','elLst','','line:(10|10):(90|10):::::
line:(90|10):(90|90):::::
line:(90|90):(10|90):::::
line:(10|90):(10|10):::::
fill:(10|90):(10|10):(90|10):(90|90):c1:
','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('stGen','c1','','#000000-127','',8,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('stGen1','owner','','root:UI','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('stGen1','name','','Состояние общее 1','Состояние общее 1',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('stGen1','dscr','','Общее состояние группы, для включенных в бокс элементов
Автор: Роман Савоченко <rom_as@oscada.org>
Версия: 1.0.0
Свойства:
- var=true - цвет бордюра красный
- var=false - цвет бордюра зелёный
- var=EVAL - цвет бордюра чёрный','Общее состояние группы, для включенных в бокс элементов
Автор: Роман Савоченко <rom_as@oscada.org>
Версия: 1.0.0
Свойства:
- var=true - цвет бордюра красный
- var=false - цвет бордюра зелёный
- var=EVAL - цвет бордюра чёрный',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('stGen1','backColor','','','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('stGen1','bordWidth','','1','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('stGen1','bordColor','','#000000','',40,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('stGen1','bordStyle','','2','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('trDuo','name','','Тр.: одна вторичная обмотка','Тр.: одна вторичная обмотка',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('trDuo','dscr','','Трансформатор с одной вторичной обмоткой, по ГОСТ 2.723-68
Автор: Максим Лысенко
Версия: 1.0.0','Трансформатор с одной вторичной обмоткой, по ГОСТ 2.723-68
Автор: Максим Лысенко
Версия: 1.0.0',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('trDuo','lineWdth','','2','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('trDuo','lineClr','','black','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('trDuo','elLst','','arc:(25|70):(25|70):(50|70):(50|45):(25|70):::::
arc:(25|30):(25|30):(50|30):(50|5):(25|30):::::
','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('trStarTriangl','owner','','root:UI','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('trStarTriangl','name','','Тр.: звезда в треуг.','Тр.: звезда в треуг.',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('trStarTriangl','dscr','','Трансформатор трёхфазный, звезда в треугольник
Автор: Роман Савоченко <rom_as@oscada.org>
Версия: 1.0.0','Трансформатор трёхфазный, звезда в треугольник
Автор: Роман Савоченко <rom_as@oscada.org>
Версия: 1.0.0',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('trStarTriangl','lineWdth','','2','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('trStarTriangl','lineClr','','black','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('trStarTriangl','elLst','','arc:(25|70):(25|70):(50|70):(50|45):(25|70):::::
arc:(25|30):(25|30):(50|30):(50|5):(25|30):::::
line:(50|49.063):(32.5|80.938):::::
line:(32.5|81):(68.125|81):::::
line:(50|49.063):(68.125|80.625):::::
line:(50|30.312):(32.5|44.375):::::
line:(50|7.5):(50|30.312):::::
line:(66.875|44.375):(50|30.312):::::
line:(18.125|43.438):(84.375|9.688):::::
line:(80.313|15.625):(84.375|9.688):::::
line:(77.188|10.313):(84.375|9.688):::::
','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('trTrianglStar','owner','','root:UI','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('trTrianglStar','name','','Тр.: треуг. в звезда','Тр.: треуг. в звезда',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('trTrianglStar','dscr','','Трансформатор трёхфазный, треугольник в звезду
Автор: Роман Савоченко <rom_as@oscada.org>
Версия: 1.0.0','Трансформатор трёхфазный, треугольник в звезду
Автор: Роман Савоченко <rom_as@oscada.org>
Версия: 1.0.0',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('trTrianglStar','lineWdth','','2','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('trTrianglStar','lineClr','','black','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('trTrianglStar','elLst','','arc:(25|70):(25|70):(50|70):(50|45):(25|70):::::
arc:(25|30):(25|30):(50|30):(50|5):(25|30):::::
line:(50|9.063):(32.5|40.938):::::
line:(32.5|41):(68.125|41):::::
line:(50|9.063):(68.125|40.625):::::
line:(50|70.312):(32.5|84.375):::::
line:(50|47.5):(50|70.312):::::
line:(66.875|84.375):(50|70.312):::::
','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('trTrianglStarH','name','','Тр.: треуг. в звезда, гор.','Тр.: треуг. в звезда, гор.',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('trTrianglStarH','dscr','','Трансформатор трёхфазный, треугольник в звезду, горизонтальный
Автор: Роман Савоченко <rom_as@oscada.org>
Версия: 1.0.0','Трансформатор трёхфазный, треугольник в звезду, горизонтальный
Автор: Роман Савоченко <rom_as@oscada.org>
Версия: 1.0.0',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('trTrianglStarH','lineWdth','','2','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('trTrianglStarH','lineClr','','black','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('trTrianglStarH','elLst','','arc:(45|50):(45|50):(70|50):(70|25):(45|50):::::
arc:(5|50):(5|50):(30|50):(30|25):(5|50):::::
line:(30|29.063):(12.5|60.938):::::
line:(12.5|61):(48.125|61):::::
line:(30|29.063):(48.125|60.625):::::
line:(70|50.312):(52.5|64.375):::::
line:(70|27.5):(70|50.312):::::
line:(86.875|64.375):(70|50.312):::::
','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('trTrio','name','','Тр.: две вторичные обмотками','Тр.: две вторичные обмотками',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('trTrio','dscr','','Трансформатор с двумя вторичными обмотками, по ГОСТ 2.723-68
Автор: Максим Лысенко
Версия: 1.0.0','Трансформатор с двумя вторичными обмотками, по ГОСТ 2.723-68
Автор: Максим Лысенко
Версия: 1.0.0',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('trTrio','lineWdth','','2','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('trTrio','lineClr','','black','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('trTrio','elLst','','arc:(5|65):(5|65):(30|65):(30|40):(5|65):::::
arc:(25|30):(25|30):(50|30):(50|5):(25|30):::::
arc:(45|65):(45|65):(70|65):(70|40):(45|65):::::
','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('triangle','name','','Треугольник','Треугольник',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('triangle','dscr','','Равносторонний треугольник
Автор: Роман Савоченко <rom_as@oscada.org>
Версия: 1.0.0','Равносторонний треугольник
Автор: Роман Савоченко <rom_as@oscada.org>
Версия: 1.0.0',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('triangle','geomX','','827','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('triangle','geomY','','412','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('triangle','geomZ','','7','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('triangle','lineWdth','','4','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('triangle','fillColor','','white','',32,'','','','');
INSERT INTO "wlb_ElectroEls_io" VALUES('triangle','elLst','','line:(50|10):(95|88):::::
line:(50|10):(5|88):::::
line:(5|88):(95|88):::::
fill:(5|88):(95|88):(50|10)::
','',32,'','','','');
CREATE TABLE 'wlb_ElectroEls' ("ID" TEXT DEFAULT '' ,"ICO" TEXT DEFAULT '' ,"PARENT" TEXT DEFAULT '' ,"PR_TR" INTEGER DEFAULT '0' ,"PROC" TEXT DEFAULT '' ,"ru#PROC" TEXT DEFAULT '' ,"PROC_PER" INTEGER DEFAULT '-1' ,"ATTRS" TEXT DEFAULT '*' ,"TIMESTAMP" INTEGER DEFAULT '0' , PRIMARY KEY ("ID"));
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
6XQA54fEYXe+Q16O0G63m06n4CmMsd+wBUHpMBdF9wAAAABJRU5ErkJggg==','/wlb_originals/wdg_ElFigure',0,'JavaLikeCalc.JavaScript
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
','',-1,'name;dscr;geomW;geomH;lineClr;fillColor;elLst;p1x;p1y;p2x;p2y;p3x;p3y;p4x;p4y;c1;s1;',1500037082);
INSERT INTO "wlb_ElectroEls" VALUES('El_KeySqr_1_tst','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
TkSuQmCC','/wlb_originals/wdg_ElFigure',0,'JavaLikeCalc.JavaScript
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
}','',-1,'name;dscr;geomW;geomH;lineClr;elLst;p1x;p1y;p2x;p2y;p3x;p3y;p4x;p4y;c1;s1;',1500037083);
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
/A24me+pnGj7iwAAAABJRU5ErkJggg==','/wlb_originals/wdg_ElFigure',0,'JavaLikeCalc.JavaScript
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
','',-1,'name;dscr;geomW;geomH;lineClr;fillColor;elLst;p1x;p1y;p2x;p2y;p3x;p3y;p4x;p4y;c1;s1;',1500037083);
INSERT INTO "wlb_ElectroEls" VALUES('El_KeySqr_2_tst','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','/wlb_originals/wdg_ElFigure',0,'JavaLikeCalc.JavaScript
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
}','',-1,'name;geomW;geomH;lineClr;elLst;p1x;p1y;p2x;p2y;p3x;p3y;p4x;p4y;c1;s1;',1500037083);
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
KB6Pz8/Pi84SCoV+AY8g+verurPrAAAAAElFTkSuQmCC','/wlb_originals/wdg_ElFigure',0,'JavaLikeCalc.JavaScript
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
{ c1 = fillColor = lineClr = "darkgrey"; }','',-1,'name;dscr;geomW;geomH;lineClr;fillColor;elLst;p1x;p1y;p2x;p2y;p3x;p3y;p4x;p4y;p5x;p5y;p6x;p6y;p7x;p7y;p8x;p8y;c1;s1;',1500037083);
INSERT INTO "wlb_ElectroEls" VALUES('El_KeySqr_3_tst','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','/wlb_originals/wdg_ElFigure',0,'JavaLikeCalc.JavaScript
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
}','',-1,'name;dscr;geomW;geomH;lineClr;elLst;p1x;p1y;p2x;p2y;p3x;p3y;p4x;p4y;p5x;p5y;p6x;p6y;p7x;p7y;p8x;p8y;c1;s1;',1500037083);
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
xrKs8pFULpcBQHmEEsXzfLPZlGUZXWzuXpx7e/0PVUTDDgGJdOIAAAAASUVORK5CYII=','/wlb_originals/wdg_ElFigure',0,'JavaLikeCalc.JavaScript
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
{ c1 = fillColor = lineClr = "darkgrey"; }','',-1,'name;dscr;geomW;geomH;lineClr;fillColor;elLst;p1x;p1y;p2x;p2y;p3x;p3y;p4x;p4y;p5x;p5y;p6x;p6y;p7x;p7y;p8x;p8y;p9x;p9y;p10x;p10y;p11x;p11y;p14x;p14y;p15x;p15y;p16x;p16y;p17x;p17y;p18x;p18y;c1;s1;',1500037083);
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
4/WfVPRN8kqLrcsAAAAASUVORK5CYII=','/wlb_originals/wdg_ElFigure',0,'JavaLikeCalc.JavaScript
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
}','',-1,'name;dscr;geomW;geomH;lineClr;elLst;p1x;p1y;p2x;p2y;p3x;p3y;p4x;p4y;p5x;p5y;p6x;p6y;p7x;p7y;p8x;p8y;p9x;p9y;p10x;p10y;p11x;p11y;p14x;p14y;p15x;p15y;p16x;p16y;p17x;p17y;p18x;p18y;s1;',1500037083);
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
AAAAAElFTkSuQmCC','/wlb_originals/wdg_ElFigure',0,'JavaLikeCalc.JavaScript
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
{ c1 = fillColor = lineClr = "darkgrey"; }','',-1,'name;dscr;geomW;geomH;lineClr;fillColor;elLst;p1x;p1y;p2x;p2y;p3x;p3y;p4x;p4y;c1;s1;',1500037083);
INSERT INTO "wlb_ElectroEls" VALUES('El_KeySqr_5_tst','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','/wlb_originals/wdg_ElFigure',0,'JavaLikeCalc.JavaScript
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
}','',-1,'name;geomW;geomH;lineClr;elLst;p1x;p1y;p2x;p2y;p3x;p3y;p4x;p4y;c1;s1;',1500037083);
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
PB7PyMiI5CwOh+Mf3mo0OI3AkzIAAAAASUVORK5CYII=','/wlb_originals/wdg_ElFigure',0,'JavaLikeCalc.JavaScript
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
','',-1,'name;dscr;geomW;geomH;lineClr;fillColor;elLst;p1x;p1y;p2x;p2y;p3x;p3y;p4x;p4y;p5x;p5y;p6x;p6y;c1;s1;',1500037083);
INSERT INTO "wlb_ElectroEls" VALUES('El_KeySqr_6_tst','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
8T8yUKQt2iUmhQAAAABJRU5ErkJggg==','/wlb_originals/wdg_ElFigure',0,'JavaLikeCalc.JavaScript
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
}','',-1,'name;dscr;geomW;geomH;lineClr;elLst;p1x;p1y;p2x;p2y;p3x;p3y;p4x;p4y;p7x;p7y;p8x;p8y;c1;s1;',1500037083);
INSERT INTO "wlb_ElectroEls" VALUES('El_Key_1','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','/wlb_originals/wdg_ElFigure',0,'JavaLikeCalc.JavaScript
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
}','',-1,'name;dscr;geomW;geomH;lineClr;elLst;p1x;p1y;p2x;p2y;p3x;p3y;p4x;p4y;c1;s1;',1500037083);
INSERT INTO "wlb_ElectroEls" VALUES('El_Key_1_tst','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','/wlb_originals/wdg_ElFigure',0,'JavaLikeCalc.JavaScript
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
}','',-1,'name;dscr;geomW;geomH;lineClr;elLst;p1x;p1y;p2x;p2y;p3x;p3y;p4x;p4y;c1;s1;',1500037083);
INSERT INTO "wlb_ElectroEls" VALUES('El_Key_2','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','/wlb_originals/wdg_ElFigure',0,'JavaLikeCalc.JavaScript
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
}','',-1,'name;dscr;geomW;geomH;lineClr;elLst;p1x;p1y;p2x;p2y;p3x;p3y;p4x;p4y;c1;s1;',1500037083);
INSERT INTO "wlb_ElectroEls" VALUES('El_Key_2_tst','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','/wlb_originals/wdg_ElFigure',0,'JavaLikeCalc.JavaScript
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
}','',-1,'name;geomW;geomH;lineClr;elLst;p1x;p1y;p2x;p2y;p3x;p3y;p4x;p4y;c1;s1;',1500037083);
INSERT INTO "wlb_ElectroEls" VALUES('El_Key_3','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','/wlb_originals/wdg_ElFigure',0,'JavaLikeCalc.JavaScript
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
}','',-1,'name;dscr;geomW;geomH;lineClr;elLst;p1x;p1y;p2x;p2y;p3x;p3y;p4x;p4y;p5x;p5y;p6x;p6y;p7x;p7y;p8x;p8y;c1;s1;',1500037083);
INSERT INTO "wlb_ElectroEls" VALUES('El_Key_3_tst','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
cBJVrQIAAAAASUVORK5CYII=','/wlb_originals/wdg_ElFigure',0,'JavaLikeCalc.JavaScript
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
}','',-1,'name;dscr;geomW;geomH;lineClr;elLst;p1x;p1y;p2x;p2y;p3x;p3y;p4x;p4y;p5x;p5y;p6x;p6y;p7x;p7y;p8x;p8y;c1;s1;',1500037083);
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
RhIQG0lAbCQBsZEExKYvl8vJ5b2qkcvl/gBF1skqfQ6NXAAAAABJRU5ErkJggg==','/wlb_originals/wdg_ElFigure',0,'JavaLikeCalc.JavaScript
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
}','',-1,'name;dscr;geomW;geomH;lineWdth;lineClr;elLst;p1x;p1y;p2x;p2y;p3x;p3y;p4x;p4y;',1500037083);
INSERT INTO "wlb_ElectroEls" VALUES('El_Krug','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','/wlb_originals/wdg_ElFigure',0,'','',-1,'name;dscr;geomW;geomH;lineWdth;lineClr;elLst;',1500037083);
INSERT INTO "wlb_ElectroEls" VALUES('El_Zemlia','iVBORw0KGgoAAAANSUhEUgAAAEAAAABACAYAAACqaXHeAAAABHNCSVQICAgIfAhkiAAAAAlwSFlz
AAAOxAAADsQBlSsOGwAAAbJJREFUeJzt2q+rFFEYxvGPv7BcRbSJmDcKgs0gt2gTQRAMYhDMtgv+
A6Imm/+BSdBiElEMBhUs2lyTqMmqYQ3HBVm4yt0d3veM+35h4pzn4TvDOXMOQ1EURVEURTEIh3Ai
u0QmE3zEfWwkd0lhgtnv6yH25taJ508BM7zHtdRGwSwKmOEHLmaW+hu7AzL2YTMgZykiBHRNCcgu
kM3aC+hhnT6HA3iGr9HhPbwBh/EAj3E0OrwHAXNO4RFOY39UaE8C4CSeYysqsDcBc27iakRQrwL2
CJqgexUQRgnILpDNWAUcM9AcMVYBN3BPWzZXYqwC4DqeakdvR5YdZMwC4CDOa5/Sl5cZoIfN0BBs
avuIl5ju5Mb/QcA33Nb2EdOd3jx2AU+07fTSjFXAF5zBu1UHGquAW0MNNPZVYGVKQHaBbEpAdoFt
+IwPEUG9CfiJOziLFxGBvS2DdwUeiNKPgDfaZmYaHdyDgNe4gE8Z4T0ICJnstqO3STCcEpBdIJu1
FzA0i3+JfcdbHM8sFcmigEu5deKZC3iFK9iVWyeeiXZOv5b/CdOeeA8fV0VRFEVRFP/kF2ajOWl2
YyV/AAAAAElFTkSuQmCC','/wlb_originals/wdg_ElFigure',0,'','',-1,'owner;name;dscr;geomW;geomH;lineClr;fillColor;elLst;',1500037083);
INSERT INTO "wlb_ElectroEls" VALUES('El_lhor','iVBORw0KGgoAAAANSUhEUgAAAEAAAABACAYAAACqaXHeAAAABHNCSVQICAgIfAhkiAAAAAlwSFlz
AAAOxAAADsQBlSsOGwAAAEtJREFUeJzt0EENw0AAxEBfFEblj6NwUhTVPjKDwHIBAAAAAAAAb3Cq
zzpi6VTPOmLpWgesGbAOWDNgHbB2V991BAAAAAAAAMC//QDfdwIa5LCYSwAAAABJRU5ErkJggg==','/wlb_originals/wdg_ElFigure',0,'','',-1,'name;dscr;geomH;lineWdth;lineClr;elLst;',1500037083);
INSERT INTO "wlb_ElectroEls" VALUES('El_lvert','iVBORw0KGgoAAAANSUhEUgAAAEAAAABACAYAAACqaXHeAAAABHNCSVQICAgIfAhkiAAAAAlwSFlz
AAAOxAAADsQBlSsOGwAAAKZJREFUeJzt0LENgEAQxMATLdF/G98OVEB6Q2BLG6/lmX3umXk+drZl
ru3Dv1EALaApgBbQFEALaAqgBTQF0AKaAmgBTQG0gKYAWkBTAC2gKYAW0BRAC2gKoAU0BdACmgJo
AU0BtICmAFpAUwAtoCmAFtAUQAtoCqAFNAXQApoCaAFNAbSApgBaQFMALaApgBbQFEALaAqgBTQF
0AKaAmgBTQG0gOYFE3EHgsZqNckAAAAASUVORK5CYII=','/wlb_originals/wdg_ElFigure',0,'','',-1,'name;dscr;geomW;lineWdth;lineClr;elLst;',1500037083);
INSERT INTO "wlb_ElectroEls" VALUES('Induct','iVBORw0KGgoAAAANSUhEUgAAAEAAAABACAYAAACqaXHeAAAABHNCSVQICAgIfAhkiAAAAAlwSFlz
AAAOxAAADsQBlSsOGwAAA6JJREFUeJzt281rXUUYx/FP05fENFYSjRSt1Yq4qApioytREBUXigpa
Uap20X+ggojoTtzZtYigUMGNbusfYH1BoepCrW/VFkFbG5O0akybNNfFcw7TNvVuvHPmcK9fOORc
uLnzm+c8c+Z5nplhwFnVcHtDWI9J3IDL8GbDGoqxBc/hE5xGB0tFFTXECJ7G51LH62u5oK5GGMML
WHBuxzs4hgPlpOVnNV7GvNTpabyNO3AFLi2mrgHuw59S53/BQ1hXUlRTbMJHYox38AOmiipqmMdx
SnT+VPV5qKiihnlPcv33C2tpnAkcEZ0/g11l5XQnh1tuEdEenBDjv7XkMMAGMQXCyepqLTkMsCTc
v/791V2+W5wcBjiGxep+EpdnaKNn5DDAEcxV9yO4JUMbrecV54a/k2Xl/Du5xucCHsRFwgvWYr80
NFpDLgMcx5XYJobZVNXWByI2GAg24xspH/hbVH9uFF7RCnJOUSdwELeL6HANbsYjuB5bq8+fZtRQ
nNr9D1hZDVoSiVLfswqP4Xcrq0KdLv/XFwxjh4gL6ndBfc1jppy0YE3m338Ue3BJ9XkJ3+N1fCte
jH3LreLJ1098BrtFbND3TOBdye1n8JSWJ0a95H78JTq/KJ78wHQe3pJc/2sD4vY1YzgkGWB3WTnd
yZEOXyuqQkQ16MsMbfSMHAYYl6bXOcxmaKNnDFSt/kLkMMCMlPePV1dryWGAH/FHdX8xbsrQRuvZ
K80CB7V4GswVnJwWK8FrxRCYE3l/32d/NeN4RwqFZ7HTgEWD20TH66Ewi2cwWlLU+eR8Ir/iZ1ES
GxN1wLuxXdQJJnAVfsqooTjDeEI8/fMLIgvSAkrfs10skAxcSWxIFEY+s7Iouii8oCi5S2J34TWR
INUcxT5RGmv10vl/5WpR96vH/jzeEFtkhwvqaoRRvCqWwTrC/V8yIFvk4E5pHeCMWC1ubTicgz3S
y+64Fi+P52AE30kGeLGsnO7kSIc3SwshC2KXeGvJYYCNIgsk3P+3DG30jBwGGJJOoixr+YaIHAY4
KXV6g1QhbiU5DHBYrAoR74LrMrTRevZJs8D+wlq6kqsesIwHRK6xUeT8XxmA7K9mEz6U8oBDuK2o
ogLcK8rj9VA4ioe1LB/IWRI7LOL/KREXjOEesTNsWiRI6/T5LpH1eN6Fj81N44ty0ppjGE+KqlB9
jmhgDk6ezTV4Fh9Lhih+dLbE4elRcWh6a/V3b8Ma/uds/gH4H+3N1tSFQwAAAABJRU5ErkJggg==','/wlb_originals/wdg_ElFigure',0,'','',-1,'owner;name;dscr;geomW;geomH;geomXsc;geomYsc;lineWdth;elLst;',1500037083);
INSERT INTO "wlb_ElectroEls" VALUES('Sharp','iVBORw0KGgoAAAANSUhEUgAAAEAAAABACAYAAACqaXHeAAAABHNCSVQICAgIfAhkiAAAAAlwSFlz
AAAOxAAADsQBlSsOGwAABHpJREFUeJzl20toXVUUBuAvvY+kValWrVShVUFFQUEUcaSoOKkKFqrS
WmksSH1NBFEnUlARRZzoTBRxJuIDEZ35qOBAFAfqwCdIn0nTV/ry0SZxsM7hROlk79zksOkPGdzA
/s/ea/17rXX+mzB39PACZjCBu9EZAG8x6GIzjmIaO7Aai9rc1EKjg4ewXyhhl1DCUJubWmgM4xEc
FEoYE0rotrmphUYfD+OQUMJ2rG11Ry2gJ5SwTyhhD+6ofn/KYETUhL9FELaJIBSLnvTWtkgEYa9G
CWsVWBOG8Yy8qv5/JWwXhbGY7tATh5/BJEal3+VaCfsqnjERzCLmhI7Y7KTI4DjWS5fxiCiMhyqe
XUIJRUyMQyLz45pxd1T65nuiRR6seHaI4BaBnsj8hMjg/upzP5GnL5Swv+KZUNCw1BWZr8fdMWzM
4BkWNeFoxbMNawazxflHR2R+TGTwCNZJV0JHXIfZLXKNQpTQF5k/IjK4WwQhFSN4EFOaYWn1gPa4
IFgnDj8jgrFRuhKIIOzVop+wAssy1vVFEI5o3v7WS9/8iLgOrfkJ3+NDLM9cv1EcfkYUyFHpd7lV
P2Eax/E2Vmas74vMz25t66VPjK35CV9WD5zGe+JKpKIjMj8hMjhefU7NYCt+wsX4QFTj4/gIF2bw
dEXmx0UwJ+UNS634CSvxqUYJ7+L8DJ6eyPykpkVuyOBpxU9YIQ4+Vf18Ia8mDInM79YMSxukK6EV
P+EifIITmpqwKoOnLw5dt8jdIihF+Alni4PXQfhKXhCIINTDUlF+wiq8I4IwhY9xaQZP3SKL9BPO
Et3hhIj8VtExUlG0n3AB3hJBOIHPcXkGT9F+wjK8r1HCZ7gkg6doP+EcvC4GpSl8gysyeIr2E5Zr
CuOMGJxylFC0n3AaXsM/YvM/4bJMrmL9hPPwpiYIW3FVBk/RfsISvCEmtBnhKVyZyVWsn3AuXtUU
xq9xbQZP0X7CiLgOtRK+w9UZPEX7CWfiZRGEKfyI6zJ4ivYTzhDdoVbCt7gmg6doP2ExXsSf1UP/
EEpIlXHRfsJSvIK/NDXh+gyeov2ErlBCPatvF0HIeWixfsJSPK+5Dj/gRnlVvVg/oYOXNEr4HTfI
qwmjCvUTTsfTOCYi/ytukS6/+fQT5h09/60Jv+Am6UqYDz9hOmN9FobxFA5XD92JW6UrYZB+wmY8
l7huTlhcPfCYyODPuDmDZ1B+QlcLf706hMc1k96kUEJONR6Un7DgWCKuQ53B33CbvBY5CD+hNTyB
AyIIe3C7vM0Pwk9oBYvxqCaDtbubGoRB+Qmt4UmNEnbiTnndYdTc/YRW0Bct6YDmre0u6TI+mZ9Q
zD9wdfGYpjvsEK+uqRmc7SdMY4uCrsIibNK8vx/GPdIPMCQyv0UhbXE2ak+vNjZ3ilaXqoSOgjJ/
Moxq7vJR3KvAbM4Fw7hPUxjHFNTfB4lNmiHnIO5XuLRT0RMtsS6M+/CAU+w6DIlxt3aE9oogFNHf
B4WOmBB3aQrjKReEetKrv/F51gDqwb+eXuvCL9Ux+gAAAABJRU5ErkJggg==','/wlb_originals/wdg_ElFigure',0,'','',-1,'name;dscr;geomX;geomY;geomW;geomH;geomZ;lineWdth;elLst;',1500037083);
INSERT INTO "wlb_ElectroEls" VALUES('arrowSharp','iVBORw0KGgoAAAANSUhEUgAAAEAAAABACAYAAACqaXHeAAAABHNCSVQICAgIfAhkiAAAAAlwSFlz
AAAOxAAADsQBlSsOGwAAA/JJREFUeJztmc1OE1EUx38zEVrabt3jI4gEDIRPg+DGGOMj6F5XpAgh
CjyJ8R2MS58CKODahSaAhIQE6uLMcT5b77QzvQXvP2nSzNzO3HvO/3z8T+E/hzdE727b2MQ9Gy8N
MA/cRwzxA/hmcS8DhQ/MAS3gV/A5BGaDe3cei8iBbxKfA2DB3rbKhw88ITz8KfAB2AHOgmuHwDJ3
lAmPCQ9/DWwBjeCzHVxTI0xb2mMp8IA14Bg54BmwAVQiayrAJiETjoBV7FapQuABk4SHvwI+AvWM
tQ0kHK4IjfCIW24E9XwbuADWgWqX9VWgGaxtEzLh1sFHYl49f4F4vmbw2xqwG/xGmTDNLUuM0Wx/
hXg+efhRJPYrwfcoaggTNBy0Ogw9fOJ1/gzxfJL2NSTePwGfgzVjiTVV0iVygSFnwhzxUrdBOuHV
gPfAJRLn7eB7k7QRGkh10BJ5gHSMQwcfeIq0t9rkbBEvdSAH2kMOfAOcAN+D75dI7CcNVkH6hCgT
Vhiy6jBJePhrpMNrJNaMIoz4jXj9GMnwa4gh2sA5woRkTtASqUxoARMlnKMnPEO8qAdYJ/sAu8Sp
/DByf4K4AXfINmAzeEcbMdpagefIDR/xfJTCWU1OBWGEUvgAqRJRCnsIraPJc5t0CNURQ0ZDaAJL
4aAxr0lsnewktkXo+X1gpsszZxEDKRM2STNhDGGCJtEWYryBIarnbxBKdipje8F99fw83cuYj5S6
KBN2SJfRMYQJ+uyBzhMWiZe6ddK0ryMJTxuZfWApxzuWiTdSTdKNVD24Hs0rpc4TOun5ZMKrI57X
VrYFTJHPOz7S/h4RttK7ZHeTA5sndNLzUVQRz6uY6Tc+VxEjqJhqkg6H0ucJJnpeN7JHSPsWUur6
ydAeIomVCVdkl8jS5gmmer6CMEKTknq+iPLkETJBHbBJtgMKnyeY6PkGkguipW6qn5d2wDTxENwm
OwQLmSeY6vlRJDlFm5wlyklCPvHqoCUyS0r3PU8w0fMNJBdEPT+f5yU9YoF4s9QkW3H2NE/Io+f3
EGGjnp9hMI2IjzQ9usdzxOOFzBN60fPa2w8aK8Rb8b7mCR5inbx6/pj+S12v8BAhdEL+eUIqVD3g
FfI/XV49bxt55wk/gZd0cNoL4G2HB3TT87ZhOk94Bzzv9iAPGElcM9HztmE6Txgh577z6nnbMJkn
GKMXPW8bpvOEf6JfPW8bJvOEjihCz9uG6TwhhaL1vG2YzBP+ogw9bxum8wR84A1hqStSz9tGcp5w
CrwmI6QfAF8pT8/bhs4TvgDjnRY9QCZAtynhmUL/yBm3vA8HBwcHBwcHBwcHBwcHBwcHBwcHB3v4
AyDam/oBGHDXAAAAAElFTkSuQmCC','/wlb_originals/wdg_ElFigure',0,'','',-1,'name;dscr;geomX;geomY;geomW;geomH;geomZ;lineWdth;elLst;',1500037083);
INSERT INTO "wlb_ElectroEls" VALUES('battery','iVBORw0KGgoAAAANSUhEUgAAAEAAAABACAYAAACqaXHeAAAABHNCSVQICAgIfAhkiAAAAAlwSFlz
AAAOxAAADsQBlSsOGwAAASZJREFUeJzt1zFKA0EYhuEXCSkEbWzUykbwADYKClZi4wW0shAPYecl
TGWR1jtYeIpcIEVKFWsLV4iQ/98fp3Pfp8nu7Hz5wjBDWJAkSZIkSZIkSdIvB8B28OysMR85AUaF
eVF/Kb9W/DGnwH7w7KYxH7kGxoV5UX8pX12Af6uyxfaC8c3GfGsm6y93VnbAQzB+BFw05FszWX+5
c/BHYPALUDnDP+6BxdL9DvDUXW8Al8ALcAjMgF3gvbt/XZHPbAGf3fUd8Bh8PgPHwDTJN5sCt3z/
ry47B65WlFfzfZkJsJ7MyforecAjUFqAeTD+Abw15FszWf9fOlPZFu47An35SGkLJ/0egYrBL0DV
iHixKi8sWT5S+d5sXjUvSZIkSZIkSZIG4gvGZClP5o30+QAAAABJRU5ErkJggg==','/wlb_originals/wdg_ElFigure',0,'','',-1,'name;dscr;geomW;geomH;lineWdth;elLst;',1500037083);
INSERT INTO "wlb_ElectroEls" VALUES('conBus','iVBORw0KGgoAAAANSUhEUgAAAEAAAABACAYAAACqaXHeAAAABHNCSVQICAgIfAhkiAAAAAlwSFlz
AAAOxAAADsQBlSsOGwAAAIRJREFUeJzt1LENwkAMBdAPYoIbkJKaMiOkzAwMmBkoCNKFAc4ReU+y
dL7q24UTAAAAAAAA4Awu3fueZKoKMtic5JUkt+6zbTVXJBpoymfOJPsFJMmaZBkaZ7xH31yrUhyF
BVQHqPZ7A1qSZ0WQgVrf9AtYt9odiT/0nRMAAAAAAAA4kTebsQnkLLRE0QAAAABJRU5ErkJggg==','/wlb_originals/wdg_ElFigure',0,'','',-1,'owner;name;dscr;lineWdth;lineClr;elLst;',1500037083);
INSERT INTO "wlb_ElectroEls" VALUES('converterAC_DC','iVBORw0KGgoAAAANSUhEUgAAAEAAAABACAYAAACqaXHeAAAABHNCSVQICAgIfAhkiAAAAAlwSFlz
AAAOxAAADsQBlSsOGwAABHNJREFUeJzl21uoVFUcx/GPdgzTJ8NCyM7MiBIFFSVkiEYSQRTRQz1E
9VAPBWHaWxL2EAQ+dIWMoIeC6KG7dLG0EpOMoscyEpPAisguiEVeysv08J/RYXs8s+8zzvnCYdic
vdas/2/91+W39h6mONMS1zMxfRANqZHjONy9SArwLBbV2pz62Y3V3YuxzudMPI57cBRH6m9X5czG
LCzvXD+Ew10BpoueP4pH8HXtzauWBViDS4QQi3SG+ljixiMi+M/rbF3FNLAOC7FDiHGCUZ/wxrFe
pP0XYpgf7L0hmQGjxDhew1V4R0x8jeRNo5oBDTwngv9SBP/zRDeOogAtvIKb8T7ucJrgGT0BWnge
S7DNJD3fZZQEaOEF3IBPcLc+wTM6k2ATL2IpNuMB/Jim4ChkQFNMeCuwHfdjT9rCWTNgDBfhUszF
T/gKv2Wspyzm41UsxntizKfq+SSzsAm/Y9kk96zFLrFlbuNffIRr8nxpQeaL9b0ten48RZllIsZN
Ip4T9BNgkQi0jZ3CNT6KjfgPf+MunJ05jHw0xfrextu4IGW5XALMxVbR2x/jSpzV+d8cPIb92Ivb
VD+vNJ0Ufrv0wZNDgGnCRbXxIc6foNIxrMQhIcKN0olwnuxDpyU6o413pUv7XnJlwDw8LKzk6ThH
ZMI/+KFTx2QizBP79L24LmXjWyIDD4t1/pR9fQpyzwFpmI1ncAzfi61o8sSJWKu3du77Vqwo/Wh1
2tfufDZztrFSAYg54UmRCfvxFK7FZZ3P9fhVDJc3cHGKOhv4TMxBG+UPnhoEIDJhDf4QPXZABH2w
c/2LGFJzUtQ1Ltb3Nj6VL+17OUWAKrbCB/AEXsd9uEKcOe4Tqf+WEOd4n3om8vN99/Z5KTMDkszG
uZiRoUxD9PxRsdRdWFJbasmAJAc6f2lp4WVxjFV5zw+bGcrs54syTALk8vNFGZbzgKacfr4ow5AB
TQX8fFEGnQGl+fm8DDID5ouev1ocqqxSc/AMToAm3sQt2IDbxelS7QxCgKbo+cXiGeRqsT0eCHUL
0MJLuEnsxu40wOCpV4DuOr8UW0TPDyTte6lLgO4O73rh6u41gAlvIupYBhtib78EH4hNzp4avjcV
VWdA8vn8SkMUPNVmQK1+Pi9VZUDq5/ODpgoBMj2fHzRlD4E8fv7BEr9/nxA/M2UciXXP7dvi9Dbt
AWa7xL/v+nxXZUdiTfn9/NqS2gB/Zi1QhgBNJ/38Ftn9/LoS2pCbogIM3M8XpcgqMBR+vih5BWga
Ej9flDwCNA2Rny9K1jmgJWb7FWLMr1I8+CUFy/dyCN9kKZBFgKr8/IYS6uiyWzyFTk1aAXr9/Gax
1JU14e0sqR5ydEgaAar287eWWNexrAX6CdDr57cJP1/2UvdXyfVlYjIBzgg/X5TTLYNnjJ8vykQC
LHAG+fmiJIdA9/2ehWp6Pj9okgLMEu8D7hAbnobiLyYNE5dLvKrTFeC42EQsF1mwAE/X2rR6mCFi
3q3zktaU/+nslP/x9JTnf0hsOGCVcGQMAAAAAElFTkSuQmCC','/wlb_originals/wdg_ElFigure',0,'','',-1,'name;dscr;geomW;geomH;lineWdth;elLst;',1500037083);
INSERT INTO "wlb_ElectroEls" VALUES('converterDC_DC','iVBORw0KGgoAAAANSUhEUgAAAEAAAABACAYAAACqaXHeAAAABHNCSVQICAgIfAhkiAAAAAlwSFlz
AAAOxAAADsQBlSsOGwAAA6RJREFUeJzt2z2IHVUYxvGfIYGw6a1C7twlaUSwCKiEBAyyIIhY2Ig2
WliEmBUbLRTsLCxEVARBBUkRQQiihogKBj/Rxo+AFtskiiAiQRsTTEws3h1zud7Nzsw583F3/cOw
zHLOmfM+5/O5Z4ZNznVT99uxpY+KdMhlXChvpgV4Hns6rU73rGC5vNm6+nc7nsGDuISL3derdXZg
AQdW7x/DhVKALaLlL+FJfNt59dplEY/jBiHEHqtDfetUwosi+E+7rF3LjPA0duO0EONfNvqEtwsv
iG7/uRjmf04mmO4BG4ldeAM34y0x8Y2mE23UHjDCiyL4L0TwP81KuBEFGOMo7sI7uM8awVN9CNyI
25OrdpWP8XXG8krGeAm34JRrtHxJVQH24bmUmk3xqPwCjPEylnACh60TPNUF+ApPNK7af/ksY1lQ
4FXRUO/hYZytkrGqAN+sXkOkEBPeQXyIQzhTNfO8L4M7cQx78bYY85VavmSeV4GdouVvxZc4ombw
zK8ABd7E3TiOe/Fjk4LmUYBCtPxe4VmW8XPTwqrOAdeLZSYXZ/FLg3xjMdsfFGP+iITgqS7AkjAS
uXgKr9TMU67z+8Rsv6xht5+kqgC/44fUh01wrmb6coe3JNb5QxpMeLOoKsAH8v5GcL5G2hFeF9vb
E2KTcyZXRaoK8Nfq1TWTfv6U2N5mafmSIW+EZvn5dff2dRnqMljZz6cyRAFq+flUhiZAbT+fypAE
KNf5O8Sq84CWg2c4k2ChoZ9PZQg9oHDVz3+ipp9Ppe8ekOznU+mzB2Tx86n0JUAhk59PpQ8BChn9
fCpdCzDGa7gTJ3G/HoOnWwFa8fOpdCXApJ//CA/pYcKbRRfLYKt+PpW2e8D0+fxhAwqedntAJ34+
lbZ6QGd+PpU2BOjUz6eSewg08fOPZHz+OSF+bRbExuRX7G/48DHexxW8a8b7OGtwJeP1/TrP2i9i
PCliztYDCs39fM73Dn6rmyGHAIWE83nxDl9vpArQu59PJWUVGISfT6WpAIWB+PlUmghQGJCfT6Xu
HJD9fF7sGXJxHt/VyVBHgLb8/PEMZZSs4LY6GaoK0Nr5vLzvHdRukCoCtO3n78lY1t91M6wnQOvn
8/gjc3m1uJYAc+HnU1lrGZwbP5/KLAEWzZGfT2V6COwQX1ft1tH5fN9MC7AgPi07LTY8I9V9/Txw
E7ZN/qMU4LLYRBwQvWARz3ZatW7YJmJeETH//+nspv94etPzD+Br71UEFxUSAAAAAElFTkSuQmCC','/wlb_originals/wdg_ElFigure',0,'','',-1,'name;dscr;lineWdth;elLst;',1500037083);
INSERT INTO "wlb_ElectroEls" VALUES('fuse1','iVBORw0KGgoAAAANSUhEUgAAAEAAAABACAYAAACqaXHeAAAABHNCSVQICAgIfAhkiAAAAAlwSFlz
AAAOxAAADsQBlSsOGwAAAMpJREFUeJzt2DEKwkAURdGrCIHYuxgldu7KnVnrYuwVbdTCXv9gkh+Z
e+rH8HgMTAhIkiRJkiRJUkVmgcwc2AxdZCBH4PEpEBmgBS691BnfErh+CiwKDtsBp5/qjGcNHCLB
kgHufFlzQu7R4HzIFv/AAbILZHOA7ALZqh+g5BncA+ehivRsFQ1WfwMiWuAJdNlFCnS8O7ffgtXf
AAfILpDNAbILZKt+gJIPoYbAszIRTTToL7HAITdg20ud8d2yC0iSJEmSJEnSpLwAvZARaHejt+wA
AAAASUVORK5CYII=','/wlb_originals/wdg_ElFigure',0,'','',-1,'name;dscr;lineWdth;lineClr;elLst;',1500037083);
INSERT INTO "wlb_ElectroEls" VALUES('key','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
mcn/AQiUmKNwBpD0AAAAAElFTkSuQmCC','/wlb_originals/wdg_ElFigure',0,'JavaLikeCalc.JavaScript
if(var.isEVal())	s1 = 1;
else {
	s1 = 0;
	if(var)	p1x=85; p1y=60;
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
}','',-1,'name;dscr;lineClr;elLst;p1x;p1y;c1;s1;s2;',1500037083);
INSERT INTO "wlb_ElectroEls" VALUES('limU','iVBORw0KGgoAAAANSUhEUgAAAEAAAABACAYAAACqaXHeAAAABHNCSVQICAgIfAhkiAAAAAlwSFlz
AAAOxAAADsQBlSsOGwAAAn9JREFUeJzt2kuIjVEcAPCfOxO5yiOvPEJeibKwIcxsZGGjLCShLFiM
QpLXKBQLlLKjrDwiKUlZYKcolKwUobyirJQ3YXHMkvvdme9859ac3+ZuTuf/v2f+5zvf/9why7Is
y7Isy7IsaxntqFcVqBWtwAn86sMc23G20aABfQgQyyhcw2tc0ftFuIunZSVVlbrwl7+DKWlTSWM5
3glboN+Zjgc4hkGJc6ncYJwSSn9i4lySWCmU/rKqAxc5BmtYFDGHMdiH+/iBjhLnvq3BKVLkGKzj
UynpVG8IPv9vQDMvQkuFPVqWNqzHLqwTzu2yLMTNIgObWYBvGqxmk+aiCyeFUv1e4tzfig5M9Spc
x2G8//v5M1EeSRagp/SnChWQ7MsXVcdv5T2d5+MFNovXi3QIOTfsKGuREviXEcKR9xBnhCSTqnIL
tGMjpmEtPlQYu0/K2gKLhRZ3g/hteMttgZHoxj1c0gKl36OKLVDDHkwW2t2WKv0qKmAB1uA4nlUQ
rymxF2ASjuAqLkSO1SsxF6AN24Q7vkP4GDFWr8V8BizBKuzAq4hx+iRWBczEQVzE5UgxShFjAQZg
C4biKL5EiFGasrdADauFq62teFvy/KVrZgEGafxmNQE7hcuIOwXGx1L4VjlfiRWY5Cs6G4ypCU/8
5TiAxwXmrcLXqgLNwHPhyd/vDMM5XMf4xLkksQkvhZvYfmcW3mC31vypParhOC+U/tjEuSTRLbzo
TE+dSArzhN5+d+pEUhiNG0J/PyxxLpVrw17hzJ+dOJckOoXS71L9bwrJjcMtnJauyUmmhv14gjmJ
cylV0TIeKHSE3XgUL53WVtO6/1maZVmWZVnz/gA8iGIALsNG5AAAAABJRU5ErkJggg==','/wlb_originals/wdg_ElFigure',0,'','',-1,'name;dscr;lineWdth;lineClr;elLst;',1500037083);
INSERT INTO "wlb_ElectroEls" VALUES('load1','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
rkJggg==','/wlb_originals/wdg_ElFigure',0,'','',-1,'name;dscr;geomX;geomY;geomZ;lineWdth;elLst;',1500037083);
INSERT INTO "wlb_ElectroEls" VALUES('load2','iVBORw0KGgoAAAANSUhEUgAAAEAAAABACAYAAACqaXHeAAAABHNCSVQICAgIfAhkiAAAAAlwSFlz
AAAOxAAADsQBlSsOGwAAChVJREFUeJzt23uwlWUVBvDfOeDhpCCoSCAiF0VT0xwQsNR0plGcoDSY
Mi9ZmGmmFmXGlFmZlTlOXrM0L5hJmmWTUlMqQ6VpKeMFDEGUAhUVs7E0pYuX/ni+3dnss8++nJvM
5DNz5sz+9vu933rXuy7PWu+3eQP/32jpwb1tGIeJ2BZD0Ip/4lk8gZX4e89E7Fs0q4DSoqfjfRgu
i24pm+u14v+r2IAluBYr8Leeidv7aEYB43EiPoD/YCHuxTI8id0wGZdjJHbE3jgA++A3uASL8XKv
SN9PaMex+CPuwXEYhc0qxs3FQxXXWrEl9sf1WItvY4c+lLdXMQRfEp++EGNqjK2mgHK8CUfhYdyO
nXtJxj7DEPwAa3AoBtYZX08BJeyIm7EU+/ZAvj5Fu+z8Ghze4D2NKgBGYxEekAyyyeFYMftDm7in
GQWQbHI7bsDWTdzX5xgvAe8C9c2+HM0qAKaJok9o8r5eQ2vF5zZJdS/hW/o+Xd2Nq/AZr5MrVCpg
nOT57+HxfpJhvhCmI/WMmXYLlQqYLiTnF/0ow0ohSe/BNv34XGysgDahtwvFL/sLr+EaYY3j+/G5
2DjIjRNuf69YQX/iT/grJkntUI4Rwhd2xfYYjFeK8WsljiyTIqxplFvAxOLzsu5M1ENsEJo9rfi8
kwTju7Ea38C7JFC2irXujA8KqXpCYslMod4No9wCtpUg9GQ3F9ETvIzHpGY4CXPwZvwMX8Mj4pYv
iKU+hxcxVEjVnviQ1BlLcY5YUl1LLlfAEFFAd0rWFjHNrYUPNIubJPVOFTO/XNLjuopx4/HL4m+u
bNaTstj52A8ni+Lm4+t4vtaDyxVQcodXmxS+DQfJrg3BeXhK442QgWJ9x8iCZwtFruQgA3AYtsJ7
pThbVfb9q8Islwp9P1WUeYbEmLo4AcuxRYOCk53/nPjghfg4/owfajyiz5Zg9gR+X2PcaEmZF+O3
+K4ovyu5psp6Fklwb0iQ5VKtNYJROF8WPFcKqFZMwe8km0yRnesKk2Thl0nX6IYuxrVKcfaAxIDD
JDgeUkfGKRJIr9UAx5iGB3FwvYGyGzeJ8DN0bo7sIn64ArO6mGM73FaMmyBKO7OLsW8R0z5VFLqZ
xIFbGpB1P/wFn603cKikoi/ozBBLaJF09ABuxV415mvXEQ9O0zk9nSN0eyTGSrA6qMo8A8S9lmBY
2fV9JWa8v4YMJZk/JaX93nXGukS0Wi2XDhQ3WaFj1+phsOTz9ZKiSvNOKAQq7cpxxbzV4sb44v6P
VVxvL+ZcLNZUC8OFbl8qnaku8XaJ3vtXXB+K0/EMvqr5QHmImPBtYjVn406JI9tIUDtPZ1dql/hw
i+rV4kixsHkNyDFTuEQtqzVMfPt6HZoajivxqGSKzRt4WCVapGNcSlOPiKuRPuFaidqV2F9S2Gxd
V4pflj5jrX4lWcd9khZr4uBCoKOwhwSbB9WPuI1gO/xUCqBPFkKtlPK7Gm4UE6+VScbgLpyLQTXG
DRBidKc6bjBQfOth8c2fSFQvBcZWMcdp+ASuxq+lhlguaedGobDTxd/L8/UZ4koXFM+5D7tXkeNA
occzawlb4EgJqNPqjJslm7lH6UJXZrWD5M4XhVquLq6PEMuYKSb9jETntVKdvSqBb3up3t6Gp8WK
rscfsECquSeEOB1dfF86USIx54pi3HH4R52FDZaiaL3UBF11snbHj/B5KftrYmfx10USOE4U6nm/
tMv2KgQdJOZ1gphXiwSzLUSRc8RC1kosWSkR/3Fxg2qbMFuUO6mekGU4pJBvVhdzkhi3HB9tdNJ9
RQlrJIKeLaSk2gNqNUW3xhGy8Ffwb1l8NV9sE4Ut0FxTdhCu03UaJxv1EE4pXeiK8JR/3yrm2VJM
0J2+XYvOh6jV5mkREx4pabGZpuy/cBbeKn2ChlBLw7sIMSoxuX2ke3uwkIprJEVtqBC0tZi3TXL8
QbKocbI7rxV/JRJ0kQ7/H4EPC19Y2ugiyvCoxK45xbPWVnw/pHjWi/UmGos7hBOMLru+rdDKRdI3
WC1md44ElfWSly8Xbv+8mP15OvL8AskcX5TAOUOHNZwosaHRgqwaRsu5xlk6W/juxXc1M8tmsvMP
q053WyR/T5HAd5Uo5HGxhrukqjtTdn+8jRnePEl9pWryfjHbMVKLnKUzI2wWp0idUKnITmmwGqaL
6RzR5EMbPRk6QFpbEyWLrJTuzTzZnd7oDI8UN/2ODnLUEBEaJvn0OuHhzaBRBYzWUXUSEvOMWNCp
TT6zK7TIOcNa2VDivveJ6/0PlUFwN7yzuLlbbeYGsF5cZobs/C0SvCZLMXZZLz2n1Gr7ipTu+wgv
WVg5qBxHC1vrTgRuFC9LkDyieN7FEgemiAX25vHYrdIZHi9Z58c27iNuhKFCa2s1RGqh2dPhbwod
HqWjIdJIN6oZDJB0PFfI3OTKAeUL3VX8fonmO8PdwUWisEtF0GXSvupNvCIF0unF8+6tHFCugNHF
59WVg/oIT0rknyTFyRqh2b2JqVK3/ArfrzagPAaUWsdP97IQtXC/1ATnS+5/rJfmLbXF5wsfmCuk
qxPKFdBe3LihGw8cKuRigubfEhko9PUYscKpkq66+3LGUKkFTpV0e4YuFl96eAklv28V32kUIyS3
zpCgtoswwbubmOMmidZnSMeodDTWzEsarUKyTpJW2lVSvTZ8NPaCFArD1NBYGVqkZ3CFFBlHSPw4
U4LZYuH8jRy1D5Be4z3yat6x0gW+WXjCKinHn5dSulXY3FBpvuwpFjRG3OpQ3TjmnyFUtCZPLsP+
kjHusHFDcyvpzz2DT6vTfyswSDpGVxafd8TxUle8IHXJYmnPLRCmulCO0tbLwccVeLdsRrcwUVLR
0XXGtUtH5Skpiat1iQfIgcVjQj7q9e2HSWl9fJXvhsuOzhPSdLUo6hzpSU5WuxnaMNpEy6WIXA2D
xcTXib9uVWO+Eh9fip9LxdcVy5ssZ/5Tmpa6l3Gy8PJRVb7bQRqKj0gBU6tVXY4Jwv1XSGFSeV+L
HH3dpsFT3L7ERCEk5cdQLbIzt4rPH6jxxZewvRQ5q4u5y+PCbmIlXTVI+xVt4lv3yI63yq49Kq/O
TezB3JtLnbFOgmTpeO1c6Qm87rtfwjjJBhfJOfwqoZMje2HuNnkRc4WY/QGS3ioPPl93zBHBTpOs
0CtRtkAr3iER/E5Jf7WC6euCdmF33WmNNYIx0vu/3yZk+pUYLBXUGnmDtKeNyhJ2krT4gFjCJo3B
YgnPCgkZ24O5NpfzgVXyPsBOPZaun9COj0g7eYm0wrfTmEW0Cl8/UAqktRJc653l9xuaybtj5eDi
cKkcF0rZulSaG88V17fQ8bO5KdJknSos8xJpV28yP5vrzg8nd5De3SwphUtnfqX5Xiv7e0nK4gU2
0V+R9oR5bSZWUfrp7JaijA0SM9ZJvn+hhzK+gTfQh/gvVuQ/B95JoKcAAAAASUVORK5CYII=','/wlb_originals/wdg_ElFigure',0,'','',-1,'name;dscr;lineWdth;lineClr;elLst;',1500037083);
INSERT INTO "wlb_ElectroEls" VALUES('reactor','iVBORw0KGgoAAAANSUhEUgAAAEAAAABACAYAAACqaXHeAAAABHNCSVQICAgIfAhkiAAAAAlwSFlz
AAAOxAAADsQBlSsOGwAAA9VJREFUeJzt2t2LVlUUx/GPOs1I2iSlaVlpCb3q4EUlCV3UEPTiRaQJ
XfQHFJQ31V8gXdVlUATepRSBF0FXWmBlDWSUJUKQI2U0ZVChE804znTxOwPxWI+eZ848Z2Y836vn
OZy999rrrL32WmtvGhoaGhoaGhoaGi5LFtUwZh/WYBVuxxS+w2mMYKybwnRDActxK7ZgEBuLZ324
qnjnjEz8DL7FQQxhGGe7IOOs0IOd2ItfMSlf+xx+lAmelEkOFc/OFe9MFm324qmir3nDUmzDR/gL
4ziKPdiBdVghVrACV//r93qZ8J6izXjRx4d4XKxmTtOPV/CbfMljeF6WwOIS/Swp2ryA40Vfp7G7
GGNOMoCPRdgTeBG9FfTbi5dkqUzhEDZV0G+l3I0vxWQ/wf3yFatiCbbicDHGEdxVYf8zYkAmP4l9
uG4Wx1qNd4qxjpgDltAvZj8uk7+hC2OuFSWMy3KozScsFYc3JWY/m1++ldWyHKbEMdayO2wTb39C
1ny32SqO8TQe6/bgPbLPT4m3r9LhlZHh5UKGg7ocLO2UAOWYara6TumVOGEU2zvpoExwMs1yPCEa
f0OcUV2MFzJcgSdFtllnQOL0oxKt1c0GfINfJNEqRScWsAUr8YUkM3VzUuKQVbivbONOFDAoafQH
EpDUzXmRZZHINqv0Sb5+TrK6ucItmJBlWcopl7WANeJoRvBnybazye/iA/olSLpkWvfOG7WPqjbi
SvyMm3FtmcH+h+8r6OO8yHQTNmtvBWM4Nf2ntSS2D3e2abxMihZ/SxRWhQ/YXEEfy/A+HhCnONrm
3eN4evpPJ06wKvpwG76SrWwmjOIhvKf95C+g1QIuZQm8Jeb2TNnBWliH18Ukt6tmKVxMflqWQKsP
OKU9ExICX4Mf8EdJAf9LmCq5mPwXUHYJjEjpeo0UMec9ZRUwJnFAD+6tXpzu04kTPCgp6KPqSYMr
pRMFDEkh5B5zKxrsiE4UMIwDuEOqQvOaThRwFvtlR3hWvQWRGdNpILQfn4sV7DKPfUGnCpjAa+IL
npMawbxkJqHwAbwpucGrSmZhC4V+OZwYl8OKtSXabpA8oIpcoFY2yTHVpCjhUi1hwSiAHFQeEUs4
LIcWF6vTLygFEEs4JFHisBxatNsiF5wCiE/YLcdVU1J82CUTbN0q54QCqt6/x/Apvsb1cm/gETws
Jes+SafJYeoOUdS7UtdbUPRIoeNtKVhOX5KawE+SVZ5VswV065rcerGAQfEV/eIfFuNBOWHu6v3A
aeq4KNkrW+VKuWPwWQ0yNDQ0NDQ0NDQ0NFzu/AOlStFDLDObwgAAAABJRU5ErkJggg==','/wlb_originals/wdg_ElFigure',0,'','',-1,'name;dscr;lineWdth;lineClr;elLst;',1500037083);
INSERT INTO "wlb_ElectroEls" VALUES('srcAC','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','/wlb_originals/wdg_ElFigure',0,'','',-1,'owner;name;dscr;geomW;geomH;lineWdth;lineClr;elLst;',1500037083);
INSERT INTO "wlb_ElectroEls" VALUES('stGen','iVBORw0KGgoAAAANSUhEUgAAAEAAAABACAYAAACqaXHeAAAABHNCSVQICAgIfAhkiAAAAAlwSFlz
AAAOxAAADsQBlSsOGwAAANRJREFUeJzt27sRgkAYReGLI6mtWAFtWYTd0YopgTbg8pu4h8f5Zkhg
g8vJN5GkExsa729J7j2HdDAnef16eEryPtgzffvRaxHimWQpzmzdmOTR+lgFWLL/AKsu9ACaAegB
NAPQA2gGoAfQDEAPoBmAHkAzAD2AZgB6AM0A9ACaAegBNAPQA2gGoAfQDEAPoBmAHkAzAD2AZgB6
AM0A9ACaAegBNAPQA2gGoAfQDEAPoBmAHkAzAD2AZgB6AK26LzB2WfFfq/9QBWjetDgKL01J0pl9
AK7hPS0VI6B9AAAAAElFTkSuQmCC','/wlb_originals/wdg_ElFigure',0,'JavaLikeCalc.JavaScript
if(var.isEVal())	c1 = "#000000-0";
else c1 = var ? "red" : "lime";','',-1,'owner;name;dscr;lineWdth;lineClr;elLst;c1;',1500037083);
INSERT INTO "wlb_ElectroEls" VALUES('stGen1','iVBORw0KGgoAAAANSUhEUgAAAEAAAABACAYAAACqaXHeAAAABHNCSVQICAgIfAhkiAAAAAlwSFlz
AAAOxAAADsQBlSsOGwAAAi1JREFUeJzt2y9o1VEYxvHPzoUJwk0u3TRBWHIgbCAzbQiaZjI5ixq2
Bf+EuXI1qGUapmUzbJaryTSTFk0mTQZhQWwGmSAIgkEwnKM3Lr6w12+48HLfy+8J5z6c3znPO4IZ
LKncwhTOtfoyFjGNPdzAGnr4gAcYtN6XeNO+hyf4gWutvocJnG/1Mi7gVOtbxh0cxW7r38IhvMYL
PGy/fYovuNnq+03TQquvYx5z+IUr6Lfnf8ZtbOAMrMjL8YLJaBWB9Iq6lLKysH/LwWYA69EqAjlS
MBatIpD5gnfRKgKZg9FoFYFsMdzIpKRECwimX7ATrSKQiQ5+4nu0kiBO8N8DUrNRsBmtIpBu9hXw
ltwekH4FbMB4sIhIBh2cxsdoJUH8LoYHoBnZze4BfXK/Dg/garSKQKaKeumRlW5Rb3yycilaQDQD
hnd5GekV9VIxK7NFveXNytloAdEM/n1kJftWeKWoyY6sTHbwTY2oZOQk/z0gNetFTXNlZazI+/+n
ZSMye8Bodg/YLmoiMy0dNaq6Gy0kiA65PWA8uwfchW60ikAG1Dh8VmaKaoJpyb4TXNq/5WAzoI6p
ZGW8qDM6WZkq8u4CyZ2NQPOArWgVgYwWdS4vK4tFHUrMynRRJzKzskfu1+H0x+JrRR1Ezkqvg8P4
Gq0kiGMj+IRXYsbXu2pk/Vmr4Xl7fr/Vj1rf30DTKmYNww0X1Qn4yaZpVZ2GHVPP/R9ju/Xu4L12
EoTNP/ULaGtRSJXgAAAAAElFTkSuQmCC','/wlb_originals/wdg_Box',0,'JavaLikeCalc.JavaScript
if(var.isEVal())	bordColor = "black";
else bordColor = var ? "red" : "lime";','',-1,'owner;name;dscr;backColor;bordWidth;bordColor;bordStyle;',1500037083);
INSERT INTO "wlb_ElectroEls" VALUES('trDuo','iVBORw0KGgoAAAANSUhEUgAAAEAAAABACAYAAACqaXHeAAAABHNCSVQICAgIfAhkiAAAAAlwSFlz
AAAOxAAADsQBlSsOGwAABUhJREFUeJzt223MlmMYB/Df81QS6hn1sEilIg35QE/IW2NKL8PYzIYP
lhLzMsPGNJ+8zfhCZpMxCWEzfDDT8jaUly8lqdATZaSMCmPEh+O8Vc8j9TzndV333fTf7g/3fV/X
/39ex3We53Eex3Ge7MX/G00V6+2HIzEEB6fv8AvWYw1Wpe+VoAoD9MEoTMUE9E26tQ/8td1nM17D
y/gMv5bZuLIN0IZpuBCr8So+xgqsw5Z03QE4DCNxAs7FEXgBc/Bhye0sHANwC9rxBial33rsxr09
0rWT8WbiuAn9i29mOWjF02JMXyuv4QNwXeJ6Kn1vaAzHQryPUxQzxJowDouwAMMK4CwFLXgGS3Fi
CfxjsAzz0K8E/iz0wn1ichtTok5b0rg3aTYMxolxOk25nqUJ05PWySXqdAn98BLmq6ZrtuB5vCjW
FHXHVOGqTq9Q88ykOblCzZ3iCeHrmyvUbMbbeKxCzX9FH2zEFXXQnobvsW8OSe5bG43fheurGkux
FcfmkOQaoA1finV91ViLLzA2hyTXAEOwAT9l8nQHP4nhNziHJNcA/UVEV1n8vh1+EaFzVnyQa4Ae
tsXxVaOmuzsR5k6Ra4At6I19Mnm6g32EB9icQ5JrgLU4SH2Ck35Je20OSa4BluNQ9YnTW5P28jpo
/4ODRRecWgft8zTAJLhexOgTMnm6g4lYItxwXXGVGIeDKtQcLBZf03OJighgFuBbEQ/0LIBvV+iV
tNYl7YbA1fhaudmgGsaKHjejCLKisjcH4lkRnFym87jsiUMwQmRyjsNQkdwglrXtYkwvwuf4Dn90
4GnFXLEAugQ/5ja8yPTVGNG4hSKPX1seD8OVOEtUiFaKSK4dm9I1LSKuGC1KZ8tF935UFFRgfzyA
M3ApPiqw7YWgSczM7ZiN40Xysh3v4lbxcH3FKm77+ac5/dY3XXNbumc17klcj6Tv56i+prnb6IkL
xAS1WozVmzCwG1wDcXPiqHGdr+CJtow0Vo1zq3hTzbr+xmqF09q9W7f7vWHRJIqa7XhITHR3ibf3
Hm7HUaKb99Z5CPRO/43ELFFdWo07E9fsxD1RgYYo0qJtYhJcILpubRIcKvJ3Z+MYkcVZIvYC1BIp
Lem640R5bVnimSMemthLcD/GC09TSMW4SDc4X7ity3V2gz2EGxyOk8RsP0RnN7hUuMEvhBv8swNP
K54UPeZiBbjBonANvlJOTbAj2sSEOLMCrd3CCOGTZ6lmKdwTd4ghMLwCvV1iplgGH1ah5uGiF2QH
Q0VgMR6sg+7DwrvUFf/7hMg40RVXZfJ0ByvxTWpDt5FrgFGpEfXIymwQS+6jc0hyDTAIP9gW1VWJ
TUk7KxOVa4AD8JsokFaN35N21iaJXAP8accdn1WiOX06rha7TJKDjeIN7LerC0tAH9EDs+afXAOs
EQXSll1dWAJahAtck0OSa4DFYjla5SqwhkEi3bY4hyTXAEvE2nx0Jk93MFq0/5M6aO+Ax8Wm5qo3
Sb0j8gV1xxTVb5MbnzQnVai5U9Q2Sj6nmsmwRZwjaJiNksTO8PUiPC17q+wMkS06qUSdLqOXqAGs
EBmbsjBWBEF3a7DN0sRQmCcSmmXUCNvwqTg40XDb5WsYJrK5i3GqYoZDM07DB3hdnCVqaAwQb+l7
XC8vYdGKGxLXXHvQuaH+oiTWLtYIU3Tt0FSryDK9lThuVNLDlx3FnSiKIheJtHnHY3O1LW59dT42
N1icC5iT7ikFVR2cHCne6EQxgf3XwclNwlCviBl/jz442RF9RB1hqB2Pzv5s29HZz5X80HuxF9vw
N//VD9fPmVdiAAAAAElFTkSuQmCC','/wlb_originals/wdg_ElFigure',0,'','',-1,'name;dscr;lineWdth;lineClr;elLst;',1500037083);
INSERT INTO "wlb_ElectroEls" VALUES('trStarTriangl','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','/wlb_originals/wdg_ElFigure',0,'','',-1,'owner;name;dscr;lineWdth;lineClr;elLst;',1500037083);
INSERT INTO "wlb_ElectroEls" VALUES('trTrianglStar','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','/wlb_originals/wdg_ElFigure',0,'','',-1,'owner;name;dscr;lineWdth;lineClr;elLst;',1500037083);
INSERT INTO "wlb_ElectroEls" VALUES('trTrianglStarH','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
rkJggg==','/wlb_originals/wdg_ElFigure',0,'','',-1,'name;dscr;lineWdth;lineClr;elLst;',1500037083);
INSERT INTO "wlb_ElectroEls" VALUES('trTrio','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','/wlb_originals/wdg_ElFigure',0,'','',-1,'name;dscr;lineWdth;lineClr;elLst;',1500037083);
INSERT INTO "wlb_ElectroEls" VALUES('triangle','iVBORw0KGgoAAAANSUhEUgAAAEAAAABACAYAAACqaXHeAAAABHNCSVQICAgIfAhkiAAAAAlwSFlz
AAAOxAAADsQBlSsOGwAABk1JREFUeJztmm1oVFcax393Xk1MGm0ydqNV6+LLYBPcrUQiiFCKr5TU
FoN0W4lKRXFpCWYVF0GKCMvWWigVsULfNAahxaZC0yDsthvmQ9f6wbeYpmmM1Y1KY5uZ0Lz23vP0
w51js9Ykk5iZk9j5wYX5Mtzn/7/POec5zzmQJk2aNL9jLIPvngRkxX//BEQNxmKEV4Evgf8Ce8yG
knoWAj8CEn9+AP5sNKIUkgW8Czgej0c8Ho8ADvAOvw6JB5pngS5A1q1bJ6WlpToLOoFnjEaWAiYD
dYDMnDlTIpGI1NXVyYwZM7QJ/8GdHB9IvEA50A3Inj17xLZtsW1bdu/erQ3oAl4BPAbjTBp/BJoA
FQ6H5caNG6JpbW2VefPmCaCAb4BZRiNNAh7gdUAyMzPl0KFDYtv2HQNs25aDBw9KRkaGzoTXeMCy
4CmgFZCVK1dKNBqVu4lGo7J8+XJtwP+AJ41GPIr4gJOAysjIkNra2t+I19TU1MiECRP0UPgw/t9x
z1pAeTwe2bZtm3R1dQ1oQGdnp2zdulXXBgp4znDs982jwBeATJ06Vc6fPz+geBERpZScO3dO8vPz
9VD4NzDNpID7ZTfws8fjkX379g0qvj979+7VWdAH/N2whhHzGHADkOLiYrly5UrCBjQ3N8uiRYt0
FrQCM00KGQkZwFuA4/P5pKqqSpRSCRuglJLKykrx+Xx6n/AmMMGoomGyDLgNyIoVK6S3tzdh8Zre
3l5ZtmyZngzbcJfScUEA+AyQUCg06LI3FDU1NZKXl6eHwqeA36iyBPAAL+Hu7GT79u0j+vqanp4e
KS8v779b3MQYrxDzgTOACoVC0tLSMmLxmubmZp0FCrd7lG9U4SBYwF5AgsGgHDhwQBzHuW8DHMeR
/fv3SzAY1JnwKmZ7mQPyBPAtIIsXL5bW1tb7Fq+5fv26FBcXawOagD8ZVXoPgsD7gOP3+6W6unrU
xGtOnjwpfr9fL4vvxt85ZngK6LQsS0pLSyUWi426AbFYTNauXSuWZekJcczsFnOBGuLLXiQSGVbR
kyhKKamrq5NQKNR/WXzYqPI4LwNdlmXJjh07/q/RMdrYti0VFRU6C7qAvxrWTjbQAsicOXPk8uXL
SROvqa+vl9mzZ+ssaMZgKz0A/BOQQCAgR44cGZVlbygcx5HDhw9LIBDQJvwjHkvKWQRcBaSoqCgp
E99ARKNRWbhwoTagBShKtXgvbptLJk2aJKdOnUqZeE11dbXk5ORoEz6Kx5QSLKAE6ACkrKxMOjo6
Um5ALBaT9evXawM6gKdJUYU4BfgcUFlZWdLY2Jhy8ZqGhgaZOHGi3if8Kx5b0ikHfvb5fLJr166k
rPmJopSSnTt36sZJH+6pUlKZBTQAUlhYKE1NTcbEaxobG6WgoEAPhcu4rbikEMRtc9ler1cqKyuN
fn2NUkqOHj0qXq9XABu3fZaUfcITxNtcS5culfb2dtPa79De3i5LlizRWdBGEnaL2cAngOTl5cnp
06fHxNfXKKWktrZWcnNztQkfM8oV4vO4l5hk48aN0t3dbVrzb+ju7paysjJtQDuwbrTE5wAXAJk+
fbpcunTJtNYBuXDhgkybNk2bcA54aChxQ1VPXmAb8ILf77cqKiooKSnB601Z0TUsJk+eTE9PD5FI
BKXUFNz54CtcQ0bE48DXgBQUFMitW7dMf+QhuXnzpsyfP19nQQMwf6TiPcB7gJOZmSnHjx8fUxPf
QCil5NixY/qyhb59NqISuYh4vb969Wq5ffu2aW0J09bWJqtWrdJZEMO9lzgscnFPd1R2dracPXvW
tKZhc+bMGcnOztb7hE/jmhLmReJNzk2bNqWk0THaOI4jGzZs6N9E/Uui4v8A1AMSDoelvr7etJYR
c/HiRX37TICLwCN3i737nM0PbAZmW5bFli1bmDt37nAyZ0wRDofZvHkzlmUBzMU9txz03lEhcB2Q
BQsW3PM213gjGo1KYWGhzoJruEv7PQkCHwAqJydHTpw4MS6WvaFQSklVVZVunykGOVVagVs5yZo1
a4y0uZJFLBaTkpISnQXf417gAH4tEALAG7jjP+Dz+QgEjHSak0ZfXx+2bYPbOXob+Fv89x0eI97t
ecCfBvpduuo/I14FjgEzErJ0/HIN+M50EGnSpEmTZizwCyo7DkKehJ9GAAAAAElFTkSuQmCC','/wlb_originals/wdg_ElFigure',0,'','',-1,'name;dscr;geomX;geomY;geomZ;lineWdth;fillColor;elLst;',1500037083);
CREATE TABLE 'wlb_ElectroEls_uio' ("IDW" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"IDC" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"IO_TYPE" INTEGER DEFAULT '0' ,"IO_VAL" TEXT DEFAULT '' ,"ru#IO_VAL" TEXT DEFAULT '' ,"SELF_FLG" INTEGER DEFAULT '0' ,"CFG_TMPL" TEXT DEFAULT '' ,"ru#CFG_TMPL" TEXT DEFAULT '' ,"CFG_VAL" TEXT DEFAULT '' ,"ru#CFG_VAL" TEXT DEFAULT '' , PRIMARY KEY ("IDW","ID","IDC"));
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_KeySqr_1','DESCR','','Описание','Описание',131077,NULL,NULL,10,'Parameter|DESCR','Parameter|DESCR','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_KeySqr_1','errPresent','','Наличие ошибки','Наличие ошибки',131072,NULL,'',8,'','','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_KeySqr_1','st','','Статус ошибки','Статус ошибки',131072,NULL,'',10,'Parameter|st','Parameter|st','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_KeySqr_1','val','','Значение','Значение',131072,NULL,'',10,'Parameter|var','Parameter|var','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_KeySqr_1_tst','val','','�������','�������',131072,NULL,'',10,'��������|val','��������|val','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_KeySqr_2','DESCR','','Описание','Описание',131077,NULL,NULL,10,'Parameter|DESCR','Parameter|DESCR','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_KeySqr_2','errPresent','','Наличие ошибки','Наличие ошибки',131072,NULL,'',8,'','','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_KeySqr_2','st','','Статус ошибки','Статус ошибки',131072,NULL,'',10,'Parameter|st','Parameter|st','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_KeySqr_2','val','','Значение','Значение',131072,NULL,'',10,'Parameter|var','Parameter|var','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_KeySqr_2_tst','val','','�������','�������',131072,NULL,'',10,'��������|val','��������|val','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_KeySqr_3','DESCR','','Описание','Описание',131077,NULL,NULL,10,'Parameter|DESCR','Parameter|DESCR','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_KeySqr_3','errPresent','','Наличие ошибки','Наличие ошибки',131072,NULL,'',8,'','','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_KeySqr_3','st','','Статус ошибки','Статус ошибки',131072,NULL,'',10,'Parameter|st','Parameter|st','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_KeySqr_3','val','','Значение','Значение',131072,NULL,'',10,'Parameter|var','Parameter|var','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_KeySqr_3_tst','val','','�������','�������',131072,NULL,'',10,'��������|val','��������|val','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_KeySqr_4','DESCR','','Описание','Описание',131077,NULL,NULL,10,'Parameter|DESCR','Parameter|DESCR','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_KeySqr_4','errPresent','','Наличие ошибки','Наличие ошибки',131072,NULL,'',8,'','','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_KeySqr_4','st','','Статус ошибки','Статус ошибки',131072,NULL,'',10,'Parameter|st','Parameter|st','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_KeySqr_4','val','','Значение','Значение',131072,NULL,'',10,'Parameter|var','Parameter|var','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_KeySqr_4_tst','val','','�������','�������',131072,NULL,'',10,'��������|val','��������|val','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_KeySqr_5','st','','Статус ошибки','Статус ошибки',131072,NULL,'',10,'Parameter|st','Parameter|st','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_KeySqr_5','val','','Значение','Значение',131072,NULL,'',10,'Parameter|var','Parameter|var','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_KeySqr_5','val1','','Значение 1','Значение 1',131072,NULL,'',10,'Parameter1|var','Parameter1|var','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_KeySqr_5_tst','val','','�������','�������',131072,NULL,'',10,'��������|val','��������|val','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_KeySqr_6','DESCR','','Описание','Описание',131077,NULL,NULL,10,'Parameter|DESCR','Parameter|DESCR','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_KeySqr_6','errPresent','','Наличие ошибки','Наличие ошибки',131072,NULL,'',8,'','','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_KeySqr_6','st','','Статус ошибки','Статус ошибки',131072,NULL,'',10,'Parameter|st','Parameter|st','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_KeySqr_6','val','','Значение','Значение',131072,NULL,'',10,'Parameter|var','Parameter|var','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_KeySqr_6_tst','val','','�������','�������',131072,NULL,'',10,'��������|val','��������|val','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_Key_1','val','','Значение','Значение',131072,NULL,'',10,'Parameter|val','Parameter|val','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_Key_1_tst','val','','�������','�������',131072,NULL,'',10,'��������|val','��������|val','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_Key_2','val','','Значение','Значение',131072,NULL,'',10,'Parameter|val','Parameter|val','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_Key_2_tst','val','','�������','�������',131072,NULL,'',10,'��������|val','��������|val','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_Key_3','val','','Значение','Значение',131072,NULL,'',10,'Parameter|val','Parameter|val','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_Key_3_tst','val','','�������','�������',131072,NULL,'',10,'��������|val','��������|val','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('El_Key_h','val','','Значение','Значение',131072,NULL,'',10,'Parameter|val','Parameter|val','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('key','var','','Переменная','Переменная',131072,NULL,'',10,'Параметр|var','Параметр|var','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('stGen','var','','Состояние','Состояние',131072,'0','',10,'Parameter|var','Parameter|var','','');
INSERT INTO "wlb_ElectroEls_uio" VALUES('stGen1','var','','Состояние','Состояние',131072,'0','',10,'Parameter|var','Parameter|var','','');
COMMIT;
