PRAGMA foreign_keys=OFF;
BEGIN TRANSACTION;
CREATE TABLE 'BlckCalcBlcks_boiler9_io' ("BLK_ID" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"TLNK" INTEGER DEFAULT '' ,"LNK" TEXT DEFAULT '' ,"VAL" TEXT DEFAULT '' , PRIMARY KEY ("BLK_ID","ID"));
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SrcGBF','Pi',0,'','1.5');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SrcGBF','Fo',1,'Node3.Fi','113.424982505404');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SrcGBF','Po',0,'','1.42728478645261');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SrcGBF','So',0,'','1.13');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SrcGBF','lo',0,'','500');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SrcGBF','Noise',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SrcGBF','Kpr',0,'','0.95');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SrcGBF','Q0',0,'','1.2');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SrcGBF','f_frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('7FW','Fi',0,'','4.94065645841247e-321');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('7FW','Pi',1,'Node6.Po1','36.1866253607701');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('7FW','Ti',1,'3FW.To','292.415005104271');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('7FW','Fo',1,'DS.Fi1','-4.03600489991491e-06');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('7FW','Po',0,'','35.8279341224978');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('7FW','To',0,'','273.011380780064');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('7FW','So',0,'','0.00785');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('7FW','lo',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('7FW','S_v1',0,'','0.004');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('7FW','S_v2',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('7FW','l_v1',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('7FW','l_v2',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('7FW','Q0',0,'','1000');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('7FW','Kpr',0,'','0.001');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('7FW','Ct',0,'','20');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('7FW','Riz',0,'','20');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('7FW','Fwind',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('7FW','Twind',0,'','273');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('7FW','frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('7FW','Ftmp',0,'','0.434584654749449');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SrcGC','Pi',0,'','1.3');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SrcGC','Fo',1,'3GC.Fi','6.15434801160656');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SrcGC','Po',0,'','1.08637358580754');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SrcGC','So',0,'','0.1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SrcGC','lo',0,'','2000');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SrcGC','Noise',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SrcGC','Kpr',0,'','0.95');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SrcGC','Q0',0,'','1.33');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SrcGC','f_frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node5','Fi',0,'','143.860762359486');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node5','Pi',1,'AH2.Po2','1.15986558048338');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node5','Fo1',1,'11G.Fi','71.930381179739');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node5','Po1',0,'','1.14890575918545');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node5','So1',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node5','lo1',0,'','40');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node5','Fo2',1,'13G.Fi','71.930381179739');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node5','Po2',0,'','1.14890575918545');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node5','So2',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node5','lo2',0,'','40');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node5','Kpr',0,'','0.95');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node5','Q0',0,'','1.293');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node5','f_frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node5','F1tmp',0,'','71.930381179743');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node5','F2tmp',0,'','71.930381179743');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node6','Fi',0,'','120.002135818003');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node6','Pi',1,'3FW.Po','36.1866254785578');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node6','Fo1',1,'7FW.Fi','4.94065645841247e-321');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node6','Po1',0,'','36.1866251895236');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node6','So1',0,'','0.017');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node6','lo1',0,'','30');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node6','Fo2',1,'5FW.Fi','120.002026859932');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node6','Po2',0,'','36.0653165845839');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node6','So2',0,'','0.017');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node6','lo2',0,'','30');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node6','Kpr',0,'','0.001');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node6','Q0',0,'','1000');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node6','f_frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node6','F1tmp',0,'','0.000108128428309769');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node6','F2tmp',0,'','120.002027689575');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node7','Fi',0,'','120.002031235172');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node7','Pi',1,'5FW.Po','35.9571177450713');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node7','Fo1',1,'Node8.Fi3','87.5518635330187');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node7','Po1',0,'','35.8925401446013');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node7','So1',0,'','0.017');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node7','lo1',0,'','15');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node7','Fo2',1,'EC1.Fi2','32.4497532527025');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node7','Po2',0,'','35.9482462377058');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node7','So2',0,'','0.017');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node7','lo2',0,'','20');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node7','Kpr',0,'','0.001');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node7','Q0',0,'','1000');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node7','f_frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node7','F1tmp',0,'','87.5522733213883');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node7','F2tmp',0,'','32.4497579137841');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node8','Fi1',0,'','-0.00209211876927915');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node8','Pi1',1,'DS.Po1','35.8280051154174');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node8','Ti1',1,'DS.To1','511.321634430079');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node8','Si1',0,'','0.00785');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node8','Fi2',0,'','32.4493462979837');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node8','Pi2',1,'EC2.Po2','35.836851159152');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node8','Ti2',1,'EC2.To2','672.935896907612');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node8','Si2',0,'','0.017');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node8','Fi3',0,'','87.5519919890019');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node8','Pi3',1,'Node7.Po1','35.8925416011274');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node8','Ti3',1,'5FW.To','292.381283068664');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node8','Si3',0,'','0.017');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node8','Fo',1,'Drum.Fi1','120.002024811318');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node8','Po',0,'','35.8279264357544');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node8','To',0,'','395.185364507819');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node8','So',0,'','0.017');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node8','lo',0,'','5');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node8','Kpr',0,'','0.001');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node8','Q0',0,'','1000');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node8','Ct',0,'','5');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node8','Riz',0,'','20');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node8','Fwind',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node8','Twind',0,'','273');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node8','f_frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3FW','Fi',0,'','120.002026810281');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3FW','Pi',1,'SrcWater.Po','39.8790647186307');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3FW','Ti',0,'','293');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3FW','Fo',1,'Node6.Fi','120.002092034578');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3FW','Po',0,'','36.1866254793117');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3FW','To',0,'','292.415005104323');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3FW','So',0,'','0.017');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3FW','lo',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3FW','S_v1',0,'','0.004');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3FW','S_v2',0,'','0.004');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3FW','l_v1',0,'','38.4434355630288');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3FW','l_v2',1,'3FW.l_v1','38.4434355630288');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3FW','Q0',0,'','1000');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3FW','Kpr',0,'','0.001');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3FW','Ct',0,'','20');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3FW','Riz',0,'','20');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3FW','Fwind',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3FW','Twind',0,'','273');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3FW','frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3FW','Ftmp',0,'','7.12371896551218');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('OhS','Fi1',0,'','263.497024756568');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('OhS','Pi1',1,'Drum.Po2','0.978512871822965');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('OhS','Ti1',1,'Drum.To2','864.541239434409');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('OhS','Fi2',0,'','120.002026543416');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('OhS','Pi2',1,'DS.Po2','33.6309424600753');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('OhS','Ti2',1,'DS.To2','511.325293230747');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('OhS','ki',0,'','0.9');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('OhS','li1',0,'','30');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('OhS','li2',0,'','120');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('OhS','Si1',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('OhS','Si2',0,'','0.5');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('OhS','Kpr1',0,'','0.95');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('OhS','Q0i1',0,'','1.2');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('OhS','Ci1',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('OhS','Kpr2',0,'','0.95');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('OhS','Q0i2',0,'','0.8');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('OhS','Ci2',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('OhS','f_frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('OhS','lo1',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('OhS','lo2',0,'','50');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('OhS','Po1',0,'','0.976910420083689');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('OhS','Po2',0,'','33.625174674332');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('OhS','To1',0,'','761.854605126456');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('OhS','To2',0,'','745.860834468199');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('OhS','Fo1',1,'EC2.Fi1','263.497024756568');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('OhS','Fo2',1,'4GN3.Fi','120.002026544367');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('OhS','So1',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('OhS','So2',0,'','0.5');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node10','Fi',0,'','263.497024756573');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node10','Pi',1,'AH1.Po1','0.970701528170671');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node10','Fo1',1,'1G.Fi','131.748512378291');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node10','Po1',0,'','0.968578394499684');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node10','So1',0,'','5');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node10','lo1',0,'','50');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node10','Fo2',1,'2G.Fi','131.748512378291');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node10','Po2',0,'','0.968578394499684');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node10','So2',0,'','5');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node10','lo2',0,'','50');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node10','Kpr',0,'','0.95');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node10','Q0',0,'','1.2');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node10','f_frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node10','F1tmp',0,'','131.748512378286');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node10','F2tmp',0,'','131.748512378286');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('9GBF','Fi',0,'','56.7124912527041');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('9GBF','Pi',1,'7GBF.Po','1.41515987451424');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('9GBF','Ti',1,'7GBF.To','312.823852141096');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('9GBF','Fo',1,'Node1.Fi1','56.7124912527044');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('9GBF','Po',0,'','1.33560768055864');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('9GBF','To',0,'','312.392625705022');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('9GBF','So',0,'','0.5');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('9GBF','lo',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('9GBF','S_v1',0,'','0.25');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('9GBF','S_v2',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('9GBF','l_v1',0,'','100');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('9GBF','l_v2',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('9GBF','Q0',0,'','1.2');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('9GBF','Kpr',0,'','0.95');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('9GBF','Ct',0,'','20');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('9GBF','Riz',0,'','20');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('9GBF','Fwind',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('9GBF','Twind',0,'','273');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('9GBF','frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('9GBF','Ftmp',0,'','0.0136096764806904');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node12','Fi1',0,'','131.748512378292');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node12','Pi1',1,'SEA.Po','1.0030724574977');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node12','Ti1',1,'SEA.To','479.69204625348');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node12','Si1',0,'','5');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node12','Fi2',0,'','131.748512378292');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node12','Pi2',1,'SEB.Po','1.0030724574977');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node12','Ti2',1,'SEB.To','479.69204625348');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node12','Si2',0,'','5');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node12','Fo',1,'Atmosph.Fi','263.497024756588');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node12','Po',0,'','1.00156492196041');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node12','To',0,'','479.613635317614');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node12','So',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node12','lo',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node12','Kpr',0,'','0.95');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node12','Q0',0,'','1.2');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node12','Ct',0,'','20');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node12','Riz',0,'','20');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node12','Fwind',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node12','Twind',0,'','273');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node12','f_frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3GC','Fi',0,'','6.15434801160656');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3GC','Pi',1,'SrcGC.Po','1.08637358580754');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3GC','Ti',0,'','300');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3GC','Fo',1,'4GC.Fi','6.15434801160657');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3GC','Po',0,'','1.0793279218454');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3GC','To',0,'','299.5298873584');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3GC','So',0,'','0.1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3GC','lo',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3GC','S_v1',0,'','0.1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3GC','S_v2',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3GC','l_v1',0,'','100');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3GC','l_v2',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3GC','Q0',0,'','1.33');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3GC','Kpr',0,'','0.95');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3GC','Ct',0,'','20');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3GC','Riz',0,'','20');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3GC','Fwind',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3GC','Twind',0,'','273');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3GC','frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3GC','Ftmp',0,'','0.0221836792512248');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('2G','Fi',0,'','131.748512378291');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('2G','Pi',1,'Node10.Po2','0.968578394499684');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('2G','Ti',1,'AH1.To1','474.475561498867');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('2G','Fo',1,'SEB.Fi','131.748512378292');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('2G','Po',0,'','0.966959210122029');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('2G','To',0,'','474.306702867234');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('2G','So',0,'','5');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('2G','lo',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('2G','S_v1',0,'','5');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('2G','S_v2',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('2G','l_v1',0,'','100');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('2G','l_v2',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('2G','Q0',0,'','1.2');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('2G','Kpr',0,'','0.95');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('2G','Ct',0,'','20');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('2G','Riz',0,'','20');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('2G','Fwind',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('2G','Twind',0,'','273');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('2G','frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('2G','Ftmp',0,'','0.0150022875324919');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node1','Fi1',0,'','56.7124912527044');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node1','Pi1',1,'9GBF.Po','1.33560768055864');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node1','Ti1',1,'9GBF.To','312.392625705022');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node1','Si1',0,'','0.5');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node1','Fi2',0,'','56.7124912527045');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node1','Pi2',1,'10GBF.Po','1.33560768055864');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node1','Ti2',1,'10GBF.To','312.392625705022');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node1','Si2',0,'','0.5');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node1','Fo',1,'FireChamber.Fi1','113.424982505409');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node1','Po',0,'','1.31396295855582');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node1','To',0,'','312.357926183522');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node1','So',0,'','0.5');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node1','lo',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node1','Kpr',0,'','0.95');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node1','Q0',0,'','1.2');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node1','Ct',0,'','20');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node1','Riz',0,'','20');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node1','Fwind',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node1','Twind',0,'','273');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node1','f_frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node2','Fi1',0,'','71.9303811797381');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node2','Pi1',1,'11G.Po','1.13443407023852');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node2','Ti1',1,'11G.To','599.187753425649');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node2','Si1',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node2','Fi2',0,'','71.9303811797381');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node2','Pi2',1,'13G.Po','1.13443407023852');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node2','Ti2',1,'13G.To','599.187753425649');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node2','Si2',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node2','Fo',1,'FireChamber.Fi4','143.860762359474');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node2','Po',0,'','1.12518355884656');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node2','To',0,'','598.961162074245');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node2','So',0,'','1.766');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node2','lo',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node2','Kpr',0,'','0.95');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node2','Q0',0,'','1.293');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node2','Ct',0,'','20');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node2','Riz',0,'','20');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node2','Fwind',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node2','Twind',0,'','273');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node2','f_frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('11G','Fi',0,'','71.930381179739');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('11G','Pi',1,'Node5.Po1','1.14890575918545');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('11G','Ti',1,'AH2.To2','599.793549719148');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('11G','Fo',1,'Node2.Fi1','71.9303811797381');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('11G','Po',0,'','1.13443407023852');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('11G','To',0,'','599.187753428399');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('11G','So',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('11G','lo',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('11G','S_v1',0,'','0.7');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('11G','S_v2',0,'','0.3');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('11G','l_v1',0,'','80');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('11G','l_v2',0,'','80');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('11G','Q0',0,'','1.293');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('11G','Kpr',0,'','0.95');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('11G','Ct',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('11G','Riz',0,'','20');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('11G','Fwind',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('11G','Twind',0,'','273');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('11G','frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('11G','Ftmp',0,'','0.0112515005317738');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node3','Fi',0,'','113.424982505404');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node3','Pi',1,'SrcGBF.Po','1.42728478645261');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node3','Fo1',1,'6GBF.Fi','56.7124912527033');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node3','Po1',0,'','1.42322091992014');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node3','So1',0,'','1.13');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node3','lo1',0,'','15');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node3','Fo2',1,'5GBF.Fi','56.7124912527033');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node3','Po2',0,'','1.42322091992014');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node3','So2',0,'','1.13');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node3','lo2',0,'','15');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node3','Kpr',0,'','0.95');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node3','Q0',0,'','1.2');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node3','f_frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node3','F1tmp',0,'','56.7124912527018');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node3','F2tmp',0,'','56.7124912527018');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node4','Fi',0,'','143.860762359487');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node4','Pi',1,'SrcAir.Po','0.988854896346954');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node4','Fo1',1,'BFA.Fi','71.9303811797411');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node4','Po1',0,'','0.984357975459145');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node4','So1',0,'','1.766');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node4','lo1',0,'','50');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node4','Fo2',1,'BFB.Fi','71.9303811797411');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node4','Po2',0,'','0.984357975459145');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node4','So2',0,'','1.766');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node4','lo2',0,'','50');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node4','Kpr',0,'','0.95');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node4','Q0',0,'','1.293');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node4','f_frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node4','F1tmp',0,'','71.9303811797437');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node4','F2tmp',0,'','71.9303811797437');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node9','Fi1',0,'','71.9303811797444');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node9','Pi1',1,'BFA.Po','1.18622985347462');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node9','Ti1',1,'BFA.To','405.008513768293');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node9','Si1',0,'','1.766');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node9','Fi2',0,'','71.9303811797444');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node9','Pi2',1,'BFB.Po','1.18622985347462');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node9','Ti2',1,'BFB.To','405.008513768293');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node9','Si2',0,'','1.766');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node9','Fo',1,'AH1.Fi2','143.860762359488');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node9','Po',0,'','1.18332082850928');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node9','To',0,'','404.916816203185');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node9','So',0,'','1.766');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node9','lo',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node9','Kpr',0,'','0.95');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node9','Q0',0,'','1.293');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node9','Ct',0,'','20');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node9','Riz',0,'','20');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node9','Fwind',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node9','Twind',0,'','273');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node9','f_frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3GN','Fi',0,'','0.0569318800252727');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3GN','Pi',1,'SrcNG.Po','1.44999438660746');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3GN','Ti',0,'','297');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3GN','Fo',1,'4GN.Fi','0.0569318800253092');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3GN','Po',0,'','1.44999018997985');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3GN','To',0,'','281.706734035223');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3GN','So',0,'','0.05');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3GN','lo',0,'','30');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3GN','S_v1',0,'','0.05');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3GN','S_v2',0,'','0.05');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3GN','l_v1',0,'','100');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3GN','l_v2',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3GN','Q0',0,'','0.7');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3GN','Kpr',0,'','0.95');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3GN','Ct',0,'','20');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3GN','Riz',0,'','20');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3GN','Fwind',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3GN','Twind',0,'','273');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3GN','frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3GN','Ftmp',0,'','0.0176116372043295');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GC','Fi',0,'','6.15434801160657');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GC','Pi',1,'3GC.Po','1.0793279218454');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GC','Ti',1,'3GC.To','299.5298873584');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GC','Fo',1,'5GC.Fi','6.15434801160667');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GC','Po',0,'','1.07223576711843');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GC','To',0,'','299.066844766832');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GC','So',0,'','0.1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GC','lo',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GC','S_v1',0,'','0.1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GC','S_v2',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GC','l_v1',0,'','100');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GC','l_v2',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GC','Q0',0,'','1.33');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GC','Kpr',0,'','0.95');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GC','Ct',0,'','20');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GC','Riz',0,'','20');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GC','Fwind',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GC','Twind',0,'','273');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GC','frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GC','Ftmp',0,'','0.0998265565254606');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('AH1','Fi1',0,'','263.497024756575');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('AH1','Pi1',1,'EC1.Po1','0.97225608082126');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('AH1','Ti1',1,'EC1.To1','508.654372617286');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('AH1','Fi2',0,'','143.860762359488');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('AH1','Pi2',1,'Node9.Po','1.18332082850928');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('AH1','Ti2',1,'Node9.To','404.916816203185');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('AH1','ki',0,'','0.9');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('AH1','li1',0,'','5.5');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('AH1','li2',0,'','40');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('AH1','Si1',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('AH1','Si2',0,'','1.766');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('AH1','Kpr1',0,'','0.95');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('AH1','Q0i1',0,'','1.2');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('AH1','Ci1',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('AH1','Kpr2',0,'','0.95');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('AH1','Q0i2',0,'','1.293');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('AH1','Ci2',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('AH1','f_frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('AH1','lo1',0,'','5');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('AH1','lo2',0,'','5');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('AH1','Po1',0,'','0.970701528170671');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('AH1','Po2',0,'','1.17198424588706');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('AH1','To1',0,'','474.47535365977');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('AH1','To2',0,'','469.485219644776');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('AH1','Fo1',1,'Node10.Fi','263.497024756573');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('AH1','Fo2',1,'AH2.Fi2','143.860762359489');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('AH1','So1',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('AH1','So2',0,'','1.766');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SEA','Fi',0,'','131.748512378292');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SEA','Pi',1,'1G.Po','0.966959210122029');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SEA','Ti',1,'AH1.To1','474.47527137402');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SEA','Fo',1,'Node12.Fi1','131.748512378292');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SEA','Po',0,'','1.0030724574977');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SEA','To',0,'','479.692046371551');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SEA','So',0,'','5');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SEA','lo',0,'','2');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SEA','Kmrg',0,'','0.267249449349907');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SEA','N',0,'','83.4570378547403');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SEA','V',0,'','1.4');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SEA','Kpmp',0,'','2e-06');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SEA','Kslp',0,'','0.09');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SEA','Q0',0,'','1.2');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SEA','Ct',0,'','100');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SEA','Riz',0,'','100');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SEA','Fwind',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SEA','Twind',0,'','273');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SEA','frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SEA','Ftmp',0,'','0.406952860506926');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('AH2','Fi1',0,'','263.49702475657');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('AH2','Pi1',1,'EC2.Po1','0.975363155606217');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('AH2','Ti1',1,'EC2.To1','678.842436351744');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('AH2','Fi2',0,'','143.860762359489');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('AH2','Pi2',1,'AH1.Po2','1.17198424588706');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('AH2','Ti2',1,'AH1.To2','469.485219644776');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('AH2','ki',0,'','0.9');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('AH2','li1',0,'','6.5');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('AH2','li2',0,'','40');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('AH2','Si1',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('AH2','Si2',0,'','1.766');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('AH2','Kpr1',0,'','0.95');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('AH2','Q0i1',0,'','1.2');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('AH2','Ci1',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('AH2','Kpr2',0,'','0.95');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('AH2','Q0i2',0,'','1.293');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('AH2','Ci2',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('AH2','f_frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('AH2','lo1',0,'','15');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('AH2','lo2',0,'','15');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('AH2','Po1',0,'','0.973696329615125');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('AH2','Po2',0,'','1.15986558048338');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('AH2','To1',0,'','609.864290672697');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('AH2','To2',0,'','599.793486959106');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('AH2','Fo1',1,'EC1.Fi1','263.497024756575');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('AH2','Fo2',1,'Node5.Fi','143.860762359486');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('AH2','So1',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('AH2','So2',0,'','1.766');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SEB','Fi',0,'','131.748512378292');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SEB','Pi',1,'2G.Po','0.966959210122029');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SEB','Ti',1,'AH1.To1','474.475187215647');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SEB','Fo',1,'Node12.Fi2','131.748512378292');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SEB','Po',0,'','1.0030724574977');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SEB','To',0,'','479.692046276683');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SEB','So',0,'','5');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SEB','lo',0,'','2');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SEB','Kmrg',0,'','0.267249449349907');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SEB','N',1,'SEA.N','83.4570378547403');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SEB','V',0,'','1.4');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SEB','Kpmp',0,'','2e-06');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SEB','Kslp',0,'','0.09');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SEB','Q0',0,'','1.2');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SEB','Ct',0,'','100');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SEB','Riz',0,'','100');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SEB','Fwind',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SEB','Twind',0,'','273');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SEB','frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SEB','Ftmp',0,'','0.406952875440128');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GBF','Fi',0,'','56.7124912527033');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GBF','Pi',1,'Node3.Po2','1.42322091992014');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GBF','Ti',0,'','313');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GBF','Fo',1,'7GBF.Fi','56.7124912527032');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GBF','Po',0,'','1.41924762322943');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GBF','To',0,'','312.912123359316');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GBF','So',0,'','1.13');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GBF','lo',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GBF','S_v1',0,'','1.13');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GBF','S_v2',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GBF','l_v1',0,'','100');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GBF','l_v2',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GBF','Q0',0,'','1.2');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GBF','Kpr',0,'','0.95');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GBF','Ct',0,'','20');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GBF','Riz',0,'','20');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GBF','Fwind',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GBF','Twind',0,'','273');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GBF','frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GBF','Ftmp',0,'','0.00602198072890756');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GN3','Fi',0,'','120.002026546282');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GN3','Pi',1,'OhS.Po2','33.6251746743327');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GN3','Ti',1,'OhS.To2','745.860046890474');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GN3','Fo',1,'ParNet.Fi','120.002026546312');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GN3','Po',0,'','33.6056931578351');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GN3','To',0,'','745.458480154905');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GN3','So',0,'','0.5');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GN3','lo',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GN3','S_v1',0,'','0.3');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GN3','S_v2',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GN3','l_v1',0,'','90');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GN3','l_v2',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GN3','Q0',0,'','0.8');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GN3','Kpr',0,'','0.95');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GN3','Ct',0,'','20');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GN3','Riz',0,'','20');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GN3','Fwind',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GN3','Twind',0,'','273');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GN3','frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GN3','Ftmp',0,'','0.189172638424098');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('10GBF','Fi',0,'','56.7124912527041');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('10GBF','Pi',1,'8GBF.Po','1.41515987451424');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('10GBF','Ti',1,'8GBF.To','312.823852141096');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('10GBF','Fo',1,'Node1.Fi2','56.7124912527045');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('10GBF','Po',0,'','1.33560768055864');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('10GBF','To',0,'','312.392625705022');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('10GBF','So',0,'','0.5');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('10GBF','lo',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('10GBF','S_v1',0,'','0.25');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('10GBF','S_v2',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('10GBF','l_v1',0,'','100');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('10GBF','l_v2',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('10GBF','Q0',0,'','1.2');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('10GBF','Kpr',0,'','0.95');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('10GBF','Ct',0,'','20');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('10GBF','Riz',0,'','20');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('10GBF','Fwind',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('10GBF','Twind',0,'','273');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('10GBF','frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('10GBF','Ftmp',0,'','0.010917147196228');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('DS','Fi1',0,'','4.04791586944113e-06');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('DS','Pi1',1,'7FW.Po','35.8279896396018');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('DS','Ti1',1,'7FW.To','273.01138076956');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('DS','Fi2',0,'','120.002026544241');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('DS','Pi2',1,'Drum.Po1','33.6366303717587');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('DS','Ti2',1,'Drum.To1','511.325286818707');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('DS','ki',0,'','0.8');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('DS','li1',0,'','20');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('DS','li2',0,'','5');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('DS','Si1',0,'','0.00785');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('DS','Si2',0,'','0.5');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('DS','Kpr1',0,'','0.001');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('DS','Q0i1',0,'','1000');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('DS','Ci1',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('DS','Kpr2',0,'','0.95');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('DS','Q0i2',0,'','0.8');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('DS','Ci2',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('DS','f_frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('DS','lo1',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('DS','lo2',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('DS','Po1',0,'','35.827867851551');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('DS','Po2',0,'','33.6309424600756');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('DS','To1',0,'','511.325276770183');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('DS','To2',0,'','511.325280387652');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('DS','Fo1',1,'Node8.Fi1','-1.37644836487472e-05');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('DS','Fo2',1,'OhS.Fi2','120.002026544459');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('DS','So1',0,'','0.00785');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('DS','So2',0,'','0.5');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('BFA','Fi',0,'','71.9303811797411');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('BFA','Pi',1,'Node4.Po1','0.984357975459145');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('BFA','Ti',0,'','383');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('BFA','Fo',1,'Node9.Fi1','71.9303811797444');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('BFA','Po',0,'','1.18622985347462');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('BFA','To',0,'','405.008513768293');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('BFA','So',0,'','1.766');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('BFA','lo',0,'','20');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('BFA','Kmrg',0,'','0.400570283060486');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('BFA','N',0,'','58.2671070875674');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('BFA','V',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('BFA','Kpmp',0,'','2e-05');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('BFA','Kslp',0,'','0.0537');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('BFA','Q0',0,'','1.293');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('BFA','Ct',0,'','100');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('BFA','Riz',0,'','100');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('BFA','Fwind',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('BFA','Twind',0,'','273');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('BFA','frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('BFA','Ftmp',0,'','0.0180197642081762');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SrcWater','Pi',0,'','40');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SrcWater','Fo',1,'3FW.Fi','120.002026861348');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SrcWater','Po',0,'','39.879064718831');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SrcWater','So',0,'','0.017');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SrcWater','lo',0,'','100');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SrcWater','Noise',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SrcWater','Kpr',0,'','0.001');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SrcWater','Q0',0,'','1000');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SrcWater','f_frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('BFB','Fi',0,'','71.9303811797411');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('BFB','Pi',1,'Node4.Po2','0.984357975459145');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('BFB','Ti',0,'','383');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('BFB','Fo',1,'Node9.Fi2','71.9303811797444');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('BFB','Po',0,'','1.18622985347462');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('BFB','To',0,'','405.008513768293');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('BFB','So',0,'','1.766');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('BFB','lo',0,'','20');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('BFB','Kmrg',0,'','0.400570283060486');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('BFB','N',1,'BFA.N','58.2671070875674');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('BFB','V',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('BFB','Kpmp',0,'','2e-05');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('BFB','Kslp',0,'','0.0537');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('BFB','Q0',0,'','1.293');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('BFB','Ct',0,'','100');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('BFB','Riz',0,'','100');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('BFB','Fwind',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('BFB','Twind',0,'','273');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('BFB','frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('BFB','Ftmp',0,'','0.0180197643279143');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GN','Fi',0,'','0.0569318800253088');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GN','Pi',1,'3GN.Po','1.44999018997985');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GN','Ti',1,'3GN.To','281.706734035223');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GN','Fo',1,'5GN.Fi','0.0569318800252805');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GN','Po',0,'','1.44998631539862');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GN','To',0,'','276.158630864187');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GN','So',0,'','0.05');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GN','lo',0,'','20');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GN','S_v1',0,'','0.05');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GN','S_v2',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GN','l_v1',0,'','100');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GN','l_v2',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GN','Q0',0,'','0.7');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GN','Kpr',0,'','0.95');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GN','Ct',0,'','20');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GN','Riz',0,'','20');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GN','Fwind',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GN','Twind',0,'','273');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GN','frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GN','Ftmp',0,'','0.0176116371745379');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('13G','Fi',0,'','71.930381179739');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('13G','Pi',1,'Node5.Po2','1.14890575918545');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('13G','Ti',1,'AH2.To2','599.793549719148');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('13G','Fo',1,'Node2.Fi2','71.9303811797381');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('13G','Po',0,'','1.13443407023852');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('13G','To',0,'','599.187753428399');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('13G','So',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('13G','lo',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('13G','S_v1',0,'','0.7');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('13G','S_v2',0,'','0.3');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('13G','l_v1',0,'','80');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('13G','l_v2',0,'','80');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('13G','Q0',0,'','1.293');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('13G','Kpr',0,'','0.95');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('13G','Ct',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('13G','Riz',0,'','20');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('13G','Fwind',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('13G','Twind',0,'','273');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('13G','frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('13G','Ftmp',0,'','0.011251501271382');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('1G','Fi',0,'','131.748512378291');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('1G','Pi',1,'Node10.Po1','0.968578394499684');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('1G','Ti',1,'AH1.To1','474.475561498867');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('1G','Fo',1,'SEA.Fi','131.748512378292');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('1G','Po',0,'','0.966959210122029');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('1G','To',0,'','474.306702867234');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('1G','So',0,'','5');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('1G','lo',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('1G','S_v1',0,'','5');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('1G','S_v2',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('1G','l_v1',0,'','100');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('1G','l_v2',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('1G','Q0',0,'','1.2');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('1G','Kpr',0,'','0.95');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('1G','Ct',0,'','20');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('1G','Riz',0,'','20');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('1G','Fwind',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('1G','Twind',0,'','273');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('1G','frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('1G','Ftmp',0,'','0.0150022882112595');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5FW','Fi',0,'','120.002026869336');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5FW','Pi',1,'Node6.Po2','36.0653165870269');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5FW','Ti',1,'3FW.To','292.415005104293');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5FW','Fo',1,'Node7.Fi','120.00202994877');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5FW','Po',0,'','35.9571176864555');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5FW','To',0,'','292.381283068631');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5FW','So',0,'','0.017');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5FW','lo',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5FW','S_v1',0,'','0.009');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5FW','S_v2',0,'','0.009');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5FW','l_v1',0,'','100');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5FW','l_v2',1,'5FW.l_v1','100');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5FW','Q0',0,'','1000');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5FW','Kpr',0,'','0.001');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5FW','Ct',0,'','20');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5FW','Riz',0,'','20');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5FW','Fwind',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5FW','Twind',0,'','273');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5FW','frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5FW','Ftmp',0,'','0.24409834842013');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SrcNG','Pi',0,'','1.45');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SrcNG','Fo',1,'3GN.Fi','0.0569318800252665');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SrcNG','Po',0,'','1.44999438660746');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SrcNG','So',0,'','0.05');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SrcNG','lo',0,'','70');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SrcNG','Noise',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SrcNG','Kpr',0,'','0.95');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SrcNG','Q0',0,'','0.7');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SrcNG','f_frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GC','Fi',0,'','6.15434801160667');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GC','Pi',1,'4GC.Po','1.07223576711843');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GC','Ti',1,'4GC.To','299.066844766832');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GC','Fo',1,'6GC.Fi','6.15434801160658');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GC','Po',0,'','1.02619294497065');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GC','To',0,'','298.391853986037');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GC','So',0,'','0.1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GC','lo',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GC','S_v1',0,'','0.04');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GC','S_v2',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GC','l_v1',0,'','96.6192762757215');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GC','l_v2',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GC','Q0',0,'','1.33');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GC','Kpr',0,'','0.95');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GC','Ct',0,'','20');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GC','Riz',0,'','20');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GC','Fwind',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GC','Twind',0,'','273');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GC','frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GC','Ftmp',0,'','0.0221836791338808');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GBF','Fi',0,'','56.7124912527033');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GBF','Pi',1,'Node3.Po1','1.42322091992014');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GBF','Ti',0,'','313');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GBF','Fo',1,'8GBF.Fi','56.7124912527032');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GBF','Po',0,'','1.41924762322943');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GBF','To',0,'','312.912123359316');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GBF','So',0,'','1.13');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GBF','lo',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GBF','S_v1',0,'','1.13');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GBF','S_v2',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GBF','l_v1',0,'','100');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GBF','l_v2',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GBF','Q0',0,'','1.2');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GBF','Kpr',0,'','0.95');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GBF','Ct',0,'','20');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GBF','Riz',0,'','20');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GBF','Fwind',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GBF','Twind',0,'','273');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GBF','frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GBF','Ftmp',0,'','0.00483059613749161');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('ParNet','Fi',0,'','120.0020265442');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('ParNet','Pi',1,'4GN3.Po','33.605693157835');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('ParNet','Po',0,'','33.6');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('ParNet','So',0,'','0.5');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('ParNet','lo',0,'','100');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('ParNet','Kpr',0,'','0.95');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('ParNet','Noise',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('ParNet','Q0',0,'','0.8');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('ParNet','frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GN','Fi',0,'','0.0569318800252805');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GN','Pi',1,'4GN.Po','1.44998631539862');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GN','Ti',1,'4GN.To','276.158630864187');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GN','Fo',1,'6GN.Fi','0.0569318800252726');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GN','Po',0,'','0.980374473017035');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GN','To',0,'','273.364753798238');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GN','So',0,'','0.05');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GN','lo',0,'','15');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GN','S_v1',0,'','0.025');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GN','S_v2',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GN','l_v1',0,'','0.527772271659435');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GN','l_v2',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GN','Q0',0,'','0.7');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GN','Kpr',0,'','0.95');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GN','Ct',0,'','20');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GN','Riz',0,'','20');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GN','Fwind',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GN','Twind',0,'','273');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GN','frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GN','Ftmp',0,'','0.0176116371510276');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GC','Fi',0,'','6.15434801160658');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GC','Pi',1,'5GC.Po','1.02619294497065');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GC','Ti',1,'5GC.To','298.391853986037');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GC','Fo',1,'FireChamber.Fi3','6.15434801160667');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GC','Po',0,'','1.02420773045494');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GC','To',0,'','297.974495531498');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GC','So',0,'','0.1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GC','lo',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GC','S_v1',0,'','0.1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GC','S_v2',0,'','0.1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GC','l_v1',0,'','100');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GC','l_v2',0,'','100');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GC','Q0',0,'','1.33');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GC','Kpr',0,'','0.95');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GC','Ct',0,'','20');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GC','Riz',0,'','20');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GC','Fwind',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GC','Twind',0,'','273');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GC','frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GC','Ftmp',0,'','0.0221836790622149');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('EC1','Fi1',0,'','263.497024756575');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('EC1','Pi1',1,'AH2.Po1','0.973696329615125');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('EC1','Ti1',1,'AH2.To1','609.864468741827');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('EC1','Fi2',0,'','32.4497622836801');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('EC1','Pi2',1,'Node7.Po2','35.9482462363784');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('EC1','Ti2',1,'5FW.To','292.381283068647');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('EC1','ki',0,'','0.95');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('EC1','li1',0,'','15');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('EC1','li2',0,'','100');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('EC1','Si1',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('EC1','Si2',0,'','0.017');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('EC1','Kpr1',0,'','0.95');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('EC1','Q0i1',0,'','1.29');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('EC1','Ci1',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('EC1','Kpr2',0,'','0.001');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('EC1','Q0i2',0,'','1000');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('EC1','Ci2',0,'','4');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('EC1','f_frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('EC1','lo1',0,'','5');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('EC1','lo2',0,'','5');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('EC1','Po1',0,'','0.97225608082126');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('EC1','Po2',0,'','35.9393771311447');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('EC1','To1',0,'','508.654606854903');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('EC1','To2',0,'','501.454201872225');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('EC1','Fo1',1,'AH1.Fi1','263.497024756575');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('EC1','Fo2',1,'EC2.Fi2','32.4497590049955');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('EC1','So1',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('EC1','So2',0,'','0.017');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('EC2','Fi1',0,'','263.497024756568');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('EC2','Pi1',1,'OhS.Po1','0.976910420083689');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('EC2','Ti1',1,'OhS.To1','761.854601397785');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('EC2','Fi2',0,'','32.4498022846007');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('EC2','Pi2',1,'EC1.Po2','35.9393771311447');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('EC2','Ti2',1,'EC1.To2','501.454201872225');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('EC2','ki',0,'','0.95');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('EC2','li1',0,'','15');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('EC2','li2',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('EC2','Si1',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('EC2','Si2',0,'','0.005');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('EC2','Kpr1',0,'','0.95');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('EC2','Q0i1',0,'','1.2');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('EC2','Ci1',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('EC2','Kpr2',0,'','0.001');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('EC2','Q0i2',0,'','1000');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('EC2','Ci2',0,'','4');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('EC2','f_frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('EC2','lo1',0,'','5');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('EC2','lo2',0,'','5');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('EC2','Po1',0,'','0.975363155606217');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('EC2','Po2',0,'','35.8368156246128');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('EC2','To1',0,'','678.842044914527');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('EC2','To2',0,'','672.936251997199');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('EC2','Fo1',1,'AH2.Fi1','263.49702475657');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('EC2','Fo2',1,'Node8.Fi2','32.4493394688895');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('EC2','So1',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('EC2','So2',0,'','0.005');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('7GBF','Fi',0,'','56.7124912527032');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('7GBF','Pi',1,'5GBF.Po','1.41924762322943');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('7GBF','Ti',1,'5GBF.To','312.912123359316');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('7GBF','Fo',1,'9GBF.Fi','56.7124912527041');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('7GBF','Po',0,'','1.41515987451424');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('7GBF','To',0,'','312.823852141096');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('7GBF','So',0,'','1.13');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('7GBF','lo',0,'','15');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('7GBF','S_v1',0,'','1.13');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('7GBF','S_v2',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('7GBF','l_v1',0,'','100');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('7GBF','l_v2',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('7GBF','Q0',0,'','1.2');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('7GBF','Kpr',0,'','0.95');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('7GBF','Ct',0,'','20');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('7GBF','Riz',0,'','20');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('7GBF','Fwind',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('7GBF','Twind',0,'','273');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('7GBF','frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('7GBF','Ftmp',0,'','0.0270989128659375');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SrcAir','Pi',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SrcAir','Fo',1,'Node4.Fi','143.860762359487');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SrcAir','Po',0,'','0.988854896346954');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SrcAir','So',0,'','2');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SrcAir','lo',0,'','15');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SrcAir','Noise',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SrcAir','Kpr',0,'','0.95');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SrcAir','Q0',0,'','1.293');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SrcAir','f_frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('FireChamber','Fi1',0,'','113.424982505409');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('FireChamber','Pi1',1,'Node1.Po','1.31396295855582');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('FireChamber','Fi2',0,'','0.0569318800252679');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('FireChamber','Pi2',1,'6GN.Po','0.980373026187965');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('FireChamber','Fi3',0,'','6.15434801160667');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('FireChamber','Pi3',1,'6GC.Po','1.02420773045494');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('FireChamber','Fi4',0,'','143.860762359474');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('FireChamber','Pi4',1,'Node2.Po','1.12518355884656');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('FireChamber','Ti1',1,'Node1.To','312.357926183522');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('FireChamber','Ti2',1,'6GN.To','273.132322763636');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('FireChamber','Ti3',1,'6GC.To','297.974495531498');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('FireChamber','Ti4',1,'Node2.To','598.961162052536');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('FireChamber','Vi2',0,'','90');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('FireChamber','Fo',1,'Drum.Fi2','263.497024756553');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('FireChamber','Vi1',0,'','830');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('FireChamber','f_frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('FireChamber','Po',0,'','0.980353996305538');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('FireChamber','To',0,'','1102.3669542121');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('FireChamber','CO',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('FireChamber','O2',0,'','1.99207904234834');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('FireChamber','F_DG',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('FireChamber','Sum_toplivo',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('FireChamber','Q',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('FireChamber','Si1',0,'','0.25');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('FireChamber','Si2',0,'','0.025');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('FireChamber','Si3',0,'','0.04');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('FireChamber','Si4',0,'','0.5');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('FireChamber','lo',0,'','3');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Drum','Fi1',0,'','120.002029256193');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Drum','Pi1',1,'Node8.Po','35.8280420203758');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Drum','Ti1',1,'Node8.To','395.185374959387');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Drum','Si1',0,'','0.004');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Drum','Fi2',0,'','263.497024756553');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Drum','Pi2',1,'FireChamber.Po','0.980353996305538');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Drum','Ti2',1,'FireChamber.To','1102.36695424166');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Drum','Si2',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Drum','Vi1',0,'','16.8');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Drum','Fstm',0,'','120.002026543886');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Drum','Po1',0,'','33.6366303717586');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Drum','Lo',0,'','50.5528984322313');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Drum','To1',0,'','511.325286818708');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Drum','Fo',1,'DS.Fi2','120.002026544241');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Drum','So1',0,'','0.5');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Drum','lo1',0,'','5');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Drum','Fo2',1,'OhS.Fi1','263.497024756568');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Drum','Po2',0,'','0.978512871822965');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Drum','To2',0,'','864.541239442261');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Drum','S',0,'','14.25');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Drum','Tv',0,'','511.325286818708');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Drum','f_frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Drum','k',0,'','2.15');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GN','Fi',0,'','0.0569318800252087');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GN','Pi',1,'5GN.Po','0.980374473017035');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GN','Ti',1,'5GN.To','273.364753798238');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GN','Fo',1,'FireChamber.Fi2','0.0569318800252753');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GN','Po',0,'','0.980373026187967');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GN','To',0,'','273.132322763635');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GN','So',0,'','0.05');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GN','lo',0,'','8');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GN','S_v1',0,'','0.05');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GN','S_v2',0,'','0.05');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GN','l_v1',0,'','100');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GN','l_v2',0,'','100');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GN','Q0',0,'','0.7');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GN','Kpr',0,'','0.95');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GN','Ct',0,'','20');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GN','Riz',0,'','20');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GN','Fwind',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GN','Twind',0,'','273');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GN','frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GN','Ftmp',0,'','0.0587054570987826');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Atmosph','Fi',0,'','263.497024756588');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Atmosph','Pi',1,'Node12.Po','1.00156492196041');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Atmosph','Po',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Atmosph','So',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Atmosph','lo',0,'','100');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Atmosph','Kpr',0,'','0.95');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Atmosph','Noise',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Atmosph','Q0',0,'','1.2');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Atmosph','frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('8GBF','Fi',0,'','56.7124912527032');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('8GBF','Pi',1,'6GBF.Po','1.41924762322943');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('8GBF','Ti',1,'6GBF.To','312.912123359316');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('8GBF','Fo',1,'10GBF.Fi','56.7124912527041');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('8GBF','Po',0,'','1.41515987451424');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('8GBF','To',0,'','312.823852141096');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('8GBF','So',0,'','1.13');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('8GBF','lo',0,'','15');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('8GBF','S_v1',0,'','1.13');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('8GBF','S_v2',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('8GBF','l_v1',0,'','100');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('8GBF','l_v2',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('8GBF','Q0',0,'','1.2');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('8GBF','Kpr',0,'','0.95');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('8GBF','Ct',0,'','20');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('8GBF','Riz',0,'','20');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('8GBF','Fwind',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('8GBF','Twind',0,'','273');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('8GBF','frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('8GBF','Ftmp',0,'','0.0289835764672072');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('7FW','f_frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3FW','f_frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('9GBF','f_frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3GC','f_frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('2G','f_frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('11G','f_frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3GN','f_frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GC','f_frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SEA','f_frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GBF','f_frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SEB','f_frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GN3','f_frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('10GBF','f_frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('BFA','f_frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('BFB','f_frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GN','f_frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('13G','f_frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('1G','f_frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5FW','f_frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GC','f_frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GBF','f_frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GN','f_frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GC','f_frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('7GBF','f_frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GN','f_frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('8GBF','f_frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('FireChamber','tmp1',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('7FW','Position',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3FW','Position',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('9GBF','Position',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3GC','Position',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('2G','Position',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('11G','Position',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3GN','Position',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GC','Position',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GBF','Position',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GN3','Position',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('10GBF','Position',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GN','Position',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('13G','Position',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('1G','Position',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5FW','Position',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GC','Position',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GBF','Position',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GN','Position',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GC','Position',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('7GBF','Position',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GN','Position',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('8GBF','Position',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3GN','noBack',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3GN','Kln',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3GN','Pot',0,'','1.65539403906117');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3GN','Fit',0,'','-0.0171311692692742');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GN','noBack',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GN','Kln',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GN','Pot',0,'','1.06124961454514');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GN','Fit',0,'','3.95252516672997e-323');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GN','noBack',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GN','Kln',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GN','Pot',0,'','1.63400731904421');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GN','Fit',0,'','-0.0014119176907065');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GN','noBack',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GN','Kln',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GN','Pot',0,'','1.59418198995609');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GN','Fit',0,'','4.58035488684914e-05');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SrcGBF','Fit',0,'','113.424982505351');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('7FW','Kln',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('7FW','noBack',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('7FW','Fit',0,'','4.94065645841247e-323');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SrcGC','Fit',0,'','6.15434801159173');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3FW','Kln',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3FW','noBack',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3FW','Fit',0,'','4.94065645841247e-323');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('9GBF','Kln',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('9GBF','noBack',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('9GBF','Fit',0,'','4.94065645841247e-323');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3GC','Kln',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3GC','noBack',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3GC','Fit',0,'','0.0072747178945105');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('2G','Kln',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('2G','noBack',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('2G','Fit',0,'','<EVAL>');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('11G','Kln',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('11G','noBack',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('11G','Fit',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GC','Kln',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GC','noBack',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GC','Fit',0,'','-0.00525951915637014');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SEA','Fit',0,'','131.748512378292');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GBF','Kln',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GBF','noBack',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GBF','Fit',0,'','7.74471385884412');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SEB','Fit',0,'','131.748512378292');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GN3','Kln',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GN3','noBack',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GN3','Fit',0,'','inf');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('10GBF','Kln',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('10GBF','noBack',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('10GBF','Fit',0,'','4.94065645841247e-323');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('BFA','Fit',0,'','71.9303811797421');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('BFB','Fit',0,'','71.9303811797421');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SrcWater','Fit',0,'','120.002026867687');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('13G','Kln',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('13G','noBack',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('13G','Fit',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('1G','Kln',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('1G','noBack',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('1G','Fit',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SrcNG','Fit',0,'','0.0569318800251766');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5FW','Kln',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5FW','noBack',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5FW','Fit',0,'','-1921.83949616043');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GC','Kln',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GC','noBack',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GC','Fit',0,'','-0.003071289811726');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GBF','Kln',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GBF','noBack',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GBF','Fit',0,'','-18.4202963625732');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GC','Kln',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GC','noBack',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GC','Fit',0,'','7.75136962070458e-17');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('7GBF','Kln',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('7GBF','noBack',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('7GBF','Fit',0,'','2.98484814794589');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SrcAir','Fit',0,'','143.860762359487');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('8GBF','Kln',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('8GBF','noBack',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('8GBF','Fit',0,'','-7.87046005105913');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node1','FoR',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node1','Fit',0,'','-12.2439730608091');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node9','FoR',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node9','Fit',0,'','0.120035867737939');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node12','FoR',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node12','Fit',0,'','3.7848333214031e-77');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Atmosph','f_frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node8','FoR',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node8','Fit',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node2','FoR',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node2','Fit',0,'','1.47325952927438e-105');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('ParNet','f_frq',0,'','200');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('BFA','Kpr',0,'','0.95');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SEA','Kpr',0,'','0.95');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('SEB','Kpr',0,'','0.95');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('BFB','Kpr',0,'','0.95');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('FireChamber','So',0,'','2');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('FireChamber','V',0,'','830');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('7FW','t_v1',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('7FW','t_v2',0,'','3');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('7FW','tmp_l1',0,'','4.94065645841247e-321');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('7FW','tmp_l2',0,'','1.48219693752374e-321');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3FW','t_v1',0,'','3');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3FW','t_v2',0,'','3');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3FW','tmp_l1',0,'','38.4434357563499');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3FW','tmp_l2',0,'','38.4434357563499');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('9GBF','t_v1',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('9GBF','t_v2',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('9GBF','tmp_l1',0,'','99.9999999999858');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('9GBF','tmp_l2',0,'','4.94065645841247e-321');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3GC','t_v1',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3GC','t_v2',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3GC','tmp_l1',0,'','100.000000000014');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3GC','tmp_l2',0,'','4.94065645841247e-321');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('2G','t_v1',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('2G','t_v2',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('2G','tmp_l1',0,'','100.000000000014');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('2G','tmp_l2',0,'','4.94065645841247e-321');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('11G','t_v1',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('11G','t_v2',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('11G','tmp_l1',0,'','80.0000000000142');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('11G','tmp_l2',0,'','80.0000000000142');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3GN','t_v1',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3GN','t_v2',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3GN','tmp_l1',0,'','100.000000000014');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('3GN','tmp_l2',0,'','4.94065645841247e-321');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GC','t_v1',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GC','t_v2',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GC','tmp_l1',0,'','100.000000000014');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GC','tmp_l2',0,'','4.94065645841247e-321');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GBF','t_v1',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GBF','t_v2',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GBF','tmp_l1',0,'','100.000000000014');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GBF','tmp_l2',0,'','4.94065645841247e-321');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GN3','t_v1',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GN3','t_v2',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GN3','tmp_l1',0,'','90.0000000000142');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GN3','tmp_l2',0,'','4.94065645841247e-321');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('10GBF','t_v1',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('10GBF','t_v2',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('10GBF','tmp_l1',0,'','99.9999999999858');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('10GBF','tmp_l2',0,'','4.94065645841247e-321');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GN','t_v1',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GN','t_v2',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GN','tmp_l1',0,'','100.000000000014');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('4GN','tmp_l2',0,'','4.94065645841247e-321');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('13G','t_v1',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('13G','t_v2',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('13G','tmp_l1',0,'','80.0000000000142');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('13G','tmp_l2',0,'','80.0000000000142');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('1G','t_v1',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('1G','t_v2',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('1G','tmp_l1',0,'','100.000000000014');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('1G','tmp_l2',0,'','4.94065645841247e-321');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5FW','t_v1',0,'','3');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5FW','t_v2',0,'','3');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5FW','tmp_l1',0,'','99.9999999999957');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5FW','tmp_l2',0,'','99.9999999999957');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GC','t_v1',0,'','3');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GC','t_v2',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GC','tmp_l1',0,'','96.6192762757173');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GC','tmp_l2',0,'','4.94065645841247e-321');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GBF','t_v1',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GBF','t_v2',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GBF','tmp_l1',0,'','100.000000000014');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GBF','tmp_l2',0,'','4.94065645841247e-321');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GN','t_v1',0,'','3');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GN','t_v2',0,'','3');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GN','tmp_l1',0,'','0.527772271659402');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('5GN','tmp_l2',0,'','1.48219693752374e-321');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GC','t_v1',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GC','t_v2',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GC','tmp_l1',0,'','100.000000000014');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GC','tmp_l2',0,'','100.000000000014');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('7GBF','t_v1',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('7GBF','t_v2',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('7GBF','tmp_l1',0,'','100.000000000014');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('7GBF','tmp_l2',0,'','4.94065645841247e-321');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GN','t_v1',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GN','t_v2',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GN','tmp_l1',0,'','100.000000000014');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('6GN','tmp_l2',0,'','100.000000000014');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('8GBF','t_v1',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('8GBF','t_v2',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('8GBF','tmp_l1',0,'','100.000000000014');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('8GBF','tmp_l2',0,'','4.94065645841247e-321');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node5','Pot1',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node5','Pot2',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node6','Pot1',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node6','Pot2',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node7','Pot1',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node7','Pot2',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node10','Pot1',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node10','Pot2',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node3','Pot1',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node3','Pot2',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node4','Pot1',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_io" VALUES('Node4','Pot2',0,'','1');
CREATE TABLE 'BlckCalcBlcks_boiler9_cntr_io' ("BLK_ID" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"TLNK" INTEGER DEFAULT '' ,"LNK" TEXT DEFAULT '' ,"VAL" TEXT DEFAULT '' , PRIMARY KEY ("BLK_ID","ID"));
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('LC121','var',2,'boiler9.Drum.Lo','50.5528984305931');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('LC121','sp',0,'','50.5529135014202');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('LC121','max',0,'','100');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('LC121','min',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('LC121','out',6,'boiler9.3FW.l_v1','38.4434355630288');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('LC121','auto',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('LC121','casc',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('LC121','Kp',0,'','2');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('LC121','Ti',0,'','60000');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('LC121','Td',0,'','120000');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('LC121','Tf',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('LC121','Hup',0,'','100');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('LC121','Hdwn',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('LC121','Zi',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('LC121','K1',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('LC121','in1',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('LC121','K2',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('LC121','in2',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('LC121','K3',0,'','0.4');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('LC121','in3',1,'FC101.var','120.002026572978');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('LC121','K4',0,'','-0.4');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('LC121','in4',2,'boiler9.Node8.Fo','120.002024789398');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('LC121','cycle',0,'','1000');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('LC121','int',0,'','38.443350004761');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('LC121','dif',0,'','50.5529093712294');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('LC121','lag',0,'','1.09406362867048e-05');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('TCA1','var',2,'boiler9.4GN3.To','745.458480094361');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('TCA1','sp',0,'','745.458480094361');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('TCA1','max',0,'','800');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('TCA1','min',0,'','273');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('TCA1','out',6,'boiler9.7FW.l_v1','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('TCA1','auto',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('TCA1','casc',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('TCA1','Kp',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('TCA1','Ti',0,'','10000');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('TCA1','Td',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('TCA1','Tf',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('TCA1','Hup',0,'','100');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('TCA1','Hdwn',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('TCA1','Zi',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('TCA1','K1',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('TCA1','in1',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('TCA1','K2',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('TCA1','in2',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('TCA1','K3',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('TCA1','in3',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('TCA1','K4',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('TCA1','in4',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('TCA1','cycle',0,'','1000');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('TCA1','int',0,'','-1.53062131857951e-08');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('TCA1','dif',0,'','89.6505654828009');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('TCA1','lag',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PSA76','var',2,'boiler9.FireChamber.Po','0.980353996305538');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PSA76','sp',0,'','0.980353996305538');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PSA76','max',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PSA76','min',0,'','0.9');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PSA76','out',6,'boiler9.SEA.N','83.4570378547403');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PSA76','auto',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PSA76','casc',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PSA76','Kp',0,'','-0.2');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PSA76','Ti',0,'','3000');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PSA76','Td',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PSA76','Tf',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PSA76','Hup',0,'','100');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PSA76','Hdwn',0,'','2');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PSA76','Zi',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PSA76','K1',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PSA76','in1',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PSA76','K2',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PSA76','in2',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PSA76','K3',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PSA76','in3',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PSA76','K4',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PSA76','in4',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PSA76','cycle',0,'','1000');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PSA76','int',0,'','83.4570378547403');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PSA76','dif',0,'','80.3539963055378');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PSA76','lag',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC102','var',2,'boiler9.3GN.Fi','0.0569318800252035');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC102','sp',1,'Fsum.S_f_p','0.0376613934972674');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC102','max',0,'','6');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC102','min',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC102','out',6,'boiler9.5GN.l_v1','0.527772271659435');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC102','auto',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC102','casc',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC102','Kp',0,'','0.25');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC102','Ti',0,'','3000');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC102','Td',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC102','Tf',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC102','Hup',0,'','100');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC102','Hdwn',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC102','Zi',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC102','K1',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC102','in1',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC102','K2',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC102','in2',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC102','K3',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC102','in3',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC102','K4',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC102','in4',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC102','cycle',0,'','1000');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC102','int',0,'','0.527772271659435');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC102','dif',0,'','0.948864667086725');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC102','lag',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC105','var',2,'boiler9.3GC.Fi','6.15434801160656');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC105','sp',1,'Fsum.S_f_k','6.23146419829144');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC105','max',0,'','10');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC105','min',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC105','out',6,'boiler9.5GC.l_v1','96.6192762757215');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC105','auto',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC105','casc',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC105','Kp',0,'','0.25');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC105','Ti',0,'','3000');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC105','Td',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC105','Tf',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC105','Hup',0,'','100');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC105','Hdwn',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC105','Zi',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC105','K1',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC105','in1',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC105','K2',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC105','in2',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC105','K3',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC105','in3',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC105','K4',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC105','in4',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC105','cycle',0,'','1000');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC105','int',0,'','96.6192762757215');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC105','dif',0,'','61.5434801160656');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC105','lag',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC103','var',2,'boiler9.5GBF.Fi','56.7124912527033');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC103','sp',1,'Fsum.S_f_dl','63.2555286609093');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC103','max',0,'','70');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC103','min',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC103','out',6,'boiler9.9GBF.l_v1','100');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC103','auto',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC103','casc',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC103','Kp',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC103','Ti',0,'','3000');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC103','Td',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC103','Tf',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC103','Hup',0,'','100');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC103','Hdwn',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC103','Zi',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC103','K1',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC103','in1',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC103','K2',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC103','in2',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC103','K3',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC103','in3',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC103','K4',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC103','in4',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC103','cycle',0,'','1000');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC103','int',0,'','90.6528037025628');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC103','dif',0,'','81.017844646719');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC103','lag',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC104','var',2,'boiler9.6GBF.Fi','56.7124912527033');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC104','sp',1,'Fsum.S_f_dp','63.2555286609093');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC104','max',0,'','70');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC104','min',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC104','out',6,'boiler9.10GBF.l_v1','100');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC104','auto',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC104','casc',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC104','Kp',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC104','Ti',0,'','3000');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC104','Td',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC104','Tf',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC104','Hup',0,'','100');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC104','Hdwn',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC104','Zi',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC104','K1',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC104','in1',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC104','K2',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC104','in2',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC104','K3',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC104','in3',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC104','K4',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC104','in4',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC104','cycle',0,'','1000');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC104','int',0,'','90.6528037025628');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC104','dif',0,'','81.017844646719');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC104','lag',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('5FW_inv','IVar',1,'TCA1.out','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('5FW_inv','OVar',6,'boiler9.5FW.l_v1','100');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('Fsum','F_sum',1,'FC101.out','74.4182149752745');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('Fsum','F_d_l',1,'FC103.var','56.7124912527033');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('Fsum','F_d_p',1,'FC104.var','56.7124912527033');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('Fsum','F_k',1,'FC105.var','6.15434801160656');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('Fsum','S_f_p',0,'','0.0376613934972674');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('Fsum','S_f_k',0,'','6.23146419829144');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('Fsum','S_f_dl',0,'','63.2555286609093');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('Fsum','S_f_dp',0,'','63.2555286609093');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PCA51','var',2,'boiler9.4GN3.Po','33.6056931578372');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PCA51','sp',0,'','33.6056931578372');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PCA51','max',0,'','50');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PCA51','min',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PCA51','out',0,'','88.895960391647');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PCA51','auto',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PCA51','casc',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PCA51','Kp',0,'','0.2');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PCA51','Ti',0,'','60000');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PCA51','Td',0,'','10000');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PCA51','Tf',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PCA51','Hup',0,'','100');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PCA51','Hdwn',0,'','5');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PCA51','Zi',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PCA51','K1',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PCA51','in1',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PCA51','K2',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PCA51','in2',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PCA51','K3',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PCA51','in3',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PCA51','K4',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PCA51','in4',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PCA51','cycle',0,'','1000');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PCA51','int',0,'','88.8959603915762');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PCA51','dif',0,'','67.2113863157436');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PCA51','lag',0,'','6.91358081894577e-11');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('Fsum','f_max',0,'','170');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('Fsum','f_min',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('F_air_gas','var',1,'Air_Gas.F_out','1.6183048332643');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('F_air_gas','sp',0,'','1.6183048332643');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('F_air_gas','max',0,'','100');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('F_air_gas','min',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('F_air_gas','out',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('F_air_gas','auto',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('F_air_gas','casc',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('F_air_gas','Kp',0,'','-0.1');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('F_air_gas','Ti',0,'','50000');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('F_air_gas','Td',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('F_air_gas','Tf',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('F_air_gas','Hup',0,'','100');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('F_air_gas','Hdwn',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('F_air_gas','Zi',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('F_air_gas','K1',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('F_air_gas','in1',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('F_air_gas','K2',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('F_air_gas','in2',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('F_air_gas','K3',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('F_air_gas','in3',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('F_air_gas','K4',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('F_air_gas','in4',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('F_air_gas','cycle',0,'','1000');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('F_air_gas','int',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('F_air_gas','dif',0,'','1.6183048332643');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('F_air_gas','lag',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('Air_Gas','F_air',2,'boiler9.Node2.Fo','143.860762359474');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('Air_Gas','F_gas',1,'PCA51.out','88.895960391647');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('Air_Gas','F_out',0,'','1.6183048332643');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('LC121','manIn',0,'','38.4434355630288');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('LC121','Kd',0,'','5');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('LC121','Tzd',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('LC121','followSp',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('LC121','f_frq',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('TCA1','manIn',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('TCA1','Kd',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('TCA1','Tzd',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('TCA1','followSp',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('TCA1','f_frq',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC102','manIn',0,'','0.527772271659435');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC102','Kd',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC102','Tzd',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC102','followSp',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC102','f_frq',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC103','manIn',0,'','100');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC103','Kd',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC103','Tzd',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC103','followSp',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC103','f_frq',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC104','manIn',0,'','100');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC104','Kd',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC104','Tzd',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC104','followSp',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC104','f_frq',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC105','manIn',0,'','96.6192762757215');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC105','Kd',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC105','Tzd',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC105','followSp',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC105','f_frq',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('F_air_gas','manIn',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('F_air_gas','Kd',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('F_air_gas','Tzd',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('F_air_gas','followSp',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('F_air_gas','f_frq',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PCA51','manIn',0,'','88.895960391647');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PCA51','Kd',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PCA51','Tzd',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PCA51','followSp',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PCA51','f_frq',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PSA76','manIn',0,'','83.4570378547403');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PSA76','Kd',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PSA76','Tzd',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PSA76','followSp',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('PSA76','f_frq',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('QAC151','var',2,'boiler9.FireChamber.O2','1.99207904234823');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('QAC151','sp',0,'','1.99207904234818');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('QAC151','max',0,'','15');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('QAC151','min',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('QAC151','manIn',0,'','58.2671070875674');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('QAC151','out',6,'boiler9.BFA.N','58.2671070875674');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('QAC151','auto',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('QAC151','casc',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('QAC151','Kp',0,'','0.1');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('QAC151','Ti',0,'','3000');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('QAC151','Kd',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('QAC151','Td',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('QAC151','Tzd',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('QAC151','Hup',0,'','100');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('QAC151','Hdwn',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('QAC151','Zi',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('QAC151','followSp',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('QAC151','K1',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('QAC151','in1',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('QAC151','K2',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('QAC151','in2',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('QAC151','K3',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('QAC151','in3',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('QAC151','K4',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('QAC151','in4',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('QAC151','f_frq',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('QAC151','int',0,'','58.2671070875675');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('QAC151','dif',0,'','13.2805269489882');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('QAC151','lag',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC101','var',2,'boiler9.4GN3.Fi','120.002026572978');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC101','sp',0,'','120');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC101','max',0,'','150');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC101','min',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC101','manIn',0,'','74.4182149752745');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC101','out',0,'','74.4182149752745');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC101','auto',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC101','casc',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC101','Kp',0,'','0.1');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC101','Ti',0,'','5000');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC101','Kd',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC101','Td',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC101','Tzd',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC101','Hup',0,'','100');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC101','Hdwn',0,'','5');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC101','Zi',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC101','followSp',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC101','K1',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC101','in1',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC101','K2',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC101','in2',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC101','K3',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC101','in3',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC101','K4',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC101','in4',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC101','f_frq',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC101','int',0,'','74.4183500801397');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC101','dif',0,'','80.0013510486522');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('FC101','lag',0,'','0');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('Fsum','Casc_f_p',1,'FC102.casc','1');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('Fsum','Si_f_p',1,'FC102.sp','0.0376720753405156');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('Fsum','Casc_f_k',1,'FC105.casc','1');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('Fsum','Si_f_k',1,'FC105.sp','6.23148607063714');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('Fsum','Casc_f_dl',1,'FC103.casc','1');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('Fsum','Si_f_dl',1,'FC103.sp','63.2555516268723');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('Fsum','Casc_f_dp',1,'FC104.casc','1');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('Fsum','Si_f_dp',1,'FC104.sp','63.2555516268723');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('Fsum','t_zd',0,'','3');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('Fsum','f_frq',0,'','1');
INSERT INTO "BlckCalcBlcks_boiler9_cntr_io" VALUES('Fsum','FsumW',0,'','126.511057321819');
CREATE TABLE 'BlckCalcPrm_boiler9' ("SHIFR" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '' ,"IO" TEXT DEFAULT '' ,"ru#IO" TEXT DEFAULT '' ,"uk#IO" TEXT DEFAULT '' , PRIMARY KEY ("SHIFR"));
INSERT INTO "BlckCalcPrm_boiler9" VALUES('Fdrum','Fdrum','Fбар.','Fбар.','Water flow into the drum','Расход воды в барабан','Витрати води до барабану',1,'Drum.Fi1:var:Value','Drum.Fi1:var:Значение','Drum.Fi1:var:Значення');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('Pdrum','Pdrum','Pбар.','Pбар.','Pressure in the boiler drum','Давление в барабане котла','Тиск у барабані котла',1,'Drum.Po1:var:Value','Drum.Po1:var:Значение','Drum.Po1:var:Значення');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('FC102','FC102','','','Flow of the natural gas after 3GN','Расход природного газа после 3ГП','Витрати природного газу після 3ГП',1,'3GN.Fo:var:Value','3GN.Fo:var:Значение','3GN.Fo:var:Значення');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('PSA53','PSA53','','','GN pressure before the regulating valve ','Давление ГП до регулирующей заслонки','Тиск ГП до регулюючої засувки',1,'3GN.Po:var:Value
*r.2:prec:Precision
*s.at:ed:Unit of measure
*r.0:min:Minimum
*r.40:max:Maximum','3GN.Po:var:Значение
*r.2:prec:Точноcть
*s.ат:ed:Единица измерения
*r.0:min:Минимум
*r.40:max:Максимум','3GN.Po:var:Значення
*r.2:prec:Точність
*s.ат:ed:Одиниця виміру
*r.0:min:Мінімум
*r.40:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('PSA54','PSA54','','','GN pressure after the regulating valve','Давление ГП после регулирующей заслонки','Тиск ГП після регулюючої засувки',1,'5GN.Po:var:Value
*r.3:prec:Precision
*s.at:ed:Unit of measure
*r.0:min:Minimum
*r.1.5:max:Maximum','5GN.Po:var:Значение
*r.3:prec:Точноcть
*s.ат:ed:Единица измерения
*r.0:min:Минимум
*r.1.5:max:Максимум','5GN.Po:var:Значення
*r.3:prec:Точність
*s.ат:ed:Одиниця виміру
*r.0:min:Мінімум
*r.1.5:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('T2','T2','','','Natural gas temperature','Температура природного газа','Температура природного газу',1,'3GN.To:var:Value
*r.2:prec:Precision
*s.deg.K:ed:Unit of measure
*r.223:min:Minimum
*r.323:max:Maximum','3GN.To:var:Значение
*r.2:prec:Точноcть
*s.град.К:ed:Единица измерения
*r.223:min:Минимум
*r.323:max:Максимум','3GN.To:var:Значення
*r.2:prec:Точність
*s.град.К:ed:Одиниця виміру
*r.223:min:Мінімум
*r.323:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('PA56','PA56','','','GN pressure before the right burner','Давление ГП перед правой горелкой','Тиск ГП перед правою горелкою',1,'6GN.Po:var:Value
*r.2:prec:Precision
*s.at:ed:Unit of measure
*r.0:min:Minimum
*r.40:max:Maximum','6GN.Po:var:Значение
*r.2:prec:Точноcть
*s.ат:ed:Единица измерения
*r.0:min:Минимум
*r.40:max:Максимум','6GN.Po:var:Значення
*r.2:prec:Точність
*s.ат:ed:Одиниця виміру
*r.0:min:Мінімум
*r.40:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('PA55','PA55','','','GN pressure before the left burner','Давление ГП перед левой горелкой','Тиск ГП перед лівою горелкою',1,'6GN.Po:var:Value
*r.2:prec:Precision
*s.at:ed:Unit of measure
*r.0:min:Minimum
*r.40:max:Maximum','6GN.Po:var:Значение
*r.2:prec:Точноcть
*s.ат:ed:Единица измерения
*r.0:min:Минимум
*r.40:max:Максимум','6GN.Po:var:Значення
*r.2:prec:Точність
*s.ат:ed:Одиниця виміру
*r.0:min:Мінімум
*r.40:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('PSA59','PSA59','','','GBF pressure after valve on the left gas pipeline','Давление ГД после заслонки на левом газопроводе','Тиск ГД після засувки на лівому газопроводі',1,'10GBF.Po:var:Value
*r.3:prec:Precision
*s.at:ed:Unit of measure
*r.1:min:Minimum
*r.2:max:Maximum','10GBF.Po:var:Значение
*r.3:prec:Точность
*s.ат:ed:Единица измерения
*r.1:min:Минимум
*r.2:max:Максимум','10GBF.Po:var:Значення
*r.3:prec:Точність
*s.ат:ed:Одиниця виміру
*r.1:min:Мінімум
*r.2:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('FC103','FC103','','','GBF flow on the gas pipeline left','Расход ГД по левому газопроводу','Витрати ГД по лівому газопроводу',1,'5GBF.Fi:var:Value','5GBF.Fi:var:Значение','5GBF.Fi:var:Значення');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('P72','P72','','','Air pressure in the upper tier of the left burner','Давление воздуха на верхнем ярусе левой горелки','Тиск повітря на верхньому ярусі лівої горелки',1,'Node2.Po:var:Value
*r.2:prec:Precision
*s.at:ed:Unit of measure
*r.0:min:Minimum
*r.1.2:max:Maximum','Node2.Po:var:Значение
*r.2:prec:Точноcть
*s.ат:ed:Единица измерения
*r.0:min:Минимум
*r.1.2:max:Максимум','Node2.Po:var:Значення
*r.2:prec:Точність
*s.ат:ed:Одиниця виміру
*r.0:min:Мінімум
*r.1.2:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('P74','P74','','','Air pressure on the lower tier of the left burner','Давление воздуха на нижнем ярусе левой горелки','Тиск повітря на нижньому ярусі лівої горелки',1,'Node2.Po:var:Value
*r.2:prec:Precision
*s.at:ed:Unit of measure
*r.0:min:Minimum
*r.1.16:max:Maximum','Node2.Po:var:Значение
*r.2:prec:Точноcть
*s.ат:ed:Единица измерения
*r.0:min:Минимум
*r.1.16:max:Максимум','Node2.Po:var:Значення
*r.2:prec:Точність
*s.ат:ed:Одиниця виміру
*r.0:min:Мінімум
*r.1.16:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('PSA70','PSA70','','','Air pressure after the second stage of the A/H','Давление воздуха после второй ступени воздухо-подогревателя','Тиск повітря після другого ступеня повітря-підігрівача',1,'AH2.Po2:var:Value
*r.2:prec:Precision
*s.at:ed:Unit of measure
*r.0:min:Minimum
*r.1.2:max:Maximum','AH2.Po2:var:Значение
*r.2:prec:Точноcть
*s.ат:ed:Единица измерения
*r.0:min:Минимум
*r.1.2:max:Максимум','AH2.Po2:var:Значення
*r.2:prec:Точність
*s.ат:ed:Одиниця виміру
*r.0:min:Мінімум
*r.1.2:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('PSA71','PSA71','','','Air pressure after the second stage of the A/H','Давление воздуха после второй ступени воздухо-подогревателя','Тиск повітря після другого ступеня повітря-підігрівача',1,'AH2.Po2:var:Value
*r.2:prec:Precision
*s.at:ed:Unit of measure
*r.0:min:Minimum
*r.1.2:max:Maximum','AH2.Po2:var:Значение
*r.2:prec:Точноcть
*s.ат:ed:Единица измерения
*r.0:min:Минимум
*r.1.2:max:Максимум','AH2.Po2:var:Значення
*r.2:prec:Точність
*s.ат:ed:Одиниця виміру
*r.0:min:Мінімум
*r.1.2:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('PSA73','PSA73','','','Air pressure in the upper tier of the right burner','Давление воздуха на верхнем ярусе правой горелки','Тиск повітря на верхньому ярусі правої горелки',1,'Node2.Po:var:Value
*r.2:prec:Precision
*s.at:ed:Unit of measure
*r.0:min:Minimum
*r.1.16:max:Maximum','Node2.Po:var:Значение
*r.2:prec:Точноcть
*s.ат:ed:Единица измерения
*r.0:min:Минимум
*r.1.16:max:Максимум','Node2.Po:var:Значення
*r.2:prec:Точність
*s.ат:ed:Одиниця виміру
*r.0:min:Мінімум
*r.1.16:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('PSA75','PSA75','','','Air pressure on the lower tier of the right burner','Давление воздуха на нижнем ярусе правой горелки','Тиск повітря на нижньому ярусі правої горелки',1,'Node2.Po:var:Value
*r.2:prec:Precision
*s.at:ed:Unit of measure
*r.0:min:Minimum
*r.1.16:max:Maximum','Node2.Po:var:Значение
*r.2:prec:Точноcть
*s.ат:ed:Единица измерения
*r.0:min:Минимум
*r.1.16:max:Максимум','Node2.Po:var:Значення
*r.2:prec:Точність
*s.ат:ed:Одиниця виміру
*r.0:min:Мінімум
*r.1.16:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('FC106','FC106','','','Air flow to the left burner','Расход воздуха на левую горелку','Витрати повітря на ліву горелку',1,'Node2.Fi1:var:Value
*r.1:prec:Precision
*s.t/h:ed:Unit of measure
*r.0:min:Minimum
*r.100:max:Maximum','Node2.Fi1:var:Значение
*r.1:prec:Точноcть
*s.т/ч:ed:Единица измерения
*r.0:min:Минимум
*r.100:max:Максимум','Node2.Fi1:var:Значення
*r.1:prec:Точність
*s.т/год:ed:Одиниця виміру
*r.0:min:Мінімум
*r.100:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('FC107','FC107','','','Air flow to the right burner','Расход воздуха на правую горелку','Витрати повітря на праву горелку',1,'Node2.Fi2:var:Value
*r.1:prec:Precision
*s.t/h:ed:Unit of measure
*r.0:min:Minimum
*r.100:max:Maximum','Node2.Fi2:var:Значение
*r.1:prec:Точноcть
*s.т/ч:ed:Единица измерения
*r.0:min:Минимум
*r.100:max:Максимум','Node2.Fi2:var:Значення
*r.1:prec:Точність
*s.т/год:ed:Одиниця виміру
*r.0:min:Мінімум
*r.100:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('FC105','FC105','','','Coke gas flow of after 5GC','Расход коксового газа после 5ГК','Витрати коксового газу після 5ГК',1,'5GC.Fo:var:Value','5GC.Fo:var:Значение','5GC.Fo:var:Значення');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('LC121','LC121','','','Water level in the boiler drum','Уровень воды в барабане котла','Рівень води у барабані котла',1,'Drum.Lo:var:Value','Drum.Lo:var:Значение','Drum.Lo:var:Значення');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('PSA52','PSA52','','','Steam pressure in the the boiler drum','Давление пара в барабане котла','Тиск пару у барабані котла',1,'Drum.Po1:var:Value
*r.2:prec:Precision
*s.at:ed:Unit of measure
*r.0:min:Minimum
*r.50:max:Maximum','Drum.Po1:var:Значение
*r.2:prec:Точноcть
*s.ат:ed:Единица измерения
*r.0:min:Минимум
*r.40:max:Максимум','Drum.Po1:var:Значення
*r.2:prec:Точність
*s.ат:ed:Одиниця виміру
*r.0:min:Мінімум
*r.40:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('PCSA76','PCSA76','','','Vacuum in the fire chamber on the left','Разрежение в топке слева','Розрідження у топці ліворуч',1,'FireChamber.Po:var:Value
*r.3:prec:Precision
*s.at:ed:Unit of measure
*r.0.9:min:Minimum
*r.1:max:Maximum','FireChamber.Po:var:Значение
*r.3:prec:Точноcть
*s.ат:ed:Единица измерения
*r.0.9:min:Минимум
*r.1:max:Максимум','FireChamber.Po:var:Значення
*r.3:prec:Точність
*s.ат:ed:Одиниця виміру
*r.0.9:min:Мінімум
*r.1:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('PSA85','PSA85','','','Air pressure after the A/H','Давление воздуха после воздухо-подогревателя','Тиск повітря після повітря-підігрівача',1,'AH2.Po2:var:Value
*r.3:prec:Precision
*s.at:ed:Unit of measure
*r.0:min:Minimum
*r.2:max:Maximum','AH2.Po2:var:Значение
*r.3:prec:Точноcть
*s.ат:ed:Единица измерения
*r.0:min:Минимум
*r.2:max:Максимум','AH2.Po2:var:Значення
*r.3:prec:Точність
*s.ат:ed:Одиниця виміру
*r.0:min:Мінімум
*r.2:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('PSA60','PSA60','','','GBF pressure after valve on the right gas pipeline','Давление ГД после заслонки на правом газопроводе','Тиск ГД після засувки на правому газопроводі',1,'9GBF.Po:var:Value
*r.3:prec:Precision
*s.at:ed:Unit of measure
*r.1:min:Minimum
*r.2:max:Maximum','9GBF.Po:var:Значение
*r.3:prec:Точность
*s.ат:ed:Единица измерения
*r.1:min:Минимум
*r.2:max:Максимум','9GBF.Po:var:Значення
*r.3:prec:Точність
*s.ат:ed:Одиниця виміру
*r.1:min:Мінімум
*r.2:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('FC104','FC104','','','GBF flow on the gas pipeline right','Расход ГД по правому газопроводу','Витрати ГД по правому газопроводу',1,'6GBF.Fi:var:Value','6GBF.Fi:var:Значение','6GBF.Fi:var:Значення');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('PSA81','PSA81','','','FW pressure on the right feeding line','Давление ВП по правой питательной линии','Тиск ВП по правій лінії живлення',1,'SrcWater.Po:var:Value
*r.2:prec:Precision
*s.at:ed:Unit of measure
*r.0:min:Minimum
*r.60:max:Maximum','SrcWater.Po:var:Значение
*r.2:prec:Точноcть
*s.ат:ed:Единица измерения
*r.0:min:Минимум
*r.60:max:Максимум','SrcWater.Po:var:Значення
*r.2:prec:Точність
*s.ат:ed:Одиниця виміру
*r.0:min:Мінімум
*r.60:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('TCA1','TCA1','','','Steam temperature after MSV','Температура пара после ГПЗ','Температура пару після ГПЗ',1,'4GN3.To:var:Value','4GN3.To:var:Значение','4GN3.To:var:Значення');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('FC101','FC101','','','Steam flow from the boiler','Расход пара из котла','Витрати пару з котла',1,'4GN3.Fo:var:Value
*r.2:prec:Precision
*s.t/h:ed:Unit of measure
*r.0:min:Minimum
*r.100:max:Maximum','4GN3.Fo:var:Значение
*r.2:prec:Точноcть
*s.т/ч:ed:Единица измерения
*r.0:min:Минимум
*r.100:max:Максимум','4GN3.Fo:var:Значення
*r.2:prec:Точність
*s.т/год:ed:Одиниця виміру
*r.0:min:Мінімум
*r.100:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('PCA51','PCA51','','','Steam pressure after the MSV','Давление пара после ГПЗ','Тиск пару після ГПЗ',1,'4GN3.Po:var:Value
*r.2:prec:Precision
*s.at:ed:Unit of measure
*r.0:min:Minimum
*r.50:max:Maximum','4GN3.Po:var:Значение
*r.2:prec:Точноcть
*s.ат:ed:Единица измерения
*r.0:min:Минимум
*r.50:max:Максимум','4GN3.Po:var:Значення
*r.2:prec:Точність
*s.ат:ed:Одиниця виміру
*r.0:min:Мінімум
*r.50:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('QA151','QA151','','','Oxygen content in FG after the superheater','Содержание кислорода в ДГ после пароперегревателя','Вміст кисню у ДГ після пароперегрівача',1,'FireChamber.O2:var:Value
*r.2:prec:Precision
*s.%:ed:Unit of measure
*r.0:min:Minimum
*r.20:max:Maximum','FireChamber.O2:var:Значение
*r.2:prec:Точноcть
*s.%:ed:Единица измерения
*r.0:min:Минимум
*r.20:max:Максимум','FireChamber.O2:var:Значення
*r.2:prec:Точність
*s.%:ed:Одиниця виміру
*r.0:min:Мінімум
*r.20:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('QA152','QA152','','','CO content in FG after the superheater','Содержание CO в ДГ после пароперегревателя','Вміст CO у ДГ після пароперегрівача',1,'FireChamber.CO:var:Value
*r.2:prec:Precision
*s.%:ed:Unit of measure
*r.0:min:Minimum
*r.20:max:Maximum','FireChamber.CO:var:Значение
*r.2:prec:Точноcть
*s.%:ed:Единица измерения
*r.0:min:Минимум
*r.20:max:Максимум','FireChamber.CO:var:Значення
*r.2:prec:Точність
*s.%:ed:Одиниця виміру
*r.0:min:Мінімум
*r.20:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('PSA64','PSA64','','','GC pressure after the regulating valve ','Давление ГК после регулирующего клапана','Тиск ГК після регулюючого клапану',1,'4GC.Po:var:Value
*r.3:prec:Precision
*s.at:ed:Unit of measure
*r.0:min:Minimum
*r.2:max:Maximum','4GC.Po:var:Значение
*r.3:prec:Точноcть
*s.ат:ed:Единица измерения
*r.0:min:Минимум
*r.2:max:Максимум','4GC.Po:var:Значення
*r.3:prec:Точність
*s.ат:ed:Одиниця виміру
*r.0:min:Мінімум
*r.2:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('FC102_1','FC102 1','','','Natural gas flow after 4GN','Расход природного газа после 4ГП','Витрати природного газу після 4ГП',1,'4GN.Fo:var:Value','4GN.Fo:var:Значение','4GN.Fo:var:Значення');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('FC102_2','FC102 2','','','Natural gas flow after 5GN','Расход природного газа после 5ГП','Витрати природного газу після 5ГП',1,'5GN.Fo:var:Value','5GN.Fo:var:Значение','5GN.Fo:var:Значення');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('FC102_3','FC102 3','','','Natural gas flow after 6GN','Расход природного газа после 6ГП','Витрати природного газу після 6ГП',1,'6GN.Fo:var:Value','6GN.Fo:var:Значение','6GN.Fo:var:Значення');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('FC102_0','FC102 0','','','Natural gas flow after the source','Расход природного газа после источника','Витрати природного газу після джерела',1,'SrcNG.Fo:var:Value','SrcNG.Fo:var:Значение','SrcNG.Fo:var:Значення');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('l_3GN','l 3GN','','','3GN position','Положение 3ГП','Положення 3ГП',1,'3GN.l_v1:var:Value','3GN.l_v1:var:Значение','3GN.l_v1:var:Значення');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('l_4GN','l 4GN','','','4GN position','Положение 4ГП','Положення 4ГП',1,'4GN.l_v1:var:Value','4GN.l_v1:var:Значение','4GN.l_v1:var:Значення');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('l_5GN','l 5GN','','','5GN position','Положение 5ГП','Положення 5ГП',1,'5GN.l_v1:var:Value','5GN.l_v1:var:Значение','5GN.l_v1:var:Значення');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('l_6GN_1','l 6GN 1','','','6GN_1 position','Положение 6ГП_1','Положення 6ГП_1',1,'6GN.l_v1:var:Value','6GN.l_v1:var:Значение','6GN.l_v1:var:Значення');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('l_6GN_2','l 6GN 2','','','6GN_2 position','Положение 6ГП_2','Положення 6ГП_2',1,'6GN.l_v2:var:Value','6GN.l_v2:var:Значение','6GN.l_v2:var:Значення');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('P_GN_S','P GN S','','','GN pressure after the source','Давление ГП после источника','Тиск ГП після джерела',1,'SrcNG.Po:var:Value','SrcNG.Po:var:Значение','SrcNG.Po:var:Значення');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('P_4GN','P 4GN','','','GN pressure after 4GN','Давление ГП после 4ГП','Тиск ГП після 4ГП',1,'4GN.Po:var:Value','4GN.Po:var:Значение','4GN.Po:var:Значення');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('Src_GN_Pi','Src GN Pi','','','Input pressure at the source of GN','Входное давление на источнике ГП','Вхідний тиск на джерелі ГП',1,'SrcNG.Pi:var:Value','SrcNG.Pi:var:Значение','SrcNG.Pi:var:Значення');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('l_3FW_1','l 3FW 1','','','3FW(1) position','Положение 3ВП(1)','Положення 3ВП(1)',1,'3FW.l_v1:var:Value','3FW.l_v1:var:Значение','3FW.l_v1:var:Значення');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('l_5FW','l 5FW','','','5FW(1) position','Положение 5ВП(1)','Положення 5ВП(1)',1,'5FW.l_v1:var:Value','5FW.l_v1:var:Значение','5FW.l_v1:var:Значення');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('l_5FW_2','l 5FW 2','','','5FW(2) position','Положение 5ВП(2)','Положення 5ВП(2)',1,'5FW.l_v2:var:Value','5FW.l_v2:var:Значение','5FW.l_v2:var:Значення');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('l_7FW','l 7FW','','','7FW position','Положение 7ВП','Положення 7ВП',1,'7FW.l_v1:var:Value','7FW.l_v1:var:Значение','7FW.l_v1:var:Значення');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('F_3FW','F 3FW','','','Water flow after 3FW','Расход воды после 3ВП','Витрати води після 3ВП',1,'3FW.Fo:var:Value','3FW.Fo:var:Значение','3FW.Fo:var:Значення');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('F_7FW','F 7FW','','','Water flow after 7FW','Расход воды после 7ВП','Витрати води після 7ВП',1,'7FW.Fo:var:Value','7FW.Fo:var:Значение','7FW.Fo:var:Значення');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('F_Node8','F Node8','','','Water flow after Node8','Расход воды после Уз8','Витрати води після Вуз8',1,'Node8.Fo:var:Value','Node8.Fo:var:Значение','Node8.Fo:var:Значення');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('F_5FW','F 5FW','','','Water flow after 5FW','Расход воды после 5ВП','Витрати води після 5ВП',1,'5FW.Fo:var:Value','5FW.Fo:var:Значение','5FW.Fo:var:Значення');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('l_7GBF','l 7GBF','','','7GBF position','Положение 7ГД','Положення 7ГД',1,'7GBF.l_v1:var:Value','7GBF.l_v1:var:Значение','7GBF.l_v1:var:Значення');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('l_8GBF','l 8GBF','','','8GBF position','Положение 8ГД','Положення 8ГД',1,'8GBF.l_v1:var:Value','8GBF.l_v1:var:Значение','8GBF.l_v1:var:Значення');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('l_5GC','l 5GC','','','5GC position','Положение 5ГК','Положення 5ГК',1,'5GC.l_v1:var:Value','5GC.l_v1:var:Значение','5GC.l_v1:var:Значення');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('LСA122','LСA122','','','Level of water in the clean slot of the drum, in the right','Уровень воды в чистом отсеке барабана, справа','Рівень води у чистому відсіку барабану, праворуч',1,'Drum.Lo:var:Value
*r.0:prec:Precision
*s.%:ed:Unit of measure
*r.0:min:Minimum
*r.100:max:Maximum','Drum.Lo:var:Значение
*r.0:prec:Точноcть
*s.%:ed:Единица измерения
*r.0:min:Минимум
*r.100:max:Максимум','Drum.Lo:var:Значення
*r.0:prec:Точність
*s.%:ed:Одиниця виміру
*r.0:min:Мінімум
*r.100:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('P61','P61','','','GBF pressure before the left burner','Давл. ГД перед левой горелкой','Тиск ГД перед лівою горелкою',1,'9GBF.Po:var:Value
*r.2:prec:Precision
*s.at:ed:Unit of measure
*r.0:min:Minimum
*r.1.6:max:Maximum','9GBF.Po:var:Значение
*r.2:prec:Точноcть
*s.ат:ed:Единица измерения
*r.0:min:Минимум
*r.1.6:max:Максимум','9GBF.Po:var:Значення
*r.2:prec:Точність
*s.ат:ed:Одиниця виміру
*r.0:min:Мінімум
*r.1.6:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('P62','P62','','','GBF pressure before the right burner','Давление ГД перед правой горелкой','Тиск ГД перед правою горелкою',1,'10GBF.Po:var:Value
*r.2:prec:Precision
*s.at:ed:Unit of measure
*r.0:min:Minimum
*r.1.6:max:Maximum','10GBF.Po:var:Значение
*r.2:prec:Точноcть
*s.ат:ed:Единица измерения
*r.0:min:Минимум
*r.1.6:max:Максимум','10GBF.Po:var:Значення
*r.2:prec:Точність
*s.ат:ed:Одиниця виміру
*r.0:min:Мінімум
*r.1.6:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('P65','P65','','','GC pressure before the left burner','Давление ГК перед левой горелкой','Тиск ГК перед лівою горелкою',1,'6GC.Po:var:Value
*r.2:prec:Precision
*s.at:ed:Unit of measure
*r.0:min:Minimum
*r.1.6:max:Maximum','6GC.Po:var:Значение
*r.2:prec:Точноcть
*s.ат:ed:Единица измерения
*r.0:min:Минимум
*r.1.6:max:Максимум','6GC.Po:var:Значення
*r.2:prec:Точність
*s.ат:ed:Одиниця виміру
*r.0:min:Мінімум
*r.1.6:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('P66','P66','','','GC pressure before the right burner','Давление ГК перед правой горелкой','Тиск ГК перед правою горелкою',1,'6GC.Po:var:Value
*r.2:prec:Precision
*s.at:ed:Unit of measure
*r.0:min:Minimum
*r.1.6:max:Maximum','6GC.Po:var:Значение
*r.2:prec:Точноcть
*s.ат:ed:Единица измерения
*r.0:min:Минимум
*r.1.6:max:Максимум','6GC.Po:var:Значення
*r.2:prec:Точність
*s.ат:ed:Одиниця виміру
*r.0:min:Мінімум
*r.1.6:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('P67','P67','','','Air pressure before the first stage of the A/H to the left','Давление воздуха до первой ступени В/П слева.','Тиск повітря перед першим ступенем В/П ліворуч.',1,'Node9.Po:var:Value
*r.2:prec:Precision
*s.at:ed:Unit of measure
*r.0:min:Minimum
*r.1.2:max:Maximum','Node9.Po:var:Значение
*r.2:prec:Точноcть
*s.ат:ed:Единица измерения
*r.0:min:Минимум
*r.1.2:max:Максимум','Node9.Po:var:Значення
*r.2:prec:Точність
*s.ат:ed:Одиниця виміру
*r.0:min:Мінімум
*r.1.2:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('P73','P73','','','Air pressure in the upper tier of the right burner','Давление воздуха на верхнем ярусе правой горелки','Тиск повітря на верхньому ярусі правої горелки',1,'Node2.Po:var:Value
*r.2:prec:Precision
*s.at:ed:Unit of measure
*r.0:min:Minimum
*r.1.16:max:Maximum','Node2.Po:var:Значение
*r.2:prec:Точноcть
*s.ат:ed:Единица измерения
*r.0:min:Минимум
*r.1.16:max:Максимум','Node2.Po:var:Значення
*r.2:prec:Точність
*s.ат:ed:Одиниця виміру
*r.0:min:Мінімум
*r.1.16:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('P68','P68','','','Air pressure before the first stage of the A/H to the right','Давление воздуха до первой ступени В/П справа','Тиск повітря перед першим ступенем В/П праворуч',1,'Node9.Po:var:Value
*r.2:prec:Precision
*s.at:ed:Unit of measure
*r.0:min:Minimum
*r.1.2:max:Maximum','Node9.Po:var:Значение
*r.2:prec:Точноcть
*s.ат:ed:Единица измерения
*r.0:min:Минимум
*r.200:max:Максимум','Node9.Po:var:Значення
*r.2:prec:Точність
*s.ат:ed:Одиниця виміру
*r.0:min:Мінімум
*r.200:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('P75','P75','','','Air pressure on the lower tier of the right burner','Давление воздуха на нижнем ярусе правой горелки','Тиск повітря на нижньому ярусі правої горелки',1,'Node2.Po:var:Value
*r.2:prec:Precision
*s.at:ed:Unit of measure
*r.0:min:Minimum
*r.1.16:max:Maximum','Node2.Po:var:Значение
*r.2:prec:Точноcть
*s.ат:ed:Единица измерения
*r.0:min:Минимум
*r.1.16:max:Максимум','Node2.Po:var:Значення
*r.2:prec:Точність
*s.ат:ed:Одиниця виміру
*r.0:min:Мінімум
*r.1.16:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('LCVG121','LCVG121','','','3FWL position','Положение 3ВПл','Положення 3ВПл',1,'3FW.l_v1:var:Value
*r.0:prec:Precision
*s.%:ed:Unit of measure
*r.0:min:Minimum
*r.100:max:Maximum','3FW.l_v1:var:Значение
*r.0:prec:Точноcть
*s.%:ed:Единица измерения
*r.0:min:Минимум
*r.100:max:Максимум','3FW.l_v1:var:Значення
*r.0:prec:Точність
*s.%:ed:Одиниця виміру
*r.0:min:Мінімум
*r.100:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('P78','P78','','','Vacuum in the "SE-A" front','Разрежение перед "ДС-А"','Розрідження перед "ДС-А"',1,'1G.Po:var:Value
*r.2:prec:Precision
*s.at:ed:Unit of measure
*r.0.9:min:Minimum
*r.1:max:Maximum','1G.Po:var:Значение
*r.2:prec:Точноcть
*s.ат:ed:Единица измерения
*r.0.9:min:Минимум
*r.1:max:Максимум','1G.Po:var:Значення
*r.2:prec:Точність
*s.ат:ed:Одиниця виміру
*r.0.9:min:Мінімум
*r.1:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('LCVG122','LCVG122','','','3FWR position','Положение 3ВПп','Положення 3ВПп',1,'3FW.l_v2:var:Value
*r.0:prec:Precision
*s.%:ed:Unit of measure
*r.0:min:Minimum
*r.100:max:Maximum','3FW.l_v2:var:Значение
*r.0:prec:Точноcть
*s.%:ed:Единица измерения
*r.0:min:Минимум
*r.100:max:Максимум','3FW.l_v2:var:Значення
*r.0:prec:Точність
*s.%:ed:Одиниця виміру
*r.0:min:Мінімум
*r.100:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('P79','P79','','','Vacuum in the "SE-B" front','Разрежение перед "ДС-Б"','Розрідження перед "ДС-Б"',1,'2G.Po:var:Value
*r.2:prec:Precision
*s.at:ed:Unit of measure
*r.0.9:min:Minimum
*r.1:max:Maximum','2G.Po:var:Значение
*r.2:prec:Точноcть
*s.ат:ed:Единица измерения
*r.0.9:min:Минимум
*r.1:max:Максимум','2G.Po:var:Значення
*r.2:prec:Точність
*s.ат:ed:Одиниця виміру
*r.0.9:min:Мінімум
*r.1:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('G_13SH','G 13SH','','','13G position','Положение 13Ш','Положення 13Ш',1,'13G.l_v1:var:Value','13G.l_v1:var:Значение','13G.l_v1:var:Значення');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('PCSA77','PCSA77','','','Vacuum in the fire chamber on the right','Разрежение в топке справа','Розрідження у топці праворуч',1,'FireChamber.Po:var:Value
*r.3:prec:Precision
*s.at:ed:Unit of measure
*r.0.9:min:Minimum
*r.1:max:Maximum','FireChamber.Po:var:Значение
*r.3:prec:Точноcть
*s.ат:ed:Единица измерения
*r.0.9:min:Минимум
*r.1:max:Максимум','FireChamber.Po:var:Значення
*r.3:prec:Точність
*s.ат:ed:Одиниця виміру
*r.0.9:min:Мінімум
*r.1:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('F109','F109','','','Water flow for the thermostat','Расход воды на терморегулятор','Витрати води на терморегулятор',1,'7FW.Fo:var:Value
*r.2:prec:Precision
*s.t/h:ed:Unit of measure
*r.0:min:Minimum
*r.200:max:Maximum','7FW.Fo:var:Значение
*r.2:prec:Точноcть
*s.т/ч:ed:Единица измерения
*r.0:min:Минимум
*r.200:max:Максимум','7FW.Fo:var:Значення
*r.2:prec:Точність
*s.т/год:ed:Одиниця виміру
*r.0:min:Мінімум
*r.200:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('G_14SH','G 14SH','','','14G position','Положение 14Ш','Положення 14Ш',1,'13G.l_v2:var:Value','13G.l_v2:var:Значение','13G.l_v2:var:Значення');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('PSA57_1','PSA57 1','','','GBF pressure on the general pipeline','Давление ГД на общем трубопроводе','Тиск ГД на загальному трубопроводі',1,'Node3.Pi:var:Value
*r.2:prec:Precision
*s.at:ed:Unit of measure
*r.0:min:Minimum
*r.2:max:Maximum','Node3.Pi:var:Значение
*r.2:prec:Точноcть
*s.ат:ed:Единица измерения
*r.0:min:Минимум
*r.2:max:Максимум','Node3.Pi:var:Значення
*r.2:prec:Точність
*s.ат:ed:Одиниця виміру
*r.0:min:Мінімум
*r.2:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('PSA57_2','PSA57 2','','','GBF pressure on the general pipeline','Давление ГД на общем трубопроводе','Тиск ГД на загальному трубопроводі',1,'Node3.Pi:var:Value
*r.2:prec:Precision
*s.at:ed:Unit of measure
*r.0:min:Minimum
*r.2:max:Maximum','Node3.Pi:var:Значение
*r.2:prec:Точноcть
*s.ат:ed:Единица измерения
*r.0:min:Минимум
*r.2:max:Максимум','Node3.Pi:var:Значення
*r.2:prec:Точність
*s.ат:ed:Одиниця виміру
*r.0:min:Мінімум
*r.2:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('FCVG102','FCVG102','','','5GN position','Положение 5ГП','Положення 5ГП',1,'5GN.l_v1:var:Value','5GN.l_v1:var:Значение','5GN.l_v1:var:Значення');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('FCVG104','FCVG104','','','8GBF position','Положение 8ГД','Положення 8ГД',1,'8GBF.l_v1:var:Value','8GBF.l_v1:var:Значение','8GBF.l_v1:var:Значення');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('FCVG105','FCVG105','','','4GC position','Положение 4ГК','Положення 4ГК',1,'4GC.l_v1:var:Value','4GC.l_v1:var:Значение','4GC.l_v1:var:Значення');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('LSA123','LSA123','','','Level of water in the clean slot of the drum, in the left','Уровень воды в чистом отсеке барабана, слева','Рівень води у чистому відсіку барабану, ліворуч',1,'Drum.Lo:var:Value
*r.0:prec:Precision
*s.%:ed:Unit of measure
*r.0:min:Minimum
*r.100:max:Maximum','Drum.Lo:var:Значение
*r.0:prec:Точноcть
*s.%:ed:Единица измерения
*r.0:min:Минимум
*r.100:max:Максимум','Drum.Lo:var:Значення
*r.0:prec:Точність
*s.%:ed:Одиниця виміру
*r.0:min:Мінімум
*r.100:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('LSA124','LSA124','','','The level of water in a clean drum slot, right','Уровень воды в чистом отсеке барабана, справа','Рівень води у чистому відсіку барабану, праворуч',1,'Drum.Lo:var:Value
*r.0:prec:Precision
*s.%:ed:Unit of measure
*r.0:min:Minimum
*r.100:max:Maximum','Drum.Lo:var:Значение
*r.0:prec:Точноcть
*s.%:ed:Единица измерения
*r.0:min:Минимум
*r.100:max:Максимум','Drum.Lo:var:Значення
*r.0:prec:Точність
*s.%:ed:Одиниця виміру
*r.0:min:Мінімум
*r.100:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('PSA63_1','PSA63 1','','','GC pressure after 5GC','Давление ГК после 5ГК','Тиск ГК після 5ГК',1,'5GC.Po:var:Value
*r.2:prec:Precision
*s.at:ed:Unit of measure
*r.0:min:Minimum
*r.2:max:Maximum','5GC.Po:var:Значение
*r.2:prec:Точноcть
*s.ат:ed:Единица измерения
*r.0:min:Минимум
*r.2:max:Максимум','5GC.Po:var:Значення
*r.2:prec:Точність
*s.ат:ed:Одиниця виміру
*r.0:min:Мінімум
*r.2:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('QA153','QA153','','','Oxygen content in the exhaust FG','Содержание кислорода в уходящих ДГ','Вміст кисню у відхідних ДГ',1,'FireChamber.O2:var:Value
*r.2:prec:Precision
*s.%:ed:Unit of measure
*r.0:min:Minimum
*r.20:max:Maximum','FireChamber.O2:var:Значение
*r.2:prec:Точноcть
*s.%:ed:Единица измерения
*r.0:min:Минимум
*r.20:max:Максимум','FireChamber.O2:var:Значення
*r.2:prec:Точність
*s.%:ed:Одиниця виміру
*r.0:min:Мінімум
*r.20:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('PSA63_2','PSA63 2','','','GC pressure after 3GC','Давление ГК после 3ГК','Тиск ГК після 3ГК',1,'3GC.Po:var:Value
*r.2:prec:Precision
*s.at:ed:Unit of measure
*r.0:min:Minimum
*r.2:max:Maximum','3GC.Po:var:Значение
*r.2:prec:Точноcть
*s.ат:ed:Единица измерения
*r.0:min:Минимум
*r.2:max:Максимум','3GC.Po:var:Значення
*r.2:prec:Точність
*s.ат:ed:Одиниця виміру
*r.0:min:Мінімум
*r.2:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('TA25','TA25','','','FG temperature before the "SE-A"','Температура ДГ перед "ДС-А"','Температура ДГ перед "ДС-А"',1,'1G.To:var:Value
*r.2:prec:Precision
*s.deg.K:ed:Unit of measure
*r.273:min:Minimum
*r.673:max:Maximum','1G.To:var:Значение
*r.2:prec:Точноcть
*s.град.К:ed:Единица измерения
*r.273:min:Минимум
*r.673:max:Максимум','1G.To:var:Значення
*r.2:prec:Точність
*s.град.К:ed:Одиниця виміру
*r.273:min:Мінімум
*r.673:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('TA26','TA26','','','FG temperature before the "SE-B"','Температура ДГ перед "ДС-Б"','Температура ДГ перед "ДС-Б"',1,'2G.To:var:Value
*r.2:prec:Precision
*s.deg.K:ed:Unit of measure
*r.273:min:Minimum
*r.673:max:Maximum','2G.To:var:Значение
*r.2:prec:Точноcть
*s.град.К:ed:Единица измерения
*r.273:min:Минимум
*r.673:max:Максимум','2G.To:var:Значення
*r.2:prec:Точність
*s.град.К:ed:Одиниця виміру
*r.273:min:Мінімум
*r.673:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('T13','T13','','','FG temperature before the superheater on the left','Температура ДГ перед пароперегревателем слева','Температура ДГ перед пароперегрівачем ліворуч',1,'Drum.To2:var:Value
*r.2:prec:Precision
*s.deg.К:ed:Unit of measure
*r.273:min:Minimum
*r.1027:max:Maximum','Drum.To2:var:Значение
*r.2:prec:Точноcть
*s.град.К:ed:Единица измерения
*r.273:min:Минимум
*r.1027:max:Максимум','Drum.To2:var:Значення
*r.2:prec:Точність
*s.град.К:ed:Одиниця виміру
*r.273:min:Мінімум
*r.1027:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('T14','T14','','','FG temperature before the superheater on the right','Температура ДГ перед пароперегревателем справа','Температура ДГ перед пароперегрівачем праворуч',1,'Drum.To2:var:Value
*r.2:prec:Precision
*s.deg.К:ed:Unit of measure
*r.273:min:Minimum
*r.1027:max:Maximum','Drum.To2:var:Значение
*r.2:prec:Точноcть
*s.град.К:ed:Единица измерения
*r.273:min:Минимум
*r.1073:max:Максимум','Drum.To2:var:Значення
*r.2:prec:Точність
*s.град.К:ed:Одиниця виміру
*r.273:min:Мінімум
*r.1073:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('T20','T20','','','FG temperature before the first stage of the A/H on the right','Температура ДГ перед первой ступенью воздухо-перегревателя справа','Температура ДГ перед першим ступенем повітря-перегрівача праворуч',1,'EC1.To1:var:Value
*r.2:prec:Precision
*s.deg.K:ed:Unit of measure
*r.273:min:Minimum
*r.873:max:Maximum','EC1.To1:var:Значение
*r.2:prec:Точноcть
*s.град.К:ed:Единица измерения
*r.273:min:Минимум
*r.873:max:Максимум','EC1.To1:var:Значення
*r.2:prec:Точність
*s.град.К:ed:Одиниця виміру
*r.273:min:Мінімум
*r.873:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('T15','T15','','','FG temperature before the second stage of the economizer on the left','Температура ДГ перед второй ступенью экономайзера слева','Температура ДГ перед другим ступенем економайзеру лівору',1,'OhS.To1:var:Value
*r.2:prec:Precision
*s.deg.K:ed:Unit of measure
*r.273:min:Minimum
*r.873:max:Maximum','OhS.To1:var:Значение
*r.2:prec:Точноcть
*s.град.К:ed:Единица измерения
*r.273:min:Минимум
*r.873:max:Максимум','OhS.To1:var:Значення
*r.2:prec:Точність
*s.град.К:ed:Одиниця виміру
*r.273:min:Мінімум
*r.873:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('T21','T21','','','FG temperature before the second stage of the A/H on the left','Температура ДГ перед второй ступенью воздухо-перегревателя слева','Температура ДГ перед другим ступенем повітря-перегрівача ліворуч',1,'EC2.To1:var:Value
*r.2:prec:Precision
*s.deg.K:ed:Unit of measure
*r.273:min:Minimum
*r.873:max:Maximum','EC2.To1:var:Значение
*r.2:prec:Точноcть
*s.град.К:ed:Единица измерения
*r.273:min:Минимум
*r.873:max:Максимум','EC2.To1:var:Значення
*r.2:prec:Точність
*s.град.К:ed:Одиниця виміру
*r.273:min:Мінімум
*r.873:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('T16','T16','','','FG temperature before the second stage of the economizer on the right','Температура ДГ перед второй ступенью экономайзера справа','Температура ДГ перед другим ступенем економайзеру праворуч',1,'OhS.To1:var:Value
*r.2:prec:Precision
*s.deg.K:ed:Unit of measure
*r.273:min:Minimum
*r.873:max:Maximum','OhS.To1:var:Значение
*r.2:prec:Точноcть
*s.град.К:ed:Единица измерения
*r.273:min:Минимум
*r.873:max:Максимум','OhS.To1:var:Значення
*r.2:prec:Точність
*s.град.К:ed:Одиниця виміру
*r.273:min:Мінімум
*r.873:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('T22','T22','','','FG temperature before the second stage of the A/H on the right','Температура ДГ перед второй ступенью воздухо-перегревателя справа','Температура ДГ перед другим ступенем повітря-перегрівача праворуч',1,'EC2.To1:var:Value
*r.2:prec:Precision
*s.deg.K:ed:Unit of measure
*r.273:min:Minimum
*r.873:max:Maximum','EC2.To1:var:Значение
*r.2:prec:Точноcть
*s.град.К:ed:Единица измерения
*r.273:min:Минимум
*r.873:max:Максимум','EC2.To1:var:Значення
*r.2:prec:Точність
*s.град.К:ed:Одиниця виміру
*r.273:min:Мінімум
*r.873:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('T17','T17','','','FG temperature after the second stage of the economizer on the left','Температура ДГ после второй ступени экономайзера слева','Температура ДГ після другого ступеня економайзеру ліворуч',1,'EC2.To1:var:Value
*r.2:prec:Precision
*s.deg.K:ed:Unit of measure
*r.273:min:Minimum
*r.873:max:Maximum','EC2.To1:var:Значение
*r.2:prec:Точноcть
*s.град.К:ed:Единица измерения
*r.273:min:Минимум
*r.873:max:Максимум','EC2.To1:var:Значення
*r.2:prec:Точність
*s.град.К:ed:Одиниця виміру
*r.273:min:Мінімум
*r.873:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('T3','T3','','','GBF temperature','Температура ГД','Температура ГД',1,'5GBF.Ti:var:Value
*r.2:prec:Precision
*s.deg.K:ed:Unit of measure
*r.273:min:Minimum
*r.373:max:Maximum','5GBF.Ti:var:Значение
*r.2:prec:Точноcть
*s.град.К:ed:Единица измерения
*r.273:min:Минимум
*r.373:max:Максимум','5GBF.Ti:var:Значення
*r.2:prec:Точність
*s.град.К:ed:Одиниця виміру
*r.273:min:Мінімум
*r.373:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('T23','T23','','','FG temperature before the first stage of the economizer on the left','Температура ДГ перед первой ступенью экономайзера слева','Температура ДГ перед першим ступенем економайзеру ліворуч',1,'AH2.To1:var:Value
*r.2:prec:Precision
*s.deg.K:ed:Unit of measure
*r.273:min:Minimum
*r.873:max:Maximum','AH2.To1:var:Значение
*r.2:prec:Точноcть
*s.град.К:ed:Единица измерения
*r.273:min:Минимум
*r.873:max:Максимум','AH2.To1:var:Значення
*r.2:prec:Точність
*s.град.К:ed:Одиниця виміру
*r.273:min:Мінімум
*r.873:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('T18','T18','','','FG temperature after the secind stage of the economizer on the right','Температура ДГ после второй ступени экономайзера справа','Температура ДГ після другого ступеня економайзеру праворуч',1,'EC2.To1:var:Value
*r.2:prec:Precision
*s.deg.K:ed:Unit of measure
*r.273:min:Minimum
*r.873:max:Maximum','EC2.To1:var:Значение
*r.2:prec:Точноcть
*s.град.К:ed:Единица измерения
*r.273:min:Минимум
*r.873:max:Максимум','EC2.To1:var:Значення
*r.2:prec:Точність
*s.град.К:ed:Одиниця виміру
*r.273:min:Мінімум
*r.873:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('PSA53_1','PSA53 1','','','GN pressure before the diaphragm','Давление ГП перед диафрагмой','Тиск ГП перед діафрагмою',1,'3GN.Po:var:Value
*r.2:prec:Precision
*s.at:ed:Unit of measure
*r.0:min:Minimum
*r.40:max:Maximum','3GN.Po:var:Значение
*r.2:prec:Точноcть
*s.ат:ed:Единица измерения
*r.0:min:Минимум
*r.40:max:Максимум','3GN.Po:var:Значення
*r.2:prec:Точність
*s.ат:ed:Одиниця виміру
*r.0:min:Мінімум
*r.40:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('PSA53_2','PSA53 2','','','GN pressure before the diaphragm','Давление ГП перед диафрагмой','Тиск ГП перед діафрагмою',1,'3GN.Po:var:Value
*r.2:prec:Precision
*s.at:ed:Unit of measure
*r.0:min:Minimum
*r.40:max:Maximum','3GN.Po:var:Значение
*r.2:prec:Точноcть
*s.ат:ed:Единица измерения
*r.0:min:Минимум
*r.40:max:Максимум','3GN.Po:var:Значення
*r.2:prec:Точність
*s.ат:ed:Одиниця виміру
*r.0:min:Мінімум
*r.40:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('PSA80','PSA80','','','FW pressure on the left feeding line','Давление ВП по левой питательной линии','Тиск ВП по лівій лінії живлення',1,'SrcWater.Po:var:Value
*r.2:prec:Precision
*s.at:ed:Unit of measure
*r.0:min:Minimum
*r.60:max:Maximum','SrcWater.Po:var:Значение
*r.2:prec:Точноcть
*s.ат:ed:Единица измерения
*r.0:min:Минимум
*r.60:max:Максимум','SrcWater.Po:var:Значення
*r.2:prec:Точність
*s.ат:ed:Одиниця виміру
*r.0:min:Мінімум
*r.60:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('FCVG103','FCVG103','','','7GBF position','Положение 7ГД','Положення 7ГД',1,'7GBF.l_v1:var:Value','7GBF.l_v1:var:Значение','7GBF.l_v1:var:Значення');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('T24','T24','','','FG temperature before the first stage of the economizer on the right','Температура ДГ перед первой ступенью экономайзера справа','Температура ДГ перед першим ступенем економайзеру праворуч',1,'AH2.To1:var:Value
*r.2:prec:Precision
*s.deg.K:ed:Unit of measure
*r.273:min:Minimum
*r.873:max:Maximum','AH2.To1:var:Значение
*r.2:prec:Точноcть
*s.град.К:ed:Единица измерения
*r.273:min:Минимум
*r.873:max:Максимум','AH2.To1:var:Значення
*r.2:prec:Точність
*s.град.К:ed:Одиниця виміру
*r.273:min:Мінімум
*r.873:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('T19','T19','','','FG temperature before the first stage of the A/H on the left','Температура ДГ перед первой ступенью воздухо-перегревателя слева','Температура ДГ перед першим ступенем повітря-перегрівача ліворуч',1,'EC1.To1:var:Value
*r.2:prec:Precision
*s.deg.K:ed:Unit of measure
*r.273:min:Minimum
*r.873:max:Maximum','EC1.To1:var:Значение
*r.2:prec:Точноcть
*s.град.К:ed:Единица измерения
*r.273:min:Минимум
*r.873:max:Максимум','EC1.To1:var:Значення
*r.2:prec:Точність
*s.град.К:ed:Одиниця виміру
*r.273:min:Мінімум
*r.873:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('T5','T5','','','GC temperature on the boiler','Температура ГК на котел','Температура ГК на котел',1,'4GC.To:var:Value
*r.2:prec:Precision
*s.deg.K:ed:Unit of measure
*r.273:min:Minimum
*r.373:max:Maximum','4GC.To:var:Значение
*r.2:prec:Точноcть
*s.град.К:ed:Единица измерения
*r.273:min:Минимум
*r.373:max:Максимум','4GC.To:var:Значення
*r.2:prec:Точність
*s.град.К:ed:Одиниця виміру
*r.273:min:Мінімум
*r.373:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('T7','T7','','','Air temperature after the second stage of the A/H on the left','Температура воздуха после второй ступени воздухо-перегревателя слева','Температура повітря після другого ступеню повітря-перегрівача ліворуч',1,'AH2.To2:var:Value
*r.2:prec:Precision
*s.deg.K:ed:Unit of measure
*r.273:min:Minimum
*r.773:max:Maximum','AH2.To2:var:Значение
*r.2:prec:Точноcть
*s.град.К:ed:Единица измерения
*r.273:min:Минимум
*r.773:max:Максимум','AH2.To2:var:Значення
*r.2:prec:Точність
*s.град.К:ed:Одиниця виміру
*r.273:min:Мінімум
*r.773:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('T8','T8','','','Air temperature after the second stage of the A/H on the right','Температура воздуха после второй ступени воздухо-перегревателя справа','Температура повітря після другого ступеню повітря-перегрівача праворуч',1,'AH2.To2:var:Value
*r.2:prec:Precision
*s.deg.K:ed:Unit of measure
*r.273:min:Minimum
*r.773:max:Maximum','AH2.To2:var:Значение
*r.2:prec:Точноcть
*s.град.К:ed:Единица измерения
*r.273:min:Минимум
*r.773:max:Максимум','AH2.To2:var:Значення
*r.2:prec:Точність
*s.град.К:ed:Одиниця виміру
*r.273:min:Мінімум
*r.773:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('T35','T35','','','Temperature of the A/H on the left feeding line','Температура воды по левой питательной линии','Температура повітря-перегрівача по лівій лінії живлення',1,'3FW.To:var:Value
*r.2:prec:Precision
*s.deg.K:ed:Unit of measure
*r.273:min:Minimum
*r.473:max:Maximum','3FW.To:var:Значение
*r.2:prec:Точноcть
*s.град.К:ed:Единица измерения
*r.273:min:Минимум
*r.473:max:Максимум','3FW.To:var:Значення
*r.2:prec:Точність
*s.град.К:ed:Одиниця виміру
*r.273:min:Мінімум
*r.473:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('T36','T36','','','Temperature of the A/H on the right feeding line','Температура воды по правой питательной линии','Температура повітря-перегрівача по правій лінії живлення',1,'3FW.To:var:Value
*r.2:prec:Precision
*s.deg.K:ed:Unit of measure
*r.273:min:Minimum
*r.473:max:Maximum','3FW.To:var:Значение
*r.2:prec:Точноcть
*s.град.К:ed:Единица измерения
*r.273:min:Минимум
*r.473:max:Максимум','3FW.To:var:Значення
*r.2:prec:Точність
*s.град.К:ed:Одиниця виміру
*r.273:min:Мінімум
*r.473:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('T37','T37','','','Water temperature after the economizer on the left','Температура воды после экономайзера слева','Температура води після економайзеру ліворуч',1,'EC2.To2:var:Value
*r.2:prec:Precision
*s.deg.K:ed:Unit of measure
*r.273:min:Minimum
*r.673:max:Maximum
','EC2.To2:var:Значение
*r.2:prec:Точноcть
*s.град.К:ed:Единица измерения
*r.273:min:Минимум
*r.673:max:Максимум
','EC2.To2:var:Значення
*r.2:prec:Точність
*s.град.К:ed:Одиниця виміру
*r.273:min:Мінімум
*r.673:max:Максимум
');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('T38','T38','','','Water temperature after the economizer on the right','Температура воды после экономайзера справа','Температура води після економайзеру праворуч',1,'EC2.To2:var:Value
*r.2:prec:Precision
*s.deg.K:ed:Unit of measure
*r.273:min:Minimum
*r.673:max:Maximum','EC2.To2:var:Значение
*r.2:prec:Точноcть
*s.град.К:ed:Единица измерения
*r.273:min:Минимум
*r.673:max:Максимум','EC2.To2:var:Значення
*r.2:prec:Точність
*s.град.К:ed:Одиниця виміру
*r.273:min:Мінімум
*r.673:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('PCA52','PCA52','','','Steam pressure in the boiler drum','Давление пара в барабане котла','Тиск пару у барабані котла',1,'Drum.Po1:var:Value
*r.2:prec:Precision
*s.at:ed:Unit of measure
*r.0:min:Minimum
*r.50:max:Maximum','Drum.Po1:var:Значение
*r.2:prec:Точность
*s.ат:ed:Единица измерения
*r.0:min:Минимум
*r.50:max:Максимум','Drum.Po1:var:Значення
*r.2:prec:Точність
*s.ат:ed:Одиниця виміру
*r.0:min:Мінімум
*r.50:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('PSA52_1','PSA52 1','','','Steam pressure in the boiler drum','Давление пара в барабане котла','Тиск пару у барабані котла',1,'Drum.Po1:var:Value
*r.2:prec:Precision
*s.at:ed:Unit of measure
*r.0:min:Minimum
*r.40:max:Maximum','Drum.Po1:var:Значение
*r.2:prec:Точноcть
*s.ат:ed:Единица измерения
*r.0:min:Минимум
*r.40:max:Максимум','Drum.Po1:var:Значення
*r.2:prec:Точність
*s.ат:ed:Одиниця виміру
*r.0:min:Мінімум
*r.40:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('FA110','FA110','','','Air-superheater flow on the left feeding line','Расход воздухо-перегревателя по левой питательной линии','Витрати повітря-перегрівача по лівій лінії живлення',1,'SrcWater.Fo:var:Value
*r.2:prec:Precision
*s.t/h:ed:Unit of measure
*r.0:min:Minimum
*r.200:max:Maximum','SrcWater.Fo:var:Значение
*r.2:prec:Точноcть
*s.т/ч:ed:Единица измерения
*r.0:min:Минимум
*r.200:max:Максимум','SrcWater.Fo:var:Значення
*r.2:prec:Точність
*s.т/год:ed:Одиниця виміру
*r.0:min:Мінімум
*r.200:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('FA108','FA108','','','Air-superheater flow on the right feeding line','Расход воздухо-перегревателя по правой питательной линии','Витрати повітря-перегрівача по правій лінії живлення',1,'SrcWater.Fo:var:Value
*r.2:prec:Precision
*s.t/h:ed:Unit of measure
*r.0:min:Minimum
*r.200:max:Maximum','SrcWater.Fo:var:Значение
*r.2:prec:Точноcть
*s.т/ч:ed:Единица измерения
*r.0:min:Минимум
*r.200:max:Максимум','SrcWater.Fo:var:Значення
*r.2:prec:Точність
*s.т/год:ed:Одиниця виміру
*r.0:min:Мінімум
*r.200:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('P103','P103','','','GBF pressure on the diaphragm on the left','Давление ГД на диафрагме слева','Тиск ГД на діафрагмі ліворуч',1,'Node3.Po2:var:Value
*r.1:prec:Precision
*s.at:ed:Unit of measure
*r.0:min:Minimum
*r.2:max:Maximum','Node3.Po2:var:Значение
*r.1:prec:Точноcть
*s.ат:ed:Единица измерения
*r.0:min:Минимум
*r.2:max:Максимум','Node3.Po2:var:Значення
*r.1:prec:Точність
*s.ат:ed:Одиниця виміру
*r.0:min:Мінімум
*r.2:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('P104','P104','','','GBF pressure on the diaphragm on the right','Давление ГД на диафрагме справа','Тиск ГД на діафрагмі праворуч',1,'Node3.Po1:var:Value
*r.1:prec:Precision
*s.at:ed:Unit of measure
*r.0:min:Minimum
*r.2:max:Maximum','Node3.Po1:var:Значение
*r.2:prec:Точноcть
*s.ат:ed:Единица измерения
*r.0:min:Минимум
*r.2:max:Максимум','Node3.Po1:var:Значення
*r.2:prec:Точність
*s.ат:ed:Одиниця виміру
*r.0:min:Мінімум
*r.2:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('G_11SH','G 11SH','','','11G position','Положение 11Ш','Положення 11Ш',1,'11G.l_v1:var:Value','11G.l_v1:var:Значение','11G.l_v1:var:Значення');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('TCVG1_1','TCVG1 1','','','7FW position','Положение 7ВП','Положення 7ВП',1,'7FW.l_v1:var:Value','7FW.l_v1:var:Значение','7FW.l_v1:var:Значення');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('PCVG76','PCVG76','','','SEA productivity','Производительность ДСА','Продуктивність ДСА',1,'SEA.N:var:Value
*r.1:prec:Precision
*s.rpm:ed:Unit of measure
*r.0:min:Minimum
*r.100:max:Maximum','SEA.N:var:Значение
*r.1:prec:Точность
*s.об/мин:ed:Единица измерения
*r.0:min:Минимум
*r.100:max:Максимум','SEA.N:var:Значення
*r.1:prec:Точність
*s.об/хвил:ed:Одиниця виміру
*r.0:min:Мінімум
*r.100:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('TCVG1_2','TCVG1 2','','','5FWL position','Положение 5ВПл','Положення 5ВПл',1,'5FW.l_v1:var:Value','5FW.l_v1:var:Значение','5FW.l_v1:var:Значення');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('PCVG77','PCVG77','','','SEB productivity','Производительность ДСБ','Продуктивність ДСБ',1,'SEB.N:var:Value
*r.1:prec:Precision
*s.rpm:ed:Unit of measure
*r.0:min:Minimum
*r.100:max:Maximum','SEB.N:var:Значение
*r.1:prec:Точность
*s.об/мин:ed:Единица измерения
*r.0:min:Минимум
*r.100:max:Максимум','SEB.N:var:Значення
*r.1:prec:Точність
*s.об/хвил:ed:Одиниця виміру
*r.0:min:Мінімум
*r.100:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('G_12SH','G 12SH','','','12G position','Положение 12Ш','Положення 12Ш',1,'11G.l_v2:var:Value','11G.l_v2:var:Значение','11G.l_v2:var:Значення');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('FCV106','FCV106','','','BFA productivity','Производительность ДВА','Продуктивність ДВА',1,'BFA.N:var:Value
*r.1:prec:Precision
*s.rpm:ed:Unit of measure
*r.0:min:Minimum
*r.100:max:Maximum','BFA.N:var:Значение
*r.1:prec:Точность
*s.об/мин:ed:Единица измерения
*r.0:min:Минимум
*r.100:max:Максимум','BFA.N:var:Значення
*r.1:prec:Точність
*s.об/хвил:ed:Одиниця виміру
*r.0:min:Мінімум
*r.100:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9" VALUES('FCV107','FCV107','','','BFB productivity','Производительность ДВБ','Продуктивність ДВБ',1,'BFB.N:var:Value
*r.1:prec:Precision
*s.rpm:ed:Unit of measure
*r.0:min:Minimum
*r.100:max:Maximum','BFB.N:var:Значение
*r.1:prec:Точность
*s.об/мин:ed:Единица измерения
*r.0:min:Минимум
*r.100:max:Максимум','BFB.N:var:Значення
*r.1:prec:Точність
*s.об/хвил:ed:Одиниця виміру
*r.0:min:Мінімум
*r.100:max:Максимум');
CREATE TABLE 'BlckCalcPrm_boiler9_cntr' ("SHIFR" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '' ,"IO" TEXT DEFAULT '' ,"ru#IO" TEXT DEFAULT '' ,"uk#IO" TEXT DEFAULT '' , PRIMARY KEY ("SHIFR"));
INSERT INTO "BlckCalcPrm_boiler9_cntr" VALUES('LC121','LC121','','','Level in the boiler drum','Уровень в барабане котла','Рівень у барабані котла',1,'LC121.sp:sp
LC121.out:out
LC121.manIn:manIn
LC121.var:var
*r.1:prec:Precision
*s.%:ed:Unit of measure
*r.20:wMin:Lower warn. border
*r.80:wMax:Upper warn. border
*r.10:aMin:Lower al. border
*r.90:aMax:Upper al. border
LC121.min:min
LC121.max:max
LC121.auto:auto
LC121.Kp:Kp
LC121.Ti:Ti
LC121.Kd:Kd
LC121.Td:Td
LC121.Tzd:Tzd
LC121.Hup:Hup
LC121.Hdwn:Hdwn
LC121.Zi:Zi
LC121.K1:K1
LC121.K2:K2
LC121.K3:K3
LC121.K4:K4','LC121.sp:sp
LC121.out:out
LC121.manIn:manIn
LC121.var:var
*r.1:prec:Точность
*s.%:ed:Единица измерения
*r.20:wMin:Нижняя предуп. граница
*r.80:wMax:Верхняя предуп. граница
*r.10:aMin:Нижняя ав. граница
*r.90:aMax:Верхняя ав. граница
LC121.min:min
LC121.max:max
LC121.auto:auto
LC121.Kp:Kp
LC121.Ti:Ti
LC121.Kd:Kd
LC121.Td:Td
LC121.Tzd:Tzd
LC121.Hup:Hup
LC121.Hdwn:Hdwn
LC121.Zi:Zi
LC121.K1:K1
LC121.K2:K2
LC121.K3:K3
LC121.K4:K4','LC121.sp:sp
LC121.out:out
LC121.manIn:manIn
LC121.var:var
*r.1:prec:Точність
*s.%:ed:Одиниця виміру
*r.20:wMin:Нижня попередж. границя
*r.80:wMax:Верхня попередж. границя
*r.10:aMin:Нижня ав. границя
*r.90:aMax:Верхня ав. границя
LC121.min:min
LC121.max:max
LC121.auto:auto
LC121.Kp:Kp
LC121.Ti:Ti
LC121.Kd:Kd
LC121.Td:Td
LC121.Tzd:Tzd
LC121.Hup:Hup
LC121.Hdwn:Hdwn
LC121.Zi:Zi
LC121.K1:K1
LC121.K2:K2
LC121.K3:K3
LC121.K4:K4');
INSERT INTO "BlckCalcPrm_boiler9_cntr" VALUES('PSA76','PSA76','','','Vacuum in the fire chamber','Разрежение в топке','Розрідження у топці',1,'PSA76.sp:sp
PSA76.out:out
PSA76.manIn:manIn
PSA76.var:var
*r.3:prec:Precision
*s.at:ed:Unit of measure
PSA76.min:min
PSA76.max:max
PSA76.auto:auto
PSA76.Kp:Kp
PSA76.Ti:Ti
PSA76.Kd:Kd
PSA76.Td:Td
PSA76.Tzd:Tzd
PSA76.Hup:Hup
PSA76.Hdwn:Hdwn
PSA76.Zi:Zi
PSA76.K1:K1
PSA76.K2:K2
PSA76.K3:K3
PSA76.K4:K4','PSA76.sp:sp
PSA76.out:out
PSA76.manIn:manIn
PSA76.var:var
*r.3:prec:Точность
*s.ат:ed:Единица измерения
PSA76.min:min
PSA76.max:max
PSA76.auto:auto
PSA76.Kp:Kp
PSA76.Ti:Ti
PSA76.Kd:Kd
PSA76.Td:Td
PSA76.Tzd:Tzd
PSA76.Hup:Hup
PSA76.Hdwn:Hdwn
PSA76.Zi:Zi
PSA76.K1:K1
PSA76.K2:K2
PSA76.K3:K3
PSA76.K4:K4','PSA76.sp:sp
PSA76.out:out
PSA76.manIn:manIn
PSA76.var:var
*r.3:prec:Точність
*s.ат:ed:Одиниця виміру
PSA76.min:min
PSA76.max:max
PSA76.auto:auto
PSA76.Kp:Kp
PSA76.Ti:Ti
PSA76.Kd:Kd
PSA76.Td:Td
PSA76.Tzd:Tzd
PSA76.Hup:Hup
PSA76.Hdwn:Hdwn
PSA76.Zi:Zi
PSA76.K1:K1
PSA76.K2:K2
PSA76.K3:K3
PSA76.K4:K4');
INSERT INTO "BlckCalcPrm_boiler9_cntr" VALUES('FC102','FC102','','','Natural gas flow','Расход природного газа','Витрати природного газу',1,'FC102.sp:sp
FC102.out:out
FC102.manIn:manIn
FC102.var:var
*r.1:prec:Precision
*s.t/h:ed:Unit of measure
FC102.min:min
FC102.max:max
FC102.auto:auto
FC102.casc:casc
FC102.Kp:Kp
FC102.Ti:Ti
FC102.Kd:Kd
FC102.Td:Td
FC102.Tzd:Tzd
FC102.Hup:Hup
FC102.Hdwn:Hdwn
FC102.Zi:Zi
FC102.K1:K1
FC102.K2:K2
FC102.K3:K3
FC102.K4:K4','FC102.sp:sp
FC102.out:out
FC102.manIn:manIn
FC102.var:var
*r.1:prec:Точность
*s.т/ч:ed:Единица измерения
FC102.min:min
FC102.max:max
FC102.auto:auto
FC102.casc:casc
FC102.Kp:Kp
FC102.Ti:Ti
FC102.Kd:Kd
FC102.Td:Td
FC102.Tzd:Tzd
FC102.Hup:Hup
FC102.Hdwn:Hdwn
FC102.Zi:Zi
FC102.K1:K1
FC102.K2:K2
FC102.K3:K3
FC102.K4:K4','FC102.sp:sp
FC102.out:out
FC102.manIn:manIn
FC102.var:var
*r.1:prec:Точність
*s.т/год:ed:Одиниця виміру
FC102.min:min
FC102.max:max
FC102.auto:auto
FC102.casc:casc
FC102.Kp:Kp
FC102.Ti:Ti
FC102.Kd:Kd
FC102.Td:Td
FC102.Tzd:Tzd
FC102.Hup:Hup
FC102.Hdwn:Hdwn
FC102.Zi:Zi
FC102.K1:K1
FC102.K2:K2
FC102.K3:K3
FC102.K4:K4');
INSERT INTO "BlckCalcPrm_boiler9_cntr" VALUES('FC104','FC104','','','Blast furnace gas flow on the right gas pipeline','Расход доменного газа по правому газопроводу','Витрати доменного газу по правому газопроводу',1,'FC104.sp:sp
FC104.out:out
FC104.manIn:manIn
FC104.var:var
*r.1:prec:Precision
*s.t/h:ed:Unit of measure
FC104.min:min
FC104.max:max
FC104.auto:auto
FC104.casc:casc
FC104.Kp:Kp
FC104.Ti:Ti
FC104.Kd:Kd
FC104.Td:Td
FC104.Tzd:Tzd
FC104.Hup:Hup
FC104.Hdwn:Hdwn
FC104.Zi:Zi
FC104.K1:K1
FC104.K2:K2
FC104.K3:K3
FC104.K4:K4','FC104.sp:sp
FC104.out:out
FC104.manIn:manIn
FC104.var:var
*r.1:prec:Точность
*s.т/ч:ed:Единица измерения
FC104.min:min
FC104.max:max
FC104.auto:auto
FC104.casc:casc
FC104.Kp:Kp
FC104.Ti:Ti
FC104.Kd:Kd
FC104.Td:Td
FC104.Tzd:Tzd
FC104.Hup:Hup
FC104.Hdwn:Hdwn
FC104.Zi:Zi
FC104.K1:K1
FC104.K2:K2
FC104.K3:K3
FC104.K4:K4','FC104.sp:sp
FC104.out:out
FC104.manIn:manIn
FC104.var:var
*r.1:prec:Точність
*s.т/год:ed:Одиниця виміру
FC104.min:min
FC104.max:max
FC104.auto:auto
FC104.casc:casc
FC104.Kp:Kp
FC104.Ti:Ti
FC104.Kd:Kd
FC104.Td:Td
FC104.Tzd:Tzd
FC104.Hup:Hup
FC104.Hdwn:Hdwn
FC104.Zi:Zi
FC104.K1:K1
FC104.K2:K2
FC104.K3:K3
FC104.K4:K4');
INSERT INTO "BlckCalcPrm_boiler9_cntr" VALUES('FC103','FC103','','','Blast furnace gas flow on the left gas pipeline','Расход доменного газа по левому газопроводу','Витрати доменного газу по лівому газопроводу',1,'FC103.sp:sp
FC103.out:out
FC103.manIn:manIn
FC103.var:var
*r.1:prec:Precision
*s.t/h:ed:Unit of measure
FC103.min:min
FC103.max:max
FC103.auto:auto
FC103.casc:casc
FC103.Kp:Kp
FC103.Ti:Ti
FC103.Kd:Kd
FC103.Td:Td
FC103.Tzd:Tzd
FC103.Hup:Hup
FC103.Hdwn:Hdwn
FC103.Zi:Zi
FC103.K1:K1
FC103.K2:K2
FC103.K3:K3
FC103.K4:K4','FC103.sp:sp
FC103.out:out
FC103.manIn:manIn
FC103.var:var
*r.1:prec:Точность
*s.т/ч:ed:Единица измерения
FC103.min:min
FC103.max:max
FC103.auto:auto
FC103.casc:casc
FC103.Kp:Kp
FC103.Ti:Ti
FC103.Kd:Kd
FC103.Td:Td
FC103.Tzd:Tzd
FC103.Hup:Hup
FC103.Hdwn:Hdwn
FC103.Zi:Zi
FC103.K1:K1
FC103.K2:K2
FC103.K3:K3
FC103.K4:K4','FC103.sp:sp
FC103.out:out
FC103.manIn:manIn
FC103.var:var
*r.1:prec:Точність
*s.т/год:ed:Одиниця виміру
FC103.min:min
FC103.max:max
FC103.auto:auto
FC103.casc:casc
FC103.Kp:Kp
FC103.Ti:Ti
FC103.Kd:Kd
FC103.Td:Td
FC103.Tzd:Tzd
FC103.Hup:Hup
FC103.Hdwn:Hdwn
FC103.Zi:Zi
FC103.K1:K1
FC103.K2:K2
FC103.K3:K3
FC103.K4:K4');
INSERT INTO "BlckCalcPrm_boiler9_cntr" VALUES('FC105','FC105','','','Coke oven gas flow','Расход коксового газа','Витрати коксового газу',1,'FC105.sp:sp
FC105.out:out
FC105.manIn:manIn
FC105.var:var
*r.1:prec:Precision
*s.t/h:ed:Unit of measure
FC105.min:min
FC105.max:max
FC105.auto:auto
FC105.casc:casc
FC105.Kp:Kp
FC105.Ti:Ti
FC105.Kd:Kd
FC105.Td:Td
FC105.Tzd:Tzd
FC105.Hup:Hup
FC105.Hdwn:Hdwn
FC105.Zi:Zi
FC105.K1:K1
FC105.K2:K2
FC105.K3:K3
FC105.K4:K4','FC105.sp:sp
FC105.out:out
FC105.manIn:manIn
FC105.var:var
*r.1:prec:Точность
*s.т/ч:ed:Единица измерения
FC105.min:min
FC105.max:max
FC105.auto:auto
FC105.casc:casc
FC105.Kp:Kp
FC105.Ti:Ti
FC105.Kd:Kd
FC105.Td:Td
FC105.Tzd:Tzd
FC105.Hup:Hup
FC105.Hdwn:Hdwn
FC105.Zi:Zi
FC105.K1:K1
FC105.K2:K2
FC105.K3:K3
FC105.K4:K4','FC105.sp:sp
FC105.out:out
FC105.manIn:manIn
FC105.var:var
*r.1:prec:Точність
*s.т/год:ed:Одиниця виміру
FC105.min:min
FC105.max:max
FC105.auto:auto
FC105.casc:casc
FC105.Kp:Kp
FC105.Ti:Ti
FC105.Kd:Kd
FC105.Td:Td
FC105.Tzd:Tzd
FC105.Hup:Hup
FC105.Hdwn:Hdwn
FC105.Zi:Zi
FC105.K1:K1
FC105.K2:K2
FC105.K3:K3
FC105.K4:K4');
INSERT INTO "BlckCalcPrm_boiler9_cntr" VALUES('TCA1','TCA1','','','Steam temperature at the output','Температура пара на выходе','Температура пару на виході',1,'TCA1.sp:sp
TCA1.out:out
TCA1.manIn:manIn
TCA1.var:var
*r.0:prec:Precision
*s.K:ed:Unit of measure
TCA1.min:min
TCA1.max:max
TCA1.auto:auto
TCA1.Kp:Kp
TCA1.Ti:Ti
TCA1.Kd:Kd
TCA1.Td:Td
TCA1.Tzd:Tzd
TCA1.Hup:Hup
TCA1.Hdwn:Hdwn
TCA1.Zi:Zi
TCA1.K1:K1
TCA1.K2:K2
TCA1.K3:K3
TCA1.K4:K4','TCA1.sp:sp
TCA1.out:out
TCA1.manIn:manIn
TCA1.var:var
*r.0:prec:Точность
*s.К:ed:Единица измерения
TCA1.min:min
TCA1.max:max
TCA1.auto:auto
TCA1.Kp:Kp
TCA1.Ti:Ti
TCA1.Kd:Kd
TCA1.Td:Td
TCA1.Tzd:Tzd
TCA1.Hup:Hup
TCA1.Hdwn:Hdwn
TCA1.Zi:Zi
TCA1.K1:K1
TCA1.K2:K2
TCA1.K3:K3
TCA1.K4:K4','TCA1.sp:sp
TCA1.out:out
TCA1.manIn:manIn
TCA1.var:var
*r.0:prec:Точність
*s.град.К:ed:Одиниця виміру
TCA1.min:min
TCA1.max:max
TCA1.auto:auto
TCA1.Kp:Kp
TCA1.Ti:Ti
TCA1.Kd:Kd
TCA1.Td:Td
TCA1.Tzd:Tzd
TCA1.Hup:Hup
TCA1.Hdwn:Hdwn
TCA1.Zi:Zi
TCA1.K1:K1
TCA1.K2:K2
TCA1.K3:K3
TCA1.K4:K4');
INSERT INTO "BlckCalcPrm_boiler9_cntr" VALUES('5FW','5FW','5ВП','5ВП','','','',1,'5FW_inv.OVar:var:Value
*r.1:prec:Precision
*s.%:ed:Unit of measure
*r.0:min:Minimum
*r.100:max:Maximum','5FW_inv.OVar:var:Значение
*r.1:prec:Точноcть
*s.%:ed:Единица измерения
*r.0:min:Минимум
*r.100:max:Максимум','5FW_inv.OVar:var:Значення
*r.1:prec:Точність
*s.%:ed:Одиниця виміру
*r.0:min:Мінімум
*r.100:max:Максимум');
INSERT INTO "BlckCalcPrm_boiler9_cntr" VALUES('QAC151','QAC151','','','Percentage of oxygen in the flue gases','Процент кислорода в дымовых газах','Відсоток кисню у димових газах',1,'QAC151.sp:sp
QAC151.out:out
QAC151.manIn:manIn
QAC151.var:var
*r.1:prec:Precision
*s.%:ed:Unit of measure
QAC151.min:min
QAC151.max:max
QAC151.auto:auto
QAC151.Kp:Kp
QAC151.Ti:Ti
QAC151.Kd:Kd
QAC151.Td:Td
QAC151.Tzd:Tzd
QAC151.Hup:Hup
QAC151.Hdwn:Hdwn
QAC151.Zi:Zi
QAC151.K1:K1
QAC151.K2:K2
QAC151.K3:K3
QAC151.K4:K4','QAC151.sp:sp
QAC151.out:out
QAC151.manIn:manIn
QAC151.var:var
*r.1:prec:Точность
*s.%:ed:Единица измерения
QAC151.min:min
QAC151.max:max
QAC151.auto:auto
QAC151.Kp:Kp
QAC151.Ti:Ti
QAC151.Kd:Kd
QAC151.Td:Td
QAC151.Tzd:Tzd
QAC151.Hup:Hup
QAC151.Hdwn:Hdwn
QAC151.Zi:Zi
QAC151.K1:K1
QAC151.K2:K2
QAC151.K3:K3
QAC151.K4:K4','QAC151.sp:sp
QAC151.out:out
QAC151.manIn:manIn
QAC151.var:var
*r.1:prec:Точність
*s.%:ed:Одиниця виміру
QAC151.min:min
QAC151.max:max
QAC151.auto:auto
QAC151.Kp:Kp
QAC151.Ti:Ti
QAC151.Kd:Kd
QAC151.Td:Td
QAC151.Tzd:Tzd
QAC151.Hup:Hup
QAC151.Hdwn:Hdwn
QAC151.Zi:Zi
QAC151.K1:K1
QAC151.K2:K2
QAC151.K3:K3
QAC151.K4:K4');
INSERT INTO "BlckCalcPrm_boiler9_cntr" VALUES('FC101','FC101','','','Steam flow from the boiler','Расход пара из котла','Витрати пару з котла',1,'FC101.sp:sp
FC101.out:out
FC101.manIn:manIn
FC101.var:var
*r.0:prec:Precision
*s.t/h:ed:Unit of measure
FC101.min:min
FC101.max:max
FC101.auto:auto
FC101.Kp:Kp
FC101.Ti:Ti
FC101.Kd:Kd
FC101.Td:Td
FC101.Tzd:Tzd
FC101.Hup:Hup
FC101.Hdwn:Hdwn
FC101.Zi:Zi
FC101.K1:K1
FC101.K2:K2
FC101.K3:K3
FC101.K4:K4','FC101.sp:sp
FC101.out:out
FC101.manIn:manIn
FC101.var:var
*r.0:prec:Точность
*s.т/ч:ed:Единица измерения
FC101.min:min
FC101.max:max
FC101.auto:auto
FC101.Kp:Kp
FC101.Ti:Ti
FC101.Kd:Kd
FC101.Td:Td
FC101.Tzd:Tzd
FC101.Hup:Hup
FC101.Hdwn:Hdwn
FC101.Zi:Zi
FC101.K1:K1
FC101.K2:K2
FC101.K3:K3
FC101.K4:K4','FC101.sp:sp
FC101.out:out
FC101.manIn:manIn
FC101.var:var
*r.0:prec:Точність
*s.т/год:ed:Одиниця виміру
FC101.min:min
FC101.max:max
FC101.auto:auto
FC101.Kp:Kp
FC101.Ti:Ti
FC101.Kd:Kd
FC101.Td:Td
FC101.Tzd:Tzd
FC101.Hup:Hup
FC101.Hdwn:Hdwn
FC101.Zi:Zi
FC101.K1:K1
FC101.K2:K2
FC101.K3:K3
FC101.K4:K4');
CREATE TABLE 'flb_boiler9' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"MAXCALCTM" INTEGER DEFAULT '' ,"FORMULA" TEXT DEFAULT '' ,"ru#FORMULA" TEXT DEFAULT '' ,"uk#FORMULA" TEXT DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO "flb_boiler9" VALUES('Divider','Divider','Делитель','Дільник','','','',10,'F_out=F_air/F_gas;','F_out=F_air/F_gas;','F_out=F_air/F_gas;');
INSERT INTO "flb_boiler9" VALUES('Inversion','Inversion','Инверсия','Інверсія','The block for variable (100 - var) inversion.','Блок для инвертирования переменной(100 - var)','Блок для інвертування змінної(100 - var)',10,'OVar = 100. - IVar;','','');
INSERT INTO "flb_boiler9" VALUES('Fsum','Total fuel flow in boiler','Суммарный расход топлива в котле','Сумарні витрати палива у котлі','','','',10,'FsumW -=  (FsumW - (f_min+(f_max-f_min)*F_sum/100))/max(1,t_zd*f_frq);
S_f_dl = !Casc_f_dl ? Si_f_dl : FsumW/2;
S_f_dp = !Casc_f_dp ? Si_f_dp : FsumW/2;
S_f_k = !Casc_f_k ? Si_f_k : max(0,(FsumW - F_d_l - F_d_p)/2.1);
S_f_p = !Casc_f_p ? Si_f_p : max(0,(FsumW - F_d_l - F_d_p - 2.1*F_k)/4.3);','FsumW -=  (FsumW - (f_min+(f_max-f_min)*F_sum/100))/max(1,t_zd*f_frq);
S_f_dl = !Casc_f_dl ? Si_f_dl : FsumW/2;
S_f_dp = !Casc_f_dp ? Si_f_dp : FsumW/2;
S_f_k = !Casc_f_k ? Si_f_k : max(0,(FsumW - F_d_l - F_d_p)/2.1);
S_f_p = !Casc_f_p ? Si_f_p : max(0,(FsumW - F_d_l - F_d_p - 2.1*F_k)/4.3);','FsumW -=  (FsumW - (f_min+(f_max-f_min)*F_sum/100))/max(1,t_zd*f_frq);
S_f_dl = !Casc_f_dl ? Si_f_dl : FsumW/2;
S_f_dp = !Casc_f_dp ? Si_f_dp : FsumW/2;
S_f_k = !Casc_f_k ? Si_f_k : max(0,(FsumW - F_d_l - F_d_p)/2.1);
S_f_p = !Casc_f_p ? Si_f_p : max(0,(FsumW - F_d_l - F_d_p - 2.1*F_k)/4.3);');
CREATE TABLE 'flb_boiler9_io' ("F_ID" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"TYPE" INTEGER DEFAULT '' ,"MODE" INTEGER DEFAULT '' ,"DEF" TEXT DEFAULT '' ,"ru#DEF" TEXT DEFAULT '' ,"uk#DEF" TEXT DEFAULT '' ,"HIDE" INTEGER DEFAULT '' ,"POS" INTEGER DEFAULT '' , PRIMARY KEY ("F_ID","ID"));
INSERT INTO "flb_boiler9_io" VALUES('Divider','F_air','Input air flow','Входной расход воздуха','Вхідні витрати повітря',2,0,'','','',0,0);
INSERT INTO "flb_boiler9_io" VALUES('Divider','F_gas','Input fuel flow','Входной расход топлива','Відні витрати палива',2,0,'','','',0,1);
INSERT INTO "flb_boiler9_io" VALUES('Divider','F_out','Output proportion','Выходное соотношение','Вихідне співвідношення',2,1,'','','',0,2);
INSERT INTO "flb_boiler9_io" VALUES('Inversion','IVar','Input variable','Входная переменная','Вхідна змінна',2,0,'','','',0,0);
INSERT INTO "flb_boiler9_io" VALUES('Inversion','OVar','Output variable','Выходная переменная','Вихідна змінна',2,1,'','','',0,1);
INSERT INTO "flb_boiler9_io" VALUES('Fsum','F_sum','Total fuel flow','Суммарный расход топлива','Сумарні витрати палива',2,0,'88.9175','','',0,0);
INSERT INTO "flb_boiler9_io" VALUES('Fsum','t_zd','Lag time (s)','Время запазд. (с)','Час затримки (с)',2,0,'5','','',0,1);
INSERT INTO "flb_boiler9_io" VALUES('Fsum','F_d_l','BFG flow on left pipeline','Расход ГД по левому трубопроводу','Витрати ГД по лівому трубопроводу',2,0,'','','',0,2);
INSERT INTO "flb_boiler9_io" VALUES('Fsum','F_d_p','BFG flow on right pipeline','Расход ГД по правому трубопроводу','Витрати ГД по правому трубопроводу',2,0,'','','',0,3);
INSERT INTO "flb_boiler9_io" VALUES('Fsum','F_k','COG flow','Расход ГК','Витрати ГК',2,0,'','','',0,4);
INSERT INTO "flb_boiler9_io" VALUES('Fsum','Casc_f_p','Cascade mode on NG','Режим каскада по ГП','Режим каскаду за ГП',3,0,'','','',0,5);
INSERT INTO "flb_boiler9_io" VALUES('Fsum','Si_f_p','Input setpoint on NG','Вход задания по ГП','Вхід завдання за ГП',2,0,'','','',0,6);
INSERT INTO "flb_boiler9_io" VALUES('Fsum','S_f_p','Setpoint on NG','Задание по ГП','Завдання за ГП',2,1,'','','',0,7);
INSERT INTO "flb_boiler9_io" VALUES('Fsum','Casc_f_k','Cascade mode on COG','Режим каскада по ГК','Режим каскаду за ГК',3,0,'','','',0,8);
INSERT INTO "flb_boiler9_io" VALUES('Fsum','Si_f_k','Input setpoint on COG','Вход задания по ГК','Вхід завдання за ГК',2,0,'','','',0,9);
INSERT INTO "flb_boiler9_io" VALUES('Fsum','S_f_k','Setpoint on COG','Задание по ГК','Завдання за ГК',2,1,'','','',0,10);
INSERT INTO "flb_boiler9_io" VALUES('Fsum','Casc_f_dl','Cascade mode on BFG left','Режим каскада по ГД слева','Режим каскаду за ГД ліворуч',3,0,'','','',0,11);
INSERT INTO "flb_boiler9_io" VALUES('Fsum','Si_f_dl','Input setpoint on BFG left','Вход задания по ГД слева','Вхід завдання за ГД ліворуч',2,0,'','','',0,12);
INSERT INTO "flb_boiler9_io" VALUES('Fsum','S_f_dl','Setpoint on BFG left','Задание по ГД слева','Завдання за ГД ліворуч',2,1,'','','',0,13);
INSERT INTO "flb_boiler9_io" VALUES('Fsum','Casc_f_dp','Cascade mode on BFG right','Режим каскада по ГД справа','Режим каскаду за ГД праворуч',3,0,'','','',0,14);
INSERT INTO "flb_boiler9_io" VALUES('Fsum','Si_f_dp','Input setpoint on BFG right','Вход задания по ГД справа','Вхід завдання за ГД праворуч',2,0,'','','',0,15);
INSERT INTO "flb_boiler9_io" VALUES('Fsum','S_f_dp','Setpoint on BFG right','Задание по ГД справа','Завдання за ГД праворуч',2,1,'','','',0,16);
INSERT INTO "flb_boiler9_io" VALUES('Fsum','f_max','Scale maximum on total flow','Максимум шкалы по суммарному расходу','Максимум шкали за сумарними витратами',2,0,'150','','',0,17);
INSERT INTO "flb_boiler9_io" VALUES('Fsum','f_min','Scale minimum on total flow','Минимум шкалы по суммарному расходу','Мінімум шкали за сумарними витратами',2,0,'0','','',0,18);
INSERT INTO "flb_boiler9_io" VALUES('Fsum','f_frq','Calculate frequency (Hz)','Частота обсчёта (Гц)','Частота обчислення (Гц)',2,0,'100','','',0,19);
INSERT INTO "flb_boiler9_io" VALUES('Fsum','FsumW','Work flow','Рабочий расход','Робочі витрати',2,1,'0','','',0,20);
CREATE TABLE 'BlckCalcBlcks_boiler9' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"FUNC" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '0' ,"PROC" INTEGER DEFAULT '0' ,"PRIOR" TEXT DEFAULT '' ,"LNK_OUT_WR_CH" INTEGER DEFAULT '0' , PRIMARY KEY ("ID"));
INSERT INTO "BlckCalcBlcks_boiler9" VALUES('SrcGBF','','','','Source of blast furnace gas','Источник доменного газа','Джерело доменного газу','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_src_press',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9" VALUES('7FW','','','','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_valve',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9" VALUES('SrcGC','','','','Source of coke oven gas','Источник коксового газа','Джерело коксового газу','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_src_press',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9" VALUES('Node5','','','','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_pipe1_2',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9" VALUES('Node6','','','','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_pipe1_2',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9" VALUES('Node7','','','','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_pipe1_2',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9" VALUES('Node8','','','','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_pipe3_1',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9" VALUES('3FW','','','','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_valve',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9" VALUES('OhS','','','','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_heatExch',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9" VALUES('Node10','','','','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_pipe1_2',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9" VALUES('9GBF','','','','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_valve',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9" VALUES('Node12','','','','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_pipe2_1',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9" VALUES('3GC','','','','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_valve',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9" VALUES('2G','','','','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_valve',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9" VALUES('Node1','','','','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_pipe2_1',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9" VALUES('Node2','','','','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_pipe2_1',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9" VALUES('11G','','','','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_valve',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9" VALUES('Node3','','','','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_pipe1_2',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9" VALUES('Node4','','','','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_pipe1_2',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9" VALUES('Node9','','','','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_pipe2_1',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9" VALUES('3GN','','','','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_valve',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9" VALUES('4GC','','','','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_valve',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9" VALUES('AH1','','','','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_heatExch',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9" VALUES('SEA','','','','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_compressor',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9" VALUES('AH2','','','','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_heatExch',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9" VALUES('SEB','','','','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_compressor',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9" VALUES('5GBF','','','','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_valve',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9" VALUES('4GN3','','','','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_valve',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9" VALUES('10GBF','','','','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_valve',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9" VALUES('DS','','','','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_heatExch',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9" VALUES('BFA','','','','Blow fan А','Дутьевой вентилятор А','Дуттьовий вентилятор А','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_compressor',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9" VALUES('SrcWater','','','','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_src_press',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9" VALUES('BFB','','','','Blow fan B','Дутьевой вентилятор Б','Дуттьовий вентилятор Б','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_compressor',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9" VALUES('4GN','','','','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_valve',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9" VALUES('13G','','','','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_valve',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9" VALUES('1G','','','','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_valve',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9" VALUES('5FW','','','','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_valve',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9" VALUES('SrcNG','','','','Source of natural gas','Источник природного газа','Джерело природнього газу','DAQ.JavaLikeCalc.lib_techApp.src_press',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9" VALUES('5GC','','','','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_valve',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9" VALUES('6GBF','','','','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_valve',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9" VALUES('ParNet','','','','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_net',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9" VALUES('5GN','','','','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_valve',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9" VALUES('6GC','','','','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_valve',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9" VALUES('EC1','','','','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_heatExch',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9" VALUES('EC2','','','','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_heatExch',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9" VALUES('7GBF','','','','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_valve',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9" VALUES('SrcAir','','','','Air source','Источник воздуха','Джерело повітря','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_src_press',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9" VALUES('FireChamber','','','','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_boilerBurner',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9" VALUES('Drum','','','','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_boilerBarrel',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9" VALUES('6GN','','','','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_valve',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9" VALUES('Atmosph','','','','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_net',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9" VALUES('8GBF','','','','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_valve',1,1,'',0);
CREATE TABLE 'BlckCalcBlcks_boiler9_cntr' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"FUNC" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '0' ,"PROC" INTEGER DEFAULT '0' ,"PRIOR" TEXT DEFAULT '' ,"LNK_OUT_WR_CH" INTEGER DEFAULT '0' , PRIMARY KEY ("ID"));
INSERT INTO "BlckCalcBlcks_boiler9_cntr" VALUES('LC121','','','','','','','sub_Special.mod_FLibComplex1.fnc_pid',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9_cntr" VALUES('TCA1','','','','','','','sub_Special.mod_FLibComplex1.fnc_pid',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9_cntr" VALUES('PSA76','','','','','','','sub_Special.mod_FLibComplex1.fnc_pid',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9_cntr" VALUES('FC102','','','','','','','sub_Special.mod_FLibComplex1.fnc_pid',1,1,'Fsum',0);
INSERT INTO "BlckCalcBlcks_boiler9_cntr" VALUES('FC105','','','','','','','sub_Special.mod_FLibComplex1.fnc_pid',1,1,'Fsum',0);
INSERT INTO "BlckCalcBlcks_boiler9_cntr" VALUES('FC103','','','','','','','sub_Special.mod_FLibComplex1.fnc_pid',1,1,'Fsum',0);
INSERT INTO "BlckCalcBlcks_boiler9_cntr" VALUES('FC104','','','','','','','sub_Special.mod_FLibComplex1.fnc_pid',1,1,'Fsum',0);
INSERT INTO "BlckCalcBlcks_boiler9_cntr" VALUES('5FW_inv','','','','','','','sub_DAQ.mod_JavaLikeCalc.lib_boiler9.fnc_Inversion',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9_cntr" VALUES('Fsum','','','','','','','sub_DAQ.mod_JavaLikeCalc.lib_boiler9.fnc_Fsum',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9_cntr" VALUES('PCA51','','','','','','','sub_Special.mod_FLibComplex1.fnc_pid',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9_cntr" VALUES('F_air_gas','','','','','','','sub_Special.mod_FLibComplex1.fnc_pid',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9_cntr" VALUES('Air_Gas','','','','','','','sub_DAQ.mod_JavaLikeCalc.lib_boiler9.fnc_Divider',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9_cntr" VALUES('QAC151','','','','','','','sub_Special.mod_FLibComplex1.fnc_pid',1,1,'',0);
INSERT INTO "BlckCalcBlcks_boiler9_cntr" VALUES('FC101','','','','','','','sub_Special.mod_FLibComplex1.fnc_pid',1,1,'',0);
CREATE TABLE 'UserFuncLibs' ("ID" TEXT DEFAULT '''''' ,"NAME" TEXT DEFAULT '''''' ,"ru#NAME" TEXT DEFAULT '''''' ,"DESCR" TEXT DEFAULT '''''' ,"ru#DESCR" TEXT DEFAULT '''''' ,"DB" TEXT DEFAULT '''''' ,"PROG_TR" INTEGER DEFAULT '''''' ,"uk#NAME" TEXT DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO "UserFuncLibs" VALUES('boiler9','Boiler #9','Котёл #9','','','flb_boiler9',1,'Котел #9');
CREATE TABLE 'DAQ_BlockCalc' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ENABLE" INTEGER DEFAULT '0' ,"START" INTEGER DEFAULT '0' ,"MESS_LEV" INTEGER DEFAULT '3' ,"REDNT" INTEGER DEFAULT '0' ,"REDNT_RUN" TEXT DEFAULT '<high>' ,"PRM_BD" TEXT DEFAULT 'system' ,"BLOCK_SH" TEXT DEFAULT 'block' ,"SCHEDULE" TEXT DEFAULT '1' ,"PRIOR" INTEGER DEFAULT '0' ,"ITER" INTEGER DEFAULT '1' , PRIMARY KEY ("ID"));
INSERT INTO "DAQ_BlockCalc" VALUES('boiler9','DMC Boiler9','ДМК Котёл9','ДМК Котел9','Contains a simulator of the boiler unit #9 of the Dnepr Metallurgical Combine.
Author: Roman Savochenko <roman@oscada.org>
Version: 2.0.0','Содержит модель котлоагрегата №9 Днепровского Металлургического Комбината.
Автор: Роман Савоченко <roman@oscada.org>
Версия: 2.0.0','Містить модель котлоагрегату №9 Дніпровського Металургійного Комбінату.
Автор: Роман Савоченко <roman@oscada.org>
Версія: 2.0.0',1,1,3,0,'<high>','BlckCalcPrm_boiler9','BlckCalcBlcks_boiler9','0.005',0,1);
INSERT INTO "DAQ_BlockCalc" VALUES('boiler9_cntr','DMC Boiler9 Controller','ДМК Котёл9 Контроллер','ДМК Котел9 Контролер','Contains a simulator of the control system of the boiler unit #9 of the Dnepr Metallurgical Combine.
Author: Roman Savochenko <roman@oscada.org>
Version: 2.0.0','Содержит модель системы управления котлоагрегата №9 Днепровского Металлургического Комбината.
Автор: Роман Савоченко <roman@oscada.org>
Версия: 2.0.0','Містить модель системи керування котлоагрегату №9 Дніпровського Металургійного Комбінату.
Автор: Роман Савоченко <roman@oscada.org>
Версія: 2.0.0',1,1,3,0,'<high>','BlckCalcPrm_boiler9_cntr','BlckCalcBlcks_boiler9_cntr','1',0,1);
COMMIT;
