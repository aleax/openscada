PRAGMA foreign_keys=OFF;
BEGIN TRANSACTION;
CREATE TABLE 'AMRDevsPrm_test' ("SHIFR" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '' ,"DEV_TP" TEXT DEFAULT '' ,"DEV_ADDR" TEXT DEFAULT '' ,"DEV_PRMS" TEXT DEFAULT '' ,"ADDR" TEXT DEFAULT '' , PRIMARY KEY ("SHIFR"));
INSERT INTO "AMRDevsPrm_test" VALUES('test','','','','',1,'Ergomera','6','<prms FirmwareVer="41" Thread="2" FragMerge="1">
<Attrs>
I:40000:var:Var
</Attrs>
</prms>
','testModBus');
CREATE TABLE 'Archive_mess_proc' ("ID" TEXT DEFAULT '' ,"MODUL" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"START" INTEGER DEFAULT '' ,"CATEG" TEXT DEFAULT '' ,"LEVEL" INTEGER DEFAULT '' ,"ADDR" TEXT DEFAULT '' ,"DBArchSize" DOUBLE DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"A_PRMS" TEXT DEFAULT '' , PRIMARY KEY ("ID","MODUL"));
INSERT INTO "Archive_mess_proc" VALUES('NetRequsts','FSArch','','Requests to server through transport Sockets.',1,'/sub_Transport/mod_Sockets*',1,'ARCHIVES/MESS/Net/',10000.0,'','Запити до сервера через транспорт Sockets.','','Запросы к серверу через транспорт Sockets.','<prms XML="0" MSize="1024" NFiles="30" TmSize="30" PackTm="10" CheckTm="60" PackInfoFiles="0" PrevDbl="0" />
');
INSERT INTO "Archive_mess_proc" VALUES('test','FSArch','Test','',1,'*',0,'ARCHIVES/MESS/TEST/',10000.0,'Test','','Test','','<prms XML="0" MSize="1024" NFiles="30" TmSize="30" PackTm="10" CheckTm="60" PackInfoFiles="0" PrevDbl="0" />
');
INSERT INTO "Archive_mess_proc" VALUES('StatErrors','FSArch','','Local errors'' archive',1,'/*',4,'ARCHIVES/MESS/stError/',10000.0,'','Архів локальних помилок','','Архив локальных ощибок','<prms XML="1" MSize="300" NFiles="10" TmSize="30" PackTm="10" CheckTm="60" PackInfoFiles="0" PrevDbl="0" />
');
INSERT INTO "Archive_mess_proc" VALUES('test','DBArch','','',0,'*',0,'MySQL.arch',0.0,'','','','','<prms Size="1" />
');
CREATE TABLE 'Archive_val' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"START" INTEGER DEFAULT '' ,"SrcMode" INTEGER DEFAULT '' ,"Source" TEXT DEFAULT '' ,"VTYPE" INTEGER DEFAULT '' ,"BPER" DOUBLE DEFAULT '' ,"BSIZE" INTEGER DEFAULT '' ,"BHGRD" INTEGER DEFAULT '' ,"BHRES" INTEGER DEFAULT '' ,"ArchS" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"CombMode" INTEGER DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO "Archive_val" VALUES('gN1_dP','','',1,2,'LogicLev.experiment.gN1.dP',4,1.0,100,1,1,'FSArch.1s;FSArch.1h;','','','','',0);
INSERT INTO "Archive_val" VALUES('gN1_T','','',1,2,'LogicLev.experiment.gN1.T',4,1.0,100,1,1,'FSArch.1s;FSArch.1h;','','','','',0);
INSERT INTO "Archive_val" VALUES('gN1_P','','',1,2,'LogicLev.experiment.gN1.P',4,1.0,100,1,1,'FSArch.1s;FSArch.1h;','','','','',0);
INSERT INTO "Archive_val" VALUES('P3_var','','',1,2,'LogicLev.experiment.P3.var',4,1.0,100,1,1,'FSArch.1s;FSArch.1m;','','','','',0);
INSERT INTO "Archive_val" VALUES('MemInfo_use','','',1,1,'System.AutoDA.MemInfo.use',1,1.0,100,1,0,'DBArch.1s;FSArch.1s;FSArch.1m;','','','','',0);
INSERT INTO "Archive_val" VALUES('F3_var','','',1,2,'LogicLev.experiment.F3.var',4,1.0,100,1,1,'FSArch.1s;FSArch.1m;','','','','',0);
INSERT INTO "Archive_val" VALUES('P4_var','','',1,2,'LogicLev.experiment.P4.var',4,1.0,100,1,1,'FSArch.1s;FSArch.1m;','','','','',0);
INSERT INTO "Archive_val" VALUES('CPULoad_load','','',1,1,'System.AutoDA.CPULoad.load',4,1.0,100,1,0,'DBArch.1s;FSArch.1s;FSArch.1m;','','','','',0);
INSERT INTO "Archive_val" VALUES('gN1_Q','','',1,2,'LogicLev.experiment.gN1.Q',4,1.0,100,1,1,'FSArch.1s;FSArch.1h;','','','','',0);
INSERT INTO "Archive_val" VALUES('F4_var','','',1,2,'LogicLev.experiment.F4.var',4,1.0,100,1,1,'FSArch.1s;FSArch.1m;','','','','',0);
INSERT INTO "Archive_val" VALUES('gN1_F','','',1,2,'LogicLev.experiment.gN1.F',4,1.0,100,1,1,'FSArch.1s;FSArch.1h;','','','','',0);
INSERT INTO "Archive_val" VALUES('gN1_DS','','',1,2,'LogicLev.experiment.gN1.DS',4,1.0,100,1,1,'FSArch.1s;FSArch.1h;','','','','',0);
INSERT INTO "Archive_val" VALUES('ST8612_var','','',1,2,'BlockCalc.KM101.ST8612.var',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('F101_var','','',1,2,'BlockCalc.KM101.F101.var',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('P101_var','','',1,2,'BlockCalc.KM101.P101.var',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('PT1006_1_var','','',1,2,'BlockCalc.KM101.PT1006_1.var',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('TE1314_1_var','','',1,2,'BlockCalc.KM101.TE1314_1.var',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('TE1313_1_var','','',1,2,'BlockCalc.KM101.TE1313_1.var',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('TE1314_2_var','','',1,2,'BlockCalc.KM101.TE1314_2.var',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('Pi_var','','',1,2,'LogicLev.experiment.Pi.var',4,1.0,100,1,1,'FSArch.1s;FSArch.1m;FSArch.1h;','','','','',0);
INSERT INTO "Archive_val" VALUES('PC_КРД1_var','','',1,2,'BlockCalc.Anast1to2node_cntr.PC_КРД1.var',4,1.0,100,1,0,'FSArch.1s;FSArch.1m;FSArch.1h;','','','','',0);
INSERT INTO "Archive_val" VALUES('P_PP1_var','','',1,2,'LogicLev.experiment.P_PP1.var',4,1.0,100,1,1,'FSArch.1s;FSArch.1m;FSArch.1h;','','','','',0);
INSERT INTO "Archive_val" VALUES('P_PP3_var','','',1,2,'LogicLev.experiment.P_PP3.var',4,1.0,100,1,1,'FSArch.1s;FSArch.1m;FSArch.1h;','','','','',0);
INSERT INTO "Archive_val" VALUES('PC_КРД2_var','','',1,2,'BlockCalc.Anast1to2node_cntr.PC_КРД2.var',4,1.0,100,1,0,'FSArch.1s;FSArch.1m;FSArch.1h;','','','','',0);
INSERT INTO "Archive_val" VALUES('P_PP5_var','','',1,2,'LogicLev.experiment.P_PP5.var',4,1.0,100,1,1,'FSArch.1s;FSArch.1m;FSArch.1h;','','','','',0);
INSERT INTO "Archive_val" VALUES('PC_КРД3_var','','',1,2,'BlockCalc.Anast1to2node_cntr.PC_КРД3.var',4,1.0,100,1,0,'FSArch.1s;FSArch.1m;FSArch.1h;','','','','',0);
INSERT INTO "Archive_val" VALUES('F_PP1_var','','',1,2,'LogicLev.experiment.F_PP1.var',4,1.0,100,1,1,'FSArch.1s;FSArch.1m;FSArch.1h;','','','','',0);
INSERT INTO "Archive_val" VALUES('Ti_var','','',1,2,'LogicLev.experiment.Ti.var',4,1.0,100,1,1,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('T_PP1_var','','',1,2,'LogicLev.experiment.T_PP1.var',4,1.0,100,1,1,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('T_PP3_var','','',1,2,'LogicLev.experiment.T_PP3.var',4,1.0,100,1,1,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('T_PP5_var','','',1,2,'LogicLev.experiment.T_PP5.var',4,1.0,100,1,1,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('PT0204_var','','',1,2,'BlockCalc.KM101.PT0204.var',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('F103_var','','',1,2,'BlockCalc.KM102.F103.var',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('PT0204_var0','','',1,2,'BlockCalc.KM102.PT0204.var',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('P103_var','','',1,2,'BlockCalc.KM102.P103.var',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('PT1006_1_var0','','',1,2,'BlockCalc.KM102.PT1006_1.var',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('ST8612_var0','','',1,2,'BlockCalc.KM102.ST8612.var',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('TE1313_1_var0','','',1,2,'BlockCalc.KM102.TE1313_1.var',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('TE1314_1_var0','','',1,2,'BlockCalc.KM102.TE1314_1.var',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('TE1314_2_var0','','',1,2,'BlockCalc.KM102.TE1314_2.var',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('test_reg0','','',1,2,'ModBus.testRTU.test.reg0',1,1.0,100,1,1,'','','','','',0);
INSERT INTO "Archive_val" VALUES('ST8612_var1','','',1,2,'BlockCalc.KM201.ST8612.var',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('F101_var0','','',1,2,'BlockCalc.KM201.F101.var',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('P101_var0','','',1,2,'BlockCalc.KM201.P101.var',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('PT1006_1_var1','','',1,2,'BlockCalc.KM201.PT1006_1.var',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('PT0204_var1','','',1,2,'BlockCalc.KM201.PT0204.var',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('TE1313_1_var1','','',1,2,'BlockCalc.KM201.TE1313_1.var',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('TE1314_1_var1','','',1,2,'BlockCalc.KM201.TE1314_1.var',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('TE1314_2_var1','','',1,2,'BlockCalc.KM201.TE1314_2.var',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('F103_var0','','',1,2,'BlockCalc.КМ202.F103.var',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('TE1313_1_var2','','',1,2,'BlockCalc.КМ202.TE1313_1.var',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('PT0204_var2','','',1,2,'BlockCalc.КМ202.PT0204.var',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('TE1314_1_var2','','',1,2,'BlockCalc.КМ202.TE1314_1.var',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('TE1314_2_var2','','',1,2,'BlockCalc.КМ202.TE1314_2.var',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('PT1006_1_var2','','',1,2,'BlockCalc.КМ202.PT1006_1.var',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('P103_var1','','',1,2,'BlockCalc.КМ202.P103.var',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('ST8612_var2','','',1,2,'BlockCalc.КМ202.ST8612.var',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('PC_КРД1_sp','','',1,2,'BlockCalc.Anast1to2node_cntr.PC_КРД1.sp',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('PC_КРД1_out','','',1,2,'BlockCalc.Anast1to2node_cntr.PC_КРД1.out',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('PC_КРД1_auto','','',1,2,'BlockCalc.Anast1to2node_cntr.PC_КРД1.auto',0,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('PC_КРД2_sp','','',1,2,'BlockCalc.Anast1to2node_cntr.PC_КРД2.sp',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('PC_КРД2_out','','',1,2,'BlockCalc.Anast1to2node_cntr.PC_КРД2.out',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('PC_КРД2_auto','','',1,2,'BlockCalc.Anast1to2node_cntr.PC_КРД2.auto',0,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('PC_КРД3_sp','','',1,2,'BlockCalc.Anast1to2node_cntr.PC_КРД3.sp',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('PC_КРД3_out','','',1,2,'BlockCalc.Anast1to2node_cntr.PC_КРД3.out',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('PC_КРД3_auto','','',1,2,'BlockCalc.Anast1to2node_cntr.PC_КРД3.auto',0,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('PT1006_1_var3','','',1,2,'BlockCalc.KM301.PT1006_1.var',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('P101_var1','','',1,2,'BlockCalc.KM301.P101.var',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('TE1314_1_var3','','',1,2,'BlockCalc.KM301.TE1314_1.var',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('TE1314_2_var3','','',1,2,'BlockCalc.KM301.TE1314_2.var',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('TE1313_1_var3','','',1,2,'BlockCalc.KM301.TE1313_1.var',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('F101_var1','','',1,2,'BlockCalc.KM301.F101.var',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('ST8612_var3','','',1,2,'BlockCalc.KM301.ST8612.var',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('PT0204_var3','','',1,2,'BlockCalc.KM301.PT0204.var',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('P103_var2','','',1,2,'BlockCalc.KM302.P103.var',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('F103_var1','','',1,2,'BlockCalc.KM302.F103.var',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('PT1006_1_var4','','',1,2,'BlockCalc.KM302.PT1006_1.var',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('TE1314_1_var4','','',1,2,'BlockCalc.KM302.TE1314_1.var',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('TE1314_2_var4','','',1,2,'BlockCalc.KM302.TE1314_2.var',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('PT0204_var4','','',1,2,'BlockCalc.KM302.PT0204.var',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('TE1313_1_var4','','',1,2,'BlockCalc.KM302.TE1313_1.var',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('ST8612_var4','','',1,2,'BlockCalc.KM302.ST8612.var',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('SurgeKM101_sp','','',1,2,'BlockCalc.Anast1to2node_cntr.SurgeKM101.sp',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('SurgeKM101_var','','',1,2,'BlockCalc.Anast1to2node_cntr.SurgeKM101.var',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('SurgeKM101_out','','',1,2,'BlockCalc.Anast1to2node_cntr.SurgeKM101.out',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('SurgeKM101_auto','','',1,2,'BlockCalc.Anast1to2node_cntr.SurgeKM101.auto',0,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('SurgeKM201_sp0','','',1,2,'BlockCalc.Anast1to2node_cntr.SurgeKM201.sp',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('SurgeKM201_var0','','',1,2,'BlockCalc.Anast1to2node_cntr.SurgeKM201.var',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('SurgeKM201_out0','','',1,2,'BlockCalc.Anast1to2node_cntr.SurgeKM201.out',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('SurgeKM201_auto0','','',1,2,'BlockCalc.Anast1to2node_cntr.SurgeKM201.auto',0,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('SurgeKM301_sp','','',1,2,'BlockCalc.Anast1to2node_cntr.SurgeKM301.sp',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('SurgeKM301_var','','',1,2,'BlockCalc.Anast1to2node_cntr.SurgeKM301.var',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('SurgeKM301_out','','',1,2,'BlockCalc.Anast1to2node_cntr.SurgeKM301.out',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('SurgeKM301_auto','','',1,2,'BlockCalc.Anast1to2node_cntr.SurgeKM301.auto',0,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('SurgeKM102_sp','','',1,2,'BlockCalc.Anast1to2node_cntr.SurgeKM102.sp',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('SurgeKM102_var','','',1,2,'BlockCalc.Anast1to2node_cntr.SurgeKM102.var',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('SurgeKM102_out','','',1,2,'BlockCalc.Anast1to2node_cntr.SurgeKM102.out',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('SurgeKM102_auto','','',1,2,'BlockCalc.Anast1to2node_cntr.SurgeKM102.auto',0,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('SurgeKM202_sp','','',1,2,'BlockCalc.Anast1to2node_cntr.SurgeKM202.sp',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('SurgeKM202_var','','',1,2,'BlockCalc.Anast1to2node_cntr.SurgeKM202.var',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('SurgeKM202_out','','',1,2,'BlockCalc.Anast1to2node_cntr.SurgeKM202.out',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('SurgeKM202_auto','','',1,2,'BlockCalc.Anast1to2node_cntr.SurgeKM202.auto',0,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('SurgeKM302_sp','','',1,2,'BlockCalc.Anast1to2node_cntr.SurgeKM302.sp',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('SurgeKM302_var','','',1,2,'BlockCalc.Anast1to2node_cntr.SurgeKM302.var',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('SurgeKM302_out','','',1,2,'BlockCalc.Anast1to2node_cntr.SurgeKM302.out',4,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('SurgeKM302_auto','','',1,2,'BlockCalc.Anast1to2node_cntr.SurgeKM302.auto',0,1.0,100,1,0,'FSArch.1s;','','','','',0);
INSERT INTO "Archive_val" VALUES('test_t1_1','','',1,0,'',4,1.0,100,1,0,'FSArch.1h;','','','','',0);
CREATE TABLE 'Archive_val_proc' ("ID" TEXT DEFAULT '' ,"MODUL" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"START" INTEGER DEFAULT '' ,"ADDR" TEXT DEFAULT '' ,"V_PER" DOUBLE DEFAULT '' ,"A_PER" INTEGER DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"A_PRMS" TEXT DEFAULT '' , PRIMARY KEY ("ID","MODUL"));
INSERT INTO "Archive_val_proc" VALUES('1m','FSArch','','Minute''s archive',1,'ARCHIVES/VAL/1m',60.0,60,'','Хвилинний архів','','Минутный архив','<prms TmSize="800" NFiles="100" MaxCapacity="0" Round="0.01" PackTm="10" CheckTm="60" PackInfoFiles="0" />
');
INSERT INTO "Archive_val_proc" VALUES('1s','FSArch','','Second''s archive',1,'ARCHIVES/VAL/1s',1.0,60,'','Секундний архів.','','Секундный архив.','<prms TmSize="800" NFiles="100" MaxCapacity="0" Round="0.01" PackTm="10" CheckTm="60" PackInfoFiles="1" />
');
INSERT INTO "Archive_val_proc" VALUES('1s','DBArch','','',0,'MySQL.arch',1.0,60,'','','','','<prms Size="25" />
');
INSERT INTO "Archive_val_proc" VALUES('1h','FSArch','','Hour''s archive',1,'ARCHIVES/VAL/1h',3600.0,60,'','Годинковий архів.','','Часовой архив.','<prms TmSize="800" NFiles="100" MaxCapacity="0" Round="0.01" PackTm="10" CheckTm="60" PackInfoFiles="0" />
');
CREATE TABLE 'BFN_SymbAlarm' ("ID" INTEGER DEFAULT '' ,"CODE" INTEGER DEFAULT '' ,"TEXT" TEXT DEFAULT '' ,"ru#TEXT" TEXT DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO "BFN_SymbAlarm" VALUES(0,10,'Сбой датчика внутренней темп.','Сбой датчика внутренней темп.');
INSERT INTO "BFN_SymbAlarm" VALUES(1,10,'Высокая темп., Лето','Высокая темп., Лето');
INSERT INTO "BFN_SymbAlarm" VALUES(2,10,'Высокая темп.','Высокая темп.');
INSERT INTO "BFN_SymbAlarm" VALUES(3,10,'Низкая темп.','Низкая темп.');
INSERT INTO "BFN_SymbAlarm" VALUES(4,10,'Абсолют высокой темп.','Абсолют высокой темп.');
INSERT INTO "BFN_SymbAlarm" VALUES(5,64,'Неправильное откр. клапана 1','Неправильное откр. клапана 1');
INSERT INTO "BFN_SymbAlarm" VALUES(6,68,'Неправильное откр. клапана 2','Неправильное откр. клапана 2');
INSERT INTO "BFN_SymbAlarm" VALUES(7,40,'Абсолют высокой влажности','Абсолют высокой влажности');
INSERT INTO "BFN_SymbAlarm" VALUES(8,19,'Сбой датчика внешней темп.','Сбой датчика внешней темп.');
INSERT INTO "BFN_SymbAlarm" VALUES(9,40,'Сбой датчика внутренней темп.','Сбой датчика внутренней темп.');
INSERT INTO "BFN_SymbAlarm" VALUES(10,19,'Неправильн. располож. датчика внешн. темп.','Неправильн. располож. датчика внешн. темп.');
INSERT INTO "BFN_SymbAlarm" VALUES(11,121,'Батареи аварийного открытия','Батареи аварийного открытия');
INSERT INTO "BFN_SymbAlarm" VALUES(12,10,'Аварийное открытие','Аварийное открытие');
INSERT INTO "BFN_SymbAlarm" VALUES(13,0,'Тест аварийной сигнализации','Тест аварийной сигнализации');
INSERT INTO "BFN_SymbAlarm" VALUES(14,0,'Сбой системы','Сбой системы');
INSERT INTO "BFN_SymbAlarm" VALUES(15,66,'Ложное открыт. клапана 1В','Ложное открыт. клапана 1В');
INSERT INTO "BFN_SymbAlarm" VALUES(16,70,'Ложное открыт. клапана 2В','Ложное открыт. клапана 2В');
INSERT INTO "BFN_SymbAlarm" VALUES(70,0,'Отклонение темп. обогреваемой зоны','Отклонение темп. обогреваемой зоны');
INSERT INTO "BFN_SymbAlarm" VALUES(71,0,'Неправ. положение бок. притока 1','Неправ. положение бок. притока 1');
CREATE TABLE 'BFN_SymbCode' ("ID" INTEGER DEFAULT '' ,"TEXT" TEXT DEFAULT '' ,"ru#TEXT" TEXT DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO "BFN_SymbCode" VALUES(1,'Партия - часы','Партия - часы');
INSERT INTO "BFN_SymbCode" VALUES(2,'Часы реального времени','Часы реального времени');
INSERT INTO "BFN_SymbCode" VALUES(8,'Статус','Статус');
INSERT INTO "BFN_SymbCode" VALUES(10,'Внутр. температура','Внутр. температура');
INSERT INTO "BFN_SymbCode" VALUES(11,'Установл. темп.','Установл. темп.');
INSERT INTO "BFN_SymbCode" VALUES(12,'Установл. темп.(включая темп.комф.)','Установл. темп.(включая темп.комф.)');
INSERT INTO "BFN_SymbCode" VALUES(13,'Макс.темп. текущ.сутки','Макс.темп. текущ.сутки');
INSERT INTO "BFN_SymbCode" VALUES(14,'Миним.темп. текущ.сутки','Миним.темп. текущ.сутки');
INSERT INTO "BFN_SymbCode" VALUES(19,'Внешняя темп.','Внешняя темп.');
INSERT INTO "BFN_SymbCode" VALUES(21,'Темп.дополн.вентиляции','Темп.дополн.вентиляции');
INSERT INTO "BFN_SymbCode" VALUES(22,'Актуальная темп.комфорта','Актуальная темп.комфорта');
INSERT INTO "BFN_SymbCode" VALUES(23,'Макс. допустимая темп.комфорта','Макс. допустимая темп.комфорта');
INSERT INTO "BFN_SymbCode" VALUES(25,'Контроль отопления','Контроль отопления');
INSERT INTO "BFN_SymbCode" VALUES(26,'Мин.отопление','Мин.отопление');
INSERT INTO "BFN_SymbCode" VALUES(31,'Темп. охлаждения','Темп. охлаждения');
INSERT INTO "BFN_SymbCode" VALUES(33,'Потребн. охлаждения','Потребн. охлаждения');
INSERT INTO "BFN_SymbCode" VALUES(34,'Актив-ть мин. отопление','Актив-ть мин. отопление');
INSERT INTO "BFN_SymbCode" VALUES(35,'Темп. панели CPU','Темп. панели CPU');
INSERT INTO "BFN_SymbCode" VALUES(36,'Set temperature front','Set temperature front');
INSERT INTO "BFN_SymbCode" VALUES(37,'Set temperature front (incl. comfort temp.)','Set temperature front (incl. comfort temp.)');
INSERT INTO "BFN_SymbCode" VALUES(38,'Set humidity front','Set humidity front');
INSERT INTO "BFN_SymbCode" VALUES(39,'Air demand front','Air demand front');
INSERT INTO "BFN_SymbCode" VALUES(40,'Внутренняя влажность','Внутренняя влажность');
INSERT INTO "BFN_SymbCode" VALUES(41,'Установлен.влажность','Установлен.влажность');
INSERT INTO "BFN_SymbCode" VALUES(45,'Контроль влажности','Контроль влажности');
INSERT INTO "BFN_SymbCode" VALUES(49,'Set max. ventilation front','Set max. ventilation front');
INSERT INTO "BFN_SymbCode" VALUES(50,'Точка начала увлажнения','Точка начала увлажнения');
INSERT INTO "BFN_SymbCode" VALUES(51,'Необходим.увлажнения','Необходим.увлажнения');
INSERT INTO "BFN_SymbCode" VALUES(54,'Humidity - front','Humidity - front');
INSERT INTO "BFN_SymbCode" VALUES(59,'Влажность - ТЫЛ','Влажность - ТЫЛ');
INSERT INTO "BFN_SymbCode" VALUES(60,'Потребн.воздуха','Потребн.воздуха');
INSERT INTO "BFN_SymbCode" VALUES(61,'Мин. Вентиляция','Мин. Вентиляция');
INSERT INTO "BFN_SymbCode" VALUES(62,'Установл. макс. вент-ция','Установл. макс. вент-ция');
INSERT INTO "BFN_SymbCode" VALUES(68,'Позиц. клапана 2','Позиц. клапана 2');
INSERT INTO "BFN_SymbCode" VALUES(70,'Позиц. клапана 2В','Позиц. клапана 2В');
INSERT INTO "BFN_SymbCode" VALUES(79,'Min. Ventilation front','Min. Ventilation front');
INSERT INTO "BFN_SymbCode" VALUES(99,'Min. Ventilation rear','Min. Ventilation rear');
INSERT INTO "BFN_SymbCode" VALUES(100,'Сигн-ция высокой темп.','Сигн-ция высокой темп.');
INSERT INTO "BFN_SymbCode" VALUES(101,'Сигн-ция низкой темп.','Сигн-ция низкой темп.');
INSERT INTO "BFN_SymbCode" VALUES(102,'Статус сигн-ции низкой темп.','Статус сигн-ции низкой темп.');
INSERT INTO "BFN_SymbCode" VALUES(103,'Летн. сигн-ция при темп. 20 °C','Летн. сигн-ция при темп. 20 °C');
INSERT INTO "BFN_SymbCode" VALUES(104,'Летн. сигн-ция при темп. 30 °C','Летн. сигн-ция при темп. 30 °C');
INSERT INTO "BFN_SymbCode" VALUES(105,'Сигн-ция абсолютно высок. темп.','Сигн-ция абсолютно высок. темп.');
INSERT INTO "BFN_SymbCode" VALUES(106,'Сигн-ция внешн. темп.','Сигн-ция внешн. темп.');
INSERT INTO "BFN_SymbCode" VALUES(107,'Неправ. располож. внеш. Датчика','Неправ. располож. внеш. Датчика');
INSERT INTO "BFN_SymbCode" VALUES(110,'Сигн-ция абсолют. влажности','Сигн-ция абсолют. влажности');
INSERT INTO "BFN_SymbCode" VALUES(111,'Сигн-ция высок. влажности','Сигн-ция высок. влажности');
INSERT INTO "BFN_SymbCode" VALUES(112,'Сигн-ция датчика влажности','Сигн-ция датчика влажности');
INSERT INTO "BFN_SymbCode" VALUES(119,'Режим сигнализации','Режим сигнализации');
INSERT INTO "BFN_SymbCode" VALUES(120,'Темп. аварийн.открытия','Темп. аварийн.открытия');
INSERT INTO "BFN_SymbCode" VALUES(126,'Активированная сигн-ция','Активированная сигн-ция');
INSERT INTO "BFN_SymbCode" VALUES(130,'Темп. график - темп. 1','Темп. график - темп. 1');
INSERT INTO "BFN_SymbCode" VALUES(131,'Темп. график - темп. 2','Темп. график - темп. 2');
INSERT INTO "BFN_SymbCode" VALUES(132,'Темп. график - темп. 3','Темп. график - темп. 3');
INSERT INTO "BFN_SymbCode" VALUES(133,'Темп. график - темп. 4','Темп. график - темп. 4');
INSERT INTO "BFN_SymbCode" VALUES(134,'Темп. график - темп. 5','Темп. график - темп. 5');
INSERT INTO "BFN_SymbCode" VALUES(135,'Темп. график - темп. 6','Темп. график - темп. 6');
INSERT INTO "BFN_SymbCode" VALUES(136,'Темп. график - день 1','Темп. график - день 1');
INSERT INTO "BFN_SymbCode" VALUES(137,'Темп. график - день 2','Темп. график - день 2');
INSERT INTO "BFN_SymbCode" VALUES(138,'Темп. график - день 3','Темп. график - день 3');
INSERT INTO "BFN_SymbCode" VALUES(139,'Темп. график - день 4','Темп. график - день 4');
INSERT INTO "BFN_SymbCode" VALUES(140,'Темп. график - день 5','Темп. график - день 5');
INSERT INTO "BFN_SymbCode" VALUES(141,'Темп. график - день 6','Темп. график - день 6');
INSERT INTO "BFN_SymbCode" VALUES(142,'График влажн. - влажн. 1','График влажн. - влажн. 1');
INSERT INTO "BFN_SymbCode" VALUES(143,'График влажн. - влажн. 2','График влажн. - влажн. 2');
INSERT INTO "BFN_SymbCode" VALUES(144,'График влажн. - влажн. 3','График влажн. - влажн. 3');
INSERT INTO "BFN_SymbCode" VALUES(145,'График влажн. - влажн. 4','График влажн. - влажн. 4');
INSERT INTO "BFN_SymbCode" VALUES(146,'График влажн. - влажн. 5','График влажн. - влажн. 5');
INSERT INTO "BFN_SymbCode" VALUES(147,'График влажн. - влажн. 6','График влажн. - влажн. 6');
INSERT INTO "BFN_SymbCode" VALUES(148,'График  влажн. - 1-ый день','График  влажн. - 1-ый день');
INSERT INTO "BFN_SymbCode" VALUES(149,'График  влажн. - 2-ой день','График  влажн. - 2-ой день');
INSERT INTO "BFN_SymbCode" VALUES(150,'График  влажн. - 3-ий день','График  влажн. - 3-ий день');
INSERT INTO "BFN_SymbCode" VALUES(151,'График  влажн. - 4-ый день','График  влажн. - 4-ый день');
INSERT INTO "BFN_SymbCode" VALUES(152,'График  влажн. - 5-ый день','График  влажн. - 5-ый день');
INSERT INTO "BFN_SymbCode" VALUES(153,'График  влажн. - 6-ой день','График  влажн. - 6-ой день');
INSERT INTO "BFN_SymbCode" VALUES(154,'Граф. мин. вент-ции - вент-ция 1','Граф. мин. вент-ции - вент-ция 1');
INSERT INTO "BFN_SymbCode" VALUES(155,'Граф. мин. вент-ции - вент-ция 2','Граф. мин. вент-ции - вент-ция 2');
INSERT INTO "BFN_SymbCode" VALUES(156,'Граф. мин. вент-ции - вент-ция 3','Граф. мин. вент-ции - вент-ция 3');
INSERT INTO "BFN_SymbCode" VALUES(157,'Граф. мин. вент-ции - вент-ция 4','Граф. мин. вент-ции - вент-ция 4');
INSERT INTO "BFN_SymbCode" VALUES(158,'Граф. мин. вент-ции - вент-ция 5','Граф. мин. вент-ции - вент-ция 5');
INSERT INTO "BFN_SymbCode" VALUES(159,'Граф. мин. вент-ции - вент-ция 6','Граф. мин. вент-ции - вент-ция 6');
INSERT INTO "BFN_SymbCode" VALUES(160,'Граф. мин. вент-ции - 1-ый день','Граф. мин. вент-ции - 1-ый день');
INSERT INTO "BFN_SymbCode" VALUES(161,'Граф. мин. вент-ции - 2-ый день','Граф. мин. вент-ции - 2-ый день');
INSERT INTO "BFN_SymbCode" VALUES(162,'Граф. мин. вент-ции - 3-ый день','Граф. мин. вент-ции - 3-ый день');
INSERT INTO "BFN_SymbCode" VALUES(163,'Граф. мин. вент-ции - 4-ый день','Граф. мин. вент-ции - 4-ый день');
INSERT INTO "BFN_SymbCode" VALUES(164,'Граф. мин. вент-ции - 5-ый день','Граф. мин. вент-ции - 5-ый день');
INSERT INTO "BFN_SymbCode" VALUES(165,'Граф. мин. вент-ции - 6-ый день','Граф. мин. вент-ции - 6-ый день');
INSERT INTO "BFN_SymbCode" VALUES(166,'Граф. макс. вент-ции - вент-ция 1','Граф. макс. вент-ции - вент-ция 1');
INSERT INTO "BFN_SymbCode" VALUES(167,'Граф. макс. вент-ции - вент-ция 2','Граф. макс. вент-ции - вент-ция 2');
INSERT INTO "BFN_SymbCode" VALUES(168,'Граф. макс. вент-ции - вент-ция 3','Граф. макс. вент-ции - вент-ция 3');
INSERT INTO "BFN_SymbCode" VALUES(169,'Граф. макс. вент-ции - вент-ция 4','Граф. макс. вент-ции - вент-ция 4');
INSERT INTO "BFN_SymbCode" VALUES(170,'Граф. макс. вент-ции - вент-ция 5','Граф. макс. вент-ции - вент-ция 5');
INSERT INTO "BFN_SymbCode" VALUES(171,'Граф. макс. вент-ции - вент-ция 6','Граф. макс. вент-ции - вент-ция 6');
INSERT INTO "BFN_SymbCode" VALUES(172,'График макс. вент-ции - 1-ый день','График макс. вент-ции - 1-ый день');
INSERT INTO "BFN_SymbCode" VALUES(173,'График макс. вент-ции - 2-ый день','График макс. вент-ции - 2-ый день');
INSERT INTO "BFN_SymbCode" VALUES(174,'График макс. вент-ции - 3-ый день','График макс. вент-ции - 3-ый день');
INSERT INTO "BFN_SymbCode" VALUES(175,'График макс. вент-ции - 4-ый день','График макс. вент-ции - 4-ый день');
INSERT INTO "BFN_SymbCode" VALUES(176,'График макс. вент-ции - 5-ый день','График макс. вент-ции - 5-ый день');
INSERT INTO "BFN_SymbCode" VALUES(177,'График макс. вент-ции - 6-ый день','График макс. вент-ции - 6-ый день');
INSERT INTO "BFN_SymbCode" VALUES(178,'Низкое давлен. Вытяжка 1','Низкое давлен. Вытяжка 1');
INSERT INTO "BFN_SymbCode" VALUES(179,'Низкое давлен. Вытяжка 2','Низкое давлен. Вытяжка 2');
INSERT INTO "BFN_SymbCode" VALUES(180,'Низкое давлен. Вытяжка 3','Низкое давлен. Вытяжка 3');
INSERT INTO "BFN_SymbCode" VALUES(181,'Низкое давлен. Вытяжка 4','Низкое давлен. Вытяжка 4');
INSERT INTO "BFN_SymbCode" VALUES(182,'Низкое давлен. Вытяжка 5','Низкое давлен. Вытяжка 5');
INSERT INTO "BFN_SymbCode" VALUES(183,'Низкое давлен. Вытяжка 6','Низкое давлен. Вытяжка 6');
INSERT INTO "BFN_SymbCode" VALUES(184,'Низкое давлен. Вытяжка 7','Низкое давлен. Вытяжка 7');
INSERT INTO "BFN_SymbCode" VALUES(185,'Низкое давлен. Вытяжка 8','Низкое давлен. Вытяжка 8');
INSERT INTO "BFN_SymbCode" VALUES(186,'Низкое давлен.Клапан 1, поз.1','Низкое давлен.Клапан 1, поз.1');
INSERT INTO "BFN_SymbCode" VALUES(187,'Низкое давлен.Клапан 1, поз.2','Низкое давлен.Клапан 1, поз.2');
INSERT INTO "BFN_SymbCode" VALUES(188,'Низкое давлен.Клапан 1, поз.3','Низкое давлен.Клапан 1, поз.3');
INSERT INTO "BFN_SymbCode" VALUES(189,'Низкое давлен.Клапан 1, поз.4','Низкое давлен.Клапан 1, поз.4');
INSERT INTO "BFN_SymbCode" VALUES(190,'Низкое давлен.Клапан 1, поз.5','Низкое давлен.Клапан 1, поз.5');
INSERT INTO "BFN_SymbCode" VALUES(191,'Низкое давлен.Клапан 1, поз.6','Низкое давлен.Клапан 1, поз.6');
INSERT INTO "BFN_SymbCode" VALUES(192,'Низкое давлен.Клапан 1, поз.7','Низкое давлен.Клапан 1, поз.7');
INSERT INTO "BFN_SymbCode" VALUES(193,'Низкое давлен.Клапан 1, поз.8','Низкое давлен.Клапан 1, поз.8');
INSERT INTO "BFN_SymbCode" VALUES(234,'Темп. график - темп. 1(отопл.)','Темп. график - темп. 1(отопл.)');
INSERT INTO "BFN_SymbCode" VALUES(235,'Темп. график - темп. 2(отопл.)','Темп. график - темп. 2(отопл.)');
INSERT INTO "BFN_SymbCode" VALUES(236,'Темп. график - темп. 3(отопл.)','Темп. график - темп. 3(отопл.)');
INSERT INTO "BFN_SymbCode" VALUES(237,'Темп. график - темп. 4(отопл.)','Темп. график - темп. 4(отопл.)');
INSERT INTO "BFN_SymbCode" VALUES(238,'Темп. график - темп. 5(отопл.)','Темп. график - темп. 5(отопл.)');
INSERT INTO "BFN_SymbCode" VALUES(239,'Темп. график - темп. 6(отопл.)','Темп. график - темп. 6(отопл.)');
INSERT INTO "BFN_SymbCode" VALUES(250,'Ф-ция пауза','Ф-ция пауза');
INSERT INTO "BFN_SymbCode" VALUES(258,'Помещ. пусто: вент-ры','Помещ. пусто: вент-ры');
INSERT INTO "BFN_SymbCode" VALUES(265,'Помещ. пусто: отопл.','Помещ. пусто: отопл.');
INSERT INTO "BFN_SymbCode" VALUES(270,'Статус морозостойкости','Статус морозостойкости');
INSERT INTO "BFN_SymbCode" VALUES(271,'Помещ. пусто: мин. темп.','Помещ. пусто: мин. темп.');
INSERT INTO "BFN_SymbCode" VALUES(300,'Вручную/Автоматически','Вручную/Автоматически');
INSERT INTO "BFN_SymbCode" VALUES(301,'Фактор комфорта','Фактор комфорта');
INSERT INTO "BFN_SymbCode" VALUES(302,'Пуск вент-ции комфорта','Пуск вент-ции комфорта');
INSERT INTO "BFN_SymbCode" VALUES(303,'Макс. комфорт при','Макс. комфорт при');
INSERT INTO "BFN_SymbCode" VALUES(310,'Номинальн. производит.','Номинальн. производит.');
INSERT INTO "BFN_SymbCode" VALUES(311,'Бесступ. производит.','Бесступ. производит.');
INSERT INTO "BFN_SymbCode" VALUES(312,'Производит. MultiStep 1','Производит. MultiStep 1');
INSERT INTO "BFN_SymbCode" VALUES(313,'Производит. MultiStep 2','Производит. MultiStep 2');
INSERT INTO "BFN_SymbCode" VALUES(314,'Производит. MultiStep 3','Производит. MultiStep 3');
INSERT INTO "BFN_SymbCode" VALUES(315,'Производит. MultiStep 4','Производит. MultiStep 4');
INSERT INTO "BFN_SymbCode" VALUES(316,'Производит. MultiStep 5','Производит. MultiStep 5');
INSERT INTO "BFN_SymbCode" VALUES(317,'Производит. MultiStep 6','Производит. MultiStep 6');
INSERT INTO "BFN_SymbCode" VALUES(356,'Произв. Бесступ. 2','Произв. Бесступ. 2');
INSERT INTO "BFN_SymbCode" VALUES(365,'Макс. граница понижения темп. для удаления влажн.','Макс. граница понижения темп. для удаления влажн.');
INSERT INTO "BFN_SymbCode" VALUES(382,'Время цикла охлажд.','Время цикла охлажд.');
INSERT INTO "BFN_SymbCode" VALUES(418,'Настройка-режим работы','Настройка-режим работы');
INSERT INTO "BFN_SymbCode" VALUES(419,'Вид поголовья','Вид поголовья');
INSERT INTO "BFN_SymbCode" VALUES(424,'Режим управления отопл.','Режим управления отопл.');
INSERT INTO "BFN_SymbCode" VALUES(425,'Датчик влажн. выбран','Датчик влажн. выбран');
INSERT INTO "BFN_SymbCode" VALUES(435,'Кол-во групп MultiStep','Кол-во групп MultiStep');
INSERT INTO "BFN_SymbCode" VALUES(440,'Увлажнение подключено','Увлажнение подключено');
INSERT INTO "BFN_SymbCode" VALUES(444,'Счетчик воды','Счетчик воды');
INSERT INTO "BFN_SymbCode" VALUES(462,'Низкое давлен.Клапан 1В, поз.1','Низкое давлен.Клапан 1В, поз.1');
INSERT INTO "BFN_SymbCode" VALUES(463,'Низкое давлен.Клапан 1В, поз.2','Низкое давлен.Клапан 1В, поз.2');
INSERT INTO "BFN_SymbCode" VALUES(464,'Низкое давлен.Клапан 1В, поз.3','Низкое давлен.Клапан 1В, поз.3');
INSERT INTO "BFN_SymbCode" VALUES(465,'Низкое давлен.Клапан 1В, поз.4','Низкое давлен.Клапан 1В, поз.4');
INSERT INTO "BFN_SymbCode" VALUES(466,'Низкое давлен.Клапан 1В, поз.5','Низкое давлен.Клапан 1В, поз.5');
INSERT INTO "BFN_SymbCode" VALUES(467,'Низкое давлен.Клапан 1В, поз.6','Низкое давлен.Клапан 1В, поз.6');
INSERT INTO "BFN_SymbCode" VALUES(468,'Низкое давлен.Клапан 1В, поз.7','Низкое давлен.Клапан 1В, поз.7');
INSERT INTO "BFN_SymbCode" VALUES(469,'Низкое давлен.Клапан 1В, поз.8','Низкое давлен.Клапан 1В, поз.8');
INSERT INTO "BFN_SymbCode" VALUES(470,'Низкое давлен.Клапан 1В, поз.9','Низкое давлен.Клапан 1В, поз.9');
INSERT INTO "BFN_SymbCode" VALUES(471,'Низкое давлен.Клапан 1В, поз.10','Низкое давлен.Клапан 1В, поз.10');
INSERT INTO "BFN_SymbCode" VALUES(480,'Общий расход воды','Общий расход воды');
INSERT INTO "BFN_SymbCode" VALUES(481,'Расход воды вчера','Расход воды вчера');
INSERT INTO "BFN_SymbCode" VALUES(482,'Расход воды сегодня','Расход воды сегодня');
INSERT INTO "BFN_SymbCode" VALUES(484,'Литров за импульс','Литров за импульс');
INSERT INTO "BFN_SymbCode" VALUES(488,'Вытяжка 1 серводвиг.','Вытяжка 1 серводвиг.');
INSERT INTO "BFN_SymbCode" VALUES(489,'Вытяжка 2 серводвиг.','Вытяжка 2 серводвиг.');
INSERT INTO "BFN_SymbCode" VALUES(493,'Плата I/O (ввода-вывода), номер 1','Плата I/O (ввода-вывода), номер 1');
INSERT INTO "BFN_SymbCode" VALUES(494,'1-ый тип регулирования скорости','1-ый тип регулирования скорости');
INSERT INTO "BFN_SymbCode" VALUES(495,'2-ой тип регулирования скорости','2-ой тип регулирования скорости');
INSERT INTO "BFN_SymbCode" VALUES(496,'Уставка влажности - ТЫЛ','Уставка влажности - ТЫЛ');
INSERT INTO "BFN_SymbCode" VALUES(497,'Мин. темп. сегодня - ТЫЛ','Мин. темп. сегодня - ТЫЛ');
INSERT INTO "BFN_SymbCode" VALUES(498,'Макс. темп. сегодня - ТЫЛ','Макс. темп. сегодня - ТЫЛ');
INSERT INTO "BFN_SymbCode" VALUES(499,'Set temperature rear (incl. comfort temp.)','Set temperature rear (incl. comfort temp.)');
INSERT INTO "BFN_SymbCode" VALUES(579,'Плата I/O (ввода-вывода), номер 2','Плата I/O (ввода-вывода), номер 2');
INSERT INTO "BFN_SymbCode" VALUES(586,'Плата I/O (ввода-вывода), номер 3','Плата I/O (ввода-вывода), номер 3');
INSERT INTO "BFN_SymbCode" VALUES(620,'Низкое давлен. Вытяжка 9','Низкое давлен. Вытяжка 9');
INSERT INTO "BFN_SymbCode" VALUES(621,'Низкое давлен. Вытяжка 10','Низкое давлен. Вытяжка 10');
INSERT INTO "BFN_SymbCode" VALUES(622,'Низкое давлен.Клапан 1, поз.9','Низкое давлен.Клапан 1, поз.9');
INSERT INTO "BFN_SymbCode" VALUES(623,'Низкое давлен.Клапан 1, поз.10','Низкое давлен.Клапан 1, поз.10');
INSERT INTO "BFN_SymbCode" VALUES(636,'Кол-во голов в жив. Помещении','Кол-во голов в жив. Помещении');
INSERT INTO "BFN_SymbCode" VALUES(637,'Скорость воздуха','Скорость воздуха');
INSERT INTO "BFN_SymbCode" VALUES(638,'Ощущаемая темп.','Ощущаемая темп.');
INSERT INTO "BFN_SymbCode" VALUES(640,'Total air out demand','Total air out demand');
INSERT INTO "BFN_SymbCode" VALUES(641,'Установл. мин.скор.воздуха','Установл. мин.скор.воздуха');
INSERT INTO "BFN_SymbCode" VALUES(642,'Расчетн. макс. скор.воздуха','Расчетн. макс. скор.воздуха');
INSERT INTO "BFN_SymbCode" VALUES(646,'PAD температура','PAD температура');
INSERT INTO "BFN_SymbCode" VALUES(647,'Макс. влажн. при откл. охлажд.','Макс. влажн. при откл. охлажд.');
INSERT INTO "BFN_SymbCode" VALUES(673,'Потребн.в увлажн. - ТЫЛ','Потребн.в увлажн. - ТЫЛ');
INSERT INTO "BFN_SymbCode" VALUES(677,'Темп. график - день 7','Темп. график - день 7');
INSERT INTO "BFN_SymbCode" VALUES(678,'Темп. график - день 8','Темп. график - день 8');
INSERT INTO "BFN_SymbCode" VALUES(679,'Темп. график - темп. 7','Темп. график - темп. 7');
INSERT INTO "BFN_SymbCode" VALUES(680,'Темп. график - темп. 8','Темп. график - темп. 8');
INSERT INTO "BFN_SymbCode" VALUES(681,'Темп. график - темп. 7(отопл.)','Темп. график - темп. 7(отопл.)');
INSERT INTO "BFN_SymbCode" VALUES(682,'Темп. график - темп. 8(отопл.)','Темп. график - темп. 8(отопл.)');
INSERT INTO "BFN_SymbCode" VALUES(694,'Граф. мин. вент-ции - вент-ция 7','Граф. мин. вент-ции - вент-ция 7');
INSERT INTO "BFN_SymbCode" VALUES(695,'Граф. мин. вент-ции - вент-ция 8','Граф. мин. вент-ции - вент-ция 8');
INSERT INTO "BFN_SymbCode" VALUES(702,'Air demand rear','Air demand rear');
INSERT INTO "BFN_SymbCode" VALUES(712,'Производит. MultiStep 7','Производит. MultiStep 7');
INSERT INTO "BFN_SymbCode" VALUES(713,'Производит. MultiStep 8','Производит. MultiStep 8');
INSERT INTO "BFN_SymbCode" VALUES(723,'График влажн. - влажн. 7','График влажн. - влажн. 7');
INSERT INTO "BFN_SymbCode" VALUES(724,'График влажн. - влажн. 8','График влажн. - влажн. 8');
INSERT INTO "BFN_SymbCode" VALUES(725,'График макс. вент-ции - 7-ой день','График макс. вент-ции - 7-ой день');
INSERT INTO "BFN_SymbCode" VALUES(726,'График макс. вент-ции - 8-ой день','График макс. вент-ции - 8-ой день');
INSERT INTO "BFN_SymbCode" VALUES(727,'Граф. мин. вент-ции - 7-ой день','Граф. мин. вент-ции - 7-ой день');
INSERT INTO "BFN_SymbCode" VALUES(728,'Граф. мин. вент-ции - 8-ой день','Граф. мин. вент-ции - 8-ой день');
INSERT INTO "BFN_SymbCode" VALUES(735,'Temperature in front brooding zone 1','Temperature in front brooding zone 1');
INSERT INTO "BFN_SymbCode" VALUES(736,'Temperature in front brooding zone 2','Temperature in front brooding zone 2');
INSERT INTO "BFN_SymbCode" VALUES(737,'Temperature in front brooding zone 3','Temperature in front brooding zone 3');
INSERT INTO "BFN_SymbCode" VALUES(738,'Temperature in front brooding zone 4','Temperature in front brooding zone 4');
INSERT INTO "BFN_SymbCode" VALUES(749,'PAD temperature pump #2','PAD temperature pump #2');
INSERT INTO "BFN_SymbCode" VALUES(768,'Shutter opening #front','Shutter opening #front');
INSERT INTO "BFN_SymbCode" VALUES(769,'Shutter opening #rear','Shutter opening #rear');
INSERT INTO "BFN_SymbCode" VALUES(770,'Уставка актуального ночного понижения','Уставка актуального ночного понижения');
INSERT INTO "BFN_SymbCode" VALUES(771,'Актуальн.ночное понижение','Актуальн.ночное понижение');
INSERT INTO "BFN_SymbCode" VALUES(802,'Установл. мин. вент-ция','Установл. мин. вент-ция');
INSERT INTO "BFN_SymbCode" VALUES(818,'Set temperature rear','Set temperature rear');
INSERT INTO "BFN_SymbCode" VALUES(819,'Set humidity rear','Set humidity rear');
INSERT INTO "BFN_SymbCode" VALUES(821,'Установл. макс. вент-ция - ТЫЛ','Установл. макс. вент-ция - ТЫЛ');
INSERT INTO "BFN_SymbCode" VALUES(822,'Фактор быстрого охлажд. для данного возр.','Фактор быстрого охлажд. для данного возр.');
INSERT INTO "BFN_SymbCode" VALUES(830,'Граф. макс. вент-ции - вент-ция 7','Граф. макс. вент-ции - вент-ция 7');
INSERT INTO "BFN_SymbCode" VALUES(831,'Граф. макс. вент-ции - вент-ция 8','Граф. макс. вент-ции - вент-ция 8');
INSERT INTO "BFN_SymbCode" VALUES(834,'График  влажн. - 7-ой день','График  влажн. - 7-ой день');
INSERT INTO "BFN_SymbCode" VALUES(835,'График  влажн. - 8-ой день','График  влажн. - 8-ой день');
INSERT INTO "BFN_SymbCode" VALUES(872,'Brooding heater 1 temp. Setpoint - Front zone','Brooding heater 1 temp. Setpoint - Front zone');
INSERT INTO "BFN_SymbCode" VALUES(873,'Brooding heater 2 temp. setpoint - Front zone','Brooding heater 2 temp. setpoint - Front zone');
INSERT INTO "BFN_SymbCode" VALUES(874,'Brooding heater 1 temp. setpoint - Rear zone','Brooding heater 1 temp. setpoint - Rear zone');
INSERT INTO "BFN_SymbCode" VALUES(875,'Brooding heater 2 temp. setpoint - Rear zone','Brooding heater 2 temp. setpoint - Rear zone');
INSERT INTO "BFN_SymbCode" VALUES(930,'Front brooding heater 2 requirement','Front brooding heater 2 requirement');
INSERT INTO "BFN_SymbCode" VALUES(931,'Front brooding heater 1 requirement','Front brooding heater 1 requirement');
INSERT INTO "BFN_SymbCode" VALUES(932,'Rear brooding heater 2 requirement','Rear brooding heater 2 requirement');
INSERT INTO "BFN_SymbCode" VALUES(933,'Rear brooding heater 1 requirement','Rear brooding heater 1 requirement');
INSERT INTO "BFN_SymbCode" VALUES(934,'24 volt supply','24 volt supply');
INSERT INTO "BFN_SymbCode" VALUES(935,'24 volt supply (last 4 hour period)','24 volt supply (last 4 hour period)');
INSERT INTO "BFN_SymbCode" VALUES(948,'PAD start principle (0=m/s, 1= °C, 2= m/s+°C)','PAD start principle (0=m/s, 1= °C, 2= m/s+°C)');
INSERT INTO "BFN_SymbCode" VALUES(950,'Датчик внешн. среды 1 знач.','Датчик внешн. среды 1 знач.');
INSERT INTO "BFN_SymbCode" VALUES(951,'Датчик внешн. среды 2 знач.','Датчик внешн. среды 2 знач.');
INSERT INTO "BFN_SymbCode" VALUES(953,'Датчик внешн. среды 3 знач.','Датчик внешн. среды 3 знач.');
INSERT INTO "BFN_SymbCode" VALUES(954,'Датчик внешн. среды 4 знач.','Датчик внешн. среды 4 знач.');
INSERT INTO "BFN_SymbCode" VALUES(990,'Статус прогр. корма','Статус прогр. корма');
INSERT INTO "BFN_SymbCode" VALUES(1012,'Расх. воды/корма за прошлые сутки','Расх. воды/корма за прошлые сутки');
INSERT INTO "BFN_SymbCode" VALUES(1020,'Весы д/ животн. 1','Весы д/ животн. 1');
INSERT INTO "BFN_SymbCode" VALUES(1021,'Весы д/ животн. 2','Весы д/ животн. 2');
INSERT INTO "BFN_SymbCode" VALUES(1022,'Ежедневный привес, весы 1','Ежедневный привес, весы 1');
INSERT INTO "BFN_SymbCode" VALUES(1023,'Ежедневный привес, весы 2','Ежедневный привес, весы 2');
INSERT INTO "BFN_SymbCode" VALUES(1024,'Ежедневный привес, средний вес','Ежедневный привес, средний вес');
INSERT INTO "BFN_SymbCode" VALUES(1025,'Общий средн. вес животных','Общий средн. вес животных');
INSERT INTO "BFN_SymbCode" VALUES(1026,'Ориент.вес сейчас 1','Ориент.вес сейчас 1');
INSERT INTO "BFN_SymbCode" VALUES(1027,'Ориент.вес сейчас 2','Ориент.вес сейчас 2');
INSERT INTO "BFN_SymbCode" VALUES(1029,'Max. humidity where PAD cooling stops','Max. humidity where PAD cooling stops');
INSERT INTO "BFN_SymbCode" VALUES(1030,'Кол-во взвеш-х животных за данный период, весы 1','Кол-во взвеш-х животных за данный период, весы 1');
INSERT INTO "BFN_SymbCode" VALUES(1031,'Кол-во взвеш-х животных за данный период, весы 2','Кол-во взвеш-х животных за данный период, весы 2');
INSERT INTO "BFN_SymbCode" VALUES(1040,'Станд. отклонен. веса на весах 1','Станд. отклонен. веса на весах 1');
INSERT INTO "BFN_SymbCode" VALUES(1041,'Станд. отклонен. веса на весах 2','Станд. отклонен. веса на весах 2');
INSERT INTO "BFN_SymbCode" VALUES(1045,'Общее станд. отклонен. веса.','Общее станд. отклонен. веса.');
INSERT INTO "BFN_SymbCode" VALUES(1050,'Однородность веса животных 1','Однородность веса животных 1');
INSERT INTO "BFN_SymbCode" VALUES(1051,'Однородность веса животных 2','Однородность веса животных 2');
INSERT INTO "BFN_SymbCode" VALUES(1059,'Number of spray cooling relays','Number of spray cooling relays');
INSERT INTO "BFN_SymbCode" VALUES(1065,'Front inlet 1 position','Front inlet 1 position');
INSERT INTO "BFN_SymbCode" VALUES(1067,'Front inlet 2 position','Front inlet 2 position');
INSERT INTO "BFN_SymbCode" VALUES(1075,'Rear inlet 1 position','Rear inlet 1 position');
INSERT INTO "BFN_SymbCode" VALUES(1077,'Rear inlet 2 position','Rear inlet 2 position');
INSERT INTO "BFN_SymbCode" VALUES(1098,'Inside temperature front','Inside temperature front');
INSERT INTO "BFN_SymbCode" VALUES(1099,'Inside temperature rear','Inside temperature rear');
INSERT INTO "BFN_SymbCode" VALUES(1100,'Загружен. погол. 1','Загружен. погол. 1');
INSERT INTO "BFN_SymbCode" VALUES(1101,'Загружен. погол. 2','Загружен. погол. 2');
INSERT INTO "BFN_SymbCode" VALUES(1102,'Живого погол.1','Живого погол.1');
INSERT INTO "BFN_SymbCode" VALUES(1103,'Живого погол.2','Живого погол.2');
INSERT INTO "BFN_SymbCode" VALUES(1108,'Cooling requirement - front','Cooling requirement - front');
INSERT INTO "BFN_SymbCode" VALUES(1109,'Cooling requirement - rear','Cooling requirement - rear');
INSERT INTO "BFN_SymbCode" VALUES(1125,'Актуальн. падеж, погол. 1','Актуальн. падеж, погол. 1');
INSERT INTO "BFN_SymbCode" VALUES(1127,'Актуальн. падеж, погол. 2','Актуальн. падеж, погол. 2');
INSERT INTO "BFN_SymbCode" VALUES(1129,'Актуальн. падеж, погол. Итог','Актуальн. падеж, погол. Итог');
INSERT INTO "BFN_SymbCode" VALUES(1266,'Pure Tunnel installed','Pure Tunnel installed');
INSERT INTO "BFN_SymbCode" VALUES(1267,'Pure LPV installed','Pure LPV installed');
INSERT INTO "BFN_SymbCode" VALUES(1610,'Интенсивность света в темноте','Интенсивность света в темноте');
INSERT INTO "BFN_SymbCode" VALUES(1621,'Inside temperature 1','Inside temperature 1');
INSERT INTO "BFN_SymbCode" VALUES(1622,'Inside temperature 2','Inside temperature 2');
INSERT INTO "BFN_SymbCode" VALUES(1623,'Inside temperature 3','Inside temperature 3');
INSERT INTO "BFN_SymbCode" VALUES(1624,'Inside temperature 4','Inside temperature 4');
INSERT INTO "BFN_SymbCode" VALUES(1625,'Inside temperature 5','Inside temperature 5');
INSERT INTO "BFN_SymbCode" VALUES(1626,'Inside temperature 6','Inside temperature 6');
INSERT INTO "BFN_SymbCode" VALUES(1627,'Inside temperature 7','Inside temperature 7');
INSERT INTO "BFN_SymbCode" VALUES(1628,'Inside temperature 8','Inside temperature 8');
INSERT INTO "BFN_SymbCode" VALUES(1629,'Humidification demand - front','Humidification demand - front');
INSERT INTO "BFN_SymbCode" VALUES(1668,'Water meter 2 consumption total','Water meter 2 consumption total');
INSERT INTO "BFN_SymbCode" VALUES(1669,'Water meter 2 consumption yesterday','Water meter 2 consumption yesterday');
INSERT INTO "BFN_SymbCode" VALUES(1670,'Water meter 2 consumption today','Water meter 2 consumption today');
INSERT INTO "BFN_SymbCode" VALUES(1673,'Water meter 3 consumption total','Water meter 3 consumption total');
INSERT INTO "BFN_SymbCode" VALUES(1674,'Water meter 3 consumption yesterday','Water meter 3 consumption yesterday');
INSERT INTO "BFN_SymbCode" VALUES(1675,'Water meter 3 consumption today','Water meter 3 consumption today');
INSERT INTO "BFN_SymbCode" VALUES(1678,'Water meter 4 consumption total','Water meter 4 consumption total');
INSERT INTO "BFN_SymbCode" VALUES(1679,'Water meter 4 consumption yesterday','Water meter 4 consumption yesterday');
INSERT INTO "BFN_SymbCode" VALUES(1680,'Water meter 4 consumption today','Water meter 4 consumption today');
INSERT INTO "BFN_SymbCode" VALUES(1683,'Water meter 5 consumption total','Water meter 5 consumption total');
INSERT INTO "BFN_SymbCode" VALUES(1684,'Water meter 5 consumption yesterday','Water meter 5 consumption yesterday');
INSERT INTO "BFN_SymbCode" VALUES(1685,'Water meter 5 consumption today','Water meter 5 consumption today');
INSERT INTO "BFN_SymbCode" VALUES(1688,'Water meter 6 consumption total','Water meter 6 consumption total');
INSERT INTO "BFN_SymbCode" VALUES(1689,'Water meter 6 consumption yesterday','Water meter 6 consumption yesterday');
INSERT INTO "BFN_SymbCode" VALUES(1690,'Water meter 6 consumption today','Water meter 6 consumption today');
INSERT INTO "BFN_SymbCode" VALUES(2580,'Корм за импульс','Корм за импульс');
INSERT INTO "BFN_SymbCode" VALUES(2640,'Весы д/животн.  - Тип 1','Весы д/животн.  - Тип 1');
INSERT INTO "BFN_SymbCode" VALUES(2641,'Весы д/животн.  - Тип 2','Весы д/животн.  - Тип 2');
INSERT INTO "BFN_SymbCode" VALUES(2660,'Кол-во видов животных','Кол-во видов животных');
INSERT INTO "BFN_SymbCode" VALUES(2701,'Чашечная система кормления','Чашечная система кормления');
INSERT INTO "BFN_SymbCode" VALUES(3020,'Содержимое бункера 1','Содержимое бункера 1');
INSERT INTO "BFN_SymbCode" VALUES(3021,'Содержимое бункера 2','Содержимое бункера 2');
INSERT INTO "BFN_SymbCode" VALUES(3027,'Day silo state','Day silo state');
INSERT INTO "BFN_SymbCode" VALUES(3031,'Актуальн. Корм/животн.','Актуальн. Корм/животн.');
INSERT INTO "BFN_SymbCode" VALUES(3033,'Актуальн. прибавка веса','Актуальн. прибавка веса');
INSERT INTO "BFN_SymbCode" VALUES(3041,'Актуальн. соотн. вода/животн.','Актуальн. соотн. вода/животн.');
INSERT INTO "BFN_SymbCode" VALUES(3901,'Текущее положение бок. притока 1','Текущее положение бок. притока 1');
INSERT INTO "BFN_SymbCode" VALUES(3903,'Текущее положение бок. притока 2','Текущее положение бок. притока 2');
INSERT INTO "BFN_SymbCode" VALUES(3905,'Текущее положение бок. притока 3','Текущее положение бок. притока 3');
INSERT INTO "BFN_SymbCode" VALUES(3907,'Текущее положение бок. притока 4','Текущее положение бок. притока 4');
INSERT INTO "BFN_SymbCode" VALUES(3909,'Текущее положение бок. притока 5','Текущее положение бок. притока 5');
INSERT INTO "BFN_SymbCode" VALUES(3911,'Текущее положение бок. притока 6','Текущее положение бок. притока 6');
INSERT INTO "BFN_SymbCode" VALUES(3917,'Текущее положение туннельн. притока 1','Текущее положение туннельн. притока 1');
INSERT INTO "BFN_SymbCode" VALUES(3919,'Текущее положение туннельн. притока 2','Текущее положение туннельн. притока 2');
INSERT INTO "BFN_SymbCode" VALUES(3991,'Упр. пониж. давлением (приток) измерение','Упр. пониж. давлением (приток) измерение');
INSERT INTO "BFN_SymbCode" VALUES(3992,'Упр. пониж. давлением (приток) уставка','Упр. пониж. давлением (приток) уставка');
INSERT INTO "BFN_SymbCode" VALUES(7000,'Выбор языка','Выбор языка');
INSERT INTO "BFN_SymbCode" VALUES(8189,'Датчик для CO2, мин. вентиляция (1 = установлено)','Датчик для CO2, мин. вентиляция (1 = установлено)');
INSERT INTO "BFN_SymbCode" VALUES(8190,'Регулятор СО2 активен (Ja (Да)=1 / Nein (Нет)=0)','Регулятор СО2 активен (Ja (Да)=1 / Nein (Нет)=0)');
INSERT INTO "BFN_SymbCode" VALUES(8191,'Фактическое значение для СО2','Фактическое значение для СО2');
INSERT INTO "BFN_SymbCode" VALUES(8192,'Заданное значение для СО2','Заданное значение для СО2');
INSERT INTO "BFN_SymbCode" VALUES(8193,'Рассчитанная CO2 вентиляция','Рассчитанная CO2 вентиляция');
INSERT INTO "BFN_SymbCode" VALUES(8196,'Нижняя граница тревоги СО2','Нижняя граница тревоги СО2');
INSERT INTO "BFN_SymbCode" VALUES(8197,'Низкое СО2, тревога вкл./выкл.','Низкое СО2, тревога вкл./выкл.');
INSERT INTO "BFN_SymbCode" VALUES(8198,'Верхняя граница тревоги СО2','Верхняя граница тревоги СО2');
INSERT INTO "BFN_SymbCode" VALUES(8199,'Высокое СО2, тревога вкл./выкл.','Высокое СО2, тревога вкл./выкл.');
INSERT INTO "BFN_SymbCode" VALUES(8339,'Темп-ра туннеля','Темп-ра туннеля');
INSERT INTO "BFN_SymbCode" VALUES(8346,'Темп. в зоне высиживания 1','Темп. в зоне высиживания 1');
INSERT INTO "BFN_SymbCode" VALUES(8347,'Темп. в зоне высиживания 2','Темп. в зоне высиживания 2');
INSERT INTO "BFN_SymbCode" VALUES(8348,'Темп. в зоне высиживания 3','Темп. в зоне высиживания 3');
INSERT INTO "BFN_SymbCode" VALUES(8349,'Темп. в зоне высиживания 4','Темп. в зоне высиживания 4');
INSERT INTO "BFN_SymbCode" VALUES(8353,'Кол-во нагревателей','Кол-во нагревателей');
INSERT INTO "BFN_SymbCode" VALUES(8480,'Кол-во групп MultiStep туннель','Кол-во групп MultiStep туннель');
INSERT INTO "BFN_SymbCode" VALUES(8802,'Темп. в зоне высиживания 1','Темп. в зоне высиживания 1');
INSERT INTO "BFN_SymbCode" VALUES(8803,'Темп. в зоне высиживания 2','Темп. в зоне высиживания 2');
INSERT INTO "BFN_SymbCode" VALUES(8804,'Темп. в зоне высиживания 3','Темп. в зоне высиживания 3');
INSERT INTO "BFN_SymbCode" VALUES(8805,'Темп. в зоне высиживания 4','Темп. в зоне высиживания 4');
INSERT INTO "BFN_SymbCode" VALUES(8812,'Потребность в обогреве зоны высиживания 1','Потребность в обогреве зоны высиживания 1');
INSERT INTO "BFN_SymbCode" VALUES(8813,'Потребность в обогреве зоны высиживания 2','Потребность в обогреве зоны высиживания 2');
INSERT INTO "BFN_SymbCode" VALUES(8814,'Потребность в обогреве зоны высиживания 3','Потребность в обогреве зоны высиживания 3');
INSERT INTO "BFN_SymbCode" VALUES(8815,'Потребность в обогреве зоны высиживания 4','Потребность в обогреве зоны высиживания 4');
INSERT INTO "BFN_SymbCode" VALUES(8826,'Нагреватель 1 тип','Нагреватель 1 тип');
INSERT INTO "BFN_SymbCode" VALUES(8827,'Нагреватель 2 тип','Нагреватель 2 тип');
INSERT INTO "BFN_SymbCode" VALUES(8927,'Начать применение темп. границы PAD-охлажд-я','Начать применение темп. границы PAD-охлажд-я');
INSERT INTO "BFN_SymbCode" VALUES(8929,'Начать применение темп. границы PAD-охлажд-я в станд. Режиме','Начать применение темп. границы PAD-охлажд-я в станд. Режиме');
INSERT INTO "BFN_SymbCode" VALUES(8931,'PAD-потребность в охлажд.','PAD-потребность в охлажд.');
INSERT INTO "BFN_SymbCode" VALUES(8939,'PAD-охлаждение не установлено/1 насос/2 насоса','PAD-охлаждение не установлено/1 насос/2 насоса');
INSERT INTO "BFN_SymbCode" VALUES(9005,'Расход корма А, всего','Расход корма А, всего');
INSERT INTO "BFN_SymbCode" VALUES(9006,'Расход корма В, всего','Расход корма В, всего');
INSERT INTO "BFN_SymbCode" VALUES(12920,'Set temperature tunnel','Set temperature tunnel');
CREATE TABLE 'CIFPrm_test' ("SHIFR" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '' ,"TMPL" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' , PRIMARY KEY ("SHIFR"));
INSERT INTO "CIFPrm_test" VALUES('test','','',1,'S7.test','','','','');
CREATE TABLE 'CIFPrm_test_io' ("PRM_ID" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"VALUE" TEXT DEFAULT '' , PRIMARY KEY ("PRM_ID","ID"));
INSERT INTO "CIFPrm_test_io" VALUES('test','f_frq','1');
INSERT INTO "CIFPrm_test_io" VALUES('test','f_start','0');
INSERT INTO "CIFPrm_test_io" VALUES('test','f_stop','0');
INSERT INTO "CIFPrm_test_io" VALUES('test','f_err','0');
INSERT INTO "CIFPrm_test_io" VALUES('test','in','DB1.4');
INSERT INTO "CIFPrm_test_io" VALUES('test','in1','DB1.6');
INSERT INTO "CIFPrm_test_io" VALUES('test','in2','DB1.8');
INSERT INTO "CIFPrm_test_io" VALUES('test','in3','DB1.10');
INSERT INTO "CIFPrm_test_io" VALUES('test','in4','DB1.12');
INSERT INTO "CIFPrm_test_io" VALUES('test','in5','DB1.14');
INSERT INTO "CIFPrm_test_io" VALUES('test','in6','DB1.16');
INSERT INTO "CIFPrm_test_io" VALUES('test','in7','DB1.18');
INSERT INTO "CIFPrm_test_io" VALUES('test','in8','DB1.20');
INSERT INTO "CIFPrm_test_io" VALUES('test','in9','DB1.22');
INSERT INTO "CIFPrm_test_io" VALUES('test','in10','DB1.24');
INSERT INTO "CIFPrm_test_io" VALUES('test','in11','DB1.26');
INSERT INTO "CIFPrm_test_io" VALUES('test','in12','DB1.28');
INSERT INTO "CIFPrm_test_io" VALUES('test','in13','DB1.30');
INSERT INTO "CIFPrm_test_io" VALUES('test','in14','DB1.32');
INSERT INTO "CIFPrm_test_io" VALUES('test','in15','DB1.34');
INSERT INTO "CIFPrm_test_io" VALUES('test','in16','DB1.36');
INSERT INTO "CIFPrm_test_io" VALUES('test','in17','DB1.38');
INSERT INTO "CIFPrm_test_io" VALUES('test','in18','DB1.40');
INSERT INTO "CIFPrm_test_io" VALUES('test','in19','DB1.42');
INSERT INTO "CIFPrm_test_io" VALUES('test','in20','DB1.44');
INSERT INTO "CIFPrm_test_io" VALUES('test','in21','DB1.46');
INSERT INTO "CIFPrm_test_io" VALUES('test','in22','DB1.48');
INSERT INTO "CIFPrm_test_io" VALUES('test','in23','DB1.50');
INSERT INTO "CIFPrm_test_io" VALUES('test','in24','DB1.52');
INSERT INTO "CIFPrm_test_io" VALUES('test','in25','DB1.54');
INSERT INTO "CIFPrm_test_io" VALUES('test','in26','DB1.56');
INSERT INTO "CIFPrm_test_io" VALUES('test','in27','DB1.58');
INSERT INTO "CIFPrm_test_io" VALUES('test','in28','DB1.60');
INSERT INTO "CIFPrm_test_io" VALUES('test','in29','DB1.62');
INSERT INTO "CIFPrm_test_io" VALUES('test','in30','DB1.64');
INSERT INTO "CIFPrm_test_io" VALUES('test','in31','DB1.68');
INSERT INTO "CIFPrm_test_io" VALUES('test','in32','DB1.70');
INSERT INTO "CIFPrm_test_io" VALUES('test','in33','DB1.72');
INSERT INTO "CIFPrm_test_io" VALUES('test','in34','DB1.74');
INSERT INTO "CIFPrm_test_io" VALUES('test','in35','DB1.76');
INSERT INTO "CIFPrm_test_io" VALUES('test','in36','DB1.78');
INSERT INTO "CIFPrm_test_io" VALUES('test','in37','DB1.80');
INSERT INTO "CIFPrm_test_io" VALUES('test','in38','DB1.82');
INSERT INTO "CIFPrm_test_io" VALUES('test','in39','DB1.84');
INSERT INTO "CIFPrm_test_io" VALUES('test','in40','DB1.86');
INSERT INTO "CIFPrm_test_io" VALUES('test','in41','DB1.90');
INSERT INTO "CIFPrm_test_io" VALUES('test','in42','DB1.92');
INSERT INTO "CIFPrm_test_io" VALUES('test','in43','DB1.94');
INSERT INTO "CIFPrm_test_io" VALUES('test','in44','DB1.96');
INSERT INTO "CIFPrm_test_io" VALUES('test','in45','DB1.98');
INSERT INTO "CIFPrm_test_io" VALUES('test','in46','DB1.100');
INSERT INTO "CIFPrm_test_io" VALUES('test','in47','DB1.102');
INSERT INTO "CIFPrm_test_io" VALUES('test','in48','DB1.104');
INSERT INTO "CIFPrm_test_io" VALUES('test','in49','DB1.106');
INSERT INTO "CIFPrm_test_io" VALUES('test','in50','DB1.108');
CREATE TABLE 'CfgExtHosts' ("OP_USER" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"TRANSP" TEXT DEFAULT '' ,"ADDR" TEXT DEFAULT '' ,"USER" TEXT DEFAULT '' ,"PASS" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' , PRIMARY KEY ("OP_USER","ID"));
INSERT INTO "CfgExtHosts" VALUES('roman','loop','Loop','Sockets','TCP:localhost:10005','root','openscada','Loop','Loop');
INSERT INTO "CfgExtHosts" VALUES('*','loop','Loop','Sockets','TCP:localhost:10005','root','openscada','Loop','Loop');
INSERT INTO "CfgExtHosts" VALUES('roman','loopSSL','Loop SSL','SSL','localhost:10045','root','openscada','Loop SSL','Loop SSL');
INSERT INTO "CfgExtHosts" VALUES('*','loopSSL','Loop SSL','SSL','localhost:10045','root','openscada','Loop SSL','Loop SSL');
CREATE TABLE 'DAQ_AMRDevs' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ENABLE" INTEGER DEFAULT '' ,"START" INTEGER DEFAULT '' ,"REDNT" INTEGER DEFAULT '' ,"REDNT_RUN" TEXT DEFAULT '' ,"PRM_BD" TEXT DEFAULT '' ,"SCHEDULE" TEXT DEFAULT '' ,"PRIOR" INTEGER DEFAULT '' ,"BUS_ADDR" TEXT DEFAULT '' ,"TM_REQ" INTEGER DEFAULT '' ,"TM_REST" INTEGER DEFAULT '' ,"REQ_TRY" INTEGER DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO "DAQ_AMRDevs" VALUES('test','','','','',1,0,0,'<high>','AMRDevsPrm_test','1',0,'',0,30,3);
CREATE TABLE 'DAQ_BFN' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"ENABLE" INTEGER DEFAULT '' ,"START" INTEGER DEFAULT '' ,"REDNT" INTEGER DEFAULT '' ,"REDNT_RUN" TEXT DEFAULT '' ,"PRM_BD" TEXT DEFAULT '' ,"SCHEDULE" TEXT DEFAULT '' ,"PRIOR" INTEGER DEFAULT '' ,"SYNCPER" DOUBLE DEFAULT '' ,"ADDR" TEXT DEFAULT '' ,"HOUSE" TEXT DEFAULT '' ,"USER" TEXT DEFAULT '' ,"PASS" TEXT DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO "DAQ_BFN" VALUES('test','','','','',0,0,0,'<high>','TmplPrm_test','* * * * *',0,600.0,'Sockets.BFN','','admin','fowl2008');
CREATE TABLE 'DAQ_DCON' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"ENABLE" INTEGER DEFAULT '' ,"START" INTEGER DEFAULT '' ,"REDNT" INTEGER DEFAULT '' ,"REDNT_RUN" TEXT DEFAULT '' ,"PRM_BD" TEXT DEFAULT '' ,"PERIOD" DOUBLE DEFAULT '' ,"PRIOR" INTEGER DEFAULT '' ,"ADDR" TEXT DEFAULT '' ,"REQ_TRY" INTEGER DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO "DAQ_DCON" VALUES('test','','','','',1,0,0,'<high>','DCONPrm_test',1.0,0,'testModBus',1);
CREATE TABLE 'DAQ_DiamondBoards' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"ENABLE" INTEGER DEFAULT '' ,"START" INTEGER DEFAULT '' ,"BOARD" INTEGER DEFAULT '' ,"PRM_BD_A" TEXT DEFAULT '' ,"PRM_BD_D" TEXT DEFAULT '' ,"DATA_EMUL" INTEGER DEFAULT '' ,"ADDR" INTEGER DEFAULT '' ,"INT" INTEGER DEFAULT '' ,"DIO_CFG" INTEGER DEFAULT '' ,"ADMODE" INTEGER DEFAULT '' ,"ADRANGE" INTEGER DEFAULT '' ,"ADPOLAR" INTEGER DEFAULT '' ,"ADGAIN" INTEGER DEFAULT '' ,"ADCONVRATE" INTEGER DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"REDNT" INTEGER DEFAULT '' ,"REDNT_RUN" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO "DAQ_DiamondBoards" VALUES('test','','',0,0,25,'DiamPrmA_test','DiamPrmD_test',0,640,5,0,1,1,0,1,500,'','',0,'<high>','','');
CREATE TABLE 'DAQ_ICP_DAS' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ENABLE" INTEGER DEFAULT '' ,"START" INTEGER DEFAULT '' ,"REDNT" INTEGER DEFAULT '' ,"REDNT_RUN" TEXT DEFAULT '' ,"PRM_BD" TEXT DEFAULT '' ,"PERIOD" DOUBLE DEFAULT '' ,"PRIOR" INTEGER DEFAULT '' ,"BUS" INTEGER DEFAULT '' ,"BAUD" INTEGER DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO "DAQ_ICP_DAS" VALUES('test','','','','',1,0,0,'<high>','DCONPrm_test',1.0,0,0,2400);
CREATE TABLE 'DAQ_JavaLikeCalc' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"ENABLE" INTEGER DEFAULT '' ,"START" INTEGER DEFAULT '' ,"PRM_BD" TEXT DEFAULT '' ,"FUNC" TEXT DEFAULT '' ,"PRIOR" INTEGER DEFAULT '' ,"ITER" INTEGER DEFAULT '' ,"REDNT" INTEGER DEFAULT '' ,"REDNT_RUN" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"SCHEDULE" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO "DAQ_JavaLikeCalc" VALUES('testCalc','Test calculator','Test calculator',1,0,'testCalc_prm','Controller.test',0,1,0,'<high>','Тестовий обчислювач','Тестовий обчислювач','10','Тестовый вычислитель','Тестовый вычислитель');
CREATE TABLE 'DAQ_LogicLev' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"ENABLE" INTEGER DEFAULT '' ,"START" INTEGER DEFAULT '' ,"PRM_BD" TEXT DEFAULT '' ,"PERIOD" INTEGER DEFAULT '' ,"PRIOR" INTEGER DEFAULT '' ,"REDNT" INTEGER DEFAULT '' ,"REDNT_RUN" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"PRM_BD_REFL" TEXT DEFAULT '' ,"SCHEDULE" TEXT DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO "DAQ_LogicLev" VALUES('experiment','Experimental','It have the group of experimental parameters.',1,1,'LogLevPrm_experiment',0,0,0,'<high>','Експериментальний','Містить групу експериментальних параметрів.','Экспериментальный','Содержит группу экспериментальных параметров.','','1');
CREATE TABLE 'DAQ_SNMP' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"ENABLE" INTEGER DEFAULT '' ,"START" INTEGER DEFAULT '' ,"PRM_BD" TEXT DEFAULT '' ,"PERIOD" INTEGER DEFAULT '' ,"PRIOR" INTEGER DEFAULT '' ,"ADDR" TEXT DEFAULT '' ,"COMM" TEXT DEFAULT '' ,"PATTR_LIM" INTEGER DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"REDNT" INTEGER DEFAULT '' ,"REDNT_RUN" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO "DAQ_SNMP" VALUES('localhost','','',0,0,'SNMPPrm_localhost',10,0,'localhost','public',100,'','',0,'<high>','','');
CREATE TABLE 'DAQ_Siemens' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"ENABLE" INTEGER DEFAULT '' ,"START" INTEGER DEFAULT '' ,"PRM_BD" TEXT DEFAULT '' ,"PERIOD" INTEGER DEFAULT '' ,"PRIOR" INTEGER DEFAULT '' ,"ASINC_WR" INTEGER DEFAULT '' ,"TYPE" INTEGER DEFAULT '' ,"ADDR" TEXT DEFAULT '' ,"SLOT" INTEGER DEFAULT '' ,"CIF_DEV" INTEGER DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"REDNT" INTEGER DEFAULT '' ,"REDNT_RUN" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO "DAQ_Siemens" VALUES('test','','',1,0,'CIFPrm_test',1000,0,0,1,'192.168.2.12',2,0,'','',0,'<high>','','');
CREATE TABLE 'DAQ_SoundCard' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"ENABLE" INTEGER DEFAULT '' ,"START" INTEGER DEFAULT '' ,"PRM_BD" TEXT DEFAULT '' ,"CARD" TEXT DEFAULT '' ,"SMPL_RATE" INTEGER DEFAULT '' ,"SMPL_TYPE" INTEGER DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"REDNT" INTEGER DEFAULT '' ,"REDNT_RUN" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO "DAQ_SoundCard" VALUES('test','','',1,0,'SoundCard_test','NVidia nForce3: NVid',8000,1,'','',0,'<high>','','');
CREATE TABLE 'DAQ_System' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"ENABLE" INTEGER DEFAULT '' ,"START" INTEGER DEFAULT '' ,"AUTO_FILL" INTEGER DEFAULT '' ,"PRM_BD" TEXT DEFAULT '' ,"PERIOD" INTEGER DEFAULT '' ,"PRIOR" INTEGER DEFAULT '' ,"REDNT" INTEGER DEFAULT '' ,"REDNT_RUN" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO "DAQ_System" VALUES('AutoDA','Active data','Active data of operation system.',1,1,1,'AutoDAprm',1000,0,0,'<high>','Активні дані','Активні дані операційної системи.','Активные данные','Активные данные операционной системы.');
CREATE TABLE 'DB' ("ID" TEXT DEFAULT '' ,"TYPE" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"ADDR" TEXT DEFAULT '' ,"CODEPAGE" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' , PRIMARY KEY ("ID","TYPE"));
INSERT INTO "DB" VALUES('LibBD','MySQL','Functions libraries','Many functions libraries.','server.diya.org;roman;123456;oscadaUserLibs;;;utf8','UTF-8',0,'Бібліотеки функцій','Декілька бібліотек функцій.','Библиотеки функций','Содержит множество библиотек функций.');
INSERT INTO "DB" VALUES('GenDB','SQLite','Generic DB','','./AGLKS/St.db','UTF-8',0,'Головна БД','','Generic DB','');
INSERT INTO "DB" VALUES('LibDB','SQLite','Functions libraries','Many functions libraries.','LibsDB/OscadaLibs.db','UTF-8',1,'Бібліотеки функцій','Декілька бібліотек функцій.','Библиотеки функций','Содержит множество библиотек функций.');
INSERT INTO "DB" VALUES('AnastModel','SQLite','AGLKS model','AGLKS model contents::
- Genericstation;
- КМ101;
- КМ102;
- КМ201;
- КМ202;
- КМ301;
- КМ302;','Model.db','UTF-8',1,'Модель АГЛКС','Модель АГЛКС містить:
- Загальностанційка;
- КМ101;
- КМ102;
- КМ201;
- КМ202;
- КМ301;
- КМ302;','Модель АГЛКС','Модель Анастасиеской ГЛКС в составе:
- Общестанционные;
- КМ101;
- КМ102;
- КМ201;
- КМ202;
- КМ301;
- КМ302;');
INSERT INTO "DB" VALUES('arch','FireBird','','','/var/tmp/arch.fbd;roman;123456','UTF-8',0,'','','','');
INSERT INTO "DB" VALUES('testFB','FireBird','','','server.diya.org:/var/tmp/test.fbd;roman;123456','UTF-8',0,'','','','');
INSERT INTO "DB" VALUES('GenDB','MySQL','Generic DB','','server.diya.org;roman;123456;oscadaDemoSt;;;utf8','UTF-8',0,'Головна БД','','Основная БД','');
INSERT INTO "DB" VALUES('arch','MySQL','','','server.diya.org;roman;123456;oscadaArch;;;;5,2,2','',0,'','','','');
INSERT INTO "DB" VALUES('AnastModel','MySQL','AGLKS model','AGLKS model contents::
- Genericstation;
- КМ101;
- КМ102;
- КМ201;
- КМ202;
- КМ301;
- КМ302;','server.diya.org;roman;123456;oscadaAnastas;;;utf8','UTF-8',0,'Модель АГЛКС','Модель АГЛКС містить:
- Загальностанційка;
- КМ101;
- КМ102;
- КМ201;
- КМ202;
- КМ301;
- КМ302;','Модель АГЛКС','Модель Анастасиеской ГЛКС в составе:
- Общестанционные;
- КМ101;
- КМ102;
- КМ201;
- КМ202;
- КМ301;
- КМ302;');
INSERT INTO "DB" VALUES('vcaBase','MySQL','VCA: Main libraries','Contents:
- Library "Main elements"
- Library "Mnemo''s elements"
- Library "Documents"','server.diya.org;roman;123456;oscadaVcaBase;;;utf8','UTF-8',0,'СВК: Головні бібліотеки','Вміст:
- Бібліотека "Головні елементи"
- Бібліотека "Елементи мнемосхеми"
- Бібліотека "Документи"','СВУ: Базовые библиотеки','Содержит:
- Библиотека "Базовые элементы"
- Библиотека "Элементы мнемосхемы"
- Библиотека "Документы"');
INSERT INTO "DB" VALUES('vcaBase','SQLite','VCA: Main libraries','Contents:
- Library "Main elements"
- Library "Mnemo''s elements"
- Library "Documents"','LibsDB/vcaBase.db','UTF-8',1,'СВК: Головні бібліотеки','Вміст:
- Бібліотека "Головні елементи"
- Бібліотека "Елементи мнемосхеми"
- Бібліотека "Документи"','СВУ: Базовые библиотеки','Содержит:
- Библиотека "Базовые элементы"
- Библиотека "Элементы мнемосхемы"
- Библиотека "Документы"');
INSERT INTO "DB" VALUES('exp','SQLite','Export DB','','../exp.db','UTF-8',0,'Експорта БД','','Export DB','');
INSERT INTO "DB" VALUES('vcaTest','MySQL','VCA: Tests','Contain the library of tests frames.','server.diya.org;roman;123456;oscadaVcaTest;;;utf8','UTF-8',0,'СВК: Тести','Містить бібліотеку тестових кадрів.','СВУ: Тесты','Содержит библиотеку тестовых кадров.');
INSERT INTO "DB" VALUES('test','MySQL','','','server.diya.org;roman;123456;test;;;utf8','',0,'','','','');
INSERT INTO "DB" VALUES('vcaTest','SQLite','VCA: Tests','Contain the library of tests frames.','LibsDB/vcaTest.db','UTF-8',1,'СВК: Тести','Містить бібліотеку тестових кадрів.','СВУ: Тесты','Содержит библиотеку тестовых кадров.');
INSERT INTO "DB" VALUES('vcaAGLKS','SQLite','VCA: AGLKS','Contain of the libraries and the project of visualisation area AGLKS.','vca.db','UTF-8',1,'СВК: АГЛКС','Містить бібліотеки та проект середовища візуалізвції АГЛКС.','СВУ: АГЛКС','Содержит библиотеки и проект среды визуализации АГЛКС.');
INSERT INTO "DB" VALUES('vcaAGLKS','MySQL','VCA: AGLKS','The DB contain libraries and project for visualisation environment AGLKS.','server.diya.org;roman;123456;oscadaVcaAGLKS;;;utf8','UTF-8',0,'СВК: АГЛКС','Містить бібліотеки та проект середовища візуалізвції АГЛКС.','VCA: AGLKS','The DB contain libraries and project for visualisation environment AGLKS.');
INSERT INTO "DB" VALUES('test','PostgreSQL','','','maksim.diya.org;;postgres;12345678;vcaTest','UTF-8',0,'','','','');
INSERT INTO "DB" VALUES('QuickStart','MySQL','','','server.diya.org;roman;123456;oscadaQuickStart;;;utf8','UTF-8',0,'','','','');
CREATE TABLE 'DCONPrm_test' ("SHIFR" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '' ,"MOD_ADDR" INTEGER DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"CRC_CTRL" INTEGER DEFAULT '' ,"HOST_SIGNAL" INTEGER DEFAULT '' ,"AI_METHOD" INTEGER DEFAULT '' ,"AI_RANGE" INTEGER DEFAULT '' ,"AO_METHOD" INTEGER DEFAULT '' ,"AO_RANGE" INTEGER DEFAULT '' ,"DI_METHOD" INTEGER DEFAULT '' ,"DO_METHOD" INTEGER DEFAULT '' ,"CI_METHOD" INTEGER DEFAULT '' , PRIMARY KEY ("SHIFR"));
INSERT INTO "DCONPrm_test" VALUES('test','','','','',1,0,'','',1,0,8,0,0,0,0,0,0);
CREATE TABLE 'LogLevPrm_experiment_io' ("PRM_ID" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"VALUE" TEXT DEFAULT '' ,"uk#VALUE" TEXT DEFAULT '' ,"ru#VALUE" TEXT DEFAULT '' , PRIMARY KEY ("PRM_ID","ID"));
INSERT INTO "LogLevPrm_experiment_io" VALUES('F3','in','BlockCalc.Anast1to2node.F3.var','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F3','var','45.2508186009347','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F3','ed','tone/h','т/год','т/ч');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F3','min','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F3','max','100','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F3','aMin','10','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F3','aMax','90','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F3','wMin','35','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F3','wMax','80','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F3','iAdd','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F3','iMult','1','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F3','f_start','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F3','f_frq','1','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F3','f_stop','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F3','f_err','0','0','0');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F4','in','BlockCalc.Anast1to2node.F4.var','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F4','var','52.8916722339593','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F4','ed','tone/h','т/год','т/ч');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F4','min','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F4','max','100','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F4','aMin','10','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F4','aMax','90','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F4','wMin','20','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F4','wMax','80','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F4','iAdd','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F4','iMult','1','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F4','f_start','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F4','f_frq','1','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F4','f_stop','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F4','f_err','0','0','0');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP1','in','BlockCalc.Anast1to2node.T_PP1.var','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP1','var','20.4002015794259','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP1','ed','°C','°С','°С');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP1','min','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP1','max','50','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP1','aMin','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP1','aMax','40','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP1','wMin','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP1','wMax','30','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP1','iAdd','-270','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP1','iMult','1','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP1','f_start','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP1','f_frq','1','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP1','f_stop','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP1','f_err','0','0','0');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP3','in','BlockCalc.Anast1to2node.T_PP3.var','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP3','var','32.179458378928','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP3','ed','°C','°С','°С');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP3','min','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP3','max','100','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP3','aMin','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP3','aMax','50','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP3','wMin','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP3','wMax','40','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP3','iAdd','-273','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP3','iMult','1','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP3','f_start','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP3','f_frq','1','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP3','f_stop','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP3','f_err','0','0','0');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP5','in','BlockCalc.Anast1to2node.T_PP5.var','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP5','var','32.04571885145','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP5','ed','°C','°С','°С');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP5','min','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP5','max','100','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP5','aMin','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP5','aMax','50','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP5','wMin','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP5','wMax','40','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP5','iAdd','-273','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP5','iMult','1','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP5','f_start','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP5','f_frq','1','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP5','f_stop','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP5','f_err','0','0','0');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP1','in','BlockCalc.Anast1to2node.P_PP1.var','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP1','var','5.6411114663383','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP1','ed','at','кгс/см2','кгс/см2');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP1','min','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP1','max','10','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP1','aMin','4','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP1','aMax','8','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP1','wMin','5','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP1','wMax','7','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP1','iAdd','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP1','iMult','1','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP1','f_start','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP1','f_frq','1','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP1','f_stop','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP1','f_err','0','0','0');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP3','in','BlockCalc.Anast1to2node.P_PP3.var','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP3','var','24.2251988252887','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP3','ed','kgH/cm2','кгс/см2','кгс/см2');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP3','min','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP3','max','50','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP3','aMin','10','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP3','aMax','30','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP3','wMin','15','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP3','wMax','25','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP3','iAdd','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP3','iMult','1','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP3','f_start','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP3','f_frq','1','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP3','f_stop','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP3','f_err','0','0','0');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP5','in','BlockCalc.Anast1to2node.P_PP5.var','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP5','var','23.695176183434','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP5','ed','kgH/cm2','кгс/см2','кгс/см2');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP5','min','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP5','max','50','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP5','aMin','10','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP5','aMax','30','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP5','wMin','15','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP5','wMax','25','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP5','iAdd','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP5','iMult','1','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP5','f_start','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP5','f_frq','1','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP5','f_stop','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP5','f_err','0','0','0');
INSERT INTO "LogLevPrm_experiment_io" VALUES('КШ6close','alrm_md','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('КШ6close','alrm_mess','Клапан закрыт!','Клапан закрыт!','Клапан закрыт!');
INSERT INTO "LogLevPrm_experiment_io" VALUES('КШ6close','in','BlockCalc.Anast1to2node.КШ6.st_open','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('КШ6close','f_frq','1','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('КШ6close','f_start','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('КШ6close','f_stop','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('КШ6close','f_err','0','0','0');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P3','in','BlockCalc.Anast1to2node.P3.var','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P3','var','22.8947440612092','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P3','ed','kgH/cm2','кгс/см2','кгс/см2');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P3','min','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P3','max','100','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P3','aMin','10','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P3','aMax','90','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P3','wMin','20','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P3','wMax','80','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P3','iAdd','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P3','iMult','1','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P3','f_start','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P3','f_frq','1','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P3','f_stop','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P3','f_err','0','0','0');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P4','in','BlockCalc.Anast1to2node.P4.var','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P4','var','23.695176183434','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P4','ed','kgH/cm2','кгс/см2','кгс/см2');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P4','min','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P4','max','50','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P4','aMin','30','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P4','aMax','10','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P4','wMin','25','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P4','wMax','15','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P4','iAdd','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P4','iMult','1','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P4','f_start','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P4','f_frq','1','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P4','f_stop','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P4','f_err','0','0','0');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Pi','in','BlockCalc.Anast1to2node.Pi.var','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Pi','var','8.97507910495963','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Pi','ed','kgH/cm2','кгс/см2','кгс/см2');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Pi','min','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Pi','max','20','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Pi','aMin','4','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Pi','aMax','15','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Pi','wMin','5','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Pi','wMax','10','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Pi','iAdd','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Pi','iMult','1','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Pi','f_start','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Pi','f_frq','1','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Pi','f_stop','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Pi','f_err','0','0','0');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Ti','in','BlockCalc.Anast1to2node.Ti.var','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Ti','var','20','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Ti','ed','grad.C','°С','°С');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Ti','min','-50','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Ti','max','50','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Ti','aMin','-20','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Ti','aMax','40','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Ti','wMin','-10','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Ti','wMax','30','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Ti','iAdd','-273','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Ti','iMult','1','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Ti','f_start','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Ti','f_frq','1','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Ti','f_stop','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Ti','f_err','0','0','0');
INSERT INTO "LogLevPrm_experiment_io" VALUES('PT0503','in','BlockCalc.Anast1to2node.PT0503.var','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('PT0503','var','5.79410822690184','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('PT0503','ed','kgH/cm2','кгс/см2','кгс/см2');
INSERT INTO "LogLevPrm_experiment_io" VALUES('PT0503','min','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('PT0503','max','10','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('PT0503','aMin','2','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('PT0503','aMax','8','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('PT0503','wMin','3','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('PT0503','wMax','7','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('PT0503','iAdd','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('PT0503','iMult','1','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('PT0503','f_start','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('PT0503','f_frq','1','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('PT0503','f_stop','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('PT0503','f_err','0','0','0');
INSERT INTO "LogLevPrm_experiment_io" VALUES('КШ7','cmdOpen','BlockCalc.Anast1to2node.КШ7.com','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('КШ7','cmdClose','BlockCalc.Anast1to2node.КШ7','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('КШ7','cmdStop','BlockCalc.Anast1to2node.КШ7','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('КШ7','stOpen','BlockCalc.Anast1to2node.КШ7.st_open','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('КШ7','stClose','BlockCalc.Anast1to2node.КШ7.st_close','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('КШ7','f_frq','1','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('КШ7','f_start','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('КШ7','f_stop','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('КШ7','tCmd','5','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('КШ7','last_cmd','3','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('КШ7','w_tm','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('КШ7','f_err','0','0','0');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP1','in','BlockCalc.Anast1to2node.F_PP1.var','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP1','var','101.605243385313','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP1','ed','tone/h','т/год','т/ч');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP1','min','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP1','max','150','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP1','aMin','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP1','aMax','110','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP1','wMin','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP1','wMax','105','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP1','iAdd','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP1','iMult','1','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP1','f_start','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP1','f_frq','1','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP1','f_stop','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP1','f_err','0','0','0');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP3','in','BlockCalc.Anast1to2node.F_PP3.var','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP3','var','98.652403641346','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP3','ed','tone/h','т/год','т/ч');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP3','min','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP3','max','150','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP3','aMin','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP3','aMax','110','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP3','wMin','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP3','wMax','100','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP3','iAdd','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP3','iMult','1','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP3','f_start','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP3','f_frq','1','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP3','f_stop','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP3','f_err','0','0','0');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP5','in','BlockCalc.Anast1to2node.F_PP5.var','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP5','var','52.8924807374067','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP5','ed','tone/h','т/год','т/ч');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP5','min','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP5','max','100','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP5','aMin','10','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP5','aMax','90','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP5','wMin','35','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP5','wMax','80','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP5','iAdd','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP5','iMult','1','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP5','f_start','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP5','f_frq','1','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP5','f_stop','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP5','f_err','0','0','0');
INSERT INTO "LogLevPrm_experiment_io" VALUES('gN1','f_frq','1','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('gN1','f_start','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('gN1','f_stop','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('gN1','f_err','0','0','0');
INSERT INTO "LogLevPrm_experiment_io" VALUES('gN1','F','200.237139145721','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('gN1','Q','10653.3366971973','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('gN1','P','8.30569012011666','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('gN1','T','14.7051613222366','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('gN1','dP','6.06779209532489','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('gN1','DS','1.05816672507588','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F3','scSqr','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F3','HystBnd','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F3','speed','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F3','prec','2','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F3','Tf','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F3','dmsk','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F3','plcMin','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F3','plcMax','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F3','plcExcess','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F3','plcImit','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F3','plcImitIn','','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F3','this','<TCntrNodeObj path="/sub_DAQ/mod_LogicLev/cntr_experiment/prm_F3/"/>','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F3','SHIFR','F3','F3','F3');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F3','NAME','F3','F3','F3');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F3','DESCR','The gas flow through a pipe to Glinsk','Витрати газу через трубу на Глінск','Расход газа через трубу на Глинск');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F4','scSqr','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F4','HystBnd','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F4','speed','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F4','prec','2','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F4','Tf','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F4','dmsk','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F4','plcMin','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F4','plcMax','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F4','plcExcess','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F4','plcImit','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F4','plcImitIn','','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F4','this','<TCntrNodeObj path="/sub_DAQ/mod_LogicLev/cntr_experiment/prm_F4/"/>','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F4','SHIFR','F4','F4','F4');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F4','NAME','F4','F4','F4');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F4','DESCR','Gas flow through the diaphragm PP5','Витрати газу через діафрагму PP5','Расход газа через диафрагму PP5');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP1','scSqr','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP1','HystBnd','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP1','speed','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP1','prec','2','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP1','Tf','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP1','dmsk','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP1','plcMin','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP1','plcMax','150','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP1','plcExcess','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP1','plcImit','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP1','plcImitIn','','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP1','this','<TCntrNodeObj path="/sub_DAQ/mod_LogicLev/cntr_experiment/prm_F_PP1/"/>','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP1','SHIFR','F_PP1','F_PP1','F_PP1');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP1','NAME','F_PP1','F_PP1','F PP1');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP1','DESCR','Gas flow through the diaphragm PP1','Витрати газу через діафрагму PP1','Расход газа через диафрагму PP1');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP3','scSqr','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP3','HystBnd','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP3','speed','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP3','prec','2','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP3','Tf','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP3','dmsk','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP3','plcMin','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP3','plcMax','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP3','plcExcess','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP3','plcImit','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP3','plcImitIn','','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP3','this','<TCntrNodeObj path="/sub_DAQ/mod_LogicLev/cntr_experiment/prm_F_PP3/"/>','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP3','SHIFR','F_PP3','F_PP3','F_PP3');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP3','NAME','F_PP3','F PP3','F PP3');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP3','DESCR','Gas flow through the diaphragm PP3','Витрати газу через діафрагму PP3','Расход газа через диафрагму PP3');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP5','scSqr','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP5','HystBnd','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP5','speed','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP5','prec','2','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP5','Tf','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP5','dmsk','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP5','plcMin','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP5','plcMax','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP5','plcExcess','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP5','plcImit','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP5','plcImitIn','','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP5','this','<TCntrNodeObj path="/sub_DAQ/mod_LogicLev/cntr_experiment/prm_F_PP5/"/>','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP5','SHIFR','F_PP5','F_PP5','F_PP5');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP5','NAME','F_PP5','F_PP5','F PP5');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP5','DESCR','Gas flow through the diaphragm PP5','Витрати газу через діафрагму PP5',' ');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P3','scSqr','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P3','HystBnd','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P3','speed','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P3','prec','2','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P3','Tf','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P3','dmsk','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P3','plcMin','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P3','plcMax','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P3','plcExcess','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P3','plcImit','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P3','plcImitIn','','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P3','this','<TCntrNodeObj path="/sub_DAQ/mod_LogicLev/cntr_experiment/prm_P3/"/>','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P3','SHIFR','P3','P3','P3');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P3','NAME','P3','P3','P3');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P3','DESCR','The gas pressure in the pipe to Glinsk','Тиск газу у трубі на Глінск','Давление газа в трубе на Глинск');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P4','scSqr','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P4','HystBnd','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P4','speed','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P4','prec','2','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P4','Tf','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P4','dmsk','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P4','plcMin','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P4','plcMax','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P4','plcExcess','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P4','plcImit','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P4','plcImitIn','','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P4','this','<TCntrNodeObj path="/sub_DAQ/mod_LogicLev/cntr_experiment/prm_P4/"/>','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P4','SHIFR','P4','P4','P4');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P4','NAME','P4','P4','P4');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P4','DESCR','Pressure P4','Тиск газу на діафрагмі PP5','Давление газа на диафрагме PP5');
INSERT INTO "LogLevPrm_experiment_io" VALUES('PT0503','scSqr','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('PT0503','HystBnd','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('PT0503','speed','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('PT0503','prec','2','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('PT0503','Tf','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('PT0503','dmsk','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('PT0503','plcMin','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('PT0503','plcMax','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('PT0503','plcExcess','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('PT0503','plcImit','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('PT0503','plcImitIn','','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('PT0503','this','<TCntrNodeObj path="/sub_DAQ/mod_LogicLev/cntr_experiment/prm_PT0503/"/>','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('PT0503','SHIFR','PT0503','PT0503','PT0503');
INSERT INTO "LogLevPrm_experiment_io" VALUES('PT0503','NAME','PT0503','PT0503','PT0503');
INSERT INTO "LogLevPrm_experiment_io" VALUES('PT0503','DESCR','The gas pressure in the separator С1','Тиск газу у сепараторі С1',' ');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP1','scSqr','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP1','HystBnd','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP1','speed','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP1','prec','2','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP1','Tf','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP1','dmsk','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP1','plcMin','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP1','plcMax','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP1','plcExcess','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP1','plcImit','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP1','plcImitIn','','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP1','this','<TCntrNodeObj path="/sub_DAQ/mod_LogicLev/cntr_experiment/prm_P_PP1/"/>','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP1','SHIFR','P_PP1','P_PP1','P_PP1');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP1','NAME','P_PP1','P_PP1','P PP1');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP1','DESCR','Gas pressure at the diaphragm PP1','Тиск газу на діафрагмі PP1','Давление газа на диафрагме PP1');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP3','scSqr','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP3','HystBnd','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP3','speed','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP3','prec','2','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP3','Tf','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP3','dmsk','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP3','plcMin','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP3','plcMax','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP3','plcExcess','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP3','plcImit','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP3','plcImitIn','','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP3','this','<TCntrNodeObj path="/sub_DAQ/mod_LogicLev/cntr_experiment/prm_P_PP3/"/>','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP3','SHIFR','P_PP3','P_PP3','P_PP3');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP3','NAME','P_PP3','P_PP3','P PP3');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP3','DESCR','Gas pressure at the diaphragm PP3','Тиск газу на діафрагмі PP3','Давление газа на диафрагме PP3');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP5','scSqr','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP5','HystBnd','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP5','speed','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP5','prec','2','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP5','Tf','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP5','dmsk','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP5','plcMin','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP5','plcMax','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP5','plcExcess','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP5','plcImit','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP5','plcImitIn','','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP5','this','<TCntrNodeObj path="/sub_DAQ/mod_LogicLev/cntr_experiment/prm_P_PP5/"/>','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP5','SHIFR','P_PP5','P_PP5','P_PP5');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP5','NAME','P_PP5','P_PP5','P PP5');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP5','DESCR','Gas pressure at the diaphragm PP5','Тиск газу на діафрагмі PP5','Давление газа на диафрагме PP5 ');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Pi','scSqr','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Pi','HystBnd','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Pi','speed','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Pi','prec','2','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Pi','Tf','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Pi','dmsk','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Pi','plcMin','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Pi','plcMax','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Pi','plcExcess','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Pi','plcImit','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Pi','plcImitIn','','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Pi','this','<TCntrNodeObj path="/sub_DAQ/mod_LogicLev/cntr_experiment/prm_Pi/"/>','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Pi','SHIFR','Pi','Pi','Pi');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Pi','NAME','Pi','Pi','Pi');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Pi','DESCR','The gas pressure at the input of the CS.','Тиск газу на вході КС.','Давление газа на входе КС.');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F3','inProc','','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F3','subMode','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F3','subVar','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F3','alSup','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F4','inProc','','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F4','subMode','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F4','subVar','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F4','alSup','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP1','inProc','','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP1','subMode','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP1','subVar','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP1','alSup','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP3','inProc','','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP3','subMode','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP3','subVar','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP3','alSup','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP5','inProc','','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP5','subMode','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP5','subVar','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('F_PP5','alSup','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P3','inProc','','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P3','subMode','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P3','subVar','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P3','alSup','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P4','inProc','','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P4','subMode','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P4','subVar','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P4','alSup','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('PT0503','inProc','','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('PT0503','subMode','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('PT0503','subVar','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('PT0503','alSup','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP1','inProc','','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP1','subMode','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP1','subVar','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP1','alSup','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP3','inProc','','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP3','subMode','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP3','subVar','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP3','alSup','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP5','inProc','','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP5','subMode','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP5','subVar','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('P_PP5','alSup','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Pi','inProc','','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Pi','subMode','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Pi','subVar','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Pi','alSup','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP1','inProc','','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP1','scSqr','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP1','subMode','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP1','subVar','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP1','alSup','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP1','HystBnd','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP1','speed','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP1','prec','2','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP1','Tf','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP1','plcMin','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP1','plcMax','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP1','plcExcess','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP1','plcImit','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP1','plcImitIn','','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP1','this','<TCntrNodeObj path="/sub_DAQ/mod_LogicLev/cntr_experiment/prm_T_PP1/"/>','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP1','SHIFR','T_PP1','T_PP1','T_PP1');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP1','NAME','T_PP1','T_PP1','T PP1');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP1','DESCR','The gas temperature at the diaphragm PP1','Температура газу на діафрагмі PP1','Температура газа на диафрагме PP1');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP5','inProc','','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP5','scSqr','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP5','subMode','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP5','subVar','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP5','alSup','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP5','HystBnd','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP5','speed','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP5','prec','2','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP5','Tf','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP5','plcMin','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP5','plcMax','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP5','plcExcess','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP5','plcImit','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP5','plcImitIn','','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP5','this','<TCntrNodeObj path="/sub_DAQ/mod_LogicLev/cntr_experiment/prm_T_PP5/"/>','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP5','SHIFR','T_PP5','T_PP5','T_PP5');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP5','NAME','T_PP5','T_PP5','T PP5');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP5','DESCR','The gas temperature at the diaphragm PP5','Температура газу на діафрагмі PP5','Температура газа на диафрагме PP5');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Ti','inProc','','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Ti','scSqr','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Ti','subMode','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Ti','subVar','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Ti','alSup','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Ti','HystBnd','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Ti','speed','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Ti','prec','2','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Ti','Tf','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Ti','plcMin','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Ti','plcMax','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Ti','plcExcess','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Ti','plcImit','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Ti','plcImitIn','','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Ti','this','<TCntrNodeObj path="/sub_DAQ/mod_LogicLev/cntr_experiment/prm_Ti/"/>','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Ti','SHIFR','Ti','Ti','Ti');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Ti','NAME','Ti','Ti','Ti');
INSERT INTO "LogLevPrm_experiment_io" VALUES('Ti','DESCR','The gas temperature at the input of the CS.','Температура газу на вході КС.','Температура газа на входе КС.');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP3','inProc','','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP3','scSqr','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP3','subMode','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP3','subVar','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP3','alSup','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP3','HystBnd','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP3','speed','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP3','prec','2','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP3','Tf','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP3','plcMin','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP3','plcMax','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP3','plcExcess','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP3','plcImit','0','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP3','plcImitIn','','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP3','this','<TCntrNodeObj path="/sub_DAQ/mod_LogicLev/cntr_experiment/prm_T_PP3/"/>','','');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP3','SHIFR','T_PP3','T_PP3','T_PP3');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP3','NAME','T_PP3','T_PP3','T PP3');
INSERT INTO "LogLevPrm_experiment_io" VALUES('T_PP3','DESCR','The gas temperature at the diaphragm PP3','Температура газу на діафрагмі PP3','Температура газа на диафрагме PP3');
CREATE TABLE 'ModBus_node' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '' ,"ADDR" INTEGER DEFAULT '' ,"InTR" TEXT DEFAULT '' ,"PRT" TEXT DEFAULT '' ,"MODE" INTEGER DEFAULT '' ,"DT_PER" DOUBLE DEFAULT '' ,"TO_TR" TEXT DEFAULT '' ,"TO_PRT" TEXT DEFAULT '' ,"TO_ADDR" INTEGER DEFAULT '' ,"DT_PROG" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"uk#DT_PROG" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"ru#DT_PROG" TEXT DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO "ModBus_node" VALUES('test','Test',' ',1,1,'*','*',0,1.0,'Serial.exlar','RTU',10,'JavaLikeCalc.JavaScript
//Test
R4+=1;
C4 = !C4;

//AT101_1 processing
R100 = 65536*(AT101_1_Ti-273)/150;
R101 = 65536*(AT101_1_To-273)/100;
//AT101_1 processing
R103 = 65536*(AT101_2_Ti-273)/150;
R104 = 65536*(AT101_2_To-273)/100;','Тест',' ','JavaLikeCalc.JavaScript
//Test
R4+=1;
C4 = !C4;

//AT101_1 processing
R100 = 65536*(AT101_1_Ti-273)/150;
R101 = 65536*(AT101_1_To-273)/100;
//AT101_1 processing
R103 = 65536*(AT101_2_Ti-273)/150;
R104 = 65536*(AT101_2_To-273)/100;','Тест',' ','JavaLikeCalc.JavaScript
//Test
R4+=1;
C4 = !C4;

//AT101_1 processing
R100 = 65536*(AT101_1_Ti-273)/150;
R101 = 65536*(AT101_1_To-273)/100;
//AT101_1 processing
R103 = 65536*(AT101_2_Ti-273)/150;
R104 = 65536*(AT101_2_To-273)/100;');
INSERT INTO "ModBus_node" VALUES('gate','','',0,2,'Sockets.testModBus','*',1,1.0,'Serial.exlar','RTU',1,'','','','','','','');
CREATE TABLE 'ModBus_node_io' ("NODE_ID" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"TYPE" INTEGER DEFAULT '' ,"FLAGS" INTEGER DEFAULT '' ,"VALUE" TEXT DEFAULT '' ,"POS" INTEGER DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#VALUE" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#VALUE" TEXT DEFAULT '' , PRIMARY KEY ("NODE_ID","ID"));
INSERT INTO "ModBus_node_io" VALUES('test','R0','Register 0',1,0,'1000',0,'Регістр 0','1000','Регистр 0','1000');
INSERT INTO "ModBus_node_io" VALUES('test','R3w','Register 3 (write)',1,0,'10',2,'Регістр 3 (запис)','10','Регистр 3 (запись)','10');
INSERT INTO "ModBus_node_io" VALUES('test','R1','Register 1',1,16,'LogicLev.experiment.F3.var',1,'Регістр 1','LogicLev.experiment.F3.var','Регистр 1','LogicLev.experiment.F3.var');
INSERT INTO "ModBus_node_io" VALUES('test','C3','Bit 3',3,0,'1',9,'Біт 3','1','Бит 3','1');
INSERT INTO "ModBus_node_io" VALUES('test','C6','Bit 6',3,16,'BlockCalc.Anast1to2node.КШ1.st_open',11,'Біт 6','BlockCalc.Anast1to2node.КШ1.st_open','Бит 6','BlockCalc.Anast1to2node.КШ1.st_open');
INSERT INTO "ModBus_node_io" VALUES('test','R4','Register 4',1,0,'75360',3,'Регістр 4','30678','Регистр 4','43186');
INSERT INTO "ModBus_node_io" VALUES('test','C4','Bit 4',3,0,'0',10,'Біт 4','0','Бит 4','1');
INSERT INTO "ModBus_node_io" VALUES('test','C8w','Bit 8',3,0,'0',12,'Біт 8','0','Бит 8','0');
INSERT INTO "ModBus_node_io" VALUES('test','AT101_1_Ti','AT101_1.Ti',2,16,'BlockCalc.KM101.AT101_1.Ti',13,'AT101_1.Ti','BlockCalc.KM101.AT101_1.Ti','AT101_1.Ti','BlockCalc.KM101.AT101_1.Ti');
INSERT INTO "ModBus_node_io" VALUES('test','AT101_1_To','AT101_1.To',2,16,'BlockCalc.KM101.AT101_1.To',14,'AT101_1.To','BlockCalc.KM101.AT101_1.To','AT101_1.To','BlockCalc.KM101.AT101_1.To');
INSERT INTO "ModBus_node_io" VALUES('test','AT101_2_Ti','AT101_2.Ti',2,16,'BlockCalc.KM101.AT101_2.Ti',15,'AT101_2.Ti','BlockCalc.KM101.AT101_2.Ti','AT101_2.Ti','BlockCalc.KM101.AT101_2.Ti');
INSERT INTO "ModBus_node_io" VALUES('test','AT101_2_To','AT101_2.To',2,16,'BlockCalc.KM101.AT101_2.To',16,'AT101_2.To','BlockCalc.KM101.AT101_2.To','AT101_2.To','BlockCalc.KM101.AT101_2.To');
INSERT INTO "ModBus_node_io" VALUES('test','R100','AT101_1.Ti code',1,0,'25332',17,'AT101_1.Ti code','25400','AT101_1.Ti code','25395');
INSERT INTO "ModBus_node_io" VALUES('test','R101','AT101_1.To code',1,0,'16553',18,'AT101_1.To code','16567','AT101_1.To code','16572');
INSERT INTO "ModBus_node_io" VALUES('test','R102w','AT101_1.Wc',1,16,'BlockCalc.KM101.AT101_1.Wc',19,'AT101_1.Wc','BlockCalc.KM101.AT101_1.Wc','AT101_1.Wc','BlockCalc.KM101.AT101_1.Wc');
INSERT INTO "ModBus_node_io" VALUES('test','R103','AT101_2.Ti code',1,0,'54795',20,'AT101_2.Ti code','54886','AT101_2.Ti code','54884');
INSERT INTO "ModBus_node_io" VALUES('test','R104','AT101_2.To code',1,0,'21680',21,'AT101_2.To code','21687','AT101_2.To code','21695');
INSERT INTO "ModBus_node_io" VALUES('test','R105w','AT101_2.Wc',1,16,'BlockCalc.KM101.AT101_2.Wc',22,'AT101_2.Wc','BlockCalc.KM101.AT101_2.Wc','AT101_2.Wc','BlockCalc.KM101.AT101_2.Wc');
INSERT INTO "ModBus_node_io" VALUES('test','C100w','КШ2.com',3,16,'BlockCalc.Anast1to2node.КШ2.com',23,'КШ2.com','BlockCalc.Anast1to2node.КШ2.com','КШ102.com','BlockCalc.KM101.КШ102.com');
INSERT INTO "ModBus_node_io" VALUES('test','C101','КШ2.st_open',3,16,'BlockCalc.Anast1to2node.КШ2.st_open',24,'КШ2.st_open','BlockCalc.Anast1to2node.КШ2.st_open','КШ102.st_open','BlockCalc.KM101.КШ102.st_open');
INSERT INTO "ModBus_node_io" VALUES('test','C102','КШ2.st_close',3,16,'BlockCalc.Anast1to2node.КШ2.st_close',25,'КШ2.st_close','BlockCalc.Anast1to2node.КШ2.st_close','КШ102.st_close','BlockCalc.KM101.КШ102.st_close');
INSERT INTO "ModBus_node_io" VALUES('test','R_i6w','Register 6,7',1,0,'-5',4,'Регістр 6,7','-4','Регистр 6,7','-5');
INSERT INTO "ModBus_node_io" VALUES('test','R_f8w','Register 8,9',2,0,'3.14159265',5,'Регістр 8,9','3.14159265','Регистр 8,9','3.14159265');
INSERT INTO "ModBus_node_io" VALUES('test','R_i8:10:w','Register 10,13',1,0,'123456789',6,'Регістр 10,13','21474836470','Регистр 10,13','123456789');
INSERT INTO "ModBus_node_io" VALUES('test','R_d:14:w','Register 14-17',2,0,'3.14159265',7,'Регістр 14-17','3.14159265','Регистр 14-17','3.14159265');
INSERT INTO "ModBus_node_io" VALUES('test','R_d18w','Register 18-21',2,0,'2.7',8,'Регістр 18-21','2.7','Регистр 18-21','2.7');
CREATE TABLE 'ModBus_sdevs' ("ID" TEXT DEFAULT '' ,"SPEED" INTEGER DEFAULT '' ,"LEN" INTEGER DEFAULT '' ,"TWOSTOP" INTEGER DEFAULT '' ,"PARITY" INTEGER DEFAULT '' ,"TM_FRM" INTEGER DEFAULT '' ,"TM_CHAR" DOUBLE DEFAULT '' ,"TM_REQ" INTEGER DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO "ModBus_sdevs" VALUES('/dev/ttyS0',19200,8,1,0,320,1.88,640);
CREATE TABLE 'OPC_UA_ep' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '' ,"SerialzType" INTEGER DEFAULT '' ,"URL" TEXT DEFAULT '' ,"ServCert" TEXT DEFAULT '' ,"ServPvKey" TEXT DEFAULT '' ,"SecPolicies" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO "OPC_UA_ep" VALUES('test','','','','',1,0,'opc.tcp://127.0.0.1:4841','-----BEGIN CERTIFICATE-----
MIIEhDCCA2ygAwIBAgIJALTtgLlmdsp6MA0GCSqGSIb3DQEBBQUAMIGhMQswCQYD
VQQGEwJVQTEXMBUGA1UECAwORG5lcHJvcGV0cm92c2sxGTAXBgNVBAcMEERuZXBy
b2R6ZXJ6aGluc2sxFzAVBgNVBAoMDk9wZW5TQ0FEQSBUZWFtMQswCQYDVQQLDAJS
TTEWMBQGA1UEAwwNT1BDIFVBIFNlcnZlcjEgMB4GCSqGSIb3DQEJARYRb3NjYWRh
QG9zY2FkYS5vcmcwHhcNMTMxMjIxMTkwNTQ4WhcNMjMxMjE5MTkwNTQ4WjCBoTEL
MAkGA1UEBhMCVUExFzAVBgNVBAgMDkRuZXByb3BldHJvdnNrMRkwFwYDVQQHDBBE
bmVwcm9kemVyemhpbnNrMRcwFQYDVQQKDA5PcGVuU0NBREEgVGVhbTELMAkGA1UE
CwwCUk0xFjAUBgNVBAMMDU9QQyBVQSBTZXJ2ZXIxIDAeBgkqhkiG9w0BCQEWEW9z
Y2FkYUBvc2NhZGEub3JnMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA
xDaaKoM7i+lXuhE8FUOBkWJePybmfQBHWzN9nSphEMKpdZcyAAWO7K+tBzPI+bjK
n6lvXiWXVa+9wADZy7/c4bbJuDo4RBU4lQFpmxhtifOPcxWXDyzs2xWccwCL04c0
IZJVqcK1Q2Y/vN6dyR70UuqMkowYFKe1LFtf1xNzMVvVm/WRC57Gn4vLcw9Ngxd7
bPe0J2Tfi+B4WO8EfOTJLJF90sKHBkSODX36ISQnQLxggRgIrg78Foy6DNhxwRa1
7NmYebQmjoWjHmQRBW+C9LYFweyOUkUhIrtDhr5xIEN69IQqRgBiPCgJmcH9hdoo
RprrS8o/OjwnwBYndRkuYwIDAQABo4G8MIG5MB0GA1UdDgQWBBSEGzl4/Fv8hLTr
U92PoHtJxs57eTAfBgNVHSMEGDAWgBSEGzl4/Fv8hLTrU92PoHtJxs57eTAMBgNV
HRMEBTADAQH/MA4GA1UdDwEB/wQEAwIC9DAgBgNVHSUBAf8EFjAUBggrBgEFBQcD
AQYIKwYBBQUHAwIwNwYDVR0RBDAwLoYacm9tYW4vT3BlblNDQURBL0RBUS5PUENf
VUGCCm9zY2FkYS5vcmeHBFLPWEkwDQYJKoZIhvcNAQEFBQADggEBACVxY7ok5cfV
CndAUCAx0zFr/Atb1jQZRXjQjYFhCvT0Zws3vcPdcdLNIMKAzKP0ylIcj1EaiN8z
6I2NK/HV0MzfKtGDRZRS6n7zdz5dJePhRgwYgmJWKlfKA5L2gcXgZKBLbKiLa50o
lLSUMY2tij3AP5C5iWk8wSzH34p5cGWMMjtm0S6b85T5sxF4GbsOdQi4a6puNyCo
m+C6Xu4RkQqzqSvLfCOO0WqwOSskZ0/LiNy38Ple/sSKi6fTfVmMbY4WzmrufWs1
w536xFqBAtG2w8plhyh8jIYGGXiKeCA5Jdt+c2rtzNeRnKB0xIwodGA8QhlSpzmu
QEsBr3HUqMI=
-----END CERTIFICATE-----','-----BEGIN RSA PRIVATE KEY-----
MIIEpAIBAAKCAQEAxDaaKoM7i+lXuhE8FUOBkWJePybmfQBHWzN9nSphEMKpdZcy
AAWO7K+tBzPI+bjKn6lvXiWXVa+9wADZy7/c4bbJuDo4RBU4lQFpmxhtifOPcxWX
Dyzs2xWccwCL04c0IZJVqcK1Q2Y/vN6dyR70UuqMkowYFKe1LFtf1xNzMVvVm/WR
C57Gn4vLcw9Ngxd7bPe0J2Tfi+B4WO8EfOTJLJF90sKHBkSODX36ISQnQLxggRgI
rg78Foy6DNhxwRa17NmYebQmjoWjHmQRBW+C9LYFweyOUkUhIrtDhr5xIEN69IQq
RgBiPCgJmcH9hdooRprrS8o/OjwnwBYndRkuYwIDAQABAoIBAFPS9929nszQ7TeC
cWasJkhT++uC1OkGG3Xs83cXScx9PSQNacD0nvCixIrVJsD4nyfCnN9ds99ZavkB
5AnVd6m+C0N3K956KmgN6G3n67QJp3kE+jnzeWTHQMUosJO3qR/Ggm45znJy1jv7
5vCrFrntlgxtaUQ5uX6vsqwRlR66WrcElEqaRNkpdw6pv+sLqHLS6gQHSw4Kmqjj
DO91eP1pEPcFNQs7LX8NZQ3if0QO5Rqhmkj+FpiaJHe/YS9SSAvRcVek8IfD72PD
7v+PlerM6/wWVaVxiFlA/rX7ndW982+iTt7EfBCVE2M/U4AIU8vKfEDFZK7vG/D4
mfQudcECgYEA9JbUPm+zhkAlaNg/F9A1fESNH2xJLy8Ulw5pWVXPOe+yDpPDSglK
eY7sStlWLSt13HTGEHCpL0+I5APhCooItGy3+foD4zGh5x+CQbfEiqG7Ej+O7jWT
Gs90QootnLjOISqD/ZO3CqHqYRNMdk4caHrGmLp1xWgB9pCRzXAlBzMCgYEAzV4C
9ErGOaqhP8RMl78aNzQEISFIuE4dc7hNQTYYGOml5nCpukAR5sGn8GOM0/fG6qky
tXM/4FtC1+a8CPUUfDlKxcmjf0hwwNgstNjA61GL1QP7MFgGqLBF2iLRtrY9p+7s
rQ+9M36B3oFsc3SQvxT5kqJJCH5YveKdeDQ9fBECgYEAmg5yiw0SV4oJxhsGynoD
Zk1LUjKDeiu+XVLfejx5NPXMAjCB/h0SArTKEm8IP5UzPHN02aAMgFcpUu06jsfY
qcQcsWAPmMeHQatwAPK94c/yo5DS9ylZibaHdO3RwvgIJY3A6zRiQ1ku5owgShfy
/r/uXP589aUIL4RpwatUN08CgYEAvFIGQYBED+NhJHlchxaN8qs3Ui4UYjX3mgMa
jL8PD+FJ6WvMqGdSQxm4Z57SKBohhVQMlQmPPKxTOmpyaYBb/aIH6CGEVRbXlm9l
Z50QI3nO0VJqSCWgTZX/KA4f4v1wwos862HwbMUIUEK8cY8loKOaTJWWhTxfIhkp
r22wKBECgYA8kgOS7C/V6jDuOlUElOcMfG4j2hGD1osJxXqw0RyWxol610cSVYKl
8ZiFXtZufutO+SdGGCQyTPghpnTrhL8Ew4MdDWsUKxi9ebwK1ClTobxX9x+ZmrBU
eD0fnYnJVm0HzoQBMtgjdlu4QJ63rKJdoD8e3IwwnwlVrKb2rAeUGQ==
-----END RSA PRIVATE KEY-----','None:1
Basic128Rsa15:3
Basic256:3
','','');
CREATE TABLE 'SNMPPrm_localhost' ("SHIFR" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '' ,"OID_LS" TEXT DEFAULT '' , PRIMARY KEY ("SHIFR"));
INSERT INTO "SNMPPrm_localhost" VALUES('test','','','','',1,'system');
CREATE TABLE 'SYS' ("user" TEXT DEFAULT '' ,"id" TEXT DEFAULT '' ,"val" TEXT DEFAULT '' ,"uk#val" TEXT DEFAULT '' ,"ru#val" TEXT DEFAULT '' , PRIMARY KEY ("user","id"));
INSERT INTO "SYS" VALUES('root','/sub_UI/mod_QTCfg/StartPath','/','','');
INSERT INTO "SYS" VALUES('root','/sub_UI/mod_QTCfg/StartUser','roman','','');
INSERT INTO "SYS" VALUES('root','/sub_UI/mod_Vision/StartUser','root','','');
INSERT INTO "SYS" VALUES('root','/sub_UI/mod_Vision/RunPrjs','','','');
INSERT INTO "SYS" VALUES('root','/sub_UI/mod_Vision/VCAstation','.','','');
INSERT INTO "SYS" VALUES('root','/sub_UI/mod_Vision/PlayCom','play -q %f','','');
INSERT INTO "SYS" VALUES('roman','/sub_Transport/mod_Sockets/ReqMode','0','','');
INSERT INTO "SYS" VALUES('roman','/sub_DAQ/mod_LogicLev/onlAttr','0','','');
INSERT INTO "SYS" VALUES('roman','/sub_Archive/messArch','','','');
INSERT INTO "SYS" VALUES('roman','/sub_Archive/vShowTrnd','1','','');
INSERT INTO "SYS" VALUES('roman','/sub_Archive/vaSize','600','','');
INSERT INTO "SYS" VALUES('roman','/sub_Archive/vPctW','600','','');
INSERT INTO "SYS" VALUES('roman','/sub_Archive/mod_FSArch/mess_test/messSize','600','','');
INSERT INTO "SYS" VALUES('roman','/sub_Special/mod_FLibSYS/fnc_tmFStr/ntCalc','1','','');
INSERT INTO "SYS" VALUES('roman','/sub_UI/mod_VCAEngine/ses_AGLKS/fs_addr/ntCalc','1','','');
INSERT INTO "SYS" VALUES('roman','/sub_UI/mod_VCAEngine/vcaWdgList/ntCalc','1','','');
INSERT INTO "SYS" VALUES('roman','/sub_UI/mod_VCAEngine/vcaAttrList/ntCalc','1','','');
INSERT INTO "SYS" VALUES('roman','/sub_Archive/mod_DBArch/mess_test/messSize','600','','');
INSERT INTO "SYS" VALUES('roman','/sub_Special/mod_FLibSYS/fnc_strParsePath/ntCalc','1','','');
INSERT INTO "SYS" VALUES('roman','/sub_Special/mod_FLibSYS/fnc_sysCall/ntCalc','1','','');
INSERT INTO "SYS" VALUES('root','/sub_UI/mod_VCAEngine/SynthCom','echo "%t" | ru_tts -p 0.05 -r 0.4 -s /usr/share/ru_tts/lexicon | sox -t raw -s -b 8 -r 10k -c 1 -v 0.8 - -t ogg -','','');
INSERT INTO "SYS" VALUES('root','/sub_UI/mod_VCAEngine/SynthCode','KOI8-U','','');
INSERT INTO "SYS" VALUES('roman','/sub_UI/mod_VCAEngine/showAttr','1','','');
INSERT INTO "SYS" VALUES('roman','/sub_Archive/vaTm','0','','');
INSERT INTO "SYS" VALUES('roman','/sub_DAQ/mod_JavaLikeCalc/lib_doc/fnc_getVal/ntCalc','1','','');
INSERT INTO "SYS" VALUES('root','/sub_UI/mod_QTStarter/StartMod','QTCfg','','');
INSERT INTO "SYS" VALUES('roman','/sub_Transport/mod_SSL/ReqMode','1','','');
INSERT INTO "SYS" VALUES('roman','/sub_Transport/mod_SSL/ReqReq','15 01 00 00 00 06 01 01 00 03 00 06 ','','');
INSERT INTO "SYS" VALUES('roman','/sub_Transport/mod_SSL/ReqTm','0.477','','');
INSERT INTO "SYS" VALUES('roman','/sub_Transport/mod_Serial/ReqReq','ATZ\r
','','');
INSERT INTO "SYS" VALUES('roman','/sub_Transport/mod_Serial/ToTmOut','1','','');
INSERT INTO "SYS" VALUES('roman','/sub_Transport/mod_Serial/ReqMode','0','','');
INSERT INTO "SYS" VALUES('roman','/sub_Transport/mod_Serial/ReqTm','7.521','','');
INSERT INTO "SYS" VALUES('roman','/sub_Transport/mod_Serial/ReqAnsw','
OK
','','');
INSERT INTO "SYS" VALUES('roman','/sub_Transport/mod_SSL/ReqAnsw','15 01 00 00 00 04 01 01 01 0b ','','');
INSERT INTO "SYS" VALUES('root','/sub_DAQ/RdStLevel','10','','');
INSERT INTO "SYS" VALUES('root','/sub_DAQ/RdTaskPer','1','','');
INSERT INTO "SYS" VALUES('root','/sub_DAQ/RdRestConnTm','20','','');
INSERT INTO "SYS" VALUES('root','/sub_DAQ/RdRestDtTm','2','','');
INSERT INTO "SYS" VALUES('root','/sub_DAQ/RdStList','loop;','','');
INSERT INTO "SYS" VALUES('roman','/sub_Special/mod_FLibSYS/fnc_floatSplitWord/ntCalc','1','','');
INSERT INTO "SYS" VALUES('roman','/sub_Special/mod_FLibSYS/fnc_floatMergeWord/ntCalc','1','','');
INSERT INTO "SYS" VALUES('roman','/sub_DAQ/mod_DiamondBoards/addType','a_prm','','');
INSERT INTO "SYS" VALUES('root','/WorkDB','MySQL.GenDB','','');
INSERT INTO "SYS" VALUES('root','/IcoDir','./icons','','');
INSERT INTO "SYS" VALUES('root','/ModDir','../../lib/openscada','','');
INSERT INTO "SYS" VALUES('root','/SaveAtExit','0','','');
INSERT INTO "SYS" VALUES('root','/SavePeriod','0','','');
INSERT INTO "SYS" VALUES('root','/MessLev','0','','');
INSERT INTO "SYS" VALUES('root','/LogTarget','10','','');
INSERT INTO "SYS" VALUES('roman','/sub_DAQ/mod_Siemens/onlOff','1','','');
INSERT INTO "SYS" VALUES('root','/Lang2CodeBase','en','','');
INSERT INTO "SYS" VALUES('roman','/sub_UI/mod_VCAEngine/wdgAttr','Pi_нд','','');
INSERT INTO "SYS" VALUES('roman','/sub_DAQ/mod_BlockCalc/cntr_Anast1to2node/showHide','0','','');
INSERT INTO "SYS" VALUES('roman','/sub_Special/mod_FLibSYS/fnc_dbReqSQL/ntCalc','1','','');
INSERT INTO "SYS" VALUES('root','/StName','Demo station','Демо станція','Демо станция');
INSERT INTO "SYS" VALUES('root','/sub_ModSched/ChkPer','0','','');
INSERT INTO "SYS" VALUES('root','/sub_Archive/MessBufSize','1000','','');
INSERT INTO "SYS" VALUES('root','/sub_Archive/MessPeriod','5','','');
INSERT INTO "SYS" VALUES('root','/sub_Archive/ValPeriod','1000','','');
INSERT INTO "SYS" VALUES('root','/sub_Archive/ValPriority','10','','');
INSERT INTO "SYS" VALUES('root','/sub_Archive/MaxReqMess','3000','','');
INSERT INTO "SYS" VALUES('root','/sub_UI/mod_WebVision/SessTimeLife','10','','');
INSERT INTO "SYS" VALUES('root','/sub_Protocol/mod_HTTP/AuthTime','10','','');
INSERT INTO "SYS" VALUES('root','/sub_Protocol/mod_SelfSystem/SessTimeLife','60','','');
INSERT INTO "SYS" VALUES('root','/sub_Protocol/mod_SelfSystem/ComprLev','0','','');
INSERT INTO "SYS" VALUES('root','/sub_Protocol/mod_SelfSystem/ComprBrd','80','','');
INSERT INTO "SYS" VALUES('roman','/sub_Special/mod_FLibSYS/fnc_tmTime/ntCalc','1','','');
INSERT INTO "SYS" VALUES('roman','/sub_Special/mod_FLibSYS/fnc_tmDate/ntCalc','1','','');
INSERT INTO "SYS" VALUES('roman','/sub_Special/mod_FLibSYS/fnc_tmStrPTime/ntCalc','1','','');
INSERT INTO "SYS" VALUES('root','/sub_UI/mod_Vision/UserPass','openscada','','');
INSERT INTO "SYS" VALUES('root','/sub_UI/mod_WebUser/DefPg','FlowTec.txt','','');
INSERT INTO "SYS" VALUES('roman','/sub_Transport/mod_Sockets/ReqReq','01 03 00 00 00 05 85 c9 ','','');
INSERT INTO "SYS" VALUES('roman','/sub_Transport/mod_Sockets/ReqAnsw','01 03 0a 03 e8 00 2e 00 00 00 0a 5f d7 ed 13 ','','');
INSERT INTO "SYS" VALUES('roman','/sub_Transport/mod_Sockets/ToTmOut','1','','');
INSERT INTO "SYS" VALUES('roman','/sub_Transport/mod_Sockets/ReqTm','1.01с','','');
INSERT INTO "SYS" VALUES('roman','/sub_DAQ/mod_JavaLikeCalc/lib_servProc/fnc_crc16/ntCalc','100','','');
INSERT INTO "SYS" VALUES('root','/sub_UI/mod_Vision/CachePgLife','1.5','','');
INSERT INTO "SYS" VALUES('roman','/sub_Special/mod_SystemTests/test_Param/ntCalc','1','','');
INSERT INTO "SYS" VALUES('roman','/sub_UI/mod_Vision/devWinState','AAAA/wAAAAD9AAAAAQAAAAAAAADyAAACWfwCAAAAAvwAAABXAAABLwAAAH0A////+gAAAAECAAAA
AvsAAAAQAFAAcgBvAGoAVAByAGUAZQEAAAAA/////wAAAHIA////+wAAAA4AVwBkAGcAVAByAGUA
ZQEAAAAA/////wAAAHIA/////AAAAYwAAAEkAAAAfQD////6AAAAAAIAAAAC+wAAABgASQBuAHMA
cABBAHQAdAByAEQAbwBjAGsBAAAAAP////8AAAByAP////sAAAAWAEkAbgBzAHAATABuAGsARABv
AGMAawEAAAAA/////wAAAHIA////AAADCAAAAlkAAAAEAAAABAAAAAgAAAAI/AAAAAUAAAACAAAA
BwAAABgAdgBpAHMASQB0AFQAbwBvAGwAQgBhAHIBAAAAAP////8AAAAAAAAAAAAAABIAZQBsAEYA
aQBnAFQAbwBvAGwAAAABW/////8AAAAAAAAAAAAAAAYAZABvAGMAAAABW/////8AAAAAAAAAAAAA
AAoAQQBHAEwASwBTAAAAAAD/////AAAAAAAAAAAAAAAIAHQAZQBzAHQAAAAAAP////8AAAAAAAAA
AAAAABIAbwByAGkAZwBpAG4AYQBsAHMBAAABW/////8AAAAAAAAAAAAAABYAdwBkAGcAVABvAG8A
bABWAGkAZQB3AAAAAkEAAAFHAAAAAAAAAAAAAAACAAAAAQAAAAgATQBhAGkAbgAAAAAA/////wAA
AAAAAAAAAAAAAgAAAAEAAAAKAG0AbgBFAGwAcwEAAAAA/////wAAAAAAAAAAAAAAAwAAAAAAAAAD
AAAAAA==','','');
INSERT INTO "SYS" VALUES('root','/sub_Protocol/mod_HTTP/AutoLogin','<aLog><it addrs="*" user="user"/></aLog>','','');
INSERT INTO "SYS" VALUES('roman','/sub_Special/mod_SystemTests/test_XML/ntCalc','1','','');
INSERT INTO "SYS" VALUES('roman','/sub_Special/mod_SystemTests/test_Mess/ntCalc','1','','');
INSERT INTO "SYS" VALUES('roman','/sub_Special/mod_SystemTests/test_SOAttach/ntCalc','1','','');
INSERT INTO "SYS" VALUES('roman','/sub_Special/mod_SystemTests/test_Val/ntCalc','1','','');
INSERT INTO "SYS" VALUES('roman','/sub_Special/mod_SystemTests/test_DB/ntCalc','1','','');
INSERT INTO "SYS" VALUES('roman','/sub_Special/mod_SystemTests/test_ValBuf/ntCalc','1','','');
INSERT INTO "SYS" VALUES('roman','/sub_Special/mod_SystemTests/test_Base64Code/ntCalc','1','','');
INSERT INTO "SYS" VALUES('roman','/sub_DAQ/mod_JavaLikeCalc/lib_techApp/fnc_pipeBase/ntCalc','1000','','');
INSERT INTO "SYS" VALUES('roman','/sub_DAQ/mod_JavaLikeCalc/lib_techApp/fnc_cooler/ntCalc','1000','','');
INSERT INTO "SYS" VALUES('roman','/sub_DAQ/mod_JavaLikeCalc/lib_techApp/fnc_heatExch/ntCalc','1000','','');
INSERT INTO "SYS" VALUES('roman','/sub_DAQ/mod_JavaLikeCalc/lib_techApp/fnc_pipe2_1/ntCalc','1000','','');
INSERT INTO "SYS" VALUES('roman','/sub_DAQ/mod_JavaLikeCalc/lib_techApp/fnc_boilerBarrel/ntCalc','1000','','');
INSERT INTO "SYS" VALUES('roman','/sub_DAQ/mod_JavaLikeCalc/lib_techApp/fnc_boilerBurner/ntCalc','1000','','');
INSERT INTO "SYS" VALUES('roman','/sub_DAQ/mod_JavaLikeCalc/lib_techApp/fnc_lag/ntCalc','1000','','');
INSERT INTO "SYS" VALUES('roman','/sub_DAQ/mod_JavaLikeCalc/lib_techApp/fnc_noise/ntCalc','1000','','');
INSERT INTO "SYS" VALUES('roman','/sub_DAQ/mod_JavaLikeCalc/lib_techApp/fnc_ballCrane/ntCalc','1000','','');
INSERT INTO "SYS" VALUES('roman','/sub_DAQ/mod_JavaLikeCalc/lib_techApp/fnc_separator/ntCalc','1000','','');
INSERT INTO "SYS" VALUES('roman','/sub_DAQ/mod_JavaLikeCalc/lib_techApp/fnc_klap/ntCalc','1000','','');
INSERT INTO "SYS" VALUES('roman','/sub_DAQ/mod_JavaLikeCalc/lib_techAppExp/fnc_klap/ntCalc','1000','','');
INSERT INTO "SYS" VALUES('roman','/sub_DAQ/mod_JavaLikeCalc/lib_techApp/fnc_lagClean/ntCalc','1000','','');
INSERT INTO "SYS" VALUES('roman','/sub_DAQ/mod_JavaLikeCalc/lib_techApp/fnc_net/ntCalc','1000','','');
INSERT INTO "SYS" VALUES('roman','/sub_DAQ/mod_JavaLikeCalc/lib_techAppExp/fnc_net/ntCalc','1000','','');
INSERT INTO "SYS" VALUES('roman','/sub_DAQ/mod_JavaLikeCalc/lib_techApp/fnc_src_press/ntCalc','1000','','');
INSERT INTO "SYS" VALUES('roman','/sub_DAQ/mod_JavaLikeCalc/lib_techAppExp/fnc_src_press/ntCalc','1000','','');
INSERT INTO "SYS" VALUES('roman','/sub_DAQ/mod_JavaLikeCalc/lib_techApp/fnc_compressor/ntCalc','1000','','');
INSERT INTO "SYS" VALUES('roman','/sub_DAQ/mod_JavaLikeCalc/lib_techApp/fnc_src_flow/ntCalc','1000','','');
INSERT INTO "SYS" VALUES('roman','/sub_DAQ/mod_JavaLikeCalc/lib_techApp/fnc_pipe1_1/ntCalc','1000','','');
INSERT INTO "SYS" VALUES('roman','/sub_DAQ/mod_JavaLikeCalc/lib_techApp/fnc_pipe1_2/ntCalc','1000','','');
INSERT INTO "SYS" VALUES('roman','/sub_DAQ/mod_JavaLikeCalc/lib_techApp/fnc_pipe1_3/ntCalc','1000','','');
INSERT INTO "SYS" VALUES('roman','/sub_DAQ/mod_JavaLikeCalc/lib_techApp/fnc_pipe1_4/ntCalc','1000','','');
INSERT INTO "SYS" VALUES('roman','/sub_DAQ/mod_JavaLikeCalc/lib_techApp/fnc_klapMech/ntCalc','1000','','');
INSERT INTO "SYS" VALUES('roman','/sub_DAQ/mod_JavaLikeCalc/lib_techApp/fnc_diafragma/ntCalc','1000','','');
INSERT INTO "SYS" VALUES('roman','/sub_DAQ/mod_JavaLikeCalc/lib_techApp/fnc_pipe3_1/ntCalc','1000','','');
INSERT INTO "SYS" VALUES('roman','/sub_Archive/vArch','','','');
INSERT INTO "SYS" VALUES('roman','/sub_UI/mod_Vision/uiProps','<UI devWinState="AAAA/wAAAAD9AAAAAQAAAAAAAAEAAAACdvwCAAAAAvwAAAA4AAABOQAAAH0A////+gAAAAACAAAA&#010;AvsAAAAQAFAAcgBvAGoAVAByAGUAZQEAAAAA/////wAAAHIA////+wAAAA4AVwBkAGcAVAByAGUA&#010;ZQEAAAAA/////wAAAHIA/////AAAAXcAAAE3AAAAfQD////6AAAAAAIAAAAC+wAAABgASQBuAHMA&#010;cABBAHQAdAByAEQAbwBjAGsBAAAAAP////8AAAByAP////sAAAAWAEkAbgBzAHAATABuAGsARABv&#010;AGMAawEAAAAA/////wAAAHIA////AAAC+gAAAnYAAAAEAAAABAAAAAgAAAAI/AAAAAIAAAACAAAA&#010;BAAAABgAdgBpAHMASQB0AFQAbwBvAGwAQgBhAHIBAAAAAP////8AAAAAAAAAAAAAABIAbwByAGkA&#010;ZwBpAG4AYQBsAHMBAAABWwAAAqUAAAAAAAAAAAAAABIAZQBsAEYAaQBnAFQAbwBvAGwAAAACJv//&#010;//8AAAAAAAAAAAAAABYAdwBkAGcAVABvAG8AbABWAGkAZQB3AAAAAkH/////AAAAAAAAAAAAAAAC&#010;AAAABQAAAAoAQQBHAEwASwBTAAAAAAD/////AAAAAAAAAAAAAAAIAE0AYQBpAG4AAAAAAP////8A&#010;AAAAAAAAAAAAAAYAZABvAGMAAAAAAP////8AAAAAAAAAAAAAAAoAbQBuAEUAbABzAAAAAAD/////&#010;AAAAAAAAAAAAAAAIAHQAZQBzAHQAAAAAAP////8AAAAAAAAAAA==" mnEls_icSz="16" visItToolBar_icSz="16" originals_icSz="16" wdgToolView_icSz="16" elFigTool_icSz="16"/>
','','');
INSERT INTO "SYS" VALUES('roman','/sub_Archive/messCat','*','','');
INSERT INTO "SYS" VALUES('roman','/sub_Archive/messSize','6000','','');
INSERT INTO "SYS" VALUES('root','/sub_UI/mod_VCAEngine/AutoSess','<Sess />','','');
INSERT INTO "SYS" VALUES('roman','/sub_Special/mod_FLibSYS/fnc_str2int/ntCalc','1','','');
INSERT INTO "SYS" VALUES('roman','/sub_Special/mod_FLibSYS/fnc_tmCron/ntCalc','1','','');
INSERT INTO "SYS" VALUES('roman','/sub_Archive/messLev','1','','');
INSERT INTO "SYS" VALUES('root','/sub_UI/mod_Vision/uiProps','<UI devWinState="AAAA/wAAAAD9AAAAAQAAAAAAAAEVAAACUPwCAAAAAvwAAAA3AAABIwAAAH0A////+gAAAAECAAAA&#010;AvsAAAAQAFAAcgBvAGoAVAByAGUAZQEAAAAA/////wAAAHIA////+wAAAA4AVwBkAGcAVAByAGUA&#010;ZQEAAAAA/////wAAAHIA/////AAAAWAAAAEnAAAAfQD////6AAAAAAIAAAAC+wAAABgASQBuAHMA&#010;cABBAHQAdAByAEQAbwBjAGsBAAAAAP////8AAAByAP////sAAAAWAEkAbgBzAHAATABuAGsARABv&#010;AGMAawEAAAAA/////wAAAHIA////AAAEOwAAAlAAAAAEAAAABAAAAAgAAAAI/AAAAAIAAAACAAAA&#010;CAAAABgAdgBpAHMASQB0AFQAbwBvAGwAQgBhAHIBAAAAAP////8AAAAAAAAAAAAAABYAdwBkAGcA&#010;VABvAG8AbABWAGkAZQB3AAAAAZQAAAEjAAAAAAAAAAAAAAASAGUAbABGAGkAZwBUAG8AbwBsAAAA&#010;Apz/////AAAAAAAAAAAAAAAIAE0AYQBpAG4AAAAAAP////8AAAAAAAAAAAAAAAYAZABvAGMAAAAA&#010;AP////8AAAAAAAAAAAAAABIAbwByAGkAZwBpAG4AYQBsAHMBAAABlAAAA0wAAAAAAAAAAAAAAAoA&#010;QQBHAEwASwBTAAAAAAD/////AAAAAAAAAAAAAAAIAHQAZQBzAHQAAAAAAP////8AAAAAAAAAAAAA&#010;AAMAAAABAAAACgBtAG4ARQBsAHMBAAAAAP////8AAAAAAAAAAA==" visItToolBar_icSz="16" originals_icSz="16" wdgToolView_icSz="16" mnEls_icSz="16" elFigTool_icSz="16" />
','','');
INSERT INTO "SYS" VALUES('root','/DemoStation/sub_UI/mod_QTCfg/StartPath','/DemoStation','','');
INSERT INTO "SYS" VALUES('root','/DemoStation/sub_UI/mod_QTCfg/StartUser','root','','');
INSERT INTO "SYS" VALUES('root','/DemoStation/sub_UI/mod_Vision/StartUser','root','','');
INSERT INTO "SYS" VALUES('root','/DemoStation/sub_UI/mod_Vision/UserPass','','','');
INSERT INTO "SYS" VALUES('root','/DemoStation/sub_UI/mod_Vision/RunPrjs','','','');
INSERT INTO "SYS" VALUES('root','/DemoStation/sub_UI/mod_Vision/VCAstation','.','','');
INSERT INTO "SYS" VALUES('root','/DemoStation/sub_UI/mod_Vision/PlayCom','play -q %f','','');
INSERT INTO "SYS" VALUES('root','/DemoStation/sub_UI/mod_Vision/CachePgLife','1','','');
INSERT INTO "SYS" VALUES('root','/Workdir','/mnt/home/roman/work/OScadaD/share/openscada_AGLKS','','');
INSERT INTO "SYS" VALUES('roman','/sub_Archive/mod_DBArch/mess_test/messTm','0','','');
INSERT INTO "SYS" VALUES('roman','/sub_DAQ/mod_JavaLikeCalc/lib_Controller/fnc_test/ntCalc','1','','');
INSERT INTO "SYS" VALUES('roman','/sub_Archive/mod_DBArch/mess_test/messCat','','','');
INSERT INTO "SYS" VALUES('roman','/sub_DAQ/mod_JavaLikeCalc/lib_Controller/fnc_test1/ntCalc','10000','','');
INSERT INTO "SYS" VALUES('roman','/sub_Archive/mod_FSArch/mess_StatErrors/messSize','600','','');
INSERT INTO "SYS" VALUES('roman','/sub_Archive/mod_FSArch/mess_NetRequsts/messSize','600','','');
INSERT INTO "SYS" VALUES('roman','/sub_DAQ/mod_OPC_UA/cntr_test/prm_test/selND','free (2:"DAQ.System.AutoDA.MemInfo.free")','','');
INSERT INTO "SYS" VALUES('roman','/sub_DAQ/mod_DAQGate/selCPRM','ModBus.testTCP.test','','');
CREATE TABLE 'Security_grp' ("NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"USERS" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' , PRIMARY KEY ("NAME"));
INSERT INTO "Security_grp" VALUES('users','Users group','user;root;roman;test1;','Група користувачів','Группа пользователей');
INSERT INTO "Security_grp" VALUES('BD','Data bases','roman;','Бази даних','Базы данных');
INSERT INTO "Security_grp" VALUES('Archive','Archives','root;test;','Архіви','Архивы');
INSERT INTO "Security_grp" VALUES('Security','Security','root;test;','Безпека','Безопасность');
INSERT INTO "Security_grp" VALUES('UI','Users'' interfaces','root;','Інтерфейси користувача','Интерфейсы пользователя');
INSERT INTO "Security_grp" VALUES('root','Administartors'' group','root;roman;','Група адміністраторів.','Группа администраторов.');
INSERT INTO "Security_grp" VALUES('Protocol','Transport''s protocols','root;','Транспортні протоколи','Транспортные протоколы');
INSERT INTO "Security_grp" VALUES('Special','Special','root;test;','Спеціальні','Специальные');
INSERT INTO "Security_grp" VALUES('DAQ','Data acquisition','root;','Збір даних','Сбор данных');
INSERT INTO "Security_grp" VALUES('ModSched','Modules control','root;','Керування модулями','Управление модулями');
INSERT INTO "Security_grp" VALUES('Transport','Transport','root;','Транспорти','Транспорты');
CREATE TABLE 'Security_user' ("NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"PASS" TEXT DEFAULT '' ,"PICTURE" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"LONGDESCR" TEXT DEFAULT '' ,"uk#LONGDESCR" TEXT DEFAULT '' , PRIMARY KEY ("NAME"));
INSERT INTO "Security_user" VALUES('roman','Roman Savochenko','roBH1UjQVoAL.','/9j/4AAQSkZJRgABAQEBLAEsAAD/4Q/4RXhpZgAATU0AKgAAAAgABwESAAMAAAABAAEAAAEaAAUA
AAABAAAAYgEbAAUAAAABAAAAagEoAAMAAAABAAIAAAExAAIAAAAUAAAAcgEyAAIAAAAUAAAAhodp
AAQAAAABAAAAmgAAAMQAAAEsAAAAAQAAASwAAAABQWRvYmUgUGhvdG9zaG9wIDcuMAAyMDA1OjA4
OjMwIDE2OjI4OjM2AAADoAEAAwAAAAEAAQAAoAIABAAAAAEAAAGAoAMABAAAAAEAAAHyAAAAAAAG
AQMAAwAAAAEABgAAARoABQAAAAEAAAESARsABQAAAAEAAAEaASgAAwAAAAEAAgAAAgEABAAAAAEA
AAEiAgIABAAAAAEAAA7OAAAAAAAAAEgAAAABAAAASAAAAAH/2P/gABBKRklGAAEBAAABAAEAAP/b
AEMACAYGBwYFCAcHBwkJCAoMFA0MCwsMGRITDxQdGh8eHRocHCAkLicgIiwjHBwoNyksMDE0NDQf
Jzk9ODI8LjM0Mv/bAEMBCQkJDAsMGA0NGDIhHCEyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIy
MjIyMjIyMjIyMjIyMjIyMjIyMjIyMv/AABEIAJYAcwMBIgACEQEDEQH/xAAfAAABBQEBAQEBAQAA
AAAAAAAAAQIDBAUGBwgJCgv/xAC1EAACAQMDAgQDBQUEBAAAAX0BAgMABBEFEiExQQYTUWEHInEU
MoGRoQgjQrHBFVLR8CQzYnKCCQoWFxgZGiUmJygpKjQ1Njc4OTpDREVGR0hJSlNUVVZXWFlaY2Rl
ZmdoaWpzdHV2d3h5eoOEhYaHiImKkpOUlZaXmJmaoqOkpaanqKmqsrO0tba3uLm6wsPExcbHyMnK
0tPU1dbX2Nna4eLj5OXm5+jp6vHy8/T19vf4+fr/xAAfAQADAQEBAQEBAQEBAAAAAAAAAQIDBAUG
BwgJCgv/xAC1EQACAQIEBAMEBwUEBAABAncAAQIDEQQFITEGEkFRB2FxEyIygQgUQpGhscEJIzNS
8BVictEKFiQ04SXxFxgZGiYnKCkqNTY3ODk6Q0RFRkdISUpTVFVWV1hZWmNkZWZnaGlqc3R1dnd4
eXqCg4SFhoeIiYqSk5SVlpeYmZqio6Slpqeoqaqys7S1tre4ubrCw8TFxsfIycrS09TV1tfY2dri
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','Роман Савоченко','Роман Савоченко','','');
INSERT INTO "Security_user" VALUES('root','Administrator (superuser)!!!','roZEK8sP/t6Qg','','Адміністратор (суперкористувач)!!!','Администратор (суперпользователь)!!!','','');
INSERT INTO "Security_user" VALUES('user','Simple user','us6EKZMmfBVwI','','Звичайний користувач','Простой пользователь','','');
INSERT INTO "Security_user" VALUES('test','Тестовий користувач','teMGKvBPcptKo','','Тестовий користувач','','','');
CREATE TABLE 'Siemens_CIFdevs' ("ID" INTEGER DEFAULT '' ,"ADDR" INTEGER DEFAULT '' ,"SPEED" INTEGER DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO "Siemens_CIFdevs" VALUES(0,10,6);
INSERT INTO "Siemens_CIFdevs" VALUES(1,0,0);
INSERT INTO "Siemens_CIFdevs" VALUES(2,0,0);
INSERT INTO "Siemens_CIFdevs" VALUES(3,0,0);
CREATE TABLE 'SoundCard_test' ("SHIFR" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '' ,"CHANNEL" INTEGER DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' , PRIMARY KEY ("SHIFR"));
INSERT INTO "SoundCard_test" VALUES('test','','',1,0,'','','','');
INSERT INTO "SoundCard_test" VALUES('test1','','',1,1,'','','','');
CREATE TABLE 'Transport_in' ("ID" TEXT DEFAULT '' ,"MODULE" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"DESCRIPT" TEXT DEFAULT '' ,"ADDR" TEXT DEFAULT '' ,"PROT" TEXT DEFAULT '' ,"START" INTEGER DEFAULT '' ,"BufLen" INTEGER DEFAULT '' ,"TMS" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#DESCRIPT" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DESCRIPT" TEXT DEFAULT '' ,"A_PRMS" TEXT DEFAULT '' , PRIMARY KEY ("ID","MODULE"));
INSERT INTO "Transport_in" VALUES('Self','Sockets','','','TCP::10005:1','SelfSystem',1,5,'1.88:320','Власний','','','','');
INSERT INTO "Transport_in" VALUES('WEB_1','Sockets','WEB 1','Main transport for WEB interface.','TCP::10002:0','HTTP',1,5,'1.88:320','WEB 1','Головний транспорт для WEB інтерфейсу.','WEB 1','Главный транспотр для WEB интерфейса.','<prms MaxQueue="10" MaxClients="100" BufLen="5" KeepAliveCnt="100" KeepAliveTm="5" TaskPrior="0" />
');
INSERT INTO "Transport_in" VALUES('WEB_2','Sockets','WEB 2','Reserve transport for WEB interface.','TCP::10004:0','HTTP',1,5,'1.88:320','WEB 2','Резервний транспорт для WEB інтерфейсу.','WEB 2','Резервный транспотр для WEB интерфейса.','<prms MaxQueue="10" MaxClients="100" BufLen="5" KeepAliveCnt="100" KeepAliveTm="5" TaskPrior="0" />
');
INSERT INTO "Transport_in" VALUES('WEB_2','SSL','WEB 2','Reserve security transport for WEB interface.','*:10044','HTTP',1,5,'1.88:320','WEB 2','Резервний безпечний транспорт для WEB інтерфейсу.','','Резервный безопасный транспотр для WEB интерфейса.','<prms MaxClients="100" BufLen="5" KeepAliveCnt="100" KeepAliveTm="5" TaskPrior="0" PKeyPass="123456">
<CertKey>
-----BEGIN CERTIFICATE-----
MIIEETCCAvmgAwIBAgIJAIgqZ1h4oyoBMA0GCSqGSIb3DQEBBQUAMIGeMQswCQYD
VQQGEwJVQTEXMBUGA1UECAwORG5lcHJvcGV0cm92c2sxGDAWBgNVBAcMD0RuZXBy
b2R6ZXJ6aW5zazESMBAGA1UECgwJT3BlblNDQURBMREwDwYDVQQLDAhTZWN1cml0
eTETMBEGA1UEAwwKb3NjYWRhLm9yZzEgMB4GCSqGSIb3DQEJARYRb3NjYWRhQG9z
Y2FkYS5vcmcwHhcNMTIwNjI1MDgzMDMxWhcNMjIwNjIzMDgzMDMxWjCBnjELMAkG
A1UEBhMCVUExFzAVBgNVBAgMDkRuZXByb3BldHJvdnNrMRgwFgYDVQQHDA9EbmVw
cm9kemVyemluc2sxEjAQBgNVBAoMCU9wZW5TQ0FEQTERMA8GA1UECwwIU2VjdXJp
dHkxEzARBgNVBAMMCm9zY2FkYS5vcmcxIDAeBgkqhkiG9w0BCQEWEW9zY2FkYUBv
c2NhZGEub3JnMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAyr2D6VhO
RYAF9qWS62lAzaTaZ6WHdHs4tjtYtd0Z9xY+UCHa/47bKHAeEyd3SM9DVpK7XwNP
OmewH5Dn6USQyLrwW96WU6ZV0lqjMT4aZnuTdKaaodIvGp3HXD3nfqsXgOOTrBH5
893aV2220VgaVL0QSOLUAylT0Venr6kXzY4YqCzz//NpFuCCOsvsTZXLD6utz7/z
OaEpqnnZ76TkDZtCG3xxSS/PxjmejIcSFbl1rU8E1nCgfyV/ONRZ0cHPymRE08CN
bE3mdEa1R0yw89ARFWGoh3/tQ63kgxT6ufG5j3GG+SOE1jtridGCayd8Mud2isPZ
NG7gDNugYjMVewIDAQABo1AwTjAdBgNVHQ4EFgQUBKNudjoE+GZaKssVXmA44fG5
AjEwHwYDVR0jBBgwFoAUBKNudjoE+GZaKssVXmA44fG5AjEwDAYDVR0TBAUwAwEB
/zANBgkqhkiG9w0BAQUFAAOCAQEAYCSCN1K0Qoe8Zpdr3bLnFW19DfWYemuTGUmG
mKLDQVswsNnUkqxoXrLU2fFQ98VncN268xt2cfW0WN0sN9Z1uX2YfUAxxzo5Uid+
OEqadRqSbvUgNW6znkD83zzOWNtFb/nf/UTTKaEgYTwvezIqcGsnpAd8b2F6JW7p
8zPsDgeNqXzv+kNvs8wJjpXkWIqGAC5YqkrN5jYUUBgj9nmk7VHaVUZa/YTRiarm
GbI7tBJtJWvpgI1e1gAZGmYh8u0fn5a6tqDrONIkoJYMArLmDMiavSmTsh2bAEb9
U/9OWDou6AtQesZq6lsf0xW0tl5Szj+D3PiHcokk7GRTua36rA==
-----END CERTIFICATE-----
-----BEGIN RSA PRIVATE KEY-----
Proc-Type: 4,ENCRYPTED
DEK-Info: DES-EDE3-CBC,D5F8F856E237054A

WfwrtuFxUIQaVgQ1LDRzbR/o4eldb7HPbraRv+or/uLJ2bZNeKtK1aQuXE8dTJS/
t1MM3JjKBy0MyN4rpjeyhAkBBHn7pNiYGO5QlCfPJsbztdMWB+KkYSfMmzIUuGnH
1ZsIYokc+D6uK7EYPoORMGE4qSmUj7cDtK9uTKODDdZfXaC+rKWYr0jTa8Nrvoac
o6tTkBogJsZh6uVTQOh9hmIKMhrTepoSzxnTvLcw96GYWmOaapeOzGHy8Z5J4yyy
47/GUQj1UjQ2GYxcya0/Lyii2qZBZrdngUndDZqbhVvPmN+pM3e6r/tx2R7a/btD
TACriXtfqnn5DLdg3phA+nzuCsHP13Pw3MFJd5VB216UhXdKeD6bVl1NEIH6qaq2
1TvtJHIvOaVJgfaPyuqMhMuZ6xKDEL2M9jmqlNOPY5L1kx7Nk47LVAnkoR6qh7c9
E+8QLf8UuGNYLBeKdrIuCIjf/ul4zP3SS/kaaQO5VU35PkRkBaujidhdX5FEsP/C
rb/MoLkxQPoUJ6300kP8PDx55LsVNr8Vw0ujYF4tiQ5uotMXfI+iX0CE6Su/gO0L
pw2XvcklRqUeFPLjdYtJBNlrAnI/KHtCoZu+MezTn7VktNlUP2tAUvRWKEd/XMrX
+q9GCMXTOWOW+g/hMpBbpqc7OYOAYVIUWnuCQWrMOpttTRSsvAXtSx/R1GzDFLOQ
t3yiuHrW+mcq5SlbyHUy5F+FiPBplDCwpwXOed1K5pLlepPbp5OHkCJosCoiEHjE
xhFxoXh8pKr8jGaFiWcdNJnVJGQvDqowRDVOh9hMU7EyRbgkqUn416jyL4jtlzQP
9wfcurNREDLPjOWn86yPdJFFVc6VRwEsRr/nblFEz+Oz+2a0WMw3iLcFAB+xIhNp
G2uT6J/qC7NWmnQIOK90O5wrODFq7TuLMsm58jVeNylT4vBnY9jn1THvpghxCJg+
KendTSkbrXNPUJM1IvL0TBNPI72kfH57poB2dYkj+R9fGFeq8QvIYJYFQe2vG77C
RNViojoUJPAeYBBjTq2SkvLZkucQzV2oZgfQCSbhpyXXgonQ/Dj+XrU52iAVJOVJ
EAVRqNfLvILttq54uAI1DLU10TdXyOJreh82Ci1wvCr2qKHbtFXKt0hVIpCrTdzU
JdN12HCIz8b/+9U88qia2YhH8SGwuHyoz33hGzH3bH7hCiTt51sBkl2ERHBLTiGm
tEv5UIvpTquq3iKaZZjr8kViM5TPNBISprKJ4FIq6wmyRYt6R1lyLMaXfAetYirZ
5/iMJx5hFP/RHsE5x+OuNDBra8wzZtlgicT9QGYAiS4O9uDbxh1vn7DvAdyk+bxn
FEB6Me2klM6lX0m99gRxjVrY646PrnexBweNs64g8tBAS7CfLEpLoatbOtDKPY2o
gC2fW0PaxIugZ6DEBSpOlJFtxoFaa9rr+B6taOO/xv9CF1np0AKRhJG431qjELyl
Ixp1PkOjs7COr0guxZEX8pM4wIF3uF77sbNYViv/5fil1JH8mfwusKljLF5pTs9e
fdfa9weotHT55KrlRp06LxgUqckwoHoGjKDwR8bCk7s0QobgxuFufjEqUZg44VXw
-----END RSA PRIVATE KEY-----
</CertKey>
</prms>
');
INSERT INTO "Transport_in" VALUES('WEB_1','SSL','WEB 1','Main security transport for WEB interface.','*:10042','HTTP',1,5,'1.88:320','WEB 1','Головний безпечний транспорт для WEB інтерфейсу.','','Главный безопасный транспотр для WEB интерфейса.','<prms MaxClients="100" BufLen="5" KeepAliveCnt="100" KeepAliveTm="5" TaskPrior="0" PKeyPass="123456">
<CertKey>
-----BEGIN CERTIFICATE-----
MIIEETCCAvmgAwIBAgIJAIgqZ1h4oyoBMA0GCSqGSIb3DQEBBQUAMIGeMQswCQYD
VQQGEwJVQTEXMBUGA1UECAwORG5lcHJvcGV0cm92c2sxGDAWBgNVBAcMD0RuZXBy
b2R6ZXJ6aW5zazESMBAGA1UECgwJT3BlblNDQURBMREwDwYDVQQLDAhTZWN1cml0
eTETMBEGA1UEAwwKb3NjYWRhLm9yZzEgMB4GCSqGSIb3DQEJARYRb3NjYWRhQG9z
Y2FkYS5vcmcwHhcNMTIwNjI1MDgzMDMxWhcNMjIwNjIzMDgzMDMxWjCBnjELMAkG
A1UEBhMCVUExFzAVBgNVBAgMDkRuZXByb3BldHJvdnNrMRgwFgYDVQQHDA9EbmVw
cm9kemVyemluc2sxEjAQBgNVBAoMCU9wZW5TQ0FEQTERMA8GA1UECwwIU2VjdXJp
dHkxEzARBgNVBAMMCm9zY2FkYS5vcmcxIDAeBgkqhkiG9w0BCQEWEW9zY2FkYUBv
c2NhZGEub3JnMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAyr2D6VhO
RYAF9qWS62lAzaTaZ6WHdHs4tjtYtd0Z9xY+UCHa/47bKHAeEyd3SM9DVpK7XwNP
OmewH5Dn6USQyLrwW96WU6ZV0lqjMT4aZnuTdKaaodIvGp3HXD3nfqsXgOOTrBH5
893aV2220VgaVL0QSOLUAylT0Venr6kXzY4YqCzz//NpFuCCOsvsTZXLD6utz7/z
OaEpqnnZ76TkDZtCG3xxSS/PxjmejIcSFbl1rU8E1nCgfyV/ONRZ0cHPymRE08CN
bE3mdEa1R0yw89ARFWGoh3/tQ63kgxT6ufG5j3GG+SOE1jtridGCayd8Mud2isPZ
NG7gDNugYjMVewIDAQABo1AwTjAdBgNVHQ4EFgQUBKNudjoE+GZaKssVXmA44fG5
AjEwHwYDVR0jBBgwFoAUBKNudjoE+GZaKssVXmA44fG5AjEwDAYDVR0TBAUwAwEB
/zANBgkqhkiG9w0BAQUFAAOCAQEAYCSCN1K0Qoe8Zpdr3bLnFW19DfWYemuTGUmG
mKLDQVswsNnUkqxoXrLU2fFQ98VncN268xt2cfW0WN0sN9Z1uX2YfUAxxzo5Uid+
OEqadRqSbvUgNW6znkD83zzOWNtFb/nf/UTTKaEgYTwvezIqcGsnpAd8b2F6JW7p
8zPsDgeNqXzv+kNvs8wJjpXkWIqGAC5YqkrN5jYUUBgj9nmk7VHaVUZa/YTRiarm
GbI7tBJtJWvpgI1e1gAZGmYh8u0fn5a6tqDrONIkoJYMArLmDMiavSmTsh2bAEb9
U/9OWDou6AtQesZq6lsf0xW0tl5Szj+D3PiHcokk7GRTua36rA==
-----END CERTIFICATE-----
-----BEGIN RSA PRIVATE KEY-----
Proc-Type: 4,ENCRYPTED
DEK-Info: DES-EDE3-CBC,D5F8F856E237054A

WfwrtuFxUIQaVgQ1LDRzbR/o4eldb7HPbraRv+or/uLJ2bZNeKtK1aQuXE8dTJS/
t1MM3JjKBy0MyN4rpjeyhAkBBHn7pNiYGO5QlCfPJsbztdMWB+KkYSfMmzIUuGnH
1ZsIYokc+D6uK7EYPoORMGE4qSmUj7cDtK9uTKODDdZfXaC+rKWYr0jTa8Nrvoac
o6tTkBogJsZh6uVTQOh9hmIKMhrTepoSzxnTvLcw96GYWmOaapeOzGHy8Z5J4yyy
47/GUQj1UjQ2GYxcya0/Lyii2qZBZrdngUndDZqbhVvPmN+pM3e6r/tx2R7a/btD
TACriXtfqnn5DLdg3phA+nzuCsHP13Pw3MFJd5VB216UhXdKeD6bVl1NEIH6qaq2
1TvtJHIvOaVJgfaPyuqMhMuZ6xKDEL2M9jmqlNOPY5L1kx7Nk47LVAnkoR6qh7c9
E+8QLf8UuGNYLBeKdrIuCIjf/ul4zP3SS/kaaQO5VU35PkRkBaujidhdX5FEsP/C
rb/MoLkxQPoUJ6300kP8PDx55LsVNr8Vw0ujYF4tiQ5uotMXfI+iX0CE6Su/gO0L
pw2XvcklRqUeFPLjdYtJBNlrAnI/KHtCoZu+MezTn7VktNlUP2tAUvRWKEd/XMrX
+q9GCMXTOWOW+g/hMpBbpqc7OYOAYVIUWnuCQWrMOpttTRSsvAXtSx/R1GzDFLOQ
t3yiuHrW+mcq5SlbyHUy5F+FiPBplDCwpwXOed1K5pLlepPbp5OHkCJosCoiEHjE
xhFxoXh8pKr8jGaFiWcdNJnVJGQvDqowRDVOh9hMU7EyRbgkqUn416jyL4jtlzQP
9wfcurNREDLPjOWn86yPdJFFVc6VRwEsRr/nblFEz+Oz+2a0WMw3iLcFAB+xIhNp
G2uT6J/qC7NWmnQIOK90O5wrODFq7TuLMsm58jVeNylT4vBnY9jn1THvpghxCJg+
KendTSkbrXNPUJM1IvL0TBNPI72kfH57poB2dYkj+R9fGFeq8QvIYJYFQe2vG77C
RNViojoUJPAeYBBjTq2SkvLZkucQzV2oZgfQCSbhpyXXgonQ/Dj+XrU52iAVJOVJ
EAVRqNfLvILttq54uAI1DLU10TdXyOJreh82Ci1wvCr2qKHbtFXKt0hVIpCrTdzU
JdN12HCIz8b/+9U88qia2YhH8SGwuHyoz33hGzH3bH7hCiTt51sBkl2ERHBLTiGm
tEv5UIvpTquq3iKaZZjr8kViM5TPNBISprKJ4FIq6wmyRYt6R1lyLMaXfAetYirZ
5/iMJx5hFP/RHsE5x+OuNDBra8wzZtlgicT9QGYAiS4O9uDbxh1vn7DvAdyk+bxn
FEB6Me2klM6lX0m99gRxjVrY646PrnexBweNs64g8tBAS7CfLEpLoatbOtDKPY2o
gC2fW0PaxIugZ6DEBSpOlJFtxoFaa9rr+B6taOO/xv9CF1np0AKRhJG431qjELyl
Ixp1PkOjs7COr0guxZEX8pM4wIF3uF77sbNYViv/5fil1JH8mfwusKljLF5pTs9e
fdfa9weotHT55KrlRp06LxgUqckwoHoGjKDwR8bCk7s0QobgxuFufjEqUZg44VXw
-----END RSA PRIVATE KEY-----
</CertKey>
</prms>
');
INSERT INTO "Transport_in" VALUES('Self','SSL','','','127.0.0.1:10045','SelfSystem',1,5,'1.88:320','Власний','','','','<prms MaxClients="10" BufLen="5" KeepAliveCnt="100" KeepAliveTm="5" TaskPrior="0" PKeyPass="123456">
<CertKey>
-----BEGIN CERTIFICATE-----
MIIEETCCAvmgAwIBAgIJAIgqZ1h4oyoBMA0GCSqGSIb3DQEBBQUAMIGeMQswCQYD
VQQGEwJVQTEXMBUGA1UECAwORG5lcHJvcGV0cm92c2sxGDAWBgNVBAcMD0RuZXBy
b2R6ZXJ6aW5zazESMBAGA1UECgwJT3BlblNDQURBMREwDwYDVQQLDAhTZWN1cml0
eTETMBEGA1UEAwwKb3NjYWRhLm9yZzEgMB4GCSqGSIb3DQEJARYRb3NjYWRhQG9z
Y2FkYS5vcmcwHhcNMTIwNjI1MDgzMDMxWhcNMjIwNjIzMDgzMDMxWjCBnjELMAkG
A1UEBhMCVUExFzAVBgNVBAgMDkRuZXByb3BldHJvdnNrMRgwFgYDVQQHDA9EbmVw
cm9kemVyemluc2sxEjAQBgNVBAoMCU9wZW5TQ0FEQTERMA8GA1UECwwIU2VjdXJp
dHkxEzARBgNVBAMMCm9zY2FkYS5vcmcxIDAeBgkqhkiG9w0BCQEWEW9zY2FkYUBv
c2NhZGEub3JnMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAyr2D6VhO
RYAF9qWS62lAzaTaZ6WHdHs4tjtYtd0Z9xY+UCHa/47bKHAeEyd3SM9DVpK7XwNP
OmewH5Dn6USQyLrwW96WU6ZV0lqjMT4aZnuTdKaaodIvGp3HXD3nfqsXgOOTrBH5
893aV2220VgaVL0QSOLUAylT0Venr6kXzY4YqCzz//NpFuCCOsvsTZXLD6utz7/z
OaEpqnnZ76TkDZtCG3xxSS/PxjmejIcSFbl1rU8E1nCgfyV/ONRZ0cHPymRE08CN
bE3mdEa1R0yw89ARFWGoh3/tQ63kgxT6ufG5j3GG+SOE1jtridGCayd8Mud2isPZ
NG7gDNugYjMVewIDAQABo1AwTjAdBgNVHQ4EFgQUBKNudjoE+GZaKssVXmA44fG5
AjEwHwYDVR0jBBgwFoAUBKNudjoE+GZaKssVXmA44fG5AjEwDAYDVR0TBAUwAwEB
/zANBgkqhkiG9w0BAQUFAAOCAQEAYCSCN1K0Qoe8Zpdr3bLnFW19DfWYemuTGUmG
mKLDQVswsNnUkqxoXrLU2fFQ98VncN268xt2cfW0WN0sN9Z1uX2YfUAxxzo5Uid+
OEqadRqSbvUgNW6znkD83zzOWNtFb/nf/UTTKaEgYTwvezIqcGsnpAd8b2F6JW7p
8zPsDgeNqXzv+kNvs8wJjpXkWIqGAC5YqkrN5jYUUBgj9nmk7VHaVUZa/YTRiarm
GbI7tBJtJWvpgI1e1gAZGmYh8u0fn5a6tqDrONIkoJYMArLmDMiavSmTsh2bAEb9
U/9OWDou6AtQesZq6lsf0xW0tl5Szj+D3PiHcokk7GRTua36rA==
-----END CERTIFICATE-----
-----BEGIN RSA PRIVATE KEY-----
Proc-Type: 4,ENCRYPTED
DEK-Info: DES-EDE3-CBC,D5F8F856E237054A

WfwrtuFxUIQaVgQ1LDRzbR/o4eldb7HPbraRv+or/uLJ2bZNeKtK1aQuXE8dTJS/
t1MM3JjKBy0MyN4rpjeyhAkBBHn7pNiYGO5QlCfPJsbztdMWB+KkYSfMmzIUuGnH
1ZsIYokc+D6uK7EYPoORMGE4qSmUj7cDtK9uTKODDdZfXaC+rKWYr0jTa8Nrvoac
o6tTkBogJsZh6uVTQOh9hmIKMhrTepoSzxnTvLcw96GYWmOaapeOzGHy8Z5J4yyy
47/GUQj1UjQ2GYxcya0/Lyii2qZBZrdngUndDZqbhVvPmN+pM3e6r/tx2R7a/btD
TACriXtfqnn5DLdg3phA+nzuCsHP13Pw3MFJd5VB216UhXdKeD6bVl1NEIH6qaq2
1TvtJHIvOaVJgfaPyuqMhMuZ6xKDEL2M9jmqlNOPY5L1kx7Nk47LVAnkoR6qh7c9
E+8QLf8UuGNYLBeKdrIuCIjf/ul4zP3SS/kaaQO5VU35PkRkBaujidhdX5FEsP/C
rb/MoLkxQPoUJ6300kP8PDx55LsVNr8Vw0ujYF4tiQ5uotMXfI+iX0CE6Su/gO0L
pw2XvcklRqUeFPLjdYtJBNlrAnI/KHtCoZu+MezTn7VktNlUP2tAUvRWKEd/XMrX
+q9GCMXTOWOW+g/hMpBbpqc7OYOAYVIUWnuCQWrMOpttTRSsvAXtSx/R1GzDFLOQ
t3yiuHrW+mcq5SlbyHUy5F+FiPBplDCwpwXOed1K5pLlepPbp5OHkCJosCoiEHjE
xhFxoXh8pKr8jGaFiWcdNJnVJGQvDqowRDVOh9hMU7EyRbgkqUn416jyL4jtlzQP
9wfcurNREDLPjOWn86yPdJFFVc6VRwEsRr/nblFEz+Oz+2a0WMw3iLcFAB+xIhNp
G2uT6J/qC7NWmnQIOK90O5wrODFq7TuLMsm58jVeNylT4vBnY9jn1THvpghxCJg+
KendTSkbrXNPUJM1IvL0TBNPI72kfH57poB2dYkj+R9fGFeq8QvIYJYFQe2vG77C
RNViojoUJPAeYBBjTq2SkvLZkucQzV2oZgfQCSbhpyXXgonQ/Dj+XrU52iAVJOVJ
EAVRqNfLvILttq54uAI1DLU10TdXyOJreh82Ci1wvCr2qKHbtFXKt0hVIpCrTdzU
JdN12HCIz8b/+9U88qia2YhH8SGwuHyoz33hGzH3bH7hCiTt51sBkl2ERHBLTiGm
tEv5UIvpTquq3iKaZZjr8kViM5TPNBISprKJ4FIq6wmyRYt6R1lyLMaXfAetYirZ
5/iMJx5hFP/RHsE5x+OuNDBra8wzZtlgicT9QGYAiS4O9uDbxh1vn7DvAdyk+bxn
FEB6Me2klM6lX0m99gRxjVrY646PrnexBweNs64g8tBAS7CfLEpLoatbOtDKPY2o
gC2fW0PaxIugZ6DEBSpOlJFtxoFaa9rr+B6taOO/xv9CF1np0AKRhJG431qjELyl
Ixp1PkOjs7COr0guxZEX8pM4wIF3uF77sbNYViv/5fil1JH8mfwusKljLF5pTs9e
fdfa9weotHT55KrlRp06LxgUqckwoHoGjKDwR8bCk7s0QobgxuFufjEqUZg44VXw
-----END RSA PRIVATE KEY-----
</CertKey>
</prms>
');
INSERT INTO "Transport_in" VALUES('testModBus','Serial','Test ModBus',' ','/dev/ttyS1:115200:8N1','ModBus',0,5,'0.95:48','Test ModBus',' ','Тест ModBus','','<prms TMS="0.95:48"/>
');
INSERT INTO "Transport_in" VALUES('testModBus','Sockets','Test ModBus',' ','TCP::10502:1','ModBus',1,5,'0.57:320','Тест ModBus',' ','Тест ModBus','','<prms MaxQueue="10" MaxClients="10" BufLen="5" KeepAliveCnt="100" KeepAliveTm="5" TaskPrior="0"/>
');
INSERT INTO "Transport_in" VALUES('testOPC','Sockets','','','TCP::4841','OPC_UA',1,5,'','','','','','<prms MaxQueue="10" MaxClients="10" BufLen="5" KeepAliveReqs="0" KeepAliveTm="60" TaskPrior="0" />
');
INSERT INTO "Transport_in" VALUES('testModem','Serial','Test Modem',' ','/dev/ttyUSB0:115200:8N1::1','UserProtocol.test',0,0,'','Test Modem',' ','','','<prms TMS="0.95:48" MdmPreInit="0.5" MdmPostInit="1" MdmInitStr1="ATZ" MdmInitStr2="" MdmInitResp="OK" MdmRingReq="RING" MdmRingAnswer="ATA" MdmRingAnswerResp="CONNECT" MdmHangUp="+++ATH" MdmHangUpResp="OK"/>
');
CREATE TABLE 'Transport_out' ("ID" TEXT DEFAULT '' ,"MODULE" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"DESCRIPT" TEXT DEFAULT '' ,"ADDR" TEXT DEFAULT '' ,"START" INTEGER DEFAULT '' ,"SSLCertKey" TEXT DEFAULT '' ,"SSLKeyPass" TEXT DEFAULT '' ,"TMS" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#DESCRIPT" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DESCRIPT" TEXT DEFAULT '' ,"A_PRMS" TEXT DEFAULT '' , PRIMARY KEY ("ID","MODULE"));
INSERT INTO "Transport_out" VALUES('testModBus','Serial','Test ModBus',' ','/dev/ttyS0:115200:8N1',0,'','','50:4','Test ModBus',' ','Тест ModBus','','<prms TMS="97:0.95"/>
');
INSERT INTO "Transport_out" VALUES('testModBus','Sockets','Test ModBus',' ','TCP:localhost:10502',1,'','','1000','Тест ModBus',' ','Тест ModBus','','<prms tms="6:1"/>
');
INSERT INTO "Transport_out" VALUES('UIQtCfgloopSSL','SSL','','','localhost:10045',0,'','','','','','','','<prms PKeyPass="12" TMS="5:1">
<CertKey/>
</prms>
');
INSERT INTO "Transport_out" VALUES('UIQtCfgloop','Sockets','','','TCP:localhost:10005',0,'','','1000','','','','','<prms tms="10:2" />
');
INSERT INTO "Transport_out" VALUES('DAQRedundantloop','Sockets','','','TCP:localhost:10005',0,'','','1000','','','','','');
INSERT INTO "Transport_out" VALUES('DAQGatetestloop','Sockets','','','TCP:localhost:10005',0,'','','1000','','','','','');
INSERT INTO "Transport_out" VALUES('testModem','Serial','Test Modem',' ','/dev/ttyACM0:38400:8N1::0679859815',0,'','','','Test Modem',' ','','','<prms TMS="5000:1000" MdmTm="30" MdmLifeTime="30" MdmPreInit="0.5" MdmPostInit="1" MdmInitStr1="ATZ" MdmInitStr2="" MdmInitResp="OK" MdmDialStr="ATDT" MdmCnctResp="CONNECT" MdmBusyResp="BUSY" MdmNoCarResp="NO CARRIER" MdmNoDialToneResp="NO DIALTONE" MdmHangUp="+++ATH" MdmHangUpResp="OK"/>
');
INSERT INTO "Transport_out" VALUES('HDDTemp','Sockets','Parameter Hddtemp','','TCP:127.0.0.1:7634',0,'','','','Параметр Hddtemp','','Параметр Hddtemp','','<prms tms="5:0.01" />
');
INSERT INTO "Transport_out" VALUES('OPCUAtest','Sockets','','OPC UA автоматично створений транспорт для контроллеру ''test''.','TCP:127.0.0.1:4841',0,'','','','','OPC UA автоматично створений транспорт для контроллеру ''test''.','','OPC UA автоматично створений транспорт для контроллеру ''test''.','<prms tms="5:0.1" />
');
INSERT INTO "Transport_out" VALUES('sys_UPS','Sockets','UPS','','TCP:localhost:3493',0,'','','','UPS','','','','<prms tms="3:0.1" />
');
CREATE TABLE 'UserProtocol_uPrt' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '' ,"InPROG" TEXT DEFAULT '' ,"uk#InPROG" TEXT DEFAULT '' ,"OutTR" TEXT DEFAULT '' ,"OutPROG" TEXT DEFAULT '' ,"uk#OutPROG" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"ru#InPROG" TEXT DEFAULT '' ,"ru#OutPROG" TEXT DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO "UserProtocol_uPrt" VALUES('test','','','','',1,'JavaLikeCalc.JavaScript
answer = request;','JavaLikeCalc.JavaScript
SYS.messDebug("TEST Self",request);
answer = request;
return 0;','*','JavaLikeCalc.JavaScript
io.setText(tr.messIO(io.text()));','','','','JavaLikeCalc.JavaScript
SYS.messDebug("TEST REQ: ",request);
answer = ">+05.123+04.153+07.234-02.356+10.000-05.133+02.345+08.234";
//answer = ">+04.153+07.234-02.356+10.000-05.133+02.345+08.234";
//answer = "?";
CRC = 0;
for(i=0;i<answer.length;i++) CRC += answer.charCodeAt(i);
answer += (CRC&0xFF).toString(16)+"\r";
SYS.messDebug("TEST ANSV: "+answer.charCodeAt(0),answer);
return 0;','JavaLikeCalc.JavaScript
rez=Special.FLibSYS.strDec4Bin(tr.messIO(io.text()));
while(true)
{
	trez = tr.messIO("");
	if( !trez.length ) break;
	rez+=trez;
}
io.setAttr( "status", (rez==io.text()) ? "online" : "offline" );');
INSERT INTO "UserProtocol_uPrt" VALUES('DCON','','','','',1,'JavaLikeCalc.JavaScript
var enCRC = true;
//SYS.messDebug("TEST REQ: ",request);
//Test request for full
if(request.length < 4 || request[request.length-1] != "\r")
{
  if(request.length > 10) request = "";
  return true;
}
//Check for integrity of the request (CRC) and address
if(enCRC)
{
	CRC = 0;
	for(i = 0; i < (request.length-3); i++) CRC += request.charCodeAt(i);
	if(CRC != request.slice(request.length-3,request.length-1).toInt(16) || request.slice(1,3).toInt(16) != 10) return false;
}
//Analysis of the request and response prepare
if(request.charCodeAt(0) == "#") answer = ">+05.123+04.153+07.234-02.356+10.000-05.133+02.345+08.234";
else if(request.charCodeAt(0) == "@") answer = ">AB3C";
else answer = "?";
//Finish response
if(enCRC)
{
	CRC = 0;
	for(i=0; i < answer.length; i++) CRC += answer.charCodeAt(i);
	answer += (CRC&0xFF).toString(16)+"\r";
}
//SYS.messDebug("TEST ANSV: "+answer.charCodeAt(0),answer);
return 0;','','*','JavaLikeCalc.JavaScript
//Result request prepare
request = io.name().slice(0,1) + io.attr("addr").toInt().toString(16,2) + io.text();
if(io.attr("CRC").toInt())
{
	CRC = 0;
	for(i = 0; i < request.length; i++) CRC += request.charCodeAt(i);
	request += (CRC&0xFF).toString(16) + "\r";
}
else request += "\r";
//Send request
resp = tr.messIO(request);
while(resp[resp.length-1] != "\r")
{
  tresp = tr.messIO("");
  if(!tresp.length) break;
  resp += tresp;
}
if(io.attr("CRC").toInt())
{
	//Analysis response
	if(resp.length < 4 || resp[resp.length-1] != "\r") { io.setAttr("err","10:Error or no response."); return; }
	//Check response to the integrity (CRC)
	CRC = 0;
	for(i = 0; i < (resp.length-3); i++) CRC += resp.charCodeAt(i);
	if(CRC != resp.slice(resp.length-3,resp.length-1).toInt(16)) { io.setAttr("err","11:CRC error."); return; }
}
//Analysis response
else if(resp.length < 2 || resp[resp.length-1] != "\r") { io.setAttr("err","10:Error or no response."); return; }
if(resp[0] != ">") { io.setAttr("err","12:"+resp[0]+":DCON error."); return; }
//The result return
io.setAttr("err","");
io.setText(resp.slice(1,resp.length-3));','','','','JavaLikeCalc.JavaScript
var enCRC = true;
//SYS.messDebug("TEST REQ: ",request);
//Проверка запроса на полноту
if(request.length < 4 || request[request.length-1] != "\r")
{
  if(request.length > 10) request = "";
  return true;
}
//Проверка запроса на целостность (CRC) и адрес
if(enCRC)
{
	CRC = 0;
	for(i = 0; i < (request.length-3); i++) CRC += request.charCodeAt(i);
	if(CRC != request.slice(request.length-3,request.length-1).toInt(16) || request.slice(1,3).toInt(16) != 10) return false;
}
//Анализ запроса и подготовка ответа
if(request.charCodeAt(0) == "#") answer = ">+05.123+04.153+07.234-02.356+10.000-05.133+02.345+08.234";
else if(request.charCodeAt(0) == "@") answer = ">AB3C";
else answer = "?";
//Завершение ответа
if(enCRC)
{
	CRC = 0;
	for(i=0; i < answer.length; i++) CRC += answer.charCodeAt(i);
	answer += (CRC&0xFF).toString(16)+"\r";
}
//SYS.messDebug("TEST ANSV: "+answer.charCodeAt(0),answer);
return 0;','JavaLikeCalc.JavaScript
//Формирование конечного запроса
request = io.name().slice(0,1)+io.attr("addr").toInt().toString(16,2)+io.text();
if(io.attr("CRC").toInt())
{
	CRC = 0;
	for(i=0; i < request.length; i++) CRC += request.charCodeAt(i);
	request += (CRC&0xFF).toString(16)+"\r";
}
else request += "\r";
//Отправка запроса
resp = tr.messIO(request);
while(resp[resp.length-1] != "\r")
{
  tresp = tr.messIO("");
  if(!tresp.length) break;
  resp += tresp;
}
if(io.attr("CRC").toInt())
{
	//Анализ ответа
	if(resp.length < 4 || resp[resp.length-1] != "\r") { io.setAttr("err","10:Ошибка или нет ответа."); return; }
	//Проверка ответа на целостность (CRC)
	CRC = 0;
	for(i = 0; i < (resp.length-3); i++) CRC += resp.charCodeAt(i);
	if(CRC != resp.slice(resp.length-3,resp.length-1).toInt(16)) { io.setAttr("err","11:Ошибка CRC."); return; }
}
//Анализ ответа
else if(resp.length < 2 || resp[resp.length-1] != "\r") { io.setAttr("err","10:Ошибка или нет ответа."); return; }
if(resp[0] != ">") { io.setAttr("err","12:"+resp[0]+":Ошибка DCON."); return; }
//Возврат результата
io.setAttr("err","");
io.setText(resp.slice(1,resp.length-3));');
INSERT INTO "UserProtocol_uPrt" VALUES('TM510x','Elemer TM510x','','Elemer TM5102 and TM5103','',1,'','','*','JavaLikeCalc.JavaScript
//Request form:
//<mess addr="1">{req}</mess> - message tag
//  addr - remote station address (1...254)

io.setAttr("err","");
addr = io.attr("addr").toInt();
if(addr < 1 || addr > 254) { io.setAttr("err","1:Device address out of range 1...254"); return; }
request = ":"+addr.toString(10)+";"+io.text()+";";
//> Calc KS
KS = 0xFFFF;
for(i = 1; i < request.length; i++)
{
	KS = KS ^ request.charCodeAt(i);
	for(j = 0; j < 8; j++)
		KS = (KS&0x01) ? (KS >> 1)^0xA001 : (KS >> 1);
}
request += KS.toString(10)+"\r";
//SYS.messDebug("PRT","Request: "+request);

//Send request
resp = tr.messIO(request);
while(resp.length && resp[resp.length-1] != "\r")
{
	tresp = tr.messIO("");
	if(!tresp.length) break;
  	resp += tresp;
}
if(resp.length && resp.charCodeAt(0) == 0xFF) resp = resp.slice(1);
if(resp.length < 7 || resp[resp.length-1] != "\r" ||
	resp[0] != "!" || resp.slice(1).toInt() != addr || (KSpos=resp.lastIndexOf(";")) < 0)
{ io.setAttr("err","2:No or error respond"); return; }
//SYS.messDebug("PRT","Respond: "+resp);

//> Calc KS
KS = 0xFFFF;
for(i = 1; i < min(KSpos+1,resp.length); i++)
{
	KS = KS ^ resp.charCodeAt(i);
	for(j = 0; j < 8; j++)
		KS = (KS&0x01) ? (KS >> 1)^0xA001 : (KS >> 1);
}
if(KS != resp.slice(KSpos+1).toInt(10)) { io.setAttr("err","6:KS error."); return; }
io.setText(resp.slice(resp.indexOf(";")+1,KSpos));','','','','','');
CREATE TABLE 'VCALibs' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"DB_TBL" TEXT DEFAULT '' ,"ICO" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO "VCALibs" VALUES('originals','Original widgets','','wlb_originals','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','Базовые виджеты','','Базові віджети','');
CREATE TABLE 'WebUser_uPg' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '' ,"PROG" TEXT DEFAULT '' ,"uk#PROG" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"ru#PROG" TEXT DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO "WebUser_uPg" VALUES('test','','','','',1,'JavaLikeCalc.JavaScript
SYS.messDebug("TESTWWW","HTTPreq: "+HTTPreq+". URL: "+url+". Sender: "+sender+". User: "+user);
page = "TEST 00";','JavaLikeCalc.JavaScript
SYS.messDebug("TESTWWW","HTTPreq: "+HTTPreq+". URL: "+url+". Sender: "+sender+". User: "+user);
SYS.messDebug("TESTWWW","User-Agent: "+HTTPvars["User-Agent"]);
SYS.messDebug("TESTWWW","Page: "+page);
page = "TEST 00";','','','');
INSERT INTO "WebUser_uPg" VALUES('FlowTec.txt','','','','',1,'JavaLikeCalc.JavaScript
SYS.messDebug("TESTWWW","HTTPreq: "+HTTPreq+". URL: "+url+". Sender: "+sender+". User: "+user);
SYS.messDebug("TESTWWW","User-Agent: "+HTTPvars["User-Agent"]);
SYS.messDebug("TESTWWW","Page: "+page);
page = "TEST 00";','JavaLikeCalc.JavaScript
//SYS.messDebug("TESTWWW","HTTPreq: "+HTTPreq+". URL: "+url+". Sender: "+sender+". User: "+user);
//for( var i in HTTPvars ) SYS.messDebug("test","Prop ''"+i+"'' = "+HTTPvars[i]);
if( cnts.length )
{
  //SYS.messDebug("TESTWWW","Content-Disposition:  "+cnts[0].attr("Content-Disposition"));
  //SYS.messDebug("TESTWWW","Content-Type:  "+cnts[0].attr("Content-Type"));
  dataBlk = cnts[0].text();
  if( dataBlk.length )
  {
    SYS.messDebug("TESTWWW","Text: "+dataBlk);
    SYS.messDebug("TESTWWW","Object: ''"+dataBlk.parse(0,"\r\n")+"''");
    for( i = 2; (sc=dataBlk.parse(i,"\r\n")).length; i++ )
    {
      if( sc.slice(0,1) == "v" ) SYS.messDebug("TESTWWW","Calc: ''"+sc+"''");
      else if( sc.slice(0,2) == " n" )
      {
        nd = dataBlk.parse(++i,"\r\n");
        SYS.messDebug("TESTWWW",
		"Node: ''"+sc.slice(1)+"'': "+nd.parse(0," ",2,true)+" "+nd.parse(1," ",2,true)+" = "+nd.parse(3," ",2,true));
        SYS.DAQ.LogicLev.experiment.F3.ed.set(nd.parse(3," ",2,true));
      }
    }
  }
}
//SYS.messDebug("TESTWWW","Page: "+page);
page = "TEST 00";','','','JavaLikeCalc.JavaScript
//SYS.messDebug("TESTWWW","HTTPreq: "+HTTPreq+". URL: "+url+". Sender: "+sender+". User: "+user);
//for( var i in HTTPvars ) SYS.messDebug("test","Prop ''"+i+"'' = "+HTTPvars[i]);
if( cnts.length )
{
  //SYS.messDebug("TESTWWW","Content-Disposition:  "+cnts[0].attr("Content-Disposition"));
  //SYS.messDebug("TESTWWW","Content-Type:  "+cnts[0].attr("Content-Type"));
  dataBlk = cnts[0].text();
  if( dataBlk.length )
  {
    SYS.messDebug("TESTWWW","Text: "+dataBlk);
    SYS.messDebug("TESTWWW","Object: ''"+dataBlk.parse(0,"\r\n")+"''");
    for( i = 2; (sc=dataBlk.parse(i,"\r\n")).length; i++ )
    {
      if( sc.slice(0,1) == "v" ) SYS.messDebug("TESTWWW","Calc: ''"+sc+"''");
      else if( sc.slice(0,2) == " n" )
      {
        nd = dataBlk.parse(++i,"\r\n");
        SYS.messDebug("TESTWWW",
		"Node: ''"+sc.slice(1)+"'': "+nd.parse(0," ",2,true)+" "+nd.parse(1," ",2,true)+" = "+nd.parse(3," ",2,true));
        SYS.DAQ.LogicLev.experiment.F3.ed.set(nd.parse(3," ",2,true));
      }
    }
  }
}
//SYS.messDebug("TESTWWW","Page: "+page);
page = "TEST 00";');
CREATE TABLE 'testCalc_prm' ("SHIFR" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '' ,"FLD" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' , PRIMARY KEY ("SHIFR"));
INSERT INTO "testCalc_prm" VALUES('test','','',1,'out:o:Выход','','','','');
CREATE TABLE 'testCalc_val' ("ID" TEXT DEFAULT '' ,"VAL" TEXT DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO "testCalc_val" VALUES('out','50');
INSERT INTO "testCalc_val" VALUES('test','1');
INSERT INTO "testCalc_val" VALUES('offset','100');
CREATE TABLE 'wlb_originals' ("ID" TEXT DEFAULT '' ,"ICO" TEXT DEFAULT '' ,"PARENT" TEXT DEFAULT '' ,"PROC" TEXT DEFAULT '' ,"PROC_PER" INTEGER DEFAULT '' ,"USER" TEXT DEFAULT '' ,"GRP" TEXT DEFAULT '' ,"PERMIT" INTEGER DEFAULT '' ,"ATTRS" TEXT DEFAULT '' ,"ru#PROC" TEXT DEFAULT '' ,"DBV" INTEGER DEFAULT '' ,"uk#PROC" TEXT DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO "wlb_originals" VALUES('ElFigure','','','
',-1,'root','UI',436,'geomW;geomH;','',2,'');
INSERT INTO "wlb_originals" VALUES('Box','','','
',-1,'root','UI',436,'','',2,'');
INSERT INTO "wlb_originals" VALUES('FormEl','','','
',-1,'root','UI',436,'geomW;geomH;','',2,'');
INSERT INTO "wlb_originals" VALUES('Text','','','
',-1,'root','UI',436,'','
',2,'');
INSERT INTO "wlb_originals" VALUES('Document','','','
',-1,'root','UI',436,'','',2,'');
INSERT INTO "wlb_originals" VALUES('Protocol','','','',-1,'root','UI',436,'active;itProp;','',2,'');
INSERT INTO "wlb_originals" VALUES('Media','','','
',-1,'root','UI',436,'','',2,'');
INSERT INTO "wlb_originals" VALUES('Diagram','','','
',-1,'root','UI',436,'','',2,'');
CREATE TABLE 'wlb_originals_io' ("IDW" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"IO_VAL" TEXT DEFAULT '' ,"SELF_FLG" INTEGER DEFAULT '' ,"CFG_TMPL" TEXT DEFAULT '' ,"CFG_VAL" TEXT DEFAULT '' ,"IDC" TEXT DEFAULT '' ,"ru#IO_VAL" TEXT DEFAULT '' ,"ru#CFG_TMPL" TEXT DEFAULT '' ,"ru#CFG_VAL" TEXT DEFAULT '' ,"uk#IO_VAL" TEXT DEFAULT '' ,"uk#CFG_TMPL" TEXT DEFAULT '' ,"uk#CFG_VAL" TEXT DEFAULT '' , PRIMARY KEY ("IDW","ID","IDC"));
INSERT INTO "wlb_originals_io" VALUES('ElFigure','active','0',0,'','','','','','','','','');
INSERT INTO "wlb_originals_io" VALUES('ElFigure','geomW','79',0,'','','','','','','','','');
INSERT INTO "wlb_originals_io" VALUES('ElFigure','geomH','70',0,'','','','','','','','','');
INSERT INTO "wlb_originals_io" VALUES('ElFigure','elLst','',0,'','','','','','','','','');
INSERT INTO "wlb_originals_io" VALUES('Box','active','0',0,'','','','','','','','','');
INSERT INTO "wlb_originals_io" VALUES('FormEl','active','0',0,'','','','','','','','','');
INSERT INTO "wlb_originals_io" VALUES('FormEl','geomW','136',0,'','','','','','','','','');
INSERT INTO "wlb_originals_io" VALUES('FormEl','geomH','41',0,'','','','','','','','','');
INSERT INTO "wlb_originals_io" VALUES('FormEl','elType','0',0,'','','','','','','','','');
INSERT INTO "wlb_originals_io" VALUES('FormEl','view','0',0,'','','','','','','','','');
INSERT INTO "wlb_originals_io" VALUES('Text','active','0',0,'','','','','','','','','');
INSERT INTO "wlb_originals_io" VALUES('Text','geomW','100',0,'','','','','','','','','');
INSERT INTO "wlb_originals_io" VALUES('Text','geomH','50',0,'','','','','','','','','');
INSERT INTO "wlb_originals_io" VALUES('Text','alignment','10',0,'','','','','','','','','');
INSERT INTO "wlb_originals_io" VALUES('Text','text','Text field',0,'','','','Text field','','','Text field','','');
INSERT INTO "wlb_originals_io" VALUES('Text','numbArg','0',0,'','','','','','','','','');
INSERT INTO "wlb_originals_io" VALUES('Document','active','0',0,'','','','','','','','','');
INSERT INTO "wlb_originals_io" VALUES('Protocol','active','0',0,'','','','','','','','','');
INSERT INTO "wlb_originals_io" VALUES('Protocol','itProp','0',0,'','','','','','','','','');
INSERT INTO "wlb_originals_io" VALUES('Media','active','0',0,'','','','','','','','','');
INSERT INTO "wlb_originals_io" VALUES('Media','type','0',0,'','','','','','','','','');
INSERT INTO "wlb_originals_io" VALUES('Media','areas','0',0,'','','','','','','','','');
INSERT INTO "wlb_originals_io" VALUES('Diagram','active','0',0,'','','','','','','','','');
INSERT INTO "wlb_originals_io" VALUES('Diagram','type','0',0,'','','','','','','','','');
INSERT INTO "wlb_originals_io" VALUES('Diagram','parNum','1',0,'','','','','','','','','');
INSERT INTO "wlb_originals_io" VALUES('Document','time','0',0,'','','','','','','','','');
INSERT INTO "wlb_originals_io" VALUES('Document','n','0',0,'','','','','','','','','');
CREATE TABLE 'FSArch_Pack' ("FILE" TEXT DEFAULT '' ,"BEGIN" TEXT DEFAULT '' ,"END" TEXT DEFAULT '' ,"PRM1" TEXT DEFAULT '' ,"PRM2" TEXT DEFAULT '' ,"PRM3" TEXT DEFAULT '' , PRIMARY KEY ("FILE"));
CREATE TABLE 'OPC_UA_Prm_test' ("SHIFR" TEXT DEFAULT '' ,"OWNER" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '0' ,"ND_LS" TEXT DEFAULT '' , PRIMARY KEY ("SHIFR","OWNER"));
INSERT INTO "OPC_UA_Prm_test" VALUES('test','','','','','','','',1,'2:"DAQ.System.AutoDA.CPULoad.load"
2:"DAQ.System.AutoDA.MemInfo.use"
2:"DAQ.System.AutoDA.MemInfo.free"');
CREATE TABLE 'ModBusPrm_testTCP' ("SHIFR" TEXT DEFAULT '' ,"OWNER" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '0' ,"ATTR_LS" TEXT DEFAULT '' ,"ru#ATTR_LS" TEXT DEFAULT '' ,"uk#ATTR_LS" TEXT DEFAULT '' , PRIMARY KEY ("SHIFR","OWNER"));
INSERT INTO "ModBusPrm_testTCP" VALUES('test','','','','','','','',1,'R:0:r:reg0:Register 0
R:1:r:reg1:Register 1
R:3:rw:reg3:Register 3
R:4:r:reg4:Register 4
C:3:rw:coil3:Bit
C:8:rw:coil8:Bit','R:0:r:reg0:Регистр 0
R:1:r:reg1:Регистр 1
R:3:rw:reg3:Регистр 3
R:4:r:reg4:Регистр 4
C:3:rw:coil3:Бит
C:8:rw:coil8:Бит','R:0:r:reg0:Регістр 0
R:1:r:reg1:Регістр 1
R:3:rw:reg3:Регістр 3
R:4:r:reg4:Регістр 4
C:3:rw:coil3:Біт
C:8:rw:coil8:Біт');
INSERT INTO "ModBusPrm_testTCP" VALUES('test1','test','','','','','','',1,'R_i4:6:rw:i6:Integer32 [6,7]
R_u4:6:r:u6:Integer32 (unsigned) [6,7]
R_i8:10:r:i10:Integer64 [10-13]
R_f:8:rw:f8:Float [8,9]
R_d:14:rw:d14:Double [14-17]
R_d:18:rw:d18:Double [18-21]','','');
CREATE TABLE 'LogLevPrm_experiment' ("SHIFR" TEXT DEFAULT '' ,"OWNER" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '0' ,"PRM" TEXT DEFAULT '' , PRIMARY KEY ("SHIFR","OWNER"));
INSERT INTO "LogLevPrm_experiment" VALUES('F3','','F3','F3','F3','The gas flow through a pipe to Glinsk','Расход газа через трубу на Глинск','Витрати газу через трубу на Глінск',1,'base.anUnif');
INSERT INTO "LogLevPrm_experiment" VALUES('F4','','F4','F4','F4','Gas flow through the diaphragm PP5','Расход газа через диафрагму PP5','Витрати газу через діафрагму PP5',1,'base.anUnif');
INSERT INTO "LogLevPrm_experiment" VALUES('T_PP1','','T_PP1','T PP1','T_PP1','The gas temperature at the diaphragm PP1','Температура газа на диафрагме PP1','Температура газу на діафрагмі PP1',1,'base.anUnif');
INSERT INTO "LogLevPrm_experiment" VALUES('T_PP3','','T_PP3','T PP3','T_PP3','The gas temperature at the diaphragm PP3','Температура газа на диафрагме PP3','Температура газу на діафрагмі PP3',1,'base.anUnif');
INSERT INTO "LogLevPrm_experiment" VALUES('T_PP5','','T_PP5','T PP5','T_PP5','The gas temperature at the diaphragm PP5','Температура газа на диафрагме PP5','Температура газу на діафрагмі PP5',1,'base.anUnif');
INSERT INTO "LogLevPrm_experiment" VALUES('P_PP1','','P_PP1','P PP1','P_PP1','Gas pressure at the diaphragm PP1','Давление газа на диафрагме PP1','Тиск газу на діафрагмі PP1',1,'base.anUnif');
INSERT INTO "LogLevPrm_experiment" VALUES('P_PP3','','P_PP3','P PP3','P_PP3','Gas pressure at the diaphragm PP3','Давление газа на диафрагме PP3','Тиск газу на діафрагмі PP3',1,'base.anUnif');
INSERT INTO "LogLevPrm_experiment" VALUES('P_PP5','','P_PP5','P PP5','P_PP5','Gas pressure at the diaphragm PP5','Давление газа на диафрагме PP5 ','Тиск газу на діафрагмі PP5',1,'base.anUnif');
INSERT INTO "LogLevPrm_experiment" VALUES('КШ6close','','Close KSH6','Закрытие КШ6','Закриття КШ6','Alarm for closing the valve KSH6','Сигнализация по закрытию крана КШ6','Сигналізація по закриттю крана КШ6',1,'base.digAlarm');
INSERT INTO "LogLevPrm_experiment" VALUES('P3','','P3','P3','P3','The gas pressure in the pipe to Glinsk','Давление газа в трубе на Глинск','Тиск газу у трубі на Глінск',1,'base.anUnif');
INSERT INTO "LogLevPrm_experiment" VALUES('P4','','P4','P4','P4','Pressure P4','Давление газа на диафрагме PP5','Тиск газу на діафрагмі PP5',1,'base.anUnif');
INSERT INTO "LogLevPrm_experiment" VALUES('Pi','','Pi','Pi','Pi','The gas pressure at the input of the CS.','Давление газа на входе КС.','Тиск газу на вході КС.',1,'base.anUnif');
INSERT INTO "LogLevPrm_experiment" VALUES('Ti','','Ti','Ti','Ti','The gas temperature at the input of the CS.','Температура газа на входе КС.','Температура газу на вході КС.',1,'base.anUnif');
INSERT INTO "LogLevPrm_experiment" VALUES('PT0503','','PT0503','PT0503','PT0503','The gas pressure in the separator С1',' ','Тиск газу у сепараторі С1',1,'base.anUnif');
INSERT INTO "LogLevPrm_experiment" VALUES('КШ7','','KSH7','Кран КШ7','КШ7','The control block of the ball valve KSH7','Блок управления шарового крана КШ7','Блок керування шарового крану КШ7',1,'base.digitBlock');
INSERT INTO "LogLevPrm_experiment" VALUES('F_PP1','','F_PP1','F PP1','F_PP1','Gas flow through the diaphragm PP1','Расход газа через диафрагму PP1','Витрати газу через діафрагму PP1',1,'base.anUnif');
INSERT INTO "LogLevPrm_experiment" VALUES('F_PP3','','F_PP3','F PP3','F PP3','Gas flow through the diaphragm PP3','Расход газа через диафрагму PP3','Витрати газу через діафрагму PP3',1,'base.anUnif');
INSERT INTO "LogLevPrm_experiment" VALUES('F_PP5','','F_PP5','F PP5','F_PP5','Gas flow through the diaphragm PP5',' ','Витрати газу через діафрагму PP5',1,'base.anUnif');
INSERT INTO "LogLevPrm_experiment" VALUES('gN1','','Account node 1','Узел учёта 1','Вузол обліку 1',' ',' ',' ',1,'base.gasPoint');
CREATE TABLE 'DAQ_OPC_UA' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ENABLE" INTEGER DEFAULT '0' ,"START" INTEGER DEFAULT '0' ,"MESS_LEV" INTEGER DEFAULT '3' ,"REDNT" INTEGER DEFAULT '0' ,"REDNT_RUN" TEXT DEFAULT '<high>' ,"PRM_BD" TEXT DEFAULT '' ,"SCHEDULE" TEXT DEFAULT '1' ,"PRIOR" INTEGER DEFAULT '0' ,"SYNCPER" DOUBLE DEFAULT '60' ,"EndPoint" TEXT DEFAULT 'opc.tcp://localhost' ,"SecPolicy" TEXT DEFAULT 'None' ,"SecMessMode" INTEGER DEFAULT '1' ,"Cert" TEXT DEFAULT '' ,"PvKey" TEXT DEFAULT '' ,"AuthUser" TEXT DEFAULT '' ,"AuthPass" TEXT DEFAULT '' ,"AttrsLimit" INTEGER DEFAULT '100' , PRIMARY KEY ("ID"));
INSERT INTO "DAQ_OPC_UA" VALUES('test','','','','','','',1,1,3,0,'<high>','OPC_UA_Prm_test','1',0,60.0,'opc.tcp://127.0.0.1:4841/OSCADA_OPC/None','None',1,'-----BEGIN CERTIFICATE-----
MIIEhDCCA2ygAwIBAgIJAJtukvCfqH7fMA0GCSqGSIb3DQEBBQUAMIGhMQswCQYD
VQQGEwJVQTEXMBUGA1UECAwORG5lcHJvcGV0cm92c2sxGTAXBgNVBAcMEERuZXBy
b2R6ZXJ6aGluc2sxFzAVBgNVBAoMDk9wZW5TQ0FEQSBUZWFtMQswCQYDVQQLDAJS
TTEWMBQGA1UEAwwNT1BDIFVBIENsaWVudDEgMB4GCSqGSIb3DQEJARYRb3NjYWRh
QG9zY2FkYS5vcmcwHhcNMTMxMjIxMTkxMjAzWhcNMjMxMjE5MTkxMjAzWjCBoTEL
MAkGA1UEBhMCVUExFzAVBgNVBAgMDkRuZXByb3BldHJvdnNrMRkwFwYDVQQHDBBE
bmVwcm9kemVyemhpbnNrMRcwFQYDVQQKDA5PcGVuU0NBREEgVGVhbTELMAkGA1UE
CwwCUk0xFjAUBgNVBAMMDU9QQyBVQSBDbGllbnQxIDAeBgkqhkiG9w0BCQEWEW9z
Y2FkYUBvc2NhZGEub3JnMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA
6ERqpUnSls+HCjY69B7+OBhIZyuINs5LIABkBf83F2K/3nZTfUg4D/3IDq7j9Wvk
nJ2pgrkXMGSGDaXFr49l8belJAoSK137vD9cgxrMa8iuF5JQ1baFCdyZo49pFgFP
Y7wUhpcjT4o49PSyI5CDAP8M0zzfPpeQhY/Afy3OROJ1gcUSyCKMiT8GbWtNhFvl
qiDv1qQzfUDPR3TLcbFa+e5BwzLsHt1nXxDX8Hvm1+1/kCqMJaxMoWGMt8b/8SDy
lwDdX7nQvImDJXsoypcZOCW3c4QhWHuVLpItiB80rhE75reC6U0MU2FOzoCDuI/r
VUzzvF8EQ73bJH314aqsxwIDAQABo4G8MIG5MB0GA1UdDgQWBBQkYspqSoMp1UP1
zhSvSw3R0UCoCTAfBgNVHSMEGDAWgBQkYspqSoMp1UP1zhSvSw3R0UCoCTAMBgNV
HRMEBTADAQH/MA4GA1UdDwEB/wQEAwIC9DAgBgNVHSUBAf8EFjAUBggrBgEFBQcD
AQYIKwYBBQUHAwIwNwYDVR0RBDAwLoYacm9tYW4vT3BlblNDQURBL0RBUS5PUENf
VUGCCm9zY2FkYS5vcmeHBFLPWEkwDQYJKoZIhvcNAQEFBQADggEBAFKw+Y03Lu26
3qDYFbpUxXZSb5PgVyH0pnAK+dWQppkOChWl5sdhLiAgPZRIwZoykC6XMSv/cOki
QQ+zLiI9ikgCj0E/qxUoaLNlr6j6hFK4gEgGGeVIe9/GtBiNwYfYO/Dt+qjCz4GG
nIVXIlYhbTc+/cnAzYhsLT5PLlw6Ji01ynCxZ9kTDzB0G1CN/glLIEgm8caE9BSV
SyXG8RJ9JQ0FwIJa8gh3HXkUWnE7IzRausvxe7lGr39zR6BrMp0EHjDPIKBLRQQU
bEzTeUgViycX9CtrJyFLGMdGR49hYLvX4FEd+W7CFyVrFcHWag4AQ7+thB2H6tIf
26AyCHTK1zc=
-----END CERTIFICATE-----','-----BEGIN RSA PRIVATE KEY-----
MIIEowIBAAKCAQEA6ERqpUnSls+HCjY69B7+OBhIZyuINs5LIABkBf83F2K/3nZT
fUg4D/3IDq7j9WvknJ2pgrkXMGSGDaXFr49l8belJAoSK137vD9cgxrMa8iuF5JQ
1baFCdyZo49pFgFPY7wUhpcjT4o49PSyI5CDAP8M0zzfPpeQhY/Afy3OROJ1gcUS
yCKMiT8GbWtNhFvlqiDv1qQzfUDPR3TLcbFa+e5BwzLsHt1nXxDX8Hvm1+1/kCqM
JaxMoWGMt8b/8SDylwDdX7nQvImDJXsoypcZOCW3c4QhWHuVLpItiB80rhE75reC
6U0MU2FOzoCDuI/rVUzzvF8EQ73bJH314aqsxwIDAQABAoIBADY5Yrxb07IpmNkl
d/XuFmN6J80OOKqy1o8OCEvuHSp0f16sOSmSZqQ1FzN2WPm+yoxVsnE2TNebYEBn
ZxNpYt9fhEo5n6xIMLDuCecBpp+zJhGSsxRxm/WFbsjkfiAJBkO+cPxK5I95MXre
dYggALWG1bOrDmNKdw73VHVceD9UPUc+JsYeWI2tyk/X6f5zDlhRLScmMvS/KH8K
mbfsvyOtfsttFN0sKY9nqxPyIyzD0ilbPfaOU2Xh92WBkHVFW4KZcWwgiSIoBgNE
AmFfMhziQkcg5a05fQ/GOV+Z0LPOi/YwluogN3HY8ljT9/Rp/ZHRQbPmy1VPsFHO
MLEmFQECgYEA9O2mc2Z2cbTlWt0gCwijYxVGYBYNW3Sp+D/8+cp1DkA96aN6cR3u
JzSXhyz21YtSU2lNQxolRCuqbwu+4O+EeviEGxVl6us6nzxILnV4V9ZxUOWV2QrL
1+UMqt3GaUTuQ62TbAJAlUVNJ8hp8fmMBOvxPG6TXG0HqrO2h5thhqcCgYEA8sRA
Hqyc7zEj7VOesDSxRMAY6fdHxD2Pl0bdM8tnA/enVaHUuCNip+lmMw3bSSK51+/C
Ux7/q/YxxhJqZ9fzmDa7mqSDmKvgQVq+BuRR5afFrpzem5fseMxx4ZsZz2Y0TtDb
ln0u1v7QBDEEazBtAsrTQfWklRjaXrrTkeQwjOECgYACawI0q3NJ5hZJkltJoJk0
VjZupmkVWQ6Ag+k0cho7vMifsNx5l2kNfAhu7jO1of17E1QO8Igjpya0l/IXeGXf
bOqo9coeypm2jrrLsp8whs5xlQKotOGUTQ+V9ECMM4ECrAkDjPYkQx3i4aiiGeuW
9NlZ1mGgDPeBaOrJZfBNiwKBgQCRH0IZVCSIVD7e23LscMNbjt4V1u2+4PIag5Eu
KjSNyZjtds1bukN6juq6MFlbEfg4nqny2IAMQyMj/dQ/+Xc0Edv6KMhGFtD7IIyi
QBC5EZ75r+re5mXFHKojLH20Xqt8RhxOjMzYVwDw0ICMtykjVy4MqxRU0MORh2t8
NgXT4QKBgAr4X53AUj3mkj3S150J0XF7BziGjccZLXhghU57Q3CzSoWOjSleVapM
ZlyAHJIdqoYXFmzWMAAdRMkPoHq+mvLdYFdUqxIqlAVjB0Lz8t8WCUgkmxny8CEs
xZewE0A10K9LnTRVENQPBRL/Uay9hUmpyLaL/gV0AEHvvQfEErDD
-----END RSA PRIVATE KEY-----','','',100);
CREATE TABLE 'ModBusPrm_testRTU' ("SHIFR" TEXT DEFAULT '' ,"OWNER" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '0' ,"ATTR_LS" TEXT DEFAULT '' ,"uk#ATTR_LS" TEXT DEFAULT '' ,"ru#ATTR_LS" TEXT DEFAULT '' , PRIMARY KEY ("SHIFR","OWNER"));
INSERT INTO "ModBusPrm_testRTU" VALUES('test','','','','','','','',1,'R:0:r:reg0:Register 0
R:1:r:reg1:Register 1
R:3:rw:reg3:Register 3
R:4:r:reg4:Register 4
C:3:r:coil3:Bit
R_f:0,3:rw:float:Float
R_i4:0,3:rw:int:Int
R_b1:0:rw:rebBit:rebBit
R_i2:0:rw:int16:Int16','R:0:r:reg0:Регістр 0
R:1:r:reg1:Регістр 1
R:3:rw:reg3:Регістр 3
R:4:r:reg4:Регістр 4
C:3:r:coil3:Біт
R_f:0,3:rw:float:Float
R_i4:0,3:rw:int:Int
R_b1:0:rw:rebBit:rebBit
R_i2:0:rw:int16:Int16','R:0:r:reg0:Регистр 0
R:1:r:reg1:Регистр 1
R:3:rw:reg3:Регистр 3
R:4:r:reg4:Регистр 4
C:3:r:coil3:Бит
R_f:0,3:rw:float:Float
R_i4:0,3:rw:int:Int
R_b1:0:rw:rebBit:rebBit
R_i2:0:rw:int16:Int16');
CREATE TABLE 'DAQ_ModBus' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"ENABLE" INTEGER DEFAULT '0' ,"START" INTEGER DEFAULT '0' ,"MESS_LEV" INTEGER DEFAULT '3' ,"REDNT" INTEGER DEFAULT '0' ,"REDNT_RUN" TEXT DEFAULT '<high>' ,"PRM_BD" TEXT DEFAULT '' ,"PRM_BD_L" TEXT DEFAULT '' ,"SCHEDULE" TEXT DEFAULT '1' ,"PRIOR" INTEGER DEFAULT '0' ,"PROT" TEXT DEFAULT 'TCP' ,"ADDR" TEXT DEFAULT '' ,"NODE" INTEGER DEFAULT '1' ,"FRAG_MERGE" INTEGER DEFAULT '0' ,"WR_MULTI" INTEGER DEFAULT '0' ,"WR_ASYNCH" INTEGER DEFAULT '0' ,"TM_REQ" INTEGER DEFAULT '0' ,"TM_REST" INTEGER DEFAULT '30' ,"REQ_TRY" INTEGER DEFAULT '1' ,"MAX_BLKSZ" INTEGER DEFAULT '200' , PRIMARY KEY ("ID"));
INSERT INTO "DAQ_ModBus" VALUES('testRTU','','','','','','',1,0,3,0,'<high>','ModBusPrm_testRTU','','1',0,'RTU','Serial.testModBus',1,1,0,0,0,30,1,200);
INSERT INTO "DAQ_ModBus" VALUES('testTCP','','','','','','',1,1,3,0,'<high>','ModBusPrm_testTCP','','1',0,'TCP','Sockets.testModBus',1,1,0,0,0,30,3,200);
CREATE TABLE 'DAQ_DAQGate' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ENABLE" INTEGER DEFAULT '0' ,"START" INTEGER DEFAULT '0' ,"MESS_LEV" INTEGER DEFAULT '3' ,"REDNT" INTEGER DEFAULT '0' ,"REDNT_RUN" TEXT DEFAULT '<high>' ,"PRM_BD" TEXT DEFAULT '' ,"PERIOD" INTEGER DEFAULT '0' ,"SCHEDULE" TEXT DEFAULT '1' ,"PRIOR" INTEGER DEFAULT '0' ,"TM_REST" INTEGER DEFAULT '30' ,"TM_REST_DT" DOUBLE DEFAULT '1' ,"GATH_MESS_LEV" INTEGER DEFAULT '1' ,"SYNCPER" DOUBLE DEFAULT '0' ,"STATIONS" TEXT DEFAULT '' ,"CNTRPRM" TEXT DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO "DAQ_DAQGate" VALUES('test','Test','Тест','Test',' ',' ',' ',1,1,3,0,'<high>','DAQGatePrm_test',0,'1',0,30,1.0,1,60.0,'loop','System.AutoDA
ModBus.testTCP.test');
CREATE TABLE 'DAQGatePrm_test' ("SHIFR" TEXT DEFAULT '' ,"OWNER" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '0' ,"PRM_ADDR" TEXT DEFAULT '' ,"ATTRS" TEXT DEFAULT '' , PRIMARY KEY ("SHIFR","OWNER"));
INSERT INTO "DAQGatePrm_test" VALUES('CPU0Load','','CPU Load :0','Нагрузка процессора: 0','Навантаження CPU: 0','','','',1,'System/AutoDA/prm_CPU0Load','<Attrs>
<a id="SHIFR" nm="ID" tp="5" flg="772" />
<a id="OWNER" nm="Owner" tp="5" flg="772" />
<a id="NAME" nm="Name" tp="5" flg="768" />
<a id="DESCR" nm="Description" tp="5" flg="776" />
<a id="load" nm="Load (%)" tp="4" flg="772" />
<a id="sys" nm="System (%)" tp="4" flg="772" />
<a id="user" nm="User (%)" tp="4" flg="772" />
<a id="idle" nm="Idle (%)" tp="4" flg="772" />
</Attrs>
');
INSERT INTO "DAQGatePrm_test" VALUES('CPU1Load','','CPU Load :1','Нагрузка процессора: 1','Навантаження CPU: 1','','','',1,'System/AutoDA/prm_CPU1Load','<Attrs>
<a id="SHIFR" nm="ID" tp="5" flg="772" />
<a id="OWNER" nm="Owner" tp="5" flg="772" />
<a id="NAME" nm="Name" tp="5" flg="768" />
<a id="DESCR" nm="Description" tp="5" flg="776" />
<a id="load" nm="Load (%)" tp="4" flg="772" />
<a id="sys" nm="System (%)" tp="4" flg="772" />
<a id="user" nm="User (%)" tp="4" flg="772" />
<a id="idle" nm="Idle (%)" tp="4" flg="772" />
</Attrs>
');
INSERT INTO "DAQGatePrm_test" VALUES('CPULoad','','Full CPU Load','Общая нагрузка процессора','Повне навантаження CPU','','','',1,'System/AutoDA/prm_CPULoad','<Attrs>
<a id="SHIFR" nm="ID" tp="5" flg="772" />
<a id="OWNER" nm="Owner" tp="5" flg="772" />
<a id="NAME" nm="Name" tp="5" flg="768" />
<a id="DESCR" nm="Description" tp="5" flg="776" />
<a id="load" nm="Load (%)" tp="4" flg="772" />
<a id="sys" nm="System (%)" tp="4" flg="772" />
<a id="user" nm="User (%)" tp="4" flg="772" />
<a id="idle" nm="Idle (%)" tp="4" flg="772" />
</Attrs>
');
INSERT INTO "DAQGatePrm_test" VALUES('Interface_eth0','','Interface statistic: eth0','Статистика интерфейсов: eth0','Статистика інтерфейсу: eth0','','','',1,'System/AutoDA/prm_Interface_eth0','<Attrs>
<a id="SHIFR" nm="ID" tp="5" flg="772" />
<a id="OWNER" nm="Owner" tp="5" flg="772" />
<a id="NAME" nm="Name" tp="5" flg="768" />
<a id="DESCR" nm="Description" tp="5" flg="776" />
<a id="rcv" nm="Receive (B)" tp="4" flg="772" />
<a id="rcvSp" nm="Receive speed (B/s)" tp="4" flg="772" />
<a id="trns" nm="Transmit (B)" tp="4" flg="772" />
<a id="trnsSp" nm="Transmit speed (B/s)" tp="4" flg="772" />
</Attrs>
');
INSERT INTO "DAQGatePrm_test" VALUES('Interface_lo','','Interface statistic: lo','Статистика интерфейсов: lo','Статистика інтерфейсу: lo','','','',1,'System/AutoDA/prm_Interface_lo','<Attrs>
<a id="SHIFR" nm="ID" tp="5" flg="772" />
<a id="OWNER" nm="Owner" tp="5" flg="772" />
<a id="NAME" nm="Name" tp="5" flg="768" />
<a id="DESCR" nm="Description" tp="5" flg="776" />
<a id="rcv" nm="Receive (B)" tp="4" flg="772" />
<a id="rcvSp" nm="Receive speed (B/s)" tp="4" flg="772" />
<a id="trns" nm="Transmit (B)" tp="4" flg="772" />
<a id="trnsSp" nm="Transmit speed (B/s)" tp="4" flg="772" />
</Attrs>
');
INSERT INTO "DAQGatePrm_test" VALUES('Interface_wlan0','','Interface statistic: wlan0','Статистика интерфейсов: wlan0','Статистика інтерфейсу: wlan0','','','',1,'System/AutoDA/prm_Interface_wlan0','<Attrs>
<a id="SHIFR" nm="ID" tp="5" flg="772" />
<a id="OWNER" nm="Owner" tp="5" flg="772" />
<a id="NAME" nm="Name" tp="5" flg="768" />
<a id="DESCR" nm="Description" tp="5" flg="776" />
<a id="rcv" nm="Receive (B)" tp="4" flg="772" />
<a id="rcvSp" nm="Receive speed (B/s)" tp="4" flg="772" />
<a id="trns" nm="Transmit (B)" tp="4" flg="772" />
<a id="trnsSp" nm="Transmit speed (B/s)" tp="4" flg="772" />
</Attrs>
');
INSERT INTO "DAQGatePrm_test" VALUES('MemInfo','','Memory info','Информация про память','Інформація про пам''ять','','','',1,'System/AutoDA/prm_MemInfo','<Attrs>
<a id="SHIFR" nm="ID" tp="5" flg="772" />
<a id="OWNER" nm="Owner" tp="5" flg="772" />
<a id="NAME" nm="Name" tp="5" flg="768" />
<a id="DESCR" nm="Description" tp="5" flg="776" />
<a id="free" nm="Free (kB)" tp="1" flg="772" />
<a id="total" nm="Total (kB)" tp="1" flg="772" />
<a id="use" nm="Use (kB)" tp="1" flg="772" />
<a id="buff" nm="Buffers (kB)" tp="1" flg="772" />
<a id="cache" nm="Cached (kB)" tp="1" flg="772" />
<a id="sw_free" nm="Swap free (kB)" tp="1" flg="772" />
<a id="sw_total" nm="Swap total (kB)" tp="1" flg="772" />
<a id="sw_use" nm="Swap use (kB)" tp="1" flg="772" />
</Attrs>
');
INSERT INTO "DAQGatePrm_test" VALUES('SensorsData','','Data sensors','Данные сенсоров','Дані сенсорів','','','',1,'System/AutoDA/prm_SensorsData','<Attrs>
<a id="SHIFR" nm="ID" tp="5" flg="772" />
<a id="OWNER" nm="Owner" tp="5" flg="772" />
<a id="NAME" nm="Name" tp="5" flg="768" />
<a id="DESCR" nm="Description" tp="5" flg="776" />
<a id="thinkpad_fan1" nm="thinkpad fan1" tp="4" flg="772" />
<a id="thinkpad_temp1" nm="thinkpad temp1" tp="4" flg="772" />
<a id="thinkpad_temp2" nm="thinkpad temp2" tp="4" flg="772" />
<a id="thinkpad_temp3" nm="thinkpad temp3" tp="4" flg="772" />
<a id="thinkpad_temp4" nm="thinkpad temp4" tp="4" flg="772" />
<a id="thinkpad_temp5" nm="thinkpad temp5" tp="4" flg="772" />
<a id="thinkpad_temp6" nm="thinkpad temp6" tp="4" flg="772" />
<a id="thinkpad_temp7" nm="thinkpad temp7" tp="4" flg="772" />
<a id="thinkpad_temp8" nm="thinkpad temp8" tp="4" flg="772" />
<a id="k8temp_temp1" nm="k8temp temp1" tp="4" flg="772" />
<a id="k8temp_temp2" nm="k8temp temp2" tp="4" flg="772" />
<a id="k8temp_temp3" nm="k8temp temp3" tp="4" flg="772" />
<a id="k8temp_temp4" nm="k8temp temp4" tp="4" flg="772" />
</Attrs>
');
INSERT INTO "DAQGatePrm_test" VALUES('Statistic_sda','','HD statistic: sda','Статистика НЖМД: sda','Статистика НЖМД: sda','','','',1,'System/AutoDA/prm_Statistic_sda','<Attrs>
<a id="SHIFR" nm="ID" tp="5" flg="772" />
<a id="OWNER" nm="Owner" tp="5" flg="772" />
<a id="NAME" nm="Name" tp="5" flg="768" />
<a id="DESCR" nm="Description" tp="5" flg="776" />
<a id="rd" nm="Read (B)" tp="4" flg="772" />
<a id="rdSp" nm="Read speed (B/s)" tp="4" flg="772" />
<a id="wr" nm="Write (B)" tp="4" flg="772" />
<a id="wrSp" nm="Write speed (B/s)" tp="4" flg="772" />
</Attrs>
');
INSERT INTO "DAQGatePrm_test" VALUES('Statistic_sda1','','HD statistic: sda1','Статистика НЖМД: sda1','Статистика НЖМД: sda1','','','',1,'System/AutoDA/prm_Statistic_sda1','<Attrs>
<a id="SHIFR" nm="ID" tp="5" flg="772" />
<a id="OWNER" nm="Owner" tp="5" flg="772" />
<a id="NAME" nm="Name" tp="5" flg="768" />
<a id="DESCR" nm="Description" tp="5" flg="776" />
<a id="rd" nm="Read (B)" tp="4" flg="772" />
<a id="rdSp" nm="Read speed (B/s)" tp="4" flg="772" />
<a id="wr" nm="Write (B)" tp="4" flg="772" />
<a id="wrSp" nm="Write speed (B/s)" tp="4" flg="772" />
</Attrs>
');
INSERT INTO "DAQGatePrm_test" VALUES('Statistic_sda2','','HD statistic: sda2','Статистика НЖМД: sda2','Статистика НЖМД: sda2','','','',1,'System/AutoDA/prm_Statistic_sda2','<Attrs>
<a id="SHIFR" nm="ID" tp="5" flg="772" />
<a id="OWNER" nm="Owner" tp="5" flg="772" />
<a id="NAME" nm="Name" tp="5" flg="768" />
<a id="DESCR" nm="Description" tp="5" flg="776" />
<a id="rd" nm="Read (B)" tp="4" flg="772" />
<a id="rdSp" nm="Read speed (B/s)" tp="4" flg="772" />
<a id="wr" nm="Write (B)" tp="4" flg="772" />
<a id="wrSp" nm="Write speed (B/s)" tp="4" flg="772" />
</Attrs>
');
INSERT INTO "DAQGatePrm_test" VALUES('Statistic_sda5','','HD statistic: sda5','Статистика НЖМД: sda5','Статистика НЖМД: sda5','','','',1,'System/AutoDA/prm_Statistic_sda5','<Attrs>
<a id="SHIFR" nm="ID" tp="5" flg="772" />
<a id="OWNER" nm="Owner" tp="5" flg="772" />
<a id="NAME" nm="Name" tp="5" flg="768" />
<a id="DESCR" nm="Description" tp="5" flg="776" />
<a id="rd" nm="Read (B)" tp="4" flg="772" />
<a id="rdSp" nm="Read speed (B/s)" tp="4" flg="772" />
<a id="wr" nm="Write (B)" tp="4" flg="772" />
<a id="wrSp" nm="Write speed (B/s)" tp="4" flg="772" />
</Attrs>
');
INSERT INTO "DAQGatePrm_test" VALUES('Statistic_sda6','','HD statistic: sda6','Статистика НЖМД: sda6','Статистика НЖМД: sda6','','','',1,'System/AutoDA/prm_Statistic_sda6','<Attrs>
<a id="SHIFR" nm="ID" tp="5" flg="772" />
<a id="OWNER" nm="Owner" tp="5" flg="772" />
<a id="NAME" nm="Name" tp="5" flg="768" />
<a id="DESCR" nm="Description" tp="5" flg="776" />
<a id="rd" nm="Read (B)" tp="4" flg="772" />
<a id="rdSp" nm="Read speed (B/s)" tp="4" flg="772" />
<a id="wr" nm="Write (B)" tp="4" flg="772" />
<a id="wrSp" nm="Write speed (B/s)" tp="4" flg="772" />
</Attrs>
');
INSERT INTO "DAQGatePrm_test" VALUES('Statistic_sda7','','HD statistic: sda7','Статистика НЖМД: sda7','Статистика НЖМД: sda7','','','',1,'System/AutoDA/prm_Statistic_sda7','<Attrs>
<a id="SHIFR" nm="ID" tp="5" flg="772" />
<a id="OWNER" nm="Owner" tp="5" flg="772" />
<a id="NAME" nm="Name" tp="5" flg="768" />
<a id="DESCR" nm="Description" tp="5" flg="776" />
<a id="rd" nm="Read (B)" tp="4" flg="772" />
<a id="rdSp" nm="Read speed (B/s)" tp="4" flg="772" />
<a id="wr" nm="Write (B)" tp="4" flg="772" />
<a id="wrSp" nm="Write speed (B/s)" tp="4" flg="772" />
</Attrs>
');
INSERT INTO "DAQGatePrm_test" VALUES('Statistic_sda8','','HD statistic: sda8','Статистика НЖМД: sda8','Статистика НЖМД: sda8','','','',1,'System/AutoDA/prm_Statistic_sda8','<Attrs>
<a id="SHIFR" nm="ID" tp="5" flg="772" />
<a id="OWNER" nm="Owner" tp="5" flg="772" />
<a id="NAME" nm="Name" tp="5" flg="768" />
<a id="DESCR" nm="Description" tp="5" flg="776" />
<a id="rd" nm="Read (B)" tp="4" flg="772" />
<a id="rdSp" nm="Read speed (B/s)" tp="4" flg="772" />
<a id="wr" nm="Write (B)" tp="4" flg="772" />
<a id="wrSp" nm="Write speed (B/s)" tp="4" flg="772" />
</Attrs>
');
INSERT INTO "DAQGatePrm_test" VALUES('Statistic_sda9','','HD statistic: sda9','Статистика НЖМД: sda9','Статистика НЖМД: sda9','','','',1,'System/AutoDA/prm_Statistic_sda9','<Attrs>
<a id="SHIFR" nm="ID" tp="5" flg="772" />
<a id="OWNER" nm="Owner" tp="5" flg="772" />
<a id="NAME" nm="Name" tp="5" flg="768" />
<a id="DESCR" nm="Description" tp="5" flg="776" />
<a id="rd" nm="Read (B)" tp="4" flg="772" />
<a id="rdSp" nm="Read speed (B/s)" tp="4" flg="772" />
<a id="wr" nm="Write (B)" tp="4" flg="772" />
<a id="wrSp" nm="Write speed (B/s)" tp="4" flg="772" />
</Attrs>
');
INSERT INTO "DAQGatePrm_test" VALUES('UpTimeStation','','Station up time','Время работы станции','Час роботи станції','','','',1,'System/AutoDA/prm_UpTimeStation','<Attrs>
<a id="SHIFR" nm="ID" tp="5" flg="772" />
<a id="OWNER" nm="Owner" tp="5" flg="772" />
<a id="NAME" nm="Name" tp="5" flg="768" />
<a id="DESCR" nm="Description" tp="5" flg="776" />
<a id="full" nm="Full seconds" tp="1" flg="772" />
<a id="sec" nm="Seconds" tp="1" flg="772" />
<a id="min" nm="Minutes" tp="1" flg="772" />
<a id="hour" nm="Hours" tp="1" flg="772" />
<a id="day" nm="Days" tp="1" flg="772" />
</Attrs>
');
INSERT INTO "DAQGatePrm_test" VALUES('UpTimeSystem','','System up time','Время работы системы','Час роботи системи','','','',1,'System/AutoDA/prm_UpTimeSystem','<Attrs>
<a id="SHIFR" nm="ID" tp="5" flg="772" />
<a id="OWNER" nm="Owner" tp="5" flg="772" />
<a id="NAME" nm="Name" tp="5" flg="768" />
<a id="DESCR" nm="Description" tp="5" flg="776" />
<a id="full" nm="Full seconds" tp="1" flg="772" />
<a id="sec" nm="Seconds" tp="1" flg="772" />
<a id="min" nm="Minutes" tp="1" flg="772" />
<a id="hour" nm="Hours" tp="1" flg="772" />
<a id="day" nm="Days" tp="1" flg="772" />
</Attrs>
');
INSERT INTO "DAQGatePrm_test" VALUES('test','','','','','','','',1,'ModBus/testTCP/prm_test','<Attrs>
<a id="SHIFR" nm="ID" tp="5" flg="772" />
<a id="OWNER" nm="Owner" tp="5" flg="772" />
<a id="NAME" nm="Name" tp="5" flg="768" />
<a id="DESCR" nm="Description" tp="5" flg="776" />
<a id="reg0" nm="Register 0" tp="1" flg="772" />
<a id="reg1" nm="Register 1" tp="1" flg="772" />
<a id="reg3" nm="Register 3" tp="1" flg="768" />
<a id="reg4" nm="Register 4" tp="1" flg="772" />
<a id="coil3" nm="Bit" tp="0" flg="768" />
<a id="coil8" nm="Bit" tp="0" flg="768" />
</Attrs>
');
INSERT INTO "DAQGatePrm_test" VALUES('test1','test','','','','','','',1,'ModBus/testTCP/prm_test/prm_test1','<Attrs>
<a id="SHIFR" nm="ID" tp="5" flg="772" />
<a id="OWNER" nm="Owner" tp="5" flg="772" />
<a id="NAME" nm="Name" tp="5" flg="768" />
<a id="DESCR" nm="Description" tp="5" flg="776" />
<a id="i6" nm="Integer32 [6,7]" tp="1" flg="768" />
<a id="u6" nm="Integer32 (unsigned) [6,7]" tp="1" flg="772" />
<a id="i10" nm="Integer64 [10-13]" tp="1" flg="772" />
<a id="f8" nm="Float [8,9]" tp="4" flg="768" />
<a id="d14" nm="Double [14-17]" tp="4" flg="768" />
<a id="d18" nm="Double [18-21]" tp="4" flg="768" />
</Attrs>
');
COMMIT;
