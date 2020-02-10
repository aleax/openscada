PRAGMA foreign_keys=OFF;
BEGIN TRANSACTION;
CREATE TABLE IF NOT EXISTS 'VCALibs' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"DB_TBL" TEXT DEFAULT '' ,"ICO" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO VCALibs VALUES('AGLKS','AGLKS','Library of the interface items of the project "AGLKS".
Author: Roman Savochenko <roman@oscada.org>
Version: 2.0.0
','wlb_AGLKS','','АГЛКС','Бібліотека сторінок АГЛКС
Автор: Роман Савоченко <roman@oscada.org>
Версія: 2.0.0
','АГЛКС','Библиотека страниц АГЛКС
Автор: Роман Савоченко <roman@oscada.org>
Версия: 2.0.0
');
CREATE TABLE IF NOT EXISTS 'prj_AGLKS_incl' ("IDW" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"PARENT" TEXT DEFAULT '' ,"ATTRS" TEXT DEFAULT '' ,"DBV" INTEGER DEFAULT '' , PRIMARY KEY ("IDW","ID"));
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so','pgCont','/wlb_Main/wdg_RootPgSo/wdg_pgCont','geomY;geomH;geomZ;pgGrp;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so','infoW','/wlb_Main/wdg_RootPgSo/wdg_infoW','pgGrp;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph','trnd1','/wlb_Main/wdg_grpGraph/wdg_trnd1','curSek;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','el1','/prj_AGLKS/pg_so/pg_1/pg_ggraph/wdg_el1','name;dscr;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','el2','/prj_AGLKS/pg_so/pg_1/pg_ggraph/wdg_el2','name;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','el3','/prj_AGLKS/pg_so/pg_1/pg_ggraph/wdg_el3','name;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','el4','/prj_AGLKS/pg_so/pg_1/pg_ggraph/wdg_el4','name;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','el5','/prj_AGLKS/pg_so/pg_1/pg_ggraph/wdg_el5','name;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','el6','/prj_AGLKS/pg_so/pg_1/pg_ggraph/wdg_el6','name;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','el7','/prj_AGLKS/pg_so/pg_1/pg_ggraph/wdg_el7','name;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','el8','/prj_AGLKS/pg_so/pg_1/pg_ggraph/wdg_el8','name;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','trnd1','/prj_AGLKS/pg_so/pg_1/pg_ggraph/wdg_trnd1','curSek;sclWin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','el3','/prj_AGLKS/pg_so/pg_1/pg_ggraph/wdg_el3','name;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','el2','/prj_AGLKS/pg_so/pg_1/pg_ggraph/wdg_el2','name;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','el1','/prj_AGLKS/pg_so/pg_1/pg_ggraph/wdg_el1','name;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','tiPer','/wlb_Main/wdg_cntrRegul/wdg_tiPer','value;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_1','doc','/wlb_doc/wdg_docRepInstVals/wdg_doc','p1ed;p1name;p1prec;p1var;p2ed;p2max;p2min;p2name;p2prec;p2var;p3ed;p3max;p3min;p3name;p3prec;p3var;p4ed;p4max;p4min;p4name;p4prec;p4var;p5ed;p5max;p5min;p5name;p5prec;p5var;p6ed;p6max;p6min;p6name;p6prec;p6var;title;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','trnd','/wlb_Main/wdg_cntrRegul/wdg_trnd','curSek;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','kd','/wlb_Main/wdg_cntrRegul/wdg_kd','value;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_3','doc','/wlb_doc/wdg_docRepDay/wdg_doc','p1cntr;p1ed;p1max;p1min;p1name;p1prec;p1var;p2ed;p2max;p2min;p2name;p2prec;p2var;p3ed;p3max;p3min;p3name;p3prec;p3var;p4ed;p4max;p4min;p4name;p4prec;p4var;p5ed;p5max;p5min;p5name;p5prec;p5var;title;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','td','/wlb_Main/wdg_cntrRegul/wdg_td','value;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','kp','/wlb_Main/wdg_cntrRegul/wdg_kp','value;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_4','doc','/wlb_doc/wdg_docRepMonth/wdg_doc','p1cntr;p1ed;p1max;p1min;p1name;p1prec;p1var;p2ed;p2max;p2min;p2name;p2prec;p2var;p3ed;p3max;p3min;p3name;p3prec;p3var;p4ed;p4max;p4min;p4name;p4prec;p4var;p5ed;p5max;p5min;p5name;p5prec;p5var;title;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_5','doc','/wlb_test/wdg_docTestGasNodeDayRep/wdg_doc','cDPup;cDTr;cKL;cKsh;cKtup;cMoldN;cMolsCO2;cPatm;cPots;cPperekl;cSotn;cVsDin;pDP;pDS;pEnterpr;pP;pQ;pT;pTitle;сDSu;сOtbTp;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','PCV1','/wlb_AGLKS/wdg_KCH_MN1/wdg_PCV1','out;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','PCV2','/wlb_AGLKS/wdg_KCH_MN1/wdg_PCV2','out;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','PCV3','/wlb_AGLKS/wdg_KCH_MN1/wdg_PCV3','out;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','T_PP1','/wlb_AGLKS/wdg_KCH_MN1/wdg_T_PP1','pErr;pNAME;pName;pVal;prec;spName;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','T_PP3','/wlb_AGLKS/wdg_KCH_MN1/wdg_T_PP3','pErr;pNAME;pName;pVal;prec;spName;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','T_PP5','/wlb_AGLKS/wdg_KCH_MN1/wdg_T_PP5','pErr;pNAME;pName;pVal;prec;spName;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','P_PP1','/wlb_AGLKS/wdg_KCH_MN1/wdg_P_PP1','pErr;pNAME;pName;pNameSz;pVal;prec;spName;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','P_PP3','/wlb_AGLKS/wdg_KCH_MN1/wdg_P_PP3','pErr;pNAME;pName;pVal;prec;spName;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','P_PP5','/wlb_AGLKS/wdg_KCH_MN1/wdg_P_PP5','pErr;pNAME;pName;pVal;prec;spName;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','PC1','/wlb_AGLKS/wdg_KCH_MN1/wdg_PC1','pErr;pModeA;pModeC;pName;pPrec;pVal;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','PC2','/wlb_AGLKS/wdg_KCH_MN1/wdg_PC2','pErr;pModeA;pModeC;pName;pPrec;pVal;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','PC3','/wlb_AGLKS/wdg_KCH_MN1/wdg_PC3','pErr;pModeA;pModeC;pName;pPrec;pVal;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','BC1','/wlb_AGLKS/wdg_KCH_MN1/wdg_BC1','DESCR;close;com;digComs;digStts;shifr;st_close;st_open;stop;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','BC2','/wlb_AGLKS/wdg_KCH_MN1/wdg_BC2','DESCR;close;com;digComs;digStts;shifr;st_close;st_open;stop;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','BC5','/wlb_AGLKS/wdg_KCH_MN1/wdg_BC5','DESCR;close;com;digComs;digStts;shifr;st_close;st_open;stop;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','BC6','/wlb_AGLKS/wdg_KCH_MN1/wdg_BC6','DESCR;close;com;digComs;digStts;shifr;st_close;st_open;stop;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','BC7','/wlb_AGLKS/wdg_KCH_MN1/wdg_BC7','DESCR;close;com;digComs;digStts;shifr;st_close;st_open;stop;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','BC21','/wlb_AGLKS/wdg_KCH_MN1/wdg_BC21','DESCR;close;com;digComs;digStts;shifr;st_close;st_open;stop;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','BC22','/wlb_AGLKS/wdg_KCH_MN1/wdg_BC22','DESCR;close;com;digComs;digStts;shifr;st_close;st_open;stop;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','Pi','/wlb_AGLKS/wdg_KCH_MN1/wdg_Pi','pErr;pModeA;pModeC;pName;pPrec;pVal;spName;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','Ti','/wlb_AGLKS/wdg_KCH_MN1/wdg_Ti','pErr;pModeA;pModeC;pName;pPrec;pVal;spName;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','F_PP1','/wlb_AGLKS/wdg_KCH_MN1/wdg_F_PP1','pErr;pNAME;pName;pNameSz;pVal;prec;spName;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','F_PP3','/wlb_AGLKS/wdg_KCH_MN1/wdg_F_PP3','pErr;pNAME;pName;pVal;prec;spName;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','F_PP5','/wlb_AGLKS/wdg_KCH_MN1/wdg_F_PP5','pErr;pNAME;pName;pVal;prec;spName;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_10','Ti_нд','/wlb_test/wdg_mn_gen/wdg_Ti_нд','pErr;pModeA;pModeC;pName;pPrec;pVal;spName;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_10','Pi_нд','/wlb_test/wdg_mn_gen/wdg_Pi_нд','pErr;pModeA;pModeC;pName;pPrec;pVal;spName;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr','trnd1','/wlb_Main/wdg_grpCadr/wdg_trnd1','curSek;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','el1','/prj_AGLKS/pg_so/pg_1/pg_gcadr/wdg_el1','digComs;digStts;max;min;prmAMax;prmAMin;prmAnalog;prmAuto;prmCasc;prmClose;prmCloseSt;prmCom;prmDemention;prmDescr;prmId;prmImpQdwnTm;prmImpQupTm;prmManIn;prmOpenSt;prmOut;prmPrec;prmShifr;prmSp;prmStop;prmVar;prmVarIn;prmWMax;prmWMin;prmAddr;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','el2','/prj_AGLKS/pg_so/pg_1/pg_gcadr/wdg_el2','digComs;digStts;max;min;prmAMax;prmAMin;prmAnalog;prmAuto;prmCasc;prmClose;prmCloseSt;prmCom;prmDemention;prmDescr;prmId;prmImpQdwnTm;prmImpQupTm;prmManIn;prmOpenSt;prmOut;prmPrec;prmShifr;prmSp;prmStop;prmVar;prmVarIn;prmWMax;prmWMin;prmAddr;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','el3','/prj_AGLKS/pg_so/pg_1/pg_gcadr/wdg_el3','digComs;digStts;max;min;prmAMax;prmAMin;prmAnalog;prmAuto;prmCasc;prmClose;prmCloseSt;prmCom;prmDemention;prmDescr;prmId;prmImpQdwnTm;prmImpQupTm;prmManIn;prmOpenSt;prmOut;prmPrec;prmShifr;prmSp;prmStop;prmVar;prmVarIn;prmWMax;prmWMin;prmAddr;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','el4','/prj_AGLKS/pg_so/pg_1/pg_gcadr/wdg_el4','digComs;digStts;max;min;prmAMax;prmAMin;prmAnalog;prmAuto;prmCasc;prmClose;prmCloseSt;prmCom;prmDemention;prmDescr;prmId;prmImpQdwnTm;prmImpQupTm;prmManIn;prmOpenSt;prmOut;prmPrec;prmShifr;prmSp;prmStop;prmVar;prmVarIn;prmWMax;prmWMin;prmAddr;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','el5','/prj_AGLKS/pg_so/pg_1/pg_gcadr/wdg_el5','digComs;digStts;max;min;prmAMax;prmAMin;prmAnalog;prmAuto;prmCasc;prmClose;prmCloseSt;prmCom;prmDemention;prmDescr;prmId;prmImpQdwnTm;prmImpQupTm;prmManIn;prmOpenSt;prmOut;prmPrec;prmShifr;prmSp;prmStop;prmVar;prmVarIn;prmWMax;prmWMin;prmAddr;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','el6','/prj_AGLKS/pg_so/pg_1/pg_gcadr/wdg_el6','digComs;digStts;max;min;prmAMax;prmAMin;prmAnalog;prmAuto;prmCasc;prmClose;prmCloseSt;prmCom;prmDemention;prmDescr;prmId;prmImpQdwnTm;prmImpQupTm;prmManIn;prmOpenSt;prmOut;prmPrec;prmShifr;prmSp;prmStop;prmVar;prmVarIn;prmWMax;prmWMin;prmAddr;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','el7','/prj_AGLKS/pg_so/pg_1/pg_gcadr/wdg_el7','digComs;digStts;max;min;prmAMax;prmAMin;prmAnalog;prmAuto;prmCasc;prmClose;prmCloseSt;prmCom;prmDemention;prmDescr;prmId;prmImpQdwnTm;prmImpQupTm;prmManIn;prmOpenSt;prmOut;prmPrec;prmShifr;prmSp;prmStop;prmVar;prmVarIn;prmWMax;prmWMin;prmAddr;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','el8','/prj_AGLKS/pg_so/pg_1/pg_gcadr/wdg_el8','digComs;digStts;max;min;prmAMax;prmAMin;prmAnalog;prmAuto;prmCasc;prmClose;prmCloseSt;prmCom;prmDemention;prmDescr;prmId;prmImpQdwnTm;prmImpQupTm;prmManIn;prmOpenSt;prmOut;prmPrec;prmShifr;prmSp;prmStop;prmVar;prmVarIn;prmWMax;prmWMin;prmAddr;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','trnd1','/prj_AGLKS/pg_so/pg_1/pg_gcadr/wdg_trnd1','curSek;prm0val;prm1val;prm2val;prm3val;prm4val;prm5val;prm6val;prm7val;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_gview/pg_1','el1_1','/prj_AGLKS/pg_so/pg_1/pg_gview/wdg_el1_1','name;addr;var;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_gview/pg_1','el1_2','/prj_AGLKS/pg_so/pg_1/pg_gview/wdg_el1_2','name;addr;var;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_gview/pg_1','el1_3','/prj_AGLKS/pg_so/pg_1/pg_gview/wdg_el1_3','name;addr;var;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_gview/pg_1','el1_4','/prj_AGLKS/pg_so/pg_1/pg_gview/wdg_el1_4','name;addr;var;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph','trnd1','/wlb_Main/wdg_grpGraph/wdg_trnd1','curSek;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','el7','/prj_AGLKS/pg_so/pg_2/pg_ggraph/wdg_el7','name;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','el6','/prj_AGLKS/pg_so/pg_2/pg_ggraph/wdg_el6','name;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','el5','/prj_AGLKS/pg_so/pg_2/pg_ggraph/wdg_el5','name;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','el2','/prj_AGLKS/pg_so/pg_2/pg_ggraph/wdg_el2','name;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','el3','/prj_AGLKS/pg_so/pg_2/pg_ggraph/wdg_el3','name;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','el4','/prj_AGLKS/pg_so/pg_2/pg_ggraph/wdg_el4','name;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','el1','/prj_AGLKS/pg_so/pg_2/pg_ggraph/wdg_el1','name;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','F_PP101','/wlb_AGLKS/wdg_CM101/wdg_F_PP101','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','F_PP102','/wlb_AGLKS/wdg_CM101/wdg_F_PP102','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','KPP101','/wlb_AGLKS/wdg_CM101/wdg_KPP101','out;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','N_CM','/wlb_AGLKS/wdg_CM101/wdg_N_CM','pErr;pModeA;pModeC;pName;pPrec;pVal;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','T_PP101','/wlb_AGLKS/wdg_CM101/wdg_T_PP101','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','T_PP102','/wlb_AGLKS/wdg_CM101/wdg_T_PP102','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','KS101','/wlb_AGLKS/wdg_CM101/wdg_KS101','DESCR;close;com;digComs;digStts;shifr;st_close;st_open;stop;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','KS102','/wlb_AGLKS/wdg_CM101/wdg_KS102','DESCR;close;com;digComs;digStts;shifr;st_close;st_open;stop;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','KS104','/wlb_AGLKS/wdg_CM101/wdg_KS104','DESCR;close;com;digComs;digStts;shifr;st_close;st_open;stop;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','KS105','/wlb_AGLKS/wdg_CM101/wdg_KS105','DESCR;close;com;digComs;digStts;shifr;st_close;st_open;stop;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','KS106','/wlb_AGLKS/wdg_CM101/wdg_KS106','DESCR;close;com;digComs;digStts;shifr;st_close;st_open;stop;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','P_PP101','/wlb_AGLKS/wdg_CM101/wdg_P_PP101','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','P_PP102','/wlb_AGLKS/wdg_CM101/wdg_P_PP102','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','T_CM101_1','/wlb_AGLKS/wdg_CM101/wdg_T_CM101_1','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','T_CM101_2','/wlb_AGLKS/wdg_CM101/wdg_T_CM101_2','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','TAT101_1','/wlb_AGLKS/wdg_CM101/wdg_TAT101_1','pErr;pModeA;pModeC;pName;pPrec;pVal;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','TAT101_2','/wlb_AGLKS/wdg_CM101/wdg_TAT101_2','pErr;pModeA;pModeC;pName;pPrec;pVal;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','P_CM101_1','/wlb_AGLKS/wdg_CM101/wdg_P_CM101_1','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','P_CM101_2','/wlb_AGLKS/wdg_CM101/wdg_P_CM101_2','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_control/pg_grph_panel','size','/wlb_Main/wdg_grph_panel/wdg_size','value;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_control/pg_grph_panel','date','/wlb_Main/wdg_grph_panel/wdg_date','value;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_control/pg_doc_panel','size','/wlb_Main/wdg_doc_panel/wdg_size','value;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_control/pg_doc_panel','date','/wlb_Main/wdg_doc_panel/wdg_date','value;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','ki','/wlb_Main/wdg_cntrRegul/wdg_ki','value;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','rfct','/wlb_Main/wdg_cntrRegul/wdg_rfct','value;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','k1','/wlb_Main/wdg_cntrRegul/wdg_k1','value;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','k2','/wlb_Main/wdg_cntrRegul/wdg_k2','value;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','k3','/wlb_Main/wdg_cntrRegul/wdg_k3','value;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','k4','/wlb_Main/wdg_cntrRegul/wdg_k4','value;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','e1','/prj_AGLKS/pg_so/pg_rg/pg_rg/wdg_e1','p1_addr;p1_clr;p1_max;p1_min;p1_name;p2_addr;p2_max;p2_min;p2_name;p3_addr;p3_max;p3_min;p3_name;p4_addr;p4_max;p4_min;p4_name;p5_addr;p5_max;p5_min;p5_name;title;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','e2','/prj_AGLKS/pg_so/pg_rg/pg_rg/wdg_e2','p1_addr;p1_max;p1_min;p1_name;p2_addr;p2_max;p2_min;p2_name;p3_addr;p3_max;p3_min;p3_name;p4_addr;p4_max;p4_min;p4_name;p5_addr;p5_max;p5_min;p5_name;title;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','e3','/prj_AGLKS/pg_so/pg_rg/pg_rg/wdg_e3','p1_addr;p1_max;p1_min;p1_name;p2_addr;p2_max;p2_min;p2_name;p3_addr;p3_max;p3_min;p3_name;p4_addr;p4_max;p4_min;p4_name;title;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','e5','/prj_AGLKS/pg_so/pg_rg/pg_rg/wdg_e5','p1_addr;p1_max;p1_min;p1_name;p2_addr;p2_max;p2_min;p2_name;p3_addr;p3_max;p3_min;p3_name;p4_addr;p4_max;p4_min;p4_name;p5_addr;p5_max;p5_min;p5_name;title;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','e6','/prj_AGLKS/pg_so/pg_rg/pg_rg/wdg_e6','p1_addr;p1_max;p1_min;p1_name;p2_addr;p2_max;p2_min;p2_name;p3_addr;p3_max;p3_min;p3_name;title;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','e7','/prj_AGLKS/pg_so/pg_rg/pg_rg/wdg_e7','p1_addr;p1_max;p1_min;p1_name;p2_addr;p2_max;p2_min;p2_name;p3_addr;p3_max;p3_min;p3_name;p4_addr;p4_max;p4_min;p4_name;p5_addr;p5_max;p5_min;p5_name;title;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','e8','/prj_AGLKS/pg_so/pg_rg/pg_rg/wdg_e8','p1_addr;p1_max;p1_min;p1_name;p2_addr;p2_max;p2_min;p2_name;p3_addr;p3_max;p3_min;p3_name;title;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph','trnd1','/wlb_Main/wdg_grpGraph/wdg_trnd1','curSek;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','el1','/prj_AGLKS/pg_so/pg_3/pg_ggraph/wdg_el1','name;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','el2','/prj_AGLKS/pg_so/pg_3/pg_ggraph/wdg_el2','name;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','el3','/prj_AGLKS/pg_so/pg_3/pg_ggraph/wdg_el3','name;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','el4','/prj_AGLKS/pg_so/pg_3/pg_ggraph/wdg_el4','name;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','el5','/prj_AGLKS/pg_so/pg_3/pg_ggraph/wdg_el5','name;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','el6','/prj_AGLKS/pg_so/pg_3/pg_ggraph/wdg_el6','name;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','el7','/prj_AGLKS/pg_so/pg_3/pg_ggraph/wdg_el7','name;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','el8','/prj_AGLKS/pg_so/pg_3/pg_ggraph/wdg_el8','name;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','trnd1','/prj_AGLKS/pg_so/pg_3/pg_ggraph/wdg_trnd1','curSek;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','F_PP101','/wlb_AGLKS/wdg_CM101/wdg_F_PP101','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','F_PP102','/wlb_AGLKS/wdg_CM101/wdg_F_PP102','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','txtCM101_1','/wlb_AGLKS/wdg_CM101/wdg_txtCM101_1','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','txtCM101_2','/wlb_AGLKS/wdg_CM101/wdg_txtCM101_2','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','KPP101','/wlb_AGLKS/wdg_CM101/wdg_KPP101','out;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','N_CM','/wlb_AGLKS/wdg_CM101/wdg_N_CM','pErr;pModeA;pModeC;pName;pPrec;pVal;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','txtKPP101','/wlb_AGLKS/wdg_CM101/wdg_txtKPP101','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','Text1','/wlb_AGLKS/wdg_CM101/wdg_Text1','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','Text3','/wlb_AGLKS/wdg_CM101/wdg_Text3','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','Text4','/wlb_AGLKS/wdg_CM101/wdg_Text4','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','Text5','/wlb_AGLKS/wdg_CM101/wdg_Text5','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','Text6','/wlb_AGLKS/wdg_CM101/wdg_Text6','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','Text7','/wlb_AGLKS/wdg_CM101/wdg_Text7','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','T_PP101','/wlb_AGLKS/wdg_CM101/wdg_T_PP101','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','T_PP102','/wlb_AGLKS/wdg_CM101/wdg_T_PP102','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','KS101','/wlb_AGLKS/wdg_CM101/wdg_KS101','DESCR;close;com;digComs;digStts;shifr;st_close;st_open;stop;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','KS102','/wlb_AGLKS/wdg_CM101/wdg_KS102','DESCR;close;com;digComs;digStts;shifr;st_close;st_open;stop;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','KS104','/wlb_AGLKS/wdg_CM101/wdg_KS104','DESCR;close;com;digComs;digStts;shifr;st_close;st_open;stop;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','KS105','/wlb_AGLKS/wdg_CM101/wdg_KS105','DESCR;close;com;digComs;digStts;shifr;st_close;st_open;stop;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','KS106','/wlb_AGLKS/wdg_CM101/wdg_KS106','DESCR;close;com;digComs;digStts;shifr;st_close;st_open;stop;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','CM101','/wlb_AGLKS/wdg_CM101/wdg_CM101','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','P_PP101','/wlb_AGLKS/wdg_CM101/wdg_P_PP101','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','P_PP102','/wlb_AGLKS/wdg_CM101/wdg_P_PP102','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','txtKS101','/wlb_AGLKS/wdg_CM101/wdg_txtKS101','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','txtKS102','/wlb_AGLKS/wdg_CM101/wdg_txtKS102','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','txtKS103','/wlb_AGLKS/wdg_CM101/wdg_txtKS103','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','txtKS104','/wlb_AGLKS/wdg_CM101/wdg_txtKS104','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','txtKS105','/wlb_AGLKS/wdg_CM101/wdg_txtKS105','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','txtKS107','/wlb_AGLKS/wdg_CM101/wdg_txtKS107','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','T_CM101_1','/wlb_AGLKS/wdg_CM101/wdg_T_CM101_1','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','txtKS121','/wlb_AGLKS/wdg_CM101/wdg_txtKS121','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','T_CM101_2','/wlb_AGLKS/wdg_CM101/wdg_T_CM101_2','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','txtKS122','/wlb_AGLKS/wdg_CM101/wdg_txtKS122','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','txtKS123','/wlb_AGLKS/wdg_CM101/wdg_txtKS123','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','txtAT101_1','/wlb_AGLKS/wdg_CM101/wdg_txtAT101_1','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','txtAT101_2','/wlb_AGLKS/wdg_CM101/wdg_txtAT101_2','geomY;text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','TAT101_1','/wlb_AGLKS/wdg_CM101/wdg_TAT101_1','pErr;pModeA;pModeC;pName;pPrec;pVal;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','TAT101_2','/wlb_AGLKS/wdg_CM101/wdg_TAT101_2','pErr;pModeA;pModeC;pName;pPrec;pVal;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','P_CM101_1','/wlb_AGLKS/wdg_CM101/wdg_P_CM101_1','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','P_CM101_2','/wlb_AGLKS/wdg_CM101/wdg_P_CM101_2','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','txtPP101','/wlb_AGLKS/wdg_CM101/wdg_txtPP101','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','txtPP102','/wlb_AGLKS/wdg_CM101/wdg_txtPP102','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','ti','/wlb_Main/wdg_cntrRegul/wdg_ti','value;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','tzd','/wlb_Main/wdg_cntrRegul/wdg_tzd','value;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','zn','/wlb_Main/wdg_cntrRegul/wdg_zn','value;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','tiMin','/wlb_Main/wdg_cntrRegul/wdg_tiMin','value;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','h1','/wlb_Main/wdg_cntrRegul/wdg_h1','value;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','h2','/wlb_Main/wdg_cntrRegul/wdg_h2','value;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','el8','/prj_AGLKS/pg_so/pg_2/pg_ggraph/wdg_el8','name;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','trnd1','/prj_AGLKS/pg_so/pg_2/pg_ggraph/wdg_trnd1','curSek;sclWin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','el4','/prj_AGLKS/pg_so/pg_1/pg_ggraph/wdg_el4','name;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','el5','/prj_AGLKS/pg_so/pg_1/pg_ggraph/wdg_el5','name;aMax;aMin;addr;color;digComs;digStts;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','trnd1','/prj_AGLKS/pg_so/pg_1/pg_ggraph/wdg_trnd1','curSek;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph','trnd1','/wlb_Main/wdg_grpGraph/wdg_trnd1','curSek;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','el1','/prj_AGLKS/pg_so/pg_4/pg_ggraph/wdg_el1','name;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','el2','/prj_AGLKS/pg_so/pg_4/pg_ggraph/wdg_el2','name;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','el3','/prj_AGLKS/pg_so/pg_4/pg_ggraph/wdg_el3','name;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','el4','/prj_AGLKS/pg_so/pg_4/pg_ggraph/wdg_el4','name;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','el5','/prj_AGLKS/pg_so/pg_4/pg_ggraph/wdg_el5','name;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','el6','/prj_AGLKS/pg_so/pg_4/pg_ggraph/wdg_el6','name;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','el7','/prj_AGLKS/pg_so/pg_4/pg_ggraph/wdg_el7','name;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','el8','/prj_AGLKS/pg_so/pg_4/pg_ggraph/wdg_el8','name;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','trnd1','/prj_AGLKS/pg_so/pg_4/pg_ggraph/wdg_trnd1','curSek;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','F_PP101','/wlb_AGLKS/wdg_CM101/wdg_F_PP101','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','F_PP102','/wlb_AGLKS/wdg_CM101/wdg_F_PP102','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','txtCM101_1','/wlb_AGLKS/wdg_CM101/wdg_txtCM101_1','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','txtCM101_2','/wlb_AGLKS/wdg_CM101/wdg_txtCM101_2','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','KPP101','/wlb_AGLKS/wdg_CM101/wdg_KPP101','out;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','N_CM','/wlb_AGLKS/wdg_CM101/wdg_N_CM','pErr;pModeA;pModeC;pName;pPrec;pVal;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','txtKPP101','/wlb_AGLKS/wdg_CM101/wdg_txtKPP101','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','Text3','/wlb_AGLKS/wdg_CM101/wdg_Text3','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','Text4','/wlb_AGLKS/wdg_CM101/wdg_Text4','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','Text5','/wlb_AGLKS/wdg_CM101/wdg_Text5','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','Text7','/wlb_AGLKS/wdg_CM101/wdg_Text7','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','T_PP101','/wlb_AGLKS/wdg_CM101/wdg_T_PP101','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','T_PP102','/wlb_AGLKS/wdg_CM101/wdg_T_PP102','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','KS101','/wlb_AGLKS/wdg_CM101/wdg_KS101','DESCR;close;com;digComs;digStts;shifr;st_close;st_open;stop;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','KS102','/wlb_AGLKS/wdg_CM101/wdg_KS102','DESCR;close;com;digComs;digStts;shifr;st_close;st_open;stop;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','KS104','/wlb_AGLKS/wdg_CM101/wdg_KS104','DESCR;close;com;digComs;digStts;shifr;st_close;st_open;stop;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','KS105','/wlb_AGLKS/wdg_CM101/wdg_KS105','DESCR;close;com;digComs;digStts;shifr;st_close;st_open;stop;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','KS106','/wlb_AGLKS/wdg_CM101/wdg_KS106','DESCR;close;com;digComs;digStts;shifr;st_close;st_open;stop;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','CM101','/wlb_AGLKS/wdg_CM101/wdg_CM101','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','P_PP101','/wlb_AGLKS/wdg_CM101/wdg_P_PP101','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','P_PP102','/wlb_AGLKS/wdg_CM101/wdg_P_PP102','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','txtKS101','/wlb_AGLKS/wdg_CM101/wdg_txtKS101','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','txtKS102','/wlb_AGLKS/wdg_CM101/wdg_txtKS102','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','txtKS103','/wlb_AGLKS/wdg_CM101/wdg_txtKS103','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','txtKS104','/wlb_AGLKS/wdg_CM101/wdg_txtKS104','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','txtKS105','/wlb_AGLKS/wdg_CM101/wdg_txtKS105','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','txtKS107','/wlb_AGLKS/wdg_CM101/wdg_txtKS107','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','T_CM101_1','/wlb_AGLKS/wdg_CM101/wdg_T_CM101_1','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','T_CM101_2','/wlb_AGLKS/wdg_CM101/wdg_T_CM101_2','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','txtKS121','/wlb_AGLKS/wdg_CM101/wdg_txtKS121','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','txtKS122','/wlb_AGLKS/wdg_CM101/wdg_txtKS122','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','txtKS123','/wlb_AGLKS/wdg_CM101/wdg_txtKS123','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','txtAT101_1','/wlb_AGLKS/wdg_CM101/wdg_txtAT101_1','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','txtAT101_2','/wlb_AGLKS/wdg_CM101/wdg_txtAT101_2','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','TAT101_1','/wlb_AGLKS/wdg_CM101/wdg_TAT101_1','pErr;pModeA;pModeC;pName;pPrec;pVal;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','TAT101_2','/wlb_AGLKS/wdg_CM101/wdg_TAT101_2','pErr;pModeA;pModeC;pName;pPrec;pVal;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','P_CM101_1','/wlb_AGLKS/wdg_CM101/wdg_P_CM101_1','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','P_CM101_2','/wlb_AGLKS/wdg_CM101/wdg_P_CM101_2','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','txtPP101','/wlb_AGLKS/wdg_CM101/wdg_txtPP101','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','txtPP102','/wlb_AGLKS/wdg_CM101/wdg_txtPP102','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','F_PP101','/wlb_AGLKS/wdg_CM101/wdg_F_PP101','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','F_PP102','/wlb_AGLKS/wdg_CM101/wdg_F_PP102','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','txtCM101_1','/wlb_AGLKS/wdg_CM101/wdg_txtCM101_1','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','txtCM101_2','/wlb_AGLKS/wdg_CM101/wdg_txtCM101_2','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','KPP101','/wlb_AGLKS/wdg_CM101/wdg_KPP101','out;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','N_CM','/wlb_AGLKS/wdg_CM101/wdg_N_CM','pErr;pModeA;pModeC;pName;pPrec;pVal;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','txtKPP101','/wlb_AGLKS/wdg_CM101/wdg_txtKPP101','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','Text1','/wlb_AGLKS/wdg_CM101/wdg_Text1','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','Text3','/wlb_AGLKS/wdg_CM101/wdg_Text3','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','Text4','/wlb_AGLKS/wdg_CM101/wdg_Text4','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','Text5','/wlb_AGLKS/wdg_CM101/wdg_Text5','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','Text6','/wlb_AGLKS/wdg_CM101/wdg_Text6','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','Text7','/wlb_AGLKS/wdg_CM101/wdg_Text7','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','T_PP101','/wlb_AGLKS/wdg_CM101/wdg_T_PP101','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','T_PP102','/wlb_AGLKS/wdg_CM101/wdg_T_PP102','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','KS101','/wlb_AGLKS/wdg_CM101/wdg_KS101','DESCR;close;com;digComs;digStts;shifr;st_close;st_open;stop;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','KS102','/wlb_AGLKS/wdg_CM101/wdg_KS102','DESCR;close;com;digComs;digStts;shifr;st_close;st_open;stop;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','KS104','/wlb_AGLKS/wdg_CM101/wdg_KS104','DESCR;close;com;digComs;digStts;shifr;st_close;st_open;stop;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','KS105','/wlb_AGLKS/wdg_CM101/wdg_KS105','DESCR;close;com;digComs;digStts;shifr;st_close;st_open;stop;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','KS106','/wlb_AGLKS/wdg_CM101/wdg_KS106','DESCR;close;com;digComs;digStts;shifr;st_close;st_open;stop;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','CM101','/wlb_AGLKS/wdg_CM101/wdg_CM101','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','P_PP101','/wlb_AGLKS/wdg_CM101/wdg_P_PP101','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','P_PP102','/wlb_AGLKS/wdg_CM101/wdg_P_PP102','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','txtKS101','/wlb_AGLKS/wdg_CM101/wdg_txtKS101','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','txtKS102','/wlb_AGLKS/wdg_CM101/wdg_txtKS102','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','txtKS103','/wlb_AGLKS/wdg_CM101/wdg_txtKS103','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','txtKS104','/wlb_AGLKS/wdg_CM101/wdg_txtKS104','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','txtKS105','/wlb_AGLKS/wdg_CM101/wdg_txtKS105','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','txtKS107','/wlb_AGLKS/wdg_CM101/wdg_txtKS107','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','T_CM101_1','/wlb_AGLKS/wdg_CM101/wdg_T_CM101_1','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','T_CM101_2','/wlb_AGLKS/wdg_CM101/wdg_T_CM101_2','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','txtKS121','/wlb_AGLKS/wdg_CM101/wdg_txtKS121','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','txtKS122','/wlb_AGLKS/wdg_CM101/wdg_txtKS122','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','txtKS123','/wlb_AGLKS/wdg_CM101/wdg_txtKS123','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','txtAT101_1','/wlb_AGLKS/wdg_CM101/wdg_txtAT101_1','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','txtAT101_2','/wlb_AGLKS/wdg_CM101/wdg_txtAT101_2','geomY;text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','TAT101_1','/wlb_AGLKS/wdg_CM101/wdg_TAT101_1','pErr;pModeA;pModeC;pName;pPrec;pVal;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','TAT101_2','/wlb_AGLKS/wdg_CM101/wdg_TAT101_2','pErr;pModeA;pModeC;pName;pPrec;pVal;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','P_CM101_1','/wlb_AGLKS/wdg_CM101/wdg_P_CM101_1','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','P_CM101_2','/wlb_AGLKS/wdg_CM101/wdg_P_CM101_2','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','txtPP101','/wlb_AGLKS/wdg_CM101/wdg_txtPP101','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','txtPP102','/wlb_AGLKS/wdg_CM101/wdg_txtPP102','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph','trnd1','/wlb_Main/wdg_grpGraph/wdg_trnd1','curSek;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','el1','/prj_AGLKS/pg_so/pg_5/pg_ggraph/wdg_el1','name;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','el2','/prj_AGLKS/pg_so/pg_5/pg_ggraph/wdg_el2','name;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','el3','/prj_AGLKS/pg_so/pg_5/pg_ggraph/wdg_el3','name;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','el4','/prj_AGLKS/pg_so/pg_5/pg_ggraph/wdg_el4','name;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','el5','/prj_AGLKS/pg_so/pg_5/pg_ggraph/wdg_el5','name;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','el6','/prj_AGLKS/pg_so/pg_5/pg_ggraph/wdg_el6','name;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','el7','/prj_AGLKS/pg_so/pg_5/pg_ggraph/wdg_el7','name;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','el8','/prj_AGLKS/pg_so/pg_5/pg_ggraph/wdg_el8','name;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph','trnd1','/wlb_Main/wdg_grpGraph/wdg_trnd1','curSek;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','el1','/prj_AGLKS/pg_so/pg_2/pg_ggraph/wdg_el1','name;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','el2','/prj_AGLKS/pg_so/pg_2/pg_ggraph/wdg_el2','name;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','el3','/prj_AGLKS/pg_so/pg_2/pg_ggraph/wdg_el3','name;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','el4','/prj_AGLKS/pg_so/pg_2/pg_ggraph/wdg_el4','name;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','el5','/prj_AGLKS/pg_so/pg_2/pg_ggraph/wdg_el5','name;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','el6','/prj_AGLKS/pg_so/pg_2/pg_ggraph/wdg_el6','name;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','el7','/prj_AGLKS/pg_so/pg_2/pg_ggraph/wdg_el7','name;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','el8','/prj_AGLKS/pg_so/pg_2/pg_ggraph/wdg_el8','name;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','trnd1','/prj_AGLKS/pg_so/pg_2/pg_ggraph/wdg_trnd1','curSek;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','F_PP101','/wlb_AGLKS/wdg_CM101/wdg_F_PP101','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','F_PP102','/wlb_AGLKS/wdg_CM101/wdg_F_PP102','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','txtCM101_1','/wlb_AGLKS/wdg_CM101/wdg_txtCM101_1','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','txtCM101_2','/wlb_AGLKS/wdg_CM101/wdg_txtCM101_2','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','KPP101','/wlb_AGLKS/wdg_CM101/wdg_KPP101','out;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','N_CM','/wlb_AGLKS/wdg_CM101/wdg_N_CM','pErr;pModeA;pModeC;pName;pPrec;pVal;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','txtKPP101','/wlb_AGLKS/wdg_CM101/wdg_txtKPP101','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','Text3','/wlb_AGLKS/wdg_CM101/wdg_Text3','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','Text4','/wlb_AGLKS/wdg_CM101/wdg_Text4','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','Text5','/wlb_AGLKS/wdg_CM101/wdg_Text5','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','Text7','/wlb_AGLKS/wdg_CM101/wdg_Text7','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','T_PP101','/wlb_AGLKS/wdg_CM101/wdg_T_PP101','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','T_PP102','/wlb_AGLKS/wdg_CM101/wdg_T_PP102','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','KS101','/wlb_AGLKS/wdg_CM101/wdg_KS101','DESCR;close;com;digComs;digStts;shifr;st_close;st_open;stop;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','KS102','/wlb_AGLKS/wdg_CM101/wdg_KS102','DESCR;close;com;digComs;digStts;shifr;st_close;st_open;stop;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','KS104','/wlb_AGLKS/wdg_CM101/wdg_KS104','DESCR;close;com;digComs;digStts;shifr;st_close;st_open;stop;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','KS105','/wlb_AGLKS/wdg_CM101/wdg_KS105','DESCR;close;com;digComs;digStts;shifr;st_close;st_open;stop;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','KS106','/wlb_AGLKS/wdg_CM101/wdg_KS106','DESCR;close;com;digComs;digStts;shifr;st_close;st_open;stop;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','CM101','/wlb_AGLKS/wdg_CM101/wdg_CM101','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','P_PP101','/wlb_AGLKS/wdg_CM101/wdg_P_PP101','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','P_PP102','/wlb_AGLKS/wdg_CM101/wdg_P_PP102','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','txtKS101','/wlb_AGLKS/wdg_CM101/wdg_txtKS101','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','txtKS102','/wlb_AGLKS/wdg_CM101/wdg_txtKS102','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','txtKS103','/wlb_AGLKS/wdg_CM101/wdg_txtKS103','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','txtKS104','/wlb_AGLKS/wdg_CM101/wdg_txtKS104','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','txtKS105','/wlb_AGLKS/wdg_CM101/wdg_txtKS105','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','txtKS107','/wlb_AGLKS/wdg_CM101/wdg_txtKS107','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','T_CM101_1','/wlb_AGLKS/wdg_CM101/wdg_T_CM101_1','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','T_CM101_2','/wlb_AGLKS/wdg_CM101/wdg_T_CM101_2','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','txtKS121','/wlb_AGLKS/wdg_CM101/wdg_txtKS121','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','txtKS122','/wlb_AGLKS/wdg_CM101/wdg_txtKS122','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','txtKS123','/wlb_AGLKS/wdg_CM101/wdg_txtKS123','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','txtAT101_1','/wlb_AGLKS/wdg_CM101/wdg_txtAT101_1','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','txtAT101_2','/wlb_AGLKS/wdg_CM101/wdg_txtAT101_2','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','TAT101_1','/wlb_AGLKS/wdg_CM101/wdg_TAT101_1','pErr;pModeA;pModeC;pName;pPrec;pVal;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','TAT101_2','/wlb_AGLKS/wdg_CM101/wdg_TAT101_2','pErr;pModeA;pModeC;pName;pPrec;pVal;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','P_CM101_1','/wlb_AGLKS/wdg_CM101/wdg_P_CM101_1','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','P_CM101_2','/wlb_AGLKS/wdg_CM101/wdg_P_CM101_2','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','txtPP101','/wlb_AGLKS/wdg_CM101/wdg_txtPP101','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','txtPP102','/wlb_AGLKS/wdg_CM101/wdg_txtPP102','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph','trnd1','/wlb_Main/wdg_grpGraph/wdg_trnd1','curSek;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','el1','/prj_AGLKS/pg_so/pg_3/pg_ggraph/wdg_el1','name;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','el2','/prj_AGLKS/pg_so/pg_3/pg_ggraph/wdg_el2','name;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','el3','/prj_AGLKS/pg_so/pg_3/pg_ggraph/wdg_el3','name;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','el4','/prj_AGLKS/pg_so/pg_3/pg_ggraph/wdg_el4','name;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','el5','/prj_AGLKS/pg_so/pg_3/pg_ggraph/wdg_el5','name;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','el6','/prj_AGLKS/pg_so/pg_3/pg_ggraph/wdg_el6','name;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','el7','/prj_AGLKS/pg_so/pg_3/pg_ggraph/wdg_el7','name;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','el8','/prj_AGLKS/pg_so/pg_3/pg_ggraph/wdg_el8','name;aMax;aMin;addr;ed;max;min;pModeA;pModeC;prec;wMax;wMin;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','trnd1','/prj_AGLKS/pg_so/pg_3/pg_ggraph/wdg_trnd1','curSek;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','F_PP101','/wlb_AGLKS/wdg_CM101/wdg_F_PP101','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','F_PP102','/wlb_AGLKS/wdg_CM101/wdg_F_PP102','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','txtCM101_1','/wlb_AGLKS/wdg_CM101/wdg_txtCM101_1','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','txtCM101_2','/wlb_AGLKS/wdg_CM101/wdg_txtCM101_2','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','KPP101','/wlb_AGLKS/wdg_CM101/wdg_KPP101','out;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','N_CM','/wlb_AGLKS/wdg_CM101/wdg_N_CM','pErr;pModeA;pModeC;pName;pPrec;pVal;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','txtKPP101','/wlb_AGLKS/wdg_CM101/wdg_txtKPP101','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','Text1','/wlb_AGLKS/wdg_CM101/wdg_Text1','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','Text3','/wlb_AGLKS/wdg_CM101/wdg_Text3','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','Text4','/wlb_AGLKS/wdg_CM101/wdg_Text4','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','Text5','/wlb_AGLKS/wdg_CM101/wdg_Text5','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','Text6','/wlb_AGLKS/wdg_CM101/wdg_Text6','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','Text7','/wlb_AGLKS/wdg_CM101/wdg_Text7','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','T_PP101','/wlb_AGLKS/wdg_CM101/wdg_T_PP101','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','T_PP102','/wlb_AGLKS/wdg_CM101/wdg_T_PP102','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','KS101','/wlb_AGLKS/wdg_CM101/wdg_KS101','DESCR;close;com;digComs;digStts;shifr;st_close;st_open;stop;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','KS102','/wlb_AGLKS/wdg_CM101/wdg_KS102','DESCR;close;com;digComs;digStts;shifr;st_close;st_open;stop;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','KS104','/wlb_AGLKS/wdg_CM101/wdg_KS104','DESCR;close;com;digComs;digStts;shifr;st_close;st_open;stop;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','KS105','/wlb_AGLKS/wdg_CM101/wdg_KS105','DESCR;close;com;digComs;digStts;shifr;st_close;st_open;stop;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','KS106','/wlb_AGLKS/wdg_CM101/wdg_KS106','DESCR;close;com;digComs;digStts;shifr;st_close;st_open;stop;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','CM101','/wlb_AGLKS/wdg_CM101/wdg_CM101','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','P_PP101','/wlb_AGLKS/wdg_CM101/wdg_P_PP101','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','P_PP102','/wlb_AGLKS/wdg_CM101/wdg_P_PP102','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','txtKS101','/wlb_AGLKS/wdg_CM101/wdg_txtKS101','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','txtKS102','/wlb_AGLKS/wdg_CM101/wdg_txtKS102','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','txtKS103','/wlb_AGLKS/wdg_CM101/wdg_txtKS103','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','txtKS104','/wlb_AGLKS/wdg_CM101/wdg_txtKS104','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','txtKS105','/wlb_AGLKS/wdg_CM101/wdg_txtKS105','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','txtKS107','/wlb_AGLKS/wdg_CM101/wdg_txtKS107','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','T_CM101_1','/wlb_AGLKS/wdg_CM101/wdg_T_CM101_1','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','T_CM101_2','/wlb_AGLKS/wdg_CM101/wdg_T_CM101_2','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','txtKS121','/wlb_AGLKS/wdg_CM101/wdg_txtKS121','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','txtKS122','/wlb_AGLKS/wdg_CM101/wdg_txtKS122','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','txtKS123','/wlb_AGLKS/wdg_CM101/wdg_txtKS123','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','txtAT101_1','/wlb_AGLKS/wdg_CM101/wdg_txtAT101_1','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','txtAT101_2','/wlb_AGLKS/wdg_CM101/wdg_txtAT101_2','geomY;text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','TAT101_1','/wlb_AGLKS/wdg_CM101/wdg_TAT101_1','pErr;pModeA;pModeC;pName;pPrec;pVal;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','TAT101_2','/wlb_AGLKS/wdg_CM101/wdg_TAT101_2','pErr;pModeA;pModeC;pName;pPrec;pVal;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','P_CM101_1','/wlb_AGLKS/wdg_CM101/wdg_P_CM101_1','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','P_CM101_2','/wlb_AGLKS/wdg_CM101/wdg_P_CM101_2','pErr;pNAME;pName;pVal;prec;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','txtPP101','/wlb_AGLKS/wdg_CM101/wdg_txtPP101','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','txtPP102','/wlb_AGLKS/wdg_CM101/wdg_txtPP102','text;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','comprEn101','/wlb_AGLKS/wdg_KCH_MN1/wdg_comprEn101','pVar;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','comprEn102','/wlb_AGLKS/wdg_KCH_MN1/wdg_comprEn102','pVar;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','comprEn201','/wlb_AGLKS/wdg_KCH_MN1/wdg_comprEn201','pVar;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','comprEn202','/wlb_AGLKS/wdg_KCH_MN1/wdg_comprEn202','pVar;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','comprEn301','/wlb_AGLKS/wdg_KCH_MN1/wdg_comprEn301','pVar;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','comprEn302','/wlb_AGLKS/wdg_KCH_MN1/wdg_comprEn302','pVar;',2);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_10','prescrRunSimple','/wlb_test/wdg_mn_gen/wdg_prescrRunSimple','prExtCurCom;prExtMode;prExtProg;prExtStartTm;prExtWork;',0);
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_control/pg_treeSelect','apply','/wlb_Main/wdg_treeSelect/wdg_apply','value;','');
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_control/pg_treeSelect','cancel','/wlb_Main/wdg_treeSelect/wdg_cancel','value;','');
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so','alarms','/wlb_originals/wdg_Protocol','geomX;geomY;geomW;geomH;geomZ;headVis;tSize;trcPer;lev;viewOrd;col;itProp;it0lev;it0fnt;it0color;it1lev;it1color;it2lev;it2color;','');
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_control/pg_cntrPasp','data','/wlb_Main/wdg_cntrPaspExt/wdg_data','owner;','');
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_control/pg_cntrPasp','sel_data','/wlb_Main/wdg_cntrPaspExt/wdg_sel_data','owner;value;','');
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_control/pg_cntrPasp','sel_trends','/wlb_Main/wdg_cntrPaspExt/wdg_sel_trends','owner;value;','');
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_control/pg_cntrPasp','sel_view','/wlb_Main/wdg_cntrPaspExt/wdg_sel_view','owner;value;','');
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_control/pg_cntrPasp','trends','/wlb_Main/wdg_cntrPaspExt/wdg_trends','owner;','');
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_control/pg_cntrPasp','trendsList','/wlb_Main/wdg_cntrPaspExt/wdg_trendsList','owner;','');
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_control/pg_cntrPasp','view','/wlb_Main/wdg_cntrPaspExt/wdg_view','owner;','');
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_control/pg_cntrPasp','viewLabSet','/wlb_Main/wdg_cntrPaspExt/wdg_viewLabSet','owner;','');
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_control/pg_cntrPasp','viewSet','/wlb_Main/wdg_cntrPaspExt/wdg_viewSet','owner;value;','');
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_control/pg_cntrPaspOld','doc','/wlb_Main/wdg_cntrPasp/wdg_doc','owner;','');
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','CM101','/wlb_AGLKS/wdg_CM101/wdg_CM101','text;','');
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','e10','/prj_AGLKS/pg_so/pg_rg/pg_rg/wdg_e10','p1_addr;p1_max;p1_min;p1_name;p2_addr;p2_max;p2_min;p2_name;p3_addr;p3_max;p3_min;p3_name;title;','');
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','e11','/prj_AGLKS/pg_so/pg_rg/pg_rg/wdg_e11','p1_addr;p1_max;p1_min;p1_name;p2_addr;p2_max;p2_min;p2_name;p3_addr;p3_max;p3_min;p3_name;p4_addr;p4_max;p4_min;p4_name;p5_addr;p5_max;p5_min;p5_name;title;','');
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','e12','/prj_AGLKS/pg_so/pg_rg/pg_rg/wdg_e12','p1_addr;p1_max;p1_min;p1_name;p2_addr;p2_max;p2_min;p2_name;p3_addr;p3_max;p3_min;p3_name;title;','');
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','e13','/prj_AGLKS/pg_so/pg_rg/pg_rg/wdg_e13','p1_addr;p1_max;p1_min;p1_name;p2_addr;p2_max;p2_min;p2_name;p3_addr;p3_max;p3_min;p3_name;p4_addr;p4_max;p4_min;p4_name;p5_addr;p5_max;p5_min;p5_name;title;','');
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','e14','/prj_AGLKS/pg_so/pg_rg/pg_rg/wdg_e14','p1_addr;p1_max;p1_min;p1_name;p2_addr;p2_max;p2_min;p2_name;p3_addr;p3_max;p3_min;p3_name;title;','');
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','e15','/prj_AGLKS/pg_so/pg_rg/pg_rg/wdg_e15','p1_addr;p1_max;p1_min;p1_name;p2_addr;p2_max;p2_min;p2_name;p3_addr;p3_max;p3_min;p3_name;p4_addr;p4_max;p4_min;p4_name;p5_addr;p5_max;p5_min;p5_name;title;','');
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','e16','/prj_AGLKS/pg_so/pg_rg/pg_rg/wdg_e16','p1_addr;p1_max;p1_min;p1_name;p2_addr;p2_max;p2_min;p2_name;p3_addr;p3_max;p3_min;p3_name;title;','');
INSERT INTO prj_AGLKS_incl VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','e9','/prj_AGLKS/pg_so/pg_rg/pg_rg/wdg_e9','p1_addr;p1_max;p1_min;p1_name;p2_addr;p2_max;p2_min;p2_name;p3_addr;p3_max;p3_min;p3_name;p4_addr;p4_max;p4_min;p4_name;p5_addr;p5_max;p5_min;p5_name;title;','');
CREATE TABLE IF NOT EXISTS 'prj_AGLKS_io' ("IDW" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"IO_VAL" TEXT DEFAULT '' ,"SELF_FLG" INTEGER DEFAULT '' ,"CFG_TMPL" TEXT DEFAULT '' ,"CFG_VAL" TEXT DEFAULT '' ,"IDC" TEXT DEFAULT '' ,"uk#IO_VAL" TEXT DEFAULT '' ,"uk#CFG_TMPL" TEXT DEFAULT '' ,"uk#CFG_VAL" TEXT DEFAULT '' ,"ru#IO_VAL" TEXT DEFAULT '' ,"ru#CFG_TMPL" TEXT DEFAULT '' ,"ru#CFG_VAL" TEXT DEFAULT '' , PRIMARY KEY ("IDW","ID","IDC"));
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so','evProc','ws_BtPress:/prev:prev:/pg_so/*/*/$
ws_BtPress:/next:next:/pg_so/*/*/$
ws_BtPress:/go_mn:open:/pg_so/*/mn/*
ws_BtPress:/go_graph:open:/pg_so/*/ggraph/*
ws_BtPress:/go_cadr:open:/pg_so/*/gcadr/*
ws_BtPress:/go_view:open:/pg_so/*/gview/*
ws_BtPress:/go_doc:open:/pg_so/*/doc/*
ws_BtPress:/go_resg:open:/pg_so/rg/rg/*
ws_BtPress:/so1:open:/pg_so/1/*/*
ws_BtPress:/so2:open:/pg_so/2/*/*
ws_BtPress:/so3:open:/pg_so/3/*/*
ws_BtPress:/so4:open:/pg_so/4/*/*
ws_BtPress:/so5:open:/pg_so/5/*/*
ws_BtPress:/so6:open:/pg_so/6/*/*
ws_BtPress:/so7:open:/pg_so/7/*/*
ws_BtPress:/so8:open:/pg_so/8/*/*
ws_BtPress:/so9:open:/pg_so/9/*/*
ws_BtPress:*:open:/pg_control/pg_terminator',0,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so','pgOpen','1',0,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so','pgGrp','main',0,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so','pgGrp','so',32,'','','pgCont','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so','pgGrp','cntr',32,'','','infoW','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1','name','Main station',0,'','','','Загальностанційка','','','Общестанционка','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1','dscr','Main station control',0,'','','','Керування загальностанційкою','','','Управление общестанционкой','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1','geomX','5',0,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph','evProc','ws_FocusIn:/trnd1:open:/pg_control/pg_grph_panel
ws_FocusIn:/el1:open:/pg_control/pg_ElCadr
ws_FocusIn:/el2:open:/pg_control/pg_ElCadr
ws_FocusIn:/el3:open:/pg_control/pg_ElCadr
ws_FocusIn:/el4:open:/pg_control/pg_ElCadr
ws_FocusIn:/el5:open:/pg_control/pg_ElCadr
ws_FocusIn:/el6:open:/pg_control/pg_ElCadr
ws_FocusIn:/el7:open:/pg_control/pg_ElCadr
ws_FocusIn:/el8:open:/pg_control/pg_ElCadr',32,'','','','ws_FocusIn:/trnd1:open:/pg_control/pg_grph_panel
ws_FocusIn:/el1:open:/pg_control/pg_ElCadr
ws_FocusIn:/el2:open:/pg_control/pg_ElCadr
ws_FocusIn:/el3:open:/pg_control/pg_ElCadr
ws_FocusIn:/el4:open:/pg_control/pg_ElCadr
ws_FocusIn:/el5:open:/pg_control/pg_ElCadr
ws_FocusIn:/el6:open:/pg_control/pg_ElCadr
ws_FocusIn:/el7:open:/pg_control/pg_ElCadr
ws_FocusIn:/el8:open:/pg_control/pg_ElCadr','','','ws_FocusIn:/trnd1:open:/pg_control/pg_grph_panel
ws_FocusIn:/el1:open:/pg_control/pg_ElCadr
ws_FocusIn:/el2:open:/pg_control/pg_ElCadr
ws_FocusIn:/el3:open:/pg_control/pg_ElCadr
ws_FocusIn:/el4:open:/pg_control/pg_ElCadr
ws_FocusIn:/el5:open:/pg_control/pg_ElCadr
ws_FocusIn:/el6:open:/pg_control/pg_ElCadr
ws_FocusIn:/el7:open:/pg_control/pg_ElCadr
ws_FocusIn:/el8:open:/pg_control/pg_ElCadr','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph','pgGrp','so',32,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph','curSek','1247474043',40,'','','trnd1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','name','Generic',41,'','','','Загальне','','','Общее','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','wMin',NULL,34,'Parameter|wMin','prm:/LogicLev/gen/T_PP1/wMin','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','addr',NULL,42,'Parameter|var','prm:/LogicLev/gen/F_PP1/var','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','name','Graphics group element',42,'Parameter|NAME','prm:/LogicLev/gen/F_PP1/NAME','el8','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','min',NULL,42,'Parameter|min','prm:/LogicLev/gen/P_PP5/min','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','name','Graphics group element',42,'Parameter|NAME','prm:/BlockCalc/gen_cntr/PC_PCV3/NAME','el7','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','addr',NULL,42,'Parameter|var','prm:/BlockCalc/gen_cntr/PC_PCV3/var','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','ed',NULL,34,'Parameter|ed','prm:/BlockCalc/gen_cntr/PC_PCV3/ed','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','name','Graphics group element',42,'Parameter|NAME','prm:/LogicLev/gen/P_PP5/NAME','el6','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','addr',NULL,42,'Parameter|var','prm:/LogicLev/gen/P_PP5/var','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','ed',NULL,34,'Parameter|ed','prm:/LogicLev/gen/P_PP5/ed','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','addr',NULL,42,'Parameter|var','prm:/BlockCalc/gen_cntr/PC_PCV2/var','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','ed',NULL,34,'Parameter|ed','prm:/BlockCalc/gen_cntr/PC_PCV2/ed','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','max',NULL,42,'Parameter|max','prm:/BlockCalc/gen_cntr/PC_PCV2/max','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','name','Graphics group element',42,'Parameter|NAME','prm:/LogicLev/gen/P_PP3/NAME','el4','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','addr',NULL,42,'Parameter|var','prm:/LogicLev/gen/P_PP3/var','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','ed',NULL,34,'Parameter|ed','prm:/LogicLev/gen/P_PP3/ed','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','max',NULL,42,'Parameter|max','prm:/LogicLev/gen/P_PP3/max','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','min',NULL,42,'Parameter|min','prm:/LogicLev/gen/P_PP3/min','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','name','Graphics group element',42,'Parameter|NAME','prm:/LogicLev/gen/P_PP1/NAME','el3','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','addr',NULL,42,'Parameter|var','prm:/LogicLev/gen/P_PP1/var','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','ed',NULL,34,'Parameter|ed','prm:/LogicLev/gen/P_PP1/ed','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','max',NULL,42,'Parameter|max','prm:/LogicLev/gen/P_PP1/max','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','name','Graphics group element',42,'Parameter|NAME','prm:/BlockCalc/gen_cntr/PC_PCV1/NAME','el2','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','addr',NULL,42,'Parameter|var','prm:/BlockCalc/gen_cntr/PC_PCV1/var','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','ed',NULL,34,'Parameter|ed','prm:/BlockCalc/gen_cntr/PC_PCV1/ed','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','max',NULL,42,'Parameter|max','prm:/BlockCalc/gen_cntr/PC_PCV1/max','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','pModeC',NULL,34,'Parameter|casc','prm:/LogicLev/gen/T_PP1','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','max',NULL,42,'Parameter|max','prm:/LogicLev/gen/Pi/max','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','ed',NULL,34,'Parameter|ed','prm:/LogicLev/gen/Pi/ed','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','addr',NULL,42,'Parameter|var','prm:/LogicLev/gen/Pi/var','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','name','Graphics group element',42,'Parameter|NAME','prm:/LogicLev/gen/Pi/NAME','el1','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','pModeA',NULL,34,'Parameter|auto','prm:/LogicLev/gen/T_PP3','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','pModeC',NULL,34,'Parameter|casc','prm:/LogicLev/gen/T_PP3','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','wMax',NULL,34,'Parameter|wMax','prm:/LogicLev/gen/T_PP1/wMax','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','prec',NULL,34,'Parameter|prec','prm:/LogicLev/gen/T_PP1','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','ed',NULL,34,'Parameter|ed','prm:/LogicLev/gen/Ti/ed','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','max',NULL,42,'Parameter|max','prm:/LogicLev/gen/Ti/max','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','min',NULL,42,'Parameter|min','prm:/LogicLev/gen/Ti/min','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','pModeA',NULL,34,'Parameter|auto','prm:/LogicLev/gen/Ti','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','pModeC',NULL,34,'Parameter|casc','prm:/LogicLev/gen/Ti','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','prec',NULL,34,'Parameter|prec','prm:/LogicLev/gen/Ti','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','wMax',NULL,34,'Parameter|wMax','prm:/LogicLev/gen/Ti/wMax','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','wMin',NULL,34,'Parameter|wMin','prm:/LogicLev/gen/Ti/wMin','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','name','Graphics group element',42,'Parameter|NAME','prm:/LogicLev/gen/T_PP1/NAME','el2','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','aMax',NULL,34,'Parameter|aMax','prm:/LogicLev/gen/T_PP1/aMax','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','aMin',NULL,34,'Parameter|aMin','prm:/LogicLev/gen/T_PP1/aMin','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','addr',NULL,42,'Parameter|var','prm:/LogicLev/gen/T_PP1/var','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','ed',NULL,34,'Parameter|ed','prm:/LogicLev/gen/T_PP1/ed','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','max',NULL,42,'Parameter|max','prm:/LogicLev/gen/T_PP1/max','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','prec',NULL,34,'Parameter|prec','prm:/BlockCalc/CM101/PT1006_1/prec','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','addr',NULL,42,'Parameter|var','prm:/LogicLev/gen/T_PP3/var','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','ed',NULL,34,'Parameter|ed','prm:/LogicLev/gen/T_PP3/ed','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','max',NULL,42,'Parameter|max','prm:/LogicLev/gen/T_PP3/max','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','min',NULL,42,'Parameter|min','prm:/LogicLev/gen/T_PP3/min','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc','name','Documents',0,'','','','Документи','','','Документы','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc','dscr','Logical container of page group: "Documents"',0,'','','','Логічний контейнер групи сторінок: "Документи"','','','Логический контейнер группы страниц: "Документы"','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_1','tmpl','<body docProcLang="JavaLikeCalc.JavaScript">
<h1>Table of accumulation instantaneous values</h1>
<TABLE border="1" cellpadding="2" cellspacing="0" width="100%">
  <TR align="center" valign="center">
    <TD>View</TD><TD>Compare</TD><TD>&nbsp;</TD><TD>Point configuration</TD><TD>Archivation</TD><TD>Options</TD><TD>?</TD>
  </TR>
  <TR align="center" valign="center">
    <TD colspan="2">&nbsp;</TD><TD colspan="2">Information over day</TD>
    <TD colspan="3" rowspan="2">DD MM YYYY<?dp return Special.FLibSYS.tmFStr(time,"%d %m %Y");?></TD>
  </TR>
  <TR align="center" valign="center"><TD colspan="2">&nbsp;</TD><TD colspan="2">Information over month</TD></TR>
  <TR align="center" valign="center"><TD colspan="7">Accumulated values by calculator # pipeline #</TD></TR>
  <TR><TD>Date</TD><TD>Time</TD><TD>Flow</TD><TD>Day capacity</TD><TD>Pressure</TD><TD>Temperatura</TD><TD>Pressure diff.</TD></TR>
  <TR><TD colspan="2">&nbsp;</TD><TD>1000x m3/h</TD><TD>1000x m3</TD><TD>kgF/cm2</TD><TD>grad.C</TD><TD>kgF/cm2</TD></TR>
  <TR docRept="1">
    <TD>DD MM YYYY<?dp return Special.FLibSYS.tmFStr(rTime,"%d %m %Y");?></TD>
    <TD>hh:mm:ss<?dp return Special.FLibSYS.tmFStr(rTime,"%H:%M:%S");?></TD>
    <TD>XX.XXX<?dp return DAQ.JavaLikeCalc.lib_doc.getVal(pF,rTime,0,2,"",true);?></TD>
    <TD>X.XXX<?dp return DAQ.JavaLikeCalc.lib_doc.getVal(pQ,rTime,0,3,"",true);?></TD>
    <TD>X.XXX<?dp return DAQ.JavaLikeCalc.lib_doc.getVal(pP,rTime,0,3,"",true);?></TD>
    <TD>X.XXX<?dp return DAQ.JavaLikeCalc.lib_doc.getVal(pT,rTime,0,3,"",true);?></TD>
    <TD>XXX.XXX<?dp return DAQ.JavaLikeCalc.lib_doc.getVal(pDP,rTime,0,3,"",true);?></TD>
  </TR>
</TABLE>
</body>',32,'','','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so','demoPlayProc','stepCur++; stepTm = 20;
//>> Open main mnemo
if(stepCur == 0)
{
	this.pg_1.pg_mn.pg_1.attrSet("pgOpen",true);
	this.attrSet("tipStatus","Main mnemo open.");
}
//>> Open main graphics
else if(stepCur == 1)
{
	this.pg_1.pg_ggraph.pg_1.attrSet("pgOpen",true);
	this.attrSet("tipStatus","Main graphics open.");
}
//>> Setpoint set more for PC PCV1
else if(stepCur == 2)
{
	SYS.DAQ.BlockCalc.gen_cntr.PC_PCV1.sp.set(6);
	this.attrSet("tipStatus","The regulator PC_PCV1 setpoint increase.");
}
//>> Open contours group
else if(stepCur == 3)
{
	this.pg_1.pg_gcadr.pg_1.attrSet("pgOpen",true);
	this.attrSet("tipStatus","Contours group open.");
}
//>> Open overview cadr
else if(stepCur == 4)
{
	this.pg_1.pg_gview.pg_1.attrSet("pgOpen",true);
	this.attrSet("tipStatus","Overview frame open.");
}
//>> Open document
else if(stepCur == 5)
{
	this.pg_1.pg_doc.pg_1.attrSet("pgOpen",true);
	this.attrSet("tipStatus","Document open.");
}
//>> Open result graphics
else if(stepCur == 6)
{
	this.pg_rg.pg_rg.pg_1.attrSet("pgOpen",true);
	this.attrSet("tipStatus","Result graphics open.");
}
//>> Open mnemo for CM101
else if(stepCur == 7)
{
	this.pg_2.pg_mn.pg_CM101.attrSet("pgOpen",true);
	this.attrSet("tipStatus","Mnemo CM101 open.");
}
//>> Open graphics for CM101
else if(stepCur == 8)
{
	this.pg_2.pg_ggraph.pg_1.attrSet("pgOpen",true);
	this.attrSet("tipStatus","Graphics CM101 open.");
}
//>> Open PID-control panel
else if(stepCur == 9)
{
	this.ownerSess().pg_control.pg_ElCadr.attrSet("pgOpenSrc",this.pg_1.pg_mn.pg_1.wdg_PC1.attr("path"));
	this.attrSet("tipStatus","The parameter PC_PCV1 control panel open.");
	stepTm = 1;
}
//>> Open PID-control page
else if(stepCur == 10)
{
	this.ownerSess().pg_control.pg_cntrRegul.attrSet("pgOpenSrc",this.ownerSess().pg_control.pg_ElCadr.attr("path"));
	this.attrSet("tipStatus","The regulator PC_PCV1 control panel open.");
}
//>> Setpoint restore for PC PCV1
else if(stepCur == 11)
{
	SYS.DAQ.BlockCalc.gen_cntr.PC_PCV1.sp.set(5.8);
	this.attrSet("tipStatus","The regulator PC_PCV1 setpoint restore.");
}
else { stepCur = -1; stepTm = 0; }',40,'','','','stepCur++; stepTm = 20;
//>> Open main mnemo
if(stepCur == 0)
{
	this.pg_1.pg_mn.pg_1.attrSet("pgOpen",true);
	this.attrSet("tipStatus","Відкриття головної мнемосхеми.");
}
//>> Open main graphics
else if(stepCur == 1)
{
	this.pg_1.pg_ggraph.pg_1.attrSet("pgOpen",true);
	this.attrSet("tipStatus","Відкриття головних графіків.");
}
//>> Setpoint set more for PC PCV1
else if(stepCur == 2)
{
	SYS.DAQ.BlockCalc.gen_cntr.PC_PCV1.sp.set(6);
	this.attrSet("tipStatus","Збільшення завдання регулятору PC_КРТ1.");
}
//>> Open contours group
else if(stepCur == 3)
{
	this.pg_1.pg_gcadr.pg_1.attrSet("pgOpen",true);
	this.attrSet("tipStatus","Відкриття групи контурів.");
}
//>> Open overview cadr
else if(stepCur == 4)
{
	this.pg_1.pg_gview.pg_1.attrSet("pgOpen",true);
	this.attrSet("tipStatus","Відкриття оглядового кадру.");
}
//>> Open document
else if(stepCur == 5)
{
	this.pg_1.pg_doc.pg_1.attrSet("pgOpen",true);
	this.attrSet("tipStatus","Відкриття документу.");
}
//>> Open result graphics
else if(stepCur == 6)
{
	this.pg_rg.pg_rg.pg_1.attrSet("pgOpen",true);
	this.attrSet("tipStatus","Відкриття зведенних графіків.");
}
//>> Open mnemo for CM101
else if(stepCur == 7)
{
	this.pg_2.pg_mn.pg_CM101.attrSet("pgOpen",true);
	this.attrSet("tipStatus","Відкриття мнемосхеми КМ101.");
}
//>> Open graphics for CM101
else if(stepCur == 8)
{
	this.pg_2.pg_ggraph.pg_1.attrSet("pgOpen",true);
	this.attrSet("tipStatus","Відкриття графіків КМ101.");
}
//>> Open PID-control panel
else if(stepCur == 9)
{
	this.ownerSess().pg_control.pg_ElCadr.attrSet("pgOpenSrc",this.pg_1.pg_mn.pg_1.wdg_PC1.attr("path"));
	this.attrSet("tipStatus","Відкриття панелі управління параметру PC_КРТ1.");
	stepTm = 1;
}
//>> Open PID-control page
else if(stepCur == 10)
{
	this.ownerSess().pg_control.pg_cntrRegul.attrSet("pgOpenSrc",this.ownerSess().pg_control.pg_ElCadr.attr("path"));
	this.attrSet("tipStatus","Відкриття панелі управління регулятору PC_КРТ1.");
}
//>> Setpoint restore for PC PCV1
else if(stepCur == 11)
{
	SYS.DAQ.BlockCalc.gen_cntr.PC_PCV1.sp.set(5.8);
	this.attrSet("tipStatus","Відновлення завдання регулятору PC_КРТ1.");
}
else { stepCur = -1; stepTm = 0; }','','','stepCur++; stepTm = 20;
//>> Open main mnemo
if(stepCur == 0)
{
	this.pg_1.pg_mn.pg_1.attrSet("pgOpen",true);
	this.attrSet("tipStatus","Открытие главной мнемосхемы.");
}
//>> Open main graphics
else if(stepCur == 1)
{
	this.pg_1.pg_ggraph.pg_1.attrSet("pgOpen",true);
	this.attrSet("tipStatus","Открытие главных графиков.");
}
//>> Setpoint set more for PC PCV1
else if(stepCur == 2)
{
	SYS.DAQ.BlockCalc.gen_cntr.PC_PCV1.sp.set(6);
	this.attrSet("tipStatus","Увеличение задания регулятора PC_КРД1.");
}
//>> Open contours group
else if(stepCur == 3)
{
	this.pg_1.pg_gcadr.pg_1.attrSet("pgOpen",true);
	this.attrSet("tipStatus","Открытие группы контуров.");
}
//>> Open overview cadr
else if(stepCur == 4)
{
	this.pg_1.pg_gview.pg_1.attrSet("pgOpen",true);
	this.attrSet("tipStatus","Открытие обзорного кадра.");
}
//>> Open document
else if(stepCur == 5)
{
	this.pg_1.pg_doc.pg_1.attrSet("pgOpen",true);
	this.attrSet("tipStatus","Открытие документа.");
}
//>> Open result graphics
else if(stepCur == 6)
{
	this.pg_rg.pg_rg.pg_1.attrSet("pgOpen",true);
	this.attrSet("tipStatus","Открытие сводных графиков.");
}
//>> Open mnemo for CM101
else if(stepCur == 7)
{
	this.pg_2.pg_mn.pg_CM101.attrSet("pgOpen",true);
	this.attrSet("tipStatus","Открытие мнемосхемы КМ101.");
}
//>> Open graphics for CM101
else if(stepCur == 8)
{
	this.pg_2.pg_ggraph.pg_1.attrSet("pgOpen",true);
	this.attrSet("tipStatus","Открытие графиков КМ101.");
}
//>> Open PID-control panel
else if(stepCur == 9)
{
	this.ownerSess().pg_control.pg_ElCadr.attrSet("pgOpenSrc",this.pg_1.pg_mn.pg_1.wdg_PC1.attr("path"));
	this.attrSet("tipStatus","Открытие панели управления параметра PC_КРД1.");
	stepTm = 1;
}
//>> Open PID-control page
else if(stepCur == 10)
{
	this.ownerSess().pg_control.pg_cntrRegul.attrSet("pgOpenSrc",this.ownerSess().pg_control.pg_ElCadr.attr("path"));
	this.attrSet("tipStatus","Открытие панели управления регулятора PC_КРД1.");
}
//>> Setpoint restore for PC PCV1
else if(stepCur == 11)
{
	SYS.DAQ.BlockCalc.gen_cntr.PC_PCV1.sp.set(5.8);
	this.attrSet("tipStatus","Восстановление задания регулятора PC_КРД1.");
}
else { stepCur = -1; stepTm = 0; }','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','min','0',42,'Parameter|min','prm:/LogicLev/gen/F_PP1/min','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','max','100',42,'Parameter|max','prm:/LogicLev/gen/F_PP1/max','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_5','сDSu','60',33,'','60','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_3','pgNoOpenProc','1',0,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_4','pgNoOpenProc','1',0,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_5','pgNoOpenProc','1',0,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_5','cDPup','150',33,'','150','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_5','cDTr','100',33,'','100','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_5','cKL','0.3',33,'','0.3','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_5','pT',NULL,34,'Node|T','prm:/LogicLev/gen/gN1/T','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_5','pQ',NULL,34,'Node|Q','prm:/LogicLev/gen/gN1/Q','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_5','pP',NULL,34,'Node|P','prm:/LogicLev/gen/gN1/P','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_5','pDS',NULL,34,'Node|DS','prm:/LogicLev/gen/gN1/DS','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_5','pDP',NULL,34,'Node|dP','prm:/LogicLev/gen/gN1/dP','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_5','cKsh','0.2',33,'','0.2','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_5','cKtup','0.1',33,'','0.1','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_5','cMoldN','70',33,'','70','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_5','cMolsCO2','10',33,'','10','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_5','cPatm','95',33,'','95','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_5','cPots','80',33,'','80','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_5','cPperekl','85',33,'','85','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_5','cSotn','0.6',33,'','0.6','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_5','cVsDin','32',33,'','32','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn','name','Mnemos',0,'','','','Мнемосхеми','','','Мнемосхемы','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn','dscr','Logical containers of page group: "Mnemos"',0,'','','','Логічний контейнер групи сторінок: "Мнемосхеми"','','','Логический контейнер группы страниц: "Мнемосхемы"','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','name','Main',32,'','','','Загальна','','','Общая','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pgOpen','1',0,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pgGrp','so',32,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pName','',34,'Parameter|NAME','prm:/LogicLev/gen/Pi/NAME','Pi','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','st_open',NULL,38,'Parameter|st_open','prm:/BlockCalc/gen/BC22/st_open','BC22','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','st_close',NULL,38,'Parameter|st_close','prm:/BlockCalc/gen/BC22/st_close','BC22','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','com','0',38,'Parameter|com','prm:/BlockCalc/gen/BC1/com','BC1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pVal','0',34,'Parameter|var','prm:/BlockCalc/gen_cntr/PC_PCV3/var','PC3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pName','',34,'Parameter|NAME','prm:/BlockCalc/gen_cntr/PC_PCV3/NAME','PC3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pVal','0',34,'Parameter|var','prm:/LogicLev/gen/T_PP1/var','T_PP1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pName','T',33,'Parameter|NAME','T','T_PP3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pVal','0',34,'Parameter|var','prm:/LogicLev/gen/T_PP3/var','T_PP3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pName','T',33,'Parameter|NAME','T','T_PP5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pVal','0',34,'Parameter|var','prm:/LogicLev/gen/T_PP5/var','T_PP5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pName','P',33,'Parameter|NAME','P','P_PP1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pVal','0',34,'Parameter|var','prm:/LogicLev/gen/P_PP1/var','P_PP1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pName','P',33,'Parameter|NAME','P','P_PP3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pVal','0',34,'Parameter|var','prm:/LogicLev/gen/P_PP3/var','P_PP3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pName','P',33,'Parameter|NAME','P','P_PP5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pVal','0',34,'Parameter|var','prm:/LogicLev/gen/P_PP5/var','P_PP5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pErr','<EVAL>',34,'Parameter|err','prm:/BlockCalc/gen_cntr/PC_PCV1/err','PC1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pName','',34,'Parameter|NAME','prm:/BlockCalc/gen_cntr/PC_PCV1/NAME','PC1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pVal','0',34,'Parameter|var','prm:/BlockCalc/gen_cntr/PC_PCV1/var','PC1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pErr','<EVAL>',34,'Parameter|err','prm:/BlockCalc/gen_cntr/PC_PCV2/err','PC2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pName','',34,'Parameter|NAME','prm:/BlockCalc/gen_cntr/PC_PCV2/NAME','PC2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pVal','0',34,'Parameter|var','prm:/BlockCalc/gen_cntr/PC_PCV2/var','PC2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pErr','<EVAL>',34,'Parameter|err','prm:/BlockCalc/gen_cntr/PC_PCV3/err','PC3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pName','T',33,'Parameter|NAME','T','T_PP1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','out','0',34,'Parameter|out','prm:/BlockCalc/gen_cntr/PC_PCV1/out','PCV1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','out','0',34,'Parameter|out','prm:/BlockCalc/gen_cntr/PC_PCV2/out','PCV2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','out','0',34,'Parameter|out','prm:/BlockCalc/gen_cntr/PC_PCV3/out','PCV3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pVal','0',34,'Parameter|var','prm:/LogicLev/gen/Pi/var','Pi','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_10','pErr','<EVAL>',34,'Parameter|err','prm:/LogicLev/gen/P3/err','Ti_нд','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_10','pName','',34,'Parameter|NAME','prm:/LogicLev/gen/P3/NAME','Ti_нд','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_10','pgGrp','so',32,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr','evProc','ws_FocusIn:/trnd1:open:/pg_control/pg_grph_panel',32,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr','pgGrp','so',32,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr','curSek','1283263396',40,'','','trnd1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','name','Contour 1',32,'','','','Контури 1','','','Контура 1','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','grpName','Generic',41,'','Generic','','Загальні','','Загальні','Общие','','Общие');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmAddr','<EVAL>',42,'Parameter|var','prm:/BlockCalc/gen_cntr/PC_PCV3/var','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmVar','0',38,'Parameter|var','prm:/BlockCalc/gen_cntr/PC_PCV3/var','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmStop','<EVAL>',38,'Parameter|stop','prm:/BlockCalc/gen_cntr/PC_PCV3','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmOut','0',38,'Parameter|out','prm:/BlockCalc/gen_cntr/PC_PCV3/out','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmCom','<EVAL>',38,'Parameter|com','prm:/BlockCalc/gen_cntr/PC_PCV3','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmClose','<EVAL>',38,'Parameter|close','prm:/BlockCalc/gen_cntr/PC_PCV3','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmCloseSt','<EVAL>',34,'Parameter|st_close','prm:/BlockCalc/gen_cntr/PC_PCV3','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmCasc','0',38,'Parameter|casc','prm:/BlockCalc/gen_cntr/PC_PCV3','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmAuto','0',38,'Parameter|auto','prm:/BlockCalc/gen_cntr/PC_PCV3/auto','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','min','0',42,'Parameter|min','prm:/BlockCalc/gen_cntr/PC_PCV3/min','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','max','100',42,'Parameter|max','prm:/BlockCalc/gen_cntr/PC_PCV3/max','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmSp','0',38,'Parameter|sp','prm:/LogicLev/gen/P_PP5','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmOut','0',38,'Parameter|out','prm:/LogicLev/gen/P_PP5','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmDemention','',34,'Parameter|ed','prm:/LogicLev/gen/P_PP5/ed','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmClose','<EVAL>',38,'Parameter|close','prm:/LogicLev/gen/P_PP5','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmCloseSt','<EVAL>',34,'Parameter|st_close','prm:/LogicLev/gen/P_PP5','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmCasc','0',38,'Parameter|casc','prm:/LogicLev/gen/P_PP5','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmAuto','0',38,'Parameter|auto','prm:/LogicLev/gen/P_PP5','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','min','0',42,'Parameter|min','prm:/LogicLev/gen/P_PP5/min','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmSp','0',38,'Parameter|sp','prm:/BlockCalc/gen_cntr/PC_PCV2/sp','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmDemention','',34,'Parameter|ed','prm:/BlockCalc/gen_cntr/PC_PCV2/ed','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmOpenSt','<EVAL>',34,'Parameter|st_open','prm:/BlockCalc/gen_cntr/PC_PCV2','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmCom','<EVAL>',38,'Parameter|com','prm:/BlockCalc/gen_cntr/PC_PCV2','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmCloseSt','<EVAL>',34,'Parameter|st_close','prm:/BlockCalc/gen_cntr/PC_PCV2','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','max','100',42,'Parameter|max','prm:/BlockCalc/gen_cntr/PC_PCV2/max','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmVar','0',38,'Parameter|var','prm:/LogicLev/gen/P_PP3/var','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmShifr','',34,'Parameter|NAME','prm:/LogicLev/gen/P_PP3/NAME','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmSp','0',38,'Parameter|sp','prm:/LogicLev/gen/P_PP3','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so','path','/prj_AGLKS/pg_so',8,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg','name','Result graphics',0,'','','','Зведені графіки','','','Сводные графики','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gview','dscr','Pages template: "Overview frames group"',32,'','','','Шаблон сторінок: "Група оглядових кадрів"','','','Шаблон страниц: "Группа обзорных кадров"','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gview','evProc','ws_FocusIn:/el1_1:open:/pg_control/pg_ElCadr
ws_FocusIn:/el1_2:open:/pg_control/pg_ElCadr
ws_FocusIn:/el1_3:open:/pg_control/pg_ElCadr
ws_FocusIn:/el1_4:open:/pg_control/pg_ElCadr
ws_FocusIn:/el1_5:open:/pg_control/pg_ElCadr
ws_FocusIn:/el1_6:open:/pg_control/pg_ElCadr
ws_FocusIn:/el2_1:open:/pg_control/pg_ElCadr
ws_FocusIn:/el2_2:open:/pg_control/pg_ElCadr
ws_FocusIn:/el2_3:open:/pg_control/pg_ElCadr
ws_FocusIn:/el2_4:open:/pg_control/pg_ElCadr
ws_FocusIn:/el2_5:open:/pg_control/pg_ElCadr
ws_FocusIn:/el2_6:open:/pg_control/pg_ElCadr
ws_FocusIn:/el3_1:open:/pg_control/pg_ElCadr
ws_FocusIn:/el3_2:open:/pg_control/pg_ElCadr
ws_FocusIn:/el3_3:open:/pg_control/pg_ElCadr
ws_FocusIn:/el3_4:open:/pg_control/pg_ElCadr
ws_FocusIn:/el3_5:open:/pg_control/pg_ElCadr
ws_FocusIn:/el3_6:open:/pg_control/pg_ElCadr
ws_FocusIn:/el4_1:open:/pg_control/pg_ElCadr
ws_FocusIn:/el4_2:open:/pg_control/pg_ElCadr
ws_FocusIn:/el4_3:open:/pg_control/pg_ElCadr
ws_FocusIn:/el4_4:open:/pg_control/pg_ElCadr
ws_FocusIn:/el4_5:open:/pg_control/pg_ElCadr
ws_FocusIn:/el4_6:open:/pg_control/pg_ElCadr',32,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gview','pgGrp','so',32,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gview/pg_1','name','Generic',41,'','Generic','','Загальні','','Загальні','Общие','','Общие');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gview/pg_1','name','Views page''s element',34,'Parameter|NAME','prm:/LogicLev/gen/P4/NAME','el1_4','Елемент оглядового кадру','','','Элемент обзорного кадра','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gview/pg_1','addr','<EVAL>',42,'Parameter|var','prm:/LogicLev/gen/P4/var','el1_4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gview/pg_1','var','<EVAL>',34,'Parameter|var','prm:/LogicLev/gen/P3/var','el1_3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gview/pg_1','name','Views page''s element',34,'Parameter|NAME','prm:/LogicLev/gen/P3/NAME','el1_3','Елемент оглядового кадру','','','Элемент обзорного кадра','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gview/pg_1','addr','<EVAL>',42,'Parameter|var','prm:/LogicLev/gen/P3/var','el1_3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gview/pg_1','addr','<EVAL>',42,'Parameter|var','prm:/LogicLev/gen/F4/var','el1_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gview/pg_1','var','<EVAL>',34,'Parameter|var','prm:/LogicLev/gen/F4/var','el1_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gview/pg_1','name','Views page''s element',34,'Parameter|NAME','prm:/LogicLev/gen/F4/NAME','el1_2','Елемент оглядового кадру','','','Элемент обзорного кадра','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gview/pg_1','addr','<EVAL>',42,'Parameter|var','prm:/LogicLev/gen/F3/var','el1_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gview/pg_1','var','<EVAL>',34,'Parameter|var','prm:/LogicLev/gen/F3/var','el1_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gview/pg_1','name','Views page''s element',34,'Parameter|NAME','prm:/LogicLev/gen/F3/NAME','el1_1','Елемент оглядового кадру','','','Элемент обзорного кадра','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2','name','CM101',0,'','','','КМ101','','','КМ101','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph','evProc','ws_FocusIn:/trnd1:open:/pg_control/pg_grph_panel
ws_FocusIn:/el1:open:/pg_control/pg_ElCadr
ws_FocusIn:/el2:open:/pg_control/pg_ElCadr
ws_FocusIn:/el3:open:/pg_control/pg_ElCadr
ws_FocusIn:/el4:open:/pg_control/pg_ElCadr
ws_FocusIn:/el5:open:/pg_control/pg_ElCadr
ws_FocusIn:/el6:open:/pg_control/pg_ElCadr
ws_FocusIn:/el7:open:/pg_control/pg_ElCadr
ws_FocusIn:/el8:open:/pg_control/pg_ElCadr',32,'','','','ws_FocusIn:/trnd1:open:/pg_control/pg_grph_panel
ws_FocusIn:/el1:open:/pg_control/pg_ElCadr
ws_FocusIn:/el2:open:/pg_control/pg_ElCadr
ws_FocusIn:/el3:open:/pg_control/pg_ElCadr
ws_FocusIn:/el4:open:/pg_control/pg_ElCadr
ws_FocusIn:/el5:open:/pg_control/pg_ElCadr
ws_FocusIn:/el6:open:/pg_control/pg_ElCadr
ws_FocusIn:/el7:open:/pg_control/pg_ElCadr
ws_FocusIn:/el8:open:/pg_control/pg_ElCadr','','','ws_FocusIn:/trnd1:open:/pg_control/pg_grph_panel
ws_FocusIn:/el1:open:/pg_control/pg_ElCadr
ws_FocusIn:/el2:open:/pg_control/pg_ElCadr
ws_FocusIn:/el3:open:/pg_control/pg_ElCadr
ws_FocusIn:/el4:open:/pg_control/pg_ElCadr
ws_FocusIn:/el5:open:/pg_control/pg_ElCadr
ws_FocusIn:/el6:open:/pg_control/pg_ElCadr
ws_FocusIn:/el7:open:/pg_control/pg_ElCadr
ws_FocusIn:/el8:open:/pg_control/pg_ElCadr','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph','pgGrp','so',32,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph','curSek','1205241018',40,'','','trnd1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','wMax',NULL,34,'Parameter|wMax','prm:/BlockCalc/CM101/PT0204','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','wMin',NULL,34,'Parameter|wMin','prm:/BlockCalc/CM101/PT0204','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','max',NULL,42,'Parameter|max','prm:/BlockCalc/CM101/ST8612/max','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','min',NULL,42,'Parameter|min','prm:/BlockCalc/CM101/ST8612/min','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','prec',NULL,34,'Parameter|prec','prm:/BlockCalc/CM101/ST8612/prec','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','wMax',NULL,34,'Parameter|wMax','prm:/BlockCalc/CM101/ST8612','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','wMin',NULL,34,'Parameter|wMin','prm:/BlockCalc/CM101/ST8612','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','name','Graphics group element',42,'Parameter|NAME','prm:/BlockCalc/CM101/F101/NAME','el2','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','addr',NULL,42,'Parameter|var','prm:/BlockCalc/CM101/F101/var','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','pModeA',NULL,34,'Parameter|auto','prm:/BlockCalc/CM101/F101','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','pModeC',NULL,34,'Parameter|casc','prm:/BlockCalc/CM101/F101','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','aMax',NULL,34,'Parameter|aMax','prm:/BlockCalc/CM101/F101','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','aMin',NULL,34,'Parameter|aMin','prm:/BlockCalc/CM101/F101','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','ed',NULL,34,'Parameter|ed','prm:/BlockCalc/CM101/F101/ed','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','max',NULL,42,'Parameter|max','prm:/BlockCalc/CM101/F101/max','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','min',NULL,42,'Parameter|min','prm:/BlockCalc/CM101/F101/min','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','prec',NULL,34,'Parameter|prec','prm:/BlockCalc/CM101/F101/prec','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','wMax',NULL,34,'Parameter|wMax','prm:/BlockCalc/CM101/F101','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','wMin',NULL,34,'Parameter|wMin','prm:/BlockCalc/CM101/F101','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','name','Graphics group element',42,'Parameter|NAME','prm:/BlockCalc/CM101/P101/NAME','el3','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','addr',NULL,42,'Parameter|var','prm:/BlockCalc/CM101/P101/var','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','pModeA',NULL,34,'Parameter|auto','prm:/BlockCalc/CM101/P101','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','pModeC',NULL,34,'Parameter|casc','prm:/BlockCalc/CM101/P101','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','aMax',NULL,34,'Parameter|aMax','prm:/BlockCalc/CM101/P101','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','aMin',NULL,34,'Parameter|aMin','prm:/BlockCalc/CM101/P101','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','ed',NULL,34,'Parameter|ed','prm:/BlockCalc/CM101/P101/ed','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','max',NULL,42,'Parameter|max','prm:/BlockCalc/CM101/P101/max','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','min',NULL,42,'Parameter|min','prm:/BlockCalc/CM101/P101/min','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','prec',NULL,34,'Parameter|prec','prm:/BlockCalc/CM101/P101/prec','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','wMax',NULL,34,'Parameter|wMax','prm:/BlockCalc/CM101/P101','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','wMin',NULL,34,'Parameter|wMin','prm:/BlockCalc/CM101/P101','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','name','Graphics group element',42,'Parameter|NAME','prm:/BlockCalc/CM101/PT1006_1/NAME','el4','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','addr',NULL,42,'Parameter|var','prm:/BlockCalc/CM101/PT1006_1/var','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','pModeA',NULL,34,'Parameter|auto','prm:/BlockCalc/CM101/PT1006_1','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','pModeC',NULL,34,'Parameter|casc','prm:/BlockCalc/CM101/PT1006_1','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','aMax',NULL,34,'Parameter|aMax','prm:/BlockCalc/CM101/PT1006_1','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','aMin',NULL,34,'Parameter|aMin','prm:/BlockCalc/CM101/PT1006_1','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','ed',NULL,34,'Parameter|ed','prm:/BlockCalc/CM101/PT1006_1/ed','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','max',NULL,42,'Parameter|max','prm:/BlockCalc/CM101/PT1006_1/max','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn','name','Mnemos',0,'','','','Мнемосхеми','','','Мнемосхемы','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn','dscr','Logical containers of page group: "Mnemos"',0,'','','','Логічний контейнер групи сторінок: "Мнемосхеми"','','','Логический контейнер группы страниц: "Мнемосхемы"','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pgGrp','so',32,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM101/TE1314_1/var','TAT101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pName','',34,'Parameter|NAME','prm:/BlockCalc/CM101/TE1314_1/NAME','TAT101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pErr','<EVAL>',34,'Parameter|err','prm:/BlockCalc/CM101/TE1314_1/err','TAT101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','com','0',38,'Parameter|com','prm:/BlockCalc/CM101/BC104/com','KS104','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','st_open',NULL,38,'Parameter|st_open','prm:/BlockCalc/CM101/BC102/st_open','KS102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM101/TE1313_1/var','T_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pName','T',33,'Parameter|NAME','T','T_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM101/ST8612/var','N_CM','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pName','F',33,'Parameter|NAME','F','F_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM101/F101/var','F_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pName','F',33,'Parameter|NAME','F','F_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM101/F102/var','F_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','out','0',34,'Parameter|out','prm:/BlockCalc/gen_cntr/SurgeCM101/out','KPP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pErr','<EVAL>',34,'Parameter|err','prm:/BlockCalc/CM101/ST8612/err','N_CM','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pName','',34,'Parameter|NAME','prm:/BlockCalc/CM101/ST8612/NAME','N_CM','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pName','T',33,'Parameter|NAME','T','T_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM101/TE1314_1/var','T_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','com','0',38,'Parameter|com','prm:/BlockCalc/CM101/BC101/com','KS101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','shifr',NULL,38,'Parameter|NAME','prm:/BlockCalc/CM101/BC101/NAME','KS101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','st_close',NULL,38,'Parameter|st_close','prm:/BlockCalc/CM101/BC101/st_close','KS101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','st_open',NULL,38,'Parameter|st_open','prm:/BlockCalc/CM101/BC101/st_open','KS101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','com','0',38,'Parameter|com','prm:/BlockCalc/CM101/BC102/com','KS102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','shifr',NULL,38,'Parameter|NAME','prm:/BlockCalc/CM101/BC102/NAME','KS102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','st_close',NULL,38,'Parameter|st_close','prm:/BlockCalc/CM101/BC102/st_close','KS102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control','name','Control panels',0,'','','','Панелі керування','','','Панели управления','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_ElCadr','pgGrp','cntr',32,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_ElCadr','max','100',42,'<page>|max','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_ElCadr','min','0',42,'<page>|min','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_ElCadr','prmAuto','0',46,'<page>|auto','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_ElCadr','prmCasc','0',46,'<page>|casc','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_ElCadr','prmDemention','',42,'<page>|ed','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_ElCadr','prmOut','0',42,'<page>|out','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_ElCadr','prmShifr','',42,'<page>|NAME','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_ElCadr','prmSp','0',46,'<page>|sp','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_ElCadr','prmVar','0',46,'<page>|var','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_10','pVal','0',34,'Parameter|var','prm:/LogicLev/gen/F3/var','Pi_нд','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_10','pName','',34,'Parameter|NAME','prm:/LogicLev/gen/F3/NAME','Pi_нд','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_10','pErr','<EVAL>',34,'Parameter|err','prm:/LogicLev/gen/F3/err','Pi_нд','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_10','pgNoOpenProc','1',0,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','ed',NULL,34,'Parameter|ed','prm:/LogicLev/gen/F_PP1/ed','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','max',NULL,42,'Parameter|max','prm:/BlockCalc/gen_cntr/PC_PCV3/max','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','min',NULL,42,'Parameter|min','prm:/BlockCalc/gen_cntr/PC_PCV3/min','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','max',NULL,42,'Parameter|max','prm:/LogicLev/gen/P_PP5/max','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','min',NULL,42,'Parameter|min','prm:/BlockCalc/gen_cntr/PC_PCV2/min','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','name','Graphics group element',42,'Parameter|NAME','prm:/BlockCalc/gen_cntr/PC_PCV2/NAME','el5','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','min',NULL,42,'Parameter|min','prm:/LogicLev/gen/P_PP1/min','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','min',NULL,42,'Parameter|min','prm:/BlockCalc/gen_cntr/PC_PCV1/min','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','min',NULL,42,'Parameter|min','prm:/LogicLev/gen/Pi/min','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','name','Graphics group element',42,'Parameter|NAME','prm:/LogicLev/gen/T_PP3/NAME','el3','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','aMax',NULL,34,'Parameter|aMax','prm:/LogicLev/gen/T_PP3/aMax','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','aMin',NULL,34,'Parameter|aMin','prm:/LogicLev/gen/T_PP3/aMin','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','aMax',NULL,34,'Parameter|aMax','prm:/BlockCalc/CM101/PT0204','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','wMin',NULL,34,'Parameter|wMin','prm:/BlockCalc/CM101/PT1006_1','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','name','Graphics group element',42,'Parameter|NAME','prm:/BlockCalc/CM101/PT0204/NAME','el5','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','addr',NULL,42,'Parameter|var','prm:/BlockCalc/CM101/PT0204/var','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','pModeA',NULL,34,'Parameter|auto','prm:/BlockCalc/CM101/PT0204','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','pModeC',NULL,34,'Parameter|casc','prm:/BlockCalc/CM101/PT0204','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','st_close',NULL,38,'Parameter|st_close','prm:/BlockCalc/gen/BC7/st_close','BC7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_doc_panel','value','0',8,'','','size','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_doc_panel','value','0',8,'','','date','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_5','сOtbTp','Angle',33,'','Angle','doc','Кутовий','','Кутовий','Угловой','','Угловой');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pVal','0',34,'Parameter|var','prm:/LogicLev/gen/Ti/var','Ti','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pName','F',33,'Parameter|NAME','F','F_PP1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pVal','0',34,'Parameter|var','prm:/LogicLev/gen/F_PP1/var','F_PP1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pName','F',33,'Parameter|NAME','F','F_PP3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pVal','0',34,'Parameter|var','prm:/LogicLev/gen/F_PP3/var','F_PP3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pName','F',33,'Parameter|NAME','F','F_PP5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pVal','0',34,'Parameter|var','prm:/LogicLev/gen/F_PP5/var','F_PP5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_10','pVal','0',34,'Parameter|var','prm:/LogicLev/gen/P3/var','Ti_нд','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','DESCR','<EVAL>',42,'<page>|DESCR','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','Hdwn','<EVAL>',46,'<page>|Hdwn','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','Hup','<EVAL>',46,'<page>|Hup','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','K1','<EVAL>',46,'<page>|K1','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','K2','<EVAL>',46,'<page>|K2','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','K3','<EVAL>',46,'<page>|K3','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmManIn','<EVAL>',38,'Parameter|manIn','prm:/LogicLev/gen/Pi','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','st_open',NULL,38,'Parameter|st_open','prm:/BlockCalc/gen/BC21/st_open','BC21','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','com','0',38,'Parameter|com','prm:/BlockCalc/gen/BC22/com','BC22','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','st_open',NULL,38,'Parameter|st_open','prm:/BlockCalc/gen/BC7/st_open','BC7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','com','0',38,'Parameter|com','prm:/BlockCalc/gen/BC21/com','BC21','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','shifr',NULL,38,'Parameter|NAME','prm:/BlockCalc/gen/BC21/NAME','BC21','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_ElCadr','prmCom','<EVAL>',46,'<page>|com','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_ElCadr','prmStop','<EVAL>',46,'<page>|stop','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_ElCadr','prmClose','<EVAL>',46,'<page>|close','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_ElCadr','prmOpenSt','<EVAL>',42,'<page>|st_open','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_ElCadr','prmCloseSt','<EVAL>',42,'<page>|st_close','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','st_close',NULL,38,'Parameter|st_close','prm:/BlockCalc/gen/BC6/st_close','BC6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','st_open',NULL,38,'Parameter|st_open','prm:/BlockCalc/gen/BC6/st_open','BC6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','shifr',NULL,38,'Parameter|NAME','prm:/BlockCalc/gen/BC1/NAME','BC1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','st_close',NULL,38,'Parameter|st_close','prm:/BlockCalc/gen/BC1/st_close','BC1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','st_open',NULL,38,'Parameter|st_open','prm:/BlockCalc/gen/BC1/st_open','BC1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','com','0',38,'Parameter|com','prm:/BlockCalc/gen/BC2/com','BC2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','shifr',NULL,38,'Parameter|NAME','prm:/BlockCalc/gen/BC2/NAME','BC2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','st_close',NULL,38,'Parameter|st_close','prm:/BlockCalc/gen/BC2/st_close','BC2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','st_open',NULL,38,'Parameter|st_open','prm:/BlockCalc/gen/BC2/st_open','BC2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','com','0',38,'Parameter|com','prm:/BlockCalc/gen/BC5/com','BC5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','shifr',NULL,38,'Parameter|NAME','prm:/BlockCalc/gen/BC5/NAME','BC5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','st_close',NULL,38,'Parameter|st_close','prm:/BlockCalc/gen/BC5/st_close','BC5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM101/TT0204/var','T_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM101/TT0202/var','T_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pName','T',33,'Parameter|NAME','T','T_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM101/PT1006_1/var','P_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM101/P101/var','P_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pName','P',33,'Parameter|NAME','P','P_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','st_open',NULL,38,'Parameter|st_open','prm:/BlockCalc/CM101/BC106/st_open','KS106','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','shifr',NULL,38,'Parameter|NAME','prm:/BlockCalc/CM101/BC106/NAME','KS106','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','com','0',38,'Parameter|com','prm:/BlockCalc/CM101/BC106/com','KS106','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','st_close',NULL,38,'Parameter|st_close','prm:/BlockCalc/CM101/BC105/st_close','KS105','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','shifr',NULL,38,'Parameter|NAME','prm:/BlockCalc/CM101/BC105/NAME','KS105','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','com','0',38,'Parameter|com','prm:/BlockCalc/CM101/BC105/com','KS105','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','st_close',NULL,38,'Parameter|st_close','prm:/BlockCalc/CM101/BC104/st_close','KS104','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','shifr',NULL,38,'Parameter|NAME','prm:/BlockCalc/CM101/BC104/NAME','KS104','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg','name','Result graphics',32,'','','','Зведені графіки','','','Сводные графики','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg','pgGrp','so',32,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','name','Page 1',32,'','','','Сторінка 1','','','Страница 1','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_name',NULL,34,'Parameter 3|NAME','prm:/BlockCalc/CM102/TE1314_2/NAME','e8',NULL,'Parameter 3|NAME','prm:/BlockCalc/CM102/TE1314_2/NAME',NULL,'Parameter 3|NAME','prm:/BlockCalc/CM102/TE1314_2/NAME');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_name',NULL,34,'Parameter 2|NAME','prm:/BlockCalc/CM102/TE1314_1/NAME','e8',NULL,'Parameter 2|NAME','prm:/BlockCalc/CM102/TE1314_1/NAME',NULL,'Parameter 2|NAME','prm:/BlockCalc/CM102/TE1314_1/NAME');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_addr',NULL,34,'Parameter 3|var','prm:/BlockCalc/CM102/TE1314_2/var','e8','','Parameter 3|var','','','Parameter 3|var','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_max',NULL,34,'Parameter 3|max','prm:/BlockCalc/CM102/TE1314_2/max','e8','','Parameter 3|max','','','Parameter 3|max','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_min',NULL,34,'Parameter 3|min','prm:/BlockCalc/CM102/TE1314_2/min','e8','','Parameter 3|min','','','Parameter 3|min','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_min',NULL,34,'Parameter 2|min','prm:/BlockCalc/CM102/TE1314_1/min','e8','','Parameter 2|min','','','Parameter 2|min','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_max',NULL,34,'Parameter 1|max','prm:/BlockCalc/CM102/TE1313_1/max','e8','','Parameter 1|max','','','Parameter 1|max','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_min',NULL,34,'Parameter 1|min','prm:/BlockCalc/CM102/TE1313_1/min','e8','','Parameter 1|min','','','Parameter 1|min','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_name',NULL,34,'Parameter 1|NAME','prm:/BlockCalc/CM102/TE1313_1/NAME','e8',NULL,'Parameter 1|NAME','prm:/BlockCalc/CM102/TE1313_1/NAME',NULL,'Parameter 1|NAME','prm:/BlockCalc/CM102/TE1313_1/NAME');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_addr',NULL,34,'Parameter 2|var','prm:/BlockCalc/CM102/TE1314_1/var','e8','','Parameter 2|var','','','Parameter 2|var','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_max',NULL,34,'Parameter 2|max','prm:/BlockCalc/CM102/TE1314_1/max','e8','','Parameter 2|max','','','Parameter 2|max','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_addr',NULL,34,'Parameter 1|var','prm:/BlockCalc/CM102/TE1313_1/var','e8','','Parameter 1|var','','','Parameter 1|var','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p5_max',NULL,34,'Parameter 5|max','prm:/BlockCalc/CM102/PT0204/max','e7','','','','','Parameter 5|max','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p5_min',NULL,34,'Parameter 5|min','prm:/BlockCalc/CM102/PT0204/min','e7','','','','','Parameter 5|min','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p5_name',NULL,34,'Parameter 5|NAME','prm:/BlockCalc/CM102/PT0204/NAME','e7','','','',NULL,'Parameter 5|NAME','prm:/BlockCalc/CM102/PT0204/NAME');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','title','CM102 (N,F,P)',33,'','CM102 (N,F,P)','e7','КМ102 (N,F,P)','','КМ102 (N,F,P)','КМ102 (N,F,P)','','КМ102 (N,F,P)');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p5_addr',NULL,34,'Parameter 5|var','prm:/BlockCalc/CM102/PT0204/var','e7','','','','','Parameter 5|var','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_name',NULL,34,'Parameter 3|NAME','prm:/BlockCalc/CM102/P103/NAME','e7','','','',NULL,'Parameter 3|NAME','prm:/BlockCalc/CM102/P103/NAME');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p4_addr',NULL,34,'Parameter 4|var','prm:/BlockCalc/CM102/PT1006_1/var','e7','','','','','Parameter 4|var','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p4_max',NULL,34,'Parameter 4|max','prm:/BlockCalc/CM102/PT1006_1/max','e7','','','','','Parameter 4|max','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p4_min',NULL,34,'Parameter 4|min','prm:/BlockCalc/CM102/PT1006_1/min','e7','','','','','Parameter 4|min','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p4_name',NULL,34,'Parameter 4|NAME','prm:/BlockCalc/CM102/PT1006_1/NAME','e7','','','',NULL,'Parameter 4|NAME','prm:/BlockCalc/CM102/PT1006_1/NAME');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','grpName','Generic',41,'','Generic','','Загальні','','Загальні','Общие','','Общие');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_max',NULL,34,'Parameter 3|max','prm:/BlockCalc/CM102/P103/max','e7','','','','','Parameter 3|max','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_min',NULL,34,'Parameter 3|min','prm:/BlockCalc/CM102/P103/min','e7','','','','','Parameter 3|min','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_max',NULL,34,'Parameter 2|max','prm:/BlockCalc/CM102/F103/max','e7','','','','','Parameter 2|max','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_min',NULL,34,'Parameter 2|min','prm:/BlockCalc/CM102/F103/min','e7','','','','','Parameter 2|min','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_name',NULL,34,'Parameter 2|NAME','prm:/BlockCalc/CM102/F103/NAME','e7','','','',NULL,'Parameter 2|NAME','prm:/BlockCalc/CM102/F103/NAME');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_addr',NULL,34,'Parameter 3|var','prm:/BlockCalc/CM102/P103/var','e7','','','','','Parameter 3|var','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_addr',NULL,34,'Parameter 2|var','prm:/BlockCalc/CM102/F103/var','e7','','','','','Parameter 2|var','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_name',NULL,34,'Parameter 1|NAME','prm:/BlockCalc/CM102/ST8612/NAME','e7','','','',NULL,'Parameter 1|NAME','prm:/BlockCalc/CM102/ST8612/NAME');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','title','CM101 (T)',33,'','CM101 (T)','e6','КМ101 (T)','','КМ101 (T)','КМ101 (T)','','КМ101 (T)');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_addr',NULL,34,'Parameter 1|var','prm:/BlockCalc/CM102/ST8612/var','e7','','','','','Parameter 1|var','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_max',NULL,34,'Parameter 1|max','prm:/BlockCalc/CM102/ST8612/max','e7','','','','','Parameter 1|max','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_min',NULL,34,'Parameter 1|min','prm:/BlockCalc/CM102/ST8612/min','e7','','','','','Parameter 1|min','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_min',NULL,34,'Parameter 3|min','prm:/BlockCalc/CM101/TE1314_2/min','e6','','Parameter 3|min','','','Parameter 3|min','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_name',NULL,34,'Parameter 3|NAME','prm:/BlockCalc/CM101/TE1314_2/NAME','e6',NULL,'Parameter 3|NAME','prm:/BlockCalc/CM101/TE1314_2/NAME',NULL,'Parameter 3|NAME','prm:/BlockCalc/CM101/TE1314_2/NAME');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_min',NULL,34,'Parameter 2|min','prm:/BlockCalc/CM101/TE1314_1/min','e6','','Parameter 2|min','','','Parameter 2|min','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_name',NULL,34,'Parameter 2|NAME','prm:/BlockCalc/CM101/TE1314_1/NAME','e6',NULL,'Parameter 2|NAME','prm:/BlockCalc/CM101/TE1314_1/NAME',NULL,'Parameter 2|NAME','prm:/BlockCalc/CM101/TE1314_1/NAME');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_addr',NULL,34,'Parameter 3|var','prm:/BlockCalc/CM101/TE1314_2/var','e6','','Parameter 3|var','','','Parameter 3|var','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_max',NULL,34,'Parameter 3|max','prm:/BlockCalc/CM101/TE1314_2/max','e6','','Parameter 3|max','','','Parameter 3|max','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_max',NULL,34,'Parameter 2|max','prm:/BlockCalc/CM101/TE1314_1/max','e6','','Parameter 2|max','','','Parameter 2|max','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_max',NULL,34,'Parameter 1|max','prm:/BlockCalc/CM101/TE1313_1/max','e6','','Parameter 1|max','','','Parameter 1|max','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_min',NULL,34,'Parameter 1|min','prm:/BlockCalc/CM101/TE1313_1/min','e6','','Parameter 1|min','','','Parameter 1|min','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_name',NULL,34,'Parameter 1|NAME','prm:/BlockCalc/CM101/TE1313_1/NAME','e6',NULL,'Parameter 1|NAME','prm:/BlockCalc/CM101/TE1313_1/NAME',NULL,'Parameter 1|NAME','prm:/BlockCalc/CM101/TE1313_1/NAME');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_addr',NULL,34,'Parameter 2|var','prm:/BlockCalc/CM101/TE1314_1/var','e6','','Parameter 2|var','','','Parameter 2|var','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_addr',NULL,34,'Parameter 1|var','prm:/BlockCalc/CM101/TE1313_1/var','e6','','Parameter 1|var','','','Parameter 1|var','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p5_max',NULL,34,'Parameter 5|max','prm:/BlockCalc/CM101/PT0204/max','e5','','Parameter 5|max','','','Parameter 5|max','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p5_min',NULL,34,'Parameter 5|min','prm:/BlockCalc/CM101/PT0204/min','e5','','Parameter 5|min','','','Parameter 5|min','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p5_name',NULL,34,'Parameter 5|NAME','prm:/BlockCalc/CM101/PT0204/NAME','e5',NULL,'Parameter 5|NAME','prm:/BlockCalc/CM101/PT0204/NAME',NULL,'Parameter 5|NAME','prm:/BlockCalc/CM101/PT0204/NAME');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','title','CM101 (N,F,P)',33,'','CM101 (N,F,P)','e5','КМ101 (N,F,P)','','КМ101 (N,F,P)','КМ101 (N,F,P)','','КМ101 (N,F,P)');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pName','',34,'Parameter|NAME','prm:/LogicLev/gen/Ti/NAME','Ti','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3','name','CM102',0,'','','','КМ102','','','КМ102','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph','evProc','ws_FocusIn:/trnd1:open:/pg_control/pg_grph_panel
ws_FocusIn:/el1:open:/pg_control/pg_ElCadr
ws_FocusIn:/el2:open:/pg_control/pg_ElCadr
ws_FocusIn:/el3:open:/pg_control/pg_ElCadr
ws_FocusIn:/el4:open:/pg_control/pg_ElCadr
ws_FocusIn:/el5:open:/pg_control/pg_ElCadr
ws_FocusIn:/el6:open:/pg_control/pg_ElCadr
ws_FocusIn:/el7:open:/pg_control/pg_ElCadr
ws_FocusIn:/el8:open:/pg_control/pg_ElCadr',32,'','','','ws_FocusIn:/trnd1:open:/pg_control/pg_grph_panel
ws_FocusIn:/el1:open:/pg_control/pg_ElCadr
ws_FocusIn:/el2:open:/pg_control/pg_ElCadr
ws_FocusIn:/el3:open:/pg_control/pg_ElCadr
ws_FocusIn:/el4:open:/pg_control/pg_ElCadr
ws_FocusIn:/el5:open:/pg_control/pg_ElCadr
ws_FocusIn:/el6:open:/pg_control/pg_ElCadr
ws_FocusIn:/el7:open:/pg_control/pg_ElCadr
ws_FocusIn:/el8:open:/pg_control/pg_ElCadr','','','ws_FocusIn:/trnd1:open:/pg_control/pg_grph_panel
ws_FocusIn:/el1:open:/pg_control/pg_ElCadr
ws_FocusIn:/el2:open:/pg_control/pg_ElCadr
ws_FocusIn:/el3:open:/pg_control/pg_ElCadr
ws_FocusIn:/el4:open:/pg_control/pg_ElCadr
ws_FocusIn:/el5:open:/pg_control/pg_ElCadr
ws_FocusIn:/el6:open:/pg_control/pg_ElCadr
ws_FocusIn:/el7:open:/pg_control/pg_ElCadr
ws_FocusIn:/el8:open:/pg_control/pg_ElCadr','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph','pgGrp','so',32,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph','curSek','1205241018',40,'','','trnd1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','name','CM102',41,'','','','КМ102','','','КМ102','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','addr',NULL,42,'Parameter|var','prm:/BlockCalc/CM102/TE1314_1/var','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','ed',NULL,34,'Parameter|ed','prm:/BlockCalc/CM102/TE1314_1/ed','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','max',NULL,42,'Parameter|max','prm:/BlockCalc/CM102/TE1314_1/max','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','min',NULL,42,'Parameter|min','prm:/BlockCalc/CM102/TE1314_1/min','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','name','Graphics group element',42,'Parameter|NAME','prm:/BlockCalc/CM102/TE1314_2/NAME','el8','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','addr',NULL,42,'Parameter|var','prm:/BlockCalc/CM102/TE1313_1/var','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','ed',NULL,34,'Parameter|ed','prm:/BlockCalc/CM102/TE1313_1/ed','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','max',NULL,42,'Parameter|max','prm:/BlockCalc/CM102/TE1313_1/max','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','min',NULL,42,'Parameter|min','prm:/BlockCalc/CM102/TE1313_1/min','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','name','Graphics group element',42,'Parameter|NAME','prm:/BlockCalc/CM102/TE1314_1/NAME','el7','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','ed',NULL,34,'Parameter|ed','prm:/BlockCalc/CM102/PT0204/ed','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','max',NULL,42,'Parameter|max','prm:/BlockCalc/CM102/PT0204/max','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','min',NULL,42,'Parameter|min','prm:/BlockCalc/CM102/PT0204/min','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','name','Graphics group element',42,'Parameter|NAME','prm:/BlockCalc/CM102/TE1313_1/NAME','el6','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','max',NULL,42,'Parameter|max','prm:/BlockCalc/CM102/PT1006_1/max','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','min',NULL,42,'Parameter|min','prm:/BlockCalc/CM102/PT1006_1/min','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','name','Graphics group element',42,'Parameter|NAME','prm:/BlockCalc/CM102/PT0204/NAME','el5','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','addr',NULL,42,'Parameter|var','prm:/BlockCalc/CM102/PT0204/var','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','max',NULL,42,'Parameter|max','prm:/BlockCalc/CM102/P103/max','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','min',NULL,42,'Parameter|min','prm:/BlockCalc/CM102/P103/min','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','name','Graphics group element',42,'Parameter|NAME','prm:/BlockCalc/CM102/PT1006_1/NAME','el4','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','addr',NULL,42,'Parameter|var','prm:/BlockCalc/CM102/PT1006_1/var','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','ed',NULL,34,'Parameter|ed','prm:/BlockCalc/CM102/PT1006_1/ed','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','min',NULL,42,'Parameter|min','prm:/BlockCalc/CM102/F103/min','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','name','Graphics group element',42,'Parameter|NAME','prm:/BlockCalc/CM102/P103/NAME','el3','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','addr',NULL,42,'Parameter|var','prm:/BlockCalc/CM102/P103/var','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','ed',NULL,34,'Parameter|ed','prm:/BlockCalc/CM102/P103/ed','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','name','Graphics group element',42,'Parameter|NAME','prm:/BlockCalc/CM102/F103/NAME','el2','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','addr',NULL,42,'Parameter|var','prm:/BlockCalc/CM102/F103/var','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','ed',NULL,34,'Parameter|ed','prm:/BlockCalc/CM102/F103/ed','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','max',NULL,42,'Parameter|max','prm:/BlockCalc/CM102/F103/max','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','addr',NULL,42,'Parameter|var','prm:/BlockCalc/CM102/ST8612/var','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','ed',NULL,34,'Parameter|ed','prm:/BlockCalc/CM102/ST8612/ed','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','max',NULL,42,'Parameter|max','prm:/BlockCalc/CM102/ST8612/max','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','min',NULL,42,'Parameter|min','prm:/BlockCalc/CM102/ST8612/min','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','name','Graphics group element',42,'Parameter|NAME','prm:/BlockCalc/CM102/ST8612/NAME','el1','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn','name','Mnemos',0,'','','','Мнемосхеми','','','Мнемосхемы','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn','dscr','Logical containers of page group: "Mnemos"',0,'','','','Логічний контейнер групи сторінок: "Мнемосхеми"','','','Логический контейнер группы страниц: "Мнемосхемы"','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','name','CM102',32,'','','','КМ102','','','КМ102','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','pgGrp','so',32,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM102/TE1314_1/var','TAT101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','pName','',34,'Parameter|NAME','prm:/BlockCalc/CM102/TE1314_1/NAME','TAT101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','pErr','<EVAL>',34,'Parameter|err','prm:/BlockCalc/CM102/TE1314_1/err','TAT101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','text','AT102/2',32,'','','txtAT101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM102/TT0202/var','T_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','text','BC117',32,'','','txtKS107','КК117','','','КШ117','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','pName','T',33,'Parameter|NAME','T','T_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','st_close',NULL,38,'Parameter|st_close','prm:/BlockCalc/CM102/BC115/st_close','KS105','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','shifr',NULL,38,'Parameter|NAME','prm:/BlockCalc/CM102/BC115/NAME','KS105','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','text','C102/1',32,'','','Text7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','pName','T',33,'Parameter|NAME','T','T_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM102/TE1313_1/var','T_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','pName','T',33,'Parameter|NAME','T','T_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM102/TE1314_1/var','T_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','com','0',38,'Parameter|com','prm:/BlockCalc/CM102/BC111/com','KS101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','shifr',NULL,38,'Parameter|NAME','prm:/BlockCalc/CM102/BC111/NAME','KS101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','st_close',NULL,38,'Parameter|st_close','prm:/BlockCalc/CM102/BC111/st_close','KS101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','st_open',NULL,38,'Parameter|st_open','prm:/BlockCalc/CM102/BC111/st_open','KS101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','com','0',38,'Parameter|com','prm:/BlockCalc/CM102/BC112/com','KS102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','shifr',NULL,38,'Parameter|NAME','prm:/BlockCalc/CM102/BC112/NAME','KS102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','st_close',NULL,38,'Parameter|st_close','prm:/BlockCalc/CM102/BC112/st_close','KS102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','st_open',NULL,38,'Parameter|st_open','prm:/BlockCalc/CM102/BC112/st_open','KS102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','com','0',38,'Parameter|com','prm:/BlockCalc/CM102/BC114/com','KS104','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','shifr',NULL,38,'Parameter|NAME','prm:/BlockCalc/CM102/BC114/NAME','KS104','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','st_close',NULL,38,'Parameter|st_close','prm:/BlockCalc/CM102/BC114/st_close','KS104','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','st_open',NULL,38,'Parameter|st_open','prm:/BlockCalc/CM102/BC114/st_open','KS104','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','com','0',38,'Parameter|com','prm:/BlockCalc/CM102/BC115/com','KS105','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','text','BC115',32,'','','txtKS105','КК115','','','КШ115','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM102/P103/var','P_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','pName','P',33,'Parameter|NAME','P','P_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','st_open',NULL,38,'Parameter|st_open','prm:/BlockCalc/CM102/BC115/st_open','KS105','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','com','0',38,'Parameter|com','prm:/BlockCalc/CM102/BC116/com','KS106','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','shifr',NULL,38,'Parameter|NAME','prm:/BlockCalc/CM102/BC116/NAME','KS106','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','st_close',NULL,38,'Parameter|st_close','prm:/BlockCalc/CM102/BC116/st_close','KS106','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','st_open',NULL,38,'Parameter|st_open','prm:/BlockCalc/CM102/BC116/st_open','KS106','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','pName','P',33,'Parameter|NAME','P','P_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM102/PT1006_1/var','P_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','text','BC111',32,'','','txtKS101','КК111','','','КШ111','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','text','BC112',32,'','','txtKS102','КК112','','','КШ112','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','text','BC113',32,'','','txtKS103','КК113','','','КШ113','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','text','BC114',32,'','','txtKS104','КК114','','','КШ114','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','text','From PP5',32,'','','Text1','З PP5','','','Из PP5','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM102/ST8612/var','N_CM','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','text','KPP102',32,'','','txtKPP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','text','C102/2',32,'','','Text5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','text','To PP7',32,'','','Text6','До PP7','','','На PP7','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','pName','',34,'Parameter|NAME','prm:/BlockCalc/CM102/ST8612/NAME','N_CM','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','out','0',34,'Parameter|out','prm:/BlockCalc/CM102/KPP102/out','KPP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','text','C102/3',32,'','','Text4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','text','BC116',32,'','','Text3','КК116','','','КШ116','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM102/F103/var','F_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','pName','F',33,'Parameter|NAME','F','F_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM102/F104/var','F_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','pErr','<EVAL>',34,'Parameter|err','prm:/BlockCalc/CM102/ST8612/err','N_CM','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','text','CM102/2',32,'','','txtCM101_2','КМ102/2','','КМ102/2','КМ102/2','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','text','CM102/1',32,'','','txtCM101_1','КМ102/1','','КМ102/1','КМ102/1','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','pName','F',33,'Parameter|NAME','F','F_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p5_addr',NULL,34,'Parameter 5|var','prm:/BlockCalc/CM101/PT0204/var','e5','','Parameter 5|var','','','Parameter 5|var','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p4_name',NULL,34,'Parameter 4|NAME','prm:/BlockCalc/CM101/PT1006_1/NAME','e5',NULL,'Parameter 4|NAME','prm:/BlockCalc/CM101/PT1006_1/NAME',NULL,'Parameter 4|NAME','prm:/BlockCalc/CM101/PT1006_1/NAME');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p4_max',NULL,34,'Parameter 4|max','prm:/BlockCalc/CM101/PT1006_1/max','e5','','Parameter 4|max','','','Parameter 4|max','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p4_min',NULL,34,'Parameter 4|min','prm:/BlockCalc/CM101/PT1006_1/min','e5','','Parameter 4|min','','','Parameter 4|min','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_name',NULL,34,'Parameter 3|NAME','prm:/BlockCalc/CM101/P101/NAME','e5',NULL,'Parameter 3|NAME','prm:/BlockCalc/CM101/P101/NAME',NULL,'Parameter 3|NAME','prm:/BlockCalc/CM101/P101/NAME');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_min',NULL,34,'Parameter 3|min','prm:/BlockCalc/CM101/P101/min','e5','','Parameter 3|min','','','Parameter 3|min','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_max',NULL,34,'Parameter 3|max','prm:/BlockCalc/CM101/P101/max','e5','','Parameter 3|max','','','Parameter 3|max','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_addr',NULL,34,'Parameter 3|var','prm:/BlockCalc/CM101/P101/var','e5','','Parameter 3|var','','','Parameter 3|var','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_min',NULL,34,'Parameter 2|min','prm:/BlockCalc/CM101/F101/min','e5','','Parameter 2|min','','','Parameter 2|min','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_name',NULL,34,'Parameter 2|NAME','prm:/BlockCalc/CM101/F101/NAME','e5',NULL,'Parameter 2|NAME','prm:/BlockCalc/CM101/F101/NAME',NULL,'Parameter 2|NAME','prm:/BlockCalc/CM101/F101/NAME');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_max',NULL,34,'Parameter 2|max','prm:/BlockCalc/CM101/F101/max','e5','','Parameter 2|max','','','Parameter 2|max','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_addr',NULL,34,'Parameter 2|var','prm:/BlockCalc/CM101/F101/var','e5','','Parameter 2|var','','','Parameter 2|var','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_name',NULL,34,'Parameter 1|NAME','prm:/BlockCalc/CM101/ST8612/NAME','e5',NULL,'Parameter 1|NAME','prm:/BlockCalc/CM101/ST8612/NAME',NULL,'Parameter 1|NAME','prm:/BlockCalc/CM101/ST8612/NAME');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_max',NULL,34,'Parameter 1|max','prm:/BlockCalc/CM101/ST8612/max','e5','','Parameter 1|max','','','Parameter 1|max','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_min',NULL,34,'Parameter 1|min','prm:/BlockCalc/CM101/ST8612/min','e5','','Parameter 1|min','','','Parameter 1|min','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_addr',NULL,34,'Parameter 1|var','prm:/BlockCalc/CM101/ST8612/var','e5','','Parameter 1|var','','','Parameter 1|var','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','title','Mainstation (T)',33,'','Mainstation (T)','e3','Загальностанційка (T)','','Загальностанційка (T)','Общестанционка (T)','','Общестанционка (T)');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p4_name','<EVAL>',34,'Parameter 4|NAME','prm:/LogicLev/gen/T_PP5/NAME','e3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p4_min','<EVAL>',34,'Parameter 4|min','prm:/LogicLev/gen/T_PP5/min','e3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p4_max','<EVAL>',34,'Parameter 4|max','prm:/LogicLev/gen/T_PP5/max','e3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p4_addr','<EVAL>',34,'Parameter 4|var','prm:/LogicLev/gen/T_PP5/var','e3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_min','<EVAL>',34,'Parameter 3|min','prm:/LogicLev/gen/T_PP3/min','e3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_name','<EVAL>',34,'Parameter 3|NAME','prm:/LogicLev/gen/T_PP3/NAME','e3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p4_addr',NULL,34,'Parameter 4|var','prm:/BlockCalc/CM101/PT1006_1/var','e5','','Parameter 4|var','','','Parameter 4|var','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_max','<EVAL>',34,'Parameter 3|max','prm:/LogicLev/gen/T_PP3/max','e3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_addr','<EVAL>',34,'Parameter 3|var','prm:/LogicLev/gen/T_PP3/var','e3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_name','<EVAL>',34,'Parameter 2|NAME','prm:/LogicLev/gen/T_PP1/NAME','e3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_min','<EVAL>',34,'Parameter 2|min','prm:/LogicLev/gen/T_PP1/min','e3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_max','<EVAL>',34,'Parameter 2|max','prm:/LogicLev/gen/T_PP1/max','e3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_addr','<EVAL>',34,'Parameter 2|var','prm:/LogicLev/gen/T_PP1/var','e3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_name','<EVAL>',34,'Parameter 1|NAME','prm:/LogicLev/gen/Ti/NAME','e3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_min','<EVAL>',34,'Parameter 1|min','prm:/LogicLev/gen/Ti/min','e3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_max','<EVAL>',34,'Parameter 1|max','prm:/LogicLev/gen/Ti/max','e3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_addr','<EVAL>',34,'Parameter 1|var','prm:/LogicLev/gen/Ti/var','e3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','title','Mainstation (P, F)',33,'','Mainstation (P, F)','e2','Загальностанційка (P, F)','','Загальностанційка (P, F)','Общестанционка (P, F)','','Общестанционка (P, F)');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p5_name','<EVAL>',34,'Parameter 5|NAME','','e2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p5_min','<EVAL>',34,'Parameter 5|min','','e2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p5_max','<EVAL>',34,'Parameter 5|max','','e2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p5_addr','<EVAL>',34,'Parameter 5|var','','e2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p4_name','<EVAL>',34,'Parameter 4|NAME','','e2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p4_min','<EVAL>',34,'Parameter 4|min','','e2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p4_max','<EVAL>',34,'Parameter 4|max','','e2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p4_addr','<EVAL>',34,'Parameter 4|var','','e2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_name','<EVAL>',34,'Parameter 3|NAME','prm:/LogicLev/gen/F_PP1/NAME','e2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_min','<EVAL>',34,'Parameter 3|min','prm:/LogicLev/gen/F_PP1/min','e2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_max','<EVAL>',34,'Parameter 3|max','prm:/LogicLev/gen/F_PP1/max','e2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_addr','<EVAL>',34,'Parameter 3|var','prm:/LogicLev/gen/F_PP1/var','e2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_name','<EVAL>',34,'Parameter 2|NAME','prm:/BlockCalc/gen_cntr/PC_PCV3/NAME','e2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_min','<EVAL>',34,'Parameter 2|min','prm:/BlockCalc/gen_cntr/PC_PCV3/min','e2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_max','<EVAL>',34,'Parameter 2|max','prm:/BlockCalc/gen_cntr/PC_PCV3/max','e2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_addr','<EVAL>',34,'Parameter 2|var','prm:/BlockCalc/gen_cntr/PC_PCV3/var','e2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_name','<EVAL>',34,'Parameter 1|NAME','prm:/LogicLev/gen/P_PP5/NAME','e2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_min','<EVAL>',34,'Parameter 1|min','prm:/LogicLev/gen/P_PP5/min','e2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_max','<EVAL>',34,'Parameter 1|max','prm:/LogicLev/gen/P_PP5/max','e2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_addr','<EVAL>',34,'Parameter 1|var','prm:/LogicLev/gen/P_PP5/var','e2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','title','Mainstation (P)',33,'','Mainstation (P)','e1','Загальностанційка (P)','','Загальностанційка (P)','Общестанционка (P)','','Общестанционка (P)');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p5_name',NULL,34,'Parameter 5|NAME','prm:/BlockCalc/gen_cntr/PC_PCV2/NAME','e1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p5_min',NULL,34,'Parameter 5|min','prm:/BlockCalc/gen_cntr/PC_PCV2/min','e1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p5_max',NULL,34,'Parameter 5|max','prm:/BlockCalc/gen_cntr/PC_PCV2/max','e1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p5_addr',NULL,34,'Parameter 5|var','prm:/BlockCalc/gen_cntr/PC_PCV2/var','e1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p4_name',NULL,34,'Parameter 4|NAME','prm:/LogicLev/gen/P_PP3/NAME','e1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p4_min',NULL,34,'Parameter 4|min','prm:/LogicLev/gen/P_PP3/min','e1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p4_max',NULL,34,'Parameter 4|max','prm:/LogicLev/gen/P_PP3/max','e1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p4_addr',NULL,34,'Parameter 4|var','prm:/LogicLev/gen/P_PP3/var','e1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_name',NULL,34,'Parameter 3|NAME','prm:/LogicLev/gen/P_PP1/NAME','e1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_min',NULL,34,'Parameter 3|min','prm:/LogicLev/gen/P_PP1/min','e1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_max',NULL,34,'Parameter 3|max','prm:/LogicLev/gen/P_PP1/max','e1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_addr',NULL,34,'Parameter 3|var','prm:/LogicLev/gen/P_PP1/var','e1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_name',NULL,34,'Parameter 2|NAME','prm:/BlockCalc/gen_cntr/PC_PCV1/NAME','e1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_min',NULL,34,'Parameter 2|min','prm:/BlockCalc/gen_cntr/PC_PCV1/min','e1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_max',NULL,34,'Parameter 2|max','prm:/BlockCalc/gen_cntr/PC_PCV1/max','e1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_addr',NULL,34,'Parameter 2|var','prm:/BlockCalc/gen_cntr/PC_PCV1/var','e1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_name',NULL,34,'Parameter 1|NAME','prm:/LogicLev/gen/Pi/NAME','e1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_min',NULL,34,'Parameter 1|min','prm:/LogicLev/gen/Pi/min','e1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_max',NULL,34,'Parameter 1|max','prm:/LogicLev/gen/Pi/max','e1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_clr','#b000b0',96,'Parameter 1','#b000b0','e1','','Параметр 1','','','Параметр 1','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_addr',NULL,34,'Parameter 1|var','prm:/LogicLev/gen/Pi/var','e1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmSp','0',38,'Parameter|sp','prm:/BlockCalc/gen_cntr/PC_PCV3/sp','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmShifr','',34,'Parameter|NAME','prm:/BlockCalc/gen_cntr/PC_PCV3/NAME','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmOpenSt','<EVAL>',34,'Parameter|st_open','prm:/BlockCalc/gen_cntr/PC_PCV3','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmDemention','',34,'Parameter|ed','prm:/BlockCalc/gen_cntr/PC_PCV3/ed','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmAddr','<EVAL>',42,'Parameter|var','prm:/LogicLev/gen/P_PP5/var','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmVar','0',38,'Parameter|var','prm:/LogicLev/gen/P_PP5/var','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmStop','<EVAL>',38,'Parameter|stop','prm:/LogicLev/gen/P_PP5','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmShifr','',34,'Parameter|NAME','prm:/LogicLev/gen/P_PP5/NAME','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmOpenSt','<EVAL>',34,'Parameter|st_open','prm:/LogicLev/gen/P_PP5','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmCom','<EVAL>',38,'Parameter|com','prm:/LogicLev/gen/P_PP5','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','max','100',42,'Parameter|max','prm:/LogicLev/gen/P_PP5/max','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmAddr','<EVAL>',42,'Parameter|var','prm:/BlockCalc/gen_cntr/PC_PCV2/var','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmVar','0',38,'Parameter|var','prm:/BlockCalc/gen_cntr/PC_PCV2/var','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmStop','<EVAL>',38,'Parameter|stop','prm:/BlockCalc/gen_cntr/PC_PCV2','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmShifr','',34,'Parameter|NAME','prm:/BlockCalc/gen_cntr/PC_PCV2/NAME','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmOut','0',38,'Parameter|out','prm:/BlockCalc/gen_cntr/PC_PCV2/out','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmClose','<EVAL>',38,'Parameter|close','prm:/BlockCalc/gen_cntr/PC_PCV2','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmCasc','0',38,'Parameter|casc','prm:/BlockCalc/gen_cntr/PC_PCV2','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmAuto','0',38,'Parameter|auto','prm:/BlockCalc/gen_cntr/PC_PCV2/auto','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','min','0',42,'Parameter|min','prm:/BlockCalc/gen_cntr/PC_PCV2/min','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmAddr','<EVAL>',42,'Parameter|var','prm:/LogicLev/gen/P_PP3/var','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmStop','<EVAL>',38,'Parameter|stop','prm:/LogicLev/gen/P_PP3','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmOut','0',38,'Parameter|out','prm:/LogicLev/gen/P_PP3','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmOpenSt','<EVAL>',34,'Parameter|st_open','prm:/LogicLev/gen/P_PP3','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmDemention','',34,'Parameter|ed','prm:/LogicLev/gen/P_PP3/ed','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmCom','<EVAL>',38,'Parameter|com','prm:/LogicLev/gen/P_PP3','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmCloseSt','<EVAL>',34,'Parameter|st_close','prm:/LogicLev/gen/P_PP3','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmClose','<EVAL>',38,'Parameter|close','prm:/LogicLev/gen/P_PP3','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmCasc','0',38,'Parameter|casc','prm:/LogicLev/gen/P_PP3','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmAuto','0',38,'Parameter|auto','prm:/LogicLev/gen/P_PP3','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','min','0',42,'Parameter|min','prm:/LogicLev/gen/P_PP3/min','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','max','100',42,'Parameter|max','prm:/LogicLev/gen/P_PP3/max','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmAddr','<EVAL>',42,'Parameter|var','prm:/LogicLev/gen/P_PP1/var','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmVar','0',38,'Parameter|var','prm:/LogicLev/gen/P_PP1/var','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmStop','<EVAL>',38,'Parameter|stop','prm:/LogicLev/gen/P_PP1','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmShifr','',34,'Parameter|NAME','prm:/LogicLev/gen/P_PP1/NAME','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmSp','0',38,'Parameter|sp','prm:/LogicLev/gen/P_PP1','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmOut','0',38,'Parameter|out','prm:/LogicLev/gen/P_PP1','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmOpenSt','<EVAL>',34,'Parameter|st_open','prm:/LogicLev/gen/P_PP1','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmDemention','',34,'Parameter|ed','prm:/LogicLev/gen/P_PP1/ed','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmCom','<EVAL>',38,'Parameter|com','prm:/LogicLev/gen/P_PP1','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmCloseSt','<EVAL>',34,'Parameter|st_close','prm:/LogicLev/gen/P_PP1','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmClose','<EVAL>',38,'Parameter|close','prm:/LogicLev/gen/P_PP1','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmCasc','0',38,'Parameter|casc','prm:/LogicLev/gen/P_PP1','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmAuto','0',38,'Parameter|auto','prm:/LogicLev/gen/P_PP1','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','min','0',42,'Parameter|min','prm:/LogicLev/gen/P_PP1/min','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','max','100',42,'Parameter|max','prm:/LogicLev/gen/P_PP1/max','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmAddr','<EVAL>',42,'Parameter|var','prm:/BlockCalc/gen_cntr/PC_PCV1/var','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmVar','0',38,'Parameter|var','prm:/BlockCalc/gen_cntr/PC_PCV1/var','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmStop','<EVAL>',38,'Parameter|stop','prm:/BlockCalc/gen_cntr/PC_PCV1','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmSp','0',38,'Parameter|sp','prm:/BlockCalc/gen_cntr/PC_PCV1/sp','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmShifr','',34,'Parameter|NAME','prm:/BlockCalc/gen_cntr/PC_PCV1/NAME','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmOut','0',38,'Parameter|out','prm:/BlockCalc/gen_cntr/PC_PCV1/out','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmOpenSt','<EVAL>',34,'Parameter|st_open','prm:/BlockCalc/gen_cntr/PC_PCV1','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmDemention','',34,'Parameter|ed','prm:/BlockCalc/gen_cntr/PC_PCV1/ed','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmCom','<EVAL>',38,'Parameter|com','prm:/BlockCalc/gen_cntr/PC_PCV1','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmCloseSt','<EVAL>',34,'Parameter|st_close','prm:/BlockCalc/gen_cntr/PC_PCV1','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmClose','<EVAL>',38,'Parameter|close','prm:/BlockCalc/gen_cntr/PC_PCV1','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmCasc','0',38,'Parameter|casc','prm:/BlockCalc/gen_cntr/PC_PCV1','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmAuto','0',38,'Parameter|auto','prm:/BlockCalc/gen_cntr/PC_PCV1/auto','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','min','0',42,'Parameter|min','prm:/BlockCalc/gen_cntr/PC_PCV1/min','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmAddr','<EVAL>',42,'Parameter|var','prm:/LogicLev/gen/Pi/var','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','max','100',42,'Parameter|max','prm:/BlockCalc/gen_cntr/PC_PCV1/max','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmVar','0',38,'Parameter|var','prm:/LogicLev/gen/Pi/var','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmStop','<EVAL>',38,'Parameter|stop','prm:/LogicLev/gen/Pi','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmSp','0',38,'Parameter|sp','prm:/LogicLev/gen/Pi','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmShifr','',34,'Parameter|NAME','prm:/LogicLev/gen/Pi/NAME','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmOut','0',38,'Parameter|out','prm:/LogicLev/gen/Pi','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmOpenSt','<EVAL>',34,'Parameter|st_open','prm:/LogicLev/gen/Pi','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmDemention','',34,'Parameter|ed','prm:/LogicLev/gen/Pi/ed','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmCom','<EVAL>',38,'Parameter|com','prm:/LogicLev/gen/Pi','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmCloseSt','<EVAL>',34,'Parameter|st_close','prm:/LogicLev/gen/Pi','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmClose','<EVAL>',38,'Parameter|close','prm:/LogicLev/gen/Pi','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmCasc','0',38,'Parameter|casc','prm:/LogicLev/gen/Pi','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmAuto','0',38,'Parameter|auto','prm:/LogicLev/gen/Pi','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','max','100',42,'Parameter|max','prm:/LogicLev/gen/Pi/max','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','min','0',42,'Parameter|min','prm:/LogicLev/gen/Pi/min','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','max',NULL,42,'Parameter|max','prm:/LogicLev/gen/F_PP1/max','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','shifr',NULL,38,'Parameter|NAME','prm:/BlockCalc/gen/BC22/NAME','BC22','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','st_close',NULL,38,'Parameter|st_close','prm:/BlockCalc/gen/BC21/st_close','BC21','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','shifr',NULL,38,'Parameter|NAME','prm:/BlockCalc/gen/BC7/NAME','BC7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','com','0',38,'Parameter|com','prm:/BlockCalc/gen/BC7/com','BC7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','st_open',NULL,38,'Parameter|st_open','prm:/BlockCalc/gen/BC5/st_open','BC5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','com','0',38,'Parameter|com','prm:/BlockCalc/gen/BC6/com','BC6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','shifr',NULL,38,'Parameter|NAME','prm:/BlockCalc/gen/BC6/NAME','BC6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pName','T',33,'Parameter|NAME','T','T_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pName','P',33,'Parameter|NAME','P','P_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','st_close',NULL,38,'Parameter|st_close','prm:/BlockCalc/CM101/BC106/st_close','KS106','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','st_open',NULL,38,'Parameter|st_open','prm:/BlockCalc/CM101/BC105/st_open','KS105','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','st_open',NULL,38,'Parameter|st_open','prm:/BlockCalc/CM101/BC104/st_open','KS104','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','text','AT102/1',32,'','','txtAT101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','geomY','301',32,'','','txtAT101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','text','BC134',32,'','','txtKS123','КК134','','','КШ134','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','text','BC126',32,'','','txtKS122','КК126','','','КШ126','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM102/TT0204/var','T_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','text','BC125',32,'','','txtKS121','КК125','','','КШ125','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','pName','T',33,'Parameter|NAME','T','T_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_grph_panel','value','0.01',8,'','','size','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_grph_panel','value','0',8,'','','date','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gview/pg_1','dscr','Pages template: "Overview frames group"',32,'','','','Шаблон сторінок: "Група оглядових кадрів"','','','Pages'' template: "Overview frames group"','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','min',NULL,42,'Parameter|min','prm:/LogicLev/gen/F_PP1/min','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','curSek','1285253917',40,'','','trnd1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','pModeA',NULL,34,'Parameter|auto','prm:/LogicLev/gen/T_PP1','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','min',NULL,42,'Parameter|min','prm:/LogicLev/gen/T_PP1/min','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','name','Temperatures',41,'','','','Температури','','','Температуры','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmAuto','0',38,'Parameter|auto','prm:/LogicLev/gen/F_PP1','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmCasc','0',38,'Parameter|casc','prm:/LogicLev/gen/F_PP1','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmClose','<EVAL>',38,'Parameter|close','prm:/LogicLev/gen/F_PP1','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmCloseSt','<EVAL>',34,'Parameter|st_close','prm:/LogicLev/gen/F_PP1','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmCom','<EVAL>',38,'Parameter|com','prm:/LogicLev/gen/F_PP1','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmDemention','',34,'Parameter|ed','prm:/LogicLev/gen/F_PP1/ed','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmOpenSt','<EVAL>',34,'Parameter|st_open','prm:/LogicLev/gen/F_PP1','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmOut','0',38,'Parameter|out','prm:/LogicLev/gen/F_PP1','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmShifr','',34,'Parameter|NAME','prm:/LogicLev/gen/F_PP1/NAME','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmSp','0',38,'Parameter|sp','prm:/LogicLev/gen/F_PP1','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmStop','<EVAL>',38,'Parameter|stop','prm:/LogicLev/gen/F_PP1','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmVar','0',38,'Parameter|var','prm:/LogicLev/gen/F_PP1/var','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmAddr','<EVAL>',42,'Parameter|var','prm:/LogicLev/gen/F_PP1/var','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gview/pg_1','var','<EVAL>',34,'Parameter|var','prm:/LogicLev/gen/P4/var','el1_4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','name','Graphics group element',42,'Parameter|NAME','prm:/BlockCalc/CM101/ST8612/NAME','el1','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','addr',NULL,42,'Parameter|var','prm:/BlockCalc/CM101/ST8612/var','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','pModeA',NULL,34,'Parameter|auto','prm:/BlockCalc/CM101/ST8612','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','pModeC',NULL,34,'Parameter|casc','prm:/BlockCalc/CM101/ST8612','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','aMax',NULL,34,'Parameter|aMax','prm:/BlockCalc/CM101/ST8612','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','aMin',NULL,34,'Parameter|aMin','prm:/BlockCalc/CM101/ST8612','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','name','CM101',41,'','','','КМ101','','','КМ101','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pErr','<EVAL>',34,'Parameter|err','prm:/BlockCalc/CM101/TE1314_2/err','TAT101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pName','',34,'Parameter|NAME','prm:/BlockCalc/CM101/TE1314_2/NAME','TAT101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM101/TE1314_2/var','TAT101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pName','P',33,'Parameter|NAME','P','P_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM101/PT0202/var','P_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pName','P',33,'Parameter|NAME','P','P_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM101/PT0204/var','P_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','addr',NULL,42,'Parameter|var','prm:/BlockCalc/CM102/TE1314_2/var','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','ed',NULL,34,'Parameter|ed','prm:/BlockCalc/CM102/TE1314_2/ed','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','max',NULL,42,'Parameter|max','prm:/BlockCalc/CM102/TE1314_2/max','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','min',NULL,42,'Parameter|min','prm:/BlockCalc/CM102/TE1314_2/min','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','curSek','1283328064',40,'','','trnd1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','pErr','<EVAL>',34,'Parameter|err','prm:/BlockCalc/CM102/TE1314_2/err','TAT101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','pName','',34,'Parameter|NAME','prm:/BlockCalc/CM102/TE1314_2/NAME','TAT101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM102/TE1314_2/var','TAT101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','pName','P',33,'Parameter|NAME','P','P_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM102/PT0202/var','P_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','pName','P',33,'Parameter|NAME','P','P_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM102/PT0204/var','P_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','text','PP103',32,'','','txtPP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','text','PP104',32,'','','txtPP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','title','CM102 (T)',33,'','CM102 (T)','e8','КМ102 (T)','','КМ102 (T)','КМ102 (T)','','КМ102 (T)');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','K4','<EVAL>',46,'<page>|K4','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','prec','1',34,'Parameter|prec','prm:/LogicLev/gen/P_PP1/prec','P_PP1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmVarIn','<EVAL>',38,'Parameter|varIn','prm:/LogicLev/gen/P_PP1','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmVarIn','<EVAL>',38,'Parameter|varIn','prm:/LogicLev/gen/P_PP3','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmVarIn','<EVAL>',38,'Parameter|varIn','prm:/LogicLev/gen/P_PP5','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmVarIn','<EVAL>',38,'Parameter|varIn','prm:/LogicLev/gen/F_PP1','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','min',NULL,42,'Parameter|min','prm:/BlockCalc/CM101/PT0204/min','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','prec',NULL,34,'Parameter|prec','prm:/BlockCalc/CM101/PT0204/prec','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','max',NULL,42,'Parameter|max','prm:/BlockCalc/CM101/PT0204/max','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','aMin',NULL,34,'Parameter|aMin','prm:/BlockCalc/CM101/PT0204','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','ed',NULL,34,'Parameter|ed','prm:/BlockCalc/CM101/PT0204/ed','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','wMax',NULL,34,'Parameter|wMax','prm:/BlockCalc/CM101/PT1006_1','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','wMax',NULL,34,'Parameter|wMax','prm:/LogicLev/gen/T_PP3/wMax','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','ed',NULL,34,'Parameter|ed','prm:/BlockCalc/CM101/ST8612/ed','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','wMin',NULL,34,'Parameter|wMin','prm:/LogicLev/gen/T_PP3/wMin','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','min',NULL,42,'Parameter|min','prm:/BlockCalc/CM101/PT1006_1/min','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','name','Graphics group element',42,'Parameter|NAME','prm:/LogicLev/gen/T_PP5/NAME','el4','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so','backColor','ivory',96,'backColor','','','','backColor','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_ElCadr','prmManIn','<EVAL>',46,'<page>|manIn','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','name','Graphics group element',42,'Parameter|NAME','prm:/BlockCalc/CM101/TE1313_1/NAME','el6','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','addr',NULL,42,'Parameter|var','prm:/BlockCalc/CM101/TE1313_1/var','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','pModeA',NULL,34,'Parameter|auto','prm:/BlockCalc/CM101/TE1313_1','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','pModeC',NULL,34,'Parameter|casc','prm:/BlockCalc/CM101/TE1313_1','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','aMax',NULL,34,'Parameter|aMax','prm:/BlockCalc/CM101/TE1313_1','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','prec',NULL,34,'Parameter|prec','prm:/LogicLev/gen/T_PP3','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','aMin',NULL,34,'Parameter|aMin','prm:/BlockCalc/CM101/TE1313_1','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','ed',NULL,34,'Parameter|ed','prm:/BlockCalc/CM101/TE1313_1/ed','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','max',NULL,42,'Parameter|max','prm:/BlockCalc/CM101/TE1313_1/max','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','addr',NULL,42,'Parameter|var','prm:/LogicLev/gen/Ti/var','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','aMin',NULL,34,'Parameter|aMin','prm:/LogicLev/gen/Ti/aMin','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','aMax',NULL,34,'Parameter|aMax','prm:/LogicLev/gen/Ti/aMax','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','name','Graphics group element',42,'Parameter|NAME','prm:/LogicLev/gen/Ti/NAME','el1','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pModeA','<EVAL>',34,'Parameter|auto','prm:/BlockCalc/gen_cntr/PC_PCV1/auto','PC1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pModeC','<EVAL>',34,'Parameter|casc','prm:/BlockCalc/gen_cntr/PC_PCV1','PC1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pPrec','<EVAL>',34,'Parameter|prec','prm:/BlockCalc/gen_cntr/PC_PCV1/prec','PC1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pModeA','<EVAL>',34,'Parameter|auto','prm:/BlockCalc/gen_cntr/PC_PCV2/auto','PC2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pModeC','<EVAL>',34,'Parameter|casc','prm:/BlockCalc/gen_cntr/PC_PCV2','PC2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pPrec','<EVAL>',34,'Parameter|prec','prm:/BlockCalc/gen_cntr/PC_PCV2/prec','PC2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pModeA','<EVAL>',34,'Parameter|auto','prm:/BlockCalc/gen_cntr/PC_PCV3/auto','PC3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pModeC','<EVAL>',34,'Parameter|casc','prm:/BlockCalc/gen_cntr/PC_PCV3','PC3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pPrec','<EVAL>',34,'Parameter|prec','prm:/BlockCalc/gen_cntr/PC_PCV3/prec','PC3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_10','pModeA','<EVAL>',34,'Parameter|auto','prm:/LogicLev/gen/F3','Pi_нд','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_10','pModeC','<EVAL>',34,'Parameter|casc','prm:/LogicLev/gen/F3','Pi_нд','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_10','pPrec','<EVAL>',34,'Parameter|prec','val:2','Pi_нд','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_10','pModeA','<EVAL>',34,'Parameter|auto','prm:/LogicLev/gen/P3','Ti_нд','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_10','pModeC','<EVAL>',34,'Parameter|casc','prm:/LogicLev/gen/P3','Ti_нд','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_10','pPrec','<EVAL>',34,'Parameter|prec','val:2','Ti_нд','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pModeA','<EVAL>',34,'Parameter|auto','prm:/BlockCalc/CM101/ST8612','N_CM','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pModeC','<EVAL>',34,'Parameter|casc','prm:/BlockCalc/CM101/ST8612','N_CM','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pPrec','<EVAL>',34,'Parameter|prec','prm:/BlockCalc/CM101/ST8612/prec','N_CM','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pModeA','<EVAL>',34,'Parameter|auto','prm:/BlockCalc/CM101/TE1314_1','TAT101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pModeC','<EVAL>',34,'Parameter|casc','prm:/BlockCalc/CM101/TE1314_1','TAT101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pPrec','<EVAL>',34,'Parameter|prec','prm:/BlockCalc/CM101/TE1314_1/prec','TAT101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pModeA','<EVAL>',34,'Parameter|auto','prm:/BlockCalc/CM101/TE1314_2','TAT101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pModeC','<EVAL>',34,'Parameter|casc','prm:/BlockCalc/CM101/TE1314_2','TAT101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pPrec','<EVAL>',34,'Parameter|prec','prm:/BlockCalc/CM101/TE1314_2/prec','TAT101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','pModeA','<EVAL>',34,'Parameter|auto','prm:/BlockCalc/CM102/ST8612','N_CM','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','pModeC','<EVAL>',34,'Parameter|casc','prm:/BlockCalc/CM102/ST8612','N_CM','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','pPrec','<EVAL>',34,'Parameter|prec','prm:/BlockCalc/CM102/ST8612/prec','N_CM','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','pModeA','<EVAL>',34,'Parameter|auto','prm:/BlockCalc/CM102/TE1314_1','TAT101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','pModeC','<EVAL>',34,'Parameter|casc','prm:/BlockCalc/CM102/TE1314_1','TAT101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','pPrec','<EVAL>',34,'Parameter|prec','prm:/BlockCalc/CM102/TE1314_1/prec','TAT101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','pModeA','<EVAL>',34,'Parameter|auto','prm:/BlockCalc/CM102/TE1314_2','TAT101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','pModeC','<EVAL>',34,'Parameter|casc','prm:/BlockCalc/CM102/TE1314_2','TAT101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','pPrec','<EVAL>',34,'Parameter|prec','prm:/BlockCalc/CM102/TE1314_2/prec','TAT101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_ElCadr','prmAMax','<EVAL>',42,'<page>|aMax','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_ElCadr','prmAMin','<EVAL>',42,'<page>|aMin','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_ElCadr','prmAnalog','<EVAL>',42,'<page>|analog','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_ElCadr','prmColor','<EVAL>',42,'<page>|color','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_ElCadr','prmDescr','<EVAL>',42,'<page>|DESCR','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_ElCadr','prmId','<EVAL>',42,'<page>|SHIFR','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_ElCadr','prmImpQdwnTm','<EVAL>',42,'<page>|impQdwn','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_ElCadr','prmImpQupTm','<EVAL>',42,'<page>|impQup','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_ElCadr','prmPrec','<EVAL>',42,'<page>|prec','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_ElCadr','prmWMax','<EVAL>',42,'<page>|wMax','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_ElCadr','prmWMin','<EVAL>',42,'<page>|wMin','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_ElCadr','digComs','OPEN;CLOSE;STOP',42,'<page>|digComs','','','ВІДКР;ЗАКР;СТОП','','','ОТКР;ЗАКР;СТОП','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_ElCadr','digStts','<EVAL>',42,'<page>|digStts','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmAMax','<EVAL>',34,'Parameter|aMax','prm:/LogicLev/gen/Pi/aMax','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmAMin','<EVAL>',34,'Parameter|aMin','prm:/LogicLev/gen/Pi/aMin','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmAnalog','<EVAL>',34,'Parameter|analog','prm:/LogicLev/gen/Pi','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmColor','<EVAL>',34,'Parametr|color','prm:/LogicLev/gen/Pi','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmDescr','<EVAL>',34,'Parameter|DESCR','prm:/LogicLev/gen/Pi/DESCR','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmId','<EVAL>',34,'Parameter|SHIFR','prm:/LogicLev/gen/Pi/SHIFR','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmImpQdwnTm','<EVAL>',34,'Parameter|impQdwn','prm:/LogicLev/gen/Pi','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmImpQupTm','<EVAL>',34,'Parameter|impQup','prm:/LogicLev/gen/Pi','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmPrec','<EVAL>',34,'Parameter|prec','prm:/LogicLev/gen/Pi','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmWMax','<EVAL>',34,'Parameter|wMax','prm:/LogicLev/gen/Pi/wMax','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmWMin','<EVAL>',34,'Parameter|wMin','prm:/LogicLev/gen/Pi/wMin','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','digComs','OPEN;CLOSE;STOP',34,'Parameter|digComs','prm:/LogicLev/gen/Pi','el1','ВІДКР;ЗАКР;СТОП','','','ОТКР;ЗАКР;СТОП','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','digStts','<EVAL>',34,'Parameter|digStts','prm:/LogicLev/gen/Pi','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmManIn','<EVAL>',38,'Parameter|manIn','prm:/BlockCalc/gen_cntr/PC_PCV1/manIn','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmAMax','<EVAL>',34,'Parameter|aMax','prm:/BlockCalc/gen_cntr/PC_PCV1','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmAMin','<EVAL>',34,'Parameter|aMin','prm:/BlockCalc/gen_cntr/PC_PCV1','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmAnalog','<EVAL>',34,'Parameter|analog','prm:/BlockCalc/gen_cntr/PC_PCV1','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmColor','<EVAL>',34,'Parametr|color','prm:/BlockCalc/gen_cntr/PC_PCV1','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmDescr','<EVAL>',34,'Parameter|DESCR','prm:/BlockCalc/gen_cntr/PC_PCV1/DESCR','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmId','<EVAL>',34,'Parameter|SHIFR','prm:/BlockCalc/gen_cntr/PC_PCV1/SHIFR','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmImpQdwnTm','<EVAL>',34,'Parameter|impQdwn','prm:/BlockCalc/gen_cntr/PC_PCV1','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmImpQupTm','<EVAL>',34,'Parameter|impQup','prm:/BlockCalc/gen_cntr/PC_PCV1','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmPrec','<EVAL>',34,'Parameter|prec','prm:/BlockCalc/gen_cntr/PC_PCV1/prec','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmWMax','<EVAL>',34,'Parameter|wMax','prm:/BlockCalc/gen_cntr/PC_PCV1','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmWMin','<EVAL>',34,'Parameter|wMin','prm:/BlockCalc/gen_cntr/PC_PCV1','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','digComs','OPEN;CLOSE;STOP',34,'Parameter|digComs','prm:/BlockCalc/gen_cntr/PC_PCV1','el2','ВІДКР;ЗАКР;СТОП','','','ОТКР;ЗАКР;СТОП','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','digStts','<EVAL>',34,'Parameter|digStts','prm:/BlockCalc/gen_cntr/PC_PCV1','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmManIn','<EVAL>',38,'Parameter|manIn','prm:/LogicLev/gen/P_PP1','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmAMax','<EVAL>',34,'Parameter|aMax','prm:/LogicLev/gen/P_PP1/aMax','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmAMin','<EVAL>',34,'Parameter|aMin','prm:/LogicLev/gen/P_PP1/aMin','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmAnalog','<EVAL>',34,'Parameter|analog','prm:/LogicLev/gen/P_PP1','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmColor','<EVAL>',34,'Parametr|color','prm:/LogicLev/gen/P_PP1','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmDescr','<EVAL>',34,'Parameter|DESCR','prm:/LogicLev/gen/P_PP1/DESCR','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmId','<EVAL>',34,'Parameter|SHIFR','prm:/LogicLev/gen/P_PP1/SHIFR','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmImpQdwnTm','<EVAL>',34,'Parameter|impQdwn','prm:/LogicLev/gen/P_PP1','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmImpQupTm','<EVAL>',34,'Parameter|impQup','prm:/LogicLev/gen/P_PP1','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmPrec','<EVAL>',34,'Parameter|prec','prm:/LogicLev/gen/P_PP1','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmWMax','<EVAL>',34,'Parameter|wMax','prm:/LogicLev/gen/P_PP1/wMax','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmWMin','<EVAL>',34,'Parameter|wMin','prm:/LogicLev/gen/P_PP1/wMin','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','digComs','OPEN;CLOSE;STOP',34,'Parameter|digComs','prm:/LogicLev/gen/P_PP1','el3','ВІДКР;ЗАКР;СТОП','','','ОТКР;ЗАКР;СТОП','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','digStts','<EVAL>',34,'Parameter|digStts','prm:/LogicLev/gen/P_PP1','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmManIn','<EVAL>',38,'Parameter|manIn','prm:/LogicLev/gen/P_PP3','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmAMax','<EVAL>',34,'Parameter|aMax','prm:/LogicLev/gen/P_PP3/aMax','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmAMin','<EVAL>',34,'Parameter|aMin','prm:/LogicLev/gen/P_PP3/aMin','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmAnalog','<EVAL>',34,'Parameter|analog','prm:/LogicLev/gen/P_PP3','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmColor','<EVAL>',34,'Parametr|color','prm:/LogicLev/gen/P_PP3','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmDescr','<EVAL>',34,'Parameter|DESCR','prm:/LogicLev/gen/P_PP3/DESCR','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmId','<EVAL>',34,'Parameter|SHIFR','prm:/LogicLev/gen/P_PP3/SHIFR','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmImpQdwnTm','<EVAL>',34,'Parameter|impQdwn','prm:/LogicLev/gen/P_PP3','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmImpQupTm','<EVAL>',34,'Parameter|impQup','prm:/LogicLev/gen/P_PP3','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmPrec','<EVAL>',34,'Parameter|prec','prm:/LogicLev/gen/P_PP3','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmWMax','<EVAL>',34,'Parameter|wMax','prm:/LogicLev/gen/P_PP3/wMax','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmWMin','<EVAL>',34,'Parameter|wMin','prm:/LogicLev/gen/P_PP3/wMin','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','digComs','OPEN;CLOSE;STOP',34,'Parameter|digComs','prm:/LogicLev/gen/P_PP3','el4','ВІДКР;ЗАКР;СТОП','','','ОТКР;ЗАКР;СТОП','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','digStts','<EVAL>',34,'Parameter|digStts','prm:/LogicLev/gen/P_PP3','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmManIn','<EVAL>',38,'Parameter|manIn','prm:/BlockCalc/gen_cntr/PC_PCV2/manIn','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmAMax','<EVAL>',34,'Parameter|aMax','prm:/BlockCalc/gen_cntr/PC_PCV2','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmAMin','<EVAL>',34,'Parameter|aMin','prm:/BlockCalc/gen_cntr/PC_PCV2','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmAnalog','<EVAL>',34,'Parameter|analog','prm:/BlockCalc/gen_cntr/PC_PCV2','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmColor','<EVAL>',34,'Parametr|color','prm:/BlockCalc/gen_cntr/PC_PCV2','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmDescr','<EVAL>',34,'Parameter|DESCR','prm:/BlockCalc/gen_cntr/PC_PCV2/DESCR','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmId','<EVAL>',34,'Parameter|SHIFR','prm:/BlockCalc/gen_cntr/PC_PCV2/SHIFR','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmImpQdwnTm','<EVAL>',34,'Parameter|impQdwn','prm:/BlockCalc/gen_cntr/PC_PCV2','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmImpQupTm','<EVAL>',34,'Parameter|impQup','prm:/BlockCalc/gen_cntr/PC_PCV2','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmPrec','<EVAL>',34,'Parameter|prec','prm:/BlockCalc/gen_cntr/PC_PCV2/prec','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmWMax','<EVAL>',34,'Parameter|wMax','prm:/BlockCalc/gen_cntr/PC_PCV2','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmWMin','<EVAL>',34,'Parameter|wMin','prm:/BlockCalc/gen_cntr/PC_PCV2','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','digComs','OPEN;CLOSE;STOP',34,'Parameter|digComs','prm:/BlockCalc/gen_cntr/PC_PCV2','el5','ВІДКР;ЗАКР;СТОП','','','ОТКР;ЗАКР;СТОП','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','digStts','<EVAL>',34,'Parameter|digStts','prm:/BlockCalc/gen_cntr/PC_PCV2','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmManIn','<EVAL>',38,'Parameter|manIn','prm:/LogicLev/gen/P_PP5','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmAMax','<EVAL>',34,'Parameter|aMax','prm:/LogicLev/gen/P_PP5/aMax','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmAMin','<EVAL>',34,'Parameter|aMin','prm:/LogicLev/gen/P_PP5/aMin','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmAnalog','<EVAL>',34,'Parameter|analog','prm:/LogicLev/gen/P_PP5','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmColor','<EVAL>',34,'Parametr|color','prm:/LogicLev/gen/P_PP5','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmDescr','<EVAL>',34,'Parameter|DESCR','prm:/LogicLev/gen/P_PP5/DESCR','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmId','<EVAL>',34,'Parameter|SHIFR','prm:/LogicLev/gen/P_PP5/SHIFR','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmImpQdwnTm','<EVAL>',34,'Parameter|impQdwn','prm:/LogicLev/gen/P_PP5','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmImpQupTm','<EVAL>',34,'Parameter|impQup','prm:/LogicLev/gen/P_PP5','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmPrec','<EVAL>',34,'Parameter|prec','prm:/LogicLev/gen/P_PP5','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmWMax','<EVAL>',34,'Parameter|wMax','prm:/LogicLev/gen/P_PP5/wMax','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmWMin','<EVAL>',34,'Parameter|wMin','prm:/LogicLev/gen/P_PP5/wMin','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','digComs','OPEN;CLOSE;STOP',34,'Parameter|digComs','prm:/LogicLev/gen/P_PP5','el6','ВІДКР;ЗАКР;СТОП','','','ОТКР;ЗАКР;СТОП','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','digStts','<EVAL>',34,'Parameter|digStts','prm:/LogicLev/gen/P_PP5','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmManIn','<EVAL>',38,'Parameter|manIn','prm:/BlockCalc/gen_cntr/PC_PCV3/manIn','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmAMax','<EVAL>',34,'Parameter|aMax','prm:/BlockCalc/gen_cntr/PC_PCV3','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmAMin','<EVAL>',34,'Parameter|aMin','prm:/BlockCalc/gen_cntr/PC_PCV3','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmAnalog','<EVAL>',34,'Parameter|analog','prm:/BlockCalc/gen_cntr/PC_PCV3','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmColor','<EVAL>',34,'Parametr|color','prm:/BlockCalc/gen_cntr/PC_PCV3','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmDescr','<EVAL>',34,'Parameter|DESCR','prm:/BlockCalc/gen_cntr/PC_PCV3/DESCR','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmId','<EVAL>',34,'Parameter|SHIFR','prm:/BlockCalc/gen_cntr/PC_PCV3/SHIFR','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmImpQdwnTm','<EVAL>',34,'Parameter|impQdwn','prm:/BlockCalc/gen_cntr/PC_PCV3','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmImpQupTm','<EVAL>',34,'Parameter|impQup','prm:/BlockCalc/gen_cntr/PC_PCV3','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmPrec','<EVAL>',34,'Parameter|prec','prm:/BlockCalc/gen_cntr/PC_PCV3/prec','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmWMax','<EVAL>',34,'Parameter|wMax','prm:/BlockCalc/gen_cntr/PC_PCV3','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmWMin','<EVAL>',34,'Parameter|wMin','prm:/BlockCalc/gen_cntr/PC_PCV3','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','digComs','OPEN;CLOSE;STOP',34,'Parameter|digComs','prm:/BlockCalc/gen_cntr/PC_PCV3','el7','ВІДКР;ЗАКР;СТОП','','','ОТКР;ЗАКР;СТОП','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','digStts','<EVAL>',34,'Parameter|digStts','prm:/BlockCalc/gen_cntr/PC_PCV3','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmManIn','<EVAL>',38,'Parameter|manIn','prm:/LogicLev/gen/F_PP1','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmAMax','<EVAL>',34,'Parameter|aMax','prm:/LogicLev/gen/F_PP1/aMax','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmAMin','<EVAL>',34,'Parameter|aMin','prm:/LogicLev/gen/F_PP1/aMin','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmAnalog','<EVAL>',34,'Parameter|analog','prm:/LogicLev/gen/F_PP1','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmColor','<EVAL>',34,'Parametr|color','prm:/LogicLev/gen/F_PP1','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmDescr','<EVAL>',34,'Parameter|DESCR','prm:/LogicLev/gen/F_PP1/DESCR','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmId','<EVAL>',34,'Parameter|SHIFR','prm:/LogicLev/gen/F_PP1/SHIFR','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmImpQdwnTm','<EVAL>',34,'Parameter|impQdwn','prm:/LogicLev/gen/F_PP1','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmImpQupTm','<EVAL>',34,'Parameter|impQup','prm:/LogicLev/gen/F_PP1','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmPrec','<EVAL>',34,'Parameter|prec','prm:/LogicLev/gen/F_PP1','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmWMax','<EVAL>',34,'Parameter|wMax','prm:/LogicLev/gen/F_PP1/wMax','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmWMin','<EVAL>',34,'Parameter|wMin','prm:/LogicLev/gen/F_PP1/wMin','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','digComs','OPEN;CLOSE;STOP',34,'Parameter|digComs','prm:/LogicLev/gen/F_PP1','el8','ВІДКР;ЗАКР;СТОП','','','ОТКР;ЗАКР;СТОП','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','digStts','<EVAL>',34,'Parameter|digStts','prm:/LogicLev/gen/F_PP1','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','curSek','1283327627',40,'','','trnd1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prm0val','<EVAL>',32,'','','trnd1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prm1val','<EVAL>',32,'','','trnd1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prm2val','<EVAL>',32,'','','trnd1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prm3val','<EVAL>',32,'','','trnd1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prm4val','<EVAL>',32,'','','trnd1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prm5val','<EVAL>',32,'','','trnd1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prm6val','<EVAL>',32,'','','trnd1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prm7val','<EVAL>',32,'','','trnd1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','Kp','<EVAL>',46,'<page>|Kp','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','NAME','<EVAL>',42,'<page>|NAME','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','SHIFR','<EVAL>',42,'<page>|SHIFR','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','Td','<EVAL>',46,'<page>|Td','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','Ti','<EVAL>',46,'<page>|Ti','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','Zi','<EVAL>',46,'<page>|Zi','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','ed','<EVAL>',42,'<page>|ed','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','max','<EVAL>',42,'<page>|max','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','min','<EVAL>',42,'<page>|min','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','out','<EVAL>',42,'<page>|out','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','out_addr','<EVAL>',42,'<page>|out','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','sp','<EVAL>',42,'<page>|sp','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','sp_addr','<EVAL>',42,'<page>|sp','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','var','<EVAL>',42,'<page>|var','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','var_addr','<EVAL>',42,'<page>|var','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','KImpRfact','<EVAL>',46,'<page>|KImpRfact','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','Kd','<EVAL>',46,'<page>|Kd','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','Ki','<EVAL>',46,'<page>|Ki','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','TImpMin','1',46,'<page>|TImpMin','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','TImpPer','1',46,'<page>|TImpPer','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','Tzd','<EVAL>',46,'<page>|Tzd','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','auto_addr','<EVAL>',42,'<page>|auto','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','impQup_addr','<EVAL>',42,'<page>|impQup','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','impQdwn_addr','<EVAL>',42,'<page>|impQdwn','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','prec','1',42,'<page>|prec','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','value','0',8,'','','k1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','value','0',8,'','','k2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','value','0',8,'','','k3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','value','0',8,'','','k4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','value','0',8,'','','rfct','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','value','0',8,'','','tiPer','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','curSek','1262685011',0,'','','trnd','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','value','0',8,'','','kd','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','value','0',8,'','','ki','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','value','0',8,'','','kp','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','value','0',8,'','','td','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','value','0',8,'','','ti','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','value','0',8,'','','tzd','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','value','0',8,'','','zn','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','value','0',8,'','','tiMin','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','value','0',8,'','','h1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','value','0',8,'','','h2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','pModeA',NULL,34,'Parameter|auto','prm:/LogicLev/gen/Pi','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','pModeC',NULL,34,'Parameter|casc','prm:/LogicLev/gen/Pi','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','aMax',NULL,34,'Parameter|aMax','prm:/LogicLev/gen/Pi/aMax','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','aMin',NULL,34,'Parameter|aMin','prm:/LogicLev/gen/Pi/aMin','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','prec',NULL,34,'Parameter|prec','prm:/LogicLev/gen/Pi/prec','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','wMax',NULL,34,'Parameter|wMax','prm:/LogicLev/gen/Pi/wMax','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','wMin',NULL,34,'Parameter|wMin','prm:/LogicLev/gen/Pi/wMin','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','pModeA',NULL,34,'Parameter|auto','prm:/BlockCalc/gen_cntr/PC_PCV1/auto','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','pModeC',NULL,34,'Parameter|casc','prm:/BlockCalc/gen_cntr/PC_PCV1','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','aMax',NULL,34,'Parameter|aMax','prm:/BlockCalc/gen_cntr/PC_PCV1','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','aMin',NULL,34,'Parameter|aMin','prm:/BlockCalc/gen_cntr/PC_PCV1','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','prec',NULL,34,'Parameter|prec','prm:/BlockCalc/gen_cntr/PC_PCV1/prec','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','wMax',NULL,34,'Parameter|wMax','prm:/BlockCalc/gen_cntr/PC_PCV1','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','wMin',NULL,34,'Parameter|wMin','prm:/BlockCalc/gen_cntr/PC_PCV1','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','pModeA',NULL,34,'Parameter|auto','prm:/LogicLev/gen/P_PP1','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','pModeC',NULL,34,'Parameter|casc','prm:/LogicLev/gen/P_PP1','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','aMax',NULL,34,'Parameter|aMax','prm:/LogicLev/gen/P_PP1/aMax','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','aMin',NULL,34,'Parameter|aMin','prm:/LogicLev/gen/P_PP1/aMin','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','prec',NULL,34,'Parameter|prec','prm:/LogicLev/gen/P_PP1','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','wMax',NULL,34,'Parameter|wMax','prm:/LogicLev/gen/P_PP1/wMax','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','wMin',NULL,34,'Parameter|wMin','prm:/LogicLev/gen/P_PP1/wMin','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','pModeA',NULL,34,'Parameter|auto','prm:/LogicLev/gen/P_PP3','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','pModeC',NULL,34,'Parameter|casc','prm:/LogicLev/gen/P_PP3','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','aMax',NULL,34,'Parameter|aMax','prm:/LogicLev/gen/P_PP3/aMax','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','aMin',NULL,34,'Parameter|aMin','prm:/LogicLev/gen/P_PP3/aMin','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','prec',NULL,34,'Parameter|prec','prm:/LogicLev/gen/P_PP3','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','wMax',NULL,34,'Parameter|wMax','prm:/LogicLev/gen/P_PP3/wMax','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','wMin',NULL,34,'Parameter|wMin','prm:/LogicLev/gen/P_PP3/wMin','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','pModeA',NULL,34,'Parameter|auto','prm:/BlockCalc/gen_cntr/PC_PCV2/auto','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','pModeC',NULL,34,'Parameter|casc','prm:/BlockCalc/gen_cntr/PC_PCV2','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','aMax',NULL,34,'Parameter|aMax','prm:/BlockCalc/gen_cntr/PC_PCV2','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','aMin',NULL,34,'Parameter|aMin','prm:/BlockCalc/gen_cntr/PC_PCV2','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','prec',NULL,34,'Parameter|prec','prm:/BlockCalc/gen_cntr/PC_PCV2/prec','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','wMax',NULL,34,'Parameter|wMax','prm:/BlockCalc/gen_cntr/PC_PCV2','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','wMin',NULL,34,'Parameter|wMin','prm:/BlockCalc/gen_cntr/PC_PCV2','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','pModeA',NULL,34,'Parameter|auto','prm:/LogicLev/gen/P_PP5','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','pModeC',NULL,34,'Parameter|casc','prm:/LogicLev/gen/P_PP5','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','aMax',NULL,34,'Parameter|aMax','prm:/LogicLev/gen/P_PP5/aMax','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','aMin',NULL,34,'Parameter|aMin','prm:/LogicLev/gen/P_PP5/aMin','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','prec',NULL,34,'Parameter|prec','prm:/LogicLev/gen/P_PP5','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','wMax',NULL,34,'Parameter|wMax','prm:/LogicLev/gen/P_PP5/wMax','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','wMin',NULL,34,'Parameter|wMin','prm:/LogicLev/gen/P_PP5/wMin','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','pModeA',NULL,34,'Parameter|auto','prm:/BlockCalc/gen_cntr/PC_PCV3/auto','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','pModeC',NULL,34,'Parameter|casc','prm:/BlockCalc/gen_cntr/PC_PCV3','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','aMax',NULL,34,'Parameter|aMax','prm:/BlockCalc/gen_cntr/PC_PCV3','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','aMin',NULL,34,'Parameter|aMin','prm:/BlockCalc/gen_cntr/PC_PCV3','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','prec',NULL,34,'Parameter|prec','prm:/BlockCalc/gen_cntr/PC_PCV3/prec','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','wMax',NULL,34,'Parameter|wMax','prm:/BlockCalc/gen_cntr/PC_PCV3','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','wMin',NULL,34,'Parameter|wMin','prm:/BlockCalc/gen_cntr/PC_PCV3','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','pModeA',NULL,34,'Parameter|auto','prm:/LogicLev/gen/F_PP1','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','pModeC',NULL,34,'Parameter|casc','prm:/LogicLev/gen/F_PP1','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','aMax',NULL,34,'Parameter|aMax','prm:/LogicLev/gen/F_PP1/aMax','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','aMin',NULL,34,'Parameter|aMin','prm:/LogicLev/gen/F_PP1/aMin','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','prec',NULL,34,'Parameter|prec','prm:/LogicLev/gen/F_PP1','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','wMax',NULL,34,'Parameter|wMax','prm:/LogicLev/gen/F_PP1/wMax','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','wMin',NULL,34,'Parameter|wMin','prm:/LogicLev/gen/F_PP1/wMin','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','pModeA',NULL,34,'Parameter|auto','prm:/BlockCalc/CM102/ST8612','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','pModeC',NULL,34,'Parameter|casc','prm:/BlockCalc/CM102/ST8612','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','aMax',NULL,34,'Parameter|aMax','prm:/BlockCalc/CM102/ST8612','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','aMin',NULL,34,'Parameter|aMin','prm:/BlockCalc/CM102/ST8612','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','prec',NULL,34,'Parameter|prec','prm:/BlockCalc/CM102/ST8612/prec','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','wMax',NULL,34,'Parameter|wMax','prm:/BlockCalc/CM102/ST8612','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','wMin',NULL,34,'Parameter|wMin','prm:/BlockCalc/CM102/ST8612','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','pModeA',NULL,34,'Parameter|auto','prm:/BlockCalc/CM102/F103','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','pModeC',NULL,34,'Parameter|casc','prm:/BlockCalc/CM102/F103','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','aMax',NULL,34,'Parameter|aMax','prm:/BlockCalc/CM102/F103','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','aMin',NULL,34,'Parameter|aMin','prm:/BlockCalc/CM102/F103','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','prec',NULL,34,'Parameter|prec','prm:/BlockCalc/CM102/F103/prec','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','wMax',NULL,34,'Parameter|wMax','prm:/BlockCalc/CM102/F103','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','wMin',NULL,34,'Parameter|wMin','prm:/BlockCalc/CM102/F103','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','pModeA',NULL,34,'Parameter|auto','prm:/BlockCalc/CM102/P103','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','pModeC',NULL,34,'Parameter|casc','prm:/BlockCalc/CM102/P103','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','aMax',NULL,34,'Parameter|aMax','prm:/BlockCalc/CM102/P103','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','aMin',NULL,34,'Parameter|aMin','prm:/BlockCalc/CM102/P103','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','prec',NULL,34,'Parameter|prec','prm:/BlockCalc/CM102/P103/prec','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','wMax',NULL,34,'Parameter|wMax','prm:/BlockCalc/CM102/P103','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','wMin',NULL,34,'Parameter|wMin','prm:/BlockCalc/CM102/P103','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','pModeA',NULL,34,'Parameter|auto','prm:/BlockCalc/CM102/PT1006_1','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','pModeC',NULL,34,'Parameter|casc','prm:/BlockCalc/CM102/PT1006_1','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','aMax',NULL,34,'Parameter|aMax','prm:/BlockCalc/CM102/PT1006_1','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','aMin',NULL,34,'Parameter|aMin','prm:/BlockCalc/CM102/PT1006_1','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','prec',NULL,34,'Parameter|prec','prm:/BlockCalc/CM102/PT1006_1/prec','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','wMax',NULL,34,'Parameter|wMax','prm:/BlockCalc/CM102/PT1006_1','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','wMin',NULL,34,'Parameter|wMin','prm:/BlockCalc/CM102/PT1006_1','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','pModeA',NULL,34,'Parameter|auto','prm:/BlockCalc/CM102/PT0204','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','pModeC',NULL,34,'Parameter|casc','prm:/BlockCalc/CM102/PT0204','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','aMax',NULL,34,'Parameter|aMax','prm:/BlockCalc/CM102/PT0204','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','aMin',NULL,34,'Parameter|aMin','prm:/BlockCalc/CM102/PT0204','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','prec',NULL,34,'Parameter|prec','prm:/BlockCalc/CM102/PT0204/prec','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','wMax',NULL,34,'Parameter|wMax','prm:/BlockCalc/CM102/PT0204','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','wMin',NULL,34,'Parameter|wMin','prm:/BlockCalc/CM102/PT0204','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','pModeA',NULL,34,'Parameter|auto','prm:/BlockCalc/CM102/TE1313_1','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','pModeC',NULL,34,'Parameter|casc','prm:/BlockCalc/CM102/TE1313_1','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','aMax',NULL,34,'Parameter|aMax','prm:/BlockCalc/CM102/TE1313_1','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','aMin',NULL,34,'Parameter|aMin','prm:/BlockCalc/CM102/TE1313_1','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','prec',NULL,34,'Parameter|prec','prm:/BlockCalc/CM102/TE1313_1/prec','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','wMax',NULL,34,'Parameter|wMax','prm:/BlockCalc/CM102/TE1313_1','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','wMin',NULL,34,'Parameter|wMin','prm:/BlockCalc/CM102/TE1313_1','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','pModeA',NULL,34,'Parameter|auto','prm:/BlockCalc/CM102/TE1314_1','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','pModeC',NULL,34,'Parameter|casc','prm:/BlockCalc/CM102/TE1314_1','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','aMax',NULL,34,'Parameter|aMax','prm:/BlockCalc/CM102/TE1314_1','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','aMin',NULL,34,'Parameter|aMin','prm:/BlockCalc/CM102/TE1314_1','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','prec',NULL,34,'Parameter|prec','prm:/BlockCalc/CM102/TE1314_1/prec','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','wMax',NULL,34,'Parameter|wMax','prm:/BlockCalc/CM102/TE1314_1','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','wMin',NULL,34,'Parameter|wMin','prm:/BlockCalc/CM102/TE1314_1','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','pModeA',NULL,34,'Parameter|auto','prm:/BlockCalc/CM102/TE1314_2','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','pModeC',NULL,34,'Parameter|casc','prm:/BlockCalc/CM102/TE1314_2','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','aMax',NULL,34,'Parameter|aMax','prm:/BlockCalc/CM102/TE1314_2','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','aMin',NULL,34,'Parameter|aMin','prm:/BlockCalc/CM102/TE1314_2','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','prec',NULL,34,'Parameter|prec','prm:/BlockCalc/CM102/TE1314_2/prec','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','wMax',NULL,34,'Parameter|wMax','prm:/BlockCalc/CM102/TE1314_2','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','wMin',NULL,34,'Parameter|wMin','prm:/BlockCalc/CM102/TE1314_2','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','min',NULL,42,'Parameter|min','prm:/BlockCalc/CM101/TE1313_1/min','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','prec',NULL,34,'Parameter|prec','prm:/BlockCalc/CM101/TE1313_1/prec','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','wMax',NULL,34,'Parameter|wMax','prm:/BlockCalc/CM101/TE1313_1','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','wMin',NULL,34,'Parameter|wMin','prm:/BlockCalc/CM101/TE1313_1','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','name','Graphics group element',42,'Parameter|NAME','prm:/BlockCalc/CM101/TE1314_1/NAME','el7','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','addr',NULL,42,'Parameter|var','prm:/BlockCalc/CM101/TE1314_1/var','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','pModeA',NULL,34,'Parameter|auto','prm:/BlockCalc/CM101/TE1314_1','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','pModeC',NULL,34,'Parameter|casc','prm:/BlockCalc/CM101/TE1314_1','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','aMax',NULL,34,'Parameter|aMax','prm:/BlockCalc/CM101/TE1314_1','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','aMin',NULL,34,'Parameter|aMin','prm:/BlockCalc/CM101/TE1314_1','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','ed',NULL,34,'Parameter|ed','prm:/BlockCalc/CM101/TE1314_1/ed','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','max',NULL,42,'Parameter|max','prm:/BlockCalc/CM101/TE1314_1/max','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','min',NULL,42,'Parameter|min','prm:/BlockCalc/CM101/TE1314_1/min','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','prec',NULL,34,'Parameter|prec','prm:/BlockCalc/CM101/TE1314_1/prec','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','wMax',NULL,34,'Parameter|wMax','prm:/BlockCalc/CM101/TE1314_1','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','wMin',NULL,34,'Parameter|wMin','prm:/BlockCalc/CM101/TE1314_1','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','name','Graphics group element',42,'Parameter|NAME','prm:/BlockCalc/CM101/TE1314_2/NAME','el8','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','addr',NULL,42,'Parameter|var','prm:/BlockCalc/CM101/TE1314_2/var','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','pModeA',NULL,34,'Parameter|auto','prm:/BlockCalc/CM101/TE1314_2','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','pModeC',NULL,34,'Parameter|casc','prm:/BlockCalc/CM101/TE1314_2','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','aMax',NULL,34,'Parameter|aMax','prm:/BlockCalc/CM101/TE1314_2','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','aMin',NULL,34,'Parameter|aMin','prm:/BlockCalc/CM101/TE1314_2','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','ed',NULL,34,'Parameter|ed','prm:/BlockCalc/CM101/TE1314_2/ed','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','max',NULL,42,'Parameter|max','prm:/BlockCalc/CM101/TE1314_2/max','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','min',NULL,42,'Parameter|min','prm:/BlockCalc/CM101/TE1314_2/min','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','prec',NULL,34,'Parameter|prec','prm:/BlockCalc/CM101/TE1314_2/prec','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','wMax',NULL,34,'Parameter|wMax','prm:/BlockCalc/CM101/TE1314_2','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','wMin',NULL,34,'Parameter|wMin','prm:/BlockCalc/CM101/TE1314_2','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','curSek','1415550014',40,'','','trnd1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','aMax',NULL,34,'Parameter|aMax','prm:/LogicLev/gen/T_PP5/aMax','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','aMin',NULL,34,'Parameter|aMin','prm:/LogicLev/gen/T_PP5/aMin','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','addr',NULL,42,'Parameter|var','prm:/LogicLev/gen/T_PP5/var','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','ed',NULL,34,'Parameter|ed','prm:/LogicLev/gen/T_PP5/ed','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','max',NULL,42,'Parameter|max','prm:/LogicLev/gen/T_PP5/max','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','min',NULL,42,'Parameter|min','prm:/LogicLev/gen/T_PP5/min','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','pModeA',NULL,34,'Parameter|auto','prm:/LogicLev/gen/T_PP5','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','pModeC',NULL,34,'Parameter|casc','prm:/LogicLev/gen/T_PP5','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','prec',NULL,34,'Parameter|prec','prm:/LogicLev/gen/T_PP5','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','wMax',NULL,34,'Parameter|wMax','prm:/LogicLev/gen/T_PP5/wMax','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','wMin',NULL,34,'Parameter|wMin','prm:/LogicLev/gen/T_PP5/wMin','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','name','Graphics group element',42,'Parameter|NAME','prm:/BlockCalc/CM102cntr/BC112/NAME','el5','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','aMax',NULL,34,'Parameter|aMax','prm:/BlockCalc/CM102cntr/BC112','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','aMin',NULL,34,'Parameter|aMin','prm:/BlockCalc/CM102cntr/BC112','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','addr',NULL,42,'Parameter|var','prm:/BlockCalc/CM102cntr/BC112/com','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','ed',NULL,34,'Parameter|ed','prm:/BlockCalc/CM102cntr/BC112','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','max',NULL,42,'Parameter|max','val:5','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','min',NULL,42,'Parameter|min','val:-1','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','pModeA',NULL,34,'Parameter|auto','prm:/BlockCalc/CM102cntr/BC112','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','pModeC',NULL,34,'Parameter|casc','prm:/BlockCalc/CM102cntr/BC112','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','prec',NULL,34,'Parameter|prec','prm:/BlockCalc/CM102cntr/BC112','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','wMax',NULL,34,'Parameter|wMax','prm:/BlockCalc/CM102cntr/BC112','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','wMin',NULL,34,'Parameter|wMin','prm:/BlockCalc/CM102cntr/BC112','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','color','cyan-127',41,'','cyan-127','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','digComs','OPEN;CLOSE',33,'','OPEN;CLOSE','el5','ВІДКР;ЗАКР','','ВІДКР;ЗАКР','ОТКР;ЗАКР','','ОТКР;ЗАКР');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','digRevers','0',33,'','','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','digStts','OPEN;CLOSE',33,'','OPEN;CLOSE','el5','ВІДКР;ЗАКР','','ВІДКР;ЗАКР','ОТКР;ЗАКР','','ОТКР;ЗАКР');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pErr','<EVAL>',34,'Parameter|err','prm:/LogicLev/gen/Pi/err','Pi','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pModeA','<EVAL>',34,'Parameter|auto','prm:/LogicLev/gen/Pi','Pi','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pModeC','<EVAL>',34,'Parameter|casc','prm:/LogicLev/gen/Pi','Pi','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pPrec','<EVAL>',34,'Parameter|prec','prm:/LogicLev/gen/Pi/prec','Pi','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pErr','<EVAL>',34,'Parameter|err','prm:/LogicLev/gen/Ti/err','Ti','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pModeA','<EVAL>',34,'Parameter|auto','prm:/LogicLev/gen/Ti','Ti','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pModeC','<EVAL>',34,'Parameter|casc','prm:/LogicLev/gen/Ti','Ti','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pPrec','<EVAL>',34,'Parameter|prec','val:1','Ti','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','curSek','1262785012',8,'','','trnd1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','prec','1',34,'Parameter|prec','val:3','T_PP1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','prec','1',34,'Parameter|prec','prm:/LogicLev/gen/F_PP1/prec','F_PP1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','prec','1',34,'Parameter|prec','val:3','T_PP3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','prec','1',34,'Parameter|prec','prm:/LogicLev/gen/P_PP3/prec','P_PP3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','prec','1',34,'Parameter|prec','prm:/LogicLev/gen/F_PP3/prec','F_PP3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','prec','1',34,'Parameter|prec','val:3','T_PP5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','prec','1',34,'Parameter|prec','prm:/LogicLev/gen/P_PP5/prec','P_PP5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','prec','1',34,'Parameter|prec','prm:/LogicLev/gen/F_PP5/prec','F_PP5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM101/F101/prec','F_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM101/TE1313_1/prec','T_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM101/P101/prec','P_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM101/F102/prec','F_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM101/TE1314_1/prec','T_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM101/PT1006_1/prec','P_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM101/TT0202/prec','T_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM101/TT0204/prec','T_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM101/PT0202/prec','P_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM101/PT0204/prec','P_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM102/F103/prec','F_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM102/TE1313_1/prec','T_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM102/P103/prec','P_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM102/F104/prec','F_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM102/TE1314_1/prec','T_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM102/PT1006_1/prec','P_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM102/TT0202/prec','T_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM102/TT0204/prec','T_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM102/PT0202/prec','P_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM102/PT0204/prec','P_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4','name','CM201',0,'','','','КМ201','','','КМ201','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph','pgGrp','so',32,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph','curSek','1205241018',40,'','','trnd1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','name','CM201',41,'','','','КМ201','','','КМ201','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','name','Graphics group element',42,'Parameter|NAME','prm:/BlockCalc/CM201/ST8612/NAME','el1','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','name','Graphics group element',42,'Parameter|NAME','prm:/BlockCalc/CM201/F101/NAME','el2','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','name','Graphics group element',42,'Parameter|NAME','prm:/BlockCalc/CM201/P101/NAME','el3','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','name','Graphics group element',42,'Parameter|NAME','prm:/BlockCalc/CM201/PT1006_1/NAME','el4','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','name','Graphics group element',42,'Parameter|NAME','prm:/BlockCalc/CM201/PT0204/NAME','el5','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','name','Graphics group element',42,'Parameter|NAME','prm:/BlockCalc/CM201/TE1313_1/NAME','el6','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','name','Graphics group element',42,'Parameter|NAME','prm:/BlockCalc/CM201/TE1314_1/NAME','el7','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','name','Graphics group element',42,'Parameter|NAME','prm:/BlockCalc/CM201/TE1314_2/NAME','el8','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn','name','Mnemos',0,'','','','Мнемосхеми','','','Мнемосхемы','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn','dscr','Logical containers of page group: "Mnemos"',0,'','','','Логічний контейнер групи сторінок: "Мнемосхеми"','','','Логический контейнер группы страниц: "Мнемосхемы"','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','name','CM201',32,'','','','КМ201','','','КМ201','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','pgGrp','so',32,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','pName','F',33,'Parameter|NAME','F','F_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM201/F101/var','F_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM201/F101/prec','F_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','pName','F',33,'Parameter|NAME','F','F_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM201/F102/var','F_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM201/F102/prec','F_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','text','CM201/1',32,'','','txtCM101_1','КМ201/1','','КМ201/1','КМ201/1','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','text','CM201/2',32,'','','txtCM101_2','КМ201/2','','КМ201/2','КМ201/2','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','out','0',34,'Parameter|out','prm:/BlockCalc/gen_cntr/SurgeCM201/out','KPP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','pErr','<EVAL>',34,'Parameter|err','prm:/BlockCalc/CM201/ST8612/err','N_CM','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','pModeA','<EVAL>',34,'Parameter|auto','prm:/BlockCalc/CM201/ST8612','N_CM','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','pModeC','<EVAL>',34,'Parameter|casc','prm:/BlockCalc/CM201/ST8612','N_CM','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','pName','',34,'Parameter|NAME','prm:/BlockCalc/CM201/ST8612/NAME','N_CM','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','pPrec','<EVAL>',34,'Parameter|prec','prm:/BlockCalc/CM201/ST8612/prec','N_CM','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM201/ST8612/var','N_CM','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','text','KPP201',32,'','','txtKPP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','text','BC206',32,'','','Text3','КК206','','','КШ206','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','text','C201/3',32,'','','Text4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','text','C201/2',32,'','','Text5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','text','C201/1',32,'','','Text7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','pName','T',33,'Parameter|NAME','T','T_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM201/TE1313_1/var','T_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM201/TE1313_1/prec','T_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','pName','T',33,'Parameter|NAME','T','T_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM201/TE1314_1/var','T_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM201/TE1314_1/prec','T_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','com','0',38,'Parameter|com','prm:/BlockCalc/CM201/BC101/com','KS101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','shifr',NULL,38,'Parameter|NAME','prm:/BlockCalc/CM201/BC101/NAME','KS101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','st_close',NULL,38,'Parameter|st_close','prm:/BlockCalc/CM201/BC101/st_close','KS101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','st_open',NULL,38,'Parameter|st_open','prm:/BlockCalc/CM201/BC101/st_open','KS101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','com','0',38,'Parameter|com','prm:/BlockCalc/CM201/BC102/com','KS102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','shifr',NULL,38,'Parameter|NAME','prm:/BlockCalc/CM201/BC102/NAME','KS102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','st_close',NULL,38,'Parameter|st_close','prm:/BlockCalc/CM201/BC102/st_close','KS102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','st_open',NULL,38,'Parameter|st_open','prm:/BlockCalc/CM201/BC102/st_open','KS102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','com','0',38,'Parameter|com','prm:/BlockCalc/CM201/BC104/com','KS104','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','shifr',NULL,38,'Parameter|NAME','prm:/BlockCalc/CM201/BC104/NAME','KS104','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','st_close',NULL,38,'Parameter|st_close','prm:/BlockCalc/CM201/BC104/st_close','KS104','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','st_open',NULL,38,'Parameter|st_open','prm:/BlockCalc/CM201/BC104/st_open','KS104','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','com','0',38,'Parameter|com','prm:/BlockCalc/CM201/BC105/com','KS105','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','shifr',NULL,38,'Parameter|NAME','prm:/BlockCalc/CM201/BC105/NAME','KS105','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','st_close',NULL,38,'Parameter|st_close','prm:/BlockCalc/CM201/BC105/st_close','KS105','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','st_open',NULL,38,'Parameter|st_open','prm:/BlockCalc/CM201/BC105/st_open','KS105','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','com','0',38,'Parameter|com','prm:/BlockCalc/CM201/BC106/com','KS106','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','shifr',NULL,38,'Parameter|NAME','prm:/BlockCalc/CM201/BC106/NAME','KS106','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','st_close',NULL,38,'Parameter|st_close','prm:/BlockCalc/CM201/BC106/st_close','KS106','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','st_open',NULL,38,'Parameter|st_open','prm:/BlockCalc/CM201/BC106/st_open','KS106','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','pName','P',33,'Parameter|NAME','P','P_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM201/P101/var','P_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM201/P101/prec','P_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','pName','P',33,'Parameter|NAME','P','P_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM201/PT1006_1/var','P_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM201/PT1006_1/prec','P_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','text','BC201',32,'','','txtKS101','КК201','','','КШ201','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','text','BC202',32,'','','txtKS102','КК202','','','КШ202','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','text','BC203',32,'','','txtKS103','КК203','','','КШ203','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','text','BC204',32,'','','txtKS104','КК204','','','КШ204','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','text','BC205',32,'','','txtKS105','КК205','','','КШ205','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','text','BC207',32,'','','txtKS107','КК207','','','КШ207','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','pName','T',33,'Parameter|NAME','T','T_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM201/TT0202/var','T_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM201/TT0202/prec','T_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','pName','T',33,'Parameter|NAME','T','T_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM201/TT0204/var','T_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM201/TT0204/prec','T_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','text','BC221',32,'','','txtKS121','КК221','','','КШ221','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','text','BC222',32,'','','txtKS122','КК222','','','КШ222','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','text','BC223',32,'','','txtKS123','КК223','','','КШ223','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','text','AT201/1',32,'','','txtAT101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','text','AT201/2',32,'','','txtAT101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','pErr','<EVAL>',34,'Parameter|err','prm:/BlockCalc/CM201/TE1314_1/err','TAT101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','pModeA','<EVAL>',34,'Parameter|auto','prm:/BlockCalc/CM201/TE1314_1','TAT101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','pModeC','<EVAL>',34,'Parameter|casc','prm:/BlockCalc/CM201/TE1314_1','TAT101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','pName','',34,'Parameter|NAME','prm:/BlockCalc/CM201/TE1314_1/NAME','TAT101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','pPrec','<EVAL>',34,'Parameter|prec','prm:/BlockCalc/CM201/TE1314_1/prec','TAT101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM201/TE1314_1/var','TAT101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','pErr','<EVAL>',34,'Parameter|err','prm:/BlockCalc/CM201/TE1314_2/err','TAT101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','pModeA','<EVAL>',34,'Parameter|auto','prm:/BlockCalc/CM201/TE1314_2','TAT101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','pModeC','<EVAL>',34,'Parameter|casc','prm:/BlockCalc/CM201/TE1314_2','TAT101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','pName','',34,'Parameter|NAME','prm:/BlockCalc/CM201/TE1314_2/NAME','TAT101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','pPrec','<EVAL>',34,'Parameter|prec','prm:/BlockCalc/CM201/TE1314_2/prec','TAT101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM201/TE1314_2/var','TAT101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','pName','P',33,'Parameter|NAME','P','P_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM201/PT0202/var','P_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM201/PT0202/prec','P_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','pName','P',33,'Parameter|NAME','P','P_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM201/PT0204/var','P_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM201/PT0204/prec','P_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','text','PP201',32,'','','txtPP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','text','PP202',32,'','','txtPP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','aMax',NULL,34,'Parameter|aMax','prm:/BlockCalc/CM201/ST8612','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','aMin',NULL,34,'Parameter|aMin','prm:/BlockCalc/CM201/ST8612','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','addr',NULL,42,'Parameter|var','prm:/BlockCalc/CM201/ST8612/var','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','ed',NULL,34,'Parameter|ed','prm:/BlockCalc/CM201/ST8612/ed','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','max',NULL,42,'Parameter|max','prm:/BlockCalc/CM201/ST8612/max','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','min',NULL,42,'Parameter|min','prm:/BlockCalc/CM201/ST8612/min','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','pModeA',NULL,34,'Parameter|auto','prm:/BlockCalc/CM201/ST8612','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','pModeC',NULL,34,'Parameter|casc','prm:/BlockCalc/CM201/ST8612','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','prec',NULL,34,'Parameter|prec','prm:/BlockCalc/CM201/ST8612/prec','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','wMax',NULL,34,'Parameter|wMax','prm:/BlockCalc/CM201/ST8612','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','wMin',NULL,34,'Parameter|wMin','prm:/BlockCalc/CM201/ST8612','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','aMax',NULL,34,'Parameter|aMax','prm:/BlockCalc/CM201/F101','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','aMin',NULL,34,'Parameter|aMin','prm:/BlockCalc/CM201/F101','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','addr',NULL,42,'Parameter|var','prm:/BlockCalc/CM201/F101/var','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','ed',NULL,34,'Parameter|ed','prm:/BlockCalc/CM201/F101/ed','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','max',NULL,42,'Parameter|max','prm:/BlockCalc/CM201/F101/max','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','min',NULL,42,'Parameter|min','prm:/BlockCalc/CM201/F101/min','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','pModeA',NULL,34,'Parameter|auto','prm:/BlockCalc/CM201/F101','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','pModeC',NULL,34,'Parameter|casc','prm:/BlockCalc/CM201/F101','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','prec',NULL,34,'Parameter|prec','prm:/BlockCalc/CM201/F101/prec','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','wMax',NULL,34,'Parameter|wMax','prm:/BlockCalc/CM201/F101','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','wMin',NULL,34,'Parameter|wMin','prm:/BlockCalc/CM201/F101','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','aMax',NULL,34,'Parameter|aMax','prm:/BlockCalc/CM201/P101','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','aMin',NULL,34,'Parameter|aMin','prm:/BlockCalc/CM201/P101','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','addr',NULL,42,'Parameter|var','prm:/BlockCalc/CM201/P101/var','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','ed',NULL,34,'Parameter|ed','prm:/BlockCalc/CM201/P101/ed','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','max',NULL,42,'Parameter|max','prm:/BlockCalc/CM201/P101/max','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','min',NULL,42,'Parameter|min','prm:/BlockCalc/CM201/P101/min','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','pModeA',NULL,34,'Parameter|auto','prm:/BlockCalc/CM201/P101','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','pModeC',NULL,34,'Parameter|casc','prm:/BlockCalc/CM201/P101','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','prec',NULL,34,'Parameter|prec','prm:/BlockCalc/CM201/P101/prec','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','wMax',NULL,34,'Parameter|wMax','prm:/BlockCalc/CM201/P101','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','wMin',NULL,34,'Parameter|wMin','prm:/BlockCalc/CM201/P101','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','aMax',NULL,34,'Parameter|aMax','prm:/BlockCalc/CM201/PT1006_1','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','aMin',NULL,34,'Parameter|aMin','prm:/BlockCalc/CM201/PT1006_1','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','addr',NULL,42,'Parameter|var','prm:/BlockCalc/CM201/PT1006_1/var','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','ed',NULL,34,'Parameter|ed','prm:/BlockCalc/CM201/PT1006_1/ed','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','max',NULL,42,'Parameter|max','prm:/BlockCalc/CM201/PT1006_1/max','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','min',NULL,42,'Parameter|min','prm:/BlockCalc/CM201/PT1006_1/min','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','pModeA',NULL,34,'Parameter|auto','prm:/BlockCalc/CM201/PT1006_1','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','pModeC',NULL,34,'Parameter|casc','prm:/BlockCalc/CM201/PT1006_1','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','prec',NULL,34,'Parameter|prec','prm:/BlockCalc/CM201/PT1006_1/prec','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','wMax',NULL,34,'Parameter|wMax','prm:/BlockCalc/CM201/PT1006_1','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','wMin',NULL,34,'Parameter|wMin','prm:/BlockCalc/CM201/PT1006_1','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','aMax',NULL,34,'Parameter|aMax','prm:/BlockCalc/CM201/PT0204','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','aMin',NULL,34,'Parameter|aMin','prm:/BlockCalc/CM201/PT0204','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','addr',NULL,42,'Parameter|var','prm:/BlockCalc/CM201/PT0204/var','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','ed',NULL,34,'Parameter|ed','prm:/BlockCalc/CM201/PT0204/ed','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','max',NULL,42,'Parameter|max','prm:/BlockCalc/CM201/PT0204/max','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','min',NULL,42,'Parameter|min','prm:/BlockCalc/CM201/PT0204/min','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','pModeA',NULL,34,'Parameter|auto','prm:/BlockCalc/CM201/PT0204','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','pModeC',NULL,34,'Parameter|casc','prm:/BlockCalc/CM201/PT0204','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','prec',NULL,34,'Parameter|prec','prm:/BlockCalc/CM201/PT0204/prec','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','wMax',NULL,34,'Parameter|wMax','prm:/BlockCalc/CM201/PT0204','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','wMin',NULL,34,'Parameter|wMin','prm:/BlockCalc/CM201/PT0204','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','aMax',NULL,34,'Parameter|aMax','prm:/BlockCalc/CM201/TE1313_1','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','aMin',NULL,34,'Parameter|aMin','prm:/BlockCalc/CM201/TE1313_1','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','addr',NULL,42,'Parameter|var','prm:/BlockCalc/CM201/TE1313_1/var','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','ed',NULL,34,'Parameter|ed','prm:/BlockCalc/CM201/TE1313_1/ed','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','max',NULL,42,'Parameter|max','prm:/BlockCalc/CM201/TE1313_1/max','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','min',NULL,42,'Parameter|min','prm:/BlockCalc/CM201/TE1313_1/min','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','pModeA',NULL,34,'Parameter|auto','prm:/BlockCalc/CM201/TE1313_1','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','pModeC',NULL,34,'Parameter|casc','prm:/BlockCalc/CM201/TE1313_1','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','prec',NULL,34,'Parameter|prec','prm:/BlockCalc/CM201/TE1313_1/prec','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','wMax',NULL,34,'Parameter|wMax','prm:/BlockCalc/CM201/TE1313_1','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','wMin',NULL,34,'Parameter|wMin','prm:/BlockCalc/CM201/TE1313_1','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','aMax',NULL,34,'Parameter|aMax','prm:/BlockCalc/CM201/TE1314_1','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','aMin',NULL,34,'Parameter|aMin','prm:/BlockCalc/CM201/TE1314_1','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','addr',NULL,42,'Parameter|var','prm:/BlockCalc/CM201/TE1314_1/var','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','ed',NULL,34,'Parameter|ed','prm:/BlockCalc/CM201/TE1314_1/ed','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','max',NULL,42,'Parameter|max','prm:/BlockCalc/CM201/TE1314_1/max','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','min',NULL,42,'Parameter|min','prm:/BlockCalc/CM201/TE1314_1/min','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','pModeA',NULL,34,'Parameter|auto','prm:/BlockCalc/CM201/TE1314_1','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','pModeC',NULL,34,'Parameter|casc','prm:/BlockCalc/CM201/TE1314_1','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','prec',NULL,34,'Parameter|prec','prm:/BlockCalc/CM201/TE1314_1/prec','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','wMax',NULL,34,'Parameter|wMax','prm:/BlockCalc/CM201/TE1314_1','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','wMin',NULL,34,'Parameter|wMin','prm:/BlockCalc/CM201/TE1314_1','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','aMax',NULL,34,'Parameter|aMax','prm:/BlockCalc/CM201/TE1314_2','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','aMin',NULL,34,'Parameter|aMin','prm:/BlockCalc/CM201/TE1314_2','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','addr',NULL,42,'Parameter|var','prm:/BlockCalc/CM201/TE1314_2/var','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','ed',NULL,34,'Parameter|ed','prm:/BlockCalc/CM201/TE1314_2/ed','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','max',NULL,42,'Parameter|max','prm:/BlockCalc/CM201/TE1314_2/max','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','min',NULL,42,'Parameter|min','prm:/BlockCalc/CM201/TE1314_2/min','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','pModeA',NULL,34,'Parameter|auto','prm:/BlockCalc/CM201/TE1314_2','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','pModeC',NULL,34,'Parameter|casc','prm:/BlockCalc/CM201/TE1314_2','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','prec',NULL,34,'Parameter|prec','prm:/BlockCalc/CM201/TE1314_2/prec','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','wMax',NULL,34,'Parameter|wMax','prm:/BlockCalc/CM201/TE1314_2','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','wMin',NULL,34,'Parameter|wMin','prm:/BlockCalc/CM201/TE1314_2','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_10','spName','Pressure after first stage',33,'','Pressure after first stage','Pi_нд','Тиск після першої ступені','','Тиск після першої ступені','Давление после первой ступени','','Давление после первой ступени');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_10','spName','Temperature after first stage',33,'','Temperature after first stage','Ti_нд','Температура після першої ступені','','Температура після першої ступені','Температура после первой ступени','','Температура после первой ступени');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','name','CM202',32,'','','','КМ202','','','КМ202','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','pgGrp','so',32,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','pName','F',33,'Parameter|NAME','F','F_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM202/F103/var','F_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM202/F103/prec','F_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','pName','F',33,'Parameter|NAME','F','F_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM202/F104/var','F_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM202/F104/prec','F_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','text','CM202/1',32,'','','txtCM101_1','КМ202/1','','','КМ202/1','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','text','CM202/2',32,'','','txtCM101_2','КМ202/2','','','КМ202/2','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','out','0',34,'Parameter|out','prm:/BlockCalc/CM202/KPP102/out','KPP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','pErr','<EVAL>',34,'Parameter|err','prm:/BlockCalc/CM202/ST8612/err','N_CM','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','pModeA','<EVAL>',34,'Parameter|auto','prm:/BlockCalc/CM202/ST8612','N_CM','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','pModeC','<EVAL>',34,'Parameter|casc','prm:/BlockCalc/CM202/ST8612','N_CM','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','pName','',34,'Parameter|NAME','prm:/BlockCalc/CM202/ST8612/NAME','N_CM','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','pPrec','<EVAL>',34,'Parameter|prec','prm:/BlockCalc/CM202/ST8612/prec','N_CM','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM202/ST8612/var','N_CM','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','text','KPP202',32,'','','txtKPP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','text','From PP5',32,'','','Text1','З PP5','','','Из PP5','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','text','BC216',32,'','','Text3','КК216','','','КШ216','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','text','C202/3',32,'','','Text4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','text','C202/2',32,'','','Text5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','text','To PP7',32,'','','Text6','До PP7','','','На PP7','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','text','C202/1',32,'','','Text7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','pName','T',33,'Parameter|NAME','T','T_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM202/TE1313_1/var','T_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM202/TE1313_1/prec','T_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','pName','T',33,'Parameter|NAME','T','T_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM202/TE1314_1/var','T_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM202/TE1314_1/prec','T_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','com','0',38,'Parameter|com','prm:/BlockCalc/CM202/BC111/com','KS101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','shifr',NULL,38,'Parameter|NAME','prm:/BlockCalc/CM202/BC111/NAME','KS101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','st_close',NULL,38,'Parameter|st_close','prm:/BlockCalc/CM202/BC111/st_close','KS101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','st_open',NULL,38,'Parameter|st_open','prm:/BlockCalc/CM202/BC111/st_open','KS101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','com','0',38,'Parameter|com','prm:/BlockCalc/CM202/BC112/com','KS102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','shifr',NULL,38,'Parameter|NAME','prm:/BlockCalc/CM202/BC112/NAME','KS102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','st_close',NULL,38,'Parameter|st_close','prm:/BlockCalc/CM202/BC112/st_close','KS102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','st_open',NULL,38,'Parameter|st_open','prm:/BlockCalc/CM202/BC112/st_open','KS102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','com','0',38,'Parameter|com','prm:/BlockCalc/CM202/BC114/com','KS104','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','shifr',NULL,38,'Parameter|NAME','prm:/BlockCalc/CM202/BC114/NAME','KS104','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','st_close',NULL,38,'Parameter|st_close','prm:/BlockCalc/CM202/BC114/st_close','KS104','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','st_open',NULL,38,'Parameter|st_open','prm:/BlockCalc/CM202/BC114/st_open','KS104','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','com','0',38,'Parameter|com','prm:/BlockCalc/CM202/BC115/com','KS105','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','shifr',NULL,38,'Parameter|NAME','prm:/BlockCalc/CM202/BC115/NAME','KS105','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','st_close',NULL,38,'Parameter|st_close','prm:/BlockCalc/CM202/BC115/st_close','KS105','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','st_open',NULL,38,'Parameter|st_open','prm:/BlockCalc/CM202/BC115/st_open','KS105','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','com','0',38,'Parameter|com','prm:/BlockCalc/CM202/BC116/com','KS106','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','shifr',NULL,38,'Parameter|NAME','prm:/BlockCalc/CM202/BC116/NAME','KS106','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','st_close',NULL,38,'Parameter|st_close','prm:/BlockCalc/CM202/BC116/st_close','KS106','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','st_open',NULL,38,'Parameter|st_open','prm:/BlockCalc/CM202/BC116/st_open','KS106','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','pName','P',33,'Parameter|NAME','P','P_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM202/P103/var','P_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM202/P103/prec','P_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','pName','P',33,'Parameter|NAME','P','P_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM202/PT1006_1/var','P_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM202/PT1006_1/prec','P_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','text','BC211',32,'','','txtKS101','КК211','','','КШ211','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','text','BC212',32,'','','txtKS102','КК212','','','КШ212','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','text','BC213',32,'','','txtKS103','КК213','','','КШ213','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','text','BC214',32,'','','txtKS104','КК214','','','КШ214','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','text','BC215',32,'','','txtKS105','КК215','','','КШ215','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','text','BC217',32,'','','txtKS107','КК217','','','КШ217','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','pName','T',33,'Parameter|NAME','T','T_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM202/TT0202/var','T_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM202/TT0202/prec','T_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','pName','T',33,'Parameter|NAME','T','T_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM202/TT0204/var','T_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM202/TT0204/prec','T_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','text','BC225',32,'','','txtKS121','КК225','','','КШ225','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','text','BC226',32,'','','txtKS122','КК226','','','КШ226','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','text','BC234',32,'','','txtKS123','КК234','','','КШ234','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','text','AT202/1',32,'','','txtAT101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','geomY','301',32,'','','txtAT101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','text','AT202/2',32,'','','txtAT101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','pErr','<EVAL>',34,'Parameter|err','prm:/BlockCalc/CM202/TE1314_1/err','TAT101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','pModeA','<EVAL>',34,'Parameter|auto','prm:/BlockCalc/CM202/TE1314_1','TAT101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','pModeC','<EVAL>',34,'Parameter|casc','prm:/BlockCalc/CM202/TE1314_1','TAT101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','pName','',34,'Parameter|NAME','prm:/BlockCalc/CM202/TE1314_1/NAME','TAT101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','pPrec','<EVAL>',34,'Parameter|prec','prm:/BlockCalc/CM202/TE1314_1/prec','TAT101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM202/TE1314_1/var','TAT101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','pErr','<EVAL>',34,'Parameter|err','prm:/BlockCalc/CM202/TE1314_2/err','TAT101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','pModeA','<EVAL>',34,'Parameter|auto','prm:/BlockCalc/CM202/TE1314_2','TAT101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','pModeC','<EVAL>',34,'Parameter|casc','prm:/BlockCalc/CM202/TE1314_2','TAT101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','pName','',34,'Parameter|NAME','prm:/BlockCalc/CM202/TE1314_2/NAME','TAT101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','pPrec','<EVAL>',34,'Parameter|prec','prm:/BlockCalc/CM202/TE1314_2/prec','TAT101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM202/TE1314_2/var','TAT101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','pName','P',33,'Parameter|NAME','P','P_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM202/PT0202/var','P_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM202/PT0202/prec','P_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','pName','P',33,'Parameter|NAME','P','P_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM202/PT0204/var','P_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM202/PT0204/prec','P_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','text','PP203',32,'','','txtPP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','text','PP204',32,'','','txtPP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5','name','CM202',0,'','','','КМ202','','','КМ202','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph','pgGrp','so',32,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph','curSek','1205241018',40,'','','trnd1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','name','CM202',41,'','','','КМ202','','','КМ202','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','name','Graphics group element',42,'Parameter|NAME','prm:/BlockCalc/CM202/ST8612/NAME','el1','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','name','Graphics group element',42,'Parameter|NAME','prm:/BlockCalc/CM202/F103/NAME','el2','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','name','Graphics group element',42,'Parameter|NAME','prm:/BlockCalc/CM202/P103/NAME','el3','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','name','Graphics group element',42,'Parameter|NAME','prm:/BlockCalc/CM202/PT1006_1/NAME','el4','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','name','Graphics group element',42,'Parameter|NAME','prm:/BlockCalc/CM202/PT0204/NAME','el5','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','name','Graphics group element',42,'Parameter|NAME','prm:/BlockCalc/CM202/TE1313_1/NAME','el6','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','name','Graphics group element',42,'Parameter|NAME','prm:/BlockCalc/CM202/TE1314_1/NAME','el7','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','name','Graphics group element',42,'Parameter|NAME','prm:/BlockCalc/CM202/TE1314_2/NAME','el8','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn','name','Mnemos',0,'','','','Мнемосхеми','','','Мнемосхемы','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn','dscr','Logical containers of page group: "Mnemos"',0,'','','','Логічний контейнер групи сторінок: "Мнемосхеми"','','','Логический контейнер группы страниц: "Мнемосхемы"','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','aMax',NULL,34,'Parameter|aMax','prm:/BlockCalc/CM202/ST8612','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','aMin',NULL,34,'Parameter|aMin','prm:/BlockCalc/CM202/ST8612','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','addr',NULL,42,'Parameter|var','prm:/BlockCalc/CM202/ST8612/var','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','ed',NULL,34,'Parameter|ed','prm:/BlockCalc/CM202/ST8612/ed','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','max',NULL,42,'Parameter|max','prm:/BlockCalc/CM202/ST8612/max','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','min',NULL,42,'Parameter|min','prm:/BlockCalc/CM202/ST8612/min','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','pModeA',NULL,34,'Parameter|auto','prm:/BlockCalc/CM202/ST8612','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','pModeC',NULL,34,'Parameter|casc','prm:/BlockCalc/CM202/ST8612','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','prec',NULL,34,'Parameter|prec','prm:/BlockCalc/CM202/ST8612/prec','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','wMax',NULL,34,'Parameter|wMax','prm:/BlockCalc/CM202/ST8612','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','wMin',NULL,34,'Parameter|wMin','prm:/BlockCalc/CM202/ST8612','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','aMax',NULL,34,'Parameter|aMax','prm:/BlockCalc/CM202/F103','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','aMin',NULL,34,'Parameter|aMin','prm:/BlockCalc/CM202/F103','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','addr',NULL,42,'Parameter|var','prm:/BlockCalc/CM202/F103/var','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','ed',NULL,34,'Parameter|ed','prm:/BlockCalc/CM202/F103/ed','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','max',NULL,42,'Parameter|max','prm:/BlockCalc/CM202/F103/max','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','min',NULL,42,'Parameter|min','prm:/BlockCalc/CM202/F103/min','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','pModeA',NULL,34,'Parameter|auto','prm:/BlockCalc/CM202/F103','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','pModeC',NULL,34,'Parameter|casc','prm:/BlockCalc/CM202/F103','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','prec',NULL,34,'Parameter|prec','prm:/BlockCalc/CM202/F103/prec','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','wMax',NULL,34,'Parameter|wMax','prm:/BlockCalc/CM202/F103','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','wMin',NULL,34,'Parameter|wMin','prm:/BlockCalc/CM202/F103','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','aMax',NULL,34,'Parameter|aMax','prm:/BlockCalc/CM202/P103','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','aMin',NULL,34,'Parameter|aMin','prm:/BlockCalc/CM202/P103','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','addr',NULL,42,'Parameter|var','prm:/BlockCalc/CM202/P103/var','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','ed',NULL,34,'Parameter|ed','prm:/BlockCalc/CM202/P103/ed','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','max',NULL,42,'Parameter|max','prm:/BlockCalc/CM202/P103/max','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','min',NULL,42,'Parameter|min','prm:/BlockCalc/CM202/P103/min','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','pModeA',NULL,34,'Parameter|auto','prm:/BlockCalc/CM202/P103','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','pModeC',NULL,34,'Parameter|casc','prm:/BlockCalc/CM202/P103','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','prec',NULL,34,'Parameter|prec','prm:/BlockCalc/CM202/P103/prec','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','wMax',NULL,34,'Parameter|wMax','prm:/BlockCalc/CM202/P103','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','wMin',NULL,34,'Parameter|wMin','prm:/BlockCalc/CM202/P103','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','aMax',NULL,34,'Parameter|aMax','prm:/BlockCalc/CM202/PT1006_1','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','aMin',NULL,34,'Parameter|aMin','prm:/BlockCalc/CM202/PT1006_1','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','addr',NULL,42,'Parameter|var','prm:/BlockCalc/CM202/PT1006_1/var','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','ed',NULL,34,'Parameter|ed','prm:/BlockCalc/CM202/PT1006_1/ed','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','max',NULL,42,'Parameter|max','prm:/BlockCalc/CM202/PT1006_1/max','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','min',NULL,42,'Parameter|min','prm:/BlockCalc/CM202/PT1006_1/min','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','pModeA',NULL,34,'Parameter|auto','prm:/BlockCalc/CM202/PT1006_1','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','pModeC',NULL,34,'Parameter|casc','prm:/BlockCalc/CM202/PT1006_1','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','prec',NULL,34,'Parameter|prec','prm:/BlockCalc/CM202/PT1006_1/prec','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','wMax',NULL,34,'Parameter|wMax','prm:/BlockCalc/CM202/PT1006_1','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','wMin',NULL,34,'Parameter|wMin','prm:/BlockCalc/CM202/PT1006_1','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','aMax',NULL,34,'Parameter|aMax','prm:/BlockCalc/CM202/PT0204','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','aMin',NULL,34,'Parameter|aMin','prm:/BlockCalc/CM202/PT0204','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','addr',NULL,42,'Parameter|var','prm:/BlockCalc/CM202/PT0204/var','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','ed',NULL,34,'Parameter|ed','prm:/BlockCalc/CM202/PT0204/ed','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','max',NULL,42,'Parameter|max','prm:/BlockCalc/CM202/PT0204/max','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','min',NULL,42,'Parameter|min','prm:/BlockCalc/CM202/PT0204/min','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','pModeA',NULL,34,'Parameter|auto','prm:/BlockCalc/CM202/PT0204','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','pModeC',NULL,34,'Parameter|casc','prm:/BlockCalc/CM202/PT0204','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','prec',NULL,34,'Parameter|prec','prm:/BlockCalc/CM202/PT0204/prec','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','wMax',NULL,34,'Parameter|wMax','prm:/BlockCalc/CM202/PT0204','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','wMin',NULL,34,'Parameter|wMin','prm:/BlockCalc/CM202/PT0204','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','aMax',NULL,34,'Parameter|aMax','prm:/BlockCalc/CM202/TE1313_1','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','aMin',NULL,34,'Parameter|aMin','prm:/BlockCalc/CM202/TE1313_1','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','addr',NULL,42,'Parameter|var','prm:/BlockCalc/CM202/TE1313_1/var','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','ed',NULL,34,'Parameter|ed','prm:/BlockCalc/CM202/TE1313_1/ed','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','max',NULL,42,'Parameter|max','prm:/BlockCalc/CM202/TE1313_1/max','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','min',NULL,42,'Parameter|min','prm:/BlockCalc/CM202/TE1313_1/min','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','pModeA',NULL,34,'Parameter|auto','prm:/BlockCalc/CM202/TE1313_1','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','pModeC',NULL,34,'Parameter|casc','prm:/BlockCalc/CM202/TE1313_1','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','prec',NULL,34,'Parameter|prec','prm:/BlockCalc/CM202/TE1313_1/prec','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','wMax',NULL,34,'Parameter|wMax','prm:/BlockCalc/CM202/TE1313_1','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','wMin',NULL,34,'Parameter|wMin','prm:/BlockCalc/CM202/TE1313_1','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','aMax',NULL,34,'Parameter|aMax','prm:/BlockCalc/CM202/TE1314_1','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','aMin',NULL,34,'Parameter|aMin','prm:/BlockCalc/CM202/TE1314_1','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','addr',NULL,42,'Parameter|var','prm:/BlockCalc/CM202/TE1314_1/var','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','ed',NULL,34,'Parameter|ed','prm:/BlockCalc/CM202/TE1314_1/ed','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','max',NULL,42,'Parameter|max','prm:/BlockCalc/CM202/TE1314_1/max','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','min',NULL,42,'Parameter|min','prm:/BlockCalc/CM202/TE1314_1/min','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','pModeA',NULL,34,'Parameter|auto','prm:/BlockCalc/CM202/TE1314_1','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','pModeC',NULL,34,'Parameter|casc','prm:/BlockCalc/CM202/TE1314_1','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','prec',NULL,34,'Parameter|prec','prm:/BlockCalc/CM202/TE1314_1/prec','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','wMax',NULL,34,'Parameter|wMax','prm:/BlockCalc/CM202/TE1314_1','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','wMin',NULL,34,'Parameter|wMin','prm:/BlockCalc/CM202/TE1314_1','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','aMax',NULL,34,'Parameter|aMax','prm:/BlockCalc/CM202/TE1314_2','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','aMin',NULL,34,'Parameter|aMin','prm:/BlockCalc/CM202/TE1314_2','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','addr',NULL,42,'Parameter|var','prm:/BlockCalc/CM202/TE1314_2/var','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','ed',NULL,34,'Parameter|ed','prm:/BlockCalc/CM202/TE1314_2/ed','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','max',NULL,42,'Parameter|max','prm:/BlockCalc/CM202/TE1314_2/max','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','min',NULL,42,'Parameter|min','prm:/BlockCalc/CM202/TE1314_2/min','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','pModeA',NULL,34,'Parameter|auto','prm:/BlockCalc/CM202/TE1314_2','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','pModeC',NULL,34,'Parameter|casc','prm:/BlockCalc/CM202/TE1314_2','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','prec',NULL,34,'Parameter|prec','prm:/BlockCalc/CM202/TE1314_2/prec','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','wMax',NULL,34,'Parameter|wMax','prm:/BlockCalc/CM202/TE1314_2','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','wMin',NULL,34,'Parameter|wMin','prm:/BlockCalc/CM202/TE1314_2','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6','name','CM301',0,'','','','КМ301','','','КМ301','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph','pgGrp','so',32,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph','curSek','1205241018',40,'','','trnd1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','name','CM301',41,'','','','КМ301','','','КМ301','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','name','Graphics group element',42,'Parameter|NAME','prm:/BlockCalc/CM301/ST8612/NAME','el1','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','aMax',NULL,34,'Parameter|aMax','prm:/BlockCalc/CM301/ST8612','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','aMin',NULL,34,'Parameter|aMin','prm:/BlockCalc/CM301/ST8612','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','addr',NULL,42,'Parameter|var','prm:/BlockCalc/CM301/ST8612/var','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','ed',NULL,34,'Parameter|ed','prm:/BlockCalc/CM301/ST8612/ed','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','max',NULL,42,'Parameter|max','prm:/BlockCalc/CM301/ST8612/max','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','min',NULL,42,'Parameter|min','prm:/BlockCalc/CM301/ST8612/min','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','pModeA',NULL,34,'Parameter|auto','prm:/BlockCalc/CM301/ST8612','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','pModeC',NULL,34,'Parameter|casc','prm:/BlockCalc/CM301/ST8612','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','prec',NULL,34,'Parameter|prec','prm:/BlockCalc/CM301/ST8612/prec','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','wMax',NULL,34,'Parameter|wMax','prm:/BlockCalc/CM301/ST8612','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','wMin',NULL,34,'Parameter|wMin','prm:/BlockCalc/CM301/ST8612','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','name','Graphics group element',42,'Parameter|NAME','prm:/BlockCalc/CM301/F101/NAME','el2','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','aMax',NULL,34,'Parameter|aMax','prm:/BlockCalc/CM301/F101','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','aMin',NULL,34,'Parameter|aMin','prm:/BlockCalc/CM301/F101','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','addr',NULL,42,'Parameter|var','prm:/BlockCalc/CM301/F101/var','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','ed',NULL,34,'Parameter|ed','prm:/BlockCalc/CM301/F101/ed','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','max',NULL,42,'Parameter|max','prm:/BlockCalc/CM301/F101/max','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','min',NULL,42,'Parameter|min','prm:/BlockCalc/CM301/F101/min','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','pModeA',NULL,34,'Parameter|auto','prm:/BlockCalc/CM301/F101','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','pModeC',NULL,34,'Parameter|casc','prm:/BlockCalc/CM301/F101','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','prec',NULL,34,'Parameter|prec','prm:/BlockCalc/CM301/F101/prec','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','wMax',NULL,34,'Parameter|wMax','prm:/BlockCalc/CM301/F101','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','wMin',NULL,34,'Parameter|wMin','prm:/BlockCalc/CM301/F101','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','name','Graphics group element',42,'Parameter|NAME','prm:/BlockCalc/CM301/P101/NAME','el3','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','aMax',NULL,34,'Parameter|aMax','prm:/BlockCalc/CM301/P101','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','aMin',NULL,34,'Parameter|aMin','prm:/BlockCalc/CM301/P101','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','addr',NULL,42,'Parameter|var','prm:/BlockCalc/CM301/P101/var','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','ed',NULL,34,'Parameter|ed','prm:/BlockCalc/CM301/P101/ed','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','max',NULL,42,'Parameter|max','prm:/BlockCalc/CM301/P101/max','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','min',NULL,42,'Parameter|min','prm:/BlockCalc/CM301/P101/min','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','pModeA',NULL,34,'Parameter|auto','prm:/BlockCalc/CM301/P101','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','pModeC',NULL,34,'Parameter|casc','prm:/BlockCalc/CM301/P101','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','prec',NULL,34,'Parameter|prec','prm:/BlockCalc/CM301/P101/prec','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','wMax',NULL,34,'Parameter|wMax','prm:/BlockCalc/CM301/P101','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','wMin',NULL,34,'Parameter|wMin','prm:/BlockCalc/CM301/P101','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','name','Graphics group element',42,'Parameter|NAME','prm:/BlockCalc/CM301/PT1006_1/NAME','el4','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','aMax',NULL,34,'Parameter|aMax','prm:/BlockCalc/CM301/PT1006_1','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','aMin',NULL,34,'Parameter|aMin','prm:/BlockCalc/CM301/PT1006_1','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','addr',NULL,42,'Parameter|var','prm:/BlockCalc/CM301/PT1006_1/var','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','ed',NULL,34,'Parameter|ed','prm:/BlockCalc/CM301/PT1006_1/ed','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','max',NULL,42,'Parameter|max','prm:/BlockCalc/CM301/PT1006_1/max','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','min',NULL,42,'Parameter|min','prm:/BlockCalc/CM301/PT1006_1/min','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','pModeA',NULL,34,'Parameter|auto','prm:/BlockCalc/CM301/PT1006_1','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','pModeC',NULL,34,'Parameter|casc','prm:/BlockCalc/CM301/PT1006_1','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','prec',NULL,34,'Parameter|prec','prm:/BlockCalc/CM301/PT1006_1/prec','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','wMax',NULL,34,'Parameter|wMax','prm:/BlockCalc/CM301/PT1006_1','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','wMin',NULL,34,'Parameter|wMin','prm:/BlockCalc/CM301/PT1006_1','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','name','Graphics group element',42,'Parameter|NAME','prm:/BlockCalc/CM301/PT0204/NAME','el5','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','aMax',NULL,34,'Parameter|aMax','prm:/BlockCalc/CM301/PT0204','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','aMin',NULL,34,'Parameter|aMin','prm:/BlockCalc/CM301/PT0204','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','addr',NULL,42,'Parameter|var','prm:/BlockCalc/CM301/PT0204/var','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','ed',NULL,34,'Parameter|ed','prm:/BlockCalc/CM301/PT0204/ed','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','max',NULL,42,'Parameter|max','prm:/BlockCalc/CM301/PT0204/max','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','min',NULL,42,'Parameter|min','prm:/BlockCalc/CM301/PT0204/min','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','pModeA',NULL,34,'Parameter|auto','prm:/BlockCalc/CM301/PT0204','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','pModeC',NULL,34,'Parameter|casc','prm:/BlockCalc/CM301/PT0204','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','prec',NULL,34,'Parameter|prec','prm:/BlockCalc/CM301/PT0204/prec','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','wMax',NULL,34,'Parameter|wMax','prm:/BlockCalc/CM301/PT0204','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','wMin',NULL,34,'Parameter|wMin','prm:/BlockCalc/CM301/PT0204','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','name','Graphics group element',42,'Parameter|NAME','prm:/BlockCalc/CM301/TE1313_1/NAME','el6','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','aMax',NULL,34,'Parameter|aMax','prm:/BlockCalc/CM301/TE1313_1','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','aMin',NULL,34,'Parameter|aMin','prm:/BlockCalc/CM301/TE1313_1','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','addr',NULL,42,'Parameter|var','prm:/BlockCalc/CM301/TE1313_1/var','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','ed',NULL,34,'Parameter|ed','prm:/BlockCalc/CM301/TE1313_1/ed','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','max',NULL,42,'Parameter|max','prm:/BlockCalc/CM301/TE1313_1/max','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','min',NULL,42,'Parameter|min','prm:/BlockCalc/CM301/TE1313_1/min','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','pModeA',NULL,34,'Parameter|auto','prm:/BlockCalc/CM301/TE1313_1','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','pModeC',NULL,34,'Parameter|casc','prm:/BlockCalc/CM301/TE1313_1','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','prec',NULL,34,'Parameter|prec','prm:/BlockCalc/CM301/TE1313_1/prec','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','wMax',NULL,34,'Parameter|wMax','prm:/BlockCalc/CM301/TE1313_1','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','wMin',NULL,34,'Parameter|wMin','prm:/BlockCalc/CM301/TE1313_1','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','name','Graphics group element',42,'Parameter|NAME','prm:/BlockCalc/CM301/TE1314_1/NAME','el7','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','aMax',NULL,34,'Parameter|aMax','prm:/BlockCalc/CM301/TE1314_1','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','aMin',NULL,34,'Parameter|aMin','prm:/BlockCalc/CM301/TE1314_1','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','addr',NULL,42,'Parameter|var','prm:/BlockCalc/CM301/TE1314_1/var','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','ed',NULL,34,'Parameter|ed','prm:/BlockCalc/CM301/TE1314_1/ed','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','max',NULL,42,'Parameter|max','prm:/BlockCalc/CM301/TE1314_1/max','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','min',NULL,42,'Parameter|min','prm:/BlockCalc/CM301/TE1314_1/min','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','pModeA',NULL,34,'Parameter|auto','prm:/BlockCalc/CM301/TE1314_1','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','pModeC',NULL,34,'Parameter|casc','prm:/BlockCalc/CM301/TE1314_1','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','prec',NULL,34,'Parameter|prec','prm:/BlockCalc/CM301/TE1314_1/prec','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','wMax',NULL,34,'Parameter|wMax','prm:/BlockCalc/CM301/TE1314_1','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','wMin',NULL,34,'Parameter|wMin','prm:/BlockCalc/CM301/TE1314_1','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','name','Graphics group element',42,'Parameter|NAME','prm:/BlockCalc/CM301/TE1314_2/NAME','el8','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','aMax',NULL,34,'Parameter|aMax','prm:/BlockCalc/CM301/TE1314_2','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','aMin',NULL,34,'Parameter|aMin','prm:/BlockCalc/CM301/TE1314_2','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','addr',NULL,42,'Parameter|var','prm:/BlockCalc/CM301/TE1314_2/var','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','ed',NULL,34,'Parameter|ed','prm:/BlockCalc/CM301/TE1314_2/ed','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','max',NULL,42,'Parameter|max','prm:/BlockCalc/CM301/TE1314_2/max','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','min',NULL,42,'Parameter|min','prm:/BlockCalc/CM301/TE1314_2/min','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','pModeA',NULL,34,'Parameter|auto','prm:/BlockCalc/CM301/TE1314_2','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','pModeC',NULL,34,'Parameter|casc','prm:/BlockCalc/CM301/TE1314_2','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','prec',NULL,34,'Parameter|prec','prm:/BlockCalc/CM301/TE1314_2/prec','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','wMax',NULL,34,'Parameter|wMax','prm:/BlockCalc/CM301/TE1314_2','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','wMin',NULL,34,'Parameter|wMin','prm:/BlockCalc/CM301/TE1314_2','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','curSek','1282204340',40,'','','trnd1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn','name','Mnemos',0,'','','','Мнемосхеми','','','Мнемосхемы','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn','dscr','Logical containers of page group: "Mnemos"',0,'','','','Логічний контейнер групи сторінок: "Мнемосхеми"','','','Логический контейнер группы страниц: "Мнемосхемы"','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','pgGrp','so',32,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','pName','F',33,'Parameter|NAME','F','F_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM301/F101/var','F_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM301/F101/prec','F_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','pName','F',33,'Parameter|NAME','F','F_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM301/F102/var','F_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM301/F102/prec','F_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','text','CM301/1',32,'','','txtCM101_1','КМ301/1','','КМ301/1','КМ301/1','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','text','CM301/2',32,'','','txtCM101_2','КМ301/2','','КМ301/2','КМ301/2','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','out','0',34,'Parameter|out','prm:/BlockCalc/gen_cntr/SurgeCM301/out','KPP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','pErr','<EVAL>',34,'Parameter|err','prm:/BlockCalc/CM301/ST8612/err','N_CM','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','pModeA','<EVAL>',34,'Parameter|auto','prm:/BlockCalc/CM301/ST8612','N_CM','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','pModeC','<EVAL>',34,'Parameter|casc','prm:/BlockCalc/CM301/ST8612','N_CM','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','pName','',34,'Parameter|NAME','prm:/BlockCalc/CM301/ST8612/NAME','N_CM','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','pPrec','<EVAL>',34,'Parameter|prec','prm:/BlockCalc/CM301/ST8612/prec','N_CM','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM301/ST8612/var','N_CM','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','text','KPP301',32,'','','txtKPP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','text','BC306',32,'','','Text3','КК306','','','КШ306','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','text','C301/3',32,'','','Text4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','text','C301/2',32,'','','Text5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','text','C301/1',32,'','','Text7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','pName','T',33,'Parameter|NAME','T','T_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM301/TE1313_1/var','T_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM301/TE1313_1/prec','T_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','pName','T',33,'Parameter|NAME','T','T_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM301/TE1314_1/var','T_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM301/TE1314_1/prec','T_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','com','0',38,'Parameter|com','prm:/BlockCalc/CM301/BC101/com','KS101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','shifr',NULL,38,'Parameter|NAME','prm:/BlockCalc/CM301/BC101/NAME','KS101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','st_close',NULL,38,'Parameter|st_close','prm:/BlockCalc/CM301/BC101/st_close','KS101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','st_open',NULL,38,'Parameter|st_open','prm:/BlockCalc/CM301/BC101/st_open','KS101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','com','0',38,'Parameter|com','prm:/BlockCalc/CM301/BC102/com','KS102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','shifr',NULL,38,'Parameter|NAME','prm:/BlockCalc/CM301/BC102/NAME','KS102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','st_close',NULL,38,'Parameter|st_close','prm:/BlockCalc/CM301/BC102/st_close','KS102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','st_open',NULL,38,'Parameter|st_open','prm:/BlockCalc/CM301/BC102/st_open','KS102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','com','0',38,'Parameter|com','prm:/BlockCalc/CM301/BC104/com','KS104','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','shifr',NULL,38,'Parameter|NAME','prm:/BlockCalc/CM301/BC104/NAME','KS104','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','st_close',NULL,38,'Parameter|st_close','prm:/BlockCalc/CM301/BC104/st_close','KS104','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','st_open',NULL,38,'Parameter|st_open','prm:/BlockCalc/CM301/BC104/st_open','KS104','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','com','0',38,'Parameter|com','prm:/BlockCalc/CM301/BC105/com','KS105','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','shifr',NULL,38,'Parameter|NAME','prm:/BlockCalc/CM301/BC105/NAME','KS105','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','st_close',NULL,38,'Parameter|st_close','prm:/BlockCalc/CM301/BC105/st_close','KS105','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','st_open',NULL,38,'Parameter|st_open','prm:/BlockCalc/CM301/BC105/st_open','KS105','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','com','0',38,'Parameter|com','prm:/BlockCalc/CM301/BC106/com','KS106','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','shifr',NULL,38,'Parameter|NAME','prm:/BlockCalc/CM301/BC106/NAME','KS106','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','st_close',NULL,38,'Parameter|st_close','prm:/BlockCalc/CM301/BC106/st_close','KS106','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','st_open',NULL,38,'Parameter|st_open','prm:/BlockCalc/CM301/BC106/st_open','KS106','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','pName','P',33,'Parameter|NAME','P','P_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM301/P101/var','P_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM301/P101/prec','P_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','pName','P',33,'Parameter|NAME','P','P_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM301/PT1006_1/var','P_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM301/PT1006_1/prec','P_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','text','BC301',32,'','','txtKS101','КК301','','','КШ301','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','text','BC302',32,'','','txtKS102','КК302','','','КШ302','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','text','BC303',32,'','','txtKS103','КК303','','','КШ303','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','text','BC304',32,'','','txtKS104','КК304','','','КШ304','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','text','BC305',32,'','','txtKS105','КК305','','','КШ305','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','text','BC307',32,'','','txtKS107','КК307','','','КШ307','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','pName','T',33,'Parameter|NAME','T','T_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM301/TT0202/var','T_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM301/TT0202/prec','T_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','pName','T',33,'Parameter|NAME','T','T_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM301/TT0204/var','T_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM301/TT0204/prec','T_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','text','BC321',32,'','','txtKS121','КК321','','','КШ321','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','text','BC322',32,'','','txtKS122','КК322','','','КШ322','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','text','BC323',32,'','','txtKS123','КК323','','','КШ323','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','text','AT301/1',32,'','','txtAT101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','text','AT301/2',32,'','','txtAT101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','pErr','<EVAL>',34,'Parameter|err','prm:/BlockCalc/CM301/TE1314_1/err','TAT101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','pModeA','<EVAL>',34,'Parameter|auto','prm:/BlockCalc/CM301/TE1314_1','TAT101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','pModeC','<EVAL>',34,'Parameter|casc','prm:/BlockCalc/CM301/TE1314_1','TAT101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','pName','',34,'Parameter|NAME','prm:/BlockCalc/CM301/TE1314_1/NAME','TAT101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','pPrec','<EVAL>',34,'Parameter|prec','prm:/BlockCalc/CM301/TE1314_1/prec','TAT101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM301/TE1314_1/var','TAT101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','pErr','<EVAL>',34,'Parameter|err','prm:/BlockCalc/CM301/TE1314_2/err','TAT101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','pModeA','<EVAL>',34,'Parameter|auto','prm:/BlockCalc/CM301/TE1314_2','TAT101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','pModeC','<EVAL>',34,'Parameter|casc','prm:/BlockCalc/CM301/TE1314_2','TAT101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','pName','',34,'Parameter|NAME','prm:/BlockCalc/CM301/TE1314_2/NAME','TAT101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','pPrec','<EVAL>',34,'Parameter|prec','prm:/BlockCalc/CM301/TE1314_2/prec','TAT101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM301/TE1314_2/var','TAT101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','pName','P',33,'Parameter|NAME','P','P_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM301/PT0202/var','P_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM301/PT0202/prec','P_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','pName','P',33,'Parameter|NAME','P','P_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM301/PT0204/var','P_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM301/PT0204/prec','P_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','text','PP301',32,'','','txtPP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','text','PP302',32,'','','txtPP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7','name','CM302',0,'','','','КМ302','','','КМ302','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph','pgGrp','so',32,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph','curSek','1205241018',40,'','','trnd1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','name','CM302',41,'','','','КМ302','','','КМ302','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','name','Graphics group element',42,'Parameter|NAME','prm:/BlockCalc/CM302/ST8612/NAME','el1','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','aMax',NULL,34,'Parameter|aMax','prm:/BlockCalc/CM302/ST8612','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','aMin',NULL,34,'Parameter|aMin','prm:/BlockCalc/CM302/ST8612','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','addr',NULL,42,'Parameter|var','prm:/BlockCalc/CM302/ST8612/var','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','ed',NULL,34,'Parameter|ed','prm:/BlockCalc/CM302/ST8612/ed','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','max',NULL,42,'Parameter|max','prm:/BlockCalc/CM302/ST8612/max','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','min',NULL,42,'Parameter|min','prm:/BlockCalc/CM302/ST8612/min','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','pModeA',NULL,34,'Parameter|auto','prm:/BlockCalc/CM302/ST8612','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','pModeC',NULL,34,'Parameter|casc','prm:/BlockCalc/CM302/ST8612','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','prec',NULL,34,'Parameter|prec','prm:/BlockCalc/CM302/ST8612/prec','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','wMax',NULL,34,'Parameter|wMax','prm:/BlockCalc/CM302/ST8612','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','wMin',NULL,34,'Parameter|wMin','prm:/BlockCalc/CM302/ST8612','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','name','Graphics group element',42,'Parameter|NAME','prm:/BlockCalc/CM302/F103/NAME','el2','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','aMax',NULL,34,'Parameter|aMax','prm:/BlockCalc/CM302/F103','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','aMin',NULL,34,'Parameter|aMin','prm:/BlockCalc/CM302/F103','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','addr',NULL,42,'Parameter|var','prm:/BlockCalc/CM302/F103/var','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','ed',NULL,34,'Parameter|ed','prm:/BlockCalc/CM302/F103/ed','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','max',NULL,42,'Parameter|max','prm:/BlockCalc/CM302/F103/max','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','min',NULL,42,'Parameter|min','prm:/BlockCalc/CM302/F103/min','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','pModeA',NULL,34,'Parameter|auto','prm:/BlockCalc/CM302/F103','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','pModeC',NULL,34,'Parameter|casc','prm:/BlockCalc/CM302/F103','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','prec',NULL,34,'Parameter|prec','prm:/BlockCalc/CM302/F103/prec','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','wMax',NULL,34,'Parameter|wMax','prm:/BlockCalc/CM302/F103','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','wMin',NULL,34,'Parameter|wMin','prm:/BlockCalc/CM302/F103','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','name','Graphics group element',42,'Parameter|NAME','prm:/BlockCalc/CM302/P103/NAME','el3','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','aMax',NULL,34,'Parameter|aMax','prm:/BlockCalc/CM302/P103','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','aMin',NULL,34,'Parameter|aMin','prm:/BlockCalc/CM302/P103','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','addr',NULL,42,'Parameter|var','prm:/BlockCalc/CM302/P103/var','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','ed',NULL,34,'Parameter|ed','prm:/BlockCalc/CM302/P103/ed','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','max',NULL,42,'Parameter|max','prm:/BlockCalc/CM302/P103/max','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','min',NULL,42,'Parameter|min','prm:/BlockCalc/CM302/P103/min','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','pModeA',NULL,34,'Parameter|auto','prm:/BlockCalc/CM302/P103','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','pModeC',NULL,34,'Parameter|casc','prm:/BlockCalc/CM302/P103','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','prec',NULL,34,'Parameter|prec','prm:/BlockCalc/CM302/P103/prec','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','wMax',NULL,34,'Parameter|wMax','prm:/BlockCalc/CM302/P103','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','wMin',NULL,34,'Parameter|wMin','prm:/BlockCalc/CM302/P103','el3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','name','Graphics group element',42,'Parameter|NAME','prm:/BlockCalc/CM302/PT1006_1/NAME','el4','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','aMax',NULL,34,'Parameter|aMax','prm:/BlockCalc/CM302/PT1006_1','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','aMin',NULL,34,'Parameter|aMin','prm:/BlockCalc/CM302/PT1006_1','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','addr',NULL,42,'Parameter|var','prm:/BlockCalc/CM302/PT1006_1/var','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','ed',NULL,34,'Parameter|ed','prm:/BlockCalc/CM302/PT1006_1/ed','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','max',NULL,42,'Parameter|max','prm:/BlockCalc/CM302/PT1006_1/max','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','min',NULL,42,'Parameter|min','prm:/BlockCalc/CM302/PT1006_1/min','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','pModeA',NULL,34,'Parameter|auto','prm:/BlockCalc/CM302/PT1006_1','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','pModeC',NULL,34,'Parameter|casc','prm:/BlockCalc/CM302/PT1006_1','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','prec',NULL,34,'Parameter|prec','prm:/BlockCalc/CM302/PT1006_1/prec','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','wMax',NULL,34,'Parameter|wMax','prm:/BlockCalc/CM302/PT1006_1','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','wMin',NULL,34,'Parameter|wMin','prm:/BlockCalc/CM302/PT1006_1','el4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','name','Graphics group element',42,'Parameter|NAME','prm:/BlockCalc/CM302/PT0204/NAME','el5','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','aMax',NULL,34,'Parameter|aMax','prm:/BlockCalc/CM302/PT0204','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','aMin',NULL,34,'Parameter|aMin','prm:/BlockCalc/CM302/PT0204','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','addr',NULL,42,'Parameter|var','prm:/BlockCalc/CM302/PT0204/var','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','ed',NULL,34,'Parameter|ed','prm:/BlockCalc/CM302/PT0204/ed','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','max',NULL,42,'Parameter|max','prm:/BlockCalc/CM302/PT0204/max','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','min',NULL,42,'Parameter|min','prm:/BlockCalc/CM302/PT0204/min','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','pModeA',NULL,34,'Parameter|auto','prm:/BlockCalc/CM302/PT0204','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','pModeC',NULL,34,'Parameter|casc','prm:/BlockCalc/CM302/PT0204','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','prec',NULL,34,'Parameter|prec','prm:/BlockCalc/CM302/PT0204/prec','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','wMax',NULL,34,'Parameter|wMax','prm:/BlockCalc/CM302/PT0204','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','wMin',NULL,34,'Parameter|wMin','prm:/BlockCalc/CM302/PT0204','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','name','Graphics group element',42,'Parameter|NAME','prm:/BlockCalc/CM302/TE1313_1/NAME','el6','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','aMax',NULL,34,'Parameter|aMax','prm:/BlockCalc/CM302/TE1313_1','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','aMin',NULL,34,'Parameter|aMin','prm:/BlockCalc/CM302/TE1313_1','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','addr',NULL,42,'Parameter|var','prm:/BlockCalc/CM302/TE1313_1/var','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','ed',NULL,34,'Parameter|ed','prm:/BlockCalc/CM302/TE1313_1/ed','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','max',NULL,42,'Parameter|max','prm:/BlockCalc/CM302/TE1313_1/max','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','min',NULL,42,'Parameter|min','prm:/BlockCalc/CM302/TE1313_1/min','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','pModeA',NULL,34,'Parameter|auto','prm:/BlockCalc/CM302/TE1313_1','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','pModeC',NULL,34,'Parameter|casc','prm:/BlockCalc/CM302/TE1313_1','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','prec',NULL,34,'Parameter|prec','prm:/BlockCalc/CM302/TE1313_1/prec','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','wMax',NULL,34,'Parameter|wMax','prm:/BlockCalc/CM302/TE1313_1','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','wMin',NULL,34,'Parameter|wMin','prm:/BlockCalc/CM302/TE1313_1','el6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','name','Graphics group element',42,'Parameter|NAME','prm:/BlockCalc/CM302/TE1314_1/NAME','el7','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','aMax',NULL,34,'Parameter|aMax','prm:/BlockCalc/CM302/TE1314_1','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','aMin',NULL,34,'Parameter|aMin','prm:/BlockCalc/CM302/TE1314_1','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','addr',NULL,42,'Parameter|var','prm:/BlockCalc/CM302/TE1314_1/var','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','ed',NULL,34,'Parameter|ed','prm:/BlockCalc/CM302/TE1314_1/ed','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','max',NULL,42,'Parameter|max','prm:/BlockCalc/CM302/TE1314_1/max','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','min',NULL,42,'Parameter|min','prm:/BlockCalc/CM302/TE1314_1/min','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','pModeA',NULL,34,'Parameter|auto','prm:/BlockCalc/CM302/TE1314_1','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','pModeC',NULL,34,'Parameter|casc','prm:/BlockCalc/CM302/TE1314_1','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','prec',NULL,34,'Parameter|prec','prm:/BlockCalc/CM302/TE1314_1/prec','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','wMax',NULL,34,'Parameter|wMax','prm:/BlockCalc/CM302/TE1314_1','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','wMin',NULL,34,'Parameter|wMin','prm:/BlockCalc/CM302/TE1314_1','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','name','Graphics group element',42,'Parameter|NAME','prm:/BlockCalc/CM302/TE1314_2/NAME','el8','Елемент групи графіків','','','Элемент группы графиков','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','aMax',NULL,34,'Parameter|aMax','prm:/BlockCalc/CM302/TE1314_2','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','aMin',NULL,34,'Parameter|aMin','prm:/BlockCalc/CM302/TE1314_2','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','addr',NULL,42,'Parameter|var','prm:/BlockCalc/CM302/TE1314_2/var','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','ed',NULL,34,'Parameter|ed','prm:/BlockCalc/CM302/TE1314_2/ed','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','max',NULL,42,'Parameter|max','prm:/BlockCalc/CM302/TE1314_2/max','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','min',NULL,42,'Parameter|min','prm:/BlockCalc/CM302/TE1314_2/min','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','pModeA',NULL,34,'Parameter|auto','prm:/BlockCalc/CM302/TE1314_2','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','pModeC',NULL,34,'Parameter|casc','prm:/BlockCalc/CM302/TE1314_2','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','prec',NULL,34,'Parameter|prec','prm:/BlockCalc/CM302/TE1314_2/prec','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','wMax',NULL,34,'Parameter|wMax','prm:/BlockCalc/CM302/TE1314_2','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','wMin',NULL,34,'Parameter|wMin','prm:/BlockCalc/CM302/TE1314_2','el8','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','curSek','1553365960',40,'','','trnd1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn','name','Mnemos',0,'','','','Мнемосхеми','','','Мнемосхемы','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn','dscr','Logical containers of page group: "Mnemos"',0,'','','','Логічний контейнер групи сторінок: "Мнемосхеми"','','','Логический контейнер группы страниц: "Мнемосхемы"','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','name','CM102',32,'','','','КМ302','','','КМ102','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','pgGrp','so',32,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','pName','F',33,'Parameter|NAME','F','F_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM302/F103/var','F_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM302/F103/prec','F_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','pName','F',33,'Parameter|NAME','F','F_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM302/F104/var','F_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM302/F104/prec','F_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','text','CM302/1',32,'','','txtCM101_1','КМ302/1','','','КМ302/1','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','text','CM302/2',32,'','','txtCM101_2','КМ302/2','','','КМ302/2','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','out','0',34,'Parameter|out','prm:/BlockCalc/CM302/KPP102/out','KPP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','pErr','<EVAL>',34,'Parameter|err','prm:/BlockCalc/CM302/ST8612/err','N_CM','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','pModeA','<EVAL>',34,'Parameter|auto','prm:/BlockCalc/CM302/ST8612','N_CM','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','pModeC','<EVAL>',34,'Parameter|casc','prm:/BlockCalc/CM302/ST8612','N_CM','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','pName','',34,'Parameter|NAME','prm:/BlockCalc/CM302/ST8612/NAME','N_CM','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','pPrec','<EVAL>',34,'Parameter|prec','prm:/BlockCalc/CM302/ST8612/prec','N_CM','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM302/ST8612/var','N_CM','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','text','KPP302',32,'','','txtKPP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','text','From PP5',32,'','','Text1','З PP5','','','Из PP5','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','text','BC316',32,'','','Text3','КК316','','','КШ316','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','text','C302/3',32,'','','Text4','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','text','C302/2',32,'','','Text5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','text','To PP7',32,'','','Text6','До PP7','','','На PP7','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','text','C302/1',32,'','','Text7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','pName','T',33,'Parameter|NAME','T','T_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM302/TE1313_1/var','T_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM302/TE1313_1/prec','T_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','pName','T',33,'Parameter|NAME','T','T_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM302/TE1314_1/var','T_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM302/TE1314_1/prec','T_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','com','0',38,'Parameter|com','prm:/BlockCalc/CM302/BC111/com','KS101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','shifr',NULL,38,'Parameter|NAME','prm:/BlockCalc/CM302/BC111/NAME','KS101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','st_close',NULL,38,'Parameter|st_close','prm:/BlockCalc/CM302/BC111/st_close','KS101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','st_open',NULL,38,'Parameter|st_open','prm:/BlockCalc/CM302/BC111/st_open','KS101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','com','0',38,'Parameter|com','prm:/BlockCalc/CM302/BC112/com','KS102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','shifr',NULL,38,'Parameter|NAME','prm:/BlockCalc/CM302/BC112/NAME','KS102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','st_close',NULL,38,'Parameter|st_close','prm:/BlockCalc/CM302/BC112/st_close','KS102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','st_open',NULL,38,'Parameter|st_open','prm:/BlockCalc/CM302/BC112/st_open','KS102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','com','0',38,'Parameter|com','prm:/BlockCalc/CM302/BC114/com','KS104','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','shifr',NULL,38,'Parameter|NAME','prm:/BlockCalc/CM302/BC114/NAME','KS104','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','st_close',NULL,38,'Parameter|st_close','prm:/BlockCalc/CM302/BC114/st_close','KS104','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','st_open',NULL,38,'Parameter|st_open','prm:/BlockCalc/CM302/BC114/st_open','KS104','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','com','0',38,'Parameter|com','prm:/BlockCalc/CM302/BC115/com','KS105','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','shifr',NULL,38,'Parameter|NAME','prm:/BlockCalc/CM302/BC115/NAME','KS105','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','st_close',NULL,38,'Parameter|st_close','prm:/BlockCalc/CM302/BC115/st_close','KS105','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','st_open',NULL,38,'Parameter|st_open','prm:/BlockCalc/CM302/BC115/st_open','KS105','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','com','0',38,'Parameter|com','prm:/BlockCalc/CM302/BC116/com','KS106','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','shifr',NULL,38,'Parameter|NAME','prm:/BlockCalc/CM302/BC116/NAME','KS106','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','st_close',NULL,38,'Parameter|st_close','prm:/BlockCalc/CM302/BC116/st_close','KS106','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','st_open',NULL,38,'Parameter|st_open','prm:/BlockCalc/CM302/BC116/st_open','KS106','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','pName','P',33,'Parameter|NAME','P','P_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM302/P103/var','P_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM302/P103/prec','P_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','pName','P',33,'Parameter|NAME','P','P_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM302/PT1006_1/var','P_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM302/PT1006_1/prec','P_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','text','BC311',32,'','','txtKS101','КК311','','','КШ311','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','text','BC312',32,'','','txtKS102','КК312','','','КШ312','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','text','BC313',32,'','','txtKS103','КК313','','','КШ313','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','text','BC314',32,'','','txtKS104','КК314','','','КШ314','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','text','BC315',32,'','','txtKS105','КК315','','','КШ315','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','text','BC317',32,'','','txtKS107','КК317','','','КШ317','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','pName','T',33,'Parameter|NAME','T','T_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM302/TT0202/var','T_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM302/TT0202/prec','T_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','pName','T',33,'Parameter|NAME','T','T_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM302/TT0204/var','T_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM302/TT0204/prec','T_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','text','BC325',32,'','','txtKS121','КК325','','','КШ325','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','text','BC326',32,'','','txtKS122','КК326','','','КШ326','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','text','BC334',32,'','','txtKS123','КК334','','','КШ334','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','text','AT302/1',32,'','','txtAT101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','geomY','301',32,'','','txtAT101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','text','AT302/2',32,'','','txtAT101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','pErr','<EVAL>',34,'Parameter|err','prm:/BlockCalc/CM302/TE1314_1/err','TAT101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','pModeA','<EVAL>',34,'Parameter|auto','prm:/BlockCalc/CM302/TE1314_1','TAT101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','pModeC','<EVAL>',34,'Parameter|casc','prm:/BlockCalc/CM302/TE1314_1','TAT101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','pName','',34,'Parameter|NAME','prm:/BlockCalc/CM302/TE1314_1/NAME','TAT101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','pPrec','<EVAL>',34,'Parameter|prec','prm:/BlockCalc/CM302/TE1314_1/prec','TAT101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM302/TE1314_1/var','TAT101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','pErr','<EVAL>',34,'Parameter|err','prm:/BlockCalc/CM302/TE1314_2/err','TAT101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','pModeA','<EVAL>',34,'Parameter|auto','prm:/BlockCalc/CM302/TE1314_2','TAT101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','pModeC','<EVAL>',34,'Parameter|casc','prm:/BlockCalc/CM302/TE1314_2','TAT101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','pName','',34,'Parameter|NAME','prm:/BlockCalc/CM302/TE1314_2/NAME','TAT101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','pPrec','<EVAL>',34,'Parameter|prec','prm:/BlockCalc/CM302/TE1314_2/prec','TAT101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM302/TE1314_2/var','TAT101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','pName','P',33,'Parameter|NAME','P','P_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM302/PT0202/var','P_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM302/PT0202/prec','P_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','pName','P',33,'Parameter|NAME','P','P_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','pVal','0',34,'Parameter|var','prm:/BlockCalc/CM302/PT0204/var','P_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','prec','1',34,'Parameter|prec','prm:/BlockCalc/CM302/PT0204/prec','P_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','text','PP303',32,'','','txtPP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','text','PP304',32,'','','txtPP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','name','CM301',32,'','','','КМ301','','','КМ301','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pVar','<EVAL>',34,'Parameter|var','prm:/BlockCalc/CM101/ST8612/var','comprEn101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pVar','<EVAL>',34,'Parameter|var','prm:/BlockCalc/CM102/ST8612/var','comprEn102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pVar','<EVAL>',34,'Parameter|var','prm:/BlockCalc/CM201/ST8612/var','comprEn201','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pVar','<EVAL>',34,'Parameter|var','prm:/BlockCalc/CM202/ST8612/var','comprEn202','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pVar','<EVAL>',34,'Parameter|var','prm:/BlockCalc/CM301/ST8612/var','comprEn301','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pVar','<EVAL>',34,'Parameter|var','prm:/BlockCalc/CM302/ST8612/var','comprEn302','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_ElCadr','prmVarIn','<EVAL>',46,'<page>|varIn','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmVarIn','<EVAL>',38,'Parameter|varIn','prm:/LogicLev/gen/Pi','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmVarIn','<EVAL>',38,'Parameter|varIn','prm:/BlockCalc/gen_cntr/PC_PCV1','el2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmVarIn','<EVAL>',38,'Parameter|varIn','prm:/BlockCalc/gen_cntr/PC_PCV2','el5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr/pg_1','prmVarIn','<EVAL>',38,'Parameter|varIn','prm:/BlockCalc/gen_cntr/PC_PCV3','el7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','curSek','1283324818',40,'','','trnd1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2','dscr','Compressor CM101',0,'','','','Компресор КМ101','','','Компрессор КМ101','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3','dscr','Compressor CM102',0,'','','','Компресор КМ102','','','Компрессор КМ102','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4','dscr','Compressor CM201',0,'','','','Компресор КМ201','','','Компрессор КМ201','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5','dscr','Compressor CM202',0,'','','','Компресор КМ202','','','Компрессор КМ202','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6','dscr','Compressor CM301',0,'','','','Компресор КМ301','','','Компрессор КМ301','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7','dscr','Compressor CM302',0,'','','','Компресор КМ302','','','Компрессор КМ302','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','dscr','',34,'Parameter|DESCR','prm:/LogicLev/gen/Pi/DESCR','el1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg','grpName','Common',41,'','Generic','','Загальні','','Загальні','Общие','','Общие');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_treeSelect','value','',0,'','','apply','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_treeSelect','value','',0,'','','cancel','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_10','prExtCurCom','0',34,'Controller|curCom','prm:/LogicLev/prescription/manager/curCom','prescrRunSimple','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_10','prExtMode','0',38,'Controller|mode','prm:/LogicLev/prescription/manager/mode','prescrRunSimple','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_10','prExtProg','',38,'Controller|prog','prm:/LogicLev/prescription/manager/prog','prescrRunSimple','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_10','prExtStartTm','0',34,'Controller|startTm','prm:/LogicLev/prescription/manager/startTm','prescrRunSimple','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_10','prExtWork','<TVarObj>
</TVarObj>
',34,'Controller|work','prm:/LogicLev/prescription/manager/work','prescrRunSimple','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pErr','',34,'Parameter|err','prm:/LogicLev/gen/F_PP1/err','F_PP1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pNAME','',34,'Parameter|NAME','prm:/LogicLev/gen/F_PP1/NAME','F_PP1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pErr','',34,'Parameter|err','prm:/LogicLev/gen/F_PP3/err','F_PP3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pNAME','',34,'Parameter|NAME','prm:/LogicLev/gen/F_PP3/NAME','F_PP3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pErr','',34,'Parameter|err','prm:/LogicLev/gen/F_PP5/err','F_PP5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pNAME','',34,'Parameter|NAME','prm:/LogicLev/gen/F_PP5/NAME','F_PP5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pErr','',34,'Parameter|err','prm:/LogicLev/gen/P_PP1/err','P_PP1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pNAME','',34,'Parameter|NAME','prm:/LogicLev/gen/P_PP1/NAME','P_PP1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pErr','',34,'Parameter|err','prm:/LogicLev/gen/P_PP3/err','P_PP3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pNAME','',34,'Parameter|NAME','prm:/LogicLev/gen/P_PP3/NAME','P_PP3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pErr','',34,'Parameter|err','prm:/LogicLev/gen/P_PP5/err','P_PP5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pNAME','',34,'Parameter|NAME','prm:/LogicLev/gen/P_PP5/NAME','P_PP5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pErr','',34,'Parameter|err','prm:/LogicLev/gen/T_PP1/err','T_PP1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pNAME','',34,'Parameter|NAME','prm:/LogicLev/gen/T_PP1/NAME','T_PP1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pErr','',34,'Parameter|err','prm:/LogicLev/gen/T_PP3/err','T_PP3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pNAME','',34,'Parameter|NAME','prm:/LogicLev/gen/T_PP3/NAME','T_PP3','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pErr','',34,'Parameter|err','prm:/LogicLev/gen/T_PP5/err','T_PP5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pNAME','',34,'Parameter|NAME','prm:/LogicLev/gen/T_PP5/NAME','T_PP5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pErr','',34,'Parameter|err','prm:/BlockCalc/CM101/F101/err','F_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM101/F101/NAME','F_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pErr','',34,'Parameter|err','prm:/BlockCalc/CM101/F102/err','F_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM101/F102/NAME','F_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pErr','',34,'Parameter|err','prm:/BlockCalc/CM101/PT0202/err','P_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM101/PT0202/NAME','P_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pNameSz','0',33,'','','P_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pErr','',34,'Parameter|err','prm:/BlockCalc/CM101/PT0204/err','P_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM101/PT0204/NAME','P_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pErr','',34,'Parameter|err','prm:/BlockCalc/CM101/P101/err','P_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM101/P101/NAME','P_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pErr','',34,'Parameter|err','prm:/BlockCalc/CM101/PT1006_1/err','P_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM101/PT1006_1/NAME','P_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pErr','',34,'Parameter|err','prm:/BlockCalc/CM101/TT0202/err','T_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM101/TT0202/NAME','T_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pErr','',34,'Parameter|err','prm:/BlockCalc/CM101/TT0204/err','T_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM101/TT0204/NAME','T_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pErr','',34,'Parameter|err','prm:/BlockCalc/CM101/TE1313_1/err','T_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM101/TE1313_1/NAME','T_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pErr','',34,'Parameter|err','prm:/BlockCalc/CM101/TE1314_1/err','T_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM101/TE1314_1/NAME','T_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','pErr','',34,'Parameter|err','prm:/BlockCalc/CM201/F101/err','F_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM201/F101/NAME','F_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','pErr','',34,'Parameter|err','prm:/BlockCalc/CM201/F102/err','F_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM201/F102/NAME','F_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','pErr','',34,'Parameter|err','prm:/BlockCalc/CM201/PT0202/err','P_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM201/PT0202/NAME','P_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','pErr','',34,'Parameter|err','prm:/BlockCalc/CM201/PT0204/err','P_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM201/PT0204/NAME','P_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','pErr','',34,'Parameter|err','prm:/BlockCalc/CM201/P101/err','P_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM201/P101/NAME','P_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','pErr','',34,'Parameter|err','prm:/BlockCalc/CM201/PT1006_1/err','P_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM201/PT1006_1/NAME','P_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','pErr','',34,'Parameter|err','prm:/BlockCalc/CM201/TT0202/err','T_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM201/TT0202/NAME','T_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','pErr','',34,'Parameter|err','prm:/BlockCalc/CM201/TT0204/err','T_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM201/TT0204/NAME','T_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','pErr','',34,'Parameter|err','prm:/BlockCalc/CM201/TE1313_1/err','T_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM201/TE1313_1/NAME','T_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','pErr','',34,'Parameter|err','prm:/BlockCalc/CM201/TE1314_1/err','T_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM201/TE1314_1/NAME','T_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','pErr','',34,'Parameter|err','prm:/BlockCalc/CM301/F101/err','F_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM301/F101/NAME','F_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','pErr','',34,'Parameter|err','prm:/BlockCalc/CM301/F102/err','F_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM301/F102/NAME','F_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','pErr','',34,'Parameter|err','prm:/BlockCalc/CM301/PT0202/err','P_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM301/PT0202/NAME','P_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','pErr','',34,'Parameter|err','prm:/BlockCalc/CM301/PT0204/err','P_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM301/PT0204/NAME','P_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','pErr','',34,'Parameter|err','prm:/BlockCalc/CM301/P101/err','P_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM301/P101/NAME','P_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','pErr','',34,'Parameter|err','prm:/BlockCalc/CM301/PT1006_1/err','P_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM301/PT1006_1/NAME','P_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','pErr','',34,'Parameter|err','prm:/BlockCalc/CM301/TT0202/err','T_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM301/TT0202/NAME','T_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','pErr','',34,'Parameter|err','prm:/BlockCalc/CM301/TT0204/err','T_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM301/TT0204/NAME','T_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','pErr','',34,'Parameter|err','prm:/BlockCalc/CM301/TE1313_1/err','T_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM301/TE1313_1/NAME','T_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','pErr','',34,'Parameter|err','prm:/BlockCalc/CM301/TE1314_1/err','T_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM301/TE1314_1/NAME','T_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','pErr','',34,'Parameter|err','prm:/BlockCalc/CM102/F103/err','F_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM102/F103/NAME','F_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','pErr','',34,'Parameter|err','prm:/BlockCalc/CM102/F104/err','F_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM102/F104/NAME','F_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','pErr','',34,'Parameter|err','prm:/BlockCalc/CM102/P103/err','P_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM102/P103/NAME','P_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','pErr','',34,'Parameter|err','prm:/BlockCalc/CM102/PT1006_1/err','P_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM102/PT1006_1/NAME','P_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','pErr','',34,'Parameter|err','prm:/BlockCalc/CM102/TE1313_1/err','T_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM102/TE1313_1/NAME','T_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','pErr','',34,'Parameter|err','prm:/BlockCalc/CM102/TE1314_1/err','T_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM102/TE1314_1/NAME','T_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','pErr','',34,'Parameter|err','prm:/BlockCalc/CM202/F103','F_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM202/F103','F_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','pErr','',34,'Parameter|err','prm:/BlockCalc/CM202/F104','F_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM202/F104','F_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','pErr','',34,'Parameter|err','prm:/BlockCalc/CM202/P103','P_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM202/P103','P_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','pErr','',34,'Parameter|err','prm:/BlockCalc/CM202/PT1006_1','P_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM202/PT1006_1','P_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','pErr','',34,'Parameter|err','prm:/BlockCalc/CM202/TE1313_1','T_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM202/TE1313_1','T_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','pErr','',34,'Parameter|err','prm:/BlockCalc/CM202/TE1314_1','T_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM202/TE1314_1','T_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','pErr','',34,'Parameter|err','prm:/BlockCalc/CM302/F103/err','F_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM302/F103/NAME','F_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','pErr','',34,'Parameter|err','prm:/BlockCalc/CM302/F104/err','F_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM302/F104/NAME','F_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','pErr','',34,'Parameter|err','prm:/BlockCalc/CM302/P103/err','P_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM302/P103/NAME','P_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','pErr','',34,'Parameter|err','prm:/BlockCalc/CM302/PT1006_1/err','P_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM302/PT1006_1/NAME','P_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','pErr','',34,'Parameter|err','prm:/BlockCalc/CM302/TE1313_1/err','T_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM302/TE1313_1/NAME','T_PP101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','pErr','',34,'Parameter|err','prm:/BlockCalc/CM302/TE1314_1/err','T_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM302/TE1314_1/NAME','T_PP102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','pErr','',34,'Parameter|err','prm:/BlockCalc/CM102/PT0202/err','P_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM102/PT0202/NAME','P_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','pErr','',34,'Parameter|err','prm:/BlockCalc/CM102/PT0204/err','P_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM102/PT0204/NAME','P_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','pErr','',34,'Parameter|err','prm:/BlockCalc/CM102/TT0202/err','T_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM102/TT0202/NAME','T_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','pErr','',34,'Parameter|err','prm:/BlockCalc/CM102/TT0204/err','T_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM102/TT0204/NAME','T_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','pErr','',34,'Parameter|err','prm:/BlockCalc/CM202/PT0202','P_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM202/PT0202','P_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','pErr','',34,'Parameter|err','prm:/BlockCalc/CM202/PT0204','P_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM202/PT0204','P_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','pErr','',34,'Parameter|err','prm:/BlockCalc/CM202/TT0202','T_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM202/TT0202','T_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','pErr','',34,'Parameter|err','prm:/BlockCalc/CM202/TT0204','T_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM202/TT0204','T_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','pErr','',34,'Parameter|err','prm:/BlockCalc/CM302/PT0202/err','P_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM302/PT0202/NAME','P_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','pErr','',34,'Parameter|err','prm:/BlockCalc/CM302/PT0204/err','P_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM302/PT0204/NAME','P_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','pErr','',34,'Parameter|err','prm:/BlockCalc/CM302/TT0202/err','T_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM302/TT0202/NAME','T_CM101_1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','pErr','',34,'Parameter|err','prm:/BlockCalc/CM302/TT0204/err','T_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','pNAME','',34,'Parameter|NAME','prm:/BlockCalc/CM302/TT0204/NAME','T_CM101_2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','digComs','',34,'Parameter|digComs','prm:/BlockCalc/gen/BC1','BC1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','digStts','',34,'Parameter|digStts','prm:/BlockCalc/gen/BC1','BC1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so','geomX','4',32,'','','alarms','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so','geomY','605',32,'','','alarms','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so','geomW','900',32,'','','alarms','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so','geomH','65',32,'','','alarms','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so','geomZ','34',32,'','','alarms','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so','headVis','0',32,'','','alarms','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so','tSize','0',32,'','','alarms','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so','trcPer','1',32,'','','alarms','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so','lev','-1',32,'','','alarms','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so','viewOrd','5',32,'','','alarms','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so','col','tm;cat;mess',32,'','','alarms','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so','itProp','3',32,'','','alarms','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so','it0lev','5',0,'','','alarms','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so','it0fnt','',0,'','','alarms','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so','it0color','gray',0,'','','alarms','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so','it1lev','4',0,'','','alarms','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so','it1color','red',0,'','','alarms','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so','it2lev','0',0,'','','alarms','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so','it2color','yellow',0,'','','alarms','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so','geomH','551',32,'','','pgCont','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so','geomZ','27',32,'','','pgCont','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph','geomYsc','0.96',32,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph','geomYsc','0.96',32,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrRegul','geomYsc','0.96',32,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gcadr','geomYsc','0.96',32,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_gview','geomYsc','0.96',32,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph','geomYsc','0.96',32,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph','geomYsc','0.96',32,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph','geomYsc','0.96',32,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph','geomYsc','0.96',32,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph','geomYsc','0.96',32,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg','geomYsc','0.96',32,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_1','geomYsc','0.96',32,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_2','geomYsc','0.96',32,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_3','geomYsc','0.96',32,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_4','geomYsc','0.96',32,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_5','geomYsc','0.96',32,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_6','geomYsc','0.96',32,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pgNoOpenProc','1',0,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pNameSz','15',33,'','','F_PP1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','spName','Gas flow through the diaphragm one',33,'','Gas flow through the diaphragm one','F_PP1','Витрати газу через діафрагму один','','Витрати газу через діафрагму один','Расход газа через диафрагму один','','Расход газа через диафрагму один');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','spName','Gas flow through the diaphragm three',33,'','Gas flow through the diaphragm three','F_PP3','Витрати газу через діафрагму три','','Витрати газу через діафрагму три','Расход газа через диафрагму три','','Расход газа через диафрагму три');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','spName','Gas flow through the diaphragm five',33,'','Gas flow through the diaphragm five','F_PP5','Витрати газу через діафрагму п''ять','','Витрати газу через діафрагму п''ять','Расход газа через диафрагму пять','','Расход газа через диафрагму пять');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','dscr','Pages group: "Generic"',32,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_2','dscr','Pages group: "Temperatures"',32,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','dscr','Pages group: "CM101"',32,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_ggraph/pg_1','dscr','Pages group: "CM102"',32,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_ggraph/pg_1','dscr','Pages group: "CM201"',32,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_ggraph/pg_1','dscr','Pages group: "CM202"',32,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_ggraph/pg_1','dscr','Pages group: "CM301"',32,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_ggraph/pg_1','dscr','Pages group: "CM302"',32,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_ElCadr','prmComText','',46,'<page>|com_text','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_ElCadr','prmStText','',42,'<page>|st_text','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','spName','Gas pressure at the input',33,'','Gas pressure at the input','Pi','','','','Давление газа на входе','','Давление газа на входе');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','spName','Gas temperature at the input',33,'','Gas temperature at the input','Ti','','','','Температура газа на входе','','Температура газа на входе');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','pNameSz','15',33,'','','P_PP1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','spName','Gas pressure at the diaphragm one',33,'','Gas pressure at the diaphragm one','P_PP1','','','','Давление газа на диафрагме один','','Давление газа на диафрагме один');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','spName','Gas temperature at the diaphragm one',33,'','Gas temperature at the diaphragm one','T_PP1','','','','Температура газа на диафрагме один','','Температура газа на диафрагме один');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','spName','Gas pressure at the diaphragm three',33,'','Gas pressure at the diaphragm three','P_PP3','','','','Давление газа на диафрагме три','','Давление газа на диафрагме три');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','spName','Gas pressure at the diaphragm five',33,'','Gas pressure at the diaphragm five','P_PP5','','','','Давление газа на диафрагме пять','','Давление газа на диафрагме пять');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','spName','Gas temperature at the diaphragm three',33,'','Gas temperature at the diaphragm three','T_PP3','','','','Температура газа на диафрагме три','','Температура газа на диафрагме три');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','spName','Gas temperature at the diaphragm five',33,'','Gas temperature at the diaphragm five','T_PP5','','','','Температура газа на диафрагме пять','','Температура газа на диафрагме пять');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrPasp','owner','root:op',32,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrPasp','name','Passport',32,'','','','Паспорт','','','Паспорт','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrPasp','geomZ','9',32,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrPasp','pName',NULL,42,'<page>|NAME','','','','<page>|NAME','','','<page>|NAME','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrPasp','owner','root:op',32,'','','data','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrPasp','owner','root:op',32,'','','sel_data','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrPasp','value','0',0,'','','sel_data','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrPasp','owner','root:op',32,'','','sel_trends','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrPasp','value','0',0,'','','sel_trends','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrPasp','owner','root:op',32,'','','sel_view','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrPasp','value','1',0,'','','sel_view','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrPasp','owner','root:op',32,'','','trends','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrPasp','owner','root:op',32,'','','trendsList','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrPasp','owner','root:op',32,'','','view','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrPasp','owner','root:op',32,'','','viewLabSet','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrPasp','owner','root:op',32,'','','viewSet','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrPasp','value','',8,'','','viewSet','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrPaspOld','owner','root:op',32,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrPaspOld','geomZ','9',32,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrPaspOld','pName',NULL,42,'<page>|NAME','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_cntrPaspOld','owner','root:UI',32,'','','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_ggraph/pg_1','sclWin','0',160,'','','trnd1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_1','name','Instantaneous values of PP1',32,'','','','Миттєві значення PP1','','','Мгновенные значения PP1','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_3','name','Hour-averaged values of PP1',32,'','','','Середні за годину значення PP1','','','Средние за час значения PP1','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_3','title','Diaphragm PP1',33,'','Diaphragm PP1','doc','Діафрагма PP1','','Діафрагма PP1','Диафрагма PP1','','Диафрагма PP1');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_4','name','Day-averaged values of PP1',32,'','','','Середні за добу значення PP1','','','Средние за сутки значения PP1','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_4','title','Diaphragm PP1',33,'','Diaphragm PP1','doc','Діафрагма PP1','','Діафрагма PP1','Диафрагма PP1','','Диафрагма PP1');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_5','name','Day report of PP1',32,'','','','Добовий звіт для PP1','','','Суточный отчёт для PP1','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_5','pEnterpr','Anastasievska Gas Compressing Station',33,'','Anastasievska Gas Compressing Station','doc','Анастасіївська Газо Компресорна Станція','','Анастасіївська Газо Компресорна Станція','Анастасиевская Газо Компрессорная Станция','','Анастасиевская Газо Компрессорная Станция');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_5','pTitle','Diaphragm PP1',33,'','Diaphragm PP1','doc','Діафрагма PP1','','Діафрагма PP1','Диафрагма PP1','','Диафрагма PP1');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','DESCR','',34,'Parameter|DESCR','prm:/BlockCalc/gen/BC1/DESCR','BC1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','close','',38,'Parameter|close','prm:/BlockCalc/gen/BC1','BC1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','stop','0',38,'Parameter|stop','prm:/BlockCalc/gen/BC1','BC1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','DESCR','',34,'Parameter|DESCR','prm:/BlockCalc/gen/BC2/DESCR','BC2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','close','',38,'Parameter|close','prm:/BlockCalc/gen/BC2','BC2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','digComs','',34,'Parameter|digComs','prm:/BlockCalc/gen/BC2','BC2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','digStts','',34,'Parameter|digStts','prm:/BlockCalc/gen/BC2','BC2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','stop','0',38,'Parameter|stop','prm:/BlockCalc/gen/BC2','BC2','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','DESCR','',34,'Parameter|DESCR','prm:/BlockCalc/gen/BC21/DESCR','BC21','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','close','',38,'Parameter|close','prm:/BlockCalc/gen/BC21','BC21','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','digComs','',34,'Parameter|digComs','prm:/BlockCalc/gen/BC21','BC21','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','digStts','',34,'Parameter|digStts','prm:/BlockCalc/gen/BC21','BC21','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','stop','0',38,'Parameter|stop','prm:/BlockCalc/gen/BC21','BC21','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','DESCR','',34,'Parameter|DESCR','prm:/BlockCalc/gen/BC22/DESCR','BC22','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','close','',38,'Parameter|close','prm:/BlockCalc/gen/BC22','BC22','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','digComs','',34,'Parameter|digComs','prm:/BlockCalc/gen/BC22','BC22','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','digStts','',34,'Parameter|digStts','prm:/BlockCalc/gen/BC22','BC22','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','stop','0',38,'Parameter|stop','prm:/BlockCalc/gen/BC22','BC22','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','DESCR','',34,'Parameter|DESCR','prm:/BlockCalc/gen/BC5/DESCR','BC5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','close','',38,'Parameter|close','prm:/BlockCalc/gen/BC5','BC5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','digComs','',34,'Parameter|digComs','prm:/BlockCalc/gen/BC5','BC5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','digStts','',34,'Parameter|digStts','prm:/BlockCalc/gen/BC5','BC5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','stop','0',38,'Parameter|stop','prm:/BlockCalc/gen/BC5','BC5','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','DESCR','',34,'Parameter|DESCR','prm:/BlockCalc/gen/BC6/DESCR','BC6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','close','',38,'Parameter|close','prm:/BlockCalc/gen/BC6','BC6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','digComs','',34,'Parameter|digComs','prm:/BlockCalc/gen/BC6','BC6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','digStts','',34,'Parameter|digStts','prm:/BlockCalc/gen/BC6','BC6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','stop','0',38,'Parameter|stop','prm:/BlockCalc/gen/BC6','BC6','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','DESCR','',34,'Parameter|DESCR','prm:/BlockCalc/gen/BC7/DESCR','BC7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','close','',38,'Parameter|close','prm:/BlockCalc/gen/BC7','BC7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','digComs','',34,'Parameter|digComs','prm:/BlockCalc/gen/BC7','BC7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','digStts','',34,'Parameter|digStts','prm:/BlockCalc/gen/BC7','BC7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_mn/pg_1','stop','0',38,'Parameter|stop','prm:/BlockCalc/gen/BC7','BC7','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','DESCR','',34,'Parameter|DESCR','prm:/BlockCalc/CM101/BC101/DESCR','KS101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','close','',38,'Parameter|close','prm:/BlockCalc/CM101/BC101','KS101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','digComs','',34,'Parameter|digComs','prm:/BlockCalc/CM101/BC101','KS101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','digStts','',34,'Parameter|digStts','prm:/BlockCalc/CM101/BC101','KS101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','stop','0',38,'Parameter|stop','prm:/BlockCalc/CM101/BC101','KS101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','DESCR','',34,'Parameter|DESCR','prm:/BlockCalc/CM101/BC102/DESCR','KS102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','close','',38,'Parameter|close','prm:/BlockCalc/CM101/BC102','KS102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','digComs','',34,'Parameter|digComs','prm:/BlockCalc/CM101/BC102','KS102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','digStts','',34,'Parameter|digStts','prm:/BlockCalc/CM101/BC102','KS102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','stop','0',38,'Parameter|stop','prm:/BlockCalc/CM101/BC102','KS102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','DESCR','',34,'Parameter|DESCR','prm:/BlockCalc/CM101/BC104/DESCR','KS104','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','close','',38,'Parameter|close','prm:/BlockCalc/CM101/BC104','KS104','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','digComs','',34,'Parameter|digComs','prm:/BlockCalc/CM101/BC104','KS104','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','digStts','',34,'Parameter|digStts','prm:/BlockCalc/CM101/BC104','KS104','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','stop','0',38,'Parameter|stop','prm:/BlockCalc/CM101/BC104','KS104','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','DESCR','',34,'Parameter|DESCR','prm:/BlockCalc/CM101/BC105/DESCR','KS105','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','close','',38,'Parameter|close','prm:/BlockCalc/CM101/BC105','KS105','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','digComs','',34,'Parameter|digComs','prm:/BlockCalc/CM101/BC105','KS105','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','digStts','',34,'Parameter|digStts','prm:/BlockCalc/CM101/BC105','KS105','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','stop','0',38,'Parameter|stop','prm:/BlockCalc/CM101/BC105','KS105','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','DESCR','',34,'Parameter|DESCR','prm:/BlockCalc/CM101/BC106/DESCR','KS106','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','close','',38,'Parameter|close','prm:/BlockCalc/CM101/BC106','KS106','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','digComs','',34,'Parameter|digComs','prm:/BlockCalc/CM101/BC106','KS106','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','digStts','',34,'Parameter|digStts','prm:/BlockCalc/CM101/BC106','KS106','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','stop','0',38,'Parameter|stop','prm:/BlockCalc/CM101/BC106','KS106','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','DESCR','',34,'Parameter|DESCR','prm:/BlockCalc/CM102/BC111/DESCR','KS101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','close','',38,'Parameter|close','prm:/BlockCalc/CM102/BC111','KS101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','digComs','',34,'Parameter|digComs','prm:/BlockCalc/CM102/BC111','KS101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','digStts','',34,'Parameter|digStts','prm:/BlockCalc/CM102/BC111','KS101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','stop','0',38,'Parameter|stop','prm:/BlockCalc/CM102/BC111','KS101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','DESCR','',34,'Parameter|DESCR','prm:/BlockCalc/CM102/BC112/DESCR','KS102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','close','',38,'Parameter|close','prm:/BlockCalc/CM102/BC112','KS102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','digComs','',34,'Parameter|digComs','prm:/BlockCalc/CM102/BC112','KS102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','digStts','',34,'Parameter|digStts','prm:/BlockCalc/CM102/BC112','KS102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','stop','0',38,'Parameter|stop','prm:/BlockCalc/CM102/BC112','KS102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','DESCR','',34,'Parameter|DESCR','prm:/BlockCalc/CM102/BC114/DESCR','KS104','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','close','',38,'Parameter|close','prm:/BlockCalc/CM102/BC114','KS104','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','digComs','',34,'Parameter|digComs','prm:/BlockCalc/CM102/BC114','KS104','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','digStts','',34,'Parameter|digStts','prm:/BlockCalc/CM102/BC114','KS104','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','stop','0',38,'Parameter|stop','prm:/BlockCalc/CM102/BC114','KS104','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','DESCR','',34,'Parameter|DESCR','prm:/BlockCalc/CM102/BC115/DESCR','KS105','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','close','',38,'Parameter|close','prm:/BlockCalc/CM102/BC115','KS105','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','digComs','',34,'Parameter|digComs','prm:/BlockCalc/CM102/BC115','KS105','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','digStts','',34,'Parameter|digStts','prm:/BlockCalc/CM102/BC115','KS105','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','stop','0',38,'Parameter|stop','prm:/BlockCalc/CM102/BC115','KS105','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','DESCR','',34,'Parameter|DESCR','prm:/BlockCalc/CM102/BC116/DESCR','KS106','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','close','',38,'Parameter|close','prm:/BlockCalc/CM102/BC116','KS106','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','digComs','',34,'Parameter|digComs','prm:/BlockCalc/CM102/BC116','KS106','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','digStts','',34,'Parameter|digStts','prm:/BlockCalc/CM102/BC116','KS106','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','stop','0',38,'Parameter|stop','prm:/BlockCalc/CM102/BC116','KS106','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','DESCR','',34,'Parameter|DESCR','prm:/BlockCalc/CM201/BC101/DESCR','KS101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','close','',38,'Parameter|close','prm:/BlockCalc/CM201/BC101','KS101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','digComs','',34,'Parameter|digComs','prm:/BlockCalc/CM201/BC101','KS101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','digStts','',34,'Parameter|digStts','prm:/BlockCalc/CM201/BC101','KS101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','stop','0',38,'Parameter|stop','prm:/BlockCalc/CM201/BC101','KS101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','DESCR','',34,'Parameter|DESCR','prm:/BlockCalc/CM201/BC102/DESCR','KS102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','close','',38,'Parameter|close','prm:/BlockCalc/CM201/BC102','KS102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','digComs','',34,'Parameter|digComs','prm:/BlockCalc/CM201/BC102','KS102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','digStts','',34,'Parameter|digStts','prm:/BlockCalc/CM201/BC102','KS102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','stop','0',38,'Parameter|stop','prm:/BlockCalc/CM201/BC102','KS102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','DESCR','',34,'Parameter|DESCR','prm:/BlockCalc/CM201/BC104/DESCR','KS104','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','close','',38,'Parameter|close','prm:/BlockCalc/CM201/BC104','KS104','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','digComs','',34,'Parameter|digComs','prm:/BlockCalc/CM201/BC104','KS104','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','digStts','',34,'Parameter|digStts','prm:/BlockCalc/CM201/BC104','KS104','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','stop','0',38,'Parameter|stop','prm:/BlockCalc/CM201/BC104','KS104','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','DESCR','',34,'Parameter|DESCR','prm:/BlockCalc/CM201/BC105/DESCR','KS105','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','close','',38,'Parameter|close','prm:/BlockCalc/CM201/BC105','KS105','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','digComs','',34,'Parameter|digComs','prm:/BlockCalc/CM201/BC105','KS105','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','digStts','',34,'Parameter|digStts','prm:/BlockCalc/CM201/BC105','KS105','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','stop','0',38,'Parameter|stop','prm:/BlockCalc/CM201/BC105','KS105','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','DESCR','',34,'Parameter|DESCR','prm:/BlockCalc/CM201/BC106/DESCR','KS106','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','close','',38,'Parameter|close','prm:/BlockCalc/CM201/BC106','KS106','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','digComs','',34,'Parameter|digComs','prm:/BlockCalc/CM201/BC106','KS106','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','digStts','',34,'Parameter|digStts','prm:/BlockCalc/CM201/BC106','KS106','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','stop','0',38,'Parameter|stop','prm:/BlockCalc/CM201/BC106','KS106','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','DESCR','',34,'Parameter|DESCR','prm:/BlockCalc/CM202/BC111/DESCR','KS101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','close','',38,'Parameter|close','prm:/BlockCalc/CM202/BC111','KS101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','digComs','',34,'Parameter|digComs','prm:/BlockCalc/CM202/BC111','KS101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','digStts','',34,'Parameter|digStts','prm:/BlockCalc/CM202/BC111','KS101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','stop','0',38,'Parameter|stop','prm:/BlockCalc/CM202/BC111','KS101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','DESCR','',34,'Parameter|DESCR','prm:/BlockCalc/CM202/BC112/DESCR','KS102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','close','',38,'Parameter|close','prm:/BlockCalc/CM202/BC112','KS102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','digComs','',34,'Parameter|digComs','prm:/BlockCalc/CM202/BC112','KS102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','digStts','',34,'Parameter|digStts','prm:/BlockCalc/CM202/BC112','KS102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','stop','0',38,'Parameter|stop','prm:/BlockCalc/CM202/BC112','KS102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','DESCR','',34,'Parameter|DESCR','prm:/BlockCalc/CM202/BC114/DESCR','KS104','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','close','',38,'Parameter|close','prm:/BlockCalc/CM202/BC114','KS104','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','digComs','',34,'Parameter|digComs','prm:/BlockCalc/CM202/BC114','KS104','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','digStts','',34,'Parameter|digStts','prm:/BlockCalc/CM202/BC114','KS104','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','stop','0',38,'Parameter|stop','prm:/BlockCalc/CM202/BC114','KS104','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','DESCR','',34,'Parameter|DESCR','prm:/BlockCalc/CM202/BC115/DESCR','KS105','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','close','',38,'Parameter|close','prm:/BlockCalc/CM202/BC115','KS105','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','digComs','',34,'Parameter|digComs','prm:/BlockCalc/CM202/BC115','KS105','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','digStts','',34,'Parameter|digStts','prm:/BlockCalc/CM202/BC115','KS105','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','stop','0',38,'Parameter|stop','prm:/BlockCalc/CM202/BC115','KS105','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','DESCR','',34,'Parameter|DESCR','prm:/BlockCalc/CM202/BC116/DESCR','KS106','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','close','',38,'Parameter|close','prm:/BlockCalc/CM202/BC116','KS106','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','digComs','',34,'Parameter|digComs','prm:/BlockCalc/CM202/BC116','KS106','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','digStts','',34,'Parameter|digStts','prm:/BlockCalc/CM202/BC116','KS106','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','stop','0',38,'Parameter|stop','prm:/BlockCalc/CM202/BC116','KS106','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','DESCR','',34,'Parameter|DESCR','prm:/BlockCalc/CM301/BC101/DESCR','KS101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','close','',38,'Parameter|close','prm:/BlockCalc/CM301/BC101','KS101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','digComs','',34,'Parameter|digComs','prm:/BlockCalc/CM301/BC101','KS101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','digStts','',34,'Parameter|digStts','prm:/BlockCalc/CM301/BC101','KS101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','stop','0',38,'Parameter|stop','prm:/BlockCalc/CM301/BC101','KS101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','DESCR','',34,'Parameter|DESCR','prm:/BlockCalc/CM301/BC102/DESCR','KS102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','close','',38,'Parameter|close','prm:/BlockCalc/CM301/BC102','KS102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','digComs','',34,'Parameter|digComs','prm:/BlockCalc/CM301/BC102','KS102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','digStts','',34,'Parameter|digStts','prm:/BlockCalc/CM301/BC102','KS102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','stop','0',38,'Parameter|stop','prm:/BlockCalc/CM301/BC102','KS102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','DESCR','',34,'Parameter|DESCR','prm:/BlockCalc/CM301/BC104/DESCR','KS104','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','close','',38,'Parameter|close','prm:/BlockCalc/CM301/BC104','KS104','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','digComs','',34,'Parameter|digComs','prm:/BlockCalc/CM301/BC104','KS104','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','digStts','',34,'Parameter|digStts','prm:/BlockCalc/CM301/BC104','KS104','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','stop','0',38,'Parameter|stop','prm:/BlockCalc/CM301/BC104','KS104','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','DESCR','',34,'Parameter|DESCR','prm:/BlockCalc/CM301/BC105/DESCR','KS105','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','close','',38,'Parameter|close','prm:/BlockCalc/CM301/BC105','KS105','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','digComs','',34,'Parameter|digComs','prm:/BlockCalc/CM301/BC105','KS105','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','digStts','',34,'Parameter|digStts','prm:/BlockCalc/CM301/BC105','KS105','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','stop','0',38,'Parameter|stop','prm:/BlockCalc/CM301/BC105','KS105','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','DESCR','',34,'Parameter|DESCR','prm:/BlockCalc/CM301/BC106/DESCR','KS106','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','close','',38,'Parameter|close','prm:/BlockCalc/CM301/BC106','KS106','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','digComs','',34,'Parameter|digComs','prm:/BlockCalc/CM301/BC106','KS106','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','digStts','',34,'Parameter|digStts','prm:/BlockCalc/CM301/BC106','KS106','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','stop','0',38,'Parameter|stop','prm:/BlockCalc/CM301/BC106','KS106','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','DESCR','',34,'Parameter|DESCR','prm:/BlockCalc/CM302/BC111/DESCR','KS101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','close','',38,'Parameter|close','prm:/BlockCalc/CM302/BC111','KS101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','digComs','',34,'Parameter|digComs','prm:/BlockCalc/CM302/BC111','KS101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','digStts','',34,'Parameter|digStts','prm:/BlockCalc/CM302/BC111','KS101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','stop','0',38,'Parameter|stop','prm:/BlockCalc/CM302/BC111','KS101','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','DESCR','',34,'Parameter|DESCR','prm:/BlockCalc/CM302/BC112/DESCR','KS102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','close','',38,'Parameter|close','prm:/BlockCalc/CM302/BC112','KS102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','digComs','',34,'Parameter|digComs','prm:/BlockCalc/CM302/BC112','KS102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','digStts','',34,'Parameter|digStts','prm:/BlockCalc/CM302/BC112','KS102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','stop','0',38,'Parameter|stop','prm:/BlockCalc/CM302/BC112','KS102','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','DESCR','',34,'Parameter|DESCR','prm:/BlockCalc/CM302/BC114/DESCR','KS104','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','close','',38,'Parameter|close','prm:/BlockCalc/CM302/BC114','KS104','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','digComs','',34,'Parameter|digComs','prm:/BlockCalc/CM302/BC114','KS104','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','digStts','',34,'Parameter|digStts','prm:/BlockCalc/CM302/BC114','KS104','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','stop','0',38,'Parameter|stop','prm:/BlockCalc/CM302/BC114','KS104','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','DESCR','',34,'Parameter|DESCR','prm:/BlockCalc/CM302/BC115/DESCR','KS105','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','close','',38,'Parameter|close','prm:/BlockCalc/CM302/BC115','KS105','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','digComs','',34,'Parameter|digComs','prm:/BlockCalc/CM302/BC115','KS105','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','digStts','',34,'Parameter|digStts','prm:/BlockCalc/CM302/BC115','KS105','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','stop','0',38,'Parameter|stop','prm:/BlockCalc/CM302/BC115','KS105','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','DESCR','',34,'Parameter|DESCR','prm:/BlockCalc/CM302/BC116/DESCR','KS106','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','close','',38,'Parameter|close','prm:/BlockCalc/CM302/BC116','KS106','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','digComs','',34,'Parameter|digComs','prm:/BlockCalc/CM302/BC116','KS106','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','digStts','',34,'Parameter|digStts','prm:/BlockCalc/CM302/BC116','KS106','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','stop','0',38,'Parameter|stop','prm:/BlockCalc/CM302/BC116','KS106','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so','geomY','59',32,'','','pgCont','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_ggraph/pg_1','sclWin','0',160,'','','trnd1','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','name','CM101',32,'','','','КМ101','','','КМ101','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_2/pg_mn/pg_CM101','text','CM101',32,'','','CM101','КМ101','','','КМ101','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_3/pg_mn/pg_CM101','text','CM102',32,'','','CM101','КМ102','','','КМ102','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_4/pg_mn/pg_1','text','КМ201',32,'','','CM101','КМ201','','','КМ201','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_5/pg_mn/pg_CM101','text','CM202',32,'','','CM101','КМ202','','','КМ202','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_6/pg_mn/pg_CM101','text','CM301',32,'','','CM101','КМ301','','','КМ301','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_7/pg_mn/pg_CM101','text','CM302',32,'','','CM101','КМ302','','','КМ302','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_addr',NULL,34,'Parameter 1|var','prm:/BlockCalc/CM201/TE1313_1/var','e10','','','','','Parameter 1|var','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_max',NULL,34,'Parameter 1|max','prm:/BlockCalc/CM201/TE1313_1/max','e10','','','','','Parameter 1|max','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_min',NULL,34,'Parameter 1|min','prm:/BlockCalc/CM201/TE1313_1/min','e10','','','','','Parameter 1|min','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_name',NULL,34,'Parameter 1|NAME','prm:/BlockCalc/CM201/TE1313_1/NAME','e10','','','',NULL,'Parameter 1|NAME','prm:/BlockCalc/CM201/TE1313_1/NAME');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_addr',NULL,34,'Parameter 2|var','prm:/BlockCalc/CM201/TE1314_1/var','e10','','','','','Parameter 2|var','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_max',NULL,34,'Parameter 2|max','prm:/BlockCalc/CM201/TE1314_1/max','e10','','','','','Parameter 2|max','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_min',NULL,34,'Parameter 2|min','prm:/BlockCalc/CM201/TE1314_1/min','e10','','','','','Parameter 2|min','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_name',NULL,34,'Parameter 2|NAME','prm:/BlockCalc/CM201/TE1314_1/NAME','e10','','','',NULL,'Parameter 2|NAME','prm:/BlockCalc/CM201/TE1314_1/NAME');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_addr',NULL,34,'Parameter 3|var','prm:/BlockCalc/CM201/TE1314_2/var','e10','','','','','Parameter 3|var','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_max',NULL,34,'Parameter 3|max','prm:/BlockCalc/CM201/TE1314_2/max','e10','','','','','Parameter 3|max','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_min',NULL,34,'Parameter 3|min','prm:/BlockCalc/CM201/TE1314_2/min','e10','','','','','Parameter 3|min','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_name',NULL,34,'Parameter 3|NAME','prm:/BlockCalc/CM201/TE1314_2/NAME','e10','','','',NULL,'Parameter 3|NAME','prm:/BlockCalc/CM201/TE1314_2/NAME');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','title','CM201 (T)',33,'','CM201 (T)','e10','КМ201 (T)','','КМ201 (T)','КМ201 (T)','','КМ201 (T)');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_addr',NULL,34,'Parameter 1|var','prm:/BlockCalc/CM202/ST8612/var','e11','','','','','Parameter 1|var','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_max',NULL,34,'Parameter 1|max','prm:/BlockCalc/CM202/ST8612/max','e11','','','','','Parameter 1|max','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_min',NULL,34,'Parameter 1|min','prm:/BlockCalc/CM202/ST8612/min','e11','','','','','Parameter 1|min','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_name',NULL,34,'Parameter 1|NAME','prm:/BlockCalc/CM202/ST8612/NAME','e11','','','',NULL,'Parameter 1|NAME','prm:/BlockCalc/CM202/ST8612/NAME');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_addr',NULL,34,'Parameter 2|var','prm:/BlockCalc/CM202/F103/var','e11','','','','','Parameter 2|var','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_max',NULL,34,'Parameter 2|max','prm:/BlockCalc/CM202/F103/max','e11','','','','','Parameter 2|max','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_min',NULL,34,'Parameter 2|min','prm:/BlockCalc/CM202/F103/min','e11','','','','','Parameter 2|min','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_name',NULL,34,'Parameter 2|NAME','prm:/BlockCalc/CM202/F103/NAME','e11','','','',NULL,'Parameter 2|NAME','prm:/BlockCalc/CM202/F103/NAME');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_addr',NULL,34,'Parameter 3|var','prm:/BlockCalc/CM202/P103/var','e11','','','','','Parameter 3|var','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_max',NULL,34,'Parameter 3|max','prm:/BlockCalc/CM202/P103/max','e11','','','','','Parameter 3|max','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_min',NULL,34,'Parameter 3|min','prm:/BlockCalc/CM202/P103/min','e11','','','','','Parameter 3|min','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_name',NULL,34,'Parameter 3|NAME','prm:/BlockCalc/CM202/P103/NAME','e11','','','',NULL,'Parameter 3|NAME','prm:/BlockCalc/CM202/P103/NAME');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p4_addr',NULL,34,'Parameter 4|var','prm:/BlockCalc/CM202/PT1006_1/var','e11','','','','','Parameter 4|var','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p4_max',NULL,34,'Parameter 4|max','prm:/BlockCalc/CM202/PT1006_1/max','e11','','','','','Parameter 4|max','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p4_min',NULL,34,'Parameter 4|min','prm:/BlockCalc/CM202/PT1006_1/min','e11','','','','','Parameter 4|min','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p4_name',NULL,34,'Parameter 4|NAME','prm:/BlockCalc/CM202/PT1006_1/NAME','e11','','','',NULL,'Parameter 4|NAME','prm:/BlockCalc/CM202/PT1006_1/NAME');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p5_addr',NULL,34,'Parameter 5|var','prm:/BlockCalc/CM202/PT0204/var','e11','','','','','Parameter 5|var','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p5_max',NULL,34,'Parameter 5|max','prm:/BlockCalc/CM202/PT0204/max','e11','','','','','Parameter 5|max','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p5_min',NULL,34,'Parameter 5|min','prm:/BlockCalc/CM202/PT0204/min','e11','','','','','Parameter 5|min','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p5_name',NULL,34,'Parameter 5|NAME','prm:/BlockCalc/CM202/PT0204/NAME','e11','','','',NULL,'Parameter 5|NAME','prm:/BlockCalc/CM202/PT0204/NAME');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','title','CM202 (N,F,P)',33,'','CM202 (N,F,P)','e11','КМ202 (N,F,P)','','КМ202 (N,F,P)','КМ202 (N,F,P)','','КМ202 (N,F,P)');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_addr',NULL,34,'Parameter 1|var','prm:/BlockCalc/CM202/TE1313_1/var','e12','','','','','Parameter 1|var','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_max',NULL,34,'Parameter 1|max','prm:/BlockCalc/CM202/TE1313_1/max','e12','','','','','Parameter 1|max','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_min',NULL,34,'Parameter 1|min','prm:/BlockCalc/CM202/TE1313_1/min','e12','','','','','Parameter 1|min','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_name',NULL,34,'Parameter 1|NAME','prm:/BlockCalc/CM202/TE1313_1/NAME','e12','','','',NULL,'Parameter 1|NAME','prm:/BlockCalc/CM202/TE1313_1/NAME');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_addr',NULL,34,'Parameter 2|var','prm:/BlockCalc/CM202/TE1314_1/var','e12','','','','','Parameter 2|var','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_max',NULL,34,'Parameter 2|max','prm:/BlockCalc/CM202/TE1314_1/max','e12','','','','','Parameter 2|max','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_min',NULL,34,'Parameter 2|min','prm:/BlockCalc/CM202/TE1314_1/min','e12','','','','','Parameter 2|min','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_name',NULL,34,'Parameter 2|NAME','prm:/BlockCalc/CM202/TE1314_1/NAME','e12','','','',NULL,'Parameter 2|NAME','prm:/BlockCalc/CM202/TE1314_1/NAME');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_addr',NULL,34,'Parameter 3|var','prm:/BlockCalc/CM202/TE1314_2/var','e12','','','','','Parameter 3|var','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_max',NULL,34,'Parameter 3|max','prm:/BlockCalc/CM202/TE1314_2/max','e12','','','','','Parameter 3|max','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_min',NULL,34,'Parameter 3|min','prm:/BlockCalc/CM202/TE1314_2/min','e12','','','','','Parameter 3|min','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_name',NULL,34,'Parameter 3|NAME','prm:/BlockCalc/CM202/TE1314_2/NAME','e12','','','',NULL,'Parameter 3|NAME','prm:/BlockCalc/CM202/TE1314_2/NAME');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','title','CM202 (T)',33,'','CM202 (T)','e12','КМ202 (T)','','КМ202 (T)','КМ202 (T)','','КМ202 (T)');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_addr',NULL,34,'Parameter 1|var','prm:/BlockCalc/CM301/ST8612/var','e13','','','','','Parameter 1|var','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_max',NULL,34,'Parameter 1|max','prm:/BlockCalc/CM301/ST8612/max','e13','','','','','Parameter 1|max','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_min',NULL,34,'Parameter 1|min','prm:/BlockCalc/CM301/ST8612/min','e13','','','','','Parameter 1|min','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_name',NULL,34,'Parameter 1|NAME','prm:/BlockCalc/CM301/ST8612/NAME','e13','','','',NULL,'Parameter 1|NAME','prm:/BlockCalc/CM301/ST8612/NAME');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_addr',NULL,34,'Parameter 2|var','prm:/BlockCalc/CM301/F101/var','e13','','','','','Parameter 2|var','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_max',NULL,34,'Parameter 2|max','prm:/BlockCalc/CM301/F101/max','e13','','','','','Parameter 2|max','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_min',NULL,34,'Parameter 2|min','prm:/BlockCalc/CM301/F101/min','e13','','','','','Parameter 2|min','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_name',NULL,34,'Parameter 2|NAME','prm:/BlockCalc/CM301/F101/NAME','e13','','','',NULL,'Parameter 2|NAME','prm:/BlockCalc/CM301/F101/NAME');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_addr',NULL,34,'Parameter 3|var','prm:/BlockCalc/CM301/P101/var','e13','','','','','Parameter 3|var','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_max',NULL,34,'Parameter 3|max','prm:/BlockCalc/CM301/P101/max','e13','','','','','Parameter 3|max','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_min',NULL,34,'Parameter 3|min','prm:/BlockCalc/CM301/P101/min','e13','','','','','Parameter 3|min','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_name',NULL,34,'Parameter 3|NAME','prm:/BlockCalc/CM301/P101/NAME','e13','','','',NULL,'Parameter 3|NAME','prm:/BlockCalc/CM301/P101/NAME');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p4_addr',NULL,34,'Parameter 4|var','prm:/BlockCalc/CM301/PT1006_1/var','e13','','','','','Parameter 4|var','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p4_max',NULL,34,'Parameter 4|max','prm:/BlockCalc/CM301/PT1006_1/max','e13','','','','','Parameter 4|max','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p4_min',NULL,34,'Parameter 4|min','prm:/BlockCalc/CM301/PT1006_1/min','e13','','','','','Parameter 4|min','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p4_name',NULL,34,'Parameter 4|NAME','prm:/BlockCalc/CM301/PT1006_1/NAME','e13','','','',NULL,'Parameter 4|NAME','prm:/BlockCalc/CM301/PT1006_1/NAME');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p5_addr',NULL,34,'Parameter 5|var','prm:/BlockCalc/CM301/PT0204/var','e13','','','','','Parameter 5|var','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p5_max',NULL,34,'Parameter 5|max','prm:/BlockCalc/CM301/PT0204/max','e13','','','','','Parameter 5|max','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p5_min',NULL,34,'Parameter 5|min','prm:/BlockCalc/CM301/PT0204/min','e13','','','','','Parameter 5|min','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p5_name',NULL,34,'Parameter 5|NAME','prm:/BlockCalc/CM301/PT0204/NAME','e13','','','',NULL,'Parameter 5|NAME','prm:/BlockCalc/CM301/PT0204/NAME');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','title','CM301 (N,F,P)',33,'','CM301 (N,F,P)','e13','КМ301 (N,F,P)','','КМ301 (N,F,P)','КМ301 (N,F,P)','','КМ301 (N,F,P)');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_addr',NULL,34,'Parameter 1|var','prm:/BlockCalc/CM301/TE1313_1/var','e14','','','','','Parameter 1|var','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_max',NULL,34,'Parameter 1|max','prm:/BlockCalc/CM301/TE1313_1/max','e14','','','','','Parameter 1|max','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_min',NULL,34,'Parameter 1|min','prm:/BlockCalc/CM301/TE1313_1/min','e14','','','','','Parameter 1|min','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_name',NULL,34,'Parameter 1|NAME','prm:/BlockCalc/CM301/TE1313_1/NAME','e14','','','',NULL,'Parameter 1|NAME','prm:/BlockCalc/CM301/TE1313_1/NAME');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_addr',NULL,34,'Parameter 2|var','prm:/BlockCalc/CM301/TE1314_1/var','e14','','','','','Parameter 2|var','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_max',NULL,34,'Parameter 2|max','prm:/BlockCalc/CM301/TE1314_1/max','e14','','','','','Parameter 2|max','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_min',NULL,34,'Parameter 2|min','prm:/BlockCalc/CM301/TE1314_1/min','e14','','','','','Parameter 2|min','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_name',NULL,34,'Parameter 2|NAME','prm:/BlockCalc/CM301/TE1314_1/NAME','e14','','','',NULL,'Parameter 2|NAME','prm:/BlockCalc/CM301/TE1314_1/NAME');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_addr',NULL,34,'Parameter 3|var','prm:/BlockCalc/CM301/TE1314_2/var','e14','','','','','Parameter 3|var','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_max',NULL,34,'Parameter 3|max','prm:/BlockCalc/CM301/TE1314_2/max','e14','','','','','Parameter 3|max','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_min',NULL,34,'Parameter 3|min','prm:/BlockCalc/CM301/TE1314_2/min','e14','','','','','Parameter 3|min','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_name',NULL,34,'Parameter 3|NAME','prm:/BlockCalc/CM301/TE1314_2/NAME','e14','','','',NULL,'Parameter 3|NAME','prm:/BlockCalc/CM301/TE1314_2/NAME');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','title','CM301 (T)',33,'','CM301 (T)','e14','КМ301 (T)','','КМ301 (T)','КМ301 (T)','','КМ301 (T)');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_addr',NULL,34,'Parameter 1|var','prm:/BlockCalc/CM302/ST8612/var','e15','','','','','Parameter 1|var','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_max',NULL,34,'Parameter 1|max','prm:/BlockCalc/CM302/ST8612/max','e15','','','','','Parameter 1|max','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_min',NULL,34,'Parameter 1|min','prm:/BlockCalc/CM302/ST8612/min','e15','','','','','Parameter 1|min','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_name',NULL,34,'Parameter 1|NAME','prm:/BlockCalc/CM302/ST8612/NAME','e15','','','',NULL,'Parameter 1|NAME','prm:/BlockCalc/CM302/ST8612/NAME');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_addr',NULL,34,'Parameter 2|var','prm:/BlockCalc/CM302/F103/var','e15','','','','','Parameter 2|var','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_max',NULL,34,'Parameter 2|max','prm:/BlockCalc/CM302/F103/max','e15','','','','','Parameter 2|max','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_min',NULL,34,'Parameter 2|min','prm:/BlockCalc/CM302/F103/min','e15','','','','','Parameter 2|min','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_name',NULL,34,'Parameter 2|NAME','prm:/BlockCalc/CM302/F103/NAME','e15','','','',NULL,'Parameter 2|NAME','prm:/BlockCalc/CM302/F103/NAME');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_addr',NULL,34,'Parameter 3|var','prm:/BlockCalc/CM302/P103/var','e15','','','','','Parameter 3|var','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_max',NULL,34,'Parameter 3|max','prm:/BlockCalc/CM302/P103/max','e15','','','','','Parameter 3|max','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_min',NULL,34,'Parameter 3|min','prm:/BlockCalc/CM302/P103/min','e15','','','','','Parameter 3|min','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_name',NULL,34,'Parameter 3|NAME','prm:/BlockCalc/CM302/P103/NAME','e15','','','',NULL,'Parameter 3|NAME','prm:/BlockCalc/CM302/P103/NAME');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p4_addr',NULL,34,'Parameter 4|var','prm:/BlockCalc/CM302/PT1006_1/var','e15','','','','','Parameter 4|var','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p4_max',NULL,34,'Parameter 4|max','prm:/BlockCalc/CM302/PT1006_1/max','e15','','','','','Parameter 4|max','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p4_min',NULL,34,'Parameter 4|min','prm:/BlockCalc/CM302/PT1006_1/min','e15','','','','','Parameter 4|min','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p4_name',NULL,34,'Parameter 4|NAME','prm:/BlockCalc/CM302/PT1006_1/NAME','e15','','','',NULL,'Parameter 4|NAME','prm:/BlockCalc/CM302/PT1006_1/NAME');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p5_addr',NULL,34,'Parameter 5|var','prm:/BlockCalc/CM302/PT0204/var','e15','','','','','Parameter 5|var','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p5_max',NULL,34,'Parameter 5|max','prm:/BlockCalc/CM302/PT0204/max','e15','','','','','Parameter 5|max','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p5_min',NULL,34,'Parameter 5|min','prm:/BlockCalc/CM302/PT0204/min','e15','','','','','Parameter 5|min','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p5_name',NULL,34,'Parameter 5|NAME','prm:/BlockCalc/CM302/PT0204/NAME','e15','','','',NULL,'Parameter 5|NAME','prm:/BlockCalc/CM302/PT0204/NAME');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','title','CM302 (N,F,P)',33,'','CM302 (N,F,P)','e15','КМ302 (N,F,P)','','КМ302 (N,F,P)','КМ302 (N,F,P)','','КМ302 (N,F,P)');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_addr',NULL,34,'Parameter 1|var','prm:/BlockCalc/CM302/TE1313_1/var','e16','','','','','Parameter 1|var','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_max',NULL,34,'Parameter 1|max','prm:/BlockCalc/CM302/TE1313_1/max','e16','','','','','Parameter 1|max','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_min',NULL,34,'Parameter 1|min','prm:/BlockCalc/CM302/TE1313_1/min','e16','','','','','Parameter 1|min','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_name',NULL,34,'Parameter 1|NAME','prm:/BlockCalc/CM302/TE1313_1/NAME','e16','','','',NULL,'Parameter 1|NAME','prm:/BlockCalc/CM302/TE1313_1/NAME');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_addr',NULL,34,'Parameter 2|var','prm:/BlockCalc/CM302/TE1314_1/var','e16','','','','','Parameter 2|var','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_max',NULL,34,'Parameter 2|max','prm:/BlockCalc/CM302/TE1314_1/max','e16','','','','','Parameter 2|max','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_min',NULL,34,'Parameter 2|min','prm:/BlockCalc/CM302/TE1314_1/min','e16','','','','','Parameter 2|min','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_name',NULL,34,'Parameter 2|NAME','prm:/BlockCalc/CM302/TE1314_1/NAME','e16','','','',NULL,'Parameter 2|NAME','prm:/BlockCalc/CM302/TE1314_1/NAME');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_addr',NULL,34,'Parameter 3|var','prm:/BlockCalc/CM302/TE1314_2/var','e16','','','','','Parameter 3|var','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_max',NULL,34,'Parameter 3|max','prm:/BlockCalc/CM302/TE1314_2/max','e16','','','','','Parameter 3|max','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_min',NULL,34,'Parameter 3|min','prm:/BlockCalc/CM302/TE1314_2/min','e16','','','','','Parameter 3|min','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_name',NULL,34,'Parameter 3|NAME','prm:/BlockCalc/CM302/TE1314_2/NAME','e16','','','',NULL,'Parameter 3|NAME','prm:/BlockCalc/CM302/TE1314_2/NAME');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','title','CM302 (T)',33,'','CM302 (T)','e16','КМ302 (T)','','КМ302 (T)','КМ302 (T)','','КМ302 (T)');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_addr',NULL,34,'Parameter 1|var','prm:/BlockCalc/CM201/ST8612/var','e9','','','','','Parameter 1|var','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_max',NULL,34,'Parameter 1|max','prm:/BlockCalc/CM201/ST8612/max','e9','','','','','Parameter 1|max','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_min',NULL,34,'Parameter 1|min','prm:/BlockCalc/CM201/ST8612/min','e9','','','','','Parameter 1|min','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p1_name',NULL,34,'Parameter 1|NAME','prm:/BlockCalc/CM201/ST8612/NAME','e9','','','',NULL,'Parameter 1|NAME','prm:/BlockCalc/CM201/ST8612/NAME');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_addr',NULL,34,'Parameter 2|var','prm:/BlockCalc/CM201/F101/var','e9','','','','','Parameter 2|var','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_max',NULL,34,'Parameter 2|max','prm:/BlockCalc/CM201/F101/max','e9','','','','','Parameter 2|max','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_min',NULL,34,'Parameter 2|min','prm:/BlockCalc/CM201/F101/min','e9','','','','','Parameter 2|min','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p2_name',NULL,34,'Parameter 2|NAME','prm:/BlockCalc/CM201/F101/NAME','e9','','','',NULL,'Parameter 2|NAME','prm:/BlockCalc/CM201/F101/NAME');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_addr',NULL,34,'Parameter 3|var','prm:/BlockCalc/CM201/P101/var','e9','','','','','Parameter 3|var','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_max',NULL,34,'Parameter 3|max','prm:/BlockCalc/CM201/P101/max','e9','','','','','Parameter 3|max','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_min',NULL,34,'Parameter 3|min','prm:/BlockCalc/CM201/P101/min','e9','','','','','Parameter 3|min','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p3_name',NULL,34,'Parameter 3|NAME','prm:/BlockCalc/CM201/P101/NAME','e9','','','',NULL,'Parameter 3|NAME','prm:/BlockCalc/CM201/P101/NAME');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p4_addr',NULL,34,'Parameter 4|var','prm:/BlockCalc/CM201/PT1006_1/var','e9','','','','','Parameter 4|var','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p4_max',NULL,34,'Parameter 4|max','prm:/BlockCalc/CM201/PT1006_1/max','e9','','','','','Parameter 4|max','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p4_min',NULL,34,'Parameter 4|min','prm:/BlockCalc/CM201/PT1006_1/min','e9','','','','','Parameter 4|min','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p4_name',NULL,34,'Parameter 4|NAME','prm:/BlockCalc/CM201/PT1006_1/NAME','e9','','','',NULL,'Parameter 4|NAME','prm:/BlockCalc/CM201/PT1006_1/NAME');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p5_addr',NULL,34,'Parameter 5|var','prm:/BlockCalc/CM201/PT0204/var','e9','','','','','Parameter 5|var','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p5_max',NULL,34,'Parameter 5|max','prm:/BlockCalc/CM201/PT0204/max','e9','','','','','Parameter 5|max','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p5_min',NULL,34,'Parameter 5|min','prm:/BlockCalc/CM201/PT0204/min','e9','','','','','Parameter 5|min','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','p5_name',NULL,34,'Parameter 5|NAME','prm:/BlockCalc/CM201/PT0204/NAME','e9','','','',NULL,'Parameter 5|NAME','prm:/BlockCalc/CM201/PT0204/NAME');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_rg/pg_rg/pg_1','title','CM201 (N,F,P)',33,'','CM201 (N,F,P)','e9','КМ201 (N,F,P)','','КМ201 (N,F,P)','КМ201 (N,F,P)','','КМ201 (N,F,P)');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_control/pg_graphCalc','owner','root:op',32,'','','','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_1','p1var','',34,'Parameter1|var','','doc','','Parameter1|var','','','Parameter1|var','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_1','p1ed','',34,'Parameter1|ed','','doc','','Parameter1|ed','','','Parameter1|ed','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_1','p1name','',34,'Parameter1|NAME','','doc','','Parameter1|NAME','','','Parameter1|NAME','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_1','p2var','',34,'Parameter2|var','prm:/LogicLev/gen/gN1/F','doc','','Parameter2|var','','','Parameter2|var','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_1','p3var','',34,'Parameter3|var','prm:/LogicLev/gen/gN1/Q','doc','','Parameter3|var','','','Parameter3|var','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_1','p4var','',34,'Parameter4|var','prm:/LogicLev/gen/gN1/P','doc','','Parameter4|var','','','Parameter4|var','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_1','p5var','',34,'Parameter5|var','prm:/LogicLev/gen/gN1/T','doc','','Parameter5|var','','','Parameter5|var','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_1','p6var','',34,'Parameter6|var','prm:/LogicLev/gen/gN1/dP','doc','','Parameter6|var','','','Parameter6|var','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_1','p2ed','',34,'Parameter2|ed','val:m3/h','doc','','Parameter2|ed','val:м3/год','','Parameter2|ed','val:м3/ч');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_1','p3ed','',34,'Parameter3|ed','val:m3','doc','','Parameter3|ed','val:м3','','Parameter3|ed','val:м3');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_1','p4ed','',34,'Parameter4|ed','val:kgF/cm2','doc','','Parameter4|ed','val:кГс/см2','','Parameter4|ed','val:кГс/см2');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_1','p5ed','',34,'Parameter5|ed','val:°С','doc','','Parameter5|ed','val:°С','','Parameter5|ed','val:°С');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_1','p6ed','',34,'Parameter6|ed','val:kgF/cm2','doc','','Parameter6|ed','val:кГс/см2','','Parameter6|ed','val:кГс/см2');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_1','p2name','',34,'Parameter2|NAME','val:Flow','doc','','Parameter2|NAME','val:Витрати','','Parameter2|NAME','val:Расход');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_1','p3name','',34,'Parameter3|NAME','val:Capacity','doc','','Parameter3|NAME','val:Об''єм','','Parameter3|NAME','val:Объём');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_1','p4name','',34,'Parameter4|NAME','val:Pressure','doc','','Parameter4|NAME','val:Тиск','','Parameter4|NAME','val:Давление');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_1','p5name','',34,'Parameter5|NAME','val:Temperature','doc','','Parameter5|NAME','val:Температура','','Parameter5|NAME','val:Температура');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_1','p6name','',34,'Parameter6|NAME','val:Pressure diff.','doc','','Parameter6|NAME','val:Перепад','','Parameter6|NAME','val:Перепад');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_1','p1prec','0',34,'Parameter1|prec','','doc','','Parameter1|prec','','','Parameter1|prec','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_1','p2prec','0',34,'Parameter2|prec','val:2','doc','','Parameter2|prec','','','Parameter2|prec','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_1','p3prec','0',34,'Parameter3|prec','val:3','doc','','Parameter3|prec','','','Parameter3|prec','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_1','p4prec','0',34,'Parameter4|prec','val:3','doc','','Parameter4|prec','','','Parameter4|prec','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_1','p5prec','0',34,'Parameter5|prec','val:3','doc','','Parameter5|prec','','','Parameter5|prec','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_1','p6prec','0',34,'Parameter6|prec','val:3','doc','','Parameter6|prec','','','Parameter6|prec','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_1','title','Diaphragm PP1',33,'','Diaphragm PP1','doc','Діафрагма PP1','','Діафрагма PP1','Диафрагма PP1','','Диафрагма PP1');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_3','p1cntr','1',33,'Parameter1|isCntr','1','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_3','p1ed','',34,'Parameter1|ed','val:m3','doc','','','val:м3','','','val:м3');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_3','p1name','',34,'Parameter1|NAME','val:Capacity','doc','','','val:Об''єм','','','val:Объём');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_3','p1prec','0',34,'Parameter1|prec','val:3','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_3','p1var','',34,'Parameter1|var','prm:/LogicLev/gen/gN1/Q','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_3','p2ed','',34,'Parameter2|ed','val:kgF/m2','doc','','','val:кГс/м2','','','val:кГс/м2');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_3','p2name','',34,'Parameter2|NAME','val:Aver. pressure diff.','doc','','','val:Сер. переп.','','','val:Ср. переп.');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_3','p2prec','0',34,'Parameter2|prec','val:2','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_3','p2var','',34,'Parameter2|var','prm:/LogicLev/gen/gN1/dP','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_3','p3ed','',34,'Parameter3|ed','val:kgF/cm','doc','','','val:кГс/см','','','val:кГс/см');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_3','p3name','',34,'Parameter3|NAME','val:St. pressure','doc','','','val:Ст. тиск','','','val:Ст. давл.');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_3','p3prec','0',34,'Parameter3|prec','val:2','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_3','p3var','',34,'Parameter3|var','prm:/LogicLev/gen/gN1/P','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_3','p4ed','',34,'Parameter4|ed','val:°С','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_3','p4name','',34,'Parameter4|NAME','val:Aver. temp.','doc','','','val:Сер. темп.','','','val:Ср. темп.');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_3','p4prec','0',34,'Parameter4|prec','val:2','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_3','p4var','',34,'Parameter4|var','prm:/LogicLev/gen/gN1/T','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_3','p5ed','',34,'Parameter5|ed','val:kg/m3','doc','','','val:кг/м3','','','val:кг/м3');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_3','p5name','',34,'Parameter5|NAME','val:Density','doc','','','val:Щільність','','','val:Плотность');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_3','p5prec','0',34,'Parameter5|prec','val:2','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_3','p5var','',34,'Parameter5|var','prm:/LogicLev/gen/gN1/DS','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_4','p1cntr','1',33,'Parameter1|isCntr','1','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_4','p1ed','',34,'Parameter1|ed','val:m3','doc','','','val:м3','','','val:м3');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_4','p1name','',34,'Parameter1|NAME','val:Capacity','doc','','','val:Об''єм','','','val:Объём');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_4','p1prec','0',34,'Parameter1|prec','val:3','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_4','p1var','',34,'Parameter1|var','prm:/LogicLev/gen/gN1/Q','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_4','p2ed','',34,'Parameter2|ed','val:kgF/m2','doc','','','val:кГс/м2','','','val:кГс/м2');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_4','p2name','',34,'Parameter2|NAME','val:Aver. pressure diff.','doc','','','val:Сер. переп.','','','val:Ср. переп.');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_4','p2prec','0',34,'Parameter2|prec','val:2','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_4','p2var','',34,'Parameter2|var','prm:/LogicLev/gen/gN1/dP','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_4','p3ed','',34,'Parameter3|ed','val:kgF/cm','doc','','','val:кГс/см','','','val:кГс/см');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_4','p3name','',34,'Parameter3|NAME','val:St. pressure','doc','','','val:Ст. тиск','','','val:Ст. давл.');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_4','p3prec','0',34,'Parameter3|prec','val:2','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_4','p3var','',34,'Parameter3|var','prm:/LogicLev/gen/gN1/P','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_4','p4ed','',34,'Parameter4|ed','val:°С','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_4','p4name','',34,'Parameter4|NAME','val:Aver. temp.','doc','','','val:Сер. темп.','','','val:Ср. темп.');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_4','p4prec','0',34,'Parameter4|prec','val:2','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_4','p4var','',34,'Parameter4|var','prm:/LogicLev/gen/gN1/T','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_4','p5ed','',34,'Parameter5|ed','val:kg/m3','doc','','','val:кг/м3','','','val:кг/м3');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_4','p5name','',34,'Parameter5|NAME','val:Density','doc','','','val:Щільність','','','val:Плотность');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_4','p5prec','0',34,'Parameter5|prec','val:2','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_4','p5var','',34,'Parameter5|var','prm:/LogicLev/gen/gN1/DS','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_1','p2max','',34,'Parameter2|max','val:200','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_1','p2min','',34,'Parameter2|min','val:0','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_1','p3max','',34,'Parameter3|max','val:0','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_1','p3min','',34,'Parameter3|min','val:0','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_1','p4max','',34,'Parameter4|max','val:10','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_1','p4min','',34,'Parameter4|min','val:0','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_1','p5max','',34,'Parameter5|max','val:50','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_1','p5min','',34,'Parameter5|min','val:-50','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_1','p6max','',34,'Parameter6|max','val:10','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_1','p6min','',34,'Parameter6|min','val:0','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_3','p1max','',34,'Parameter1|max','val:0','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_3','p1min','',34,'Parameter1|min','val:0','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_3','p2max','',34,'Parameter2|max','val:10','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_3','p2min','',34,'Parameter2|min','val:0','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_3','p3max','',34,'Parameter3|max','val:10','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_3','p3min','',34,'Parameter3|min','val:0','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_3','p4max','',34,'Parameter4|max','val:50','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_3','p4min','',34,'Parameter4|min','val:-50','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_3','p5max','',34,'Parameter5|max','val:2','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_3','p5min','',34,'Parameter5|min','val:0','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_4','p1max','',34,'Parameter1|max','val:0','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_4','p1min','',34,'Parameter1|min','val:0','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_4','p2max','',34,'Parameter2|max','val:10','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_4','p2min','',34,'Parameter2|min','val:0','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_4','p3max','',34,'Parameter3|max','val:10','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_4','p3min','',34,'Parameter3|min','val:0','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_4','p4max','',34,'Parameter4|max','val:50','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_4','p4min','',34,'Parameter4|min','val:-50','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_4','p5max','',34,'Parameter5|max','val:2','doc','','','','','','');
INSERT INTO prj_AGLKS_io VALUES('/prj_AGLKS/pg_so/pg_1/pg_doc/pg_4','p5min','',34,'Parameter5|min','val:0','doc','','','','','','');
CREATE TABLE IF NOT EXISTS 'prj_AGLKS_stl' ("ID" TEXT DEFAULT '' ,"V_0" TEXT DEFAULT '' ,"V_1" TEXT DEFAULT '' ,"V_2" TEXT DEFAULT '' ,"V_3" TEXT DEFAULT '' ,"V_4" TEXT DEFAULT '' ,"V_5" TEXT DEFAULT '' ,"V_6" TEXT DEFAULT '' ,"V_7" TEXT DEFAULT '' ,"V_8" TEXT DEFAULT '' ,"V_9" TEXT DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO prj_AGLKS_stl VALUES('<Styles>','Default','Light','','','','','','','','');
INSERT INTO prj_AGLKS_stl VALUES('backColor','gray','ivory','','','','','','','','');
INSERT INTO prj_AGLKS_stl VALUES('backColorFrame','#5A5A5A','#E1E1D4','','','','','','','','');
INSERT INTO prj_AGLKS_stl VALUES('backColorButton','#555555','#D8D8CB','','','','','','','','');
INSERT INTO prj_AGLKS_stl VALUES('backColorButtonSel','#777799','#AEAEE0','','','','','','','','');
INSERT INTO prj_AGLKS_stl VALUES('labColorAlarm','#ff0000','#FF0000','','','','','','','','');
INSERT INTO prj_AGLKS_stl VALUES('labColorGood','#00FF00','#008600','','','','','','','','');
INSERT INTO prj_AGLKS_stl VALUES('labColorWarning','#ffff00','#B4B400','','','','','','','','');
INSERT INTO prj_AGLKS_stl VALUES('labColor','white','black','','','','','','','','');
INSERT INTO prj_AGLKS_stl VALUES('backColorVal','black','white','','','','','','','','');
INSERT INTO prj_AGLKS_stl VALUES('labColorGrph1','#b000b0','#ae77c3-200','','','','','','','','');
INSERT INTO prj_AGLKS_stl VALUES('labColorGrph2','orange-200','orange-200','','','','','','','','');
INSERT INTO prj_AGLKS_stl VALUES('labColorGrph3','blue-200','blue-200','','','','','','','','');
INSERT INTO prj_AGLKS_stl VALUES('labColorGrph4','lightgreen-200','#199696-200','','','','','','','','');
INSERT INTO prj_AGLKS_stl VALUES('labColorGrph5','cyan-200','#3264C8-200','','','','','','','','');
INSERT INTO prj_AGLKS_stl VALUES('labColorGrph6','tomato-200','tomato-200','','','','','','','','');
INSERT INTO prj_AGLKS_stl VALUES('labColorGrph7','magenta-200','magenta-200','','','','','','','','');
INSERT INTO prj_AGLKS_stl VALUES('labColorGrph8','yellow-200','#B6B600-200','','','','','','','','');
INSERT INTO prj_AGLKS_stl VALUES('labColorGrph10','darkcyan-200','darkcyan-200','','','','','','','','');
INSERT INTO prj_AGLKS_stl VALUES('labColorGrph9','lightyellow-200','#69695C-200','','','','','','','','');
CREATE TABLE IF NOT EXISTS 'wlb_AGLKS_incl' ("IDW" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"PARENT" TEXT DEFAULT '' ,"ATTRS" TEXT DEFAULT '' ,"DBV" INTEGER DEFAULT '' , PRIMARY KEY ("IDW","ID"));
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_Pipe_simple_gr1','/wlb_mnEls/wdg_El_Pipe_simple_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_StrelaVB_gr1','/wlb_mnEls/wdg_El_StrelaVB_gr','geomX;geomY;geomXsc;geomYsc;geomZ;fillColor;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','El_Pipe_simple_vert_gr4','/wlb_mnEls/wdg_El_Pipe_simple_vert_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','El_Pipe_simple_vert_gr6','/wlb_mnEls/wdg_El_Pipe_simple_vert_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','El_Strela_gr6','/wlb_mnEls/wdg_El_Strela_gr','geomX;geomY;geomXsc;geomYsc;geomZ;fillColor;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','El_Pipe_simple_vert_gr5','/wlb_mnEls/wdg_El_Pipe_simple_vert_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','dr1','/wlb_mnEls/wdg_El_Armatura','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','El_angleTL_gr2','/wlb_mnEls/wdg_El_angleTL_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','El_TroinikHB_gr2','/wlb_mnEls/wdg_El_TroinikHB_gr','geomX;geomY;geomXsc;geomYsc;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','El_Pipe_simple_vert_gr1','/wlb_mnEls/wdg_El_Pipe_simple_vert_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','F_PP101','/wlb_Main/wdg_anShow1','name;geomX;geomY;geomW;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','El_Pipe_simple_vert_gr8','/wlb_mnEls/wdg_El_Pipe_simple_vert_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','F_PP102','/wlb_Main/wdg_anShow1','name;geomX;geomY;geomW;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','El_TroinikHB_gr3','/wlb_mnEls/wdg_El_TroinikHB_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','El_Strela_gr3','/wlb_mnEls/wdg_El_Strela_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','txtCM101_1','/wlb_originals/wdg_Text','geomX;geomY;geomW;geomH;geomZ;font;alignment;text;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','txtCM101_2','/wlb_originals/wdg_Text','geomX;geomY;geomW;geomH;geomZ;font;alignment;text;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','El_TroinikHT_gr1','/wlb_mnEls/wdg_El_TroinikHT_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','KPP101','/wlb_mnEls/wdg_El_Kran_polozh','name;geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','Line_simple1','/wlb_mnEls/wdg_Line_simple','geomX;geomY;geomW;geomH;geomXsc;geomYsc;geomZ;elLst;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','Line_simple2','/wlb_mnEls/wdg_Line_simple','geomX;geomY;geomW;geomH;geomXsc;geomYsc;geomZ;elLst;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','Line_simple3','/wlb_mnEls/wdg_Line_simple','geomX;geomY;geomW;geomH;geomXsc;geomYsc;geomZ;elLst;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','N_CM','/wlb_Main/wdg_anShow','name;geomX;geomY;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','txtKPP101','/wlb_originals/wdg_Text','geomX;geomY;geomW;geomH;geomZ;font;alignment;text;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','El_Pipe_simple_gr2','/wlb_mnEls/wdg_El_Pipe_simple_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','El_Pipe_simple_gr4','/wlb_mnEls/wdg_El_Pipe_simple_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','El_Pipe_simple_gr15','/wlb_mnEls/wdg_El_Pipe_simple_gr','geomX;geomY;geomXsc;geomYsc;geomZ;fillColor;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','Text1','/wlb_originals/wdg_Text','geomX;geomY;geomW;geomH;geomZ;font;color;alignment;text;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','El_Pipe_simple_gr9','/wlb_mnEls/wdg_El_Pipe_simple_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','AT101_1','/wlb_mnEls/wdg_cooler','name;geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','Text2','/wlb_originals/wdg_Text','name;geomX;geomY;geomW;geomH;geomZ;font;color;alignment;text;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','El_Pipe_simple_gr10','/wlb_mnEls/wdg_El_Pipe_simple_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','AT101_2','/wlb_mnEls/wdg_cooler','name;geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','Text3','/wlb_originals/wdg_Text','geomX;geomY;geomW;geomH;geomZ;font;alignment;text;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','El_Pipe_simple_vert_gr9','/wlb_mnEls/wdg_El_Pipe_simple_vert_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','Text4','/wlb_originals/wdg_Text','geomX;geomY;geomW;geomH;geomZ;font;alignment;text;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','Text5','/wlb_originals/wdg_Text','geomX;geomY;geomW;geomH;geomZ;font;alignment;text;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','El_Pipe_simple_gr12','/wlb_mnEls/wdg_El_Pipe_simple_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','Text6','/wlb_originals/wdg_Text','name;geomX;geomY;geomW;geomH;geomZ;font;color;alignment;text;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','El_Pipe_simple_vert_gr7','/wlb_mnEls/wdg_El_Pipe_simple_vert_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','Text7','/wlb_originals/wdg_Text','geomX;geomY;geomW;geomH;geomZ;font;alignment;text;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','Text8','/wlb_originals/wdg_Text','name;geomX;geomY;geomW;geomH;geomZ;font;color;alignment;text;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','ElFigure1','/wlb_originals/wdg_ElFigure','geomX;geomY;geomW;geomH;geomYsc;geomZ;elLst;p1x;p1y;p2x;p2y;p3x;p3y;p4x;p4y;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','El_Pipe_simple_gr14','/wlb_mnEls/wdg_El_Pipe_simple_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','ElFigure2','/wlb_originals/wdg_ElFigure','name;geomX;geomY;geomW;geomH;geomYsc;geomZ;elLst;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','ElFigure3','/wlb_originals/wdg_ElFigure','name;geomX;geomY;geomW;geomH;geomYsc;geomZ;elLst;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','El_Pipe_simple_gr1','/wlb_mnEls/wdg_El_Pipe_simple_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','ElFigure4','/wlb_originals/wdg_ElFigure','name;geomX;geomY;geomW;geomH;geomYsc;geomZ;elLst;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','ElFigure5','/wlb_originals/wdg_ElFigure','name;geomX;geomY;geomW;geomH;geomYsc;geomZ;elLst;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','ElFigure6','/wlb_originals/wdg_ElFigure','geomX;geomY;geomW;geomH;geomYsc;geomZ;elLst;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','ElFigure7','/wlb_originals/wdg_ElFigure','name;geomX;geomY;geomW;geomH;geomXsc;geomYsc;geomZ;elLst;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','El_Pipe_simple_vert_gr3','/wlb_mnEls/wdg_El_Pipe_simple_vert_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','El_pipe_1_gr1','/wlb_mnEls/wdg_El_pipe_1_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_TroinikHT_gr1','/wlb_mnEls/wdg_El_TroinikHT_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','El_angleTL_gr3','/wlb_mnEls/wdg_El_angleTL_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','El_Pipe_simple_vert_gr15','/wlb_mnEls/wdg_El_Pipe_simple_vert_gr','geomX;geomY;geomXsc;geomYsc;geomZ;fillColor;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','El_Pipe_simple_vert_gr12','/wlb_mnEls/wdg_El_Pipe_simple_vert_gr','geomX;geomY;geomXsc;geomYsc;geomZ;fillColor;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','El_Pipe_simple_vert_gr11','/wlb_mnEls/wdg_El_Pipe_simple_vert_gr','geomX;geomY;geomXsc;geomYsc;geomZ;fillColor;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','T_PP101','/wlb_Main/wdg_anShow1','name;geomX;geomY;geomW;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','T_PP102','/wlb_Main/wdg_anShow1','name;geomX;geomY;geomW;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','El_Pipe_simple_vert_gr10','/wlb_mnEls/wdg_El_Pipe_simple_vert_gr','geomX;geomY;geomXsc;geomYsc;geomZ;fillColor;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','El_Pipe_simple_gr11','/wlb_mnEls/wdg_El_Pipe_simple_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','KS101','/wlb_mnEls/wdg_El_Kran_Sh','name;geomX;geomY;geomH;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','El_angleTR_gr3','/wlb_mnEls/wdg_El_angleTR_gr','name;geomX;geomY;geomXsc;geomYsc;geomZ;fillColor;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','KS102','/wlb_mnEls/wdg_El_Kran_Sh','name;geomX;geomY;geomH;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','CM101_1','/wlb_originals/wdg_ElFigure','geomX;geomY;geomW;geomH;geomXsc;geomYsc;geomZ;fillColor;elLst;p1x;p1y;p2x;p2y;p4x;p4y;p6x;p6y;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','KS103','/wlb_mnEls/wdg_El_Kran_Sh','name;geomX;geomY;geomH;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','CM101_2','/wlb_originals/wdg_ElFigure','geomX;geomY;geomW;geomH;geomXsc;geomYsc;geomZ;fillColor;elLst;p1x;p1y;p2x;p2y;p4x;p4y;p6x;p6y;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','KS104','/wlb_mnEls/wdg_El_Kran_Sh','name;geomX;geomY;geomH;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','KS105','/wlb_mnEls/wdg_El_Kran_Sh','name;geomX;geomY;geomH;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','KS106','/wlb_mnEls/wdg_El_Kran_Sh','name;geomX;geomY;geomH;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','KS107','/wlb_mnEls/wdg_El_Kran_Sh','name;geomX;geomY;geomH;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','KS121','/wlb_mnEls/wdg_El_Kran_Sh','name;geomX;geomY;geomH;geomXsc;geomYsc;geomZ;orient;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','KS122','/wlb_mnEls/wdg_El_Kran_Sh','name;geomX;geomY;geomH;geomXsc;geomYsc;geomZ;orient;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','KS123','/wlb_mnEls/wdg_El_Kran_Sh','name;geomX;geomY;geomH;geomXsc;geomYsc;geomZ;orient;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','CM101','/wlb_originals/wdg_Text','geomX;geomY;geomW;geomH;geomXsc;geomYsc;geomZ;font;color;alignment;text;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','P_PP101','/wlb_Main/wdg_anShow1','name;geomX;geomY;geomW;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','P_PP102','/wlb_Main/wdg_anShow1','name;geomX;geomY;geomW;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','C101_1','/wlb_mnEls/wdg_sep','name;geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','C101_2','/wlb_mnEls/wdg_sep','name;geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','C101_3','/wlb_mnEls/wdg_sep','name;geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','axis','/wlb_originals/wdg_ElFigure','geomX;geomY;geomW;geomH;geomXsc;geomZ;lineWdth;elLst;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','txtKS101','/wlb_originals/wdg_Text','geomX;geomY;geomW;geomH;geomZ;font;alignment;text;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','txtKS102','/wlb_originals/wdg_Text','geomX;geomY;geomW;geomH;geomZ;font;alignment;text;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','txtKS103','/wlb_originals/wdg_Text','geomX;geomY;geomW;geomH;geomZ;font;alignment;text;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','txtKS104','/wlb_originals/wdg_Text','geomX;geomY;geomW;geomH;geomZ;font;alignment;text;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','txtKS105','/wlb_originals/wdg_Text','geomX;geomY;geomW;geomH;geomZ;font;alignment;text;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','PP101','/wlb_mnEls/wdg_El_d','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','txtKS107','/wlb_originals/wdg_Text','geomX;geomY;geomW;geomH;geomZ;font;alignment;text;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','PP102','/wlb_mnEls/wdg_El_d','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','T_CM101_1','/wlb_Main/wdg_anShow1','name;geomX;geomY;geomW;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','T_CM101_2','/wlb_Main/wdg_anShow1','name;geomX;geomY;geomW;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','txtKS121','/wlb_originals/wdg_Text','geomX;geomY;geomW;geomH;geomZ;font;alignment;text;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','txtKS122','/wlb_originals/wdg_Text','geomX;geomY;geomW;geomH;geomZ;font;alignment;text;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','txtKS123','/wlb_originals/wdg_Text','geomX;geomY;geomW;geomH;geomZ;font;alignment;text;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','El_Pipe_simple_vert_gr13','/wlb_mnEls/wdg_El_Pipe_simple_vert_gr','geomX;geomY;geomXsc;geomYsc;geomZ;fillColor;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','El_Pipe_simple_vert_gr14','/wlb_mnEls/wdg_El_Pipe_simple_vert_gr','geomX;geomY;geomXsc;geomYsc;geomZ;fillColor;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','El_angleTL_gr1','/wlb_mnEls/wdg_El_angleTL_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','txtAT101_1','/wlb_originals/wdg_Text','geomX;geomY;geomW;geomH;geomZ;font;alignment;text;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','txtAT101_2','/wlb_originals/wdg_Text','geomX;geomY;geomW;geomH;geomZ;font;alignment;text;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','El_Troinik_gr9','/wlb_mnEls/wdg_El_Troinik_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','El_Pipe_simple_gr3','/wlb_mnEls/wdg_El_Pipe_simple_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','TAT101_1','/wlb_Main/wdg_anShow','name;geomX;geomY;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','TAT101_2','/wlb_Main/wdg_anShow','name;geomX;geomY;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','El_angleBR_gr1','/wlb_mnEls/wdg_El_angleBR_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','P_CM101_1','/wlb_Main/wdg_anShow1','name;geomX;geomY;geomW;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','P_CM101_2','/wlb_Main/wdg_anShow1','name;geomX;geomY;geomW;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','El_Troinik_gr2','/wlb_mnEls/wdg_El_Troinik_gr','name;geomX;geomY;geomXsc;geomYsc;geomZ;orient;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','El_Troinik_gr7','/wlb_mnEls/wdg_El_Troinik_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','El_angleTR_gr2','/wlb_mnEls/wdg_El_angleTR_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','El_angleBR_gr2','/wlb_mnEls/wdg_El_angleBR_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','El_Strela_gr1','/wlb_mnEls/wdg_El_Strela_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','txtPP101','/wlb_originals/wdg_Text','geomX;geomY;geomW;geomH;geomZ;font;alignment;text;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','txtPP102','/wlb_originals/wdg_Text','geomX;geomY;geomW;geomH;geomZ;font;alignment;text;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_round_square1','/wlb_mnEls/wdg_El_round_square1','geomX;geomY;geomXsc;geomYsc;geomZ;orient;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_Pipe_simple_gr5','/wlb_mnEls/wdg_El_Pipe_simple_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_Strela_gr7','/wlb_mnEls/wdg_El_Strela_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_Pipe_simple_gr4','/wlb_mnEls/wdg_El_Pipe_simple_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_TroinikHB_gr3','/wlb_mnEls/wdg_El_TroinikHB_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_Pipe_simple_vert_gr4','/wlb_mnEls/wdg_El_Pipe_simple_vert_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','El_StrelaVB_gr2','/wlb_mnEls/wdg_El_StrelaVB_gr','geomX;geomY;geomXsc;geomYsc;geomZ;fillColor;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_Pipe_simple_gr9','/wlb_mnEls/wdg_El_Pipe_simple_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_TroinikHB_gr2','/wlb_mnEls/wdg_El_TroinikHB_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_Strela_gr5','/wlb_mnEls/wdg_El_Strela_gr','geomX;geomY;geomXsc;geomYsc;geomZ;fillColor;orient;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_Pipe_simple_gr8','/wlb_mnEls/wdg_El_Pipe_simple_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_Pipe_simple_vert_gr5','/wlb_mnEls/wdg_El_Pipe_simple_vert_gr','geomX;geomY;geomXsc;geomYsc;geomZ;fillColor;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_Pipe_simple_gr13','/wlb_mnEls/wdg_El_Pipe_simple_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_Pipe_simple_vert_gr9','/wlb_mnEls/wdg_El_Pipe_simple_vert_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_Strela_gr6','/wlb_mnEls/wdg_El_Strela_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_round_square11','/wlb_mnEls/wdg_El_round_square1','geomX;geomY;geomXsc;geomYsc;geomZ;orient;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','PCV1','/wlb_mnEls/wdg_El_Kran_polozh','geomX;geomY;geomXsc;geomYsc;geomZ;c1;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','PCV2','/wlb_mnEls/wdg_El_Kran_polozh','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','PCV3','/wlb_mnEls/wdg_El_Kran_polozh','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_Pipe_simple_gr10','/wlb_mnEls/wdg_El_Pipe_simple_gr','geomX;geomY;geomXsc;geomYsc;geomZ;fillColor;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','T_PP1','/wlb_Main/wdg_anShow1','geomX;geomY;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','Text1','/wlb_originals/wdg_Text','geomX;geomY;geomW;geomH;geomZ;alignment;text;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','Text2','/wlb_originals/wdg_Text','geomX;geomY;geomW;geomH;geomZ;font;alignment;text;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_Pipe_simple_gr16','/wlb_mnEls/wdg_El_Pipe_simple_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','T_PP3','/wlb_Main/wdg_anShow1','geomX;geomY;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','Text3','/wlb_originals/wdg_Text','geomX;geomY;geomW;geomH;geomZ;font;alignment;text;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_Pipe_simple_gr15','/wlb_mnEls/wdg_El_Pipe_simple_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','Text4','/wlb_originals/wdg_Text','geomX;geomY;geomW;geomH;geomZ;alignment;text;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_Pipe_simple_vert_gr2','/wlb_mnEls/wdg_El_Pipe_simple_vert_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','T_PP5','/wlb_Main/wdg_anShow1','geomX;geomY;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','Text5','/wlb_originals/wdg_Text','geomX;geomY;geomW;geomH;geomZ;font;color;alignment;text;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_Pipe_simple_gr7','/wlb_mnEls/wdg_El_Pipe_simple_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','Text6','/wlb_originals/wdg_Text','geomX;geomY;geomW;geomH;geomZ;font;alignment;text;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_Pipe_simple_vert_gr1','/wlb_mnEls/wdg_El_Pipe_simple_vert_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','Text7','/wlb_originals/wdg_Text','geomX;geomY;geomW;geomH;geomZ;alignment;text;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_pipe_1_gr4','/wlb_mnEls/wdg_El_pipe_1_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','Text8','/wlb_originals/wdg_Text','geomY;geomW;geomH;geomZ;font;color;alignment;text;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','Text9','/wlb_originals/wdg_Text','geomX;geomY;geomW;geomH;geomZ;alignment;text;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','P_PP1','/wlb_Main/wdg_anShow1','geomX;geomY;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_angleBR_gr2','/wlb_mnEls/wdg_El_angleBR_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','P_PP3','/wlb_Main/wdg_anShow1','geomX;geomY;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_Krest_gr1','/wlb_mnEls/wdg_El_Krest_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_Pipe_simple_gr11','/wlb_mnEls/wdg_El_Pipe_simple_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','El_StrelaVB_gr3','/wlb_mnEls/wdg_El_StrelaVB_gr','geomX;geomY;geomXsc;geomYsc;geomZ;fillColor;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','El_TroinikHT_gr2','/wlb_mnEls/wdg_El_TroinikHT_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','P_PP5','/wlb_Main/wdg_anShow1','geomX;geomY;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_Pipe_simple_gr12','/wlb_mnEls/wdg_El_Pipe_simple_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','ElFigure1','/wlb_originals/wdg_ElFigure','geomX;geomY;geomW;geomH;geomYsc;geomZ;elLst;p1x;p1y;p2x;p2y;p3x;p3y;p4x;p4y;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_angleBR_gr1','/wlb_mnEls/wdg_El_angleBR_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_Pipe_simple_vert_gr10','/wlb_mnEls/wdg_El_Pipe_simple_vert_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','ElFigure2','/wlb_originals/wdg_ElFigure','geomX;geomY;geomW;geomH;geomYsc;geomZ;elLst;p1x;p1y;p2x;p2y;p3x;p3y;p4x;p4y;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','ElFigure3','/wlb_originals/wdg_ElFigure','geomX;geomY;geomW;geomH;geomZ;elLst;p1x;p1y;p2x;p2y;p3x;p3y;p4x;p4y;p5x;p5y;p7x;p7y;p8x;p8y;p9x;p9y;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_Pipe_simple_vert_gr13','/wlb_mnEls/wdg_El_Pipe_simple_vert_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_pipe_1_gr6','/wlb_mnEls/wdg_El_pipe_1_gr','geomX;geomY;geomXsc;geomYsc;geomZ;fillColor;orient;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','ElFigure5','/wlb_originals/wdg_ElFigure','geomX;geomY;geomW;geomH;geomZ;elLst;p1x;p1y;p2x;p2y;p3x;p3y;p4x;p4y;p5x;p5y;p7x;p7y;p8x;p8y;p9x;p9y;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_Pipe_simple_gr17','/wlb_mnEls/wdg_El_Pipe_simple_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','ElFigure6','/wlb_originals/wdg_ElFigure','geomX;geomY;geomW;geomH;geomYsc;geomZ;elLst;p1x;p1y;p2x;p2y;p3x;p3y;p4x;p4y;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_Krest_gr5','/wlb_mnEls/wdg_El_Krest_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','ElFigure7','/wlb_originals/wdg_ElFigure','geomX;geomY;geomW;geomH;geomZ;elLst;p1x;p1y;p2x;p2y;p3x;p3y;p4x;p4y;p5x;p5y;p7x;p7y;p8x;p8y;p9x;p9y;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_Pipe_simple_vert_gr12','/wlb_mnEls/wdg_El_Pipe_simple_vert_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','El_TroinikVL_gr1','/wlb_mnEls/wdg_El_TroinikVL_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_Krest_gr4','/wlb_mnEls/wdg_El_Krest_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_Krest_gr2','/wlb_mnEls/wdg_El_Krest_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','ElFigure9','/wlb_originals/wdg_ElFigure','geomX;geomY;geomW;geomH;geomYsc;geomZ;orient;elLst;p1x;p1y;p2x;p2y;p3x;p3y;p4x;p4y;p5x;p5y;p6x;p6y;p7x;p7y;p8x;p8y;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','PC1','/wlb_Main/wdg_anShow','geomX;geomY;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','PC2','/wlb_Main/wdg_anShow','geomX;geomY;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','PC3','/wlb_Main/wdg_anShow','geomX;geomY;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','Title','/wlb_originals/wdg_Text','name;geomX;geomY;geomW;geomH;geomXsc;geomYsc;geomZ;font;color;alignment;text;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_Pipe_simple_vert_gr8','/wlb_mnEls/wdg_El_Pipe_simple_vert_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_Pipe_simple_gr2','/wlb_mnEls/wdg_El_Pipe_simple_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','Text10','/wlb_originals/wdg_Text','geomX;geomY;geomW;geomH;geomZ;alignment;text;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','El_TroinikVL_gr2','/wlb_mnEls/wdg_El_TroinikVL_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','Text11','/wlb_originals/wdg_Text','geomX;geomY;geomW;geomH;geomZ;font;alignment;text;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','BC1','/wlb_mnEls/wdg_El_Kran_Sh','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','Text12','/wlb_originals/wdg_Text','geomX;geomY;geomW;geomH;geomZ;font;alignment;text;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','BC2','/wlb_mnEls/wdg_El_Kran_Sh','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_Pipe_simple_vert_gr11','/wlb_mnEls/wdg_El_Pipe_simple_vert_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','Text13','/wlb_originals/wdg_Text','geomX;geomY;geomW;geomH;geomZ;font;alignment;text;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_angleTL_gr1','/wlb_mnEls/wdg_El_angleTL_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','Text14','/wlb_originals/wdg_Text','geomX;geomY;geomW;geomH;geomZ;font;alignment;text;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_pipe_1_gr2','/wlb_mnEls/wdg_El_pipe_1_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','Text20','/wlb_originals/wdg_Text','geomX;geomY;geomW;geomH;geomZ;font;alignment;text;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','Text15','/wlb_originals/wdg_Text','geomX;geomY;geomW;geomH;geomZ;font;color;alignment;text;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','BC5','/wlb_mnEls/wdg_El_Kran_Sh','geomX;geomY;geomH;geomXsc;geomYsc;geomZ;orient;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','Text21','/wlb_originals/wdg_Text','geomX;geomY;geomW;geomH;geomZ;font;color;alignment;text;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','Text16','/wlb_originals/wdg_Text','geomX;geomY;geomW;geomH;geomZ;font;alignment;text;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','BC6','/wlb_mnEls/wdg_El_Kran_Sh','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_Krest_gr3','/wlb_mnEls/wdg_El_Krest_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','Text22','/wlb_originals/wdg_Text','geomX;geomY;geomW;geomH;geomZ;font;color;alignment;text;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','Text17','/wlb_originals/wdg_Text','geomX;geomY;geomW;geomH;geomZ;font;alignment;text;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','BC7','/wlb_mnEls/wdg_El_Kran_Sh','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','Text23','/wlb_originals/wdg_Text','geomX;geomY;geomW;geomH;geomZ;alignment;text;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','Text18','/wlb_originals/wdg_Text','geomX;geomY;geomW;geomH;geomZ;font;color;alignment;text;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','Text24','/wlb_originals/wdg_Text','geomX;geomY;geomW;geomH;geomZ;font;alignment;text;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','Text19','/wlb_originals/wdg_Text','geomX;geomY;geomW;geomH;geomZ;font;alignment;text;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','Box1','/wlb_originals/wdg_Box','geomX;geomY;geomW;geomH;geomZ;backColor;bordWidth;bordColor;bordStyle;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','Text25','/wlb_originals/wdg_Text','geomX;geomY;geomW;geomH;geomZ;font;alignment;text;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','Box2','/wlb_originals/wdg_Box','geomX;geomY;geomW;geomH;geomZ;backColor;bordWidth;bordColor;bordStyle;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','Text26','/wlb_originals/wdg_Text','geomX;geomY;geomW;geomH;geomZ;font;color;alignment;text;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','BC21','/wlb_mnEls/wdg_El_Kran_Sh','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','BC22','/wlb_mnEls/wdg_El_Kran_Sh','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_d1','/wlb_mnEls/wdg_El_d','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_d2','/wlb_mnEls/wdg_El_d','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_d3','/wlb_mnEls/wdg_El_d','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_d4','/wlb_mnEls/wdg_El_d','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','Pi','/wlb_Main/wdg_anShow','geomX;geomY;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','Ti','/wlb_Main/wdg_anShow','geomX;geomY;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','compr1','/wlb_originals/wdg_ElFigure','geomX;geomY;geomW;geomH;geomZ;fillColor;elLst;p1x;p1y;p2x;p2y;p4x;p4y;p6x;p6y;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','compr2','/wlb_originals/wdg_ElFigure','geomX;geomY;geomW;geomH;geomZ;fillColor;elLst;p1x;p1y;p2x;p2y;p4x;p4y;p6x;p6y;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','compr3','/wlb_originals/wdg_ElFigure','geomX;geomY;geomW;geomH;geomZ;fillColor;elLst;p1x;p1y;p2x;p2y;p4x;p4y;p6x;p6y;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','compr4','/wlb_originals/wdg_ElFigure','geomX;geomY;geomW;geomH;geomZ;fillColor;elLst;p1x;p1y;p2x;p2y;p4x;p4y;p6x;p6y;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','compr5','/wlb_originals/wdg_ElFigure','geomX;geomY;geomW;geomH;geomZ;fillColor;elLst;p1x;p1y;p2x;p2y;p4x;p4y;p6x;p6y;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','compr6','/wlb_originals/wdg_ElFigure','geomX;geomY;geomW;geomH;geomZ;fillColor;elLst;p1x;p1y;p2x;p2y;p4x;p4y;p6x;p6y;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_Pipe_simple_vert_gr3','/wlb_mnEls/wdg_El_Pipe_simple_vert_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_Pipe_simple_gr3','/wlb_mnEls/wdg_El_Pipe_simple_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_Troinik_gr2','/wlb_mnEls/wdg_El_Troinik_gr','geomX;geomY;geomXsc;geomYsc;geomZ;orient;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_Pipe_simple_gr6','/wlb_mnEls/wdg_El_Pipe_simple_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_pipe_1_gr26','/wlb_mnEls/wdg_El_pipe_1_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','El_TroinikHT_gr3','/wlb_mnEls/wdg_El_TroinikHT_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_Pipe_simple_vert_gr6','/wlb_mnEls/wdg_El_Pipe_simple_vert_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_Strela_gr2','/wlb_mnEls/wdg_El_Strela_gr','geomX;geomY;geomXsc;geomYsc;geomZ;orient;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_angleBR_gr3','/wlb_mnEls/wdg_El_angleBR_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_Pipe_simple_vert_gr7','/wlb_mnEls/wdg_El_Pipe_simple_vert_gr','geomX;geomY;geomXsc;geomYsc;geomZ;fillColor;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','El_TroinikHT_gr4','/wlb_mnEls/wdg_El_TroinikHT_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','El_TroinikHT_gr5','/wlb_mnEls/wdg_El_TroinikHT_gr','geomX;geomY;geomXsc;geomYsc;geomZ;fillColor;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','El_TroinikHT_gr6','/wlb_mnEls/wdg_El_TroinikHT_gr','geomX;geomY;geomXsc;geomYsc;geomZ;fillColor;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_pipe_1_gr13','/wlb_mnEls/wdg_El_pipe_1_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','F_PP1','/wlb_Main/wdg_anShow1','geomX;geomY;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','F_PP3','/wlb_Main/wdg_anShow1','geomX;geomY;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_angleBR_gr4','/wlb_mnEls/wdg_El_angleBR_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','F_PP5','/wlb_Main/wdg_anShow1','geomX;geomY;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_Pipe_simple_gr14','/wlb_mnEls/wdg_El_Pipe_simple_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','El_StrelaVB_gr1','/wlb_mnEls/wdg_El_StrelaVB_gr','geomX;geomY;geomXsc;geomYsc;geomZ;fillColor;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','El_Pipe_simple_vert_gr2','/wlb_mnEls/wdg_El_Pipe_simple_vert_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','El_Pipe_simple_gr5','/wlb_mnEls/wdg_El_Pipe_simple_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','El_Pipe_simple_gr6','/wlb_mnEls/wdg_El_Pipe_simple_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','El_Pipe_simple_gr7','/wlb_mnEls/wdg_El_Pipe_simple_gr','geomX;geomY;geomW;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','El_Pipe_simple_gr8','/wlb_mnEls/wdg_El_Pipe_simple_gr','geomX;geomY;geomW;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','El_Pipe_simple_gr13','/wlb_mnEls/wdg_El_Pipe_simple_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','El_angleTR_gr1','/wlb_mnEls/wdg_El_angleTR_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','comprEn101','/wlb_AGLKS/wdg_comprEn','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','comprEn102','/wlb_AGLKS/wdg_comprEn','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','comprEn201','/wlb_AGLKS/wdg_comprEn','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','comprEn202','/wlb_AGLKS/wdg_comprEn','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','comprEn301','/wlb_AGLKS/wdg_comprEn','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','comprEn302','/wlb_AGLKS/wdg_comprEn','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_Obr_StrelaHR_gr1','/wlb_mnEls/wdg_El_Obr_StrelaHR_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_StrelaVB_gr2','/wlb_mnEls/wdg_El_StrelaVB_gr','geomX;geomY;geomXsc;geomYsc;geomZ;fillColor;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_TroinikHT_gr2','/wlb_mnEls/wdg_El_TroinikHT_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_TroinikHT_gr3','/wlb_mnEls/wdg_El_TroinikHT_gr','geomX;geomY;geomXsc;geomYsc;geomZ;fillColor;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_TroinikHT_gr4','/wlb_mnEls/wdg_El_TroinikHT_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_angleTL_gr2','/wlb_mnEls/wdg_El_angleTL_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_angleTL_gr3','/wlb_mnEls/wdg_El_angleTL_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_angleTL_gr4','/wlb_mnEls/wdg_El_angleTL_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_angleTL_gr5','/wlb_mnEls/wdg_El_angleTL_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_angleTL_gr6','/wlb_mnEls/wdg_El_angleTL_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_angleTL_gr7','/wlb_mnEls/wdg_El_angleTL_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_angleTL_gr8','/wlb_mnEls/wdg_El_angleTL_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_angleTL_gr9','/wlb_mnEls/wdg_El_angleTL_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('CM101','El_Obr_StrelaHR_gr1','/wlb_mnEls/wdg_El_Obr_StrelaHR_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_angleTR_gr10','/wlb_mnEls/wdg_El_angleTR_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_angleTR_gr2','/wlb_mnEls/wdg_El_angleTR_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_angleTR_gr3','/wlb_mnEls/wdg_El_angleTR_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_angleTR_gr4','/wlb_mnEls/wdg_El_angleTR_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_angleTR_gr5','/wlb_mnEls/wdg_El_angleTR_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_angleTR_gr6','/wlb_mnEls/wdg_El_angleTR_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_angleTR_gr7','/wlb_mnEls/wdg_El_angleTR_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_angleTR_gr8','/wlb_mnEls/wdg_El_angleTR_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
INSERT INTO wlb_AGLKS_incl VALUES('KCH_MN1','El_angleTR_gr9','/wlb_mnEls/wdg_El_angleTR_gr','geomX;geomY;geomXsc;geomYsc;geomZ;',2);
CREATE TABLE IF NOT EXISTS 'wlb_AGLKS_io' ("IDW" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"IO_VAL" TEXT DEFAULT '' ,"SELF_FLG" INTEGER DEFAULT '' ,"CFG_TMPL" TEXT DEFAULT '' ,"CFG_VAL" TEXT DEFAULT '' ,"IDC" TEXT DEFAULT '' ,"uk#IO_VAL" TEXT DEFAULT '' ,"uk#CFG_TMPL" TEXT DEFAULT '' ,"ru#IO_VAL" TEXT DEFAULT '' ,"ru#CFG_TMPL" TEXT DEFAULT '' ,"ru#CFG_VAL" TEXT DEFAULT '' ,"uk#CFG_VAL" TEXT DEFAULT '' , PRIMARY KEY ("IDW","ID","IDC"));
INSERT INTO wlb_AGLKS_io VALUES('CM101','name','CM101',32,'','','','','','CM101','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomW','900',32,'','','','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomH','580',32,'','','','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','backColor','#5A5A5A',96,'backColorFrame','','','','','','backColorFrame','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','bordWidth','1',32,'','','','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','bordColor','black',32,'','','','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.2',32,'','','El_Troinik_gr7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','286',32,'','','El_Troinik_gr7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','709',32,'','','El_Troinik_gr7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','274',32,'','','KS105','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','743',32,'','','KS105','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','name','BC105',32,'','','KS105','КК105','','КШ105','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','128',32,'','','KS104','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.3',32,'','','KS104','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.3',32,'','','KS104','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomH','103.34',32,'','','KS104','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','319',32,'','','KS104','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','743',32,'','','KS104','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','name','BC104',32,'','','KS104','КК104','','КШ104','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','p6y','12',0,'','','CM101_2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','p6x','45',0,'','','CM101_2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','p1x','1',0,'','','CM101_2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','p1y','1',0,'','','CM101_2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','p2x','1',0,'','','CM101_2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','p2y','50',0,'','','CM101_2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','p4x','45',0,'','','CM101_2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','p4y','41',0,'','','CM101_2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','elLst','line:1:2::::
line:2:4::::
line:1:6::::
line:6:4::::
fill:1:2:4:6:#c0c0c0
',32,'','','CM101_2','line:1:2::::
line:2:4::::
line:1:6::::
line:6:4::::
fill:1:2:4:6:#c0c0c0
','','line:1:2::::
line:2:4::::
line:1:6::::
line:6:4::::
fill:1:2:4:6:#c0c0c0
','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','fillColor','green',32,'','','CM101_2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','116',32,'','','CM101_2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.3',32,'','','KS103','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','516',32,'','','CM101_2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','74',32,'','','KS103','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','1.56',32,'','','CM101_2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','1.6',32,'','','CM101_2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomH','55',32,'','','CM101_2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomW','49',32,'','','CM101_2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','227',32,'','','CM101_2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','elLst','line:1:2::::
line:2:4::::
line:1:6::::
line:6:4::::
fill:1:2:4:6:#c0c0c0
',32,'','','CM101_1','line:1:2::::
line:2:4::::
line:1:6::::
line:6:4::::
fill:1:2:4:6:#c0c0c0
','','line:1:2::::
line:2:4::::
line:1:6::::
line:6:4::::
fill:1:2:4:6:#c0c0c0
','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.3',32,'','','KS103','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','p1x','1',0,'','','CM101_1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','p1y','1',0,'','','CM101_1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','p2x','1',0,'','','CM101_1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','p2y','50',0,'','','CM101_1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','p4x','45',0,'','','CM101_1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','p4y','41',0,'','','CM101_1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','p6x','45',0,'','','CM101_1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','p6y','12',0,'','','CM101_1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','name','BC121',32,'','','KS103','КК121','','КШ121','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','745',32,'','','KS103','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','73',32,'','','KS103','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomH','103.34',32,'','','KS103','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','name','АТ101 1',32,'','','AT101_1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','59',32,'','','El_Pipe_simple_vert_gr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.3',32,'','','El_Pipe_simple_vert_gr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.2',32,'','','El_Pipe_simple_vert_gr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','772',32,'','','El_Pipe_simple_vert_gr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','304',32,'','','El_Pipe_simple_vert_gr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','text','From PP1',32,'','','Text1','З PP1','','Из PP1','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomH','20',32,'','','Text1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','81',32,'','','Text1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','font','Arial 12 1',32,'','','Text1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','color','#B4B400',32,'','','Text1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','name','T PP101',32,'','','T_PP101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','129',32,'','','T_PP101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','278',32,'','','T_PP101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.2',32,'','','El_Pipe_simple_gr14','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','330',32,'','','El_Pipe_simple_gr14','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.5',32,'','','El_Pipe_simple_gr14','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','789',32,'','','El_Pipe_simple_gr14','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','14',32,'','','ElFigure3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','elLst','line:(7|0):(7|99):::::
line:(0|0):(14|0):::::
',32,'','','ElFigure3','line:(7|0):(7|99):::::
line:(0|0):(14|0):::::
','','line:(7|0):(7|99):::::
line:(0|0):(14|0):::::
','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.85',32,'','','ElFigure3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','349',32,'','','ElFigure3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomW','15',32,'','','ElFigure3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomH','100',32,'','','ElFigure3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','381',32,'','','ElFigure3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.2',32,'','','El_Pipe_simple_gr13','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','29',32,'','','El_Pipe_simple_gr13','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','name','',32,'','','ElFigure3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','793',32,'','','El_Pipe_simple_gr13','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','85',32,'','','El_Pipe_simple_gr13','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.64',32,'','','El_Pipe_simple_gr13','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.5',32,'','','ElFigure2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','24',32,'','','ElFigure2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','elLst','line:(7|0):(7|99):::::
line:(0|0):(14|0):::::
',32,'','','ElFigure2','line:(7|0):(7|99):::::
line:(0|0):(14|0):::::
','','line:(7|0):(7|99):::::
line:(0|0):(14|0):::::
','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomW','15',32,'','','ElFigure2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomH','100',32,'','','ElFigure2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomW','63',32,'','','Text1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','5',32,'','','Text1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','347',32,'','','Text1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','48',32,'','','El_Pipe_simple_vert_gr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.35',32,'','','El_Pipe_simple_vert_gr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','577',32,'','','El_Pipe_simple_gr11','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','331',32,'','','El_Pipe_simple_gr11','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','27',32,'','','El_Pipe_simple_gr10','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','92',32,'','','Text8','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','font','Arial 12 1',32,'','','Text8','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','color','blue',32,'','','Text8','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','text','Liquid drop',32,'','','Text8','Злив рідини','','Слив жидкости','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','726',32,'','','El_Pipe_simple_gr10','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','132',32,'','','El_Pipe_simple_gr10','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.24',32,'','','El_Pipe_simple_gr10','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.2',32,'','','El_Pipe_simple_gr10','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','name','',32,'','','Text8','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomW','93.03',32,'','','Text8','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomH','20.17',32,'','','Text8','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','746',32,'','','Text8','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','483',32,'','','Text8','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','font','Arial 12 1',32,'','','Text6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','color','#B4B400',32,'','','Text6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','text','To PP3',32,'','','Text6','До PP3','','На PP3','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','147',32,'','','Text7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','332',32,'','','Text7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomW','43',32,'','','Text7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomH','20',32,'','','Text7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','111',32,'','','Text7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','font','Arial 12 0 0 0 0',32,'','','Text7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','text','C101/1',32,'','','Text7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','86',32,'','','Text6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomW','64',32,'','','Text6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomH','24.17',32,'','','Text6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','793',32,'','','Text6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','316',32,'','','Text6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomH','20',32,'','','Text5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','112',32,'','','Text5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','font','Arial 12 0 0 0 0',32,'','','Text5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','text','C101/2',32,'','','Text5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','name','',32,'','','Text6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomW','43',32,'','','Text5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','410',32,'','','Text5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','332',32,'','','Text5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','58',32,'','','El_Pipe_simple_vert_gr9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.2',32,'','','El_Pipe_simple_vert_gr9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.3',32,'','','El_Pipe_simple_vert_gr9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','104',32,'','','El_Pipe_simple_vert_gr9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','774',32,'','','El_Pipe_simple_vert_gr9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','text','C101/3',32,'','','Text4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','332',32,'','','Text4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomW','43',32,'','','Text4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomH','20',32,'','','Text4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','113',32,'','','Text4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','font','Arial 12 0 0 0 0',32,'','','Text4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','293',32,'','','El_Pipe_simple_vert_gr8','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','53',32,'','','El_Pipe_simple_vert_gr8','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','650',32,'','','Text4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.2',32,'','','El_Pipe_simple_vert_gr8','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.4',32,'','','El_Pipe_simple_vert_gr8','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','90',32,'','','Text3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','font','Arial 12 0 0 0 0',32,'','','Text3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','text','BC106',32,'','','Text3','КК106','','КШ106','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','561',32,'','','El_Pipe_simple_vert_gr8','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomH','20',32,'','','Text3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','121',32,'','','Text3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomW','49',32,'','','Text3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','482',32,'','','Text3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','62',32,'','','AT101_2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','name','АТ101 1',32,'','','AT101_2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.47',32,'','','AT101_2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.5',32,'','','AT101_2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','599',32,'','','AT101_2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','320',32,'','','AT101_2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','69',32,'','','Text2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomW','64',32,'','','Text2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomH','24.17',32,'','','Text2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','82',32,'','','Text2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','font','Arial 12 1',32,'','','Text2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','color','#B4B400',32,'','','Text2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','text','To torch',32,'','','Text2','На факел','','На свечу','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','709',32,'','','El_Pipe_simple_vert_gr7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','102',32,'','','El_Pipe_simple_vert_gr7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.2',32,'','','El_Pipe_simple_vert_gr7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','2.33',32,'','','El_Pipe_simple_vert_gr7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','50',32,'','','El_Pipe_simple_vert_gr7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','107',32,'','','El_angleBR_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','85',32,'','','El_angleBR_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','421',32,'','','El_Pipe_simple_vert_gr13','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','443',32,'','','El_Pipe_simple_vert_gr13','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.2',32,'','','El_Pipe_simple_vert_gr13','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.6',32,'','','El_Pipe_simple_vert_gr13','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','47',32,'','','El_Pipe_simple_vert_gr13','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','fillColor','blue',32,'','','El_Pipe_simple_vert_gr13','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','661',32,'','','El_Pipe_simple_vert_gr14','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','372',32,'','','El_Pipe_simple_vert_gr14','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.2',32,'','','El_Pipe_simple_vert_gr14','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.45',32,'','','El_Pipe_simple_vert_gr14','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','45',32,'','','El_Pipe_simple_vert_gr14','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','fillColor','blue',32,'','','El_Pipe_simple_vert_gr14','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','661',32,'','','El_Pipe_simple_vert_gr15','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','439',32,'','','El_Pipe_simple_vert_gr15','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.2',32,'','','El_Pipe_simple_vert_gr15','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.65',32,'','','El_Pipe_simple_vert_gr15','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','49',32,'','','El_Pipe_simple_vert_gr15','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','fillColor','blue',32,'','','El_Pipe_simple_vert_gr15','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','150',32,'','','El_TroinikHT_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','fillColor','blue',32,'','','El_TroinikHT_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','1',32,'','','','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','1',32,'','','','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','329',32,'','','txtAT101_1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','301',32,'','','txtAT101_1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomW','55',32,'','','txtAT101_1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomH','24',32,'','','txtAT101_1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','104',32,'','','txtAT101_1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','font','Arial 12 0 0 0 0',32,'','','txtAT101_1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','text','AT101/1',32,'','','txtAT101_1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','590',32,'','','txtAT101_2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','303',32,'','','txtAT101_2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomW','55',32,'','','txtAT101_2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomH','24',32,'','','txtAT101_2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','105',32,'','','txtAT101_2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','font','Arial 12 0 0 0 0',32,'','','txtAT101_2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','text','AT101/2',32,'','','txtAT101_2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','name','T АТ 101/1',32,'','','TAT101_1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','312',32,'','','TAT101_1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','429',32,'','','TAT101_1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','34',32,'','','TAT101_1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','name','T АТ 101/1',32,'','','TAT101_2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','559',32,'','','TAT101_2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','429',32,'','','TAT101_2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','36',32,'','','TAT101_2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','name','P CM101/1',32,'','','P_CM101_1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','274',32,'','','P_CM101_1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','374',32,'','','P_CM101_1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomW','102',32,'','','P_CM101_1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','99',32,'','','P_CM101_1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','name','P CM101/1',32,'','','P_CM101_2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','530',32,'','','P_CM101_2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','374',32,'','','P_CM101_2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomW','102',32,'','','P_CM101_2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','101',32,'','','P_CM101_2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','fillColor','blue',32,'','','El_angleTR_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','2',32,'','','El_angleTR_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.2',32,'','','El_angleTR_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.2',32,'','','El_angleTR_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','158',32,'','','El_angleTR_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','499',32,'','','El_angleTR_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','name','',32,'','','El_angleTR_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.2',32,'','','El_angleTR_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','3',32,'','','El_angleTR_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.2',32,'','','El_angleTR_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','331',32,'','','El_angleTR_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','4',32,'','','El_angleTR_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','561',32,'','','El_angleTR_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.2',32,'','','El_angleTR_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.2',32,'','','El_angleTR_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','name','',32,'','','El_Troinik_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.2',32,'','','El_angleTL_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','8',32,'','','El_angleTL_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','301',32,'','','El_angleTR_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','331',32,'','','El_angleTR_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','107',32,'','','El_Troinik_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','151',32,'','','El_Troinik_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.2',32,'','','El_Troinik_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.2',32,'','','El_Troinik_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','39',32,'','','El_Troinik_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','orient','0',32,'','','El_Troinik_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','132',32,'','','El_angleTL_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.2',32,'','','El_angleTL_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','774',32,'','','El_angleTL_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','5',32,'','','El_angleTL_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.2',32,'','','El_angleTL_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','203',32,'','','txtPP101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','363',32,'','','txtPP101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomW','43',32,'','','txtPP101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomH','20',32,'','','txtPP101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','106',32,'','','txtPP101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','font','Arial 12 0 0 0 0',32,'','','txtPP101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','text','PP101',32,'','','txtPP101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','331',32,'','','El_angleTL_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.2',32,'','','El_angleTL_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','521',32,'','','El_angleTL_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','1',32,'','','El_angleTL_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','472',32,'','','txtPP102','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','359',32,'','','txtPP102','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomW','48',32,'','','txtPP102','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomH','20',32,'','','txtPP102','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','107',32,'','','txtPP102','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','font','Arial 12 0 0 0 0',32,'','','txtPP102','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','text','PP102',32,'','','txtPP102','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.2',32,'','','El_angleTL_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.2',32,'','','El_angleTL_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','331',32,'','','El_angleTL_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','259',32,'','','El_angleTL_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','13',32,'','','El_angleBR_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.2',32,'','','El_angleBR_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.2',32,'','','El_angleBR_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','488',32,'','','ElFigure1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomW','16',32,'','','ElFigure1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','250',32,'','','ElFigure1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','1.71',32,'','','El_Pipe_simple_gr11','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','22',32,'','','El_Pipe_simple_gr11','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.2',32,'','','El_Pipe_simple_gr11','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','1.3',32,'','','ElFigure7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','elLst','line:(18|3):(18|47.23):::::
line:(12|47.23):(25|47.23):::::
line:(0|3.08):(18|3):::::
',32,'','','ElFigure7','line:(18|3):(18|47.23):::::
line:(12|47.23):(25|47.23):::::
line:(0|3.08):(18|3):::::
','','line:(18|3):(18|47.23):::::
line:(12|47.23):(25|47.23):::::
line:(0|3.08):(18|3):::::
','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','66',32,'','','ElFigure7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','201',32,'','','ElFigure7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomW','25',32,'','','ElFigure7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomH','52',32,'','','ElFigure7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','1',32,'','','ElFigure7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','254',32,'','','CM101_1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','227',32,'','','CM101_1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomW','49',32,'','','CM101_1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','77',32,'','','KS102','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomH','103.34',32,'','','KS102','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.3',32,'','','KS102','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.3',32,'','','KS102','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','270',32,'','','KS102','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','68',32,'','','KS102','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','name','BC102',32,'','','KS102','КК102','','КШ102','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','63',32,'','','KS101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.3',32,'','','KS101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','65',32,'','','ElFigure6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','elLst','line:(7|3):(7|47.23):::::
line:(1|47.23):(14|47.23):::::
',32,'','','ElFigure6','line:(7|3):(7|47.23):::::
line:(1|47.23):(14|47.23):::::
','','line:(7|3):(7|47.23):::::
line:(1|47.23):(14|47.23):::::
','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','226',32,'','','ElFigure7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','name','',32,'','','ElFigure7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','319',32,'','','KS101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomH','103.34',32,'','','KS101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.3',32,'','','KS101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','68',32,'','','KS101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomW','102',32,'','','T_PP102','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','100',32,'','','T_PP102','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','name','BC101',32,'','','KS101','КК101','','КШ101','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','231',32,'','','T_PP102','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','411',32,'','','T_PP102','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','name','T PP102',32,'','','T_PP102','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','93',32,'','','T_PP101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomW','102',32,'','','T_PP101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','274',32,'','','ElFigure6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomW','16',32,'','','ElFigure6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomH','52',32,'','','ElFigure6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','1',32,'','','ElFigure6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','12',32,'','','ElFigure5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','216',32,'','','ElFigure6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','elLst','line:(7|0):(7|99):::::
line:(0|0):(14|0):::::
',32,'','','ElFigure5','line:(7|0):(7|99):::::
line:(0|0):(14|0):::::
','','line:(7|0):(7|99):::::
line:(0|0):(14|0):::::
','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','349',32,'','','ElFigure5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomW','15',32,'','','ElFigure5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomH','100',32,'','','ElFigure5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.85',32,'','','ElFigure5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','16',32,'','','El_Pipe_simple_gr15','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','fillColor','blue',32,'','','El_Pipe_simple_gr15','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','635',32,'','','ElFigure5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','name','',32,'','','ElFigure5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','177',32,'','','El_Pipe_simple_gr15','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','6.62',32,'','','El_Pipe_simple_gr15','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.2',32,'','','El_Pipe_simple_gr15','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','499',32,'','','El_Pipe_simple_gr15','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomH','100',32,'','','ElFigure4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.5',32,'','','ElFigure4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','10',32,'','','ElFigure4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','elLst','line:(7|0):(7|99):::::
line:(0|0):(14|0):::::
',32,'','','ElFigure4','line:(7|0):(7|99):::::
line:(0|0):(14|0):::::
','','line:(7|0):(7|99):::::
line:(0|0):(14|0):::::
','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomW','15',32,'','','ElFigure4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','31',32,'','','El_Pipe_simple_gr14','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','349',32,'','','ElFigure4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','584',32,'','','ElFigure4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','name','',32,'','','ElFigure4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','114',32,'','','CM101_1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','fillColor','green',32,'','','CM101_1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','1.56',32,'','','CM101_1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomH','55',32,'','','CM101_1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','1.6',32,'','','CM101_1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','23',32,'','','El_Pipe_simple_gr12','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','321',32,'','','ElFigure2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','name','',32,'','','ElFigure2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','p2x','6',0,'','','ElFigure1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.2',32,'','','El_Pipe_simple_gr12','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','p2y','95',0,'','','ElFigure1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','p3x','1',0,'','','ElFigure1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','p3y','94.23',0,'','','ElFigure1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','p4x','12',0,'','','ElFigure1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','p4y','94.23',0,'','','ElFigure1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','728',32,'','','El_Pipe_simple_gr12','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','286',32,'','','El_Pipe_simple_gr12','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.19',32,'','','El_Pipe_simple_gr12','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','790',32,'','','Text2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','60',32,'','','AT101_1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','name','',32,'','','Text2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.5',32,'','','AT101_1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.47',32,'','','AT101_1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','300',32,'','','El_Pipe_simple_vert_gr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.2',32,'','','El_Pipe_simple_vert_gr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','259',32,'','','El_Pipe_simple_vert_gr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','46',32,'','','El_Pipe_simple_vert_gr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.2',32,'','','El_Pipe_simple_vert_gr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.4',32,'','','El_Pipe_simple_vert_gr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','294',32,'','','El_Pipe_simple_vert_gr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','301',32,'','','El_Pipe_simple_vert_gr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.35',32,'','','El_Pipe_simple_vert_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','38',32,'','','El_Pipe_simple_vert_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.2',32,'','','El_Pipe_simple_vert_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','301',32,'','','El_Pipe_simple_vert_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','35',32,'','','El_Pipe_simple_vert_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','50',32,'','','El_Pipe_simple_vert_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','2.35',32,'','','El_Pipe_simple_vert_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.2',32,'','','El_Pipe_simple_vert_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','107',32,'','','El_Pipe_simple_vert_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','101',32,'','','El_Pipe_simple_vert_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','text','KPP101',32,'','','txtKPP101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomH','20',32,'','','txtKPP101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','79',32,'','','txtKPP101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','font','Arial 12 0 0 0 0',32,'','','txtKPP101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomW','43',32,'','','txtKPP101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','385',32,'','','txtKPP101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','55',32,'','','txtKPP101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','52',32,'','','El_Pipe_simple_vert_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.2',32,'','','El_Pipe_simple_vert_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.35',32,'','','El_Pipe_simple_vert_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','349',32,'','','ElFigure2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','elLst','line:1:2::::
line:3:4::::
',32,'','','ElFigure1','line:1:2::::
line:3:4::::
','','line:1:2::::
line:3:4::::
','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','p1y','5',0,'','','ElFigure1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','p1x','6',0,'','','ElFigure1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomH','100',32,'','','ElFigure1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','64',32,'','','ElFigure1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.75',32,'','','ElFigure1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','298',32,'','','El_Pipe_simple_vert_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','67',32,'','','N_CM','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','521',32,'','','El_Pipe_simple_vert_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','129',32,'','','N_CM','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','181',32,'','','N_CM','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','name','N CM',32,'','','N_CM','N КМ','','N КМ','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','21',32,'','','dr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.3',32,'','','dr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.3',32,'','','dr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','293',32,'','','dr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','146',32,'','','dr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','fillColor','blue',32,'','','El_Strela_gr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','55',32,'','','El_Strela_gr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.2',32,'','','El_Strela_gr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.2',32,'','','El_Strela_gr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.2',32,'','','El_Pipe_simple_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','33',32,'','','El_Pipe_simple_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','25',32,'','','El_Pipe_simple_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','331',32,'','','El_Pipe_simple_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.26',32,'','','El_Pipe_simple_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.2',32,'','','El_Pipe_simple_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','15',32,'','','El_Pipe_simple_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.2',32,'','','El_StrelaVB_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','121',32,'','','El_StrelaVB_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.2',32,'','','El_StrelaVB_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','452',32,'','','El_StrelaVB_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','158',32,'','','El_StrelaVB_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','126',32,'','','El_Pipe_simple_gr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','151',32,'','','El_Pipe_simple_gr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','1.72',32,'','','El_Pipe_simple_gr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.2',32,'','','El_Pipe_simple_gr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','19',32,'','','El_Pipe_simple_gr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','126',32,'','','El_Pipe_simple_gr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','85',32,'','','El_Pipe_simple_gr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','2.7',32,'','','El_Pipe_simple_gr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.2',32,'','','El_Pipe_simple_gr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','24',32,'','','El_Pipe_simple_gr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','name','F PP101',32,'','','F_PP101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','129',32,'','','F_PP101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','256',32,'','','F_PP101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomW','102',32,'','','F_PP101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','98',32,'','','F_PP101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','321',32,'','','El_Pipe_simple_gr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','151',32,'','','El_Pipe_simple_gr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','1.76',32,'','','El_Pipe_simple_gr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.2',32,'','','El_Pipe_simple_gr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','18',32,'','','El_Pipe_simple_gr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','name','F PP102',32,'','','F_PP102','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','411',32,'','','F_PP102','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','209',32,'','','F_PP102','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomW','102',32,'','','F_PP102','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','103',32,'','','F_PP102','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','521',32,'','','El_Pipe_simple_gr7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','151',32,'','','El_Pipe_simple_gr7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomW','100',32,'','','El_Pipe_simple_gr7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','1.91',32,'','','El_Pipe_simple_gr7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.2',32,'','','El_Pipe_simple_gr7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','19',32,'','','El_Pipe_simple_gr7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','420',32,'','','El_Pipe_simple_gr8','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','85',32,'','','El_Pipe_simple_gr8','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomW','100',32,'','','El_Pipe_simple_gr8','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','3.3',32,'','','El_Pipe_simple_gr8','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.2',32,'','','El_Pipe_simple_gr8','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','17',32,'','','El_Pipe_simple_gr8','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','265',32,'','','txtCM101_1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','261',32,'','','txtCM101_1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomW','54',32,'','','txtCM101_1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomH','20',32,'','','txtCM101_1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','115',32,'','','txtCM101_1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','font','Arial 12 0 0 0 0',32,'','','txtCM101_1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','text','CM101/1',32,'','','txtCM101_1','КМ101/1','','КМ101/1','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','95',32,'','','El_Pipe_simple_gr9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','331',32,'','','El_Pipe_simple_gr9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','1.7',32,'','','El_Pipe_simple_gr9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.2',32,'','','El_Pipe_simple_gr9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','26',32,'','','El_Pipe_simple_gr9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','525',32,'','','txtCM101_2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','261',32,'','','txtCM101_2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomW','54',32,'','','txtCM101_2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomH','20',32,'','','txtCM101_2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','117',32,'','','txtCM101_2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','font','Arial 12 0 0 0 0',32,'','','txtCM101_2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','text','CM101/2',32,'','','txtCM101_2','КМ101/2','','КМ101/2','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','name','KPP101',32,'','','KPP101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','391',32,'','','KPP101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','73',32,'','','KPP101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.3',32,'','','KPP101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.3',32,'','','KPP101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','83',32,'','','KPP101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','177',32,'','','Line_simple1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','352',32,'','','Line_simple1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomW','128.33',32,'','','Line_simple1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomH','291.11',32,'','','Line_simple1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.2',32,'','','Line_simple1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.3',32,'','','Line_simple1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','54',32,'','','Line_simple1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','elLst','line:(6.67|26):(105|26):::::
line:(105|26):(105|253):::::
line:(40|253):(105|253):::::
',32,'','','Line_simple1','line:(6.67|26):(105|26):::::
line:(105|26):(105|253):::::
line:(40|253):(105|253):::::
','','line:(6.67|26):(105|26):::::
line:(105|26):(105|253):::::
line:(40|253):(105|253):::::
','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','442',32,'','','Line_simple2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','352',32,'','','Line_simple2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomW','128.33',32,'','','Line_simple2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomH','291.11',32,'','','Line_simple2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.2',32,'','','Line_simple2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.3',32,'','','Line_simple2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','56',32,'','','Line_simple2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','elLst','line:(6.67|26):(105|26):::::
line:(105|26):(105|253):::::
line:(40|253):(105|253):::::
',32,'','','Line_simple2','line:(6.67|26):(105|26):::::
line:(105|26):(105|253):::::
line:(40|253):(105|253):::::
','','line:(6.67|26):(105|26):::::
line:(105|26):(105|253):::::
line:(40|253):(105|253):::::
','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','682',32,'','','Line_simple3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','352',32,'','','Line_simple3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomW','128.33',32,'','','Line_simple3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomH','291.11',32,'','','Line_simple3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.2',32,'','','Line_simple3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.3',32,'','','Line_simple3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','57',32,'','','Line_simple3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','elLst','line:(6.67|26):(105|26):::::
line:(105|26):(105|253):::::
line:(40|253):(105|253):::::
',32,'','','Line_simple3','line:(6.67|26):(105|26):::::
line:(105|26):(105|253):::::
line:(40|253):(105|253):::::
','','line:(6.67|26):(105|26):::::
line:(105|26):(105|253):::::
line:(40|253):(105|253):::::
','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','851',32,'','','El_Strela_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','85',32,'','','El_Strela_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.2',32,'','','El_Strela_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.2',32,'','','El_Strela_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','30',32,'','','El_Strela_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.2',32,'','','El_TroinikHT_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.2',32,'','','El_TroinikHT_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','50',32,'','','El_TroinikHT_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','331',32,'','','El_TroinikHT_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','833',32,'','','El_Strela_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','330',32,'','','El_Strela_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.2',32,'','','El_Strela_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.2',32,'','','El_Strela_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','32',32,'','','El_Strela_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','125',32,'','','El_TroinikHB_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.2',32,'','','El_TroinikHB_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','85',32,'','','El_TroinikHB_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.2',32,'','','El_TroinikHB_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','709',32,'','','El_TroinikHB_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.2',32,'','','El_TroinikHB_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.2',32,'','','El_TroinikHB_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','85',32,'','','El_TroinikHB_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','774',32,'','','El_TroinikHB_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','827',32,'','','El_Strela_gr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','499',32,'','','El_Strela_gr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.15',32,'','','El_Pipe_simple_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','95',32,'','','El_Pipe_simple_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','282',32,'','','El_Pipe_simple_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','28',32,'','','El_Pipe_simple_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.2',32,'','','El_Pipe_simple_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','2.07',32,'','','El_Pipe_simple_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','331',32,'','','El_Pipe_simple_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','320',32,'','','El_Pipe_simple_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomW','900',32,'','','','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomH','580',32,'','','','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','backColor','#5A5A5A',96,'backColorFrame','','','','backColorFrame','','backColorFrame','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','bordWidth','1',32,'','','','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','bordColor','black',32,'','','','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','657',32,'','','ElFigure6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','44',32,'','','El_Pipe_simple_gr16','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_Pipe_simple_gr16','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','2.12',32,'','','El_Pipe_simple_gr16','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','font','Arial 14',32,'','','Text12','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','78',32,'','','Text12','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomH','20',32,'','','Text12','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomW','40',32,'','','Text12','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','170',32,'','','Text12','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p1y','5',0,'','','ElFigure9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p1x','20',0,'','','ElFigure9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','elLst','line:8:2::::
line:3:4::::
line:1:5::::
line:6:7::::
',32,'','','ElFigure9','line:8:2::::
line:3:4::::
line:1:5::::
line:6:7::::
','','line:8:2::::
line:3:4::::
line:1:5::::
line:6:7::::
','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','orient','180',32,'','','ElFigure9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','71',32,'','','ElFigure9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.7',32,'','','ElFigure9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomH','100',32,'','','ElFigure9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p8x','28',0,'','','ElFigure7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p8y','77',0,'','','ElFigure7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p9x','44',0,'','','ElFigure7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p9y','77',0,'','','ElFigure7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','24',32,'','','ElFigure9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','216',32,'','','ElFigure9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomW','30',32,'','','ElFigure9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p7y','77',0,'','','ElFigure7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p7x','35',0,'','','ElFigure7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p5y','52',0,'','','ElFigure7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p5x','12',0,'','','ElFigure7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p1x','7',0,'','','ElFigure7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p1y','1',0,'','','ElFigure7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p2x','35',0,'','','ElFigure7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p2y','1',0,'','','ElFigure7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p3x','2',0,'','','ElFigure7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p3y','52',0,'','','ElFigure7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p4x','7',0,'','','ElFigure7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p4y','62',0,'','','ElFigure7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','elLst','line:1:4:::::
line:3:4:::::
line:4:5:::::
line:3:5:::::
line:2:7:::::
line:8:9:::::
fill:4:3:5::
',32,'','','ElFigure7','line:1:4:::::
line:3:4:::::
line:4:5:::::
line:3:5:::::
line:2:7:::::
line:8:9:::::
fill:4:3:5::
','','line:1:4:::::
line:3:4:::::
line:4:5:::::
line:3:5:::::
line:2:7:::::
line:8:9:::::
fill:4:3:5::
','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','73',32,'','','ElFigure7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','45',32,'','','El_Pipe_simple_gr17','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','197',32,'','','ElFigure7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','123',32,'','','ElFigure7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomW','44',32,'','','ElFigure7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomH','81',32,'','','ElFigure7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_Pipe_simple_gr17','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.3',32,'','','El_Pipe_simple_gr17','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','318',32,'','','El_Pipe_simple_gr17','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p3x','1',0,'','','ElFigure6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p3y','94.23',0,'','','ElFigure6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p4x','12',0,'','','ElFigure6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p4y','94.23',0,'','','ElFigure6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','800',32,'','','El_Pipe_simple_gr17','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p2y','95',0,'','','ElFigure6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p1y','5',0,'','','ElFigure6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p2x','6',0,'','','ElFigure6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p1x','6',0,'','','ElFigure6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','elLst','line:1:2::::
line:3:4::::
',32,'','','ElFigure6','line:1:2::::
line:3:4::::
','','line:1:2::::
line:3:4::::
','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','64',32,'','','ElFigure6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.55',32,'','','ElFigure6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomH','100',32,'','','ElFigure6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomW','16',32,'','','ElFigure6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','260',32,'','','ElFigure6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','145',32,'','','El_TroinikHT_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','593',32,'','','El_Pipe_simple_gr16','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','118',32,'','','El_Pipe_simple_gr16','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p9y','50',0,'','','ElFigure5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p9x','19',0,'','','ElFigure5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p8y','50',0,'','','ElFigure5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p2x','9',0,'','','ElFigure5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p1y','5',0,'','','ElFigure5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p1x','38',0,'','','ElFigure5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','elLst','line:1:4::::
line:3:4::::
line:4:5::::
line:3:5::::
line:2:8::::
line:7:9::::
fill:4:3:5:
',32,'','','ElFigure5','line:1:4::::
line:3:4::::
line:4:5::::
line:3:5::::
line:2:8::::
line:7:9::::
fill:4:3:5:
','','line:1:4::::
line:3:4::::
line:4:5::::
line:3:5::::
line:2:8::::
line:7:9::::
fill:4:3:5:
','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','72',32,'','','ElFigure5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomH','54',32,'','','ElFigure5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_Pipe_simple_gr15','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','40',32,'','','El_Pipe_simple_gr15','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','542',32,'','','ElFigure5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','70',32,'','','ElFigure5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomW','44',32,'','','ElFigure5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomW','44',32,'','','ElFigure3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_Pipe_simple_gr13','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.17',32,'','','El_Pipe_simple_gr13','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','258',32,'','','El_Pipe_simple_gr13','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','400',32,'','','El_Pipe_simple_gr13','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p4x','12',0,'','','ElFigure2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p4y','5',0,'','','ElFigure2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p3y','5',0,'','','ElFigure2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p3x','1',0,'','','ElFigure2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','fillColor','blue',32,'','','El_TroinikHT_gr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','129',32,'','','El_TroinikHT_gr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','269',32,'','','ElFigure3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','805',32,'','','ElFigure3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p2y','95',0,'','','ElFigure2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p1y','5',0,'','','ElFigure2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p2x','6',0,'','','ElFigure2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p1x','6',0,'','','ElFigure2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','661',32,'','','El_TroinikHT_gr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','499',32,'','','El_TroinikHT_gr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','elLst','line:1:2::::
line:3:4::::
',32,'','','ElFigure2','line:1:2::::
line:3:4::::
','','line:1:2::::
line:3:4::::
','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','59',32,'','','ElFigure2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.8',32,'','','ElFigure2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomH','100',32,'','','ElFigure2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','64',32,'','','El_angleBR_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','149',32,'','','El_angleBR_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','50',32,'','','El_Pipe_simple_gr13','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomW','16',32,'','','ElFigure2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','444',32,'','','ElFigure2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','351',32,'','','ElFigure2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','51',32,'','','El_Pipe_simple_gr12','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p4y','5',0,'','','ElFigure1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','400',32,'','','El_Pipe_simple_gr12','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','323',32,'','','El_Pipe_simple_gr12','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.17',32,'','','El_Pipe_simple_gr12','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_Pipe_simple_gr12','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p4x','12',0,'','','ElFigure1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p3x','1',0,'','','ElFigure1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p3y','5',0,'','','ElFigure1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p2y','95',0,'','','ElFigure1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p2x','6',0,'','','ElFigure1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_angleBR_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_angleBR_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p1y','5',0,'','','ElFigure1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p1x','6',0,'','','ElFigure1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','elLst','line:1:2::::
line:3:4::::
',32,'','','ElFigure1','line:1:2::::
line:3:4::::
','','line:1:2::::
line:3:4::::
','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','58',32,'','','ElFigure1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomH','100',32,'','','ElFigure1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.7',32,'','','ElFigure1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','290',32,'','','ElFigure1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomW','16',32,'','','ElFigure1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','391',32,'','','El_Pipe_simple_gr11','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.17',32,'','','El_Pipe_simple_gr11','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_Pipe_simple_gr11','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','48',32,'','','El_Pipe_simple_gr11','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','289',32,'','','ElFigure1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','111',32,'','','P_PP5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','400',32,'','','El_Pipe_simple_gr11','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','201',32,'','','P_PP5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_Pipe_simple_gr10','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','49',32,'','','El_Pipe_simple_gr10','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','fillColor','blue',32,'','','El_Pipe_simple_gr10','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','582',32,'','','P_PP5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','529',32,'','','El_Pipe_simple_gr14','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p1x','38',0,'','','ElFigure3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p1y','5',0,'','','ElFigure3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p2x','9',0,'','','ElFigure3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p2y','4',0,'','','ElFigure3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p3x','33',0,'','','ElFigure3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p3y','27',0,'','','ElFigure3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p4x','38',0,'','','ElFigure3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p4y','37',0,'','','ElFigure3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p5x','43',0,'','','ElFigure3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p5y','27',0,'','','ElFigure3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p7x','2',0,'','','ElFigure3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p7y','50',0,'','','ElFigure3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p8x','9',0,'','','ElFigure3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p8y','50',0,'','','ElFigure3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p9x','17',0,'','','ElFigure3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p9y','50',0,'','','ElFigure3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','elLst','line:1:4::::
line:3:4::::
line:4:5::::
line:3:5::::
line:2:8::::
line:7:9::::
fill:4:3:5:
',32,'','','ElFigure3','line:1:4::::
line:3:4::::
line:4:5::::
line:3:5::::
line:2:8::::
line:7:9::::
fill:4:3:5:
','','line:1:4::::
line:3:4::::
line:4:5::::
line:3:5::::
line:2:8::::
line:7:9::::
fill:4:3:5:
','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.2',32,'','','El_TroinikVL_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','107',32,'','','El_TroinikVL_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','282',32,'','','El_TroinikVL_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','414',32,'','','El_angleTL_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','391',32,'','','El_angleTL_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_pipe_1_gr13','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','66',32,'','','El_pipe_1_gr13','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_angleTL_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_angleTL_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','75',32,'','','ElFigure3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomH','54',32,'','','ElFigure3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_pipe_1_gr13','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','258',32,'','','El_pipe_1_gr13','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','582',32,'','','F_PP5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','223',32,'','','F_PP5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','115',32,'','','F_PP5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','414',32,'','','El_pipe_1_gr13','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','119',32,'','','El_TroinikVL_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.2',32,'','','El_TroinikVL_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_angleBR_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','32',32,'','','El_angleBR_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','198',32,'','','El_TroinikHT_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_TroinikHT_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','223',32,'','','F_PP1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','375',32,'','','F_PP1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','118',32,'','','F_PP1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','413',32,'','','F_PP3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','450',32,'','','F_PP3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','120',32,'','','F_PP3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','661',32,'','','El_StrelaVB_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','452',32,'','','El_StrelaVB_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.2',32,'','','El_StrelaVB_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.2',32,'','','El_StrelaVB_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','123',32,'','','El_StrelaVB_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','fillColor','blue',32,'','','El_StrelaVB_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','9',32,'','','El_TroinikHT_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','107',32,'','','El_TroinikHT_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','331',32,'','','El_TroinikHT_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.2',32,'','','El_TroinikHT_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.2',32,'','','El_TroinikHT_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','92',32,'','','BC1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','92.5',32,'','','Text12','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.3',32,'','','BC1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.3',32,'','','BC1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomH','106.66',32,'','','BC1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','120',32,'','','El_TroinikHT_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','orient','180',32,'','','El_pipe_1_gr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p1x','1',0,'','','compr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p1y','1',0,'','','compr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p2x','1',0,'','','compr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p2y','50',0,'','','compr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p4x','45',0,'','','compr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p4y','41',0,'','','compr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p6x','45',0,'','','compr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p6y','12',0,'','','compr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.2',32,'','','El_TroinikHT_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.2',32,'','','El_TroinikHT_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','331',32,'','','El_TroinikHT_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','709',32,'','','El_TroinikHT_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','234',32,'','','El_pipe_1_gr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','492',32,'','','El_pipe_1_gr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_pipe_1_gr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_pipe_1_gr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','68',32,'','','El_pipe_1_gr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','fillColor','blue',32,'','','El_pipe_1_gr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','98',32,'','','BC1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','186',32,'','','BC1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','text','BC2',32,'','','Text11','КК2','','КШ2','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','font','Arial 14',32,'','','Text11','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','elLst','line:1:2::::
line:2:4::::
line:1:6::::
line:6:4::::
fill:1:2:4:6:#c0c0c0
',32,'','','compr6','line:1:2::::
line:2:4::::
line:1:6::::
line:6:4::::
fill:1:2:4:6:#c0c0c0
','','line:1:2:::::
line:2:4:::::
line:1:6:::::
line:6:4:::::
fill:1:2:4:6:#c0c0c0:
','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','fillColor','green',32,'','','compr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','135',32,'','','compr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','76',32,'','','Text11','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomH','20',32,'','','Text11','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomW','40',32,'','','Text11','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','122',32,'','','Text11','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','92.5',32,'','','Text11','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomH','55',32,'','','compr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomW','49',32,'','','compr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','344',32,'','','compr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','700',32,'','','compr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','text','CM202',32,'','','Text10','КМ202','','КМ202','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','137',32,'','','Text10','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomH','20',32,'','','Text10','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomW','42',32,'','','Text10','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','302',32,'','','Text10','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','65',32,'','','El_pipe_1_gr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_pipe_1_gr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','703',32,'','','Text10','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','text','Mainstation',32,'','','Title','Загальностанційка','','Общестанционка','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','color','white',96,'labColor','','Title','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p6y','50',0,'','','ElFigure9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p7x','10',0,'','','ElFigure9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p7y','50',0,'','','ElFigure9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p8x','20',0,'','','ElFigure9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p8y','5',0,'','','ElFigure9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','166',32,'','','PC1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','84',32,'','','PC1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','86',32,'','','PC1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','519',32,'','','PC2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','37',32,'','','PC2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','102',32,'','','PC2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','777',32,'','','PC3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','237',32,'','','PC3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','101',32,'','','PC3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','name','',32,'','','Title','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','5',32,'','','Title','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','5',32,'','','Title','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomW','218.75',32,'','','Title','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomH','30',32,'','','Title','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','1.6',32,'','','Title','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','1.56',32,'','','Title','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','88',32,'','','Title','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','font','Arial 20 0 1 1 0',32,'','','Title','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','1.2',32,'','','El_Pipe_simple_gr15','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','39',32,'','','El_Pipe_simple_gr14','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','339',32,'','','El_Pipe_simple_gr15','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','75',32,'','','El_Pipe_simple_gr15','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_Pipe_simple_gr14','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.4',32,'','','El_Pipe_simple_gr14','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p5y','5',0,'','','ElFigure9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p6x','20',0,'','','ElFigure9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p5x','10',0,'','','ElFigure9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p4y','99.29',0,'','','ElFigure9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p4x','25',0,'','','ElFigure9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','118',32,'','','El_Pipe_simple_gr14','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p3y','99.29',0,'','','ElFigure9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p3x','14',0,'','','ElFigure9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p2y','99.29',0,'','','ElFigure9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p2x','20',0,'','','ElFigure9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','413',32,'','','P_PP3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','492',32,'','','El_Pipe_simple_gr10','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','5.89',32,'','','El_Pipe_simple_gr10','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','254',32,'','','El_Pipe_simple_gr10','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','428',32,'','','P_PP3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','119',32,'','','P_PP3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','54',32,'','','El_Pipe_simple_gr8','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','127',32,'','','El_Pipe_simple_gr9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','198',32,'','','El_Pipe_simple_gr9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.65',32,'','','El_Pipe_simple_gr9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_Pipe_simple_gr9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','37',32,'','','El_Pipe_simple_gr9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','224',32,'','','El_round_square11','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','246',32,'','','El_round_square11','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.4',32,'','','El_round_square11','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.8',32,'','','El_round_square11','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','121',32,'','','El_round_square11','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','orient','90',32,'','','El_round_square11','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_TroinikHB_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','14',32,'','','El_TroinikHB_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','323',32,'','','El_TroinikHB_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_TroinikHB_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','560',32,'','','El_TroinikHB_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','800',32,'','','El_Strela_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','118',32,'','','El_Strela_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_Strela_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_Strela_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','139',32,'','','El_Strela_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','orient','0',32,'','','El_Strela_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','10',32,'','','El_TroinikHB_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_TroinikHB_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_TroinikHB_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','323',32,'','','El_TroinikHB_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','467',32,'','','El_TroinikHB_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','421',32,'','','El_StrelaVB_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','452',32,'','','El_StrelaVB_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.2',32,'','','El_StrelaVB_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.2',32,'','','El_StrelaVB_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','122',32,'','','El_StrelaVB_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','fillColor','blue',32,'','','El_StrelaVB_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','834',32,'','','El_Strela_gr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','492',32,'','','El_Strela_gr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_Strela_gr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_Strela_gr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','132',32,'','','El_Strela_gr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','fillColor','blue',32,'','','El_Strela_gr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','orient','0',32,'','','El_Strela_gr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','453',32,'','','El_Strela_gr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','75',32,'','','El_Strela_gr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_Strela_gr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_Strela_gr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','142',32,'','','El_Strela_gr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','857',32,'','','El_Strela_gr7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','318',32,'','','El_Strela_gr7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_Strela_gr7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_Strela_gr7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','140',32,'','','El_Strela_gr7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','189',32,'','','PCV1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','186',32,'','','PCV1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.3',32,'','','PCV1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.3',32,'','','PCV1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','93',32,'','','PCV1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','c1','#d3d3d3',32,'','','PCV1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','64',32,'','','El_Pipe_simple_vert_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','168',32,'','','El_Pipe_simple_vert_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_Pipe_simple_vert_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.32',32,'','','El_Pipe_simple_vert_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','56',32,'','','El_Pipe_simple_vert_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','566',32,'','','PCV2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','106',32,'','','PCV2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.3',32,'','','PCV2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.3',32,'','','PCV2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','99',32,'','','PCV2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','145',32,'','','El_Pipe_simple_vert_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','168',32,'','','El_Pipe_simple_vert_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_Pipe_simple_vert_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.32',32,'','','El_Pipe_simple_vert_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','63',32,'','','El_Pipe_simple_vert_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','828',32,'','','PCV3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','306',32,'','','PCV3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.3',32,'','','PCV3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.3',32,'','','PCV3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','141',32,'','','PCV3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','323',32,'','','El_Pipe_simple_vert_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','194',32,'','','El_Pipe_simple_vert_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_Pipe_simple_vert_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','2.1',32,'','','El_Pipe_simple_vert_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','24',32,'','','El_Pipe_simple_vert_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','414',32,'','','El_Pipe_simple_vert_gr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','276',32,'','','El_Pipe_simple_vert_gr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_Pipe_simple_vert_gr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','1.21',32,'','','El_Pipe_simple_vert_gr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','23',32,'','','El_Pipe_simple_vert_gr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','601',32,'','','El_Pipe_simple_vert_gr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','384',32,'','','El_Pipe_simple_vert_gr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_Pipe_simple_vert_gr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','1.1',32,'','','El_Pipe_simple_vert_gr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','74',32,'','','El_Pipe_simple_vert_gr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','fillColor','blue',32,'','','El_Pipe_simple_vert_gr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','223',32,'','','T_PP1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','397',32,'','','T_PP1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','113',32,'','','T_PP1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','356',32,'','','Text1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','243',32,'','','Text1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomW','43',32,'','','Text1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomH','20',32,'','','Text1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','126',32,'','','Text1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','text','CM101',32,'','','Text1','КМ101','','КМ101','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','514',32,'','','El_Pipe_simple_vert_gr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','137',32,'','','El_Pipe_simple_vert_gr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_Pipe_simple_vert_gr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','2.6',32,'','','El_Pipe_simple_vert_gr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','15',32,'','','El_Pipe_simple_vert_gr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','646.5',32,'','','Text2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','351',32,'','','Text2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomW','35',32,'','','Text2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomH','20.38',32,'','','Text2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','80',32,'','','Text2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','font','Arial 14',32,'','','Text2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','text','PP5',32,'','','Text2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','234',32,'','','El_Pipe_simple_vert_gr7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','320',32,'','','El_Pipe_simple_vert_gr7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_Pipe_simple_vert_gr7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','1.75',32,'','','El_Pipe_simple_vert_gr7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','70',32,'','','El_Pipe_simple_vert_gr7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','fillColor','blue',32,'','','El_Pipe_simple_vert_gr7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','413',32,'','','T_PP3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','472',32,'','','T_PP3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','116',32,'','','T_PP3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','766.5',32,'','','Text3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','292',32,'','','Text3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomW','35',32,'','','Text3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomH','20.38',32,'','','Text3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','82',32,'','','Text3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','font','Arial 14',32,'','','Text3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','text','PP7',32,'','','Text3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','323',32,'','','El_Pipe_simple_vert_gr8','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','92',32,'','','El_Pipe_simple_vert_gr8','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_Pipe_simple_vert_gr8','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.81',32,'','','El_Pipe_simple_vert_gr8','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','17',32,'','','El_Pipe_simple_vert_gr8','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','357',32,'','','Text4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','308',32,'','','Text4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomW','42',32,'','','Text4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomH','20',32,'','','Text4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','127',32,'','','Text4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','text','CM201',32,'','','Text4','КМ201','','КМ201','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','467',32,'','','El_Pipe_simple_vert_gr9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','341',32,'','','El_Pipe_simple_vert_gr9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_Pipe_simple_vert_gr9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.56',32,'','','El_Pipe_simple_vert_gr9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','21',32,'','','El_Pipe_simple_vert_gr9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','582',32,'','','T_PP5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','245',32,'','','T_PP5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','110',32,'','','T_PP5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','818.5',32,'','','Text5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','476',32,'','','Text5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomW','41',32,'','','Text5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomH','20',32,'','','Text5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','91',32,'','','Text5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','font','Arial 12 1',32,'','','Text5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','color','blue',32,'','','Text5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','text','Drop',32,'','','Text5','Злив','','Слив','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','230.5',32,'','','Text6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','271',32,'','','Text6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomW','27',32,'','','Text6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomH','20',32,'','','Text6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','122',32,'','','Text6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','font','Arial 15 1',32,'','','Text6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','text','C1',32,'','','Text6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','357',32,'','','Text7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','375',32,'','','Text7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomW','41',32,'','','Text7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomH','20',32,'','','Text7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','128',32,'','','Text7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','text','CM301',32,'','','Text7','КМ301','','КМ301','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','181',32,'','','Text8','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomW','73',32,'','','Text8','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomH','20',32,'','','Text8','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','96',32,'','','Text8','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','font','Arial 17 1',32,'','','Text8','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','color','#B4B400',32,'','','Text8','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','text','Gas LP',32,'','','Text8','Газ НТ','','Газ НД','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','703',32,'','','Text9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','237',32,'','','Text9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomW','41',32,'','','Text9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomH','20',32,'','','Text9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','136',32,'','','Text9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','text','CM102',32,'','','Text9','КМ102','','КМ102','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','223',32,'','','P_PP1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','353',32,'','','P_PP1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','112',32,'','','P_PP1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.15',32,'','','El_Pipe_simple_gr8','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_Pipe_simple_gr8','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','339',32,'','','El_Pipe_simple_gr8','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','397',32,'','','El_Pipe_simple_gr8','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','53',32,'','','El_Pipe_simple_gr7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_Pipe_simple_gr7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.15',32,'','','El_Pipe_simple_gr7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','327',32,'','','El_Pipe_simple_gr7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','27',32,'','','El_Pipe_simple_gr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','339',32,'','','El_Pipe_simple_gr7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.95',32,'','','El_Pipe_simple_gr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_Pipe_simple_gr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','262',32,'','','El_Pipe_simple_gr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','260',32,'','','El_Pipe_simple_gr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','61',32,'','','El_Pipe_simple_gr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_Pipe_simple_gr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_Pipe_simple_gr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','198',32,'','','El_Pipe_simple_gr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_Pipe_simple_gr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','43',32,'','','El_Pipe_simple_gr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','218',32,'','','El_Pipe_simple_gr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.7',32,'','','El_Pipe_simple_gr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','323',32,'','','El_Pipe_simple_gr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','579',32,'','','El_Pipe_simple_gr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','601',32,'','','El_StrelaVB_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','430',32,'','','El_StrelaVB_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_StrelaVB_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_StrelaVB_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','152',32,'','','El_StrelaVB_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','52',32,'','','El_Pipe_simple_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_Pipe_simple_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_Pipe_simple_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','149',32,'','','El_Pipe_simple_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','127',32,'','','El_Pipe_simple_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.7',32,'','','El_Pipe_simple_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_Pipe_simple_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','30',32,'','','El_Pipe_simple_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','198',32,'','','El_Pipe_simple_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','33',32,'','','El_Pipe_simple_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','33',32,'','','El_Pipe_simple_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_Pipe_simple_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','149',32,'','','El_Pipe_simple_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_Pipe_simple_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','orient','90',32,'','','El_round_square1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','82',32,'','','El_Pipe_simple_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','130',32,'','','El_round_square1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.8',32,'','','El_round_square1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.4',32,'','','El_round_square1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','309',32,'','','El_round_square1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','591',32,'','','El_round_square1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','fillColor','blue',32,'','','El_TroinikHT_gr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','118',32,'','','El_angleBR_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_angleBR_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','514',32,'','','El_angleBR_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','13',32,'','','El_angleBR_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','323',32,'','','El_angleBR_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','75',32,'','','El_angleBR_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_angleBR_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_angleBR_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','709',32,'','','El_TroinikVL_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','151',32,'','','El_TroinikVL_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.2',32,'','','El_TroinikVL_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.2',32,'','','El_TroinikVL_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','124',32,'','','El_TroinikVL_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_pipe_1_gr26','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_pipe_1_gr26','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','252',32,'','','El_pipe_1_gr26','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','746',32,'','','El_pipe_1_gr26','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','126',32,'','','El_TroinikHT_gr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','421',32,'','','El_TroinikHT_gr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','499',32,'','','El_TroinikHT_gr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.2',32,'','','El_TroinikHT_gr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.2',32,'','','El_TroinikHT_gr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','67',32,'','','El_pipe_1_gr26','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p8x','9',0,'','','ElFigure5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p7y','50',0,'','','ElFigure5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p7x','2',0,'','','ElFigure5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p5y','27',0,'','','ElFigure5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p5x','42.5',0,'','','ElFigure5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p4y','37',0,'','','ElFigure5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_angleBR_gr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','151',32,'','','El_angleBR_gr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p4x','38',0,'','','ElFigure5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p3y','27',0,'','','ElFigure5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p3x','33',0,'','','ElFigure5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','orient','0',32,'','','El_Troinik_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','60',32,'','','El_Troinik_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','323',32,'','','El_Troinik_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','327',32,'','','El_Troinik_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_Troinik_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_Troinik_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_pipe_1_gr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','198',32,'','','El_pipe_1_gr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','234',32,'','','El_pipe_1_gr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','700',32,'','','compr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','276',32,'','','compr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomW','49',32,'','','compr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomH','55',32,'','','compr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','134',32,'','','compr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','fillColor','green',32,'','','compr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','elLst','line:1:2::::
line:2:4::::
line:1:6::::
line:6:4::::
fill:1:2:4:6:#c0c0c0
',32,'','','compr5','line:1:2::::
line:2:4::::
line:1:6::::
line:6:4::::
fill:1:2:4:6:#c0c0c0
','','line:1:2:::::
line:2:4:::::
line:1:6:::::
line:6:4:::::
fill:1:2:4:6:#c0c0c0:
','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p1x','1',0,'','','compr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p1y','1',0,'','','compr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p2x','1',0,'','','compr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p2y','50',0,'','','compr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p4x','45',0,'','','compr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p4y','41',0,'','','compr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p6x','45',0,'','','compr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p6y','12',0,'','','compr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','127',32,'','','El_TroinikHT_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','772',32,'','','El_TroinikHT_gr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p2y','50',0,'','','compr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p4x','45',0,'','','compr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p4y','41',0,'','','compr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p6x','45',0,'','','compr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p6y','12',0,'','','compr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','746',32,'','','El_Pipe_simple_vert_gr13','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','271',32,'','','El_Pipe_simple_vert_gr13','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_Pipe_simple_vert_gr13','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','1.15',32,'','','El_Pipe_simple_vert_gr13','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','16',32,'','','El_Pipe_simple_vert_gr13','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.2',32,'','','El_TroinikHT_gr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','330',32,'','','El_TroinikHT_gr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.2',32,'','','El_TroinikHT_gr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p2x','1',0,'','','compr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p1x','1',0,'','','compr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p1y','1',0,'','','compr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','elLst','line:1:2::::
line:2:4::::
line:1:6::::
line:6:4::::
fill:1:2:4:6:#c0c0c0
',32,'','','compr4','line:1:2::::
line:2:4::::
line:1:6::::
line:6:4::::
fill:1:2:4:6:#c0c0c0
','','line:1:2::::
line:2:4::::
line:1:6::::
line:6:4::::
fill:1:2:4:6:#c0c0c0
','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','5',32,'','','El_angleBR_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','700',32,'','','compr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','211',32,'','','compr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomW','49',32,'','','compr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomH','55',32,'','','compr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','133',32,'','','compr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','fillColor','green',32,'','','compr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','57',32,'','','El_pipe_1_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_pipe_1_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_pipe_1_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','149',32,'','','El_pipe_1_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','129',32,'','','El_TroinikHT_gr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.2',32,'','','El_TroinikHT_gr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.2',32,'','','El_TroinikHT_gr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','145',32,'','','El_pipe_1_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','69',32,'','','El_Pipe_simple_vert_gr12','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.35',32,'','','El_Pipe_simple_vert_gr12','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','217',32,'','','El_Pipe_simple_vert_gr12','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','234',32,'','','El_Pipe_simple_vert_gr12','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p6y','12',0,'','','compr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p6x','45',0,'','','compr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p4y','41',0,'','','compr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p2y','50',0,'','','compr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p4x','45',0,'','','compr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p2x','1',0,'','','compr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p1y','1',0,'','','compr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p4y','41',0,'','','compr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p6x','45',0,'','','compr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p6y','12',0,'','','compr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','560',32,'','','El_Pipe_simple_vert_gr11','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','341',32,'','','El_Pipe_simple_vert_gr11','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_Pipe_simple_vert_gr11','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.56',32,'','','El_Pipe_simple_vert_gr11','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','19',32,'','','El_Pipe_simple_vert_gr11','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','354',32,'','','compr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','349',32,'','','compr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomW','49',32,'','','compr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomH','55',32,'','','compr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','125',32,'','','compr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','fillColor','green',32,'','','compr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','elLst','line:1:2::::
line:2:4::::
line:1:6::::
line:6:4::::
fill:1:2:4:6:#c0c0c0
',32,'','','compr3','line:1:2::::
line:2:4::::
line:1:6::::
line:6:4::::
fill:1:2:4:6:#c0c0c0
','','line:1:2::::
line:2:4::::
line:1:6::::
line:6:4::::
fill:1:2:4:6:#c0c0c0
','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p1x','1',0,'','','compr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p4x','45',0,'','','compr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p2x','1',0,'','','compr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p2y','50',0,'','','compr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p1y','1',0,'','','compr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p1x','1',0,'','','compr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','fillColor','green',32,'','','compr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','elLst','line:1:2::::
line:2:4::::
line:1:6::::
line:6:4::::
fill:1:2:4:6:#c0c0c0
',32,'','','compr2','line:1:2::::
line:2:4::::
line:1:6::::
line:6:4::::
fill:1:2:4:6:#c0c0c0
','','line:1:2::::
line:2:4::::
line:1:6::::
line:6:4::::
fill:1:2:4:6:#c0c0c0
','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','681',32,'','','El_angleBR_gr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','259',32,'','','El_angleBR_gr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_angleBR_gr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_Pipe_simple_vert_gr12','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','124',32,'','','compr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomW','49',32,'','','compr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomH','55',32,'','','compr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','281',32,'','','compr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','354',32,'','','compr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','text','BC1',32,'','','Text12','КК1','','КШ1','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','98',32,'','','BC2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','137',32,'','','BC2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.3',32,'','','BC2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.3',32,'','','BC2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','89',32,'','','BC2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','597.5',32,'','','Text13','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','332',32,'','','Text13','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomW','27',32,'','','Text13','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomH','20',32,'','','Text13','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','131',32,'','','Text13','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','font','Arial 15 1',32,'','','Text13','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','text','C2',32,'','','Text13','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','290.5',32,'','','Text14','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','152',32,'','','Text14','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomW','35',32,'','','Text14','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomH','20',32,'','','Text14','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','106',32,'','','Text14','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','font','Arial 14',32,'','','Text14','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','text','BC5',32,'','','Text14','КК5','','КШ5','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','482.5',32,'','','Text20','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','364',32,'','','Text20','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomW','35',32,'','','Text20','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomH','20',32,'','','Text20','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','84',32,'','','Text20','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','font','Arial 13',32,'','','Text20','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','text','BC7',32,'','','Text20','КК7','','КШ7','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','698',32,'','','Text15','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','194',32,'','','Text15','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomW','54',32,'','','Text15','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomH','20',32,'','','Text15','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','109',32,'','','Text15','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','font','Arial 13 1',32,'','','Text15','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','color','',32,'','','Text15','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','text','CS HP',32,'','','Text15','КС ВТ','','КУ ВД','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','311',32,'','','BC5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','168',32,'','','BC5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomH','100',32,'','','BC5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.3',32,'','','BC5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.3',32,'','','BC5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','98',32,'','','BC5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','orient','-90',32,'','','BC5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','401.5',32,'','','Text21','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','61',32,'','','Text21','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomW','73',32,'','','Text21','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomH','20',32,'','','Text21','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','90',32,'','','Text21','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','font','Arial 12 1',32,'','','Text21','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','color','#B4B400',32,'','','Text21','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','text','To torch',32,'','','Text21','На факел','','На факел','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','279.5',32,'','','Text16','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','236',32,'','','Text16','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomW','35',32,'','','Text16','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomH','20',32,'','','Text16','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','77',32,'','','Text16','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','font','Arial 14',32,'','','Text16','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','text','PP1',32,'','','Text16','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','485',32,'','','BC6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','311',32,'','','BC6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomH','106.66',32,'','','BC6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.3',32,'','','BC6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.3',32,'','','BC6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','114',32,'','','BC6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','725.5',32,'','','Text22','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','105',32,'','','Text22','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomW','103',32,'','','Text22','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomH','20',32,'','','Text22','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','94',32,'','','Text22','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','font','Arial 12 1',32,'','','Text22','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','color','#B4B400',32,'','','Text22','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','text','Gas to Glinsk',32,'','','Text22','Газ на Глинськ','','Газ на Глинск','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','432.5',32,'','','Text17','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','298',32,'','','Text17','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomW','35',32,'','','Text17','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomH','20.38',32,'','','Text17','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','79',32,'','','Text17','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','font','Arial 14',32,'','','Text17','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','text','PP3',32,'','','Text17','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','485',32,'','','BC7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','381',32,'','','BC7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomH','106.66',32,'','','BC7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.3',32,'','','BC7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.3',32,'','','BC7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','104',32,'','','BC7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','703',32,'','','Text23','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','369',32,'','','Text23','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomW','41',32,'','','Text23','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomH','20',32,'','','Text23','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','138',32,'','','Text23','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','text','CM302',32,'','','Text23','КМ302','','КМ302','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','353',32,'','','Text18','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','193',32,'','','Text18','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomW','54',32,'','','Text18','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomH','20',32,'','','Text18','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','107',32,'','','Text18','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','font','Arial 13 1',32,'','','Text18','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','color','',32,'','','Text18','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','text','CS LP',32,'','','Text18','КС НТ','','КУ НД','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','477.5',32,'','','Text24','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','295',32,'','','Text24','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomW','40',32,'','','Text24','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomH','20',32,'','','Text24','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','83',32,'','','Text24','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','font','Arial 14',32,'','','Text24','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','text','BC6',32,'','','Text24','КК6','','КШ6','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','524.5',32,'','','Text19','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','295',32,'','','Text19','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomW','53',32,'','','Text19','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomH','20',32,'','','Text19','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','81',32,'','','Text19','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','font','Arial 14',32,'','','Text19','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','text','BC21',32,'','','Text19','КК21','','КШ21','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','346',32,'','','Box1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','189',32,'','','Box1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomW','67',32,'','','Box1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomH','232',32,'','','Box1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','26',32,'','','Box1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','backColor','',32,'','','Box1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','bordWidth','1',32,'','','Box1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','bordColor','#5A5A5A',32,'','','Box1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','bordStyle','8',32,'','','Box1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','527.5',32,'','','Text25','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','364',32,'','','Text25','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomW','41',32,'','','Text25','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomH','20',32,'','','Text25','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','129',32,'','','Text25','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','font','Arial 13',32,'','','Text25','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','text','BC22',32,'','','Text25','КК22','','КШ22','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','683',32,'','','Box2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','190',32,'','','Box2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomW','82',32,'','','Box2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomH','232',32,'','','Box2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','0',32,'','','Box2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','backColor','',32,'','','Box2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','bordWidth','1',32,'','','Box2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','bordColor','#5A5A5A',32,'','','Box2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','bordStyle','8',32,'','','Box2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','811.5',32,'','','Text26','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','334',32,'','','Text26','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomW','74',32,'','','Text26','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomH','19',32,'','','Text26','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','97',32,'','','Text26','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','font','Arial 12 1',32,'','','Text26','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','color','#B4B400',32,'','','Text26','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','text','Gas to GHP',32,'','','Text26','Газ у ГВТ','','Газ в ГВД','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','532',32,'','','BC21','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','311',32,'','','BC21','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomH','106.66',32,'','','BC21','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.3',32,'','','BC21','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.3',32,'','','BC21','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','117',32,'','','BC21','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','531',32,'','','BC22','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','381',32,'','','BC22','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomH','106.66',32,'','','BC22','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.3',32,'','','BC22','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.3',32,'','','BC22','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','105',32,'','','BC22','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','275',32,'','','El_d1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','253',32,'','','El_d1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.4',32,'','','El_d1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.4',32,'','','El_d1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','95',32,'','','El_d1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','430',32,'','','El_d2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','313',32,'','','El_d2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomH','101.92',32,'','','El_d2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.4',32,'','','El_d2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.4',32,'','','El_d2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','108',32,'','','El_d2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','643',32,'','','El_d3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','313',32,'','','El_d3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomH','101.92',32,'','','El_d3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.4',32,'','','El_d3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.4',32,'','','El_d3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','100',32,'','','El_d3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','763',32,'','','El_d4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','308',32,'','','El_d4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomW','102.5',32,'','','El_d4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomH','101.92',32,'','','El_d4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.4',32,'','','El_d4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.4',32,'','','El_d4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','103',32,'','','El_d4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','44',32,'','','Pi','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','230',32,'','','Pi','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','87',32,'','','Pi','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','514',32,'','','El_Krest_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','323',32,'','','El_Krest_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_Krest_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_Krest_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','46',32,'','','El_Krest_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','681',32,'','','El_Krest_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','324',32,'','','El_Krest_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_Krest_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_Krest_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','36',32,'','','El_Krest_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','323',32,'','','El_Krest_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','262',32,'','','El_Krest_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_Krest_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_Krest_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','31',32,'','','El_Krest_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','414',32,'','','El_Krest_gr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','323',32,'','','El_Krest_gr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_Krest_gr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_Krest_gr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','34',32,'','','El_Krest_gr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','44',32,'','','Ti','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','270',32,'','','Ti','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','85',32,'','','Ti','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','746',32,'','','El_Krest_gr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','317',32,'','','El_Krest_gr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_Krest_gr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_Krest_gr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','38',32,'','','El_Krest_gr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','354',32,'','','compr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','215',32,'','','compr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomW','49',32,'','','compr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomH','55',32,'','','compr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','123',32,'','','compr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','fillColor','green',32,'','','compr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','elLst','line:1:2::::
line:2:4::::
line:1:6::::
line:6:4::::
fill:1:2:4:6:#c0c0c0
',32,'','','compr1','line:1:2::::
line:2:4::::
line:1:6::::
line:6:4::::
fill:1:2:4:6:#c0c0c0
','','line:1:2::::
line:2:4::::
line:1:6::::
line:6:4::::
fill:1:2:4:6:#c0c0c0
','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p1x','1',0,'','','compr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p1y','1',0,'','','compr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p2x','1',0,'','','compr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p2y','50',0,'','','compr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p4x','45',0,'','','compr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p4y','41',0,'','','compr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p6x','45',0,'','','compr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p6y','12',0,'','','compr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','681',32,'','','El_Pipe_simple_vert_gr10','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','277',32,'','','El_Pipe_simple_vert_gr10','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_Pipe_simple_vert_gr10','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','1.15',32,'','','El_Pipe_simple_vert_gr10','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','7',32,'','','El_Pipe_simple_vert_gr10','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','772',32,'','','El_pipe_1_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','286',32,'','','El_pipe_1_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.2',32,'','','El_pipe_1_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.2',32,'','','El_pipe_1_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','40',32,'','','El_pipe_1_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','678',32,'','','txtKS123','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','443',32,'','','txtKS123','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomW','48',32,'','','txtKS123','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomH','20',32,'','','txtKS123','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','91',32,'','','txtKS123','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','font','Arial 12 0 0 0 0',32,'','','txtKS123','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','text','BC123',32,'','','txtKS123','КК123','','КШ123','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','421',32,'','','El_Pipe_simple_vert_gr12','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','371',32,'','','El_Pipe_simple_vert_gr12','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.2',32,'','','El_Pipe_simple_vert_gr12','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.45',32,'','','El_Pipe_simple_vert_gr12','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','44',32,'','','El_Pipe_simple_vert_gr12','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','fillColor','blue',32,'','','El_Pipe_simple_vert_gr12','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','fillColor','blue',32,'','','El_Pipe_simple_vert_gr11','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','43',32,'','','El_Pipe_simple_vert_gr11','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.6',32,'','','El_Pipe_simple_vert_gr11','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','442',32,'','','El_Pipe_simple_vert_gr11','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.2',32,'','','El_Pipe_simple_vert_gr11','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','158',32,'','','El_Pipe_simple_vert_gr11','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','text','BC122',32,'','','txtKS122','КК122','','КШ122','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','font','Arial 12 0 0 0 0',32,'','','txtKS122','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','87',32,'','','txtKS122','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','61',32,'','','El_Troinik_gr9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.2',32,'','','El_Troinik_gr7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','51',32,'','','El_Troinik_gr7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.2',32,'','','El_angleBR_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','11',32,'','','El_angleBR_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.2',32,'','','El_angleBR_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','282',32,'','','El_angleBR_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','50',32,'','','El_angleBR_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','709',32,'','','El_Troinik_gr9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','132',32,'','','El_Troinik_gr9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.2',32,'','','El_Troinik_gr9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.2',32,'','','El_Troinik_gr9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomW','48',32,'','','txtKS122','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomH','20',32,'','','txtKS122','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','443',32,'','','txtKS122','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','439',32,'','','txtKS122','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.5',32,'','','El_Pipe_simple_vert_gr10','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','42',32,'','','El_Pipe_simple_vert_gr10','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','fillColor','blue',32,'','','El_Pipe_simple_vert_gr10','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.2',32,'','','El_Pipe_simple_vert_gr10','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','370',32,'','','El_Pipe_simple_vert_gr10','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','158',32,'','','El_Pipe_simple_vert_gr10','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','text','BC121',32,'','','txtKS121','КК121','','КШ121','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','font','Arial 12 0 0 0 0',32,'','','txtKS121','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','85',32,'','','txtKS121','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomH','20',32,'','','txtKS121','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','text','BC103',32,'','','txtKS103','КК103','','КШ103','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','731',32,'','','txtKS104','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','303',32,'','','txtKS104','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomW','43',32,'','','txtKS104','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomH','20',32,'','','txtKS104','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','88',32,'','','txtKS104','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','font','Arial 12 0 0 0 0',32,'','','txtKS104','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','text','BC104',32,'','','txtKS104','КК104','','КШ104','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','731',32,'','','txtKS105','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','258',32,'','','txtKS105','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomW','48',32,'','','txtKS105','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomH','20',32,'','','txtKS105','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','89',32,'','','txtKS105','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','font','Arial 12 0 0 0 0',32,'','','txtKS105','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','text','BC105',32,'','','txtKS105','КК105','','КШ105','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','204',32,'','','PP101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','322',32,'','','PP101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.4',32,'','','PP101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.4',32,'','','PP101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','69',32,'','','PP101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','734',32,'','','txtKS107','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','105',32,'','','txtKS107','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomW','48',32,'','','txtKS107','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomH','20',32,'','','txtKS107','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','80',32,'','','txtKS107','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','font','Arial 12 0 0 0 0',32,'','','txtKS107','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','text','BC107',32,'','','txtKS107','КК107','','КШ107','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','475',32,'','','PP102','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','322',32,'','','PP102','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.4',32,'','','PP102','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.4',32,'','','PP102','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','72',32,'','','PP102','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','name','T CM101/1',32,'','','T_CM101_1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','274',32,'','','T_CM101_1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','396',32,'','','T_CM101_1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomW','102',32,'','','T_CM101_1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','94',32,'','','T_CM101_1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','name','T CM101/1',32,'','','T_CM101_2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','530',32,'','','T_CM101_2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','396',32,'','','T_CM101_2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomW','102',32,'','','T_CM101_2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','96',32,'','','T_CM101_2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','174',32,'','','txtKS121','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','441',32,'','','txtKS121','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomW','48',32,'','','txtKS121','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','78',32,'','','txtKS103','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','font','Arial 12 0 0 0 0',32,'','','txtKS103','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomH','20',32,'','','txtKS103','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomW','48',32,'','','txtKS103','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomW','80',32,'','','axis','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomH','22',32,'','','axis','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','4.66',32,'','','axis','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','41',32,'','','axis','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','lineWdth','7',32,'','','axis','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','elLst','line:(0|9):(80|9):::::
',32,'','','axis','line:(0|9):(80|9):::::
','','line:(0|9):(80|9):::::
','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','65',32,'','','txtKS101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','306',32,'','','txtKS101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomW','48',32,'','','txtKS101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomH','20',32,'','','txtKS101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','84',32,'','','txtKS101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','font','Arial 12 0 0 0 0',32,'','','txtKS101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','text','BC101',32,'','','txtKS101','КК101','','КШ101','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','62',32,'','','txtKS102','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','255',32,'','','txtKS102','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomW','48',32,'','','txtKS102','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomH','20',32,'','','txtKS102','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','95',32,'','','txtKS102','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','font','Arial 12 0 0 0 0',32,'','','txtKS102','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','text','BC102',32,'','','txtKS102','КК102','','КШ102','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','734',32,'','','txtKS103','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','57',32,'','','txtKS103','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','237',32,'','','axis','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','259',32,'','','axis','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','110',32,'','','C101_3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.7',32,'','','C101_3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','139',32,'','','KS106','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomH','103.34',32,'','','KS106','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.3',32,'','','KS106','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.3',32,'','','KS106','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','118',32,'','','KS106','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','name','BC121',32,'','','KS107','КК121','','КШ121','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','745',32,'','','KS107','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','120',32,'','','KS107','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomH','103.34',32,'','','KS107','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.3',32,'','','KS107','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.3',32,'','','KS107','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','76',32,'','','KS107','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','name','BC121',32,'','','KS121','КК121','','КШ121','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','159',32,'','','KS121','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','413.24',32,'','','KS121','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomH','103.34',32,'','','KS121','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.3',32,'','','KS121','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.3',32,'','','KS121','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','68',32,'','','KS121','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','orient','90',32,'','','KS121','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','name','BC122',32,'','','KS122','КК122','','КШ122','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','422',32,'','','KS122','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','413.24',32,'','','KS122','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomH','103.34',32,'','','KS122','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.3',32,'','','KS122','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.3',32,'','','KS122','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','70',32,'','','KS122','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','orient','90',32,'','','KS122','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','name','BC123',32,'','','KS123','КК123','','КШ123','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','662',32,'','','KS123','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','413.8',32,'','','KS123','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomH','103.34',32,'','','KS123','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.3',32,'','','KS123','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.3',32,'','','KS123','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','73',32,'','','KS123','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','orient','90',32,'','','KS123','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','5',32,'','','CM101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','5',32,'','','CM101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomW','150',32,'','','CM101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomH','30',32,'','','CM101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','1.6',32,'','','CM101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','1.56',32,'','','CM101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','71',32,'','','CM101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','font','Arial 20 0 1 1 0',32,'','','CM101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','color','white',96,'labColor','','CM101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','text','CM101',32,'','','CM101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','name','P PP101',32,'','','P_PP101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','129',32,'','','P_PP101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','234',32,'','','P_PP101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomW','102',32,'','','P_PP101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','97',32,'','','P_PP101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','name','P PP102',32,'','','P_PP102','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','411',32,'','','P_PP102','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','187',32,'','','P_PP102','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomW','102',32,'','','P_PP102','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','102',32,'','','P_PP102','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','name','C101/1',32,'','','C101_1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','146',32,'','','C101_1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','303',32,'','','C101_1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.57',32,'','','C101_1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.7',32,'','','C101_1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','108',32,'','','C101_1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','name','C101/1',32,'','','C101_2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','409',32,'','','C101_2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','303',32,'','','C101_2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.57',32,'','','C101_2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.7',32,'','','C101_2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','109',32,'','','C101_2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','name','C101/1',32,'','','C101_3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','649',32,'','','C101_3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','303',32,'','','C101_3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.57',32,'','','C101_3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','492',32,'','','KS106','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','name','BC121',32,'','','KS106','КК121','','КШ121','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','75',32,'','','KS105','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.3',32,'','','KS105','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomH','103.34',32,'','','KS105','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.3',32,'','','KS105','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','p2y','4',0,'','','ElFigure5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','320',32,'','','AT101_1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','338',32,'','','AT101_1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('comprEn','name','Compressor run',32,'','','','Робота компресора','','Работа компрессора','','','');
INSERT INTO wlb_AGLKS_io VALUES('comprEn','geomW','80',32,'','','','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('comprEn','geomH','80',32,'','','','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('comprEn','fillColor','lightgrey',32,'','','','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('comprEn','elLst','line:(14|8):(69|40):::::
line:(69|40):(14|72):::::
line:(14|72):(14|8):::::
fill:(14|8):(69|40):(14|72):c1:
',32,'','','','line:(14|8):(69|40):::::
line:(69|40):(14|72):::::
line:(14|72):(14|8):::::
fill:(14|8):(69|40):(14|72):c1:
','','line:(14|8):(69|40):::::
line:(69|40):(14|72):::::
line:(14|72):(14|8):::::
fill:(14|8):(69|40):(14|72):c1:
','','','');
INSERT INTO wlb_AGLKS_io VALUES('comprEn','c1','#ff0000',8,'','','','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','362',32,'','','comprEn101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','222',32,'','','comprEn101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.3',32,'','','comprEn101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.31',32,'','','comprEn101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','143',32,'','','comprEn101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','707',32,'','','comprEn102','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','217',32,'','','comprEn102','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.3',32,'','','comprEn102','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.31',32,'','','comprEn102','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','144',32,'','','comprEn102','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','362',32,'','','comprEn201','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','287',32,'','','comprEn201','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.3',32,'','','comprEn201','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.31',32,'','','comprEn201','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','145',32,'','','comprEn201','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','707',32,'','','comprEn202','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','282',32,'','','comprEn202','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.3',32,'','','comprEn202','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.31',32,'','','comprEn202','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','147',32,'','','comprEn202','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','362',32,'','','comprEn301','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','354',32,'','','comprEn301','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.3',32,'','','comprEn301','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.31',32,'','','comprEn301','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','146',32,'','','comprEn301','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','707',32,'','','comprEn302','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','349',32,'','','comprEn302','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.3',32,'','','comprEn302','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.31',32,'','','comprEn302','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','148',32,'','','comprEn302','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','1',32,'','','','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','1',32,'','','','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_TroinikHT_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','alignment','10',32,'','','Text13','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','alignment','10',32,'','','Text1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','alignment','10',32,'','','Text10','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','alignment','10',32,'','','Text11','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','alignment','10',32,'','','Text12','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','alignment','10',32,'','','Text14','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','alignment','10',32,'','','Text15','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','alignment','10',32,'','','Text16','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','alignment','10',32,'','','Text17','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','alignment','10',32,'','','Text18','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','alignment','10',32,'','','Text19','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','alignment','10',32,'','','Text2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','alignment','10',32,'','','Text20','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','alignment','10',32,'','','Text21','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','alignment','10',32,'','','Text22','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','alignment','10',32,'','','Text23','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','alignment','10',32,'','','Text24','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','alignment','10',32,'','','Text25','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','alignment','10',32,'','','Text26','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','alignment','10',32,'','','Text3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','alignment','10',32,'','','Text4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','alignment','10',32,'','','Text5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','alignment','10',32,'','','Text6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','alignment','10',32,'','','Text7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','alignment','10',32,'','','Text8','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','alignment','10',32,'','','Text9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','alignment','10',32,'','','Title','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','alignment','10',32,'','','CM101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','alignment','10',32,'','','Text1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','alignment','10',32,'','','Text2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','alignment','10',32,'','','Text3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','alignment','10',32,'','','Text4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','alignment','10',32,'','','Text5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','alignment','10',32,'','','Text6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','alignment','10',32,'','','Text7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','alignment','10',32,'','','Text8','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','alignment','10',32,'','','txtAT101_1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','alignment','10',32,'','','txtAT101_2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','alignment','10',32,'','','txtCM101_1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','alignment','10',32,'','','txtCM101_2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','alignment','10',32,'','','txtKPP101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','alignment','10',32,'','','txtKS101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','alignment','10',32,'','','txtKS102','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','alignment','10',32,'','','txtKS103','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','alignment','10',32,'','','txtKS104','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','alignment','10',32,'','','txtKS105','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','alignment','10',32,'','','txtKS107','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','alignment','10',32,'','','txtKS121','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','alignment','10',32,'','','txtKS122','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','alignment','10',32,'','','txtKS123','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','alignment','10',32,'','','txtPP101','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','alignment','10',32,'','','txtPP102','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','14',32,'','','El_Obr_StrelaHR_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','198',32,'','','El_Obr_StrelaHR_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_Obr_StrelaHR_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_Obr_StrelaHR_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','12',32,'','','El_Obr_StrelaHR_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','fillColor','blue',32,'','','El_StrelaVB_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','234',32,'','','El_StrelaVB_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','430',32,'','','El_StrelaVB_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_StrelaVB_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_StrelaVB_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','153',32,'','','El_StrelaVB_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','fillColor','blue',32,'','','El_StrelaVB_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomZ','1',32,'','','El_Obr_StrelaHR_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','fillColor','blue',32,'','','El_StrelaVB_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomYsc','0.2',32,'','','El_Obr_StrelaHR_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','64',32,'','','El_TroinikHT_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','198',32,'','','El_TroinikHT_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_TroinikHT_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_TroinikHT_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','149',32,'','','El_TroinikHT_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','601',32,'','','El_TroinikHT_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','492',32,'','','El_TroinikHT_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_TroinikHT_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_TroinikHT_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','150',32,'','','El_TroinikHT_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','514',32,'','','El_TroinikHT_gr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','393',32,'','','El_TroinikHT_gr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_TroinikHT_gr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_TroinikHT_gr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','4',32,'','','El_TroinikHT_gr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','9',32,'','','El_angleTL_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','353',32,'','','El_angleTL_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','397',32,'','','El_angleTL_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_angleTL_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_angleTL_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','20',32,'','','El_angleTL_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','353',32,'','','El_angleTL_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','327',32,'','','El_angleTL_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_angleTL_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_angleTL_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','29',32,'','','El_angleTL_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','353',32,'','','El_angleTL_gr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','262',32,'','','El_angleTL_gr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_angleTL_gr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_angleTL_gr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','42',32,'','','El_angleTL_gr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','560',32,'','','El_angleTL_gr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','393',32,'','','El_angleTL_gr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_angleTL_gr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_angleTL_gr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','22',32,'','','El_angleTL_gr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','700',32,'','','El_angleTL_gr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','259',32,'','','El_angleTL_gr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_angleTL_gr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_angleTL_gr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','1',32,'','','El_angleTL_gr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','700',32,'','','El_angleTL_gr7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','324',32,'','','El_angleTL_gr7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_angleTL_gr7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_angleTL_gr7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','3',32,'','','El_angleTL_gr7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','700',32,'','','El_angleTL_gr8','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','391',32,'','','El_angleTL_gr8','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_angleTL_gr8','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_angleTL_gr8','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','6',32,'','','El_angleTL_gr8','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','746',32,'','','El_angleTL_gr9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','384',32,'','','El_angleTL_gr9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_angleTL_gr9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_angleTL_gr9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','28',32,'','','El_angleTL_gr9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomY','331',32,'','','El_Obr_StrelaHR_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomXsc','0.2',32,'','','El_Obr_StrelaHR_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('CM101','geomX','8',32,'','','El_Obr_StrelaHR_gr1','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','727',32,'','','El_angleTR_gr10','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','384',32,'','','El_angleTR_gr10','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_angleTR_gr10','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_angleTR_gr10','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','55',32,'','','El_angleTR_gr10','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','323',32,'','','El_angleTR_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','397',32,'','','El_angleTR_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_angleTR_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_angleTR_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','8',32,'','','El_angleTR_gr2','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','381',32,'','','El_angleTR_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','391',32,'','','El_angleTR_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_angleTR_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_angleTR_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','18',32,'','','El_angleTR_gr3','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','381',32,'','','El_angleTR_gr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','323',32,'','','El_angleTR_gr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_angleTR_gr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_angleTR_gr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','25',32,'','','El_angleTR_gr4','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','381',32,'','','El_angleTR_gr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','258',32,'','','El_angleTR_gr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_angleTR_gr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_angleTR_gr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','35',32,'','','El_angleTR_gr5','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','467',32,'','','El_angleTR_gr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','393',32,'','','El_angleTR_gr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_angleTR_gr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_angleTR_gr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','11',32,'','','El_angleTR_gr6','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','681',32,'','','El_angleTR_gr7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','391',32,'','','El_angleTR_gr7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_angleTR_gr7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_angleTR_gr7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','2',32,'','','El_angleTR_gr7','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','727',32,'','','El_angleTR_gr8','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','252',32,'','','El_angleTR_gr8','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_angleTR_gr8','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_angleTR_gr8','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','41',32,'','','El_angleTR_gr8','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomX','727',32,'','','El_angleTR_gr9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomY','317',32,'','','El_angleTR_gr9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomXsc','0.2',32,'','','El_angleTR_gr9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomYsc','0.2',32,'','','El_angleTR_gr9','','','','','','');
INSERT INTO wlb_AGLKS_io VALUES('KCH_MN1','geomZ','47',32,'','','El_angleTR_gr9','','','','','','');
CREATE TABLE IF NOT EXISTS 'wlb_AGLKS_mime' ("ID" TEXT DEFAULT '' ,"MIME" TEXT DEFAULT '' ,"DATA" TEXT DEFAULT '' , PRIMARY KEY ("ID"));
CREATE TABLE IF NOT EXISTS 'wlb_AGLKS_uio' ("IDW" TEXT DEFAULT '' ,"IDC" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"IO_TYPE" INTEGER DEFAULT '' ,"IO_VAL" TEXT DEFAULT '' ,"SELF_FLG" INTEGER DEFAULT '' ,"CFG_TMPL" TEXT DEFAULT '' ,"CFG_VAL" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#IO_VAL" TEXT DEFAULT '' ,"ru#CFG_TMPL" TEXT DEFAULT '' ,"ru#CFG_VAL" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#IO_VAL" TEXT DEFAULT '' ,"uk#CFG_TMPL" TEXT DEFAULT '' ,"uk#CFG_VAL" TEXT DEFAULT '' , PRIMARY KEY ("IDW","IDC","ID"));
INSERT INTO wlb_AGLKS_uio VALUES('comprEn','','pVar','Variable',131073,'<EVAL>|0;0|',10,'Parameter|var','','Переменная','','','','Змінна','','','');
CREATE TABLE IF NOT EXISTS 'prj_AGLKS' ("OWNER" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"ICO" TEXT DEFAULT '' ,"PARENT" TEXT DEFAULT '' ,"PR_TR" INTEGER DEFAULT '1' ,"PROC" TEXT DEFAULT '' ,"uk#PROC" TEXT DEFAULT '' ,"ru#PROC" TEXT DEFAULT '' ,"PROC_PER" INTEGER DEFAULT '-1' ,"FLGS" INTEGER DEFAULT '0' ,"ATTRS" TEXT DEFAULT '*' ,"TIMESTAMP" INTEGER DEFAULT '' , PRIMARY KEY ("OWNER","ID"));
INSERT INTO prj_AGLKS VALUES('/AGLKS','so','','/wlb_Main/wdg_RootPgSo',1,'','','',-1,1,'pgOpen;demoPlayProc;',1580920441);
INSERT INTO prj_AGLKS VALUES('/AGLKS/so','1','','root',1,'','','',-1,5,'name;dscr;geomX;',1580920441);
INSERT INTO prj_AGLKS VALUES('/AGLKS/so/1','ggraph','','/wlb_Main/wdg_grpGraph',1,'','','',-1,2,'geomYsc;pgGrp;',1552226019);
INSERT INTO prj_AGLKS VALUES('/AGLKS/so/1/ggraph','1','','..',1,'','','',-1,0,'name;dscr;',1552226019);
INSERT INTO prj_AGLKS VALUES('/AGLKS/so/1/ggraph','2','','..',1,'','','',-1,0,'name;dscr;',1551624161);
INSERT INTO prj_AGLKS VALUES('/AGLKS/so/1','doc','','root',1,'
','
','
',-1,5,'name;dscr;',1580920441);
INSERT INTO prj_AGLKS VALUES('/AGLKS/so/1/doc','1','','/wlb_doc/wdg_docRepInstVals',1,'','','',-1,0,'name;geomYsc;',1580920441);
INSERT INTO prj_AGLKS VALUES('/AGLKS/so/1/doc','2','','/wlb_doc/wdg_docUsersSet',1,'','','',-1,0,'geomYsc;',1400852515);
INSERT INTO prj_AGLKS VALUES('/AGLKS/so/1/doc','3','','/wlb_doc/wdg_docRepDay',1,'','','',-1,0,'name;geomYsc;pgNoOpenProc;',1580920441);
INSERT INTO prj_AGLKS VALUES('/AGLKS/so/1/doc','4','','/wlb_doc/wdg_docRepMonth',1,'','','',-1,0,'name;geomYsc;pgNoOpenProc;',1580920441);
INSERT INTO prj_AGLKS VALUES('/AGLKS/so/1/doc','5','','/wlb_test/wdg_docTestGasNodeDayRep',1,'','','',-1,0,'name;geomYsc;',1580649241);
INSERT INTO prj_AGLKS VALUES('/AGLKS/so/1','mn','','root',1,'','','',-1,5,'name;dscr;',1560142498);
INSERT INTO prj_AGLKS VALUES('/AGLKS/so/1/mn','1','','/wlb_AGLKS/wdg_KCH_MN1',1,'','','',-1,0,'name;pgOpen;pgNoOpenProc;pgGrp;',1522592964);
INSERT INTO prj_AGLKS VALUES('/AGLKS/so/1/mn','10','','/wlb_test/wdg_mn_gen',1,'','','',-1,0,'pgNoOpenProc;pgGrp;',1560142498);
INSERT INTO prj_AGLKS VALUES('/AGLKS/so/1','gcadr','','/wlb_Main/wdg_grpCadr',1,'','','',-1,2,'geomYsc;evProc;pgGrp;',1400851728);
INSERT INTO prj_AGLKS VALUES('/AGLKS/so/1/gcadr','1','','..',1,'','','',-1,0,'name;grpName;','');
INSERT INTO prj_AGLKS VALUES('/AGLKS/so','rg','','root',1,'','','',-1,5,'name;',1554066600);
INSERT INTO prj_AGLKS VALUES('/AGLKS/so/rg','rg','','/wlb_Main/wdg_ResultGraph',1,'','','',-1,2,'name;geomYsc;pgGrp;grpName;',1554066600);
INSERT INTO prj_AGLKS VALUES('/AGLKS/so/1','gview','','/wlb_Main/wdg_ViewCadr',1,'','','',-1,2,'dscr;geomYsc;evProc;pgGrp;',1400851728);
INSERT INTO prj_AGLKS VALUES('/AGLKS/so/1/gview','1','','..',1,'','','',-1,0,'name;dscr;','');
INSERT INTO prj_AGLKS VALUES('/AGLKS/so','2','','root',1,'','','',-1,5,'name;dscr;',1553365967);
INSERT INTO prj_AGLKS VALUES('/AGLKS/so/2','ggraph','','/wlb_Main/wdg_grpGraph',1,'','','',-1,2,'geomYsc;pgGrp;',1551624161);
INSERT INTO prj_AGLKS VALUES('/AGLKS/so/2','mn','','root',1,'','','',-1,5,'name;dscr;',1553365967);
INSERT INTO prj_AGLKS VALUES('/AGLKS/so/2/mn','CM101','','/wlb_AGLKS/wdg_CM101',1,'','','',-1,0,'name;pgGrp;',1553365967);
INSERT INTO prj_AGLKS VALUES('/AGLKS','control','','root',1,'','','',-1,5,'name;',1563002297);
INSERT INTO prj_AGLKS VALUES('/AGLKS/control','ElCadr','','/wlb_Main/wdg_ElCadr',1,'','','',-1,0,'pgGrp;digComs;digStts;max;min;prmAMax;prmAMin;prmAnalog;prmAuto;prmCasc;prmClose;prmCloseSt;prmColor;prmCom;prmComText;prmDemention;prmDescr;prmId;prmImpQdwnTm;prmImpQupTm;prmManIn;prmOpenSt;prmOut;prmPrec;prmShifr;prmSp;prmStText;prmStop;prmVar;prmVarIn;prmWMax;prmWMin;',1426000648);
INSERT INTO prj_AGLKS VALUES('/AGLKS/control','terminator','','/wlb_Main/wdg_terminator',1,'','','',-1,0,'','');
INSERT INTO prj_AGLKS VALUES('/AGLKS/control','doc_panel','','/wlb_Main/wdg_doc_panel',1,'','','',-1,0,'','');
INSERT INTO prj_AGLKS VALUES('/AGLKS/control','cntrRegul','','/wlb_Main/wdg_cntrRegul',1,'','','',-1,0,'geomYsc;DESCR;Hdwn;Hup;K1;K2;K3;K4;KImpRfact;Kd;Ki;Kp;NAME;SHIFR;TImpMin;TImpPer;Td;Ti;Tzd;Zi;auto_addr;ed;impQdwn_addr;impQup_addr;max;min;out;out_addr;prec;sp;sp_addr;var;var_addr;',1400851728);
INSERT INTO prj_AGLKS VALUES('/AGLKS/control','grph_panel','','/wlb_Main/wdg_grph_panel',1,'','','',-1,0,'','');
INSERT INTO prj_AGLKS VALUES('/AGLKS/so/rg/rg','1','','..',1,'','','',-1,0,'name;grpName;',1554066600);
INSERT INTO prj_AGLKS VALUES('/AGLKS/so','3','','root',1,'
','
','
',-1,5,'name;dscr;',1554066378);
INSERT INTO prj_AGLKS VALUES('/AGLKS/so/3','ggraph','','/wlb_Main/wdg_grpGraph',1,'','','',-1,2,'geomYsc;pgGrp;',1551624161);
INSERT INTO prj_AGLKS VALUES('/AGLKS/so/3/ggraph','1','','..',1,'','','',-1,0,'name;dscr;',1551624161);
INSERT INTO prj_AGLKS VALUES('/AGLKS/so/3','mn','','root',1,'
','
','
',-1,5,'name;dscr;',1554066378);
INSERT INTO prj_AGLKS VALUES('/AGLKS/so/3/mn','CM101','','/wlb_AGLKS/wdg_CM101',1,'','','',-1,0,'name;pgGrp;',1554066378);
INSERT INTO prj_AGLKS VALUES('/AGLKS/so/2/ggraph','1','','..',1,'','','',-1,0,'name;dscr;',1551624161);
INSERT INTO prj_AGLKS VALUES('/AGLKS/so','4','','root',1,'
','
','
',-1,5,'name;dscr;',1554066407);
INSERT INTO prj_AGLKS VALUES('/AGLKS/so/4','ggraph','','/wlb_Main/wdg_grpGraph',1,'','','',-1,2,'geomYsc;pgGrp;',1551624161);
INSERT INTO prj_AGLKS VALUES('/AGLKS/so/4/ggraph','1','','..',1,'','','',-1,0,'name;dscr;',1551624161);
INSERT INTO prj_AGLKS VALUES('/AGLKS/so/4','mn','','root',1,'
','
','
',-1,5,'name;dscr;',1554066407);
INSERT INTO prj_AGLKS VALUES('/AGLKS/so/4/mn','1','','/wlb_AGLKS/wdg_CM101',1,'','','',-1,0,'name;pgGrp;',1554066407);
INSERT INTO prj_AGLKS VALUES('/AGLKS/so/5/mn','CM101','','/wlb_AGLKS/wdg_CM101',1,'','','',-1,0,'name;pgGrp;',1554066437);
INSERT INTO prj_AGLKS VALUES('/AGLKS/so','5','','root',1,'
','
','
',-1,5,'name;dscr;',1554066437);
INSERT INTO prj_AGLKS VALUES('/AGLKS/so/5','ggraph','','/wlb_Main/wdg_grpGraph',1,'','','',-1,2,'geomYsc;pgGrp;',1551624161);
INSERT INTO prj_AGLKS VALUES('/AGLKS/so/5/ggraph','1','','..',1,'','','',-1,0,'name;dscr;',1551624161);
INSERT INTO prj_AGLKS VALUES('/AGLKS/so/5','mn','','root',1,'
','
','
',-1,5,'name;dscr;',1554066437);
INSERT INTO prj_AGLKS VALUES('/AGLKS/so','6','','root',1,'
','
','
',-1,5,'name;dscr;',1554066462);
INSERT INTO prj_AGLKS VALUES('/AGLKS/so/6','ggraph','','/wlb_Main/wdg_grpGraph',1,'','','',-1,2,'geomYsc;pgGrp;',1551624161);
INSERT INTO prj_AGLKS VALUES('/AGLKS/so/6/ggraph','1','','..',1,'','','',-1,0,'name;dscr;',1551624161);
INSERT INTO prj_AGLKS VALUES('/AGLKS/so/6','mn','','root',1,'
','
','
',-1,5,'name;dscr;',1554066462);
INSERT INTO prj_AGLKS VALUES('/AGLKS/so/6/mn','CM101','','/wlb_AGLKS/wdg_CM101',1,'','','',-1,0,'name;pgGrp;',1554066462);
INSERT INTO prj_AGLKS VALUES('/AGLKS/so','7','','root',1,'
','
','
',-1,5,'name;dscr;',1554066488);
INSERT INTO prj_AGLKS VALUES('/AGLKS/so/7','ggraph','','/wlb_Main/wdg_grpGraph',1,'','','',-1,2,'geomYsc;pgGrp;',1553365967);
INSERT INTO prj_AGLKS VALUES('/AGLKS/so/7/ggraph','1','','..',1,'','','',-1,0,'name;dscr;',1553365967);
INSERT INTO prj_AGLKS VALUES('/AGLKS/so/7','mn','','root',1,'
','
','
',-1,5,'name;dscr;',1554066488);
INSERT INTO prj_AGLKS VALUES('/AGLKS/so/7/mn','CM101','','/wlb_AGLKS/wdg_CM101',1,'','','',-1,0,'name;pgGrp;',1554066488);
INSERT INTO prj_AGLKS VALUES('/AGLKS/control','accept','','/wlb_Main/wdg_accept',1,'','','',-1,0,'','');
INSERT INTO prj_AGLKS VALUES('/AGLKS/so/1/doc','6','','/wlb_doc/wdg_docAlarmsRep',1,'','','',-1,0,'geomYsc;',1400852515);
INSERT INTO prj_AGLKS VALUES('/AGLKS/control','treeSelect','','/wlb_Main/wdg_treeSelect',1,'','','',-1,0,'',1383147124);
INSERT INTO prj_AGLKS VALUES('/AGLKS/control','cntrPasp','','/wlb_Main/wdg_cntrPaspExt',0,'','','',-1,0,'owner;name;geomZ;pName;',1494598746);
INSERT INTO prj_AGLKS VALUES('/AGLKS/control','cntrPaspOld','','/wlb_Main/wdg_cntrPasp',0,'','','',-1,0,'owner;geomZ;pName;',1494521098);
INSERT INTO prj_AGLKS VALUES('/AGLKS/control','graphCalc','','/wlb_Main/wdg_graphCalc',0,'','','',-1,0,'owner;',1563002297);
CREATE TABLE IF NOT EXISTS 'VCAPrjs' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"DB_TBL" TEXT DEFAULT '' ,"ICO" TEXT DEFAULT '' ,"USER" TEXT DEFAULT 'root' ,"GRP" TEXT DEFAULT 'UI' ,"PERMIT" INTEGER DEFAULT '436' ,"PER" INTEGER DEFAULT '100' ,"STYLE" INTEGER DEFAULT '-1' ,"EN_BY_NEED" INTEGER DEFAULT '1' , PRIMARY KEY ("ID"));
INSERT INTO VCAPrjs VALUES('AGLKS','AGLKS','АГЛКС','АГЛКС','The project of visualisation AGLKS.

Author: Roman Savochenko <roman@oscada.org>
Version: 2.1.0','Проект візуалізації Анастасіївської Газо-Ліфтної компресорної станції.
Автор: Роман Савоченко <roman@oscada.org>
Версія: 2.1.0','Проект визуализации Анастасиевской Газо-Лифтной компрессорной станции.
Автор: Роман Савоченко <roman@oscada.org>
Версия: 2.1.0','prj_AGLKS','iVBORw0KGgoAAAANSUhEUgAAAEAAAABACAYAAACqaXHeAAAACXBIWXMAAAsTAAALEwEAmpwYAAAA
HXRFWHRDb21tZW50AENyZWF0ZWQgd2l0aCBUaGUgR0lNUO9kJW4AABEsSURBVHic7Vp7cFzVef+d
c+/duw/tQ6u1bEn2rh8YSwbHYIuHecQmgkBVCG1pnKYUA8Mr00JppqUkHdq0mUzaNPxFOulMO4Fm
ilsIhRYbsBvigmXAtWWZyLIl6+WHXqtdvXb12L177z3n9I9777KSJXltWTiZ+pvZ0eicc8893+/8
vsf5zgWuyBW5Ilfkivz/FamYQXV1ddf19PQ8IYRYBSAGIA4gt6gr+5ykKABkWf6BEOJpl8t1N6X0
N/x+f0TTtCCAawGswa8xIEUBEI1Gbx0fH7959erVYmpqykcIuVFV1fskSfqyEOIuIYQHwFL8GoIh
FzNIkiQNAGpqanK1tbWarusEAOLxuNrY2BgUQjyv63pG1/UMY+wVxthxAFkAHwIYX6zFXwopCgAh
BAcARVGwefNmzWnPZDIkFosZuq6TeDyuNDY2LgXwvGEYWV3XM4SQn2az2U/xKwxGUQBwzvls7V6v
VziAzABDbmxsjAD4U5fLpem6nnG5XDvT6fQh+9FfGUAWBEChnAeMMIBnQqHQU46pmKb5CoA2XGYw
igWAXcikc4EBAPF43N3Y2BgSQnyrAIyXOecncBnAuGQMmEsKwQBm9xtCiLzfEEL8i67rzficwFh0
AGbKeUxlCYDnHDAURXl1YmKiEYsIRlEAMMYuyASKlfOAUQbg2VAopOu6nvF6va8PDw8fsB+9ZIB8
7gyYS84DRkgI8VRpaeljhX5DCNGKBYJRdB5ACIEkSZBlubD9fM9dVL/P5xO1tbWaEGI2J+pqbGwM
FDpRzvkrpmm24CLAuCAGzASgWIWK7Z9tTDAYFDfffHPeiU5NTZHVq1cbuq6TgYEB+fDhw+Wwk69s
Njuu6/qPAfwDigThghmgKMqCFFpofzAYFFu2bMmbyurVqw3DMEh/f7/c0NDg13X9UQDHAew67+Jw
gU6QUjovA2Zb8GL2BwIBceutt2qAxYzx8fH+hoaGIADPvJMUSLEAzMqAC13wYvYHg0GxYsWKFIDg
vA/MkKJ9ACEEsixfdgbMN0aSijrdT5N5tRGpei+s833gyT/pZkIIUEpBKQUhJL+Y2pq3wmXBPnXu
BRPs2v/E4K3XvVMaCQ3MOW5orDL3SfO9Y5RSseULu8ORUL+6u+Eb8cL3lgYGlPWrDgT9vhGlJ149
1dxx+wQAIUkSJImQpx5d4X/hz656YXmV5x8B+DkXg4yJD3r6sn9/1fX7TwJgAPIIno8BvwMgoOtc
+4PtEfXl10eksbExKRQKcVVVhQOC8/ejpk0T2WyWnJs2EHR3dyubq61QduDIdROapk0bd/cXO0oy
mQzt7u5WAoFAPvVIJpMyYwxlZWU8EubkhvXvlOV0D/tl2/UTZ/vdrH+wV/b7/bx8iYq/eW5s0/KK
a0PdpzMDZ3oyP0oO6SgLKzWxqPd3V8a82/e+ecMz9zzQ+F+wIoQOQJwPgEc4F4mPD6X6t91Wuuml
f+5QT56Mmhs2bDC8Xi+bSbk33h7nZ8+elSVJQiFAACDLw65778hQAHjtrRTv7++XKaVwu93CAgBI
pdK0oaHBHY1GzfqtJgGAEydOuDRNw8aNG40bN5zyUmqS/zm0Nd306SDt6zstSZLkisWiZv3WrpJS
vynf//Wmwd17k28COA1gGYDmuq1liV2v1T627fayl7beFs7u/2j0E1jM1uhcmotU/QoAd6THzU92
/qw/QQjBnV9k8qlTp5RUKkWFEMTxCY6iQ0ND0ujoqKSqqli2bBmrrKxklZWVrKqqilVWVjJFUYQz
bmRkRHK73WLp0qWssrLSjjISRkZGpGQyKTkMSCQSUjwelw19klZEOtzJkZh2oi1FOjs7lfHxcerz
+cTmL2TkJaVJ194P1J7de5MMQA2A9QBcAMb27R9p/fc3Bn6huqj70d9f/iSAm2GV8JT5GLADAP3k
8NjBf329/5a//c46PPCVEP3v/Wk6NDQkRaNR5vP5xEx/4PP5eHV1tVFdXW26XK68rRFCUFIy7AXS
oJSipKSEV1dXG+vWrbPHtcHn83KfL8cJIfk5OedgjJH1awddsqTTE12rtP7+s7Ku62Tt2rVGTU2N
cf01BwNCAG+8bYwBiMKKBGcAtAIYBsBf+qczLW/vSXy0e0+yElYxNw5gbD4AHuZCTD7+9LEzus7r
/vOdIfHkI1Xkpk1c6o/HpYmJCVpaWipkWRbUXqwkSXC73YhEIryqqoqpqjoNAAcQ20RQVlY2bZyi
KHC73aCUArAmJYTApRBsvva0Z3S8Qj/R7uKpVEoOh8N87dq1ZiwWY6HAbjmjBdjps/0uWIXeUQBH
AHQD0ACIY8cnBo4dnxgFcBOACAAFgDKrCYhU/S0A1o6MGL8YTOqVAKS3do8IAKi/k8pDQ0PS8PAw
NU3T2qkCZ1h4ZnC5XPmfoij5XXU8uizLUBQFqqrmn58ZZe75kqF893nmzmRV8eHhreODg4OSEALR
aJRVVlbykpISoUhZKZNV+cTEhBsAATAEoA/AFAADgAnrnNAD4CMAHwDoBDA1FwMeAYB3f578CMBy
AMbho1N8NEXJhhouedRxeubMGbmqqorJssycuMw5hxACjDEwxpAHyAF2xjjOef43bQOEgBOpdnw1
Y4fNFK0IN5QkEtwIhUJ8xYoVZiAQ4JRSCBDBuICu64r94IStfOHEAkDGBoHCCofGOQCIVL0HwHYA
piKT6u98a+1aXefeiUmZjqUVHg7lpLvvEMru93tZIpGggUCAC1sBRxlHedM0Z1FsOlCmacIwjHy/
M4cz9tFnfZlbNg+5ntjhlbdsGnB/2kxob2JDNhAIcEKIYIxBN0q4R81S0zQdtI0ZyjvCMePOYjYG
/DbsdPLB7VWPT++ynr1rK5F+8mpc6u3tlZYtW8Z4gWKc87xSTpgsdJJcfKaoM87JLqcDYL1xckqI
H7+cYm43xUPb3fLDX2OuF/6uT08mV1G/38+9Xq+YyCzTI8F2TzgkURQkOdPAT9WXANjPuHhLDu/5
gT2Oz+YDHgaA5/6y7Q9J6L1XSOi990novT3+5Q3s7u1mprVDYmVhQq6tzsg9PT3y8PAw5fMwwFHS
2mVGGJPEhTBACIGSkhLx/oFl2qmzlPu8FPV1CU9LS4sSj8elbDZLBobXaYQAD3416IXlAxRYNC+U
hwBsymRYBYAVACoAuKcNEqn6KgB36jpve/FHpxmAECznQTnnSKfTdM8+mADwwH1eKR6PS/F4nDLG
iQOAo5ijtGEYeSBc8hTVcgq3lRTOOMdUhBB5EAryfeH1enk0usrY33jduMmAu7YqkkI61M7OTnl0
dJTEh6K55Eip8cSO0tA9dREFgB+A1wFBpOqvB/B9LsT4E3/cshfANgAbAZTONIEdAGhL68ReWLfA
UwCaAZQpivKlWCxm6vya3ORUo+uWG93U55mgfX19smkaczIAAKqWtHr83mHZ50nJ7afKc4yl8mZR
yADgXAZwzkEphdfr5YoaZYd/mcjesrnf841HhOuvX2w3y8rKWCwWMz/+dGv6xmve9L/3Hzf4DjWl
v6bIhK2MeuLBoHIdgN8DwN7Zm/zm62/FY7DuMI8BkGbS5GEA5jf/orUZQBmABIBGAJ2yLPMlS5aw
1WvWGad6o1lKCe67xyUPDAxIhmHkGeDYdiH1r45+HFhZ2VySzbn4rp8HtUwmQ61YjzwLLAZgVgCc
n9vt5l19N00mhhV29RqF1m5Iql1dXfLo6Cg1ech8+ts4/lff78yEgrJ/wzX+Z0tLlRc5E19OJHO7
nvnzEw/d//UmE0A1rAgQx8wwSELvXWvbxz02fc7AiqdVAASlVEiSJI51bhl/452wdujQIZWxcfrS
T1ZN+nxrOCEtKqVUOCA4TnDfoXuHDT2hHG4aJq2t3XImkyGRSMRUVVVwzoVpmnh9747B9vZ2hdKD
bkopXtu9IW2dAhvdzpwWCH72b7u2jjY1NamnTo0pS5f2yhUVFebKlSu5JJVkvvfDrtT3fth1AFa4
S9s/YSsegZUYfQqgA8DkbFGAwLL7DgAnAIzADilOfCeEiHA4zKLRqJlOpykAyLKMcDjMTNOE2+2e
trOa5mbJpAfJZFrN5XKkrKyMLV++3PT5fJwxlrd7t9stysvLTVVV81ljJBJhuVwOHo9HMMYgy7II
h8N8zZo1hhP2NE0jqVSKJpNJH6xQ1WZv3BIAYXszOYCzALrs/kHMkgdwWLnzQVieNAnLD4hCpyWE
QCAQYDU1NblsNku9Xi93uVzC4/FwzjlCoRAXQgjHB3DOoaqqqKqqMkKhEPN6vby8vJy5XC5RQHER
CoXY+vXrhSRJCAaDDADWrVunM8byc9ogcAfAyclJ0tfXpzQ1NSlHjhxZAeuo2w7gf2E5cSc7NGAl
SCP231mPwxzAJIBT9kMmChIKh9oAQCkVoVCI2dmYIITA4/Fwuy/PFkdUVeWVlZWOApBlWQDIs4QQ
AlVVeXl5eX4OAAiHw6bzP+cck5MTpL2jxW0YWZ5OZ3uONbeNHD9+fJ1pmjlYVH8FwB5bjwF8Fg6F
rQsr1Gk2E+CYJYtydrRwwfZf4YQvRxhj56S3ACBJUv70OCPUnSO2qSGTyZDW1lZV13Wi61o8kexI
q77etctjimhrT5W1tU36TZNrAHYCOIrp9wLnvdEqqiY4myyktjdfXyaTIW1tbaqu64Rznjl58mRv
c3NzTNO0SUUhro2b/Muffizm2nJbKWs+mv7ku9/ufv/4sckhXOTtUNEAiGKqlRcpjtK5XC7X2dmZ
OHr0aIVhGJppmjosJzYBYKfHSzu33RWu3nxD8CuBoLx0252RkW13RvaWq/v6L/bdF82AhUiBwkzT
tLNtbW0jra2t6w3DyAHwwdrJnbC8NWBfeXUlt2kAboOVyVUBmLPAWqxcCAAXzYBCWuu6PnDy5Mnh
lpaWq3Rdz8A6eAVhxWdH6bnu+Fz2OrL2+CxmP/UVLYvCgEKFTdNMdXR0JJqbm2O6rmc55y4AlbDO
5ufsMua3YwJr14MASmGxZc66ZjFySXzADMeV7erqShw9erRC1/UsY8yElVZP4sIVnnUpsMKzjgXu
PrAABjhKG4bGevtOjx4+3BTOTOWyhmHqmMeOsbCPGhzlDVgZ34I/3LggH9DX1ycfPHjQncvlBtrb
24fb2o5ftSIm83Xrff7q9bJ25rTx6lACxzjP2+mHuPSftQhYhU8ZRX7pOp8UC0BW1/XsgQMHRC6X
ywghVABVpWFZf/yPVrbX379kTXJQ79B1sev62sCRcnWfed4ZL14uixP8UNO0JzHj2nnjJr93+4MV
t3u8Uu2ScrXE7l+QUzqPXDYnOI5ZPjj42bubqmDdsji2uJjKO3JJneClWLAEa1cUWICS+YcvSC6r
E5xLKKwEZbGVd6TQCSoAXMlcnavg3YXherY2AYCVq/s4AMjJXB2FZbsl9oS84EEGC3GzoI3YkwgA
AVi7kYJVeXGOpIspAhb9Tfv9tQBWwmKhwxBnfTPbGCzneTaZq0uWq/u4DOuwsQXAF2E5liysIgKz
FRqDddfm3Lo4O52BVWKqgeWMShZVbUscxUrstW6GdXM1DssxOuYh2XrMbJu0/98J4F0AE7KtxG8C
+C1Y5eRRWIwwYSmt2W1uu81lt6fshTiefxIWKIv5USWDVeQ4AeBqe70BW9lSWMzIwdokbZa2jL3m
IwDeh904DqtEHINVQpq0leX24DDmprVTQUrBunTsxSVwTHNJubqPJ3N1vQB+CquqG8Fnu+3DZxWf
udomYZX4PoQFBkgyV6fA+lhgpf2ADmtHJVgILrXfr9qTFZqAak8+CuAwgM5ydZ9T5F80SebqVFib
47abOM6NaHO1mQAmy9V9WcB2bLYjdBQr3G3F/hV6d+f+rXCcCSC7yBngFbkiiyD/B5CDW0pgBswY
AAAAAElFTkSuQmCC','root','op',436,100,0,1);
CREATE TABLE IF NOT EXISTS 'wlb_AGLKS' ("ID" TEXT DEFAULT '' ,"ICO" TEXT DEFAULT '' ,"PARENT" TEXT DEFAULT '' ,"PR_TR" INTEGER DEFAULT '0' ,"PROC" TEXT DEFAULT '' ,"ru#PROC" TEXT DEFAULT '' ,"uk#PROC" TEXT DEFAULT '' ,"PROC_PER" INTEGER DEFAULT '-1' ,"ATTRS" TEXT DEFAULT '' ,"TIMESTAMP" INTEGER DEFAULT '0' , PRIMARY KEY ("ID"));
INSERT INTO wlb_AGLKS VALUES('CM101','iVBORw0KGgoAAAANSUhEUgAAAEAAAAApCAIAAAAK8LgbAAAACXBIWXMAAA7EAAAOxAGVKw4bAAAI
pklEQVRYhdVYzXMcxRV//THfM7szsiytLAsZ2bIRjmSoFAaKVLlwRMEh4ZCqHFKcUuSQCpfc+VNy
4+IDCbigqCQXUgFSdhJXLKLIwljgD7SS1itpd3ZnZ6ZnuqdzGCHJ2tWuJG9C8rvszOz7db/fzHuv
Xzd6++236/U6/H/CdV1ar9drtdp37cnRgb9rBx4XtI9jNZvNPU8IIaZp9nGKdvRTwPVb1xFCzWoz
8qP8SalUunz5ch+naEc/BVS/qm5f28fswkjBs70+jt8R/RRw8eLF/EIIQSjJssw27D6O3xH9FHDm
zBkAkFImSaJpGmNM07Qjj5ZlWZIkCCGEkJSSEEJpB2/7X4U0z7OGhuRjjyOl5JzbjA37Pgixn1nn
L4BQ+sQTD482ccs9JaSrtv4CwZqu60cbZBtJazR8mD35ZKRABQAI4Q8ejHCubht0FmBZweuvL9y4
MXaEKe/WNlcfaBenm/pQApAcYYSZmZVvvvFarSIAVOPjDdM4MX7bQnGaJkKEDx8eC4JeAgDg3j3v
2rWpI0wP0FCg8ac/6gAjGONCoXBYvm2zz665XywgKSXA3wFg4RMAIMPDpzDGe/Kqn0m8G67rbl9n
WVav1zHGiqJYlnUQ+sP1ysLdDc3WKl9Wth++/PLLAwMDeywfV4AQghDS3QZjPE1IMYoWdqnqjqgR
11e2WkzN1oyi4Rx3FENpt9xXwLPPLtu2332au/7rf5tzZ1+cP6be7G55e/MXX5XtH59/18HlXs7D
qVPNP3x8pjQmMMFbi3oIwf0ADaNDCLh58+SHHz7XfaYMRViGv39vkMjZ7pYCP0BZ9sHC01j2zqvZ
2Zv1at1v+oqmNGoNQIAxxhTHLPa8vUv7Y4UQlhIAiOxd9EmWAQAcwBIAEEIIkAiFCIWqqpqtabZm
eZblWBjvXbh6CBBChGGYJI9UQ9M0DcPowuKcR1F0WNZunD17bmDgBwex7CGg3qpfv3bd3/ABQLM1
+5htFI2SUTp7+mwXlh/6u1nOcUeztRFzpDvraOghgGJaKBZYiwGAqqoECKRArB5lh2BSKOxiSQIp
UPIfKdm9QigVSWsrEjjjsYzTKBX2vp3JDivcYUWNiIREOD1YR0OPZk4SGfM4h0BCKShWyQIMnuch
1KGobbHwIyy1qJrDJhCwLCvLs7l/6PEFXMu99NKlNE23n3DOHcfJF8VKpbK4uLi0tLSH5dleO2t4
ePiNN94IgqBcLs/Nzfl+j0WmPwKSJOGcE0K2W5q8yy+Xy0EQSCnzwpwfzGx3Pmmacs4VRdE0TVGU
nKXr+ueff+66bqFQKBaLq6urebuvaZqu63nhEkIcdgvxiIAoiqIoIoTY326kDMNwHGdPQQSA0dHR
wcHBLMsajQYAFIvF3T2FruumabZHy4ULFwBASjk/P2+apqqqhJA8FCmljuNgjHNWHMf5yJRSzrll
Wfs1LI8IUA3j+c3NumFswlZ8I4TGx8fX19fr9Xo+k+M4jLH5+c/Gx792nLnr11/K3+Lw8HC1Wo3j
GACyLBsfH4/jeG1tLd9PFQoFxtja2m9v396cmLh99+7Z/C1gjMvlcu736OgopfT+/fsAQHX9Od9v
WtaybWdZ1r5+7QiQMnnllTnGaJalsbQX1l5zzeSlE+9NTGzOzNyp1S4TQoaGhlz37vj4imH8emJi
cm7uHcf5SNdhYKB16dLCyooVBC9GUeR5nq7/dWSkcefOJU3TNE0jZPXMmQVV/eXk5PT8/LtjY1du
3Dj99NP3ms305s2LQghVVQcGxMzMrcXF7zuOAwClkjM5Wc0U+Y/Bn7km+1Hpd7vdxTj99NMp399p
0SkAXloa9H2uaYQjA+KaH8L8xtj8/BhjSMqWlDLLskaDpunguXNcSlmt0vv3x3zfVNXEMHgYKpZF
8ljf3LTjWI2iSNNCAKjXxdLS4MSEkFKur+P3359eWyteuWICACHCMAxd12s1WFoabLWSnBIE8dWr
06kkraDcoLD51altXxljuo4ajUcacvTWW2/VajXG2Fp9za/6CCPO+FZEqerU1JTneRsbG0IIjHGz
2bQsK+/s93zKoaGharUqpcxDyHGcOI7DMJRSMsYMw0jTVFXVbWJuI6UMwzBPDE3TMMZ5PbizeqdS
rmCCG5XGtjOzs7Oqqu6e1PO8rRwQQny5+GWSJUmcRPWtYynbts+fP//JJxcmJ/9smpQx5rqulLI9
IqWU6+vruUgAwBjfujWiKOGJEyuMsTw29mQhQiiOYyGElDI/elhedprN0unTC4yx5S+WgyjgCZeZ
pDo1CoZiKPkr2JsD+Q8hJGpEQRDkt0bRGHhiQFd0AAiCk/BtTu+XTLnru7Ot1TpeKKx2YWVZli8U
+ZEJAKSpzVgx/3erahsAAEQlhmOomtr+2XcESCknZiY2K5tSyrAWAkC4HIIJ8BQgg2cES+i2glJK
pZS7z22Qlkm1G6dDWUQgEeQNtzKggIAsy/wVHwCgApqmPfPUM/sKUBTl3sI9xplIBU841ahmaQpW
AOAnP79tfeZ/nVHYv5a148UfVtIgGPnX2nyxeEDK+Pea54drxz9e/afrbtzb2D701x3dO+npht7x
YGsnhAaPDeb5BACKrljHLJWoABAHBSAKVhX1MAKS2OAJi3T94EdDGcdZhLiq6ro+PT3NGNv9LyGk
41q2o+mFF17oOO5H75ycmgLEGpRSwzA6voZ23PjYNc1mpSiR7xuGcZAG4ZtFy/fdUgnRZrNYLOap
3xO9veE8olQrFA53xiZEghC0b2G7TsQ5T9oPTrqjd1RQalK6b+e8HwhRySF3MJRSSvdWyd4sKSFN
EaUZAHAOnQJESikPth3vwDyUsZQ9KLmHu7ci6M03f3X16k8JCQCAc0RpArCn3JoAEcBhFagAGQA/
DIUCEADWxYJz9fnnP/C8rQLleR7VdfTqq7/J7/NF8ZCO/lfR7iF1D3zc9z+IYrH4b7fLC7s/YYjB
AAAAAElFTkSuQmCC','/wlb_originals/wdg_Box',0,'','','',-1,'name;geomW;geomH;geomXsc;geomYsc;backColor;bordWidth;bordColor;',1554057059);
INSERT INTO wlb_AGLKS VALUES('KCH_MN1','iVBORw0KGgoAAAANSUhEUgAAAEAAAAAqCAIAAACMZMq1AAAACXBIWXMAAAx1AAAMdQEteJR1AAAJ
yklEQVRYhe1ZS2yc1RU+9/E/Z8Yztmf8mDG2xo5N7IZHHMfEFHADuJBs0sZkAUQobRdVV2yqLlqR
TVW1O4SEqqpiQUVLKzWlJaJ1SERSkyBcTF0nDomN8rBjD7bH837+z3u7mGFie2zHM4gUpJ7F6P/P
f+6597vncc89g1588cVsNgtfT1JVlWaz2a8vAACgd3k+27bXcRBCGOOqFd5VAAihs+Nn9axuW3Yu
liswRVF85plnOOfV6bzbFojMRjDBzGLF6WXqrHN+EYXbAsAYW8dBCCGEqpivq7OroBBjzBgTZdFR
66h6+2E7APL5/PmL523dzqfyekYvMD0ez4EDByqdjHO+Z88eADAMQxTFwm+lStbRtiwQm4shhJhd
tIOgCI46x9ZDJMlCCBgDhMAwCOfrzcUxRg0NIqVGJLKZEkGwRdFmDGFcNFIutx7wxgB0Xeeci6KI
MaaU7ujYAavsLjkkd517tbzTmXS5tNWOVlOji6KNMbdtlMvRW7eaGaMAQKleX5/I5/OkKWi2PO6k
Cw1L/9oMAKXM6TQAQNeJLFuco3BYnZ4O3hkA0nVqWVyWAUAQhL6+PtjS7ocPX/rsM3UzbYRY0Wht
Ou0EgHvuWXnqqZlr17w5snTuk4jaHC83TolMk8TjSuG5sPdlwbh2Ss65LKcBoKHVT00eFrMklzIM
mfM7eGomg86c6UZI3VqsQJcvN1+40AMAErwfXoAwtG9n1Ga0BgBj+ksvnZucbLyaG1xOqk+2/KnN
8+nFi4GxsZ1ba6mpMSm1ys6ou0FUUSxJymkabW5OLy7Sa9dqT5zYt7x8CgBWGlq6unh9ffHECYVC
wWBwQy22LVgWrSqvflHCtg0IcUqZbSNCii62t6Hh8fp6LgglObGt7cjx41Jr6yZ6qk/kX5AoQP65
5/4Zi4kAgBALBLLPP39mCv08FHV8J/DrgHBFEFhHx3xYPvib89axJ3nTQ2d0vebkyUfX6ql+83Vd
tyxr3Vmmquo2CyRqmuqrr347mzVV9XYIMvIBYuxt1IFY0WcY5R3WH0evYWQffOGFM6tVtHN+IfvL
LBnhyaQoipIkVQRgdnl25uJMNlOsiGWX7Pa7e9t7PR7PtgBwTjgXKOXTc9NLoSUA0FJa6fNjjz1W
AIYtCwAwY+W+IgAkmCAgZABUUVciC8mSbFvFDEAR1WM63rFdPcUsZFlW6HookUlgjLW0BgCAgFBS
XgWV0zRCR2t+8nt7t02IpmnxeNztdldgBwx8VQghjJQahXMeCoUopQDQ2NgIAPl8vnwxDoejCIBS
6nQ4TcMEAKVWAQDJIbl8LkEUoIzSaVJ4SCQSpmn6fD7Mbcs0Nc3Sdd00TQDw+XzbrPY45rqua9rn
ZheAURaPx1VBsBkrxcbk9cloOIoApZZTpbHHjh0rArBtG7uwkTCoRJOLSQCAOCwtLN0buFcS1+zl
3r17Ozp+3Nr659nZ2aGhIU3TLl26hBAyTdOjGwdv3HjnoYcim5c35dRY0+jsdRZgW5ZFKaU07/Wy
w2cvvvP005nMDZ9veWWlMXIzkslkSqMERXDUO6DkQoqiJEIJLa2x+OeVukRFRUR4/S62t7cvLi4+
8UT90tJpj+f5QCDQ0fGPnp7l4eH08rL81w/2Icvav/8/fX1odrb5/PkH7gjA4/GU4rVQp+zfP4Vx
6E3tYZLN1tXljh795OWXD+3cudMwjFI9RkUqyAIh5PZJ3NLS4nbfLtGIQBS3Ul72IIQkSQqFfKdO
dQ0OLkxPT09MtLhccydOdFsWxdgCgNHRPaEQGRy8uXpgRUX/+HirZVmEkFu3vJHIEgB0dnZCWT2G
ELoNoLu7ezuqVVVtampaWFjAGI+Ojtq2LQgCACAETqeTEFLQC6ADQDgcxhh7vd5CUAqC0N4OmqYv
LS0RQmpqaiRJYox1dXUlEonVjocQeL3e0ms+n89kMg6HoxDWq2uCirPe+Ph4JBKZmppay+YAgOLx
vo8+SqfT+XweABhjw8PDBw4c4JwDxt8KBvc1NyMEsix9s7l5oKGhkKmCweCRI0eOHj3KOR8aGrr/
/vs5Z5zD/pGRdDqdTqcBIBAIPPvssz6fzzAM2+U6dPx47d69AABgVXYn5pyPj4+/8grp7b1aeGWM
CYIAgA4f/iiyKI4b3YIg9PdfGBgAVa2NRlv8fn939wlRmf575LeCgA75vqeq1puBn3Iu/sD3Q8HW
U6nvLiwsEEIGB+N+v7+/v1/TfudyTf/h8oOcsZ6euV27krFYc1NTm99v2fbPUmr362PDA98gB7vO
hMM7KgNg2/bkp5Ph8N9OnSpyXD5XbUMtAH/rrX7DIB6/RwY4d253KCQ8+uj1xcWZqakpVc0nkzsX
Z/4CAO8EHwgEUrc+eBs4//eDbbaNGNsxMCBhjC9c8Pb335ycnGxsxBMT94LH43Q4rlxpu3w5/fHH
kd7ezz78cDKRGAKAeng9GU6fZgO5XGNlABBCKlVlWS5xMMN21i64oqqqBYurqgpgYownJiYMw3jk
EQQA0WgUYxwMgqbpyXgcAAzDJETM5XI+n0/Xddu2T548SSkdHsYAiFKq63qhDlheXh4ZGVm9EsaK
6bHitoppmes4RCAADACwYdwTi83X1ZV3rzhCfapqIwSQFUVhlygiAEoJ53D9+vXXXps0jCsIoYaG
BgAACAFA582b062thmFsuIxkUpZlq2IAnHMs4dunJoDgEkAEALR795y+gpQ5ccXvb2n5uK8PA5CS
WHf34vst35ct/eGmX7nd+jX5R4jTB4K/mBxvxBi/+y60tmZWZR24777QwntOSqnfv+Tz5ctXYhjU
MKjDUSEAQkh7U3vUe6inZ6zEpJSOjBBCEmmNJzrbCMbz8x6M6zIZR6F9dPVqIJ5wZlKXU5SMzuwA
gBh8ygGfnumMRzboao2NBRlzhLvqFUJiMfWNNwa2WFLFMeB0um37iUBgfjU/EoF83pFKpRiLAoBt
u8Jxc/rqWCwcKwjITlnxXNnVucv2NQMAwDIALINvw1nyeWc6zTm3dV3XdffKysZi1QAAAIwRQhvY
VFEURVFKr/ML85hh4fM7HTNYdiVrtqyPn82ocNwW1G4tWTEAhDhCd26nYQmb3CxFi+SUVI9KZLKh
sGVRy1rzSZKkbRbkX1ZzFwGSJKmUcCVJEohA8MYAKM1Qui3j2LZgWRJCTBSLrYZqAFiWFI3WbX2R
z6eBAna5ihZAGLBFMonGKKotF9Y0bzJZv53rA2PUNBUALoqi15uoBgBCqKnpxtzcvjtKqiqoaztd
qRSkUhtIynI2mexKJitaiO71vgfVWaC392wVo74kqv6/na8I/R/A/5q+9gCoqm6rJ/7VJEVR/gtO
6VqgwKO3JAAAAABJRU5ErkJggg==','/wlb_originals/wdg_Box',0,'','','',-1,'geomW;geomH;geomXsc;geomYsc;backColor;bordWidth;bordColor;',1554057011);
INSERT INTO wlb_AGLKS VALUES('comprEn','iVBORw0KGgoAAAANSUhEUgAAAEAAAABACAYAAACqaXHeAAAABHNCSVQICAgIfAhkiAAAAAlwSFlz
AAAOxAAADsQBlSsOGwAAAxVJREFUeJztmj1vE0EQhp8gqrRcARRbAMXVC+VSwC9wJKCwKc9QprVT
kqQMEk2cRCliuYMYGqAMH5WdRC4AJRSATAMooQAU0Q7F+iTLIoqT3Meefa80rX1+dvzO3MxCrly5
cuXKNZRmH8xK2s+QqrTWorWWsQWhtRYBaYJokPW1unx49358YIQAwpgDUUrJ641X4wFhEICALIHM
+b6Ug0C+fPo82iD+B0BA9kC2QKZ8X0rF4uhCOAxAf+yAKJBqpSJ/fv0eLRjDABCQNyBlkIVKRdYf
PxkdCMMCCKMJ8tDzpBwEo2GUxwUgIAcgM71qkfmSeRIA/VHqGeVmqy0/vn3PHozTAhCQbs8fZotF
qS3WsgUhCgD9RqkgW211lADCktmcnBStdTaqRdQA+mMOZBLcNso4AYQQppWS2mLNTRBxAxBsW70E
UjVGykHgFoQkAITRxb5fODV/SBJAv1FqbFu92WqnCyINAGE0sP1DbbGWXludJoAwZkB8308HggsA
DrDeMK2UaK2TheACgMHQINOFgmy22vHPH1wEINi22scOYcYSQAhhCaQcBFJfq8cDwmUAYWxhhzBa
6+i7ySwAEGw3uY2dPRz1m85ESsgBrQArvs9yELDw/EW0H+5yBjRA7hFzo+QqgCaIB/GZn4sAur3/
+Wlfls5GyCcR7QPPgK4x/PR9OqurE51OJ5kvdyEDpkCMMZGVuMxkwDz25GutNp7ncenK5YnEHyKN
DGiAeJ4X29DU2Qx4C3wEtoOABXOdW3duJ3/ig0oiA7rYep7U4sSpDLgL/PV9qmt1lFKcv3gh/VPv
VxwZcNCr56kMOEg5A54CXz2P3UKBUrHEo5s33DrxQUWZAQ1s+5r2Sizxt8Fd4BqwV6nwstV2w92H
1UkzoP/9PNOXqE4CYA97McIY4+Q1ulhNcB7YVwpTqVI1Jp32NUoNmwFNkKuM4FXaowBk7sLDcXUY
gB2QZeyCwplN7pA6tQfcB84ZgyqWqBYK7rWvUWowA0rYRaWL7h6LtNayTYyLB9eltY539eS6RtLZ
c+XKlWuc9Q+SowNXJeuWHQAAAABJRU5ErkJggg==','/wlb_originals/wdg_ElFigure',0,'JavaLikeCalc.JavaScript
if( pVar.isEVal() ) c1 = "grey";
else c1 = (pVar<1) ? "red" : "green";','JavaLikeCalc.JavaScript
if( pVar.isEVal() ) c1 = "grey";
else c1 = (pVar<1) ? "red" : "green";','JavaLikeCalc.JavaScript
if( pVar.isEVal() ) c1 = "grey";
else c1 = (pVar<1) ? "red" : "green";',1000,'name;geomW;geomH;fillColor;elLst;c1;','');
CREATE TABLE IF NOT EXISTS 'prj_AGLKS_uio' ("IDW" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"IDC" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"IO_TYPE" INTEGER DEFAULT '' ,"IO_VAL" TEXT DEFAULT '' ,"SELF_FLG" INTEGER DEFAULT '' ,"CFG_TMPL" TEXT DEFAULT '' ,"CFG_VAL" TEXT DEFAULT '' , PRIMARY KEY ("IDW","ID","IDC"));
INSERT INTO prj_AGLKS_uio VALUES('/prj_AGLKS/pg_so','statLine','','Status line items',131205,'',128,'','');
INSERT INTO prj_AGLKS_uio VALUES('/prj_AGLKS/pg_so','runWin','','Run window',131089,'0|0;1;2|Original size;Maximize;Full screen',128,'','');
INSERT INTO prj_AGLKS_uio VALUES('/prj_AGLKS/pg_so','keepAspectRatio','','Keep aspect ratio on scale',131072,'1',128,'','');
INSERT INTO prj_AGLKS_uio VALUES('/prj_AGLKS/pg_so','stBarNoShow','','Not show status bar',131072,'0',128,'','');
INSERT INTO prj_AGLKS_uio VALUES('/prj_AGLKS/pg_so','winPosCntrSave','','Windows position control and save',131072,'1',128,'','');
COMMIT;
