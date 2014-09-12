PRAGMA foreign_keys=OFF;
BEGIN TRANSACTION;
CREATE TABLE 'AnastGener_blcks_io' ("BLK_ID" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"TLNK" INTEGER DEFAULT '' ,"LNK" TEXT DEFAULT '' ,"VAL" TEXT DEFAULT '' , PRIMARY KEY ("BLK_ID","ID"));
INSERT INTO "AnastGener_blcks_io" VALUES('node1_1','Fi',0,'','101.771905740409');
INSERT INTO "AnastGener_blcks_io" VALUES('node1_1','Pi',1,'node1.Po2','5.57973427783306');
INSERT INTO "AnastGener_blcks_io" VALUES('node1_1','Fo1',2,'KM101.КШ101.Fi','50.9719811665585');
INSERT INTO "AnastGener_blcks_io" VALUES('node1_1','Po1',6,'KM101.КШ101.Pi','5.54708216031555');
INSERT INTO "AnastGener_blcks_io" VALUES('node1_1','So1',0,'','0.2');
INSERT INTO "AnastGener_blcks_io" VALUES('node1_1','lo1',0,'','40');
INSERT INTO "AnastGener_blcks_io" VALUES('node1_1','Fo2',2,'KM201.КШ101.Fi','50.7846821223439');
INSERT INTO "AnastGener_blcks_io" VALUES('node1_1','Po2',6,'KM201.КШ101.Pi','5.54732185860828');
INSERT INTO "AnastGener_blcks_io" VALUES('node1_1','So2',0,'','0.2');
INSERT INTO "AnastGener_blcks_io" VALUES('node1_1','lo2',0,'','40');
INSERT INTO "AnastGener_blcks_io" VALUES('node1_1','Fo3',2,'KM301.КШ101.Fi','-4.84184332924422e-322');
INSERT INTO "AnastGener_blcks_io" VALUES('node1_1','Po3',6,'KM301.КШ101.Pi','5.57973562721918');
INSERT INTO "AnastGener_blcks_io" VALUES('node1_1','So3',0,'','0.2');
INSERT INTO "AnastGener_blcks_io" VALUES('node1_1','lo3',0,'','40');
INSERT INTO "AnastGener_blcks_io" VALUES('node1_1','Q0',0,'','1');
INSERT INTO "AnastGener_blcks_io" VALUES('node1_1','f_frq',0,'','100');
INSERT INTO "AnastGener_blcks_io" VALUES('node1_1','F1tmp',0,'','50.9773121163991');
INSERT INTO "AnastGener_blcks_io" VALUES('node1_1','F2tmp',0,'','50.7900335606969');
INSERT INTO "AnastGener_blcks_io" VALUES('node1_1','F3tmp',0,'','0.00456006331320211');
INSERT INTO "AnastGener_blcks_io" VALUES('node1_1','Pot1',0,'','5.33928112230883');
INSERT INTO "AnastGener_blcks_io" VALUES('node1_1','Pot2',0,'','5.34618472503855');
INSERT INTO "AnastGener_blcks_io" VALUES('node1_1','Pot3',0,'','5.40197994347128');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ22им','pos',0,'','0');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ22им','com',0,'','0');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ22им','st_open',0,'','0');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ22им','st_close',0,'','1');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ22им','t_full',0,'','3');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ22им','t_up',0,'','0.3');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ22им','f_frq',0,'','100');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ22им','tmp_up',0,'','0');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ22им','lst_com',0,'','0');
INSERT INTO "AnastGener_blcks_io" VALUES('netGVD','Fi',0,'','51.3606410933934');
INSERT INTO "AnastGener_blcks_io" VALUES('netGVD','Pi',1,'КРД4.Po','90.2683033509555');
INSERT INTO "AnastGener_blcks_io" VALUES('netGVD','Po',0,'','90');
INSERT INTO "AnastGener_blcks_io" VALUES('netGVD','So',0,'','0.017');
INSERT INTO "AnastGener_blcks_io" VALUES('netGVD','Kpr',0,'','0.95');
INSERT INTO "AnastGener_blcks_io" VALUES('netGVD','Noise',0,'','1');
INSERT INTO "AnastGener_blcks_io" VALUES('netGVD','Q0',0,'','1');
INSERT INTO "AnastGener_blcks_io" VALUES('netGVD','f_frq',0,'','100');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ21','Fi',0,'','53.5214864360229');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ21','Pi',1,'node20.Po2','23.9561066195563');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ21','Ti',1,'КШ6.To','305.108202862061');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ21','Fo',1,'C2.Fi','53.5199775409968');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ21','Po',0,'','23.8931711942858');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ21','To',0,'','305.032405214862');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ21','So',0,'','0.071');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ21','lo',0,'','7');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ21','S_kl1',0,'','0.071');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ21','S_kl2',0,'','0.002');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ21','l_kl1',1,'КШ21им.pos','100');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ21','l_kl2',1,'КШ22им.pos','0');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ21','Q0',0,'','1');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ21','Kln',0,'','1');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ21','Kpr',0,'','0.95');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ21','Ct',0,'','20');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ21','Riz',0,'','20');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ21','noBack',0,'','0');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ21','Fwind',0,'','1');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ21','Twind',0,'','273');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ21','f_frq',0,'','100');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ21','Pot',0,'','23.8507492058468');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ21','Fit',0,'','38.8382020654556');
INSERT INTO "AnastGener_blcks_io" VALUES('noisePP3','out',0,'','0.99043848699948');
INSERT INTO "AnastGener_blcks_io" VALUES('noisePP3','off',0,'','1');
INSERT INTO "AnastGener_blcks_io" VALUES('noisePP3','a_g1',0,'','0.01');
INSERT INTO "AnastGener_blcks_io" VALUES('noisePP3','per_g1',0,'','200');
INSERT INTO "AnastGener_blcks_io" VALUES('noisePP3','a_g2',0,'','0');
INSERT INTO "AnastGener_blcks_io" VALUES('noisePP3','per_g2',0,'','30');
INSERT INTO "AnastGener_blcks_io" VALUES('noisePP3','a_rnd',0,'','0.001');
INSERT INTO "AnastGener_blcks_io" VALUES('noisePP3','f_frq',0,'','100');
INSERT INTO "AnastGener_blcks_io" VALUES('noisePP3','tmp_g1',0,'','4.76526400000023');
INSERT INTO "AnastGener_blcks_io" VALUES('noisePP3','tmp_g2',0,'','5.30869333333349');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД2им','out',0,'','53.7677885942862');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД2им','in',2,'Anast1to2node_cntr.PC_КРД2.out','53.7673636805758');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД2им','t_lg',0,'','0.25');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД2им','f_frq',0,'','100');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД2им','cl1',0,'','53.7674326553703');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД2им','cl2',0,'','53.7675511678962');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД2им','cl3',0,'','53.7676752307693');
INSERT INTO "AnastGener_blcks_io" VALUES('netGlinsk','Fi',0,'','45.0774036557186');
INSERT INTO "AnastGener_blcks_io" VALUES('netGlinsk','Pi',1,'pipeGlinsk.Po','20.1074185221671');
INSERT INTO "AnastGener_blcks_io" VALUES('netGlinsk','Po',0,'','20');
INSERT INTO "AnastGener_blcks_io" VALUES('netGlinsk','So',0,'','0.05');
INSERT INTO "AnastGener_blcks_io" VALUES('netGlinsk','Kpr',0,'','0.95');
INSERT INTO "AnastGener_blcks_io" VALUES('netGlinsk','Noise',0,'','1');
INSERT INTO "AnastGener_blcks_io" VALUES('netGlinsk','Q0',0,'','1');
INSERT INTO "AnastGener_blcks_io" VALUES('netGlinsk','f_frq',0,'','100');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ6им','pos',0,'','100');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ6им','com',0,'','1');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ6им','st_open',0,'','1');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ6им','st_close',0,'','0');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ6им','t_full',0,'','5');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ6им','t_up',0,'','0.5');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ6им','f_frq',0,'','100');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ6им','tmp_up',0,'','0');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ6им','lst_com',0,'','1');
INSERT INTO "AnastGener_blcks_io" VALUES('P_КРД2','out',0,'','23.9719713845024');
INSERT INTO "AnastGener_blcks_io" VALUES('P_КРД2','in',1,'node20.Po1','23.9721325688221');
INSERT INTO "AnastGener_blcks_io" VALUES('P_КРД2','t_lg',0,'','0.2');
INSERT INTO "AnastGener_blcks_io" VALUES('P_КРД2','f_frq',0,'','100');
INSERT INTO "AnastGener_blcks_io" VALUES('SrcGas','Pi',0,'','9');
INSERT INTO "AnastGener_blcks_io" VALUES('SrcGas','Fo',1,'КШ1.Fi','102.818511620027');
INSERT INTO "AnastGener_blcks_io" VALUES('SrcGas','Po',0,'','8.8569019036695');
INSERT INTO "AnastGener_blcks_io" VALUES('SrcGas','So',0,'','0.385');
INSERT INTO "AnastGener_blcks_io" VALUES('SrcGas','lo',0,'','1000');
INSERT INTO "AnastGener_blcks_io" VALUES('SrcGas','Noise',1,'noisePP3.out','0.989367798357395');
INSERT INTO "AnastGener_blcks_io" VALUES('SrcGas','Q0',0,'','1');
INSERT INTO "AnastGener_blcks_io" VALUES('SrcGas','f_frq',0,'','100');
INSERT INTO "AnastGener_blcks_io" VALUES('SrcGas','Pot',0,'','8.96131699790609');
INSERT INTO "AnastGener_blcks_io" VALUES('SrcGas','Fit',0,'','102.816044486847');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД1','Fi',0,'','102.818891885142');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД1','Pi',1,'КШ1.Po','8.83555972926769');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД1','Ti',1,'КШ1.To','292.966445689868');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД1','Fo',1,'C1.Fi','102.816294931803');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД1','Po',0,'','5.7994114462801');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД1','To',0,'','290.489230346328');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД1','So',0,'','0.385');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД1','lo',0,'','10');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД1','S_kl1',0,'','0.2');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД1','S_kl2',0,'','0.05');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД1','l_kl1',2,'Anast1to2node_cntr.PCKRD1.out','40.1611711995047');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД1','l_kl2',0,'','0');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД1','Q0',0,'','1');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД1','Kln',0,'','2');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД1','Kpr',0,'','0.95');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД1','Ct',0,'','20');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД1','Riz',0,'','20');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД1','noBack',0,'','0');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД1','Fwind',0,'','1');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД1','Twind',0,'','273');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД1','f_frq',0,'','100');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД1','Pot',0,'','5.80103546715012');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД1','Fit',0,'','74.3031528715286');
INSERT INTO "AnastGener_blcks_io" VALUES('pipeGlinsk','Fi',0,'','45.0910482419709');
INSERT INTO "AnastGener_blcks_io" VALUES('pipeGlinsk','Pi',1,'КРД2.Po','22.8976185256361');
INSERT INTO "AnastGener_blcks_io" VALUES('pipeGlinsk','Fo',1,'netGlinsk.Fi','45.0772640000808');
INSERT INTO "AnastGener_blcks_io" VALUES('pipeGlinsk','Po',0,'','20.1074178630465');
INSERT INTO "AnastGener_blcks_io" VALUES('pipeGlinsk','So',0,'','0.05');
INSERT INTO "AnastGener_blcks_io" VALUES('pipeGlinsk','lo',0,'','30000');
INSERT INTO "AnastGener_blcks_io" VALUES('pipeGlinsk','Q0',0,'','1');
INSERT INTO "AnastGener_blcks_io" VALUES('pipeGlinsk','f_frq',0,'','100');
INSERT INTO "AnastGener_blcks_io" VALUES('pipeGlinsk','Pti',0,'','22.0485401296011');
INSERT INTO "AnastGener_blcks_io" VALUES('pipeGlinsk','Fto',0,'','45.0581412585703');
INSERT INTO "AnastGener_blcks_io" VALUES('pipeGlinsk','Pt1',0,'','21.1174666297269');
INSERT INTO "AnastGener_blcks_io" VALUES('pipeGlinsk','Ft1',0,'','45.6513947277947');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД2','Fi',0,'','45.0928502345238');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД2','Pi',1,'node20.Po1','23.9722174041193');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД2','Ti',0,'','273');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД2','Fo',1,'pipeGlinsk.Fi','45.0910443415599');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД2','Po',0,'','22.8976402124282');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД2','To',0,'','272.750434143376');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД2','So',0,'','0.071');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД2','lo',0,'','10');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД2','S_kl1',0,'','0.05');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД2','S_kl2',0,'','0');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД2','l_kl1',1,'КРД2им.out','53.7678101873371');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД2','l_kl2',0,'','0');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД2','Q0',0,'','1');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД2','Kln',0,'','2');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД2','Kpr',0,'','0.95');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД2','Ct',0,'','20');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД2','Riz',0,'','20');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД2','noBack',0,'','0');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД2','Fwind',0,'','1');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД2','Twind',0,'','273');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД2','f_frq',0,'','100');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД2','Pot',0,'','20.2795149522328');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД2','Fit',0,'','33.1004015952682');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД4','Fi',0,'','51.360785536659');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД4','Pi',1,'PP7.Po','91.954805334504');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД4','Ti',1,'node22.To','302.399492076344');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД4','Fo',1,'netGVD.Fi','51.3607960726521');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД4','Po',0,'','90.2683050886934');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД4','To',0,'','302.229748977642');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД4','So',0,'','0.017');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД4','lo',0,'','10');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД4','S_kl1',0,'','0.014');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД4','S_kl2',0,'','0.014');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД4','l_kl1',0,'','47.9735560935388');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД4','l_kl2',0,'','0');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД4','Q0',0,'','1');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД4','Kln',0,'','1');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД4','Kpr',0,'','0.95');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД4','Ct',0,'','20');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД4','Riz',0,'','20');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД4','noBack',0,'','0');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД4','Fwind',0,'','1');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД4','Twind',0,'','273');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД4','f_frq',0,'','100');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД4','Pot',0,'','90.4882859030793');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД4','Fit',0,'','37.2954967691498');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ7им','pos',0,'','0');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ7им','com',0,'','0');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ7им','st_open',0,'','0');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ7им','st_close',0,'','1');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ7им','t_full',0,'','3');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ7им','t_up',0,'','0.3');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ7им','f_frq',0,'','100');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ7им','tmp_up',0,'','0');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ7им','lst_com',0,'','0');
INSERT INTO "AnastGener_blcks_io" VALUES('PP1','Fi',0,'','101.786662935637');
INSERT INTO "AnastGener_blcks_io" VALUES('PP1','Pi',1,'C1.Po','5.76670971912848');
INSERT INTO "AnastGener_blcks_io" VALUES('PP1','Fo',1,'node1.Fi','101.782948135795');
INSERT INTO "AnastGener_blcks_io" VALUES('PP1','Po',0,'','5.64597592629807');
INSERT INTO "AnastGener_blcks_io" VALUES('PP1','dP',0,'','12.0730266457788');
INSERT INTO "AnastGener_blcks_io" VALUES('PP1','Sdf',0,'','0.2');
INSERT INTO "AnastGener_blcks_io" VALUES('PP1','So',0,'','0.283');
INSERT INTO "AnastGener_blcks_io" VALUES('PP1','lo',0,'','20');
INSERT INTO "AnastGener_blcks_io" VALUES('PP1','Q0',0,'','1');
INSERT INTO "AnastGener_blcks_io" VALUES('PP1','f_frq',0,'','100');
INSERT INTO "AnastGener_blcks_io" VALUES('PP1','Pot',0,'','5.52904727002653');
INSERT INTO "AnastGener_blcks_io" VALUES('PP1','Fit',0,'','73.952362760328');
INSERT INTO "AnastGener_blcks_io" VALUES('PP3','Fi',0,'','98.6434002428129');
INSERT INTO "AnastGener_blcks_io" VALUES('PP3','Pi',1,'node19.Po','24.5284785922438');
INSERT INTO "AnastGener_blcks_io" VALUES('PP3','Fo',1,'КШ6.Fi','98.6412312683695');
INSERT INTO "AnastGener_blcks_io" VALUES('PP3','Po',0,'','24.2359180185422');
INSERT INTO "AnastGener_blcks_io" VALUES('PP3','dP',0,'','29.2554203450064');
INSERT INTO "AnastGener_blcks_io" VALUES('PP3','Sdf',0,'','0.06');
INSERT INTO "AnastGener_blcks_io" VALUES('PP3','So',0,'','0.071');
INSERT INTO "AnastGener_blcks_io" VALUES('PP3','lo',0,'','10');
INSERT INTO "AnastGener_blcks_io" VALUES('PP3','Q0',0,'','1');
INSERT INTO "AnastGener_blcks_io" VALUES('PP3','f_frq',0,'','100');
INSERT INTO "AnastGener_blcks_io" VALUES('PP3','Pot',0,'','24.4923380432146');
INSERT INTO "AnastGener_blcks_io" VALUES('PP3','Fit',0,'','71.9508231998964');
INSERT INTO "AnastGener_blcks_io" VALUES('PP5','Fi',0,'','52.9831575961716');
INSERT INTO "AnastGener_blcks_io" VALUES('PP5','Pi',1,'C2.Po','23.8299737657275');
INSERT INTO "AnastGener_blcks_io" VALUES('PP5','Fo',1,'node21.Fi','52.9821217310151');
INSERT INTO "AnastGener_blcks_io" VALUES('PP5','Po',0,'','23.7051228994026');
INSERT INTO "AnastGener_blcks_io" VALUES('PP5','dP',0,'','12.4853019449828');
INSERT INTO "AnastGener_blcks_io" VALUES('PP5','Sdf',0,'','0.05');
INSERT INTO "AnastGener_blcks_io" VALUES('PP5','So',0,'','0.071');
INSERT INTO "AnastGener_blcks_io" VALUES('PP5','lo',0,'','5');
INSERT INTO "AnastGener_blcks_io" VALUES('PP5','Q0',0,'','1');
INSERT INTO "AnastGener_blcks_io" VALUES('PP5','f_frq',0,'','100');
INSERT INTO "AnastGener_blcks_io" VALUES('PP5','Pot',0,'','23.5001559218607');
INSERT INTO "AnastGener_blcks_io" VALUES('PP5','Fit',0,'','38.8654859427924');
INSERT INTO "AnastGener_blcks_io" VALUES('PP7','Fi',0,'','51.3606816283394');
INSERT INTO "AnastGener_blcks_io" VALUES('PP7','Pi',1,'node22.Po','92.2918128025351');
INSERT INTO "AnastGener_blcks_io" VALUES('PP7','Fo',1,'КРД4.Fi','51.360588088649');
INSERT INTO "AnastGener_blcks_io" VALUES('PP7','Po',0,'','91.9547927778661');
INSERT INTO "AnastGener_blcks_io" VALUES('PP7','dP',0,'','33.700033553784');
INSERT INTO "AnastGener_blcks_io" VALUES('PP7','Sdf',0,'','0.015');
INSERT INTO "AnastGener_blcks_io" VALUES('PP7','So',0,'','0.017');
INSERT INTO "AnastGener_blcks_io" VALUES('PP7','lo',0,'','10');
INSERT INTO "AnastGener_blcks_io" VALUES('PP7','Q0',0,'','1');
INSERT INTO "AnastGener_blcks_io" VALUES('PP7','f_frq',0,'','100');
INSERT INTO "AnastGener_blcks_io" VALUES('PP7','Pot',0,'','91.2198881512939');
INSERT INTO "AnastGener_blcks_io" VALUES('PP7','Fit',0,'','37.309848096239');
INSERT INTO "AnastGener_blcks_io" VALUES('netFakel','Fi',0,'','3.0704221229543e-09');
INSERT INTO "AnastGener_blcks_io" VALUES('netFakel','Pi',1,'КШ5.Po','1.00000000000176');
INSERT INTO "AnastGener_blcks_io" VALUES('netFakel','Po',0,'','1');
INSERT INTO "AnastGener_blcks_io" VALUES('netFakel','So',0,'','0.002');
INSERT INTO "AnastGener_blcks_io" VALUES('netFakel','Kpr',0,'','0.95');
INSERT INTO "AnastGener_blcks_io" VALUES('netFakel','Noise',0,'','1');
INSERT INTO "AnastGener_blcks_io" VALUES('netFakel','Q0',0,'','1');
INSERT INTO "AnastGener_blcks_io" VALUES('netFakel','f_frq',0,'','100');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ1','Fi',0,'','102.819008036853');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ1','Pi',1,'SrcGas.Po','8.85690216561261');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ1','Ti',0,'','293');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ1','Fo',1,'КРД1.Fi','102.818965143006');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ1','Po',0,'','8.83555987021978');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ1','To',0,'','292.966445669794');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ1','So',0,'','0.385');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ1','lo',0,'','5');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ1','S_kl1',0,'','0.385');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ1','S_kl2',0,'','0.02');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ1','l_kl1',1,'КШ1им.pos','100');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ1','l_kl2',1,'КШ2им.pos','0');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ1','Q0',0,'','1');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ1','Kln',0,'','1');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ1','Kpr',0,'','0.95');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ1','Ct',0,'','20');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ1','Riz',0,'','20');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ1','noBack',0,'','0');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ1','Fwind',0,'','1');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ1','Twind',0,'','273');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ1','f_frq',0,'','100');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ1','Pot',0,'','8.78987399044649');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ1','Fit',0,'','163.572788647632');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ21им','pos',0,'','100');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ21им','com',0,'','1');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ21им','st_open',0,'','1');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ21им','st_close',0,'','0');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ21им','t_full',0,'','10');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ21им','t_up',0,'','0.6');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ21им','f_frq',0,'','100');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ21им','tmp_up',0,'','0');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ21им','lst_com',0,'','1');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ5','Fi',0,'','0');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ5','Pi',1,'node1.Po1','5.64601199777216');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ5','Ti',1,'КРД1.To','290.489258934727');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ5','Fo',1,'netFakel.Fi','-1.04475992129625e-09');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ5','Po',0,'','0.999999999998311');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ5','To',0,'','272.999999994003');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ5','So',0,'','0.2');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ5','lo',0,'','10');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ5','S_kl1',0,'','0.002');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ5','S_kl2',0,'','0.001');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ5','l_kl1',1,'КШ5им.pos','0');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ5','l_kl2',0,'','0');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ5','Q0',0,'','1');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ5','Kln',0,'','1');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ5','Kpr',0,'','0.95');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ5','Ct',0,'','20');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ5','Riz',0,'','20');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ5','noBack',0,'','0');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ5','Fwind',0,'','1');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ5','Twind',0,'','273');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ5','f_frq',0,'','100');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ5','Pot',0,'','1.000000002619');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ5','Fit',0,'','2.47032822920623e-323');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ6','Fi',0,'','98.6419638933836');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ6','Pi',1,'PP3.Po','24.2360881650554');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ6','Ti',1,'node19.To','305.1697981262');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ6','Fo',1,'node20.Fi','98.6409328014353');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ6','Po',0,'','24.0189450233039');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ6','To',0,'','305.108205301676');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ6','So',0,'','0.071');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ6','lo',0,'','5');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ6','S_kl1',0,'','0.07');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ6','S_kl2',0,'','0.002');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ6','l_kl1',1,'КШ6им.pos','100');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ6','l_kl2',1,'КШ7им.pos','0');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ6','Q0',0,'','1');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ6','Kln',0,'','1');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ6','Kpr',0,'','0.95');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ6','Ct',0,'','100');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ6','Riz',0,'','100');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ6','noBack',0,'','0');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ6','Fwind',0,'','1');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ6','Twind',0,'','273');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ6','f_frq',0,'','100');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ6','Pot',0,'','24.0824664654201');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ6','Fit',0,'','71.9528087874881');
INSERT INTO "AnastGener_blcks_io" VALUES('node1','Fi',0,'','101.783434707291');
INSERT INTO "AnastGener_blcks_io" VALUES('node1','Pi',1,'PP1.Po','5.6459920609358');
INSERT INTO "AnastGener_blcks_io" VALUES('node1','Fo1',1,'КШ5.Fi','0');
INSERT INTO "AnastGener_blcks_io" VALUES('node1','Po1',0,'','5.6459970207923');
INSERT INTO "AnastGener_blcks_io" VALUES('node1','So1',0,'','0.02');
INSERT INTO "AnastGener_blcks_io" VALUES('node1','lo1',0,'','20');
INSERT INTO "AnastGener_blcks_io" VALUES('node1','Fo2',1,'node1_1.Fi','101.771792002887');
INSERT INTO "AnastGener_blcks_io" VALUES('node1','Po2',0,'','5.57973237593473');
INSERT INTO "AnastGener_blcks_io" VALUES('node1','So2',0,'','0.283');
INSERT INTO "AnastGener_blcks_io" VALUES('node1','lo2',0,'','60');
INSERT INTO "AnastGener_blcks_io" VALUES('node1','Q0',0,'','1');
INSERT INTO "AnastGener_blcks_io" VALUES('node1','f_frq',0,'','100');
INSERT INTO "AnastGener_blcks_io" VALUES('node1','F1tmp',0,'','0.000294284150332785');
INSERT INTO "AnastGener_blcks_io" VALUES('node1','F2tmp',0,'','101.783151957637');
INSERT INTO "AnastGener_blcks_io" VALUES('node1','Pot1',0,'','5.48529527455907');
INSERT INTO "AnastGener_blcks_io" VALUES('node1','Pot2',0,'','5.40120593993749');
INSERT INTO "AnastGener_blcks_io" VALUES('node20','Fi',0,'','98.6405588390338');
INSERT INTO "AnastGener_blcks_io" VALUES('node20','Pi',1,'КШ6.Po','24.0187685660855');
INSERT INTO "AnastGener_blcks_io" VALUES('node20','Fo1',1,'КРД2.Fi','45.0928654475119');
INSERT INTO "AnastGener_blcks_io" VALUES('node20','Po1',0,'','23.9721919413576');
INSERT INTO "AnastGener_blcks_io" VALUES('node20','So1',0,'','0.071');
INSERT INTO "AnastGener_blcks_io" VALUES('node20','lo1',0,'','120');
INSERT INTO "AnastGener_blcks_io" VALUES('node20','Fo2',1,'КШ21.Fi','53.5211736714975');
INSERT INTO "AnastGener_blcks_io" VALUES('node20','Po2',0,'','23.9560439575375');
INSERT INTO "AnastGener_blcks_io" VALUES('node20','So2',0,'','0.071');
INSERT INTO "AnastGener_blcks_io" VALUES('node20','lo2',0,'','5');
INSERT INTO "AnastGener_blcks_io" VALUES('node20','Q0',0,'','1');
INSERT INTO "AnastGener_blcks_io" VALUES('node20','f_frq',0,'','100');
INSERT INTO "AnastGener_blcks_io" VALUES('node20','F1tmp',0,'','45.1182995617256');
INSERT INTO "AnastGener_blcks_io" VALUES('node20','F2tmp',0,'','53.5223163578723');
INSERT INTO "AnastGener_blcks_io" VALUES('node20','Pot1',0,'','23.9995804567437');
INSERT INTO "AnastGener_blcks_io" VALUES('node20','Pot2',0,'','23.9683452808346');
INSERT INTO "AnastGener_blcks_io" VALUES('node21','Fi',0,'','52.982341227079');
INSERT INTO "AnastGener_blcks_io" VALUES('node21','Pi',1,'PP5.Po','23.7051896999527');
INSERT INTO "AnastGener_blcks_io" VALUES('node21','Fo1',2,'KM102.КШ111.Fi','52.9738661225957');
INSERT INTO "AnastGener_blcks_io" VALUES('node21','Po1',6,'KM102.КШ111.Pi','22.9064730806253');
INSERT INTO "AnastGener_blcks_io" VALUES('node21','So1',0,'','0.02');
INSERT INTO "AnastGener_blcks_io" VALUES('node21','lo1',0,'','50');
INSERT INTO "AnastGener_blcks_io" VALUES('node21','Fo2',2,'КМ202.КШ111.Fi','-1.23516411460312e-321');
INSERT INTO "AnastGener_blcks_io" VALUES('node21','Po2',6,'КМ202.КШ111.Pi','23.7051961731689');
INSERT INTO "AnastGener_blcks_io" VALUES('node21','So2',0,'','0.02');
INSERT INTO "AnastGener_blcks_io" VALUES('node21','lo2',0,'','50');
INSERT INTO "AnastGener_blcks_io" VALUES('node21','Fo3',2,'KM302.КШ111.Fi','-1.23516411460312e-321');
INSERT INTO "AnastGener_blcks_io" VALUES('node21','Po3',6,'KM302.КШ111.Pi','23.7051982034324');
INSERT INTO "AnastGener_blcks_io" VALUES('node21','So3',0,'','0.02');
INSERT INTO "AnastGener_blcks_io" VALUES('node21','lo3',0,'','50');
INSERT INTO "AnastGener_blcks_io" VALUES('node21','Q0',0,'','1');
INSERT INTO "AnastGener_blcks_io" VALUES('node21','f_frq',0,'','100');
INSERT INTO "AnastGener_blcks_io" VALUES('node21','F1tmp',0,'','52.9765585722365');
INSERT INTO "AnastGener_blcks_io" VALUES('node21','F2tmp',0,'','0.00292197182873421');
INSERT INTO "AnastGener_blcks_io" VALUES('node21','F3tmp',0,'','0.00290575918255005');
INSERT INTO "AnastGener_blcks_io" VALUES('node21','Pot1',0,'','22.0182441164267');
INSERT INTO "AnastGener_blcks_io" VALUES('node21','Pot2',0,'','23.8828420056428');
INSERT INTO "AnastGener_blcks_io" VALUES('node21','Pot3',0,'','23.0910838961391');
INSERT INTO "AnastGener_blcks_io" VALUES('node22','Fi1',6,'KM102.КШ114.Fo','51.3615481586258');
INSERT INTO "AnastGener_blcks_io" VALUES('node22','Pi1',2,'KM102.КШ114.Po','93.8144587034857');
INSERT INTO "AnastGener_blcks_io" VALUES('node22','Ti1',2,'KM102.КШ114.To','302.4562956651');
INSERT INTO "AnastGener_blcks_io" VALUES('node22','Si1',0,'','0.007');
INSERT INTO "AnastGener_blcks_io" VALUES('node22','Fi2',6,'КМ202.КШ114.Fo','0.000255669438255273');
INSERT INTO "AnastGener_blcks_io" VALUES('node22','Pi2',2,'КМ202.КШ114.Po','92.2918150363886');
INSERT INTO "AnastGener_blcks_io" VALUES('node22','Ti2',2,'КМ202.КШ114.To','273.000000010071');
INSERT INTO "AnastGener_blcks_io" VALUES('node22','Si2',0,'','0.007');
INSERT INTO "AnastGener_blcks_io" VALUES('node22','Fi3',6,'KM302.КШ114.Fo','-0.000907702075528743');
INSERT INTO "AnastGener_blcks_io" VALUES('node22','Pi3',2,'KM302.КШ114.Po','92.291831774502');
INSERT INTO "AnastGener_blcks_io" VALUES('node22','Ti3',2,'KM302.КШ114.To','273.000000010185');
INSERT INTO "AnastGener_blcks_io" VALUES('node22','Si3',0,'','0.007');
INSERT INTO "AnastGener_blcks_io" VALUES('node22','Fo',1,'PP7.Fi','51.3607609962183');
INSERT INTO "AnastGener_blcks_io" VALUES('node22','Po',0,'','92.2918263245539');
INSERT INTO "AnastGener_blcks_io" VALUES('node22','To',0,'','302.399456786639');
INSERT INTO "AnastGener_blcks_io" VALUES('node22','So',0,'','0.007');
INSERT INTO "AnastGener_blcks_io" VALUES('node22','lo',0,'','10');
INSERT INTO "AnastGener_blcks_io" VALUES('node22','Q0',0,'','1');
INSERT INTO "AnastGener_blcks_io" VALUES('node22','Ct',0,'','20');
INSERT INTO "AnastGener_blcks_io" VALUES('node22','Riz',0,'','20');
INSERT INTO "AnastGener_blcks_io" VALUES('node22','Fwind',0,'','1');
INSERT INTO "AnastGener_blcks_io" VALUES('node22','Twind',0,'','273');
INSERT INTO "AnastGener_blcks_io" VALUES('node22','f_frq',0,'','100');
INSERT INTO "AnastGener_blcks_io" VALUES('node22','FoR',0,'','59.9999999999999');
INSERT INTO "AnastGener_blcks_io" VALUES('node22','Pot',0,'','91.8396872231701');
INSERT INTO "AnastGener_blcks_io" VALUES('node22','Fit',0,'','37.3244663061258');
INSERT INTO "AnastGener_blcks_io" VALUES('node19','Fi1',6,'KM101.КШ104.Fo','49.4185074097455');
INSERT INTO "AnastGener_blcks_io" VALUES('node19','Pi1',2,'KM101.КШ104.Po','24.7968997434163');
INSERT INTO "AnastGener_blcks_io" VALUES('node19','Ti1',2,'KM101.КШ104.To','305.949651169365');
INSERT INTO "AnastGener_blcks_io" VALUES('node19','Si1',0,'','0.0314');
INSERT INTO "AnastGener_blcks_io" VALUES('node19','Fi2',6,'KM201.КШ104.Fo','49.2377647439631');
INSERT INTO "AnastGener_blcks_io" VALUES('node19','Pi2',2,'KM201.КШ104.Po','24.7949638734381');
INSERT INTO "AnastGener_blcks_io" VALUES('node19','Ti2',2,'KM201.КШ104.To','304.446671962899');
INSERT INTO "AnastGener_blcks_io" VALUES('node19','Si2',0,'','0.0314');
INSERT INTO "AnastGener_blcks_io" VALUES('node19','Fi3',6,'KM301.КШ104.Fo','-0.00844168607450759');
INSERT INTO "AnastGener_blcks_io" VALUES('node19','Pi3',2,'KM301.КШ104.Po','24.528550913881');
INSERT INTO "AnastGener_blcks_io" VALUES('node19','Ti3',2,'KM301.КШ104.To','273.000000009147');
INSERT INTO "AnastGener_blcks_io" VALUES('node19','Si3',0,'','0.0314');
INSERT INTO "AnastGener_blcks_io" VALUES('node19','Fo',1,'PP3.Fi','98.6439088077521');
INSERT INTO "AnastGener_blcks_io" VALUES('node19','Po',0,'','24.5285606457926');
INSERT INTO "AnastGener_blcks_io" VALUES('node19','To',0,'','305.169812547387');
INSERT INTO "AnastGener_blcks_io" VALUES('node19','So',0,'','0.0314');
INSERT INTO "AnastGener_blcks_io" VALUES('node19','lo',0,'','40');
INSERT INTO "AnastGener_blcks_io" VALUES('node19','Q0',0,'','1');
INSERT INTO "AnastGener_blcks_io" VALUES('node19','Ct',0,'','20');
INSERT INTO "AnastGener_blcks_io" VALUES('node19','Riz',0,'','20');
INSERT INTO "AnastGener_blcks_io" VALUES('node19','Fwind',0,'','1');
INSERT INTO "AnastGener_blcks_io" VALUES('node19','Twind',0,'','273');
INSERT INTO "AnastGener_blcks_io" VALUES('node19','f_frq',0,'','100');
INSERT INTO "AnastGener_blcks_io" VALUES('node19','FoR',0,'','85.0636924379784');
INSERT INTO "AnastGener_blcks_io" VALUES('node19','Pot',0,'','25.0241377766255');
INSERT INTO "AnastGener_blcks_io" VALUES('node19','Fit',0,'','71.9722775668757');
INSERT INTO "AnastGener_blcks_io" VALUES('C1','Fi',0,'','102.815575653421');
INSERT INTO "AnastGener_blcks_io" VALUES('C1','Pi',1,'КРД1.Po','5.79937101697185');
INSERT INTO "AnastGener_blcks_io" VALUES('C1','Si',0,'','0.385');
INSERT INTO "AnastGener_blcks_io" VALUES('C1','Fo',1,'PP1.Fi','101.78647353677');
INSERT INTO "AnastGener_blcks_io" VALUES('C1','Po',0,'','5.76670208641515');
INSERT INTO "AnastGener_blcks_io" VALUES('C1','So',0,'','0.283');
INSERT INTO "AnastGener_blcks_io" VALUES('C1','lo',0,'','5');
INSERT INTO "AnastGener_blcks_io" VALUES('C1','Fo_ж',0,'','0');
INSERT INTO "AnastGener_blcks_io" VALUES('C1','Po_ж',0,'','5.86568894856286');
INSERT INTO "AnastGener_blcks_io" VALUES('C1','Lж',0,'','1.23731195902465');
INSERT INTO "AnastGener_blcks_io" VALUES('C1','ProcЖ',0,'','0.01');
INSERT INTO "AnastGener_blcks_io" VALUES('C1','Vap',0,'','80');
INSERT INTO "AnastGener_blcks_io" VALUES('C1','Q0',0,'','1');
INSERT INTO "AnastGener_blcks_io" VALUES('C1','Qж',0,'','1000');
INSERT INTO "AnastGener_blcks_io" VALUES('C1','f_frq',0,'','100');
INSERT INTO "AnastGener_blcks_io" VALUES('C1','Pot',0,'','5.74237264252839');
INSERT INTO "AnastGener_blcks_io" VALUES('C1','Fit',0,'','73.6939262990129');
INSERT INTO "AnastGener_blcks_io" VALUES('C2','Fi',0,'','53.5192740877374');
INSERT INTO "AnastGener_blcks_io" VALUES('C2','Pi',1,'КШ21.Po','23.8929797464549');
INSERT INTO "AnastGener_blcks_io" VALUES('C2','Si',0,'','0.071');
INSERT INTO "AnastGener_blcks_io" VALUES('C2','Fo',1,'PP5.Fi','52.9830487013364');
INSERT INTO "AnastGener_blcks_io" VALUES('C2','Po',0,'','23.8299326924569');
INSERT INTO "AnastGener_blcks_io" VALUES('C2','So',0,'','0.071');
INSERT INTO "AnastGener_blcks_io" VALUES('C2','lo',0,'','5');
INSERT INTO "AnastGener_blcks_io" VALUES('C2','Fo_ж',0,'','0');
INSERT INTO "AnastGener_blcks_io" VALUES('C2','Po_ж',0,'','23.8891625264619');
INSERT INTO "AnastGener_blcks_io" VALUES('C2','Lж',0,'','1.69204578351933');
INSERT INTO "AnastGener_blcks_io" VALUES('C2','ProcЖ',0,'','0.01');
INSERT INTO "AnastGener_blcks_io" VALUES('C2','Vap',0,'','35');
INSERT INTO "AnastGener_blcks_io" VALUES('C2','Q0',0,'','1');
INSERT INTO "AnastGener_blcks_io" VALUES('C2','Qж',0,'','1000');
INSERT INTO "AnastGener_blcks_io" VALUES('C2','f_frq',0,'','100');
INSERT INTO "AnastGener_blcks_io" VALUES('C2','Pot',0,'','23.7347401295504');
INSERT INTO "AnastGener_blcks_io" VALUES('C2','Fit',0,'','38.4665359341126');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ1им','pos',0,'','100');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ1им','com',0,'','1');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ1им','st_open',0,'','1');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ1им','st_close',0,'','0');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ1им','t_full',0,'','5');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ1им','t_up',0,'','0.5');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ1им','f_frq',0,'','100');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ1им','tmp_up',0,'','0');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ1им','lst_com',0,'','1');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ2им','pos',0,'','0');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ2им','com',0,'','0');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ2им','st_open',0,'','0');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ2им','st_close',0,'','1');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ2им','t_full',0,'','3');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ2им','t_up',0,'','0.3');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ2им','f_frq',0,'','100');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ2им','tmp_up',0,'','0');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ2им','lst_com',0,'','0');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ5им','pos',0,'','0');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ5им','com',0,'','0');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ5им','st_open',0,'','0');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ5им','st_close',0,'','1');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ5им','t_full',0,'','5');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ5им','t_up',0,'','0.5');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ5им','f_frq',0,'','100');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ5им','tmp_up',0,'','0');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ5им','lst_com',0,'','0');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД1','t_kl1',0,'','5');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД1','t_kl2',0,'','5');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД1','tmp_l1',0,'','40.1544716830457');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД1','tmp_l2',0,'','0');
INSERT INTO "AnastGener_blcks_io" VALUES('node1_1','Kpr',0,'','0.95');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ21','t_kl1',0,'','10');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ21','t_kl2',0,'','5');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ21','tmp_l1',0,'','99.9999999999929');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ21','tmp_l2',0,'','0');
INSERT INTO "AnastGener_blcks_io" VALUES('SrcGas','Kpr',0,'','0.95');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД2','t_kl1',0,'','3');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД2','t_kl2',0,'','5');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД2','tmp_l1',0,'','53.7745330449001');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД2','tmp_l2',0,'','0');
INSERT INTO "AnastGener_blcks_io" VALUES('pipeGlinsk','Kpr',0,'','0.95');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД4','t_kl1',0,'','10');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД4','t_kl2',0,'','5');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД4','tmp_l1',0,'','47.9719634666091');
INSERT INTO "AnastGener_blcks_io" VALUES('КРД4','tmp_l2',0,'','0');
INSERT INTO "AnastGener_blcks_io" VALUES('PP1','Kpr',0,'','0.95');
INSERT INTO "AnastGener_blcks_io" VALUES('PP3','Kpr',0,'','0.95');
INSERT INTO "AnastGener_blcks_io" VALUES('PP5','Kpr',0,'','0.95');
INSERT INTO "AnastGener_blcks_io" VALUES('PP7','Kpr',0,'','0.95');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ1','t_kl1',0,'','10');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ1','t_kl2',0,'','5');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ1','tmp_l1',0,'','99.9999999999929');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ1','tmp_l2',0,'','1.23516411460312e-321');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ5','t_kl1',0,'','10');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ5','t_kl2',0,'','5');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ5','tmp_l1',0,'','0');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ5','tmp_l2',0,'','0');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ6','t_kl1',0,'','10');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ6','t_kl2',0,'','5');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ6','tmp_l1',0,'','99.9999999999929');
INSERT INTO "AnastGener_blcks_io" VALUES('КШ6','tmp_l2',0,'','0');
INSERT INTO "AnastGener_blcks_io" VALUES('node1','Kpr',0,'','0.95');
INSERT INTO "AnastGener_blcks_io" VALUES('node20','Kpr',0,'','0.95');
INSERT INTO "AnastGener_blcks_io" VALUES('node21','Kpr',0,'','0.95');
INSERT INTO "AnastGener_blcks_io" VALUES('node22','Kpr',0,'','0.95');
INSERT INTO "AnastGener_blcks_io" VALUES('node19','Kpr',0,'','0.95');
CREATE TABLE 'AnastGener_cntr_blocks_io' ("BLK_ID" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"TLNK" INTEGER DEFAULT '' ,"LNK" TEXT DEFAULT '' ,"VAL" TEXT DEFAULT '' , PRIMARY KEY ("BLK_ID","ID"));
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PC_КРД2','var',2,'Anast1to2node.P_КРД2.out','23.9721410874234');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PC_КРД2','sp',0,'','24');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PC_КРД2','max',0,'','50');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PC_КРД2','min',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PC_КРД2','out',0,'','53.7669757516282');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PC_КРД2','auto',0,'','1');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PC_КРД2','casc',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PC_КРД2','Kp',0,'','-0.2');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PC_КРД2','Ti',0,'','5000');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PC_КРД2','Td',0,'','1000');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PC_КРД2','Tf',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PC_КРД2','Hup',0,'','100');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PC_КРД2','Hdwn',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PC_КРД2','Zi',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PC_КРД2','K1',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PC_КРД2','in1',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PC_КРД2','K2',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PC_КРД2','in2',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PC_КРД2','K3',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PC_КРД2','in3',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PC_КРД2','K4',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PC_КРД2','in4',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PC_КРД2','cycle',0,'','1000');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PC_КРД2','int',0,'','53.7794366699675');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PC_КРД2','dif',0,'','47.9429648215381');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PC_КРД2','lag',0,'','-0.0013173533086956');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD1','var',2,'Anast1to2node.КРД1.Po','5.79943241234207');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD1','sp',0,'','5.8');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD1','max',0,'','10');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD1','min',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD1','out',0,'','40.1610149307074');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD1','auto',0,'','1');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD1','casc',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD1','Kp',0,'','1');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD1','Ti',0,'','5000');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD1','Td',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD1','Tf',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD1','Hup',0,'','100');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD1','Hdwn',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD1','Zi',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD1','K1',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD1','in1',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD1','K2',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD1','in2',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD1','K3',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD1','in3',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD1','K4',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD1','in4',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD1','cycle',0,'','1000');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD1','int',0,'','40.1553390541281');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD1','dif',0,'','57.9943241234207');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD1','lag',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD3','var',2,'Anast1to2node.КРД4.Pi','91.9548147885053');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD3','sp',0,'','91.9539072904281');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD3','max',0,'','120');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD3','min',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD3','out',6,'Anast1to2node.КРД4.l_kl1','47.9736984007829');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD3','auto',0,'','1');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD3','casc',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD3','Kp',0,'','-3');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD3','Ti',0,'','5000');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD3','Td',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD3','Tf',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD3','Hup',0,'','100');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD3','Hdwn',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD3','Zi',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD3','K1',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD3','in1',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD3','K2',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD3','in2',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD3','K3',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD3','in3',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD3','K4',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD3','in4',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD3','cycle',0,'','1000');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD3','int',0,'','47.97142965559');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD3','dif',0,'','76.6290123237544');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD3','lag',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD3','manIn',0,'','47.9736984007829');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PC_КРД2','manIn',0,'','53.7669757516282');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PC_КРД2','f_frq',0,'','5');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD1','manIn',0,'','40.1610149307074');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD1','f_frq',0,'','5');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD3','f_frq',0,'','5');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PC_КРД2','Kd',0,'','1');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PC_КРД2','Tzd',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PC_КРД2','followSp',0,'','1');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD1','Kd',0,'','1');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD1','Tzd',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD1','followSp',0,'','1');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD3','Kd',0,'','1');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD3','Tzd',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('PCKRD3','followSp',0,'','1');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM101','var',2,'KM101.КМ101_2.Kzp','0.883920311310893');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM101','sp',0,'','0.9');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM101','max',0,'','2');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM101','min',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM101','manIn',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM101','out',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM101','auto',0,'','1');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM101','casc',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM101','Kp',0,'','-1');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM101','Ti',0,'','5000');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM101','Kd',0,'','1');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM101','Td',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM101','Tzd',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM101','Hup',0,'','100');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM101','Hdwn',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM101','Zi',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM101','followSp',0,'','1');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM101','K1',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM101','in1',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM101','K2',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM101','in2',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM101','K3',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM101','in3',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM101','K4',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM101','in4',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM101','f_frq',0,'','5');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM101','int',0,'','0.803984434455357');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM101','dif',0,'','44.1960155655446');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM101','lag',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM201','var',2,'KM201.КМ101_2.Kzp','0.885959274417412');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM201','sp',0,'','0.9');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM201','max',0,'','2');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM201','min',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM201','manIn',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM201','out',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM201','auto',0,'','1');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM201','casc',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM201','Kp',0,'','-1');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM201','Ti',0,'','5000');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM201','Kd',0,'','1');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM201','Td',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM201','Tzd',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM201','Hup',0,'','100');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM201','Hdwn',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM201','Zi',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM201','followSp',0,'','1');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM201','K1',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM201','in1',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM201','K2',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM201','in2',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM201','K3',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM201','in3',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM201','K4',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM201','in4',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM201','f_frq',0,'','5');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM201','int',0,'','0.702036279129392');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM201','dif',0,'','44.2979637208706');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM201','lag',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM301','var',2,'KM301.КМ101_2.Kzp','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM301','sp',0,'','0.9');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM301','max',0,'','2');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM301','min',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM301','manIn',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM301','out',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM301','auto',0,'','1');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM301','casc',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM301','Kp',0,'','-1');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM301','Ti',0,'','5000');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM301','Kd',0,'','1');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM301','Td',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM301','Tzd',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM301','Hup',0,'','100');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM301','Hdwn',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM301','Zi',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM301','followSp',0,'','1');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM301','K1',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM301','in1',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM301','K2',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM301','in2',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM301','K3',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM301','in3',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM301','K4',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM301','in4',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM301','f_frq',0,'','5');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM301','int',0,'','45');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM301','dif',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM301','lag',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM102','var',2,'KM102.KM102_2.Kzp','0.500474947141475');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM102','sp',0,'','0.9');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM102','max',0,'','2');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM102','min',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM102','manIn',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM102','out',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM102','auto',0,'','1');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM102','casc',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM102','Kp',0,'','-1');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM102','Ti',0,'','5000');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM102','Kd',0,'','1');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM102','Td',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM102','Tzd',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM102','Hup',0,'','100');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM102','Hdwn',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM102','Zi',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM102','followSp',0,'','1');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM102','K1',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM102','in1',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM102','K2',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM102','in2',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM102','K3',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM102','in3',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM102','K4',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM102','in4',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM102','f_frq',0,'','5');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM102','int',0,'','19.9762526429262');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM102','dif',0,'','25.0237473570738');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM102','lag',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM202','var',2,'КМ202.KM102_2.Kzp','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM202','sp',0,'','0.9');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM202','max',0,'','2');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM202','min',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM202','manIn',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM202','out',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM202','auto',0,'','1');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM202','casc',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM202','Kp',0,'','-1');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM202','Ti',0,'','5000');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM202','Kd',0,'','1');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM202','Td',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM202','Tzd',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM202','Hup',0,'','100');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM202','Hdwn',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM202','Zi',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM202','followSp',0,'','1');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM202','K1',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM202','in1',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM202','K2',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM202','in2',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM202','K3',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM202','in3',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM202','K4',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM202','in4',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM202','f_frq',0,'','5');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM202','int',0,'','45');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM202','dif',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM202','lag',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM302','var',2,'KM302.KM102_2.Kzp','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM302','sp',0,'','0.9');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM302','max',0,'','2');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM302','min',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM302','manIn',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM302','out',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM302','auto',0,'','1');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM302','casc',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM302','Kp',0,'','-1');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM302','Ti',0,'','5000');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM302','Kd',0,'','1');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM302','Td',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM302','Tzd',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM302','Hup',0,'','100');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM302','Hdwn',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM302','Zi',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM302','followSp',0,'','1');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM302','K1',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM302','in1',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM302','K2',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM302','in2',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM302','K3',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM302','in3',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM302','K4',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM302','in4',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM302','f_frq',0,'','5');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM302','int',0,'','45');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM302','dif',0,'','0');
INSERT INTO "AnastGener_cntr_blocks_io" VALUES('SurgeKM302','lag',0,'','0');
CREATE TABLE 'AnastGener_cntr_prm' ("SHIFR" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '' ,"IO" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#IO" TEXT DEFAULT '' ,"uk#IO" TEXT DEFAULT '' , PRIMARY KEY ("SHIFR"));
INSERT INTO "AnastGener_cntr_prm" VALUES('PC_КРД1','PC_KRD1','The pressure regulator at the input of the CS.',1,'PCKRD1.sp:sp
PCKRD1.var:var
*s.at:ed:Dimension
*i.1:prec:Precision
PCKRD1.out:out
PCKRD1.manIn:manIn
PCKRD1.max:max
PCKRD1.min:min
PCKRD1.auto:auto
PCKRD1.Kp:Kp
PCKRD1.Ti:Ti
PCKRD1.Kd:Kd
PCKRD1.Td:Td
PCKRD1.Tzd:Tzd
PCKRD1.Tf:Tf
PCKRD1.Zi:Zi
PCKRD1.Hup:Hup
PCKRD1.Hdwn:Hdwn','PC КРД1','Регулятор давления на входе КС.','РС_КРД1','Регулятор тиску на вході КС.','PCKRD1.sp:sp
PCKRD1.var:var
*s.ат:ed:Ед. измерения
*i.1:prec:Точность
PCKRD1.out:out
PCKRD1.manIn:manIn
PCKRD1.max:max
PCKRD1.min:min
PCKRD1.auto:auto
PCKRD1.Kp:Kp
PCKRD1.Ti:Ti
PCKRD1.Kd:Kd
PCKRD1.Td:Td
PCKRD1.Tzd:Tzd
PCKRD1.Tf:Tf
PCKRD1.Zi:Zi
PCKRD1.Hup:Hup
PCKRD1.Hdwn:Hdwn','PCKRD1.sp:sp
PCKRD1.var:var
*s.ат:ed:Од.виміру
*i.1:prec:Точність
PCKRD1.out:out
PCKRD1.manIn:manIn
PCKRD1.max:max
PCKRD1.min:min
PCKRD1.auto:auto
PCKRD1.Kp:Kp
PCKRD1.Ti:Ti
PCKRD1.Kd:Kd
PCKRD1.Td:Td
PCKRD1.Tzd:Tzd
PCKRD1.Tf:Tf
PCKRD1.Zi:Zi
PCKRD1.Hup:Hup
PCKRD1.Hdwn:Hdwn');
INSERT INTO "AnastGener_cntr_prm" VALUES('PC_КРД2','PC_KRD2','The pressure regulator between the compressors of low and high pressure.',1,'PC_КРД2.sp:sp
PC_КРД2.var:var
*s.at:ed:Dimension
*i.1:prec:Precision
PC_КРД2.out:out
PC_КРД2.manIn:manIn
PC_КРД2.max:max
PC_КРД2.min:min
PC_КРД2.auto:auto
PC_КРД2.Kp:Kp
PC_КРД2.Ti:Ti
PC_КРД2.Kd:Kd
PC_КРД2.Td:Td
PC_КРД2.Tzd:Tzd
PC_КРД2.Tf:Tf
PC_КРД2.Zi:Zi
PC_КРД2.Hup:Hup
PC_КРД2.Hdwn:Hdwn','PC КРД2','Регулятор давления между компрессорами низкого и высокого давлений.','РС_КРД2','Регулятор тиску між компресорами низького та високого тиску.','PC_КРД2.sp:sp
PC_КРД2.var:var
*s.ат:ed:Ед. измерения
*i.1:prec:Точность
PC_КРД2.out:out
PC_КРД2.manIn:manIn
PC_КРД2.max:max
PC_КРД2.min:min
PC_КРД2.auto:auto
PC_КРД2.Kp:Kp
PC_КРД2.Ti:Ti
PC_КРД2.Kd:Kd
PC_КРД2.Td:Td
PC_КРД2.Tzd:Tzd
PC_КРД2.Tf:Tf
PC_КРД2.Zi:Zi
PC_КРД2.Hup:Hup
PC_КРД2.Hdwn:Hdwn','PC_КРД2.sp:sp
PC_КРД2.var:var
*s.ат:ed:Од.виміру
*i.1:prec:Точність
PC_КРД2.out:out
PC_КРД2.manIn:manIn
PC_КРД2.max:max
PC_КРД2.min:min
PC_КРД2.auto:auto
PC_КРД2.Kp:Kp
PC_КРД2.Ti:Ti
PC_КРД2.Kd:Kd
PC_КРД2.Td:Td
PC_КРД2.Tzd:Tzd
PC_КРД2.Tf:Tf
PC_КРД2.Zi:Zi
PC_КРД2.Hup:Hup
PC_КРД2.Hdwn:Hdwn');
INSERT INTO "AnastGener_cntr_prm" VALUES('PC_КРД3','PC_KRD3','Regulator of the pressure at the output of CS.',1,'PCKRD3.sp:sp
PCKRD3.var:var
*s.at:ed:Dimension
*i.1:prec:Precision
PCKRD3.out:out
PCKRD3.manIn:manIn
PCKRD3.max:max
PCKRD3.min:min
PCKRD3.auto:auto
PCKRD3.Kp:Kp
PCKRD3.Ti:Ti
PCKRD3.Kd:Kd
PCKRD3.Td:Td
PCKRD3.Tzd:Tzd
PCKRD3.Tf:Tf
PCKRD3.Zi:Zi
PCKRD3.Hup:Hup
PCKRD3.Hdwn:Hdwn','PC КРД3','Регулятор давления на выходе КС.','РС_КРД3','Регулятор тиску на виході КС.','PCKRD3.sp:sp
PCKRD3.var:var
*s.ат:ed:Ед. измерения
*i.1:prec:Точность
PCKRD3.out:out
PCKRD3.manIn:manIn
PCKRD3.max:max
PCKRD3.min:min
PCKRD3.auto:auto
PCKRD3.Kp:Kp
PCKRD3.Ti:Ti
PCKRD3.Kd:Kd
PCKRD3.Td:Td
PCKRD3.Tzd:Tzd
PCKRD3.Tf:Tf
PCKRD3.Zi:Zi
PCKRD3.Hup:Hup
PCKRD3.Hdwn:Hdwn','PCKRD3.sp:sp
PCKRD3.var:var
*s.ат:ed:Од.виміру
*i.1:prec:Точність
PCKRD3.out:out
PCKRD3.manIn:manIn
PCKRD3.max:max
PCKRD3.min:min
PCKRD3.auto:auto
PCKRD3.Kp:Kp
PCKRD3.Ti:Ti
PCKRD3.Kd:Kd
PCKRD3.Td:Td
PCKRD3.Tzd:Tzd
PCKRD3.Tf:Tf
PCKRD3.Zi:Zi
PCKRD3.Hup:Hup
PCKRD3.Hdwn:Hdwn');
INSERT INTO "AnastGener_cntr_prm" VALUES('SurgeKM101','SurgeKM101','Surge protection regulator for KM101',1,'SurgeKM101.sp:sp
SurgeKM101.var:var
*s.at:ed:Dimension
*i.2:prec:Precision
SurgeKM101.out:out
SurgeKM101.manIn:manIn
SurgeKM101.max:max
SurgeKM101.min:min
SurgeKM101.auto:auto
SurgeKM101.Kp:Kp
SurgeKM101.Ti:Ti
SurgeKM101.Kd:Kd
SurgeKM101.Td:Td
SurgeKM101.Tzd:Tzd
SurgeKM101.Tf:Tf
SurgeKM101.Zi:Zi
SurgeKM101.Hup:Hup
SurgeKM101.Hdwn:Hdwn','ПомпажKM101','Регулятор защиты от помпажа KM101','ПомпажKM101','Регулятор захисту від помпажа для KM101','SurgeKM101.sp:sp
SurgeKM101.var:var
*s.ат:ed:Единица
*i.2:prec:Точность
SurgeKM101.out:out
SurgeKM101.manIn:manIn
SurgeKM101.max:max
SurgeKM101.min:min
SurgeKM101.auto:auto
SurgeKM101.Kp:Kp
SurgeKM101.Ti:Ti
SurgeKM101.Kd:Kd
SurgeKM101.Td:Td
SurgeKM101.Tzd:Tzd
SurgeKM101.Tf:Tf
SurgeKM101.Zi:Zi
SurgeKM101.Hup:Hup
SurgeKM101.Hdwn:Hdwn','SurgeKM101.sp:sp
SurgeKM101.var:var
*s.ат:ed:Одиниця
*i.2:prec:Точність
SurgeKM101.out:out
SurgeKM101.manIn:manIn
SurgeKM101.max:max
SurgeKM101.min:min
SurgeKM101.auto:auto
SurgeKM101.Kp:Kp
SurgeKM101.Ti:Ti
SurgeKM101.Kd:Kd
SurgeKM101.Td:Td
SurgeKM101.Tzd:Tzd
SurgeKM101.Tf:Tf
SurgeKM101.Zi:Zi
SurgeKM101.Hup:Hup
SurgeKM101.Hdwn:Hdwn');
INSERT INTO "AnastGener_cntr_prm" VALUES('SurgeKM201','SurgeKM201','Surge protection regulator for KM201',1,'SurgeKM201.sp:sp
SurgeKM201.var:var
*s.at:ed:Dimension
*i.2:prec:Precision
SurgeKM201.out:out
SurgeKM201.manIn:manIn
SurgeKM201.max:max
SurgeKM201.min:min
SurgeKM201.auto:auto
SurgeKM201.Kp:Kp
SurgeKM201.Ti:Ti
SurgeKM201.Kd:Kd
SurgeKM201.Td:Td
SurgeKM201.Tzd:Tzd
SurgeKM201.Tf:Tf
SurgeKM201.Zi:Zi
SurgeKM201.Hup:Hup
SurgeKM201.Hdwn:Hdwn','ПомпажKM201','Регулятор защиты от помпажа KM201','ПомпажKM201','Регулятор захисту від помпажа для KM201','SurgeKM201.sp:sp
SurgeKM201.var:var
*s.ат:ed:Единица
*i.2:prec:Точность
SurgeKM201.out:out
SurgeKM201.manIn:manIn
SurgeKM201.max:max
SurgeKM201.min:min
SurgeKM201.auto:auto
SurgeKM201.Kp:Kp
SurgeKM201.Ti:Ti
SurgeKM201.Kd:Kd
SurgeKM201.Td:Td
SurgeKM201.Tzd:Tzd
SurgeKM201.Tf:Tf
SurgeKM201.Zi:Zi
SurgeKM201.Hup:Hup
SurgeKM201.Hdwn:Hdwn','SurgeKM201.sp:sp
SurgeKM201.var:var
*s.ат:ed:Одиниця
*i.2:prec:Точність
SurgeKM201.out:out
SurgeKM201.manIn:manIn
SurgeKM201.max:max
SurgeKM201.min:min
SurgeKM201.auto:auto
SurgeKM201.Kp:Kp
SurgeKM201.Ti:Ti
SurgeKM201.Kd:Kd
SurgeKM201.Td:Td
SurgeKM201.Tzd:Tzd
SurgeKM201.Tf:Tf
SurgeKM201.Zi:Zi
SurgeKM201.Hup:Hup
SurgeKM201.Hdwn:Hdwn');
INSERT INTO "AnastGener_cntr_prm" VALUES('SurgeKM301','SurgeKM301','Surge protection regulator for KM301',1,'SurgeKM301.sp:sp
SurgeKM301.var:var
*s.at:ed:Dimension
*i.2:prec:Precision
SurgeKM301.out:out
SurgeKM301.manIn:manIn
SurgeKM301.max:max
SurgeKM301.min:min
SurgeKM301.auto:auto
SurgeKM301.Kp:Kp
SurgeKM301.Ti:Ti
SurgeKM301.Kd:Kd
SurgeKM301.Td:Td
SurgeKM301.Tzd:Tzd
SurgeKM301.Tf:Tf
SurgeKM301.Zi:Zi
SurgeKM301.Hup:Hup
SurgeKM301.Hdwn:Hdwn','ПомпажKM301','Регулятор защиты от помпажа KM301','ПомпажKM301','Регулятор захисту від помпажа для KM301','SurgeKM301.sp:sp
SurgeKM301.var:var
*s.ат:ed:Единица
*i.2:prec:Точность
SurgeKM301.out:out
SurgeKM301.manIn:manIn
SurgeKM301.max:max
SurgeKM301.min:min
SurgeKM301.auto:auto
SurgeKM301.Kp:Kp
SurgeKM301.Ti:Ti
SurgeKM301.Kd:Kd
SurgeKM301.Td:Td
SurgeKM301.Tzd:Tzd
SurgeKM301.Tf:Tf
SurgeKM301.Zi:Zi
SurgeKM301.Hup:Hup
SurgeKM301.Hdwn:Hdwn','SurgeKM301.sp:sp
SurgeKM301.var:var
*s.ат:ed:Одиниця
*i.2:prec:Точність
SurgeKM301.out:out
SurgeKM301.manIn:manIn
SurgeKM301.max:max
SurgeKM301.min:min
SurgeKM301.auto:auto
SurgeKM301.Kp:Kp
SurgeKM301.Ti:Ti
SurgeKM301.Kd:Kd
SurgeKM301.Td:Td
SurgeKM301.Tzd:Tzd
SurgeKM301.Tf:Tf
SurgeKM301.Zi:Zi
SurgeKM301.Hup:Hup
SurgeKM301.Hdwn:Hdwn');
INSERT INTO "AnastGener_cntr_prm" VALUES('SurgeKM102','SurgeKM102','Surge protection regulator for KM102',1,'SurgeKM102.sp:sp
SurgeKM102.var:var
*s.at:ed:Dimension
*i.2:prec:Precision
SurgeKM102.out:out
SurgeKM102.manIn:manIn
SurgeKM102.max:max
SurgeKM102.min:min
SurgeKM102.auto:auto
SurgeKM102.Kp:Kp
SurgeKM102.Ti:Ti
SurgeKM102.Kd:Kd
SurgeKM102.Td:Td
SurgeKM102.Tzd:Tzd
SurgeKM102.Tf:Tf
SurgeKM102.Zi:Zi
SurgeKM102.Hup:Hup
SurgeKM102.Hdwn:Hdwn','ПомпажKM102','Регулятор защиты от помпажа KM102','ПомпажKM102','Регулятор захисту від помпажа для KM102','SurgeKM102.sp:sp
SurgeKM102.var:var
*s.ат:ed:Единица
*i.2:prec:Точность
SurgeKM102.out:out
SurgeKM102.manIn:manIn
SurgeKM102.max:max
SurgeKM102.min:min
SurgeKM102.auto:auto
SurgeKM102.Kp:Kp
SurgeKM102.Ti:Ti
SurgeKM102.Kd:Kd
SurgeKM102.Td:Td
SurgeKM102.Tzd:Tzd
SurgeKM102.Tf:Tf
SurgeKM102.Zi:Zi
SurgeKM102.Hup:Hup
SurgeKM102.Hdwn:Hdwn','SurgeKM102.sp:sp
SurgeKM102.var:var
*s.ат:ed:Одиниця
*i.2:prec:Точність
SurgeKM102.out:out
SurgeKM102.manIn:manIn
SurgeKM102.max:max
SurgeKM102.min:min
SurgeKM102.auto:auto
SurgeKM102.Kp:Kp
SurgeKM102.Ti:Ti
SurgeKM102.Kd:Kd
SurgeKM102.Td:Td
SurgeKM102.Tzd:Tzd
SurgeKM102.Tf:Tf
SurgeKM102.Zi:Zi
SurgeKM102.Hup:Hup
SurgeKM102.Hdwn:Hdwn');
INSERT INTO "AnastGener_cntr_prm" VALUES('SurgeKM202','SurgeKM202','Surge protection regulator for KM202',1,'SurgeKM202.sp:sp
SurgeKM202.var:var
*s.at:ed:Dimension
*i.2:prec:Precision
SurgeKM202.out:out
SurgeKM202.manIn:manIn
SurgeKM202.max:max
SurgeKM202.min:min
SurgeKM202.auto:auto
SurgeKM202.Kp:Kp
SurgeKM202.Ti:Ti
SurgeKM202.Kd:Kd
SurgeKM202.Td:Td
SurgeKM202.Tzd:Tzd
SurgeKM202.Tf:Tf
SurgeKM202.Zi:Zi
SurgeKM202.Hup:Hup
SurgeKM202.Hdwn:Hdwn','ПомпажKM202','Регулятор защиты от помпажа KM202','ПомпажKM202','Регулятор захисту від помпажа для KM202','SurgeKM202.sp:sp
SurgeKM202.var:var
*s.ат:ed:Единица
*i.2:prec:Точность
SurgeKM202.out:out
SurgeKM202.manIn:manIn
SurgeKM202.max:max
SurgeKM202.min:min
SurgeKM202.auto:auto
SurgeKM202.Kp:Kp
SurgeKM202.Ti:Ti
SurgeKM202.Kd:Kd
SurgeKM202.Td:Td
SurgeKM202.Tzd:Tzd
SurgeKM202.Tf:Tf
SurgeKM202.Zi:Zi
SurgeKM202.Hup:Hup
SurgeKM202.Hdwn:Hdwn','SurgeKM202.sp:sp
SurgeKM202.var:var
*s.ат:ed:Одиниця
*i.2:prec:Точність
SurgeKM202.out:out
SurgeKM202.manIn:manIn
SurgeKM202.max:max
SurgeKM202.min:min
SurgeKM202.auto:auto
SurgeKM202.Kp:Kp
SurgeKM202.Ti:Ti
SurgeKM202.Kd:Kd
SurgeKM202.Td:Td
SurgeKM202.Tzd:Tzd
SurgeKM202.Tf:Tf
SurgeKM202.Zi:Zi
SurgeKM202.Hup:Hup
SurgeKM202.Hdwn:Hdwn');
INSERT INTO "AnastGener_cntr_prm" VALUES('SurgeKM302','SurgeKM302','Surge protection regulator for KM302',1,'SurgeKM302.sp:sp
SurgeKM302.var:var
*s.at:ed:Dimension
*i.2:prec:Precision
SurgeKM302.out:out
SurgeKM302.manIn:manIn
SurgeKM302.max:max
SurgeKM302.min:min
SurgeKM302.auto:auto
SurgeKM302.Kp:Kp
SurgeKM302.Ti:Ti
SurgeKM302.Kd:Kd
SurgeKM302.Td:Td
SurgeKM302.Tzd:Tzd
SurgeKM302.Tf:Tf
SurgeKM302.Zi:Zi
SurgeKM302.Hup:Hup
SurgeKM302.Hdwn:Hdwn','ПомпажKM302','Регулятор защиты от помпажа KM302','ПомпажKM302','Регулятор захисту від помпажа для KM302','SurgeKM302.sp:sp
SurgeKM302.var:var
*s.ат:ed:Единица
*i.2:prec:Точность
SurgeKM302.out:out
SurgeKM302.manIn:manIn
SurgeKM302.max:max
SurgeKM302.min:min
SurgeKM302.auto:auto
SurgeKM302.Kp:Kp
SurgeKM302.Ti:Ti
SurgeKM302.Kd:Kd
SurgeKM302.Td:Td
SurgeKM302.Tzd:Tzd
SurgeKM302.Tf:Tf
SurgeKM302.Zi:Zi
SurgeKM302.Hup:Hup
SurgeKM302.Hdwn:Hdwn','SurgeKM302.sp:sp
SurgeKM302.var:var
*s.ат:ed:Одиниця
*i.2:prec:Точність
SurgeKM302.out:out
SurgeKM302.manIn:manIn
SurgeKM302.max:max
SurgeKM302.min:min
SurgeKM302.auto:auto
SurgeKM302.Kp:Kp
SurgeKM302.Ti:Ti
SurgeKM302.Kd:Kd
SurgeKM302.Td:Td
SurgeKM302.Tzd:Tzd
SurgeKM302.Tf:Tf
SurgeKM302.Zi:Zi
SurgeKM302.Hup:Hup
SurgeKM302.Hdwn:Hdwn');
CREATE TABLE 'AnastGener_prm' ("SHIFR" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '' ,"IO" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"ru#IO" TEXT DEFAULT '' ,"uk#IO" TEXT DEFAULT '' , PRIMARY KEY ("SHIFR"));
INSERT INTO "AnastGener_prm" VALUES('F2','F2','Gas flow through the diaphragm PP1',1,'PP1.Fi:var:Variable','','Витрати газу через діафрагму PP1','','Расход через диафрагму PP1','PP1.Fi:var:Переменная','PP1.Fi:var:Змінна');
INSERT INTO "AnastGener_prm" VALUES('F3','F3','The gas flow through a pipe to Glinsk',1,'pipeGlinsk.Fi:var:Variable','','Витрати газу через трубу на Глінск','','Расход газа через трубу на Глинск','pipeGlinsk.Fi:var:Переменная','pipeGlinsk.Fi:var:Змінна');
INSERT INTO "AnastGener_prm" VALUES('F4','F4','Gas flow through the diaphragm PP5',1,'PP5.Fi:var:Variable','','Витрати газу через діафрагму PP5','','Расход через диафрагму PP5','PP5.Fi:var:Переменная','PP5.Fi:var:Змінна');
INSERT INTO "AnastGener_prm" VALUES('КШ21','KSH21','The control block of the ball valve KSH21',1,'КШ21им.com:com
КШ21им.st_open:st_open
КШ21им.st_close:st_close','КШ21','Блок керування кульовим краном КШ21','КШ21','Блок управления шарового крана КШ21','','');
INSERT INTO "AnastGener_prm" VALUES('КШ22','KSH22','The control block of the ball valve KSH22',1,'КШ22им.com:com
КШ22им.st_open:st_open
КШ22им.st_close:st_close','КШ22','Блок керування кульовим краном КШ22','КШ22','Блок управления шарового крана КШ22','','');
INSERT INTO "AnastGener_prm" VALUES('PT0804','PT0804','The gas pressure in the pipe to Glinsk',1,'pipeGlinsk.Pi:var:Variable','','Тиск газу у трубі на Глінск','','Давление в трубе на Глинск','pipeGlinsk.Pi:var:Переменная','pipeGlinsk.Pi:var:Змінна');
INSERT INTO "AnastGener_prm" VALUES('F7_8','F7_8','Gas flow through the diaphragm PP7',1,'PP7.Fi:var:Variable','','Витрати газу через діафрагму PP7','','Расход через диафрагму PP7','PP7.Fi:var:Переменная','PP7.Fi:var:Змінна');
INSERT INTO "AnastGener_prm" VALUES('L1','L1','The liquid level in the separator C2',1,'C2.Lж:var:Variable','L1','Рівень рідини у сепараторі C2','L1','Уровень жидкости в сепараторе C2','C2.Lж:var:Переменная','C2.Lж:var:Змінна');
INSERT INTO "AnastGener_prm" VALUES('T_PP1','T_PP1','The gas temperature at the diaphragm PP1',1,'КРД1.To:var:Variable','','Температура газу на діафрагмі PP1','','Температура газа на диафрагме PP1','КРД1.To:var:Переменная','КРД1.To:var:Змінна');
INSERT INTO "AnastGener_prm" VALUES('T_PP3','T_PP3','The gas temperature at the diaphragm PP3',1,'node19.To:var:Variable','','Температура газу на діафрагмі PP3','','Температура газа на диафрагме PP3','node19.To:var:Переменная','node19.To:var:Змінна');
INSERT INTO "AnastGener_prm" VALUES('T_PP5','T_PP5','The gas temperature at the diaphragm PP5',1,'КШ21.To:var:Variable','','Температура газу на діафрагмі PP5','','Температура газа на диафрагме PP5','КШ21.To:var:Переменная','КШ21.To:var:Змінна');
INSERT INTO "AnastGener_prm" VALUES('P_PP1','P_PP1','Gas pressure at the diaphragm PP1',1,'PP1.Po:var:Variable','','Тиск газу на діафрагмі PP1','','Давление газа на диафрагме PP1','PP1.Po:var:Переменная','PP1.Po:var:Змінна');
INSERT INTO "AnastGener_prm" VALUES('P_PP3','P_PP3','Gas pressure at the diaphragm PP3',1,'PP3.Po:var:Variable','','Тиск газу на діафрагмі PP3','','Давление газа на диафрагме PP3','PP3.Po:var:Переменная','PP3.Po:var:Змінна');
INSERT INTO "AnastGener_prm" VALUES('P_PP5','P_PP5','Gas pressure at the diaphragm PP5',1,'PP5.Po:var:Variable','','Тиск газу на діафрагмі PP5','','Давление газа на диафрагме PP5','PP5.Po:var:Переменная','PP5.Po:var:Змінна');
INSERT INTO "AnastGener_prm" VALUES('P3','P3','The gas pressure in the pipe to Glinsk',1,'pipeGlinsk.Pi:var:Variable','','Тиск газу у трубі на Глінск','','Давление в трубе на Глинск','pipeGlinsk.Pi:var:Переменная','pipeGlinsk.Pi:var:Змінна');
INSERT INTO "AnastGener_prm" VALUES('P4','P4','Gas pressure at the diaphragm PP5',1,'PP5.Po:var:Variable','','Тиск газу на діафрагмі PP5','','Давление газа на диафрагме PP5','PP5.Po:var:Переменная','PP5.Po:var:Змінна');
INSERT INTO "AnastGener_prm" VALUES('PT1606','PT1606','The gas pressure in the separator С1',1,'C1.Po:var:Variable','','Тиск газу у сепараторі С1','','Давление газа в сепараторе С1','C1.Po:var:Переменная','C1.Po:var:Змінна');
INSERT INTO "AnastGener_prm" VALUES('Pi','Pi','The gas pressure at the input of the CS.',1,'КШ1.Pi:var:Variable','','Тиск газу на вході у КС.','','Давление газа на входе КС.','КШ1.Pi:var:Переменная','КШ1.Pi:var:Змінна');
INSERT INTO "AnastGener_prm" VALUES('PT0404','PT0404','Gas pressure at the diaphragm PP5',1,'PP5.Pi:var:Variable','','Тиск газу на діафрагмі PP5','','Давление газа на диафрагме PP5','PP5.Pi:var:Переменная','PP5.Pi:var:Змінна');
INSERT INTO "AnastGener_prm" VALUES('PT0406','PT0406','Gas pressure at the diaphragm PP7',1,'PP7.Po:var:Variable','','Тиск газу на діафрагмі PP7','','Давление газа на диафрагме PP7','PP7.Po:var:Переменная','PP7.Po:var:Змінна');
INSERT INTO "AnastGener_prm" VALUES('Ti','Ti','The gas temperature at the input of the CS.',1,'КШ1.Ti:var:Variable','','Температура газу на вході КС.','','Температура газа на входе КС.','КШ1.Ti:var:Переменная','КШ1.Ti:var:Змінна');
INSERT INTO "AnastGener_prm" VALUES('PT0503','PT0503','The gas pressure in the separator С1',1,'C1.Pi:var:Variable','','Тиск газу у сепараторі С1','','Давление газа в сепараторе С1','C1.Pi:var:Переменная','C1.Pi:var:Змінна');
INSERT INTO "AnastGener_prm" VALUES('КШ6','KSH6','The control block of the ball valve KSH6',1,'КШ6им.com:com
КШ6им.st_open:st_open
КШ6им.st_close:st_close','КШ6','Блок керування кульовим краном КШ6','КШ6','Блок управления шарового крана КШ6','','');
INSERT INTO "AnastGener_prm" VALUES('КШ7','KSH7','',1,'КШ7им.com:com
КШ7им.st_open:st_open
КШ7им.st_close:st_close','КШ7','Блок керування шарового крану КШ7','КШ7','Блок управления шарового крана КШ7','','');
INSERT INTO "AnastGener_prm" VALUES('PC0601','PC0601','Gas pressure at the diaphragm PP3',1,'PP3.Pi:var:Variable','','Тиск газу на діафрагмі PP3','','Давление газа на диафрагме PP3','PP3.Pi:var:Переменная','PP3.Pi:var:Змінна');
INSERT INTO "AnastGener_prm" VALUES('F_PP1','F_PP1','Gas flow through the diaphragm PP1',1,'PP1.Fo:var:Variable','','Витрати газу через діафрагму PP1','','Расход через диафрагму PP1','PP1.Fo:var:Переменная','PP1.Fo:var:Змінна');
INSERT INTO "AnastGener_prm" VALUES('F_PP3','F_PP3','Gas flow through the diaphragm PP3',1,'PP3.Fo:var:Variable','','Витрати газу через діафрагму PP3','','Расход газа через диафрагму PP3','PP3.Fo:var:Переменная','PP3.Fo:var:Змінна');
INSERT INTO "AnastGener_prm" VALUES('F_PP5','F_PP5','Gas flow through the diaphragm PP5',1,'PP5.Fo:var:Variable','','Витрати газу через діафрагму PP5','','Расход через диафрагму PP5','PP5.Fo:var:Переменная','PP5.Fo:var:Змінна');
INSERT INTO "AnastGener_prm" VALUES('F5_6','F5_6','Gas flow through the diaphragm PP5',1,'PP5.Fi:var:Variable','','Витрати газу через діафрагму PP5','','Расход через диафрагму PP5','PP5.Fi:var:Переменная','PP5.Fi:var:Змінна');
INSERT INTO "AnastGener_prm" VALUES('PT0605','PT0605','The gas pressure before control valve KRD1',1,'КРД1.Pi:var:Variable','','Тиск газу перед регулюючим клапаном КРД1','','Давление газа перед регулирующим клапаном КРД1','КРД1.Pi:var:Переменная','КРД1.Pi:var:Змінна');
INSERT INTO "AnastGener_prm" VALUES('КШ1','KSH1','The control block of the ball valve KSH1',1,'КШ1им.com:com
КШ1им.st_open:st_open
КШ1им.st_close:st_close','КШ1','Блок керування кульовим краном КШ1','КШ1','Блок управления шарового крана КШ1','','');
INSERT INTO "AnastGener_prm" VALUES('КШ2','KSH2','The control block of the ball valve KSH2',1,'КШ2им.com:com
КШ2им.st_open:st_open
КШ2им.st_close:st_close','КШ2','Блок керування кульовим краном КШ2','КШ2','Блок управления шарового крана КШ2','','');
INSERT INTO "AnastGener_prm" VALUES('КШ5','KSH5','The control block of the ball valve KSH5',1,'КШ5им.com:com
КШ5им.st_open:st_open
КШ5им.st_close:st_close','КШ5','Блок керування кульовим краном КШ5','КШ5','Блок управления шарового крана КШ5','','');
CREATE TABLE 'AnastKM101_blocks_io' ("BLK_ID" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"TLNK" INTEGER DEFAULT '' ,"LNK" TEXT DEFAULT '' ,"VAL" TEXT DEFAULT '' , PRIMARY KEY ("BLK_ID","ID"));
INSERT INTO "AnastKM101_blocks_io" VALUES('КМ101_1','Fi',0,'','50.4490301888019');
INSERT INTO "AnastKM101_blocks_io" VALUES('КМ101_1','Pi',1,'PP101.Po','5.39429843844697');
INSERT INTO "AnastKM101_blocks_io" VALUES('КМ101_1','Ti',0,'','273');
INSERT INTO "AnastKM101_blocks_io" VALUES('КМ101_1','Fo',1,'AT101_1.Fi','50.4444784403846');
INSERT INTO "AnastKM101_blocks_io" VALUES('КМ101_1','Po',0,'','10.2644669604643');
INSERT INTO "AnastKM101_blocks_io" VALUES('КМ101_1','To',0,'','331.114249585267');
INSERT INTO "AnastKM101_blocks_io" VALUES('КМ101_1','So',0,'','0.071');
INSERT INTO "AnastKM101_blocks_io" VALUES('КМ101_1','lo',0,'','55');
INSERT INTO "AnastKM101_blocks_io" VALUES('КМ101_1','Kzp',0,'','0.682665519464263');
INSERT INTO "AnastKM101_blocks_io" VALUES('КМ101_1','N',0,'','8.2');
INSERT INTO "AnastKM101_blocks_io" VALUES('КМ101_1','V',0,'','1.5');
INSERT INTO "AnastKM101_blocks_io" VALUES('КМ101_1','Kpmp',0,'','0.025');
INSERT INTO "AnastKM101_blocks_io" VALUES('КМ101_1','Kslp',0,'','0.2');
INSERT INTO "AnastKM101_blocks_io" VALUES('КМ101_1','Q0',0,'','1');
INSERT INTO "AnastKM101_blocks_io" VALUES('КМ101_1','Ct',0,'','100');
INSERT INTO "AnastKM101_blocks_io" VALUES('КМ101_1','Riz',0,'','100');
INSERT INTO "AnastKM101_blocks_io" VALUES('КМ101_1','Fwind',0,'','1');
INSERT INTO "AnastKM101_blocks_io" VALUES('КМ101_1','Twind',0,'','273');
INSERT INTO "AnastKM101_blocks_io" VALUES('КМ101_1','f_frq',0,'','100');
INSERT INTO "AnastKM101_blocks_io" VALUES('КМ101_1','Pot',0,'','10.8436371312579');
INSERT INTO "AnastKM101_blocks_io" VALUES('КМ101_1','Fit',0,'','50.447860218721');
INSERT INTO "AnastKM101_blocks_io" VALUES('КМ101_2','Fi',0,'','49.9353278510799');
INSERT INTO "AnastKM101_blocks_io" VALUES('КМ101_2','Pi',1,'PP102.Po','9.8402856120246');
INSERT INTO "AnastKM101_blocks_io" VALUES('КМ101_2','Ti',1,'AT101_1.To','298.280785886178');
INSERT INTO "AnastKM101_blocks_io" VALUES('КМ101_2','Fo',1,'AT101_2.Fi','49.929968769895');
INSERT INTO "AnastKM101_blocks_io" VALUES('КМ101_2','Po',0,'','25.8745845594574');
INSERT INTO "AnastKM101_blocks_io" VALUES('КМ101_2','To',0,'','398.603354924666');
INSERT INTO "AnastKM101_blocks_io" VALUES('КМ101_2','So',0,'','0.0314');
INSERT INTO "AnastKM101_blocks_io" VALUES('КМ101_2','lo',0,'','55');
INSERT INTO "AnastKM101_blocks_io" VALUES('КМ101_2','Kzp',0,'','0.88392088903263');
INSERT INTO "AnastKM101_blocks_io" VALUES('КМ101_2','N',1,'КМ101_1.N','8.2');
INSERT INTO "AnastKM101_blocks_io" VALUES('КМ101_2','V',0,'','1');
INSERT INTO "AnastKM101_blocks_io" VALUES('КМ101_2','Kpmp',0,'','0.035');
INSERT INTO "AnastKM101_blocks_io" VALUES('КМ101_2','Kslp',0,'','0.2');
INSERT INTO "AnastKM101_blocks_io" VALUES('КМ101_2','Q0',0,'','1');
INSERT INTO "AnastKM101_blocks_io" VALUES('КМ101_2','Ct',0,'','100');
INSERT INTO "AnastKM101_blocks_io" VALUES('КМ101_2','Riz',0,'','100');
INSERT INTO "AnastKM101_blocks_io" VALUES('КМ101_2','Fwind',0,'','1');
INSERT INTO "AnastKM101_blocks_io" VALUES('КМ101_2','Twind',0,'','273');
INSERT INTO "AnastKM101_blocks_io" VALUES('КМ101_2','f_frq',0,'','100');
INSERT INTO "AnastKM101_blocks_io" VALUES('КМ101_2','Pot',0,'','27.7278237505709');
INSERT INTO "AnastKM101_blocks_io" VALUES('КМ101_2','Fit',0,'','49.9340881580197');
INSERT INTO "AnastKM101_blocks_io" VALUES('KPP101','Fi',0,'','1.61247077806096e-251');
INSERT INTO "AnastKM101_blocks_io" VALUES('KPP101','Pi',1,'node4_1.Po2','25.353364907383');
INSERT INTO "AnastKM101_blocks_io" VALUES('KPP101','Ti',1,'AT101_2.To','306.09122250061');
INSERT INTO "AnastKM101_blocks_io" VALUES('KPP101','Fo',1,'node3_1.Fi2','-0.000406670722891623');
INSERT INTO "AnastKM101_blocks_io" VALUES('KPP101','Po',0,'','5.48253890873659');
INSERT INTO "AnastKM101_blocks_io" VALUES('KPP101','To',0,'','273.357005787614');
INSERT INTO "AnastKM101_blocks_io" VALUES('KPP101','So',0,'','0.071');
INSERT INTO "AnastKM101_blocks_io" VALUES('KPP101','lo',0,'','10');
INSERT INTO "AnastKM101_blocks_io" VALUES('KPP101','S_kl1',0,'','0.04');
INSERT INTO "AnastKM101_blocks_io" VALUES('KPP101','S_kl2',0,'','0.001');
INSERT INTO "AnastKM101_blocks_io" VALUES('KPP101','l_kl1',2,'Anast1to2node_cntr.SurgeKM101.out','0');
INSERT INTO "AnastKM101_blocks_io" VALUES('KPP101','l_kl2',0,'','0');
INSERT INTO "AnastKM101_blocks_io" VALUES('KPP101','Q0',0,'','1');
INSERT INTO "AnastKM101_blocks_io" VALUES('KPP101','Kln',0,'','2');
INSERT INTO "AnastKM101_blocks_io" VALUES('KPP101','Kpr',0,'','0.95');
INSERT INTO "AnastKM101_blocks_io" VALUES('KPP101','Ct',0,'','20');
INSERT INTO "AnastKM101_blocks_io" VALUES('KPP101','Riz',0,'','20');
INSERT INTO "AnastKM101_blocks_io" VALUES('KPP101','noBack',0,'','0');
INSERT INTO "AnastKM101_blocks_io" VALUES('KPP101','Fwind',0,'','1');
INSERT INTO "AnastKM101_blocks_io" VALUES('KPP101','Twind',0,'','273');
INSERT INTO "AnastKM101_blocks_io" VALUES('KPP101','f_frq',0,'','100');
INSERT INTO "AnastKM101_blocks_io" VALUES('KPP101','Pot',0,'','5.21403969646924');
INSERT INTO "AnastKM101_blocks_io" VALUES('KPP101','Fit',0,'','2.47032822920623e-323');
INSERT INTO "AnastKM101_blocks_io" VALUES('DR100','Fi',0,'','7.1159156774132e-05');
INSERT INTO "AnastKM101_blocks_io" VALUES('DR100','Pi',1,'КШ106.Po','5.48252934658735');
INSERT INTO "AnastKM101_blocks_io" VALUES('DR100','Ti',1,'КШ106.To','273');
INSERT INTO "AnastKM101_blocks_io" VALUES('DR100','Fo',1,'node3_1.Fi1','-0.00169341662648114');
INSERT INTO "AnastKM101_blocks_io" VALUES('DR100','Po',0,'','5.48253430667982');
INSERT INTO "AnastKM101_blocks_io" VALUES('DR100','To',0,'','272.999999970236');
INSERT INTO "AnastKM101_blocks_io" VALUES('DR100','So',0,'','0.2');
INSERT INTO "AnastKM101_blocks_io" VALUES('DR100','lo',0,'','10');
INSERT INTO "AnastKM101_blocks_io" VALUES('DR100','S_kl1',0,'','0.05');
INSERT INTO "AnastKM101_blocks_io" VALUES('DR100','S_kl2',0,'','0.01');
INSERT INTO "AnastKM101_blocks_io" VALUES('DR100','l_kl1',0,'','50');
INSERT INTO "AnastKM101_blocks_io" VALUES('DR100','l_kl2',0,'','0');
INSERT INTO "AnastKM101_blocks_io" VALUES('DR100','Q0',0,'','1');
INSERT INTO "AnastKM101_blocks_io" VALUES('DR100','Kln',0,'','1');
INSERT INTO "AnastKM101_blocks_io" VALUES('DR100','Kpr',0,'','0.95');
INSERT INTO "AnastKM101_blocks_io" VALUES('DR100','Ct',0,'','20');
INSERT INTO "AnastKM101_blocks_io" VALUES('DR100','Riz',0,'','20');
INSERT INTO "AnastKM101_blocks_io" VALUES('DR100','noBack',0,'','0');
INSERT INTO "AnastKM101_blocks_io" VALUES('DR100','Fwind',0,'','1');
INSERT INTO "AnastKM101_blocks_io" VALUES('DR100','Twind',0,'','273');
INSERT INTO "AnastKM101_blocks_io" VALUES('DR100','f_frq',0,'','100');
INSERT INTO "AnastKM101_blocks_io" VALUES('DR100','Pot',0,'','5.21393301112062');
INSERT INTO "AnastKM101_blocks_io" VALUES('DR100','Fit',0,'','0.0442671479945538');
INSERT INTO "AnastKM101_blocks_io" VALUES('AT101_1','Fi',0,'','50.4442570459255');
INSERT INTO "AnastKM101_blocks_io" VALUES('AT101_1','Pi',1,'КМ101_1.Po','10.2644172916099');
INSERT INTO "AnastKM101_blocks_io" VALUES('AT101_1','Ti',1,'КМ101_1.To','331.114301256085');
INSERT INTO "AnastKM101_blocks_io" VALUES('AT101_1','Si',0,'','0.071');
INSERT INTO "AnastKM101_blocks_io" VALUES('AT101_1','li',0,'','10');
INSERT INTO "AnastKM101_blocks_io" VALUES('AT101_1','Fo',1,'C101_2.Fi','50.4440936744792');
INSERT INTO "AnastKM101_blocks_io" VALUES('AT101_1','Po',0,'','10.1344239445453');
INSERT INTO "AnastKM101_blocks_io" VALUES('AT101_1','To',0,'','298.280766032448');
INSERT INTO "AnastKM101_blocks_io" VALUES('AT101_1','So',0,'','0.071');
INSERT INTO "AnastKM101_blocks_io" VALUES('AT101_1','lo',0,'','2');
INSERT INTO "AnastKM101_blocks_io" VALUES('AT101_1','Tair',0,'','290');
INSERT INTO "AnastKM101_blocks_io" VALUES('AT101_1','Wc',0,'','200');
INSERT INTO "AnastKM101_blocks_io" VALUES('AT101_1','Q0',0,'','1');
INSERT INTO "AnastKM101_blocks_io" VALUES('AT101_1','Ct',0,'','100');
INSERT INTO "AnastKM101_blocks_io" VALUES('AT101_1','Rt',0,'','1');
INSERT INTO "AnastKM101_blocks_io" VALUES('AT101_1','f_frq',0,'','100');
INSERT INTO "AnastKM101_blocks_io" VALUES('AT101_1','Pot',0,'','10.5636608773328');
INSERT INTO "AnastKM101_blocks_io" VALUES('AT101_1','Fit',0,'','37.839259204133');
INSERT INTO "AnastKM101_blocks_io" VALUES('AT101_2','Fi',0,'','49.9298539359569');
INSERT INTO "AnastKM101_blocks_io" VALUES('AT101_2','Pi',1,'КМ101_2.Po','25.8744539511343');
INSERT INTO "AnastKM101_blocks_io" VALUES('AT101_2','Ti',1,'КМ101_2.To','398.603383940072');
INSERT INTO "AnastKM101_blocks_io" VALUES('AT101_2','Si',0,'','0.0314');
INSERT INTO "AnastKM101_blocks_io" VALUES('AT101_2','li',0,'','20');
INSERT INTO "AnastKM101_blocks_io" VALUES('AT101_2','Fo',1,'C101_3.Fi','49.9293362799985');
INSERT INTO "AnastKM101_blocks_io" VALUES('AT101_2','Po',0,'','25.6153521756976');
INSERT INTO "AnastKM101_blocks_io" VALUES('AT101_2','To',0,'','306.091208861316');
INSERT INTO "AnastKM101_blocks_io" VALUES('AT101_2','So',0,'','0.0314');
INSERT INTO "AnastKM101_blocks_io" VALUES('AT101_2','lo',0,'','5');
INSERT INTO "AnastKM101_blocks_io" VALUES('AT101_2','Tair',0,'','283');
INSERT INTO "AnastKM101_blocks_io" VALUES('AT101_2','Wc',0,'','200');
INSERT INTO "AnastKM101_blocks_io" VALUES('AT101_2','Q0',0,'','1');
INSERT INTO "AnastKM101_blocks_io" VALUES('AT101_2','Ct',0,'','100');
INSERT INTO "AnastKM101_blocks_io" VALUES('AT101_2','Rt',0,'','1');
INSERT INTO "AnastKM101_blocks_io" VALUES('AT101_2','f_frq',0,'','100');
INSERT INTO "AnastKM101_blocks_io" VALUES('AT101_2','Pot',0,'','27.0712126806336');
INSERT INTO "AnastKM101_blocks_io" VALUES('AT101_2','Fit',0,'','37.4306674619275');
INSERT INTO "AnastKM101_blocks_io" VALUES('node3_1','Fi1',0,'','-0.00182427383555114');
INSERT INTO "AnastKM101_blocks_io" VALUES('node3_1','Pi1',1,'DR100.Po','5.48253913955186');
INSERT INTO "AnastKM101_blocks_io" VALUES('node3_1','Ti1',1,'DR100.To','272.999999970236');
INSERT INTO "AnastKM101_blocks_io" VALUES('node3_1','Si1',0,'','0.1');
INSERT INTO "AnastKM101_blocks_io" VALUES('node3_1','Fi2',0,'','-0.000427817170119968');
INSERT INTO "AnastKM101_blocks_io" VALUES('node3_1','Pi2',1,'KPP101.Po','5.482540578875');
INSERT INTO "AnastKM101_blocks_io" VALUES('node3_1','Ti2',1,'KPP101.To','273.357004745952');
INSERT INTO "AnastKM101_blocks_io" VALUES('node3_1','Si2',0,'','0.07');
INSERT INTO "AnastKM101_blocks_io" VALUES('node3_1','Fi3',0,'','50.9724259310207');
INSERT INTO "AnastKM101_blocks_io" VALUES('node3_1','Pi3',1,'КШ101.Po','5.51612552721674');
INSERT INTO "AnastKM101_blocks_io" VALUES('node3_1','Ti3',1,'КШ101.To','290.421289125811');
INSERT INTO "AnastKM101_blocks_io" VALUES('node3_1','Si3',0,'','0.2');
INSERT INTO "AnastKM101_blocks_io" VALUES('node3_1','Fo',1,'C101_1.Fi','50.9643286213166');
INSERT INTO "AnastKM101_blocks_io" VALUES('node3_1','Po',0,'','5.48254417576119');
INSERT INTO "AnastKM101_blocks_io" VALUES('node3_1','To',0,'','290.384502177235');
INSERT INTO "AnastKM101_blocks_io" VALUES('node3_1','So',0,'','0.2');
INSERT INTO "AnastKM101_blocks_io" VALUES('node3_1','lo',0,'','50');
INSERT INTO "AnastKM101_blocks_io" VALUES('node3_1','Q0',0,'','1');
INSERT INTO "AnastKM101_blocks_io" VALUES('node3_1','Ct',0,'','20');
INSERT INTO "AnastKM101_blocks_io" VALUES('node3_1','Riz',0,'','20');
INSERT INTO "AnastKM101_blocks_io" VALUES('node3_1','Fwind',0,'','1');
INSERT INTO "AnastKM101_blocks_io" VALUES('node3_1','Twind',0,'','273');
INSERT INTO "AnastKM101_blocks_io" VALUES('node3_1','f_frq',0,'','100');
INSERT INTO "AnastKM101_blocks_io" VALUES('node3_1','FoR',0,'','44.7977730124046');
INSERT INTO "AnastKM101_blocks_io" VALUES('node3_1','Pot',0,'','5.21387517550895');
INSERT INTO "AnastKM101_blocks_io" VALUES('node3_1','Fit',0,'','38.1026117702689');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ101','Fi',0,'','50.9732434210428');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ101','Pi',0,'','5.54716442669033');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ101','Ti',2,'Anast1to2node.КРД1.To','290.489460179923');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ101','Fo',1,'node3_1.Fi3','50.9726468596836');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ101','Po',0,'','5.51614291412778');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ101','To',0,'','290.421354372243');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ101','So',0,'','0.2');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ101','lo',0,'','5');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ101','S_kl1',0,'','0.2');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ101','S_kl2',0,'','0.05');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ101','l_kl1',1,'КШ101им.pos','100');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ101','l_kl2',1,'КШ102им.pos','0');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ101','Q0',0,'','1');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ101','Kln',0,'','1');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ101','Kpr',0,'','0.95');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ101','Ct',0,'','20');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ101','Riz',0,'','20');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ101','noBack',0,'','0');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ101','Fwind',0,'','1');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ101','Twind',0,'','273');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ101','f_frq',0,'','100');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ101','Pot',0,'','5.27686195592518');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ101','Fit',0,'','38.1825907349745');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ104','Fi',0,'','49.4280893032258');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ104','Pi',1,'node4_1.Po3','25.0943478414969');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ104','Ti',1,'AT101_2.To','306.091281416195');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ104','Fo',0,'','49.4191067847051');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ104','Po',0,'','24.7973200948992');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ104','To',0,'','305.949684273818');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ104','So',0,'','0.03');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ104','lo',0,'','100');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ104','S_kl1',0,'','0.03');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ104','S_kl2',0,'','0.01');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ104','l_kl1',1,'КШ104им.pos','100');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ104','l_kl2',1,'КШ105им.pos','0');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ104','Q0',0,'','1');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ104','Kln',0,'','1');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ104','Kpr',0,'','0.95');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ104','Ct',0,'','20');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ104','Riz',0,'','20');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ104','noBack',0,'','1');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ104','Fwind',0,'','1');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ104','Twind',0,'','273');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ104','f_frq',0,'','100');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ104','Pot',0,'','25.5440963848017');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ104','Fit',0,'','37.0566743886216');
INSERT INTO "AnastKM101_blocks_io" VALUES('node4_1','Fi',0,'','49.4300036874803');
INSERT INTO "AnastKM101_blocks_io" VALUES('node4_1','Pi',1,'C101_3.Po','25.3534027981398');
INSERT INTO "AnastKM101_blocks_io" VALUES('node4_1','Fo1',1,'КШ106.Fi','0');
INSERT INTO "AnastKM101_blocks_io" VALUES('node4_1','Po1',0,'','25.3533756159971');
INSERT INTO "AnastKM101_blocks_io" VALUES('node4_1','So1',0,'','0.0314');
INSERT INTO "AnastKM101_blocks_io" VALUES('node4_1','lo1',0,'','5');
INSERT INTO "AnastKM101_blocks_io" VALUES('node4_1','Fo2',1,'KPP101.Fi','1.59801645476185e-251');
INSERT INTO "AnastKM101_blocks_io" VALUES('node4_1','Po2',0,'','25.3534782662938');
INSERT INTO "AnastKM101_blocks_io" VALUES('node4_1','So2',0,'','0.0314');
INSERT INTO "AnastKM101_blocks_io" VALUES('node4_1','lo2',0,'','5');
INSERT INTO "AnastKM101_blocks_io" VALUES('node4_1','Fo3',1,'КШ104.Fi','49.4280292902411');
INSERT INTO "AnastKM101_blocks_io" VALUES('node4_1','Po3',0,'','25.0943132158322');
INSERT INTO "AnastKM101_blocks_io" VALUES('node4_1','So3',0,'','0.0314');
INSERT INTO "AnastKM101_blocks_io" VALUES('node4_1','lo3',0,'','5');
INSERT INTO "AnastKM101_blocks_io" VALUES('node4_1','Q0',0,'','1');
INSERT INTO "AnastKM101_blocks_io" VALUES('node4_1','f_frq',0,'','100');
INSERT INTO "AnastKM101_blocks_io" VALUES('node4_1','F1tmp',0,'','-0.000313155881982887');
INSERT INTO "AnastKM101_blocks_io" VALUES('node4_1','F2tmp',0,'','0.000161854653965907');
INSERT INTO "AnastKM101_blocks_io" VALUES('node4_1','F3tmp',0,'','49.4283667905487');
INSERT INTO "AnastKM101_blocks_io" VALUES('node4_1','Pot1',0,'','26.2854977694532');
INSERT INTO "AnastKM101_blocks_io" VALUES('node4_1','Pot2',0,'','26.6645745052463');
INSERT INTO "AnastKM101_blocks_io" VALUES('node4_1','Pot3',0,'','26.1026891687553');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ106','Fi',0,'','0');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ106','Pi',1,'node4_1.Po1','25.3535208244665');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ106','Ti',1,'AT101_2.To','306.091285938535');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ106','Fo',1,'DR100.Fi','7.99119167796749e-05');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ106','Po',0,'','5.48256932975069');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ106','To',0,'','273');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ106','So',0,'','0.0314');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ106','lo',0,'','10');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ106','S_kl1',0,'','0.0314');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ106','S_kl2',0,'','0.01');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ106','l_kl1',1,'КШ106им.pos','0');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ106','l_kl2',0,'','0');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ106','Q0',0,'','1');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ106','Kln',0,'','1');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ106','Kpr',0,'','0.95');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ106','Ct',0,'','20');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ106','Riz',0,'','20');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ106','noBack',0,'','0');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ106','Fwind',0,'','1');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ106','Twind',0,'','273');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ106','f_frq',0,'','100');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ106','Pot',0,'','5.21397436923412');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ106','Fit',0,'','0');
INSERT INTO "AnastKM101_blocks_io" VALUES('C101_1','Fi',0,'','50.9642363837389');
INSERT INTO "AnastKM101_blocks_io" VALUES('C101_1','Pi',1,'node3_1.Po','5.48253408335786');
INSERT INTO "AnastKM101_blocks_io" VALUES('C101_1','Si',0,'','0.2');
INSERT INTO "AnastKM101_blocks_io" VALUES('C101_1','Fo',1,'PP101.Fi','50.4496451194198');
INSERT INTO "AnastKM101_blocks_io" VALUES('C101_1','Po',0,'','5.44928838849442');
INSERT INTO "AnastKM101_blocks_io" VALUES('C101_1','So',0,'','0.2');
INSERT INTO "AnastKM101_blocks_io" VALUES('C101_1','lo',0,'','40');
INSERT INTO "AnastKM101_blocks_io" VALUES('C101_1','Fo_ж',0,'','0');
INSERT INTO "AnastKM101_blocks_io" VALUES('C101_1','Po_ж',0,'','5.50960973129428');
INSERT INTO "AnastKM101_blocks_io" VALUES('C101_1','Lж',0,'','6.34942984965424');
INSERT INTO "AnastKM101_blocks_io" VALUES('C101_1','ProcЖ',0,'','0.01');
INSERT INTO "AnastKM101_blocks_io" VALUES('C101_1','Vap',0,'','9.5');
INSERT INTO "AnastKM101_blocks_io" VALUES('C101_1','Q0',0,'','1');
INSERT INTO "AnastKM101_blocks_io" VALUES('C101_1','Qж',0,'','1000');
INSERT INTO "AnastKM101_blocks_io" VALUES('C101_1','f_frq',0,'','100');
INSERT INTO "AnastKM101_blocks_io" VALUES('C101_1','Pot',0,'','5.14948874023436');
INSERT INTO "AnastKM101_blocks_io" VALUES('C101_1','Fit',0,'','37.7880580203515');
INSERT INTO "AnastKM101_blocks_io" VALUES('C101_2','Fi',0,'','50.4441410430429');
INSERT INTO "AnastKM101_blocks_io" VALUES('C101_2','Pi',1,'AT101_1.Po','10.1344304847633');
INSERT INTO "AnastKM101_blocks_io" VALUES('C101_2','Si',0,'','0.071');
INSERT INTO "AnastKM101_blocks_io" VALUES('C101_2','Fo',1,'PP102.Fi','49.9356683817757');
INSERT INTO "AnastKM101_blocks_io" VALUES('C101_2','Po',0,'','9.99692518780511');
INSERT INTO "AnastKM101_blocks_io" VALUES('C101_2','So',0,'','0.071');
INSERT INTO "AnastKM101_blocks_io" VALUES('C101_2','lo',0,'','50');
INSERT INTO "AnastKM101_blocks_io" VALUES('C101_2','Fo_ж',0,'','0');
INSERT INTO "AnastKM101_blocks_io" VALUES('C101_2','Po_ж',0,'','10.0540703708102');
INSERT INTO "AnastKM101_blocks_io" VALUES('C101_2','Lж',0,'','6.01528242159123');
INSERT INTO "AnastKM101_blocks_io" VALUES('C101_2','ProcЖ',0,'','0.01');
INSERT INTO "AnastKM101_blocks_io" VALUES('C101_2','Vap',0,'','9.5');
INSERT INTO "AnastKM101_blocks_io" VALUES('C101_2','Q0',0,'','1');
INSERT INTO "AnastKM101_blocks_io" VALUES('C101_2','Qж',0,'','1000');
INSERT INTO "AnastKM101_blocks_io" VALUES('C101_2','f_frq',0,'','100');
INSERT INTO "AnastKM101_blocks_io" VALUES('C101_2','Pot',0,'','10.3103302826744');
INSERT INTO "AnastKM101_blocks_io" VALUES('C101_2','Fit',0,'','37.4280342569487');
INSERT INTO "AnastKM101_blocks_io" VALUES('C101_3','Fi',0,'','49.9294134292814');
INSERT INTO "AnastKM101_blocks_io" VALUES('C101_3','Pi',1,'AT101_2.Po','25.6153599434278');
INSERT INTO "AnastKM101_blocks_io" VALUES('C101_3','Si',0,'','0.0314');
INSERT INTO "AnastKM101_blocks_io" VALUES('C101_3','Fo',1,'node4_1.Fi','49.4284885540643');
INSERT INTO "AnastKM101_blocks_io" VALUES('C101_3','Po',0,'','25.3533702782337');
INSERT INTO "AnastKM101_blocks_io" VALUES('C101_3','So',0,'','0.0314');
INSERT INTO "AnastKM101_blocks_io" VALUES('C101_3','lo',0,'','8');
INSERT INTO "AnastKM101_blocks_io" VALUES('C101_3','Fo_ж',0,'','0');
INSERT INTO "AnastKM101_blocks_io" VALUES('C101_3','Po_ж',0,'','25.4087675477289');
INSERT INTO "AnastKM101_blocks_io" VALUES('C101_3','Lж',0,'','18.4675016070352');
INSERT INTO "AnastKM101_blocks_io" VALUES('C101_3','ProcЖ',0,'','0.01');
INSERT INTO "AnastKM101_blocks_io" VALUES('C101_3','Vap',0,'','3');
INSERT INTO "AnastKM101_blocks_io" VALUES('C101_3','Q0',0,'','1');
INSERT INTO "AnastKM101_blocks_io" VALUES('C101_3','Qж',0,'','1000');
INSERT INTO "AnastKM101_blocks_io" VALUES('C101_3','f_frq',0,'','100');
INSERT INTO "AnastKM101_blocks_io" VALUES('C101_3','Pot',0,'','26.5861090936371');
INSERT INTO "AnastKM101_blocks_io" VALUES('C101_3','Fit',0,'','37.0601681024761');
INSERT INTO "AnastKM101_blocks_io" VALUES('PP101','Fi',0,'','50.4497128990008');
INSERT INTO "AnastKM101_blocks_io" VALUES('PP101','Pi',1,'C101_1.Po','5.44929366825537');
INSERT INTO "AnastKM101_blocks_io" VALUES('PP101','Fo',1,'КМ101_1.Fi','50.4488688390024');
INSERT INTO "AnastKM101_blocks_io" VALUES('PP101','Po',0,'','5.39428273414133');
INSERT INTO "AnastKM101_blocks_io" VALUES('PP101','dP',0,'','5.50110296117261');
INSERT INTO "AnastKM101_blocks_io" VALUES('PP101','Sdf',0,'','0.15');
INSERT INTO "AnastKM101_blocks_io" VALUES('PP101','So',0,'','0.2');
INSERT INTO "AnastKM101_blocks_io" VALUES('PP101','lo',0,'','7');
INSERT INTO "AnastKM101_blocks_io" VALUES('PP101','Q0',0,'','1');
INSERT INTO "AnastKM101_blocks_io" VALUES('PP101','f_frq',0,'','100');
INSERT INTO "AnastKM101_blocks_io" VALUES('PP101','Pot',0,'','5.03525664785889');
INSERT INTO "AnastKM101_blocks_io" VALUES('PP101','Fit',0,'','37.8041274113249');
INSERT INTO "AnastKM101_blocks_io" VALUES('PP102','Fi',0,'','49.935725382797');
INSERT INTO "AnastKM101_blocks_io" VALUES('PP102','Pi',1,'C101_2.Po','9.99693487729569');
INSERT INTO "AnastKM101_blocks_io" VALUES('PP102','Fo',1,'КМ101_2.Fi','49.9351683596735');
INSERT INTO "AnastKM101_blocks_io" VALUES('PP102','Po',0,'','9.84025368089022');
INSERT INTO "AnastKM101_blocks_io" VALUES('PP102','dP',0,'','15.6677007909068');
INSERT INTO "AnastKM101_blocks_io" VALUES('PP102','Sdf',0,'','0.065');
INSERT INTO "AnastKM101_blocks_io" VALUES('PP102','So',0,'','0.071');
INSERT INTO "AnastKM101_blocks_io" VALUES('PP102','lo',0,'','7');
INSERT INTO "AnastKM101_blocks_io" VALUES('PP102','Q0',0,'','1');
INSERT INTO "AnastKM101_blocks_io" VALUES('PP102','f_frq',0,'','100');
INSERT INTO "AnastKM101_blocks_io" VALUES('PP102','Pot',0,'','10.0115141796379');
INSERT INTO "AnastKM101_blocks_io" VALUES('PP102','Fit',0,'','37.429637618184');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ102им','pos',0,'','0');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ104им','com',0,'','1');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ104им','pos',0,'','100');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ102им','com',0,'','0');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ102им','st_open',0,'','0');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ102им','st_close',0,'','1');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ102им','t_full',0,'','3');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ102им','t_up',0,'','0.3');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ102им','f_frq',0,'','100');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ102им','tmp_up',0,'','0');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ102им','lst_com',0,'','0');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ101им','pos',0,'','100');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ101им','com',0,'','1');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ101им','st_open',0,'','1');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ101им','st_close',0,'','0');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ101им','t_full',0,'','5');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ101им','t_up',0,'','0.5');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ101им','f_frq',0,'','100');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ101им','tmp_up',0,'','0');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ101им','lst_com',0,'','1');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ104им','st_open',0,'','1');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ104им','st_close',0,'','0');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ104им','t_full',0,'','5');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ104им','t_up',0,'','0.5');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ104им','f_frq',0,'','100');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ104им','tmp_up',0,'','0');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ104им','lst_com',0,'','1');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ105им','pos',0,'','0');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ105им','com',0,'','0');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ105им','st_open',0,'','0');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ105им','st_close',0,'','1');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ105им','t_full',0,'','5');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ105им','t_up',0,'','0.5');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ105им','f_frq',0,'','100');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ105им','tmp_up',0,'','0');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ105им','lst_com',0,'','0');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ106им','pos',0,'','0');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ106им','com',0,'','0');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ106им','st_open',0,'','0');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ106им','st_close',0,'','1');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ106им','t_full',0,'','5');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ106им','t_up',0,'','0.5');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ106им','f_frq',0,'','100');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ106им','tmp_up',0,'','0');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ106им','lst_com',0,'','0');
INSERT INTO "AnastKM101_blocks_io" VALUES('КМ101_1','Kpr',0,'','0.95');
INSERT INTO "AnastKM101_blocks_io" VALUES('КМ101_2','Kpr',0,'','0.95');
INSERT INTO "AnastKM101_blocks_io" VALUES('KPP101','t_kl1',0,'','3');
INSERT INTO "AnastKM101_blocks_io" VALUES('KPP101','t_kl2',0,'','5');
INSERT INTO "AnastKM101_blocks_io" VALUES('KPP101','tmp_l1',0,'','7.4109846876187e-322');
INSERT INTO "AnastKM101_blocks_io" VALUES('KPP101','tmp_l2',0,'','0');
INSERT INTO "AnastKM101_blocks_io" VALUES('DR100','t_kl1',0,'','10');
INSERT INTO "AnastKM101_blocks_io" VALUES('DR100','t_kl2',0,'','5');
INSERT INTO "AnastKM101_blocks_io" VALUES('DR100','tmp_l1',0,'','49.9999999999964');
INSERT INTO "AnastKM101_blocks_io" VALUES('DR100','tmp_l2',0,'','0');
INSERT INTO "AnastKM101_blocks_io" VALUES('node3_1','Kpr',0,'','0.95');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ101','t_kl1',0,'','10');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ101','t_kl2',0,'','5');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ101','tmp_l1',0,'','99.9999999999929');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ101','tmp_l2',0,'','0');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ104','t_kl1',0,'','10');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ104','t_kl2',0,'','5');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ104','tmp_l1',0,'','99.9999999999929');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ104','tmp_l2',0,'','0');
INSERT INTO "AnastKM101_blocks_io" VALUES('node4_1','Kpr',0,'','0.95');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ106','t_kl1',0,'','10');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ106','t_kl2',0,'','5');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ106','tmp_l1',0,'','0');
INSERT INTO "AnastKM101_blocks_io" VALUES('КШ106','tmp_l2',0,'','0');
INSERT INTO "AnastKM101_blocks_io" VALUES('PP101','Kpr',0,'','0.95');
INSERT INTO "AnastKM101_blocks_io" VALUES('PP102','Kpr',0,'','0.95');
CREATE TABLE 'AnastKM101_prm' ("SHIFR" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '' ,"IO" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#IO" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#IO" TEXT DEFAULT '' , PRIMARY KEY ("SHIFR"));
INSERT INTO "AnastKM101_prm" VALUES('FN101','FN101','Flow at the output of compressor',1,'КШ104.Fi:var:Variable','Витрати на виході компресора','Расход на выходе компрессора','','КШ104.Fi:var:Переменная','','КШ104.Fi:var:Змінна');
INSERT INTO "AnastKM101_prm" VALUES('F101','F101','Flow on the diaphragm PP101',1,'PP101.Fi:var:Variable
*s.tone/h:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.100:max:Maximum','Витрати на діафрагмі PP101','Расход на диафрагме PP101','','PP101.Fi:var:Переменная
*s.т/ч:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.100:max:Максимум','','PP101.Fi:var:Змінна
*s.т/год:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.100:max:Максимум');
INSERT INTO "AnastKM101_prm" VALUES('F102','F102','Flow on the diaphragm PP102',1,'PP102.Fi:var:Variable
*s.tone/h:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.100:max:Maximum','Витрати на діафрагмі PP102','Расход на диафрагме PP102','','PP102.Fi:var:Переменная
*s.т/ч:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.100:max:Максимум','','PP102.Fi:var:Змінна
*s.т/год:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.100:max:Максимум');
INSERT INTO "AnastKM101_prm" VALUES('TE1313_1','TE1313_1','The temperature at the input of the first stage of the compressor',1,'node3_1.To:var:Variable
*s.К:ed:Dimension
*i.0:prec:Precision
*r.273:min:Minimum
*r.373:max:Maximum','Температура на вході першої ступені компресора','Температура на входе первой ступени компрессора','','node3_1.To:var:Переменная
*s.К:ed:Ед.измерения
*i.0:prec:Точность
*r.273:min:Минимум
*r.373:max:Максимум','','node3_1.To:var:Змінна
*s.К:ed:Од.виміру
*i.0:prec:Точність
*r.273:min:Мінімум
*r.373:max:Максимум');
INSERT INTO "AnastKM101_prm" VALUES('PT0202','PT0202_1','Pressure after the first stage of the compressor',1,'КМ101_1.Po:var:Variable
*s.at:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.20:max:Maximum','Тиск після першої ступені компресору','Давление после первой ступени компрессора','','КМ101_1.Po:var:Переменная
*s.ат:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.20:max:Максимум','','КМ101_1.Po:var:Змінна
*s.ат:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.20:max:Максимум');
INSERT INTO "AnastKM101_prm" VALUES('PT0204','PT0204_1','Pressure after the second stage of the compressor',1,'КМ101_2.Po:var:Variable
*s.at:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.50:max:Maximum','Тиск після другої ступені компресору','Давление после второй ступени компрессора','','КМ101_2.Po:var:Переменная
*s.ат:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.50:max:Максимум','','КМ101_2.Po:var:Змінна
*s.ат:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.50:max:Максимум');
INSERT INTO "AnastKM101_prm" VALUES('TE1314_1','TE1314_1','The temperature after the fridge of the first stage of the compressor',1,'AT101_1.To:var:Variable
*s.К:ed:Dimension
*i.0:prec:Precision
*r.273:min:Minimum
*r.373:max:Maximum','Температура після холодильника першої ступені компресора','Температура после холодильника первой ступени компрессора','','AT101_1.To:var:Переменная
*s.К:ed:Ед.измерения
*i.0:prec:Точность
*r.273:min:Минимум
*r.373:max:Максимум','','AT101_1.To:var:Змінна
*s.К:ed:Од.виміру
*i.0:prec:Точність
*r.273:min:Мінімум
*r.373:max:Максимум');
INSERT INTO "AnastKM101_prm" VALUES('PT1006_1','PT1006_1','Pressure on the diaphragm PP102',1,'PP102.Pi:var:Variable
*s.at:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.20:max:Maximum','Тиск на діафрагмі PP102','Давление на диафрагме PP102','','PP102.Pi:var:Переменная
*s.ат:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.20:max:Максимум','','PP102.Pi:var:Змінна
*s.ат:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.20:max:Максимум');
INSERT INTO "AnastKM101_prm" VALUES('ST8612','ST8612_1','Rotation speed of the compressor',1,'КМ101_1.N:var:Variable
КМ101_1.N:varIn:Variable input
*s.1000x rpm:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.10:max:Maximum','Оберти компресору','Обороты вращения компрессора','','КМ101_1.N:var:Переменная
КМ101_1.N:varIn:Ввод переменной
*s.1000x об.мин.:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.10:max:Максимум','','КМ101_1.N:var:Змінна
КМ101_1.N:varIn:Ввід змінної
*s.1000x об.хвил:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.10:max:Максимум');
INSERT INTO "AnastKM101_prm" VALUES('KPP101','KPP101','Anti-surge control valve of the compressor',1,'KPP101.l_kl1:out:Output
KPP101.l_kl1:var:Variable
KPP101.l_kl1:varIn:Variable input
*s.%:ed:Dimension
*i.0:prec:Precision
*r.0:min:Minimum
*r.100:max:Maximum','Антипомпажний регулюючий клапан компресору','Антипомпажный регулирующий клапан компрессора','','KPP101.l_kl1:out:Выход
KPP101.l_kl1:var:Переменная
KPP101.l_kl1:varIn:Ввод переменной
*s.%:ed:Ед.измерения
*i.0:prec:Точность
*r.0:min:Минимум
*r.100:max:Максимум','','KPP101.l_kl1:out:Вихід
KPP101.l_kl1:var:Змінна
KPP101.l_kl1:varIn:Ввід змінної
*s.%:ed:Од.виміру
*i.0:prec:Точність
*r.0:min:Мінімум
*r.100:max:Максимум');
INSERT INTO "AnastKM101_prm" VALUES('P101','P101','Pressure on the diaphragm PP101',1,'PP101.Pi:var:Variable
*s.at:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.10:max:Maximum','Тиск на діафрагмі PP101','Давление на диафрагме PP101','','PP101.Pi:var:Переменная
*s.ат:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.10:max:Максимум','','PP101.Pi:var:Змінна
*s.ат:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.10:max:Максимум');
INSERT INTO "AnastKM101_prm" VALUES('TT0202','TE1202_1','Temperature after the first stage of the compressor',1,'КМ101_1.To:var:Variable
*s.К:ed:Dimension
*i.0:prec:Precision
*r.273:min:Minimum
*r.373:max:Maximum','Температура після першої ступені компресора','Температура после первой ступени компрессора','','КМ101_1.To:var:Переменная
*s.К:ed:Ед.измерения
*i.0:prec:Точность
*r.273:min:Минимум
*r.373:max:Максимум','','КМ101_1.To:var:Змінна
*s.К:ed:Од.виміру
*i.0:prec:Точність
*r.273:min:Мінімум
*r.373:max:Максимум');
INSERT INTO "AnastKM101_prm" VALUES('TT0204','TE1205_1','Temperature after the second stage of the compressor',1,'КМ101_2.To:var:Variable
*s.К:ed:Dimension
*i.0:prec:Precision
*r.273:min:Minimum
*r.433:max:Maximum','Температура після другої ступені компресора','Температура после второй ступени компрессора','','КМ101_2.To:var:Переменная
*s.К:ed:Ед.измерения
*i.0:prec:Точность
*r.273:min:Минимум
*r.433:max:Максимум','','КМ101_2.To:var:Змінна
*s.К:ed:Од.виміру
*i.0:prec:Точність
*r.273:min:Мінімум
*r.433:max:Максимум');
INSERT INTO "AnastKM101_prm" VALUES('TE1314_2','TE1206_1','The temperature after the fridge of the second stage of the compressor',1,'AT101_2.To:var:Variable
*s.К:ed:Dimension
*i.0:prec:Precision
*r.273:min:Minimum
*r.373:max:Maximum','Температура після холодильника другої ступені компресора','Температура после холодильника второй ступени компрессора','','AT101_2.To:var:Переменная
*s.К:ed:Ед.измерения
*i.0:prec:Точность
*r.273:min:Минимум
*r.373:max:Максимум','','AT101_2.To:var:Змінна
*s.К:ed:Од.виміру
*i.0:prec:Точність
*r.273:min:Мінімум
*r.373:max:Максимум');
INSERT INTO "AnastKM101_prm" VALUES('КШ101','KSH101','The control block of the ball valve KSH101',1,'КШ101им.com:com
КШ101им.st_open:st_open
КШ101им.st_close:st_close','Блок керування кульовим краном КШ101','Блок управления шарового крана КШ101','КШ101','','КШ101','');
INSERT INTO "AnastKM101_prm" VALUES('КШ102','KSH102','The control block of the ball valve KSH102',1,'КШ102им.com:com
КШ102им.st_open:st_open
КШ102им.st_close:st_close','Блок керування кульовим краном КШ102','Блок управления шарового крана КШ102','КШ102','','КШ102','');
INSERT INTO "AnastKM101_prm" VALUES('КШ104','KSH104','The control block of the ball valve KSH104',1,'КШ104им.com:com
КШ104им.st_open:st_open
КШ104им.st_close:st_close','Блок керування кульовим краном КШ104','Блок управления шарового крана КШ104','КШ104','','КШ104','');
INSERT INTO "AnastKM101_prm" VALUES('КШ105','KSH105','The control block of the ball valve KSH105',1,'КШ105им.com:com
КШ105им.st_open:st_open
КШ105им.st_close:st_close','Блок керування кульовим краном КШ105','Блок управления шарового крана КШ105','КШ105','','КШ105','');
INSERT INTO "AnastKM101_prm" VALUES('КШ106','KSH106','The control block of the ball valve KSH106',1,'КШ106им.com:com
КШ106им.st_open:st_open
КШ106им.st_close:st_close','Блок керування кульовим краном КШ106','Блок управления шарового крана КШ106','КШ106','','КШ106','');
INSERT INTO "AnastKM101_prm" VALUES('AT101_1','AT101_1','Control block of the fridge after the first stage of the compressor',1,'AT101_1.Ti:Ti
AT101_1.To:To
AT101_1.Wc:Wc','Блок контролю холодильника після першої ступені компресору','Блок контроля холодильника после первой ступени компрессора','','','','');
INSERT INTO "AnastKM101_prm" VALUES('AT101_2','AT101_2','Control block of the fridge after the second stage of the compressor',1,'AT101_2.Ti:Ti
AT101_2.To:To
AT101_2.Wc:Wc','Блок контролю холодильника після другої ступені компресору','Блок контроля холодильника после второй ступени компрессора','','','','');
CREATE TABLE 'AnastKM102_blocks_io' ("BLK_ID" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"TLNK" INTEGER DEFAULT '' ,"LNK" TEXT DEFAULT '' ,"VAL" TEXT DEFAULT '' , PRIMARY KEY ("BLK_ID","ID"));
INSERT INTO "AnastKM102_blocks_io" VALUES('KPP102','Fi',0,'','-2.47032822920623e-321');
INSERT INTO "AnastKM102_blocks_io" VALUES('KPP102','Pi',1,'node6_1.Po2','97.023724234605');
INSERT INTO "AnastKM102_blocks_io" VALUES('KPP102','Ti',1,'AT102_2.To','302.59704850528');
INSERT INTO "AnastKM102_blocks_io" VALUES('KPP102','Fo',1,'node5_1.Fi2','0');
INSERT INTO "AnastKM102_blocks_io" VALUES('KPP102','Po',0,'','97.0145409390343');
INSERT INTO "AnastKM102_blocks_io" VALUES('KPP102','To',0,'','273.000000008736');
INSERT INTO "AnastKM102_blocks_io" VALUES('KPP102','So',0,'','0.017');
INSERT INTO "AnastKM102_blocks_io" VALUES('KPP102','lo',0,'','10');
INSERT INTO "AnastKM102_blocks_io" VALUES('KPP102','S_kl1',0,'','0.015');
INSERT INTO "AnastKM102_blocks_io" VALUES('KPP102','S_kl2',0,'','0.015');
INSERT INTO "AnastKM102_blocks_io" VALUES('KPP102','l_kl1',2,'Anast1to2node_cntr.SurgeKM102.out','0');
INSERT INTO "AnastKM102_blocks_io" VALUES('KPP102','l_kl2',0,'','0');
INSERT INTO "AnastKM102_blocks_io" VALUES('KPP102','Q0',0,'','1');
INSERT INTO "AnastKM102_blocks_io" VALUES('KPP102','Kln',0,'','2');
INSERT INTO "AnastKM102_blocks_io" VALUES('KPP102','Kpr',0,'','0.95');
INSERT INTO "AnastKM102_blocks_io" VALUES('KPP102','Ct',0,'','20');
INSERT INTO "AnastKM102_blocks_io" VALUES('KPP102','Riz',0,'','20');
INSERT INTO "AnastKM102_blocks_io" VALUES('KPP102','noBack',0,'','0');
INSERT INTO "AnastKM102_blocks_io" VALUES('KPP102','Fwind',0,'','1');
INSERT INTO "AnastKM102_blocks_io" VALUES('KPP102','Twind',0,'','273');
INSERT INTO "AnastKM102_blocks_io" VALUES('KPP102','f_frq',0,'','100');
INSERT INTO "AnastKM102_blocks_io" VALUES('KPP102','Pot',0,'','100.418398526209');
INSERT INTO "AnastKM102_blocks_io" VALUES('KPP102','Fit',0,'','-0.571661292842614');
INSERT INTO "AnastKM102_blocks_io" VALUES('net102','Fi',0,'','-2.47522029840776e-07');
INSERT INTO "AnastKM102_blocks_io" VALUES('net102','Pi',1,'КШ113.Po','0.999999998532915');
INSERT INTO "AnastKM102_blocks_io" VALUES('net102','Po',0,'','1');
INSERT INTO "AnastKM102_blocks_io" VALUES('net102','So',0,'','0.01');
INSERT INTO "AnastKM102_blocks_io" VALUES('net102','Kpr',0,'','0.95');
INSERT INTO "AnastKM102_blocks_io" VALUES('net102','Noise',0,'','1');
INSERT INTO "AnastKM102_blocks_io" VALUES('net102','Q0',0,'','1');
INSERT INTO "AnastKM102_blocks_io" VALUES('net102','f_frq',0,'','100');
INSERT INTO "AnastKM102_blocks_io" VALUES('DR101','Fi',0,'','4.62036872502228e-06');
INSERT INTO "AnastKM102_blocks_io" VALUES('DR101','Pi',1,'КШ116.Po','22.2426084608522');
INSERT INTO "AnastKM102_blocks_io" VALUES('DR101','Ti',1,'КШ116.To','273');
INSERT INTO "AnastKM102_blocks_io" VALUES('DR101','Fo',1,'node5_1.Fi1','-0.000522849298362862');
INSERT INTO "AnastKM102_blocks_io" VALUES('DR101','Po',0,'','22.2426488746504');
INSERT INTO "AnastKM102_blocks_io" VALUES('DR101','To',0,'','272.999999998362');
INSERT INTO "AnastKM102_blocks_io" VALUES('DR101','So',0,'','0.0314');
INSERT INTO "AnastKM102_blocks_io" VALUES('DR101','lo',0,'','3');
INSERT INTO "AnastKM102_blocks_io" VALUES('DR101','S_kl1',0,'','0.00314');
INSERT INTO "AnastKM102_blocks_io" VALUES('DR101','S_kl2',0,'','0.00314');
INSERT INTO "AnastKM102_blocks_io" VALUES('DR101','l_kl1',0,'','50');
INSERT INTO "AnastKM102_blocks_io" VALUES('DR101','l_kl2',0,'','0');
INSERT INTO "AnastKM102_blocks_io" VALUES('DR101','Q0',0,'','1');
INSERT INTO "AnastKM102_blocks_io" VALUES('DR101','Kln',0,'','1');
INSERT INTO "AnastKM102_blocks_io" VALUES('DR101','Kpr',0,'','0.95');
INSERT INTO "AnastKM102_blocks_io" VALUES('DR101','Ct',0,'','20');
INSERT INTO "AnastKM102_blocks_io" VALUES('DR101','Riz',0,'','20');
INSERT INTO "AnastKM102_blocks_io" VALUES('DR101','noBack',0,'','0');
INSERT INTO "AnastKM102_blocks_io" VALUES('DR101','Fwind',0,'','1');
INSERT INTO "AnastKM102_blocks_io" VALUES('DR101','Twind',0,'','273');
INSERT INTO "AnastKM102_blocks_io" VALUES('DR101','f_frq',0,'','100');
INSERT INTO "AnastKM102_blocks_io" VALUES('DR101','Pot',0,'','20.670186602699');
INSERT INTO "AnastKM102_blocks_io" VALUES('DR101','Fit',0,'','0.144757022511773');
INSERT INTO "AnastKM102_blocks_io" VALUES('С102_1','Fi',0,'','52.9487632618921');
INSERT INTO "AnastKM102_blocks_io" VALUES('С102_1','Pi',1,'node5_1.Po','22.2427793882583');
INSERT INTO "AnastKM102_blocks_io" VALUES('С102_1','Si',0,'','0.0314');
INSERT INTO "AnastKM102_blocks_io" VALUES('С102_1','Fo',1,'PP103.Fi','52.4160451361844');
INSERT INTO "AnastKM102_blocks_io" VALUES('С102_1','Po',0,'','21.8988442984421');
INSERT INTO "AnastKM102_blocks_io" VALUES('С102_1','So',0,'','0.0314');
INSERT INTO "AnastKM102_blocks_io" VALUES('С102_1','lo',0,'','40');
INSERT INTO "AnastKM102_blocks_io" VALUES('С102_1','Fo_ж',0,'','0');
INSERT INTO "AnastKM102_blocks_io" VALUES('С102_1','Po_ж',0,'','21.9495975033992');
INSERT INTO "AnastKM102_blocks_io" VALUES('С102_1','Lж',0,'','21.1471687320987');
INSERT INTO "AnastKM102_blocks_io" VALUES('С102_1','ProcЖ',0,'','0.01');
INSERT INTO "AnastKM102_blocks_io" VALUES('С102_1','Vap',0,'','2.4');
INSERT INTO "AnastKM102_blocks_io" VALUES('С102_1','Q0',0,'','1');
INSERT INTO "AnastKM102_blocks_io" VALUES('С102_1','Qж',0,'','1000');
INSERT INTO "AnastKM102_blocks_io" VALUES('С102_1','f_frq',0,'','100');
INSERT INTO "AnastKM102_blocks_io" VALUES('С102_1','Pot',0,'','20.0205661668655');
INSERT INTO "AnastKM102_blocks_io" VALUES('С102_1','Fit',0,'','38.0815151265775');
INSERT INTO "AnastKM102_blocks_io" VALUES('AT102_1','Fi',0,'','52.4112153476326');
INSERT INTO "AnastKM102_blocks_io" VALUES('AT102_1','Pi',1,'KM102_1.Po','45.9359056911944');
INSERT INTO "AnastKM102_blocks_io" VALUES('AT102_1','Ti',1,'KM102_1.To','344.908781254884');
INSERT INTO "AnastKM102_blocks_io" VALUES('AT102_1','Si',0,'','0.015');
INSERT INTO "AnastKM102_blocks_io" VALUES('AT102_1','li',0,'','10');
INSERT INTO "AnastKM102_blocks_io" VALUES('AT102_1','Fo',1,'C102_2.Fi','52.411095363977');
INSERT INTO "AnastKM102_blocks_io" VALUES('AT102_1','Po',0,'','45.2316125848421');
INSERT INTO "AnastKM102_blocks_io" VALUES('AT102_1','To',0,'','295.853985893063');
INSERT INTO "AnastKM102_blocks_io" VALUES('AT102_1','So',0,'','0.017');
INSERT INTO "AnastKM102_blocks_io" VALUES('AT102_1','lo',0,'','2');
INSERT INTO "AnastKM102_blocks_io" VALUES('AT102_1','Tair',0,'','283');
INSERT INTO "AnastKM102_blocks_io" VALUES('AT102_1','Wc',0,'','200');
INSERT INTO "AnastKM102_blocks_io" VALUES('AT102_1','Q0',0,'','1');
INSERT INTO "AnastKM102_blocks_io" VALUES('AT102_1','Ct',0,'','100');
INSERT INTO "AnastKM102_blocks_io" VALUES('AT102_1','Rt',0,'','1');
INSERT INTO "AnastKM102_blocks_io" VALUES('AT102_1','f_frq',0,'','100');
INSERT INTO "AnastKM102_blocks_io" VALUES('AT102_1','Pot',0,'','44.4700839903117');
INSERT INTO "AnastKM102_blocks_io" VALUES('AT102_1','Fit',0,'','38.0819539655514');
INSERT INTO "AnastKM102_blocks_io" VALUES('AT102_2','Fi',0,'','51.8823278879906');
INSERT INTO "AnastKM102_blocks_io" VALUES('AT102_2','Pi',1,'KM102_2.Po','99.6653191985681');
INSERT INTO "AnastKM102_blocks_io" VALUES('AT102_2','Ti',1,'KM102_2.To','378.157118683198');
INSERT INTO "AnastKM102_blocks_io" VALUES('AT102_2','Si',0,'','0.007');
INSERT INTO "AnastKM102_blocks_io" VALUES('AT102_2','li',0,'','20');
INSERT INTO "AnastKM102_blocks_io" VALUES('AT102_2','Fo',1,'C102_3.Fi','51.88224767217');
INSERT INTO "AnastKM102_blocks_io" VALUES('AT102_2','Po',0,'','98.2036640067066');
INSERT INTO "AnastKM102_blocks_io" VALUES('AT102_2','To',0,'','302.597028984268');
INSERT INTO "AnastKM102_blocks_io" VALUES('AT102_2','So',0,'','0.00785');
INSERT INTO "AnastKM102_blocks_io" VALUES('AT102_2','lo',0,'','5');
INSERT INTO "AnastKM102_blocks_io" VALUES('AT102_2','Tair',0,'','283');
INSERT INTO "AnastKM102_blocks_io" VALUES('AT102_2','Wc',0,'','200');
INSERT INTO "AnastKM102_blocks_io" VALUES('AT102_2','Q0',0,'','1');
INSERT INTO "AnastKM102_blocks_io" VALUES('AT102_2','Ct',0,'','100');
INSERT INTO "AnastKM102_blocks_io" VALUES('AT102_2','Rt',0,'','1');
INSERT INTO "AnastKM102_blocks_io" VALUES('AT102_2','f_frq',0,'','100');
INSERT INTO "AnastKM102_blocks_io" VALUES('AT102_2','Pot',0,'','102.786678846804');
INSERT INTO "AnastKM102_blocks_io" VALUES('AT102_2','Fit',0,'','37.0335349287507');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ111','Fi',0,'','52.975285853156');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ111','Pi',0,'','22.9069169690583');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ111','Ti',2,'Anast1to2node.КШ21.To','305.032394275123');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ111','Fo',1,'node5_1.Fi3','52.9748857131199');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ111','Po',0,'','22.5774639556264');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ111','To',0,'','304.883882553769');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ111','So',0,'','0.0314');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ111','lo',0,'','5');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ111','S_kl1',0,'','0.0314');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ111','S_kl2',0,'','0.002');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ111','l_kl1',1,'КШ111им.pos','100');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ111','l_kl2',1,'КШ112им.pos','0');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ111','Q0',0,'','1');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ111','Kln',0,'','1');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ111','Kpr',0,'','0.95');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ111','Ct',0,'','20');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ111','Riz',0,'','20');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ111','noBack',0,'','0');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ111','Fwind',0,'','1');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ111','Twind',0,'','273');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ111','f_frq',0,'','100');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ111','Pot',0,'','21.3671095075236');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ111','Fit',0,'','38.4838680292946');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ113','Fi',0,'','0');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ113','Pi',1,'node6_1.Po4','97.0237546313385');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ113','Ti',0,'','273');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ113','Fo',1,'net102.Fi','2.7533195876183e-07');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ113','Po',0,'','0.999999998007133');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ113','To',0,'','272.999999995966');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ113','So',0,'','0.00785');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ113','lo',0,'','10');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ113','S_kl1',0,'','0.00785');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ113','S_kl2',0,'','0.002');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ113','l_kl1',0,'','0');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ113','l_kl2',0,'','0');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ113','Q0',0,'','1');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ113','Kln',0,'','1');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ113','Kpr',0,'','0.95');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ113','Ct',0,'','20');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ113','Riz',0,'','20');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ113','noBack',0,'','0');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ113','Fwind',0,'','1');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ113','Twind',0,'','273');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ113','f_frq',0,'','100');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ113','Pot',0,'','0.999957173185118');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ113','Fit',0,'','0');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ114','Fi',0,'','51.3632187390885');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ114','Pi',1,'node6_1.Po3','95.0207210676986');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ114','Ti',1,'AT102_2.To','302.597116921797');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ114','Fo',0,'','51.3626175806035');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ114','Po',0,'','93.814609238474');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ114','To',0,'','302.456441913775');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ114','So',0,'','0.00785');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ114','lo',0,'','100');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ114','S_kl1',0,'','0.00785');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ114','S_kl2',0,'','0.002');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ114','l_kl1',1,'КШ114им.pos','100');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ114','l_kl2',1,'КШ115им.pos','0');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ114','Q0',0,'','1');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ114','Kln',0,'','1');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ114','Kpr',0,'','0.95');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ114','Ct',0,'','20');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ114','Riz',0,'','20');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ114','noBack',0,'','1');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ114','Fwind',0,'','1');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ114','Twind',0,'','273');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ114','f_frq',0,'','100');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ114','Pot',0,'','94.694514564321');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ114','Fit',0,'','37.5187167024292');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ116','Fi',0,'','0');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ116','Pi',1,'node6_1.Po1','97.0237891272321');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ116','Ti',1,'AT102_2.To','302.597121808524');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ116','Fo',1,'DR101.Fi','4.35837467776559e-06');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ116','Po',0,'','22.2427375954057');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ116','To',0,'','273');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ116','So',0,'','0.0078');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ116','lo',0,'','1');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ116','S_kl1',0,'','0.0078');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ116','S_kl2',0,'','0.0078');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ116','l_kl1',1,'КШ116им.pos','0');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ116','l_kl2',0,'','0');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ116','Q0',0,'','1');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ116','Kln',0,'','1');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ116','Kpr',0,'','0.95');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ116','Ct',0,'','20');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ116','Riz',0,'','20');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ116','noBack',0,'','0');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ116','Fwind',0,'','1');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ116','Twind',0,'','273');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ116','f_frq',0,'','100');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ116','Pot',0,'','20.6324366115474');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ116','Fit',0,'','0');
INSERT INTO "AnastKM102_blocks_io" VALUES('node5_1','Fi1',0,'','-0.000122337270033962');
INSERT INTO "AnastKM102_blocks_io" VALUES('node5_1','Pi1',1,'DR101.Po','22.2426791801872');
INSERT INTO "AnastKM102_blocks_io" VALUES('node5_1','Ti1',1,'DR101.To','272.999999998362');
INSERT INTO "AnastKM102_blocks_io" VALUES('node5_1','Si1',0,'','0.0314');
INSERT INTO "AnastKM102_blocks_io" VALUES('node5_1','Fi2',0,'','0');
INSERT INTO "AnastKM102_blocks_io" VALUES('node5_1','Pi2',1,'KPP102.Po','97.0145409390343');
INSERT INTO "AnastKM102_blocks_io" VALUES('node5_1','Ti2',1,'KPP102.To','273.000000008736');
INSERT INTO "AnastKM102_blocks_io" VALUES('node5_1','Si2',0,'','0');
INSERT INTO "AnastKM102_blocks_io" VALUES('node5_1','Fi3',0,'','52.9748354856799');
INSERT INTO "AnastKM102_blocks_io" VALUES('node5_1','Pi3',1,'КШ111.Po','22.5774408670355');
INSERT INTO "AnastKM102_blocks_io" VALUES('node5_1','Ti3',1,'КШ111.To','304.883883489931');
INSERT INTO "AnastKM102_blocks_io" VALUES('node5_1','Si3',0,'','0.0314');
INSERT INTO "AnastKM102_blocks_io" VALUES('node5_1','Fo',1,'С102_1.Fi','52.9484505786785');
INSERT INTO "AnastKM102_blocks_io" VALUES('node5_1','Po',0,'','22.2426679094681');
INSERT INTO "AnastKM102_blocks_io" VALUES('node5_1','To',0,'','304.824204336832');
INSERT INTO "AnastKM102_blocks_io" VALUES('node5_1','So',0,'','0.2');
INSERT INTO "AnastKM102_blocks_io" VALUES('node5_1','lo',0,'','50');
INSERT INTO "AnastKM102_blocks_io" VALUES('node5_1','Q0',0,'','1');
INSERT INTO "AnastKM102_blocks_io" VALUES('node5_1','Ct',0,'','20');
INSERT INTO "AnastKM102_blocks_io" VALUES('node5_1','Riz',0,'','20');
INSERT INTO "AnastKM102_blocks_io" VALUES('node5_1','Fwind',0,'','1');
INSERT INTO "AnastKM102_blocks_io" VALUES('node5_1','Twind',0,'','273');
INSERT INTO "AnastKM102_blocks_io" VALUES('node5_1','f_frq',0,'','100');
INSERT INTO "AnastKM102_blocks_io" VALUES('node5_1','FoR',0,'','49.8962433857394');
INSERT INTO "AnastKM102_blocks_io" VALUES('node5_1','Pot',0,'','20.6955244054771');
INSERT INTO "AnastKM102_blocks_io" VALUES('node5_1','Fit',0,'','38.6468204088627');
INSERT INTO "AnastKM102_blocks_io" VALUES('KM102_1','Fi',0,'','52.4151512244984');
INSERT INTO "AnastKM102_blocks_io" VALUES('KM102_1','Pi',1,'PP103.Po','21.0672754887876');
INSERT INTO "AnastKM102_blocks_io" VALUES('KM102_1','Ti',0,'','273');
INSERT INTO "AnastKM102_blocks_io" VALUES('KM102_1','Fo',1,'AT102_1.Fi','52.4112682725117');
INSERT INTO "AnastKM102_blocks_io" VALUES('KM102_1','Po',0,'','45.9359528866587');
INSERT INTO "AnastKM102_blocks_io" VALUES('KM102_1','To',0,'','344.908770208785');
INSERT INTO "AnastKM102_blocks_io" VALUES('KM102_1','So',0,'','0.017');
INSERT INTO "AnastKM102_blocks_io" VALUES('KM102_1','lo',0,'','55');
INSERT INTO "AnastKM102_blocks_io" VALUES('KM102_1','Kzp',0,'','0.465581951497473');
INSERT INTO "AnastKM102_blocks_io" VALUES('KM102_1','N',0,'','11');
INSERT INTO "AnastKM102_blocks_io" VALUES('KM102_1','V',0,'','0.27');
INSERT INTO "AnastKM102_blocks_io" VALUES('KM102_1','Kpmp',0,'','0.02');
INSERT INTO "AnastKM102_blocks_io" VALUES('KM102_1','Kslp',0,'','0.2');
INSERT INTO "AnastKM102_blocks_io" VALUES('KM102_1','Q0',0,'','1');
INSERT INTO "AnastKM102_blocks_io" VALUES('KM102_1','Ct',0,'','100');
INSERT INTO "AnastKM102_blocks_io" VALUES('KM102_1','Riz',0,'','100');
INSERT INTO "AnastKM102_blocks_io" VALUES('KM102_1','Fwind',0,'','1');
INSERT INTO "AnastKM102_blocks_io" VALUES('KM102_1','Twind',0,'','273');
INSERT INTO "AnastKM102_blocks_io" VALUES('KM102_1','f_frq',0,'','100');
INSERT INTO "AnastKM102_blocks_io" VALUES('KM102_1','Pot',0,'','46.0725119514837');
INSERT INTO "AnastKM102_blocks_io" VALUES('KM102_1','Fit',0,'','52.4137670039657');
INSERT INTO "AnastKM102_blocks_io" VALUES('KM102_2','Fi',0,'','51.8832392122994');
INSERT INTO "AnastKM102_blocks_io" VALUES('KM102_2','Pi',1,'PP104.Po','43.9587917164095');
INSERT INTO "AnastKM102_blocks_io" VALUES('KM102_2','Ti',1,'AT102_1.To','295.853994319882');
INSERT INTO "AnastKM102_blocks_io" VALUES('KM102_2','Fo',1,'AT102_2.Fi','51.88237465199');
INSERT INTO "AnastKM102_blocks_io" VALUES('KM102_2','Po',0,'','99.6653420512902');
INSERT INTO "AnastKM102_blocks_io" VALUES('KM102_2','To',0,'','378.157124961249');
INSERT INTO "AnastKM102_blocks_io" VALUES('KM102_2','So',0,'','0.00785');
INSERT INTO "AnastKM102_blocks_io" VALUES('KM102_2','lo',0,'','55');
INSERT INTO "AnastKM102_blocks_io" VALUES('KM102_2','Kzp',0,'','0.500468241046507');
INSERT INTO "AnastKM102_blocks_io" VALUES('KM102_2','N',1,'KM102_1.N','11');
INSERT INTO "AnastKM102_blocks_io" VALUES('KM102_2','V',0,'','0.13');
INSERT INTO "AnastKM102_blocks_io" VALUES('KM102_2','Kpmp',0,'','0.02');
INSERT INTO "AnastKM102_blocks_io" VALUES('KM102_2','Kslp',0,'','0.2');
INSERT INTO "AnastKM102_blocks_io" VALUES('KM102_2','Q0',0,'','1');
INSERT INTO "AnastKM102_blocks_io" VALUES('KM102_2','Ct',0,'','100');
INSERT INTO "AnastKM102_blocks_io" VALUES('KM102_2','Riz',0,'','100');
INSERT INTO "AnastKM102_blocks_io" VALUES('KM102_2','Fwind',0,'','1');
INSERT INTO "AnastKM102_blocks_io" VALUES('KM102_2','Twind',0,'','273');
INSERT INTO "AnastKM102_blocks_io" VALUES('KM102_2','f_frq',0,'','100');
INSERT INTO "AnastKM102_blocks_io" VALUES('KM102_2','Pot',0,'','105.987273594529');
INSERT INTO "AnastKM102_blocks_io" VALUES('KM102_2','Fit',0,'','51.8819871107681');
INSERT INTO "AnastKM102_blocks_io" VALUES('node6_1','Fi',0,'','51.3633716621038');
INSERT INTO "AnastKM102_blocks_io" VALUES('node6_1','Pi',1,'C102_3.Po','97.0237479775984');
INSERT INTO "AnastKM102_blocks_io" VALUES('node6_1','Fo1',1,'КШ116.Fi','0');
INSERT INTO "AnastKM102_blocks_io" VALUES('node6_1','Po1',0,'','97.0237392752647');
INSERT INTO "AnastKM102_blocks_io" VALUES('node6_1','So1',0,'','0.006');
INSERT INTO "AnastKM102_blocks_io" VALUES('node6_1','lo1',0,'','30');
INSERT INTO "AnastKM102_blocks_io" VALUES('node6_1','Fo2',1,'KPP102.Fi','-2.47032822920623e-321');
INSERT INTO "AnastKM102_blocks_io" VALUES('node6_1','Po2',0,'','97.0237384988261');
INSERT INTO "AnastKM102_blocks_io" VALUES('node6_1','So2',0,'','0.002');
INSERT INTO "AnastKM102_blocks_io" VALUES('node6_1','lo2',0,'','30');
INSERT INTO "AnastKM102_blocks_io" VALUES('node6_1','Fo3',1,'КШ114.Fi','51.3630233607936');
INSERT INTO "AnastKM102_blocks_io" VALUES('node6_1','Po3',0,'','95.0206875930244');
INSERT INTO "AnastKM102_blocks_io" VALUES('node6_1','So3',0,'','0.006');
INSERT INTO "AnastKM102_blocks_io" VALUES('node6_1','lo3',0,'','5');
INSERT INTO "AnastKM102_blocks_io" VALUES('node6_1','Fo4',1,'КШ113.Fi','0');
INSERT INTO "AnastKM102_blocks_io" VALUES('node6_1','Po4',0,'','97.0237298232575');
INSERT INTO "AnastKM102_blocks_io" VALUES('node6_1','So4',0,'','0.006');
INSERT INTO "AnastKM102_blocks_io" VALUES('node6_1','lo4',0,'','10');
INSERT INTO "AnastKM102_blocks_io" VALUES('node6_1','Q0',0,'','1');
INSERT INTO "AnastKM102_blocks_io" VALUES('node6_1','f_frq',0,'','100');
INSERT INTO "AnastKM102_blocks_io" VALUES('node6_1','F1tmp',0,'','0.000245011205778596');
INSERT INTO "AnastKM102_blocks_io" VALUES('node6_1','F2tmp',0,'','7.62221791835992e-05');
INSERT INTO "AnastKM102_blocks_io" VALUES('node6_1','F3tmp',0,'','51.3630455135423');
INSERT INTO "AnastKM102_blocks_io" VALUES('node6_1','F4tmp',0,'','4.91517652632102e-06');
INSERT INTO "AnastKM102_blocks_io" VALUES('node6_1','Pot1',0,'','99.7120911148752');
INSERT INTO "AnastKM102_blocks_io" VALUES('node6_1','Pot2',0,'','102.187471654186');
INSERT INTO "AnastKM102_blocks_io" VALUES('node6_1','Pot3',0,'','96.7285702901443');
INSERT INTO "AnastKM102_blocks_io" VALUES('node6_1','Pot4',0,'','102.816636476565');
INSERT INTO "AnastKM102_blocks_io" VALUES('PP103','Fi',0,'','52.415746221341');
INSERT INTO "AnastKM102_blocks_io" VALUES('PP103','Pi',1,'С102_1.Po','21.8987202054638');
INSERT INTO "AnastKM102_blocks_io" VALUES('PP103','Fo',1,'KM102_1.Fi','52.4152068838912');
INSERT INTO "AnastKM102_blocks_io" VALUES('PP103','Po',0,'','21.0673033744963');
INSERT INTO "AnastKM102_blocks_io" VALUES('PP103','dP',0,'','83.1386259164306');
INSERT INTO "AnastKM102_blocks_io" VALUES('PP103','Sdf',0,'','0.02');
INSERT INTO "AnastKM102_blocks_io" VALUES('PP103','So',0,'','0.0314');
INSERT INTO "AnastKM102_blocks_io" VALUES('PP103','lo',0,'','7');
INSERT INTO "AnastKM102_blocks_io" VALUES('PP103','Q0',0,'','1');
INSERT INTO "AnastKM102_blocks_io" VALUES('PP103','f_frq',0,'','100');
INSERT INTO "AnastKM102_blocks_io" VALUES('PP103','Pot',0,'','18.2874094324631');
INSERT INTO "AnastKM102_blocks_io" VALUES('PP103','Fit',0,'','38.0818872167009');
INSERT INTO "AnastKM102_blocks_io" VALUES('PP104','Fi',0,'','51.8837330490873');
INSERT INTO "AnastKM102_blocks_io" VALUES('PP104','Pi',1,'C102_2.Po','44.6692367760974');
INSERT INTO "AnastKM102_blocks_io" VALUES('PP104','Fo',1,'KM102_2.Fi','51.8832724136991');
INSERT INTO "AnastKM102_blocks_io" VALUES('PP104','Po',0,'','43.9588247139507');
INSERT INTO "AnastKM102_blocks_io" VALUES('PP104','dP',0,'','71.0385321297782');
INSERT INTO "AnastKM102_blocks_io" VALUES('PP104','Sdf',0,'','0.015');
INSERT INTO "AnastKM102_blocks_io" VALUES('PP104','So',0,'','0.017');
INSERT INTO "AnastKM102_blocks_io" VALUES('PP104','lo',0,'','7');
INSERT INTO "AnastKM102_blocks_io" VALUES('PP104','Q0',0,'','1');
INSERT INTO "AnastKM102_blocks_io" VALUES('PP104','f_frq',0,'','100');
INSERT INTO "AnastKM102_blocks_io" VALUES('PP104','Pot',0,'','42.0718168785713');
INSERT INTO "AnastKM102_blocks_io" VALUES('PP104','Fit',0,'','37.6972413769621');
INSERT INTO "AnastKM102_blocks_io" VALUES('C102_2','Fi',0,'','52.4111129742093');
INSERT INTO "AnastKM102_blocks_io" VALUES('C102_2','Pi',1,'AT102_1.Po','45.2316240913139');
INSERT INTO "AnastKM102_blocks_io" VALUES('C102_2','Si',0,'','0.017');
INSERT INTO "AnastKM102_blocks_io" VALUES('C102_2','Fo',1,'PP104.Fi','51.8836495805632');
INSERT INTO "AnastKM102_blocks_io" VALUES('C102_2','Po',0,'','44.6691916798764');
INSERT INTO "AnastKM102_blocks_io" VALUES('C102_2','So',0,'','0.017');
INSERT INTO "AnastKM102_blocks_io" VALUES('C102_2','lo',0,'','50');
INSERT INTO "AnastKM102_blocks_io" VALUES('C102_2','Fo_ж',0,'','0');
INSERT INTO "AnastKM102_blocks_io" VALUES('C102_2','Po_ж',0,'','44.718976505748');
INSERT INTO "AnastKM102_blocks_io" VALUES('C102_2','Lж',0,'','24.8924129358241');
INSERT INTO "AnastKM102_blocks_io" VALUES('C102_2','ProcЖ',0,'','0.01');
INSERT INTO "AnastKM102_blocks_io" VALUES('C102_2','Vap',0,'','2');
INSERT INTO "AnastKM102_blocks_io" VALUES('C102_2','Q0',0,'','1');
INSERT INTO "AnastKM102_blocks_io" VALUES('C102_2','Qж',0,'','1000');
INSERT INTO "AnastKM102_blocks_io" VALUES('C102_2','f_frq',0,'','100');
INSERT INTO "AnastKM102_blocks_io" VALUES('C102_2','Pot',0,'','43.424525814954');
INSERT INTO "AnastKM102_blocks_io" VALUES('C102_2','Fit',0,'','37.7013136906827');
INSERT INTO "AnastKM102_blocks_io" VALUES('C102_3','Fi',0,'','51.8823382761971');
INSERT INTO "AnastKM102_blocks_io" VALUES('C102_3','Pi',1,'AT102_2.Po','98.2036658852066');
INSERT INTO "AnastKM102_blocks_io" VALUES('C102_3','Si',0,'','0.00785');
INSERT INTO "AnastKM102_blocks_io" VALUES('C102_3','Fo',1,'node6_1.Fi','51.362992236282');
INSERT INTO "AnastKM102_blocks_io" VALUES('C102_3','Po',0,'','97.0237231048438');
INSERT INTO "AnastKM102_blocks_io" VALUES('C102_3','So',0,'','0.00785');
INSERT INTO "AnastKM102_blocks_io" VALUES('C102_3','lo',0,'','8');
INSERT INTO "AnastKM102_blocks_io" VALUES('C102_3','Fo_ж',0,'','0');
INSERT INTO "AnastKM102_blocks_io" VALUES('C102_3','Po_ж',0,'','97.0751351733665');
INSERT INTO "AnastKM102_blocks_io" VALUES('C102_3','Lж',0,'','34.2747123485103');
INSERT INTO "AnastKM102_blocks_io" VALUES('C102_3','ProcЖ',0,'','0.01');
INSERT INTO "AnastKM102_blocks_io" VALUES('C102_3','Vap',0,'','1.5');
INSERT INTO "AnastKM102_blocks_io" VALUES('C102_3','Q0',0,'','1');
INSERT INTO "AnastKM102_blocks_io" VALUES('C102_3','Qж',0,'','1000');
INSERT INTO "AnastKM102_blocks_io" VALUES('C102_3','f_frq',0,'','100');
INSERT INTO "AnastKM102_blocks_io" VALUES('C102_3','Pot',0,'','100.289121612061');
INSERT INTO "AnastKM102_blocks_io" VALUES('C102_3','Fit',0,'','37.1200537368204');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ111им','pos',0,'','100');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ111им','com',0,'','1');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ111им','st_open',0,'','1');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ111им','st_close',0,'','0');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ111им','t_full',0,'','5');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ111им','t_up',0,'','0.5');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ111им','f_frq',0,'','100');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ111им','tmp_up',0,'','0');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ111им','lst_com',0,'','1');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ112им','pos',0,'','0');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ112им','com',0,'','0');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ112им','st_open',0,'','0');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ112им','st_close',0,'','1');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ112им','t_full',0,'','3');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ112им','t_up',0,'','0.3');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ112им','f_frq',0,'','100');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ112им','tmp_up',0,'','0');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ112им','lst_com',0,'','0');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ114им','pos',0,'','100');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ114им','com',0,'','1');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ114им','st_open',0,'','1');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ114им','st_close',0,'','0');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ114им','t_full',0,'','5');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ114им','t_up',0,'','0.5');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ114им','f_frq',0,'','100');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ114им','tmp_up',0,'','0');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ114им','lst_com',0,'','1');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ115им','pos',0,'','0');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ115им','com',0,'','0');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ115им','st_open',0,'','0');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ115им','st_close',0,'','1');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ115им','t_full',0,'','5');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ115им','t_up',0,'','0.5');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ115им','f_frq',0,'','100');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ115им','tmp_up',0,'','0');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ115им','lst_com',0,'','0');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ116им','pos',0,'','0');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ116им','com',0,'','0');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ116им','st_open',0,'','0');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ116им','st_close',0,'','1');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ116им','t_full',0,'','5');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ116им','t_up',0,'','0.5');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ116им','f_frq',0,'','100');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ116им','tmp_up',0,'','0');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ116им','lst_com',0,'','0');
INSERT INTO "AnastKM102_blocks_io" VALUES('KPP102','t_kl1',0,'','3');
INSERT INTO "AnastKM102_blocks_io" VALUES('KPP102','t_kl2',0,'','5');
INSERT INTO "AnastKM102_blocks_io" VALUES('KPP102','tmp_l1',0,'','7.4109846876187e-322');
INSERT INTO "AnastKM102_blocks_io" VALUES('KPP102','tmp_l2',0,'','0');
INSERT INTO "AnastKM102_blocks_io" VALUES('DR101','t_kl1',0,'','10');
INSERT INTO "AnastKM102_blocks_io" VALUES('DR101','t_kl2',0,'','5');
INSERT INTO "AnastKM102_blocks_io" VALUES('DR101','tmp_l1',0,'','49.9999999999964');
INSERT INTO "AnastKM102_blocks_io" VALUES('DR101','tmp_l2',0,'','0');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ111','t_kl1',0,'','10');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ111','t_kl2',0,'','5');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ111','tmp_l1',0,'','99.9999999999929');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ111','tmp_l2',0,'','0');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ113','t_kl1',0,'','10');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ113','t_kl2',0,'','5');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ113','tmp_l1',0,'','0');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ113','tmp_l2',0,'','0');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ114','t_kl1',0,'','10');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ114','t_kl2',0,'','5');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ114','tmp_l1',0,'','99.9999999999929');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ114','tmp_l2',0,'','0');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ116','t_kl1',0,'','10');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ116','t_kl2',0,'','5');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ116','tmp_l1',0,'','0');
INSERT INTO "AnastKM102_blocks_io" VALUES('КШ116','tmp_l2',0,'','0');
INSERT INTO "AnastKM102_blocks_io" VALUES('node5_1','Kpr',0,'','0.95');
INSERT INTO "AnastKM102_blocks_io" VALUES('KM102_1','Kpr',0,'','0.95');
INSERT INTO "AnastKM102_blocks_io" VALUES('KM102_2','Kpr',0,'','0.95');
INSERT INTO "AnastKM102_blocks_io" VALUES('node6_1','Kpr',0,'','0.95');
INSERT INTO "AnastKM102_blocks_io" VALUES('PP103','Kpr',0,'','0.95');
INSERT INTO "AnastKM102_blocks_io" VALUES('PP104','Kpr',0,'','0.95');
CREATE TABLE 'AnastKM102_prm' ("SHIFR" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '' ,"IO" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#IO" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#IO" TEXT DEFAULT '' , PRIMARY KEY ("SHIFR"));
INSERT INTO "AnastKM102_prm" VALUES('F103','F103','Flow on the diaphragm PP103',1,'PP103.Fi:var:Variable
*s.tone/h:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.100:max:Maximum','Витрати на діафрагмі PP103','Расход на диафрагме PP103','','PP103.Fi:var:Переменная
*s.т/ч:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.100:max:Максимум','','PP103.Fi:var:Змінна
*s.т/год:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.100:max:Максимум');
INSERT INTO "AnastKM102_prm" VALUES('F104','F104','Flow on the diaphragm PP104',1,'PP104.Fi:var:Variable
*s.tone/h:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.100:max:Maximum','Витрати на діафрагмі PP104','Расход на диафрагме PP104','','PP104.Fi:var:Переменная
*s.т/ч:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.100:max:Максимум','','PP104.Fi:var:Змінна
*s.т/год:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.100:max:Максимум');
INSERT INTO "AnastKM102_prm" VALUES('KPP102','KPP102','Anti-surge control valve of the compressor',1,'KPP102.l_kl1:out:Output
KPP102.l_kl1:var:Variable
KPP102.l_kl1:varIn:Variable input
*s.%:ed:Dimension
*i.0:prec:Precision
*r.0:min:Minimum
*r.100:max:Maximum','Антипомпажний регулюючий клапан компресору','Антипомпажный регулирующий клапан компрессора','','KPP102.l_kl1:out:Выход
KPP102.l_kl1:var:Переменная
KPP102.l_kl1:varIn:Ввод переменной
*s.%:ed:Ед.измерения
*i.0:prec:Точность
*r.0:min:Минимум
*r.100:max:Максимум','','KPP102.l_kl1:out:Вихід
KPP102.l_kl1:var:Змінна
KPP102.l_kl1:varIn:Ввід змінної
*s.%:ed:Од.виміру
*i.0:prec:Точність
*r.0:min:Мінімум
*r.100:max:Максимум');
INSERT INTO "AnastKM102_prm" VALUES('ST8612','ST8612_4','Rotation speed of the compressor',1,'KM102_1.N:var:Variable
KM102_1.N:varIn:Variable input
*s.1000x rpm:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.15:max:Maximum','Оберти компресору','Обороты вращения компрессора','','KM102_1.N:var:Переменная
KM102_1.N:varIn:Ввод переменной
*s.1000x об.мин.:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.15:max:Максимум','','KM102_1.N:var:Змінна
KM102_1.N:varIn:Ввід змінної
*s.1000x об.хвил:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.15:max:Максимум');
INSERT INTO "AnastKM102_prm" VALUES('TE1313_1','TE1313_4','The temperature at the input of the first stage of the compressor',1,'node5_1.To:var:Variable
*s.К:ed:Dimension
*i.0:prec:Precision
*r.273:min:Minimum
*r.373:max:Maximum','Температура на вході першої ступені компресора','Температура на входе первой ступени компрессора','','node5_1.To:var:Переменная
*s.К:ed:Ед.измерения
*i.0:prec:Точность
*r.273:min:Минимум
*r.373:max:Максимум','','node5_1.To:var:Змінна
*s.К:ed:Од.виміру
*i.0:prec:Точність
*r.273:min:Мінімум
*r.373:max:Максимум');
INSERT INTO "AnastKM102_prm" VALUES('TE1314_1','TE1314_4','The temperature after the fridge of the first stage of the compressor',1,'AT102_1.To:var:Variable
*s.К:ed:Dimension
*i.0:prec:Precision
*r.273:min:Minimum
*r.373:max:Maximum','Температура після холодильника першої ступені компресора','Температура после холодильника первой ступени компрессора','','AT102_1.To:var:Переменная
*s.К:ed:Ед.измерения
*i.0:prec:Точность
*r.273:min:Минимум
*r.373:max:Максимум','','AT102_1.To:var:Змінна
*s.К:ed:Од.виміру
*i.0:prec:Точність
*r.273:min:Мінімум
*r.373:max:Максимум');
INSERT INTO "AnastKM102_prm" VALUES('КШ111','KSH111','The control block of the ball valve KSH111',1,'КШ111им.com:com
КШ111им.st_open:st_open
КШ111им.st_close:st_close','Блок керування кульовим краном КШ111','Блок управления шарового крана КШ111','КШ111','','КШ111','');
INSERT INTO "AnastKM102_prm" VALUES('КШ112','KSH112','The control block of the ball valve KSH112',1,'КШ112им.com:com
КШ112им.st_open:st_open
КШ112им.st_close:st_close','Блок керування кульовим краном КШ112','Блок управления шарового крана КШ112','KSH112','','КШ112','');
INSERT INTO "AnastKM102_prm" VALUES('КШ114','KSH114','The control block of the ball valve KSH114',1,'КШ114им.com:com
КШ114им.st_open:st_open
КШ114им.st_close:st_close','Блок керування кульовим краном КШ114','Блок управления шарового крана КШ114','КШ114','','КШ114','');
INSERT INTO "AnastKM102_prm" VALUES('КШ115','KSH115','The control block of the ball valve KSH115',1,'КШ115им.com:com
КШ115им.st_open:st_open
КШ115им.st_close:st_close','Блок керування кульовим краном КШ115','Блок управления шарового крана КШ115','КШ115','','КШ115','');
INSERT INTO "AnastKM102_prm" VALUES('КШ116','KSH116','The control block of the ball valve KSH116',1,'КШ116им.com:com
КШ116им.st_open:st_open
КШ116им.st_close:st_close','Блок керування кульовим краном КШ116','Блок управления шарового крана КШ116','КШ116','','КШ116','');
INSERT INTO "AnastKM102_prm" VALUES('P103','P103','Pressure on the diaphragm PP103',1,'PP103.Pi:var:Variable
*s.at:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.50:max:Maximum','Тиск на діафрагмі PP103','Давление на диафрагме PP103','','PP103.Pi:var:Переменная
*s.ат:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.50:max:Максимум','','PP103.Pi:var:Змінна
*s.ат:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.50:max:Максимум');
INSERT INTO "AnastKM102_prm" VALUES('PT1006_1','PT1006_4','Pressure on the diaphragm PP104',1,'PP104.Pi:var:Variable
*s.at:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.75:max:Maximum','Тиск на діафрагмі PP104','Давление на диафрагме PP104','','PP104.Pi:var:Переменная
*s.ат:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.75:max:Максимум','','PP104.Pi:var:Змінна
*s.ат:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.75:max:Максимум');
INSERT INTO "AnastKM102_prm" VALUES('TT0202','TE1202_4','Temperature after the first stage of the compressor',1,'KM102_1.To:var:Variable
*s.К:ed:Dimension
*i.0:prec:Precision
*r.273:min:Minimum
*r.373:max:Maximum','Температура після першої ступені компресора','Температура после первой ступени компрессора','','KM102_1.To:var:Переменная
*s.К:ed:Ед.измерения
*i.0:prec:Точность
*r.273:min:Минимум
*r.373:max:Максимум','','KM102_1.To:var:Змінна
*s.К:ed:Од.виміру
*i.0:prec:Точність
*r.273:min:Мінімум
*r.373:max:Максимум');
INSERT INTO "AnastKM102_prm" VALUES('TT0204','TE1205_4','Temperature after the second stage of the compressor',1,'KM102_2.To:var:Variable
*s.К:ed:Dimension
*i.0:prec:Precision
*r.273:min:Minimum
*r.423:max:Maximum','Температура після другої ступені компресора','Температура после второй ступени компрессора','','KM102_2.To:var:Переменная
*s.К:ed:Ед.измерения
*i.0:prec:Точность
*r.273:min:Минимум
*r.423:max:Максимум','','KM102_2.To:var:Змінна
*s.К:ed:Од.виміру
*i.0:prec:Точність
*r.273:min:Мінімум
*r.423:max:Максимум');
INSERT INTO "AnastKM102_prm" VALUES('TE1314_2','TE1206_4','The temperature after the fridge of the second stage of the compressor',1,'AT102_2.To:var:Variable
*s.К:ed:Dimension
*i.0:prec:Precision
*r.273:min:Minimum
*r.373:max:Maximum','Температура після холодильника другої ступені компресора','Температура после холодильника второй ступени компрессора','','AT102_2.To:var:Переменная
*s.К:ed:Ед.измерения
*i.0:prec:Точность
*r.273:min:Минимум
*r.373:max:Максимум','','AT102_2.To:var:Змінна
*s.К:ed:Од.виміру
*i.0:prec:Точність
*r.273:min:Мінімум
*r.373:max:Максимум');
INSERT INTO "AnastKM102_prm" VALUES('PT0202','PT0202_4','Pressure after the first stage of the compressor',1,'KM102_1.Po:var:Variable
*s.at:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.75:max:Maximum','Тиск після першої ступені компресору','Давление после первой ступени компрессора','','KM102_1.Po:var:Переменная
*s.ат:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.75:max:Максимум','','KM102_1.Po:var:Змінна
*s.ат:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.75:max:Максимум');
INSERT INTO "AnastKM102_prm" VALUES('PT0204','PT0204_4','Pressure after the second stage of the compressor',1,'KM102_2.Po:var:Variable
*s.at:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.150:max:Maximum','Тиск після другої ступені компресору','Давление после второй ступени компрессора','','KM102_2.Po:var:Переменная
*s.ат:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.150:max:Максимум','','KM102_2.Po:var:Змінна
*s.ат:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.150:max:Максимум');
CREATE TABLE 'AnastKM102cntr_blocks_io' ("BLK_ID" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"TLNK" INTEGER DEFAULT '' ,"LNK" TEXT DEFAULT '' ,"VAL" TEXT DEFAULT '' , PRIMARY KEY ("BLK_ID","ID"));
INSERT INTO "AnastKM102cntr_blocks_io" VALUES('КШ111','pos',6,'KM102.КШ111.l_kl1','100');
INSERT INTO "AnastKM102cntr_blocks_io" VALUES('КШ111','com',0,'','1');
INSERT INTO "AnastKM102cntr_blocks_io" VALUES('КШ111','st_open',0,'','1');
INSERT INTO "AnastKM102cntr_blocks_io" VALUES('КШ111','st_close',0,'','0');
INSERT INTO "AnastKM102cntr_blocks_io" VALUES('КШ111','t_full',0,'','5');
INSERT INTO "AnastKM102cntr_blocks_io" VALUES('КШ111','t_up',0,'','0.5');
INSERT INTO "AnastKM102cntr_blocks_io" VALUES('КШ111','f_frq',0,'','1');
INSERT INTO "AnastKM102cntr_blocks_io" VALUES('КШ111','tmp_up',0,'','0');
INSERT INTO "AnastKM102cntr_blocks_io" VALUES('КШ111','lst_com',0,'','1');
INSERT INTO "AnastKM102cntr_blocks_io" VALUES('КШ112','pos',6,'KM102.КШ111.l_kl2','0');
INSERT INTO "AnastKM102cntr_blocks_io" VALUES('КШ112','com',0,'','0');
INSERT INTO "AnastKM102cntr_blocks_io" VALUES('КШ112','st_open',0,'','0');
INSERT INTO "AnastKM102cntr_blocks_io" VALUES('КШ112','st_close',0,'','1');
INSERT INTO "AnastKM102cntr_blocks_io" VALUES('КШ112','t_full',0,'','5');
INSERT INTO "AnastKM102cntr_blocks_io" VALUES('КШ112','t_up',0,'','0.5');
INSERT INTO "AnastKM102cntr_blocks_io" VALUES('КШ112','f_frq',0,'','1');
INSERT INTO "AnastKM102cntr_blocks_io" VALUES('КШ112','tmp_up',0,'','0');
INSERT INTO "AnastKM102cntr_blocks_io" VALUES('КШ112','lst_com',0,'','0');
INSERT INTO "AnastKM102cntr_blocks_io" VALUES('КШ113','pos',6,'KM102.КШ113.l_kl1','0');
INSERT INTO "AnastKM102cntr_blocks_io" VALUES('КШ113','com',0,'','0');
INSERT INTO "AnastKM102cntr_blocks_io" VALUES('КШ113','st_open',0,'','0');
INSERT INTO "AnastKM102cntr_blocks_io" VALUES('КШ113','st_close',0,'','1');
INSERT INTO "AnastKM102cntr_blocks_io" VALUES('КШ113','t_full',0,'','5');
INSERT INTO "AnastKM102cntr_blocks_io" VALUES('КШ113','t_up',0,'','0.5');
INSERT INTO "AnastKM102cntr_blocks_io" VALUES('КШ113','f_frq',0,'','1');
INSERT INTO "AnastKM102cntr_blocks_io" VALUES('КШ113','tmp_up',0,'','0');
INSERT INTO "AnastKM102cntr_blocks_io" VALUES('КШ113','lst_com',0,'','0');
INSERT INTO "AnastKM102cntr_blocks_io" VALUES('КШ114','pos',6,'KM102.КШ114.l_kl1','100');
INSERT INTO "AnastKM102cntr_blocks_io" VALUES('КШ114','com',0,'','1');
INSERT INTO "AnastKM102cntr_blocks_io" VALUES('КШ114','st_open',0,'','1');
INSERT INTO "AnastKM102cntr_blocks_io" VALUES('КШ114','st_close',0,'','0');
INSERT INTO "AnastKM102cntr_blocks_io" VALUES('КШ114','t_full',0,'','5');
INSERT INTO "AnastKM102cntr_blocks_io" VALUES('КШ114','t_up',0,'','0.5');
INSERT INTO "AnastKM102cntr_blocks_io" VALUES('КШ114','f_frq',0,'','1');
INSERT INTO "AnastKM102cntr_blocks_io" VALUES('КШ114','tmp_up',0,'','0');
INSERT INTO "AnastKM102cntr_blocks_io" VALUES('КШ114','lst_com',0,'','1');
INSERT INTO "AnastKM102cntr_blocks_io" VALUES('КШ116','pos',6,'KM102.КШ116.l_kl1','0');
INSERT INTO "AnastKM102cntr_blocks_io" VALUES('КШ116','com',0,'','0');
INSERT INTO "AnastKM102cntr_blocks_io" VALUES('КШ116','st_open',0,'','0');
INSERT INTO "AnastKM102cntr_blocks_io" VALUES('КШ116','st_close',0,'','1');
INSERT INTO "AnastKM102cntr_blocks_io" VALUES('КШ116','t_full',0,'','5');
INSERT INTO "AnastKM102cntr_blocks_io" VALUES('КШ116','t_up',0,'','0.5');
INSERT INTO "AnastKM102cntr_blocks_io" VALUES('КШ116','f_frq',0,'','1');
INSERT INTO "AnastKM102cntr_blocks_io" VALUES('КШ116','tmp_up',0,'','0');
INSERT INTO "AnastKM102cntr_blocks_io" VALUES('КШ116','lst_com',0,'','0');
CREATE TABLE 'AnastKM102cntr_prm' ("SHIFR" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '' ,"IO" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"ru#IO" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"uk#IO" TEXT DEFAULT '' , PRIMARY KEY ("SHIFR"));
INSERT INTO "AnastKM102cntr_prm" VALUES('КШ111','KSH111','',1,'КШ111.com:com
КШ111.st_open:st_open
КШ111.st_close:st_close','КШ111','','','КШ111','','');
INSERT INTO "AnastKM102cntr_prm" VALUES('КШ112','KSH112','',1,'КШ112.com:com
КШ112.st_open:st_open
КШ112.st_close:st_close','KSH112','','','КШ112','','');
INSERT INTO "AnastKM102cntr_prm" VALUES('КШ113','KSH113','',1,'КШ113.com:com
КШ113.st_open:st_open
КШ113.st_close:st_close','КШ113','','','КШ113','','');
INSERT INTO "AnastKM102cntr_prm" VALUES('КШ114','KSH114','',1,'КШ114.com:com
КШ114.st_open:st_open
КШ114.st_close:st_close','КШ114','','','КШ114','','');
INSERT INTO "AnastKM102cntr_prm" VALUES('КШ116','KSH116','',1,'КШ116.com:com
КШ116.st_open:st_open
КШ116.st_close:st_close','КШ116','','','КШ116','','');
CREATE TABLE 'AnastKM201_blocks_io' ("BLK_ID" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"TLNK" INTEGER DEFAULT '' ,"LNK" TEXT DEFAULT '' ,"VAL" TEXT DEFAULT '' , PRIMARY KEY ("BLK_ID","ID"));
INSERT INTO "AnastKM201_blocks_io" VALUES('КМ101_1','Fi',0,'','50.2651712062611');
INSERT INTO "AnastKM201_blocks_io" VALUES('КМ101_1','Pi',1,'PP101.Po','5.39573641448191');
INSERT INTO "AnastKM201_blocks_io" VALUES('КМ101_1','Ti',0,'','273');
INSERT INTO "AnastKM201_blocks_io" VALUES('КМ101_1','Fo',1,'AT101_1.Fi','50.2605701804719');
INSERT INTO "AnastKM201_blocks_io" VALUES('КМ101_1','Po',0,'','10.3108303809281');
INSERT INTO "AnastKM201_blocks_io" VALUES('КМ101_1','To',0,'','331.535088116705');
INSERT INTO "AnastKM201_blocks_io" VALUES('КМ101_1','So',0,'','0.071');
INSERT INTO "AnastKM201_blocks_io" VALUES('КМ101_1','lo',0,'','55');
INSERT INTO "AnastKM201_blocks_io" VALUES('КМ101_1','Kzp',0,'','0.688780536365348');
INSERT INTO "AnastKM201_blocks_io" VALUES('КМ101_1','N',0,'','8.2');
INSERT INTO "AnastKM201_blocks_io" VALUES('КМ101_1','V',0,'','1.5');
INSERT INTO "AnastKM201_blocks_io" VALUES('КМ101_1','Kpmp',0,'','0.025');
INSERT INTO "AnastKM201_blocks_io" VALUES('КМ101_1','Kslp',0,'','0.2');
INSERT INTO "AnastKM201_blocks_io" VALUES('КМ101_1','Q0',0,'','1');
INSERT INTO "AnastKM201_blocks_io" VALUES('КМ101_1','Ct',0,'','100');
INSERT INTO "AnastKM201_blocks_io" VALUES('КМ101_1','Riz',0,'','100');
INSERT INTO "AnastKM201_blocks_io" VALUES('КМ101_1','Fwind',0,'','1');
INSERT INTO "AnastKM201_blocks_io" VALUES('КМ101_1','Twind',0,'','273');
INSERT INTO "AnastKM201_blocks_io" VALUES('КМ101_1','f_frq',0,'','100');
INSERT INTO "AnastKM201_blocks_io" VALUES('КМ101_1','Pot',0,'','10.9193045203657');
INSERT INTO "AnastKM201_blocks_io" VALUES('КМ101_1','Fit',0,'','50.2639927060112');
INSERT INTO "AnastKM201_blocks_io" VALUES('КМ101_2','Fi',0,'','49.7531797625764');
INSERT INTO "AnastKM201_blocks_io" VALUES('КМ101_2','Pi',1,'PP102.Po','9.83936938379023');
INSERT INTO "AnastKM201_blocks_io" VALUES('КМ101_2','Ti',1,'AT101_1.To','292.74691709864');
INSERT INTO "AnastKM201_blocks_io" VALUES('КМ101_2','Fo',1,'AT101_2.Fi','49.7479151873342');
INSERT INTO "AnastKM201_blocks_io" VALUES('КМ101_2','Po',0,'','25.9091794648836');
INSERT INTO "AnastKM201_blocks_io" VALUES('КМ101_2','To',0,'','391.376724288388');
INSERT INTO "AnastKM201_blocks_io" VALUES('КМ101_2','So',0,'','0.0314');
INSERT INTO "AnastKM201_blocks_io" VALUES('КМ101_2','lo',0,'','55');
INSERT INTO "AnastKM201_blocks_io" VALUES('КМ101_2','Kzp',0,'','0.885960569574326');
INSERT INTO "AnastKM201_blocks_io" VALUES('КМ101_2','N',1,'КМ101_1.N','8.2');
INSERT INTO "AnastKM201_blocks_io" VALUES('КМ101_2','V',0,'','1');
INSERT INTO "AnastKM201_blocks_io" VALUES('КМ101_2','Kpmp',0,'','0.035');
INSERT INTO "AnastKM201_blocks_io" VALUES('КМ101_2','Kslp',0,'','0.2');
INSERT INTO "AnastKM201_blocks_io" VALUES('КМ101_2','Q0',0,'','1');
INSERT INTO "AnastKM201_blocks_io" VALUES('КМ101_2','Ct',0,'','100');
INSERT INTO "AnastKM201_blocks_io" VALUES('КМ101_2','Riz',0,'','100');
INSERT INTO "AnastKM201_blocks_io" VALUES('КМ101_2','Fwind',0,'','1');
INSERT INTO "AnastKM201_blocks_io" VALUES('КМ101_2','Twind',0,'','273');
INSERT INTO "AnastKM201_blocks_io" VALUES('КМ101_2','f_frq',0,'','100');
INSERT INTO "AnastKM201_blocks_io" VALUES('КМ101_2','Pot',0,'','27.6535126272466');
INSERT INTO "AnastKM201_blocks_io" VALUES('КМ101_2','Fit',0,'','49.7519643769744');
INSERT INTO "AnastKM201_blocks_io" VALUES('KPP101','Fi',0,'','1.21070942880797e-249');
INSERT INTO "AnastKM201_blocks_io" VALUES('KPP101','Pi',1,'node4_1.Po2','25.3478336915815');
INSERT INTO "AnastKM201_blocks_io" VALUES('KPP101','Ti',1,'AT101_2.To','304.584705417242');
INSERT INTO "AnastKM201_blocks_io" VALUES('KPP101','Fo',1,'node3_1.Fi2','-8.34702718241759e-05');
INSERT INTO "AnastKM201_blocks_io" VALUES('KPP101','Po',0,'','5.48332604842023');
INSERT INTO "AnastKM201_blocks_io" VALUES('KPP101','To',0,'','277.235190560256');
INSERT INTO "AnastKM201_blocks_io" VALUES('KPP101','So',0,'','0.071');
INSERT INTO "AnastKM201_blocks_io" VALUES('KPP101','lo',0,'','10');
INSERT INTO "AnastKM201_blocks_io" VALUES('KPP101','S_kl1',0,'','0.04');
INSERT INTO "AnastKM201_blocks_io" VALUES('KPP101','S_kl2',0,'','0.001');
INSERT INTO "AnastKM201_blocks_io" VALUES('KPP101','l_kl1',2,'Anast1to2node_cntr.SurgeKM201.out','0');
INSERT INTO "AnastKM201_blocks_io" VALUES('KPP101','l_kl2',0,'','0');
INSERT INTO "AnastKM201_blocks_io" VALUES('KPP101','Q0',0,'','1');
INSERT INTO "AnastKM201_blocks_io" VALUES('KPP101','Kln',0,'','2');
INSERT INTO "AnastKM201_blocks_io" VALUES('KPP101','Kpr',0,'','0.95');
INSERT INTO "AnastKM201_blocks_io" VALUES('KPP101','Ct',0,'','20');
INSERT INTO "AnastKM201_blocks_io" VALUES('KPP101','Riz',0,'','20');
INSERT INTO "AnastKM201_blocks_io" VALUES('KPP101','noBack',0,'','0');
INSERT INTO "AnastKM201_blocks_io" VALUES('KPP101','Fwind',0,'','1');
INSERT INTO "AnastKM201_blocks_io" VALUES('KPP101','Twind',0,'','273');
INSERT INTO "AnastKM201_blocks_io" VALUES('KPP101','f_frq',0,'','100');
INSERT INTO "AnastKM201_blocks_io" VALUES('KPP101','Pot',0,'','5.23644114594017');
INSERT INTO "AnastKM201_blocks_io" VALUES('KPP101','Fit',0,'','2.13883580327003');
INSERT INTO "AnastKM201_blocks_io" VALUES('DR100','Fi',0,'','0.000630678288789624');
INSERT INTO "AnastKM201_blocks_io" VALUES('DR100','Pi',1,'КШ106.Po','5.48332680149852');
INSERT INTO "AnastKM201_blocks_io" VALUES('DR100','Ti',1,'КШ106.To','273');
INSERT INTO "AnastKM201_blocks_io" VALUES('DR100','Fo',1,'node3_1.Fi1','-0.000367995867471367');
INSERT INTO "AnastKM201_blocks_io" VALUES('DR100','Po',0,'','5.48334909747733');
INSERT INTO "AnastKM201_blocks_io" VALUES('DR100','To',0,'','272.999999953489');
INSERT INTO "AnastKM201_blocks_io" VALUES('DR100','So',0,'','0.2');
INSERT INTO "AnastKM201_blocks_io" VALUES('DR100','lo',0,'','1');
INSERT INTO "AnastKM201_blocks_io" VALUES('DR100','S_kl1',0,'','0.05');
INSERT INTO "AnastKM201_blocks_io" VALUES('DR100','S_kl2',0,'','0.01');
INSERT INTO "AnastKM201_blocks_io" VALUES('DR100','l_kl1',0,'','50');
INSERT INTO "AnastKM201_blocks_io" VALUES('DR100','l_kl2',0,'','0');
INSERT INTO "AnastKM201_blocks_io" VALUES('DR100','Q0',0,'','1');
INSERT INTO "AnastKM201_blocks_io" VALUES('DR100','Kln',0,'','1');
INSERT INTO "AnastKM201_blocks_io" VALUES('DR100','Kpr',0,'','0.95');
INSERT INTO "AnastKM201_blocks_io" VALUES('DR100','Ct',0,'','20');
INSERT INTO "AnastKM201_blocks_io" VALUES('DR100','Riz',0,'','20');
INSERT INTO "AnastKM201_blocks_io" VALUES('DR100','noBack',0,'','0');
INSERT INTO "AnastKM201_blocks_io" VALUES('DR100','Fwind',0,'','1');
INSERT INTO "AnastKM201_blocks_io" VALUES('DR100','Twind',0,'','273');
INSERT INTO "AnastKM201_blocks_io" VALUES('DR100','f_frq',0,'','100');
INSERT INTO "AnastKM201_blocks_io" VALUES('DR100','Pot',0,'','4.83622293773731');
INSERT INTO "AnastKM201_blocks_io" VALUES('DR100','Fit',0,'','16.9179842059563');
INSERT INTO "AnastKM201_blocks_io" VALUES('AT101_1','Fi',0,'','50.2604638341459');
INSERT INTO "AnastKM201_blocks_io" VALUES('AT101_1','Pi',1,'КМ101_1.Po','10.3108035203983');
INSERT INTO "AnastKM201_blocks_io" VALUES('AT101_1','Ti',1,'КМ101_1.To','331.535115052422');
INSERT INTO "AnastKM201_blocks_io" VALUES('AT101_1','Si',0,'','0.065');
INSERT INTO "AnastKM201_blocks_io" VALUES('AT101_1','li',0,'','10');
INSERT INTO "AnastKM201_blocks_io" VALUES('AT101_1','Fo',1,'C101_2.Fi','50.2602983717175');
INSERT INTO "AnastKM201_blocks_io" VALUES('AT101_1','Po',0,'','10.1575450825393');
INSERT INTO "AnastKM201_blocks_io" VALUES('AT101_1','To',0,'','292.746905178066');
INSERT INTO "AnastKM201_blocks_io" VALUES('AT101_1','So',0,'','0.071');
INSERT INTO "AnastKM201_blocks_io" VALUES('AT101_1','lo',0,'','2');
INSERT INTO "AnastKM201_blocks_io" VALUES('AT101_1','Tair',0,'','283');
INSERT INTO "AnastKM201_blocks_io" VALUES('AT101_1','Wc',0,'','200');
INSERT INTO "AnastKM201_blocks_io" VALUES('AT101_1','Q0',0,'','1');
INSERT INTO "AnastKM201_blocks_io" VALUES('AT101_1','Ct',0,'','100');
INSERT INTO "AnastKM201_blocks_io" VALUES('AT101_1','Rt',0,'','1');
INSERT INTO "AnastKM201_blocks_io" VALUES('AT101_1','f_frq',0,'','100');
INSERT INTO "AnastKM201_blocks_io" VALUES('AT101_1','Pot',0,'','10.5823391227739');
INSERT INTO "AnastKM201_blocks_io" VALUES('AT101_1','Fit',0,'','37.7399346381534');
INSERT INTO "AnastKM201_blocks_io" VALUES('AT101_2','Fi',0,'','49.7478025209357');
INSERT INTO "AnastKM201_blocks_io" VALUES('AT101_2','Pi',1,'КМ101_2.Po','25.9091088279997');
INSERT INTO "AnastKM201_blocks_io" VALUES('AT101_2','Ti',1,'КМ101_2.To','391.376737682757');
INSERT INTO "AnastKM201_blocks_io" VALUES('AT101_2','Si',0,'','0.029');
INSERT INTO "AnastKM201_blocks_io" VALUES('AT101_2','li',0,'','20');
INSERT INTO "AnastKM201_blocks_io" VALUES('AT101_2','Fo',1,'C101_3.Fi','49.7470473139652');
INSERT INTO "AnastKM201_blocks_io" VALUES('AT101_2','Po',0,'','25.6080064579327');
INSERT INTO "AnastKM201_blocks_io" VALUES('AT101_2','To',0,'','304.584697034865');
INSERT INTO "AnastKM201_blocks_io" VALUES('AT101_2','So',0,'','0.0314');
INSERT INTO "AnastKM201_blocks_io" VALUES('AT101_2','lo',0,'','5');
INSERT INTO "AnastKM201_blocks_io" VALUES('AT101_2','Tair',0,'','283');
INSERT INTO "AnastKM201_blocks_io" VALUES('AT101_2','Wc',0,'','200');
INSERT INTO "AnastKM201_blocks_io" VALUES('AT101_2','Q0',0,'','1');
INSERT INTO "AnastKM201_blocks_io" VALUES('AT101_2','Ct',0,'','100');
INSERT INTO "AnastKM201_blocks_io" VALUES('AT101_2','Rt',0,'','1');
INSERT INTO "AnastKM201_blocks_io" VALUES('AT101_2','f_frq',0,'','100');
INSERT INTO "AnastKM201_blocks_io" VALUES('AT101_2','Pot',0,'','26.8981088252344');
INSERT INTO "AnastKM201_blocks_io" VALUES('AT101_2','Fit',0,'','37.3649946813934');
INSERT INTO "AnastKM201_blocks_io" VALUES('node3_1','Fi1',0,'','-0.000900313061972566');
INSERT INTO "AnastKM201_blocks_io" VALUES('node3_1','Pi1',1,'DR100.Po','5.48331069426711');
INSERT INTO "AnastKM201_blocks_io" VALUES('node3_1','Ti1',1,'DR100.To','272.999999953441');
INSERT INTO "AnastKM201_blocks_io" VALUES('node3_1','Si1',0,'','0.2');
INSERT INTO "AnastKM201_blocks_io" VALUES('node3_1','Fi2',0,'','-0.000251888032392333');
INSERT INTO "AnastKM201_blocks_io" VALUES('node3_1','Pi2',1,'KPP101.Po','5.48332735589044');
INSERT INTO "AnastKM201_blocks_io" VALUES('node3_1','Ti2',1,'KPP101.To','277.23516584023');
INSERT INTO "AnastKM201_blocks_io" VALUES('node3_1','Si2',0,'','0.071');
INSERT INTO "AnastKM201_blocks_io" VALUES('node3_1','Fi3',0,'','50.7857929747614');
INSERT INTO "AnastKM201_blocks_io" VALUES('node3_1','Pi3',1,'КШ101.Po','5.5166693968094');
INSERT INTO "AnastKM201_blocks_io" VALUES('node3_1','Ti3',1,'КШ101.To','290.421667387493');
INSERT INTO "AnastKM201_blocks_io" VALUES('node3_1','Si3',0,'','0.2');
INSERT INTO "AnastKM201_blocks_io" VALUES('node3_1','Fo',1,'C101_1.Fi','50.7788005987103');
INSERT INTO "AnastKM201_blocks_io" VALUES('node3_1','Po',0,'','5.48333790610222');
INSERT INTO "AnastKM201_blocks_io" VALUES('node3_1','To',0,'','290.384429350961');
INSERT INTO "AnastKM201_blocks_io" VALUES('node3_1','So',0,'','0.2');
INSERT INTO "AnastKM201_blocks_io" VALUES('node3_1','lo',0,'','50');
INSERT INTO "AnastKM201_blocks_io" VALUES('node3_1','Q0',0,'','1');
INSERT INTO "AnastKM201_blocks_io" VALUES('node3_1','Ct',0,'','20');
INSERT INTO "AnastKM201_blocks_io" VALUES('node3_1','Riz',0,'','20');
INSERT INTO "AnastKM201_blocks_io" VALUES('node3_1','Fwind',0,'','1');
INSERT INTO "AnastKM201_blocks_io" VALUES('node3_1','Twind',0,'','273');
INSERT INTO "AnastKM201_blocks_io" VALUES('node3_1','f_frq',0,'','100');
INSERT INTO "AnastKM201_blocks_io" VALUES('node3_1','FoR',0,'','0.145506496403985');
INSERT INTO "AnastKM201_blocks_io" VALUES('node3_1','Pot',0,'','5.23491609433684');
INSERT INTO "AnastKM201_blocks_io" VALUES('node3_1','Fit',0,'','37.1522845875593');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ101','Fi',0,'','50.7864736708611');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ101','Pi',0,'','5.54746933904396');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ101','Ti',2,'Anast1to2node.КРД1.To','290.489653277088');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ101','Fo',1,'node3_1.Fi3','50.7858407656043');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ101','Po',0,'','5.51667655763568');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ101','To',0,'','290.421692774963');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ101','So',0,'','0.2');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ101','lo',0,'','5');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ101','S_kl1',0,'','0.2');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ101','S_kl2',0,'','0.05');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ101','l_kl1',1,'КШ101им.pos','100');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ101','l_kl2',1,'КШ102им.pos','0');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ101','Q0',0,'','1');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ101','Kln',0,'','1');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ101','Kpr',0,'','0.95');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ101','Ct',0,'','20');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ101','Riz',0,'','20');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ101','noBack',0,'','0');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ101','Fwind',0,'','1');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ101','Twind',0,'','273');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ101','f_frq',0,'','100');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ101','Pot',0,'','5.29091023245053');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ101','Fit',0,'','36.0508441505443');
INSERT INTO "AnastKM201_blocks_io" VALUES('node4_1','Fi',0,'','49.2497091967973');
INSERT INTO "AnastKM201_blocks_io" VALUES('node4_1','Pi',1,'C101_3.Po','25.3478877311342');
INSERT INTO "AnastKM201_blocks_io" VALUES('node4_1','Fo1',1,'КШ106.Fi','0');
INSERT INTO "AnastKM201_blocks_io" VALUES('node4_1','Po1',0,'','25.3478791085186');
INSERT INTO "AnastKM201_blocks_io" VALUES('node4_1','So1',0,'','0.0314');
INSERT INTO "AnastKM201_blocks_io" VALUES('node4_1','lo1',0,'','5');
INSERT INTO "AnastKM201_blocks_io" VALUES('node4_1','Fo2',1,'KPP101.Fi','1.20708093143912e-249');
INSERT INTO "AnastKM201_blocks_io" VALUES('node4_1','Po2',0,'','25.3479166550688');
INSERT INTO "AnastKM201_blocks_io" VALUES('node4_1','So2',0,'','0.0177');
INSERT INTO "AnastKM201_blocks_io" VALUES('node4_1','lo2',0,'','5');
INSERT INTO "AnastKM201_blocks_io" VALUES('node4_1','Fo3',1,'КШ104.Fi','49.2478687062924');
INSERT INTO "AnastKM201_blocks_io" VALUES('node4_1','Po3',0,'','25.0905766066671');
INSERT INTO "AnastKM201_blocks_io" VALUES('node4_1','So3',0,'','0.0314');
INSERT INTO "AnastKM201_blocks_io" VALUES('node4_1','lo3',0,'','5');
INSERT INTO "AnastKM201_blocks_io" VALUES('node4_1','Q0',0,'','1');
INSERT INTO "AnastKM201_blocks_io" VALUES('node4_1','f_frq',0,'','100');
INSERT INTO "AnastKM201_blocks_io" VALUES('node4_1','F1tmp',0,'','-0.00065315929392597');
INSERT INTO "AnastKM201_blocks_io" VALUES('node4_1','F2tmp',0,'','0.00170116192124925');
INSERT INTO "AnastKM201_blocks_io" VALUES('node4_1','F3tmp',0,'','49.24866119417');
INSERT INTO "AnastKM201_blocks_io" VALUES('node4_1','Pot1',0,'','25.7971704231136');
INSERT INTO "AnastKM201_blocks_io" VALUES('node4_1','Pot2',0,'','27.0285722141214');
INSERT INTO "AnastKM201_blocks_io" VALUES('node4_1','Pot3',0,'','25.9924639237477');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ104','Fi',0,'','49.2479363638736');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ104','Pi',1,'node4_1.Po3','25.0905976101168');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ104','Ti',1,'AT101_2.To','304.584730874794');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ104','Fo',0,'','49.2388656056117');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ104','Po',0,'','24.7956827284695');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ104','To',0,'','304.44672923253');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ104','So',0,'','0.03');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ104','lo',0,'','100');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ104','S_kl1',0,'','0.03');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ104','S_kl2',0,'','0.01');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ104','l_kl1',1,'КШ104им.pos','100');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ104','l_kl2',1,'КШ105им.pos','0');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ104','Q0',0,'','1');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ104','Kln',0,'','1');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ104','Kpr',0,'','0.95');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ104','Ct',0,'','20');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ104','Riz',0,'','20');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ104','noBack',0,'','1');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ104','Fwind',0,'','1');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ104','Twind',0,'','273');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ104','f_frq',0,'','100');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ104','Pot',0,'','25.4836458871784');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ104','Fit',0,'','34.9108817224733');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ106','Fi',0,'','0');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ106','Pi',1,'node4_1.Po1','25.3480301791079');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ106','Ti',1,'AT101_2.To','304.584781579323');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ106','Fo',1,'DR100.Fi','0.00236720336950879');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ106','Po',0,'','5.48335139613992');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ106','To',0,'','273');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ106','So',0,'','0.0314');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ106','lo',0,'','10');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ106','S_kl1',0,'','0.0314');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ106','S_kl2',0,'','0.01');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ106','l_kl1',1,'КШ106им.pos','0');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ106','l_kl2',0,'','0');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ106','Q0',0,'','1');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ106','Kln',0,'','1');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ106','Kpr',0,'','0.95');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ106','Ct',0,'','20');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ106','Riz',0,'','20');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ106','noBack',0,'','0');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ106','Fwind',0,'','1');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ106','Twind',0,'','273');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ106','f_frq',0,'','100');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ106','Pot',0,'','4.85858532034619');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ106','Fit',0,'','0');
INSERT INTO "AnastKM201_blocks_io" VALUES('C101_1','Fi',0,'','50.7787130243433');
INSERT INTO "AnastKM201_blocks_io" VALUES('C101_1','Pi',1,'node3_1.Po','5.48332896895333');
INSERT INTO "AnastKM201_blocks_io" VALUES('C101_1','Si',0,'','0.2');
INSERT INTO "AnastKM201_blocks_io" VALUES('C101_1','Fo',1,'PP101.Fi','50.2659231590972');
INSERT INTO "AnastKM201_blocks_io" VALUES('C101_1','Po',0,'','5.45032982711861');
INSERT INTO "AnastKM201_blocks_io" VALUES('C101_1','So',0,'','0.2');
INSERT INTO "AnastKM201_blocks_io" VALUES('C101_1','lo',0,'','40');
INSERT INTO "AnastKM201_blocks_io" VALUES('C101_1','Fo_ж',0,'','0');
INSERT INTO "AnastKM201_blocks_io" VALUES('C101_1','Po_ж',0,'','5.49229727205756');
INSERT INTO "AnastKM201_blocks_io" VALUES('C101_1','Lж',0,'','4.41762578304743');
INSERT INTO "AnastKM201_blocks_io" VALUES('C101_1','ProcЖ',0,'','0.01');
INSERT INTO "AnastKM201_blocks_io" VALUES('C101_1','Vap',0,'','9.5');
INSERT INTO "AnastKM201_blocks_io" VALUES('C101_1','Q0',0,'','1');
INSERT INTO "AnastKM201_blocks_io" VALUES('C101_1','Qж',0,'','1000');
INSERT INTO "AnastKM201_blocks_io" VALUES('C101_1','f_frq',0,'','100');
INSERT INTO "AnastKM201_blocks_io" VALUES('C101_1','Pot',0,'','5.1709458096161');
INSERT INTO "AnastKM201_blocks_io" VALUES('C101_1','Fit',0,'','37.7441933839702');
INSERT INTO "AnastKM201_blocks_io" VALUES('C101_2','Fi',0,'','50.2603286954977');
INSERT INTO "AnastKM201_blocks_io" VALUES('C101_2','Pi',1,'AT101_1.Po','10.1575483950333');
INSERT INTO "AnastKM201_blocks_io" VALUES('C101_2','Si',0,'','0.071');
INSERT INTO "AnastKM201_blocks_io" VALUES('C101_2','Fo',1,'PP102.Fi','49.75362876469');
INSERT INTO "AnastKM201_blocks_io" VALUES('C101_2','Po',0,'','10.0213665183752');
INSERT INTO "AnastKM201_blocks_io" VALUES('C101_2','So',0,'','0.071');
INSERT INTO "AnastKM201_blocks_io" VALUES('C101_2','lo',0,'','50');
INSERT INTO "AnastKM201_blocks_io" VALUES('C101_2','Fo_ж',0,'','0');
INSERT INTO "AnastKM201_blocks_io" VALUES('C101_2','Po_ж',0,'','10.0624567429181');
INSERT INTO "AnastKM201_blocks_io" VALUES('C101_2','Lж',0,'','4.32494150177887');
INSERT INTO "AnastKM201_blocks_io" VALUES('C101_2','ProcЖ',0,'','0.01');
INSERT INTO "AnastKM201_blocks_io" VALUES('C101_2','Vap',0,'','9.5');
INSERT INTO "AnastKM201_blocks_io" VALUES('C101_2','Q0',0,'','1');
INSERT INTO "AnastKM201_blocks_io" VALUES('C101_2','Qж',0,'','1000');
INSERT INTO "AnastKM201_blocks_io" VALUES('C101_2','f_frq',0,'','100');
INSERT INTO "AnastKM201_blocks_io" VALUES('C101_2','Pot',0,'','10.3349732470663');
INSERT INTO "AnastKM201_blocks_io" VALUES('C101_2','Fit',0,'','37.3626857881806');
INSERT INTO "AnastKM201_blocks_io" VALUES('C101_3','Fi',0,'','49.7474382426755');
INSERT INTO "AnastKM201_blocks_io" VALUES('C101_3','Pi',1,'AT101_2.Po','25.6080241499032');
INSERT INTO "AnastKM201_blocks_io" VALUES('C101_3','Si',0,'','0.0314');
INSERT INTO "AnastKM201_blocks_io" VALUES('C101_3','Fo',1,'node4_1.Fi','49.2486511296297');
INSERT INTO "AnastKM201_blocks_io" VALUES('C101_3','Po',0,'','25.3478452449654');
INSERT INTO "AnastKM201_blocks_io" VALUES('C101_3','So',0,'','0.0314');
INSERT INTO "AnastKM201_blocks_io" VALUES('C101_3','lo',0,'','8');
INSERT INTO "AnastKM201_blocks_io" VALUES('C101_3','Fo_ж',0,'','0');
INSERT INTO "AnastKM201_blocks_io" VALUES('C101_3','Po_ж',0,'','25.3874671339256');
INSERT INTO "AnastKM201_blocks_io" VALUES('C101_3','Lж',0,'','13.2072963200857');
INSERT INTO "AnastKM201_blocks_io" VALUES('C101_3','ProcЖ',0,'','0.01');
INSERT INTO "AnastKM201_blocks_io" VALUES('C101_3','Vap',0,'','3');
INSERT INTO "AnastKM201_blocks_io" VALUES('C101_3','Q0',0,'','1');
INSERT INTO "AnastKM201_blocks_io" VALUES('C101_3','Qж',0,'','1000');
INSERT INTO "AnastKM201_blocks_io" VALUES('C101_3','f_frq',0,'','100');
INSERT INTO "AnastKM201_blocks_io" VALUES('C101_3','Pot',0,'','26.3987476146491');
INSERT INTO "AnastKM201_blocks_io" VALUES('C101_3','Fit',0,'','37.3023338053042');
INSERT INTO "AnastKM201_blocks_io" VALUES('PP101','Fi',0,'','50.2659942349801');
INSERT INTO "AnastKM201_blocks_io" VALUES('PP101','Pi',1,'C101_1.Po','5.45033516606439');
INSERT INTO "AnastKM201_blocks_io" VALUES('PP101','Fo',1,'КМ101_1.Fi','50.2651219855439');
INSERT INTO "AnastKM201_blocks_io" VALUES('PP101','Po',0,'','5.39573286875633');
INSERT INTO "AnastKM201_blocks_io" VALUES('PP101','dP',0,'','5.46005909362743');
INSERT INTO "AnastKM201_blocks_io" VALUES('PP101','Sdf',0,'','0.15');
INSERT INTO "AnastKM201_blocks_io" VALUES('PP101','So',0,'','0.2');
INSERT INTO "AnastKM201_blocks_io" VALUES('PP101','lo',0,'','7');
INSERT INTO "AnastKM201_blocks_io" VALUES('PP101','Q0',0,'','1');
INSERT INTO "AnastKM201_blocks_io" VALUES('PP101','f_frq',0,'','100');
INSERT INTO "AnastKM201_blocks_io" VALUES('PP101','Pot',0,'','5.05761619736924');
INSERT INTO "AnastKM201_blocks_io" VALUES('PP101','Fit',0,'','37.7386147480736');
INSERT INTO "AnastKM201_blocks_io" VALUES('PP102','Fi',0,'','49.7536872076661');
INSERT INTO "AnastKM201_blocks_io" VALUES('PP102','Pi',1,'C101_2.Po','10.0213763601967');
INSERT INTO "AnastKM201_blocks_io" VALUES('PP102','Fo',1,'КМ101_2.Fi','49.7531361040953');
INSERT INTO "AnastKM201_blocks_io" VALUES('PP102','Po',0,'','9.83936290180272');
INSERT INTO "AnastKM201_blocks_io" VALUES('PP102','dP',0,'','18.2011994098775');
INSERT INTO "AnastKM201_blocks_io" VALUES('PP102','Sdf',0,'','0.06');
INSERT INTO "AnastKM201_blocks_io" VALUES('PP102','So',0,'','0.071');
INSERT INTO "AnastKM201_blocks_io" VALUES('PP102','lo',0,'','7');
INSERT INTO "AnastKM201_blocks_io" VALUES('PP102','Q0',0,'','1');
INSERT INTO "AnastKM201_blocks_io" VALUES('PP102','f_frq',0,'','100');
INSERT INTO "AnastKM201_blocks_io" VALUES('PP102','Pot',0,'','9.98549407979398');
INSERT INTO "AnastKM201_blocks_io" VALUES('PP102','Fit',0,'','37.3637648351936');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ104им','pos',0,'','100');
INSERT INTO "AnastKM201_blocks_io" VALUES('КМ101_1','Kpr',0,'','0.95');
INSERT INTO "AnastKM201_blocks_io" VALUES('КМ101_2','Kpr',0,'','0.95');
INSERT INTO "AnastKM201_blocks_io" VALUES('KPP101','t_kl1',0,'','3');
INSERT INTO "AnastKM201_blocks_io" VALUES('KPP101','t_kl2',0,'','5');
INSERT INTO "AnastKM201_blocks_io" VALUES('KPP101','tmp_l1',0,'','7.4109846876187e-322');
INSERT INTO "AnastKM201_blocks_io" VALUES('KPP101','tmp_l2',0,'','0');
INSERT INTO "AnastKM201_blocks_io" VALUES('DR100','t_kl1',0,'','10');
INSERT INTO "AnastKM201_blocks_io" VALUES('DR100','t_kl2',0,'','5');
INSERT INTO "AnastKM201_blocks_io" VALUES('DR100','tmp_l1',0,'','49.9999999999964');
INSERT INTO "AnastKM201_blocks_io" VALUES('DR100','tmp_l2',0,'','0');
INSERT INTO "AnastKM201_blocks_io" VALUES('node3_1','Kpr',0,'','0.95');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ101','t_kl1',0,'','10');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ101','t_kl2',0,'','5');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ101','tmp_l1',0,'','99.9999999999929');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ101','tmp_l2',0,'','1.23516411460312e-321');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ104','t_kl1',0,'','10');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ104','t_kl2',0,'','5');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ104','tmp_l1',0,'','99.9999999999929');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ104','tmp_l2',0,'','0');
INSERT INTO "AnastKM201_blocks_io" VALUES('node4_1','Kpr',0,'','0.95');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ106','t_kl1',0,'','10');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ106','t_kl2',0,'','5');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ106','tmp_l1',0,'','0');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ106','tmp_l2',0,'','0');
INSERT INTO "AnastKM201_blocks_io" VALUES('PP101','Kpr',0,'','0.95');
INSERT INTO "AnastKM201_blocks_io" VALUES('PP102','Kpr',0,'','0.95');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ104им','com',0,'','1');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ104им','st_open',0,'','1');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ104им','st_close',0,'','0');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ104им','t_full',0,'','5');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ104им','t_up',0,'','0.5');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ104им','f_frq',0,'','100');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ104им','tmp_up',0,'','0');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ104им','lst_com',0,'','1');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ105им','pos',0,'','0');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ105им','com',0,'','0');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ105им','st_open',0,'','0');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ105им','st_close',0,'','1');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ105им','t_full',0,'','5');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ105им','t_up',0,'','0.5');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ105им','f_frq',0,'','100');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ105им','tmp_up',0,'','0');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ105им','lst_com',0,'','0');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ101им','pos',0,'','100');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ101им','com',0,'','1');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ101им','st_open',0,'','1');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ101им','st_close',0,'','0');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ101им','t_full',0,'','5');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ101им','t_up',0,'','0.5');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ101им','f_frq',0,'','100');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ101им','tmp_up',0,'','0');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ101им','lst_com',0,'','1');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ106им','pos',0,'','0');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ106им','com',0,'','0');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ106им','st_open',0,'','0');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ106им','st_close',0,'','1');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ106им','t_full',0,'','5');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ106им','t_up',0,'','0.5');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ106им','f_frq',0,'','100');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ106им','tmp_up',0,'','0');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ106им','lst_com',0,'','0');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ102им','pos',0,'','0');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ102им','com',0,'','0');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ102им','st_open',0,'','0');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ102им','st_close',0,'','1');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ102им','t_full',0,'','3');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ102им','t_up',0,'','0.3');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ102им','f_frq',0,'','100');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ102им','tmp_up',0,'','0');
INSERT INTO "AnastKM201_blocks_io" VALUES('КШ102им','lst_com',0,'','0');
CREATE TABLE 'AnastKM201_prm' ("SHIFR" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '' ,"IO" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"ru#IO" TEXT DEFAULT '' ,"uk#IO" TEXT DEFAULT '' , PRIMARY KEY ("SHIFR"));
INSERT INTO "AnastKM201_prm" VALUES('FN101','FN201','Flow at the output of compressor',1,'КШ104.Fi:var:Variable','','Витрати на виході компресора','','Расход на выходе компрессора','КШ104.Fi:var:Переменная','КШ104.Fi:var:Змінна');
INSERT INTO "AnastKM201_prm" VALUES('F101','F201','Flow on the diaphragm PP201',1,'PP101.Fi:var:Variable
*s.tone/h:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.100:max:Maximum','','Витрати на діафрагмі PP201','','Расход на диафрагме PP201','PP101.Fi:var:Переменная
*s.т/ч:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.100:max:Максимум','PP101.Fi:var:Змінна
*s.т/год:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.100:max:Максимум');
INSERT INTO "AnastKM201_prm" VALUES('F102','F202','Flow on the diaphragm PP202',1,'PP102.Fi:var:Variable
*s.tone/h:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.100:max:Maximum','','Витрати на діафрагмі PP202','','Расход на диафрагме PP202','PP102.Fi:var:Переменная
*s.т/ч:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.100:max:Максимум','PP102.Fi:var:Змінна
*s.т/год:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.100:max:Максимум');
INSERT INTO "AnastKM201_prm" VALUES('TE1313_1','TE1313_2','The temperature at the input of the first stage of the compressor',1,'node3_1.To:var:Variable
*s.К:ed:Dimension
*i.0:prec:Precision
*r.273:min:Minimum
*r.373:max:Maximum','','Температура на вході першої ступені компресора','','Температура на входе первой ступени компрессора','node3_1.To:var:Переменная
*s.К:ed:Ед.измерения
*i.0:prec:Точность
*r.273:min:Минимум
*r.373:max:Максимум','node3_1.To:var:Змінна
*s.К:ed:Од.виміру
*i.0:prec:Точність
*r.273:min:Мінімум
*r.373:max:Максимум');
INSERT INTO "AnastKM201_prm" VALUES('PT0202','PT0202_2','Pressure after the first stage of the compressor',1,'КМ101_1.Po:var:Variable
*s.at:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.20:max:Maximum','','Тиск після першої ступені компресору','','Давление после первой ступени компрессора','КМ101_1.Po:var:Переменная
*s.ат:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.20:max:Максимум','КМ101_1.Po:var:Змінна
*s.ат:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.20:max:Максимум');
INSERT INTO "AnastKM201_prm" VALUES('PT0204','PT0204_2','Pressure after the second stage of the compressor',1,'КМ101_2.Po:var:Variable
*s.at:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.50:max:Maximum','','Тиск після другої ступені компресору','','Давление после второй ступени компрессора','КМ101_2.Po:var:Переменная
*s.ат:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.50:max:Максимум','КМ101_2.Po:var:Змінна
*s.ат:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.50:max:Максимум');
INSERT INTO "AnastKM201_prm" VALUES('TE1314_1','TE1314_2','The temperature after the fridge of the first stage of the compressor',1,'AT101_1.To:var:Variable
*s.К:ed:Dimension
*i.0:prec:Precision
*r.273:min:Minimum
*r.373:max:Maximum','','Температура після холодильника першої ступені компресора','','Температура после холодильника первой ступени компрессора','AT101_1.To:var:Переменная
*s.К:ed:Ед.измерения
*i.0:prec:Точность
*r.273:min:Минимум
*r.373:max:Максимум','AT101_1.To:var:Змінна
*s.К:ed:Од.виміру
*i.0:prec:Точність
*r.273:min:Мінімум
*r.373:max:Максимум');
INSERT INTO "AnastKM201_prm" VALUES('PT1006_1','PT1006_2','Pressure on the diaphragm PP202',1,'PP102.Pi:var:Variable
*s.at:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.20:max:Maximum','','Тиск на діафрагмі PP202','','Давление на диафрагме PP202','PP102.Pi:var:Переменная
*s.ат:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.20:max:Максимум','PP102.Pi:var:Змінна
*s.ат:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.20:max:Максимум');
INSERT INTO "AnastKM201_prm" VALUES('ST8612','ST8612_2','Rotation speed of the compressor',1,'КМ101_1.N:var:Variable
КМ101_1.N:varIn:Variable input
*s.1000x rpm:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.10:max:Maximum','','Оберти компресору','','Обороты вращения компрессора','КМ101_1.N:var:Переменная
КМ101_1.N:varIn:Ввод переменной
*s.1000x об.мин.:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.10:max:Максимум','КМ101_1.N:var:Змінна
КМ101_1.N:varIn:Ввід змінної
*s.1000x об.хвил:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.10:max:Максимум');
INSERT INTO "AnastKM201_prm" VALUES('KPP101','KPP201','Anti-surge control valve of the compressor',1,'KPP101.l_kl1:out:Output
KPP101.l_kl1:var:Variable
KPP101.l_kl1:varIn:Variable input
*s.%:ed:Dimension
*i.0:prec:Precision
*r.0:min:Minimum
*r.100:max:Maximum','','Антипомпажний регулюючий клапан компресору','','Антипомпажный регулирующий клапан компрессора','KPP101.l_kl1:out:Выход
KPP101.l_kl1:var:Переменная
KPP101.l_kl1:varIn:Ввод переменной
*s.%:ed:Ед.измерения
*i.0:prec:Точность
*r.0:min:Минимум
*r.100:max:Максимум','KPP101.l_kl1:out:Вихід
KPP101.l_kl1:var:Змінна
KPP101.l_kl1:varIn:Ввід змінної
*s.%:ed:Од.виміру
*i.0:prec:Точність
*r.0:min:Мінімум
*r.100:max:Максимум');
INSERT INTO "AnastKM201_prm" VALUES('TT0202','TE1202_2','Temperature after the first stage of the compressor',1,'КМ101_1.To:var:Variable
*s.К:ed:Dimension
*i.0:prec:Precision
*r.273:min:Minimum
*r.373:max:Maximum','','Температура після першої ступені компресора','','Температура после первой ступени компрессора','КМ101_1.To:var:Переменная
*s.К:ed:Ед.измерения
*i.0:prec:Точность
*r.273:min:Минимум
*r.373:max:Максимум','КМ101_1.To:var:Змінна
*s.К:ed:Од.виміру
*i.0:prec:Точність
*r.273:min:Мінімум
*r.373:max:Максимум');
INSERT INTO "AnastKM201_prm" VALUES('TT0204','TE1205_2','Temperature after the second stage of the compressor',1,'КМ101_2.To:var:Variable
*s.К:ed:Dimension
*i.0:prec:Precision
*r.273:min:Minimum
*r.433:max:Maximum','','Температура після другої ступені компресора','','Температура после второй ступени компрессора','КМ101_2.To:var:Переменная
*s.К:ed:Ед.измерения
*i.0:prec:Точность
*r.273:min:Минимум
*r.433:max:Максимум','КМ101_2.To:var:Змінна
*s.К:ed:Од.виміру
*i.0:prec:Точність
*r.273:min:Мінімум
*r.433:max:Максимум');
INSERT INTO "AnastKM201_prm" VALUES('AT101_1','AT201_1','Control block of the fridge after the first stage of the compressor',1,'AT101_1.Ti:Ti
AT101_1.To:To
AT101_1.Wc:Wc','','Блок контролю холодильника після першої ступені компресору','','Блок контроля холодильника после первой ступени компрессора','','');
INSERT INTO "AnastKM201_prm" VALUES('AT101_2','AT201_2','Control block of the fridge after the second stage of the compressor',1,'AT101_2.Ti:Ti
AT101_2.To:To
AT101_2.Wc:Wc','','Блок контролю холодильника після другої ступені компресору','','Блок контроля холодильника после второй ступени компрессора','','');
INSERT INTO "AnastKM201_prm" VALUES('КШ101','KSH201','The control block of the ball valve KSH201',1,'КШ101им.com:com
КШ101им.st_open:st_open
КШ101им.st_close:st_close','КШ201','Блок керування кульовим краном КШ201','КШ201','Блок управления шарового крана КШ201','','');
INSERT INTO "AnastKM201_prm" VALUES('КШ102','KSH202','The control block of the ball valve KSH202',1,'КШ102им.com:com
КШ102им.st_open:st_open
КШ102им.st_close:st_close','КШ202','Блок керування кульовим краном КШ202','КШ202','Блок управления шарового крана КШ202','','');
INSERT INTO "AnastKM201_prm" VALUES('КШ104','KSH204','The control block of the ball valve KSH204',1,'КШ104им.com:com
КШ104им.st_open:st_open
КШ104им.st_close:st_close','КШ204','Блок керування кульовим краном КШ204','КШ204','Блок управления шарового крана КШ204','','');
INSERT INTO "AnastKM201_prm" VALUES('КШ105','KSH205','The control block of the ball valve KSH205',1,'КШ105им.com:com
КШ105им.st_open:st_open
КШ105им.st_close:st_close','КШ205','Блок керування кульовим краном КШ205','КШ205','Блок управления шарового крана КШ205','','');
INSERT INTO "AnastKM201_prm" VALUES('КШ106','KSH206','The control block of the ball valve KSH206',1,'КШ106им.com:com
КШ106им.st_open:st_open
КШ106им.st_close:st_close','КШ206','Блок керування кульовим краном КШ206','КШ206','Блок управления шарового крана КШ206','','');
INSERT INTO "AnastKM201_prm" VALUES('TE1314_2','TE1206_2','The temperature after the fridge of the second stage of the compressor',1,'AT101_2.To:var:Variable
*s.К:ed:Dimension
*i.0:prec:Precision
*r.273:min:Minimum
*r.373:max:Maximum','','Температура після холодильника другої ступені компресора','','Температура после холодильника второй ступени компрессора','AT101_2.To:var:Переменная
*s.К:ed:Ед.измерения
*i.0:prec:Точность
*r.273:min:Минимум
*r.373:max:Максимум','AT101_2.To:var:Змінна
*s.К:ed:Од.виміру
*i.0:prec:Точність
*r.273:min:Мінімум
*r.373:max:Максимум');
INSERT INTO "AnastKM201_prm" VALUES('P101','P201','Pressure on the diaphragm PP201',1,'PP101.Pi:var:Variable
*s.at:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.10:max:Maximum','','Тиск на діафрагмі PP201','','Давление на диафрагме PP201','PP101.Pi:var:Переменная
*s.ат:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.10:max:Максимум','PP101.Pi:var:Змінна
*s.ат:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.10:max:Максимум');
CREATE TABLE 'AnastKM202_blocks_io' ("BLK_ID" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"TLNK" INTEGER DEFAULT '' ,"LNK" TEXT DEFAULT '' ,"VAL" TEXT DEFAULT '' , PRIMARY KEY ("BLK_ID","ID"));
INSERT INTO "AnastKM202_blocks_io" VALUES('KPP102','Fi',0,'','8.63845305351642e-205');
INSERT INTO "AnastKM202_blocks_io" VALUES('KPP102','Pi',1,'node6_1.Po2','23.7038694014337');
INSERT INTO "AnastKM202_blocks_io" VALUES('KPP102','Ti',1,'AT102_2.To','282.999999809705');
INSERT INTO "AnastKM202_blocks_io" VALUES('KPP102','Fo',1,'node5_1.Fi2','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('KPP102','Po',0,'','23.7039729602121');
INSERT INTO "AnastKM202_blocks_io" VALUES('KPP102','To',0,'','273.000003488647');
INSERT INTO "AnastKM202_blocks_io" VALUES('KPP102','So',0,'','0.017');
INSERT INTO "AnastKM202_blocks_io" VALUES('KPP102','lo',0,'','10');
INSERT INTO "AnastKM202_blocks_io" VALUES('KPP102','S_kl1',0,'','0.015');
INSERT INTO "AnastKM202_blocks_io" VALUES('KPP102','S_kl2',0,'','0.015');
INSERT INTO "AnastKM202_blocks_io" VALUES('KPP102','l_kl1',2,'Anast1to2node_cntr.SurgeKM202.out','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('KPP102','l_kl2',0,'','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('KPP102','Q0',0,'','1');
INSERT INTO "AnastKM202_blocks_io" VALUES('KPP102','Kln',0,'','2');
INSERT INTO "AnastKM202_blocks_io" VALUES('KPP102','Kpr',0,'','0.95');
INSERT INTO "AnastKM202_blocks_io" VALUES('KPP102','Ct',0,'','20');
INSERT INTO "AnastKM202_blocks_io" VALUES('KPP102','Riz',0,'','20');
INSERT INTO "AnastKM202_blocks_io" VALUES('KPP102','noBack',0,'','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('KPP102','Fwind',0,'','1');
INSERT INTO "AnastKM202_blocks_io" VALUES('KPP102','Twind',0,'','273');
INSERT INTO "AnastKM202_blocks_io" VALUES('KPP102','f_frq',0,'','100');
INSERT INTO "AnastKM202_blocks_io" VALUES('KPP102','Pot',0,'','24.7062304532602');
INSERT INTO "AnastKM202_blocks_io" VALUES('KPP102','Fit',0,'','-0.147428821940688');
INSERT INTO "AnastKM202_blocks_io" VALUES('net102','Fi',0,'','2.42448558136048e-06');
INSERT INTO "AnastKM202_blocks_io" VALUES('net102','Pi',1,'КШ113.Po','0.999999926137432');
INSERT INTO "AnastKM202_blocks_io" VALUES('net102','Po',0,'','1');
INSERT INTO "AnastKM202_blocks_io" VALUES('net102','So',0,'','0.01');
INSERT INTO "AnastKM202_blocks_io" VALUES('net102','Kpr',0,'','0.95');
INSERT INTO "AnastKM202_blocks_io" VALUES('net102','Noise',0,'','1');
INSERT INTO "AnastKM202_blocks_io" VALUES('net102','Q0',0,'','1');
INSERT INTO "AnastKM202_blocks_io" VALUES('net102','f_frq',0,'','100');
INSERT INTO "AnastKM202_blocks_io" VALUES('DR101','Fi',0,'','0.000309903772113298');
INSERT INTO "AnastKM202_blocks_io" VALUES('DR101','Pi',1,'КШ116.Po','23.7038676333897');
INSERT INTO "AnastKM202_blocks_io" VALUES('DR101','Ti',1,'КШ116.To','273');
INSERT INTO "AnastKM202_blocks_io" VALUES('DR101','Fo',1,'node5_1.Fi1','-0.000124930231965564');
INSERT INTO "AnastKM202_blocks_io" VALUES('DR101','Po',0,'','23.7038223316566');
INSERT INTO "AnastKM202_blocks_io" VALUES('DR101','To',0,'','272.999999999996');
INSERT INTO "AnastKM202_blocks_io" VALUES('DR101','So',0,'','0.0314');
INSERT INTO "AnastKM202_blocks_io" VALUES('DR101','lo',0,'','3');
INSERT INTO "AnastKM202_blocks_io" VALUES('DR101','S_kl1',0,'','0.00314');
INSERT INTO "AnastKM202_blocks_io" VALUES('DR101','S_kl2',0,'','0.00314');
INSERT INTO "AnastKM202_blocks_io" VALUES('DR101','l_kl1',0,'','50');
INSERT INTO "AnastKM202_blocks_io" VALUES('DR101','l_kl2',0,'','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('DR101','Q0',0,'','1');
INSERT INTO "AnastKM202_blocks_io" VALUES('DR101','Kln',0,'','1');
INSERT INTO "AnastKM202_blocks_io" VALUES('DR101','Kpr',0,'','0.95');
INSERT INTO "AnastKM202_blocks_io" VALUES('DR101','Ct',0,'','20');
INSERT INTO "AnastKM202_blocks_io" VALUES('DR101','Riz',0,'','20');
INSERT INTO "AnastKM202_blocks_io" VALUES('DR101','noBack',0,'','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('DR101','Fwind',0,'','1');
INSERT INTO "AnastKM202_blocks_io" VALUES('DR101','Twind',0,'','273');
INSERT INTO "AnastKM202_blocks_io" VALUES('DR101','f_frq',0,'','100');
INSERT INTO "AnastKM202_blocks_io" VALUES('DR101','Pot',0,'','24.7009979434666');
INSERT INTO "AnastKM202_blocks_io" VALUES('DR101','Fit',0,'','0.172987523705479');
INSERT INTO "AnastKM202_blocks_io" VALUES('С102_1','Fi',0,'','1.34947822909511e-06');
INSERT INTO "AnastKM202_blocks_io" VALUES('С102_1','Pi',1,'node5_1.Po','23.7038228178004');
INSERT INTO "AnastKM202_blocks_io" VALUES('С102_1','Si',0,'','0.0314');
INSERT INTO "AnastKM202_blocks_io" VALUES('С102_1','Fo',1,'PP103.Fi','-1.50054070994927e-05');
INSERT INTO "AnastKM202_blocks_io" VALUES('С102_1','Po',0,'','23.7038230386713');
INSERT INTO "AnastKM202_blocks_io" VALUES('С102_1','So',0,'','0.0314');
INSERT INTO "AnastKM202_blocks_io" VALUES('С102_1','lo',0,'','40');
INSERT INTO "AnastKM202_blocks_io" VALUES('С102_1','Fo_ж',0,'','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('С102_1','Po_ж',0,'','23.7133583726618');
INSERT INTO "AnastKM202_blocks_io" VALUES('С102_1','Lж',0,'','3.97305582935614');
INSERT INTO "AnastKM202_blocks_io" VALUES('С102_1','ProcЖ',0,'','0.01');
INSERT INTO "AnastKM202_blocks_io" VALUES('С102_1','Vap',0,'','2.4');
INSERT INTO "AnastKM202_blocks_io" VALUES('С102_1','Q0',0,'','1');
INSERT INTO "AnastKM202_blocks_io" VALUES('С102_1','Qж',0,'','1000');
INSERT INTO "AnastKM202_blocks_io" VALUES('С102_1','f_frq',0,'','100');
INSERT INTO "AnastKM202_blocks_io" VALUES('С102_1','Pot',0,'','24.7312311757839');
INSERT INTO "AnastKM202_blocks_io" VALUES('С102_1','Fit',0,'','0.00155800981669319');
INSERT INTO "AnastKM202_blocks_io" VALUES('AT102_1','Fi',0,'','-0.000417824700136899');
INSERT INTO "AnastKM202_blocks_io" VALUES('AT102_1','Pi',1,'KM102_1.Po','23.7038209642486');
INSERT INTO "AnastKM202_blocks_io" VALUES('AT102_1','Ti',1,'KM102_1.To','273.054691906722');
INSERT INTO "AnastKM202_blocks_io" VALUES('AT102_1','Si',0,'','0.015');
INSERT INTO "AnastKM202_blocks_io" VALUES('AT102_1','li',0,'','10');
INSERT INTO "AnastKM202_blocks_io" VALUES('AT102_1','Fo',1,'C102_2.Fi','2.89943846796722e-05');
INSERT INTO "AnastKM202_blocks_io" VALUES('AT102_1','Po',0,'','23.7038435304422');
INSERT INTO "AnastKM202_blocks_io" VALUES('AT102_1','To',0,'','282.99999987166');
INSERT INTO "AnastKM202_blocks_io" VALUES('AT102_1','So',0,'','0.017');
INSERT INTO "AnastKM202_blocks_io" VALUES('AT102_1','lo',0,'','2');
INSERT INTO "AnastKM202_blocks_io" VALUES('AT102_1','Tair',0,'','283');
INSERT INTO "AnastKM202_blocks_io" VALUES('AT102_1','Wc',0,'','200');
INSERT INTO "AnastKM202_blocks_io" VALUES('AT102_1','Q0',0,'','1');
INSERT INTO "AnastKM202_blocks_io" VALUES('AT102_1','Ct',0,'','100');
INSERT INTO "AnastKM202_blocks_io" VALUES('AT102_1','Rt',0,'','1');
INSERT INTO "AnastKM202_blocks_io" VALUES('AT102_1','f_frq',0,'','100');
INSERT INTO "AnastKM202_blocks_io" VALUES('AT102_1','Pot',0,'','24.732442399371');
INSERT INTO "AnastKM202_blocks_io" VALUES('AT102_1','Fit',0,'','-0.0890221563119908');
INSERT INTO "AnastKM202_blocks_io" VALUES('AT102_2','Fi',0,'','5.99541471414036e-05');
INSERT INTO "AnastKM202_blocks_io" VALUES('AT102_2','Pi',1,'KM102_2.Po','23.7038207414182');
INSERT INTO "AnastKM202_blocks_io" VALUES('AT102_2','Ti',1,'KM102_2.To','274.179340358255');
INSERT INTO "AnastKM202_blocks_io" VALUES('AT102_2','Si',0,'','0.007');
INSERT INTO "AnastKM202_blocks_io" VALUES('AT102_2','li',0,'','20');
INSERT INTO "AnastKM202_blocks_io" VALUES('AT102_2','Fo',1,'C102_3.Fi','7.94912598272367e-06');
INSERT INTO "AnastKM202_blocks_io" VALUES('AT102_2','Po',0,'','23.7038328315958');
INSERT INTO "AnastKM202_blocks_io" VALUES('AT102_2','To',0,'','282.999999876551');
INSERT INTO "AnastKM202_blocks_io" VALUES('AT102_2','So',0,'','0.00785');
INSERT INTO "AnastKM202_blocks_io" VALUES('AT102_2','lo',0,'','5');
INSERT INTO "AnastKM202_blocks_io" VALUES('AT102_2','Tair',0,'','283');
INSERT INTO "AnastKM202_blocks_io" VALUES('AT102_2','Wc',0,'','200');
INSERT INTO "AnastKM202_blocks_io" VALUES('AT102_2','Q0',0,'','1');
INSERT INTO "AnastKM202_blocks_io" VALUES('AT102_2','Ct',0,'','100');
INSERT INTO "AnastKM202_blocks_io" VALUES('AT102_2','Rt',0,'','1');
INSERT INTO "AnastKM202_blocks_io" VALUES('AT102_2','f_frq',0,'','100');
INSERT INTO "AnastKM202_blocks_io" VALUES('AT102_2','Pot',0,'','24.7477290110479');
INSERT INTO "AnastKM202_blocks_io" VALUES('AT102_2','Fit',0,'','0.255407015091622');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ111','Fi',0,'','-1.23516411460312e-321');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ111','Pi',0,'','23.7066105444017');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ111','Ti',2,'Anast1to2node.КШ21.To','305.032379725558');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ111','Fo',1,'node5_1.Fi3','2.41738233103456e-05');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ111','Po',0,'','23.7038210219182');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ111','To',0,'','273.000000001701');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ111','So',0,'','0.0314');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ111','lo',0,'','5');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ111','S_kl1',0,'','0.0314');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ111','S_kl2',0,'','0.002');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ111','l_kl1',1,'КШ111им.pos','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ111','l_kl2',1,'КШ112им.pos','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ111','Q0',0,'','1');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ111','Kln',0,'','1');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ111','Kpr',0,'','0.95');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ111','Ct',0,'','20');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ111','Riz',0,'','20');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ111','noBack',0,'','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ111','Fwind',0,'','1');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ111','Twind',0,'','273');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ111','f_frq',0,'','100');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ111','Pot',0,'','24.7253558478491');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ111','Fit',0,'','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ113','Fi',0,'','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ113','Pi',1,'node6_1.Po4','23.7037850347741');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ113','Ti',0,'','273');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ113','Fo',1,'net102.Fi','-2.40871116175424e-06');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ113','Po',0,'','1.00000016092357');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ113','To',0,'','272.999999998993');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ113','So',0,'','0.00785');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ113','lo',0,'','10');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ113','S_kl1',0,'','0.00785');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ113','S_kl2',0,'','0.002');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ113','l_kl1',0,'','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ113','l_kl2',0,'','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ113','Q0',0,'','1');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ113','Kln',0,'','1');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ113','Kpr',0,'','0.95');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ113','Ct',0,'','20');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ113','Riz',0,'','20');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ113','noBack',0,'','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ113','Fwind',0,'','1');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ113','Twind',0,'','273');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ113','f_frq',0,'','100');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ113','Pot',0,'','1.00004217473475');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ113','Fit',0,'','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ114','Fi',0,'','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ114','Pi',1,'node6_1.Po3','23.7037286929602');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ114','Ti',1,'AT102_2.To','282.999999830851');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ114','Fo',0,'','0.00054210104184962');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ114','Po',0,'','92.2920521113436');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ114','To',0,'','273.000000010071');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ114','So',0,'','0.00785');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ114','lo',0,'','100');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ114','S_kl1',0,'','0.00785');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ114','S_kl2',0,'','0.002');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ114','l_kl1',1,'КШ114им.pos','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ114','l_kl2',1,'КШ115им.pos','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ114','Q0',0,'','1');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ114','Kln',0,'','1');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ114','Kpr',0,'','0.95');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ114','Ct',0,'','20');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ114','Riz',0,'','20');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ114','noBack',0,'','1');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ114','Fwind',0,'','1');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ114','Twind',0,'','273');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ114','f_frq',0,'','100');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ114','Pot',0,'','91.8399092307547');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ114','Fit',0,'','2.47032822920623e-322');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ116','Fi',0,'','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ116','Pi',1,'node6_1.Po1','23.7038160793298');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ116','Ti',1,'AT102_2.To','282.999999939508');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ116','Fo',1,'DR101.Fi','-0.000185204443560551');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ116','Po',0,'','23.7037752558612');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ116','To',0,'','273');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ116','So',0,'','0.0078');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ116','lo',0,'','1');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ116','S_kl1',0,'','0.0078');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ116','S_kl2',0,'','0.0078');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ116','l_kl1',1,'КШ116им.pos','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ116','l_kl2',0,'','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ116','Q0',0,'','1');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ116','Kln',0,'','1');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ116','Kpr',0,'','0.95');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ116','Ct',0,'','20');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ116','Riz',0,'','20');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ116','noBack',0,'','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ116','Fwind',0,'','1');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ116','Twind',0,'','273');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ116','f_frq',0,'','100');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ116','Pot',0,'','24.8055729734348');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ116','Fit',0,'','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('node5_1','Fi1',0,'','3.25588932921207e-05');
INSERT INTO "AnastKM202_blocks_io" VALUES('node5_1','Pi1',1,'DR101.Po','23.7038123172981');
INSERT INTO "AnastKM202_blocks_io" VALUES('node5_1','Ti1',1,'DR101.To','272.999999999997');
INSERT INTO "AnastKM202_blocks_io" VALUES('node5_1','Si1',0,'','0.0314');
INSERT INTO "AnastKM202_blocks_io" VALUES('node5_1','Fi2',0,'','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('node5_1','Pi2',1,'KPP102.Po','23.7039729602121');
INSERT INTO "AnastKM202_blocks_io" VALUES('node5_1','Ti2',1,'KPP102.To','273.000003487328');
INSERT INTO "AnastKM202_blocks_io" VALUES('node5_1','Si2',0,'','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('node5_1','Fi3',0,'','-4.8603976886153e-05');
INSERT INTO "AnastKM202_blocks_io" VALUES('node5_1','Pi3',1,'КШ111.Po','23.7038246038819');
INSERT INTO "AnastKM202_blocks_io" VALUES('node5_1','Ti3',1,'КШ111.To','273.000000001701');
INSERT INTO "AnastKM202_blocks_io" VALUES('node5_1','Si3',0,'','0.0314');
INSERT INTO "AnastKM202_blocks_io" VALUES('node5_1','Fo',1,'С102_1.Fi','-4.41648590833887e-05');
INSERT INTO "AnastKM202_blocks_io" VALUES('node5_1','Po',0,'','23.7038229254907');
INSERT INTO "AnastKM202_blocks_io" VALUES('node5_1','To',0,'','272.999999994404');
INSERT INTO "AnastKM202_blocks_io" VALUES('node5_1','So',0,'','0.2');
INSERT INTO "AnastKM202_blocks_io" VALUES('node5_1','lo',0,'','50');
INSERT INTO "AnastKM202_blocks_io" VALUES('node5_1','Q0',0,'','1');
INSERT INTO "AnastKM202_blocks_io" VALUES('node5_1','Ct',0,'','20');
INSERT INTO "AnastKM202_blocks_io" VALUES('node5_1','Riz',0,'','20');
INSERT INTO "AnastKM202_blocks_io" VALUES('node5_1','Fwind',0,'','1');
INSERT INTO "AnastKM202_blocks_io" VALUES('node5_1','Twind',0,'','273');
INSERT INTO "AnastKM202_blocks_io" VALUES('node5_1','f_frq',0,'','100');
INSERT INTO "AnastKM202_blocks_io" VALUES('node5_1','FoR',0,'','-10.1677478719936');
INSERT INTO "AnastKM202_blocks_io" VALUES('node5_1','Pot',0,'','24.73125922103');
INSERT INTO "AnastKM202_blocks_io" VALUES('node5_1','Fit',0,'','0.332954574126532');
INSERT INTO "AnastKM202_blocks_io" VALUES('KM102_1','Fi',0,'','2.69253640003187e-05');
INSERT INTO "AnastKM202_blocks_io" VALUES('KM102_1','Pi',1,'PP103.Po','23.7038234685566');
INSERT INTO "AnastKM202_blocks_io" VALUES('KM102_1','Ti',0,'','273');
INSERT INTO "AnastKM202_blocks_io" VALUES('KM102_1','Fo',1,'AT102_1.Fi','0.000404497134364573');
INSERT INTO "AnastKM202_blocks_io" VALUES('KM102_1','Po',0,'','23.7038210590106');
INSERT INTO "AnastKM202_blocks_io" VALUES('KM102_1','To',0,'','273.054691890607');
INSERT INTO "AnastKM202_blocks_io" VALUES('KM102_1','So',0,'','0.017');
INSERT INTO "AnastKM202_blocks_io" VALUES('KM102_1','lo',0,'','55');
INSERT INTO "AnastKM202_blocks_io" VALUES('KM102_1','Kzp',0,'','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('KM102_1','N',0,'','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('KM102_1','V',0,'','0.27');
INSERT INTO "AnastKM202_blocks_io" VALUES('KM102_1','Kpmp',0,'','0.02');
INSERT INTO "AnastKM202_blocks_io" VALUES('KM102_1','Kslp',0,'','0.2');
INSERT INTO "AnastKM202_blocks_io" VALUES('KM102_1','Q0',0,'','1');
INSERT INTO "AnastKM202_blocks_io" VALUES('KM102_1','Ct',0,'','100');
INSERT INTO "AnastKM202_blocks_io" VALUES('KM102_1','Riz',0,'','100');
INSERT INTO "AnastKM202_blocks_io" VALUES('KM102_1','Fwind',0,'','1');
INSERT INTO "AnastKM202_blocks_io" VALUES('KM102_1','Twind',0,'','273');
INSERT INTO "AnastKM202_blocks_io" VALUES('KM102_1','f_frq',0,'','100');
INSERT INTO "AnastKM202_blocks_io" VALUES('KM102_1','Pot',0,'','24.7309314882325');
INSERT INTO "AnastKM202_blocks_io" VALUES('KM102_1','Fit',0,'','0.00111103045091996');
INSERT INTO "AnastKM202_blocks_io" VALUES('KM102_2','Fi',0,'','1.69260283868021e-05');
INSERT INTO "AnastKM202_blocks_io" VALUES('KM102_2','Pi',1,'PP104.Po','23.7038239567091');
INSERT INTO "AnastKM202_blocks_io" VALUES('KM102_2','Ti',1,'AT102_1.To','282.999999771556');
INSERT INTO "AnastKM202_blocks_io" VALUES('KM102_2','Fo',1,'AT102_2.Fi','0.000140055508877986');
INSERT INTO "AnastKM202_blocks_io" VALUES('KM102_2','Po',0,'','23.7038208075982');
INSERT INTO "AnastKM202_blocks_io" VALUES('KM102_2','To',0,'','274.179339618702');
INSERT INTO "AnastKM202_blocks_io" VALUES('KM102_2','So',0,'','0.00785');
INSERT INTO "AnastKM202_blocks_io" VALUES('KM102_2','lo',0,'','55');
INSERT INTO "AnastKM202_blocks_io" VALUES('KM102_2','Kzp',0,'','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('KM102_2','N',1,'KM102_1.N','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('KM102_2','V',0,'','0.13');
INSERT INTO "AnastKM202_blocks_io" VALUES('KM102_2','Kpmp',0,'','0.02');
INSERT INTO "AnastKM202_blocks_io" VALUES('KM102_2','Kslp',0,'','0.2');
INSERT INTO "AnastKM202_blocks_io" VALUES('KM102_2','Q0',0,'','1');
INSERT INTO "AnastKM202_blocks_io" VALUES('KM102_2','Ct',0,'','100');
INSERT INTO "AnastKM202_blocks_io" VALUES('KM102_2','Riz',0,'','100');
INSERT INTO "AnastKM202_blocks_io" VALUES('KM102_2','Fwind',0,'','1');
INSERT INTO "AnastKM202_blocks_io" VALUES('KM102_2','Twind',0,'','273');
INSERT INTO "AnastKM202_blocks_io" VALUES('KM102_2','f_frq',0,'','100');
INSERT INTO "AnastKM202_blocks_io" VALUES('KM102_2','Pot',0,'','24.7476967466407');
INSERT INTO "AnastKM202_blocks_io" VALUES('KM102_2','Fit',0,'','-0.0315647100765251');
INSERT INTO "AnastKM202_blocks_io" VALUES('node6_1','Fi',0,'','-0.000666107337547224');
INSERT INTO "AnastKM202_blocks_io" VALUES('node6_1','Pi',1,'C102_3.Po','23.7037746653097');
INSERT INTO "AnastKM202_blocks_io" VALUES('node6_1','Fo1',1,'КШ116.Fi','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('node6_1','Po1',0,'','23.7038252877993');
INSERT INTO "AnastKM202_blocks_io" VALUES('node6_1','So1',0,'','0.00785');
INSERT INTO "AnastKM202_blocks_io" VALUES('node6_1','lo1',0,'','30');
INSERT INTO "AnastKM202_blocks_io" VALUES('node6_1','Fo2',1,'KPP102.Fi','8.36626469069829e-205');
INSERT INTO "AnastKM202_blocks_io" VALUES('node6_1','Po2',0,'','23.7037943883026');
INSERT INTO "AnastKM202_blocks_io" VALUES('node6_1','So2',0,'','0.002');
INSERT INTO "AnastKM202_blocks_io" VALUES('node6_1','lo2',0,'','30');
INSERT INTO "AnastKM202_blocks_io" VALUES('node6_1','Fo3',1,'КШ114.Fi','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('node6_1','Po3',0,'','23.7038065001281');
INSERT INTO "AnastKM202_blocks_io" VALUES('node6_1','So3',0,'','0.00785');
INSERT INTO "AnastKM202_blocks_io" VALUES('node6_1','lo3',0,'','5');
INSERT INTO "AnastKM202_blocks_io" VALUES('node6_1','Fo4',1,'КШ113.Fi','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('node6_1','Po4',0,'','23.7038000303509');
INSERT INTO "AnastKM202_blocks_io" VALUES('node6_1','So4',0,'','0.00785');
INSERT INTO "AnastKM202_blocks_io" VALUES('node6_1','lo4',0,'','10');
INSERT INTO "AnastKM202_blocks_io" VALUES('node6_1','Q0',0,'','1');
INSERT INTO "AnastKM202_blocks_io" VALUES('node6_1','f_frq',0,'','100');
INSERT INTO "AnastKM202_blocks_io" VALUES('node6_1','F1tmp',0,'','-0.000154002635061539');
INSERT INTO "AnastKM202_blocks_io" VALUES('node6_1','F2tmp',0,'','0.000116556938529383');
INSERT INTO "AnastKM202_blocks_io" VALUES('node6_1','F3tmp',0,'','0.00108031157264903');
INSERT INTO "AnastKM202_blocks_io" VALUES('node6_1','F4tmp',0,'','-0.000274278369438654');
INSERT INTO "AnastKM202_blocks_io" VALUES('node6_1','Pot1',0,'','24.6108046249314');
INSERT INTO "AnastKM202_blocks_io" VALUES('node6_1','Pot2',0,'','25.1285255724454');
INSERT INTO "AnastKM202_blocks_io" VALUES('node6_1','Pot3',0,'','24.7529267833944');
INSERT INTO "AnastKM202_blocks_io" VALUES('node6_1','Pot4',0,'','24.0365393488062');
INSERT INTO "AnastKM202_blocks_io" VALUES('PP103','Fi',0,'','8.48249441649194e-05');
INSERT INTO "AnastKM202_blocks_io" VALUES('PP103','Pi',1,'С102_1.Po','23.7038231632693');
INSERT INTO "AnastKM202_blocks_io" VALUES('PP103','Fo',1,'KM102_1.Fi','-3.70813052119997e-05');
INSERT INTO "AnastKM202_blocks_io" VALUES('PP103','Po',0,'','23.7038240032166');
INSERT INTO "AnastKM202_blocks_io" VALUES('PP103','dP',0,'','-6.19827997624449e-07');
INSERT INTO "AnastKM202_blocks_io" VALUES('PP103','Sdf',0,'','0.02');
INSERT INTO "AnastKM202_blocks_io" VALUES('PP103','So',0,'','0.0314');
INSERT INTO "AnastKM202_blocks_io" VALUES('PP103','lo',0,'','7');
INSERT INTO "AnastKM202_blocks_io" VALUES('PP103','Q0',0,'','1');
INSERT INTO "AnastKM202_blocks_io" VALUES('PP103','f_frq',0,'','100');
INSERT INTO "AnastKM202_blocks_io" VALUES('PP103','Pot',0,'','24.7310701017625');
INSERT INTO "AnastKM202_blocks_io" VALUES('PP103','Fit',0,'','0.0347800008929181');
INSERT INTO "AnastKM202_blocks_io" VALUES('PP104','Fi',0,'','4.6706935908579e-05');
INSERT INTO "AnastKM202_blocks_io" VALUES('PP104','Pi',1,'C102_2.Po','23.7038236182153');
INSERT INTO "AnastKM202_blocks_io" VALUES('PP104','Fo',1,'KM102_2.Fi','3.19303673350056e-05');
INSERT INTO "AnastKM202_blocks_io" VALUES('PP104','Po',0,'','23.7038222532319');
INSERT INTO "AnastKM202_blocks_io" VALUES('PP104','dP',0,'','3.85163375779893e-06');
INSERT INTO "AnastKM202_blocks_io" VALUES('PP104','Sdf',0,'','0.015');
INSERT INTO "AnastKM202_blocks_io" VALUES('PP104','So',0,'','0.017');
INSERT INTO "AnastKM202_blocks_io" VALUES('PP104','lo',0,'','7');
INSERT INTO "AnastKM202_blocks_io" VALUES('PP104','Q0',0,'','1');
INSERT INTO "AnastKM202_blocks_io" VALUES('PP104','f_frq',0,'','100');
INSERT INTO "AnastKM202_blocks_io" VALUES('PP104','Pot',0,'','24.7257258378967');
INSERT INTO "AnastKM202_blocks_io" VALUES('PP104','Fit',0,'','6.10188555296873');
INSERT INTO "AnastKM202_blocks_io" VALUES('C102_2','Fi',0,'','-6.74827387285084e-05');
INSERT INTO "AnastKM202_blocks_io" VALUES('C102_2','Pi',1,'AT102_1.Po','23.7037162227564');
INSERT INTO "AnastKM202_blocks_io" VALUES('C102_2','Si',0,'','0.017');
INSERT INTO "AnastKM202_blocks_io" VALUES('C102_2','Fo',1,'PP104.Fi','-2.46317322984754e-05');
INSERT INTO "AnastKM202_blocks_io" VALUES('C102_2','Po',0,'','23.7038232917397');
INSERT INTO "AnastKM202_blocks_io" VALUES('C102_2','So',0,'','0.017');
INSERT INTO "AnastKM202_blocks_io" VALUES('C102_2','lo',0,'','50');
INSERT INTO "AnastKM202_blocks_io" VALUES('C102_2','Fo_ж',0,'','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('C102_2','Po_ж',0,'','23.7133843390131');
INSERT INTO "AnastKM202_blocks_io" VALUES('C102_2','Lж',0,'','4.78052363667422');
INSERT INTO "AnastKM202_blocks_io" VALUES('C102_2','ProcЖ',0,'','0.01');
INSERT INTO "AnastKM202_blocks_io" VALUES('C102_2','Vap',0,'','2');
INSERT INTO "AnastKM202_blocks_io" VALUES('C102_2','Q0',0,'','1');
INSERT INTO "AnastKM202_blocks_io" VALUES('C102_2','Qж',0,'','1000');
INSERT INTO "AnastKM202_blocks_io" VALUES('C102_2','f_frq',0,'','100');
INSERT INTO "AnastKM202_blocks_io" VALUES('C102_2','Pot',0,'','24.7284580318349');
INSERT INTO "AnastKM202_blocks_io" VALUES('C102_2','Fit',0,'','-0.0311306889521761');
INSERT INTO "AnastKM202_blocks_io" VALUES('C102_3','Fi',0,'','0.000232809326803824');
INSERT INTO "AnastKM202_blocks_io" VALUES('C102_3','Pi',1,'AT102_2.Po','23.7038206027158');
INSERT INTO "AnastKM202_blocks_io" VALUES('C102_3','Si',0,'','0.00785');
INSERT INTO "AnastKM202_blocks_io" VALUES('C102_3','Fo',1,'node6_1.Fi','-0.000318036909024718');
INSERT INTO "AnastKM202_blocks_io" VALUES('C102_3','Po',0,'','23.7038188420151');
INSERT INTO "AnastKM202_blocks_io" VALUES('C102_3','So',0,'','0.00785');
INSERT INTO "AnastKM202_blocks_io" VALUES('C102_3','lo',0,'','8');
INSERT INTO "AnastKM202_blocks_io" VALUES('C102_3','Fo_ж',0,'','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('C102_3','Po_ж',0,'','23.7141063054864');
INSERT INTO "AnastKM202_blocks_io" VALUES('C102_3','Lж',0,'','6.85830898084809');
INSERT INTO "AnastKM202_blocks_io" VALUES('C102_3','ProcЖ',0,'','0.01');
INSERT INTO "AnastKM202_blocks_io" VALUES('C102_3','Vap',0,'','1.5');
INSERT INTO "AnastKM202_blocks_io" VALUES('C102_3','Q0',0,'','1');
INSERT INTO "AnastKM202_blocks_io" VALUES('C102_3','Qж',0,'','1000');
INSERT INTO "AnastKM202_blocks_io" VALUES('C102_3','f_frq',0,'','100');
INSERT INTO "AnastKM202_blocks_io" VALUES('C102_3','Pot',0,'','24.6232820831098');
INSERT INTO "AnastKM202_blocks_io" VALUES('C102_3','Fit',0,'','0.84291905311513');
INSERT INTO "AnastKM202_blocks_io" VALUES('KPP102','t_kl1',0,'','3');
INSERT INTO "AnastKM202_blocks_io" VALUES('KPP102','t_kl2',0,'','5');
INSERT INTO "AnastKM202_blocks_io" VALUES('KPP102','tmp_l1',0,'','7.4109846876187e-322');
INSERT INTO "AnastKM202_blocks_io" VALUES('KPP102','tmp_l2',0,'','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ114им','com',0,'','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('DR101','t_kl1',0,'','10');
INSERT INTO "AnastKM202_blocks_io" VALUES('DR101','t_kl2',0,'','5');
INSERT INTO "AnastKM202_blocks_io" VALUES('DR101','tmp_l1',0,'','49.9999999999964');
INSERT INTO "AnastKM202_blocks_io" VALUES('DR101','tmp_l2',0,'','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ114им','pos',0,'','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ111','t_kl1',0,'','10');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ111','t_kl2',0,'','5');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ111','tmp_l1',0,'','2.47032822920623e-321');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ111','tmp_l2',0,'','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ113','t_kl1',0,'','10');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ113','t_kl2',0,'','5');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ113','tmp_l1',0,'','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ113','tmp_l2',0,'','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ114','t_kl1',0,'','10');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ114','t_kl2',0,'','5');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ114','tmp_l1',0,'','2.47032822920623e-321');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ114','tmp_l2',0,'','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ116','t_kl1',0,'','10');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ116','t_kl2',0,'','5');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ116','tmp_l1',0,'','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ116','tmp_l2',0,'','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ111им','pos',0,'','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('node5_1','Kpr',0,'','0.95');
INSERT INTO "AnastKM202_blocks_io" VALUES('KM102_1','Kpr',0,'','0.95');
INSERT INTO "AnastKM202_blocks_io" VALUES('KM102_2','Kpr',0,'','0.95');
INSERT INTO "AnastKM202_blocks_io" VALUES('node6_1','Kpr',0,'','0.95');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ116им','pos',0,'','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ114им','st_open',0,'','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('PP103','Kpr',0,'','0.95');
INSERT INTO "AnastKM202_blocks_io" VALUES('PP104','Kpr',0,'','0.95');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ112им','pos',0,'','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ114им','st_close',0,'','1');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ114им','t_full',0,'','5');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ114им','t_up',0,'','0.5');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ114им','f_frq',0,'','100');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ114им','tmp_up',0,'','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ114им','lst_com',0,'','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ115им','pos',0,'','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ115им','com',0,'','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ115им','st_open',0,'','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ115им','st_close',0,'','1');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ115им','t_full',0,'','5');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ115им','t_up',0,'','0.5');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ115им','f_frq',0,'','100');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ115им','tmp_up',0,'','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ115им','lst_com',0,'','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ111им','com',0,'','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ111им','st_open',0,'','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ111им','st_close',0,'','1');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ111им','t_full',0,'','5');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ111им','t_up',0,'','0.5');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ111им','f_frq',0,'','100');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ111им','tmp_up',0,'','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ111им','lst_com',0,'','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ116им','com',0,'','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ116им','st_open',0,'','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ116им','st_close',0,'','1');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ116им','t_full',0,'','5');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ116им','t_up',0,'','0.5');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ116им','f_frq',0,'','100');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ116им','tmp_up',0,'','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ116им','lst_com',0,'','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ112им','com',0,'','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ112им','st_open',0,'','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ112им','st_close',0,'','1');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ112им','t_full',0,'','3');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ112им','t_up',0,'','0.3');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ112им','f_frq',0,'','100');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ112им','tmp_up',0,'','0');
INSERT INTO "AnastKM202_blocks_io" VALUES('КШ112им','lst_com',0,'','0');
CREATE TABLE 'AnastKM202_prm' ("SHIFR" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '' ,"IO" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"ru#IO" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"uk#IO" TEXT DEFAULT '' , PRIMARY KEY ("SHIFR"));
INSERT INTO "AnastKM202_prm" VALUES('KPP102','KPP202','Anti-surge control valve of the compressor',1,'KPP102.l_kl1:out:Output
KPP102.l_kl1:var:Variable
KPP102.l_kl1:varIn:Variable input
*s.%:ed:Dimension
*i.0:prec:Precision
*r.0:min:Minimum
*r.100:max:Maximum','','Антипомпажный регулирующий клапан компрессора','KPP102.l_kl1:out:Выход
KPP102.l_kl1:var:Переменная
KPP102.l_kl1:varIn:Ввод переменной
*s.%:ed:Ед.измерения
*i.0:prec:Точность
*r.0:min:Минимум
*r.100:max:Максимум','','Антипомпажний регулюючий клапан компресору','KPP102.l_kl1:out:Вихід
KPP102.l_kl1:var:Змінна
KPP102.l_kl1:varIn:Ввід змінної
*s.%:ed:Од.виміру
*i.0:prec:Точність
*r.0:min:Мінімум
*r.100:max:Максимум');
INSERT INTO "AnastKM202_prm" VALUES('F103','F203','Flow on the diaphragm PP203',1,'PP103.Fi:var:Variable
*s.tone/h:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.100:max:Maximum','','Расход на диафрагме PP203','PP103.Fi:var:Переменная
*s.т/ч:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.100:max:Максимум','','Витрати на діафрагмі PP203','PP103.Fi:var:Змінна
*s.т/год:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.100:max:Максимум');
INSERT INTO "AnastKM202_prm" VALUES('TT0202','TE1202_5','Temperature after the first stage of the compressor',1,'KM102_1.To:var:Variable
*s.К:ed:Dimension
*i.0:prec:Precision
*r.273:min:Minimum
*r.373:max:Maximum','','Температура после первой ступени компрессора','KM102_1.To:var:Переменная
*s.К:ed:Ед.измерения
*i.0:prec:Точность
*r.273:min:Минимум
*r.373:max:Максимум','','Температура після першої ступені компресора','KM102_1.To:var:Змінна
*s.К:ed:Од.виміру
*i.0:prec:Точність
*r.273:min:Мінімум
*r.373:max:Максимум');
INSERT INTO "AnastKM202_prm" VALUES('F104','F204','Flow on the diaphragm PP204',1,'PP104.Fi:var:Variable
*s.tone/h:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.100:max:Maximum','','Расход на диафрагме PP204','PP104.Fi:var:Переменная
*s.т/ч:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.100:max:Максимум','','Витрати на діафрагмі PP204','PP104.Fi:var:Змінна
*s.т/год:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.100:max:Максимум');
INSERT INTO "AnastKM202_prm" VALUES('TT0204','TE1205_5','Temperature after the second stage of the compressor',1,'KM102_2.To:var:Variable
*s.К:ed:Dimension
*i.0:prec:Precision
*r.273:min:Minimum
*r.423:max:Maximum','','Температура после второй ступени компрессора','KM102_2.To:var:Переменная
*s.К:ed:Ед.измерения
*i.0:prec:Точность
*r.273:min:Минимум
*r.423:max:Максимум','','Температура після другої ступені компресора','KM102_2.To:var:Змінна
*s.К:ed:Од.виміру
*i.0:prec:Точність
*r.273:min:Мінімум
*r.423:max:Максимум');
INSERT INTO "AnastKM202_prm" VALUES('TE1313_1','TE1313_5','The temperature at the input of the first stage of the compressor',1,'node5_1.To:var:Variable
*s.К:ed:Dimension
*i.0:prec:Precision
*r.273:min:Minimum
*r.373:max:Maximum','','Температура на входе первой ступени компрессора','node5_1.To:var:Переменная
*s.К:ed:Ед.измерения
*i.0:prec:Точность
*r.273:min:Минимум
*r.373:max:Максимум','','Температура на вході першої ступені компресора','node5_1.To:var:Змінна
*s.К:ed:Од.виміру
*i.0:prec:Точність
*r.273:min:Мінімум
*r.373:max:Максимум');
INSERT INTO "AnastKM202_prm" VALUES('PT0202','PT0202_5','Pressure after the first stage of the compressor',1,'KM102_1.Po:var:Variable
*s.at:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.75:max:Maximum','','Давление после первой ступени компрессора','KM102_1.Po:var:Переменная
*s.ат:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.75:max:Максимум','','Тиск після першої ступені компресору','KM102_1.Po:var:Змінна
*s.ат:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.75:max:Максимум');
INSERT INTO "AnastKM202_prm" VALUES('КШ111','KSH211','The control block of the ball valve KSH211',1,'КШ111им.com:com
КШ111им.st_open:st_open
КШ111им.st_close:st_close','КШ211','Блок управления шарового крана КШ211','','КШ211','Блок керування кульовим краном КШ211','');
INSERT INTO "AnastKM202_prm" VALUES('PT0204','PT0204_5','Pressure after the second stage of the compressor',1,'KM102_2.Po:var:Variable
*s.at:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.150:max:Maximum','','Давление после второй ступени компрессора','KM102_2.Po:var:Переменная
*s.ат:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.150:max:Максимум','','Тиск після другої ступені компресору','KM102_2.Po:var:Змінна
*s.ат:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.150:max:Максимум');
INSERT INTO "AnastKM202_prm" VALUES('КШ112','KSH212','The control block of the ball valve KSH212',1,'КШ112им.com:com
КШ112им.st_open:st_open
КШ112им.st_close:st_close','КШ212','Блок управления шарового крана КШ212','','КШ212','Блок керування кульовим краном КШ212','');
INSERT INTO "AnastKM202_prm" VALUES('КШ114','KSH214','The control block of the ball valve KSH214',1,'КШ114им.com:com
КШ114им.st_open:st_open
КШ114им.st_close:st_close','КШ214','Блок управления шарового крана КШ214','','КШ214','Блок керування кульовим краном КШ214','');
INSERT INTO "AnastKM202_prm" VALUES('КШ115','KSH215','The control block of the ball valve KSH215',1,'КШ115им.com:com
КШ115им.st_open:st_open
КШ115им.st_close:st_close','КШ215','Блок управления шарового крана КШ215','','КШ215','Блок керування кульовим краном КШ215','');
INSERT INTO "AnastKM202_prm" VALUES('КШ116','KSH216','The control block of the ball valve KSH216',1,'КШ116им.com:com
КШ116им.st_open:st_open
КШ116им.st_close:st_close','КШ216','Блок управления шарового крана КШ216','','КШ216','Блок керування кульовим краном КШ216','');
INSERT INTO "AnastKM202_prm" VALUES('TE1314_1','TE1314_5','The temperature after the fridge of the first stage of the compressor',1,'AT102_1.To:var:Variable
*s.К:ed:Dimension
*i.0:prec:Precision
*r.273:min:Minimum
*r.373:max:Maximum','','Температура после холодильника первой ступени компрессора','AT102_1.To:var:Переменная
*s.К:ed:Ед.измерения
*i.0:prec:Точность
*r.273:min:Минимум
*r.373:max:Максимум','','Температура після холодильника першої ступені компресора','AT102_1.To:var:Змінна
*s.К:ed:Од.виміру
*i.0:prec:Точність
*r.273:min:Мінімум
*r.373:max:Максимум');
INSERT INTO "AnastKM202_prm" VALUES('TE1314_2','TE1206_5','The temperature after the fridge of the second stage of the compressor',1,'AT102_2.To:var:Variable
*s.К:ed:Dimension
*i.0:prec:Precision
*r.273:min:Minimum
*r.373:max:Maximum','','Температура после холодильника второй ступени компрессора','AT102_2.To:var:Переменная
*s.К:ed:Ед.измерения
*i.0:prec:Точность
*r.273:min:Минимум
*r.373:max:Максимум','','Температура після холодильника другої ступені компресора','AT102_2.To:var:Змінна
*s.К:ed:Од.виміру
*i.0:prec:Точність
*r.273:min:Мінімум
*r.373:max:Максимум');
INSERT INTO "AnastKM202_prm" VALUES('PT1006_1','PT1006_5','Pressure on the diaphragm PP204',1,'PP104.Pi:var:Variable
*s.at:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.75:max:Maximum','','Давление на диафрагме PP204','PP104.Pi:var:Переменная
*s.ат:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.75:max:Максимум','','Тиск на діафрагмі PP204','PP104.Pi:var:Змінна
*s.ат:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.75:max:Максимум');
INSERT INTO "AnastKM202_prm" VALUES('P103','P203','Pressure on the diaphragm PP203',1,'PP103.Pi:var:Variable
*s.at:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.50:max:Maximum','','Давление на диафрагме PP203','PP103.Pi:var:Переменная
*s.ат:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.50:max:Максимум','','Тиск на діафрагмі PP203','PP103.Pi:var:Змінна
*s.ат:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.50:max:Максимум');
INSERT INTO "AnastKM202_prm" VALUES('ST8612','ST8612_5','Rotation speed of the compressor',1,'KM102_1.N:var:Variable
KM102_1.N:varIn:Variable input
*s.1000x rpm:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.15:max:Maximum','','Обороты вращения компрессора','KM102_1.N:var:Переменная
KM102_1.N:varIn:Ввод переменной
*s.1000x об.мин.:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.15:max:Максимум','','Оберти компресору','KM102_1.N:var:Змінна
KM102_1.N:varIn:Ввід змінної
*s.1000x об.хвил:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.15:max:Максимум');
CREATE TABLE 'AnastKM301_blocks_io' ("BLK_ID" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"TLNK" INTEGER DEFAULT '' ,"LNK" TEXT DEFAULT '' ,"VAL" TEXT DEFAULT '' , PRIMARY KEY ("BLK_ID","ID"));
INSERT INTO "AnastKM301_blocks_io" VALUES('КМ101_1','Fi',0,'','-5.31221561339822e-05');
INSERT INTO "AnastKM301_blocks_io" VALUES('КМ101_1','Pi',1,'PP101.Po','5.59314156969428');
INSERT INTO "AnastKM301_blocks_io" VALUES('КМ101_1','Ti',0,'','273');
INSERT INTO "AnastKM301_blocks_io" VALUES('КМ101_1','Fo',1,'AT101_1.Fi','0.000535165329826557');
INSERT INTO "AnastKM301_blocks_io" VALUES('КМ101_1','Po',0,'','5.59314240901266');
INSERT INTO "AnastKM301_blocks_io" VALUES('КМ101_1','To',0,'','273.015976665487');
INSERT INTO "AnastKM301_blocks_io" VALUES('КМ101_1','So',0,'','0.071');
INSERT INTO "AnastKM301_blocks_io" VALUES('КМ101_1','lo',0,'','55');
INSERT INTO "AnastKM301_blocks_io" VALUES('КМ101_1','Kzp',0,'','0');
INSERT INTO "AnastKM301_blocks_io" VALUES('КМ101_1','N',0,'','0');
INSERT INTO "AnastKM301_blocks_io" VALUES('КМ101_1','V',0,'','1.5');
INSERT INTO "AnastKM301_blocks_io" VALUES('КМ101_1','Kpmp',0,'','0.025');
INSERT INTO "AnastKM301_blocks_io" VALUES('КМ101_1','Kslp',0,'','0.2');
INSERT INTO "AnastKM301_blocks_io" VALUES('КМ101_1','Q0',0,'','1');
INSERT INTO "AnastKM301_blocks_io" VALUES('КМ101_1','Ct',0,'','100');
INSERT INTO "AnastKM301_blocks_io" VALUES('КМ101_1','Riz',0,'','100');
INSERT INTO "AnastKM301_blocks_io" VALUES('КМ101_1','Fwind',0,'','1');
INSERT INTO "AnastKM301_blocks_io" VALUES('КМ101_1','Twind',0,'','273');
INSERT INTO "AnastKM301_blocks_io" VALUES('КМ101_1','f_frq',0,'','100');
INSERT INTO "AnastKM301_blocks_io" VALUES('КМ101_1','Pot',0,'','5.40023768274556');
INSERT INTO "AnastKM301_blocks_io" VALUES('КМ101_1','Fit',0,'','-0.000535137058720809');
INSERT INTO "AnastKM301_blocks_io" VALUES('КМ101_2','Fi',0,'','-1.2671671320913e-05');
INSERT INTO "AnastKM301_blocks_io" VALUES('КМ101_2','Pi',1,'PP102.Po','5.59314195720879');
INSERT INTO "AnastKM301_blocks_io" VALUES('КМ101_2','Ti',1,'AT101_1.To','283.000000057448');
INSERT INTO "AnastKM301_blocks_io" VALUES('КМ101_2','Fo',1,'AT101_2.Fi','2.03715739576615e-05');
INSERT INTO "AnastKM301_blocks_io" VALUES('КМ101_2','Po',0,'','5.59314227156568');
INSERT INTO "AnastKM301_blocks_io" VALUES('КМ101_2','To',0,'','274.528749990449');
INSERT INTO "AnastKM301_blocks_io" VALUES('КМ101_2','So',0,'','0.0314');
INSERT INTO "AnastKM301_blocks_io" VALUES('КМ101_2','lo',0,'','55');
INSERT INTO "AnastKM301_blocks_io" VALUES('КМ101_2','Kzp',0,'','0');
INSERT INTO "AnastKM301_blocks_io" VALUES('КМ101_2','N',1,'КМ101_1.N','0');
INSERT INTO "AnastKM301_blocks_io" VALUES('КМ101_2','V',0,'','1');
INSERT INTO "AnastKM301_blocks_io" VALUES('КМ101_2','Kpmp',0,'','0.035');
INSERT INTO "AnastKM301_blocks_io" VALUES('КМ101_2','Kslp',0,'','0.2');
INSERT INTO "AnastKM301_blocks_io" VALUES('КМ101_2','Q0',0,'','1');
INSERT INTO "AnastKM301_blocks_io" VALUES('КМ101_2','Ct',0,'','100');
INSERT INTO "AnastKM301_blocks_io" VALUES('КМ101_2','Riz',0,'','100');
INSERT INTO "AnastKM301_blocks_io" VALUES('КМ101_2','Fwind',0,'','1');
INSERT INTO "AnastKM301_blocks_io" VALUES('КМ101_2','Twind',0,'','273');
INSERT INTO "AnastKM301_blocks_io" VALUES('КМ101_2','f_frq',0,'','100');
INSERT INTO "AnastKM301_blocks_io" VALUES('КМ101_2','Pot',0,'','5.40169165469113');
INSERT INTO "AnastKM301_blocks_io" VALUES('КМ101_2','Fit',0,'','0.000656301282639327');
INSERT INTO "AnastKM301_blocks_io" VALUES('KPP101','Fi',0,'','4.74089749792979e-272');
INSERT INTO "AnastKM301_blocks_io" VALUES('KPP101','Pi',1,'node4_1.Po2','5.59313270295964');
INSERT INTO "AnastKM301_blocks_io" VALUES('KPP101','Ti',1,'AT101_2.To','283.000000003831');
INSERT INTO "AnastKM301_blocks_io" VALUES('KPP101','Fo',1,'node3_1.Fi2','0.000137358220947444');
INSERT INTO "AnastKM301_blocks_io" VALUES('KPP101','Po',0,'','5.59314427462001');
INSERT INTO "AnastKM301_blocks_io" VALUES('KPP101','To',0,'','273.000000141304');
INSERT INTO "AnastKM301_blocks_io" VALUES('KPP101','So',0,'','0.071');
INSERT INTO "AnastKM301_blocks_io" VALUES('KPP101','lo',0,'','10');
INSERT INTO "AnastKM301_blocks_io" VALUES('KPP101','S_kl1',0,'','0.04');
INSERT INTO "AnastKM301_blocks_io" VALUES('KPP101','S_kl2',0,'','0.001');
INSERT INTO "AnastKM301_blocks_io" VALUES('KPP101','l_kl1',2,'Anast1to2node_cntr.SurgeKM301.out','0');
INSERT INTO "AnastKM301_blocks_io" VALUES('KPP101','l_kl2',0,'','0');
INSERT INTO "AnastKM301_blocks_io" VALUES('KPP101','Q0',0,'','1');
INSERT INTO "AnastKM301_blocks_io" VALUES('KPP101','Kln',0,'','2');
INSERT INTO "AnastKM301_blocks_io" VALUES('KPP101','Kpr',0,'','0.95');
INSERT INTO "AnastKM301_blocks_io" VALUES('KPP101','Ct',0,'','20');
INSERT INTO "AnastKM301_blocks_io" VALUES('KPP101','Riz',0,'','20');
INSERT INTO "AnastKM301_blocks_io" VALUES('KPP101','noBack',0,'','0');
INSERT INTO "AnastKM301_blocks_io" VALUES('KPP101','Fwind',0,'','1');
INSERT INTO "AnastKM301_blocks_io" VALUES('KPP101','Twind',0,'','273');
INSERT INTO "AnastKM301_blocks_io" VALUES('KPP101','f_frq',0,'','100');
INSERT INTO "AnastKM301_blocks_io" VALUES('KPP101','Pot',0,'','5.40157875443619');
INSERT INTO "AnastKM301_blocks_io" VALUES('KPP101','Fit',0,'','-0.000441000488177914');
INSERT INTO "AnastKM301_blocks_io" VALUES('DR100','Fi',0,'','0.00163523287978585');
INSERT INTO "AnastKM301_blocks_io" VALUES('DR100','Pi',1,'КШ106.Po','5.59314075881014');
INSERT INTO "AnastKM301_blocks_io" VALUES('DR100','Ti',1,'КШ106.To','273');
INSERT INTO "AnastKM301_blocks_io" VALUES('DR100','Fo',1,'node3_1.Fi1','-0.000562458296076986');
INSERT INTO "AnastKM301_blocks_io" VALUES('DR100','Po',0,'','5.59314890625634');
INSERT INTO "AnastKM301_blocks_io" VALUES('DR100','To',0,'','273.000000006053');
INSERT INTO "AnastKM301_blocks_io" VALUES('DR100','So',0,'','0.2');
INSERT INTO "AnastKM301_blocks_io" VALUES('DR100','lo',0,'','1');
INSERT INTO "AnastKM301_blocks_io" VALUES('DR100','S_kl1',0,'','0.05');
INSERT INTO "AnastKM301_blocks_io" VALUES('DR100','S_kl2',0,'','0.01');
INSERT INTO "AnastKM301_blocks_io" VALUES('DR100','l_kl1',0,'','50');
INSERT INTO "AnastKM301_blocks_io" VALUES('DR100','l_kl2',0,'','0');
INSERT INTO "AnastKM301_blocks_io" VALUES('DR100','Q0',0,'','1');
INSERT INTO "AnastKM301_blocks_io" VALUES('DR100','Kln',0,'','1');
INSERT INTO "AnastKM301_blocks_io" VALUES('DR100','Kpr',0,'','0.95');
INSERT INTO "AnastKM301_blocks_io" VALUES('DR100','Ct',0,'','20');
INSERT INTO "AnastKM301_blocks_io" VALUES('DR100','Riz',0,'','20');
INSERT INTO "AnastKM301_blocks_io" VALUES('DR100','noBack',0,'','0');
INSERT INTO "AnastKM301_blocks_io" VALUES('DR100','Fwind',0,'','1');
INSERT INTO "AnastKM301_blocks_io" VALUES('DR100','Twind',0,'','273');
INSERT INTO "AnastKM301_blocks_io" VALUES('DR100','f_frq',0,'','100');
INSERT INTO "AnastKM301_blocks_io" VALUES('DR100','Pot',0,'','5.78364373014117');
INSERT INTO "AnastKM301_blocks_io" VALUES('DR100','Fit',0,'','-17.4563867731226');
INSERT INTO "AnastKM301_blocks_io" VALUES('AT101_1','Fi',0,'','-0.00196354407449187');
INSERT INTO "AnastKM301_blocks_io" VALUES('AT101_1','Pi',1,'КМ101_1.Po','5.59314108214333');
INSERT INTO "AnastKM301_blocks_io" VALUES('AT101_1','Ti',1,'КМ101_1.To','273.015976675425');
INSERT INTO "AnastKM301_blocks_io" VALUES('AT101_1','Si',0,'','0.065');
INSERT INTO "AnastKM301_blocks_io" VALUES('AT101_1','li',0,'','10');
INSERT INTO "AnastKM301_blocks_io" VALUES('AT101_1','Fo',1,'C101_2.Fi','2.94245560183705e-05');
INSERT INTO "AnastKM301_blocks_io" VALUES('AT101_1','Po',0,'','5.59312254008635');
INSERT INTO "AnastKM301_blocks_io" VALUES('AT101_1','To',0,'','283.000000188317');
INSERT INTO "AnastKM301_blocks_io" VALUES('AT101_1','So',0,'','0.071');
INSERT INTO "AnastKM301_blocks_io" VALUES('AT101_1','lo',0,'','2');
INSERT INTO "AnastKM301_blocks_io" VALUES('AT101_1','Tair',0,'','283');
INSERT INTO "AnastKM301_blocks_io" VALUES('AT101_1','Wc',0,'','200');
INSERT INTO "AnastKM301_blocks_io" VALUES('AT101_1','Q0',0,'','1');
INSERT INTO "AnastKM301_blocks_io" VALUES('AT101_1','Ct',0,'','100');
INSERT INTO "AnastKM301_blocks_io" VALUES('AT101_1','Rt',0,'','1');
INSERT INTO "AnastKM301_blocks_io" VALUES('AT101_1','f_frq',0,'','100');
INSERT INTO "AnastKM301_blocks_io" VALUES('AT101_1','Pot',0,'','5.40045482639887');
INSERT INTO "AnastKM301_blocks_io" VALUES('AT101_1','Fit',0,'','-0.0796572232281504');
INSERT INTO "AnastKM301_blocks_io" VALUES('AT101_2','Fi',0,'','-1.18795764139936e-05');
INSERT INTO "AnastKM301_blocks_io" VALUES('AT101_2','Pi',1,'КМ101_2.Po','5.59314253472004');
INSERT INTO "AnastKM301_blocks_io" VALUES('AT101_2','Ti',1,'КМ101_2.To','274.528751448801');
INSERT INTO "AnastKM301_blocks_io" VALUES('AT101_2','Si',0,'','0.029');
INSERT INTO "AnastKM301_blocks_io" VALUES('AT101_2','li',0,'','20');
INSERT INTO "AnastKM301_blocks_io" VALUES('AT101_2','Fo',1,'C101_3.Fi','0.000222673297710202');
INSERT INTO "AnastKM301_blocks_io" VALUES('AT101_2','Po',0,'','5.59314094825511');
INSERT INTO "AnastKM301_blocks_io" VALUES('AT101_2','To',0,'','283.00000006327');
INSERT INTO "AnastKM301_blocks_io" VALUES('AT101_2','So',0,'','0.0314');
INSERT INTO "AnastKM301_blocks_io" VALUES('AT101_2','lo',0,'','5');
INSERT INTO "AnastKM301_blocks_io" VALUES('AT101_2','Tair',0,'','283');
INSERT INTO "AnastKM301_blocks_io" VALUES('AT101_2','Wc',0,'','200');
INSERT INTO "AnastKM301_blocks_io" VALUES('AT101_2','Q0',0,'','1');
INSERT INTO "AnastKM301_blocks_io" VALUES('AT101_2','Ct',0,'','100');
INSERT INTO "AnastKM301_blocks_io" VALUES('AT101_2','Rt',0,'','1');
INSERT INTO "AnastKM301_blocks_io" VALUES('AT101_2','f_frq',0,'','100');
INSERT INTO "AnastKM301_blocks_io" VALUES('AT101_2','Pot',0,'','5.4020877752679');
INSERT INTO "AnastKM301_blocks_io" VALUES('AT101_2','Fit',0,'','-0.025736784212778');
INSERT INTO "AnastKM301_blocks_io" VALUES('node3_1','Fi1',0,'','-0.000454967341381257');
INSERT INTO "AnastKM301_blocks_io" VALUES('node3_1','Pi1',1,'DR100.Po','5.5931282851306');
INSERT INTO "AnastKM301_blocks_io" VALUES('node3_1','Ti1',1,'DR100.To','273.000000006055');
INSERT INTO "AnastKM301_blocks_io" VALUES('node3_1','Si1',0,'','0.2');
INSERT INTO "AnastKM301_blocks_io" VALUES('node3_1','Fi2',0,'','0.000180315934122483');
INSERT INTO "AnastKM301_blocks_io" VALUES('node3_1','Pi2',1,'KPP101.Po','5.59314362283576');
INSERT INTO "AnastKM301_blocks_io" VALUES('node3_1','Ti2',1,'KPP101.To','273.000000141302');
INSERT INTO "AnastKM301_blocks_io" VALUES('node3_1','Si2',0,'','0.071');
INSERT INTO "AnastKM301_blocks_io" VALUES('node3_1','Fi3',0,'','0.000511808962510496');
INSERT INTO "AnastKM301_blocks_io" VALUES('node3_1','Pi3',1,'КШ101.Po','5.59314027647624');
INSERT INTO "AnastKM301_blocks_io" VALUES('node3_1','Ti3',1,'КШ101.To','273.000000002935');
INSERT INTO "AnastKM301_blocks_io" VALUES('node3_1','Si3',0,'','0.2');
INSERT INTO "AnastKM301_blocks_io" VALUES('node3_1','Fo',1,'C101_1.Fi','-3.89351620639424e-06');
INSERT INTO "AnastKM301_blocks_io" VALUES('node3_1','Po',0,'','5.59314295486122');
INSERT INTO "AnastKM301_blocks_io" VALUES('node3_1','To',0,'','272.999999999081');
INSERT INTO "AnastKM301_blocks_io" VALUES('node3_1','So',0,'','0.2');
INSERT INTO "AnastKM301_blocks_io" VALUES('node3_1','lo',0,'','50');
INSERT INTO "AnastKM301_blocks_io" VALUES('node3_1','Q0',0,'','1');
INSERT INTO "AnastKM301_blocks_io" VALUES('node3_1','Ct',0,'','20');
INSERT INTO "AnastKM301_blocks_io" VALUES('node3_1','Riz',0,'','20');
INSERT INTO "AnastKM301_blocks_io" VALUES('node3_1','Fwind',0,'','1');
INSERT INTO "AnastKM301_blocks_io" VALUES('node3_1','Twind',0,'','273');
INSERT INTO "AnastKM301_blocks_io" VALUES('node3_1','f_frq',0,'','100');
INSERT INTO "AnastKM301_blocks_io" VALUES('node3_1','FoR',0,'','51.7594059956549');
INSERT INTO "AnastKM301_blocks_io" VALUES('node3_1','Pot',0,'','5.40139672085537');
INSERT INTO "AnastKM301_blocks_io" VALUES('node3_1','Fit',0,'','1.09362678119313');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ101','Fi',0,'','-4.84184332924422e-322');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ101','Pi',0,'','5.57992655222994');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ101','Ti',2,'Anast1to2node.КРД1.To','290.489777474648');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ101','Fo',1,'node3_1.Fi3','0.000133216163995301');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ101','Po',0,'','5.59313654623463');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ101','To',0,'','273.000000002935');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ101','So',0,'','0.2');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ101','lo',0,'','5');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ101','S_kl1',0,'','0.2');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ101','S_kl2',0,'','0.05');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ101','l_kl1',1,'КШ101им.pos','0');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ101','l_kl2',1,'КШ102им.pos','0');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ101','Q0',0,'','1');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ101','Kln',0,'','1');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ101','Kpr',0,'','0.95');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ101','Ct',0,'','20');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ101','Riz',0,'','20');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ101','noBack',0,'','0');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ101','Fwind',0,'','1');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ101','Twind',0,'','273');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ101','f_frq',0,'','100');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ101','Pot',0,'','5.40316262054898');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ101','Fit',0,'','-0.597988403950033');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ104','Fi',0,'','0');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ104','Pi',1,'node4_1.Po3','5.59313931298012');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ104','Ti',1,'AT101_2.To','282.999999958025');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ104','Fo',0,'','-0.00897917456811899');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ104','Po',0,'','24.5294776061925');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ104','To',0,'','273.000000009147');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ104','So',0,'','0.03');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ104','lo',0,'','100');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ104','S_kl1',0,'','0.03');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ104','S_kl2',0,'','0.01');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ104','l_kl1',1,'КШ104им.pos','0');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ104','l_kl2',1,'КШ105им.pos','0');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ104','Q0',0,'','1');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ104','Kln',0,'','1');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ104','Kpr',0,'','0.95');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ104','Ct',0,'','20');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ104','Riz',0,'','20');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ104','noBack',0,'','1');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ104','Fwind',0,'','1');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ104','Twind',0,'','273');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ104','f_frq',0,'','100');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ104','Pot',0,'','25.0241440068695');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ104','Fit',0,'','2.47032822920623e-322');
INSERT INTO "AnastKM301_blocks_io" VALUES('node4_1','Fi',0,'','0.000642402335148556');
INSERT INTO "AnastKM301_blocks_io" VALUES('node4_1','Pi',1,'C101_3.Po','5.59314944257717');
INSERT INTO "AnastKM301_blocks_io" VALUES('node4_1','Fo1',1,'КШ106.Fi','0');
INSERT INTO "AnastKM301_blocks_io" VALUES('node4_1','Po1',0,'','5.59313483705973');
INSERT INTO "AnastKM301_blocks_io" VALUES('node4_1','So1',0,'','0.0314');
INSERT INTO "AnastKM301_blocks_io" VALUES('node4_1','lo1',0,'','5');
INSERT INTO "AnastKM301_blocks_io" VALUES('node4_1','Fo2',1,'KPP101.Fi','4.7266890233876e-272');
INSERT INTO "AnastKM301_blocks_io" VALUES('node4_1','Po2',0,'','5.59315224985128');
INSERT INTO "AnastKM301_blocks_io" VALUES('node4_1','So2',0,'','0.0177');
INSERT INTO "AnastKM301_blocks_io" VALUES('node4_1','lo2',0,'','5');
INSERT INTO "AnastKM301_blocks_io" VALUES('node4_1','Fo3',1,'КШ104.Fi','0');
INSERT INTO "AnastKM301_blocks_io" VALUES('node4_1','Po3',0,'','5.59314430827388');
INSERT INTO "AnastKM301_blocks_io" VALUES('node4_1','So3',0,'','0.0314');
INSERT INTO "AnastKM301_blocks_io" VALUES('node4_1','lo3',0,'','5');
INSERT INTO "AnastKM301_blocks_io" VALUES('node4_1','Q0',0,'','1');
INSERT INTO "AnastKM301_blocks_io" VALUES('node4_1','f_frq',0,'','100');
INSERT INTO "AnastKM301_blocks_io" VALUES('node4_1','F1tmp',0,'','-0.000195894006101997');
INSERT INTO "AnastKM301_blocks_io" VALUES('node4_1','F2tmp',0,'','0.000427623556743213');
INSERT INTO "AnastKM301_blocks_io" VALUES('node4_1','F3tmp',0,'','0.00041067278450734');
INSERT INTO "AnastKM301_blocks_io" VALUES('node4_1','Pot1',0,'','5.4352722783381');
INSERT INTO "AnastKM301_blocks_io" VALUES('node4_1','Pot2',0,'','5.42036218663654');
INSERT INTO "AnastKM301_blocks_io" VALUES('node4_1','Pot3',0,'','5.3236984372462');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ106','Fi',0,'','0');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ106','Pi',1,'node4_1.Po1','5.59315608574112');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ106','Ti',1,'AT101_2.To','282.999999944966');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ106','Fo',1,'DR100.Fi','0.00141897061249586');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ106','Po',0,'','5.59314497277502');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ106','To',0,'','273');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ106','So',0,'','0.0314');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ106','lo',0,'','10');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ106','S_kl1',0,'','0.0314');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ106','S_kl2',0,'','0.01');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ106','l_kl1',1,'КШ106им.pos','0');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ106','l_kl2',0,'','0');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ106','Q0',0,'','1');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ106','Kln',0,'','1');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ106','Kpr',0,'','0.95');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ106','Ct',0,'','20');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ106','Riz',0,'','20');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ106','noBack',0,'','0');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ106','Fwind',0,'','1');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ106','Twind',0,'','273');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ106','f_frq',0,'','100');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ106','Pot',0,'','5.01309587841275');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ106','Fit',0,'','0');
INSERT INTO "AnastKM301_blocks_io" VALUES('C101_1','Fi',0,'','-0.00025176252108284');
INSERT INTO "AnastKM301_blocks_io" VALUES('C101_1','Pi',1,'node3_1.Po','5.59314230280833');
INSERT INTO "AnastKM301_blocks_io" VALUES('C101_1','Si',0,'','0.2');
INSERT INTO "AnastKM301_blocks_io" VALUES('C101_1','Fo',1,'PP101.Fi','-4.19995671228158e-05');
INSERT INTO "AnastKM301_blocks_io" VALUES('C101_1','Po',0,'','5.59314191117743');
INSERT INTO "AnastKM301_blocks_io" VALUES('C101_1','So',0,'','0.2');
INSERT INTO "AnastKM301_blocks_io" VALUES('C101_1','lo',0,'','40');
INSERT INTO "AnastKM301_blocks_io" VALUES('C101_1','Fo_ж',0,'','0');
INSERT INTO "AnastKM301_blocks_io" VALUES('C101_1','Po_ж',0,'','5.62246779078349');
INSERT INTO "AnastKM301_blocks_io" VALUES('C101_1','Lж',0,'','3.0869346953746');
INSERT INTO "AnastKM301_blocks_io" VALUES('C101_1','ProcЖ',0,'','0.01');
INSERT INTO "AnastKM301_blocks_io" VALUES('C101_1','Vap',0,'','9.5');
INSERT INTO "AnastKM301_blocks_io" VALUES('C101_1','Q0',0,'','1');
INSERT INTO "AnastKM301_blocks_io" VALUES('C101_1','Qж',0,'','1000');
INSERT INTO "AnastKM301_blocks_io" VALUES('C101_1','f_frq',0,'','100');
INSERT INTO "AnastKM301_blocks_io" VALUES('C101_1','Pot',0,'','5.40135425836322');
INSERT INTO "AnastKM301_blocks_io" VALUES('C101_1','Fit',0,'','0.147603004609401');
INSERT INTO "AnastKM301_blocks_io" VALUES('C101_2','Fi',0,'','8.88727281928196e-05');
INSERT INTO "AnastKM301_blocks_io" VALUES('C101_2','Pi',1,'AT101_1.Po','5.59315078634553');
INSERT INTO "AnastKM301_blocks_io" VALUES('C101_2','Si',0,'','0.071');
INSERT INTO "AnastKM301_blocks_io" VALUES('C101_2','Fo',1,'PP102.Fi','3.06774276369693e-05');
INSERT INTO "AnastKM301_blocks_io" VALUES('C101_2','Po',0,'','5.59314225420884');
INSERT INTO "AnastKM301_blocks_io" VALUES('C101_2','So',0,'','0.071');
INSERT INTO "AnastKM301_blocks_io" VALUES('C101_2','lo',0,'','50');
INSERT INTO "AnastKM301_blocks_io" VALUES('C101_2','Fo_ж',0,'','0');
INSERT INTO "AnastKM301_blocks_io" VALUES('C101_2','Po_ж',0,'','5.62170436065387');
INSERT INTO "AnastKM301_blocks_io" VALUES('C101_2','Lж',0,'','3.00653752053');
INSERT INTO "AnastKM301_blocks_io" VALUES('C101_2','ProcЖ',0,'','0.01');
INSERT INTO "AnastKM301_blocks_io" VALUES('C101_2','Vap',0,'','9.5');
INSERT INTO "AnastKM301_blocks_io" VALUES('C101_2','Q0',0,'','1');
INSERT INTO "AnastKM301_blocks_io" VALUES('C101_2','Qж',0,'','1000');
INSERT INTO "AnastKM301_blocks_io" VALUES('C101_2','f_frq',0,'','100');
INSERT INTO "AnastKM301_blocks_io" VALUES('C101_2','Pot',0,'','5.40056348486556');
INSERT INTO "AnastKM301_blocks_io" VALUES('C101_2','Fit',0,'','-0.0226484354771605');
INSERT INTO "AnastKM301_blocks_io" VALUES('C101_3','Fi',0,'','-3.68862688902949e-05');
INSERT INTO "AnastKM301_blocks_io" VALUES('C101_3','Pi',1,'AT101_2.Po','5.59313635254203');
INSERT INTO "AnastKM301_blocks_io" VALUES('C101_3','Si',0,'','0.0314');
INSERT INTO "AnastKM301_blocks_io" VALUES('C101_3','Fo',1,'node4_1.Fi','0.000400628962250144');
INSERT INTO "AnastKM301_blocks_io" VALUES('C101_3','Po',0,'','5.59314147084375');
INSERT INTO "AnastKM301_blocks_io" VALUES('C101_3','So',0,'','0.0314');
INSERT INTO "AnastKM301_blocks_io" VALUES('C101_3','lo',0,'','8');
INSERT INTO "AnastKM301_blocks_io" VALUES('C101_3','Fo_ж',0,'','0');
INSERT INTO "AnastKM301_blocks_io" VALUES('C101_3','Po_ж',0,'','5.62023565441902');
INSERT INTO "AnastKM301_blocks_io" VALUES('C101_3','Lж',0,'','9.03139452509031');
INSERT INTO "AnastKM301_blocks_io" VALUES('C101_3','ProcЖ',0,'','0.01');
INSERT INTO "AnastKM301_blocks_io" VALUES('C101_3','Vap',0,'','3');
INSERT INTO "AnastKM301_blocks_io" VALUES('C101_3','Q0',0,'','1');
INSERT INTO "AnastKM301_blocks_io" VALUES('C101_3','Qж',0,'','1000');
INSERT INTO "AnastKM301_blocks_io" VALUES('C101_3','f_frq',0,'','100');
INSERT INTO "AnastKM301_blocks_io" VALUES('C101_3','Pot',0,'','5.40093511492469');
INSERT INTO "AnastKM301_blocks_io" VALUES('C101_3','Fit',0,'','0.131497386772777');
INSERT INTO "AnastKM301_blocks_io" VALUES('PP101','Fi',0,'','0.000117740846296231');
INSERT INTO "AnastKM301_blocks_io" VALUES('PP101','Pi',1,'C101_1.Po','5.59314183974146');
INSERT INTO "AnastKM301_blocks_io" VALUES('PP101','Fo',1,'КМ101_1.Fi','-1.50556286529672e-05');
INSERT INTO "AnastKM301_blocks_io" VALUES('PP101','Po',0,'','5.59314204169773');
INSERT INTO "AnastKM301_blocks_io" VALUES('PP101','dP',0,'','3.18803018862537e-07');
INSERT INTO "AnastKM301_blocks_io" VALUES('PP101','Sdf',0,'','0.15');
INSERT INTO "AnastKM301_blocks_io" VALUES('PP101','So',0,'','0.2');
INSERT INTO "AnastKM301_blocks_io" VALUES('PP101','lo',0,'','7');
INSERT INTO "AnastKM301_blocks_io" VALUES('PP101','Q0',0,'','1');
INSERT INTO "AnastKM301_blocks_io" VALUES('PP101','f_frq',0,'','100');
INSERT INTO "AnastKM301_blocks_io" VALUES('PP101','Pot',0,'','5.41867322751214');
INSERT INTO "AnastKM301_blocks_io" VALUES('PP101','Fit',0,'','-6.87718415920635');
INSERT INTO "AnastKM301_blocks_io" VALUES('PP102','Fi',0,'','2.87406093975067e-05');
INSERT INTO "AnastKM301_blocks_io" VALUES('PP102','Pi',1,'C101_2.Po','5.59314230291101');
INSERT INTO "AnastKM301_blocks_io" VALUES('PP102','Fo',1,'КМ101_2.Fi','-1.73622098478205e-05');
INSERT INTO "AnastKM301_blocks_io" VALUES('PP102','Po',0,'','5.59314241020549');
INSERT INTO "AnastKM301_blocks_io" VALUES('PP102','dP',0,'','5.73628141818792e-07');
INSERT INTO "AnastKM301_blocks_io" VALUES('PP102','Sdf',0,'','0.06');
INSERT INTO "AnastKM301_blocks_io" VALUES('PP102','So',0,'','0.071');
INSERT INTO "AnastKM301_blocks_io" VALUES('PP102','lo',0,'','7');
INSERT INTO "AnastKM301_blocks_io" VALUES('PP102','Q0',0,'','1');
INSERT INTO "AnastKM301_blocks_io" VALUES('PP102','f_frq',0,'','100');
INSERT INTO "AnastKM301_blocks_io" VALUES('PP102','Pot',0,'','5.33730972594639');
INSERT INTO "AnastKM301_blocks_io" VALUES('PP102','Fit',0,'','6.59905508875654');
INSERT INTO "AnastKM301_blocks_io" VALUES('КМ101_1','Kpr',0,'','0.95');
INSERT INTO "AnastKM301_blocks_io" VALUES('КМ101_2','Kpr',0,'','0.95');
INSERT INTO "AnastKM301_blocks_io" VALUES('KPP101','t_kl1',0,'','3');
INSERT INTO "AnastKM301_blocks_io" VALUES('KPP101','t_kl2',0,'','5');
INSERT INTO "AnastKM301_blocks_io" VALUES('KPP101','tmp_l1',0,'','7.4109846876187e-322');
INSERT INTO "AnastKM301_blocks_io" VALUES('KPP101','tmp_l2',0,'','0');
INSERT INTO "AnastKM301_blocks_io" VALUES('DR100','t_kl1',0,'','10');
INSERT INTO "AnastKM301_blocks_io" VALUES('DR100','t_kl2',0,'','5');
INSERT INTO "AnastKM301_blocks_io" VALUES('DR100','tmp_l1',0,'','49.9999999999964');
INSERT INTO "AnastKM301_blocks_io" VALUES('DR100','tmp_l2',0,'','0');
INSERT INTO "AnastKM301_blocks_io" VALUES('node3_1','Kpr',0,'','0.95');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ101','t_kl1',0,'','10');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ101','t_kl2',0,'','5');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ101','tmp_l1',0,'','2.47032822920623e-321');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ101','tmp_l2',0,'','0');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ104','t_kl1',0,'','10');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ104','t_kl2',0,'','5');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ104','tmp_l1',0,'','2.47032822920623e-321');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ104','tmp_l2',0,'','0');
INSERT INTO "AnastKM301_blocks_io" VALUES('node4_1','Kpr',0,'','0.95');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ106','t_kl1',0,'','10');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ106','t_kl2',0,'','5');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ106','tmp_l1',0,'','0');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ106','tmp_l2',0,'','0');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ104им','pos',0,'','0');
INSERT INTO "AnastKM301_blocks_io" VALUES('PP101','Kpr',0,'','0.95');
INSERT INTO "AnastKM301_blocks_io" VALUES('PP102','Kpr',0,'','0.95');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ104им','com',0,'','0');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ104им','st_open',0,'','0');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ104им','st_close',0,'','1');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ104им','t_full',0,'','5');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ104им','t_up',0,'','0.5');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ104им','f_frq',0,'','100');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ104им','tmp_up',0,'','0');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ104им','lst_com',0,'','0');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ105им','pos',0,'','0');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ105им','com',0,'','0');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ105им','st_open',0,'','0');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ105им','st_close',0,'','1');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ105им','t_full',0,'','5');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ105им','t_up',0,'','0.5');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ105им','f_frq',0,'','100');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ105им','tmp_up',0,'','0');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ105им','lst_com',0,'','0');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ101им','pos',0,'','0');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ101им','com',0,'','0');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ101им','st_open',0,'','0');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ101им','st_close',0,'','1');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ101им','t_full',0,'','5');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ101им','t_up',0,'','0.5');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ101им','f_frq',0,'','100');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ101им','tmp_up',0,'','0');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ101им','lst_com',0,'','0');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ106им','pos',0,'','0');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ106им','com',0,'','0');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ106им','st_open',0,'','0');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ106им','st_close',0,'','1');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ106им','t_full',0,'','5');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ106им','t_up',0,'','0.5');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ106им','f_frq',0,'','100');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ106им','tmp_up',0,'','0');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ106им','lst_com',0,'','0');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ102им','pos',0,'','0');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ102им','com',0,'','0');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ102им','st_open',0,'','0');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ102им','st_close',0,'','1');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ102им','t_full',0,'','3');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ102им','t_up',0,'','0.3');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ102им','f_frq',0,'','100');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ102им','tmp_up',0,'','0');
INSERT INTO "AnastKM301_blocks_io" VALUES('КШ102им','lst_com',0,'','0');
CREATE TABLE 'AnastKM301_prm' ("SHIFR" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '' ,"IO" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"ru#IO" TEXT DEFAULT '' ,"uk#IO" TEXT DEFAULT '' , PRIMARY KEY ("SHIFR"));
INSERT INTO "AnastKM301_prm" VALUES('FN101','FN301','Flow at the output of compressor',1,'КШ104.Fi:var:Variable','','Витрати на виході компресора','','Расход на выходе компрессора','КШ104.Fi:var:Переменная','КШ104.Fi:var:Змінна');
INSERT INTO "AnastKM301_prm" VALUES('F101','F301','Flow on the diaphragm PP301',1,'PP101.Fi:var:Variable
*s.tone/h:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.100:max:Maximum','','Витрати на діафрагмі PP301','','Расход на диафрагме PP301','PP101.Fi:var:Переменная
*s.т/ч:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.100:max:Максимум','PP101.Fi:var:Змінна
*s.т/год:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.100:max:Максимум');
INSERT INTO "AnastKM301_prm" VALUES('F102','F302','Flow on the diaphragm PP302',1,'PP102.Fi:var:Variable
*s.tone/h:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.100:max:Maximum','','Витрати на діафрагмі PP302','','Расход на диафрагме PP302','PP102.Fi:var:Переменная
*s.т/ч:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.100:max:Максимум','PP102.Fi:var:Змінна
*s.т/год:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.100:max:Максимум');
INSERT INTO "AnastKM301_prm" VALUES('TE1313_1','TE1313_3','The temperature at the input of the first stage of the compressor',1,'node3_1.To:var:Variable
*s.К:ed:Dimension
*i.0:prec:Precision
*r.273:min:Minimum
*r.373:max:Maximum','','Температура на вході першої ступені компресора','','Температура на входе первой ступени компрессора','node3_1.To:var:Переменная
*s.К:ed:Ед.измерения
*i.0:prec:Точность
*r.273:min:Минимум
*r.373:max:Максимум','node3_1.To:var:Змінна
*s.К:ed:Од.виміру
*i.0:prec:Точність
*r.273:min:Мінімум
*r.373:max:Максимум');
INSERT INTO "AnastKM301_prm" VALUES('PT0202','PT0202_3','Pressure after the first stage of the compressor',1,'КМ101_1.Po:var:Variable
*s.at:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.20:max:Maximum','','Тиск після першої ступені компресору','','Давление после первой ступени компрессора','КМ101_1.Po:var:Переменная
*s.ат:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.20:max:Максимум','КМ101_1.Po:var:Змінна
*s.ат:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.20:max:Максимум');
INSERT INTO "AnastKM301_prm" VALUES('PT0204','PT0204_3','Pressure after the second stage of the compressor',1,'КМ101_2.Po:var:Variable
*s.at:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.50:max:Maximum','','Тиск після другої ступені компресору','','Давление после второй ступени компрессора','КМ101_2.Po:var:Переменная
*s.ат:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.50:max:Максимум','КМ101_2.Po:var:Змінна
*s.ат:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.50:max:Максимум');
INSERT INTO "AnastKM301_prm" VALUES('TE1314_1','TE1314_3','The temperature after the fridge of the first stage of the compressor',1,'AT101_1.To:var:Variable
*s.К:ed:Dimension
*i.0:prec:Precision
*r.273:min:Minimum
*r.373:max:Maximum','','Температура після холодильника першої ступені компресора','','Температура после холодильника первой ступени компрессора','AT101_1.To:var:Переменная
*s.К:ed:Ед.измерения
*i.0:prec:Точность
*r.273:min:Минимум
*r.373:max:Максимум','AT101_1.To:var:Змінна
*s.К:ed:Од.виміру
*i.0:prec:Точність
*r.273:min:Мінімум
*r.373:max:Максимум');
INSERT INTO "AnastKM301_prm" VALUES('PT1006_1','PT1006_3','Pressure on the diaphragm PP302',1,'PP102.Pi:var:Variable
*s.at:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.20:max:Maximum','','Тиск на діафрагмі PP302','','Давление на диафрагме PP302','PP102.Pi:var:Переменная
*s.ат:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.20:max:Максимум','PP102.Pi:var:Змінна
*s.ат:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.20:max:Максимум');
INSERT INTO "AnastKM301_prm" VALUES('ST8612','ST8612_3','Rotation speed of the compressor',1,'КМ101_1.N:var:Variable
КМ101_1.N:varIn:Variable input
*s.1000x rpm:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.10:max:Maximum','','Оберти компресору','','Обороты вращения компрессора','КМ101_1.N:var:Переменная
КМ101_1.N:varIn:Ввод переменной
*s.1000x об.мин.:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.10:max:Максимум','КМ101_1.N:var:Змінна
КМ101_1.N:varIn:Ввід змінної
*s.1000x об.хвил:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.10:max:Максимум');
INSERT INTO "AnastKM301_prm" VALUES('KPP101','KPP301','Anti-surge control valve of the compressor',1,'KPP101.l_kl1:out:Output
KPP101.l_kl1:var:Variable
KPP101.l_kl1:varIn:Variable input
*s.%:ed:Dimension
*i.0:prec:Precision
*r.0:min:Minimum
*r.100:max:Maximum','','Антипомпажний регулюючий клапан компресору','','Антипомпажный регулирующий клапан компрессора','KPP101.l_kl1:out:Выход
KPP101.l_kl1:var:Переменная
KPP101.l_kl1:varIn:Ввод переменной
*s.%:ed:Ед.измерения
*i.0:prec:Точность
*r.0:min:Минимум
*r.100:max:Максимум','KPP101.l_kl1:out:Вихід
KPP101.l_kl1:var:Змінна
KPP101.l_kl1:varIn:Ввід змінної
*s.%:ed:Од.виміру
*i.0:prec:Точність
*r.0:min:Мінімум
*r.100:max:Максимум');
INSERT INTO "AnastKM301_prm" VALUES('TT0202','TE1202_3','Temperature after the first stage of the compressor',1,'КМ101_1.To:var:Variable
*s.К:ed:Dimension
*i.0:prec:Precision
*r.273:min:Minimum
*r.373:max:Maximum','','Температура після першої ступені компресора','','Температура после первой ступени компрессора','КМ101_1.To:var:Переменная
*s.К:ed:Ед.измерения
*i.0:prec:Точность
*r.273:min:Минимум
*r.373:max:Максимум','КМ101_1.To:var:Змінна
*s.К:ed:Од.виміру
*i.0:prec:Точність
*r.273:min:Мінімум
*r.373:max:Максимум');
INSERT INTO "AnastKM301_prm" VALUES('TT0204','TE1205_3','Temperature after the second stage of the compressor',1,'КМ101_2.To:var:Variable
*s.К:ed:Dimension
*i.0:prec:Precision
*r.273:min:Minimum
*r.433:max:Maximum','','Температура після другої ступені компресора','','Температура после второй ступени компрессора','КМ101_2.To:var:Переменная
*s.К:ed:Ед.измерения
*i.0:prec:Точность
*r.273:min:Минимум
*r.433:max:Максимум','КМ101_2.To:var:Змінна
*s.К:ed:Од.виміру
*i.0:prec:Точність
*r.273:min:Мінімум
*r.433:max:Максимум');
INSERT INTO "AnastKM301_prm" VALUES('AT101_1','AT301_1','Control block of the fridge after the first stage of the compressor',1,'AT101_1.Ti:Ti
AT101_1.To:To
AT101_1.Wc:Wc','','Блок контролю холодильника після першої ступені компресору','','Блок контроля холодильника после первой ступени компрессора','','');
INSERT INTO "AnastKM301_prm" VALUES('AT101_2','AT301_2','Control block of the fridge after the second stage of the compressor',1,'AT101_2.Ti:Ti
AT101_2.To:To
AT101_2.Wc:Wc','','Блок контролю холодильника після другої ступені компресору','','Блок контроля холодильника после второй ступени компрессора','','');
INSERT INTO "AnastKM301_prm" VALUES('КШ101','KSH301','The control block of the ball valve KSH301',1,'КШ101им.com:com
КШ101им.st_open:st_open
КШ101им.st_close:st_close','КШ301','Блок керування кульовим краном КШ301','КШ301','Блок управления шарового крана КШ301','','');
INSERT INTO "AnastKM301_prm" VALUES('КШ102','KSH302','The control block of the ball valve KSH302',1,'КШ102им.com:com
КШ102им.st_open:st_open
КШ102им.st_close:st_close','КШ302','Блок керування кульовим краном КШ302','КШ302','Блок управления шарового крана КШ302','','');
INSERT INTO "AnastKM301_prm" VALUES('КШ104','KSH304','The control block of the ball valve KSH304',1,'КШ104им.com:com
КШ104им.st_open:st_open
КШ104им.st_close:st_close','КШ304','Блок керування кульовим краном КШ304','КШ304','Блок управления шарового крана КШ304','','');
INSERT INTO "AnastKM301_prm" VALUES('КШ105','KSH305','The control block of the ball valve KSH305',1,'КШ105им.com:com
КШ105им.st_open:st_open
КШ105им.st_close:st_close','КШ305','Блок керування кульовим краном КШ305','КШ305','Блок управления шарового крана КШ305','','');
INSERT INTO "AnastKM301_prm" VALUES('КШ106','KSH306','The control block of the ball valve KSH306',1,'КШ106им.com:com
КШ106им.st_open:st_open
КШ106им.st_close:st_close','КШ306','Блок керування кульовим краном КШ306','КШ306','Блок управления шарового крана КШ306','','');
INSERT INTO "AnastKM301_prm" VALUES('TE1314_2','TE1206_3','The temperature after the fridge of the second stage of the compressor',1,'AT101_2.To:var:Variable
*s.К:ed:Dimension
*i.0:prec:Precision
*r.273:min:Minimum
*r.373:max:Maximum','','Температура після холодильника другої ступені компресора','','Температура после холодильника второй ступени компрессора','AT101_2.To:var:Переменная
*s.К:ed:Ед.измерения
*i.0:prec:Точность
*r.273:min:Минимум
*r.373:max:Максимум','AT101_2.To:var:Змінна
*s.К:ed:Од.виміру
*i.0:prec:Точність
*r.273:min:Мінімум
*r.373:max:Максимум');
INSERT INTO "AnastKM301_prm" VALUES('P101','P301','Pressure on the diaphragm PP301',1,'PP101.Pi:var:Variable
*s.at:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.10:max:Maximum','','Тиск на діафрагмі PP301','','Давление на диафрагме PP301','PP101.Pi:var:Переменная
*s.ат:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.10:max:Максимум','PP101.Pi:var:Змінна
*s.ат:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.10:max:Максимум');
CREATE TABLE 'AnastKM302_blocks_io' ("BLK_ID" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"TLNK" INTEGER DEFAULT '' ,"LNK" TEXT DEFAULT '' ,"VAL" TEXT DEFAULT '' , PRIMARY KEY ("BLK_ID","ID"));
INSERT INTO "AnastKM302_blocks_io" VALUES('KPP102','Fi',0,'','6.43175612648715e-204');
INSERT INTO "AnastKM302_blocks_io" VALUES('KPP102','Pi',1,'node6_1.Po2','23.9734134602821');
INSERT INTO "AnastKM302_blocks_io" VALUES('KPP102','Ti',1,'AT102_2.To','283.000000133064');
INSERT INTO "AnastKM302_blocks_io" VALUES('KPP102','Fo',1,'node5_1.Fi2','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('KPP102','Po',0,'','23.9735548122435');
INSERT INTO "AnastKM302_blocks_io" VALUES('KPP102','To',0,'','273.000003872872');
INSERT INTO "AnastKM302_blocks_io" VALUES('KPP102','So',0,'','0.017');
INSERT INTO "AnastKM302_blocks_io" VALUES('KPP102','lo',0,'','10');
INSERT INTO "AnastKM302_blocks_io" VALUES('KPP102','S_kl1',0,'','0.015');
INSERT INTO "AnastKM302_blocks_io" VALUES('KPP102','S_kl2',0,'','0.015');
INSERT INTO "AnastKM302_blocks_io" VALUES('KPP102','l_kl1',2,'Anast1to2node_cntr.SurgeKM302.out','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('KPP102','l_kl2',0,'','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('KPP102','Q0',0,'','1');
INSERT INTO "AnastKM302_blocks_io" VALUES('KPP102','Kln',0,'','2');
INSERT INTO "AnastKM302_blocks_io" VALUES('KPP102','Kpr',0,'','0.95');
INSERT INTO "AnastKM302_blocks_io" VALUES('KPP102','Ct',0,'','20');
INSERT INTO "AnastKM302_blocks_io" VALUES('KPP102','Riz',0,'','20');
INSERT INTO "AnastKM302_blocks_io" VALUES('KPP102','noBack',0,'','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('KPP102','Fwind',0,'','1');
INSERT INTO "AnastKM302_blocks_io" VALUES('KPP102','Twind',0,'','273');
INSERT INTO "AnastKM302_blocks_io" VALUES('KPP102','f_frq',0,'','100');
INSERT INTO "AnastKM302_blocks_io" VALUES('KPP102','Pot',0,'','26.868606147988');
INSERT INTO "AnastKM302_blocks_io" VALUES('KPP102','Fit',0,'','-0.219664239685689');
INSERT INTO "AnastKM302_blocks_io" VALUES('net102','Fi',0,'','1.69210422219517e-06');
INSERT INTO "AnastKM302_blocks_io" VALUES('net102','Pi',1,'КШ113.Po','1.00000002497253');
INSERT INTO "AnastKM302_blocks_io" VALUES('net102','Po',0,'','1');
INSERT INTO "AnastKM302_blocks_io" VALUES('net102','So',0,'','0.01');
INSERT INTO "AnastKM302_blocks_io" VALUES('net102','Kpr',0,'','0.95');
INSERT INTO "AnastKM302_blocks_io" VALUES('net102','Noise',0,'','1');
INSERT INTO "AnastKM302_blocks_io" VALUES('net102','Q0',0,'','1');
INSERT INTO "AnastKM302_blocks_io" VALUES('net102','f_frq',0,'','100');
INSERT INTO "AnastKM302_blocks_io" VALUES('DR101','Fi',0,'','8.39885305895611e-05');
INSERT INTO "AnastKM302_blocks_io" VALUES('DR101','Pi',1,'КШ116.Po','23.9733552231008');
INSERT INTO "AnastKM302_blocks_io" VALUES('DR101','Ti',1,'КШ116.To','273');
INSERT INTO "AnastKM302_blocks_io" VALUES('DR101','Fo',1,'node5_1.Fi1','-0.000340193842286847');
INSERT INTO "AnastKM302_blocks_io" VALUES('DR101','Po',0,'','23.9733730892402');
INSERT INTO "AnastKM302_blocks_io" VALUES('DR101','To',0,'','272.999999999613');
INSERT INTO "AnastKM302_blocks_io" VALUES('DR101','So',0,'','0.0314');
INSERT INTO "AnastKM302_blocks_io" VALUES('DR101','lo',0,'','3');
INSERT INTO "AnastKM302_blocks_io" VALUES('DR101','S_kl1',0,'','0.00314');
INSERT INTO "AnastKM302_blocks_io" VALUES('DR101','S_kl2',0,'','0.00314');
INSERT INTO "AnastKM302_blocks_io" VALUES('DR101','l_kl1',0,'','50');
INSERT INTO "AnastKM302_blocks_io" VALUES('DR101','l_kl2',0,'','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('DR101','Q0',0,'','1');
INSERT INTO "AnastKM302_blocks_io" VALUES('DR101','Kln',0,'','1');
INSERT INTO "AnastKM302_blocks_io" VALUES('DR101','Kpr',0,'','0.95');
INSERT INTO "AnastKM302_blocks_io" VALUES('DR101','Ct',0,'','20');
INSERT INTO "AnastKM302_blocks_io" VALUES('DR101','Riz',0,'','20');
INSERT INTO "AnastKM302_blocks_io" VALUES('DR101','noBack',0,'','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('DR101','Fwind',0,'','1');
INSERT INTO "AnastKM302_blocks_io" VALUES('DR101','Twind',0,'','273');
INSERT INTO "AnastKM302_blocks_io" VALUES('DR101','f_frq',0,'','100');
INSERT INTO "AnastKM302_blocks_io" VALUES('DR101','Pot',0,'','26.8469800335237');
INSERT INTO "AnastKM302_blocks_io" VALUES('DR101','Fit',0,'','-0.187548356187166');
INSERT INTO "AnastKM302_blocks_io" VALUES('С102_1','Fi',0,'','0.000108280106522202');
INSERT INTO "AnastKM302_blocks_io" VALUES('С102_1','Pi',1,'node5_1.Po','23.9733955933395');
INSERT INTO "AnastKM302_blocks_io" VALUES('С102_1','Si',0,'','0.0314');
INSERT INTO "AnastKM302_blocks_io" VALUES('С102_1','Fo',1,'PP103.Fi','-0.000100692457815686');
INSERT INTO "AnastKM302_blocks_io" VALUES('С102_1','Po',0,'','23.9733949074198');
INSERT INTO "AnastKM302_blocks_io" VALUES('С102_1','So',0,'','0.0314');
INSERT INTO "AnastKM302_blocks_io" VALUES('С102_1','lo',0,'','40');
INSERT INTO "AnastKM302_blocks_io" VALUES('С102_1','Fo_ж',0,'','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('С102_1','Po_ж',0,'','23.9830262773641');
INSERT INTO "AnastKM302_blocks_io" VALUES('С102_1','Lж',0,'','4.01307081013585');
INSERT INTO "AnastKM302_blocks_io" VALUES('С102_1','ProcЖ',0,'','0.01');
INSERT INTO "AnastKM302_blocks_io" VALUES('С102_1','Vap',0,'','2.4');
INSERT INTO "AnastKM302_blocks_io" VALUES('С102_1','Q0',0,'','1');
INSERT INTO "AnastKM302_blocks_io" VALUES('С102_1','Qж',0,'','1000');
INSERT INTO "AnastKM302_blocks_io" VALUES('С102_1','f_frq',0,'','100');
INSERT INTO "AnastKM302_blocks_io" VALUES('С102_1','Pot',0,'','26.8150252942661');
INSERT INTO "AnastKM302_blocks_io" VALUES('С102_1','Fit',0,'','0.0136804199541132');
INSERT INTO "AnastKM302_blocks_io" VALUES('AT102_1','Fi',0,'','0.00043473293916917');
INSERT INTO "AnastKM302_blocks_io" VALUES('AT102_1','Pi',1,'KM102_1.Po','23.9733970795736');
INSERT INTO "AnastKM302_blocks_io" VALUES('AT102_1','Ti',1,'KM102_1.To','273.074151076625');
INSERT INTO "AnastKM302_blocks_io" VALUES('AT102_1','Si',0,'','0.015');
INSERT INTO "AnastKM302_blocks_io" VALUES('AT102_1','li',0,'','10');
INSERT INTO "AnastKM302_blocks_io" VALUES('AT102_1','Fo',1,'C102_2.Fi','-2.17139093002309e-05');
INSERT INTO "AnastKM302_blocks_io" VALUES('AT102_1','Po',0,'','23.9733744788882');
INSERT INTO "AnastKM302_blocks_io" VALUES('AT102_1','To',0,'','283.000000030802');
INSERT INTO "AnastKM302_blocks_io" VALUES('AT102_1','So',0,'','0.017');
INSERT INTO "AnastKM302_blocks_io" VALUES('AT102_1','lo',0,'','2');
INSERT INTO "AnastKM302_blocks_io" VALUES('AT102_1','Tair',0,'','283');
INSERT INTO "AnastKM302_blocks_io" VALUES('AT102_1','Wc',0,'','200');
INSERT INTO "AnastKM302_blocks_io" VALUES('AT102_1','Q0',0,'','1');
INSERT INTO "AnastKM302_blocks_io" VALUES('AT102_1','Ct',0,'','100');
INSERT INTO "AnastKM302_blocks_io" VALUES('AT102_1','Rt',0,'','1');
INSERT INTO "AnastKM302_blocks_io" VALUES('AT102_1','f_frq',0,'','100');
INSERT INTO "AnastKM302_blocks_io" VALUES('AT102_1','Pot',0,'','26.8152701962305');
INSERT INTO "AnastKM302_blocks_io" VALUES('AT102_1','Fit',0,'','-0.00362841690536941');
INSERT INTO "AnastKM302_blocks_io" VALUES('AT102_2','Fi',0,'','0.000276337949469788');
INSERT INTO "AnastKM302_blocks_io" VALUES('AT102_2','Pi',1,'KM102_2.Po','23.9733911063951');
INSERT INTO "AnastKM302_blocks_io" VALUES('AT102_2','Ti',1,'KM102_2.To','274.259663808314');
INSERT INTO "AnastKM302_blocks_io" VALUES('AT102_2','Si',0,'','0.007');
INSERT INTO "AnastKM302_blocks_io" VALUES('AT102_2','li',0,'','20');
INSERT INTO "AnastKM302_blocks_io" VALUES('AT102_2','Fo',1,'C102_3.Fi','1.85539373078663e-05');
INSERT INTO "AnastKM302_blocks_io" VALUES('AT102_2','Po',0,'','23.9734244300051');
INSERT INTO "AnastKM302_blocks_io" VALUES('AT102_2','To',0,'','282.999999820689');
INSERT INTO "AnastKM302_blocks_io" VALUES('AT102_2','So',0,'','0.00785');
INSERT INTO "AnastKM302_blocks_io" VALUES('AT102_2','lo',0,'','5');
INSERT INTO "AnastKM302_blocks_io" VALUES('AT102_2','Tair',0,'','283');
INSERT INTO "AnastKM302_blocks_io" VALUES('AT102_2','Wc',0,'','200');
INSERT INTO "AnastKM302_blocks_io" VALUES('AT102_2','Q0',0,'','1');
INSERT INTO "AnastKM302_blocks_io" VALUES('AT102_2','Ct',0,'','100');
INSERT INTO "AnastKM302_blocks_io" VALUES('AT102_2','Rt',0,'','1');
INSERT INTO "AnastKM302_blocks_io" VALUES('AT102_2','f_frq',0,'','100');
INSERT INTO "AnastKM302_blocks_io" VALUES('AT102_2','Pot',0,'','26.8286390373224');
INSERT INTO "AnastKM302_blocks_io" VALUES('AT102_2','Fit',0,'','-0.314505704700398');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ111','Fi',0,'','-1.23516411460312e-321');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ111','Pi',0,'','23.7062441394034');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ111','Ti',2,'Anast1to2node.КШ21.To','305.032383839542');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ111','Fo',1,'node5_1.Fi3','0.00016997724021743');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ111','Po',0,'','23.9733727495053');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ111','To',0,'','273.000000001701');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ111','So',0,'','0.0314');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ111','lo',0,'','5');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ111','S_kl1',0,'','0.0314');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ111','S_kl2',0,'','0.002');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ111','l_kl1',1,'КШ111им.pos','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ111','l_kl2',1,'КШ112им.pos','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ111','Q0',0,'','1');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ111','Kln',0,'','1');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ111','Kpr',0,'','0.95');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ111','Ct',0,'','20');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ111','Riz',0,'','20');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ111','noBack',0,'','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ111','Fwind',0,'','1');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ111','Twind',0,'','273');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ111','f_frq',0,'','100');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ111','Pot',0,'','26.8086695292603');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ111','Fit',0,'','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ113','Fi',0,'','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ113','Pi',1,'node6_1.Po4','23.9734197674852');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ113','Ti',0,'','273');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ113','Fo',1,'net102.Fi','-5.78867252708156e-07');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ113','Po',0,'','1.00000006287266');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ113','To',0,'','272.999999998982');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ113','So',0,'','0.00785');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ113','lo',0,'','10');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ113','S_kl1',0,'','0.00785');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ113','S_kl2',0,'','0.002');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ113','l_kl1',0,'','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ113','l_kl2',0,'','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ113','Q0',0,'','1');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ113','Kln',0,'','1');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ113','Kpr',0,'','0.95');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ113','Ct',0,'','20');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ113','Riz',0,'','20');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ113','noBack',0,'','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ113','Fwind',0,'','1');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ113','Twind',0,'','273');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ113','f_frq',0,'','100');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ113','Pot',0,'','1.00004217473475');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ113','Fit',0,'','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ114','Fi',0,'','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ114','Pi',1,'node6_1.Po3','23.9733847446881');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ114','Ti',1,'AT102_2.To','282.999999919046');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ114','Fo',0,'','0.000358990831083145');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ114','Po',0,'','92.291994026347');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ114','To',0,'','273.000000010185');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ114','So',0,'','0.00785');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ114','lo',0,'','100');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ114','S_kl1',0,'','0.00785');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ114','S_kl2',0,'','0.002');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ114','l_kl1',1,'КШ114им.pos','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ114','l_kl2',1,'КШ115им.pos','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ114','Q0',0,'','1');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ114','Kln',0,'','1');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ114','Kpr',0,'','0.95');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ114','Ct',0,'','20');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ114','Riz',0,'','20');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ114','noBack',0,'','1');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ114','Fwind',0,'','1');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ114','Twind',0,'','273');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ114','f_frq',0,'','100');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ114','Pot',0,'','91.8399723466015');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ114','Fit',0,'','2.47032822920623e-322');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ116','Fi',0,'','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ116','Pi',1,'node6_1.Po1','23.9733929531403');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ116','Ti',1,'AT102_2.To','283.000000108931');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ116','Fo',1,'DR101.Fi','5.01298411033204e-05');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ116','Po',0,'','23.9733206704524');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ116','To',0,'','273');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ116','So',0,'','0.0078');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ116','lo',0,'','1');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ116','S_kl1',0,'','0.0078');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ116','S_kl2',0,'','0.0078');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ116','l_kl1',1,'КШ116им.pos','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ116','l_kl2',0,'','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ116','Q0',0,'','1');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ116','Kln',0,'','1');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ116','Kpr',0,'','0.95');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ116','Ct',0,'','20');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ116','Riz',0,'','20');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ116','noBack',0,'','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ116','Fwind',0,'','1');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ116','Twind',0,'','273');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ116','f_frq',0,'','100');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ116','Pot',0,'','26.8956438310409');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ116','Fit',0,'','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('node5_1','Fi1',0,'','0.000456709238512286');
INSERT INTO "AnastKM302_blocks_io" VALUES('node5_1','Pi1',1,'DR101.Po','23.97339141377');
INSERT INTO "AnastKM302_blocks_io" VALUES('node5_1','Ti1',1,'DR101.To','272.999999999613');
INSERT INTO "AnastKM302_blocks_io" VALUES('node5_1','Si1',0,'','0.0314');
INSERT INTO "AnastKM302_blocks_io" VALUES('node5_1','Fi2',0,'','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('node5_1','Pi2',1,'KPP102.Po','23.9735548122435');
INSERT INTO "AnastKM302_blocks_io" VALUES('node5_1','Ti2',1,'KPP102.To','273.000003872772');
INSERT INTO "AnastKM302_blocks_io" VALUES('node5_1','Si2',0,'','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('node5_1','Fi3',0,'','0.000254283464895228');
INSERT INTO "AnastKM302_blocks_io" VALUES('node5_1','Pi3',1,'КШ111.Po','23.9733804297192');
INSERT INTO "AnastKM302_blocks_io" VALUES('node5_1','Ti3',1,'КШ111.To','273.000000001701');
INSERT INTO "AnastKM302_blocks_io" VALUES('node5_1','Si3',0,'','0.0314');
INSERT INTO "AnastKM302_blocks_io" VALUES('node5_1','Fo',1,'С102_1.Fi','-9.72090953341162e-05');
INSERT INTO "AnastKM302_blocks_io" VALUES('node5_1','Po',0,'','23.9733957567417');
INSERT INTO "AnastKM302_blocks_io" VALUES('node5_1','To',0,'','272.999999994377');
INSERT INTO "AnastKM302_blocks_io" VALUES('node5_1','So',0,'','0.2');
INSERT INTO "AnastKM302_blocks_io" VALUES('node5_1','lo',0,'','50');
INSERT INTO "AnastKM302_blocks_io" VALUES('node5_1','Q0',0,'','1');
INSERT INTO "AnastKM302_blocks_io" VALUES('node5_1','Ct',0,'','20');
INSERT INTO "AnastKM302_blocks_io" VALUES('node5_1','Riz',0,'','20');
INSERT INTO "AnastKM302_blocks_io" VALUES('node5_1','Fwind',0,'','1');
INSERT INTO "AnastKM302_blocks_io" VALUES('node5_1','Twind',0,'','273');
INSERT INTO "AnastKM302_blocks_io" VALUES('node5_1','f_frq',0,'','100');
INSERT INTO "AnastKM302_blocks_io" VALUES('node5_1','FoR',0,'','-0.00037523449854531');
INSERT INTO "AnastKM302_blocks_io" VALUES('node5_1','Pot',0,'','26.8150708842158');
INSERT INTO "AnastKM302_blocks_io" VALUES('node5_1','Fit',0,'','-0.360488757341288');
INSERT INTO "AnastKM302_blocks_io" VALUES('KM102_1','Fi',0,'','5.17895622590822e-05');
INSERT INTO "AnastKM302_blocks_io" VALUES('KM102_1','Pi',1,'PP103.Po','23.9733960752099');
INSERT INTO "AnastKM302_blocks_io" VALUES('KM102_1','Ti',0,'','273');
INSERT INTO "AnastKM302_blocks_io" VALUES('KM102_1','Fo',1,'AT102_1.Fi','-0.00147851591429675');
INSERT INTO "AnastKM302_blocks_io" VALUES('KM102_1','Po',0,'','23.9733961119432');
INSERT INTO "AnastKM302_blocks_io" VALUES('KM102_1','To',0,'','273.074151006399');
INSERT INTO "AnastKM302_blocks_io" VALUES('KM102_1','So',0,'','0.017');
INSERT INTO "AnastKM302_blocks_io" VALUES('KM102_1','lo',0,'','55');
INSERT INTO "AnastKM302_blocks_io" VALUES('KM102_1','Kzp',0,'','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('KM102_1','N',0,'','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('KM102_1','V',0,'','0.27');
INSERT INTO "AnastKM302_blocks_io" VALUES('KM102_1','Kpmp',0,'','0.02');
INSERT INTO "AnastKM302_blocks_io" VALUES('KM102_1','Kslp',0,'','0.2');
INSERT INTO "AnastKM302_blocks_io" VALUES('KM102_1','Q0',0,'','1');
INSERT INTO "AnastKM302_blocks_io" VALUES('KM102_1','Ct',0,'','100');
INSERT INTO "AnastKM302_blocks_io" VALUES('KM102_1','Riz',0,'','100');
INSERT INTO "AnastKM302_blocks_io" VALUES('KM102_1','Fwind',0,'','1');
INSERT INTO "AnastKM302_blocks_io" VALUES('KM102_1','Twind',0,'','273');
INSERT INTO "AnastKM302_blocks_io" VALUES('KM102_1','f_frq',0,'','100');
INSERT INTO "AnastKM302_blocks_io" VALUES('KM102_1','Pot',0,'','26.8147761231667');
INSERT INTO "AnastKM302_blocks_io" VALUES('KM102_1','Fit',0,'','-0.0114376695488128');
INSERT INTO "AnastKM302_blocks_io" VALUES('KM102_2','Fi',0,'','1.20435280144957e-05');
INSERT INTO "AnastKM302_blocks_io" VALUES('KM102_2','Pi',1,'PP104.Po','23.9733965104816');
INSERT INTO "AnastKM302_blocks_io" VALUES('KM102_2','Ti',1,'AT102_1.To','283.000000130298');
INSERT INTO "AnastKM302_blocks_io" VALUES('KM102_2','Fo',1,'AT102_2.Fi','-0.000587409470374164');
INSERT INTO "AnastKM302_blocks_io" VALUES('KM102_2','Po',0,'','23.9733992861061');
INSERT INTO "AnastKM302_blocks_io" VALUES('KM102_2','To',0,'','274.25966127261');
INSERT INTO "AnastKM302_blocks_io" VALUES('KM102_2','So',0,'','0.00785');
INSERT INTO "AnastKM302_blocks_io" VALUES('KM102_2','lo',0,'','55');
INSERT INTO "AnastKM302_blocks_io" VALUES('KM102_2','Kzp',0,'','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('KM102_2','N',1,'KM102_1.N','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('KM102_2','V',0,'','0.13');
INSERT INTO "AnastKM302_blocks_io" VALUES('KM102_2','Kpmp',0,'','0.02');
INSERT INTO "AnastKM302_blocks_io" VALUES('KM102_2','Kslp',0,'','0.2');
INSERT INTO "AnastKM302_blocks_io" VALUES('KM102_2','Q0',0,'','1');
INSERT INTO "AnastKM302_blocks_io" VALUES('KM102_2','Ct',0,'','100');
INSERT INTO "AnastKM302_blocks_io" VALUES('KM102_2','Riz',0,'','100');
INSERT INTO "AnastKM302_blocks_io" VALUES('KM102_2','Fwind',0,'','1');
INSERT INTO "AnastKM302_blocks_io" VALUES('KM102_2','Twind',0,'','273');
INSERT INTO "AnastKM302_blocks_io" VALUES('KM102_2','f_frq',0,'','100');
INSERT INTO "AnastKM302_blocks_io" VALUES('KM102_2','Pot',0,'','26.8256832047174');
INSERT INTO "AnastKM302_blocks_io" VALUES('KM102_2','Fit',0,'','-0.0212141726371546');
INSERT INTO "AnastKM302_blocks_io" VALUES('node6_1','Fi',0,'','-0.000472792848545461');
INSERT INTO "AnastKM302_blocks_io" VALUES('node6_1','Pi',1,'C102_3.Po','23.9733392842798');
INSERT INTO "AnastKM302_blocks_io" VALUES('node6_1','Fo1',1,'КШ116.Fi','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('node6_1','Po1',0,'','23.9733935816814');
INSERT INTO "AnastKM302_blocks_io" VALUES('node6_1','So1',0,'','0.00785');
INSERT INTO "AnastKM302_blocks_io" VALUES('node6_1','lo1',0,'','30');
INSERT INTO "AnastKM302_blocks_io" VALUES('node6_1','Fo2',1,'KPP102.Fi','6.41248014694431e-204');
INSERT INTO "AnastKM302_blocks_io" VALUES('node6_1','Po2',0,'','23.9734010510017');
INSERT INTO "AnastKM302_blocks_io" VALUES('node6_1','So2',0,'','0.002');
INSERT INTO "AnastKM302_blocks_io" VALUES('node6_1','lo2',0,'','30');
INSERT INTO "AnastKM302_blocks_io" VALUES('node6_1','Fo3',1,'КШ114.Fi','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('node6_1','Po3',0,'','23.9733756999676');
INSERT INTO "AnastKM302_blocks_io" VALUES('node6_1','So3',0,'','0.00785');
INSERT INTO "AnastKM302_blocks_io" VALUES('node6_1','lo3',0,'','5');
INSERT INTO "AnastKM302_blocks_io" VALUES('node6_1','Fo4',1,'КШ113.Fi','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('node6_1','Po4',0,'','23.9734016685866');
INSERT INTO "AnastKM302_blocks_io" VALUES('node6_1','So4',0,'','0.00785');
INSERT INTO "AnastKM302_blocks_io" VALUES('node6_1','lo4',0,'','10');
INSERT INTO "AnastKM302_blocks_io" VALUES('node6_1','Q0',0,'','1');
INSERT INTO "AnastKM302_blocks_io" VALUES('node6_1','f_frq',0,'','100');
INSERT INTO "AnastKM302_blocks_io" VALUES('node6_1','F1tmp',0,'','-8.3164854157918e-05');
INSERT INTO "AnastKM302_blocks_io" VALUES('node6_1','F2tmp',0,'','-9.34447079476621e-05');
INSERT INTO "AnastKM302_blocks_io" VALUES('node6_1','F3tmp',0,'','-0.00063881774433138');
INSERT INTO "AnastKM302_blocks_io" VALUES('node6_1','F4tmp',0,'','4.12947295034999e-05');
INSERT INTO "AnastKM302_blocks_io" VALUES('node6_1','Pot1',0,'','26.8225676097937');
INSERT INTO "AnastKM302_blocks_io" VALUES('node6_1','Pot2',0,'','26.1732230241585');
INSERT INTO "AnastKM302_blocks_io" VALUES('node6_1','Pot3',0,'','27.1570836553374');
INSERT INTO "AnastKM302_blocks_io" VALUES('node6_1','Pot4',0,'','26.4318196988532');
INSERT INTO "AnastKM302_blocks_io" VALUES('PP103','Fi',0,'','5.36120045167391e-05');
INSERT INTO "AnastKM302_blocks_io" VALUES('PP103','Pi',1,'С102_1.Po','23.9733948458531');
INSERT INTO "AnastKM302_blocks_io" VALUES('PP103','Fo',1,'KM102_1.Fi','-4.30228911853007e-05');
INSERT INTO "AnastKM302_blocks_io" VALUES('PP103','Po',0,'','23.9733953158747');
INSERT INTO "AnastKM302_blocks_io" VALUES('PP103','dP',0,'','6.64575849313905e-07');
INSERT INTO "AnastKM302_blocks_io" VALUES('PP103','Sdf',0,'','0.02');
INSERT INTO "AnastKM302_blocks_io" VALUES('PP103','So',0,'','0.0314');
INSERT INTO "AnastKM302_blocks_io" VALUES('PP103','lo',0,'','7');
INSERT INTO "AnastKM302_blocks_io" VALUES('PP103','Q0',0,'','1');
INSERT INTO "AnastKM302_blocks_io" VALUES('PP103','f_frq',0,'','100');
INSERT INTO "AnastKM302_blocks_io" VALUES('PP103','Pot',0,'','26.8149281286869');
INSERT INTO "AnastKM302_blocks_io" VALUES('PP103','Fit',0,'','0.0265948323266263');
INSERT INTO "AnastKM302_blocks_io" VALUES('PP104','Fi',0,'','-3.90074923861658e-07');
INSERT INTO "AnastKM302_blocks_io" VALUES('PP104','Pi',1,'C102_2.Po','23.9733958165468');
INSERT INTO "AnastKM302_blocks_io" VALUES('PP104','Fo',1,'KM102_2.Fi','-2.59942054117476e-05');
INSERT INTO "AnastKM302_blocks_io" VALUES('PP104','Po',0,'','23.9733955847248');
INSERT INTO "AnastKM302_blocks_io" VALUES('PP104','dP',0,'','5.47432729775341e-07');
INSERT INTO "AnastKM302_blocks_io" VALUES('PP104','Sdf',0,'','0.015');
INSERT INTO "AnastKM302_blocks_io" VALUES('PP104','So',0,'','0.017');
INSERT INTO "AnastKM302_blocks_io" VALUES('PP104','lo',0,'','7');
INSERT INTO "AnastKM302_blocks_io" VALUES('PP104','Q0',0,'','1');
INSERT INTO "AnastKM302_blocks_io" VALUES('PP104','f_frq',0,'','100');
INSERT INTO "AnastKM302_blocks_io" VALUES('PP104','Pot',0,'','27.0443769735342');
INSERT INTO "AnastKM302_blocks_io" VALUES('PP104','Fit',0,'','-1.63603222620315');
INSERT INTO "AnastKM302_blocks_io" VALUES('C102_2','Fi',0,'','7.41187580735482e-05');
INSERT INTO "AnastKM302_blocks_io" VALUES('C102_2','Pi',1,'AT102_1.Po','23.9735031281597');
INSERT INTO "AnastKM302_blocks_io" VALUES('C102_2','Si',0,'','0.017');
INSERT INTO "AnastKM302_blocks_io" VALUES('C102_2','Fo',1,'PP104.Fi','4.62270725190024e-07');
INSERT INTO "AnastKM302_blocks_io" VALUES('C102_2','Po',0,'','23.9733956287569');
INSERT INTO "AnastKM302_blocks_io" VALUES('C102_2','So',0,'','0.017');
INSERT INTO "AnastKM302_blocks_io" VALUES('C102_2','lo',0,'','50');
INSERT INTO "AnastKM302_blocks_io" VALUES('C102_2','Fo_ж',0,'','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('C102_2','Po_ж',0,'','23.9831001214316');
INSERT INTO "AnastKM302_blocks_io" VALUES('C102_2','Lж',0,'','4.85215940015719');
INSERT INTO "AnastKM302_blocks_io" VALUES('C102_2','ProcЖ',0,'','0.01');
INSERT INTO "AnastKM302_blocks_io" VALUES('C102_2','Vap',0,'','2');
INSERT INTO "AnastKM302_blocks_io" VALUES('C102_2','Q0',0,'','1');
INSERT INTO "AnastKM302_blocks_io" VALUES('C102_2','Qж',0,'','1000');
INSERT INTO "AnastKM302_blocks_io" VALUES('C102_2','f_frq',0,'','100');
INSERT INTO "AnastKM302_blocks_io" VALUES('C102_2','Pot',0,'','26.8110487968738');
INSERT INTO "AnastKM302_blocks_io" VALUES('C102_2','Fit',0,'','-0.0290302511841726');
INSERT INTO "AnastKM302_blocks_io" VALUES('C102_3','Fi',0,'','-0.000170528012423456');
INSERT INTO "AnastKM302_blocks_io" VALUES('C102_3','Pi',1,'AT102_2.Po','23.973423812648');
INSERT INTO "AnastKM302_blocks_io" VALUES('C102_3','Si',0,'','0.00785');
INSERT INTO "AnastKM302_blocks_io" VALUES('C102_3','Fo',1,'node6_1.Fi','0.000204665828467623');
INSERT INTO "AnastKM302_blocks_io" VALUES('C102_3','Po',0,'','23.9734555130246');
INSERT INTO "AnastKM302_blocks_io" VALUES('C102_3','So',0,'','0.00785');
INSERT INTO "AnastKM302_blocks_io" VALUES('C102_3','lo',0,'','8');
INSERT INTO "AnastKM302_blocks_io" VALUES('C102_3','Fo_ж',0,'','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('C102_3','Po_ж',0,'','23.9838330438217');
INSERT INTO "AnastKM302_blocks_io" VALUES('C102_3','Lж',0,'','6.91835386475179');
INSERT INTO "AnastKM302_blocks_io" VALUES('C102_3','ProcЖ',0,'','0.01');
INSERT INTO "AnastKM302_blocks_io" VALUES('C102_3','Vap',0,'','1.5');
INSERT INTO "AnastKM302_blocks_io" VALUES('C102_3','Q0',0,'','1');
INSERT INTO "AnastKM302_blocks_io" VALUES('C102_3','Qж',0,'','1000');
INSERT INTO "AnastKM302_blocks_io" VALUES('C102_3','f_frq',0,'','100');
INSERT INTO "AnastKM302_blocks_io" VALUES('C102_3','Pot',0,'','26.7868124450488');
INSERT INTO "AnastKM302_blocks_io" VALUES('C102_3','Fit',0,'','0.313336706308645');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ112им','pos',0,'','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('KPP102','t_kl1',0,'','3');
INSERT INTO "AnastKM302_blocks_io" VALUES('KPP102','t_kl2',0,'','5');
INSERT INTO "AnastKM302_blocks_io" VALUES('KPP102','tmp_l1',0,'','7.4109846876187e-322');
INSERT INTO "AnastKM302_blocks_io" VALUES('KPP102','tmp_l2',0,'','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('DR101','t_kl1',0,'','10');
INSERT INTO "AnastKM302_blocks_io" VALUES('DR101','t_kl2',0,'','5');
INSERT INTO "AnastKM302_blocks_io" VALUES('DR101','tmp_l1',0,'','49.9999999999964');
INSERT INTO "AnastKM302_blocks_io" VALUES('DR101','tmp_l2',0,'','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ114им','pos',0,'','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ116им','pos',0,'','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ111','t_kl1',0,'','10');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ111','t_kl2',0,'','5');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ111','tmp_l1',0,'','2.47032822920623e-321');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ111','tmp_l2',0,'','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ113','t_kl1',0,'','10');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ113','t_kl2',0,'','5');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ113','tmp_l1',0,'','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ113','tmp_l2',0,'','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ114','t_kl1',0,'','10');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ114','t_kl2',0,'','5');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ114','tmp_l1',0,'','2.47032822920623e-321');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ114','tmp_l2',0,'','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ116','t_kl1',0,'','10');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ116','t_kl2',0,'','5');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ116','tmp_l1',0,'','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ116','tmp_l2',0,'','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ111им','pos',0,'','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('node5_1','Kpr',0,'','0.95');
INSERT INTO "AnastKM302_blocks_io" VALUES('KM102_1','Kpr',0,'','0.95');
INSERT INTO "AnastKM302_blocks_io" VALUES('KM102_2','Kpr',0,'','0.95');
INSERT INTO "AnastKM302_blocks_io" VALUES('node6_1','Kpr',0,'','0.95');
INSERT INTO "AnastKM302_blocks_io" VALUES('PP103','Kpr',0,'','0.95');
INSERT INTO "AnastKM302_blocks_io" VALUES('PP104','Kpr',0,'','0.95');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ114им','com',0,'','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ114им','st_open',0,'','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ114им','st_close',0,'','1');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ114им','t_full',0,'','5');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ114им','t_up',0,'','0.5');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ114им','f_frq',0,'','100');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ114им','tmp_up',0,'','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ114им','lst_com',0,'','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ115им','pos',0,'','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ115им','com',0,'','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ115им','st_open',0,'','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ115им','st_close',0,'','1');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ115им','t_full',0,'','5');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ115им','t_up',0,'','0.5');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ115им','f_frq',0,'','100');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ115им','tmp_up',0,'','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ115им','lst_com',0,'','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ111им','com',0,'','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ111им','st_open',0,'','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ111им','st_close',0,'','1');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ111им','t_full',0,'','5');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ111им','t_up',0,'','0.5');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ111им','f_frq',0,'','100');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ111им','tmp_up',0,'','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ111им','lst_com',0,'','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ116им','com',0,'','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ116им','st_open',0,'','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ116им','st_close',0,'','1');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ116им','t_full',0,'','5');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ116им','t_up',0,'','0.5');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ116им','f_frq',0,'','100');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ116им','tmp_up',0,'','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ116им','lst_com',0,'','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ112им','com',0,'','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ112им','st_open',0,'','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ112им','st_close',0,'','1');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ112им','t_full',0,'','3');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ112им','t_up',0,'','0.3');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ112им','f_frq',0,'','100');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ112им','tmp_up',0,'','0');
INSERT INTO "AnastKM302_blocks_io" VALUES('КШ112им','lst_com',0,'','0');
CREATE TABLE 'AnastKM302_prm' ("SHIFR" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '' ,"IO" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"ru#IO" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"uk#IO" TEXT DEFAULT '' , PRIMARY KEY ("SHIFR"));
INSERT INTO "AnastKM302_prm" VALUES('KPP102','KPP302','Anti-surge control valve of the compressor',1,'KPP102.l_kl1:out:Output
KPP102.l_kl1:var:Variable
KPP102.l_kl1:varIn:Variable input
*s.%:ed:Dimension
*i.0:prec:Precision
*r.0:min:Minimum
*r.100:max:Maximum','','Антипомпажный регулирующий клапан компрессора','KPP102.l_kl1:out:Выход
KPP102.l_kl1:var:Переменная
KPP102.l_kl1:varIn:Ввод переменной
*s.%:ed:Ед.измерения
*i.0:prec:Точность
*r.0:min:Минимум
*r.100:max:Максимум','','Антипомпажний регулюючий клапан компресору','KPP102.l_kl1:out:Вихід
KPP102.l_kl1:var:Змінна
KPP102.l_kl1:varIn:Ввід змінної
*s.%:ed:Од.виміру
*i.0:prec:Точність
*r.0:min:Мінімум
*r.100:max:Максимум');
INSERT INTO "AnastKM302_prm" VALUES('F103','F303','Flow on the diaphragm PP303',1,'PP103.Fi:var:Variable
*s.tone/h:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.100:max:Maximum','','Расход на диафрагме PP303','PP103.Fi:var:Переменная
*s.т/ч:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.100:max:Максимум','','Витрати на діафрагмі PP303','PP103.Fi:var:Змінна
*s.т/год:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.100:max:Максимум');
INSERT INTO "AnastKM302_prm" VALUES('TT0202','TE1202_6','Temperature after the first stage of the compressor',1,'KM102_1.To:var:Variable
*s.К:ed:Dimension
*i.0:prec:Precision
*r.273:min:Minimum
*r.373:max:Maximum','','Температура после первой ступени компрессора','KM102_1.To:var:Переменная
*s.К:ed:Ед.измерения
*i.0:prec:Точность
*r.273:min:Минимум
*r.373:max:Максимум','','Температура після першої ступені компресора','KM102_1.To:var:Змінна
*s.К:ed:Од.виміру
*i.0:prec:Точність
*r.273:min:Мінімум
*r.373:max:Максимум');
INSERT INTO "AnastKM302_prm" VALUES('F104','F304','Flow on the diaphragm PP304',1,'PP104.Fi:var:Variable
*s.tone/h:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.100:max:Maximum','','Расход на диафрагме PP304','PP104.Fi:var:Переменная
*s.т/ч:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.100:max:Максимум','','Витрати на діафрагмі PP304','PP104.Fi:var:Змінна
*s.т/год:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.100:max:Максимум');
INSERT INTO "AnastKM302_prm" VALUES('TT0204','TE1205_6','Temperature after the second stage of the compressor',1,'KM102_2.To:var:Variable
*s.К:ed:Dimension
*i.0:prec:Precision
*r.273:min:Minimum
*r.423:max:Maximum','','Температура после второй ступени компрессора','KM102_2.To:var:Переменная
*s.К:ed:Ед.измерения
*i.0:prec:Точность
*r.273:min:Минимум
*r.423:max:Максимум','','Температура після другої ступені компресора','KM102_2.To:var:Змінна
*s.К:ed:Од.виміру
*i.0:prec:Точність
*r.273:min:Мінімум
*r.423:max:Максимум');
INSERT INTO "AnastKM302_prm" VALUES('TE1313_1','TE1313_6','The temperature at the input of the first stage of the compressor',1,'node5_1.To:var:Variable
*s.К:ed:Dimension
*i.0:prec:Precision
*r.273:min:Minimum
*r.373:max:Maximum','','Температура на входе первой ступени компрессора','node5_1.To:var:Переменная
*s.К:ed:Ед.измерения
*i.0:prec:Точность
*r.273:min:Минимум
*r.373:max:Максимум','','Температура на вході першої ступені компресора','node5_1.To:var:Змінна
*s.К:ed:Од.виміру
*i.0:prec:Точність
*r.273:min:Мінімум
*r.373:max:Максимум');
INSERT INTO "AnastKM302_prm" VALUES('PT0202','PT0202_6','Pressure after the first stage of the compressor',1,'KM102_1.Po:var:Variable
*s.at:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.75:max:Maximum','','Давление после первой ступени компрессора','KM102_1.Po:var:Переменная
*s.ат:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.75:max:Максимум','','Тиск після першої ступені компресору','KM102_1.Po:var:Змінна
*s.ат:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.75:max:Максимум');
INSERT INTO "AnastKM302_prm" VALUES('КШ111','KSH311','The control block of the ball valve KSH311',1,'КШ111им.com:com
КШ111им.st_open:st_open
КШ111им.st_close:st_close','КШ311','Блок управления шарового крана КШ311','','КШ311','Блок керування кульовим краном КШ311','');
INSERT INTO "AnastKM302_prm" VALUES('PT0204','PT0204_6','Pressure after the second stage of the compressor',1,'KM102_2.Po:var:Variable
*s.at:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.150:max:Maximum','','Давление после второй ступени компрессора','KM102_2.Po:var:Переменная
*s.ат:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.150:max:Максимум','','Тиск після другої ступені компресору','KM102_2.Po:var:Змінна
*s.ат:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.150:max:Максимум');
INSERT INTO "AnastKM302_prm" VALUES('КШ112','KSH312','The control block of the ball valve KSH312',1,'КШ112им.com:com
КШ112им.st_open:st_open
КШ112им.st_close:st_close','КШ312','Блок управления шарового крана КШ312','','КШ312','Блок керування кульовим краном КШ312','');
INSERT INTO "AnastKM302_prm" VALUES('КШ114','KSH314','The control block of the ball valve KSH314',1,'КШ114им.com:com
КШ114им.st_open:st_open
КШ114им.st_close:st_close','КШ314','Блок управления шарового крана КШ314','','КШ314','Блок керування кульовим краном КШ314','');
INSERT INTO "AnastKM302_prm" VALUES('КШ115','KSH315','The control block of the ball valve KSH315',1,'КШ115им.com:com
КШ115им.st_open:st_open
КШ115им.st_close:st_close','КШ315','Блок управления шарового крана КШ315','','КШ315','Блок керування кульовим краном КШ315','');
INSERT INTO "AnastKM302_prm" VALUES('КШ116','KSH316','The control block of the ball valve KSH316',1,'КШ116им.com:com
КШ116им.st_open:st_open
КШ116им.st_close:st_close','КШ316','Блок управления шарового крана КШ316','','КШ316','Блок керування кульовим краном КШ316','');
INSERT INTO "AnastKM302_prm" VALUES('TE1314_1','TE1314_6','The temperature after the fridge of the first stage of the compressor',1,'AT102_1.To:var:Variable
*s.К:ed:Dimension
*i.0:prec:Precision
*r.273:min:Minimum
*r.373:max:Maximum','','Температура после холодильника первой ступени компрессора','AT102_1.To:var:Переменная
*s.К:ed:Ед.измерения
*i.0:prec:Точность
*r.273:min:Минимум
*r.373:max:Максимум','','Температура після холодильника першої ступені компресора','AT102_1.To:var:Змінна
*s.К:ed:Од.виміру
*i.0:prec:Точність
*r.273:min:Мінімум
*r.373:max:Максимум');
INSERT INTO "AnastKM302_prm" VALUES('TE1314_2','TE1206_6','The temperature after the fridge of the second stage of the compressor',1,'AT102_2.To:var:Variable
*s.К:ed:Dimension
*i.0:prec:Precision
*r.273:min:Minimum
*r.373:max:Maximum','','Температура после холодильника второй ступени компрессора','AT102_2.To:var:Переменная
*s.К:ed:Ед.измерения
*i.0:prec:Точность
*r.273:min:Минимум
*r.373:max:Максимум','','Температура після холодильника другої ступені компресора','AT102_2.To:var:Змінна
*s.К:ed:Од.виміру
*i.0:prec:Точність
*r.273:min:Мінімум
*r.373:max:Максимум');
INSERT INTO "AnastKM302_prm" VALUES('PT1006_1','PT1006_6','Pressure on the diaphragm PP304',1,'PP104.Pi:var:Variable
*s.at:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.75:max:Maximum','','Давление на диафрагме PP304','PP104.Pi:var:Переменная
*s.ат:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.75:max:Максимум','','Тиск на діафрагмі PP304','PP104.Pi:var:Змінна
*s.ат:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.75:max:Максимум');
INSERT INTO "AnastKM302_prm" VALUES('P103','P303','Pressure on the diaphragm PP303',1,'PP103.Pi:var:Variable
*s.at:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.50:max:Maximum','','Давление на диафрагме PP303','PP103.Pi:var:Переменная
*s.ат:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.50:max:Максимум','','Тиск на діафрагмі PP303','PP103.Pi:var:Змінна
*s.ат:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.50:max:Максимум');
INSERT INTO "AnastKM302_prm" VALUES('ST8612','ST8612_6','Rotation speed of the compressor',1,'KM102_1.N:var:Variable
KM102_1.N:varIn:Variable input
*s.1000x rpm:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.15:max:Maximum','','Обороты вращения компрессора','KM102_1.N:var:Переменная
KM102_1.N:varIn:Ввод переменной
*s.1000x об.мин.:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.15:max:Максимум','','Оберти компресору','KM102_1.N:var:Змінна
KM102_1.N:varIn:Ввід змінної
*s.1000x об.хвил:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.15:max:Максимум');
CREATE TABLE 'DAQ_BlockCalc' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"ENABLE" INTEGER DEFAULT '0' ,"START" INTEGER DEFAULT '0' ,"MESS_LEV" INTEGER DEFAULT '3' ,"REDNT" INTEGER DEFAULT '0' ,"REDNT_RUN" TEXT DEFAULT '<high>' ,"PRM_BD" TEXT DEFAULT 'system' ,"BLOCK_SH" TEXT DEFAULT 'block' ,"PERIOD" INTEGER DEFAULT '0' ,"SCHEDULE" TEXT DEFAULT '1' ,"PRIOR" INTEGER DEFAULT '0' ,"ITER" INTEGER DEFAULT '1' , PRIMARY KEY ("ID"));
INSERT INTO "DAQ_BlockCalc" VALUES('Anast1to2node','Mainstation','Загальностанційка','Общестанционка','Contains the model of general part of the compressor station, namely, all that is shown on the principle scheme outside of the gray blocks of individual compressors.','Містить модель загальної частини компресорної станції, а саме все що зображено на принциповій схемі за межами сірих блоків окремих компресорів.','Содержит модель общей части компрессорной станции, а именно всё что изображено на принципиальной схеме за пределами серых блоков отдельных компрессоров.',1,1,3,0,'<high>','AnastGener_prm','AnastGener_blcks',10,'1',0,1);
INSERT INTO "DAQ_BlockCalc" VALUES('KM102cntr','','','','','','
',1,1,3,0,'<high>','AnastKM102cntr_prm','AnastKM102cntr_blocks',1000,'1',0,1);
INSERT INTO "DAQ_BlockCalc" VALUES('Anast1to2node_cntr','Mainstation (controller)','Загальностанційка (контролер)','Общестанционка (контроллер)','Contains the model of the control system of the mainstation part.','Містить модель системи керування загальностанційною частиною.','Содержит модель системы управления общестанционной частью.',1,1,3,0,'<high>','AnastGener_cntr_prm','AnastGener_cntr_blocks',0,'0.2',0,1);
INSERT INTO "DAQ_BlockCalc" VALUES('KM101','The model of КМ101','Модель КМ101','Модель КМ101','Contains the model of the first compressor KМ101 of low pressure.','Містить модель першого компресора низького тиску КМ101.','Содержит модель первого компрессора низкого давления КМ101.',1,1,3,0,'<high>','AnastKM101_prm','AnastKM101_blocks',10,'1',0,1);
INSERT INTO "DAQ_BlockCalc" VALUES('KM102','The model of КМ102','Модель КМ102','Модель КМ102','Contains the model of the first compressor KМ102 of high pressure.','Містить модель першого компресора високого тиску КМ102.','Содержит модель первого компрессора высокого давления КМ102.',1,1,3,0,'<high>','AnastKM102_prm','AnastKM102_blocks',10,'1',0,1);
INSERT INTO "DAQ_BlockCalc" VALUES('KM201','The model of КМ201','Модель КМ201','Модель КМ201','Contains the model of the second compressor KМ201 of low pressure.','Містить модель другого компресора низького тиску КМ201.','Содержит модель второго компрессора низкого давления КМ201.',1,1,3,0,'<high>','AnastKM201_prm','AnastKM201_blocks',10,'1',0,1);
INSERT INTO "DAQ_BlockCalc" VALUES('КМ202','The model of КМ202','Модель КМ202','Модель КМ202','Contains the model of the second compressor KМ202 of high pressure.','Містить модель другого компресора високого тиску КМ202.','Содержит модель второго компрессора высокого давления КМ202.',1,1,3,0,'<high>','AnastKM202_prm','AnastKM202_blocks',10,'1',0,1);
INSERT INTO "DAQ_BlockCalc" VALUES('KM301','The model of КМ301','Модель КМ301','Модель КМ301','Contains the model of the third compressor KМ301 of low pressure.','Містить модель третього компресора низького тиску КМ301.','Содержит модель третьего компрессора низкого давления КМ301.',1,1,3,0,'<high>','AnastKM301_prm','AnastKM301_blocks',10,'1',0,1);
INSERT INTO "DAQ_BlockCalc" VALUES('KM302','The model of КМ302','Модель КМ302','Модель КМ302','Contains the model of the third compressor KМ302 of high pressure.','Містить модель третього компресора високого тиску КМ302.','Содержит модель третьего компрессора высокого давления КМ302.',1,1,3,0,'<high>','AnastKM302_prm','AnastKM302_blocks',10,'1',0,1);
CREATE TABLE 'AnastGener_blcks' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"FUNC" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '0' ,"PROC" INTEGER DEFAULT '0' ,"PRIOR" TEXT DEFAULT '' ,"LNK_OUT_WR_CH" INTEGER DEFAULT '0' , PRIMARY KEY ("ID"));
INSERT INTO "AnastGener_blcks" VALUES('node1_1','Node 1.1','Вузол 1.1','Узел 1.1','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_pipe1_3',1,1,'',0);
INSERT INTO "AnastGener_blcks" VALUES('КШ22им','KSH22 (control mechanism)','КШ22 (вик. механізм)','КШ22 (исп. механизм)','Control mechanism of ball crane КSH22.','Виконавчий механізм кульового крану КШ22.','Исполнительный механизм шарового крана КШ22.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "AnastGener_blcks" VALUES('netGVD','Net of GHP','Мережа ГВТ','Сеть ГВД','','','','DAQ.JavaLikeCalc.lib_techApp.net',1,1,'',0);
INSERT INTO "AnastGener_blcks" VALUES('КШ21','KSH21','КШ21','КШ21','Ball crane armature for KSH21 и KSH22','Арматура кульових кранів КШ21 та КШ22','Арматура шаровых кранов КШ21 и КШ22','DAQ.JavaLikeCalc.lib_techApp.fnc_klap',1,1,'',0);
INSERT INTO "AnastGener_blcks" VALUES('noisePP3','PP3 noise','Шум PP3','Шум PP3','Flow source noise for diaphragms PP3','Шум джерела витрат діафрагми PP3','Шум источника расхода диафрагмы PP3','DAQ.JavaLikeCalc.lib_techApp.fnc_noise',1,1,'',0);
INSERT INTO "AnastGener_blcks" VALUES('КРД2им','KRD2 (control mechanism)','КРД2 (вик. механізм)','КРД2 (исп. механизм)','','','','DAQ.JavaLikeCalc.lib_techApp.fnc_lagClean',1,1,'',0);
INSERT INTO "AnastGener_blcks" VALUES('netGlinsk','Glinsk (net)','Глінск (мережа)','Глинск (сеть)','','','','DAQ.JavaLikeCalc.lib_techApp.fnc_net',1,1,'',0);
INSERT INTO "AnastGener_blcks" VALUES('КШ6им','KSH6 (control mechanism)','КШ6 (вик. механізм)','КШ6 (исп. механизм)','Control mechanism of ball crane KSH6.','Виконавчий механізм кульового крану КШ6.','Исполнительный механизм шарового крана КШ6.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "AnastGener_blcks" VALUES('P_КРД2','P_KRD2 (Pressure sensor)','P КРД2 (Давач тиску)','P КРД2 (Датчик давления)','','','','DAQ.JavaLikeCalc.lib_techApp.fnc_lag',1,1,'',0);
INSERT INTO "AnastGener_blcks" VALUES('SrcGas','','','','Gas source to compressor station','Джерело газу на компресорну станцію.','Источник газа на компрессорную станцию','DAQ.JavaLikeCalc.lib_techApp.src_press',1,1,'',0);
INSERT INTO "AnastGener_blcks" VALUES('КРД1','KRD1','КРД1','КРД1','The crane for regulation the input pressure of  the compressor low pressure.','Клапан, який регулює вхідний тиск на всасі компресорів низького тиску.','Клапан, регулирующий входное давление на всасе компресоров низкого давления.','DAQ.JavaLikeCalc.lib_techApp.fnc_klap',1,1,'',0);
INSERT INTO "AnastGener_blcks" VALUES('pipeGlinsk','Pipe to Glinsk','Труба на Глінск','Труба на Глинск','','','','DAQ.JavaLikeCalc.lib_techApp.fnc_pipe1_1',1,1,'',0);
INSERT INTO "AnastGener_blcks" VALUES('КРД2','KRD2','КРД2','КРД2','','','','DAQ.JavaLikeCalc.lib_techApp.fnc_klap',1,1,'',0);
INSERT INTO "AnastGener_blcks" VALUES('КРД4','KRD4','КРД4','KRD4','The control crane on outgo to GHP.','Регулюючий клапан на виході у ГВТ.','Регулирующий клапан на выходе в ГВД.','DAQ.JavaLikeCalc.lib_techApp.klap',1,1,'',0);
INSERT INTO "AnastGener_blcks" VALUES('КШ7им','KSH7 (control mechanism)','КШ7 (вик. механізм)','КШ7 (исп. механизм)','Control mechanism of ball crane KSH7.','Виконавчий механізм кульового крану КШ7.','Исполнительный механизм шарового крана КШ7.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "AnastGener_blcks" VALUES('PP1','','','','Flow measure point based on diaphragm.','Вузол виміру витрат на основі діафрагми.','Узел измерения расхода на основе диафрагмы.','DAQ.JavaLikeCalc.lib_techApp.diafragma',1,1,'',0);
INSERT INTO "AnastGener_blcks" VALUES('PP3','PP3','PP3','','Flow measure point based on diaphragm.','Вимірювальна діафрагма на вході у вузол.','Измерительная диафрагма на входе в узел.','DAQ.JavaLikeCalc.lib_techApp.fnc_diafragma',1,1,'',0);
INSERT INTO "AnastGener_blcks" VALUES('PP5','PP5','PP5','','','','','DAQ.JavaLikeCalc.lib_techApp.fnc_diafragma',1,1,'',0);
INSERT INTO "AnastGener_blcks" VALUES('PP7','','','','','','','DAQ.JavaLikeCalc.lib_techApp.diafragma',1,1,'',0);
INSERT INTO "AnastGener_blcks" VALUES('netFakel','Torch (net)','Факел (мережа)','Факел (сеть)','Torch net.','Факельна мережа.','Факеьная сеть.','DAQ.JavaLikeCalc.lib_techApp.net',1,1,'',0);
INSERT INTO "AnastGener_blcks" VALUES('КШ1','KSH1','КШ1','КШ1','The ball crane for getting gas to the station.','Кульові крани подачі газу на станцію.','Шаровые краны подачи газа на станцию.','DAQ.JavaLikeCalc.lib_techApp.klap',1,1,'',0);
INSERT INTO "AnastGener_blcks" VALUES('КШ21им','KSH21 (control mechanism)','КШ21 (вик. механізм)','КШ21 (исп. механизм)','Control mechanism of ball crane KSH21.','Виконавчий механізм кульового крану КШ21.','Исполнительный механизм шарового крана КШ21.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "AnastGener_blcks" VALUES('КШ5','KSH5','КШ5','КШ5','Ball crane for dropping gas to torch.','Кульовий кран для скидання вхідного газу на факел.','Шаровый кран для сброса входного газа на факел.','DAQ.JavaLikeCalc.lib_techApp.klap',1,1,'',0);
INSERT INTO "AnastGener_blcks" VALUES('КШ6','KSH6','КШ6','КШ6','Ball cranes KSH6 and KSH7 armature','Арматура кульових кранів КШ6 та КШ7','Арматура шаровых кранов КШ6 и КШ7','DAQ.JavaLikeCalc.lib_techApp.fnc_klap',1,1,'',0);
INSERT INTO "AnastGener_blcks" VALUES('node1','Node 1','Вузол 1','Узел 1','Node to torch and compressor low pressure.','Вузол розгалуження на факел та компресор низького тиску.','Узел разветвления на факел и компрессор низкого давления.','DAQ.JavaLikeCalc.lib_techApp.pipe1_2',1,1,'',0);
INSERT INTO "AnastGener_blcks" VALUES('node20','Node 20','Вузол 20','Узел 20','','','','DAQ.JavaLikeCalc.lib_techApp.fnc_pipe1_2',1,1,'',0);
INSERT INTO "AnastGener_blcks" VALUES('node21','Node 21','Вузол 21','Узел 21','Node before CLP.','Вузол перед КНТ.','Узел перед КНД.','DAQ.JavaLikeCalc.lib_techApp.fnc_pipe1_3',1,1,'',0);
INSERT INTO "AnastGener_blcks" VALUES('node22','Node 22','Вузол 22','Узел 22','Flows from compresors CHP to net of GHP.','Потоки з компресору КВТ у мережу ГВТ.','Потоки с компресоров КВД в сеть ГВД.','DAQ.JavaLikeCalc.lib_techApp.pipe3_1',1,1,'',0);
INSERT INTO "AnastGener_blcks" VALUES('node19','Node 19','Вузол 19','Узел 19','','','','DAQ.JavaLikeCalc.lib_techApp.pipe3_1',1,1,'',0);
INSERT INTO "AnastGener_blcks" VALUES('C1','Separator C1','Сепаратор C1','Сепаратор C1','Liquid fraction droping from gas to station.','Сепаратор відгону рідинної фракції від газу який входить на станцію.','Сепаратор отгона жидкой фракции от газа входящего на станцию.','DAQ.JavaLikeCalc.lib_techApp.separator',1,1,'',0);
INSERT INTO "AnastGener_blcks" VALUES('C2','Separator C2','Сепаратор C2','Сепаратор C2','','','','DAQ.JavaLikeCalc.lib_techApp.fnc_separator',1,1,'',0);
INSERT INTO "AnastGener_blcks" VALUES('КШ1им','KSH1 (control mechanism)','КШ1 (вик. механізм)','КШ1 (исп. механизм)','Control mechanism of ball crane KSH1.','Виконавчий механізм кульового крану КШ1.','Исполнительный механизм шарового крана КШ1.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "AnastGener_blcks" VALUES('КШ2им','KSH2 (control mechanism)','КШ2 (вик. механізм)','КШ2 (исп. механизм)','Control mechanism of ball crane KSH2.','Виконавчий механізм кульового крану КШ2.','Исполнительный механизм шарового крана КШ2.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "AnastGener_blcks" VALUES('КШ5им','KSH5 (control mechanism)','КШ5 (вик. механізм)','КШ5 (исп. механизм)','Control mechanism of ball crane KSH5.','Виконавчий механізм кульового крану КШ5.','Исполнительный механизм шарового крана КШ5.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
CREATE TABLE 'AnastGener_cntr_blocks' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"FUNC" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '0' ,"PROC" INTEGER DEFAULT '0' ,"PRIOR" TEXT DEFAULT '' ,"LNK_OUT_WR_CH" INTEGER DEFAULT '0' , PRIMARY KEY ("ID"));
INSERT INTO "AnastGener_cntr_blocks" VALUES('PC_КРД2','PC KRD2','PC КРД2','PC КРД2','','','','Special.FLibComplex1.fnc_pid',1,1,'',0);
INSERT INTO "AnastGener_cntr_blocks" VALUES('PCKRD1','PC KRD1','РС КРД1','РС КРД1','','','','Special.FLibComplex1.pid',1,1,'',0);
INSERT INTO "AnastGener_cntr_blocks" VALUES('PCKRD3','PC KRD3','РС КРД3','РС КРД3','','','','Special.FLibComplex1.pid',1,1,'',0);
INSERT INTO "AnastGener_cntr_blocks" VALUES('SurgeKM101','KM101 surge','KM101 помпаж','KM101 помпаж','','','','Special.FLibComplex1.pid',1,1,'',0);
INSERT INTO "AnastGener_cntr_blocks" VALUES('SurgeKM201','KM201 surge','KM201 помпаж','KM201 помпаж','','','','Special.FLibComplex1.pid',1,1,'',0);
INSERT INTO "AnastGener_cntr_blocks" VALUES('SurgeKM301','KM301 surge','KM301 помпаж','KM301 помпаж','','','','Special.FLibComplex1.pid',1,1,'',0);
INSERT INTO "AnastGener_cntr_blocks" VALUES('SurgeKM102','KM102 surge','KM102 помпаж','KM102 помпаж','','','','Special.FLibComplex1.pid',1,1,'',0);
INSERT INTO "AnastGener_cntr_blocks" VALUES('SurgeKM202','KM202 surge','KM202 помпаж','KM202 помпаж','','','','Special.FLibComplex1.pid',1,1,'',0);
INSERT INTO "AnastGener_cntr_blocks" VALUES('SurgeKM302','KM302 surge','KM302 помпаж','KM302 помпаж','','','','Special.FLibComplex1.pid',1,1,'',0);
CREATE TABLE 'AnastKM101_blocks' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"FUNC" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '0' ,"PROC" INTEGER DEFAULT '0' ,"PRIOR" TEXT DEFAULT '' ,"LNK_OUT_WR_CH" INTEGER DEFAULT '0' , PRIMARY KEY ("ID"));
INSERT INTO "AnastKM101_blocks" VALUES('КМ101_1','Compressor КМ101 stage 1','Ступ. 1 компр. КМ101','Ступ. 1 компр. КМ101','Stage one of compressor КМ101.','Перша ступінь компресора КМ101.','Первая ступень компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.compressor',1,1,'',0);
INSERT INTO "AnastKM101_blocks" VALUES('КМ101_2','Compressor КМ101 stage 2','Ступ. 2 компр. КМ101','Ступ. 2 компр. КМ101','Stage two of compressor КМ101.','Друга ступінь компресора КМ101.','Вторая ступень компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.compressor',1,1,'',0);
INSERT INTO "AnastKM101_blocks" VALUES('KPP101','','','','Surge protect control crane of compressor КМ101.','Протипомпажний регулюючий клапан компресору КМ101.','Антипомпажный регулирующий клапан компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.fnc_klap',1,1,'',0);
INSERT INTO "AnastKM101_blocks" VALUES('DR100','Baffle 100','Дросель 100','Дросель 100','','','','DAQ.JavaLikeCalc.lib_techApp.klap',1,1,'',0);
INSERT INTO "AnastKM101_blocks" VALUES('AT101_1','Air cooler AT101_1','Пов. охолод. AT101_1','Возд. охлад. AT101_1','The air cooler for gas after compressor КМ101 stage one.','Охолоджувач газу після першої ступені компресору КМ101.','Охладитель газа после первой ступени компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.cooler',1,1,'',0);
INSERT INTO "AnastKM101_blocks" VALUES('AT101_2','Air cooler AT101_2','Пов. охолод. AT101_2','Возд. охлад. AT101_2','The air cooler for gas after compressor КМ101 stage two.','Охолоджувач газу після другої ступені компресора КМ101.','Охладитель газа после второй ступени компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.cooler',1,1,'',0);
INSERT INTO "AnastKM101_blocks" VALUES('node3_1','Node 3.1','Вузол 3.1','Узел 3.1','','','','DAQ.JavaLikeCalc.lib_techApp.pipe3_1',1,1,'',0);
INSERT INTO "AnastKM101_blocks" VALUES('КШ101','KSH101','КШ101','КШ101','Ball crane for compressor КМ101 enable.','Кульовий кран підключення компресора КМ101.','Шаровый кран подключения компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.klap',1,1,'',0);
INSERT INTO "AnastKM101_blocks" VALUES('КШ104','KSH104','КШ104','КШ104','Ball crane for compressor КМ101 enable to net.','Кульовий кран підключення компресора КМ101 у мережу.','Шаровый кран подключения компрессора КМ101 в сеть.','DAQ.JavaLikeCalc.lib_techApp.klap',1,1,'',0);
INSERT INTO "AnastKM101_blocks" VALUES('node4_1','Node 4.1','Вузол 4.1','Узел 4.1','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_pipe1_3',1,1,'',0);
INSERT INTO "AnastKM101_blocks" VALUES('КШ106','KSH106','КШ106','КШ106','Surge protect ball crane of compressor КМ101.','Протипомпажний шаровий кран компресора КМ101.','Антипомпажный шаровый кран компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.klap',1,1,'',0);
INSERT INTO "AnastKM101_blocks" VALUES('C101_1','Separator C101/1','Сепаратор C101/1','Сепаратор C101/1','Liquid fraction droping before compressor КМ101 stage one.','Сепаратор відгону рідини перед першим ступенем компресора КМ101.','Сепаратор отгона жидкости перед первой ступенью компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.separator',1,1,'',0);
INSERT INTO "AnastKM101_blocks" VALUES('C101_2','Separator C101/2','Сепаратор C101/2','Сепаратор C101/2','Liquid fraction droping after compressor КМ101 stage one.','Сепаратор відгону рідини після першого ступеня компресора КМ101.','Сепаратор отгона жидкости после первой ступени компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.separator',1,1,'',0);
INSERT INTO "AnastKM101_blocks" VALUES('C101_3','Separator C101/3','Сепаратор C101/3','Сепаратор C101/3','Liquid fraction droping after compressor КМ101 stage two.','Сепаратор відгону рідини після другого ступеня компресора КМ101.','Сепаратор отгона жидкости после второй ступени компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.separator',1,1,'',0);
INSERT INTO "AnastKM101_blocks" VALUES('PP101','','','','Flow control point, diaphragm type, for first stage of compressor КМ101.','Вузол виміру витрат на першому ступені компресора КМ101 діафрагменого типу.','Узел замера расхода на первую ступень компрессора КМ101 диафрагменного типа.','DAQ.JavaLikeCalc.lib_techApp.diafragma',1,1,'',0);
INSERT INTO "AnastKM101_blocks" VALUES('PP102','','','','Flow control point, diaphragm type, for second stage of compressor КМ101.','Вузол виміру витрат на другому ступені компресора КМ101 діафрагменого типу.','Узел замера расхода на вторую ступень компрессора КМ101 диафрагменного типа.','DAQ.JavaLikeCalc.lib_techApp.diafragma',1,1,'',0);
INSERT INTO "AnastKM101_blocks" VALUES('КШ102им','KSH102 (control mechanism)','КШ102 (вик. механізм)','КШ102 (исп. механизм)','Control mechanism of ball crane КSH102.','Виконавчий механізм кульового крану КШ102.','Исполнительный механизм шарового крана КШ102.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "AnastKM101_blocks" VALUES('КШ101им','KSH101 (control mechanism)','КШ101 (вик. механізм)','КШ101 (исп. механизм)','Control mechanism of ball crane КSH101.','Виконавчий механізм кульового крану КШ101.','Исполнительный механизм шарового крана КШ101.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "AnastKM101_blocks" VALUES('КШ104им','KSH104 (control mechanism)','КШ104 (вик. механізм)','КШ104 (исп. механизм)','Control mechanism of ball crane КSH104.','Виконавчий механізм кульового крану КШ104.','Исполнительный механизм шарового крана КШ104.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "AnastKM101_blocks" VALUES('КШ105им','KSH105 (control mechanism)','КШ105 (вик. механізм)','КШ105 (исп. механизм)','Control mechanism of ball crane КSH105.','Виконавчий механізм кульового крану КШ105.','Исполнительный механизм шарового крана КШ105.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "AnastKM101_blocks" VALUES('КШ106им','KSH106 (control mechanism)','КШ106 (вик. механізм)','КШ106 (исп. механизм)','Control mechanism of ball crane КSH106.','Виконавчий механізм кульового крану КШ106.','Исполнительный механизм шарового крана КШ106.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
CREATE TABLE 'AnastKM102_blocks' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"FUNC" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '0' ,"PROC" INTEGER DEFAULT '0' ,"PRIOR" TEXT DEFAULT '' ,"LNK_OUT_WR_CH" INTEGER DEFAULT '0' , PRIMARY KEY ("ID"));
INSERT INTO "AnastKM102_blocks" VALUES('KPP102','','','','Surge protect control crane of compressor КМ102.','Протипомпажний регулюючий клапан компресору КМ102.','Антипомпажный регулирующий клапан компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.fnc_klap',1,1,'',0);
INSERT INTO "AnastKM102_blocks" VALUES('net102','','','','','','','DAQ.JavaLikeCalc.lib_techApp.fnc_net',1,1,'',0);
INSERT INTO "AnastKM102_blocks" VALUES('DR101','Baffle 101','Дросель 101','Дросель 101','','','','DAQ.JavaLikeCalc.lib_techApp.klap',1,1,'',0);
INSERT INTO "AnastKM102_blocks" VALUES('С102_1','Separator C102/1','Сепаратор С102/1','Сепаратор С102/1','Liquid fraction droping before compressor КМ102 stage one.','Сепаратор відгону рідини перед першим ступенем компресора КМ102.','Сепаратор отгона жидкости перед первой ступенью компрессора КМ102.','DAQ.JavaLikeCalc.lib_techApp.separator',1,1,'',0);
INSERT INTO "AnastKM102_blocks" VALUES('AT102_1','Air cooler AT102_1','Пов. охолод. AT102_1','Возд. охлад. AT102_1','The air cooler for gas after compressor КМ102 stage one.','Охолоджувач газу після першої ступені компресору КМ102.','Охладитель газа после первой ступени компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.cooler',1,1,'',0);
INSERT INTO "AnastKM102_blocks" VALUES('AT102_2','Air cooler AT102_2','Пов. охолод. AT102_2','Возд. охлад. AT102_2','The air cooler for gas after compressor КМ102 stage two.','Охолоджувач газу після другої ступені компресора КМ102.','Охладитель газа после второй ступени компрессора КМ102.','DAQ.JavaLikeCalc.lib_techApp.cooler',1,1,'',0);
INSERT INTO "AnastKM102_blocks" VALUES('КШ111','KSH111','КШ111','КШ111','Ball crane for compressor КМ102 enable.','Кульовий кран підключення компресору КМ102.','Шаровый кран подключения компрессора КМ102.','DAQ.JavaLikeCalc.lib_techApp.klap',1,1,'',0);
INSERT INTO "AnastKM102_blocks" VALUES('КШ113','KSH113','КШ113','КШ113','','','','DAQ.JavaLikeCalc.lib_techApp.fnc_klap',1,1,'',0);
INSERT INTO "AnastKM102_blocks" VALUES('КШ114','KSH114','КШ114','КШ114','Ball crane for compressor КМ102 enable to net.','Кульовий кран підключення компресору КМ102 у мережу.','Шаровый кран подключения компрессора КМ101 в сеть.','DAQ.JavaLikeCalc.lib_techApp.klap',1,1,'',0);
INSERT INTO "AnastKM102_blocks" VALUES('КШ116','KSH116','КШ116','КШ116','Surge protect ball crane of compressor КМ102.','Протипомпажний шаровий кран компресора КМ102.','Антипомпажный шаровый кран компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.klap',1,1,'',0);
INSERT INTO "AnastKM102_blocks" VALUES('node5_1','Node 5.1','Вузол 5.1','Узел 5.1','','','','DAQ.JavaLikeCalc.lib_techApp.pipe3_1',1,1,'',0);
INSERT INTO "AnastKM102_blocks" VALUES('KM102_1','Compressor КМ102 stage 1','Ступ. 1 компр. КМ102','Ступ. 1 компр. КМ102','Stage one of compressor КМ102.','Перша ступінь компресора КМ102.','Первая ступень компрессора КМ102.','DAQ.JavaLikeCalc.lib_techApp.compressor',1,1,'',0);
INSERT INTO "AnastKM102_blocks" VALUES('KM102_2','Compressor КМ102 stage 2','Ступ. 2 компр. КМ102','Ступ. 2 компр. КМ102','Stage two of compressor КМ102.','Друга ступінь компресора КМ102.','Вторая ступень компрессора КМ102.','DAQ.JavaLikeCalc.lib_techApp.compressor',1,1,'',0);
INSERT INTO "AnastKM102_blocks" VALUES('node6_1','Node 6.1','Вузол 6.1','Узел 6.1','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_pipe1_4',1,1,'',0);
INSERT INTO "AnastKM102_blocks" VALUES('PP103','','','','Flow control point, diaphragm type, for first stage of compressor КМ102.','Вузол виміру витрат на першому ступені компресора КМ102 діафрагменого типу.','Узел замера расхода на первую ступень компрессора КМ102 диафрагменного типа.','DAQ.JavaLikeCalc.lib_techApp.diafragma',1,1,'',0);
INSERT INTO "AnastKM102_blocks" VALUES('PP104','','','','Flow control point, diaphragm type, for second stage of compressor КМ102.','Вузол виміру витрат на другому ступені компресора КМ102 діафрагменого типу.','Узел замера расхода на вторую ступень компрессора КМ102 диафрагменного типа.','DAQ.JavaLikeCalc.lib_techApp.diafragma',1,1,'',0);
INSERT INTO "AnastKM102_blocks" VALUES('C102_2','Separator C102/2','Сепаратор C102/2','Сепаратор C102/2','Liquid fraction droping after compressor КМ102 stage one.','Сепаратор відгону рідини після першого ступеня компресора КМ102.','Сепаратор отгона жидкости после первой ступени компрессора КМ102.','DAQ.JavaLikeCalc.lib_techApp.separator',1,1,'',0);
INSERT INTO "AnastKM102_blocks" VALUES('C102_3','Separator C102/3','Сепаратор C102/3','Сепаратор C102/3','Liquid fraction droping after compressor КМ102 stage two.','Сепаратор відгону рідини після другого ступеня компресора КМ102.','Сепаратор отгона жидкости после второй ступени компрессора КМ102.','DAQ.JavaLikeCalc.lib_techApp.separator',1,1,'',0);
INSERT INTO "AnastKM102_blocks" VALUES('КШ111им','KSH111 (control mechanism)','КШ111 (вик. механізм)','КШ111 (исп. механизм)','Control mechanism of ball crane КSH111.','Виконавчий механізм кульового крану КШ101.','Исполнительный механизм шарового крана КШ101.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "AnastKM102_blocks" VALUES('КШ112им','KSH112 (control mechanism)','КШ112 (вик. механізм)','КШ112 (исп. механизм)','Control mechanism of ball crane КSH112.','Виконавчий механізм кульового крану КШ102.','Исполнительный механизм шарового крана КШ102.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "AnastKM102_blocks" VALUES('КШ114им','KSH114 (control mechanism)','КШ114 (вик. механізм)','КШ114 (исп. механизм)','Control mechanism of ball crane КSH114.','Виконавчий механізм кульового крану КШ104.','Исполнительный механизм шарового крана КШ104.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "AnastKM102_blocks" VALUES('КШ115им','KSH115 (control mechanism)','КШ115 (вик. механізм)','КШ115 (исп. механизм)','Control mechanism of ball crane КSH115.','Виконавчий механізм кульового крану КШ105.','Исполнительный механизм шарового крана КШ105.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "AnastKM102_blocks" VALUES('КШ116им','KSH116 (control mechanism)','КШ116 (вик. механізм)','КШ116 (исп. механизм)','Control mechanism of ball crane КSH116.','Виконавчий механізм кульового крану КШ106.','Исполнительный механизм шарового крана КШ106.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
CREATE TABLE 'AnastKM102cntr_blocks' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"FUNC" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '0' ,"PROC" INTEGER DEFAULT '0' ,"PRIOR" TEXT DEFAULT '' ,"LNK_OUT_WR_CH" INTEGER DEFAULT '0' , PRIMARY KEY ("ID"));
INSERT INTO "AnastKM102cntr_blocks" VALUES('КШ111','KSH111','КШ111','КШ111','','','','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "AnastKM102cntr_blocks" VALUES('КШ112','KSH112','КШ112','KSH112','','','','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "AnastKM102cntr_blocks" VALUES('КШ113','KSH113','КШ113','КШ113','','','','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "AnastKM102cntr_blocks" VALUES('КШ114','KSH114','КШ114','КШ114','','','','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "AnastKM102cntr_blocks" VALUES('КШ116','KSH116','КШ116','КШ116','','','','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
CREATE TABLE 'AnastKM201_blocks' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"FUNC" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '0' ,"PROC" INTEGER DEFAULT '0' ,"PRIOR" TEXT DEFAULT '' ,"LNK_OUT_WR_CH" INTEGER DEFAULT '0' , PRIMARY KEY ("ID"));
INSERT INTO "AnastKM201_blocks" VALUES('КМ101_1','Compressor КМ101 stage 1','Ступ. 1 компр. КМ101','Ступ. 1 компр. КМ101','Stage one of compressor КМ101.','Перша ступінь компресора КМ101.','Первая ступень компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.compressor',1,1,'',0);
INSERT INTO "AnastKM201_blocks" VALUES('КМ101_2','Compressor КМ101 stage 2','Ступ. 2 компр. КМ101','Ступ. 2 компр. КМ101','Stage two of compressor КМ101.','Друга ступінь компресора КМ101.','Вторая ступень компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.compressor',1,1,'',0);
INSERT INTO "AnastKM201_blocks" VALUES('KPP101','','','','Surge protect control crane of compressor КМ101.','Протипомпажний регулюючий клапан компресору КМ101.','Антипомпажный регулирующий клапан компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.fnc_klap',1,1,'',0);
INSERT INTO "AnastKM201_blocks" VALUES('DR100','Baffle 100','Дросель 100','Дросель 100','','','','DAQ.JavaLikeCalc.lib_techApp.klap',1,1,'',0);
INSERT INTO "AnastKM201_blocks" VALUES('AT101_1','Air cooler AT101_1','Пов. охолод. AT101_1','Возд. охлад. AT101_1','The air cooler for gas after compressor КМ101 stage one.','Охолоджувач газу після першої ступені компресору КМ101.','Охладитель газа после первой ступени компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.cooler',1,1,'',0);
INSERT INTO "AnastKM201_blocks" VALUES('AT101_2','Air cooler AT101_2','Пов. охолод. AT101_2','Возд. охлад. AT101_2','The air cooler for gas after compressor КМ101 stage two.','Охолоджувач газу після другої ступені компресора КМ101.','Охладитель газа после второй ступени компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.cooler',1,1,'',0);
INSERT INTO "AnastKM201_blocks" VALUES('node3_1','Node 3.1','Вузол 3.1','Узел 3.1','','','','DAQ.JavaLikeCalc.lib_techApp.pipe3_1',1,1,'',0);
INSERT INTO "AnastKM201_blocks" VALUES('КШ101','KSH101','КШ101','КШ101','Ball crane for compressor КМ101 enable.','Кульовий кран підключення компресора КМ101.','Шаровый кран подключения компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.klap',1,1,'',0);
INSERT INTO "AnastKM201_blocks" VALUES('node4_1','Node 4.1','Вузол 4.1','Узел 4.1','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_pipe1_3',1,1,'',0);
INSERT INTO "AnastKM201_blocks" VALUES('КШ104','KSH104','КШ104','КШ104','Ball crane for compressor КМ101 enable to net.','Кульовий кран підключення компресора КМ101 у мережу.','Шаровый кран подключения компрессора КМ101 в сеть.','DAQ.JavaLikeCalc.lib_techApp.klap',1,1,'',0);
INSERT INTO "AnastKM201_blocks" VALUES('КШ106','KSH106','КШ106','КШ106','Surge protect ball crane of compressor КМ101.','Протипомпажний шаровий кран компресора КМ101.','Антипомпажный шаровый кран компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.klap',1,1,'',0);
INSERT INTO "AnastKM201_blocks" VALUES('C101_1','Separator C101/1','Сепаратор C101/1','Сепаратор C101/1','Liquid fraction droping before compressor КМ101 stage one.','Сепаратор відгону рідини перед першим ступенем компресора КМ101.','Сепаратор отгона жидкости перед первой ступенью компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.separator',1,1,'',0);
INSERT INTO "AnastKM201_blocks" VALUES('C101_2','Separator C101/2','Сепаратор C101/2','Сепаратор C101/2','Liquid fraction droping after compressor КМ101 stage one.','Сепаратор відгону рідини після першого ступеня компресора КМ101.','Сепаратор отгона жидкости после первой ступени компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.separator',1,1,'',0);
INSERT INTO "AnastKM201_blocks" VALUES('C101_3','Separator C101/3','Сепаратор C101/3','Сепаратор C101/3','Liquid fraction droping after compressor КМ101 stage two.','Сепаратор відгону рідини після другого ступеня компресора КМ101.','Сепаратор отгона жидкости после второй ступени компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.separator',1,1,'',0);
INSERT INTO "AnastKM201_blocks" VALUES('PP101','','','','Flow control point, diaphragm type, for first stage of compressor КМ101.','Вузол виміру витрат на першому ступені компресора КМ101 діафрагменого типу.','Узел замера расхода на первую ступень компрессора КМ101 диафрагменного типа.','DAQ.JavaLikeCalc.lib_techApp.diafragma',1,1,'',0);
INSERT INTO "AnastKM201_blocks" VALUES('PP102','','','','Flow control point, diaphragm type, for second stage of compressor КМ101.','Вузол виміру витрат на другому ступені компресора КМ101 діафрагменого типу.','Узел замера расхода на вторую ступень компрессора КМ101 диафрагменного типа.','DAQ.JavaLikeCalc.lib_techApp.diafragma',1,1,'',0);
INSERT INTO "AnastKM201_blocks" VALUES('КШ104им','KSH104 (control mechanism)','КШ104 (вик. механізм)','КШ104 (исп. механизм)','Control mechanism of ball crane КSH104.','Виконавчий механізм кульового крану КШ104.','Исполнительный механизм шарового крана КШ104.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "AnastKM201_blocks" VALUES('КШ105им','KSH105 (control mechanism)','КШ105 (вик. механізм)','КШ105 (исп. механизм)','Control mechanism of ball crane КSH105.','Виконавчий механізм кульового крану КШ105.','Исполнительный механизм шарового крана КШ105.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "AnastKM201_blocks" VALUES('КШ101им','KSH101 (control mechanism)','КШ101 (вик. механізм)','КШ101 (исп. механизм)','Control mechanism of ball crane КSH101.','Виконавчий механізм кульового крану КШ101.','Исполнительный механизм шарового крана КШ101.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "AnastKM201_blocks" VALUES('КШ106им','KSH106 (control mechanism)','КШ106 (вик. механізм)','КШ106 (исп. механизм)','Control mechanism of ball crane КSH106.','Виконавчий механізм кульового крану КШ106.','Исполнительный механизм шарового крана КШ106.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "AnastKM201_blocks" VALUES('КШ102им','KSH102 (control mechanism)','КШ102 (вик. механізм)','КШ102 (исп. механизм)','Control mechanism of ball crane КSH102.','Виконавчий механізм кульового крану КШ102.','Исполнительный механизм шарового крана КШ102.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
CREATE TABLE 'AnastKM301_blocks' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"FUNC" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '0' ,"PROC" INTEGER DEFAULT '0' ,"PRIOR" TEXT DEFAULT '' ,"LNK_OUT_WR_CH" INTEGER DEFAULT '0' , PRIMARY KEY ("ID"));
INSERT INTO "AnastKM301_blocks" VALUES('КМ101_1','Compressor КМ101 stage 1','Ступ. 1 компр. КМ101','Ступ. 1 компр. КМ101','Stage one of compressor КМ101.','Перша ступінь компресора КМ101.','Первая ступень компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.compressor',1,1,'',0);
INSERT INTO "AnastKM301_blocks" VALUES('КМ101_2','Compressor КМ101 stage 2','Ступ. 2 компр. КМ101','Ступ. 2 компр. КМ101','Stage two of compressor КМ101.','Друга ступінь компресора КМ101.','Вторая ступень компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.compressor',1,1,'',0);
INSERT INTO "AnastKM301_blocks" VALUES('KPP101','','','','Surge protect control crane of compressor КМ101.','Протипомпажний регулюючий клапан компресору КМ101.','Антипомпажный регулирующий клапан компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.fnc_klap',1,1,'',0);
INSERT INTO "AnastKM301_blocks" VALUES('DR100','Baffle 100','Дросель 100','Дросель 100','','','','DAQ.JavaLikeCalc.lib_techApp.klap',1,1,'',0);
INSERT INTO "AnastKM301_blocks" VALUES('AT101_1','Air cooler AT101_1','Пов. охолод. AT101_1','Возд. охлад. AT101_1','The air cooler for gas after compressor КМ101 stage one.','Охолоджувач газу після першої ступені компресору КМ101.','Охладитель газа после первой ступени компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.cooler',1,1,'',0);
INSERT INTO "AnastKM301_blocks" VALUES('AT101_2','Air cooler AT101_2','Пов. охолод. AT101_2','Возд. охлад. AT101_2','The air cooler for gas after compressor КМ101 stage two.','Охолоджувач газу після другої ступені компресора КМ101.','Охладитель газа после второй ступени компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.cooler',1,1,'',0);
INSERT INTO "AnastKM301_blocks" VALUES('node3_1','Node 3.1','Вузол 3.1','Узел 3.1','','','','DAQ.JavaLikeCalc.lib_techApp.pipe3_1',1,1,'',0);
INSERT INTO "AnastKM301_blocks" VALUES('КШ101','KSH101','КШ101','КШ101','Ball crane for compressor КМ101 enable.','Кульовий кран підключення компресора КМ101.','Шаровый кран подключения компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.klap',1,1,'',0);
INSERT INTO "AnastKM301_blocks" VALUES('КШ104','KSH104','КШ104','КШ104','Ball crane for compressor КМ101 enable to net.','Кульовий кран підключення компресора КМ101 у мережу.','Шаровый кран подключения компрессора КМ101 в сеть.','DAQ.JavaLikeCalc.lib_techApp.klap',1,1,'',0);
INSERT INTO "AnastKM301_blocks" VALUES('node4_1','Node 4.1','Вузол 4.1','Узел 4.1','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_pipe1_3',1,1,'',0);
INSERT INTO "AnastKM301_blocks" VALUES('КШ106','KSH106','КШ106','КШ106','Surge protect ball crane of compressor КМ101.','Протипомпажний шаровий кран компресора КМ101.','Антипомпажный шаровый кран компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.klap',1,1,'',0);
INSERT INTO "AnastKM301_blocks" VALUES('C101_1','Separator C101/1','Сепаратор C101/1','Сепаратор C101/1','Liquid fraction droping before compressor КМ101 stage one.','Сепаратор відгону рідини перед першим ступенем компресора КМ101.','Сепаратор отгона жидкости перед первой ступенью компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.separator',1,1,'',0);
INSERT INTO "AnastKM301_blocks" VALUES('C101_2','Separator C101/2','Сепаратор C101/2','Сепаратор C101/2','Liquid fraction droping after compressor КМ101 stage one.','Сепаратор відгону рідини після першого ступеня компресора КМ101.','Сепаратор отгона жидкости после первой ступени компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.separator',1,1,'',0);
INSERT INTO "AnastKM301_blocks" VALUES('C101_3','Separator C101/3','Сепаратор C101/3','Сепаратор C101/3','Liquid fraction droping after compressor КМ101 stage two.','Сепаратор відгону рідини після другого ступеня компресора КМ101.','Сепаратор отгона жидкости после второй ступени компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.separator',1,1,'',0);
INSERT INTO "AnastKM301_blocks" VALUES('PP101','','','','Flow control point, diaphragm type, for first stage of compressor КМ101.','Вузол виміру витрат на першому ступені компресора КМ101 діафрагменого типу.','Узел замера расхода на первую ступень компрессора КМ101 диафрагменного типа.','DAQ.JavaLikeCalc.lib_techApp.diafragma',1,1,'',0);
INSERT INTO "AnastKM301_blocks" VALUES('PP102','','','','Flow control point, diaphragm type, for second stage of compressor КМ101.','Вузол виміру витрат на другому ступені компресора КМ101 діафрагменого типу.','Узел замера расхода на вторую ступень компрессора КМ101 диафрагменного типа.','DAQ.JavaLikeCalc.lib_techApp.diafragma',1,1,'',0);
INSERT INTO "AnastKM301_blocks" VALUES('КШ104им','KSH104 (control mechanism)','КШ104 (вик. механізм)','КШ104 (исп. механизм)','Control mechanism of ball crane КSH104.','Виконавчий механізм кульового крану КШ104.','Исполнительный механизм шарового крана КШ104.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "AnastKM301_blocks" VALUES('КШ105им','KSH105 (control mechanism)','КШ105 (вик. механізм)','КШ105 (исп. механизм)','Control mechanism of ball crane КSH105.','Виконавчий механізм кульового крану КШ105.','Исполнительный механизм шарового крана КШ105.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "AnastKM301_blocks" VALUES('КШ101им','KSH101 (control mechanism)','КШ101 (вик. механізм)','КШ101 (исп. механизм)','Control mechanism of ball crane КSH101.','Виконавчий механізм кульового крану КШ101.','Исполнительный механизм шарового крана КШ101.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "AnastKM301_blocks" VALUES('КШ106им','KSH106 (control mechanism)','КШ106 (вик. механізм)','КШ106 (исп. механизм)','Control mechanism of ball crane КSH106.','Виконавчий механізм кульового крану КШ106.','Исполнительный механизм шарового крана КШ106.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "AnastKM301_blocks" VALUES('КШ102им','KSH102 (control mechanism)','КШ102 (вик. механізм)','КШ102 (исп. механизм)','Control mechanism of ball crane КSH102.','Виконавчий механізм кульового крану КШ102.','Исполнительный механизм шарового крана КШ102.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
CREATE TABLE 'AnastKM302_blocks' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"FUNC" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '0' ,"PROC" INTEGER DEFAULT '0' ,"PRIOR" TEXT DEFAULT '' ,"LNK_OUT_WR_CH" INTEGER DEFAULT '0' , PRIMARY KEY ("ID"));
INSERT INTO "AnastKM302_blocks" VALUES('KPP102','','','','Surge protect control crane of compressor КМ102.','Протипомпажний регулюючий клапан компресору КМ102.','Антипомпажный регулирующий клапан компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.fnc_klap',1,1,'',0);
INSERT INTO "AnastKM302_blocks" VALUES('net102','','','','','','','DAQ.JavaLikeCalc.lib_techApp.fnc_net',1,1,'',0);
INSERT INTO "AnastKM302_blocks" VALUES('DR101','Baffle 101','Дросель 101','Дросель 101','','','','DAQ.JavaLikeCalc.lib_techApp.klap',1,1,'',0);
INSERT INTO "AnastKM302_blocks" VALUES('С102_1','Separator C102/1','Сепаратор С102/1','Сепаратор С102/1','Liquid fraction droping before compressor КМ102 stage one.','Сепаратор відгону рідини перед першим ступенем компресора КМ102.','Сепаратор отгона жидкости перед первой ступенью компрессора КМ102.','DAQ.JavaLikeCalc.lib_techApp.separator',1,1,'',0);
INSERT INTO "AnastKM302_blocks" VALUES('AT102_1','Air cooler AT102_1','Пов. охолод. AT102_1','Возд. охлад. AT102_1','The air cooler for gas after compressor КМ102 stage one.','Охолоджувач газу після першої ступені компресору КМ102.','Охладитель газа после первой ступени компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.cooler',1,1,'',0);
INSERT INTO "AnastKM302_blocks" VALUES('AT102_2','Air cooler AT102_2','Пов. охолод. AT102_2','Возд. охлад. AT102_2','The air cooler for gas after compressor КМ102 stage two.','Охолоджувач газу після другої ступені компресора КМ102.','Охладитель газа после второй ступени компрессора КМ102.','DAQ.JavaLikeCalc.lib_techApp.cooler',1,1,'',0);
INSERT INTO "AnastKM302_blocks" VALUES('КШ111','KSH111','КШ111','КШ111','Ball crane for compressor КМ102 enable.','Кульовий кран підключення компресору КМ102.','Шаровый кран подключения компрессора КМ102.','DAQ.JavaLikeCalc.lib_techApp.klap',1,1,'',0);
INSERT INTO "AnastKM302_blocks" VALUES('КШ113','KSH113','КШ113','КШ113','','','','DAQ.JavaLikeCalc.lib_techApp.fnc_klap',1,1,'',0);
INSERT INTO "AnastKM302_blocks" VALUES('КШ114','KSH114','КШ114','КШ114','Ball crane for compressor КМ102 enable to net.','Кульовий кран підключення компресору КМ102 у мережу.','Шаровый кран подключения компрессора КМ101 в сеть.','DAQ.JavaLikeCalc.lib_techApp.klap',1,1,'',0);
INSERT INTO "AnastKM302_blocks" VALUES('КШ116','KSH116','КШ116','КШ116','Surge protect ball crane of compressor КМ102.','Протипомпажний шаровий кран компресора КМ102.','Антипомпажный шаровый кран компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.klap',1,1,'',0);
INSERT INTO "AnastKM302_blocks" VALUES('node5_1','Node 5.1','Вузол 5.1','Узел 5.1','','','','DAQ.JavaLikeCalc.lib_techApp.pipe3_1',1,1,'',0);
INSERT INTO "AnastKM302_blocks" VALUES('KM102_1','Compressor КМ102 stage 1','Ступ. 1 компр. КМ102','Ступ. 1 компр. КМ102','Stage one of compressor КМ102.','Перша ступінь компресора КМ102.','Первая ступень компрессора КМ102.','DAQ.JavaLikeCalc.lib_techApp.compressor',1,1,'',0);
INSERT INTO "AnastKM302_blocks" VALUES('KM102_2','Compressor КМ102 stage 2','Ступ. 2 компр. КМ102','Ступ. 2 компр. КМ102','Stage two of compressor КМ102.','Друга ступінь компресора КМ102.','Вторая ступень компрессора КМ102.','DAQ.JavaLikeCalc.lib_techApp.compressor',1,1,'',0);
INSERT INTO "AnastKM302_blocks" VALUES('node6_1','Node 6.1','Вузол 6.1','Узел 6.1','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_pipe1_4',1,1,'',0);
INSERT INTO "AnastKM302_blocks" VALUES('PP103','','','','Flow control point, diaphragm type, for first stage of compressor КМ102.','Вузол виміру витрат на першому ступені компресора КМ102 діафрагменого типу.','Узел замера расхода на первую ступень компрессора КМ102 диафрагменного типа.','DAQ.JavaLikeCalc.lib_techApp.diafragma',1,1,'',0);
INSERT INTO "AnastKM302_blocks" VALUES('PP104','','','','Flow control point, diaphragm type, for second stage of compressor КМ102.','Вузол виміру витрат на другому ступені компресора КМ102 діафрагменого типу.','Узел замера расхода на вторую ступень компрессора КМ102 диафрагменного типа.','DAQ.JavaLikeCalc.lib_techApp.diafragma',1,1,'',0);
INSERT INTO "AnastKM302_blocks" VALUES('C102_2','Separator C102/2','Сепаратор C102/2','Сепаратор C102/2','Liquid fraction droping after compressor КМ102 stage one.','Сепаратор відгону рідини після першого ступеня компресора КМ102.','Сепаратор отгона жидкости после первой ступени компрессора КМ102.','DAQ.JavaLikeCalc.lib_techApp.separator',1,1,'',0);
INSERT INTO "AnastKM302_blocks" VALUES('C102_3','Separator C102/3','Сепаратор C102/3','Сепаратор C102/3','Liquid fraction droping after compressor КМ102 stage two.','Сепаратор відгону рідини після другого ступеня компресора КМ102.','Сепаратор отгона жидкости после второй ступени компрессора КМ102.','DAQ.JavaLikeCalc.lib_techApp.separator',1,1,'',0);
INSERT INTO "AnastKM302_blocks" VALUES('КШ114им','KSH114 (control mechanism)','КШ114 (вик. механізм)','КШ114 (исп. механизм)','Control mechanism of ball crane КSH114.','Виконавчий механізм кульового крану КШ104.','Исполнительный механизм шарового крана КШ104.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "AnastKM302_blocks" VALUES('КШ115им','KSH115 (control mechanism)','КШ115 (вик. механізм)','КШ115 (исп. механизм)','Control mechanism of ball crane КSH115.','Виконавчий механізм кульового крану КШ105.','Исполнительный механизм шарового крана КШ105.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "AnastKM302_blocks" VALUES('КШ111им','KSH111 (control mechanism)','КШ111 (вик. механізм)','КШ111 (исп. механизм)','Control mechanism of ball crane КSH111.','Виконавчий механізм кульового крану КШ101.','Исполнительный механизм шарового крана КШ101.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "AnastKM302_blocks" VALUES('КШ116им','KSH116 (control mechanism)','КШ116 (вик. механізм)','КШ116 (исп. механизм)','Control mechanism of ball crane КSH116.','Виконавчий механізм кульового крану КШ106.','Исполнительный механизм шарового крана КШ106.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "AnastKM302_blocks" VALUES('КШ112им','KSH112 (control mechanism)','КШ112 (вик. механізм)','КШ112 (исп. механизм)','Control mechanism of ball crane КSH112.','Виконавчий механізм кульового крану КШ102.','Исполнительный механизм шарового крана КШ102.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
CREATE TABLE 'AnastKM202_blocks' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"FUNC" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '0' ,"PROC" INTEGER DEFAULT '0' ,"PRIOR" TEXT DEFAULT '' ,"LNK_OUT_WR_CH" INTEGER DEFAULT '0' , PRIMARY KEY ("ID"));
INSERT INTO "AnastKM202_blocks" VALUES('KPP102','','','KPP102','Surge protect control crane of compressor КМ102.','Протипомпажний регулюючий клапан компресору КМ102.','Антипомпажный регулирующий клапан компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.fnc_klap',1,1,'',0);
INSERT INTO "AnastKM202_blocks" VALUES('net102','','','','','','','DAQ.JavaLikeCalc.lib_techApp.fnc_net',1,1,'',0);
INSERT INTO "AnastKM202_blocks" VALUES('DR101','Baffle 101','Дросель 101','Дросель 101','','','','DAQ.JavaLikeCalc.lib_techApp.klap',1,1,'',0);
INSERT INTO "AnastKM202_blocks" VALUES('С102_1','Separator C102/1','Сепаратор С102/1','Сепаратор С102/1','Liquid fraction droping before compressor КМ102 stage one.','Сепаратор відгону рідини перед першим ступенем компресора КМ102.','Сепаратор отгона жидкости перед первой ступенью компрессора КМ102.','DAQ.JavaLikeCalc.lib_techApp.separator',1,1,'',0);
INSERT INTO "AnastKM202_blocks" VALUES('AT102_1','Air cooler AT102_1','Пов. охолод. AT102_1','Возд. охлад. AT102_1','The air cooler for gas after compressor КМ102 stage one.','Охолоджувач газу після першої ступені компресору КМ102.','Охладитель газа после первой ступени компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.cooler',1,1,'',0);
INSERT INTO "AnastKM202_blocks" VALUES('AT102_2','Air cooler AT102_2','Пов. охолод. AT102_2','Возд. охлад. AT102_2','The air cooler for gas after compressor КМ102 stage two.','Охолоджувач газу після другої ступені компресора КМ102.','Охладитель газа после второй ступени компрессора КМ102.','DAQ.JavaLikeCalc.lib_techApp.cooler',1,1,'',0);
INSERT INTO "AnastKM202_blocks" VALUES('КШ111','KSH111','КШ111','КШ111','Ball crane for compressor КМ102 enable.','Кульовий кран підключення компресору КМ102.','Шаровый кран подключения компрессора КМ102.','DAQ.JavaLikeCalc.lib_techApp.klap',1,1,'',0);
INSERT INTO "AnastKM202_blocks" VALUES('КШ113','KSH113','КШ113','КШ113','','','','DAQ.JavaLikeCalc.lib_techApp.fnc_klap',1,1,'',0);
INSERT INTO "AnastKM202_blocks" VALUES('КШ114','KSH114','КШ114','КШ114','Ball crane for compressor КМ102 enable to net.','Кульовий кран підключення компресору КМ102 у мережу.','Шаровый кран подключения компрессора КМ101 в сеть.','DAQ.JavaLikeCalc.lib_techApp.klap',1,1,'',0);
INSERT INTO "AnastKM202_blocks" VALUES('КШ116','KSH116','КШ116','КШ116','Surge protect ball crane of compressor КМ102.','Протипомпажний шаровий кран компресора КМ102.','Антипомпажный шаровый кран компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.klap',1,1,'',0);
INSERT INTO "AnastKM202_blocks" VALUES('node5_1','Node 5.1','Вузол 5.1','Узел 5.1','','','','DAQ.JavaLikeCalc.lib_techApp.pipe3_1',1,1,'',0);
INSERT INTO "AnastKM202_blocks" VALUES('KM102_1','Compressor КМ102 stage 1','Ступ. 1 компр. КМ102','Ступ. 1 компр. КМ102','Stage one of compressor КМ102.','Перша ступінь компресора КМ102.','Первая ступень компрессора КМ102.','DAQ.JavaLikeCalc.lib_techApp.compressor',1,1,'',0);
INSERT INTO "AnastKM202_blocks" VALUES('KM102_2','Compressor КМ102 stage 2','Ступ. 2 компр. КМ102','Ступ. 2 компр. КМ102','Stage two of compressor КМ102.','Друга ступінь компресора КМ102.','Вторая ступень компрессора КМ102.','DAQ.JavaLikeCalc.lib_techApp.compressor',1,1,'',0);
INSERT INTO "AnastKM202_blocks" VALUES('node6_1','Node 6.1','Вузол 6.1','Узел 6.1','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_pipe1_4',1,1,'',0);
INSERT INTO "AnastKM202_blocks" VALUES('PP103','','','','Flow control point, diaphragm type, for first stage of compressor КМ102.','Вузол виміру витрат на першому ступені компресора КМ102 діафрагменого типу.','Узел замера расхода на первую ступень компрессора КМ102 диафрагменного типа.','DAQ.JavaLikeCalc.lib_techApp.diafragma',1,1,'',0);
INSERT INTO "AnastKM202_blocks" VALUES('PP104','','','','Flow control point, diaphragm type, for second stage of compressor КМ102.','Вузол виміру витрат на другому ступені компресора КМ102 діафрагменого типу.','Узел замера расхода на вторую ступень компрессора КМ102 диафрагменного типа.','DAQ.JavaLikeCalc.lib_techApp.diafragma',1,1,'',0);
INSERT INTO "AnastKM202_blocks" VALUES('C102_2','Separator C102/2','Сепаратор C102/2','Сепаратор C102/2','Liquid fraction droping after compressor КМ102 stage one.','Сепаратор відгону рідини після першого ступеня компресора КМ102.','Сепаратор отгона жидкости после первой ступени компрессора КМ102.','DAQ.JavaLikeCalc.lib_techApp.separator',1,1,'',0);
INSERT INTO "AnastKM202_blocks" VALUES('C102_3','Separator C102/3','Сепаратор C102/3','Сепаратор C102/3','Liquid fraction droping after compressor КМ102 stage two.','Сепаратор відгону рідини після другого ступеня компресора КМ102.','Сепаратор отгона жидкости после второй ступени компрессора КМ102.','DAQ.JavaLikeCalc.lib_techApp.separator',1,1,'',0);
INSERT INTO "AnastKM202_blocks" VALUES('КШ114им','KSH114 (control mechanism)','КШ114 (вик. механізм)','КШ114 (исп. механизм)','Control mechanism of ball crane КSH114.','Виконавчий механізм кульового крану КШ104.','Исполнительный механизм шарового крана КШ104.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "AnastKM202_blocks" VALUES('КШ115им','KSH115 (control mechanism)','КШ115 (вик. механізм)','КШ115 (исп. механизм)','Control mechanism of ball crane КSH115.','Виконавчий механізм кульового крану КШ105.','Исполнительный механизм шарового крана КШ105.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "AnastKM202_blocks" VALUES('КШ111им','KSH111 (control mechanism)','КШ111 (вик. механізм)','КШ111 (исп. механизм)','Control mechanism of ball crane КSH111.','Виконавчий механізм кульового крану КШ101.','Исполнительный механизм шарового крана КШ101.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "AnastKM202_blocks" VALUES('КШ116им','KSH116 (control mechanism)','КШ116 (вик. механізм)','КШ116 (исп. механизм)','Control mechanism of ball crane КSH116.','Виконавчий механізм кульового крану КШ106.','Исполнительный механизм шарового крана КШ106.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "AnastKM202_blocks" VALUES('КШ112им','KSH112 (control mechanism)','КШ112 (вик. механізм)','КШ112 (исп. механизм)','Control mechanism of ball crane КSH112.','Виконавчий механізм кульового крану КШ102.','Исполнительный механизм шарового крана КШ102.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
COMMIT;
