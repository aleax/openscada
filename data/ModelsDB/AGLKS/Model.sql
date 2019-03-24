PRAGMA foreign_keys=OFF;
BEGIN TRANSACTION;
CREATE TABLE 'gen_blcks_io' ("BLK_ID" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"TLNK" INTEGER DEFAULT '' ,"LNK" TEXT DEFAULT '' ,"VAL" TEXT DEFAULT '' , PRIMARY KEY ("BLK_ID","ID"));
INSERT INTO "gen_blcks_io" VALUES('node1_1','Fi',0,'','101.656421139471');
INSERT INTO "gen_blcks_io" VALUES('node1_1','Pi',1,'node1.Po2','5.58167742144463');
INSERT INTO "gen_blcks_io" VALUES('node1_1','Fo1',2,'CM101.BC101.Fi','50.9296798223303');
INSERT INTO "gen_blcks_io" VALUES('node1_1','Po1',6,'CM101.BC101.Pi','5.54910674293414');
INSERT INTO "gen_blcks_io" VALUES('node1_1','So1',0,'','0.2');
INSERT INTO "gen_blcks_io" VALUES('node1_1','lo1',0,'','40');
INSERT INTO "gen_blcks_io" VALUES('node1_1','Fo2',2,'CM201.BC101.Fi','50.7444453360053');
INSERT INTO "gen_blcks_io" VALUES('node1_1','Po2',6,'CM201.BC101.Pi','5.5493436007765');
INSERT INTO "gen_blcks_io" VALUES('node1_1','So2',0,'','0.2');
INSERT INTO "gen_blcks_io" VALUES('node1_1','lo2',0,'','40');
INSERT INTO "gen_blcks_io" VALUES('node1_1','Fo3',2,'CM301.BC101.Fi','-4.84184332924422e-322');
INSERT INTO "gen_blcks_io" VALUES('node1_1','Po3',6,'CM301.BC101.Pi','5.58167523818365');
INSERT INTO "gen_blcks_io" VALUES('node1_1','So3',0,'','0.2');
INSERT INTO "gen_blcks_io" VALUES('node1_1','lo3',0,'','40');
INSERT INTO "gen_blcks_io" VALUES('node1_1','Q0',0,'','1');
INSERT INTO "gen_blcks_io" VALUES('node1_1','f_frq',0,'','100');
INSERT INTO "gen_blcks_io" VALUES('node1_1','F1tmp',0,'','50.9238779804676');
INSERT INTO "gen_blcks_io" VALUES('node1_1','F2tmp',0,'','50.7384289172927');
INSERT INTO "gen_blcks_io" VALUES('node1_1','F3tmp',0,'','-0.00588575828964486');
INSERT INTO "gen_blcks_io" VALUES('node1_1','Pot1',0,'','5.33928112230883');
INSERT INTO "gen_blcks_io" VALUES('node1_1','Pot2',0,'','5.34618472503855');
INSERT INTO "gen_blcks_io" VALUES('node1_1','Pot3',0,'','5.40197994347128');
INSERT INTO "gen_blcks_io" VALUES('G_BC22','pos',0,'','0');
INSERT INTO "gen_blcks_io" VALUES('G_BC22','com',0,'','0');
INSERT INTO "gen_blcks_io" VALUES('G_BC22','st_open',0,'','0');
INSERT INTO "gen_blcks_io" VALUES('G_BC22','st_close',0,'','1');
INSERT INTO "gen_blcks_io" VALUES('G_BC22','t_full',0,'','3');
INSERT INTO "gen_blcks_io" VALUES('G_BC22','t_up',0,'','0.3');
INSERT INTO "gen_blcks_io" VALUES('G_BC22','f_frq',0,'','100');
INSERT INTO "gen_blcks_io" VALUES('G_BC22','tmp_up',0,'','0');
INSERT INTO "gen_blcks_io" VALUES('G_BC22','lst_com',0,'','0');
INSERT INTO "gen_blcks_io" VALUES('netHPG','Fi',0,'','51.4857695227236');
INSERT INTO "gen_blcks_io" VALUES('netHPG','Pi',1,'PCV4.Po','90.2695970422966');
INSERT INTO "gen_blcks_io" VALUES('netHPG','Po',0,'','90');
INSERT INTO "gen_blcks_io" VALUES('netHPG','So',0,'','0.017');
INSERT INTO "gen_blcks_io" VALUES('netHPG','Kpr',0,'','0.95');
INSERT INTO "gen_blcks_io" VALUES('netHPG','Noise',0,'','1');
INSERT INTO "gen_blcks_io" VALUES('netHPG','Q0',0,'','1');
INSERT INTO "gen_blcks_io" VALUES('netHPG','f_frq',0,'','100');
INSERT INTO "gen_blcks_io" VALUES('BC21','Fi',0,'','53.5415791885251');
INSERT INTO "gen_blcks_io" VALUES('BC21','Pi',1,'node20.Po2','23.9989239641711');
INSERT INTO "gen_blcks_io" VALUES('BC21','Ti',1,'BC6.To','305.125415707595');
INSERT INTO "gen_blcks_io" VALUES('BC21','Fo',1,'C2.Fi','53.5431913848687');
INSERT INTO "gen_blcks_io" VALUES('BC21','Po',0,'','23.9360385267862');
INSERT INTO "gen_blcks_io" VALUES('BC21','To',0,'','305.049382589841');
INSERT INTO "gen_blcks_io" VALUES('BC21','So',0,'','0.071');
INSERT INTO "gen_blcks_io" VALUES('BC21','lo',0,'','7');
INSERT INTO "gen_blcks_io" VALUES('BC21','S_kl1',0,'','0.071');
INSERT INTO "gen_blcks_io" VALUES('BC21','S_kl2',0,'','0.002');
INSERT INTO "gen_blcks_io" VALUES('BC21','l_kl1',1,'G_BC21.pos','100');
INSERT INTO "gen_blcks_io" VALUES('BC21','l_kl2',1,'G_BC22.pos','0');
INSERT INTO "gen_blcks_io" VALUES('BC21','Q0',0,'','1');
INSERT INTO "gen_blcks_io" VALUES('BC21','Kln',0,'','1');
INSERT INTO "gen_blcks_io" VALUES('BC21','Kpr',0,'','0.95');
INSERT INTO "gen_blcks_io" VALUES('BC21','Ct',0,'','20');
INSERT INTO "gen_blcks_io" VALUES('BC21','Riz',0,'','20');
INSERT INTO "gen_blcks_io" VALUES('BC21','noBack',0,'','0');
INSERT INTO "gen_blcks_io" VALUES('BC21','Fwind',0,'','1');
INSERT INTO "gen_blcks_io" VALUES('BC21','Twind',0,'','273');
INSERT INTO "gen_blcks_io" VALUES('BC21','f_frq',0,'','100');
INSERT INTO "gen_blcks_io" VALUES('BC21','Pot',0,'','23.8507492058468');
INSERT INTO "gen_blcks_io" VALUES('BC21','Fit',0,'','38.8382020654556');
INSERT INTO "gen_blcks_io" VALUES('noisePP3','out',0,'','1.01014305597337');
INSERT INTO "gen_blcks_io" VALUES('noisePP3','off',0,'','1');
INSERT INTO "gen_blcks_io" VALUES('noisePP3','a_g1',0,'','0.01');
INSERT INTO "gen_blcks_io" VALUES('noisePP3','per_g1',0,'','200');
INSERT INTO "gen_blcks_io" VALUES('noisePP3','a_g2',0,'','0');
INSERT INTO "gen_blcks_io" VALUES('noisePP3','per_g2',0,'','30');
INSERT INTO "gen_blcks_io" VALUES('noisePP3','a_rnd',0,'','0.001');
INSERT INTO "gen_blcks_io" VALUES('noisePP3','f_frq',0,'','100');
INSERT INTO "gen_blcks_io" VALUES('noisePP3','tmp_g1',0,'','1.63436999999989');
INSERT INTO "gen_blcks_io" VALUES('noisePP3','tmp_g2',0,'','5.3526533333335');
INSERT INTO "gen_blcks_io" VALUES('G_PCV2','out',0,'','53.6012359586875');
INSERT INTO "gen_blcks_io" VALUES('G_PCV2','in',2,'gen_cntr.PC_PCV2.out','53.6014729388859');
INSERT INTO "gen_blcks_io" VALUES('G_PCV2','t_lg',0,'','0.25');
INSERT INTO "gen_blcks_io" VALUES('G_PCV2','f_frq',0,'','100');
INSERT INTO "gen_blcks_io" VALUES('G_PCV2','cl1',0,'','53.6014091164555');
INSERT INTO "gen_blcks_io" VALUES('G_PCV2','cl2',0,'','53.601341301757');
INSERT INTO "gen_blcks_io" VALUES('G_PCV2','cl3',0,'','53.6012890395737');
INSERT INTO "gen_blcks_io" VALUES('netGlinsk','Fi',0,'','45.0665075845315');
INSERT INTO "gen_blcks_io" VALUES('netGlinsk','Pi',1,'pipeGlinsk.Po','20.1073681952472');
INSERT INTO "gen_blcks_io" VALUES('netGlinsk','Po',0,'','20');
INSERT INTO "gen_blcks_io" VALUES('netGlinsk','So',0,'','0.05');
INSERT INTO "gen_blcks_io" VALUES('netGlinsk','Kpr',0,'','0.95');
INSERT INTO "gen_blcks_io" VALUES('netGlinsk','Noise',0,'','1');
INSERT INTO "gen_blcks_io" VALUES('netGlinsk','Q0',0,'','1');
INSERT INTO "gen_blcks_io" VALUES('netGlinsk','f_frq',0,'','100');
INSERT INTO "gen_blcks_io" VALUES('G_BC6','pos',0,'','100');
INSERT INTO "gen_blcks_io" VALUES('G_BC6','com',0,'','1');
INSERT INTO "gen_blcks_io" VALUES('G_BC6','st_open',0,'','1');
INSERT INTO "gen_blcks_io" VALUES('G_BC6','st_close',0,'','0');
INSERT INTO "gen_blcks_io" VALUES('G_BC6','t_full',0,'','5');
INSERT INTO "gen_blcks_io" VALUES('G_BC6','t_up',0,'','0.5');
INSERT INTO "gen_blcks_io" VALUES('G_BC6','f_frq',0,'','100');
INSERT INTO "gen_blcks_io" VALUES('G_BC6','tmp_up',0,'','0');
INSERT INTO "gen_blcks_io" VALUES('G_BC6','lst_com',0,'','1');
INSERT INTO "gen_blcks_io" VALUES('P_PCV2','out',0,'','24.0150347217309');
INSERT INTO "gen_blcks_io" VALUES('P_PCV2','in',1,'node20.Po1','24.0148689194691');
INSERT INTO "gen_blcks_io" VALUES('P_PCV2','t_lg',0,'','0.2');
INSERT INTO "gen_blcks_io" VALUES('P_PCV2','f_frq',0,'','100');
INSERT INTO "gen_blcks_io" VALUES('SrcGas','Pi',0,'','9');
INSERT INTO "gen_blcks_io" VALUES('SrcGas','Fo',1,'BC1.Fi','102.661556748971');
INSERT INTO "gen_blcks_io" VALUES('SrcGas','Po',0,'','9.04763918939329');
INSERT INTO "gen_blcks_io" VALUES('SrcGas','So',0,'','0.385');
INSERT INTO "gen_blcks_io" VALUES('SrcGas','lo',0,'','1000');
INSERT INTO "gen_blcks_io" VALUES('SrcGas','Noise',1,'noisePP3.out','1.00951753817055');
INSERT INTO "gen_blcks_io" VALUES('SrcGas','Q0',0,'','1');
INSERT INTO "gen_blcks_io" VALUES('SrcGas','f_frq',0,'','100');
INSERT INTO "gen_blcks_io" VALUES('SrcGas','Pot',0,'','8.96131699790609');
INSERT INTO "gen_blcks_io" VALUES('SrcGas','Fit',0,'','102.540019442926');
INSERT INTO "gen_blcks_io" VALUES('PCV1','Fi',0,'','102.662090662929');
INSERT INTO "gen_blcks_io" VALUES('PCV1','Pi',1,'BC1.Po','9.02681101841087');
INSERT INTO "gen_blcks_io" VALUES('PCV1','Ti',1,'BC1.To','292.967043165263');
INSERT INTO "gen_blcks_io" VALUES('PCV1','Fo',1,'C1.Fi','102.665115710178');
INSERT INTO "gen_blcks_io" VALUES('PCV1','Po',0,'','5.80065325135806');
INSERT INTO "gen_blcks_io" VALUES('PCV1','To',0,'','290.373859678644');
INSERT INTO "gen_blcks_io" VALUES('PCV1','So',0,'','0.385');
INSERT INTO "gen_blcks_io" VALUES('PCV1','lo',0,'','10');
INSERT INTO "gen_blcks_io" VALUES('PCV1','S_kl1',0,'','0.2');
INSERT INTO "gen_blcks_io" VALUES('PCV1','S_kl2',0,'','0.05');
INSERT INTO "gen_blcks_io" VALUES('PCV1','l_kl1',2,'gen_cntr.PC_PCV1.out','39.302204328147');
INSERT INTO "gen_blcks_io" VALUES('PCV1','l_kl2',0,'','0');
INSERT INTO "gen_blcks_io" VALUES('PCV1','Q0',0,'','1');
INSERT INTO "gen_blcks_io" VALUES('PCV1','Kln',0,'','2');
INSERT INTO "gen_blcks_io" VALUES('PCV1','Kpr',0,'','0.95');
INSERT INTO "gen_blcks_io" VALUES('PCV1','Ct',0,'','20');
INSERT INTO "gen_blcks_io" VALUES('PCV1','Riz',0,'','20');
INSERT INTO "gen_blcks_io" VALUES('PCV1','noBack',0,'','0');
INSERT INTO "gen_blcks_io" VALUES('PCV1','Fwind',0,'','1');
INSERT INTO "gen_blcks_io" VALUES('PCV1','Twind',0,'','273');
INSERT INTO "gen_blcks_io" VALUES('PCV1','f_frq',0,'','100');
INSERT INTO "gen_blcks_io" VALUES('PCV1','Pot',0,'','5.80103546715012');
INSERT INTO "gen_blcks_io" VALUES('PCV1','Fit',0,'','74.3031528715286');
INSERT INTO "gen_blcks_io" VALUES('pipeGlinsk','Fi',0,'','45.2448336814594');
INSERT INTO "gen_blcks_io" VALUES('pipeGlinsk','Pi',1,'PCV2.Po','22.9207125738463');
INSERT INTO "gen_blcks_io" VALUES('pipeGlinsk','Fo',1,'netGlinsk.Fi','45.0664969187913');
INSERT INTO "gen_blcks_io" VALUES('pipeGlinsk','Po',0,'','20.1073680939868');
INSERT INTO "gen_blcks_io" VALUES('pipeGlinsk','So',0,'','0.05');
INSERT INTO "gen_blcks_io" VALUES('pipeGlinsk','lo',0,'','30000');
INSERT INTO "gen_blcks_io" VALUES('pipeGlinsk','Q0',0,'','1');
INSERT INTO "gen_blcks_io" VALUES('pipeGlinsk','f_frq',0,'','100');
INSERT INTO "gen_blcks_io" VALUES('pipeGlinsk','Pti',0,'','22.0598922141446');
INSERT INTO "gen_blcks_io" VALUES('pipeGlinsk','Fto',0,'','45.0576784950202');
INSERT INTO "gen_blcks_io" VALUES('pipeGlinsk','Pt1',0,'','21.1246257567586');
INSERT INTO "gen_blcks_io" VALUES('pipeGlinsk','Ft1',0,'','44.8656574264926');
INSERT INTO "gen_blcks_io" VALUES('PCV2','Fi',0,'','45.2427140495495');
INSERT INTO "gen_blcks_io" VALUES('PCV2','Pi',1,'node20.Po1','24.0148689194691');
INSERT INTO "gen_blcks_io" VALUES('PCV2','Ti',0,'','273');
INSERT INTO "gen_blcks_io" VALUES('PCV2','Fo',1,'pipeGlinsk.Fi','45.2448239411331');
INSERT INTO "gen_blcks_io" VALUES('PCV2','Po',0,'','22.9207210217478');
INSERT INTO "gen_blcks_io" VALUES('PCV2','To',0,'','272.74607447027');
INSERT INTO "gen_blcks_io" VALUES('PCV2','So',0,'','0.071');
INSERT INTO "gen_blcks_io" VALUES('PCV2','lo',0,'','10');
INSERT INTO "gen_blcks_io" VALUES('PCV2','S_kl1',0,'','0.05');
INSERT INTO "gen_blcks_io" VALUES('PCV2','S_kl2',0,'','0');
INSERT INTO "gen_blcks_io" VALUES('PCV2','l_kl1',1,'G_PCV2.out','53.6012444516293');
INSERT INTO "gen_blcks_io" VALUES('PCV2','l_kl2',0,'','0');
INSERT INTO "gen_blcks_io" VALUES('PCV2','Q0',0,'','1');
INSERT INTO "gen_blcks_io" VALUES('PCV2','Kln',0,'','2');
INSERT INTO "gen_blcks_io" VALUES('PCV2','Kpr',0,'','0.95');
INSERT INTO "gen_blcks_io" VALUES('PCV2','Ct',0,'','20');
INSERT INTO "gen_blcks_io" VALUES('PCV2','Riz',0,'','20');
INSERT INTO "gen_blcks_io" VALUES('PCV2','noBack',0,'','0');
INSERT INTO "gen_blcks_io" VALUES('PCV2','Fwind',0,'','1');
INSERT INTO "gen_blcks_io" VALUES('PCV2','Twind',0,'','273');
INSERT INTO "gen_blcks_io" VALUES('PCV2','f_frq',0,'','100');
INSERT INTO "gen_blcks_io" VALUES('PCV2','Pot',0,'','20.2795149522328');
INSERT INTO "gen_blcks_io" VALUES('PCV2','Fit',0,'','33.1004015952682');
INSERT INTO "gen_blcks_io" VALUES('PCV4','Fi',0,'','51.4858196089344');
INSERT INTO "gen_blcks_io" VALUES('PCV4','Pi',1,'PP7.Po','91.9531400240928');
INSERT INTO "gen_blcks_io" VALUES('PCV4','Ti',1,'node22.To','302.429005958539');
INSERT INTO "gen_blcks_io" VALUES('PCV4','Fo',1,'netHPG.Fi','51.4857923414327');
INSERT INTO "gen_blcks_io" VALUES('PCV4','Po',0,'','90.2695970422966');
INSERT INTO "gen_blcks_io" VALUES('PCV4','To',0,'','302.261250315868');
INSERT INTO "gen_blcks_io" VALUES('PCV4','So',0,'','0.017');
INSERT INTO "gen_blcks_io" VALUES('PCV4','lo',0,'','10');
INSERT INTO "gen_blcks_io" VALUES('PCV4','S_kl1',0,'','0.014');
INSERT INTO "gen_blcks_io" VALUES('PCV4','S_kl2',0,'','0.014');
INSERT INTO "gen_blcks_io" VALUES('PCV4','l_kl1',0,'','48.1299650661653');
INSERT INTO "gen_blcks_io" VALUES('PCV4','l_kl2',0,'','0');
INSERT INTO "gen_blcks_io" VALUES('PCV4','Q0',0,'','1');
INSERT INTO "gen_blcks_io" VALUES('PCV4','Kln',0,'','1');
INSERT INTO "gen_blcks_io" VALUES('PCV4','Kpr',0,'','0.95');
INSERT INTO "gen_blcks_io" VALUES('PCV4','Ct',0,'','20');
INSERT INTO "gen_blcks_io" VALUES('PCV4','Riz',0,'','20');
INSERT INTO "gen_blcks_io" VALUES('PCV4','noBack',0,'','0');
INSERT INTO "gen_blcks_io" VALUES('PCV4','Fwind',0,'','1');
INSERT INTO "gen_blcks_io" VALUES('PCV4','Twind',0,'','273');
INSERT INTO "gen_blcks_io" VALUES('PCV4','f_frq',0,'','100');
INSERT INTO "gen_blcks_io" VALUES('PCV4','Pot',0,'','90.4882859030793');
INSERT INTO "gen_blcks_io" VALUES('PCV4','Fit',0,'','37.2954967691498');
INSERT INTO "gen_blcks_io" VALUES('G_BC7','pos',0,'','0');
INSERT INTO "gen_blcks_io" VALUES('G_BC7','com',0,'','0');
INSERT INTO "gen_blcks_io" VALUES('G_BC7','st_open',0,'','0');
INSERT INTO "gen_blcks_io" VALUES('G_BC7','st_close',0,'','1');
INSERT INTO "gen_blcks_io" VALUES('G_BC7','t_full',0,'','3');
INSERT INTO "gen_blcks_io" VALUES('G_BC7','t_up',0,'','0.3');
INSERT INTO "gen_blcks_io" VALUES('G_BC7','f_frq',0,'','100');
INSERT INTO "gen_blcks_io" VALUES('G_BC7','tmp_up',0,'','0');
INSERT INTO "gen_blcks_io" VALUES('G_BC7','lst_com',0,'','0');
INSERT INTO "gen_blcks_io" VALUES('PP1','Fi',0,'','101.639515152932');
INSERT INTO "gen_blcks_io" VALUES('PP1','Pi',1,'C1.Po','5.76808368940158');
INSERT INTO "gen_blcks_io" VALUES('PP1','Fo',1,'node1.Fi','101.643789385011');
INSERT INTO "gen_blcks_io" VALUES('PP1','Po',0,'','5.6477295043406');
INSERT INTO "gen_blcks_io" VALUES('PP1','dP',0,'','12.0361705356914');
INSERT INTO "gen_blcks_io" VALUES('PP1','Sdf',0,'','0.2');
INSERT INTO "gen_blcks_io" VALUES('PP1','So',0,'','0.283');
INSERT INTO "gen_blcks_io" VALUES('PP1','lo',0,'','20');
INSERT INTO "gen_blcks_io" VALUES('PP1','Q0',0,'','1');
INSERT INTO "gen_blcks_io" VALUES('PP1','f_frq',0,'','100');
INSERT INTO "gen_blcks_io" VALUES('PP1','Pot',0,'','5.52904727002653');
INSERT INTO "gen_blcks_io" VALUES('PP1','Fit',0,'','73.952362760328');
INSERT INTO "gen_blcks_io" VALUES('PP3','Fi',0,'','98.7536070881041');
INSERT INTO "gen_blcks_io" VALUES('PP3','Pi',1,'node19.Po','24.5716373347379');
INSERT INTO "gen_blcks_io" VALUES('PP3','Fo',1,'BC6.Fi','98.7557629234302');
INSERT INTO "gen_blcks_io" VALUES('PP3','Po',0,'','24.2789251964647');
INSERT INTO "gen_blcks_io" VALUES('PP3','dP',0,'','29.2712318324959');
INSERT INTO "gen_blcks_io" VALUES('PP3','Sdf',0,'','0.06');
INSERT INTO "gen_blcks_io" VALUES('PP3','So',0,'','0.071');
INSERT INTO "gen_blcks_io" VALUES('PP3','lo',0,'','10');
INSERT INTO "gen_blcks_io" VALUES('PP3','Q0',0,'','1');
INSERT INTO "gen_blcks_io" VALUES('PP3','f_frq',0,'','100');
INSERT INTO "gen_blcks_io" VALUES('PP3','Pot',0,'','24.4923380432146');
INSERT INTO "gen_blcks_io" VALUES('PP3','Fit',0,'','71.9508231998964');
INSERT INTO "gen_blcks_io" VALUES('PP5','Fi',0,'','53.008643744917');
INSERT INTO "gen_blcks_io" VALUES('PP5','Pi',1,'C2.Po','23.8730160350743');
INSERT INTO "gen_blcks_io" VALUES('PP5','Fo',1,'node21.Fi','53.0097302953194');
INSERT INTO "gen_blcks_io" VALUES('PP5','Po',0,'','23.7482495897444');
INSERT INTO "gen_blcks_io" VALUES('PP5','dP',0,'','12.4775880364737');
INSERT INTO "gen_blcks_io" VALUES('PP5','Sdf',0,'','0.05');
INSERT INTO "gen_blcks_io" VALUES('PP5','So',0,'','0.071');
INSERT INTO "gen_blcks_io" VALUES('PP5','lo',0,'','5');
INSERT INTO "gen_blcks_io" VALUES('PP5','Q0',0,'','1');
INSERT INTO "gen_blcks_io" VALUES('PP5','f_frq',0,'','100');
INSERT INTO "gen_blcks_io" VALUES('PP5','Pot',0,'','23.5001559218607');
INSERT INTO "gen_blcks_io" VALUES('PP5','Fit',0,'','38.8654859427924');
INSERT INTO "gen_blcks_io" VALUES('PP7','Fi',0,'','51.4858620911399');
INSERT INTO "gen_blcks_io" VALUES('PP7','Pi',1,'node22.Po','92.2917684224309');
INSERT INTO "gen_blcks_io" VALUES('PP7','Fo',1,'PCV4.Fi','51.4858196089344');
INSERT INTO "gen_blcks_io" VALUES('PP7','Po',0,'','91.9531407343216');
INSERT INTO "gen_blcks_io" VALUES('PP7','dP',0,'','33.8676329206988');
INSERT INTO "gen_blcks_io" VALUES('PP7','Sdf',0,'','0.015');
INSERT INTO "gen_blcks_io" VALUES('PP7','So',0,'','0.017');
INSERT INTO "gen_blcks_io" VALUES('PP7','lo',0,'','10');
INSERT INTO "gen_blcks_io" VALUES('PP7','Q0',0,'','1');
INSERT INTO "gen_blcks_io" VALUES('PP7','f_frq',0,'','100');
INSERT INTO "gen_blcks_io" VALUES('PP7','Pot',0,'','91.2198881512939');
INSERT INTO "gen_blcks_io" VALUES('PP7','Fit',0,'','37.309848096239');
INSERT INTO "gen_blcks_io" VALUES('netTorch','Fi',0,'','-1.15736371721721e-09');
INSERT INTO "gen_blcks_io" VALUES('netTorch','Pi',1,'BC5.Po','1.00000000000206');
INSERT INTO "gen_blcks_io" VALUES('netTorch','Po',0,'','1');
INSERT INTO "gen_blcks_io" VALUES('netTorch','So',0,'','0.002');
INSERT INTO "gen_blcks_io" VALUES('netTorch','Kpr',0,'','0.95');
INSERT INTO "gen_blcks_io" VALUES('netTorch','Noise',0,'','1');
INSERT INTO "gen_blcks_io" VALUES('netTorch','Q0',0,'','1');
INSERT INTO "gen_blcks_io" VALUES('netTorch','f_frq',0,'','100');
INSERT INTO "gen_blcks_io" VALUES('BC1','Fi',0,'','102.661658873269');
INSERT INTO "gen_blcks_io" VALUES('BC1','Pi',1,'SrcGas.Po','9.04764182688553');
INSERT INTO "gen_blcks_io" VALUES('BC1','Ti',0,'','293');
INSERT INTO "gen_blcks_io" VALUES('BC1','Fo',1,'PCV1.Fi','102.662248739077');
INSERT INTO "gen_blcks_io" VALUES('BC1','Po',0,'','9.02681275422002');
INSERT INTO "gen_blcks_io" VALUES('BC1','To',0,'','292.967043155462');
INSERT INTO "gen_blcks_io" VALUES('BC1','So',0,'','0.385');
INSERT INTO "gen_blcks_io" VALUES('BC1','lo',0,'','5');
INSERT INTO "gen_blcks_io" VALUES('BC1','S_kl1',0,'','0.385');
INSERT INTO "gen_blcks_io" VALUES('BC1','S_kl2',0,'','0.02');
INSERT INTO "gen_blcks_io" VALUES('BC1','l_kl1',1,'G_BC1.pos','100');
INSERT INTO "gen_blcks_io" VALUES('BC1','l_kl2',1,'G_BC2.pos','0');
INSERT INTO "gen_blcks_io" VALUES('BC1','Q0',0,'','1');
INSERT INTO "gen_blcks_io" VALUES('BC1','Kln',0,'','1');
INSERT INTO "gen_blcks_io" VALUES('BC1','Kpr',0,'','0.95');
INSERT INTO "gen_blcks_io" VALUES('BC1','Ct',0,'','20');
INSERT INTO "gen_blcks_io" VALUES('BC1','Riz',0,'','20');
INSERT INTO "gen_blcks_io" VALUES('BC1','noBack',0,'','0');
INSERT INTO "gen_blcks_io" VALUES('BC1','Fwind',0,'','1');
INSERT INTO "gen_blcks_io" VALUES('BC1','Twind',0,'','273');
INSERT INTO "gen_blcks_io" VALUES('BC1','f_frq',0,'','100');
INSERT INTO "gen_blcks_io" VALUES('BC1','Pot',0,'','8.78987399044649');
INSERT INTO "gen_blcks_io" VALUES('BC1','Fit',0,'','163.572788647632');
INSERT INTO "gen_blcks_io" VALUES('G_BC21','pos',0,'','100');
INSERT INTO "gen_blcks_io" VALUES('G_BC21','com',0,'','1');
INSERT INTO "gen_blcks_io" VALUES('G_BC21','st_open',0,'','1');
INSERT INTO "gen_blcks_io" VALUES('G_BC21','st_close',0,'','0');
INSERT INTO "gen_blcks_io" VALUES('G_BC21','t_full',0,'','10');
INSERT INTO "gen_blcks_io" VALUES('G_BC21','t_up',0,'','0.6');
INSERT INTO "gen_blcks_io" VALUES('G_BC21','f_frq',0,'','100');
INSERT INTO "gen_blcks_io" VALUES('G_BC21','tmp_up',0,'','0');
INSERT INTO "gen_blcks_io" VALUES('G_BC21','lst_com',0,'','1');
INSERT INTO "gen_blcks_io" VALUES('BC5','Fi',0,'','0');
INSERT INTO "gen_blcks_io" VALUES('BC5','Pi',1,'node1.Po1','5.6477320491273');
INSERT INTO "gen_blcks_io" VALUES('BC5','Ti',1,'PCV1.To','290.373870468236');
INSERT INTO "gen_blcks_io" VALUES('BC5','Fo',1,'netTorch.Fi','-1.16665543763139e-09');
INSERT INTO "gen_blcks_io" VALUES('BC5','Po',0,'','0.999999999997951');
INSERT INTO "gen_blcks_io" VALUES('BC5','To',0,'','272.999999994003');
INSERT INTO "gen_blcks_io" VALUES('BC5','So',0,'','0.2');
INSERT INTO "gen_blcks_io" VALUES('BC5','lo',0,'','10');
INSERT INTO "gen_blcks_io" VALUES('BC5','S_kl1',0,'','0.002');
INSERT INTO "gen_blcks_io" VALUES('BC5','S_kl2',0,'','0.001');
INSERT INTO "gen_blcks_io" VALUES('BC5','l_kl1',1,'G_BC5.pos','0');
INSERT INTO "gen_blcks_io" VALUES('BC5','l_kl2',0,'','0');
INSERT INTO "gen_blcks_io" VALUES('BC5','Q0',0,'','1');
INSERT INTO "gen_blcks_io" VALUES('BC5','Kln',0,'','1');
INSERT INTO "gen_blcks_io" VALUES('BC5','Kpr',0,'','0.95');
INSERT INTO "gen_blcks_io" VALUES('BC5','Ct',0,'','20');
INSERT INTO "gen_blcks_io" VALUES('BC5','Riz',0,'','20');
INSERT INTO "gen_blcks_io" VALUES('BC5','noBack',0,'','0');
INSERT INTO "gen_blcks_io" VALUES('BC5','Fwind',0,'','1');
INSERT INTO "gen_blcks_io" VALUES('BC5','Twind',0,'','273');
INSERT INTO "gen_blcks_io" VALUES('BC5','f_frq',0,'','100');
INSERT INTO "gen_blcks_io" VALUES('BC5','Pot',0,'','1.000000002619');
INSERT INTO "gen_blcks_io" VALUES('BC5','Fit',0,'','2.47032822920623e-323');
INSERT INTO "gen_blcks_io" VALUES('BC6','Fi',0,'','98.7557744010762');
INSERT INTO "gen_blcks_io" VALUES('BC6','Pi',1,'PP3.Po','24.2789425971512');
INSERT INTO "gen_blcks_io" VALUES('BC6','Ti',1,'node19.To','305.186840972374');
INSERT INTO "gen_blcks_io" VALUES('BC6','Fo',1,'node20.Fi','98.7569008536047');
INSERT INTO "gen_blcks_io" VALUES('BC6','Po',0,'','24.061576778519');
INSERT INTO "gen_blcks_io" VALUES('BC6','To',0,'','305.125415787498');
INSERT INTO "gen_blcks_io" VALUES('BC6','So',0,'','0.071');
INSERT INTO "gen_blcks_io" VALUES('BC6','lo',0,'','5');
INSERT INTO "gen_blcks_io" VALUES('BC6','S_kl1',0,'','0.07');
INSERT INTO "gen_blcks_io" VALUES('BC6','S_kl2',0,'','0.002');
INSERT INTO "gen_blcks_io" VALUES('BC6','l_kl1',1,'G_BC6.pos','100');
INSERT INTO "gen_blcks_io" VALUES('BC6','l_kl2',1,'G_BC7.pos','0');
INSERT INTO "gen_blcks_io" VALUES('BC6','Q0',0,'','1');
INSERT INTO "gen_blcks_io" VALUES('BC6','Kln',0,'','1');
INSERT INTO "gen_blcks_io" VALUES('BC6','Kpr',0,'','0.95');
INSERT INTO "gen_blcks_io" VALUES('BC6','Ct',0,'','100');
INSERT INTO "gen_blcks_io" VALUES('BC6','Riz',0,'','100');
INSERT INTO "gen_blcks_io" VALUES('BC6','noBack',0,'','0');
INSERT INTO "gen_blcks_io" VALUES('BC6','Fwind',0,'','1');
INSERT INTO "gen_blcks_io" VALUES('BC6','Twind',0,'','273');
INSERT INTO "gen_blcks_io" VALUES('BC6','f_frq',0,'','100');
INSERT INTO "gen_blcks_io" VALUES('BC6','Pot',0,'','24.0824664654201');
INSERT INTO "gen_blcks_io" VALUES('BC6','Fit',0,'','71.9528087874881');
INSERT INTO "gen_blcks_io" VALUES('node1','Fi',0,'','101.643726962295');
INSERT INTO "gen_blcks_io" VALUES('node1','Pi',1,'PP1.Po','5.6477295043406');
INSERT INTO "gen_blcks_io" VALUES('node1','Fo1',1,'BC5.Fi','0');
INSERT INTO "gen_blcks_io" VALUES('node1','Po1',0,'','5.64772801670355');
INSERT INTO "gen_blcks_io" VALUES('node1','So1',0,'','0.02');
INSERT INTO "gen_blcks_io" VALUES('node1','lo1',0,'','20');
INSERT INTO "gen_blcks_io" VALUES('node1','Fo2',1,'node1_1.Fi','101.656447290804');
INSERT INTO "gen_blcks_io" VALUES('node1','Po2',0,'','5.58167742144463');
INSERT INTO "gen_blcks_io" VALUES('node1','So2',0,'','0.283');
INSERT INTO "gen_blcks_io" VALUES('node1','lo2',0,'','60');
INSERT INTO "gen_blcks_io" VALUES('node1','Q0',0,'','1');
INSERT INTO "gen_blcks_io" VALUES('node1','f_frq',0,'','100');
INSERT INTO "gen_blcks_io" VALUES('node1','F1tmp',0,'','-0.000289122725778342');
INSERT INTO "gen_blcks_io" VALUES('node1','F2tmp',0,'','101.644016085021');
INSERT INTO "gen_blcks_io" VALUES('node1','Pot1',0,'','5.48529527455907');
INSERT INTO "gen_blcks_io" VALUES('node1','Pot2',0,'','5.40120593993749');
INSERT INTO "gen_blcks_io" VALUES('node20','Fi',0,'','98.7568623793714');
INSERT INTO "gen_blcks_io" VALUES('node20','Pi',1,'BC6.Po','24.0615678435426');
INSERT INTO "gen_blcks_io" VALUES('node20','Fo1',1,'PCV2.Fi','45.2427140495495');
INSERT INTO "gen_blcks_io" VALUES('node20','Po1',0,'','24.0148601624755');
INSERT INTO "gen_blcks_io" VALUES('node20','So1',0,'','0.071');
INSERT INTO "gen_blcks_io" VALUES('node20','lo1',0,'','120');
INSERT INTO "gen_blcks_io" VALUES('node20','Fo2',1,'BC21.Fi','53.5415247504508');
INSERT INTO "gen_blcks_io" VALUES('node20','Po2',0,'','23.9988966747759');
INSERT INTO "gen_blcks_io" VALUES('node20','So2',0,'','0.071');
INSERT INTO "gen_blcks_io" VALUES('node20','lo2',0,'','5');
INSERT INTO "gen_blcks_io" VALUES('node20','Q0',0,'','1');
INSERT INTO "gen_blcks_io" VALUES('node20','f_frq',0,'','100');
INSERT INTO "gen_blcks_io" VALUES('node20','F1tmp',0,'','45.2164597571731');
INSERT INTO "gen_blcks_io" VALUES('node20','F2tmp',0,'','53.540391063035');
INSERT INTO "gen_blcks_io" VALUES('node20','Pot1',0,'','23.9995804567437');
INSERT INTO "gen_blcks_io" VALUES('node20','Pot2',0,'','23.9683452808346');
INSERT INTO "gen_blcks_io" VALUES('node21','Fi',0,'','53.0096212933064');
INSERT INTO "gen_blcks_io" VALUES('node21','Pi',1,'PP5.Po','23.7482408843624');
INSERT INTO "gen_blcks_io" VALUES('node21','Fo1',2,'CM102.BC111.Fi','53.0186463186137');
INSERT INTO "gen_blcks_io" VALUES('node21','Po1',6,'CM102.BC111.Pi','22.9498631223434');
INSERT INTO "gen_blcks_io" VALUES('node21','So1',0,'','0.02');
INSERT INTO "gen_blcks_io" VALUES('node21','lo1',0,'','50');
INSERT INTO "gen_blcks_io" VALUES('node21','Fo2',2,'CM202.BC111.Fi','-1.23516411460312e-321');
INSERT INTO "gen_blcks_io" VALUES('node21','Po2',6,'CM202.BC111.Pi','23.7482345404507');
INSERT INTO "gen_blcks_io" VALUES('node21','So2',0,'','0.02');
INSERT INTO "gen_blcks_io" VALUES('node21','lo2',0,'','50');
INSERT INTO "gen_blcks_io" VALUES('node21','Fo3',2,'CM302.BC111.Fi','-1.23516411460312e-321');
INSERT INTO "gen_blcks_io" VALUES('node21','Po3',6,'CM302.BC111.Pi','23.7482340062545');
INSERT INTO "gen_blcks_io" VALUES('node21','So3',0,'','0.02');
INSERT INTO "gen_blcks_io" VALUES('node21','lo3',0,'','50');
INSERT INTO "gen_blcks_io" VALUES('node21','Q0',0,'','1');
INSERT INTO "gen_blcks_io" VALUES('node21','f_frq',0,'','100');
INSERT INTO "gen_blcks_io" VALUES('node21','F1tmp',0,'','53.0156397347477');
INSERT INTO "gen_blcks_io" VALUES('node21','F2tmp',0,'','-0.00299486962464089');
INSERT INTO "gen_blcks_io" VALUES('node21','F3tmp',0,'','-0.00302357181664297');
INSERT INTO "gen_blcks_io" VALUES('node21','Pot1',0,'','22.0182441164267');
INSERT INTO "gen_blcks_io" VALUES('node21','Pot2',0,'','23.8828420056428');
INSERT INTO "gen_blcks_io" VALUES('node21','Pot3',0,'','23.0910838961391');
INSERT INTO "gen_blcks_io" VALUES('node22','Fi1',6,'CM102.BC114.Fo','51.4849664040682');
INSERT INTO "gen_blcks_io" VALUES('node22','Pi1',2,'CM102.BC114.Po','93.8216133824026');
INSERT INTO "gen_blcks_io" VALUES('node22','Ti1',2,'CM102.BC114.To','302.486586227806');
INSERT INTO "gen_blcks_io" VALUES('node22','Si1',0,'','0.007');
INSERT INTO "gen_blcks_io" VALUES('node22','Fi2',6,'CM202.BC114.Fo','-0.00584144880025391');
INSERT INTO "gen_blcks_io" VALUES('node22','Pi2',2,'CM202.BC114.Po','92.2918511544472');
INSERT INTO "gen_blcks_io" VALUES('node22','Ti2',2,'CM202.BC114.To','273.00000001007');
INSERT INTO "gen_blcks_io" VALUES('node22','Si2',0,'','0.007');
INSERT INTO "gen_blcks_io" VALUES('node22','Fi3',6,'CM302.BC114.Fo','0.00693089762456782');
INSERT INTO "gen_blcks_io" VALUES('node22','Pi3',2,'CM302.BC114.Po','92.2917533949765');
INSERT INTO "gen_blcks_io" VALUES('node22','Ti3',2,'CM302.BC114.To','273.000000010185');
INSERT INTO "gen_blcks_io" VALUES('node22','Si3',0,'','0.007');
INSERT INTO "gen_blcks_io" VALUES('node22','Fo',1,'PP7.Fi','51.4857689136838');
INSERT INTO "gen_blcks_io" VALUES('node22','Po',0,'','92.2917779003813');
INSERT INTO "gen_blcks_io" VALUES('node22','To',0,'','302.428914981147');
INSERT INTO "gen_blcks_io" VALUES('node22','So',0,'','0.007');
INSERT INTO "gen_blcks_io" VALUES('node22','lo',0,'','10');
INSERT INTO "gen_blcks_io" VALUES('node22','Q0',0,'','1');
INSERT INTO "gen_blcks_io" VALUES('node22','Ct',0,'','20');
INSERT INTO "gen_blcks_io" VALUES('node22','Riz',0,'','20');
INSERT INTO "gen_blcks_io" VALUES('node22','Fwind',0,'','1');
INSERT INTO "gen_blcks_io" VALUES('node22','Twind',0,'','273');
INSERT INTO "gen_blcks_io" VALUES('node22','f_frq',0,'','100');
INSERT INTO "gen_blcks_io" VALUES('node22','FoR',0,'','59.9999999999999');
INSERT INTO "gen_blcks_io" VALUES('node22','Pot',0,'','91.8396872231701');
INSERT INTO "gen_blcks_io" VALUES('node22','Fit',0,'','37.3244663061258');
INSERT INTO "gen_blcks_io" VALUES('node19','Fi1',6,'CM101.BC104.Fo','49.4610071460718');
INSERT INTO "gen_blcks_io" VALUES('node19','Pi1',2,'CM101.BC104.Po','24.8399594037222');
INSERT INTO "gen_blcks_io" VALUES('node19','Ti1',2,'CM101.BC104.To','305.973238083207');
INSERT INTO "gen_blcks_io" VALUES('node19','Si1',0,'','0.0314');
INSERT INTO "gen_blcks_io" VALUES('node19','Fi2',6,'CM201.BC104.Fo','49.2796044267781');
INSERT INTO "gen_blcks_io" VALUES('node19','Pi2',2,'CM201.BC104.Po','24.8380158906571');
INSERT INTO "gen_blcks_io" VALUES('node19','Ti2',2,'CM201.BC104.To','304.468863873254');
INSERT INTO "gen_blcks_io" VALUES('node19','Si2',0,'','0.0314');
INSERT INTO "gen_blcks_io" VALUES('node19','Fi3',6,'CM301.BC104.Fo','0.0093474318399552');
INSERT INTO "gen_blcks_io" VALUES('node19','Pi3',2,'CM301.BC104.Po','24.5716428611888');
INSERT INTO "gen_blcks_io" VALUES('node19','Ti3',2,'CM301.BC104.To','273.000000009146');
INSERT INTO "gen_blcks_io" VALUES('node19','Si3',0,'','0.0314');
INSERT INTO "gen_blcks_io" VALUES('node19','Fo',1,'PP3.Fi','98.7536070881041');
INSERT INTO "gen_blcks_io" VALUES('node19','Po',0,'','24.5716290797721');
INSERT INTO "gen_blcks_io" VALUES('node19','To',0,'','305.186837605875');
INSERT INTO "gen_blcks_io" VALUES('node19','So',0,'','0.0314');
INSERT INTO "gen_blcks_io" VALUES('node19','lo',0,'','40');
INSERT INTO "gen_blcks_io" VALUES('node19','Q0',0,'','1');
INSERT INTO "gen_blcks_io" VALUES('node19','Ct',0,'','20');
INSERT INTO "gen_blcks_io" VALUES('node19','Riz',0,'','20');
INSERT INTO "gen_blcks_io" VALUES('node19','Fwind',0,'','1');
INSERT INTO "gen_blcks_io" VALUES('node19','Twind',0,'','273');
INSERT INTO "gen_blcks_io" VALUES('node19','f_frq',0,'','100');
INSERT INTO "gen_blcks_io" VALUES('node19','FoR',0,'','85.0636924379784');
INSERT INTO "gen_blcks_io" VALUES('node19','Pot',0,'','25.0241377766255');
INSERT INTO "gen_blcks_io" VALUES('node19','Fit',0,'','71.9722775668757');
INSERT INTO "gen_blcks_io" VALUES('C1','Fi',0,'','102.665167671339');
INSERT INTO "gen_blcks_io" VALUES('C1','Pi',1,'PCV1.Po','5.80065771708588');
INSERT INTO "gen_blcks_io" VALUES('C1','Si',0,'','0.385');
INSERT INTO "gen_blcks_io" VALUES('C1','Fo',1,'PP1.Fi','101.639617309051');
INSERT INTO "gen_blcks_io" VALUES('C1','Po',0,'','5.76808590306495');
INSERT INTO "gen_blcks_io" VALUES('C1','So',0,'','0.283');
INSERT INTO "gen_blcks_io" VALUES('C1','lo',0,'','5');
INSERT INTO "gen_blcks_io" VALUES('C1','Fo_ж',0,'','0');
INSERT INTO "gen_blcks_io" VALUES('C1','Po_ж',0,'','5.8671541772125');
INSERT INTO "gen_blcks_io" VALUES('C1','Lж',0,'','1.23835342684432');
INSERT INTO "gen_blcks_io" VALUES('C1','ProcЖ',0,'','0.01');
INSERT INTO "gen_blcks_io" VALUES('C1','Vap',0,'','80');
INSERT INTO "gen_blcks_io" VALUES('C1','Q0',0,'','1');
INSERT INTO "gen_blcks_io" VALUES('C1','Qж',0,'','1000');
INSERT INTO "gen_blcks_io" VALUES('C1','f_frq',0,'','100');
INSERT INTO "gen_blcks_io" VALUES('C1','Pot',0,'','5.74237264252839');
INSERT INTO "gen_blcks_io" VALUES('C1','Fit',0,'','73.6939262990129');
INSERT INTO "gen_blcks_io" VALUES('C2','Fi',0,'','53.543094274057');
INSERT INTO "gen_blcks_io" VALUES('C2','Pi',1,'BC21.Po','23.9360294145421');
INSERT INTO "gen_blcks_io" VALUES('C2','Si',0,'','0.071');
INSERT INTO "gen_blcks_io" VALUES('C2','Fo',1,'PP5.Fi','53.0087614391216');
INSERT INTO "gen_blcks_io" VALUES('C2','Po',0,'','23.8730247718798');
INSERT INTO "gen_blcks_io" VALUES('C2','So',0,'','0.071');
INSERT INTO "gen_blcks_io" VALUES('C2','lo',0,'','5');
INSERT INTO "gen_blcks_io" VALUES('C2','Fo_ж',0,'','0');
INSERT INTO "gen_blcks_io" VALUES('C2','Po_ж',0,'','23.9322897749853');
INSERT INTO "gen_blcks_io" VALUES('C2','Lж',0,'','1.69328580301329');
INSERT INTO "gen_blcks_io" VALUES('C2','ProcЖ',0,'','0.01');
INSERT INTO "gen_blcks_io" VALUES('C2','Vap',0,'','35');
INSERT INTO "gen_blcks_io" VALUES('C2','Q0',0,'','1');
INSERT INTO "gen_blcks_io" VALUES('C2','Qж',0,'','1000');
INSERT INTO "gen_blcks_io" VALUES('C2','f_frq',0,'','100');
INSERT INTO "gen_blcks_io" VALUES('C2','Pot',0,'','23.7347401295504');
INSERT INTO "gen_blcks_io" VALUES('C2','Fit',0,'','38.4665359341126');
INSERT INTO "gen_blcks_io" VALUES('G_BC1','pos',0,'','100');
INSERT INTO "gen_blcks_io" VALUES('G_BC1','com',0,'','1');
INSERT INTO "gen_blcks_io" VALUES('G_BC1','st_open',0,'','1');
INSERT INTO "gen_blcks_io" VALUES('G_BC1','st_close',0,'','0');
INSERT INTO "gen_blcks_io" VALUES('G_BC1','t_full',0,'','5');
INSERT INTO "gen_blcks_io" VALUES('G_BC1','t_up',0,'','0.5');
INSERT INTO "gen_blcks_io" VALUES('G_BC1','f_frq',0,'','100');
INSERT INTO "gen_blcks_io" VALUES('G_BC1','tmp_up',0,'','0');
INSERT INTO "gen_blcks_io" VALUES('G_BC1','lst_com',0,'','1');
INSERT INTO "gen_blcks_io" VALUES('G_BC2','pos',0,'','0');
INSERT INTO "gen_blcks_io" VALUES('G_BC2','com',0,'','0');
INSERT INTO "gen_blcks_io" VALUES('G_BC2','st_open',0,'','0');
INSERT INTO "gen_blcks_io" VALUES('G_BC2','st_close',0,'','1');
INSERT INTO "gen_blcks_io" VALUES('G_BC2','t_full',0,'','3');
INSERT INTO "gen_blcks_io" VALUES('G_BC2','t_up',0,'','0.3');
INSERT INTO "gen_blcks_io" VALUES('G_BC2','f_frq',0,'','100');
INSERT INTO "gen_blcks_io" VALUES('G_BC2','tmp_up',0,'','0');
INSERT INTO "gen_blcks_io" VALUES('G_BC2','lst_com',0,'','0');
INSERT INTO "gen_blcks_io" VALUES('G_BC5','pos',0,'','0');
INSERT INTO "gen_blcks_io" VALUES('G_BC5','com',0,'','0');
INSERT INTO "gen_blcks_io" VALUES('G_BC5','st_open',0,'','0');
INSERT INTO "gen_blcks_io" VALUES('G_BC5','st_close',0,'','1');
INSERT INTO "gen_blcks_io" VALUES('G_BC5','t_full',0,'','5');
INSERT INTO "gen_blcks_io" VALUES('G_BC5','t_up',0,'','0.5');
INSERT INTO "gen_blcks_io" VALUES('G_BC5','f_frq',0,'','100');
INSERT INTO "gen_blcks_io" VALUES('G_BC5','tmp_up',0,'','0');
INSERT INTO "gen_blcks_io" VALUES('G_BC5','lst_com',0,'','0');
INSERT INTO "gen_blcks_io" VALUES('PCV1','t_kl1',0,'','5');
INSERT INTO "gen_blcks_io" VALUES('PCV1','t_kl2',0,'','5');
INSERT INTO "gen_blcks_io" VALUES('PCV1','tmp_l1',0,'','39.3087096097981');
INSERT INTO "gen_blcks_io" VALUES('PCV1','tmp_l2',0,'','0');
INSERT INTO "gen_blcks_io" VALUES('node1_1','Kpr',0,'','0.95');
INSERT INTO "gen_blcks_io" VALUES('BC21','t_kl1',0,'','10');
INSERT INTO "gen_blcks_io" VALUES('BC21','t_kl2',0,'','5');
INSERT INTO "gen_blcks_io" VALUES('BC21','tmp_l1',0,'','99.9999999999929');
INSERT INTO "gen_blcks_io" VALUES('BC21','tmp_l2',0,'','0');
INSERT INTO "gen_blcks_io" VALUES('SrcGas','Kpr',0,'','0.95');
INSERT INTO "gen_blcks_io" VALUES('PCV2','t_kl1',0,'','3');
INSERT INTO "gen_blcks_io" VALUES('PCV2','t_kl2',0,'','5');
INSERT INTO "gen_blcks_io" VALUES('PCV2','tmp_l1',0,'','53.5979377087787');
INSERT INTO "gen_blcks_io" VALUES('PCV2','tmp_l2',0,'','0');
INSERT INTO "gen_blcks_io" VALUES('pipeGlinsk','Kpr',0,'','0.95');
INSERT INTO "gen_blcks_io" VALUES('PCV4','t_kl1',0,'','10');
INSERT INTO "gen_blcks_io" VALUES('PCV4','t_kl2',0,'','5');
INSERT INTO "gen_blcks_io" VALUES('PCV4','tmp_l1',0,'','48.1306331796677');
INSERT INTO "gen_blcks_io" VALUES('PCV4','tmp_l2',0,'','0');
INSERT INTO "gen_blcks_io" VALUES('PP1','Kpr',0,'','0.95');
INSERT INTO "gen_blcks_io" VALUES('PP3','Kpr',0,'','0.95');
INSERT INTO "gen_blcks_io" VALUES('PP5','Kpr',0,'','0.95');
INSERT INTO "gen_blcks_io" VALUES('PP7','Kpr',0,'','0.95');
INSERT INTO "gen_blcks_io" VALUES('BC1','t_kl1',0,'','10');
INSERT INTO "gen_blcks_io" VALUES('BC1','t_kl2',0,'','5');
INSERT INTO "gen_blcks_io" VALUES('BC1','tmp_l1',0,'','99.9999999999929');
INSERT INTO "gen_blcks_io" VALUES('BC1','tmp_l2',0,'','1.23516411460312e-321');
INSERT INTO "gen_blcks_io" VALUES('BC5','t_kl1',0,'','10');
INSERT INTO "gen_blcks_io" VALUES('BC5','t_kl2',0,'','5');
INSERT INTO "gen_blcks_io" VALUES('BC5','tmp_l1',0,'','0');
INSERT INTO "gen_blcks_io" VALUES('BC5','tmp_l2',0,'','0');
INSERT INTO "gen_blcks_io" VALUES('BC6','t_kl1',0,'','10');
INSERT INTO "gen_blcks_io" VALUES('BC6','t_kl2',0,'','5');
INSERT INTO "gen_blcks_io" VALUES('BC6','tmp_l1',0,'','99.9999999999929');
INSERT INTO "gen_blcks_io" VALUES('BC6','tmp_l2',0,'','0');
INSERT INTO "gen_blcks_io" VALUES('node1','Kpr',0,'','0.95');
INSERT INTO "gen_blcks_io" VALUES('node20','Kpr',0,'','0.95');
INSERT INTO "gen_blcks_io" VALUES('node21','Kpr',0,'','0.95');
INSERT INTO "gen_blcks_io" VALUES('node22','Kpr',0,'','0.95');
INSERT INTO "gen_blcks_io" VALUES('node19','Kpr',0,'','0.95');
CREATE TABLE 'gen_cntr_blocks_io' ("BLK_ID" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"TLNK" INTEGER DEFAULT '' ,"LNK" TEXT DEFAULT '' ,"VAL" TEXT DEFAULT '' , PRIMARY KEY ("BLK_ID","ID"));
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV2','var',2,'gen.P_PCV2.out','24.0150870588492');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV2','sp',0,'','24');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV2','max',0,'','50');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV2','min',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV2','out',0,'','53.6014729388859');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV2','auto',0,'','1');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV2','casc',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV2','Kp',0,'','-0.2');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV2','Ti',0,'','5000');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV2','Td',0,'','1000');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV2','Tf',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV2','Hup',0,'','100');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV2','Hdwn',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV2','Zi',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV2','K1',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV2','in1',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV2','K2',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV2','in2',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV2','K3',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV2','in3',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV2','K4',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV2','in4',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV2','cycle',0,'','1000');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV2','int',0,'','53.5940626802538');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV2','dif',0,'','48.0315495527907');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV2','lag',0,'','0.00137543509235627');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV1','var',2,'gen.PCV1.Po','5.80066664471415');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV1','sp',0,'','5.8');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV1','max',0,'','10');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV1','min',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV1','out',0,'','39.302204328147');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV1','auto',0,'','1');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV1','casc',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV1','Kp',0,'','1');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV1','Ti',0,'','5000');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV1','Td',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV1','Tf',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV1','Hup',0,'','100');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV1','Hdwn',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV1','Zi',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV1','K1',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV1','in1',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV1','K2',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV1','in2',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV1','K3',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV1','in3',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV1','K4',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV1','in4',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV1','cycle',0,'','1000');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV1','int',0,'','39.3088707752885');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV1','dif',0,'','58.0066664471415');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV1','lag',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV3','var',2,'gen.PCV4.Pi','91.9531322319234');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV3','sp',0,'','91.9539072904281');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV3','max',0,'','120');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV3','min',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV3','out',6,'gen.PCV4.l_kl1','48.1299650661653');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV3','auto',0,'','1');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV3','casc',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV3','Kp',0,'','-3');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV3','Ti',0,'','5000');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV3','Td',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV3','Tf',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV3','Hup',0,'','100');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV3','Hdwn',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV3','Zi',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV3','K1',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV3','in1',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV3','K2',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV3','in2',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV3','K3',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV3','in3',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV3','K4',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV3','in4',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV3','cycle',0,'','1000');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV3','int',0,'','48.131902712427');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV3','dif',0,'','76.6276101932695');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV3','lag',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV3','manIn',0,'','48.1299650661653');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV2','manIn',0,'','53.6014729388859');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV2','f_frq',0,'','5');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV1','manIn',0,'','39.302204328147');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV1','f_frq',0,'','5');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV3','f_frq',0,'','5');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV2','Kd',0,'','1');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV2','Tzd',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV2','followSp',0,'','1');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV1','Kd',0,'','1');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV1','Tzd',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV1','followSp',0,'','1');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV3','Kd',0,'','1');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV3','Tzd',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('PC_PCV3','followSp',0,'','1');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM101','var',2,'CM101.CM101_2.Kzp','0.884521502705757');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM101','sp',0,'','0.9');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM101','max',0,'','2');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM101','min',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM101','manIn',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM101','out',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM101','auto',0,'','1');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM101','casc',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM101','Kp',0,'','-1');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM101','Ti',0,'','5000');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM101','Kd',0,'','1');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM101','Td',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM101','Tzd',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM101','Hup',0,'','100');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM101','Hdwn',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM101','Zi',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM101','followSp',0,'','1');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM101','K1',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM101','in1',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM101','K2',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM101','in2',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM101','K3',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM101','in3',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM101','K4',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM101','in4',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM101','f_frq',0,'','5');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM101','int',0,'','0.773924864712129');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM101','dif',0,'','44.2260751352879');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM101','lag',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM201','var',2,'CM201.CM101_2.Kzp','0.886553812492069');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM201','sp',0,'','0.9');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM201','max',0,'','2');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM201','min',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM201','manIn',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM201','out',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM201','auto',0,'','1');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM201','casc',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM201','Kp',0,'','-1');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM201','Ti',0,'','5000');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM201','Kd',0,'','1');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM201','Td',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM201','Tzd',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM201','Hup',0,'','100');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM201','Hdwn',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM201','Zi',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM201','followSp',0,'','1');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM201','K1',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM201','in1',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM201','K2',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM201','in2',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM201','K3',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM201','in3',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM201','K4',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM201','in4',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM201','f_frq',0,'','5');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM201','int',0,'','0.672309375396544');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM201','dif',0,'','44.3276906246035');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM201','lag',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM301','var',2,'CM301.CM101_2.Kzp','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM301','sp',0,'','0.9');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM301','max',0,'','2');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM301','min',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM301','manIn',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM301','out',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM301','auto',0,'','1');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM301','casc',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM301','Kp',0,'','-1');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM301','Ti',0,'','5000');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM301','Kd',0,'','1');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM301','Td',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM301','Tzd',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM301','Hup',0,'','100');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM301','Hdwn',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM301','Zi',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM301','followSp',0,'','1');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM301','K1',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM301','in1',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM301','K2',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM301','in2',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM301','K3',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM301','in3',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM301','K4',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM301','in4',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM301','f_frq',0,'','5');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM301','int',0,'','45');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM301','dif',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM301','lag',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM102','var',2,'CM102.CM102_2.Kzp','0.499172347825233');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM102','sp',0,'','0.9');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM102','max',0,'','2');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM102','min',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM102','manIn',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM102','out',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM102','auto',0,'','1');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM102','casc',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM102','Kp',0,'','-1');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM102','Ti',0,'','5000');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM102','Kd',0,'','1');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM102','Td',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM102','Tzd',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM102','Hup',0,'','100');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM102','Hdwn',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM102','Zi',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM102','followSp',0,'','1');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM102','K1',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM102','in1',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM102','K2',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM102','in2',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM102','K3',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM102','in3',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM102','K4',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM102','in4',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM102','f_frq',0,'','5');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM102','int',0,'','20.0413826087383');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM102','dif',0,'','24.9586173912617');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM102','lag',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM202','var',2,'CM202.CM102_2.Kzp','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM202','sp',0,'','0.9');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM202','max',0,'','2');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM202','min',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM202','manIn',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM202','out',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM202','auto',0,'','1');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM202','casc',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM202','Kp',0,'','-1');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM202','Ti',0,'','5000');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM202','Kd',0,'','1');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM202','Td',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM202','Tzd',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM202','Hup',0,'','100');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM202','Hdwn',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM202','Zi',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM202','followSp',0,'','1');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM202','K1',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM202','in1',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM202','K2',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM202','in2',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM202','K3',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM202','in3',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM202','K4',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM202','in4',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM202','f_frq',0,'','5');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM202','int',0,'','45');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM202','dif',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM202','lag',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM302','var',2,'CM302.CM102_2.Kzp','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM302','sp',0,'','0.9');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM302','max',0,'','2');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM302','min',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM302','manIn',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM302','out',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM302','auto',0,'','1');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM302','casc',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM302','Kp',0,'','-1');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM302','Ti',0,'','5000');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM302','Kd',0,'','1');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM302','Td',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM302','Tzd',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM302','Hup',0,'','100');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM302','Hdwn',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM302','Zi',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM302','followSp',0,'','1');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM302','K1',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM302','in1',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM302','K2',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM302','in2',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM302','K3',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM302','in3',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM302','K4',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM302','in4',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM302','f_frq',0,'','5');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM302','int',0,'','45');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM302','dif',0,'','0');
INSERT INTO "gen_cntr_blocks_io" VALUES('SurgeCM302','lag',0,'','0');
CREATE TABLE 'gen_cntr_prm' ("SHIFR" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '' ,"IO" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#IO" TEXT DEFAULT '' ,"uk#IO" TEXT DEFAULT '' , PRIMARY KEY ("SHIFR"));
INSERT INTO "gen_cntr_prm" VALUES('PC_PCV1','PC_PCV1','Regulator of the input pressure of the CS',1,'PC_PCV1.sp:sp
PC_PCV1.var:var
*s.at:ed:Dimension
*i.1:prec:Precision
PC_PCV1.out:out
PC_PCV1.manIn:manIn
PC_PCV1.max:max
PC_PCV1.min:min
PC_PCV1.auto:auto
PC_PCV1.Kp:Kp
PC_PCV1.Ti:Ti
PC_PCV1.Kd:Kd
PC_PCV1.Td:Td
PC_PCV1.Tzd:Tzd
PC_PCV1.Tf:Tf
PC_PCV1.Zi:Zi
PC_PCV1.Hup:Hup
PC_PCV1.Hdwn:Hdwn','PC КРД1','Регулятор давления на входе КС','РС_КРТ1','Регулятор тиску на вході КС','PC_PCV1.sp:sp
PC_PCV1.var:var
*s.ат:ed:Ед. измерения
*i.1:prec:Точность
PC_PCV1.out:out
PC_PCV1.manIn:manIn
PC_PCV1.max:max
PC_PCV1.min:min
PC_PCV1.auto:auto
PC_PCV1.Kp:Kp
PC_PCV1.Ti:Ti
PC_PCV1.Kd:Kd
PC_PCV1.Td:Td
PC_PCV1.Tzd:Tzd
PC_PCV1.Tf:Tf
PC_PCV1.Zi:Zi
PC_PCV1.Hup:Hup
PC_PCV1.Hdwn:Hdwn','PC_PCV1.sp:sp
PC_PCV1.var:var
*s.ат:ed:Од.виміру
*i.1:prec:Точність
PC_PCV1.out:out
PC_PCV1.manIn:manIn
PC_PCV1.max:max
PC_PCV1.min:min
PC_PCV1.auto:auto
PC_PCV1.Kp:Kp
PC_PCV1.Ti:Ti
PC_PCV1.Kd:Kd
PC_PCV1.Td:Td
PC_PCV1.Tzd:Tzd
PC_PCV1.Tf:Tf
PC_PCV1.Zi:Zi
PC_PCV1.Hup:Hup
PC_PCV1.Hdwn:Hdwn');
INSERT INTO "gen_cntr_prm" VALUES('PC_PCV2','PC_PCV2','Regulator of the pressure between of the low and high pressure compressors',1,'PC_PCV2.sp:sp
PC_PCV2.var:var
*s.at:ed:Dimension
*i.1:prec:Precision
PC_PCV2.out:out
PC_PCV2.manIn:manIn
PC_PCV2.max:max
PC_PCV2.min:min
PC_PCV2.auto:auto
PC_PCV2.Kp:Kp
PC_PCV2.Ti:Ti
PC_PCV2.Kd:Kd
PC_PCV2.Td:Td
PC_PCV2.Tzd:Tzd
PC_PCV2.Tf:Tf
PC_PCV2.Zi:Zi
PC_PCV2.Hup:Hup
PC_PCV2.Hdwn:Hdwn','PC КРД2','Регулятор давления между компресорами низкого и высокого давленя','РС_КРТ2','Регулятор тиску між компресорами низького та високого тиску','PC_PCV2.sp:sp
PC_PCV2.var:var
*s.ат:ed:Ед. измерения
*i.1:prec:Точность
PC_PCV2.out:out
PC_PCV2.manIn:manIn
PC_PCV2.max:max
PC_PCV2.min:min
PC_PCV2.auto:auto
PC_PCV2.Kp:Kp
PC_PCV2.Ti:Ti
PC_PCV2.Kd:Kd
PC_PCV2.Td:Td
PC_PCV2.Tzd:Tzd
PC_PCV2.Tf:Tf
PC_PCV2.Zi:Zi
PC_PCV2.Hup:Hup
PC_PCV2.Hdwn:Hdwn','PC_PCV2.sp:sp
PC_PCV2.var:var
*s.ат:ed:Од.виміру
*i.1:prec:Точність
PC_PCV2.out:out
PC_PCV2.manIn:manIn
PC_PCV2.max:max
PC_PCV2.min:min
PC_PCV2.auto:auto
PC_PCV2.Kp:Kp
PC_PCV2.Ti:Ti
PC_PCV2.Kd:Kd
PC_PCV2.Td:Td
PC_PCV2.Tzd:Tzd
PC_PCV2.Tf:Tf
PC_PCV2.Zi:Zi
PC_PCV2.Hup:Hup
PC_PCV2.Hdwn:Hdwn');
INSERT INTO "gen_cntr_prm" VALUES('PC_PCV3','PC_PCV3','Regulator of the pressure at the output of the CS',1,'PC_PCV3.sp:sp
PC_PCV3.var:var
*s.at:ed:Dimension
*i.1:prec:Precision
PC_PCV3.out:out
PC_PCV3.manIn:manIn
PC_PCV3.max:max
PC_PCV3.min:min
PC_PCV3.auto:auto
PC_PCV3.Kp:Kp
PC_PCV3.Ti:Ti
PC_PCV3.Kd:Kd
PC_PCV3.Td:Td
PC_PCV3.Tzd:Tzd
PC_PCV3.Tf:Tf
PC_PCV3.Zi:Zi
PC_PCV3.Hup:Hup
PC_PCV3.Hdwn:Hdwn','PC КРД3','Регулятор давления на выходе КС','РС_КРТ3','Регулятор тиску на виході КС','PC_PCV3.sp:sp
PC_PCV3.var:var
*s.ат:ed:Ед. измерения
*i.1:prec:Точность
PC_PCV3.out:out
PC_PCV3.manIn:manIn
PC_PCV3.max:max
PC_PCV3.min:min
PC_PCV3.auto:auto
PC_PCV3.Kp:Kp
PC_PCV3.Ti:Ti
PC_PCV3.Kd:Kd
PC_PCV3.Td:Td
PC_PCV3.Tzd:Tzd
PC_PCV3.Tf:Tf
PC_PCV3.Zi:Zi
PC_PCV3.Hup:Hup
PC_PCV3.Hdwn:Hdwn','PC_PCV3.sp:sp
PC_PCV3.var:var
*s.ат:ed:Од.виміру
*i.1:prec:Точність
PC_PCV3.out:out
PC_PCV3.manIn:manIn
PC_PCV3.max:max
PC_PCV3.min:min
PC_PCV3.auto:auto
PC_PCV3.Kp:Kp
PC_PCV3.Ti:Ti
PC_PCV3.Kd:Kd
PC_PCV3.Td:Td
PC_PCV3.Tzd:Tzd
PC_PCV3.Tf:Tf
PC_PCV3.Zi:Zi
PC_PCV3.Hup:Hup
PC_PCV3.Hdwn:Hdwn');
INSERT INTO "gen_cntr_prm" VALUES('SurgeCM101','SurgeCM101','Regulator of the surge protection of the compressor CM101',1,'SurgeCM101.sp:sp
SurgeCM101.var:var
*s.at:ed:Dimension
*i.2:prec:Precision
SurgeCM101.out:out
SurgeCM101.manIn:manIn
SurgeCM101.max:max
SurgeCM101.min:min
SurgeCM101.auto:auto
SurgeCM101.Kp:Kp
SurgeCM101.Ti:Ti
SurgeCM101.Kd:Kd
SurgeCM101.Td:Td
SurgeCM101.Tzd:Tzd
SurgeCM101.Tf:Tf
SurgeCM101.Zi:Zi
SurgeCM101.Hup:Hup
SurgeCM101.Hdwn:Hdwn','ПомпажКМ101','Регулятор защиты от помпажа компресора КМ101','ПомпажКМ101','Регулятор захисту від помпажу компресору КМ101','SurgeCM101.sp:sp
SurgeCM101.var:var
*s.ат:ed:Единица
*i.2:prec:Точность
SurgeCM101.out:out
SurgeCM101.manIn:manIn
SurgeCM101.max:max
SurgeCM101.min:min
SurgeCM101.auto:auto
SurgeCM101.Kp:Kp
SurgeCM101.Ti:Ti
SurgeCM101.Kd:Kd
SurgeCM101.Td:Td
SurgeCM101.Tzd:Tzd
SurgeCM101.Tf:Tf
SurgeCM101.Zi:Zi
SurgeCM101.Hup:Hup
SurgeCM101.Hdwn:Hdwn','SurgeCM101.sp:sp
SurgeCM101.var:var
*s.ат:ed:Одиниця
*i.2:prec:Точність
SurgeCM101.out:out
SurgeCM101.manIn:manIn
SurgeCM101.max:max
SurgeCM101.min:min
SurgeCM101.auto:auto
SurgeCM101.Kp:Kp
SurgeCM101.Ti:Ti
SurgeCM101.Kd:Kd
SurgeCM101.Td:Td
SurgeCM101.Tzd:Tzd
SurgeCM101.Tf:Tf
SurgeCM101.Zi:Zi
SurgeCM101.Hup:Hup
SurgeCM101.Hdwn:Hdwn');
INSERT INTO "gen_cntr_prm" VALUES('SurgeCM201','SurgeCM201','Regulator of the surge protection of the compressor CM201',1,'SurgeCM201.sp:sp
SurgeCM201.var:var
*s.at:ed:Dimension
*i.2:prec:Precision
SurgeCM201.out:out
SurgeCM201.manIn:manIn
SurgeCM201.max:max
SurgeCM201.min:min
SurgeCM201.auto:auto
SurgeCM201.Kp:Kp
SurgeCM201.Ti:Ti
SurgeCM201.Kd:Kd
SurgeCM201.Td:Td
SurgeCM201.Tzd:Tzd
SurgeCM201.Tf:Tf
SurgeCM201.Zi:Zi
SurgeCM201.Hup:Hup
SurgeCM201.Hdwn:Hdwn','ПомпажКМ201','Регулятор защиты от помпажа компресора КМ201','ПомпажКМ201','Регулятор захисту від помпажу компресору КМ201','SurgeCM201.sp:sp
SurgeCM201.var:var
*s.ат:ed:Единица
*i.2:prec:Точность
SurgeCM201.out:out
SurgeCM201.manIn:manIn
SurgeCM201.max:max
SurgeCM201.min:min
SurgeCM201.auto:auto
SurgeCM201.Kp:Kp
SurgeCM201.Ti:Ti
SurgeCM201.Kd:Kd
SurgeCM201.Td:Td
SurgeCM201.Tzd:Tzd
SurgeCM201.Tf:Tf
SurgeCM201.Zi:Zi
SurgeCM201.Hup:Hup
SurgeCM201.Hdwn:Hdwn','SurgeCM201.sp:sp
SurgeCM201.var:var
*s.ат:ed:Одиниця
*i.2:prec:Точність
SurgeCM201.out:out
SurgeCM201.manIn:manIn
SurgeCM201.max:max
SurgeCM201.min:min
SurgeCM201.auto:auto
SurgeCM201.Kp:Kp
SurgeCM201.Ti:Ti
SurgeCM201.Kd:Kd
SurgeCM201.Td:Td
SurgeCM201.Tzd:Tzd
SurgeCM201.Tf:Tf
SurgeCM201.Zi:Zi
SurgeCM201.Hup:Hup
SurgeCM201.Hdwn:Hdwn');
INSERT INTO "gen_cntr_prm" VALUES('SurgeCM301','SurgeCM301','Regulator of the surge protection of the compressor CM301',1,'SurgeCM301.sp:sp
SurgeCM301.var:var
*s.at:ed:Dimension
*i.2:prec:Precision
SurgeCM301.out:out
SurgeCM301.manIn:manIn
SurgeCM301.max:max
SurgeCM301.min:min
SurgeCM301.auto:auto
SurgeCM301.Kp:Kp
SurgeCM301.Ti:Ti
SurgeCM301.Kd:Kd
SurgeCM301.Td:Td
SurgeCM301.Tzd:Tzd
SurgeCM301.Tf:Tf
SurgeCM301.Zi:Zi
SurgeCM301.Hup:Hup
SurgeCM301.Hdwn:Hdwn','ПомпажКМ301','Регулятор защиты от помпажа компресора КМ301','ПомпажКМ301','Регулятор захисту від помпажу компресору КМ301','SurgeCM301.sp:sp
SurgeCM301.var:var
*s.ат:ed:Единица
*i.2:prec:Точность
SurgeCM301.out:out
SurgeCM301.manIn:manIn
SurgeCM301.max:max
SurgeCM301.min:min
SurgeCM301.auto:auto
SurgeCM301.Kp:Kp
SurgeCM301.Ti:Ti
SurgeCM301.Kd:Kd
SurgeCM301.Td:Td
SurgeCM301.Tzd:Tzd
SurgeCM301.Tf:Tf
SurgeCM301.Zi:Zi
SurgeCM301.Hup:Hup
SurgeCM301.Hdwn:Hdwn','SurgeCM301.sp:sp
SurgeCM301.var:var
*s.ат:ed:Одиниця
*i.2:prec:Точність
SurgeCM301.out:out
SurgeCM301.manIn:manIn
SurgeCM301.max:max
SurgeCM301.min:min
SurgeCM301.auto:auto
SurgeCM301.Kp:Kp
SurgeCM301.Ti:Ti
SurgeCM301.Kd:Kd
SurgeCM301.Td:Td
SurgeCM301.Tzd:Tzd
SurgeCM301.Tf:Tf
SurgeCM301.Zi:Zi
SurgeCM301.Hup:Hup
SurgeCM301.Hdwn:Hdwn');
INSERT INTO "gen_cntr_prm" VALUES('SurgeCM102','SurgeCM102','Regulator of the surge protection of the compressor CM102',1,'SurgeCM102.sp:sp
SurgeCM102.var:var
*s.at:ed:Dimension
*i.2:prec:Precision
SurgeCM102.out:out
SurgeCM102.manIn:manIn
SurgeCM102.max:max
SurgeCM102.min:min
SurgeCM102.auto:auto
SurgeCM102.Kp:Kp
SurgeCM102.Ti:Ti
SurgeCM102.Kd:Kd
SurgeCM102.Td:Td
SurgeCM102.Tzd:Tzd
SurgeCM102.Tf:Tf
SurgeCM102.Zi:Zi
SurgeCM102.Hup:Hup
SurgeCM102.Hdwn:Hdwn','ПомпажКМ102','Регулятор защиты от помпажа компресора КМ102','ПомпажКМ102','Регулятор захисту від помпажу компресору КМ102','SurgeCM102.sp:sp
SurgeCM102.var:var
*s.ат:ed:Единица
*i.2:prec:Точность
SurgeCM102.out:out
SurgeCM102.manIn:manIn
SurgeCM102.max:max
SurgeCM102.min:min
SurgeCM102.auto:auto
SurgeCM102.Kp:Kp
SurgeCM102.Ti:Ti
SurgeCM102.Kd:Kd
SurgeCM102.Td:Td
SurgeCM102.Tzd:Tzd
SurgeCM102.Tf:Tf
SurgeCM102.Zi:Zi
SurgeCM102.Hup:Hup
SurgeCM102.Hdwn:Hdwn','SurgeCM102.sp:sp
SurgeCM102.var:var
*s.ат:ed:Одиниця
*i.2:prec:Точність
SurgeCM102.out:out
SurgeCM102.manIn:manIn
SurgeCM102.max:max
SurgeCM102.min:min
SurgeCM102.auto:auto
SurgeCM102.Kp:Kp
SurgeCM102.Ti:Ti
SurgeCM102.Kd:Kd
SurgeCM102.Td:Td
SurgeCM102.Tzd:Tzd
SurgeCM102.Tf:Tf
SurgeCM102.Zi:Zi
SurgeCM102.Hup:Hup
SurgeCM102.Hdwn:Hdwn');
INSERT INTO "gen_cntr_prm" VALUES('SurgeCM202','SurgeCM202','Regulator of the surge protection of the compressor CM202',1,'SurgeCM202.sp:sp
SurgeCM202.var:var
*s.at:ed:Dimension
*i.2:prec:Precision
SurgeCM202.out:out
SurgeCM202.manIn:manIn
SurgeCM202.max:max
SurgeCM202.min:min
SurgeCM202.auto:auto
SurgeCM202.Kp:Kp
SurgeCM202.Ti:Ti
SurgeCM202.Kd:Kd
SurgeCM202.Td:Td
SurgeCM202.Tzd:Tzd
SurgeCM202.Tf:Tf
SurgeCM202.Zi:Zi
SurgeCM202.Hup:Hup
SurgeCM202.Hdwn:Hdwn','ПомпажКМ202','Регулятор защиты от помпажа компресора КМ202','ПомпажКМ202','Регулятор захисту від помпажу компресору КМ202','SurgeCM202.sp:sp
SurgeCM202.var:var
*s.ат:ed:Единица
*i.2:prec:Точность
SurgeCM202.out:out
SurgeCM202.manIn:manIn
SurgeCM202.max:max
SurgeCM202.min:min
SurgeCM202.auto:auto
SurgeCM202.Kp:Kp
SurgeCM202.Ti:Ti
SurgeCM202.Kd:Kd
SurgeCM202.Td:Td
SurgeCM202.Tzd:Tzd
SurgeCM202.Tf:Tf
SurgeCM202.Zi:Zi
SurgeCM202.Hup:Hup
SurgeCM202.Hdwn:Hdwn','SurgeCM202.sp:sp
SurgeCM202.var:var
*s.ат:ed:Одиниця
*i.2:prec:Точність
SurgeCM202.out:out
SurgeCM202.manIn:manIn
SurgeCM202.max:max
SurgeCM202.min:min
SurgeCM202.auto:auto
SurgeCM202.Kp:Kp
SurgeCM202.Ti:Ti
SurgeCM202.Kd:Kd
SurgeCM202.Td:Td
SurgeCM202.Tzd:Tzd
SurgeCM202.Tf:Tf
SurgeCM202.Zi:Zi
SurgeCM202.Hup:Hup
SurgeCM202.Hdwn:Hdwn');
INSERT INTO "gen_cntr_prm" VALUES('SurgeCM302','SurgeCM302','Regulator of the surge protection of the compressor CM302',1,'SurgeCM302.sp:sp
SurgeCM302.var:var
*s.at:ed:Dimension
*i.2:prec:Precision
SurgeCM302.out:out
SurgeCM302.manIn:manIn
SurgeCM302.max:max
SurgeCM302.min:min
SurgeCM302.auto:auto
SurgeCM302.Kp:Kp
SurgeCM302.Ti:Ti
SurgeCM302.Kd:Kd
SurgeCM302.Td:Td
SurgeCM302.Tzd:Tzd
SurgeCM302.Tf:Tf
SurgeCM302.Zi:Zi
SurgeCM302.Hup:Hup
SurgeCM302.Hdwn:Hdwn','ПомпажКМ302','Регулятор защиты от помпажа компресора КМ302','ПомпажКМ302','Регулятор захисту від помпажу компресору КМ302','SurgeCM302.sp:sp
SurgeCM302.var:var
*s.ат:ed:Единица
*i.2:prec:Точность
SurgeCM302.out:out
SurgeCM302.manIn:manIn
SurgeCM302.max:max
SurgeCM302.min:min
SurgeCM302.auto:auto
SurgeCM302.Kp:Kp
SurgeCM302.Ti:Ti
SurgeCM302.Kd:Kd
SurgeCM302.Td:Td
SurgeCM302.Tzd:Tzd
SurgeCM302.Tf:Tf
SurgeCM302.Zi:Zi
SurgeCM302.Hup:Hup
SurgeCM302.Hdwn:Hdwn','SurgeCM302.sp:sp
SurgeCM302.var:var
*s.ат:ed:Одиниця
*i.2:prec:Точність
SurgeCM302.out:out
SurgeCM302.manIn:manIn
SurgeCM302.max:max
SurgeCM302.min:min
SurgeCM302.auto:auto
SurgeCM302.Kp:Kp
SurgeCM302.Ti:Ti
SurgeCM302.Kd:Kd
SurgeCM302.Td:Td
SurgeCM302.Tzd:Tzd
SurgeCM302.Tf:Tf
SurgeCM302.Zi:Zi
SurgeCM302.Hup:Hup
SurgeCM302.Hdwn:Hdwn');
CREATE TABLE 'gen_prm' ("SHIFR" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '' ,"IO" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"ru#IO" TEXT DEFAULT '' ,"uk#IO" TEXT DEFAULT '' , PRIMARY KEY ("SHIFR"));
INSERT INTO "gen_prm" VALUES('F2','F2','Gas flow through the diaphragm PP1',1,'PP1.Fi:var:Variable','','Витрати газу через діафрагму PP1','','Расход газа через диафрагму PP1','PP1.Fi:var:Переменная','PP1.Fi:var:Змінна');
INSERT INTO "gen_prm" VALUES('F3','F3','Gas flow through the pipe to Glinsk',1,'pipeGlinsk.Fi:var:Variable','','Витрати газу через трубу на Глінск','','Расход газа через трубу на Глинск','pipeGlinsk.Fi:var:Переменная','pipeGlinsk.Fi:var:Змінна');
INSERT INTO "gen_prm" VALUES('F4','F4','Gas flow through the diaphragm PP5',1,'PP5.Fi:var:Variable','','Витрати газу через діафрагму PP5','','Расход газа через диафрагму PP5','PP5.Fi:var:Переменная','PP5.Fi:var:Змінна');
INSERT INTO "gen_prm" VALUES('BC21','BC21','Control block of the ball crane BC21',1,'G_BC21.com:com
G_BC21.st_open:st_open
G_BC21.st_close:st_close','КК21','Блок керування кульовим краном КК21','КШ21','Блок управления шарового крана КШ21','','');
INSERT INTO "gen_prm" VALUES('BC22','BC22','Control block of the ball crane BC22',1,'G_BC22.com:com
G_BC22.st_open:st_open
G_BC22.st_close:st_close','КК22','Блок керування кульовим краном КК22','КШ22','Блок управления шарового крана КШ22','','');
INSERT INTO "gen_prm" VALUES('PT0804','PT0804','Gas pressure in the pipe to Glinsk',1,'pipeGlinsk.Pi:var:Variable','','Тиск газу у трубі на Глінск','','Давление в трубе на Глинск','pipeGlinsk.Pi:var:Переменная','pipeGlinsk.Pi:var:Змінна');
INSERT INTO "gen_prm" VALUES('F7_8','F7_8','Gas flow through the diaphragm PP7',1,'PP7.Fi:var:Variable','','Витрати газу через діафрагму PP7','','Расход газа через диафрагму PP7','PP7.Fi:var:Переменная','PP7.Fi:var:Змінна');
INSERT INTO "gen_prm" VALUES('L1','L1','Liquid level in the separator C2',1,'C2.Lж:var:Variable','L1','Рівень рідини у сепараторі C2','L1','Уровень жидкости в сепараторе C2','C2.Lж:var:Переменная','C2.Lж:var:Змінна');
INSERT INTO "gen_prm" VALUES('T_PP1','T_PP1','Gas temperature at the diaphragm PP1',1,'PCV1.To:var:Variable','','Температура газу на діафрагмі PP1','','Температура газа на диафрагме PP1','PCV1.To:var:Переменная','PCV1.To:var:Змінна');
INSERT INTO "gen_prm" VALUES('T_PP3','T_PP3','Gas temperature at the diaphragm PP3',1,'node19.To:var:Variable','','Температура газу на діафрагмі PP3','','Температура газа на диафрагме PP3','node19.To:var:Переменная','node19.To:var:Змінна');
INSERT INTO "gen_prm" VALUES('T_PP5','T_PP5','Gas temperature at the diaphragm PP5',1,'BC21.To:var:Variable','','Температура газу на діафрагмі PP5','','Температура газа на диафрагме PP5','BC21.To:var:Переменная','BC21.To:var:Змінна');
INSERT INTO "gen_prm" VALUES('P_PP1','P_PP1','Gas pressure at the diaphragm PP1',1,'PP1.Po:var:Variable','','Тиск газу на діафрагмі PP1','','Давление газа на диафрагме PP1','PP1.Po:var:Переменная','PP1.Po:var:Змінна');
INSERT INTO "gen_prm" VALUES('P_PP3','P_PP3','Gas pressure at the diaphragm PP3',1,'PP3.Po:var:Variable','','Тиск газу на діафрагмі PP3','','Давление газа на диафрагме PP3','PP3.Po:var:Переменная','PP3.Po:var:Змінна');
INSERT INTO "gen_prm" VALUES('P_PP5','P_PP5','Gas pressure at the diaphragm PP5',1,'PP5.Po:var:Variable','','Тиск газу на діафрагмі PP5','','Давление газа на диафрагме PP5','PP5.Po:var:Переменная','PP5.Po:var:Змінна');
INSERT INTO "gen_prm" VALUES('P3','P3','Gas pressure in the pipe to Glinsk',1,'pipeGlinsk.Pi:var:Variable','','Тиск газу у трубі на Глінск','','Давление в трубе на Глинск','pipeGlinsk.Pi:var:Переменная','pipeGlinsk.Pi:var:Змінна');
INSERT INTO "gen_prm" VALUES('P4','P4','Gas pressure at the diaphragm PP5',1,'PP5.Po:var:Variable','','Тиск газу на діафрагмі PP5','','Давление газа на диафрагме PP5','PP5.Po:var:Переменная','PP5.Po:var:Змінна');
INSERT INTO "gen_prm" VALUES('PT1606','PT1606','Gas pressure in the separator С1',1,'C1.Po:var:Variable','','Тиск газу у сепараторі С1','','Давление газа в сепараторе С1','C1.Po:var:Переменная','C1.Po:var:Змінна');
INSERT INTO "gen_prm" VALUES('Pi','Pi','Gas pressure at the input of the CS',1,'BC1.Pi:var:Variable','','Тиск газу на вході у КС','','Давление газа на входе КС','BC1.Pi:var:Переменная','BC1.Pi:var:Змінна');
INSERT INTO "gen_prm" VALUES('PT0404','PT0404','Gas pressure at the diaphragm PP5',1,'PP5.Pi:var:Variable','','Тиск газу на діафрагмі PP5','','Давление газа на диафрагме PP5','PP5.Pi:var:Переменная','PP5.Pi:var:Змінна');
INSERT INTO "gen_prm" VALUES('PT0406','PT0406','Gas pressure at the diaphragm PP7',1,'PP7.Po:var:Variable','','Тиск газу на діафрагмі PP7','','Давление газа на диафрагме PP7','PP7.Po:var:Переменная','PP7.Po:var:Змінна');
INSERT INTO "gen_prm" VALUES('Ti','Ti','Gas temperature at the input of the CS',1,'BC1.Ti:var:Variable','','Температура газу на вході КС','','Температура газа на входе КС','BC1.Ti:var:Переменная','BC1.Ti:var:Змінна');
INSERT INTO "gen_prm" VALUES('PT0503','PT0503','Gas pressure in the separator С1',1,'C1.Pi:var:Variable','','Тиск газу у сепараторі С1','','Давление газа в сепараторе С1','C1.Pi:var:Переменная','C1.Pi:var:Змінна');
INSERT INTO "gen_prm" VALUES('BC6','BC6','Control block of the ball crane BC6',1,'G_BC6.com:com
G_BC6.st_open:st_open
G_BC6.st_close:st_close','КК6','Блок керування кульовим краном КК6','КШ6','Блок управления шарового крана КШ6','','');
INSERT INTO "gen_prm" VALUES('BC7','BC7','',1,'G_BC7.com:com
G_BC7.st_open:st_open
G_BC7.st_close:st_close','КК7','Блок керування шарового крану КК7','КШ7','Блок управления шарового крана КШ7','','');
INSERT INTO "gen_prm" VALUES('PC0601','PC0601','Gas pressure at the diaphragm PP3',1,'PP3.Pi:var:Variable','','Тиск газу на діафрагмі PP3','','Давление газа на диафрагме PP3','PP3.Pi:var:Переменная','PP3.Pi:var:Змінна');
INSERT INTO "gen_prm" VALUES('F_PP1','F_PP1','Gas flow through the diaphragm PP1',1,'PP1.Fo:var:Variable','','Витрати газу через діафрагму PP1','','Расход газа через диафрагму PP1','PP1.Fo:var:Переменная','PP1.Fo:var:Змінна');
INSERT INTO "gen_prm" VALUES('F_PP3','F_PP3','Gas flow through the diaphragm PP3',1,'PP3.Fo:var:Variable','','Витрати газу через діафрагму PP3','','Расход газа через диафрагму PP3','PP3.Fo:var:Переменная','PP3.Fo:var:Змінна');
INSERT INTO "gen_prm" VALUES('F_PP5','F_PP5','Gas flow through the diaphragm PP5',1,'PP5.Fo:var:Variable','','Витрати газу через діафрагму PP5','','Расход газа через диафрагму PP5','PP5.Fo:var:Переменная','PP5.Fo:var:Змінна');
INSERT INTO "gen_prm" VALUES('F5_6','F5_6','Gas flow through the diaphragm PP5',1,'PP5.Fi:var:Variable','','Витрати газу через діафрагму PP5','','Расход газа через диафрагму PP5','PP5.Fi:var:Переменная','PP5.Fi:var:Змінна');
INSERT INTO "gen_prm" VALUES('PT0605','PT0605','Gas pressure before the control valve PCV1',1,'PCV1.Pi:var:Variable','','Тиск газу перед регулюючим клапаном КРТ1','','Давление газа перед регулирующим клапаном КРД1','PCV1.Pi:var:Переменная','PCV1.Pi:var:Змінна');
INSERT INTO "gen_prm" VALUES('BC1','BC1','Control block of the ball crane BC1',1,'G_BC1.com:com
G_BC1.st_open:st_open
G_BC1.st_close:st_close','КК1','Блок керування кульовим краном КК1','КШ1','Блок управления шарового крана КШ1','','');
INSERT INTO "gen_prm" VALUES('BC2','BC2','Control block of the ball crane BC2',1,'G_BC2.com:com
G_BC2.st_open:st_open
G_BC2.st_close:st_close','КК2','Блок керування кульовим краном КК2','КШ2','Блок управления шарового крана КШ2','','');
INSERT INTO "gen_prm" VALUES('BC5','BC5','Control block of the ball crane BC5',1,'G_BC5.com:com
G_BC5.st_open:st_open
G_BC5.st_close:st_close','КК5','Блок керування кульовим краном КК5','КШ5','Блок управления шарового крана КШ5','','');
CREATE TABLE 'CM101_blocks_io' ("BLK_ID" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"TLNK" INTEGER DEFAULT '' ,"LNK" TEXT DEFAULT '' ,"VAL" TEXT DEFAULT '' , PRIMARY KEY ("BLK_ID","ID"));
INSERT INTO "CM101_blocks_io" VALUES('CM101_1','Fi',0,'','50.4372843996325');
INSERT INTO "CM101_blocks_io" VALUES('CM101_1','Pi',1,'PP101.Po','5.39650970732886');
INSERT INTO "CM101_blocks_io" VALUES('CM101_1','Ti',0,'','273');
INSERT INTO "CM101_blocks_io" VALUES('CM101_1','Fo',1,'AT101_1.Fi','50.4422464399217');
INSERT INTO "CM101_blocks_io" VALUES('CM101_1','Po',0,'','10.2758263247369');
INSERT INTO "CM101_blocks_io" VALUES('CM101_1','To',0,'','331.141751228544');
INSERT INTO "CM101_blocks_io" VALUES('CM101_1','So',0,'','0.071');
INSERT INTO "CM101_blocks_io" VALUES('CM101_1','lo',0,'','55');
INSERT INTO "CM101_blocks_io" VALUES('CM101_1','Kzp',0,'','0.683671258405473');
INSERT INTO "CM101_blocks_io" VALUES('CM101_1','N',0,'','8.2');
INSERT INTO "CM101_blocks_io" VALUES('CM101_1','V',0,'','1.5');
INSERT INTO "CM101_blocks_io" VALUES('CM101_1','Kpmp',0,'','0.025');
INSERT INTO "CM101_blocks_io" VALUES('CM101_1','Kslp',0,'','0.2');
INSERT INTO "CM101_blocks_io" VALUES('CM101_1','Q0',0,'','1');
INSERT INTO "CM101_blocks_io" VALUES('CM101_1','Ct',0,'','100');
INSERT INTO "CM101_blocks_io" VALUES('CM101_1','Riz',0,'','100');
INSERT INTO "CM101_blocks_io" VALUES('CM101_1','Fwind',0,'','1');
INSERT INTO "CM101_blocks_io" VALUES('CM101_1','Twind',0,'','273');
INSERT INTO "CM101_blocks_io" VALUES('CM101_1','f_frq',0,'','100');
INSERT INTO "CM101_blocks_io" VALUES('CM101_1','Pot',0,'','10.8436371312579');
INSERT INTO "CM101_blocks_io" VALUES('CM101_1','Fit',0,'','50.4387024439933');
INSERT INTO "CM101_blocks_io" VALUES('CM101_2','Fi',0,'','49.9429570754183');
INSERT INTO "CM101_blocks_io" VALUES('CM101_2','Pi',1,'PP102.Po','9.85211618294938');
INSERT INTO "CM101_blocks_io" VALUES('CM101_2','Ti',1,'AT101_1.To','298.286907702808');
INSERT INTO "CM101_blocks_io" VALUES('CM101_2','Fo',1,'AT101_2.Fi','49.9485234563637');
INSERT INTO "CM101_blocks_io" VALUES('CM101_2','Po',0,'','25.9164808674262');
INSERT INTO "CM101_blocks_io" VALUES('CM101_2','To',0,'','398.638141414934');
INSERT INTO "CM101_blocks_io" VALUES('CM101_2','So',0,'','0.0314');
INSERT INTO "CM101_blocks_io" VALUES('CM101_2','lo',0,'','55');
INSERT INTO "CM101_blocks_io" VALUES('CM101_2','Kzp',0,'','0.88452154508133');
INSERT INTO "CM101_blocks_io" VALUES('CM101_2','N',1,'CM101_1.N','8.2');
INSERT INTO "CM101_blocks_io" VALUES('CM101_2','V',0,'','1');
INSERT INTO "CM101_blocks_io" VALUES('CM101_2','Kpmp',0,'','0.035');
INSERT INTO "CM101_blocks_io" VALUES('CM101_2','Kslp',0,'','0.2');
INSERT INTO "CM101_blocks_io" VALUES('CM101_2','Q0',0,'','1');
INSERT INTO "CM101_blocks_io" VALUES('CM101_2','Ct',0,'','100');
INSERT INTO "CM101_blocks_io" VALUES('CM101_2','Riz',0,'','100');
INSERT INTO "CM101_blocks_io" VALUES('CM101_2','Fwind',0,'','1');
INSERT INTO "CM101_blocks_io" VALUES('CM101_2','Twind',0,'','273');
INSERT INTO "CM101_blocks_io" VALUES('CM101_2','f_frq',0,'','100');
INSERT INTO "CM101_blocks_io" VALUES('CM101_2','Pot',0,'','27.7278237505709');
INSERT INTO "CM101_blocks_io" VALUES('CM101_2','Fit',0,'','49.9443518755762');
INSERT INTO "CM101_blocks_io" VALUES('KPP101','Fi',0,'','8.43732869344316e-265');
INSERT INTO "CM101_blocks_io" VALUES('KPP101','Pi',1,'node4_1.Po2','25.3958408425128');
INSERT INTO "CM101_blocks_io" VALUES('KPP101','Ti',1,'AT101_2.To','306.1116148757');
INSERT INTO "CM101_blocks_io" VALUES('KPP101','Fo',1,'node3_1.Fi2','0.000531331546652805');
INSERT INTO "CM101_blocks_io" VALUES('KPP101','Po',0,'','5.48467107999885');
INSERT INTO "CM101_blocks_io" VALUES('KPP101','To',0,'','273.326568051629');
INSERT INTO "CM101_blocks_io" VALUES('KPP101','So',0,'','0.071');
INSERT INTO "CM101_blocks_io" VALUES('KPP101','lo',0,'','10');
INSERT INTO "CM101_blocks_io" VALUES('KPP101','S_kl1',0,'','0.04');
INSERT INTO "CM101_blocks_io" VALUES('KPP101','S_kl2',0,'','0.001');
INSERT INTO "CM101_blocks_io" VALUES('KPP101','l_kl1',2,'gen_cntr.SurgeCM101.out','0');
INSERT INTO "CM101_blocks_io" VALUES('KPP101','l_kl2',0,'','0');
INSERT INTO "CM101_blocks_io" VALUES('KPP101','Q0',0,'','1');
INSERT INTO "CM101_blocks_io" VALUES('KPP101','Kln',0,'','2');
INSERT INTO "CM101_blocks_io" VALUES('KPP101','Kpr',0,'','0.95');
INSERT INTO "CM101_blocks_io" VALUES('KPP101','Ct',0,'','20');
INSERT INTO "CM101_blocks_io" VALUES('KPP101','Riz',0,'','20');
INSERT INTO "CM101_blocks_io" VALUES('KPP101','noBack',0,'','0');
INSERT INTO "CM101_blocks_io" VALUES('KPP101','Fwind',0,'','1');
INSERT INTO "CM101_blocks_io" VALUES('KPP101','Twind',0,'','273');
INSERT INTO "CM101_blocks_io" VALUES('KPP101','f_frq',0,'','100');
INSERT INTO "CM101_blocks_io" VALUES('KPP101','Pot',0,'','5.21403969646924');
INSERT INTO "CM101_blocks_io" VALUES('KPP101','Fit',0,'','2.47032822920623e-323');
INSERT INTO "CM101_blocks_io" VALUES('DR100','Fi',0,'','0.000138818627559277');
INSERT INTO "CM101_blocks_io" VALUES('DR100','Pi',1,'BC106.Po','5.48467355179591');
INSERT INTO "CM101_blocks_io" VALUES('DR100','Ti',1,'BC106.To','273');
INSERT INTO "CM101_blocks_io" VALUES('DR100','Fo',1,'node3_1.Fi1','0.00167737122624904');
INSERT INTO "CM101_blocks_io" VALUES('DR100','Po',0,'','5.48467146393197');
INSERT INTO "CM101_blocks_io" VALUES('DR100','To',0,'','272.999999974204');
INSERT INTO "CM101_blocks_io" VALUES('DR100','So',0,'','0.2');
INSERT INTO "CM101_blocks_io" VALUES('DR100','lo',0,'','10');
INSERT INTO "CM101_blocks_io" VALUES('DR100','S_kl1',0,'','0.05');
INSERT INTO "CM101_blocks_io" VALUES('DR100','S_kl2',0,'','0.01');
INSERT INTO "CM101_blocks_io" VALUES('DR100','l_kl1',0,'','50');
INSERT INTO "CM101_blocks_io" VALUES('DR100','l_kl2',0,'','0');
INSERT INTO "CM101_blocks_io" VALUES('DR100','Q0',0,'','1');
INSERT INTO "CM101_blocks_io" VALUES('DR100','Kln',0,'','1');
INSERT INTO "CM101_blocks_io" VALUES('DR100','Kpr',0,'','0.95');
INSERT INTO "CM101_blocks_io" VALUES('DR100','Ct',0,'','20');
INSERT INTO "CM101_blocks_io" VALUES('DR100','Riz',0,'','20');
INSERT INTO "CM101_blocks_io" VALUES('DR100','noBack',0,'','0');
INSERT INTO "CM101_blocks_io" VALUES('DR100','Fwind',0,'','1');
INSERT INTO "CM101_blocks_io" VALUES('DR100','Twind',0,'','273');
INSERT INTO "CM101_blocks_io" VALUES('DR100','f_frq',0,'','100');
INSERT INTO "CM101_blocks_io" VALUES('DR100','Pot',0,'','5.21393301112062');
INSERT INTO "CM101_blocks_io" VALUES('DR100','Fit',0,'','0.0442671479945538');
INSERT INTO "CM101_blocks_io" VALUES('AT101_1','Fi',0,'','50.4422643961388');
INSERT INTO "CM101_blocks_io" VALUES('AT101_1','Pi',1,'CM101_1.Po','10.2758335467077');
INSERT INTO "CM101_blocks_io" VALUES('AT101_1','Ti',1,'CM101_1.To','331.141743086806');
INSERT INTO "CM101_blocks_io" VALUES('AT101_1','Si',0,'','0.071');
INSERT INTO "CM101_blocks_io" VALUES('AT101_1','li',0,'','10');
INSERT INTO "CM101_blocks_io" VALUES('AT101_1','Fo',1,'C101_2.Fi','50.4424424365606');
INSERT INTO "CM101_blocks_io" VALUES('AT101_1','Po',0,'','10.145987877731');
INSERT INTO "CM101_blocks_io" VALUES('AT101_1','To',0,'','298.286909018159');
INSERT INTO "CM101_blocks_io" VALUES('AT101_1','So',0,'','0.071');
INSERT INTO "CM101_blocks_io" VALUES('AT101_1','lo',0,'','2');
INSERT INTO "CM101_blocks_io" VALUES('AT101_1','Tair',0,'','290');
INSERT INTO "CM101_blocks_io" VALUES('AT101_1','Wc',0,'','200');
INSERT INTO "CM101_blocks_io" VALUES('AT101_1','Q0',0,'','1');
INSERT INTO "CM101_blocks_io" VALUES('AT101_1','Ct',0,'','100');
INSERT INTO "CM101_blocks_io" VALUES('AT101_1','Rt',0,'','1');
INSERT INTO "CM101_blocks_io" VALUES('AT101_1','f_frq',0,'','100');
INSERT INTO "CM101_blocks_io" VALUES('AT101_1','Pot',0,'','10.5636608773328');
INSERT INTO "CM101_blocks_io" VALUES('AT101_1','Fit',0,'','37.839259204133');
INSERT INTO "CM101_blocks_io" VALUES('AT101_2','Fi',0,'','49.9485403336185');
INSERT INTO "CM101_blocks_io" VALUES('AT101_2','Pi',1,'CM101_2.Po','25.9164991882731');
INSERT INTO "CM101_blocks_io" VALUES('AT101_2','Ti',1,'CM101_2.To','398.638136236792');
INSERT INTO "CM101_blocks_io" VALUES('AT101_2','Si',0,'','0.0314');
INSERT INTO "CM101_blocks_io" VALUES('AT101_2','li',0,'','20');
INSERT INTO "CM101_blocks_io" VALUES('AT101_2','Fo',1,'C101_3.Fi','49.9490418774119');
INSERT INTO "CM101_blocks_io" VALUES('AT101_2','Po',0,'','25.657608099521');
INSERT INTO "CM101_blocks_io" VALUES('AT101_2','To',0,'','306.111622744077');
INSERT INTO "CM101_blocks_io" VALUES('AT101_2','So',0,'','0.0314');
INSERT INTO "CM101_blocks_io" VALUES('AT101_2','lo',0,'','5');
INSERT INTO "CM101_blocks_io" VALUES('AT101_2','Tair',0,'','283');
INSERT INTO "CM101_blocks_io" VALUES('AT101_2','Wc',0,'','200');
INSERT INTO "CM101_blocks_io" VALUES('AT101_2','Q0',0,'','1');
INSERT INTO "CM101_blocks_io" VALUES('AT101_2','Ct',0,'','100');
INSERT INTO "CM101_blocks_io" VALUES('AT101_2','Rt',0,'','1');
INSERT INTO "CM101_blocks_io" VALUES('AT101_2','f_frq',0,'','100');
INSERT INTO "CM101_blocks_io" VALUES('AT101_2','Pot',0,'','27.0712126806336');
INSERT INTO "CM101_blocks_io" VALUES('AT101_2','Fit',0,'','37.4306674619275');
INSERT INTO "CM101_blocks_io" VALUES('node3_1','Fi1',0,'','0.0016502825468722');
INSERT INTO "CM101_blocks_io" VALUES('node3_1','Pi1',1,'DR100.Po','5.48466934078161');
INSERT INTO "CM101_blocks_io" VALUES('node3_1','Ti1',1,'DR100.To','272.999999974204');
INSERT INTO "CM101_blocks_io" VALUES('node3_1','Si1',0,'','0.1');
INSERT INTO "CM101_blocks_io" VALUES('node3_1','Fi2',0,'','0.000555288506291828');
INSERT INTO "CM101_blocks_io" VALUES('node3_1','Pi2',1,'KPP101.Po','5.48467107999885');
INSERT INTO "CM101_blocks_io" VALUES('node3_1','Ti2',1,'KPP101.To','273.326568051629');
INSERT INTO "CM101_blocks_io" VALUES('node3_1','Si2',0,'','0.07');
INSERT INTO "CM101_blocks_io" VALUES('node3_1','Fi3',0,'','50.930777475668');
INSERT INTO "CM101_blocks_io" VALUES('node3_1','Pi3',1,'BC101.Po','5.51817840117129');
INSERT INTO "CM101_blocks_io" VALUES('node3_1','Ti3',1,'BC101.To','290.308843822459');
INSERT INTO "CM101_blocks_io" VALUES('node3_1','Si3',0,'','0.2');
INSERT INTO "CM101_blocks_io" VALUES('node3_1','Fo',1,'C101_1.Fi','50.9400224751236');
INSERT INTO "CM101_blocks_io" VALUES('node3_1','Po',0,'','5.48466762232167');
INSERT INTO "CM101_blocks_io" VALUES('node3_1','To',0,'','290.277638730498');
INSERT INTO "CM101_blocks_io" VALUES('node3_1','So',0,'','0.2');
INSERT INTO "CM101_blocks_io" VALUES('node3_1','lo',0,'','50');
INSERT INTO "CM101_blocks_io" VALUES('node3_1','Q0',0,'','1');
INSERT INTO "CM101_blocks_io" VALUES('node3_1','Ct',0,'','20');
INSERT INTO "CM101_blocks_io" VALUES('node3_1','Riz',0,'','20');
INSERT INTO "CM101_blocks_io" VALUES('node3_1','Fwind',0,'','1');
INSERT INTO "CM101_blocks_io" VALUES('node3_1','Twind',0,'','273');
INSERT INTO "CM101_blocks_io" VALUES('node3_1','f_frq',0,'','100');
INSERT INTO "CM101_blocks_io" VALUES('node3_1','FoR',0,'','44.7977730124046');
INSERT INTO "CM101_blocks_io" VALUES('node3_1','Pot',0,'','5.21387517550895');
INSERT INTO "CM101_blocks_io" VALUES('node3_1','Fit',0,'','38.1026117702689');
INSERT INTO "CM101_blocks_io" VALUES('BC101','Fi',0,'','50.9301084173167');
INSERT INTO "CM101_blocks_io" VALUES('BC101','Pi',0,'','5.54914000789552');
INSERT INTO "CM101_blocks_io" VALUES('BC101','Ti',2,'gen.PCV1.To','290.373952126398');
INSERT INTO "CM101_blocks_io" VALUES('BC101','Fo',1,'node3_1.Fi3','50.930846065489');
INSERT INTO "CM101_blocks_io" VALUES('BC101','Po',0,'','5.51818246161582');
INSERT INTO "CM101_blocks_io" VALUES('BC101','To',0,'','290.308850447735');
INSERT INTO "CM101_blocks_io" VALUES('BC101','So',0,'','0.2');
INSERT INTO "CM101_blocks_io" VALUES('BC101','lo',0,'','5');
INSERT INTO "CM101_blocks_io" VALUES('BC101','S_kl1',0,'','0.2');
INSERT INTO "CM101_blocks_io" VALUES('BC101','S_kl2',0,'','0.05');
INSERT INTO "CM101_blocks_io" VALUES('BC101','l_kl1',1,'G_BC101.pos','100');
INSERT INTO "CM101_blocks_io" VALUES('BC101','l_kl2',1,'G_BC102.pos','0');
INSERT INTO "CM101_blocks_io" VALUES('BC101','Q0',0,'','1');
INSERT INTO "CM101_blocks_io" VALUES('BC101','Kln',0,'','1');
INSERT INTO "CM101_blocks_io" VALUES('BC101','Kpr',0,'','0.95');
INSERT INTO "CM101_blocks_io" VALUES('BC101','Ct',0,'','20');
INSERT INTO "CM101_blocks_io" VALUES('BC101','Riz',0,'','20');
INSERT INTO "CM101_blocks_io" VALUES('BC101','noBack',0,'','0');
INSERT INTO "CM101_blocks_io" VALUES('BC101','Fwind',0,'','1');
INSERT INTO "CM101_blocks_io" VALUES('BC101','Twind',0,'','273');
INSERT INTO "CM101_blocks_io" VALUES('BC101','f_frq',0,'','100');
INSERT INTO "CM101_blocks_io" VALUES('BC101','Pot',0,'','5.27686195592518');
INSERT INTO "CM101_blocks_io" VALUES('BC101','Fit',0,'','38.1825907349745');
INSERT INTO "CM101_blocks_io" VALUES('BC104','Fi',0,'','49.4517846747133');
INSERT INTO "CM101_blocks_io" VALUES('BC104','Pi',1,'node4_1.Po3','25.1368530868792');
INSERT INTO "CM101_blocks_io" VALUES('BC104','Ti',1,'AT101_2.To','306.111618811043');
INSERT INTO "CM101_blocks_io" VALUES('BC104','Fo',0,'','49.4611387516121');
INSERT INTO "CM101_blocks_io" VALUES('BC104','Po',0,'','24.8400932626254');
INSERT INTO "CM101_blocks_io" VALUES('BC104','To',0,'','305.973243407765');
INSERT INTO "CM101_blocks_io" VALUES('BC104','So',0,'','0.03');
INSERT INTO "CM101_blocks_io" VALUES('BC104','lo',0,'','100');
INSERT INTO "CM101_blocks_io" VALUES('BC104','S_kl1',0,'','0.03');
INSERT INTO "CM101_blocks_io" VALUES('BC104','S_kl2',0,'','0.01');
INSERT INTO "CM101_blocks_io" VALUES('BC104','l_kl1',1,'G_BC104.pos','100');
INSERT INTO "CM101_blocks_io" VALUES('BC104','l_kl2',1,'G_BC105.pos','0');
INSERT INTO "CM101_blocks_io" VALUES('BC104','Q0',0,'','1');
INSERT INTO "CM101_blocks_io" VALUES('BC104','Kln',0,'','1');
INSERT INTO "CM101_blocks_io" VALUES('BC104','Kpr',0,'','0.95');
INSERT INTO "CM101_blocks_io" VALUES('BC104','Ct',0,'','20');
INSERT INTO "CM101_blocks_io" VALUES('BC104','Riz',0,'','20');
INSERT INTO "CM101_blocks_io" VALUES('BC104','noBack',0,'','1');
INSERT INTO "CM101_blocks_io" VALUES('BC104','Fwind',0,'','1');
INSERT INTO "CM101_blocks_io" VALUES('BC104','Twind',0,'','273');
INSERT INTO "CM101_blocks_io" VALUES('BC104','f_frq',0,'','100');
INSERT INTO "CM101_blocks_io" VALUES('BC104','Pot',0,'','25.5440963848017');
INSERT INTO "CM101_blocks_io" VALUES('BC104','Fit',0,'','37.0566743886216');
INSERT INTO "CM101_blocks_io" VALUES('node4_1','Fi',0,'','49.4502805842625');
INSERT INTO "CM101_blocks_io" VALUES('node4_1','Pi',1,'C101_3.Po','25.3957987970735');
INSERT INTO "CM101_blocks_io" VALUES('node4_1','Fo1',1,'BC106.Fi','0');
INSERT INTO "CM101_blocks_io" VALUES('node4_1','Po1',0,'','25.3957480796463');
INSERT INTO "CM101_blocks_io" VALUES('node4_1','So1',0,'','0.0314');
INSERT INTO "CM101_blocks_io" VALUES('node4_1','lo1',0,'','5');
INSERT INTO "CM101_blocks_io" VALUES('node4_1','Fo2',1,'KPP101.Fi','8.43732869344316e-265');
INSERT INTO "CM101_blocks_io" VALUES('node4_1','Po2',0,'','25.3958316015951');
INSERT INTO "CM101_blocks_io" VALUES('node4_1','So2',0,'','0.0314');
INSERT INTO "CM101_blocks_io" VALUES('node4_1','lo2',0,'','5');
INSERT INTO "CM101_blocks_io" VALUES('node4_1','Fo3',1,'BC104.Fi','49.4517683236023');
INSERT INTO "CM101_blocks_io" VALUES('node4_1','Po3',0,'','25.1368352362816');
INSERT INTO "CM101_blocks_io" VALUES('node4_1','So3',0,'','0.0314');
INSERT INTO "CM101_blocks_io" VALUES('node4_1','lo3',0,'','5');
INSERT INTO "CM101_blocks_io" VALUES('node4_1','Q0',0,'','1');
INSERT INTO "CM101_blocks_io" VALUES('node4_1','f_frq',0,'','100');
INSERT INTO "CM101_blocks_io" VALUES('node4_1','F1tmp',0,'','-0.00048423248263153');
INSERT INTO "CM101_blocks_io" VALUES('node4_1','F2tmp',0,'','-0.000510475137913421');
INSERT INTO "CM101_blocks_io" VALUES('node4_1','F3tmp',0,'','49.451275291883');
INSERT INTO "CM101_blocks_io" VALUES('node4_1','Pot1',0,'','26.2854977694532');
INSERT INTO "CM101_blocks_io" VALUES('node4_1','Pot2',0,'','26.6645745052463');
INSERT INTO "CM101_blocks_io" VALUES('node4_1','Pot3',0,'','26.1026891687553');
INSERT INTO "CM101_blocks_io" VALUES('BC106','Fi',0,'','0');
INSERT INTO "CM101_blocks_io" VALUES('BC106','Pi',1,'node4_1.Po1','25.3957884627587');
INSERT INTO "CM101_blocks_io" VALUES('BC106','Ti',1,'AT101_2.To','306.111618811043');
INSERT INTO "CM101_blocks_io" VALUES('BC106','Fo',1,'DR100.Fi','0.00015029431510567');
INSERT INTO "CM101_blocks_io" VALUES('BC106','Po',0,'','5.48467355179591');
INSERT INTO "CM101_blocks_io" VALUES('BC106','To',0,'','273');
INSERT INTO "CM101_blocks_io" VALUES('BC106','So',0,'','0.0314');
INSERT INTO "CM101_blocks_io" VALUES('BC106','lo',0,'','10');
INSERT INTO "CM101_blocks_io" VALUES('BC106','S_kl1',0,'','0.0314');
INSERT INTO "CM101_blocks_io" VALUES('BC106','S_kl2',0,'','0.01');
INSERT INTO "CM101_blocks_io" VALUES('BC106','l_kl1',1,'G_BC106.pos','0');
INSERT INTO "CM101_blocks_io" VALUES('BC106','l_kl2',0,'','0');
INSERT INTO "CM101_blocks_io" VALUES('BC106','Q0',0,'','1');
INSERT INTO "CM101_blocks_io" VALUES('BC106','Kln',0,'','1');
INSERT INTO "CM101_blocks_io" VALUES('BC106','Kpr',0,'','0.95');
INSERT INTO "CM101_blocks_io" VALUES('BC106','Ct',0,'','20');
INSERT INTO "CM101_blocks_io" VALUES('BC106','Riz',0,'','20');
INSERT INTO "CM101_blocks_io" VALUES('BC106','noBack',0,'','0');
INSERT INTO "CM101_blocks_io" VALUES('BC106','Fwind',0,'','1');
INSERT INTO "CM101_blocks_io" VALUES('BC106','Twind',0,'','273');
INSERT INTO "CM101_blocks_io" VALUES('BC106','f_frq',0,'','100');
INSERT INTO "CM101_blocks_io" VALUES('BC106','Pot',0,'','5.21397436923412');
INSERT INTO "CM101_blocks_io" VALUES('BC106','Fit',0,'','0');
INSERT INTO "CM101_blocks_io" VALUES('C101_1','Fi',0,'','50.9400439546549');
INSERT INTO "CM101_blocks_io" VALUES('C101_1','Pi',1,'node3_1.Po','5.48467162572274');
INSERT INTO "CM101_blocks_io" VALUES('C101_1','Si',0,'','0.2');
INSERT INTO "CM101_blocks_io" VALUES('C101_1','Fo',1,'PP101.Fi','50.4363222334396');
INSERT INTO "CM101_blocks_io" VALUES('C101_1','Po',0,'','5.45147186925162');
INSERT INTO "CM101_blocks_io" VALUES('C101_1','So',0,'','0.2');
INSERT INTO "CM101_blocks_io" VALUES('C101_1','lo',0,'','40');
INSERT INTO "CM101_blocks_io" VALUES('C101_1','Fo_ж',0,'','0');
INSERT INTO "CM101_blocks_io" VALUES('C101_1','Po_ж',0,'','5.5118335133369');
INSERT INTO "CM101_blocks_io" VALUES('C101_1','Lж',0,'','6.35385727213562');
INSERT INTO "CM101_blocks_io" VALUES('C101_1','ProcЖ',0,'','0.01');
INSERT INTO "CM101_blocks_io" VALUES('C101_1','Vap',0,'','9.5');
INSERT INTO "CM101_blocks_io" VALUES('C101_1','Q0',0,'','1');
INSERT INTO "CM101_blocks_io" VALUES('C101_1','Qж',0,'','1000');
INSERT INTO "CM101_blocks_io" VALUES('C101_1','f_frq',0,'','100');
INSERT INTO "CM101_blocks_io" VALUES('C101_1','Pot',0,'','5.14948874023436');
INSERT INTO "CM101_blocks_io" VALUES('C101_1','Fit',0,'','37.7880580203515');
INSERT INTO "CM101_blocks_io" VALUES('C101_2','Fi',0,'','50.4424065921151');
INSERT INTO "CM101_blocks_io" VALUES('C101_2','Pi',1,'AT101_1.Po','10.1459843134584');
INSERT INTO "CM101_blocks_io" VALUES('C101_2','Si',0,'','0.071');
INSERT INTO "CM101_blocks_io" VALUES('C101_2','Fo',1,'PP102.Fi','49.9423691491571');
INSERT INTO "CM101_blocks_io" VALUES('C101_2','Po',0,'','10.0086568000144');
INSERT INTO "CM101_blocks_io" VALUES('C101_2','So',0,'','0.071');
INSERT INTO "CM101_blocks_io" VALUES('C101_2','lo',0,'','50');
INSERT INTO "CM101_blocks_io" VALUES('C101_2','Fo_ж',0,'','0');
INSERT INTO "CM101_blocks_io" VALUES('C101_2','Po_ж',0,'','10.0658436222925');
INSERT INTO "CM101_blocks_io" VALUES('C101_2','Lж',0,'','6.01966550295802');
INSERT INTO "CM101_blocks_io" VALUES('C101_2','ProcЖ',0,'','0.01');
INSERT INTO "CM101_blocks_io" VALUES('C101_2','Vap',0,'','9.5');
INSERT INTO "CM101_blocks_io" VALUES('C101_2','Q0',0,'','1');
INSERT INTO "CM101_blocks_io" VALUES('C101_2','Qж',0,'','1000');
INSERT INTO "CM101_blocks_io" VALUES('C101_2','f_frq',0,'','100');
INSERT INTO "CM101_blocks_io" VALUES('C101_2','Pot',0,'','10.3103302826744');
INSERT INTO "CM101_blocks_io" VALUES('C101_2','Fit',0,'','37.4280342569487');
INSERT INTO "CM101_blocks_io" VALUES('C101_3','Fi',0,'','49.9490082223222');
INSERT INTO "CM101_blocks_io" VALUES('C101_3','Pi',1,'AT101_2.Po','25.6575990204274');
INSERT INTO "CM101_blocks_io" VALUES('C101_3','Si',0,'','0.0314');
INSERT INTO "CM101_blocks_io" VALUES('C101_3','Fo',1,'node4_1.Fi','49.4503135551223');
INSERT INTO "CM101_blocks_io" VALUES('C101_3','Po',0,'','25.3958077910808');
INSERT INTO "CM101_blocks_io" VALUES('C101_3','So',0,'','0.0314');
INSERT INTO "CM101_blocks_io" VALUES('C101_3','lo',0,'','8');
INSERT INTO "CM101_blocks_io" VALUES('C101_3','Fo_ж',0,'','0');
INSERT INTO "CM101_blocks_io" VALUES('C101_3','Po_ж',0,'','25.4512515175788');
INSERT INTO "CM101_blocks_io" VALUES('C101_3','Lж',0,'','18.4812421659864');
INSERT INTO "CM101_blocks_io" VALUES('C101_3','ProcЖ',0,'','0.01');
INSERT INTO "CM101_blocks_io" VALUES('C101_3','Vap',0,'','3');
INSERT INTO "CM101_blocks_io" VALUES('C101_3','Q0',0,'','1');
INSERT INTO "CM101_blocks_io" VALUES('C101_3','Qж',0,'','1000');
INSERT INTO "CM101_blocks_io" VALUES('C101_3','f_frq',0,'','100');
INSERT INTO "CM101_blocks_io" VALUES('C101_3','Pot',0,'','26.5861090936371');
INSERT INTO "CM101_blocks_io" VALUES('C101_3','Fit',0,'','37.0601681024761');
INSERT INTO "CM101_blocks_io" VALUES('PP101','Fi',0,'','50.4362830671123');
INSERT INTO "CM101_blocks_io" VALUES('PP101','Pi',1,'C101_1.Po','5.45146985112193');
INSERT INTO "CM101_blocks_io" VALUES('PP101','Fo',1,'CM101_1.Fi','50.4372652081497');
INSERT INTO "CM101_blocks_io" VALUES('PP101','Po',0,'','5.39650572057838');
INSERT INTO "CM101_blocks_io" VALUES('PP101','dP',0,'','5.49662434653509');
INSERT INTO "CM101_blocks_io" VALUES('PP101','Sdf',0,'','0.15');
INSERT INTO "CM101_blocks_io" VALUES('PP101','So',0,'','0.2');
INSERT INTO "CM101_blocks_io" VALUES('PP101','lo',0,'','7');
INSERT INTO "CM101_blocks_io" VALUES('PP101','Q0',0,'','1');
INSERT INTO "CM101_blocks_io" VALUES('PP101','f_frq',0,'','100');
INSERT INTO "CM101_blocks_io" VALUES('PP101','Pot',0,'','5.03525664785889');
INSERT INTO "CM101_blocks_io" VALUES('PP101','Fit',0,'','37.8041274113249');
INSERT INTO "CM101_blocks_io" VALUES('PP102','Fi',0,'','49.9423349454998');
INSERT INTO "CM101_blocks_io" VALUES('PP102','Pi',1,'C101_2.Po','10.0086532874365');
INSERT INTO "CM101_blocks_io" VALUES('PP102','Fo',1,'CM101_2.Fi','49.9429400572722');
INSERT INTO "CM101_blocks_io" VALUES('PP102','Po',0,'','9.85210926276382');
INSERT INTO "CM101_blocks_io" VALUES('PP102','dP',0,'','15.654895314599');
INSERT INTO "CM101_blocks_io" VALUES('PP102','Sdf',0,'','0.065');
INSERT INTO "CM101_blocks_io" VALUES('PP102','So',0,'','0.071');
INSERT INTO "CM101_blocks_io" VALUES('PP102','lo',0,'','7');
INSERT INTO "CM101_blocks_io" VALUES('PP102','Q0',0,'','1');
INSERT INTO "CM101_blocks_io" VALUES('PP102','f_frq',0,'','100');
INSERT INTO "CM101_blocks_io" VALUES('PP102','Pot',0,'','10.0115141796379');
INSERT INTO "CM101_blocks_io" VALUES('PP102','Fit',0,'','37.429637618184');
INSERT INTO "CM101_blocks_io" VALUES('G_BC102','pos',0,'','0');
INSERT INTO "CM101_blocks_io" VALUES('G_BC104','com',0,'','1');
INSERT INTO "CM101_blocks_io" VALUES('G_BC104','pos',0,'','100');
INSERT INTO "CM101_blocks_io" VALUES('G_BC102','com',0,'','0');
INSERT INTO "CM101_blocks_io" VALUES('G_BC102','st_open',0,'','0');
INSERT INTO "CM101_blocks_io" VALUES('G_BC102','st_close',0,'','1');
INSERT INTO "CM101_blocks_io" VALUES('G_BC102','t_full',0,'','3');
INSERT INTO "CM101_blocks_io" VALUES('G_BC102','t_up',0,'','0.3');
INSERT INTO "CM101_blocks_io" VALUES('G_BC102','f_frq',0,'','100');
INSERT INTO "CM101_blocks_io" VALUES('G_BC102','tmp_up',0,'','0');
INSERT INTO "CM101_blocks_io" VALUES('G_BC102','lst_com',0,'','0');
INSERT INTO "CM101_blocks_io" VALUES('G_BC101','pos',0,'','100');
INSERT INTO "CM101_blocks_io" VALUES('G_BC101','com',0,'','1');
INSERT INTO "CM101_blocks_io" VALUES('G_BC101','st_open',0,'','1');
INSERT INTO "CM101_blocks_io" VALUES('G_BC101','st_close',0,'','0');
INSERT INTO "CM101_blocks_io" VALUES('G_BC101','t_full',0,'','5');
INSERT INTO "CM101_blocks_io" VALUES('G_BC101','t_up',0,'','0.5');
INSERT INTO "CM101_blocks_io" VALUES('G_BC101','f_frq',0,'','100');
INSERT INTO "CM101_blocks_io" VALUES('G_BC101','tmp_up',0,'','0');
INSERT INTO "CM101_blocks_io" VALUES('G_BC101','lst_com',0,'','1');
INSERT INTO "CM101_blocks_io" VALUES('G_BC104','st_open',0,'','1');
INSERT INTO "CM101_blocks_io" VALUES('G_BC104','st_close',0,'','0');
INSERT INTO "CM101_blocks_io" VALUES('G_BC104','t_full',0,'','5');
INSERT INTO "CM101_blocks_io" VALUES('G_BC104','t_up',0,'','0.5');
INSERT INTO "CM101_blocks_io" VALUES('G_BC104','f_frq',0,'','100');
INSERT INTO "CM101_blocks_io" VALUES('G_BC104','tmp_up',0,'','0');
INSERT INTO "CM101_blocks_io" VALUES('G_BC104','lst_com',0,'','1');
INSERT INTO "CM101_blocks_io" VALUES('G_BC105','pos',0,'','0');
INSERT INTO "CM101_blocks_io" VALUES('G_BC105','com',0,'','0');
INSERT INTO "CM101_blocks_io" VALUES('G_BC105','st_open',0,'','0');
INSERT INTO "CM101_blocks_io" VALUES('G_BC105','st_close',0,'','1');
INSERT INTO "CM101_blocks_io" VALUES('G_BC105','t_full',0,'','5');
INSERT INTO "CM101_blocks_io" VALUES('G_BC105','t_up',0,'','0.5');
INSERT INTO "CM101_blocks_io" VALUES('G_BC105','f_frq',0,'','100');
INSERT INTO "CM101_blocks_io" VALUES('G_BC105','tmp_up',0,'','0');
INSERT INTO "CM101_blocks_io" VALUES('G_BC105','lst_com',0,'','0');
INSERT INTO "CM101_blocks_io" VALUES('G_BC106','pos',0,'','0');
INSERT INTO "CM101_blocks_io" VALUES('G_BC106','com',0,'','0');
INSERT INTO "CM101_blocks_io" VALUES('G_BC106','st_open',0,'','0');
INSERT INTO "CM101_blocks_io" VALUES('G_BC106','st_close',0,'','1');
INSERT INTO "CM101_blocks_io" VALUES('G_BC106','t_full',0,'','5');
INSERT INTO "CM101_blocks_io" VALUES('G_BC106','t_up',0,'','0.5');
INSERT INTO "CM101_blocks_io" VALUES('G_BC106','f_frq',0,'','100');
INSERT INTO "CM101_blocks_io" VALUES('G_BC106','tmp_up',0,'','0');
INSERT INTO "CM101_blocks_io" VALUES('G_BC106','lst_com',0,'','0');
INSERT INTO "CM101_blocks_io" VALUES('CM101_1','Kpr',0,'','0.95');
INSERT INTO "CM101_blocks_io" VALUES('CM101_2','Kpr',0,'','0.95');
INSERT INTO "CM101_blocks_io" VALUES('KPP101','t_kl1',0,'','3');
INSERT INTO "CM101_blocks_io" VALUES('KPP101','t_kl2',0,'','5');
INSERT INTO "CM101_blocks_io" VALUES('KPP101','tmp_l1',0,'','7.4109846876187e-322');
INSERT INTO "CM101_blocks_io" VALUES('KPP101','tmp_l2',0,'','0');
INSERT INTO "CM101_blocks_io" VALUES('DR100','t_kl1',0,'','10');
INSERT INTO "CM101_blocks_io" VALUES('DR100','t_kl2',0,'','5');
INSERT INTO "CM101_blocks_io" VALUES('DR100','tmp_l1',0,'','49.9999999999964');
INSERT INTO "CM101_blocks_io" VALUES('DR100','tmp_l2',0,'','0');
INSERT INTO "CM101_blocks_io" VALUES('node3_1','Kpr',0,'','0.95');
INSERT INTO "CM101_blocks_io" VALUES('BC101','t_kl1',0,'','10');
INSERT INTO "CM101_blocks_io" VALUES('BC101','t_kl2',0,'','5');
INSERT INTO "CM101_blocks_io" VALUES('BC101','tmp_l1',0,'','99.9999999999929');
INSERT INTO "CM101_blocks_io" VALUES('BC101','tmp_l2',0,'','0');
INSERT INTO "CM101_blocks_io" VALUES('BC104','t_kl1',0,'','10');
INSERT INTO "CM101_blocks_io" VALUES('BC104','t_kl2',0,'','5');
INSERT INTO "CM101_blocks_io" VALUES('BC104','tmp_l1',0,'','99.9999999999929');
INSERT INTO "CM101_blocks_io" VALUES('BC104','tmp_l2',0,'','0');
INSERT INTO "CM101_blocks_io" VALUES('node4_1','Kpr',0,'','0.95');
INSERT INTO "CM101_blocks_io" VALUES('BC106','t_kl1',0,'','10');
INSERT INTO "CM101_blocks_io" VALUES('BC106','t_kl2',0,'','5');
INSERT INTO "CM101_blocks_io" VALUES('BC106','tmp_l1',0,'','0');
INSERT INTO "CM101_blocks_io" VALUES('BC106','tmp_l2',0,'','0');
INSERT INTO "CM101_blocks_io" VALUES('PP101','Kpr',0,'','0.95');
INSERT INTO "CM101_blocks_io" VALUES('PP102','Kpr',0,'','0.95');
CREATE TABLE 'CM101_prm' ("SHIFR" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '' ,"IO" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#IO" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#IO" TEXT DEFAULT '' , PRIMARY KEY ("SHIFR"));
INSERT INTO "CM101_prm" VALUES('FN101','FN101','Flow at the output of the compressor',1,'BC104.Fi:var:Variable','Витрати на виході компресора','Расход на выходе компрессора','','BC104.Fi:var:Переменная','','BC104.Fi:var:Змінна');
INSERT INTO "CM101_prm" VALUES('F101','F101','Flow on the diaphragm PP101',1,'PP101.Fi:var:Variable
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
INSERT INTO "CM101_prm" VALUES('F102','F102','Flow on the diaphragm PP102',1,'PP102.Fi:var:Variable
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
INSERT INTO "CM101_prm" VALUES('TE1313_1','TE1313_1','The temperature at the input of the first stage of the compressor',1,'node3_1.To:var:Variable
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
INSERT INTO "CM101_prm" VALUES('PT0202','PT0202_1','Pressure after the first stage of the compressor',1,'CM101_1.Po:var:Variable
*s.at:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.20:max:Maximum','Тиск після першого ступеня компресору','Давление после первой ступени компрессора','','CM101_1.Po:var:Переменная
*s.ат:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.20:max:Максимум','','CM101_1.Po:var:Змінна
*s.ат:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.20:max:Максимум');
INSERT INTO "CM101_prm" VALUES('PT0204','PT0204_1','Pressure after the second stage of the compressor',1,'CM101_2.Po:var:Variable
*s.at:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.50:max:Maximum','Тиск після другого ступеня компресору','Давление после второй ступени компрессора','','CM101_2.Po:var:Переменная
*s.ат:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.50:max:Максимум','','CM101_2.Po:var:Змінна
*s.ат:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.50:max:Максимум');
INSERT INTO "CM101_prm" VALUES('TE1314_1','TE1314_1','The temperature after the fridge of the first stage of the compressor',1,'AT101_1.To:var:Variable
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
INSERT INTO "CM101_prm" VALUES('PT1006_1','PT1006_1','Pressure on the diaphragm PP102',1,'PP102.Pi:var:Variable
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
INSERT INTO "CM101_prm" VALUES('ST8612','ST8612_1','Rotation speed of the compressor',1,'CM101_1.N:var:Variable
CM101_1.N:varIn:Variable input
*s.1000x rpm:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.10:max:Maximum','Оберти компресору','Обороты вращения компрессора','','CM101_1.N:var:Переменная
CM101_1.N:varIn:Ввод переменной
*s.1000x об.мин.:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.10:max:Максимум','','CM101_1.N:var:Змінна
CM101_1.N:varIn:Ввід змінної
*s.1000x об.хвил:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.10:max:Максимум');
INSERT INTO "CM101_prm" VALUES('KPP101','KPP101','Anti-surge control valve of the compressor',1,'KPP101.l_kl1:out:Output
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
INSERT INTO "CM101_prm" VALUES('P101','P101','Pressure on the diaphragm PP101',1,'PP101.Pi:var:Variable
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
INSERT INTO "CM101_prm" VALUES('TT0202','TE1202_1','Temperature after the first stage of the compressor',1,'CM101_1.To:var:Variable
*s.К:ed:Dimension
*i.0:prec:Precision
*r.273:min:Minimum
*r.373:max:Maximum','Температура після першого ступеня компресору','Температура после первой ступени компрессора','','CM101_1.To:var:Переменная
*s.К:ed:Ед.измерения
*i.0:prec:Точность
*r.273:min:Минимум
*r.373:max:Максимум','','CM101_1.To:var:Змінна
*s.К:ed:Од.виміру
*i.0:prec:Точність
*r.273:min:Мінімум
*r.373:max:Максимум');
INSERT INTO "CM101_prm" VALUES('TT0204','TE1205_1','Temperature after the second stage of the compressor',1,'CM101_2.To:var:Variable
*s.К:ed:Dimension
*i.0:prec:Precision
*r.273:min:Minimum
*r.433:max:Maximum','Температура після другого ступеня компресору','Температура после второй ступени компрессора','','CM101_2.To:var:Переменная
*s.К:ed:Ед.измерения
*i.0:prec:Точность
*r.273:min:Минимум
*r.433:max:Максимум','','CM101_2.To:var:Змінна
*s.К:ed:Од.виміру
*i.0:prec:Точність
*r.273:min:Мінімум
*r.433:max:Максимум');
INSERT INTO "CM101_prm" VALUES('TE1314_2','TE1206_1','The temperature after the fridge of the second stage of the compressor',1,'AT101_2.To:var:Variable
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
INSERT INTO "CM101_prm" VALUES('BC101','BC101','Control block of the ball crane BC101',1,'G_BC101.com:com
G_BC101.st_open:st_open
G_BC101.st_close:st_close','Блок керування кульовим краном КК101','Блок управления шарового крана КШ101','КШ101','','КК101','');
INSERT INTO "CM101_prm" VALUES('BC102','BC102','Control block of the ball crane BC102',1,'G_BC102.com:com
G_BC102.st_open:st_open
G_BC102.st_close:st_close','Блок керування кульовим краном КК102','Блок управления шарового крана КШ102','КШ102','','КК102','');
INSERT INTO "CM101_prm" VALUES('BC104','BC104','Control block of the ball crane BC104',1,'G_BC104.com:com
G_BC104.st_open:st_open
G_BC104.st_close:st_close','Блок керування кульовим краном КК104','Блок управления шарового крана КШ104','КШ104','','КК104','');
INSERT INTO "CM101_prm" VALUES('BC105','BC105','Control block of the ball crane BC105',1,'G_BC105.com:com
G_BC105.st_open:st_open
G_BC105.st_close:st_close','Блок керування кульовим краном КК105','Блок управления шарового крана КШ105','КШ105','','КК105','');
INSERT INTO "CM101_prm" VALUES('BC106','BC106','Control block of the ball crane BC106',1,'G_BC106.com:com
G_BC106.st_open:st_open
G_BC106.st_close:st_close','Блок керування кульовим краном КК106','Блок управления шарового крана КШ106','КШ106','','КК106','');
INSERT INTO "CM101_prm" VALUES('AT101_1','AT101_1','Control block of the fridge after the first stage of the compressor',1,'AT101_1.Ti:Ti
AT101_1.To:To
AT101_1.Wc:Wc','Блок контролю холодильника після першого ступеня компресору','Блок контроля холодильника после первой ступени компрессора','','','','');
INSERT INTO "CM101_prm" VALUES('AT101_2','AT101_2','Control block of the fridge after the second stage of the compressor',1,'AT101_2.Ti:Ti
AT101_2.To:To
AT101_2.Wc:Wc','Блок контролю холодильника після другого ступеня компресору','Блок контроля холодильника после второй ступени компрессора','','','','');
CREATE TABLE 'CM102_blocks_io' ("BLK_ID" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"TLNK" INTEGER DEFAULT '' ,"LNK" TEXT DEFAULT '' ,"VAL" TEXT DEFAULT '' , PRIMARY KEY ("BLK_ID","ID"));
INSERT INTO "CM102_blocks_io" VALUES('KPP102','Fi',0,'','-2.47032822920623e-321');
INSERT INTO "CM102_blocks_io" VALUES('KPP102','Pi',1,'node6_1.Po2','97.0451629609845');
INSERT INTO "CM102_blocks_io" VALUES('KPP102','Ti',1,'AT102_2.To','302.615186403009');
INSERT INTO "CM102_blocks_io" VALUES('KPP102','Fo',1,'node5_1.Fi2','0');
INSERT INTO "CM102_blocks_io" VALUES('KPP102','Po',0,'','97.0145409390343');
INSERT INTO "CM102_blocks_io" VALUES('KPP102','To',0,'','273.000000008736');
INSERT INTO "CM102_blocks_io" VALUES('KPP102','So',0,'','0.017');
INSERT INTO "CM102_blocks_io" VALUES('KPP102','lo',0,'','10');
INSERT INTO "CM102_blocks_io" VALUES('KPP102','S_kl1',0,'','0.015');
INSERT INTO "CM102_blocks_io" VALUES('KPP102','S_kl2',0,'','0.015');
INSERT INTO "CM102_blocks_io" VALUES('KPP102','l_kl1',2,'gen_cntr.SurgeCM102.out','0');
INSERT INTO "CM102_blocks_io" VALUES('KPP102','l_kl2',0,'','0');
INSERT INTO "CM102_blocks_io" VALUES('KPP102','Q0',0,'','1');
INSERT INTO "CM102_blocks_io" VALUES('KPP102','Kln',0,'','2');
INSERT INTO "CM102_blocks_io" VALUES('KPP102','Kpr',0,'','0.95');
INSERT INTO "CM102_blocks_io" VALUES('KPP102','Ct',0,'','20');
INSERT INTO "CM102_blocks_io" VALUES('KPP102','Riz',0,'','20');
INSERT INTO "CM102_blocks_io" VALUES('KPP102','noBack',0,'','0');
INSERT INTO "CM102_blocks_io" VALUES('KPP102','Fwind',0,'','1');
INSERT INTO "CM102_blocks_io" VALUES('KPP102','Twind',0,'','273');
INSERT INTO "CM102_blocks_io" VALUES('KPP102','f_frq',0,'','100');
INSERT INTO "CM102_blocks_io" VALUES('KPP102','Pot',0,'','100.418398526209');
INSERT INTO "CM102_blocks_io" VALUES('KPP102','Fit',0,'','-0.571661292842614');
INSERT INTO "CM102_blocks_io" VALUES('net102','Fi',0,'','6.60092149020224e-10');
INSERT INTO "CM102_blocks_io" VALUES('net102','Pi',1,'BC113.Po','1.00000001195009');
INSERT INTO "CM102_blocks_io" VALUES('net102','Po',0,'','1');
INSERT INTO "CM102_blocks_io" VALUES('net102','So',0,'','0.01');
INSERT INTO "CM102_blocks_io" VALUES('net102','Kpr',0,'','0.95');
INSERT INTO "CM102_blocks_io" VALUES('net102','Noise',0,'','1');
INSERT INTO "CM102_blocks_io" VALUES('net102','Q0',0,'','1');
INSERT INTO "CM102_blocks_io" VALUES('net102','f_frq',0,'','100');
INSERT INTO "CM102_blocks_io" VALUES('DR101','Fi',0,'','-4.76713240517689e-05');
INSERT INTO "CM102_blocks_io" VALUES('DR101','Pi',1,'BC116.Po','22.2861597598136');
INSERT INTO "CM102_blocks_io" VALUES('DR101','Ti',1,'BC116.To','273');
INSERT INTO "CM102_blocks_io" VALUES('DR101','Fo',1,'node5_1.Fi1','5.37535887320822e-05');
INSERT INTO "CM102_blocks_io" VALUES('DR101','Po',0,'','22.2861342435703');
INSERT INTO "CM102_blocks_io" VALUES('DR101','To',0,'','272.999999999466');
INSERT INTO "CM102_blocks_io" VALUES('DR101','So',0,'','0.0314');
INSERT INTO "CM102_blocks_io" VALUES('DR101','lo',0,'','3');
INSERT INTO "CM102_blocks_io" VALUES('DR101','S_kl1',0,'','0.00314');
INSERT INTO "CM102_blocks_io" VALUES('DR101','S_kl2',0,'','0.00314');
INSERT INTO "CM102_blocks_io" VALUES('DR101','l_kl1',0,'','50');
INSERT INTO "CM102_blocks_io" VALUES('DR101','l_kl2',0,'','0');
INSERT INTO "CM102_blocks_io" VALUES('DR101','Q0',0,'','1');
INSERT INTO "CM102_blocks_io" VALUES('DR101','Kln',0,'','1');
INSERT INTO "CM102_blocks_io" VALUES('DR101','Kpr',0,'','0.95');
INSERT INTO "CM102_blocks_io" VALUES('DR101','Ct',0,'','20');
INSERT INTO "CM102_blocks_io" VALUES('DR101','Riz',0,'','20');
INSERT INTO "CM102_blocks_io" VALUES('DR101','noBack',0,'','0');
INSERT INTO "CM102_blocks_io" VALUES('DR101','Fwind',0,'','1');
INSERT INTO "CM102_blocks_io" VALUES('DR101','Twind',0,'','273');
INSERT INTO "CM102_blocks_io" VALUES('DR101','f_frq',0,'','100');
INSERT INTO "CM102_blocks_io" VALUES('DR101','Pot',0,'','20.670186602699');
INSERT INTO "CM102_blocks_io" VALUES('DR101','Fit',0,'','0.144757022511773');
INSERT INTO "CM102_blocks_io" VALUES('С102_1','Fi',0,'','53.0465291311258');
INSERT INTO "CM102_blocks_io" VALUES('С102_1','Pi',1,'node5_1.Po','22.2861344993608');
INSERT INTO "CM102_blocks_io" VALUES('С102_1','Si',0,'','0.0314');
INSERT INTO "CM102_blocks_io" VALUES('С102_1','Fo',1,'PP103.Fi','52.5193961825569');
INSERT INTO "CM102_blocks_io" VALUES('С102_1','Po',0,'','21.9416193527301');
INSERT INTO "CM102_blocks_io" VALUES('С102_1','So',0,'','0.0314');
INSERT INTO "CM102_blocks_io" VALUES('С102_1','lo',0,'','40');
INSERT INTO "CM102_blocks_io" VALUES('С102_1','Fo_ж',0,'','0');
INSERT INTO "CM102_blocks_io" VALUES('С102_1','Po_ж',0,'','21.9924166981321');
INSERT INTO "CM102_blocks_io" VALUES('С102_1','Lж',0,'','21.1655605841649');
INSERT INTO "CM102_blocks_io" VALUES('С102_1','ProcЖ',0,'','0.01');
INSERT INTO "CM102_blocks_io" VALUES('С102_1','Vap',0,'','2.4');
INSERT INTO "CM102_blocks_io" VALUES('С102_1','Q0',0,'','1');
INSERT INTO "CM102_blocks_io" VALUES('С102_1','Qж',0,'','1000');
INSERT INTO "CM102_blocks_io" VALUES('С102_1','f_frq',0,'','100');
INSERT INTO "CM102_blocks_io" VALUES('С102_1','Pot',0,'','20.0205661668655');
INSERT INTO "CM102_blocks_io" VALUES('С102_1','Fit',0,'','38.0815151265775');
INSERT INTO "CM102_blocks_io" VALUES('AT102_1','Fi',0,'','52.5240716067325');
INSERT INTO "CM102_blocks_io" VALUES('AT102_1','Pi',1,'CM102_1.Po','46.0194849738579');
INSERT INTO "CM102_blocks_io" VALUES('AT102_1','Ti',1,'CM102_1.To','344.874214101416');
INSERT INTO "CM102_blocks_io" VALUES('AT102_1','Si',0,'','0.015');
INSERT INTO "CM102_blocks_io" VALUES('AT102_1','li',0,'','10');
INSERT INTO "CM102_blocks_io" VALUES('AT102_1','Fo',1,'C102_2.Fi','52.5242162863392');
INSERT INTO "CM102_blocks_io" VALUES('AT102_1','Po',0,'','45.3134057585001');
INSERT INTO "CM102_blocks_io" VALUES('AT102_1','To',0,'','295.870558070076');
INSERT INTO "CM102_blocks_io" VALUES('AT102_1','So',0,'','0.017');
INSERT INTO "CM102_blocks_io" VALUES('AT102_1','lo',0,'','2');
INSERT INTO "CM102_blocks_io" VALUES('AT102_1','Tair',0,'','283');
INSERT INTO "CM102_blocks_io" VALUES('AT102_1','Wc',0,'','200');
INSERT INTO "CM102_blocks_io" VALUES('AT102_1','Q0',0,'','1');
INSERT INTO "CM102_blocks_io" VALUES('AT102_1','Ct',0,'','100');
INSERT INTO "CM102_blocks_io" VALUES('AT102_1','Rt',0,'','1');
INSERT INTO "CM102_blocks_io" VALUES('AT102_1','f_frq',0,'','100');
INSERT INTO "CM102_blocks_io" VALUES('AT102_1','Pot',0,'','44.4700839903117');
INSERT INTO "CM102_blocks_io" VALUES('AT102_1','Fit',0,'','38.0819539655514');
INSERT INTO "CM102_blocks_io" VALUES('AT102_2','Fi',0,'','52.0038888906082');
INSERT INTO "CM102_blocks_io" VALUES('AT102_2','Pi',1,'CM102_2.Po','99.6983579482039');
INSERT INTO "CM102_blocks_io" VALUES('AT102_2','Ti',1,'CM102_2.To','378.034479298227');
INSERT INTO "CM102_blocks_io" VALUES('AT102_2','Si',0,'','0.007');
INSERT INTO "CM102_blocks_io" VALUES('AT102_2','li',0,'','20');
INSERT INTO "CM102_blocks_io" VALUES('AT102_2','Fo',1,'C102_3.Fi','52.0039606979139');
INSERT INTO "CM102_blocks_io" VALUES('AT102_2','Po',0,'','98.2303291307439');
INSERT INTO "CM102_blocks_io" VALUES('AT102_2','To',0,'','302.615197529408');
INSERT INTO "CM102_blocks_io" VALUES('AT102_2','So',0,'','0.00785');
INSERT INTO "CM102_blocks_io" VALUES('AT102_2','lo',0,'','5');
INSERT INTO "CM102_blocks_io" VALUES('AT102_2','Tair',0,'','283');
INSERT INTO "CM102_blocks_io" VALUES('AT102_2','Wc',0,'','200');
INSERT INTO "CM102_blocks_io" VALUES('AT102_2','Q0',0,'','1');
INSERT INTO "CM102_blocks_io" VALUES('AT102_2','Ct',0,'','100');
INSERT INTO "CM102_blocks_io" VALUES('AT102_2','Rt',0,'','1');
INSERT INTO "CM102_blocks_io" VALUES('AT102_2','f_frq',0,'','100');
INSERT INTO "CM102_blocks_io" VALUES('AT102_2','Pot',0,'','102.786678846804');
INSERT INTO "CM102_blocks_io" VALUES('AT102_2','Fit',0,'','37.0335349287507');
INSERT INTO "CM102_blocks_io" VALUES('BC111','Fi',0,'','53.0188744783174');
INSERT INTO "CM102_blocks_io" VALUES('BC111','Pi',0,'','22.9499868885875');
INSERT INTO "CM102_blocks_io" VALUES('BC111','Ti',2,'gen.BC21.To','305.049377512615');
INSERT INTO "CM102_blocks_io" VALUES('BC111','Fo',1,'node5_1.Fi3','53.0193144391517');
INSERT INTO "CM102_blocks_io" VALUES('BC111','Po',0,'','22.6206068217592');
INSERT INTO "CM102_blocks_io" VALUES('BC111','To',0,'','304.900957893137');
INSERT INTO "CM102_blocks_io" VALUES('BC111','So',0,'','0.0314');
INSERT INTO "CM102_blocks_io" VALUES('BC111','lo',0,'','5');
INSERT INTO "CM102_blocks_io" VALUES('BC111','S_kl1',0,'','0.0314');
INSERT INTO "CM102_blocks_io" VALUES('BC111','S_kl2',0,'','0.002');
INSERT INTO "CM102_blocks_io" VALUES('BC111','l_kl1',1,'G_BC111.pos','100');
INSERT INTO "CM102_blocks_io" VALUES('BC111','l_kl2',1,'G_BC112.pos','0');
INSERT INTO "CM102_blocks_io" VALUES('BC111','Q0',0,'','1');
INSERT INTO "CM102_blocks_io" VALUES('BC111','Kln',0,'','1');
INSERT INTO "CM102_blocks_io" VALUES('BC111','Kpr',0,'','0.95');
INSERT INTO "CM102_blocks_io" VALUES('BC111','Ct',0,'','20');
INSERT INTO "CM102_blocks_io" VALUES('BC111','Riz',0,'','20');
INSERT INTO "CM102_blocks_io" VALUES('BC111','noBack',0,'','0');
INSERT INTO "CM102_blocks_io" VALUES('BC111','Fwind',0,'','1');
INSERT INTO "CM102_blocks_io" VALUES('BC111','Twind',0,'','273');
INSERT INTO "CM102_blocks_io" VALUES('BC111','f_frq',0,'','100');
INSERT INTO "CM102_blocks_io" VALUES('BC111','Pot',0,'','21.3671095075236');
INSERT INTO "CM102_blocks_io" VALUES('BC111','Fit',0,'','38.4838680292946');
INSERT INTO "CM102_blocks_io" VALUES('BC113','Fi',0,'','0');
INSERT INTO "CM102_blocks_io" VALUES('BC113','Pi',1,'node6_1.Po4','97.045166685741');
INSERT INTO "CM102_blocks_io" VALUES('BC113','Ti',0,'','273');
INSERT INTO "CM102_blocks_io" VALUES('BC113','Fo',1,'net102.Fi','6.59472062725442e-07');
INSERT INTO "CM102_blocks_io" VALUES('BC113','Po',0,'','0.999999988049924');
INSERT INTO "CM102_blocks_io" VALUES('BC113','To',0,'','272.999999995966');
INSERT INTO "CM102_blocks_io" VALUES('BC113','So',0,'','0.00785');
INSERT INTO "CM102_blocks_io" VALUES('BC113','lo',0,'','10');
INSERT INTO "CM102_blocks_io" VALUES('BC113','S_kl1',0,'','0.00785');
INSERT INTO "CM102_blocks_io" VALUES('BC113','S_kl2',0,'','0.002');
INSERT INTO "CM102_blocks_io" VALUES('BC113','l_kl1',0,'','0');
INSERT INTO "CM102_blocks_io" VALUES('BC113','l_kl2',0,'','0');
INSERT INTO "CM102_blocks_io" VALUES('BC113','Q0',0,'','1');
INSERT INTO "CM102_blocks_io" VALUES('BC113','Kln',0,'','1');
INSERT INTO "CM102_blocks_io" VALUES('BC113','Kpr',0,'','0.95');
INSERT INTO "CM102_blocks_io" VALUES('BC113','Ct',0,'','20');
INSERT INTO "CM102_blocks_io" VALUES('BC113','Riz',0,'','20');
INSERT INTO "CM102_blocks_io" VALUES('BC113','noBack',0,'','0');
INSERT INTO "CM102_blocks_io" VALUES('BC113','Fwind',0,'','1');
INSERT INTO "CM102_blocks_io" VALUES('BC113','Twind',0,'','273');
INSERT INTO "CM102_blocks_io" VALUES('BC113','f_frq',0,'','100');
INSERT INTO "CM102_blocks_io" VALUES('BC113','Pot',0,'','0.999957173185118');
INSERT INTO "CM102_blocks_io" VALUES('BC113','Fit',0,'','0');
INSERT INTO "CM102_blocks_io" VALUES('BC114','Fi',0,'','51.484485622028');
INSERT INTO "CM102_blocks_io" VALUES('BC114','Pi',1,'node6_1.Po3','95.0330957077374');
INSERT INTO "CM102_blocks_io" VALUES('BC114','Ti',1,'AT102_2.To','302.615197529408');
INSERT INTO "CM102_blocks_io" VALUES('BC114','Fo',0,'','51.4850790623713');
INSERT INTO "CM102_blocks_io" VALUES('BC114','Po',0,'','93.8216489202126');
INSERT INTO "CM102_blocks_io" VALUES('BC114','To',0,'','302.486618546291');
INSERT INTO "CM102_blocks_io" VALUES('BC114','So',0,'','0.00785');
INSERT INTO "CM102_blocks_io" VALUES('BC114','lo',0,'','100');
INSERT INTO "CM102_blocks_io" VALUES('BC114','S_kl1',0,'','0.00785');
INSERT INTO "CM102_blocks_io" VALUES('BC114','S_kl2',0,'','0.002');
INSERT INTO "CM102_blocks_io" VALUES('BC114','l_kl1',1,'G_BC114.pos','100');
INSERT INTO "CM102_blocks_io" VALUES('BC114','l_kl2',1,'G_BC115.pos','0');
INSERT INTO "CM102_blocks_io" VALUES('BC114','Q0',0,'','1');
INSERT INTO "CM102_blocks_io" VALUES('BC114','Kln',0,'','1');
INSERT INTO "CM102_blocks_io" VALUES('BC114','Kpr',0,'','0.95');
INSERT INTO "CM102_blocks_io" VALUES('BC114','Ct',0,'','20');
INSERT INTO "CM102_blocks_io" VALUES('BC114','Riz',0,'','20');
INSERT INTO "CM102_blocks_io" VALUES('BC114','noBack',0,'','1');
INSERT INTO "CM102_blocks_io" VALUES('BC114','Fwind',0,'','1');
INSERT INTO "CM102_blocks_io" VALUES('BC114','Twind',0,'','273');
INSERT INTO "CM102_blocks_io" VALUES('BC114','f_frq',0,'','100');
INSERT INTO "CM102_blocks_io" VALUES('BC114','Pot',0,'','94.694514564321');
INSERT INTO "CM102_blocks_io" VALUES('BC114','Fit',0,'','37.5187167024292');
INSERT INTO "CM102_blocks_io" VALUES('BC116','Fi',0,'','0');
INSERT INTO "CM102_blocks_io" VALUES('BC116','Pi',1,'node6_1.Po1','97.0451630863232');
INSERT INTO "CM102_blocks_io" VALUES('BC116','Ti',1,'AT102_2.To','302.615197529408');
INSERT INTO "CM102_blocks_io" VALUES('BC116','Fo',1,'DR101.Fi','-3.07799746857668e-05');
INSERT INTO "CM102_blocks_io" VALUES('BC116','Po',0,'','22.286117361549');
INSERT INTO "CM102_blocks_io" VALUES('BC116','To',0,'','273');
INSERT INTO "CM102_blocks_io" VALUES('BC116','So',0,'','0.0078');
INSERT INTO "CM102_blocks_io" VALUES('BC116','lo',0,'','1');
INSERT INTO "CM102_blocks_io" VALUES('BC116','S_kl1',0,'','0.0078');
INSERT INTO "CM102_blocks_io" VALUES('BC116','S_kl2',0,'','0.0078');
INSERT INTO "CM102_blocks_io" VALUES('BC116','l_kl1',1,'G_BC116.pos','0');
INSERT INTO "CM102_blocks_io" VALUES('BC116','l_kl2',0,'','0');
INSERT INTO "CM102_blocks_io" VALUES('BC116','Q0',0,'','1');
INSERT INTO "CM102_blocks_io" VALUES('BC116','Kln',0,'','1');
INSERT INTO "CM102_blocks_io" VALUES('BC116','Kpr',0,'','0.95');
INSERT INTO "CM102_blocks_io" VALUES('BC116','Ct',0,'','20');
INSERT INTO "CM102_blocks_io" VALUES('BC116','Riz',0,'','20');
INSERT INTO "CM102_blocks_io" VALUES('BC116','noBack',0,'','0');
INSERT INTO "CM102_blocks_io" VALUES('BC116','Fwind',0,'','1');
INSERT INTO "CM102_blocks_io" VALUES('BC116','Twind',0,'','273');
INSERT INTO "CM102_blocks_io" VALUES('BC116','f_frq',0,'','100');
INSERT INTO "CM102_blocks_io" VALUES('BC116','Pot',0,'','20.6324366115474');
INSERT INTO "CM102_blocks_io" VALUES('BC116','Fit',0,'','0');
INSERT INTO "CM102_blocks_io" VALUES('node5_1','Fi1',0,'','2.22382372530575e-06');
INSERT INTO "CM102_blocks_io" VALUES('node5_1','Pi1',1,'DR101.Po','22.2861342435703');
INSERT INTO "CM102_blocks_io" VALUES('node5_1','Ti1',1,'DR101.To','272.999999999466');
INSERT INTO "CM102_blocks_io" VALUES('node5_1','Si1',0,'','0.0314');
INSERT INTO "CM102_blocks_io" VALUES('node5_1','Fi2',0,'','0');
INSERT INTO "CM102_blocks_io" VALUES('node5_1','Pi2',1,'KPP102.Po','97.0145409390343');
INSERT INTO "CM102_blocks_io" VALUES('node5_1','Ti2',1,'KPP102.To','273.000000008736');
INSERT INTO "CM102_blocks_io" VALUES('node5_1','Si2',0,'','0');
INSERT INTO "CM102_blocks_io" VALUES('node5_1','Fi3',0,'','53.0192413422977');
INSERT INTO "CM102_blocks_io" VALUES('node5_1','Pi3',1,'BC111.Po','22.6205908911741');
INSERT INTO "CM102_blocks_io" VALUES('node5_1','Ti3',1,'BC111.To','304.900958903414');
INSERT INTO "CM102_blocks_io" VALUES('node5_1','Si3',0,'','0.0314');
INSERT INTO "CM102_blocks_io" VALUES('node5_1','Fo',1,'С102_1.Fi','53.0465490813841');
INSERT INTO "CM102_blocks_io" VALUES('node5_1','Po',0,'','22.2861344993608');
INSERT INTO "CM102_blocks_io" VALUES('node5_1','To',0,'','304.840446235842');
INSERT INTO "CM102_blocks_io" VALUES('node5_1','So',0,'','0.2');
INSERT INTO "CM102_blocks_io" VALUES('node5_1','lo',0,'','50');
INSERT INTO "CM102_blocks_io" VALUES('node5_1','Q0',0,'','1');
INSERT INTO "CM102_blocks_io" VALUES('node5_1','Ct',0,'','20');
INSERT INTO "CM102_blocks_io" VALUES('node5_1','Riz',0,'','20');
INSERT INTO "CM102_blocks_io" VALUES('node5_1','Fwind',0,'','1');
INSERT INTO "CM102_blocks_io" VALUES('node5_1','Twind',0,'','273');
INSERT INTO "CM102_blocks_io" VALUES('node5_1','f_frq',0,'','100');
INSERT INTO "CM102_blocks_io" VALUES('node5_1','FoR',0,'','49.8962433857394');
INSERT INTO "CM102_blocks_io" VALUES('node5_1','Pot',0,'','20.6955244054771');
INSERT INTO "CM102_blocks_io" VALUES('node5_1','Fit',0,'','38.6468204088627');
INSERT INTO "CM102_blocks_io" VALUES('CM102_1','Fi',0,'','52.5199736661557');
INSERT INTO "CM102_blocks_io" VALUES('CM102_1','Pi',1,'PP103.Po','21.1085596106972');
INSERT INTO "CM102_blocks_io" VALUES('CM102_1','Ti',0,'','273');
INSERT INTO "CM102_blocks_io" VALUES('CM102_1','Fo',1,'AT102_1.Fi','52.5240532248164');
INSERT INTO "CM102_blocks_io" VALUES('CM102_1','Po',0,'','46.0194601750365');
INSERT INTO "CM102_blocks_io" VALUES('CM102_1','To',0,'','344.874218346709');
INSERT INTO "CM102_blocks_io" VALUES('CM102_1','So',0,'','0.017');
INSERT INTO "CM102_blocks_io" VALUES('CM102_1','lo',0,'','55');
INSERT INTO "CM102_blocks_io" VALUES('CM102_1','Kzp',0,'','0.465463090734943');
INSERT INTO "CM102_blocks_io" VALUES('CM102_1','N',0,'','11');
INSERT INTO "CM102_blocks_io" VALUES('CM102_1','V',0,'','0.27');
INSERT INTO "CM102_blocks_io" VALUES('CM102_1','Kpmp',0,'','0.02');
INSERT INTO "CM102_blocks_io" VALUES('CM102_1','Kslp',0,'','0.2');
INSERT INTO "CM102_blocks_io" VALUES('CM102_1','Q0',0,'','1');
INSERT INTO "CM102_blocks_io" VALUES('CM102_1','Ct',0,'','100');
INSERT INTO "CM102_blocks_io" VALUES('CM102_1','Riz',0,'','100');
INSERT INTO "CM102_blocks_io" VALUES('CM102_1','Fwind',0,'','1');
INSERT INTO "CM102_blocks_io" VALUES('CM102_1','Twind',0,'','273');
INSERT INTO "CM102_blocks_io" VALUES('CM102_1','f_frq',0,'','100');
INSERT INTO "CM102_blocks_io" VALUES('CM102_1','Pot',0,'','46.0725119514837');
INSERT INTO "CM102_blocks_io" VALUES('CM102_1','Fit',0,'','52.5214285235014');
INSERT INTO "CM102_blocks_io" VALUES('CM102_2','Fi',0,'','52.0029486798335');
INSERT INTO "CM102_blocks_io" VALUES('CM102_2','Pi',1,'PP104.Po','44.0372024736709');
INSERT INTO "CM102_blocks_io" VALUES('CM102_2','Ti',1,'AT102_1.To','295.870555079229');
INSERT INTO "CM102_blocks_io" VALUES('CM102_2','Fo',1,'AT102_2.Fi','52.0038725527574');
INSERT INTO "CM102_blocks_io" VALUES('CM102_2','Po',0,'','99.6983457920425');
INSERT INTO "CM102_blocks_io" VALUES('CM102_2','To',0,'','378.034473291198');
INSERT INTO "CM102_blocks_io" VALUES('CM102_2','So',0,'','0.00785');
INSERT INTO "CM102_blocks_io" VALUES('CM102_2','lo',0,'','55');
INSERT INTO "CM102_blocks_io" VALUES('CM102_2','Kzp',0,'','0.499171086382358');
INSERT INTO "CM102_blocks_io" VALUES('CM102_2','N',1,'CM102_1.N','11');
INSERT INTO "CM102_blocks_io" VALUES('CM102_2','V',0,'','0.13');
INSERT INTO "CM102_blocks_io" VALUES('CM102_2','Kpmp',0,'','0.02');
INSERT INTO "CM102_blocks_io" VALUES('CM102_2','Kslp',0,'','0.2');
INSERT INTO "CM102_blocks_io" VALUES('CM102_2','Q0',0,'','1');
INSERT INTO "CM102_blocks_io" VALUES('CM102_2','Ct',0,'','100');
INSERT INTO "CM102_blocks_io" VALUES('CM102_2','Riz',0,'','100');
INSERT INTO "CM102_blocks_io" VALUES('CM102_2','Fwind',0,'','1');
INSERT INTO "CM102_blocks_io" VALUES('CM102_2','Twind',0,'','273');
INSERT INTO "CM102_blocks_io" VALUES('CM102_2','f_frq',0,'','100');
INSERT INTO "CM102_blocks_io" VALUES('CM102_2','Pot',0,'','105.987273594529');
INSERT INTO "CM102_blocks_io" VALUES('CM102_2','Fit',0,'','52.0042805799508');
INSERT INTO "CM102_blocks_io" VALUES('node6_1','Fi',0,'','51.483949180575');
INSERT INTO "CM102_blocks_io" VALUES('node6_1','Pi',1,'C102_3.Po','97.0451586190778');
INSERT INTO "CM102_blocks_io" VALUES('node6_1','Fo1',1,'BC116.Fi','0');
INSERT INTO "CM102_blocks_io" VALUES('node6_1','Po1',0,'','97.0451496116792');
INSERT INTO "CM102_blocks_io" VALUES('node6_1','So1',0,'','0.006');
INSERT INTO "CM102_blocks_io" VALUES('node6_1','lo1',0,'','30');
INSERT INTO "CM102_blocks_io" VALUES('node6_1','Fo2',1,'KPP102.Fi','-2.47032822920623e-321');
INSERT INTO "CM102_blocks_io" VALUES('node6_1','Po2',0,'','97.0451576909855');
INSERT INTO "CM102_blocks_io" VALUES('node6_1','So2',0,'','0.002');
INSERT INTO "CM102_blocks_io" VALUES('node6_1','lo2',0,'','30');
INSERT INTO "CM102_blocks_io" VALUES('node6_1','Fo3',1,'BC114.Fi','51.4844506270005');
INSERT INTO "CM102_blocks_io" VALUES('node6_1','Po3',0,'','95.0330857247656');
INSERT INTO "CM102_blocks_io" VALUES('node6_1','So3',0,'','0.006');
INSERT INTO "CM102_blocks_io" VALUES('node6_1','lo3',0,'','5');
INSERT INTO "CM102_blocks_io" VALUES('node6_1','Fo4',1,'BC113.Fi','0');
INSERT INTO "CM102_blocks_io" VALUES('node6_1','Po4',0,'','97.0451503782767');
INSERT INTO "CM102_blocks_io" VALUES('node6_1','So4',0,'','0.006');
INSERT INTO "CM102_blocks_io" VALUES('node6_1','lo4',0,'','10');
INSERT INTO "CM102_blocks_io" VALUES('node6_1','Q0',0,'','1');
INSERT INTO "CM102_blocks_io" VALUES('node6_1','f_frq',0,'','100');
INSERT INTO "CM102_blocks_io" VALUES('node6_1','F1tmp',0,'','-0.000268515421806531');
INSERT INTO "CM102_blocks_io" VALUES('node6_1','F2tmp',0,'','-0.000111255534122124');
INSERT INTO "CM102_blocks_io" VALUES('node6_1','F3tmp',0,'','51.4844202082714');
INSERT INTO "CM102_blocks_io" VALUES('node6_1','F4tmp',0,'','-9.12567404385394e-05');
INSERT INTO "CM102_blocks_io" VALUES('node6_1','Pot1',0,'','99.7120911148752');
INSERT INTO "CM102_blocks_io" VALUES('node6_1','Pot2',0,'','102.187471654186');
INSERT INTO "CM102_blocks_io" VALUES('node6_1','Pot3',0,'','96.7285702901443');
INSERT INTO "CM102_blocks_io" VALUES('node6_1','Pot4',0,'','102.816636476565');
INSERT INTO "CM102_blocks_io" VALUES('PP103','Fi',0,'','52.5193961825569');
INSERT INTO "CM102_blocks_io" VALUES('PP103','Pi',1,'С102_1.Po','21.9416268936822');
INSERT INTO "CM102_blocks_io" VALUES('PP103','Fo',1,'CM102_1.Fi','52.5199545177732');
INSERT INTO "CM102_blocks_io" VALUES('PP103','Po',0,'','21.1085451724531');
INSERT INTO "CM102_blocks_io" VALUES('PP103','dP',0,'','83.3113277152781');
INSERT INTO "CM102_blocks_io" VALUES('PP103','Sdf',0,'','0.02');
INSERT INTO "CM102_blocks_io" VALUES('PP103','So',0,'','0.0314');
INSERT INTO "CM102_blocks_io" VALUES('PP103','lo',0,'','7');
INSERT INTO "CM102_blocks_io" VALUES('PP103','Q0',0,'','1');
INSERT INTO "CM102_blocks_io" VALUES('PP103','f_frq',0,'','100');
INSERT INTO "CM102_blocks_io" VALUES('PP103','Pot',0,'','18.2874094324631');
INSERT INTO "CM102_blocks_io" VALUES('PP103','Fit',0,'','38.0818872167009');
INSERT INTO "CM102_blocks_io" VALUES('PP104','Fi',0,'','52.0024470729689');
INSERT INTO "CM102_blocks_io" VALUES('PP104','Pi',1,'C102_2.Po','44.749579039371');
INSERT INTO "CM102_blocks_io" VALUES('PP104','Fo',1,'CM102_2.Fi','52.0029312612336');
INSERT INTO "CM102_blocks_io" VALUES('PP104','Po',0,'','44.0371793479234');
INSERT INTO "CM102_blocks_io" VALUES('PP104','dP',0,'','71.2427797754885');
INSERT INTO "CM102_blocks_io" VALUES('PP104','Sdf',0,'','0.015');
INSERT INTO "CM102_blocks_io" VALUES('PP104','So',0,'','0.017');
INSERT INTO "CM102_blocks_io" VALUES('PP104','lo',0,'','7');
INSERT INTO "CM102_blocks_io" VALUES('PP104','Q0',0,'','1');
INSERT INTO "CM102_blocks_io" VALUES('PP104','f_frq',0,'','100');
INSERT INTO "CM102_blocks_io" VALUES('PP104','Pot',0,'','42.0718168785713');
INSERT INTO "CM102_blocks_io" VALUES('PP104','Fit',0,'','37.6972413769621');
INSERT INTO "CM102_blocks_io" VALUES('C102_2','Fi',0,'','52.5241795738101');
INSERT INTO "CM102_blocks_io" VALUES('C102_2','Pi',1,'AT102_1.Po','45.3133936622629');
INSERT INTO "CM102_blocks_io" VALUES('C102_2','Si',0,'','0.017');
INSERT INTO "CM102_blocks_io" VALUES('C102_2','Fo',1,'PP104.Fi','52.002482091882');
INSERT INTO "CM102_blocks_io" VALUES('C102_2','Po',0,'','44.7495908931815');
INSERT INTO "CM102_blocks_io" VALUES('C102_2','So',0,'','0.017');
INSERT INTO "CM102_blocks_io" VALUES('C102_2','lo',0,'','50');
INSERT INTO "CM102_blocks_io" VALUES('C102_2','Fo_ж',0,'','0');
INSERT INTO "CM102_blocks_io" VALUES('C102_2','Po_ж',0,'','44.7994193856207');
INSERT INTO "CM102_blocks_io" VALUES('C102_2','Lж',0,'','24.914246219606');
INSERT INTO "CM102_blocks_io" VALUES('C102_2','ProcЖ',0,'','0.01');
INSERT INTO "CM102_blocks_io" VALUES('C102_2','Vap',0,'','2');
INSERT INTO "CM102_blocks_io" VALUES('C102_2','Q0',0,'','1');
INSERT INTO "CM102_blocks_io" VALUES('C102_2','Qж',0,'','1000');
INSERT INTO "CM102_blocks_io" VALUES('C102_2','f_frq',0,'','100');
INSERT INTO "CM102_blocks_io" VALUES('C102_2','Pot',0,'','43.424525814954');
INSERT INTO "CM102_blocks_io" VALUES('C102_2','Fit',0,'','37.7013136906827');
INSERT INTO "CM102_blocks_io" VALUES('C102_3','Fi',0,'','52.0039260716778');
INSERT INTO "CM102_blocks_io" VALUES('C102_3','Pi',1,'AT102_2.Po','98.2303243743125');
INSERT INTO "CM102_blocks_io" VALUES('C102_3','Si',0,'','0.00785');
INSERT INTO "CM102_blocks_io" VALUES('C102_3','Fo',1,'node6_1.Fi','51.4839802747938');
INSERT INTO "CM102_blocks_io" VALUES('C102_3','Po',0,'','97.0451583229321');
INSERT INTO "CM102_blocks_io" VALUES('C102_3','So',0,'','0.00785');
INSERT INTO "CM102_blocks_io" VALUES('C102_3','lo',0,'','8');
INSERT INTO "CM102_blocks_io" VALUES('C102_3','Fo_ж',0,'','0');
INSERT INTO "CM102_blocks_io" VALUES('C102_3','Po_ж',0,'','97.0966136218798');
INSERT INTO "CM102_blocks_io" VALUES('C102_3','Lж',0,'','34.3035326317623');
INSERT INTO "CM102_blocks_io" VALUES('C102_3','ProcЖ',0,'','0.01');
INSERT INTO "CM102_blocks_io" VALUES('C102_3','Vap',0,'','1.5');
INSERT INTO "CM102_blocks_io" VALUES('C102_3','Q0',0,'','1');
INSERT INTO "CM102_blocks_io" VALUES('C102_3','Qж',0,'','1000');
INSERT INTO "CM102_blocks_io" VALUES('C102_3','f_frq',0,'','100');
INSERT INTO "CM102_blocks_io" VALUES('C102_3','Pot',0,'','100.289121612061');
INSERT INTO "CM102_blocks_io" VALUES('C102_3','Fit',0,'','37.1200537368204');
INSERT INTO "CM102_blocks_io" VALUES('G_BC111','pos',0,'','100');
INSERT INTO "CM102_blocks_io" VALUES('G_BC111','com',0,'','1');
INSERT INTO "CM102_blocks_io" VALUES('G_BC111','st_open',0,'','1');
INSERT INTO "CM102_blocks_io" VALUES('G_BC111','st_close',0,'','0');
INSERT INTO "CM102_blocks_io" VALUES('G_BC111','t_full',0,'','5');
INSERT INTO "CM102_blocks_io" VALUES('G_BC111','t_up',0,'','0.5');
INSERT INTO "CM102_blocks_io" VALUES('G_BC111','f_frq',0,'','100');
INSERT INTO "CM102_blocks_io" VALUES('G_BC111','tmp_up',0,'','0');
INSERT INTO "CM102_blocks_io" VALUES('G_BC111','lst_com',0,'','1');
INSERT INTO "CM102_blocks_io" VALUES('G_BC112','pos',0,'','0');
INSERT INTO "CM102_blocks_io" VALUES('G_BC112','com',0,'','0');
INSERT INTO "CM102_blocks_io" VALUES('G_BC112','st_open',0,'','0');
INSERT INTO "CM102_blocks_io" VALUES('G_BC112','st_close',0,'','1');
INSERT INTO "CM102_blocks_io" VALUES('G_BC112','t_full',0,'','3');
INSERT INTO "CM102_blocks_io" VALUES('G_BC112','t_up',0,'','0.3');
INSERT INTO "CM102_blocks_io" VALUES('G_BC112','f_frq',0,'','100');
INSERT INTO "CM102_blocks_io" VALUES('G_BC112','tmp_up',0,'','0');
INSERT INTO "CM102_blocks_io" VALUES('G_BC112','lst_com',0,'','0');
INSERT INTO "CM102_blocks_io" VALUES('G_BC114','pos',0,'','100');
INSERT INTO "CM102_blocks_io" VALUES('G_BC114','com',0,'','1');
INSERT INTO "CM102_blocks_io" VALUES('G_BC114','st_open',0,'','1');
INSERT INTO "CM102_blocks_io" VALUES('G_BC114','st_close',0,'','0');
INSERT INTO "CM102_blocks_io" VALUES('G_BC114','t_full',0,'','5');
INSERT INTO "CM102_blocks_io" VALUES('G_BC114','t_up',0,'','0.5');
INSERT INTO "CM102_blocks_io" VALUES('G_BC114','f_frq',0,'','100');
INSERT INTO "CM102_blocks_io" VALUES('G_BC114','tmp_up',0,'','0');
INSERT INTO "CM102_blocks_io" VALUES('G_BC114','lst_com',0,'','1');
INSERT INTO "CM102_blocks_io" VALUES('G_BC115','pos',0,'','0');
INSERT INTO "CM102_blocks_io" VALUES('G_BC115','com',0,'','0');
INSERT INTO "CM102_blocks_io" VALUES('G_BC115','st_open',0,'','0');
INSERT INTO "CM102_blocks_io" VALUES('G_BC115','st_close',0,'','1');
INSERT INTO "CM102_blocks_io" VALUES('G_BC115','t_full',0,'','5');
INSERT INTO "CM102_blocks_io" VALUES('G_BC115','t_up',0,'','0.5');
INSERT INTO "CM102_blocks_io" VALUES('G_BC115','f_frq',0,'','100');
INSERT INTO "CM102_blocks_io" VALUES('G_BC115','tmp_up',0,'','0');
INSERT INTO "CM102_blocks_io" VALUES('G_BC115','lst_com',0,'','0');
INSERT INTO "CM102_blocks_io" VALUES('G_BC116','pos',0,'','0');
INSERT INTO "CM102_blocks_io" VALUES('G_BC116','com',0,'','0');
INSERT INTO "CM102_blocks_io" VALUES('G_BC116','st_open',0,'','0');
INSERT INTO "CM102_blocks_io" VALUES('G_BC116','st_close',0,'','1');
INSERT INTO "CM102_blocks_io" VALUES('G_BC116','t_full',0,'','5');
INSERT INTO "CM102_blocks_io" VALUES('G_BC116','t_up',0,'','0.5');
INSERT INTO "CM102_blocks_io" VALUES('G_BC116','f_frq',0,'','100');
INSERT INTO "CM102_blocks_io" VALUES('G_BC116','tmp_up',0,'','0');
INSERT INTO "CM102_blocks_io" VALUES('G_BC116','lst_com',0,'','0');
INSERT INTO "CM102_blocks_io" VALUES('KPP102','t_kl1',0,'','3');
INSERT INTO "CM102_blocks_io" VALUES('KPP102','t_kl2',0,'','5');
INSERT INTO "CM102_blocks_io" VALUES('KPP102','tmp_l1',0,'','7.4109846876187e-322');
INSERT INTO "CM102_blocks_io" VALUES('KPP102','tmp_l2',0,'','0');
INSERT INTO "CM102_blocks_io" VALUES('DR101','t_kl1',0,'','10');
INSERT INTO "CM102_blocks_io" VALUES('DR101','t_kl2',0,'','5');
INSERT INTO "CM102_blocks_io" VALUES('DR101','tmp_l1',0,'','49.9999999999964');
INSERT INTO "CM102_blocks_io" VALUES('DR101','tmp_l2',0,'','0');
INSERT INTO "CM102_blocks_io" VALUES('BC111','t_kl1',0,'','10');
INSERT INTO "CM102_blocks_io" VALUES('BC111','t_kl2',0,'','5');
INSERT INTO "CM102_blocks_io" VALUES('BC111','tmp_l1',0,'','99.9999999999929');
INSERT INTO "CM102_blocks_io" VALUES('BC111','tmp_l2',0,'','0');
INSERT INTO "CM102_blocks_io" VALUES('BC113','t_kl1',0,'','10');
INSERT INTO "CM102_blocks_io" VALUES('BC113','t_kl2',0,'','5');
INSERT INTO "CM102_blocks_io" VALUES('BC113','tmp_l1',0,'','0');
INSERT INTO "CM102_blocks_io" VALUES('BC113','tmp_l2',0,'','0');
INSERT INTO "CM102_blocks_io" VALUES('BC114','t_kl1',0,'','10');
INSERT INTO "CM102_blocks_io" VALUES('BC114','t_kl2',0,'','5');
INSERT INTO "CM102_blocks_io" VALUES('BC114','tmp_l1',0,'','99.9999999999929');
INSERT INTO "CM102_blocks_io" VALUES('BC114','tmp_l2',0,'','0');
INSERT INTO "CM102_blocks_io" VALUES('BC116','t_kl1',0,'','10');
INSERT INTO "CM102_blocks_io" VALUES('BC116','t_kl2',0,'','5');
INSERT INTO "CM102_blocks_io" VALUES('BC116','tmp_l1',0,'','0');
INSERT INTO "CM102_blocks_io" VALUES('BC116','tmp_l2',0,'','0');
INSERT INTO "CM102_blocks_io" VALUES('node5_1','Kpr',0,'','0.95');
INSERT INTO "CM102_blocks_io" VALUES('CM102_1','Kpr',0,'','0.95');
INSERT INTO "CM102_blocks_io" VALUES('CM102_2','Kpr',0,'','0.95');
INSERT INTO "CM102_blocks_io" VALUES('node6_1','Kpr',0,'','0.95');
INSERT INTO "CM102_blocks_io" VALUES('PP103','Kpr',0,'','0.95');
INSERT INTO "CM102_blocks_io" VALUES('PP104','Kpr',0,'','0.95');
CREATE TABLE 'CM102_prm' ("SHIFR" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '' ,"IO" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#IO" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#IO" TEXT DEFAULT '' , PRIMARY KEY ("SHIFR"));
INSERT INTO "CM102_prm" VALUES('F103','F103','Flow on the diaphragm PP103',1,'PP103.Fi:var:Variable
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
INSERT INTO "CM102_prm" VALUES('F104','F104','Flow on the diaphragm PP104',1,'PP104.Fi:var:Variable
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
INSERT INTO "CM102_prm" VALUES('KPP102','KPP102','Anti-surge control valve of the compressor',1,'KPP102.l_kl1:out:Output
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
INSERT INTO "CM102_prm" VALUES('ST8612','ST8612_4','Rotation speed of the compressor',1,'CM102_1.N:var:Variable
CM102_1.N:varIn:Variable input
*s.1000x rpm:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.15:max:Maximum','Оберти компресору','Обороты вращения компрессора','','CM102_1.N:var:Переменная
CM102_1.N:varIn:Ввод переменной
*s.1000x об.мин.:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.15:max:Максимум','','CM102_1.N:var:Змінна
CM102_1.N:varIn:Ввід змінної
*s.1000x об.хвил:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.15:max:Максимум');
INSERT INTO "CM102_prm" VALUES('TE1313_1','TE1313_4','Temperature at the input of the first stage of the compressor',1,'node5_1.To:var:Variable
*s.К:ed:Dimension
*i.0:prec:Precision
*r.273:min:Minimum
*r.373:max:Maximum','Температура на вході першого ступеня компресору','Температура на входе первой ступени компрессора','','node5_1.To:var:Переменная
*s.К:ed:Ед.измерения
*i.0:prec:Точность
*r.273:min:Минимум
*r.373:max:Максимум','','node5_1.To:var:Змінна
*s.К:ed:Од.виміру
*i.0:prec:Точність
*r.273:min:Мінімум
*r.373:max:Максимум');
INSERT INTO "CM102_prm" VALUES('TE1314_1','TE1314_4','Temperature after the fridge of the first stage of the compressor',1,'AT102_1.To:var:Variable
*s.К:ed:Dimension
*i.0:prec:Precision
*r.273:min:Minimum
*r.373:max:Maximum','Температура після холодильника першого ступеня компресору','Температура после холодильника первой ступени компрессора','','AT102_1.To:var:Переменная
*s.К:ed:Ед.измерения
*i.0:prec:Точность
*r.273:min:Минимум
*r.373:max:Максимум','','AT102_1.To:var:Змінна
*s.К:ed:Од.виміру
*i.0:prec:Точність
*r.273:min:Мінімум
*r.373:max:Максимум');
INSERT INTO "CM102_prm" VALUES('BC111','BC111','Control block of the ball crane BC111',1,'G_BC111.com:com
G_BC111.st_open:st_open
G_BC111.st_close:st_close','Блок керування кульовим краном КК111','Блок управления шарового крана КШ111','КШ111','','КК111','');
INSERT INTO "CM102_prm" VALUES('BC112','BC112','Control block of the ball crane BC112',1,'G_BC112.com:com
G_BC112.st_open:st_open
G_BC112.st_close:st_close','Блок керування кульовим краном КК112','Блок управления шарового крана КШ112','КШ112','','КК112','');
INSERT INTO "CM102_prm" VALUES('BC114','BC114','Control block of the ball crane BC114',1,'G_BC114.com:com
G_BC114.st_open:st_open
G_BC114.st_close:st_close','Блок керування кульовим краном КК114','Блок управления шарового крана КШ114','КШ114','','КК114','');
INSERT INTO "CM102_prm" VALUES('BC115','BC115','Control block of the ball crane BC115',1,'G_BC115.com:com
G_BC115.st_open:st_open
G_BC115.st_close:st_close','Блок керування кульовим краном КК115','Блок управления шарового крана КШ115','КШ115','','КК115','');
INSERT INTO "CM102_prm" VALUES('BC116','BC116','Control block of the ball crane BC116',1,'G_BC116.com:com
G_BC116.st_open:st_open
G_BC116.st_close:st_close','Блок керування кульовим краном КК116','Блок управления шарового крана КШ116','КШ116','','КК116','');
INSERT INTO "CM102_prm" VALUES('P103','P103','Pressure on the diaphragm PP103',1,'PP103.Pi:var:Variable
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
INSERT INTO "CM102_prm" VALUES('PT1006_1','PT1006_4','Pressure on the diaphragm PP104',1,'PP104.Pi:var:Variable
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
INSERT INTO "CM102_prm" VALUES('TT0202','TE1202_4','Temperature after the first stage of the compressor',1,'CM102_1.To:var:Variable
*s.К:ed:Dimension
*i.0:prec:Precision
*r.273:min:Minimum
*r.373:max:Maximum','Температура після першого ступеня компресору','Температура после первой ступени компрессора','','CM102_1.To:var:Переменная
*s.К:ed:Ед.измерения
*i.0:prec:Точность
*r.273:min:Минимум
*r.373:max:Максимум','','CM102_1.To:var:Змінна
*s.К:ed:Од.виміру
*i.0:prec:Точність
*r.273:min:Мінімум
*r.373:max:Максимум');
INSERT INTO "CM102_prm" VALUES('TT0204','TE1205_4','Temperature after the second stage of the compressor',1,'CM102_2.To:var:Variable
*s.К:ed:Dimension
*i.0:prec:Precision
*r.273:min:Minimum
*r.423:max:Maximum','Температура після другого ступеня компресору','Температура после второй ступени компрессора','','CM102_2.To:var:Переменная
*s.К:ed:Ед.измерения
*i.0:prec:Точность
*r.273:min:Минимум
*r.423:max:Максимум','','CM102_2.To:var:Змінна
*s.К:ed:Од.виміру
*i.0:prec:Точність
*r.273:min:Мінімум
*r.423:max:Максимум');
INSERT INTO "CM102_prm" VALUES('TE1314_2','TE1206_4','Temperature after the fridge of the second stage of the compressor',1,'AT102_2.To:var:Variable
*s.К:ed:Dimension
*i.0:prec:Precision
*r.273:min:Minimum
*r.373:max:Maximum','Температура після холодильника другого ступеня компресору','Температура после холодильника второй ступени компрессора','','AT102_2.To:var:Переменная
*s.К:ed:Ед.измерения
*i.0:prec:Точность
*r.273:min:Минимум
*r.373:max:Максимум','','AT102_2.To:var:Змінна
*s.К:ed:Од.виміру
*i.0:prec:Точність
*r.273:min:Мінімум
*r.373:max:Максимум');
INSERT INTO "CM102_prm" VALUES('PT0202','PT0202_4','Pressure after the first stage of the compressor',1,'CM102_1.Po:var:Variable
*s.at:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.75:max:Maximum','Тиск після першого ступеня компресору','Давление после первой ступени компрессора','','CM102_1.Po:var:Переменная
*s.ат:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.75:max:Максимум','','CM102_1.Po:var:Змінна
*s.ат:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.75:max:Максимум');
INSERT INTO "CM102_prm" VALUES('PT0204','PT0204_4','Pressure after the second stage of the compressor',1,'CM102_2.Po:var:Variable
*s.at:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.150:max:Maximum','Тиск після другого ступеня компресору','Давление после второй ступени компрессора','','CM102_2.Po:var:Переменная
*s.ат:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.150:max:Максимум','','CM102_2.Po:var:Змінна
*s.ат:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.150:max:Максимум');
CREATE TABLE 'CM102cntr_blocks_io' ("BLK_ID" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"TLNK" INTEGER DEFAULT '' ,"LNK" TEXT DEFAULT '' ,"VAL" TEXT DEFAULT '' , PRIMARY KEY ("BLK_ID","ID"));
INSERT INTO "CM102cntr_blocks_io" VALUES('BC111','pos',6,'CM102.BC111.l_kl1','100');
INSERT INTO "CM102cntr_blocks_io" VALUES('BC111','com',0,'','1');
INSERT INTO "CM102cntr_blocks_io" VALUES('BC111','st_open',0,'','1');
INSERT INTO "CM102cntr_blocks_io" VALUES('BC111','st_close',0,'','0');
INSERT INTO "CM102cntr_blocks_io" VALUES('BC111','t_full',0,'','5');
INSERT INTO "CM102cntr_blocks_io" VALUES('BC111','t_up',0,'','0.5');
INSERT INTO "CM102cntr_blocks_io" VALUES('BC111','f_frq',0,'','1');
INSERT INTO "CM102cntr_blocks_io" VALUES('BC111','tmp_up',0,'','0');
INSERT INTO "CM102cntr_blocks_io" VALUES('BC111','lst_com',0,'','1');
INSERT INTO "CM102cntr_blocks_io" VALUES('BC112','pos',6,'CM102.BC111.l_kl2','0');
INSERT INTO "CM102cntr_blocks_io" VALUES('BC112','com',0,'','0');
INSERT INTO "CM102cntr_blocks_io" VALUES('BC112','st_open',0,'','0');
INSERT INTO "CM102cntr_blocks_io" VALUES('BC112','st_close',0,'','1');
INSERT INTO "CM102cntr_blocks_io" VALUES('BC112','t_full',0,'','5');
INSERT INTO "CM102cntr_blocks_io" VALUES('BC112','t_up',0,'','0.5');
INSERT INTO "CM102cntr_blocks_io" VALUES('BC112','f_frq',0,'','1');
INSERT INTO "CM102cntr_blocks_io" VALUES('BC112','tmp_up',0,'','0');
INSERT INTO "CM102cntr_blocks_io" VALUES('BC112','lst_com',0,'','0');
INSERT INTO "CM102cntr_blocks_io" VALUES('BC113','pos',6,'CM102.BC113.l_kl1','0');
INSERT INTO "CM102cntr_blocks_io" VALUES('BC113','com',0,'','0');
INSERT INTO "CM102cntr_blocks_io" VALUES('BC113','st_open',0,'','0');
INSERT INTO "CM102cntr_blocks_io" VALUES('BC113','st_close',0,'','1');
INSERT INTO "CM102cntr_blocks_io" VALUES('BC113','t_full',0,'','5');
INSERT INTO "CM102cntr_blocks_io" VALUES('BC113','t_up',0,'','0.5');
INSERT INTO "CM102cntr_blocks_io" VALUES('BC113','f_frq',0,'','1');
INSERT INTO "CM102cntr_blocks_io" VALUES('BC113','tmp_up',0,'','0');
INSERT INTO "CM102cntr_blocks_io" VALUES('BC113','lst_com',0,'','0');
INSERT INTO "CM102cntr_blocks_io" VALUES('BC114','pos',6,'CM102.BC114.l_kl1','100');
INSERT INTO "CM102cntr_blocks_io" VALUES('BC114','com',0,'','1');
INSERT INTO "CM102cntr_blocks_io" VALUES('BC114','st_open',0,'','1');
INSERT INTO "CM102cntr_blocks_io" VALUES('BC114','st_close',0,'','0');
INSERT INTO "CM102cntr_blocks_io" VALUES('BC114','t_full',0,'','5');
INSERT INTO "CM102cntr_blocks_io" VALUES('BC114','t_up',0,'','0.5');
INSERT INTO "CM102cntr_blocks_io" VALUES('BC114','f_frq',0,'','1');
INSERT INTO "CM102cntr_blocks_io" VALUES('BC114','tmp_up',0,'','0');
INSERT INTO "CM102cntr_blocks_io" VALUES('BC114','lst_com',0,'','1');
INSERT INTO "CM102cntr_blocks_io" VALUES('BC116','pos',6,'CM102.BC116.l_kl1','0');
INSERT INTO "CM102cntr_blocks_io" VALUES('BC116','com',0,'','0');
INSERT INTO "CM102cntr_blocks_io" VALUES('BC116','st_open',0,'','0');
INSERT INTO "CM102cntr_blocks_io" VALUES('BC116','st_close',0,'','1');
INSERT INTO "CM102cntr_blocks_io" VALUES('BC116','t_full',0,'','5');
INSERT INTO "CM102cntr_blocks_io" VALUES('BC116','t_up',0,'','0.5');
INSERT INTO "CM102cntr_blocks_io" VALUES('BC116','f_frq',0,'','1');
INSERT INTO "CM102cntr_blocks_io" VALUES('BC116','tmp_up',0,'','0');
INSERT INTO "CM102cntr_blocks_io" VALUES('BC116','lst_com',0,'','0');
CREATE TABLE 'CM102cntr_prm' ("SHIFR" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '' ,"IO" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"ru#IO" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"uk#IO" TEXT DEFAULT '' , PRIMARY KEY ("SHIFR"));
INSERT INTO "CM102cntr_prm" VALUES('BC111','BC111','',1,'BC111.com:com
BC111.st_open:st_open
BC111.st_close:st_close','КШ111','','','КК111','','');
INSERT INTO "CM102cntr_prm" VALUES('BC112','BC112','',1,'BC112.com:com
BC112.st_open:st_open
BC112.st_close:st_close','КШ112','','','КК112','','');
INSERT INTO "CM102cntr_prm" VALUES('BC113','BC113','',1,'BC113.com:com
BC113.st_open:st_open
BC113.st_close:st_close','КШ113','','','КК113','','');
INSERT INTO "CM102cntr_prm" VALUES('BC114','BC114','',1,'BC114.com:com
BC114.st_open:st_open
BC114.st_close:st_close','КШ114','','','КК114','','');
INSERT INTO "CM102cntr_prm" VALUES('BC116','BC116','',1,'BC116.com:com
BC116.st_open:st_open
BC116.st_close:st_close','КШ116','','','КК116','','');
CREATE TABLE 'CM201_blocks_io' ("BLK_ID" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"TLNK" INTEGER DEFAULT '' ,"LNK" TEXT DEFAULT '' ,"VAL" TEXT DEFAULT '' , PRIMARY KEY ("BLK_ID","ID"));
INSERT INTO "CM201_blocks_io" VALUES('CM101_1','Fi',0,'','50.2523365736044');
INSERT INTO "CM201_blocks_io" VALUES('CM101_1','Pi',1,'PP101.Po','5.39786899725542');
INSERT INTO "CM201_blocks_io" VALUES('CM101_1','Ti',0,'','273');
INSERT INTO "CM201_blocks_io" VALUES('CM101_1','Fo',1,'AT101_1.Fi','50.2573189059548');
INSERT INTO "CM201_blocks_io" VALUES('CM101_1','Po',0,'','10.3220057791189');
INSERT INTO "CM201_blocks_io" VALUES('CM101_1','To',0,'','331.562291183288');
INSERT INTO "CM201_blocks_io" VALUES('CM101_1','So',0,'','0.071');
INSERT INTO "CM201_blocks_io" VALUES('CM101_1','lo',0,'','55');
INSERT INTO "CM201_blocks_io" VALUES('CM101_1','Kzp',0,'','0.68977922091029');
INSERT INTO "CM201_blocks_io" VALUES('CM101_1','N',0,'','8.2');
INSERT INTO "CM201_blocks_io" VALUES('CM101_1','V',0,'','1.5');
INSERT INTO "CM201_blocks_io" VALUES('CM101_1','Kpmp',0,'','0.025');
INSERT INTO "CM201_blocks_io" VALUES('CM101_1','Kslp',0,'','0.2');
INSERT INTO "CM201_blocks_io" VALUES('CM101_1','Q0',0,'','1');
INSERT INTO "CM201_blocks_io" VALUES('CM101_1','Ct',0,'','100');
INSERT INTO "CM201_blocks_io" VALUES('CM101_1','Riz',0,'','100');
INSERT INTO "CM201_blocks_io" VALUES('CM101_1','Fwind',0,'','1');
INSERT INTO "CM201_blocks_io" VALUES('CM101_1','Twind',0,'','273');
INSERT INTO "CM201_blocks_io" VALUES('CM101_1','f_frq',0,'','100');
INSERT INTO "CM201_blocks_io" VALUES('CM101_1','Pot',0,'','10.9193045203657');
INSERT INTO "CM201_blocks_io" VALUES('CM101_1','Fit',0,'','50.2537584299028');
INSERT INTO "CM201_blocks_io" VALUES('CM101_2','Fi',0,'','49.7598946514195');
INSERT INTO "CM201_blocks_io" VALUES('CM101_2','Pi',1,'PP102.Po','9.85107738863571');
INSERT INTO "CM201_blocks_io" VALUES('CM101_2','Ti',1,'AT101_1.To','292.752943486513');
INSERT INTO "CM201_blocks_io" VALUES('CM101_2','Fo',1,'AT101_2.Fi','49.7654159395467');
INSERT INTO "CM201_blocks_io" VALUES('CM101_2','Po',0,'','25.9506627054125');
INSERT INTO "CM201_blocks_io" VALUES('CM101_2','To',0,'','391.411919754239');
INSERT INTO "CM201_blocks_io" VALUES('CM101_2','So',0,'','0.0314');
INSERT INTO "CM201_blocks_io" VALUES('CM101_2','lo',0,'','55');
INSERT INTO "CM201_blocks_io" VALUES('CM101_2','Kzp',0,'','0.886553806911506');
INSERT INTO "CM201_blocks_io" VALUES('CM101_2','N',1,'CM101_1.N','8.2');
INSERT INTO "CM201_blocks_io" VALUES('CM101_2','V',0,'','1');
INSERT INTO "CM201_blocks_io" VALUES('CM101_2','Kpmp',0,'','0.035');
INSERT INTO "CM201_blocks_io" VALUES('CM101_2','Kslp',0,'','0.2');
INSERT INTO "CM201_blocks_io" VALUES('CM101_2','Q0',0,'','1');
INSERT INTO "CM201_blocks_io" VALUES('CM101_2','Ct',0,'','100');
INSERT INTO "CM201_blocks_io" VALUES('CM101_2','Riz',0,'','100');
INSERT INTO "CM201_blocks_io" VALUES('CM101_2','Fwind',0,'','1');
INSERT INTO "CM201_blocks_io" VALUES('CM101_2','Twind',0,'','273');
INSERT INTO "CM201_blocks_io" VALUES('CM101_2','f_frq',0,'','100');
INSERT INTO "CM201_blocks_io" VALUES('CM101_2','Pot',0,'','27.6535126272466');
INSERT INTO "CM201_blocks_io" VALUES('CM101_2','Fit',0,'','49.7612994262815');
INSERT INTO "CM201_blocks_io" VALUES('KPP101','Fi',0,'','1.98481202365534e-263');
INSERT INTO "CM201_blocks_io" VALUES('KPP101','Pi',1,'node4_1.Po2','25.3898837158807');
INSERT INTO "CM201_blocks_io" VALUES('KPP101','Ti',1,'AT101_2.To','304.603669552392');
INSERT INTO "CM201_blocks_io" VALUES('KPP101','Fo',1,'node3_1.Fi2','0.000580634975366274');
INSERT INTO "CM201_blocks_io" VALUES('KPP101','Po',0,'','5.48537183626317');
INSERT INTO "CM201_blocks_io" VALUES('KPP101','To',0,'','276.860920515647');
INSERT INTO "CM201_blocks_io" VALUES('KPP101','So',0,'','0.071');
INSERT INTO "CM201_blocks_io" VALUES('KPP101','lo',0,'','10');
INSERT INTO "CM201_blocks_io" VALUES('KPP101','S_kl1',0,'','0.04');
INSERT INTO "CM201_blocks_io" VALUES('KPP101','S_kl2',0,'','0.001');
INSERT INTO "CM201_blocks_io" VALUES('KPP101','l_kl1',2,'gen_cntr.SurgeCM201.out','0');
INSERT INTO "CM201_blocks_io" VALUES('KPP101','l_kl2',0,'','0');
INSERT INTO "CM201_blocks_io" VALUES('KPP101','Q0',0,'','1');
INSERT INTO "CM201_blocks_io" VALUES('KPP101','Kln',0,'','2');
INSERT INTO "CM201_blocks_io" VALUES('KPP101','Kpr',0,'','0.95');
INSERT INTO "CM201_blocks_io" VALUES('KPP101','Ct',0,'','20');
INSERT INTO "CM201_blocks_io" VALUES('KPP101','Riz',0,'','20');
INSERT INTO "CM201_blocks_io" VALUES('KPP101','noBack',0,'','0');
INSERT INTO "CM201_blocks_io" VALUES('KPP101','Fwind',0,'','1');
INSERT INTO "CM201_blocks_io" VALUES('KPP101','Twind',0,'','273');
INSERT INTO "CM201_blocks_io" VALUES('KPP101','f_frq',0,'','100');
INSERT INTO "CM201_blocks_io" VALUES('KPP101','Pot',0,'','5.23644114594017');
INSERT INTO "CM201_blocks_io" VALUES('KPP101','Fit',0,'','2.13883580327003');
INSERT INTO "CM201_blocks_io" VALUES('DR100','Fi',0,'','-0.00139986598029103');
INSERT INTO "CM201_blocks_io" VALUES('DR100','Pi',1,'BC106.Po','5.4853666380332');
INSERT INTO "CM201_blocks_io" VALUES('DR100','Ti',1,'BC106.To','273');
INSERT INTO "CM201_blocks_io" VALUES('DR100','Fo',1,'node3_1.Fi1','0.000604086336482875');
INSERT INTO "CM201_blocks_io" VALUES('DR100','Po',0,'','5.48535840453755');
INSERT INTO "CM201_blocks_io" VALUES('DR100','To',0,'','272.999999989907');
INSERT INTO "CM201_blocks_io" VALUES('DR100','So',0,'','0.2');
INSERT INTO "CM201_blocks_io" VALUES('DR100','lo',0,'','1');
INSERT INTO "CM201_blocks_io" VALUES('DR100','S_kl1',0,'','0.05');
INSERT INTO "CM201_blocks_io" VALUES('DR100','S_kl2',0,'','0.01');
INSERT INTO "CM201_blocks_io" VALUES('DR100','l_kl1',0,'','50');
INSERT INTO "CM201_blocks_io" VALUES('DR100','l_kl2',0,'','0');
INSERT INTO "CM201_blocks_io" VALUES('DR100','Q0',0,'','1');
INSERT INTO "CM201_blocks_io" VALUES('DR100','Kln',0,'','1');
INSERT INTO "CM201_blocks_io" VALUES('DR100','Kpr',0,'','0.95');
INSERT INTO "CM201_blocks_io" VALUES('DR100','Ct',0,'','20');
INSERT INTO "CM201_blocks_io" VALUES('DR100','Riz',0,'','20');
INSERT INTO "CM201_blocks_io" VALUES('DR100','noBack',0,'','0');
INSERT INTO "CM201_blocks_io" VALUES('DR100','Fwind',0,'','1');
INSERT INTO "CM201_blocks_io" VALUES('DR100','Twind',0,'','273');
INSERT INTO "CM201_blocks_io" VALUES('DR100','f_frq',0,'','100');
INSERT INTO "CM201_blocks_io" VALUES('DR100','Pot',0,'','4.83622293773731');
INSERT INTO "CM201_blocks_io" VALUES('DR100','Fit',0,'','16.9179842059563');
INSERT INTO "CM201_blocks_io" VALUES('AT101_1','Fi',0,'','50.257336891645');
INSERT INTO "CM201_blocks_io" VALUES('AT101_1','Pi',1,'CM101_1.Po','10.3220130306421');
INSERT INTO "CM201_blocks_io" VALUES('AT101_1','Ti',1,'CM101_1.To','331.562283088777');
INSERT INTO "CM201_blocks_io" VALUES('AT101_1','Si',0,'','0.065');
INSERT INTO "CM201_blocks_io" VALUES('AT101_1','li',0,'','10');
INSERT INTO "CM201_blocks_io" VALUES('AT101_1','Fo',1,'C101_2.Fi','50.2575152262436');
INSERT INTO "CM201_blocks_io" VALUES('AT101_1','Po',0,'','10.1689329888187');
INSERT INTO "CM201_blocks_io" VALUES('AT101_1','To',0,'','292.752945223301');
INSERT INTO "CM201_blocks_io" VALUES('AT101_1','So',0,'','0.071');
INSERT INTO "CM201_blocks_io" VALUES('AT101_1','lo',0,'','2');
INSERT INTO "CM201_blocks_io" VALUES('AT101_1','Tair',0,'','283');
INSERT INTO "CM201_blocks_io" VALUES('AT101_1','Wc',0,'','200');
INSERT INTO "CM201_blocks_io" VALUES('AT101_1','Q0',0,'','1');
INSERT INTO "CM201_blocks_io" VALUES('AT101_1','Ct',0,'','100');
INSERT INTO "CM201_blocks_io" VALUES('AT101_1','Rt',0,'','1');
INSERT INTO "CM201_blocks_io" VALUES('AT101_1','f_frq',0,'','100');
INSERT INTO "CM201_blocks_io" VALUES('AT101_1','Pot',0,'','10.5823391227739');
INSERT INTO "CM201_blocks_io" VALUES('AT101_1','Fit',0,'','37.7399346381534');
INSERT INTO "CM201_blocks_io" VALUES('AT101_2','Fi',0,'','49.7655102714126');
INSERT INTO "CM201_blocks_io" VALUES('AT101_2','Pi',1,'CM101_2.Po','25.9506810054563');
INSERT INTO "CM201_blocks_io" VALUES('AT101_2','Ti',1,'CM101_2.To','391.41191491509');
INSERT INTO "CM201_blocks_io" VALUES('AT101_2','Si',0,'','0.029');
INSERT INTO "CM201_blocks_io" VALUES('AT101_2','li',0,'','20');
INSERT INTO "CM201_blocks_io" VALUES('AT101_2','Fo',1,'C101_3.Fi','49.7658963659578');
INSERT INTO "CM201_blocks_io" VALUES('AT101_2','Po',0,'','25.6498328698296');
INSERT INTO "CM201_blocks_io" VALUES('AT101_2','To',0,'','304.603673321952');
INSERT INTO "CM201_blocks_io" VALUES('AT101_2','So',0,'','0.0314');
INSERT INTO "CM201_blocks_io" VALUES('AT101_2','lo',0,'','5');
INSERT INTO "CM201_blocks_io" VALUES('AT101_2','Tair',0,'','283');
INSERT INTO "CM201_blocks_io" VALUES('AT101_2','Wc',0,'','200');
INSERT INTO "CM201_blocks_io" VALUES('AT101_2','Q0',0,'','1');
INSERT INTO "CM201_blocks_io" VALUES('AT101_2','Ct',0,'','100');
INSERT INTO "CM201_blocks_io" VALUES('AT101_2','Rt',0,'','1');
INSERT INTO "CM201_blocks_io" VALUES('AT101_2','f_frq',0,'','100');
INSERT INTO "CM201_blocks_io" VALUES('AT101_2','Pot',0,'','26.8981088252344');
INSERT INTO "CM201_blocks_io" VALUES('AT101_2','Fit',0,'','37.3649946813934');
INSERT INTO "CM201_blocks_io" VALUES('node3_1','Fi1',0,'','0.000405540680766616');
INSERT INTO "CM201_blocks_io" VALUES('node3_1','Pi1',1,'DR100.Po','5.48535840453755');
INSERT INTO "CM201_blocks_io" VALUES('node3_1','Ti1',1,'DR100.To','272.999999989907');
INSERT INTO "CM201_blocks_io" VALUES('node3_1','Si1',0,'','0.2');
INSERT INTO "CM201_blocks_io" VALUES('node3_1','Fi2',0,'','0.000596613194259008');
INSERT INTO "CM201_blocks_io" VALUES('node3_1','Pi2',1,'KPP101.Po','5.48537183626317');
INSERT INTO "CM201_blocks_io" VALUES('node3_1','Ti2',1,'KPP101.To','276.860920515647');
INSERT INTO "CM201_blocks_io" VALUES('node3_1','Si2',0,'','0.071');
INSERT INTO "CM201_blocks_io" VALUES('node3_1','Fi3',0,'','50.7451771048397');
INSERT INTO "CM201_blocks_io" VALUES('node3_1','Pi3',1,'BC101.Po','5.51863154869261');
INSERT INTO "CM201_blocks_io" VALUES('node3_1','Ti3',1,'BC101.To','290.308932480119');
INSERT INTO "CM201_blocks_io" VALUES('node3_1','Si3',0,'','0.2');
INSERT INTO "CM201_blocks_io" VALUES('node3_1','Fo',1,'C101_1.Fi','50.7531912003662');
INSERT INTO "CM201_blocks_io" VALUES('node3_1','Po',0,'','5.48536716211823');
INSERT INTO "CM201_blocks_io" VALUES('node3_1','To',0,'','290.278038150773');
INSERT INTO "CM201_blocks_io" VALUES('node3_1','So',0,'','0.2');
INSERT INTO "CM201_blocks_io" VALUES('node3_1','lo',0,'','50');
INSERT INTO "CM201_blocks_io" VALUES('node3_1','Q0',0,'','1');
INSERT INTO "CM201_blocks_io" VALUES('node3_1','Ct',0,'','20');
INSERT INTO "CM201_blocks_io" VALUES('node3_1','Riz',0,'','20');
INSERT INTO "CM201_blocks_io" VALUES('node3_1','Fwind',0,'','1');
INSERT INTO "CM201_blocks_io" VALUES('node3_1','Twind',0,'','273');
INSERT INTO "CM201_blocks_io" VALUES('node3_1','f_frq',0,'','100');
INSERT INTO "CM201_blocks_io" VALUES('node3_1','FoR',0,'','0.145506496403985');
INSERT INTO "CM201_blocks_io" VALUES('node3_1','Pot',0,'','5.23491609433684');
INSERT INTO "CM201_blocks_io" VALUES('node3_1','Fit',0,'','37.1522845875593');
INSERT INTO "CM201_blocks_io" VALUES('BC101','Fi',0,'','50.7445731786476');
INSERT INTO "CM201_blocks_io" VALUES('BC101','Pi',0,'','5.54936637855346');
INSERT INTO "CM201_blocks_io" VALUES('BC101','Ti',2,'gen.PCV1.To','290.373924762438');
INSERT INTO "CM201_blocks_io" VALUES('BC101','Fo',1,'node3_1.Fi3','50.7452415606616');
INSERT INTO "CM201_blocks_io" VALUES('BC101','Po',0,'','5.51863533098921');
INSERT INTO "CM201_blocks_io" VALUES('BC101','To',0,'','290.308945692316');
INSERT INTO "CM201_blocks_io" VALUES('BC101','So',0,'','0.2');
INSERT INTO "CM201_blocks_io" VALUES('BC101','lo',0,'','5');
INSERT INTO "CM201_blocks_io" VALUES('BC101','S_kl1',0,'','0.2');
INSERT INTO "CM201_blocks_io" VALUES('BC101','S_kl2',0,'','0.05');
INSERT INTO "CM201_blocks_io" VALUES('BC101','l_kl1',1,'G_BC101.pos','100');
INSERT INTO "CM201_blocks_io" VALUES('BC101','l_kl2',1,'G_BC102.pos','0');
INSERT INTO "CM201_blocks_io" VALUES('BC101','Q0',0,'','1');
INSERT INTO "CM201_blocks_io" VALUES('BC101','Kln',0,'','1');
INSERT INTO "CM201_blocks_io" VALUES('BC101','Kpr',0,'','0.95');
INSERT INTO "CM201_blocks_io" VALUES('BC101','Ct',0,'','20');
INSERT INTO "CM201_blocks_io" VALUES('BC101','Riz',0,'','20');
INSERT INTO "CM201_blocks_io" VALUES('BC101','noBack',0,'','0');
INSERT INTO "CM201_blocks_io" VALUES('BC101','Fwind',0,'','1');
INSERT INTO "CM201_blocks_io" VALUES('BC101','Twind',0,'','273');
INSERT INTO "CM201_blocks_io" VALUES('BC101','f_frq',0,'','100');
INSERT INTO "CM201_blocks_io" VALUES('BC101','Pot',0,'','5.29091023245053');
INSERT INTO "CM201_blocks_io" VALUES('BC101','Fit',0,'','36.0508441505443');
INSERT INTO "CM201_blocks_io" VALUES('node4_1','Fi',0,'','49.268656017919');
INSERT INTO "CM201_blocks_io" VALUES('node4_1','Pi',1,'C101_3.Po','25.3898578016879');
INSERT INTO "CM201_blocks_io" VALUES('node4_1','Fo1',1,'BC106.Fi','0');
INSERT INTO "CM201_blocks_io" VALUES('node4_1','Po1',0,'','25.3898675963406');
INSERT INTO "CM201_blocks_io" VALUES('node4_1','So1',0,'','0.0314');
INSERT INTO "CM201_blocks_io" VALUES('node4_1','lo1',0,'','5');
INSERT INTO "CM201_blocks_io" VALUES('node4_1','Fo2',1,'KPP101.Fi','1.98282721163168e-263');
INSERT INTO "CM201_blocks_io" VALUES('node4_1','Po2',0,'','25.3897927378104');
INSERT INTO "CM201_blocks_io" VALUES('node4_1','So2',0,'','0.0177');
INSERT INTO "CM201_blocks_io" VALUES('node4_1','lo2',0,'','5');
INSERT INTO "CM201_blocks_io" VALUES('node4_1','Fo3',1,'BC104.Fi','49.2703430451652');
INSERT INTO "CM201_blocks_io" VALUES('node4_1','Po3',0,'','25.1327176489393');
INSERT INTO "CM201_blocks_io" VALUES('node4_1','So3',0,'','0.0314');
INSERT INTO "CM201_blocks_io" VALUES('node4_1','lo3',0,'','5');
INSERT INTO "CM201_blocks_io" VALUES('node4_1','Q0',0,'','1');
INSERT INTO "CM201_blocks_io" VALUES('node4_1','f_frq',0,'','100');
INSERT INTO "CM201_blocks_io" VALUES('node4_1','F1tmp',0,'','7.04609985555917e-05');
INSERT INTO "CM201_blocks_io" VALUES('node4_1','F2tmp',0,'','-0.00111918872994016');
INSERT INTO "CM201_blocks_io" VALUES('node4_1','F3tmp',0,'','49.2697047456504');
INSERT INTO "CM201_blocks_io" VALUES('node4_1','Pot1',0,'','25.7971704231136');
INSERT INTO "CM201_blocks_io" VALUES('node4_1','Pot2',0,'','27.0285722141214');
INSERT INTO "CM201_blocks_io" VALUES('node4_1','Pot3',0,'','25.9924639237477');
INSERT INTO "CM201_blocks_io" VALUES('BC104','Fi',0,'','49.2703757639637');
INSERT INTO "CM201_blocks_io" VALUES('BC104','Pi',1,'node4_1.Po3','25.1327582812939');
INSERT INTO "CM201_blocks_io" VALUES('BC104','Ti',1,'AT101_2.To','304.603673321952');
INSERT INTO "CM201_blocks_io" VALUES('BC104','Fo',0,'','49.2797268322318');
INSERT INTO "CM201_blocks_io" VALUES('BC104','Po',0,'','24.8381138045763');
INSERT INTO "CM201_blocks_io" VALUES('BC104','To',0,'','304.468867336246');
INSERT INTO "CM201_blocks_io" VALUES('BC104','So',0,'','0.03');
INSERT INTO "CM201_blocks_io" VALUES('BC104','lo',0,'','100');
INSERT INTO "CM201_blocks_io" VALUES('BC104','S_kl1',0,'','0.03');
INSERT INTO "CM201_blocks_io" VALUES('BC104','S_kl2',0,'','0.01');
INSERT INTO "CM201_blocks_io" VALUES('BC104','l_kl1',1,'G_BC104.pos','100');
INSERT INTO "CM201_blocks_io" VALUES('BC104','l_kl2',1,'G_BC105.pos','0');
INSERT INTO "CM201_blocks_io" VALUES('BC104','Q0',0,'','1');
INSERT INTO "CM201_blocks_io" VALUES('BC104','Kln',0,'','1');
INSERT INTO "CM201_blocks_io" VALUES('BC104','Kpr',0,'','0.95');
INSERT INTO "CM201_blocks_io" VALUES('BC104','Ct',0,'','20');
INSERT INTO "CM201_blocks_io" VALUES('BC104','Riz',0,'','20');
INSERT INTO "CM201_blocks_io" VALUES('BC104','noBack',0,'','1');
INSERT INTO "CM201_blocks_io" VALUES('BC104','Fwind',0,'','1');
INSERT INTO "CM201_blocks_io" VALUES('BC104','Twind',0,'','273');
INSERT INTO "CM201_blocks_io" VALUES('BC104','f_frq',0,'','100');
INSERT INTO "CM201_blocks_io" VALUES('BC104','Pot',0,'','25.4836458871784');
INSERT INTO "CM201_blocks_io" VALUES('BC104','Fit',0,'','34.9108817224733');
INSERT INTO "CM201_blocks_io" VALUES('BC106','Fi',0,'','0');
INSERT INTO "CM201_blocks_io" VALUES('BC106','Pi',1,'node4_1.Po1','25.3898494521474');
INSERT INTO "CM201_blocks_io" VALUES('BC106','Ti',1,'AT101_2.To','304.603673321952');
INSERT INTO "CM201_blocks_io" VALUES('BC106','Fo',1,'DR100.Fi','0.00188649544894555');
INSERT INTO "CM201_blocks_io" VALUES('BC106','Po',0,'','5.48536869861613');
INSERT INTO "CM201_blocks_io" VALUES('BC106','To',0,'','273');
INSERT INTO "CM201_blocks_io" VALUES('BC106','So',0,'','0.0314');
INSERT INTO "CM201_blocks_io" VALUES('BC106','lo',0,'','10');
INSERT INTO "CM201_blocks_io" VALUES('BC106','S_kl1',0,'','0.0314');
INSERT INTO "CM201_blocks_io" VALUES('BC106','S_kl2',0,'','0.01');
INSERT INTO "CM201_blocks_io" VALUES('BC106','l_kl1',1,'G_BC106.pos','0');
INSERT INTO "CM201_blocks_io" VALUES('BC106','l_kl2',0,'','0');
INSERT INTO "CM201_blocks_io" VALUES('BC106','Q0',0,'','1');
INSERT INTO "CM201_blocks_io" VALUES('BC106','Kln',0,'','1');
INSERT INTO "CM201_blocks_io" VALUES('BC106','Kpr',0,'','0.95');
INSERT INTO "CM201_blocks_io" VALUES('BC106','Ct',0,'','20');
INSERT INTO "CM201_blocks_io" VALUES('BC106','Riz',0,'','20');
INSERT INTO "CM201_blocks_io" VALUES('BC106','noBack',0,'','0');
INSERT INTO "CM201_blocks_io" VALUES('BC106','Fwind',0,'','1');
INSERT INTO "CM201_blocks_io" VALUES('BC106','Twind',0,'','273');
INSERT INTO "CM201_blocks_io" VALUES('BC106','f_frq',0,'','100');
INSERT INTO "CM201_blocks_io" VALUES('BC106','Pot',0,'','4.85858532034619');
INSERT INTO "CM201_blocks_io" VALUES('BC106','Fit',0,'','0');
INSERT INTO "CM201_blocks_io" VALUES('C101_1','Fi',0,'','50.7532406292233');
INSERT INTO "CM201_blocks_io" VALUES('C101_1','Pi',1,'node3_1.Po','5.48537314554028');
INSERT INTO "CM201_blocks_io" VALUES('C101_1','Si',0,'','0.2');
INSERT INTO "CM201_blocks_io" VALUES('C101_1','Fo',1,'PP101.Fi','50.2513931414347');
INSERT INTO "CM201_blocks_io" VALUES('C101_1','Po',0,'','5.45242131882191');
INSERT INTO "CM201_blocks_io" VALUES('C101_1','So',0,'','0.2');
INSERT INTO "CM201_blocks_io" VALUES('C101_1','lo',0,'','40');
INSERT INTO "CM201_blocks_io" VALUES('C101_1','Fo_ж',0,'','0');
INSERT INTO "CM201_blocks_io" VALUES('C101_1','Po_ж',0,'','5.49443225748623');
INSERT INTO "CM201_blocks_io" VALUES('C101_1','Lж',0,'','4.4222040699282');
INSERT INTO "CM201_blocks_io" VALUES('C101_1','ProcЖ',0,'','0.01');
INSERT INTO "CM201_blocks_io" VALUES('C101_1','Vap',0,'','9.5');
INSERT INTO "CM201_blocks_io" VALUES('C101_1','Q0',0,'','1');
INSERT INTO "CM201_blocks_io" VALUES('C101_1','Qж',0,'','1000');
INSERT INTO "CM201_blocks_io" VALUES('C101_1','f_frq',0,'','100');
INSERT INTO "CM201_blocks_io" VALUES('C101_1','Pot',0,'','5.1709458096161');
INSERT INTO "CM201_blocks_io" VALUES('C101_1','Fit',0,'','37.7441933839702');
INSERT INTO "CM201_blocks_io" VALUES('C101_2','Fi',0,'','50.2574972838313');
INSERT INTO "CM201_blocks_io" VALUES('C101_2','Pi',1,'AT101_1.Po','10.1689329888187');
INSERT INTO "CM201_blocks_io" VALUES('C101_2','Si',0,'','0.071');
INSERT INTO "CM201_blocks_io" VALUES('C101_2','Fo',1,'PP102.Fi','49.7593221014751');
INSERT INTO "CM201_blocks_io" VALUES('C101_2','Po',0,'','10.0329301847781');
INSERT INTO "CM201_blocks_io" VALUES('C101_2','So',0,'','0.071');
INSERT INTO "CM201_blocks_io" VALUES('C101_2','lo',0,'','50');
INSERT INTO "CM201_blocks_io" VALUES('C101_2','Fo_ж',0,'','0');
INSERT INTO "CM201_blocks_io" VALUES('C101_2','Po_ж',0,'','10.0740601885689');
INSERT INTO "CM201_blocks_io" VALUES('C101_2','Lж',0,'','4.32947408324947');
INSERT INTO "CM201_blocks_io" VALUES('C101_2','ProcЖ',0,'','0.01');
INSERT INTO "CM201_blocks_io" VALUES('C101_2','Vap',0,'','9.5');
INSERT INTO "CM201_blocks_io" VALUES('C101_2','Q0',0,'','1');
INSERT INTO "CM201_blocks_io" VALUES('C101_2','Qж',0,'','1000');
INSERT INTO "CM201_blocks_io" VALUES('C101_2','f_frq',0,'','100');
INSERT INTO "CM201_blocks_io" VALUES('C101_2','Pot',0,'','10.3349732470663');
INSERT INTO "CM201_blocks_io" VALUES('C101_2','Fit',0,'','37.3626857881806');
INSERT INTO "CM201_blocks_io" VALUES('C101_3','Fi',0,'','49.7660254468534');
INSERT INTO "CM201_blocks_io" VALUES('C101_3','Pi',1,'AT101_2.Po','25.6498328698296');
INSERT INTO "CM201_blocks_io" VALUES('C101_3','Si',0,'','0.0314');
INSERT INTO "CM201_blocks_io" VALUES('C101_3','Fo',1,'node4_1.Fi','49.2691075548281');
INSERT INTO "CM201_blocks_io" VALUES('C101_3','Po',0,'','25.3898579360166');
INSERT INTO "CM201_blocks_io" VALUES('C101_3','So',0,'','0.0314');
INSERT INTO "CM201_blocks_io" VALUES('C101_3','lo',0,'','8');
INSERT INTO "CM201_blocks_io" VALUES('C101_3','Fo_ж',0,'','0');
INSERT INTO "CM201_blocks_io" VALUES('C101_3','Po_ж',0,'','25.4295224540598');
INSERT INTO "CM201_blocks_io" VALUES('C101_3','Lж',0,'','13.2215060144054');
INSERT INTO "CM201_blocks_io" VALUES('C101_3','ProcЖ',0,'','0.01');
INSERT INTO "CM201_blocks_io" VALUES('C101_3','Vap',0,'','3');
INSERT INTO "CM201_blocks_io" VALUES('C101_3','Q0',0,'','1');
INSERT INTO "CM201_blocks_io" VALUES('C101_3','Qж',0,'','1000');
INSERT INTO "CM201_blocks_io" VALUES('C101_3','f_frq',0,'','100');
INSERT INTO "CM201_blocks_io" VALUES('C101_3','Pot',0,'','26.3987476146491');
INSERT INTO "CM201_blocks_io" VALUES('C101_3','Fit',0,'','37.3023338053042');
INSERT INTO "CM201_blocks_io" VALUES('PP101','Fi',0,'','50.2513521182994');
INSERT INTO "CM201_blocks_io" VALUES('PP101','Pi',1,'C101_1.Po','5.45241929760278');
INSERT INTO "CM201_blocks_io" VALUES('PP101','Fo',1,'CM101_1.Fi','50.2523365736044');
INSERT INTO "CM201_blocks_io" VALUES('PP101','Po',0,'','5.39786699873713');
INSERT INTO "CM201_blocks_io" VALUES('PP101','dP',0,'','5.45544209502965');
INSERT INTO "CM201_blocks_io" VALUES('PP101','Sdf',0,'','0.15');
INSERT INTO "CM201_blocks_io" VALUES('PP101','So',0,'','0.2');
INSERT INTO "CM201_blocks_io" VALUES('PP101','lo',0,'','7');
INSERT INTO "CM201_blocks_io" VALUES('PP101','Q0',0,'','1');
INSERT INTO "CM201_blocks_io" VALUES('PP101','f_frq',0,'','100');
INSERT INTO "CM201_blocks_io" VALUES('PP101','Pot',0,'','5.05761619736924');
INSERT INTO "CM201_blocks_io" VALUES('PP101','Fit',0,'','37.7386147480736');
INSERT INTO "CM201_blocks_io" VALUES('PP102','Fi',0,'','49.7592878394635');
INSERT INTO "CM201_blocks_io" VALUES('PP102','Pi',1,'C101_2.Po','10.0329266617318');
INSERT INTO "CM201_blocks_io" VALUES('PP102','Fo',1,'CM101_2.Fi','49.7598946514195');
INSERT INTO "CM201_blocks_io" VALUES('PP102','Po',0,'','9.8510739185684');
INSERT INTO "CM201_blocks_io" VALUES('PP102','dP',0,'','18.1858535201479');
INSERT INTO "CM201_blocks_io" VALUES('PP102','Sdf',0,'','0.06');
INSERT INTO "CM201_blocks_io" VALUES('PP102','So',0,'','0.071');
INSERT INTO "CM201_blocks_io" VALUES('PP102','lo',0,'','7');
INSERT INTO "CM201_blocks_io" VALUES('PP102','Q0',0,'','1');
INSERT INTO "CM201_blocks_io" VALUES('PP102','f_frq',0,'','100');
INSERT INTO "CM201_blocks_io" VALUES('PP102','Pot',0,'','9.98549407979398');
INSERT INTO "CM201_blocks_io" VALUES('PP102','Fit',0,'','37.3637648351936');
INSERT INTO "CM201_blocks_io" VALUES('G_BC104','pos',0,'','100');
INSERT INTO "CM201_blocks_io" VALUES('CM101_1','Kpr',0,'','0.95');
INSERT INTO "CM201_blocks_io" VALUES('CM101_2','Kpr',0,'','0.95');
INSERT INTO "CM201_blocks_io" VALUES('KPP101','t_kl1',0,'','3');
INSERT INTO "CM201_blocks_io" VALUES('KPP101','t_kl2',0,'','5');
INSERT INTO "CM201_blocks_io" VALUES('KPP101','tmp_l1',0,'','7.4109846876187e-322');
INSERT INTO "CM201_blocks_io" VALUES('KPP101','tmp_l2',0,'','0');
INSERT INTO "CM201_blocks_io" VALUES('DR100','t_kl1',0,'','10');
INSERT INTO "CM201_blocks_io" VALUES('DR100','t_kl2',0,'','5');
INSERT INTO "CM201_blocks_io" VALUES('DR100','tmp_l1',0,'','49.9999999999964');
INSERT INTO "CM201_blocks_io" VALUES('DR100','tmp_l2',0,'','0');
INSERT INTO "CM201_blocks_io" VALUES('node3_1','Kpr',0,'','0.95');
INSERT INTO "CM201_blocks_io" VALUES('BC101','t_kl1',0,'','10');
INSERT INTO "CM201_blocks_io" VALUES('BC101','t_kl2',0,'','5');
INSERT INTO "CM201_blocks_io" VALUES('BC101','tmp_l1',0,'','99.9999999999929');
INSERT INTO "CM201_blocks_io" VALUES('BC101','tmp_l2',0,'','1.23516411460312e-321');
INSERT INTO "CM201_blocks_io" VALUES('BC104','t_kl1',0,'','10');
INSERT INTO "CM201_blocks_io" VALUES('BC104','t_kl2',0,'','5');
INSERT INTO "CM201_blocks_io" VALUES('BC104','tmp_l1',0,'','99.9999999999929');
INSERT INTO "CM201_blocks_io" VALUES('BC104','tmp_l2',0,'','0');
INSERT INTO "CM201_blocks_io" VALUES('node4_1','Kpr',0,'','0.95');
INSERT INTO "CM201_blocks_io" VALUES('BC106','t_kl1',0,'','10');
INSERT INTO "CM201_blocks_io" VALUES('BC106','t_kl2',0,'','5');
INSERT INTO "CM201_blocks_io" VALUES('BC106','tmp_l1',0,'','0');
INSERT INTO "CM201_blocks_io" VALUES('BC106','tmp_l2',0,'','0');
INSERT INTO "CM201_blocks_io" VALUES('PP101','Kpr',0,'','0.95');
INSERT INTO "CM201_blocks_io" VALUES('PP102','Kpr',0,'','0.95');
INSERT INTO "CM201_blocks_io" VALUES('G_BC104','com',0,'','1');
INSERT INTO "CM201_blocks_io" VALUES('G_BC104','st_open',0,'','1');
INSERT INTO "CM201_blocks_io" VALUES('G_BC104','st_close',0,'','0');
INSERT INTO "CM201_blocks_io" VALUES('G_BC104','t_full',0,'','5');
INSERT INTO "CM201_blocks_io" VALUES('G_BC104','t_up',0,'','0.5');
INSERT INTO "CM201_blocks_io" VALUES('G_BC104','f_frq',0,'','100');
INSERT INTO "CM201_blocks_io" VALUES('G_BC104','tmp_up',0,'','0');
INSERT INTO "CM201_blocks_io" VALUES('G_BC104','lst_com',0,'','1');
INSERT INTO "CM201_blocks_io" VALUES('G_BC105','pos',0,'','0');
INSERT INTO "CM201_blocks_io" VALUES('G_BC105','com',0,'','0');
INSERT INTO "CM201_blocks_io" VALUES('G_BC105','st_open',0,'','0');
INSERT INTO "CM201_blocks_io" VALUES('G_BC105','st_close',0,'','1');
INSERT INTO "CM201_blocks_io" VALUES('G_BC105','t_full',0,'','5');
INSERT INTO "CM201_blocks_io" VALUES('G_BC105','t_up',0,'','0.5');
INSERT INTO "CM201_blocks_io" VALUES('G_BC105','f_frq',0,'','100');
INSERT INTO "CM201_blocks_io" VALUES('G_BC105','tmp_up',0,'','0');
INSERT INTO "CM201_blocks_io" VALUES('G_BC105','lst_com',0,'','0');
INSERT INTO "CM201_blocks_io" VALUES('G_BC101','pos',0,'','100');
INSERT INTO "CM201_blocks_io" VALUES('G_BC101','com',0,'','1');
INSERT INTO "CM201_blocks_io" VALUES('G_BC101','st_open',0,'','1');
INSERT INTO "CM201_blocks_io" VALUES('G_BC101','st_close',0,'','0');
INSERT INTO "CM201_blocks_io" VALUES('G_BC101','t_full',0,'','5');
INSERT INTO "CM201_blocks_io" VALUES('G_BC101','t_up',0,'','0.5');
INSERT INTO "CM201_blocks_io" VALUES('G_BC101','f_frq',0,'','100');
INSERT INTO "CM201_blocks_io" VALUES('G_BC101','tmp_up',0,'','0');
INSERT INTO "CM201_blocks_io" VALUES('G_BC101','lst_com',0,'','1');
INSERT INTO "CM201_blocks_io" VALUES('G_BC106','pos',0,'','0');
INSERT INTO "CM201_blocks_io" VALUES('G_BC106','com',0,'','0');
INSERT INTO "CM201_blocks_io" VALUES('G_BC106','st_open',0,'','0');
INSERT INTO "CM201_blocks_io" VALUES('G_BC106','st_close',0,'','1');
INSERT INTO "CM201_blocks_io" VALUES('G_BC106','t_full',0,'','5');
INSERT INTO "CM201_blocks_io" VALUES('G_BC106','t_up',0,'','0.5');
INSERT INTO "CM201_blocks_io" VALUES('G_BC106','f_frq',0,'','100');
INSERT INTO "CM201_blocks_io" VALUES('G_BC106','tmp_up',0,'','0');
INSERT INTO "CM201_blocks_io" VALUES('G_BC106','lst_com',0,'','0');
INSERT INTO "CM201_blocks_io" VALUES('G_BC102','pos',0,'','0');
INSERT INTO "CM201_blocks_io" VALUES('G_BC102','com',0,'','0');
INSERT INTO "CM201_blocks_io" VALUES('G_BC102','st_open',0,'','0');
INSERT INTO "CM201_blocks_io" VALUES('G_BC102','st_close',0,'','1');
INSERT INTO "CM201_blocks_io" VALUES('G_BC102','t_full',0,'','3');
INSERT INTO "CM201_blocks_io" VALUES('G_BC102','t_up',0,'','0.3');
INSERT INTO "CM201_blocks_io" VALUES('G_BC102','f_frq',0,'','100');
INSERT INTO "CM201_blocks_io" VALUES('G_BC102','tmp_up',0,'','0');
INSERT INTO "CM201_blocks_io" VALUES('G_BC102','lst_com',0,'','0');
CREATE TABLE 'CM201_prm' ("SHIFR" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '' ,"IO" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"ru#IO" TEXT DEFAULT '' ,"uk#IO" TEXT DEFAULT '' , PRIMARY KEY ("SHIFR"));
INSERT INTO "CM201_prm" VALUES('FN101','FN201','Flow at the output of compressor',1,'BC104.Fi:var:Variable','','Витрати на виході компресора','','Расход на выходе компрессора','BC104.Fi:var:Переменная','BC104.Fi:var:Змінна');
INSERT INTO "CM201_prm" VALUES('F101','F201','Flow on the diaphragm PP201',1,'PP101.Fi:var:Variable
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
INSERT INTO "CM201_prm" VALUES('F102','F202','Flow on the diaphragm PP202',1,'PP102.Fi:var:Variable
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
INSERT INTO "CM201_prm" VALUES('TE1313_1','TE1313_2','Temperature at the input of the first stage of the compressor',1,'node3_1.To:var:Variable
*s.К:ed:Dimension
*i.0:prec:Precision
*r.273:min:Minimum
*r.373:max:Maximum','','Температура на вході першого ступеня компресору','','Температура на входе первой ступени компрессора','node3_1.To:var:Переменная
*s.К:ed:Ед.измерения
*i.0:prec:Точность
*r.273:min:Минимум
*r.373:max:Максимум','node3_1.To:var:Змінна
*s.К:ed:Од.виміру
*i.0:prec:Точність
*r.273:min:Мінімум
*r.373:max:Максимум');
INSERT INTO "CM201_prm" VALUES('PT0202','PT0202_2','Pressure after the first stage of the compressor',1,'CM101_1.Po:var:Variable
*s.at:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.20:max:Maximum','','Тиск після першої ступені компресору','','Давление после первой ступени компрессора','CM101_1.Po:var:Переменная
*s.ат:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.20:max:Максимум','CM101_1.Po:var:Змінна
*s.ат:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.20:max:Максимум');
INSERT INTO "CM201_prm" VALUES('PT0204','PT0204_2','Pressure after the second stage of the compressor',1,'CM101_2.Po:var:Variable
*s.at:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.50:max:Maximum','','Тиск після другого ступеня компресору','','Давление после второй ступени компрессора','CM101_2.Po:var:Переменная
*s.ат:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.50:max:Максимум','CM101_2.Po:var:Змінна
*s.ат:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.50:max:Максимум');
INSERT INTO "CM201_prm" VALUES('TE1314_1','TE1314_2','Temperature after the fridge of the first stage of the compressor',1,'AT101_1.To:var:Variable
*s.К:ed:Dimension
*i.0:prec:Precision
*r.273:min:Minimum
*r.373:max:Maximum','','Температура після холодильника першого ступеня компресору','','Температура после холодильника первой ступени компрессора','AT101_1.To:var:Переменная
*s.К:ed:Ед.измерения
*i.0:prec:Точность
*r.273:min:Минимум
*r.373:max:Максимум','AT101_1.To:var:Змінна
*s.К:ed:Од.виміру
*i.0:prec:Точність
*r.273:min:Мінімум
*r.373:max:Максимум');
INSERT INTO "CM201_prm" VALUES('PT1006_1','PT1006_2','Pressure on the diaphragm PP202',1,'PP102.Pi:var:Variable
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
INSERT INTO "CM201_prm" VALUES('ST8612','ST8612_2','Rotation speed of the compressor',1,'CM101_1.N:var:Variable
CM101_1.N:varIn:Variable input
*s.1000x rpm:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.10:max:Maximum','','Оберти компресору','','Обороты вращения компрессора','CM101_1.N:var:Переменная
CM101_1.N:varIn:Ввод переменной
*s.1000x об.мин.:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.10:max:Максимум','CM101_1.N:var:Змінна
CM101_1.N:varIn:Ввід змінної
*s.1000x об.хвил:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.10:max:Максимум');
INSERT INTO "CM201_prm" VALUES('KPP101','KPP201','Anti-surge control valve of the compressor',1,'KPP101.l_kl1:out:Output
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
INSERT INTO "CM201_prm" VALUES('TT0202','TE1202_2','Temperature after the first stage of the compressor',1,'CM101_1.To:var:Variable
*s.К:ed:Dimension
*i.0:prec:Precision
*r.273:min:Minimum
*r.373:max:Maximum','','Температура після першої ступені компресора','','Температура после первой ступени компрессора','CM101_1.To:var:Переменная
*s.К:ed:Ед.измерения
*i.0:prec:Точность
*r.273:min:Минимум
*r.373:max:Максимум','CM101_1.To:var:Змінна
*s.К:ed:Од.виміру
*i.0:prec:Точність
*r.273:min:Мінімум
*r.373:max:Максимум');
INSERT INTO "CM201_prm" VALUES('TT0204','TE1205_2','Temperature after the second stage of the compressor',1,'CM101_2.To:var:Variable
*s.К:ed:Dimension
*i.0:prec:Precision
*r.273:min:Minimum
*r.433:max:Maximum','','Температура після другої ступені компресора','','Температура после второй ступени компрессора','CM101_2.To:var:Переменная
*s.К:ed:Ед.измерения
*i.0:prec:Точность
*r.273:min:Минимум
*r.433:max:Максимум','CM101_2.To:var:Змінна
*s.К:ed:Од.виміру
*i.0:prec:Точність
*r.273:min:Мінімум
*r.433:max:Максимум');
INSERT INTO "CM201_prm" VALUES('AT101_1','AT201_1','Control block of the fridge after the first stage of the compressor',1,'AT101_1.Ti:Ti
AT101_1.To:To
AT101_1.Wc:Wc','','Блок контролю холодильника після першої ступені компресору','','Блок контроля холодильника после первой ступени компрессора','','');
INSERT INTO "CM201_prm" VALUES('AT101_2','AT201_2','Control block of the fridge after the second stage of the compressor',1,'AT101_2.Ti:Ti
AT101_2.To:To
AT101_2.Wc:Wc','','Блок контролю холодильника після другої ступені компресору','','Блок контроля холодильника после второй ступени компрессора','','');
INSERT INTO "CM201_prm" VALUES('BC101','BC201','Control block of the ball crane BC201',1,'G_BC101.com:com
G_BC101.st_open:st_open
G_BC101.st_close:st_close','КК201','Блок керування кульовим краном КК201','КШ201','Блок управления шарового крана КШ201','','');
INSERT INTO "CM201_prm" VALUES('BC102','BC202','Control block of the ball crane BC202',1,'G_BC102.com:com
G_BC102.st_open:st_open
G_BC102.st_close:st_close','КК202','Блок керування кульовим краном КК202','КШ202','Блок управления шарового крана КШ202','','');
INSERT INTO "CM201_prm" VALUES('BC104','BC204','Control block of the ball crane BC204',1,'G_BC104.com:com
G_BC104.st_open:st_open
G_BC104.st_close:st_close','КК204','Блок керування кульовим краном КК204','КШ204','Блок управления шарового крана КШ204','','');
INSERT INTO "CM201_prm" VALUES('BC105','BC205','Control block of the ball crane BC205',1,'G_BC105.com:com
G_BC105.st_open:st_open
G_BC105.st_close:st_close','КК205','Блок керування кульовим краном КК205','КШ205','Блок управления шарового крана КШ205','','');
INSERT INTO "CM201_prm" VALUES('BC106','BC206','Control block of the ball crane BC206',1,'G_BC106.com:com
G_BC106.st_open:st_open
G_BC106.st_close:st_close','КК206','Блок керування кульовим краном КК206','КШ206','Блок управления шарового крана КШ206','','');
INSERT INTO "CM201_prm" VALUES('TE1314_2','TE1206_2','Temperature after the fridge of the second stage of the compressor',1,'AT101_2.To:var:Variable
*s.К:ed:Dimension
*i.0:prec:Precision
*r.273:min:Minimum
*r.373:max:Maximum','','Температура після холодильника другого ступеня компресору','','Температура после холодильника второй ступени компрессора','AT101_2.To:var:Переменная
*s.К:ed:Ед.измерения
*i.0:prec:Точность
*r.273:min:Минимум
*r.373:max:Максимум','AT101_2.To:var:Змінна
*s.К:ed:Од.виміру
*i.0:prec:Точність
*r.273:min:Мінімум
*r.373:max:Максимум');
INSERT INTO "CM201_prm" VALUES('P101','P201','Pressure on the diaphragm PP201',1,'PP101.Pi:var:Variable
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
CREATE TABLE 'CM202_blocks_io' ("BLK_ID" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"TLNK" INTEGER DEFAULT '' ,"LNK" TEXT DEFAULT '' ,"VAL" TEXT DEFAULT '' , PRIMARY KEY ("BLK_ID","ID"));
INSERT INTO "CM202_blocks_io" VALUES('KPP102','Fi',0,'','7.81655595473469e-219');
INSERT INTO "CM202_blocks_io" VALUES('KPP102','Pi',1,'node6_1.Po2','23.7037806160893');
INSERT INTO "CM202_blocks_io" VALUES('KPP102','Ti',1,'AT102_2.To','283.000000171636');
INSERT INTO "CM202_blocks_io" VALUES('KPP102','Fo',1,'node5_1.Fi2','0');
INSERT INTO "CM202_blocks_io" VALUES('KPP102','Po',0,'','23.7039729602121');
INSERT INTO "CM202_blocks_io" VALUES('KPP102','To',0,'','273.000002289451');
INSERT INTO "CM202_blocks_io" VALUES('KPP102','So',0,'','0.017');
INSERT INTO "CM202_blocks_io" VALUES('KPP102','lo',0,'','10');
INSERT INTO "CM202_blocks_io" VALUES('KPP102','S_kl1',0,'','0.015');
INSERT INTO "CM202_blocks_io" VALUES('KPP102','S_kl2',0,'','0.015');
INSERT INTO "CM202_blocks_io" VALUES('KPP102','l_kl1',2,'gen_cntr.SurgeCM202.out','0');
INSERT INTO "CM202_blocks_io" VALUES('KPP102','l_kl2',0,'','0');
INSERT INTO "CM202_blocks_io" VALUES('KPP102','Q0',0,'','1');
INSERT INTO "CM202_blocks_io" VALUES('KPP102','Kln',0,'','2');
INSERT INTO "CM202_blocks_io" VALUES('KPP102','Kpr',0,'','0.95');
INSERT INTO "CM202_blocks_io" VALUES('KPP102','Ct',0,'','20');
INSERT INTO "CM202_blocks_io" VALUES('KPP102','Riz',0,'','20');
INSERT INTO "CM202_blocks_io" VALUES('KPP102','noBack',0,'','0');
INSERT INTO "CM202_blocks_io" VALUES('KPP102','Fwind',0,'','1');
INSERT INTO "CM202_blocks_io" VALUES('KPP102','Twind',0,'','273');
INSERT INTO "CM202_blocks_io" VALUES('KPP102','f_frq',0,'','100');
INSERT INTO "CM202_blocks_io" VALUES('KPP102','Pot',0,'','24.7062304532602');
INSERT INTO "CM202_blocks_io" VALUES('KPP102','Fit',0,'','-0.147428821940688');
INSERT INTO "CM202_blocks_io" VALUES('net102','Fi',0,'','6.60068488078532e-10');
INSERT INTO "CM202_blocks_io" VALUES('net102','Pi',1,'BC113.Po','1.00000001195011');
INSERT INTO "CM202_blocks_io" VALUES('net102','Po',0,'','1');
INSERT INTO "CM202_blocks_io" VALUES('net102','So',0,'','0.01');
INSERT INTO "CM202_blocks_io" VALUES('net102','Kpr',0,'','0.95');
INSERT INTO "CM202_blocks_io" VALUES('net102','Noise',0,'','1');
INSERT INTO "CM202_blocks_io" VALUES('net102','Q0',0,'','1');
INSERT INTO "CM202_blocks_io" VALUES('net102','f_frq',0,'','100');
INSERT INTO "CM202_blocks_io" VALUES('DR101','Fi',0,'','5.99508204834657e-05');
INSERT INTO "CM202_blocks_io" VALUES('DR101','Pi',1,'BC116.Po','23.7038870517693');
INSERT INTO "CM202_blocks_io" VALUES('DR101','Ti',1,'BC116.To','273');
INSERT INTO "CM202_blocks_io" VALUES('DR101','Fo',1,'node5_1.Fi1','4.35301794233299e-05');
INSERT INTO "CM202_blocks_io" VALUES('DR101','Po',0,'','23.7038311144225');
INSERT INTO "CM202_blocks_io" VALUES('DR101','To',0,'','272.999999999997');
INSERT INTO "CM202_blocks_io" VALUES('DR101','So',0,'','0.0314');
INSERT INTO "CM202_blocks_io" VALUES('DR101','lo',0,'','3');
INSERT INTO "CM202_blocks_io" VALUES('DR101','S_kl1',0,'','0.00314');
INSERT INTO "CM202_blocks_io" VALUES('DR101','S_kl2',0,'','0.00314');
INSERT INTO "CM202_blocks_io" VALUES('DR101','l_kl1',0,'','50');
INSERT INTO "CM202_blocks_io" VALUES('DR101','l_kl2',0,'','0');
INSERT INTO "CM202_blocks_io" VALUES('DR101','Q0',0,'','1');
INSERT INTO "CM202_blocks_io" VALUES('DR101','Kln',0,'','1');
INSERT INTO "CM202_blocks_io" VALUES('DR101','Kpr',0,'','0.95');
INSERT INTO "CM202_blocks_io" VALUES('DR101','Ct',0,'','20');
INSERT INTO "CM202_blocks_io" VALUES('DR101','Riz',0,'','20');
INSERT INTO "CM202_blocks_io" VALUES('DR101','noBack',0,'','0');
INSERT INTO "CM202_blocks_io" VALUES('DR101','Fwind',0,'','1');
INSERT INTO "CM202_blocks_io" VALUES('DR101','Twind',0,'','273');
INSERT INTO "CM202_blocks_io" VALUES('DR101','f_frq',0,'','100');
INSERT INTO "CM202_blocks_io" VALUES('DR101','Pot',0,'','24.7009979434666');
INSERT INTO "CM202_blocks_io" VALUES('DR101','Fit',0,'','0.172987523705479');
INSERT INTO "CM202_blocks_io" VALUES('С102_1','Fi',0,'','-4.76951910059132e-05');
INSERT INTO "CM202_blocks_io" VALUES('С102_1','Pi',1,'node5_1.Po','23.7038122526426');
INSERT INTO "CM202_blocks_io" VALUES('С102_1','Si',0,'','0.0314');
INSERT INTO "CM202_blocks_io" VALUES('С102_1','Fo',1,'PP103.Fi','-2.90571790120047e-05');
INSERT INTO "CM202_blocks_io" VALUES('С102_1','Po',0,'','23.7038126398924');
INSERT INTO "CM202_blocks_io" VALUES('С102_1','So',0,'','0.0314');
INSERT INTO "CM202_blocks_io" VALUES('С102_1','lo',0,'','40');
INSERT INTO "CM202_blocks_io" VALUES('С102_1','Fo_ж',0,'','0');
INSERT INTO "CM202_blocks_io" VALUES('С102_1','Po_ж',0,'','23.7133479738958');
INSERT INTO "CM202_blocks_io" VALUES('С102_1','Lж',0,'','3.97305583474469');
INSERT INTO "CM202_blocks_io" VALUES('С102_1','ProcЖ',0,'','0.01');
INSERT INTO "CM202_blocks_io" VALUES('С102_1','Vap',0,'','2.4');
INSERT INTO "CM202_blocks_io" VALUES('С102_1','Q0',0,'','1');
INSERT INTO "CM202_blocks_io" VALUES('С102_1','Qж',0,'','1000');
INSERT INTO "CM202_blocks_io" VALUES('С102_1','f_frq',0,'','100');
INSERT INTO "CM202_blocks_io" VALUES('С102_1','Pot',0,'','24.7312311757839');
INSERT INTO "CM202_blocks_io" VALUES('С102_1','Fit',0,'','0.00155800981669319');
INSERT INTO "CM202_blocks_io" VALUES('AT102_1','Fi',0,'','0.000429928851290054');
INSERT INTO "CM202_blocks_io" VALUES('AT102_1','Pi',1,'CM102_1.Po','23.7038143266504');
INSERT INTO "CM202_blocks_io" VALUES('AT102_1','Ti',1,'CM102_1.To','273.054561843042');
INSERT INTO "CM202_blocks_io" VALUES('AT102_1','Si',0,'','0.015');
INSERT INTO "CM202_blocks_io" VALUES('AT102_1','li',0,'','10');
INSERT INTO "CM202_blocks_io" VALUES('AT102_1','Fo',1,'C102_2.Fi','-1.82299810385204e-05');
INSERT INTO "CM202_blocks_io" VALUES('AT102_1','Po',0,'','23.7037920088826');
INSERT INTO "CM202_blocks_io" VALUES('AT102_1','To',0,'','283.000000018927');
INSERT INTO "CM202_blocks_io" VALUES('AT102_1','So',0,'','0.017');
INSERT INTO "CM202_blocks_io" VALUES('AT102_1','lo',0,'','2');
INSERT INTO "CM202_blocks_io" VALUES('AT102_1','Tair',0,'','283');
INSERT INTO "CM202_blocks_io" VALUES('AT102_1','Wc',0,'','200');
INSERT INTO "CM202_blocks_io" VALUES('AT102_1','Q0',0,'','1');
INSERT INTO "CM202_blocks_io" VALUES('AT102_1','Ct',0,'','100');
INSERT INTO "CM202_blocks_io" VALUES('AT102_1','Rt',0,'','1');
INSERT INTO "CM202_blocks_io" VALUES('AT102_1','f_frq',0,'','100');
INSERT INTO "CM202_blocks_io" VALUES('AT102_1','Pot',0,'','24.732442399371');
INSERT INTO "CM202_blocks_io" VALUES('AT102_1','Fit',0,'','-0.0890221563119908');
INSERT INTO "CM202_blocks_io" VALUES('AT102_2','Fi',0,'','-0.000433519876588437');
INSERT INTO "CM202_blocks_io" VALUES('AT102_2','Pi',1,'CM102_2.Po','23.7038091374195');
INSERT INTO "CM202_blocks_io" VALUES('AT102_2','Ti',1,'CM102_2.To','274.173388680918');
INSERT INTO "CM202_blocks_io" VALUES('AT102_2','Si',0,'','0.007');
INSERT INTO "CM202_blocks_io" VALUES('AT102_2','li',0,'','20');
INSERT INTO "CM202_blocks_io" VALUES('AT102_2','Fo',1,'C102_3.Fi','5.85622265134582e-05');
INSERT INTO "CM202_blocks_io" VALUES('AT102_2','Po',0,'','23.7038323313878');
INSERT INTO "CM202_blocks_io" VALUES('AT102_2','To',0,'','282.999999955612');
INSERT INTO "CM202_blocks_io" VALUES('AT102_2','So',0,'','0.00785');
INSERT INTO "CM202_blocks_io" VALUES('AT102_2','lo',0,'','5');
INSERT INTO "CM202_blocks_io" VALUES('AT102_2','Tair',0,'','283');
INSERT INTO "CM202_blocks_io" VALUES('AT102_2','Wc',0,'','200');
INSERT INTO "CM202_blocks_io" VALUES('AT102_2','Q0',0,'','1');
INSERT INTO "CM202_blocks_io" VALUES('AT102_2','Ct',0,'','100');
INSERT INTO "CM202_blocks_io" VALUES('AT102_2','Rt',0,'','1');
INSERT INTO "CM202_blocks_io" VALUES('AT102_2','f_frq',0,'','100');
INSERT INTO "CM202_blocks_io" VALUES('AT102_2','Pot',0,'','24.7477290110479');
INSERT INTO "CM202_blocks_io" VALUES('AT102_2','Fit',0,'','0.255407015091622');
INSERT INTO "CM202_blocks_io" VALUES('BC111','Fi',0,'','-1.23516411460312e-321');
INSERT INTO "CM202_blocks_io" VALUES('BC111','Pi',0,'','23.7483178780194');
INSERT INTO "CM202_blocks_io" VALUES('BC111','Ti',2,'gen.BC21.To','305.049379873855');
INSERT INTO "CM202_blocks_io" VALUES('BC111','Fo',1,'node5_1.Fi3','-3.78597707052524e-05');
INSERT INTO "CM202_blocks_io" VALUES('BC111','Po',0,'','23.7038199342454');
INSERT INTO "CM202_blocks_io" VALUES('BC111','To',0,'','273.000000001701');
INSERT INTO "CM202_blocks_io" VALUES('BC111','So',0,'','0.0314');
INSERT INTO "CM202_blocks_io" VALUES('BC111','lo',0,'','5');
INSERT INTO "CM202_blocks_io" VALUES('BC111','S_kl1',0,'','0.0314');
INSERT INTO "CM202_blocks_io" VALUES('BC111','S_kl2',0,'','0.002');
INSERT INTO "CM202_blocks_io" VALUES('BC111','l_kl1',1,'G_BC111.pos','0');
INSERT INTO "CM202_blocks_io" VALUES('BC111','l_kl2',1,'G_BC112.pos','0');
INSERT INTO "CM202_blocks_io" VALUES('BC111','Q0',0,'','1');
INSERT INTO "CM202_blocks_io" VALUES('BC111','Kln',0,'','1');
INSERT INTO "CM202_blocks_io" VALUES('BC111','Kpr',0,'','0.95');
INSERT INTO "CM202_blocks_io" VALUES('BC111','Ct',0,'','20');
INSERT INTO "CM202_blocks_io" VALUES('BC111','Riz',0,'','20');
INSERT INTO "CM202_blocks_io" VALUES('BC111','noBack',0,'','0');
INSERT INTO "CM202_blocks_io" VALUES('BC111','Fwind',0,'','1');
INSERT INTO "CM202_blocks_io" VALUES('BC111','Twind',0,'','273');
INSERT INTO "CM202_blocks_io" VALUES('BC111','f_frq',0,'','100');
INSERT INTO "CM202_blocks_io" VALUES('BC111','Pot',0,'','24.7253558478491');
INSERT INTO "CM202_blocks_io" VALUES('BC111','Fit',0,'','0');
INSERT INTO "CM202_blocks_io" VALUES('BC113','Fi',0,'','0');
INSERT INTO "CM202_blocks_io" VALUES('BC113','Pi',1,'node6_1.Po4','23.703841808686');
INSERT INTO "CM202_blocks_io" VALUES('BC113','Ti',0,'','273');
INSERT INTO "CM202_blocks_io" VALUES('BC113','Fo',1,'net102.Fi','6.59472086779765e-07');
INSERT INTO "CM202_blocks_io" VALUES('BC113','Po',0,'','0.999999988049899');
INSERT INTO "CM202_blocks_io" VALUES('BC113','To',0,'','272.999999998993');
INSERT INTO "CM202_blocks_io" VALUES('BC113','So',0,'','0.00785');
INSERT INTO "CM202_blocks_io" VALUES('BC113','lo',0,'','10');
INSERT INTO "CM202_blocks_io" VALUES('BC113','S_kl1',0,'','0.00785');
INSERT INTO "CM202_blocks_io" VALUES('BC113','S_kl2',0,'','0.002');
INSERT INTO "CM202_blocks_io" VALUES('BC113','l_kl1',0,'','0');
INSERT INTO "CM202_blocks_io" VALUES('BC113','l_kl2',0,'','0');
INSERT INTO "CM202_blocks_io" VALUES('BC113','Q0',0,'','1');
INSERT INTO "CM202_blocks_io" VALUES('BC113','Kln',0,'','1');
INSERT INTO "CM202_blocks_io" VALUES('BC113','Kpr',0,'','0.95');
INSERT INTO "CM202_blocks_io" VALUES('BC113','Ct',0,'','20');
INSERT INTO "CM202_blocks_io" VALUES('BC113','Riz',0,'','20');
INSERT INTO "CM202_blocks_io" VALUES('BC113','noBack',0,'','0');
INSERT INTO "CM202_blocks_io" VALUES('BC113','Fwind',0,'','1');
INSERT INTO "CM202_blocks_io" VALUES('BC113','Twind',0,'','273');
INSERT INTO "CM202_blocks_io" VALUES('BC113','f_frq',0,'','100');
INSERT INTO "CM202_blocks_io" VALUES('BC113','Pot',0,'','1.00004217473475');
INSERT INTO "CM202_blocks_io" VALUES('BC113','Fit',0,'','0');
INSERT INTO "CM202_blocks_io" VALUES('BC114','Fi',0,'','0');
INSERT INTO "CM202_blocks_io" VALUES('BC114','Pi',1,'node6_1.Po3','23.7037456966254');
INSERT INTO "CM202_blocks_io" VALUES('BC114','Ti',1,'AT102_2.To','282.999999955612');
INSERT INTO "CM202_blocks_io" VALUES('BC114','Fo',0,'','-0.00392285829154801');
INSERT INTO "CM202_blocks_io" VALUES('BC114','Po',0,'','92.291645783093');
INSERT INTO "CM202_blocks_io" VALUES('BC114','To',0,'','273.00000001007');
INSERT INTO "CM202_blocks_io" VALUES('BC114','So',0,'','0.00785');
INSERT INTO "CM202_blocks_io" VALUES('BC114','lo',0,'','100');
INSERT INTO "CM202_blocks_io" VALUES('BC114','S_kl1',0,'','0.00785');
INSERT INTO "CM202_blocks_io" VALUES('BC114','S_kl2',0,'','0.002');
INSERT INTO "CM202_blocks_io" VALUES('BC114','l_kl1',1,'G_BC114.pos','0');
INSERT INTO "CM202_blocks_io" VALUES('BC114','l_kl2',1,'G_BC115.pos','0');
INSERT INTO "CM202_blocks_io" VALUES('BC114','Q0',0,'','1');
INSERT INTO "CM202_blocks_io" VALUES('BC114','Kln',0,'','1');
INSERT INTO "CM202_blocks_io" VALUES('BC114','Kpr',0,'','0.95');
INSERT INTO "CM202_blocks_io" VALUES('BC114','Ct',0,'','20');
INSERT INTO "CM202_blocks_io" VALUES('BC114','Riz',0,'','20');
INSERT INTO "CM202_blocks_io" VALUES('BC114','noBack',0,'','1');
INSERT INTO "CM202_blocks_io" VALUES('BC114','Fwind',0,'','1');
INSERT INTO "CM202_blocks_io" VALUES('BC114','Twind',0,'','273');
INSERT INTO "CM202_blocks_io" VALUES('BC114','f_frq',0,'','100');
INSERT INTO "CM202_blocks_io" VALUES('BC114','Pot',0,'','91.8399092307547');
INSERT INTO "CM202_blocks_io" VALUES('BC114','Fit',0,'','2.47032822920623e-322');
INSERT INTO "CM202_blocks_io" VALUES('BC116','Fi',0,'','0');
INSERT INTO "CM202_blocks_io" VALUES('BC116','Pi',1,'node6_1.Po1','23.7038138813009');
INSERT INTO "CM202_blocks_io" VALUES('BC116','Ti',1,'AT102_2.To','282.999999955612');
INSERT INTO "CM202_blocks_io" VALUES('BC116','Fo',1,'DR101.Fi','-0.00015068305395324');
INSERT INTO "CM202_blocks_io" VALUES('BC116','Po',0,'','23.7038659541817');
INSERT INTO "CM202_blocks_io" VALUES('BC116','To',0,'','273');
INSERT INTO "CM202_blocks_io" VALUES('BC116','So',0,'','0.0078');
INSERT INTO "CM202_blocks_io" VALUES('BC116','lo',0,'','1');
INSERT INTO "CM202_blocks_io" VALUES('BC116','S_kl1',0,'','0.0078');
INSERT INTO "CM202_blocks_io" VALUES('BC116','S_kl2',0,'','0.0078');
INSERT INTO "CM202_blocks_io" VALUES('BC116','l_kl1',1,'G_BC116.pos','0');
INSERT INTO "CM202_blocks_io" VALUES('BC116','l_kl2',0,'','0');
INSERT INTO "CM202_blocks_io" VALUES('BC116','Q0',0,'','1');
INSERT INTO "CM202_blocks_io" VALUES('BC116','Kln',0,'','1');
INSERT INTO "CM202_blocks_io" VALUES('BC116','Kpr',0,'','0.95');
INSERT INTO "CM202_blocks_io" VALUES('BC116','Ct',0,'','20');
INSERT INTO "CM202_blocks_io" VALUES('BC116','Riz',0,'','20');
INSERT INTO "CM202_blocks_io" VALUES('BC116','noBack',0,'','0');
INSERT INTO "CM202_blocks_io" VALUES('BC116','Fwind',0,'','1');
INSERT INTO "CM202_blocks_io" VALUES('BC116','Twind',0,'','273');
INSERT INTO "CM202_blocks_io" VALUES('BC116','f_frq',0,'','100');
INSERT INTO "CM202_blocks_io" VALUES('BC116','Pot',0,'','24.8055729734348');
INSERT INTO "CM202_blocks_io" VALUES('BC116','Fit',0,'','0');
INSERT INTO "CM202_blocks_io" VALUES('node5_1','Fi1',0,'','0.000208726119630634');
INSERT INTO "CM202_blocks_io" VALUES('node5_1','Pi1',1,'DR101.Po','23.7038318938861');
INSERT INTO "CM202_blocks_io" VALUES('node5_1','Ti1',1,'DR101.To','272.999999999997');
INSERT INTO "CM202_blocks_io" VALUES('node5_1','Si1',0,'','0.0314');
INSERT INTO "CM202_blocks_io" VALUES('node5_1','Fi2',0,'','0');
INSERT INTO "CM202_blocks_io" VALUES('node5_1','Pi2',1,'KPP102.Po','23.7039729602121');
INSERT INTO "CM202_blocks_io" VALUES('node5_1','Ti2',1,'KPP102.To','273.000002289451');
INSERT INTO "CM202_blocks_io" VALUES('node5_1','Si2',0,'','0');
INSERT INTO "CM202_blocks_io" VALUES('node5_1','Fi3',0,'','0.000113529479824857');
INSERT INTO "CM202_blocks_io" VALUES('node5_1','Pi3',1,'BC111.Po','23.7038184655346');
INSERT INTO "CM202_blocks_io" VALUES('node5_1','Ti3',1,'BC111.To','273.000000001701');
INSERT INTO "CM202_blocks_io" VALUES('node5_1','Si3',0,'','0.0314');
INSERT INTO "CM202_blocks_io" VALUES('node5_1','Fo',1,'С102_1.Fi','-3.24273312183517e-05');
INSERT INTO "CM202_blocks_io" VALUES('node5_1','Po',0,'','23.7038122526426');
INSERT INTO "CM202_blocks_io" VALUES('node5_1','To',0,'','272.999999994404');
INSERT INTO "CM202_blocks_io" VALUES('node5_1','So',0,'','0.2');
INSERT INTO "CM202_blocks_io" VALUES('node5_1','lo',0,'','50');
INSERT INTO "CM202_blocks_io" VALUES('node5_1','Q0',0,'','1');
INSERT INTO "CM202_blocks_io" VALUES('node5_1','Ct',0,'','20');
INSERT INTO "CM202_blocks_io" VALUES('node5_1','Riz',0,'','20');
INSERT INTO "CM202_blocks_io" VALUES('node5_1','Fwind',0,'','1');
INSERT INTO "CM202_blocks_io" VALUES('node5_1','Twind',0,'','273');
INSERT INTO "CM202_blocks_io" VALUES('node5_1','f_frq',0,'','100');
INSERT INTO "CM202_blocks_io" VALUES('node5_1','FoR',0,'','-10.1677478719936');
INSERT INTO "CM202_blocks_io" VALUES('node5_1','Pot',0,'','24.73125922103');
INSERT INTO "CM202_blocks_io" VALUES('node5_1','Fit',0,'','0.332954574126532');
INSERT INTO "CM202_blocks_io" VALUES('CM102_1','Fi',0,'','-1.26719288532609e-05');
INSERT INTO "CM202_blocks_io" VALUES('CM102_1','Pi',1,'PP103.Po','23.7038126343003');
INSERT INTO "CM202_blocks_io" VALUES('CM102_1','Ti',0,'','273');
INSERT INTO "CM202_blocks_io" VALUES('CM102_1','Fo',1,'AT102_1.Fi','0.00145373356004023');
INSERT INTO "CM202_blocks_io" VALUES('CM102_1','Po',0,'','23.7038084563817');
INSERT INTO "CM202_blocks_io" VALUES('CM102_1','To',0,'','273.054561835007');
INSERT INTO "CM202_blocks_io" VALUES('CM102_1','So',0,'','0.017');
INSERT INTO "CM202_blocks_io" VALUES('CM102_1','lo',0,'','55');
INSERT INTO "CM202_blocks_io" VALUES('CM102_1','Kzp',0,'','0');
INSERT INTO "CM202_blocks_io" VALUES('CM102_1','N',0,'','0');
INSERT INTO "CM202_blocks_io" VALUES('CM102_1','V',0,'','0.27');
INSERT INTO "CM202_blocks_io" VALUES('CM102_1','Kpmp',0,'','0.02');
INSERT INTO "CM202_blocks_io" VALUES('CM102_1','Kslp',0,'','0.2');
INSERT INTO "CM202_blocks_io" VALUES('CM102_1','Q0',0,'','1');
INSERT INTO "CM202_blocks_io" VALUES('CM102_1','Ct',0,'','100');
INSERT INTO "CM202_blocks_io" VALUES('CM102_1','Riz',0,'','100');
INSERT INTO "CM202_blocks_io" VALUES('CM102_1','Fwind',0,'','1');
INSERT INTO "CM202_blocks_io" VALUES('CM102_1','Twind',0,'','273');
INSERT INTO "CM202_blocks_io" VALUES('CM102_1','f_frq',0,'','100');
INSERT INTO "CM202_blocks_io" VALUES('CM102_1','Pot',0,'','24.7309314882325');
INSERT INTO "CM202_blocks_io" VALUES('CM102_1','Fit',0,'','0.00111032582841949');
INSERT INTO "CM202_blocks_io" VALUES('CM102_2','Fi',0,'','6.22836490516359e-06');
INSERT INTO "CM202_blocks_io" VALUES('CM102_2','Pi',1,'PP104.Po','23.7038124400944');
INSERT INTO "CM202_blocks_io" VALUES('CM102_2','Ti',1,'AT102_1.To','282.99999967067');
INSERT INTO "CM202_blocks_io" VALUES('CM102_2','Fo',1,'AT102_2.Fi','-0.000620845385949605');
INSERT INTO "CM202_blocks_io" VALUES('CM102_2','Po',0,'','23.7038162513105');
INSERT INTO "CM202_blocks_io" VALUES('CM102_2','To',0,'','274.173388312536');
INSERT INTO "CM202_blocks_io" VALUES('CM102_2','So',0,'','0.00785');
INSERT INTO "CM202_blocks_io" VALUES('CM102_2','lo',0,'','55');
INSERT INTO "CM202_blocks_io" VALUES('CM102_2','Kzp',0,'','0');
INSERT INTO "CM202_blocks_io" VALUES('CM102_2','N',1,'CM102_1.N','0');
INSERT INTO "CM202_blocks_io" VALUES('CM102_2','V',0,'','0.13');
INSERT INTO "CM202_blocks_io" VALUES('CM102_2','Kpmp',0,'','0.02');
INSERT INTO "CM202_blocks_io" VALUES('CM102_2','Kslp',0,'','0.2');
INSERT INTO "CM202_blocks_io" VALUES('CM102_2','Q0',0,'','1');
INSERT INTO "CM202_blocks_io" VALUES('CM102_2','Ct',0,'','100');
INSERT INTO "CM202_blocks_io" VALUES('CM102_2','Riz',0,'','100');
INSERT INTO "CM202_blocks_io" VALUES('CM102_2','Fwind',0,'','1');
INSERT INTO "CM202_blocks_io" VALUES('CM102_2','Twind',0,'','273');
INSERT INTO "CM202_blocks_io" VALUES('CM102_2','f_frq',0,'','100');
INSERT INTO "CM202_blocks_io" VALUES('CM102_2','Pot',0,'','24.7476967466407');
INSERT INTO "CM202_blocks_io" VALUES('CM102_2','Fit',0,'','-0.0315406153143646');
INSERT INTO "CM202_blocks_io" VALUES('node6_1','Fi',0,'','-7.18774588833278e-05');
INSERT INTO "CM202_blocks_io" VALUES('node6_1','Pi',1,'C102_3.Po','23.7037487975245');
INSERT INTO "CM202_blocks_io" VALUES('node6_1','Fo1',1,'BC116.Fi','0');
INSERT INTO "CM202_blocks_io" VALUES('node6_1','Po1',0,'','23.7038158241595');
INSERT INTO "CM202_blocks_io" VALUES('node6_1','So1',0,'','0.00785');
INSERT INTO "CM202_blocks_io" VALUES('node6_1','lo1',0,'','30');
INSERT INTO "CM202_blocks_io" VALUES('node6_1','Fo2',1,'KPP102.Fi','7.81655595473469e-219');
INSERT INTO "CM202_blocks_io" VALUES('node6_1','Po2',0,'','23.7037834209415');
INSERT INTO "CM202_blocks_io" VALUES('node6_1','So2',0,'','0.002');
INSERT INTO "CM202_blocks_io" VALUES('node6_1','lo2',0,'','30');
INSERT INTO "CM202_blocks_io" VALUES('node6_1','Fo3',1,'BC114.Fi','0');
INSERT INTO "CM202_blocks_io" VALUES('node6_1','Po3',0,'','23.7038181430323');
INSERT INTO "CM202_blocks_io" VALUES('node6_1','So3',0,'','0.00785');
INSERT INTO "CM202_blocks_io" VALUES('node6_1','lo3',0,'','5');
INSERT INTO "CM202_blocks_io" VALUES('node6_1','Fo4',1,'BC113.Fi','0');
INSERT INTO "CM202_blocks_io" VALUES('node6_1','Po4',0,'','23.7038382676053');
INSERT INTO "CM202_blocks_io" VALUES('node6_1','So4',0,'','0.00785');
INSERT INTO "CM202_blocks_io" VALUES('node6_1','lo4',0,'','10');
INSERT INTO "CM202_blocks_io" VALUES('node6_1','Q0',0,'','1');
INSERT INTO "CM202_blocks_io" VALUES('node6_1','f_frq',0,'','100');
INSERT INTO "CM202_blocks_io" VALUES('node6_1','F1tmp',0,'','-4.92653389877477e-06');
INSERT INTO "CM202_blocks_io" VALUES('node6_1','F2tmp',0,'','5.92135453759319e-05');
INSERT INTO "CM202_blocks_io" VALUES('node6_1','F3tmp',0,'','0.000173399457430141');
INSERT INTO "CM202_blocks_io" VALUES('node6_1','F4tmp',0,'','-0.000299563927790626');
INSERT INTO "CM202_blocks_io" VALUES('node6_1','Pot1',0,'','24.6108046249314');
INSERT INTO "CM202_blocks_io" VALUES('node6_1','Pot2',0,'','25.1285255724454');
INSERT INTO "CM202_blocks_io" VALUES('node6_1','Pot3',0,'','24.7529267833944');
INSERT INTO "CM202_blocks_io" VALUES('node6_1','Pot4',0,'','24.0365393488062');
INSERT INTO "CM202_blocks_io" VALUES('PP103','Fi',0,'','-2.90571790120047e-05');
INSERT INTO "CM202_blocks_io" VALUES('PP103','Pi',1,'С102_1.Po','23.7038126820669');
INSERT INTO "CM202_blocks_io" VALUES('PP103','Fo',1,'CM102_1.Fi','5.2592428422388e-05');
INSERT INTO "CM202_blocks_io" VALUES('PP103','Po',0,'','23.7038122148849');
INSERT INTO "CM202_blocks_io" VALUES('PP103','dP',0,'','9.87117028043016e-08');
INSERT INTO "CM202_blocks_io" VALUES('PP103','Sdf',0,'','0.02');
INSERT INTO "CM202_blocks_io" VALUES('PP103','So',0,'','0.0314');
INSERT INTO "CM202_blocks_io" VALUES('PP103','lo',0,'','7');
INSERT INTO "CM202_blocks_io" VALUES('PP103','Q0',0,'','1');
INSERT INTO "CM202_blocks_io" VALUES('PP103','f_frq',0,'','100');
INSERT INTO "CM202_blocks_io" VALUES('PP103','Pot',0,'','24.7310701017625');
INSERT INTO "CM202_blocks_io" VALUES('PP103','Fit',0,'','0.0347800008929181');
INSERT INTO "CM202_blocks_io" VALUES('PP104','Fi',0,'','9.01626434168341e-06');
INSERT INTO "CM202_blocks_io" VALUES('PP104','Pi',1,'C102_2.Po','23.7038119277592');
INSERT INTO "CM202_blocks_io" VALUES('PP104','Fo',1,'CM102_2.Fi','-2.42104414648584e-05');
INSERT INTO "CM202_blocks_io" VALUES('PP104','Po',0,'','23.7038122602504');
INSERT INTO "CM202_blocks_io" VALUES('PP104','dP',0,'','-3.11651809500868e-08');
INSERT INTO "CM202_blocks_io" VALUES('PP104','Sdf',0,'','0.015');
INSERT INTO "CM202_blocks_io" VALUES('PP104','So',0,'','0.017');
INSERT INTO "CM202_blocks_io" VALUES('PP104','lo',0,'','7');
INSERT INTO "CM202_blocks_io" VALUES('PP104','Q0',0,'','1');
INSERT INTO "CM202_blocks_io" VALUES('PP104','f_frq',0,'','100');
INSERT INTO "CM202_blocks_io" VALUES('PP104','Pot',0,'','24.7257258378967');
INSERT INTO "CM202_blocks_io" VALUES('PP104','Fit',0,'','6.10188555296873');
INSERT INTO "CM202_blocks_io" VALUES('C102_2','Fi',0,'','-6.32776734573904e-05');
INSERT INTO "CM202_blocks_io" VALUES('C102_2','Pi',1,'AT102_1.Po','23.7037920088826');
INSERT INTO "CM202_blocks_io" VALUES('C102_2','Si',0,'','0.017');
INSERT INTO "CM202_blocks_io" VALUES('C102_2','Fo',1,'PP104.Fi','8.20355385214164e-05');
INSERT INTO "CM202_blocks_io" VALUES('C102_2','Po',0,'','23.703811510417');
INSERT INTO "CM202_blocks_io" VALUES('C102_2','So',0,'','0.017');
INSERT INTO "CM202_blocks_io" VALUES('C102_2','lo',0,'','50');
INSERT INTO "CM202_blocks_io" VALUES('C102_2','Fo_ж',0,'','0');
INSERT INTO "CM202_blocks_io" VALUES('C102_2','Po_ж',0,'','23.7133725577122');
INSERT INTO "CM202_blocks_io" VALUES('C102_2','Lж',0,'','4.78052364759093');
INSERT INTO "CM202_blocks_io" VALUES('C102_2','ProcЖ',0,'','0.01');
INSERT INTO "CM202_blocks_io" VALUES('C102_2','Vap',0,'','2');
INSERT INTO "CM202_blocks_io" VALUES('C102_2','Q0',0,'','1');
INSERT INTO "CM202_blocks_io" VALUES('C102_2','Qж',0,'','1000');
INSERT INTO "CM202_blocks_io" VALUES('C102_2','f_frq',0,'','100');
INSERT INTO "CM202_blocks_io" VALUES('C102_2','Pot',0,'','24.7284580318349');
INSERT INTO "CM202_blocks_io" VALUES('C102_2','Fit',0,'','-0.0311306889521761');
INSERT INTO "CM202_blocks_io" VALUES('C102_3','Fi',0,'','9.09658897927854e-05');
INSERT INTO "CM202_blocks_io" VALUES('C102_3','Pi',1,'AT102_2.Po','23.7038323313878');
INSERT INTO "CM202_blocks_io" VALUES('C102_3','Si',0,'','0.00785');
INSERT INTO "CM202_blocks_io" VALUES('C102_3','Fo',1,'node6_1.Fi','0.000653839503413543');
INSERT INTO "CM202_blocks_io" VALUES('C102_3','Po',0,'','23.7037913283502');
INSERT INTO "CM202_blocks_io" VALUES('C102_3','So',0,'','0.00785');
INSERT INTO "CM202_blocks_io" VALUES('C102_3','lo',0,'','8');
INSERT INTO "CM202_blocks_io" VALUES('C102_3','Fo_ж',0,'','0');
INSERT INTO "CM202_blocks_io" VALUES('C102_3','Po_ж',0,'','23.714078791932');
INSERT INTO "CM202_blocks_io" VALUES('C102_3','Lж',0,'','6.85830905452556');
INSERT INTO "CM202_blocks_io" VALUES('C102_3','ProcЖ',0,'','0.01');
INSERT INTO "CM202_blocks_io" VALUES('C102_3','Vap',0,'','1.5');
INSERT INTO "CM202_blocks_io" VALUES('C102_3','Q0',0,'','1');
INSERT INTO "CM202_blocks_io" VALUES('C102_3','Qж',0,'','1000');
INSERT INTO "CM202_blocks_io" VALUES('C102_3','f_frq',0,'','100');
INSERT INTO "CM202_blocks_io" VALUES('C102_3','Pot',0,'','24.6232820831098');
INSERT INTO "CM202_blocks_io" VALUES('C102_3','Fit',0,'','0.84291905311513');
INSERT INTO "CM202_blocks_io" VALUES('KPP102','t_kl1',0,'','3');
INSERT INTO "CM202_blocks_io" VALUES('KPP102','t_kl2',0,'','5');
INSERT INTO "CM202_blocks_io" VALUES('KPP102','tmp_l1',0,'','7.4109846876187e-322');
INSERT INTO "CM202_blocks_io" VALUES('KPP102','tmp_l2',0,'','0');
INSERT INTO "CM202_blocks_io" VALUES('G_BC114','com',0,'','0');
INSERT INTO "CM202_blocks_io" VALUES('DR101','t_kl1',0,'','10');
INSERT INTO "CM202_blocks_io" VALUES('DR101','t_kl2',0,'','5');
INSERT INTO "CM202_blocks_io" VALUES('DR101','tmp_l1',0,'','49.9999999999964');
INSERT INTO "CM202_blocks_io" VALUES('DR101','tmp_l2',0,'','0');
INSERT INTO "CM202_blocks_io" VALUES('G_BC114','pos',0,'','0');
INSERT INTO "CM202_blocks_io" VALUES('BC111','t_kl1',0,'','10');
INSERT INTO "CM202_blocks_io" VALUES('BC111','t_kl2',0,'','5');
INSERT INTO "CM202_blocks_io" VALUES('BC111','tmp_l1',0,'','2.47032822920623e-321');
INSERT INTO "CM202_blocks_io" VALUES('BC111','tmp_l2',0,'','0');
INSERT INTO "CM202_blocks_io" VALUES('BC113','t_kl1',0,'','10');
INSERT INTO "CM202_blocks_io" VALUES('BC113','t_kl2',0,'','5');
INSERT INTO "CM202_blocks_io" VALUES('BC113','tmp_l1',0,'','0');
INSERT INTO "CM202_blocks_io" VALUES('BC113','tmp_l2',0,'','0');
INSERT INTO "CM202_blocks_io" VALUES('BC114','t_kl1',0,'','10');
INSERT INTO "CM202_blocks_io" VALUES('BC114','t_kl2',0,'','5');
INSERT INTO "CM202_blocks_io" VALUES('BC114','tmp_l1',0,'','2.47032822920623e-321');
INSERT INTO "CM202_blocks_io" VALUES('BC114','tmp_l2',0,'','0');
INSERT INTO "CM202_blocks_io" VALUES('BC116','t_kl1',0,'','10');
INSERT INTO "CM202_blocks_io" VALUES('BC116','t_kl2',0,'','5');
INSERT INTO "CM202_blocks_io" VALUES('BC116','tmp_l1',0,'','0');
INSERT INTO "CM202_blocks_io" VALUES('BC116','tmp_l2',0,'','0');
INSERT INTO "CM202_blocks_io" VALUES('G_BC111','pos',0,'','0');
INSERT INTO "CM202_blocks_io" VALUES('node5_1','Kpr',0,'','0.95');
INSERT INTO "CM202_blocks_io" VALUES('CM102_1','Kpr',0,'','0.95');
INSERT INTO "CM202_blocks_io" VALUES('CM102_2','Kpr',0,'','0.95');
INSERT INTO "CM202_blocks_io" VALUES('node6_1','Kpr',0,'','0.95');
INSERT INTO "CM202_blocks_io" VALUES('G_BC116','pos',0,'','0');
INSERT INTO "CM202_blocks_io" VALUES('G_BC114','st_open',0,'','0');
INSERT INTO "CM202_blocks_io" VALUES('PP103','Kpr',0,'','0.95');
INSERT INTO "CM202_blocks_io" VALUES('PP104','Kpr',0,'','0.95');
INSERT INTO "CM202_blocks_io" VALUES('G_BC112','pos',0,'','0');
INSERT INTO "CM202_blocks_io" VALUES('G_BC114','st_close',0,'','1');
INSERT INTO "CM202_blocks_io" VALUES('G_BC114','t_full',0,'','5');
INSERT INTO "CM202_blocks_io" VALUES('G_BC114','t_up',0,'','0.5');
INSERT INTO "CM202_blocks_io" VALUES('G_BC114','f_frq',0,'','100');
INSERT INTO "CM202_blocks_io" VALUES('G_BC114','tmp_up',0,'','0');
INSERT INTO "CM202_blocks_io" VALUES('G_BC114','lst_com',0,'','0');
INSERT INTO "CM202_blocks_io" VALUES('G_BC115','pos',0,'','0');
INSERT INTO "CM202_blocks_io" VALUES('G_BC115','com',0,'','0');
INSERT INTO "CM202_blocks_io" VALUES('G_BC115','st_open',0,'','0');
INSERT INTO "CM202_blocks_io" VALUES('G_BC115','st_close',0,'','1');
INSERT INTO "CM202_blocks_io" VALUES('G_BC115','t_full',0,'','5');
INSERT INTO "CM202_blocks_io" VALUES('G_BC115','t_up',0,'','0.5');
INSERT INTO "CM202_blocks_io" VALUES('G_BC115','f_frq',0,'','100');
INSERT INTO "CM202_blocks_io" VALUES('G_BC115','tmp_up',0,'','0');
INSERT INTO "CM202_blocks_io" VALUES('G_BC115','lst_com',0,'','0');
INSERT INTO "CM202_blocks_io" VALUES('G_BC111','com',0,'','0');
INSERT INTO "CM202_blocks_io" VALUES('G_BC111','st_open',0,'','0');
INSERT INTO "CM202_blocks_io" VALUES('G_BC111','st_close',0,'','1');
INSERT INTO "CM202_blocks_io" VALUES('G_BC111','t_full',0,'','5');
INSERT INTO "CM202_blocks_io" VALUES('G_BC111','t_up',0,'','0.5');
INSERT INTO "CM202_blocks_io" VALUES('G_BC111','f_frq',0,'','100');
INSERT INTO "CM202_blocks_io" VALUES('G_BC111','tmp_up',0,'','0');
INSERT INTO "CM202_blocks_io" VALUES('G_BC111','lst_com',0,'','0');
INSERT INTO "CM202_blocks_io" VALUES('G_BC116','com',0,'','0');
INSERT INTO "CM202_blocks_io" VALUES('G_BC116','st_open',0,'','0');
INSERT INTO "CM202_blocks_io" VALUES('G_BC116','st_close',0,'','1');
INSERT INTO "CM202_blocks_io" VALUES('G_BC116','t_full',0,'','5');
INSERT INTO "CM202_blocks_io" VALUES('G_BC116','t_up',0,'','0.5');
INSERT INTO "CM202_blocks_io" VALUES('G_BC116','f_frq',0,'','100');
INSERT INTO "CM202_blocks_io" VALUES('G_BC116','tmp_up',0,'','0');
INSERT INTO "CM202_blocks_io" VALUES('G_BC116','lst_com',0,'','0');
INSERT INTO "CM202_blocks_io" VALUES('G_BC112','com',0,'','0');
INSERT INTO "CM202_blocks_io" VALUES('G_BC112','st_open',0,'','0');
INSERT INTO "CM202_blocks_io" VALUES('G_BC112','st_close',0,'','1');
INSERT INTO "CM202_blocks_io" VALUES('G_BC112','t_full',0,'','3');
INSERT INTO "CM202_blocks_io" VALUES('G_BC112','t_up',0,'','0.3');
INSERT INTO "CM202_blocks_io" VALUES('G_BC112','f_frq',0,'','100');
INSERT INTO "CM202_blocks_io" VALUES('G_BC112','tmp_up',0,'','0');
INSERT INTO "CM202_blocks_io" VALUES('G_BC112','lst_com',0,'','0');
CREATE TABLE 'CM202_prm' ("SHIFR" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '' ,"IO" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"ru#IO" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"uk#IO" TEXT DEFAULT '' , PRIMARY KEY ("SHIFR"));
INSERT INTO "CM202_prm" VALUES('KPP102','KPP202','Anti-surge control valve of the compressor',1,'KPP102.l_kl1:out:Output
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
INSERT INTO "CM202_prm" VALUES('F103','F203','Flow on the diaphragm PP203',1,'PP103.Fi:var:Variable
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
INSERT INTO "CM202_prm" VALUES('TT0202','TE1202_5','Temperature after the first stage of the compressor',1,'CM102_1.To:var:Variable
*s.К:ed:Dimension
*i.0:prec:Precision
*r.273:min:Minimum
*r.373:max:Maximum','','Температура после первой ступени компрессора','CM102_1.To:var:Переменная
*s.К:ed:Ед.измерения
*i.0:prec:Точность
*r.273:min:Минимум
*r.373:max:Максимум','','Температура після першої ступені компресора','CM102_1.To:var:Змінна
*s.К:ed:Од.виміру
*i.0:prec:Точність
*r.273:min:Мінімум
*r.373:max:Максимум');
INSERT INTO "CM202_prm" VALUES('F104','F204','Flow on the diaphragm PP204',1,'PP104.Fi:var:Variable
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
INSERT INTO "CM202_prm" VALUES('TT0204','TE1205_5','Temperature after the second stage of the compressor',1,'CM102_2.To:var:Variable
*s.К:ed:Dimension
*i.0:prec:Precision
*r.273:min:Minimum
*r.423:max:Maximum','','Температура после второй ступени компрессора','CM102_2.To:var:Переменная
*s.К:ed:Ед.измерения
*i.0:prec:Точность
*r.273:min:Минимум
*r.423:max:Максимум','','Температура після другої ступені компресора','CM102_2.To:var:Змінна
*s.К:ed:Од.виміру
*i.0:prec:Точність
*r.273:min:Мінімум
*r.423:max:Максимум');
INSERT INTO "CM202_prm" VALUES('TE1313_1','TE1313_5','The temperature at the input of the first stage of the compressor',1,'node5_1.To:var:Variable
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
INSERT INTO "CM202_prm" VALUES('PT0202','PT0202_5','Pressure after the first stage of the compressor',1,'CM102_1.Po:var:Variable
*s.at:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.75:max:Maximum','','Давление после первой ступени компрессора','CM102_1.Po:var:Переменная
*s.ат:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.75:max:Максимум','','Тиск після першого ступеня компресору','CM102_1.Po:var:Змінна
*s.ат:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.75:max:Максимум');
INSERT INTO "CM202_prm" VALUES('BC111','BC211','Control block of the ball crane BC211',1,'G_BC111.com:com
G_BC111.st_open:st_open
G_BC111.st_close:st_close','КШ211','Блок управления шарового крана КШ211','','КК211','Блок керування кульовим краном КК211','');
INSERT INTO "CM202_prm" VALUES('PT0204','PT0204_5','Pressure after the second stage of the compressor',1,'CM102_2.Po:var:Variable
*s.at:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.150:max:Maximum','','Давление после второй ступени компрессора','CM102_2.Po:var:Переменная
*s.ат:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.150:max:Максимум','','Тиск після другого ступеня компресору','CM102_2.Po:var:Змінна
*s.ат:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.150:max:Максимум');
INSERT INTO "CM202_prm" VALUES('BC112','BC212','Control block of the ball crane BC212',1,'G_BC112.com:com
G_BC112.st_open:st_open
G_BC112.st_close:st_close','КШ212','Блок управления шарового крана КШ212','','КК212','Блок керування кульовим краном КК212','');
INSERT INTO "CM202_prm" VALUES('BC114','BC214','Control block of the ball crane BC214',1,'G_BC114.com:com
G_BC114.st_open:st_open
G_BC114.st_close:st_close','КШ214','Блок управления шарового крана КШ214','','КК214','Блок керування кульовим краном КК214','');
INSERT INTO "CM202_prm" VALUES('BC115','BC215','Control block of the ball crane BC215',1,'G_BC115.com:com
G_BC115.st_open:st_open
G_BC115.st_close:st_close','КШ215','Блок управления шарового крана КШ215','','КК215','Блок керування кульовим краном КК215','');
INSERT INTO "CM202_prm" VALUES('BC116','BC216','Control block of the ball crane BC216',1,'G_BC116.com:com
G_BC116.st_open:st_open
G_BC116.st_close:st_close','КШ216','Блок управления шарового крана КШ216','','КК216','Блок керування кульовим краном КК216','');
INSERT INTO "CM202_prm" VALUES('TE1314_1','TE1314_5','The temperature after the fridge of the first stage of the compressor',1,'AT102_1.To:var:Variable
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
INSERT INTO "CM202_prm" VALUES('TE1314_2','TE1206_5','The temperature after the fridge of the second stage of the compressor',1,'AT102_2.To:var:Variable
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
INSERT INTO "CM202_prm" VALUES('PT1006_1','PT1006_5','Pressure on the diaphragm PP204',1,'PP104.Pi:var:Variable
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
INSERT INTO "CM202_prm" VALUES('P103','P203','Pressure on the diaphragm PP203',1,'PP103.Pi:var:Variable
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
INSERT INTO "CM202_prm" VALUES('ST8612','ST8612_5','Rotation speed of the compressor',1,'CM102_1.N:var:Variable
CM102_1.N:varIn:Variable input
*s.1000x rpm:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.15:max:Maximum','','Обороты вращения компрессора','CM102_1.N:var:Переменная
CM102_1.N:varIn:Ввод переменной
*s.1000x об.мин.:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.15:max:Максимум','','Оберти компресору','CM102_1.N:var:Змінна
CM102_1.N:varIn:Ввід змінної
*s.1000x об.хвил:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.15:max:Максимум');
CREATE TABLE 'CM301_blocks_io' ("BLK_ID" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"TLNK" INTEGER DEFAULT '' ,"LNK" TEXT DEFAULT '' ,"VAL" TEXT DEFAULT '' , PRIMARY KEY ("BLK_ID","ID"));
INSERT INTO "CM301_blocks_io" VALUES('CM101_1','Fi',0,'','2.28950176956007e-05');
INSERT INTO "CM301_blocks_io" VALUES('CM101_1','Pi',1,'PP101.Po','5.59313376616615');
INSERT INTO "CM301_blocks_io" VALUES('CM101_1','Ti',0,'','273');
INSERT INTO "CM301_blocks_io" VALUES('CM101_1','Fo',1,'AT101_1.Fi','-0.000586564761378129');
INSERT INTO "CM301_blocks_io" VALUES('CM101_1','Po',0,'','5.59313397430224');
INSERT INTO "CM301_blocks_io" VALUES('CM101_1','To',0,'','273.015941182151');
INSERT INTO "CM301_blocks_io" VALUES('CM101_1','So',0,'','0.071');
INSERT INTO "CM301_blocks_io" VALUES('CM101_1','lo',0,'','55');
INSERT INTO "CM301_blocks_io" VALUES('CM101_1','Kzp',0,'','0');
INSERT INTO "CM301_blocks_io" VALUES('CM101_1','N',0,'','0');
INSERT INTO "CM301_blocks_io" VALUES('CM101_1','V',0,'','1.5');
INSERT INTO "CM301_blocks_io" VALUES('CM101_1','Kpmp',0,'','0.025');
INSERT INTO "CM301_blocks_io" VALUES('CM101_1','Kslp',0,'','0.2');
INSERT INTO "CM301_blocks_io" VALUES('CM101_1','Q0',0,'','1');
INSERT INTO "CM301_blocks_io" VALUES('CM101_1','Ct',0,'','100');
INSERT INTO "CM301_blocks_io" VALUES('CM101_1','Riz',0,'','100');
INSERT INTO "CM301_blocks_io" VALUES('CM101_1','Fwind',0,'','1');
INSERT INTO "CM301_blocks_io" VALUES('CM101_1','Twind',0,'','273');
INSERT INTO "CM301_blocks_io" VALUES('CM101_1','f_frq',0,'','100');
INSERT INTO "CM301_blocks_io" VALUES('CM101_1','Pot',0,'','5.40023768274556');
INSERT INTO "CM301_blocks_io" VALUES('CM101_1','Fit',0,'','-0.000534796370268681');
INSERT INTO "CM301_blocks_io" VALUES('CM101_2','Fi',0,'','3.23518730809843e-05');
INSERT INTO "CM301_blocks_io" VALUES('CM101_2','Pi',1,'PP102.Po','5.59313400466739');
INSERT INTO "CM301_blocks_io" VALUES('CM101_2','Ti',1,'AT101_1.To','283.000000013188');
INSERT INTO "CM301_blocks_io" VALUES('CM101_2','Fo',1,'AT101_2.Fi','-8.24818712260721e-05');
INSERT INTO "CM301_blocks_io" VALUES('CM101_2','Po',0,'','5.59313179013177');
INSERT INTO "CM301_blocks_io" VALUES('CM101_2','To',0,'','274.522076141394');
INSERT INTO "CM301_blocks_io" VALUES('CM101_2','So',0,'','0.0314');
INSERT INTO "CM301_blocks_io" VALUES('CM101_2','lo',0,'','55');
INSERT INTO "CM301_blocks_io" VALUES('CM101_2','Kzp',0,'','0');
INSERT INTO "CM301_blocks_io" VALUES('CM101_2','N',1,'CM101_1.N','0');
INSERT INTO "CM301_blocks_io" VALUES('CM101_2','V',0,'','1');
INSERT INTO "CM301_blocks_io" VALUES('CM101_2','Kpmp',0,'','0.035');
INSERT INTO "CM301_blocks_io" VALUES('CM101_2','Kslp',0,'','0.2');
INSERT INTO "CM301_blocks_io" VALUES('CM101_2','Q0',0,'','1');
INSERT INTO "CM301_blocks_io" VALUES('CM101_2','Ct',0,'','100');
INSERT INTO "CM301_blocks_io" VALUES('CM101_2','Riz',0,'','100');
INSERT INTO "CM301_blocks_io" VALUES('CM101_2','Fwind',0,'','1');
INSERT INTO "CM301_blocks_io" VALUES('CM101_2','Twind',0,'','273');
INSERT INTO "CM301_blocks_io" VALUES('CM101_2','f_frq',0,'','100');
INSERT INTO "CM301_blocks_io" VALUES('CM101_2','Pot',0,'','5.40169165469113');
INSERT INTO "CM301_blocks_io" VALUES('CM101_2','Fit',0,'','0.000655794717543569');
INSERT INTO "CM301_blocks_io" VALUES('KPP101','Fi',0,'','4.95957242411095e-286');
INSERT INTO "CM301_blocks_io" VALUES('KPP101','Pi',1,'node4_1.Po2','5.59311029355491');
INSERT INTO "CM301_blocks_io" VALUES('KPP101','Ti',1,'AT101_2.To','282.999999782413');
INSERT INTO "CM301_blocks_io" VALUES('KPP101','Fo',1,'node3_1.Fi2','0.00022120956420153');
INSERT INTO "CM301_blocks_io" VALUES('KPP101','Po',0,'','5.59313014919409');
INSERT INTO "CM301_blocks_io" VALUES('KPP101','To',0,'','273.000000092615');
INSERT INTO "CM301_blocks_io" VALUES('KPP101','So',0,'','0.071');
INSERT INTO "CM301_blocks_io" VALUES('KPP101','lo',0,'','10');
INSERT INTO "CM301_blocks_io" VALUES('KPP101','S_kl1',0,'','0.04');
INSERT INTO "CM301_blocks_io" VALUES('KPP101','S_kl2',0,'','0.001');
INSERT INTO "CM301_blocks_io" VALUES('KPP101','l_kl1',2,'gen_cntr.SurgeCM301.out','0');
INSERT INTO "CM301_blocks_io" VALUES('KPP101','l_kl2',0,'','0');
INSERT INTO "CM301_blocks_io" VALUES('KPP101','Q0',0,'','1');
INSERT INTO "CM301_blocks_io" VALUES('KPP101','Kln',0,'','2');
INSERT INTO "CM301_blocks_io" VALUES('KPP101','Kpr',0,'','0.95');
INSERT INTO "CM301_blocks_io" VALUES('KPP101','Ct',0,'','20');
INSERT INTO "CM301_blocks_io" VALUES('KPP101','Riz',0,'','20');
INSERT INTO "CM301_blocks_io" VALUES('KPP101','noBack',0,'','0');
INSERT INTO "CM301_blocks_io" VALUES('KPP101','Fwind',0,'','1');
INSERT INTO "CM301_blocks_io" VALUES('KPP101','Twind',0,'','273');
INSERT INTO "CM301_blocks_io" VALUES('KPP101','f_frq',0,'','100');
INSERT INTO "CM301_blocks_io" VALUES('KPP101','Pot',0,'','5.40157875443619');
INSERT INTO "CM301_blocks_io" VALUES('KPP101','Fit',0,'','-0.000441000488177914');
INSERT INTO "CM301_blocks_io" VALUES('DR100','Fi',0,'','0.000210930375204527');
INSERT INTO "CM301_blocks_io" VALUES('DR100','Pi',1,'BC106.Po','5.59311992820393');
INSERT INTO "CM301_blocks_io" VALUES('DR100','Ti',1,'BC106.To','273');
INSERT INTO "CM301_blocks_io" VALUES('DR100','Fo',1,'node3_1.Fi1','0.000336834951113603');
INSERT INTO "CM301_blocks_io" VALUES('DR100','Po',0,'','5.59313692466279');
INSERT INTO "CM301_blocks_io" VALUES('DR100','To',0,'','273.000000001325');
INSERT INTO "CM301_blocks_io" VALUES('DR100','So',0,'','0.2');
INSERT INTO "CM301_blocks_io" VALUES('DR100','lo',0,'','1');
INSERT INTO "CM301_blocks_io" VALUES('DR100','S_kl1',0,'','0.05');
INSERT INTO "CM301_blocks_io" VALUES('DR100','S_kl2',0,'','0.01');
INSERT INTO "CM301_blocks_io" VALUES('DR100','l_kl1',0,'','50');
INSERT INTO "CM301_blocks_io" VALUES('DR100','l_kl2',0,'','0');
INSERT INTO "CM301_blocks_io" VALUES('DR100','Q0',0,'','1');
INSERT INTO "CM301_blocks_io" VALUES('DR100','Kln',0,'','1');
INSERT INTO "CM301_blocks_io" VALUES('DR100','Kpr',0,'','0.95');
INSERT INTO "CM301_blocks_io" VALUES('DR100','Ct',0,'','20');
INSERT INTO "CM301_blocks_io" VALUES('DR100','Riz',0,'','20');
INSERT INTO "CM301_blocks_io" VALUES('DR100','noBack',0,'','0');
INSERT INTO "CM301_blocks_io" VALUES('DR100','Fwind',0,'','1');
INSERT INTO "CM301_blocks_io" VALUES('DR100','Twind',0,'','273');
INSERT INTO "CM301_blocks_io" VALUES('DR100','f_frq',0,'','100');
INSERT INTO "CM301_blocks_io" VALUES('DR100','Pot',0,'','5.78364373014117');
INSERT INTO "CM301_blocks_io" VALUES('DR100','Fit',0,'','-17.4563867731226');
INSERT INTO "CM301_blocks_io" VALUES('AT101_1','Fi',0,'','0.00053618398281369');
INSERT INTO "CM301_blocks_io" VALUES('AT101_1','Pi',1,'CM101_1.Po','5.5931339430601');
INSERT INTO "CM301_blocks_io" VALUES('AT101_1','Ti',1,'CM101_1.To','273.015941184354');
INSERT INTO "CM301_blocks_io" VALUES('AT101_1','Si',0,'','0.065');
INSERT INTO "CM301_blocks_io" VALUES('AT101_1','li',0,'','10');
INSERT INTO "CM301_blocks_io" VALUES('AT101_1','Fo',1,'C101_2.Fi','-2.24667603917728e-05');
INSERT INTO "CM301_blocks_io" VALUES('AT101_1','Po',0,'','5.59313912710591');
INSERT INTO "CM301_blocks_io" VALUES('AT101_1','To',0,'','282.999999874734');
INSERT INTO "CM301_blocks_io" VALUES('AT101_1','So',0,'','0.071');
INSERT INTO "CM301_blocks_io" VALUES('AT101_1','lo',0,'','2');
INSERT INTO "CM301_blocks_io" VALUES('AT101_1','Tair',0,'','283');
INSERT INTO "CM301_blocks_io" VALUES('AT101_1','Wc',0,'','200');
INSERT INTO "CM301_blocks_io" VALUES('AT101_1','Q0',0,'','1');
INSERT INTO "CM301_blocks_io" VALUES('AT101_1','Ct',0,'','100');
INSERT INTO "CM301_blocks_io" VALUES('AT101_1','Rt',0,'','1');
INSERT INTO "CM301_blocks_io" VALUES('AT101_1','f_frq',0,'','100');
INSERT INTO "CM301_blocks_io" VALUES('AT101_1','Pot',0,'','5.40045482639887');
INSERT INTO "CM301_blocks_io" VALUES('AT101_1','Fit',0,'','-0.0796572232281504');
INSERT INTO "CM301_blocks_io" VALUES('AT101_2','Fi',0,'','0.000229655458930842');
INSERT INTO "CM301_blocks_io" VALUES('AT101_2','Pi',1,'CM101_2.Po','5.59313193379748');
INSERT INTO "CM301_blocks_io" VALUES('AT101_2','Ti',1,'CM101_2.To','274.522076554184');
INSERT INTO "CM301_blocks_io" VALUES('AT101_2','Si',0,'','0.029');
INSERT INTO "CM301_blocks_io" VALUES('AT101_2','li',0,'','20');
INSERT INTO "CM301_blocks_io" VALUES('AT101_2','Fo',1,'C101_3.Fi','0.000188566223519825');
INSERT INTO "CM301_blocks_io" VALUES('AT101_2','Po',0,'','5.59314527010567');
INSERT INTO "CM301_blocks_io" VALUES('AT101_2','To',0,'','282.999999695603');
INSERT INTO "CM301_blocks_io" VALUES('AT101_2','So',0,'','0.0314');
INSERT INTO "CM301_blocks_io" VALUES('AT101_2','lo',0,'','5');
INSERT INTO "CM301_blocks_io" VALUES('AT101_2','Tair',0,'','283');
INSERT INTO "CM301_blocks_io" VALUES('AT101_2','Wc',0,'','200');
INSERT INTO "CM301_blocks_io" VALUES('AT101_2','Q0',0,'','1');
INSERT INTO "CM301_blocks_io" VALUES('AT101_2','Ct',0,'','100');
INSERT INTO "CM301_blocks_io" VALUES('AT101_2','Rt',0,'','1');
INSERT INTO "CM301_blocks_io" VALUES('AT101_2','f_frq',0,'','100');
INSERT INTO "CM301_blocks_io" VALUES('AT101_2','Pot',0,'','5.4020877752679');
INSERT INTO "CM301_blocks_io" VALUES('AT101_2','Fit',0,'','-0.025736784212778');
INSERT INTO "CM301_blocks_io" VALUES('node3_1','Fi1',0,'','8.23309499698267e-05');
INSERT INTO "CM301_blocks_io" VALUES('node3_1','Pi1',1,'DR100.Po','5.59311131844152');
INSERT INTO "CM301_blocks_io" VALUES('node3_1','Ti1',1,'DR100.To','273.000000001321');
INSERT INTO "CM301_blocks_io" VALUES('node3_1','Si1',0,'','0.2');
INSERT INTO "CM301_blocks_io" VALUES('node3_1','Fi2',0,'','0.000177561268228818');
INSERT INTO "CM301_blocks_io" VALUES('node3_1','Pi2',1,'KPP101.Po','5.59313014919409');
INSERT INTO "CM301_blocks_io" VALUES('node3_1','Ti2',1,'KPP101.To','273.000000092615');
INSERT INTO "CM301_blocks_io" VALUES('node3_1','Si2',0,'','0.071');
INSERT INTO "CM301_blocks_io" VALUES('node3_1','Fi3',0,'','-3.83423573526475e-05');
INSERT INTO "CM301_blocks_io" VALUES('node3_1','Pi3',1,'BC101.Po','5.59313203244356');
INSERT INTO "CM301_blocks_io" VALUES('node3_1','Ti3',1,'BC101.To','273.000000002935');
INSERT INTO "CM301_blocks_io" VALUES('node3_1','Si3',0,'','0.2');
INSERT INTO "CM301_blocks_io" VALUES('node3_1','Fo',1,'C101_1.Fi','-0.000135575947707912');
INSERT INTO "CM301_blocks_io" VALUES('node3_1','Po',0,'','5.59313481099902');
INSERT INTO "CM301_blocks_io" VALUES('node3_1','To',0,'','272.999999999081');
INSERT INTO "CM301_blocks_io" VALUES('node3_1','So',0,'','0.2');
INSERT INTO "CM301_blocks_io" VALUES('node3_1','lo',0,'','50');
INSERT INTO "CM301_blocks_io" VALUES('node3_1','Q0',0,'','1');
INSERT INTO "CM301_blocks_io" VALUES('node3_1','Ct',0,'','20');
INSERT INTO "CM301_blocks_io" VALUES('node3_1','Riz',0,'','20');
INSERT INTO "CM301_blocks_io" VALUES('node3_1','Fwind',0,'','1');
INSERT INTO "CM301_blocks_io" VALUES('node3_1','Twind',0,'','273');
INSERT INTO "CM301_blocks_io" VALUES('node3_1','f_frq',0,'','100');
INSERT INTO "CM301_blocks_io" VALUES('node3_1','FoR',0,'','51.7594059956549');
INSERT INTO "CM301_blocks_io" VALUES('node3_1','Pot',0,'','5.40139672085537');
INSERT INTO "CM301_blocks_io" VALUES('node3_1','Fit',0,'','1.09362678119313');
INSERT INTO "CM301_blocks_io" VALUES('BC101','Fi',0,'','-4.84184332924422e-322');
INSERT INTO "CM301_blocks_io" VALUES('BC101','Pi',0,'','5.58168787216618');
INSERT INTO "CM301_blocks_io" VALUES('BC101','Ti',2,'gen.PCV1.To','290.37389754322');
INSERT INTO "CM301_blocks_io" VALUES('BC101','Fo',1,'node3_1.Fi3','0.000137779876564248');
INSERT INTO "CM301_blocks_io" VALUES('BC101','Po',0,'','5.59313218412456');
INSERT INTO "CM301_blocks_io" VALUES('BC101','To',0,'','273.000000002935');
INSERT INTO "CM301_blocks_io" VALUES('BC101','So',0,'','0.2');
INSERT INTO "CM301_blocks_io" VALUES('BC101','lo',0,'','5');
INSERT INTO "CM301_blocks_io" VALUES('BC101','S_kl1',0,'','0.2');
INSERT INTO "CM301_blocks_io" VALUES('BC101','S_kl2',0,'','0.05');
INSERT INTO "CM301_blocks_io" VALUES('BC101','l_kl1',1,'G_BC101.pos','0');
INSERT INTO "CM301_blocks_io" VALUES('BC101','l_kl2',1,'G_BC102.pos','0');
INSERT INTO "CM301_blocks_io" VALUES('BC101','Q0',0,'','1');
INSERT INTO "CM301_blocks_io" VALUES('BC101','Kln',0,'','1');
INSERT INTO "CM301_blocks_io" VALUES('BC101','Kpr',0,'','0.95');
INSERT INTO "CM301_blocks_io" VALUES('BC101','Ct',0,'','20');
INSERT INTO "CM301_blocks_io" VALUES('BC101','Riz',0,'','20');
INSERT INTO "CM301_blocks_io" VALUES('BC101','noBack',0,'','0');
INSERT INTO "CM301_blocks_io" VALUES('BC101','Fwind',0,'','1');
INSERT INTO "CM301_blocks_io" VALUES('BC101','Twind',0,'','273');
INSERT INTO "CM301_blocks_io" VALUES('BC101','f_frq',0,'','100');
INSERT INTO "CM301_blocks_io" VALUES('BC101','Pot',0,'','5.40316262054898');
INSERT INTO "CM301_blocks_io" VALUES('BC101','Fit',0,'','-0.597988403950033');
INSERT INTO "CM301_blocks_io" VALUES('BC104','Fi',0,'','0');
INSERT INTO "CM301_blocks_io" VALUES('BC104','Pi',1,'node4_1.Po3','5.5931764595351');
INSERT INTO "CM301_blocks_io" VALUES('BC104','Ti',1,'AT101_2.To','282.999999714834');
INSERT INTO "CM301_blocks_io" VALUES('BC104','Fo',0,'','0.00923139055181331');
INSERT INTO "CM301_blocks_io" VALUES('BC104','Po',0,'','24.5716868275176');
INSERT INTO "CM301_blocks_io" VALUES('BC104','To',0,'','273.000000009146');
INSERT INTO "CM301_blocks_io" VALUES('BC104','So',0,'','0.03');
INSERT INTO "CM301_blocks_io" VALUES('BC104','lo',0,'','100');
INSERT INTO "CM301_blocks_io" VALUES('BC104','S_kl1',0,'','0.03');
INSERT INTO "CM301_blocks_io" VALUES('BC104','S_kl2',0,'','0.01');
INSERT INTO "CM301_blocks_io" VALUES('BC104','l_kl1',1,'G_BC104.pos','0');
INSERT INTO "CM301_blocks_io" VALUES('BC104','l_kl2',1,'G_BC105.pos','0');
INSERT INTO "CM301_blocks_io" VALUES('BC104','Q0',0,'','1');
INSERT INTO "CM301_blocks_io" VALUES('BC104','Kln',0,'','1');
INSERT INTO "CM301_blocks_io" VALUES('BC104','Kpr',0,'','0.95');
INSERT INTO "CM301_blocks_io" VALUES('BC104','Ct',0,'','20');
INSERT INTO "CM301_blocks_io" VALUES('BC104','Riz',0,'','20');
INSERT INTO "CM301_blocks_io" VALUES('BC104','noBack',0,'','1');
INSERT INTO "CM301_blocks_io" VALUES('BC104','Fwind',0,'','1');
INSERT INTO "CM301_blocks_io" VALUES('BC104','Twind',0,'','273');
INSERT INTO "CM301_blocks_io" VALUES('BC104','f_frq',0,'','100');
INSERT INTO "CM301_blocks_io" VALUES('BC104','Pot',0,'','25.0241440068695');
INSERT INTO "CM301_blocks_io" VALUES('BC104','Fit',0,'','2.47032822920623e-322');
INSERT INTO "CM301_blocks_io" VALUES('node4_1','Fi',0,'','4.59783486873543e-05');
INSERT INTO "CM301_blocks_io" VALUES('node4_1','Pi',1,'C101_3.Po','5.59314908505102');
INSERT INTO "CM301_blocks_io" VALUES('node4_1','Fo1',1,'BC106.Fi','0');
INSERT INTO "CM301_blocks_io" VALUES('node4_1','Po1',0,'','5.59314219810045');
INSERT INTO "CM301_blocks_io" VALUES('node4_1','So1',0,'','0.0314');
INSERT INTO "CM301_blocks_io" VALUES('node4_1','lo1',0,'','5');
INSERT INTO "CM301_blocks_io" VALUES('node4_1','Fo2',1,'KPP101.Fi','4.95461285168684e-286');
INSERT INTO "CM301_blocks_io" VALUES('node4_1','Po2',0,'','5.59314759318974');
INSERT INTO "CM301_blocks_io" VALUES('node4_1','So2',0,'','0.0177');
INSERT INTO "CM301_blocks_io" VALUES('node4_1','lo2',0,'','5');
INSERT INTO "CM301_blocks_io" VALUES('node4_1','Fo3',1,'BC104.Fi','0');
INSERT INTO "CM301_blocks_io" VALUES('node4_1','Po3',0,'','5.59312400210236');
INSERT INTO "CM301_blocks_io" VALUES('node4_1','So3',0,'','0.0314');
INSERT INTO "CM301_blocks_io" VALUES('node4_1','lo3',0,'','5');
INSERT INTO "CM301_blocks_io" VALUES('node4_1','Q0',0,'','1');
INSERT INTO "CM301_blocks_io" VALUES('node4_1','f_frq',0,'','100');
INSERT INTO "CM301_blocks_io" VALUES('node4_1','F1tmp',0,'','0.000319407165071405');
INSERT INTO "CM301_blocks_io" VALUES('node4_1','F2tmp',0,'','0.000760810855171461');
INSERT INTO "CM301_blocks_io" VALUES('node4_1','F3tmp',0,'','-0.00103423967155551');
INSERT INTO "CM301_blocks_io" VALUES('node4_1','Pot1',0,'','5.4352722783381');
INSERT INTO "CM301_blocks_io" VALUES('node4_1','Pot2',0,'','5.42036218663654');
INSERT INTO "CM301_blocks_io" VALUES('node4_1','Pot3',0,'','5.3236984372462');
INSERT INTO "CM301_blocks_io" VALUES('BC106','Fi',0,'','0');
INSERT INTO "CM301_blocks_io" VALUES('BC106','Pi',1,'node4_1.Po1','5.59313893075828');
INSERT INTO "CM301_blocks_io" VALUES('BC106','Ti',1,'AT101_2.To','282.999999714834');
INSERT INTO "CM301_blocks_io" VALUES('BC106','Fo',1,'DR100.Fi','0.00180981050475542');
INSERT INTO "CM301_blocks_io" VALUES('BC106','Po',0,'','5.59311992820393');
INSERT INTO "CM301_blocks_io" VALUES('BC106','To',0,'','273');
INSERT INTO "CM301_blocks_io" VALUES('BC106','So',0,'','0.0314');
INSERT INTO "CM301_blocks_io" VALUES('BC106','lo',0,'','10');
INSERT INTO "CM301_blocks_io" VALUES('BC106','S_kl1',0,'','0.0314');
INSERT INTO "CM301_blocks_io" VALUES('BC106','S_kl2',0,'','0.01');
INSERT INTO "CM301_blocks_io" VALUES('BC106','l_kl1',1,'G_BC106.pos','0');
INSERT INTO "CM301_blocks_io" VALUES('BC106','l_kl2',0,'','0');
INSERT INTO "CM301_blocks_io" VALUES('BC106','Q0',0,'','1');
INSERT INTO "CM301_blocks_io" VALUES('BC106','Kln',0,'','1');
INSERT INTO "CM301_blocks_io" VALUES('BC106','Kpr',0,'','0.95');
INSERT INTO "CM301_blocks_io" VALUES('BC106','Ct',0,'','20');
INSERT INTO "CM301_blocks_io" VALUES('BC106','Riz',0,'','20');
INSERT INTO "CM301_blocks_io" VALUES('BC106','noBack',0,'','0');
INSERT INTO "CM301_blocks_io" VALUES('BC106','Fwind',0,'','1');
INSERT INTO "CM301_blocks_io" VALUES('BC106','Twind',0,'','273');
INSERT INTO "CM301_blocks_io" VALUES('BC106','f_frq',0,'','100');
INSERT INTO "CM301_blocks_io" VALUES('BC106','Pot',0,'','5.01309587841275');
INSERT INTO "CM301_blocks_io" VALUES('BC106','Fit',0,'','0');
INSERT INTO "CM301_blocks_io" VALUES('C101_1','Fi',0,'','-0.000210166139526238');
INSERT INTO "CM301_blocks_io" VALUES('C101_1','Pi',1,'node3_1.Po','5.59313445096467');
INSERT INTO "CM301_blocks_io" VALUES('C101_1','Si',0,'','0.2');
INSERT INTO "CM301_blocks_io" VALUES('C101_1','Fo',1,'PP101.Fi','8.63055158369547e-05');
INSERT INTO "CM301_blocks_io" VALUES('C101_1','Po',0,'','5.59313359540344');
INSERT INTO "CM301_blocks_io" VALUES('C101_1','So',0,'','0.2');
INSERT INTO "CM301_blocks_io" VALUES('C101_1','lo',0,'','40');
INSERT INTO "CM301_blocks_io" VALUES('C101_1','Fo_ж',0,'','0');
INSERT INTO "CM301_blocks_io" VALUES('C101_1','Po_ж',0,'','5.62245947509774');
INSERT INTO "CM301_blocks_io" VALUES('C101_1','Lж',0,'','3.08693470466339');
INSERT INTO "CM301_blocks_io" VALUES('C101_1','ProcЖ',0,'','0.01');
INSERT INTO "CM301_blocks_io" VALUES('C101_1','Vap',0,'','9.5');
INSERT INTO "CM301_blocks_io" VALUES('C101_1','Q0',0,'','1');
INSERT INTO "CM301_blocks_io" VALUES('C101_1','Qж',0,'','1000');
INSERT INTO "CM301_blocks_io" VALUES('C101_1','f_frq',0,'','100');
INSERT INTO "CM301_blocks_io" VALUES('C101_1','Pot',0,'','5.40135425836322');
INSERT INTO "CM301_blocks_io" VALUES('C101_1','Fit',0,'','0.147603004609401');
INSERT INTO "CM301_blocks_io" VALUES('C101_2','Fi',0,'','-2.90761466597559e-05');
INSERT INTO "CM301_blocks_io" VALUES('C101_2','Pi',1,'AT101_1.Po','5.59312692168549');
INSERT INTO "CM301_blocks_io" VALUES('C101_2','Si',0,'','0.071');
INSERT INTO "CM301_blocks_io" VALUES('C101_2','Fo',1,'PP102.Fi','8.54481807173548e-05');
INSERT INTO "CM301_blocks_io" VALUES('C101_2','Po',0,'','5.59313374626812');
INSERT INTO "CM301_blocks_io" VALUES('C101_2','So',0,'','0.071');
INSERT INTO "CM301_blocks_io" VALUES('C101_2','lo',0,'','50');
INSERT INTO "CM301_blocks_io" VALUES('C101_2','Fo_ж',0,'','0');
INSERT INTO "CM301_blocks_io" VALUES('C101_2','Po_ж',0,'','5.62169585273225');
INSERT INTO "CM301_blocks_io" VALUES('C101_2','Lж',0,'','3.00653752254049');
INSERT INTO "CM301_blocks_io" VALUES('C101_2','ProcЖ',0,'','0.01');
INSERT INTO "CM301_blocks_io" VALUES('C101_2','Vap',0,'','9.5');
INSERT INTO "CM301_blocks_io" VALUES('C101_2','Q0',0,'','1');
INSERT INTO "CM301_blocks_io" VALUES('C101_2','Qж',0,'','1000');
INSERT INTO "CM301_blocks_io" VALUES('C101_2','f_frq',0,'','100');
INSERT INTO "CM301_blocks_io" VALUES('C101_2','Pot',0,'','5.40056348486556');
INSERT INTO "CM301_blocks_io" VALUES('C101_2','Fit',0,'','-0.0226484354771605');
INSERT INTO "CM301_blocks_io" VALUES('C101_3','Fi',0,'','0.00045377220467983');
INSERT INTO "CM301_blocks_io" VALUES('C101_3','Pi',1,'AT101_2.Po','5.59313897994459');
INSERT INTO "CM301_blocks_io" VALUES('C101_3','Si',0,'','0.0314');
INSERT INTO "CM301_blocks_io" VALUES('C101_3','Fo',1,'node4_1.Fi','0.000649493932217063');
INSERT INTO "CM301_blocks_io" VALUES('C101_3','Po',0,'','5.59312568577158');
INSERT INTO "CM301_blocks_io" VALUES('C101_3','So',0,'','0.0314');
INSERT INTO "CM301_blocks_io" VALUES('C101_3','lo',0,'','8');
INSERT INTO "CM301_blocks_io" VALUES('C101_3','Fo_ж',0,'','0');
INSERT INTO "CM301_blocks_io" VALUES('C101_3','Po_ж',0,'','5.62021986948726');
INSERT INTO "CM301_blocks_io" VALUES('C101_3','Lж',0,'','9.03139457189366');
INSERT INTO "CM301_blocks_io" VALUES('C101_3','ProcЖ',0,'','0.01');
INSERT INTO "CM301_blocks_io" VALUES('C101_3','Vap',0,'','3');
INSERT INTO "CM301_blocks_io" VALUES('C101_3','Q0',0,'','1');
INSERT INTO "CM301_blocks_io" VALUES('C101_3','Qж',0,'','1000');
INSERT INTO "CM301_blocks_io" VALUES('C101_3','f_frq',0,'','100');
INSERT INTO "CM301_blocks_io" VALUES('C101_3','Pot',0,'','5.40093511492469');
INSERT INTO "CM301_blocks_io" VALUES('C101_3','Fit',0,'','0.131497386772777');
INSERT INTO "CM301_blocks_io" VALUES('PP101','Fi',0,'','-0.000125446532578783');
INSERT INTO "CM301_blocks_io" VALUES('PP101','Pi',1,'C101_1.Po','5.59313356232177');
INSERT INTO "CM301_blocks_io" VALUES('PP101','Fo',1,'CM101_1.Fi','-2.75459754974038e-05');
INSERT INTO "CM301_blocks_io" VALUES('PP101','Po',0,'','5.59313343475083');
INSERT INTO "CM301_blocks_io" VALUES('PP101','dP',0,'','-8.85971337210912e-08');
INSERT INTO "CM301_blocks_io" VALUES('PP101','Sdf',0,'','0.15');
INSERT INTO "CM301_blocks_io" VALUES('PP101','So',0,'','0.2');
INSERT INTO "CM301_blocks_io" VALUES('PP101','lo',0,'','7');
INSERT INTO "CM301_blocks_io" VALUES('PP101','Q0',0,'','1');
INSERT INTO "CM301_blocks_io" VALUES('PP101','f_frq',0,'','100');
INSERT INTO "CM301_blocks_io" VALUES('PP101','Pot',0,'','5.41867322751214');
INSERT INTO "CM301_blocks_io" VALUES('PP101','Fit',0,'','-6.87718415920635');
INSERT INTO "CM301_blocks_io" VALUES('PP102','Fi',0,'','-3.32943550274249e-05');
INSERT INTO "CM301_blocks_io" VALUES('PP102','Pi',1,'C101_2.Po','5.59313374819752');
INSERT INTO "CM301_blocks_io" VALUES('PP102','Fo',1,'CM101_2.Fi','3.06387001081259e-05');
INSERT INTO "CM301_blocks_io" VALUES('PP102','Po',0,'','5.59313358023199');
INSERT INTO "CM301_blocks_io" VALUES('PP102','dP',0,'','1.33580198881044e-07');
INSERT INTO "CM301_blocks_io" VALUES('PP102','Sdf',0,'','0.06');
INSERT INTO "CM301_blocks_io" VALUES('PP102','So',0,'','0.071');
INSERT INTO "CM301_blocks_io" VALUES('PP102','lo',0,'','7');
INSERT INTO "CM301_blocks_io" VALUES('PP102','Q0',0,'','1');
INSERT INTO "CM301_blocks_io" VALUES('PP102','f_frq',0,'','100');
INSERT INTO "CM301_blocks_io" VALUES('PP102','Pot',0,'','5.33730972594639');
INSERT INTO "CM301_blocks_io" VALUES('PP102','Fit',0,'','6.59905508875654');
INSERT INTO "CM301_blocks_io" VALUES('CM101_1','Kpr',0,'','0.95');
INSERT INTO "CM301_blocks_io" VALUES('CM101_2','Kpr',0,'','0.95');
INSERT INTO "CM301_blocks_io" VALUES('KPP101','t_kl1',0,'','3');
INSERT INTO "CM301_blocks_io" VALUES('KPP101','t_kl2',0,'','5');
INSERT INTO "CM301_blocks_io" VALUES('KPP101','tmp_l1',0,'','7.4109846876187e-322');
INSERT INTO "CM301_blocks_io" VALUES('KPP101','tmp_l2',0,'','0');
INSERT INTO "CM301_blocks_io" VALUES('DR100','t_kl1',0,'','10');
INSERT INTO "CM301_blocks_io" VALUES('DR100','t_kl2',0,'','5');
INSERT INTO "CM301_blocks_io" VALUES('DR100','tmp_l1',0,'','49.9999999999964');
INSERT INTO "CM301_blocks_io" VALUES('DR100','tmp_l2',0,'','0');
INSERT INTO "CM301_blocks_io" VALUES('node3_1','Kpr',0,'','0.95');
INSERT INTO "CM301_blocks_io" VALUES('BC101','t_kl1',0,'','10');
INSERT INTO "CM301_blocks_io" VALUES('BC101','t_kl2',0,'','5');
INSERT INTO "CM301_blocks_io" VALUES('BC101','tmp_l1',0,'','2.47032822920623e-321');
INSERT INTO "CM301_blocks_io" VALUES('BC101','tmp_l2',0,'','0');
INSERT INTO "CM301_blocks_io" VALUES('BC104','t_kl1',0,'','10');
INSERT INTO "CM301_blocks_io" VALUES('BC104','t_kl2',0,'','5');
INSERT INTO "CM301_blocks_io" VALUES('BC104','tmp_l1',0,'','2.47032822920623e-321');
INSERT INTO "CM301_blocks_io" VALUES('BC104','tmp_l2',0,'','0');
INSERT INTO "CM301_blocks_io" VALUES('node4_1','Kpr',0,'','0.95');
INSERT INTO "CM301_blocks_io" VALUES('BC106','t_kl1',0,'','10');
INSERT INTO "CM301_blocks_io" VALUES('BC106','t_kl2',0,'','5');
INSERT INTO "CM301_blocks_io" VALUES('BC106','tmp_l1',0,'','0');
INSERT INTO "CM301_blocks_io" VALUES('BC106','tmp_l2',0,'','0');
INSERT INTO "CM301_blocks_io" VALUES('G_BC104','pos',0,'','0');
INSERT INTO "CM301_blocks_io" VALUES('PP101','Kpr',0,'','0.95');
INSERT INTO "CM301_blocks_io" VALUES('PP102','Kpr',0,'','0.95');
INSERT INTO "CM301_blocks_io" VALUES('G_BC104','com',0,'','0');
INSERT INTO "CM301_blocks_io" VALUES('G_BC104','st_open',0,'','0');
INSERT INTO "CM301_blocks_io" VALUES('G_BC104','st_close',0,'','1');
INSERT INTO "CM301_blocks_io" VALUES('G_BC104','t_full',0,'','5');
INSERT INTO "CM301_blocks_io" VALUES('G_BC104','t_up',0,'','0.5');
INSERT INTO "CM301_blocks_io" VALUES('G_BC104','f_frq',0,'','100');
INSERT INTO "CM301_blocks_io" VALUES('G_BC104','tmp_up',0,'','0');
INSERT INTO "CM301_blocks_io" VALUES('G_BC104','lst_com',0,'','0');
INSERT INTO "CM301_blocks_io" VALUES('G_BC105','pos',0,'','0');
INSERT INTO "CM301_blocks_io" VALUES('G_BC105','com',0,'','0');
INSERT INTO "CM301_blocks_io" VALUES('G_BC105','st_open',0,'','0');
INSERT INTO "CM301_blocks_io" VALUES('G_BC105','st_close',0,'','1');
INSERT INTO "CM301_blocks_io" VALUES('G_BC105','t_full',0,'','5');
INSERT INTO "CM301_blocks_io" VALUES('G_BC105','t_up',0,'','0.5');
INSERT INTO "CM301_blocks_io" VALUES('G_BC105','f_frq',0,'','100');
INSERT INTO "CM301_blocks_io" VALUES('G_BC105','tmp_up',0,'','0');
INSERT INTO "CM301_blocks_io" VALUES('G_BC105','lst_com',0,'','0');
INSERT INTO "CM301_blocks_io" VALUES('G_BC101','pos',0,'','0');
INSERT INTO "CM301_blocks_io" VALUES('G_BC101','com',0,'','0');
INSERT INTO "CM301_blocks_io" VALUES('G_BC101','st_open',0,'','0');
INSERT INTO "CM301_blocks_io" VALUES('G_BC101','st_close',0,'','1');
INSERT INTO "CM301_blocks_io" VALUES('G_BC101','t_full',0,'','5');
INSERT INTO "CM301_blocks_io" VALUES('G_BC101','t_up',0,'','0.5');
INSERT INTO "CM301_blocks_io" VALUES('G_BC101','f_frq',0,'','100');
INSERT INTO "CM301_blocks_io" VALUES('G_BC101','tmp_up',0,'','0');
INSERT INTO "CM301_blocks_io" VALUES('G_BC101','lst_com',0,'','0');
INSERT INTO "CM301_blocks_io" VALUES('G_BC106','pos',0,'','0');
INSERT INTO "CM301_blocks_io" VALUES('G_BC106','com',0,'','0');
INSERT INTO "CM301_blocks_io" VALUES('G_BC106','st_open',0,'','0');
INSERT INTO "CM301_blocks_io" VALUES('G_BC106','st_close',0,'','1');
INSERT INTO "CM301_blocks_io" VALUES('G_BC106','t_full',0,'','5');
INSERT INTO "CM301_blocks_io" VALUES('G_BC106','t_up',0,'','0.5');
INSERT INTO "CM301_blocks_io" VALUES('G_BC106','f_frq',0,'','100');
INSERT INTO "CM301_blocks_io" VALUES('G_BC106','tmp_up',0,'','0');
INSERT INTO "CM301_blocks_io" VALUES('G_BC106','lst_com',0,'','0');
INSERT INTO "CM301_blocks_io" VALUES('G_BC102','pos',0,'','0');
INSERT INTO "CM301_blocks_io" VALUES('G_BC102','com',0,'','0');
INSERT INTO "CM301_blocks_io" VALUES('G_BC102','st_open',0,'','0');
INSERT INTO "CM301_blocks_io" VALUES('G_BC102','st_close',0,'','1');
INSERT INTO "CM301_blocks_io" VALUES('G_BC102','t_full',0,'','3');
INSERT INTO "CM301_blocks_io" VALUES('G_BC102','t_up',0,'','0.3');
INSERT INTO "CM301_blocks_io" VALUES('G_BC102','f_frq',0,'','100');
INSERT INTO "CM301_blocks_io" VALUES('G_BC102','tmp_up',0,'','0');
INSERT INTO "CM301_blocks_io" VALUES('G_BC102','lst_com',0,'','0');
CREATE TABLE 'CM301_prm' ("SHIFR" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '' ,"IO" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"ru#IO" TEXT DEFAULT '' ,"uk#IO" TEXT DEFAULT '' , PRIMARY KEY ("SHIFR"));
INSERT INTO "CM301_prm" VALUES('FN101','FN301','Flow at the output of compressor',1,'BC104.Fi:var:Variable','','Витрати на виході компресора','','Расход на выходе компрессора','BC104.Fi:var:Переменная','BC104.Fi:var:Змінна');
INSERT INTO "CM301_prm" VALUES('F101','F301','Flow on the diaphragm PP301',1,'PP101.Fi:var:Variable
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
INSERT INTO "CM301_prm" VALUES('F102','F302','Flow on the diaphragm PP302',1,'PP102.Fi:var:Variable
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
INSERT INTO "CM301_prm" VALUES('TE1313_1','TE1313_3','The temperature at the input of the first stage of the compressor',1,'node3_1.To:var:Variable
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
INSERT INTO "CM301_prm" VALUES('PT0202','PT0202_3','Pressure after the first stage of the compressor',1,'CM101_1.Po:var:Variable
*s.at:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.20:max:Maximum','','Тиск після першого ступеня компресору','','Давление после первой ступени компрессора','CM101_1.Po:var:Переменная
*s.ат:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.20:max:Максимум','CM101_1.Po:var:Змінна
*s.ат:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.20:max:Максимум');
INSERT INTO "CM301_prm" VALUES('PT0204','PT0204_3','Pressure after the second stage of the compressor',1,'CM101_2.Po:var:Variable
*s.at:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.50:max:Maximum','','Тиск після другого ступеня компресору','','Давление после второй ступени компрессора','CM101_2.Po:var:Переменная
*s.ат:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.50:max:Максимум','CM101_2.Po:var:Змінна
*s.ат:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.50:max:Максимум');
INSERT INTO "CM301_prm" VALUES('TE1314_1','TE1314_3','The temperature after the fridge of the first stage of the compressor',1,'AT101_1.To:var:Variable
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
INSERT INTO "CM301_prm" VALUES('PT1006_1','PT1006_3','Pressure on the diaphragm PP302',1,'PP102.Pi:var:Variable
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
INSERT INTO "CM301_prm" VALUES('ST8612','ST8612_3','Rotation speed of the compressor',1,'CM101_1.N:var:Variable
CM101_1.N:varIn:Variable input
*s.1000x rpm:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.10:max:Maximum','','Оберти компресору','','Обороты вращения компрессора','CM101_1.N:var:Переменная
CM101_1.N:varIn:Ввод переменной
*s.1000x об.мин.:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.10:max:Максимум','CM101_1.N:var:Змінна
CM101_1.N:varIn:Ввід змінної
*s.1000x об.хвил:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.10:max:Максимум');
INSERT INTO "CM301_prm" VALUES('KPP101','KPP301','Anti-surge control valve of the compressor',1,'KPP101.l_kl1:out:Output
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
INSERT INTO "CM301_prm" VALUES('TT0202','TE1202_3','Temperature after the first stage of the compressor',1,'CM101_1.To:var:Variable
*s.К:ed:Dimension
*i.0:prec:Precision
*r.273:min:Minimum
*r.373:max:Maximum','','Температура після першої ступені компресора','','Температура после первой ступени компрессора','CM101_1.To:var:Переменная
*s.К:ed:Ед.измерения
*i.0:prec:Точность
*r.273:min:Минимум
*r.373:max:Максимум','CM101_1.To:var:Змінна
*s.К:ed:Од.виміру
*i.0:prec:Точність
*r.273:min:Мінімум
*r.373:max:Максимум');
INSERT INTO "CM301_prm" VALUES('TT0204','TE1205_3','Temperature after the second stage of the compressor',1,'CM101_2.To:var:Variable
*s.К:ed:Dimension
*i.0:prec:Precision
*r.273:min:Minimum
*r.433:max:Maximum','','Температура після другої ступені компресора','','Температура после второй ступени компрессора','CM101_2.To:var:Переменная
*s.К:ed:Ед.измерения
*i.0:prec:Точность
*r.273:min:Минимум
*r.433:max:Максимум','CM101_2.To:var:Змінна
*s.К:ed:Од.виміру
*i.0:prec:Точність
*r.273:min:Мінімум
*r.433:max:Максимум');
INSERT INTO "CM301_prm" VALUES('AT101_1','AT301_1','Control block of the fridge after the first stage of the compressor',1,'AT101_1.Ti:Ti
AT101_1.To:To
AT101_1.Wc:Wc','','Блок контролю холодильника після першої ступені компресору','','Блок контроля холодильника после первой ступени компрессора','','');
INSERT INTO "CM301_prm" VALUES('AT101_2','AT301_2','Control block of the fridge after the second stage of the compressor',1,'AT101_2.Ti:Ti
AT101_2.To:To
AT101_2.Wc:Wc','','Блок контролю холодильника після другої ступені компресору','','Блок контроля холодильника после второй ступени компрессора','','');
INSERT INTO "CM301_prm" VALUES('BC101','BC301','Control block of the ball crane BC301',1,'G_BC101.com:com
G_BC101.st_open:st_open
G_BC101.st_close:st_close','КК301','Блок керування кульовим краном КК301','КШ301','Блок управления шарового крана КШ301','','');
INSERT INTO "CM301_prm" VALUES('BC102','BC302','Control block of the ball crane BC302',1,'G_BC102.com:com
G_BC102.st_open:st_open
G_BC102.st_close:st_close','КК302','Блок керування кульовим краном КК302','КШ302','Блок управления шарового крана КШ302','','');
INSERT INTO "CM301_prm" VALUES('BC104','BC304','Control block of the ball crane BC304',1,'G_BC104.com:com
G_BC104.st_open:st_open
G_BC104.st_close:st_close','КК304','Блок керування кульовим краном КК304','КШ304','Блок управления шарового крана КШ304','','');
INSERT INTO "CM301_prm" VALUES('BC105','BC305','Control block of the ball crane BC305',1,'G_BC105.com:com
G_BC105.st_open:st_open
G_BC105.st_close:st_close','КК305','Блок керування кульовим краном КК305','КШ305','Блок управления шарового крана КШ305','','');
INSERT INTO "CM301_prm" VALUES('BC106','BC306','Control block of the ball crane BC306',1,'G_BC106.com:com
G_BC106.st_open:st_open
G_BC106.st_close:st_close','КК306','Блок керування кульовим краном КК306','КШ306','Блок управления шарового крана КШ306','','');
INSERT INTO "CM301_prm" VALUES('TE1314_2','TE1206_3','The temperature after the fridge of the second stage of the compressor',1,'AT101_2.To:var:Variable
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
INSERT INTO "CM301_prm" VALUES('P101','P301','Pressure on the diaphragm PP301',1,'PP101.Pi:var:Variable
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
CREATE TABLE 'CM302_blocks_io' ("BLK_ID" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"TLNK" INTEGER DEFAULT '' ,"LNK" TEXT DEFAULT '' ,"VAL" TEXT DEFAULT '' , PRIMARY KEY ("BLK_ID","ID"));
INSERT INTO "CM302_blocks_io" VALUES('KPP102','Fi',0,'','5.86071516722934e-218');
INSERT INTO "CM302_blocks_io" VALUES('KPP102','Pi',1,'node6_1.Po2','23.9733592293339');
INSERT INTO "CM302_blocks_io" VALUES('KPP102','Ti',1,'AT102_2.To','282.999999585376');
INSERT INTO "CM302_blocks_io" VALUES('KPP102','Fo',1,'node5_1.Fi2','0');
INSERT INTO "CM302_blocks_io" VALUES('KPP102','Po',0,'','23.9735548122435');
INSERT INTO "CM302_blocks_io" VALUES('KPP102','To',0,'','273.000002553873');
INSERT INTO "CM302_blocks_io" VALUES('KPP102','So',0,'','0.017');
INSERT INTO "CM302_blocks_io" VALUES('KPP102','lo',0,'','10');
INSERT INTO "CM302_blocks_io" VALUES('KPP102','S_kl1',0,'','0.015');
INSERT INTO "CM302_blocks_io" VALUES('KPP102','S_kl2',0,'','0.015');
INSERT INTO "CM302_blocks_io" VALUES('KPP102','l_kl1',2,'gen_cntr.SurgeCM302.out','0');
INSERT INTO "CM302_blocks_io" VALUES('KPP102','l_kl2',0,'','0');
INSERT INTO "CM302_blocks_io" VALUES('KPP102','Q0',0,'','1');
INSERT INTO "CM302_blocks_io" VALUES('KPP102','Kln',0,'','2');
INSERT INTO "CM302_blocks_io" VALUES('KPP102','Kpr',0,'','0.95');
INSERT INTO "CM302_blocks_io" VALUES('KPP102','Ct',0,'','20');
INSERT INTO "CM302_blocks_io" VALUES('KPP102','Riz',0,'','20');
INSERT INTO "CM302_blocks_io" VALUES('KPP102','noBack',0,'','0');
INSERT INTO "CM302_blocks_io" VALUES('KPP102','Fwind',0,'','1');
INSERT INTO "CM302_blocks_io" VALUES('KPP102','Twind',0,'','273');
INSERT INTO "CM302_blocks_io" VALUES('KPP102','f_frq',0,'','100');
INSERT INTO "CM302_blocks_io" VALUES('KPP102','Pot',0,'','26.868606147988');
INSERT INTO "CM302_blocks_io" VALUES('KPP102','Fit',0,'','-0.219664239685689');
INSERT INTO "CM302_blocks_io" VALUES('net102','Fi',0,'','-2.65508308383976e-06');
INSERT INTO "CM302_blocks_io" VALUES('net102','Pi',1,'BC113.Po','0.99999992597075');
INSERT INTO "CM302_blocks_io" VALUES('net102','Po',0,'','1');
INSERT INTO "CM302_blocks_io" VALUES('net102','So',0,'','0.01');
INSERT INTO "CM302_blocks_io" VALUES('net102','Kpr',0,'','0.95');
INSERT INTO "CM302_blocks_io" VALUES('net102','Noise',0,'','1');
INSERT INTO "CM302_blocks_io" VALUES('net102','Q0',0,'','1');
INSERT INTO "CM302_blocks_io" VALUES('net102','f_frq',0,'','100');
INSERT INTO "CM302_blocks_io" VALUES('DR101','Fi',0,'','-0.000102789056962486');
INSERT INTO "CM302_blocks_io" VALUES('DR101','Pi',1,'BC116.Po','23.9733671205768');
INSERT INTO "CM302_blocks_io" VALUES('DR101','Ti',1,'BC116.To','273');
INSERT INTO "CM302_blocks_io" VALUES('DR101','Fo',1,'node5_1.Fi1','0.000424212558576136');
INSERT INTO "CM302_blocks_io" VALUES('DR101','Po',0,'','23.9733805674584');
INSERT INTO "CM302_blocks_io" VALUES('DR101','To',0,'','272.999999999798');
INSERT INTO "CM302_blocks_io" VALUES('DR101','So',0,'','0.0314');
INSERT INTO "CM302_blocks_io" VALUES('DR101','lo',0,'','3');
INSERT INTO "CM302_blocks_io" VALUES('DR101','S_kl1',0,'','0.00314');
INSERT INTO "CM302_blocks_io" VALUES('DR101','S_kl2',0,'','0.00314');
INSERT INTO "CM302_blocks_io" VALUES('DR101','l_kl1',0,'','50');
INSERT INTO "CM302_blocks_io" VALUES('DR101','l_kl2',0,'','0');
INSERT INTO "CM302_blocks_io" VALUES('DR101','Q0',0,'','1');
INSERT INTO "CM302_blocks_io" VALUES('DR101','Kln',0,'','1');
INSERT INTO "CM302_blocks_io" VALUES('DR101','Kpr',0,'','0.95');
INSERT INTO "CM302_blocks_io" VALUES('DR101','Ct',0,'','20');
INSERT INTO "CM302_blocks_io" VALUES('DR101','Riz',0,'','20');
INSERT INTO "CM302_blocks_io" VALUES('DR101','noBack',0,'','0');
INSERT INTO "CM302_blocks_io" VALUES('DR101','Fwind',0,'','1');
INSERT INTO "CM302_blocks_io" VALUES('DR101','Twind',0,'','273');
INSERT INTO "CM302_blocks_io" VALUES('DR101','f_frq',0,'','100');
INSERT INTO "CM302_blocks_io" VALUES('DR101','Pot',0,'','26.8469800335237');
INSERT INTO "CM302_blocks_io" VALUES('DR101','Fit',0,'','-0.187548356187166');
INSERT INTO "CM302_blocks_io" VALUES('С102_1','Fi',0,'','-6.11194568043596e-05');
INSERT INTO "CM302_blocks_io" VALUES('С102_1','Pi',1,'node5_1.Po','23.9733845147241');
INSERT INTO "CM302_blocks_io" VALUES('С102_1','Si',0,'','0.0314');
INSERT INTO "CM302_blocks_io" VALUES('С102_1','Fo',1,'PP103.Fi','-2.19932473170221e-05');
INSERT INTO "CM302_blocks_io" VALUES('С102_1','Po',0,'','23.9733836085613');
INSERT INTO "CM302_blocks_io" VALUES('С102_1','So',0,'','0.0314');
INSERT INTO "CM302_blocks_io" VALUES('С102_1','lo',0,'','40');
INSERT INTO "CM302_blocks_io" VALUES('С102_1','Fo_ж',0,'','0');
INSERT INTO "CM302_blocks_io" VALUES('С102_1','Po_ж',0,'','23.9830149785244');
INSERT INTO "CM302_blocks_io" VALUES('С102_1','Lж',0,'','4.01307081794422');
INSERT INTO "CM302_blocks_io" VALUES('С102_1','ProcЖ',0,'','0.01');
INSERT INTO "CM302_blocks_io" VALUES('С102_1','Vap',0,'','2.4');
INSERT INTO "CM302_blocks_io" VALUES('С102_1','Q0',0,'','1');
INSERT INTO "CM302_blocks_io" VALUES('С102_1','Qж',0,'','1000');
INSERT INTO "CM302_blocks_io" VALUES('С102_1','f_frq',0,'','100');
INSERT INTO "CM302_blocks_io" VALUES('С102_1','Pot',0,'','26.8150252942661');
INSERT INTO "CM302_blocks_io" VALUES('С102_1','Fit',0,'','0.0136804199541132');
INSERT INTO "CM302_blocks_io" VALUES('AT102_1','Fi',0,'','-0.00148071186689907');
INSERT INTO "CM302_blocks_io" VALUES('AT102_1','Pi',1,'CM102_1.Po','23.9733837262168');
INSERT INTO "CM302_blocks_io" VALUES('AT102_1','Ti',1,'CM102_1.To','273.073976691546');
INSERT INTO "CM302_blocks_io" VALUES('AT102_1','Si',0,'','0.015');
INSERT INTO "CM302_blocks_io" VALUES('AT102_1','li',0,'','10');
INSERT INTO "CM302_blocks_io" VALUES('AT102_1','Fo',1,'C102_2.Fi','6.33531084924308e-05');
INSERT INTO "CM302_blocks_io" VALUES('AT102_1','Po',0,'','23.9732758933537');
INSERT INTO "CM302_blocks_io" VALUES('AT102_1','To',0,'','283.000000274688');
INSERT INTO "CM302_blocks_io" VALUES('AT102_1','So',0,'','0.017');
INSERT INTO "CM302_blocks_io" VALUES('AT102_1','lo',0,'','2');
INSERT INTO "CM302_blocks_io" VALUES('AT102_1','Tair',0,'','283');
INSERT INTO "CM302_blocks_io" VALUES('AT102_1','Wc',0,'','200');
INSERT INTO "CM302_blocks_io" VALUES('AT102_1','Q0',0,'','1');
INSERT INTO "CM302_blocks_io" VALUES('AT102_1','Ct',0,'','100');
INSERT INTO "CM302_blocks_io" VALUES('AT102_1','Rt',0,'','1');
INSERT INTO "CM302_blocks_io" VALUES('AT102_1','f_frq',0,'','100');
INSERT INTO "CM302_blocks_io" VALUES('AT102_1','Pot',0,'','26.8152701962305');
INSERT INTO "CM302_blocks_io" VALUES('AT102_1','Fit',0,'','-0.00362841690536941');
INSERT INTO "CM302_blocks_io" VALUES('AT102_2','Fi',0,'','0.000593166627156315');
INSERT INTO "CM302_blocks_io" VALUES('AT102_2','Pi',1,'CM102_2.Po','23.9733746369832');
INSERT INTO "CM302_blocks_io" VALUES('AT102_2','Ti',1,'CM102_2.To','274.25337738304');
INSERT INTO "CM302_blocks_io" VALUES('AT102_2','Si',0,'','0.007');
INSERT INTO "CM302_blocks_io" VALUES('AT102_2','li',0,'','20');
INSERT INTO "CM302_blocks_io" VALUES('AT102_2','Fo',1,'C102_3.Fi','0.00022392663203655');
INSERT INTO "CM302_blocks_io" VALUES('AT102_2','Po',0,'','23.9734790481846');
INSERT INTO "CM302_blocks_io" VALUES('AT102_2','To',0,'','282.999999595592');
INSERT INTO "CM302_blocks_io" VALUES('AT102_2','So',0,'','0.00785');
INSERT INTO "CM302_blocks_io" VALUES('AT102_2','lo',0,'','5');
INSERT INTO "CM302_blocks_io" VALUES('AT102_2','Tair',0,'','283');
INSERT INTO "CM302_blocks_io" VALUES('AT102_2','Wc',0,'','200');
INSERT INTO "CM302_blocks_io" VALUES('AT102_2','Q0',0,'','1');
INSERT INTO "CM302_blocks_io" VALUES('AT102_2','Ct',0,'','100');
INSERT INTO "CM302_blocks_io" VALUES('AT102_2','Rt',0,'','1');
INSERT INTO "CM302_blocks_io" VALUES('AT102_2','f_frq',0,'','100');
INSERT INTO "CM302_blocks_io" VALUES('AT102_2','Pot',0,'','26.8286390373224');
INSERT INTO "CM302_blocks_io" VALUES('AT102_2','Fit',0,'','-0.314505704700398');
INSERT INTO "CM302_blocks_io" VALUES('BC111','Fi',0,'','-1.23516411460312e-321');
INSERT INTO "CM302_blocks_io" VALUES('BC111','Pi',0,'','23.7482765708766');
INSERT INTO "CM302_blocks_io" VALUES('BC111','Ti',2,'gen.BC21.To','305.049381818534');
INSERT INTO "CM302_blocks_io" VALUES('BC111','Fo',1,'node5_1.Fi3','0.000125158080717839');
INSERT INTO "CM302_blocks_io" VALUES('BC111','Po',0,'','23.9733842894812');
INSERT INTO "CM302_blocks_io" VALUES('BC111','To',0,'','273.000000001701');
INSERT INTO "CM302_blocks_io" VALUES('BC111','So',0,'','0.0314');
INSERT INTO "CM302_blocks_io" VALUES('BC111','lo',0,'','5');
INSERT INTO "CM302_blocks_io" VALUES('BC111','S_kl1',0,'','0.0314');
INSERT INTO "CM302_blocks_io" VALUES('BC111','S_kl2',0,'','0.002');
INSERT INTO "CM302_blocks_io" VALUES('BC111','l_kl1',1,'G_BC111.pos','0');
INSERT INTO "CM302_blocks_io" VALUES('BC111','l_kl2',1,'G_BC112.pos','0');
INSERT INTO "CM302_blocks_io" VALUES('BC111','Q0',0,'','1');
INSERT INTO "CM302_blocks_io" VALUES('BC111','Kln',0,'','1');
INSERT INTO "CM302_blocks_io" VALUES('BC111','Kpr',0,'','0.95');
INSERT INTO "CM302_blocks_io" VALUES('BC111','Ct',0,'','20');
INSERT INTO "CM302_blocks_io" VALUES('BC111','Riz',0,'','20');
INSERT INTO "CM302_blocks_io" VALUES('BC111','noBack',0,'','0');
INSERT INTO "CM302_blocks_io" VALUES('BC111','Fwind',0,'','1');
INSERT INTO "CM302_blocks_io" VALUES('BC111','Twind',0,'','273');
INSERT INTO "CM302_blocks_io" VALUES('BC111','f_frq',0,'','100');
INSERT INTO "CM302_blocks_io" VALUES('BC111','Pot',0,'','26.8086695292603');
INSERT INTO "CM302_blocks_io" VALUES('BC111','Fit',0,'','0');
INSERT INTO "CM302_blocks_io" VALUES('BC113','Fi',0,'','0');
INSERT INTO "CM302_blocks_io" VALUES('BC113','Pi',1,'node6_1.Po4','23.9733450791947');
INSERT INTO "CM302_blocks_io" VALUES('BC113','Ti',0,'','273');
INSERT INTO "CM302_blocks_io" VALUES('BC113','Fo',1,'net102.Fi','2.64637065819175e-06');
INSERT INTO "CM302_blocks_io" VALUES('BC113','Po',0,'','0.999999925223652');
INSERT INTO "CM302_blocks_io" VALUES('BC113','To',0,'','272.999999998982');
INSERT INTO "CM302_blocks_io" VALUES('BC113','So',0,'','0.00785');
INSERT INTO "CM302_blocks_io" VALUES('BC113','lo',0,'','10');
INSERT INTO "CM302_blocks_io" VALUES('BC113','S_kl1',0,'','0.00785');
INSERT INTO "CM302_blocks_io" VALUES('BC113','S_kl2',0,'','0.002');
INSERT INTO "CM302_blocks_io" VALUES('BC113','l_kl1',0,'','0');
INSERT INTO "CM302_blocks_io" VALUES('BC113','l_kl2',0,'','0');
INSERT INTO "CM302_blocks_io" VALUES('BC113','Q0',0,'','1');
INSERT INTO "CM302_blocks_io" VALUES('BC113','Kln',0,'','1');
INSERT INTO "CM302_blocks_io" VALUES('BC113','Kpr',0,'','0.95');
INSERT INTO "CM302_blocks_io" VALUES('BC113','Ct',0,'','20');
INSERT INTO "CM302_blocks_io" VALUES('BC113','Riz',0,'','20');
INSERT INTO "CM302_blocks_io" VALUES('BC113','noBack',0,'','0');
INSERT INTO "CM302_blocks_io" VALUES('BC113','Fwind',0,'','1');
INSERT INTO "CM302_blocks_io" VALUES('BC113','Twind',0,'','273');
INSERT INTO "CM302_blocks_io" VALUES('BC113','f_frq',0,'','100');
INSERT INTO "CM302_blocks_io" VALUES('BC113','Pot',0,'','1.00004217473475');
INSERT INTO "CM302_blocks_io" VALUES('BC113','Fit',0,'','0');
INSERT INTO "CM302_blocks_io" VALUES('BC114','Fi',0,'','0');
INSERT INTO "CM302_blocks_io" VALUES('BC114','Pi',1,'node6_1.Po3','23.9733938957116');
INSERT INTO "CM302_blocks_io" VALUES('BC114','Ti',1,'AT102_2.To','282.999999595592');
INSERT INTO "CM302_blocks_io" VALUES('BC114','Fo',0,'','0.00688231192978354');
INSERT INTO "CM302_blocks_io" VALUES('BC114','Po',0,'','92.2918580753924');
INSERT INTO "CM302_blocks_io" VALUES('BC114','To',0,'','273.000000010185');
INSERT INTO "CM302_blocks_io" VALUES('BC114','So',0,'','0.00785');
INSERT INTO "CM302_blocks_io" VALUES('BC114','lo',0,'','100');
INSERT INTO "CM302_blocks_io" VALUES('BC114','S_kl1',0,'','0.00785');
INSERT INTO "CM302_blocks_io" VALUES('BC114','S_kl2',0,'','0.002');
INSERT INTO "CM302_blocks_io" VALUES('BC114','l_kl1',1,'G_BC114.pos','0');
INSERT INTO "CM302_blocks_io" VALUES('BC114','l_kl2',1,'G_BC115.pos','0');
INSERT INTO "CM302_blocks_io" VALUES('BC114','Q0',0,'','1');
INSERT INTO "CM302_blocks_io" VALUES('BC114','Kln',0,'','1');
INSERT INTO "CM302_blocks_io" VALUES('BC114','Kpr',0,'','0.95');
INSERT INTO "CM302_blocks_io" VALUES('BC114','Ct',0,'','20');
INSERT INTO "CM302_blocks_io" VALUES('BC114','Riz',0,'','20');
INSERT INTO "CM302_blocks_io" VALUES('BC114','noBack',0,'','1');
INSERT INTO "CM302_blocks_io" VALUES('BC114','Fwind',0,'','1');
INSERT INTO "CM302_blocks_io" VALUES('BC114','Twind',0,'','273');
INSERT INTO "CM302_blocks_io" VALUES('BC114','f_frq',0,'','100');
INSERT INTO "CM302_blocks_io" VALUES('BC114','Pot',0,'','91.8399723466015');
INSERT INTO "CM302_blocks_io" VALUES('BC114','Fit',0,'','2.47032822920623e-322');
INSERT INTO "CM302_blocks_io" VALUES('BC116','Fi',0,'','0');
INSERT INTO "CM302_blocks_io" VALUES('BC116','Pi',1,'node6_1.Po1','23.9733898378474');
INSERT INTO "CM302_blocks_io" VALUES('BC116','Ti',1,'AT102_2.To','282.999999585376');
INSERT INTO "CM302_blocks_io" VALUES('BC116','Fo',1,'DR101.Fi','-1.75195317311771e-05');
INSERT INTO "CM302_blocks_io" VALUES('BC116','Po',0,'','23.9733671205768');
INSERT INTO "CM302_blocks_io" VALUES('BC116','To',0,'','273');
INSERT INTO "CM302_blocks_io" VALUES('BC116','So',0,'','0.0078');
INSERT INTO "CM302_blocks_io" VALUES('BC116','lo',0,'','1');
INSERT INTO "CM302_blocks_io" VALUES('BC116','S_kl1',0,'','0.0078');
INSERT INTO "CM302_blocks_io" VALUES('BC116','S_kl2',0,'','0.0078');
INSERT INTO "CM302_blocks_io" VALUES('BC116','l_kl1',1,'G_BC116.pos','0');
INSERT INTO "CM302_blocks_io" VALUES('BC116','l_kl2',0,'','0');
INSERT INTO "CM302_blocks_io" VALUES('BC116','Q0',0,'','1');
INSERT INTO "CM302_blocks_io" VALUES('BC116','Kln',0,'','1');
INSERT INTO "CM302_blocks_io" VALUES('BC116','Kpr',0,'','0.95');
INSERT INTO "CM302_blocks_io" VALUES('BC116','Ct',0,'','20');
INSERT INTO "CM302_blocks_io" VALUES('BC116','Riz',0,'','20');
INSERT INTO "CM302_blocks_io" VALUES('BC116','noBack',0,'','0');
INSERT INTO "CM302_blocks_io" VALUES('BC116','Fwind',0,'','1');
INSERT INTO "CM302_blocks_io" VALUES('BC116','Twind',0,'','273');
INSERT INTO "CM302_blocks_io" VALUES('BC116','f_frq',0,'','100');
INSERT INTO "CM302_blocks_io" VALUES('BC116','Pot',0,'','26.8956438310409');
INSERT INTO "CM302_blocks_io" VALUES('BC116','Fit',0,'','0');
INSERT INTO "CM302_blocks_io" VALUES('node5_1','Fi1',0,'','0.000311830758734962');
INSERT INTO "CM302_blocks_io" VALUES('node5_1','Pi1',1,'DR101.Po','23.9733680963553');
INSERT INTO "CM302_blocks_io" VALUES('node5_1','Ti1',1,'DR101.To','272.999999999798');
INSERT INTO "CM302_blocks_io" VALUES('node5_1','Si1',0,'','0.0314');
INSERT INTO "CM302_blocks_io" VALUES('node5_1','Fi2',0,'','0');
INSERT INTO "CM302_blocks_io" VALUES('node5_1','Pi2',1,'KPP102.Po','23.9735548122435');
INSERT INTO "CM302_blocks_io" VALUES('node5_1','Ti2',1,'KPP102.To','273.00000255384');
INSERT INTO "CM302_blocks_io" VALUES('node5_1','Si2',0,'','0');
INSERT INTO "CM302_blocks_io" VALUES('node5_1','Fi3',0,'','6.58426846864096e-05');
INSERT INTO "CM302_blocks_io" VALUES('node5_1','Pi3',1,'BC111.Po','23.9733799381625');
INSERT INTO "CM302_blocks_io" VALUES('node5_1','Ti3',1,'BC111.To','273.000000001701');
INSERT INTO "CM302_blocks_io" VALUES('node5_1','Si3',0,'','0.0314');
INSERT INTO "CM302_blocks_io" VALUES('node5_1','Fo',1,'С102_1.Fi','-8.23398122625715e-05');
INSERT INTO "CM302_blocks_io" VALUES('node5_1','Po',0,'','23.9733845147241');
INSERT INTO "CM302_blocks_io" VALUES('node5_1','To',0,'','272.999999994377');
INSERT INTO "CM302_blocks_io" VALUES('node5_1','So',0,'','0.2');
INSERT INTO "CM302_blocks_io" VALUES('node5_1','lo',0,'','50');
INSERT INTO "CM302_blocks_io" VALUES('node5_1','Q0',0,'','1');
INSERT INTO "CM302_blocks_io" VALUES('node5_1','Ct',0,'','20');
INSERT INTO "CM302_blocks_io" VALUES('node5_1','Riz',0,'','20');
INSERT INTO "CM302_blocks_io" VALUES('node5_1','Fwind',0,'','1');
INSERT INTO "CM302_blocks_io" VALUES('node5_1','Twind',0,'','273');
INSERT INTO "CM302_blocks_io" VALUES('node5_1','f_frq',0,'','100');
INSERT INTO "CM302_blocks_io" VALUES('node5_1','FoR',0,'','-0.00037523449854531');
INSERT INTO "CM302_blocks_io" VALUES('node5_1','Pot',0,'','26.8150708842158');
INSERT INTO "CM302_blocks_io" VALUES('node5_1','Fit',0,'','-0.360488757341288');
INSERT INTO "CM302_blocks_io" VALUES('CM102_1','Fi',0,'','-5.31388438405706e-05');
INSERT INTO "CM302_blocks_io" VALUES('CM102_1','Pi',1,'PP103.Po','23.9733833080179');
INSERT INTO "CM302_blocks_io" VALUES('CM102_1','Ti',0,'','273');
INSERT INTO "CM302_blocks_io" VALUES('CM102_1','Fo',1,'AT102_1.Fi','0.00147529772410769');
INSERT INTO "CM302_blocks_io" VALUES('CM102_1','Po',0,'','23.9733835205657');
INSERT INTO "CM302_blocks_io" VALUES('CM102_1','To',0,'','273.073976680767');
INSERT INTO "CM302_blocks_io" VALUES('CM102_1','So',0,'','0.017');
INSERT INTO "CM302_blocks_io" VALUES('CM102_1','lo',0,'','55');
INSERT INTO "CM302_blocks_io" VALUES('CM102_1','Kzp',0,'','0');
INSERT INTO "CM302_blocks_io" VALUES('CM102_1','N',0,'','0');
INSERT INTO "CM302_blocks_io" VALUES('CM102_1','V',0,'','0.27');
INSERT INTO "CM302_blocks_io" VALUES('CM102_1','Kpmp',0,'','0.02');
INSERT INTO "CM302_blocks_io" VALUES('CM102_1','Kslp',0,'','0.2');
INSERT INTO "CM302_blocks_io" VALUES('CM102_1','Q0',0,'','1');
INSERT INTO "CM302_blocks_io" VALUES('CM102_1','Ct',0,'','100');
INSERT INTO "CM302_blocks_io" VALUES('CM102_1','Riz',0,'','100');
INSERT INTO "CM302_blocks_io" VALUES('CM102_1','Fwind',0,'','1');
INSERT INTO "CM302_blocks_io" VALUES('CM102_1','Twind',0,'','273');
INSERT INTO "CM302_blocks_io" VALUES('CM102_1','f_frq',0,'','100');
INSERT INTO "CM302_blocks_io" VALUES('CM102_1','Pot',0,'','26.8147761231667');
INSERT INTO "CM302_blocks_io" VALUES('CM102_1','Fit',0,'','-0.0114304000242169');
INSERT INTO "CM302_blocks_io" VALUES('CM102_2','Fi',0,'','4.23456972330157e-05');
INSERT INTO "CM302_blocks_io" VALUES('CM102_2','Pi',1,'PP104.Po','23.9733854211067');
INSERT INTO "CM302_blocks_io" VALUES('CM102_2','Ti',1,'AT102_1.To','282.999999924696');
INSERT INTO "CM302_blocks_io" VALUES('CM102_2','Fo',1,'AT102_2.Fi','5.70320749156624e-05');
INSERT INTO "CM302_blocks_io" VALUES('CM102_2','Po',0,'','23.973370854836');
INSERT INTO "CM302_blocks_io" VALUES('CM102_2','To',0,'','274.253376997186');
INSERT INTO "CM302_blocks_io" VALUES('CM102_2','So',0,'','0.00785');
INSERT INTO "CM302_blocks_io" VALUES('CM102_2','lo',0,'','55');
INSERT INTO "CM302_blocks_io" VALUES('CM102_2','Kzp',0,'','0');
INSERT INTO "CM302_blocks_io" VALUES('CM102_2','N',1,'CM102_1.N','0');
INSERT INTO "CM302_blocks_io" VALUES('CM102_2','V',0,'','0.13');
INSERT INTO "CM302_blocks_io" VALUES('CM102_2','Kpmp',0,'','0.02');
INSERT INTO "CM302_blocks_io" VALUES('CM102_2','Kslp',0,'','0.2');
INSERT INTO "CM302_blocks_io" VALUES('CM102_2','Q0',0,'','1');
INSERT INTO "CM302_blocks_io" VALUES('CM102_2','Ct',0,'','100');
INSERT INTO "CM302_blocks_io" VALUES('CM102_2','Riz',0,'','100');
INSERT INTO "CM302_blocks_io" VALUES('CM102_2','Fwind',0,'','1');
INSERT INTO "CM302_blocks_io" VALUES('CM102_2','Twind',0,'','273');
INSERT INTO "CM302_blocks_io" VALUES('CM102_2','f_frq',0,'','100');
INSERT INTO "CM302_blocks_io" VALUES('CM102_2','Pot',0,'','26.8256832047174');
INSERT INTO "CM302_blocks_io" VALUES('CM102_2','Fit',0,'','-0.0211971803662871');
INSERT INTO "CM302_blocks_io" VALUES('node6_1','Fi',0,'','0.000950877355919065');
INSERT INTO "CM302_blocks_io" VALUES('node6_1','Pi',1,'C102_3.Po','23.9734036066456');
INSERT INTO "CM302_blocks_io" VALUES('node6_1','Fo1',1,'BC116.Fi','0');
INSERT INTO "CM302_blocks_io" VALUES('node6_1','Po1',0,'','23.9733942860316');
INSERT INTO "CM302_blocks_io" VALUES('node6_1','So1',0,'','0.00785');
INSERT INTO "CM302_blocks_io" VALUES('node6_1','lo1',0,'','30');
INSERT INTO "CM302_blocks_io" VALUES('node6_1','Fo2',1,'KPP102.Fi','5.85485445206211e-218');
INSERT INTO "CM302_blocks_io" VALUES('node6_1','Po2',0,'','23.9733506159992');
INSERT INTO "CM302_blocks_io" VALUES('node6_1','So2',0,'','0.002');
INSERT INTO "CM302_blocks_io" VALUES('node6_1','lo2',0,'','30');
INSERT INTO "CM302_blocks_io" VALUES('node6_1','Fo3',1,'BC114.Fi','0');
INSERT INTO "CM302_blocks_io" VALUES('node6_1','Po3',0,'','23.9734909517828');
INSERT INTO "CM302_blocks_io" VALUES('node6_1','So3',0,'','0.00785');
INSERT INTO "CM302_blocks_io" VALUES('node6_1','lo3',0,'','5');
INSERT INTO "CM302_blocks_io" VALUES('node6_1','Fo4',1,'BC113.Fi','0');
INSERT INTO "CM302_blocks_io" VALUES('node6_1','Po4',0,'','23.9733884171224');
INSERT INTO "CM302_blocks_io" VALUES('node6_1','So4',0,'','0.00785');
INSERT INTO "CM302_blocks_io" VALUES('node6_1','lo4',0,'','10');
INSERT INTO "CM302_blocks_io" VALUES('node6_1','Q0',0,'','1');
INSERT INTO "CM302_blocks_io" VALUES('node6_1','f_frq',0,'','100');
INSERT INTO "CM302_blocks_io" VALUES('node6_1','F1tmp',0,'','0.00019765730758779');
INSERT INTO "CM302_blocks_io" VALUES('node6_1','F2tmp',0,'','-8.3918810521251e-05');
INSERT INTO "CM302_blocks_io" VALUES('node6_1','F3tmp',0,'','0.000237153511484701');
INSERT INTO "CM302_blocks_io" VALUES('node6_1','F4tmp',0,'','0.000599985347367825');
INSERT INTO "CM302_blocks_io" VALUES('node6_1','Pot1',0,'','26.8225676097937');
INSERT INTO "CM302_blocks_io" VALUES('node6_1','Pot2',0,'','26.1732230241585');
INSERT INTO "CM302_blocks_io" VALUES('node6_1','Pot3',0,'','27.1570836553374');
INSERT INTO "CM302_blocks_io" VALUES('node6_1','Pot4',0,'','26.4318196988532');
INSERT INTO "CM302_blocks_io" VALUES('PP103','Fi',0,'','-2.19932473170221e-05');
INSERT INTO "CM302_blocks_io" VALUES('PP103','Pi',1,'С102_1.Po','23.973383697097');
INSERT INTO "CM302_blocks_io" VALUES('PP103','Fo',1,'CM102_1.Fi','2.89367251092001e-05');
INSERT INTO "CM302_blocks_io" VALUES('PP103','Po',0,'','23.9733843027348');
INSERT INTO "CM302_blocks_io" VALUES('PP103','dP',0,'','-3.00580792436037e-07');
INSERT INTO "CM302_blocks_io" VALUES('PP103','Sdf',0,'','0.02');
INSERT INTO "CM302_blocks_io" VALUES('PP103','So',0,'','0.0314');
INSERT INTO "CM302_blocks_io" VALUES('PP103','lo',0,'','7');
INSERT INTO "CM302_blocks_io" VALUES('PP103','Q0',0,'','1');
INSERT INTO "CM302_blocks_io" VALUES('PP103','f_frq',0,'','100');
INSERT INTO "CM302_blocks_io" VALUES('PP103','Pot',0,'','26.8149281286869');
INSERT INTO "CM302_blocks_io" VALUES('PP103','Fit',0,'','0.0265948323266263');
INSERT INTO "CM302_blocks_io" VALUES('PP104','Fi',0,'','-3.1280215385696e-05');
INSERT INTO "CM302_blocks_io" VALUES('PP104','Pi',1,'C102_2.Po','23.973384168098');
INSERT INTO "CM302_blocks_io" VALUES('PP104','Fo',1,'CM102_2.Fi','4.2314292430992e-05');
INSERT INTO "CM302_blocks_io" VALUES('PP104','Po',0,'','23.9733835453006');
INSERT INTO "CM302_blocks_io" VALUES('PP104','dP',0,'','-3.93470900016963e-07');
INSERT INTO "CM302_blocks_io" VALUES('PP104','Sdf',0,'','0.015');
INSERT INTO "CM302_blocks_io" VALUES('PP104','So',0,'','0.017');
INSERT INTO "CM302_blocks_io" VALUES('PP104','lo',0,'','7');
INSERT INTO "CM302_blocks_io" VALUES('PP104','Q0',0,'','1');
INSERT INTO "CM302_blocks_io" VALUES('PP104','f_frq',0,'','100');
INSERT INTO "CM302_blocks_io" VALUES('PP104','Pot',0,'','27.0443769735342');
INSERT INTO "CM302_blocks_io" VALUES('PP104','Fit',0,'','-1.63603222620315');
INSERT INTO "CM302_blocks_io" VALUES('C102_2','Fi',0,'','-3.42896315341529e-05');
INSERT INTO "CM302_blocks_io" VALUES('C102_2','Pi',1,'AT102_1.Po','23.9734059992306');
INSERT INTO "CM302_blocks_io" VALUES('C102_2','Si',0,'','0.017');
INSERT INTO "CM302_blocks_io" VALUES('C102_2','Fo',1,'PP104.Fi','0.000110193368643209');
INSERT INTO "CM302_blocks_io" VALUES('C102_2','Po',0,'','23.9733841497219');
INSERT INTO "CM302_blocks_io" VALUES('C102_2','So',0,'','0.017');
INSERT INTO "CM302_blocks_io" VALUES('C102_2','lo',0,'','50');
INSERT INTO "CM302_blocks_io" VALUES('C102_2','Fo_ж',0,'','0');
INSERT INTO "CM302_blocks_io" VALUES('C102_2','Po_ж',0,'','23.9830884685443');
INSERT INTO "CM302_blocks_io" VALUES('C102_2','Lж',0,'','4.85215941120694');
INSERT INTO "CM302_blocks_io" VALUES('C102_2','ProcЖ',0,'','0.01');
INSERT INTO "CM302_blocks_io" VALUES('C102_2','Vap',0,'','2');
INSERT INTO "CM302_blocks_io" VALUES('C102_2','Q0',0,'','1');
INSERT INTO "CM302_blocks_io" VALUES('C102_2','Qж',0,'','1000');
INSERT INTO "CM302_blocks_io" VALUES('C102_2','f_frq',0,'','100');
INSERT INTO "CM302_blocks_io" VALUES('C102_2','Pot',0,'','26.8110487968738');
INSERT INTO "CM302_blocks_io" VALUES('C102_2','Fit',0,'','-0.0290302511841726');
INSERT INTO "CM302_blocks_io" VALUES('C102_3','Fi',0,'','0.0003387276698836');
INSERT INTO "CM302_blocks_io" VALUES('C102_3','Pi',1,'AT102_2.Po','23.9734516346879');
INSERT INTO "CM302_blocks_io" VALUES('C102_3','Si',0,'','0.00785');
INSERT INTO "CM302_blocks_io" VALUES('C102_3','Fo',1,'node6_1.Fi','0.000552335548994284');
INSERT INTO "CM302_blocks_io" VALUES('C102_3','Po',0,'','23.9734524452684');
INSERT INTO "CM302_blocks_io" VALUES('C102_3','So',0,'','0.00785');
INSERT INTO "CM302_blocks_io" VALUES('C102_3','lo',0,'','8');
INSERT INTO "CM302_blocks_io" VALUES('C102_3','Fo_ж',0,'','0');
INSERT INTO "CM302_blocks_io" VALUES('C102_3','Po_ж',0,'','23.9838299761813');
INSERT INTO "CM302_blocks_io" VALUES('C102_3','Lж',0,'','6.91835394191689');
INSERT INTO "CM302_blocks_io" VALUES('C102_3','ProcЖ',0,'','0.01');
INSERT INTO "CM302_blocks_io" VALUES('C102_3','Vap',0,'','1.5');
INSERT INTO "CM302_blocks_io" VALUES('C102_3','Q0',0,'','1');
INSERT INTO "CM302_blocks_io" VALUES('C102_3','Qж',0,'','1000');
INSERT INTO "CM302_blocks_io" VALUES('C102_3','f_frq',0,'','100');
INSERT INTO "CM302_blocks_io" VALUES('C102_3','Pot',0,'','26.7868124450488');
INSERT INTO "CM302_blocks_io" VALUES('C102_3','Fit',0,'','0.313336706308645');
INSERT INTO "CM302_blocks_io" VALUES('G_BC112','pos',0,'','0');
INSERT INTO "CM302_blocks_io" VALUES('KPP102','t_kl1',0,'','3');
INSERT INTO "CM302_blocks_io" VALUES('KPP102','t_kl2',0,'','5');
INSERT INTO "CM302_blocks_io" VALUES('KPP102','tmp_l1',0,'','7.4109846876187e-322');
INSERT INTO "CM302_blocks_io" VALUES('KPP102','tmp_l2',0,'','0');
INSERT INTO "CM302_blocks_io" VALUES('DR101','t_kl1',0,'','10');
INSERT INTO "CM302_blocks_io" VALUES('DR101','t_kl2',0,'','5');
INSERT INTO "CM302_blocks_io" VALUES('DR101','tmp_l1',0,'','49.9999999999964');
INSERT INTO "CM302_blocks_io" VALUES('DR101','tmp_l2',0,'','0');
INSERT INTO "CM302_blocks_io" VALUES('G_BC114','pos',0,'','0');
INSERT INTO "CM302_blocks_io" VALUES('G_BC116','pos',0,'','0');
INSERT INTO "CM302_blocks_io" VALUES('BC111','t_kl1',0,'','10');
INSERT INTO "CM302_blocks_io" VALUES('BC111','t_kl2',0,'','5');
INSERT INTO "CM302_blocks_io" VALUES('BC111','tmp_l1',0,'','2.47032822920623e-321');
INSERT INTO "CM302_blocks_io" VALUES('BC111','tmp_l2',0,'','0');
INSERT INTO "CM302_blocks_io" VALUES('BC113','t_kl1',0,'','10');
INSERT INTO "CM302_blocks_io" VALUES('BC113','t_kl2',0,'','5');
INSERT INTO "CM302_blocks_io" VALUES('BC113','tmp_l1',0,'','0');
INSERT INTO "CM302_blocks_io" VALUES('BC113','tmp_l2',0,'','0');
INSERT INTO "CM302_blocks_io" VALUES('BC114','t_kl1',0,'','10');
INSERT INTO "CM302_blocks_io" VALUES('BC114','t_kl2',0,'','5');
INSERT INTO "CM302_blocks_io" VALUES('BC114','tmp_l1',0,'','2.47032822920623e-321');
INSERT INTO "CM302_blocks_io" VALUES('BC114','tmp_l2',0,'','0');
INSERT INTO "CM302_blocks_io" VALUES('BC116','t_kl1',0,'','10');
INSERT INTO "CM302_blocks_io" VALUES('BC116','t_kl2',0,'','5');
INSERT INTO "CM302_blocks_io" VALUES('BC116','tmp_l1',0,'','0');
INSERT INTO "CM302_blocks_io" VALUES('BC116','tmp_l2',0,'','0');
INSERT INTO "CM302_blocks_io" VALUES('G_BC111','pos',0,'','0');
INSERT INTO "CM302_blocks_io" VALUES('node5_1','Kpr',0,'','0.95');
INSERT INTO "CM302_blocks_io" VALUES('CM102_1','Kpr',0,'','0.95');
INSERT INTO "CM302_blocks_io" VALUES('CM102_2','Kpr',0,'','0.95');
INSERT INTO "CM302_blocks_io" VALUES('node6_1','Kpr',0,'','0.95');
INSERT INTO "CM302_blocks_io" VALUES('PP103','Kpr',0,'','0.95');
INSERT INTO "CM302_blocks_io" VALUES('PP104','Kpr',0,'','0.95');
INSERT INTO "CM302_blocks_io" VALUES('G_BC114','com',0,'','0');
INSERT INTO "CM302_blocks_io" VALUES('G_BC114','st_open',0,'','0');
INSERT INTO "CM302_blocks_io" VALUES('G_BC114','st_close',0,'','1');
INSERT INTO "CM302_blocks_io" VALUES('G_BC114','t_full',0,'','5');
INSERT INTO "CM302_blocks_io" VALUES('G_BC114','t_up',0,'','0.5');
INSERT INTO "CM302_blocks_io" VALUES('G_BC114','f_frq',0,'','100');
INSERT INTO "CM302_blocks_io" VALUES('G_BC114','tmp_up',0,'','0');
INSERT INTO "CM302_blocks_io" VALUES('G_BC114','lst_com',0,'','0');
INSERT INTO "CM302_blocks_io" VALUES('G_BC115','pos',0,'','0');
INSERT INTO "CM302_blocks_io" VALUES('G_BC115','com',0,'','0');
INSERT INTO "CM302_blocks_io" VALUES('G_BC115','st_open',0,'','0');
INSERT INTO "CM302_blocks_io" VALUES('G_BC115','st_close',0,'','1');
INSERT INTO "CM302_blocks_io" VALUES('G_BC115','t_full',0,'','5');
INSERT INTO "CM302_blocks_io" VALUES('G_BC115','t_up',0,'','0.5');
INSERT INTO "CM302_blocks_io" VALUES('G_BC115','f_frq',0,'','100');
INSERT INTO "CM302_blocks_io" VALUES('G_BC115','tmp_up',0,'','0');
INSERT INTO "CM302_blocks_io" VALUES('G_BC115','lst_com',0,'','0');
INSERT INTO "CM302_blocks_io" VALUES('G_BC111','com',0,'','0');
INSERT INTO "CM302_blocks_io" VALUES('G_BC111','st_open',0,'','0');
INSERT INTO "CM302_blocks_io" VALUES('G_BC111','st_close',0,'','1');
INSERT INTO "CM302_blocks_io" VALUES('G_BC111','t_full',0,'','5');
INSERT INTO "CM302_blocks_io" VALUES('G_BC111','t_up',0,'','0.5');
INSERT INTO "CM302_blocks_io" VALUES('G_BC111','f_frq',0,'','100');
INSERT INTO "CM302_blocks_io" VALUES('G_BC111','tmp_up',0,'','0');
INSERT INTO "CM302_blocks_io" VALUES('G_BC111','lst_com',0,'','0');
INSERT INTO "CM302_blocks_io" VALUES('G_BC116','com',0,'','0');
INSERT INTO "CM302_blocks_io" VALUES('G_BC116','st_open',0,'','0');
INSERT INTO "CM302_blocks_io" VALUES('G_BC116','st_close',0,'','1');
INSERT INTO "CM302_blocks_io" VALUES('G_BC116','t_full',0,'','5');
INSERT INTO "CM302_blocks_io" VALUES('G_BC116','t_up',0,'','0.5');
INSERT INTO "CM302_blocks_io" VALUES('G_BC116','f_frq',0,'','100');
INSERT INTO "CM302_blocks_io" VALUES('G_BC116','tmp_up',0,'','0');
INSERT INTO "CM302_blocks_io" VALUES('G_BC116','lst_com',0,'','0');
INSERT INTO "CM302_blocks_io" VALUES('G_BC112','com',0,'','0');
INSERT INTO "CM302_blocks_io" VALUES('G_BC112','st_open',0,'','0');
INSERT INTO "CM302_blocks_io" VALUES('G_BC112','st_close',0,'','1');
INSERT INTO "CM302_blocks_io" VALUES('G_BC112','t_full',0,'','3');
INSERT INTO "CM302_blocks_io" VALUES('G_BC112','t_up',0,'','0.3');
INSERT INTO "CM302_blocks_io" VALUES('G_BC112','f_frq',0,'','100');
INSERT INTO "CM302_blocks_io" VALUES('G_BC112','tmp_up',0,'','0');
INSERT INTO "CM302_blocks_io" VALUES('G_BC112','lst_com',0,'','0');
CREATE TABLE 'CM302_prm' ("SHIFR" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '' ,"IO" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"ru#IO" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"uk#IO" TEXT DEFAULT '' , PRIMARY KEY ("SHIFR"));
INSERT INTO "CM302_prm" VALUES('KPP102','KPP302','Anti-surge control valve of the compressor',1,'KPP102.l_kl1:out:Output
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
INSERT INTO "CM302_prm" VALUES('F103','F303','Flow on the diaphragm PP303',1,'PP103.Fi:var:Variable
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
INSERT INTO "CM302_prm" VALUES('TT0202','TE1202_6','Temperature after the first stage of the compressor',1,'CM102_1.To:var:Variable
*s.К:ed:Dimension
*i.0:prec:Precision
*r.273:min:Minimum
*r.373:max:Maximum','','Температура после первой ступени компрессора','CM102_1.To:var:Переменная
*s.К:ed:Ед.измерения
*i.0:prec:Точность
*r.273:min:Минимум
*r.373:max:Максимум','','Температура після першої ступені компресора','CM102_1.To:var:Змінна
*s.К:ed:Од.виміру
*i.0:prec:Точність
*r.273:min:Мінімум
*r.373:max:Максимум');
INSERT INTO "CM302_prm" VALUES('F104','F304','Flow on the diaphragm PP304',1,'PP104.Fi:var:Variable
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
INSERT INTO "CM302_prm" VALUES('TT0204','TE1205_6','Temperature after the second stage of the compressor',1,'CM102_2.To:var:Variable
*s.К:ed:Dimension
*i.0:prec:Precision
*r.273:min:Minimum
*r.423:max:Maximum','','Температура после второй ступени компрессора','CM102_2.To:var:Переменная
*s.К:ed:Ед.измерения
*i.0:prec:Точность
*r.273:min:Минимум
*r.423:max:Максимум','','Температура після другої ступені компресора','CM102_2.To:var:Змінна
*s.К:ed:Од.виміру
*i.0:prec:Точність
*r.273:min:Мінімум
*r.423:max:Максимум');
INSERT INTO "CM302_prm" VALUES('TE1313_1','TE1313_6','The temperature at the input of the first stage of the compressor',1,'node5_1.To:var:Variable
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
INSERT INTO "CM302_prm" VALUES('PT0202','PT0202_6','Pressure after the first stage of the compressor',1,'CM102_1.Po:var:Variable
*s.at:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.75:max:Maximum','','Давление после первой ступени компрессора','CM102_1.Po:var:Переменная
*s.ат:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.75:max:Максимум','','Тиск після першого ступеня компресору','CM102_1.Po:var:Змінна
*s.ат:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.75:max:Максимум');
INSERT INTO "CM302_prm" VALUES('BC111','BC311','Control block of the ball crane BC311',1,'G_BC111.com:com
G_BC111.st_open:st_open
G_BC111.st_close:st_close','КШ311','Блок управления шарового крана КШ311','','КК311','Блок керування кульовим краном КК311','');
INSERT INTO "CM302_prm" VALUES('PT0204','PT0204_6','Pressure after the second stage of the compressor',1,'CM102_2.Po:var:Variable
*s.at:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.150:max:Maximum','','Давление после второй ступени компрессора','CM102_2.Po:var:Переменная
*s.ат:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.150:max:Максимум','','Тиск після другого ступеня компресору','CM102_2.Po:var:Змінна
*s.ат:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.150:max:Максимум');
INSERT INTO "CM302_prm" VALUES('BC112','BC312','Control block of the ball crane BC312',1,'G_BC112.com:com
G_BC112.st_open:st_open
G_BC112.st_close:st_close','КШ312','Блок управления шарового крана КШ312','','КК312','Блок керування кульовим краном КК312','');
INSERT INTO "CM302_prm" VALUES('BC114','BC314','Control block of the ball crane BC314',1,'G_BC114.com:com
G_BC114.st_open:st_open
G_BC114.st_close:st_close','КШ314','Блок управления шарового крана КШ314','','КК314','Блок керування кульовим краном КК314','');
INSERT INTO "CM302_prm" VALUES('BC115','BC315','Control block of the ball crane BC315',1,'G_BC115.com:com
G_BC115.st_open:st_open
G_BC115.st_close:st_close','КШ315','Блок управления шарового крана КШ315','','КК315','Блок керування кульовим краном КК315','');
INSERT INTO "CM302_prm" VALUES('BC116','BC316','Control block of the ball crane BC316',1,'G_BC116.com:com
G_BC116.st_open:st_open
G_BC116.st_close:st_close','КШ316','Блок управления шарового крана КШ316','','КК316','Блок керування кульовим краном КК316','');
INSERT INTO "CM302_prm" VALUES('TE1314_1','TE1314_6','The temperature after the fridge of the first stage of the compressor',1,'AT102_1.To:var:Variable
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
INSERT INTO "CM302_prm" VALUES('TE1314_2','TE1206_6','The temperature after the fridge of the second stage of the compressor',1,'AT102_2.To:var:Variable
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
INSERT INTO "CM302_prm" VALUES('PT1006_1','PT1006_6','Pressure on the diaphragm PP304',1,'PP104.Pi:var:Variable
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
INSERT INTO "CM302_prm" VALUES('P103','P303','Pressure on the diaphragm PP303',1,'PP103.Pi:var:Variable
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
INSERT INTO "CM302_prm" VALUES('ST8612','ST8612_6','Rotation speed of the compressor',1,'CM102_1.N:var:Variable
CM102_1.N:varIn:Variable input
*s.1000x rpm:ed:Dimension
*i.1:prec:Precision
*r.0:min:Minimum
*r.15:max:Maximum','','Обороты вращения компрессора','CM102_1.N:var:Переменная
CM102_1.N:varIn:Ввод переменной
*s.1000x об.мин.:ed:Ед.измерения
*i.1:prec:Точность
*r.0:min:Минимум
*r.15:max:Максимум','','Оберти компресору','CM102_1.N:var:Змінна
CM102_1.N:varIn:Ввід змінної
*s.1000x об.хвил:ed:Од.виміру
*i.1:prec:Точність
*r.0:min:Мінімум
*r.15:max:Максимум');
CREATE TABLE 'DAQ_BlockCalc' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"ENABLE" INTEGER DEFAULT '0' ,"START" INTEGER DEFAULT '0' ,"MESS_LEV" INTEGER DEFAULT '3' ,"REDNT" INTEGER DEFAULT '0' ,"REDNT_RUN" TEXT DEFAULT '<high>' ,"PRM_BD" TEXT DEFAULT 'system' ,"BLOCK_SH" TEXT DEFAULT 'block' ,"PERIOD" INTEGER DEFAULT '0' ,"SCHEDULE" TEXT DEFAULT '1' ,"PRIOR" INTEGER DEFAULT '0' ,"ITER" INTEGER DEFAULT '1' , PRIMARY KEY ("ID"));
INSERT INTO "DAQ_BlockCalc" VALUES('gen','Mainstation','Загальностанційка','Общестанционка','Contains the model of the general part of the compressor station, that is all that is shown on the principle scheme besides the gray blocks of individual compressors.
Author: Roman Savochenko <rom_as@oscada.org>
Version: 2.0.0','Містить модель загальної частини компресорної станції, а саме все що зображено на принциповій схемі за межами сірих блоків окремих компресорів.
Автор: Роман Савоченко <rom_as@oscada.org>
Версія: 2.0.0','Содержит модель общей части компрессорной станции, а именно всё что изображено на принципиальной схеме за пределами серых блоков отдельных компрессоров.
Автор: Роман Савоченко <rom_as@oscada.org>
Версия: 2.0.0',1,1,3,0,'<high>','gen_prm','gen_blcks',0,'0.01',0,1);
INSERT INTO "DAQ_BlockCalc" VALUES('CM102cntr','','','','Author: Roman Savochenko <rom_as@oscada.org>
Version: 2.0.0','','
',1,1,3,0,'<high>','CM102cntr_prm','CM102cntr_blocks',0,'1',0,1);
INSERT INTO "DAQ_BlockCalc" VALUES('gen_cntr','Mainstation (controller)','Загальностанційка (контролер)','Общестанционка (контроллер)','Contains the model of the control system of the general part.
Author: Roman Savochenko <rom_as@oscada.org>
Version: 2.0.0','Містить модель системи керування загальностанційною частиною.
Автор: Роман Савоченко <rom_as@oscada.org>
Версія: 2.0.0','Содержит модель системы управления общестанционной частью.
Автор: Роман Савоченко <rom_as@oscada.org>
Версия: 2.0.0',1,1,3,0,'<high>','gen_cntr_prm','gen_cntr_blocks',0,'0.2',0,1);
INSERT INTO "DAQ_BlockCalc" VALUES('CM101','Model of CM101','Модель КМ101','Модель КМ101','Contains the model of the first compressor CM101 of low pressure.
Author: Roman Savochenko <rom_as@oscada.org>
Version: 2.0.0','Містить модель першого компресора низького тиску КМ101.
Автор: Роман Савоченко <rom_as@oscada.org>
Версія: 2.0.0','Содержит модель первого компрессора низкого давления КМ101.
Автор: Роман Савоченко <rom_as@oscada.org>
Версия: 2.0.0',1,1,3,0,'<high>','CM101_prm','CM101_blocks',0,'0.01',0,1);
INSERT INTO "DAQ_BlockCalc" VALUES('CM102','Model of CM102','Модель КМ102','Модель КМ102','Contains the model of the first compressor CM102 of high pressure.
Author: Roman Savochenko <rom_as@oscada.org>
Version: 2.0.0','Містить модель першого компресора високого тиску КМ102.
Автор: Роман Савоченко <rom_as@oscada.org>
Версія: 2.0.0','Содержит модель первого компрессора высокого давления КМ102.
Автор: Роман Савоченко <rom_as@oscada.org>
Версия: 2.0.0',1,1,3,0,'<high>','CM102_prm','CM102_blocks',0,'0.01',0,1);
INSERT INTO "DAQ_BlockCalc" VALUES('CM201','Model of CM201','Модель КМ201','Модель КМ201','Contains the model of the second compressor CM201 of low pressure.
Author: Roman Savochenko <rom_as@oscada.org>
Version: 2.0.0','Містить модель другого компресора низького тиску КМ201.
Автор: Роман Савоченко <rom_as@oscada.org>
Версія: 2.0.0','Содержит модель второго компрессора низкого давления КМ201.
Автор: Роман Савоченко <rom_as@oscada.org>
Версия: 2.0.0',1,1,3,0,'<high>','CM201_prm','CM201_blocks',0,'0.01',0,1);
INSERT INTO "DAQ_BlockCalc" VALUES('CM202','Model of CM202','Модель КМ202','Модель КМ202','Contains the model of the second compressor CM202 of high pressure.
Author: Roman Savochenko <rom_as@oscada.org>
Version: 2.0.0','Містить модель другого компресора високого тиску КМ202.
Автор: Роман Савоченко <rom_as@oscada.org>
Версія: 2.0.0','Содержит модель второго компрессора высокого давления КМ202.
Автор: Роман Савоченко <rom_as@oscada.org>
Версия: 2.0.0',1,1,3,0,'<high>','CM202_prm','CM202_blocks',0,'0.01',0,1);
INSERT INTO "DAQ_BlockCalc" VALUES('CM301','Model of CM301','Модель КМ301','Модель КМ301','Contains the model of the third compressor CM301 of low pressure.
Author: Roman Savochenko <rom_as@oscada.org>
Version: 2.0.0','Містить модель третього компресора низького тиску КМ301.
Автор: Роман Савоченко <rom_as@oscada.org>
Версія: 2.0.0','Содержит модель третьего компрессора низкого давления КМ301.
Автор: Роман Савоченко <rom_as@oscada.org>
Версия: 2.0.0',1,1,3,0,'<high>','CM301_prm','CM301_blocks',0,'0.01',0,1);
INSERT INTO "DAQ_BlockCalc" VALUES('CM302','Model of CM302','Модель КМ302','Модель КМ302','Contains the model of the third compressor CM302 of high pressure.
Author: Roman Savochenko <rom_as@oscada.org>
Version: 2.0.0','Містить модель третього компресора високого тиску КМ302.
Автор: Роман Савоченко <rom_as@oscada.org>
Версія: 2.0.0','Содержит модель третьего компрессора высокого давления КМ302.
Автор: Роман Савоченко <rom_as@oscada.org>
Версия: 2.0.0',1,1,3,0,'<high>','CM302_prm','CM302_blocks',0,'0.01',0,1);
CREATE TABLE 'gen_blcks' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"FUNC" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '0' ,"PROC" INTEGER DEFAULT '0' ,"PRIOR" TEXT DEFAULT '' ,"LNK_OUT_WR_CH" INTEGER DEFAULT '0' , PRIMARY KEY ("ID"));
INSERT INTO "gen_blcks" VALUES('node1_1','Node 1.1','Вузол 1.1','Узел 1.1','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_pipe1_3',1,1,'',0);
INSERT INTO "gen_blcks" VALUES('G_BC22','BC22 (control mechanism)','КК22 (вик. механізм)','КШ22 (исп. механизм)','Control mechanism of ball crane КSH22.','Виконавчий механізм кульового крану КК22.','Исполнительный механизм шарового крана КШ22.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "gen_blcks" VALUES('netHPG','Net of HPG','Мережа ГВТ','Сеть ГВД','','','','DAQ.JavaLikeCalc.lib_techApp.net',1,1,'',0);
INSERT INTO "gen_blcks" VALUES('BC21','BC21','КК21','КШ21','Ball crane armature for BC21 и BC22','Арматура кульових кранів КК21 та КК22','Арматура шаровых кранов КШ21 и КШ22','DAQ.JavaLikeCalc.lib_techApp.fnc_klap',1,1,'',0);
INSERT INTO "gen_blcks" VALUES('noisePP3','PP3 noise','Шум PP3','Шум PP3','Flow source noise for diaphragms PP3','Шум джерела витрат діафрагми PP3','Шум источника расхода диафрагмы PP3','DAQ.JavaLikeCalc.lib_techApp.fnc_noise',1,1,'',0);
INSERT INTO "gen_blcks" VALUES('G_PCV2','PCV2 (control mechanism)','КРТ2 (вик. механізм)','КРД2 (исп. механизм)','','','','DAQ.JavaLikeCalc.lib_techApp.fnc_lagClean',1,1,'',0);
INSERT INTO "gen_blcks" VALUES('netGlinsk','Glinsk (net)','Глінск (мережа)','Глинск (сеть)','','','','DAQ.JavaLikeCalc.lib_techApp.fnc_net',1,1,'',0);
INSERT INTO "gen_blcks" VALUES('G_BC6','BC6 (control mechanism)','КК6 (вик. механізм)','КШ6 (исп. механизм)','Control mechanism of ball crane BC6.','Виконавчий механізм кульового крану КК6.','Исполнительный механизм шарового крана КШ6.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "gen_blcks" VALUES('P_PCV2','P_PCV2 (Pressure sensor)','P КРТ2 (Давач тиску)','P КРД2 (Датчик давления)','','','','DAQ.JavaLikeCalc.lib_techApp.fnc_lag',1,1,'',0);
INSERT INTO "gen_blcks" VALUES('SrcGas','','','','Gas source to compressor station','Джерело газу на компресорну станцію.','Источник газа на компрессорную станцию','DAQ.JavaLikeCalc.lib_techApp.src_press',1,1,'',0);
INSERT INTO "gen_blcks" VALUES('PCV1','PCV1','КРТ1','КРД1','The crane for regulation the input pressure of  the compressor low pressure.','Клапан, який регулює вхідний тиск на всасі компресорів низького тиску.','Клапан, регулирующий входное давление на всасе компресоров низкого давления.','DAQ.JavaLikeCalc.lib_techApp.fnc_klap',1,1,'',0);
INSERT INTO "gen_blcks" VALUES('pipeGlinsk','Pipe to Glinsk','Труба на Глінск','Труба на Глинск','','','','DAQ.JavaLikeCalc.lib_techApp.fnc_pipe1_1',1,1,'',0);
INSERT INTO "gen_blcks" VALUES('PCV2','PCV2','КРТ2','КРД2','','','','DAQ.JavaLikeCalc.lib_techApp.fnc_klap',1,1,'',0);
INSERT INTO "gen_blcks" VALUES('PCV4','PCV4','КРТ4','КРД4','The control crane on outgo to GHP.','Регулюючий клапан на виході у ГВТ.','Регулирующий клапан на выходе в ГВД.','DAQ.JavaLikeCalc.lib_techApp.klap',1,1,'',0);
INSERT INTO "gen_blcks" VALUES('G_BC7','BC7 (control mechanism)','КК7 (вик. механізм)','КШ7 (исп. механизм)','Control mechanism of ball crane BC7.','Виконавчий механізм кульового крану КК7.','Исполнительный механизм шарового крана КШ7.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "gen_blcks" VALUES('PP1','','','','Flow measure point based on diaphragm.','Вузол виміру витрат на основі діафрагми.','Узел измерения расхода на основе диафрагмы.','DAQ.JavaLikeCalc.lib_techApp.diafragma',1,1,'',0);
INSERT INTO "gen_blcks" VALUES('PP3','PP3','PP3','','Flow measure point based on diaphragm.','Вимірювальна діафрагма на вході у вузол.','Измерительная диафрагма на входе в узел.','DAQ.JavaLikeCalc.lib_techApp.fnc_diafragma',1,1,'',0);
INSERT INTO "gen_blcks" VALUES('PP5','PP5','PP5','','','','','DAQ.JavaLikeCalc.lib_techApp.fnc_diafragma',1,1,'',0);
INSERT INTO "gen_blcks" VALUES('PP7','','','','','','','DAQ.JavaLikeCalc.lib_techApp.diafragma',1,1,'',0);
INSERT INTO "gen_blcks" VALUES('netTorch','Torch (net)','Факел (мережа)','Факел (сеть)','Torch net.','Факельна мережа.','Факеьная сеть.','DAQ.JavaLikeCalc.lib_techApp.net',1,1,'',0);
INSERT INTO "gen_blcks" VALUES('BC1','BC1','КК1','КШ1','The ball crane for getting gas to the station.','Кульові крани подачі газу на станцію.','Шаровые краны подачи газа на станцию.','DAQ.JavaLikeCalc.lib_techApp.klap',1,1,'',0);
INSERT INTO "gen_blcks" VALUES('G_BC21','BC21 (control mechanism)','КК21 (вик. механізм)','КШ21 (исп. механизм)','Control mechanism of ball crane BC21.','Виконавчий механізм кульового крану КК21.','Исполнительный механизм шарового крана КШ21.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "gen_blcks" VALUES('BC5','BC5','КК5','КШ5','Ball crane for dropping gas to torch.','Кульовий кран для скидання вхідного газу на факел.','Шаровый кран для сброса входного газа на факел.','DAQ.JavaLikeCalc.lib_techApp.klap',1,1,'',0);
INSERT INTO "gen_blcks" VALUES('BC6','BC6','КК6','КШ6','Ball cranes BC6 and BC7 armature','Арматура кульових кранів КК6 та КК7','Арматура шаровых кранов КШ6 и КШ7','DAQ.JavaLikeCalc.lib_techApp.fnc_klap',1,1,'',0);
INSERT INTO "gen_blcks" VALUES('node1','Node 1','Вузол 1','Узел 1','Node to torch and compressor low pressure.','Вузол розгалуження на факел та компресор низького тиску.','Узел разветвления на факел и компрессор низкого давления.','DAQ.JavaLikeCalc.lib_techApp.pipe1_2',1,1,'',0);
INSERT INTO "gen_blcks" VALUES('node20','Node 20','Вузол 20','Узел 20','','','','DAQ.JavaLikeCalc.lib_techApp.fnc_pipe1_2',1,1,'',0);
INSERT INTO "gen_blcks" VALUES('node21','Node 21','Вузол 21','Узел 21','Node before CLP.','Вузол перед КНТ.','Узел перед КНД.','DAQ.JavaLikeCalc.lib_techApp.fnc_pipe1_3',1,1,'',0);
INSERT INTO "gen_blcks" VALUES('node22','Node 22','Вузол 22','Узел 22','Flows from compresors CHP to net of GHP.','Потоки з компресору КВТ у мережу ГВТ.','Потоки с компресоров КВД в сеть ГВД.','DAQ.JavaLikeCalc.lib_techApp.pipe3_1',1,1,'',0);
INSERT INTO "gen_blcks" VALUES('node19','Node 19','Вузол 19','Узел 19','','','','DAQ.JavaLikeCalc.lib_techApp.pipe3_1',1,1,'',0);
INSERT INTO "gen_blcks" VALUES('C1','Separator C1','Сепаратор C1','Сепаратор C1','Liquid fraction droping from gas to station.','Сепаратор відгону рідинної фракції від газу який входить на станцію.','Сепаратор отгона жидкой фракции от газа входящего на станцию.','DAQ.JavaLikeCalc.lib_techApp.separator',1,1,'',0);
INSERT INTO "gen_blcks" VALUES('C2','Separator C2','Сепаратор C2','Сепаратор C2','','','','DAQ.JavaLikeCalc.lib_techApp.fnc_separator',1,1,'',0);
INSERT INTO "gen_blcks" VALUES('G_BC1','BC1 (control mechanism)','КК1 (вик. механізм)','КШ1 (исп. механизм)','Control mechanism of ball crane BC1.','Виконавчий механізм кульового крану КК1.','Исполнительный механизм шарового крана КШ1.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "gen_blcks" VALUES('G_BC2','BC2 (control mechanism)','КК2 (вик. механізм)','КШ2 (исп. механизм)','Control mechanism of ball crane BC2.','Виконавчий механізм кульового крану КК2.','Исполнительный механизм шарового крана КШ2.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "gen_blcks" VALUES('G_BC5','BC5 (control mechanism)','КК5 (вик. механізм)','КШ5 (исп. механизм)','Control mechanism of ball crane BC5.','Виконавчий механізм кульового крану КК5.','Исполнительный механизм шарового крана КШ5.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
CREATE TABLE 'gen_cntr_blocks' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"FUNC" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '0' ,"PROC" INTEGER DEFAULT '0' ,"PRIOR" TEXT DEFAULT '' ,"LNK_OUT_WR_CH" INTEGER DEFAULT '0' , PRIMARY KEY ("ID"));
INSERT INTO "gen_cntr_blocks" VALUES('PC_PCV2','PC PCV2','PC КРТ2','PC КРД2','','','','Special.FLibComplex1.fnc_pid',1,1,'',0);
INSERT INTO "gen_cntr_blocks" VALUES('PC_PCV1','PC PCV1','РС КРТ1','РС КРД1','','','','Special.FLibComplex1.pid',1,1,'',0);
INSERT INTO "gen_cntr_blocks" VALUES('PC_PCV3','PC PCV3','РС КРТ3','РС КРД3','','','','Special.FLibComplex1.pid',1,1,'',0);
INSERT INTO "gen_cntr_blocks" VALUES('SurgeCM101','CM101 surge','КМ101 помпаж','КМ101 помпаж','','','','Special.FLibComplex1.pid',1,1,'',0);
INSERT INTO "gen_cntr_blocks" VALUES('SurgeCM201','CM201 surge','КМ201 помпаж','КМ201 помпаж','','','','Special.FLibComplex1.pid',1,1,'',0);
INSERT INTO "gen_cntr_blocks" VALUES('SurgeCM301','CM301 surge','КМ301 помпаж','КМ301 помпаж','','','','Special.FLibComplex1.pid',1,1,'',0);
INSERT INTO "gen_cntr_blocks" VALUES('SurgeCM102','CM102 surge','КМ102 помпаж','КМ102 помпаж','','','','Special.FLibComplex1.pid',1,1,'',0);
INSERT INTO "gen_cntr_blocks" VALUES('SurgeCM202','CM202 surge','КМ202 помпаж','КМ202 помпаж','','','','Special.FLibComplex1.pid',1,1,'',0);
INSERT INTO "gen_cntr_blocks" VALUES('SurgeCM302','CM302 surge','КМ302 помпаж','КМ302 помпаж','','','','Special.FLibComplex1.pid',1,1,'',0);
CREATE TABLE 'CM101_blocks' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"FUNC" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '0' ,"PROC" INTEGER DEFAULT '0' ,"PRIOR" TEXT DEFAULT '' ,"LNK_OUT_WR_CH" INTEGER DEFAULT '0' , PRIMARY KEY ("ID"));
INSERT INTO "CM101_blocks" VALUES('CM101_1','Compressor CM101 stage 1','Ступ. 1 компр. КМ101','Ступ. 1 компр. КМ101','Stage one of compressor CM101.','Перша ступінь компресора КМ101.','Первая ступень компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.compressor',1,1,'',0);
INSERT INTO "CM101_blocks" VALUES('CM101_2','Compressor CM101 stage 2','Ступ. 2 компр. КМ101','Ступ. 2 компр. КМ101','Stage two of compressor CM101.','Друга ступінь компресора КМ101.','Вторая ступень компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.compressor',1,1,'',0);
INSERT INTO "CM101_blocks" VALUES('KPP101','','','','Surge protect control crane of compressor CM101.','Протипомпажний регулюючий клапан компресору КМ101.','Антипомпажный регулирующий клапан компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.fnc_klap',1,1,'',0);
INSERT INTO "CM101_blocks" VALUES('DR100','Baffle 100','Дросель 100','Дросель 100','','','','DAQ.JavaLikeCalc.lib_techApp.klap',1,1,'',0);
INSERT INTO "CM101_blocks" VALUES('AT101_1','Air cooler AT101_1','Пов. охолод. AT101_1','Возд. охлад. AT101_1','The air cooler for gas after compressor CM101 stage one.','Охолоджувач газу після першої ступені компресору КМ101.','Охладитель газа после первой ступени компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.cooler',1,1,'',0);
INSERT INTO "CM101_blocks" VALUES('AT101_2','Air cooler AT101_2','Пов. охолод. AT101_2','Возд. охлад. AT101_2','The air cooler for gas after compressor CM101 stage two.','Охолоджувач газу після другої ступені компресора КМ101.','Охладитель газа после второй ступени компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.cooler',1,1,'',0);
INSERT INTO "CM101_blocks" VALUES('node3_1','Node 3.1','Вузол 3.1','Узел 3.1','','','','DAQ.JavaLikeCalc.lib_techApp.pipe3_1',1,1,'',0);
INSERT INTO "CM101_blocks" VALUES('BC101','BC101','КК101','КШ101','Ball crane for compressor CM101 enable.','Кульовий кран підключення компресора КМ101.','Шаровый кран подключения компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.klap',1,1,'',0);
INSERT INTO "CM101_blocks" VALUES('BC104','BC104','КК104','КШ104','Ball crane for compressor CM101 enable to net.','Кульовий кран підключення компресора КМ101 у мережу.','Шаровый кран подключения компрессора КМ101 в сеть.','DAQ.JavaLikeCalc.lib_techApp.klap',1,1,'',0);
INSERT INTO "CM101_blocks" VALUES('node4_1','Node 4.1','Вузол 4.1','Узел 4.1','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_pipe1_3',1,1,'',0);
INSERT INTO "CM101_blocks" VALUES('BC106','BC106','КК106','КШ106','Surge protect ball crane of compressor CM101.','Протипомпажний шаровий кран компресора КМ101.','Антипомпажный шаровый кран компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.klap',1,1,'',0);
INSERT INTO "CM101_blocks" VALUES('C101_1','Separator C101/1','Сепаратор C101/1','Сепаратор C101/1','Liquid fraction droping before compressor CM101 stage one.','Сепаратор відгону рідини перед першим ступенем компресора КМ101.','Сепаратор отгона жидкости перед первой ступенью компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.separator',1,1,'',0);
INSERT INTO "CM101_blocks" VALUES('C101_2','Separator C101/2','Сепаратор C101/2','Сепаратор C101/2','Liquid fraction droping after compressor CM101 stage one.','Сепаратор відгону рідини після першого ступеня компресора КМ101.','Сепаратор отгона жидкости после первой ступени компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.separator',1,1,'',0);
INSERT INTO "CM101_blocks" VALUES('C101_3','Separator C101/3','Сепаратор C101/3','Сепаратор C101/3','Liquid fraction droping after compressor CM101 stage two.','Сепаратор відгону рідини після другого ступеня компресора КМ101.','Сепаратор отгона жидкости после второй ступени компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.separator',1,1,'',0);
INSERT INTO "CM101_blocks" VALUES('PP101','','','','Flow control point, diaphragm type, for first stage of compressor CM101.','Вузол виміру витрат на першому ступені компресора КМ101 діафрагменого типу.','Узел замера расхода на первую ступень компрессора КМ101 диафрагменного типа.','DAQ.JavaLikeCalc.lib_techApp.diafragma',1,1,'',0);
INSERT INTO "CM101_blocks" VALUES('PP102','','','','Flow control point, diaphragm type, for second stage of compressor CM101.','Вузол виміру витрат на другому ступені компресора КМ101 діафрагменого типу.','Узел замера расхода на вторую ступень компрессора КМ101 диафрагменного типа.','DAQ.JavaLikeCalc.lib_techApp.diafragma',1,1,'',0);
INSERT INTO "CM101_blocks" VALUES('G_BC102','BC102 (control mechanism)','КК102 (вик. механізм)','КШ102 (исп. механизм)','Control mechanism of ball crane КSH102.','Виконавчий механізм кульового крану КК102.','Исполнительный механизм шарового крана КШ102.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "CM101_blocks" VALUES('G_BC101','BC101 (control mechanism)','КК101 (вик. механізм)','КШ101 (исп. механизм)','Control mechanism of ball crane КSH101.','Виконавчий механізм кульового крану КК101.','Исполнительный механизм шарового крана КШ101.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "CM101_blocks" VALUES('G_BC104','BC104 (control mechanism)','КК104 (вик. механізм)','КШ104 (исп. механизм)','Control mechanism of ball crane КSH104.','Виконавчий механізм кульового крану КК104.','Исполнительный механизм шарового крана КШ104.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "CM101_blocks" VALUES('G_BC105','BC105 (control mechanism)','КК105 (вик. механізм)','КШ105 (исп. механизм)','Control mechanism of ball crane КSH105.','Виконавчий механізм кульового крану КК105.','Исполнительный механизм шарового крана КШ105.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "CM101_blocks" VALUES('G_BC106','BC106 (control mechanism)','КК106 (вик. механізм)','КШ106 (исп. механизм)','Control mechanism of ball crane КSH106.','Виконавчий механізм кульового крану КК106.','Исполнительный механизм шарового крана КШ106.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
CREATE TABLE 'CM102_blocks' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"FUNC" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '0' ,"PROC" INTEGER DEFAULT '0' ,"PRIOR" TEXT DEFAULT '' ,"LNK_OUT_WR_CH" INTEGER DEFAULT '0' , PRIMARY KEY ("ID"));
INSERT INTO "CM102_blocks" VALUES('KPP102','','','','Surge protect control crane of compressor КМ102.','Протипомпажний регулюючий клапан компресору КМ102.','Антипомпажный регулирующий клапан компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.fnc_klap',1,1,'',0);
INSERT INTO "CM102_blocks" VALUES('net102','','','','','','','DAQ.JavaLikeCalc.lib_techApp.fnc_net',1,1,'',0);
INSERT INTO "CM102_blocks" VALUES('DR101','Baffle 101','Дросель 101','Дросель 101','','','','DAQ.JavaLikeCalc.lib_techApp.klap',1,1,'',0);
INSERT INTO "CM102_blocks" VALUES('С102_1','Separator C102/1','Сепаратор С102/1','Сепаратор С102/1','Liquid fraction droping before compressor CM102 stage one.','Сепаратор відгону рідини перед першим ступенем компресора КМ102.','Сепаратор отгона жидкости перед первой ступенью компрессора КМ102.','DAQ.JavaLikeCalc.lib_techApp.separator',1,1,'',0);
INSERT INTO "CM102_blocks" VALUES('AT102_1','Air cooler AT102_1','Пов. охолод. AT102_1','Возд. охлад. AT102_1','The air cooler for gas after compressor CM102 stage one.','Охолоджувач газу після першої ступені компресору КМ102.','Охладитель газа после первой ступени компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.cooler',1,1,'',0);
INSERT INTO "CM102_blocks" VALUES('AT102_2','Air cooler AT102_2','Пов. охолод. AT102_2','Возд. охлад. AT102_2','The air cooler for gas after compressor CM102 stage two.','Охолоджувач газу після другої ступені компресора КМ102.','Охладитель газа после второй ступени компрессора КМ102.','DAQ.JavaLikeCalc.lib_techApp.cooler',1,1,'',0);
INSERT INTO "CM102_blocks" VALUES('BC111','BC111','КК111','КШ111','Ball crane for compressor CM102 enable.','Кульовий кран підключення компресору КМ102.','Шаровый кран подключения компрессора КМ102.','DAQ.JavaLikeCalc.lib_techApp.klap',1,1,'',0);
INSERT INTO "CM102_blocks" VALUES('BC113','BC113','КК113','КШ113','','','','DAQ.JavaLikeCalc.lib_techApp.fnc_klap',1,1,'',0);
INSERT INTO "CM102_blocks" VALUES('BC114','BC114','КК114','КШ114','Ball crane for compressor CM102 enable to net.','Кульовий кран підключення компресору КМ102 у мережу.','Шаровый кран подключения компрессора КМ101 в сеть.','DAQ.JavaLikeCalc.lib_techApp.klap',1,1,'',0);
INSERT INTO "CM102_blocks" VALUES('BC116','BC116','КК116','КШ116','Surge protect ball crane of compressor CM102.','Протипомпажний шаровий кран компресора КМ102.','Антипомпажный шаровый кран компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.klap',1,1,'',0);
INSERT INTO "CM102_blocks" VALUES('node5_1','Node 5.1','Вузол 5.1','Узел 5.1','','','','DAQ.JavaLikeCalc.lib_techApp.pipe3_1',1,1,'',0);
INSERT INTO "CM102_blocks" VALUES('CM102_1','Compressor CM102 stage 1','Ступ. 1 компр. КМ102','Ступ. 1 компр. КМ102','Stage one of compressor CM102.','Перша ступінь компресора КМ102.','Первая ступень компрессора КМ102.','DAQ.JavaLikeCalc.lib_techApp.compressor',1,1,'',0);
INSERT INTO "CM102_blocks" VALUES('CM102_2','Compressor CM102 stage 2','Ступ. 2 компр. КМ102','Ступ. 2 компр. КМ102','Stage two of compressor CM102.','Друга ступінь компресора КМ102.','Вторая ступень компрессора КМ102.','DAQ.JavaLikeCalc.lib_techApp.compressor',1,1,'',0);
INSERT INTO "CM102_blocks" VALUES('node6_1','Node 6.1','Вузол 6.1','Узел 6.1','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_pipe1_4',1,1,'',0);
INSERT INTO "CM102_blocks" VALUES('PP103','','','','Flow control point, diaphragm type, for first stage of compressor CM102.','Вузол виміру витрат на першому ступені компресора КМ102 діафрагменого типу.','Узел замера расхода на первую ступень компрессора КМ102 диафрагменного типа.','DAQ.JavaLikeCalc.lib_techApp.diafragma',1,1,'',0);
INSERT INTO "CM102_blocks" VALUES('PP104','','','','Flow control point, diaphragm type, for second stage of compressor CM102.','Вузол виміру витрат на другому ступені компресора КМ102 діафрагменого типу.','Узел замера расхода на вторую ступень компрессора КМ102 диафрагменного типа.','DAQ.JavaLikeCalc.lib_techApp.diafragma',1,1,'',0);
INSERT INTO "CM102_blocks" VALUES('C102_2','Separator C102/2','Сепаратор C102/2','Сепаратор C102/2','Liquid fraction droping after compressor CM102 stage one.','Сепаратор відгону рідини після першого ступеня компресора КМ102.','Сепаратор отгона жидкости после первой ступени компрессора КМ102.','DAQ.JavaLikeCalc.lib_techApp.separator',1,1,'',0);
INSERT INTO "CM102_blocks" VALUES('C102_3','Separator C102/3','Сепаратор C102/3','Сепаратор C102/3','Liquid fraction droping after compressor CM102 stage two.','Сепаратор відгону рідини після другого ступеня компресора КМ102.','Сепаратор отгона жидкости после второй ступени компрессора КМ102.','DAQ.JavaLikeCalc.lib_techApp.separator',1,1,'',0);
INSERT INTO "CM102_blocks" VALUES('G_BC111','BC111 (control mechanism)','КК111 (вик. механізм)','КШ111 (исп. механизм)','Control mechanism of ball crane КSH111.','Виконавчий механізм кульового крану КК101.','Исполнительный механизм шарового крана КШ101.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "CM102_blocks" VALUES('G_BC112','BC112 (control mechanism)','КК112 (вик. механізм)','КШ112 (исп. механизм)','Control mechanism of ball crane КSH112.','Виконавчий механізм кульового крану КК102.','Исполнительный механизм шарового крана КШ102.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "CM102_blocks" VALUES('G_BC114','BC114 (control mechanism)','КК114 (вик. механізм)','КШ114 (исп. механизм)','Control mechanism of ball crane КSH114.','Виконавчий механізм кульового крану КК104.','Исполнительный механизм шарового крана КШ104.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "CM102_blocks" VALUES('G_BC115','BC115 (control mechanism)','КК115 (вик. механізм)','КШ115 (исп. механизм)','Control mechanism of ball crane КSH115.','Виконавчий механізм кульового крану КК105.','Исполнительный механизм шарового крана КШ105.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "CM102_blocks" VALUES('G_BC116','BC116 (control mechanism)','КК116 (вик. механізм)','КШ116 (исп. механизм)','Control mechanism of ball crane КSH116.','Виконавчий механізм кульового крану КК106.','Исполнительный механизм шарового крана КШ106.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
CREATE TABLE 'CM102cntr_blocks' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"FUNC" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '0' ,"PROC" INTEGER DEFAULT '0' ,"PRIOR" TEXT DEFAULT '' ,"LNK_OUT_WR_CH" INTEGER DEFAULT '0' , PRIMARY KEY ("ID"));
INSERT INTO "CM102cntr_blocks" VALUES('BC111','BC111','КК111','КШ111','','','','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "CM102cntr_blocks" VALUES('BC112','BC112','КК112','КШ112','','','','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "CM102cntr_blocks" VALUES('BC113','BC113','КК113','КШ113','','','','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "CM102cntr_blocks" VALUES('BC114','BC114','КК114','КШ114','','','','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "CM102cntr_blocks" VALUES('BC116','BC116','КК116','КШ116','','','','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
CREATE TABLE 'CM201_blocks' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"FUNC" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '0' ,"PROC" INTEGER DEFAULT '0' ,"PRIOR" TEXT DEFAULT '' ,"LNK_OUT_WR_CH" INTEGER DEFAULT '0' , PRIMARY KEY ("ID"));
INSERT INTO "CM201_blocks" VALUES('CM101_1','Compressor CM101 stage 1','Ступ. 1 компр. КМ101','Ступ. 1 компр. КМ101','Stage one of compressor CM101.','Перша ступінь компресора КМ101.','Первая ступень компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.compressor',1,1,'',0);
INSERT INTO "CM201_blocks" VALUES('CM101_2','Compressor CM101 stage 2','Ступ. 2 компр. КМ101','Ступ. 2 компр. КМ101','Stage two of compressor CM101.','Друга ступінь компресора КМ101.','Вторая ступень компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.compressor',1,1,'',0);
INSERT INTO "CM201_blocks" VALUES('KPP101','','','','Surge protect control crane of compressor CM101.','Протипомпажний регулюючий клапан компресору КМ101.','Антипомпажный регулирующий клапан компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.fnc_klap',1,1,'',0);
INSERT INTO "CM201_blocks" VALUES('DR100','Baffle 100','Дросель 100','Дросель 100','','','','DAQ.JavaLikeCalc.lib_techApp.klap',1,1,'',0);
INSERT INTO "CM201_blocks" VALUES('AT101_1','Air cooler AT101_1','Пов. охолод. AT101_1','Возд. охлад. AT101_1','The air cooler for gas after compressor CM101 stage one.','Охолоджувач газу після першої ступені компресору КМ101.','Охладитель газа после первой ступени компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.cooler',1,1,'',0);
INSERT INTO "CM201_blocks" VALUES('AT101_2','Air cooler AT101_2','Пов. охолод. AT101_2','Возд. охлад. AT101_2','The air cooler for gas after compressor CM101 stage two.','Охолоджувач газу після другої ступені компресора КМ101.','Охладитель газа после второй ступени компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.cooler',1,1,'',0);
INSERT INTO "CM201_blocks" VALUES('node3_1','Node 3.1','Вузол 3.1','Узел 3.1','','','','DAQ.JavaLikeCalc.lib_techApp.pipe3_1',1,1,'',0);
INSERT INTO "CM201_blocks" VALUES('BC101','BC101','КК101','КШ101','Ball crane for compressor CM101 enable.','Кульовий кран підключення компресора КМ101.','Шаровый кран подключения компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.klap',1,1,'',0);
INSERT INTO "CM201_blocks" VALUES('node4_1','Node 4.1','Вузол 4.1','Узел 4.1','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_pipe1_3',1,1,'',0);
INSERT INTO "CM201_blocks" VALUES('BC104','BC104','КК104','КШ104','Ball crane for compressor CM101 enable to net.','Кульовий кран підключення компресора КМ101 у мережу.','Шаровый кран подключения компрессора КМ101 в сеть.','DAQ.JavaLikeCalc.lib_techApp.klap',1,1,'',0);
INSERT INTO "CM201_blocks" VALUES('BC106','BC106','КК106','КШ106','Surge protect ball crane of compressor CM101.','Протипомпажний шаровий кран компресора КМ101.','Антипомпажный шаровый кран компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.klap',1,1,'',0);
INSERT INTO "CM201_blocks" VALUES('C101_1','Separator C101/1','Сепаратор C101/1','Сепаратор C101/1','Liquid fraction droping before compressor CM101 stage one.','Сепаратор відгону рідини перед першим ступенем компресора КМ101.','Сепаратор отгона жидкости перед первой ступенью компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.separator',1,1,'',0);
INSERT INTO "CM201_blocks" VALUES('C101_2','Separator C101/2','Сепаратор C101/2','Сепаратор C101/2','Liquid fraction droping after compressor CM101 stage one.','Сепаратор відгону рідини після першого ступеня компресора КМ101.','Сепаратор отгона жидкости после первой ступени компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.separator',1,1,'',0);
INSERT INTO "CM201_blocks" VALUES('C101_3','Separator C101/3','Сепаратор C101/3','Сепаратор C101/3','Liquid fraction droping after compressor CM101 stage two.','Сепаратор відгону рідини після другого ступеня компресора КМ101.','Сепаратор отгона жидкости после второй ступени компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.separator',1,1,'',0);
INSERT INTO "CM201_blocks" VALUES('PP101','','','','Flow control point, diaphragm type, for first stage of compressor CM101.','Вузол виміру витрат на першому ступені компресора КМ101 діафрагменого типу.','Узел замера расхода на первую ступень компрессора КМ101 диафрагменного типа.','DAQ.JavaLikeCalc.lib_techApp.diafragma',1,1,'',0);
INSERT INTO "CM201_blocks" VALUES('PP102','','','','Flow control point, diaphragm type, for second stage of compressor CM101.','Вузол виміру витрат на другому ступені компресора КМ101 діафрагменого типу.','Узел замера расхода на вторую ступень компрессора КМ101 диафрагменного типа.','DAQ.JavaLikeCalc.lib_techApp.diafragma',1,1,'',0);
INSERT INTO "CM201_blocks" VALUES('G_BC104','BC104 (control mechanism)','КК104 (вик. механізм)','КШ104 (исп. механизм)','Control mechanism of ball crane КSH104.','Виконавчий механізм кульового крану КК104.','Исполнительный механизм шарового крана КШ104.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "CM201_blocks" VALUES('G_BC105','BC105 (control mechanism)','КК105 (вик. механізм)','КШ105 (исп. механизм)','Control mechanism of ball crane КSH105.','Виконавчий механізм кульового крану КК105.','Исполнительный механизм шарового крана КШ105.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "CM201_blocks" VALUES('G_BC101','BC101 (control mechanism)','КК101 (вик. механізм)','КШ101 (исп. механизм)','Control mechanism of ball crane КSH101.','Виконавчий механізм кульового крану КК101.','Исполнительный механизм шарового крана КШ101.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "CM201_blocks" VALUES('G_BC106','BC106 (control mechanism)','КК106 (вик. механізм)','КШ106 (исп. механизм)','Control mechanism of ball crane КSH106.','Виконавчий механізм кульового крану КК106.','Исполнительный механизм шарового крана КШ106.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "CM201_blocks" VALUES('G_BC102','BC102 (control mechanism)','КК102 (вик. механізм)','КШ102 (исп. механизм)','Control mechanism of ball crane КSH102.','Виконавчий механізм кульового крану КК102.','Исполнительный механизм шарового крана КШ102.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
CREATE TABLE 'CM301_blocks' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"FUNC" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '0' ,"PROC" INTEGER DEFAULT '0' ,"PRIOR" TEXT DEFAULT '' ,"LNK_OUT_WR_CH" INTEGER DEFAULT '0' , PRIMARY KEY ("ID"));
INSERT INTO "CM301_blocks" VALUES('CM101_1','Compressor CM101 stage 1','Ступ. 1 компр. КМ101','Ступ. 1 компр. КМ101','Stage one of compressor CM101.','Перша ступінь компресора КМ101.','Первая ступень компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.compressor',1,1,'',0);
INSERT INTO "CM301_blocks" VALUES('CM101_2','Compressor CM101 stage 2','Ступ. 2 компр. КМ101','Ступ. 2 компр. КМ101','Stage two of compressor CM101.','Друга ступінь компресора КМ101.','Вторая ступень компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.compressor',1,1,'',0);
INSERT INTO "CM301_blocks" VALUES('KPP101','','','','Surge protect control crane of compressor CM101.','Протипомпажний регулюючий клапан компресору КМ101.','Антипомпажный регулирующий клапан компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.fnc_klap',1,1,'',0);
INSERT INTO "CM301_blocks" VALUES('DR100','Baffle 100','Дросель 100','Дросель 100','','','','DAQ.JavaLikeCalc.lib_techApp.klap',1,1,'',0);
INSERT INTO "CM301_blocks" VALUES('AT101_1','Air cooler AT101_1','Пов. охолод. AT101_1','Возд. охлад. AT101_1','The air cooler for gas after compressor CM101 stage one.','Охолоджувач газу після першої ступені компресору КМ101.','Охладитель газа после первой ступени компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.cooler',1,1,'',0);
INSERT INTO "CM301_blocks" VALUES('AT101_2','Air cooler AT101_2','Пов. охолод. AT101_2','Возд. охлад. AT101_2','The air cooler for gas after compressor CM101 stage two.','Охолоджувач газу після другої ступені компресора КМ101.','Охладитель газа после второй ступени компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.cooler',1,1,'',0);
INSERT INTO "CM301_blocks" VALUES('node3_1','Node 3.1','Вузол 3.1','Узел 3.1','','','','DAQ.JavaLikeCalc.lib_techApp.pipe3_1',1,1,'',0);
INSERT INTO "CM301_blocks" VALUES('BC101','BC101','КК101','КШ101','Ball crane for compressor CM101 enable.','Кульовий кран підключення компресора КМ101.','Шаровый кран подключения компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.klap',1,1,'',0);
INSERT INTO "CM301_blocks" VALUES('BC104','BC104','КК104','КШ104','Ball crane for compressor CM101 enable to net.','Кульовий кран підключення компресора КМ101 у мережу.','Шаровый кран подключения компрессора КМ101 в сеть.','DAQ.JavaLikeCalc.lib_techApp.klap',1,1,'',0);
INSERT INTO "CM301_blocks" VALUES('node4_1','Node 4.1','Вузол 4.1','Узел 4.1','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_pipe1_3',1,1,'',0);
INSERT INTO "CM301_blocks" VALUES('BC106','BC106','КК106','КШ106','Surge protect ball crane of compressor CM101.','Протипомпажний шаровий кран компресора КМ101.','Антипомпажный шаровый кран компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.klap',1,1,'',0);
INSERT INTO "CM301_blocks" VALUES('C101_1','Separator C101/1','Сепаратор C101/1','Сепаратор C101/1','Liquid fraction droping before compressor CM101 stage one.','Сепаратор відгону рідини перед першим ступенем компресора КМ101.','Сепаратор отгона жидкости перед первой ступенью компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.separator',1,1,'',0);
INSERT INTO "CM301_blocks" VALUES('C101_2','Separator C101/2','Сепаратор C101/2','Сепаратор C101/2','Liquid fraction droping after compressor CM101 stage one.','Сепаратор відгону рідини після першого ступеня компресора КМ101.','Сепаратор отгона жидкости после первой ступени компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.separator',1,1,'',0);
INSERT INTO "CM301_blocks" VALUES('C101_3','Separator C101/3','Сепаратор C101/3','Сепаратор C101/3','Liquid fraction droping after compressor CM101 stage two.','Сепаратор відгону рідини після другого ступеня компресора КМ101.','Сепаратор отгона жидкости после второй ступени компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.separator',1,1,'',0);
INSERT INTO "CM301_blocks" VALUES('PP101','','','','Flow control point, diaphragm type, for first stage of compressor CM101.','Вузол виміру витрат на першому ступені компресора КМ101 діафрагменого типу.','Узел замера расхода на первую ступень компрессора КМ101 диафрагменного типа.','DAQ.JavaLikeCalc.lib_techApp.diafragma',1,1,'',0);
INSERT INTO "CM301_blocks" VALUES('PP102','','','','Flow control point, diaphragm type, for second stage of compressor CM101.','Вузол виміру витрат на другому ступені компресора КМ101 діафрагменого типу.','Узел замера расхода на вторую ступень компрессора КМ101 диафрагменного типа.','DAQ.JavaLikeCalc.lib_techApp.diafragma',1,1,'',0);
INSERT INTO "CM301_blocks" VALUES('G_BC104','BC104 (control mechanism)','КК104 (вик. механізм)','КШ104 (исп. механизм)','Control mechanism of ball crane КSH104.','Виконавчий механізм кульового крану КК104.','Исполнительный механизм шарового крана КШ104.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "CM301_blocks" VALUES('G_BC105','BC105 (control mechanism)','КК105 (вик. механізм)','КШ105 (исп. механизм)','Control mechanism of ball crane КSH105.','Виконавчий механізм кульового крану КК105.','Исполнительный механизм шарового крана КШ105.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "CM301_blocks" VALUES('G_BC101','BC101 (control mechanism)','КК101 (вик. механізм)','КШ101 (исп. механизм)','Control mechanism of ball crane КSH101.','Виконавчий механізм кульового крану КК101.','Исполнительный механизм шарового крана КШ101.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "CM301_blocks" VALUES('G_BC106','BC106 (control mechanism)','КК106 (вик. механізм)','КШ106 (исп. механизм)','Control mechanism of ball crane КSH106.','Виконавчий механізм кульового крану КК106.','Исполнительный механизм шарового крана КШ106.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "CM301_blocks" VALUES('G_BC102','BC102 (control mechanism)','КК102 (вик. механізм)','КШ102 (исп. механизм)','Control mechanism of ball crane КSH102.','Виконавчий механізм кульового крану КК102.','Исполнительный механизм шарового крана КШ102.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
CREATE TABLE 'CM302_blocks' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"FUNC" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '0' ,"PROC" INTEGER DEFAULT '0' ,"PRIOR" TEXT DEFAULT '' ,"LNK_OUT_WR_CH" INTEGER DEFAULT '0' , PRIMARY KEY ("ID"));
INSERT INTO "CM302_blocks" VALUES('KPP102','','','','Surge protect control crane of compressor CM102.','Протипомпажний регулюючий клапан компресору КМ102.','Антипомпажный регулирующий клапан компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.fnc_klap',1,1,'',0);
INSERT INTO "CM302_blocks" VALUES('net102','','','','','','','DAQ.JavaLikeCalc.lib_techApp.fnc_net',1,1,'',0);
INSERT INTO "CM302_blocks" VALUES('DR101','Baffle 101','Дросель 101','Дросель 101','','','','DAQ.JavaLikeCalc.lib_techApp.klap',1,1,'',0);
INSERT INTO "CM302_blocks" VALUES('С102_1','Separator C102/1','Сепаратор С102/1','Сепаратор С102/1','Liquid fraction droping before compressor CM102 stage one.','Сепаратор відгону рідини перед першим ступенем компресора КМ102.','Сепаратор отгона жидкости перед первой ступенью компрессора КМ102.','DAQ.JavaLikeCalc.lib_techApp.separator',1,1,'',0);
INSERT INTO "CM302_blocks" VALUES('AT102_1','Air cooler AT102_1','Пов. охолод. AT102_1','Возд. охлад. AT102_1','The air cooler for gas after compressor CM102 stage one.','Охолоджувач газу після першої ступені компресору КМ102.','Охладитель газа после первой ступени компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.cooler',1,1,'',0);
INSERT INTO "CM302_blocks" VALUES('AT102_2','Air cooler AT102_2','Пов. охолод. AT102_2','Возд. охлад. AT102_2','The air cooler for gas after compressor CM102 stage two.','Охолоджувач газу після другої ступені компресора КМ102.','Охладитель газа после второй ступени компрессора КМ102.','DAQ.JavaLikeCalc.lib_techApp.cooler',1,1,'',0);
INSERT INTO "CM302_blocks" VALUES('BC111','BC111','КК111','КШ111','Ball crane for compressor CM102 enable.','Кульовий кран підключення компресору КМ102.','Шаровый кран подключения компрессора КМ102.','DAQ.JavaLikeCalc.lib_techApp.klap',1,1,'',0);
INSERT INTO "CM302_blocks" VALUES('BC113','BC113','КК113','КШ113','','','','DAQ.JavaLikeCalc.lib_techApp.fnc_klap',1,1,'',0);
INSERT INTO "CM302_blocks" VALUES('BC114','BC114','КК114','КШ114','Ball crane for compressor CM102 enable to net.','Кульовий кран підключення компресору КМ102 у мережу.','Шаровый кран подключения компрессора КМ101 в сеть.','DAQ.JavaLikeCalc.lib_techApp.klap',1,1,'',0);
INSERT INTO "CM302_blocks" VALUES('BC116','BC116','КК116','КШ116','Surge protect ball crane of compressor CM102.','Протипомпажний шаровий кран компресора КМ102.','Антипомпажный шаровый кран компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.klap',1,1,'',0);
INSERT INTO "CM302_blocks" VALUES('node5_1','Node 5.1','Вузол 5.1','Узел 5.1','','','','DAQ.JavaLikeCalc.lib_techApp.pipe3_1',1,1,'',0);
INSERT INTO "CM302_blocks" VALUES('CM102_1','Compressor CM102 stage 1','Ступ. 1 компр. КМ102','Ступ. 1 компр. КМ102','Stage one of compressor CM102.','Перша ступінь компресора КМ102.','Первая ступень компрессора КМ102.','DAQ.JavaLikeCalc.lib_techApp.compressor',1,1,'',0);
INSERT INTO "CM302_blocks" VALUES('CM102_2','Compressor CM102 stage 2','Ступ. 2 компр. КМ102','Ступ. 2 компр. КМ102','Stage two of compressor CM102.','Друга ступінь компресора КМ102.','Вторая ступень компрессора КМ102.','DAQ.JavaLikeCalc.lib_techApp.compressor',1,1,'',0);
INSERT INTO "CM302_blocks" VALUES('node6_1','Node 6.1','Вузол 6.1','Узел 6.1','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_pipe1_4',1,1,'',0);
INSERT INTO "CM302_blocks" VALUES('PP103','','','','Flow control point, diaphragm type, for first stage of compressor CM102.','Вузол виміру витрат на першому ступені компресора КМ102 діафрагменого типу.','Узел замера расхода на первую ступень компрессора КМ102 диафрагменного типа.','DAQ.JavaLikeCalc.lib_techApp.diafragma',1,1,'',0);
INSERT INTO "CM302_blocks" VALUES('PP104','','','','Flow control point, diaphragm type, for second stage of compressor CM102.','Вузол виміру витрат на другому ступені компресора КМ102 діафрагменого типу.','Узел замера расхода на вторую ступень компрессора КМ102 диафрагменного типа.','DAQ.JavaLikeCalc.lib_techApp.diafragma',1,1,'',0);
INSERT INTO "CM302_blocks" VALUES('C102_2','Separator C102/2','Сепаратор C102/2','Сепаратор C102/2','Liquid fraction droping after compressor CM102 stage one.','Сепаратор відгону рідини після першого ступеня компресора КМ102.','Сепаратор отгона жидкости после первой ступени компрессора КМ102.','DAQ.JavaLikeCalc.lib_techApp.separator',1,1,'',0);
INSERT INTO "CM302_blocks" VALUES('C102_3','Separator C102/3','Сепаратор C102/3','Сепаратор C102/3','Liquid fraction droping after compressor CM102 stage two.','Сепаратор відгону рідини після другого ступеня компресора КМ102.','Сепаратор отгона жидкости после второй ступени компрессора КМ102.','DAQ.JavaLikeCalc.lib_techApp.separator',1,1,'',0);
INSERT INTO "CM302_blocks" VALUES('G_BC114','BC114 (control mechanism)','КК114 (вик. механізм)','КШ114 (исп. механизм)','Control mechanism of ball crane КSH114.','Виконавчий механізм кульового крану КК104.','Исполнительный механизм шарового крана КШ104.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "CM302_blocks" VALUES('G_BC115','BC115 (control mechanism)','КК115 (вик. механізм)','КШ115 (исп. механизм)','Control mechanism of ball crane КSH115.','Виконавчий механізм кульового крану КК105.','Исполнительный механизм шарового крана КШ105.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "CM302_blocks" VALUES('G_BC111','BC111 (control mechanism)','КК111 (вик. механізм)','КШ111 (исп. механизм)','Control mechanism of ball crane КSH111.','Виконавчий механізм кульового крану КК101.','Исполнительный механизм шарового крана КШ101.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "CM302_blocks" VALUES('G_BC116','BC116 (control mechanism)','КК116 (вик. механізм)','КШ116 (исп. механизм)','Control mechanism of ball crane КSH116.','Виконавчий механізм кульового крану КК106.','Исполнительный механизм шарового крана КШ106.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "CM302_blocks" VALUES('G_BC112','BC112 (control mechanism)','КК112 (вик. механізм)','КШ112 (исп. механизм)','Control mechanism of ball crane КSH112.','Виконавчий механізм кульового крану КК102.','Исполнительный механизм шарового крана КШ102.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
CREATE TABLE 'CM202_blocks' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"FUNC" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '0' ,"PROC" INTEGER DEFAULT '0' ,"PRIOR" TEXT DEFAULT '' ,"LNK_OUT_WR_CH" INTEGER DEFAULT '0' , PRIMARY KEY ("ID"));
INSERT INTO "CM202_blocks" VALUES('KPP102','','','KPP102','Surge protect control crane of compressor CM102.','Протипомпажний регулюючий клапан компресору КМ102.','Антипомпажный регулирующий клапан компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.fnc_klap',1,1,'',0);
INSERT INTO "CM202_blocks" VALUES('net102','','','','','','','DAQ.JavaLikeCalc.lib_techApp.fnc_net',1,1,'',0);
INSERT INTO "CM202_blocks" VALUES('DR101','Baffle 101','Дросель 101','Дросель 101','','','','DAQ.JavaLikeCalc.lib_techApp.klap',1,1,'',0);
INSERT INTO "CM202_blocks" VALUES('С102_1','Separator C102/1','Сепаратор С102/1','Сепаратор С102/1','Liquid fraction droping before compressor CM102 stage one.','Сепаратор відгону рідини перед першим ступенем компресора КМ102.','Сепаратор отгона жидкости перед первой ступенью компрессора КМ102.','DAQ.JavaLikeCalc.lib_techApp.separator',1,1,'',0);
INSERT INTO "CM202_blocks" VALUES('AT102_1','Air cooler AT102_1','Пов. охолод. AT102_1','Возд. охлад. AT102_1','The air cooler for gas after compressor CM102 stage one.','Охолоджувач газу після першої ступені компресору КМ102.','Охладитель газа после первой ступени компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.cooler',1,1,'',0);
INSERT INTO "CM202_blocks" VALUES('AT102_2','Air cooler AT102_2','Пов. охолод. AT102_2','Возд. охлад. AT102_2','The air cooler for gas after compressor CM102 stage two.','Охолоджувач газу після другої ступені компресора КМ102.','Охладитель газа после второй ступени компрессора КМ102.','DAQ.JavaLikeCalc.lib_techApp.cooler',1,1,'',0);
INSERT INTO "CM202_blocks" VALUES('BC111','BC111','КК111','КШ111','Ball crane for compressor CM102 enable.','Кульовий кран підключення компресору КМ102.','Шаровый кран подключения компрессора КМ102.','DAQ.JavaLikeCalc.lib_techApp.klap',1,1,'',0);
INSERT INTO "CM202_blocks" VALUES('BC113','BC113','КК113','КШ113','','','','DAQ.JavaLikeCalc.lib_techApp.fnc_klap',1,1,'',0);
INSERT INTO "CM202_blocks" VALUES('BC114','BC114','КК114','КШ114','Ball crane for compressor CM102 enable to net.','Кульовий кран підключення компресору КМ102 у мережу.','Шаровый кран подключения компрессора КМ101 в сеть.','DAQ.JavaLikeCalc.lib_techApp.klap',1,1,'',0);
INSERT INTO "CM202_blocks" VALUES('BC116','BC116','КК116','КШ116','Surge protect ball crane of compressor CM102.','Протипомпажний шаровий кран компресора КМ102.','Антипомпажный шаровый кран компрессора КМ101.','DAQ.JavaLikeCalc.lib_techApp.klap',1,1,'',0);
INSERT INTO "CM202_blocks" VALUES('node5_1','Node 5.1','Вузол 5.1','Узел 5.1','','','','DAQ.JavaLikeCalc.lib_techApp.pipe3_1',1,1,'',0);
INSERT INTO "CM202_blocks" VALUES('CM102_1','Compressor CM102 stage 1','Ступ. 1 компр. КМ102','Ступ. 1 компр. КМ102','Stage one of compressor CM102.','Перша ступінь компресора КМ102.','Первая ступень компрессора КМ102.','DAQ.JavaLikeCalc.lib_techApp.compressor',1,1,'',0);
INSERT INTO "CM202_blocks" VALUES('CM102_2','Compressor CM102 stage 2','Ступ. 2 компр. КМ102','Ступ. 2 компр. КМ102','Stage two of compressor CM102.','Друга ступінь компресора КМ102.','Вторая ступень компрессора КМ102.','DAQ.JavaLikeCalc.lib_techApp.compressor',1,1,'',0);
INSERT INTO "CM202_blocks" VALUES('node6_1','Node 6.1','Вузол 6.1','Узел 6.1','','','','sub_DAQ.mod_JavaLikeCalc.lib_techApp.fnc_pipe1_4',1,1,'',0);
INSERT INTO "CM202_blocks" VALUES('PP103','','','','Flow control point, diaphragm type, for first stage of compressor CM102.','Вузол виміру витрат на першому ступені компресора КМ102 діафрагменого типу.','Узел замера расхода на первую ступень компрессора КМ102 диафрагменного типа.','DAQ.JavaLikeCalc.lib_techApp.diafragma',1,1,'',0);
INSERT INTO "CM202_blocks" VALUES('PP104','','','','Flow control point, diaphragm type, for second stage of compressor CM102.','Вузол виміру витрат на другому ступені компресора КМ102 діафрагменого типу.','Узел замера расхода на вторую ступень компрессора КМ102 диафрагменного типа.','DAQ.JavaLikeCalc.lib_techApp.diafragma',1,1,'',0);
INSERT INTO "CM202_blocks" VALUES('C102_2','Separator C102/2','Сепаратор C102/2','Сепаратор C102/2','Liquid fraction droping after compressor CM102 stage one.','Сепаратор відгону рідини після першого ступеня компресора КМ102.','Сепаратор отгона жидкости после первой ступени компрессора КМ102.','DAQ.JavaLikeCalc.lib_techApp.separator',1,1,'',0);
INSERT INTO "CM202_blocks" VALUES('C102_3','Separator C102/3','Сепаратор C102/3','Сепаратор C102/3','Liquid fraction droping after compressor CM102 stage two.','Сепаратор відгону рідини після другого ступеня компресора КМ102.','Сепаратор отгона жидкости после второй ступени компрессора КМ102.','DAQ.JavaLikeCalc.lib_techApp.separator',1,1,'',0);
INSERT INTO "CM202_blocks" VALUES('G_BC114','BC114 (control mechanism)','КК114 (вик. механізм)','КШ114 (исп. механизм)','Control mechanism of ball crane КSH114.','Виконавчий механізм кульового крану КК104.','Исполнительный механизм шарового крана КШ104.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "CM202_blocks" VALUES('G_BC115','BC115 (control mechanism)','КК115 (вик. механізм)','КШ115 (исп. механизм)','Control mechanism of ball crane КSH115.','Виконавчий механізм кульового крану КК105.','Исполнительный механизм шарового крана КШ105.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "CM202_blocks" VALUES('G_BC111','BC111 (control mechanism)','КК111 (вик. механізм)','КШ111 (исп. механизм)','Control mechanism of ball crane КSH111.','Виконавчий механізм кульового крану КК101.','Исполнительный механизм шарового крана КШ101.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "CM202_blocks" VALUES('G_BC116','BC116 (control mechanism)','КК116 (вик. механізм)','КШ116 (исп. механизм)','Control mechanism of ball crane КSH116.','Виконавчий механізм кульового крану КК106.','Исполнительный механизм шарового крана КШ106.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
INSERT INTO "CM202_blocks" VALUES('G_BC112','BC112 (control mechanism)','КК112 (вик. механізм)','КШ112 (исп. механизм)','Control mechanism of ball crane КSH112.','Виконавчий механізм кульового крану КК102.','Исполнительный механизм шарового крана КШ102.','DAQ.JavaLikeCalc.lib_techApp.fnc_ballCrane',1,1,'',0);
COMMIT;
