PRAGMA foreign_keys=OFF;
BEGIN TRANSACTION;
CREATE TABLE 'VCALibs' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"DB_TBL" TEXT DEFAULT '' ,"ICO" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO "VCALibs" VALUES('test','Tests','Tests library','wlb_test','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','Тести','Тестова бібліотека','Тесты','Тестовая библиотека');
CREATE TABLE 'wlb_test_incl' ("IDW" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"PARENT" TEXT DEFAULT '' ,"ATTRS" TEXT DEFAULT '' ,"DBV" INTEGER DEFAULT '' , PRIMARY KEY ("IDW","ID"));
INSERT INTO "wlb_test_incl" VALUES('kompr','lkr1','/wlb_originals/wdg_Text','geomX;geomY;geomW;geomH;font;color;alignment;text;',2);
INSERT INTO "wlb_test_incl" VALUES('kompr','kr1','/wlb_originals/wdg_FormEl','active;geomX;geomY;geomW;geomH;value;view;',2);
INSERT INTO "wlb_test_incl" VALUES('kompr','FormEl1','/wlb_originals/wdg_FormEl','active;geomX;geomY;geomW;geomH;view;',2);
INSERT INTO "wlb_test_incl" VALUES('kompr','FormEl2','/wlb_originals/wdg_FormEl','active;geomX;geomY;geomW;geomH;view;',2);
INSERT INTO "wlb_test_incl" VALUES('kompr','tr1','/wlb_originals/wdg_Diagram','geomX;geomY;geomW;geomH;trcPer;sclHor;sclVer;parNum;prm0addr;prm0bordU;prm0color;prm1addr;prm1bordU;prm1color;prm2addr;prm2bordU;prm2color;',2);
INSERT INTO "wlb_test_incl" VALUES('protTest','prt1','/wlb_originals/wdg_Protocol','geomX;geomY;geomW;geomH;geomZ;time;tSize;col;',2);
INSERT INTO "wlb_test_incl" VALUES('protTest','prt2','/wlb_originals/wdg_Protocol','geomX;geomY;geomW;geomH;tSize;trcPer;viewOrd;itProp;it0lev;it0fnt;it0color;it1lev;it1fnt;it1color;',2);
INSERT INTO "wlb_test_incl" VALUES('test1','media1','/wlb_originals/wdg_Media','geomX;geomY;geomW;geomH;backColor;src;fit;type;',2);
INSERT INTO "wlb_test_incl" VALUES('test1','tr1','/wlb_originals/wdg_Diagram','geomX;geomY;geomW;geomH;bordColor;tSize;sclHor;sclVer;sclMarkFont;parNum;prm0addr;prm0bordU;prm0color;prm1addr;prm1bordU;prm1color;',2);
INSERT INTO "wlb_test_incl" VALUES('test1','Text2','/wlb_originals/wdg_Text','geomX;geomY;alignment;',2);
INSERT INTO "wlb_test_incl" VALUES('test1','newText','/wlb_originals/wdg_Text','geomX;geomY;geomW;geomH;geomZ;bordWidth;alignment;text;numbArg;arg0tp;arg0val;arg0cfg;',2);
INSERT INTO "wlb_test_incl" VALUES('DiagramTest','tr1','/wlb_originals/wdg_Diagram','perm;active;geomX;geomY;geomH;backColor;bordWidth;bordColor;trcPer;prm0addr;prm0bordU;prm0color;prm0val;prm0prop;curSek;',2);
INSERT INTO "wlb_test_incl" VALUES('DiagramTest','tr2','/wlb_originals/wdg_Diagram','geomX;geomY;geomW;geomH;geomZ;trcPer;tSize;sclHor;sclVer;parNum;prm0addr;prm0bordU;prm0color;prm0prop;prm1addr;prm1color;prm1prop;prm2addr;prm2bordU;prm2color;',2);
INSERT INTO "wlb_test_incl" VALUES('FormElTests','lst1','/wlb_originals/wdg_FormEl','perm;active;geomX;geomY;geomW;geomH;geomZ;elType;value;items;',2);
INSERT INTO "wlb_test_incl" VALUES('FormElTests','FormEl1','/wlb_originals/wdg_FormEl','perm;active;geomX;geomY;geomW;geomH;geomZ;elType;value;',2);
INSERT INTO "wlb_test_incl" VALUES('FormElTests','FormEl2','/wlb_originals/wdg_FormEl','perm;geomX;geomY;geomW;geomH;geomZ;elType;value;',2);
INSERT INTO "wlb_test_incl" VALUES('FormElTests','FormEl3','/wlb_originals/wdg_FormEl','perm;geomX;geomY;geomW;geomH;geomZ;elType;cfg;',2);
INSERT INTO "wlb_test_incl" VALUES('FormElTests','FormEl4','/wlb_originals/wdg_FormEl','perm;geomX;geomY;geomW;geomH;geomZ;elType;cfg;',2);
INSERT INTO "wlb_test_incl" VALUES('FormElTests','comb1','/wlb_originals/wdg_FormEl','perm;active;geomX;geomY;geomW;geomH;geomZ;elType;value;items;',2);
INSERT INTO "wlb_test_incl" VALUES('FormElTests','chk1','/wlb_originals/wdg_FormEl','perm;name;active;geomX;geomY;geomW;geomH;geomZ;elType;value;',2);
INSERT INTO "wlb_test_incl" VALUES('FormElTests','text1','/wlb_originals/wdg_FormEl','perm;active;geomX;geomY;geomW;geomH;geomZ;elType;value;',2);
INSERT INTO "wlb_test_incl" VALUES('FormElTests','line1','/wlb_originals/wdg_FormEl','perm;active;geomX;geomY;geomW;geomH;value;',2);
INSERT INTO "wlb_test_incl" VALUES('FormElTests','text2','/wlb_originals/wdg_FormEl','perm;active;geomX;geomY;geomW;geomH;geomZ;elType;value;wordWrap;',2);
INSERT INTO "wlb_test_incl" VALUES('FormElTests','line2','/wlb_originals/wdg_FormEl','perm;active;geomX;geomY;geomW;geomH;geomZ;elType;value;view;cfg;',2);
INSERT INTO "wlb_test_incl" VALUES('FormElTests','line3','/wlb_originals/wdg_FormEl','perm;active;geomX;geomY;geomW;geomH;geomZ;elType;value;view;cfg;',2);
INSERT INTO "wlb_test_incl" VALUES('FormElTests','line4','/wlb_originals/wdg_FormEl','perm;active;geomX;geomY;geomW;geomH;geomZ;value;view;cfg;',2);
INSERT INTO "wlb_test_incl" VALUES('FormElTests','line5','/wlb_originals/wdg_FormEl','perm;active;geomX;geomY;geomW;geomH;geomZ;value;view;',2);
INSERT INTO "wlb_test_incl" VALUES('FormElTests','line6','/wlb_originals/wdg_FormEl','perm;active;geomX;geomY;geomW;geomH;geomZ;value;view;',2);
INSERT INTO "wlb_test_incl" VALUES('FormElTests','line7','/wlb_originals/wdg_FormEl','perm;active;geomX;geomY;geomW;geomH;geomZ;elType;value;view;cfg;',2);
INSERT INTO "wlb_test_incl" VALUES('FormElTests','bt1','/wlb_originals/wdg_FormEl','perm;name;active;geomX;geomY;geomW;geomH;geomZ;elType;color;',2);
INSERT INTO "wlb_test_incl" VALUES('FormElTests','bt2','/wlb_originals/wdg_FormEl','perm;name;active;geomX;geomY;geomW;geomH;geomZ;elType;value;mode;',2);
INSERT INTO "wlb_test_incl" VALUES('FormElTests','bt3','/wlb_originals/wdg_FormEl','perm;name;active;geomX;geomY;geomW;geomH;geomZ;elType;color;',2);
INSERT INTO "wlb_test_incl" VALUES('FormElTests','bt4','/wlb_originals/wdg_FormEl','perm;active;geomX;geomY;geomW;geomH;geomZ;elType;img;',2);
INSERT INTO "wlb_test_incl" VALUES('TextEltests','text1','/wlb_originals/wdg_Text','geomX;geomY;geomW;geomH;geomZ;alignment;text;',2);
INSERT INTO "wlb_test_incl" VALUES('TextEltests','text2','/wlb_originals/wdg_Text','geomX;geomY;geomW;geomH;geomZ;font;alignment;text;',2);
INSERT INTO "wlb_test_incl" VALUES('TextEltests','text3','/wlb_originals/wdg_Text','geomX;geomY;geomW;geomH;geomZ;font;alignment;text;',2);
INSERT INTO "wlb_test_incl" VALUES('TextEltests','text5','/wlb_originals/wdg_Text','geomX;geomY;geomW;geomH;geomZ;font;color;orient;alignment;text;',2);
INSERT INTO "wlb_test_incl" VALUES('TextEltests','text6','/wlb_originals/wdg_Text','geomX;geomY;geomW;geomH;backColor;font;color;orient;alignment;text;',2);
INSERT INTO "wlb_test_incl" VALUES('TextEltests','text7','/wlb_originals/wdg_Text','geomX;geomY;geomW;geomH;geomZ;backImg;bordWidth;alignment;text;',2);
INSERT INTO "wlb_test_incl" VALUES('MediaTests','fire','/wlb_originals/wdg_Media','geomX;geomY;geomZ;backColor;src;type;',2);
INSERT INTO "wlb_test_incl" VALUES('MediaTests','mov1','/wlb_originals/wdg_Media','geomX;geomY;geomW;geomH;geomZ;backColor;src;type;speed;',2);
INSERT INTO "wlb_test_incl" VALUES('MediaTests','bake','/wlb_originals/wdg_Media','geomX;geomY;geomW;geomH;backColor;src;',2);
INSERT INTO "wlb_test_incl" VALUES('MediaTests','column','/wlb_originals/wdg_Media','active;geomX;geomY;geomW;geomH;geomZ;backColor;src;fit;areas;area0coord;area0title;',2);
INSERT INTO "wlb_test_incl" VALUES('mn_gen','Pi_нд','/wlb_Main/wdg_anShow','name;geomX;geomY;geomZ;',2);
INSERT INTO "wlb_test_incl" VALUES('mn_gen','FormEl','/wlb_test/wdg_FormElTests','geomX;geomY;geomZ;',2);
INSERT INTO "wlb_test_incl" VALUES('mn_gen','diagTests','/wlb_test/wdg_DiagramTest','geomX;geomY;geomZ;',2);
INSERT INTO "wlb_test_incl" VALUES('mn_gen','TextEl','/wlb_test/wdg_TextEltests','geomX;geomY;geomZ;',2);
INSERT INTO "wlb_test_incl" VALUES('mn_gen','media','/wlb_test/wdg_MediaTests','geomX;geomY;',2);
INSERT INTO "wlb_test_incl" VALUES('mn_gen','Ti_нд','/wlb_Main/wdg_anShow','geomX;geomY;geomZ;',2);
INSERT INTO "wlb_test_incl" VALUES('mn_gen','prescrRunSimple','/wlb_Main/wdg_prescrRunSimple','geomX;geomY;geomXsc;geomYsc;geomZ;',0);
INSERT INTO "wlb_test_incl" VALUES('ElFigureTests','ElFigure','/wlb_originals/wdg_ElFigure','geomX;geomY;geomW;geomH;lineWdth;elLst;','');
INSERT INTO "wlb_test_incl" VALUES('mn_gen','ElFigureTests1','/wlb_test/wdg_ElFigureTests','geomX;geomY;geomW;','');
INSERT INTO "wlb_test_incl" VALUES('mn_gen','prt','/wlb_originals/wdg_Protocol','name;geomX;geomY;geomW;geomH;geomZ;tSize;trcPer;viewOrd;col;itProp;it0lev;it0fnt;it0color;it1lev;it1fnt;it1color;','');
CREATE TABLE 'wlb_test_io' ("IDW" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"IO_VAL" TEXT DEFAULT '' ,"SELF_FLG" INTEGER DEFAULT '' ,"CFG_TMPL" TEXT DEFAULT '' ,"CFG_VAL" TEXT DEFAULT '' ,"IDC" TEXT DEFAULT '' ,"uk#IO_VAL" TEXT DEFAULT '' ,"uk#CFG_TMPL" TEXT DEFAULT '' ,"ru#IO_VAL" TEXT DEFAULT '' ,"ru#CFG_TMPL" TEXT DEFAULT '' ,"ru#CFG_VAL" TEXT DEFAULT '' ,"uk#CFG_VAL" TEXT DEFAULT '' , PRIMARY KEY ("IDW","ID","IDC"));
INSERT INTO "wlb_test_io" VALUES('kompr','geomW','584',32,'','','','','','','','','');
INSERT INTO "wlb_test_io" VALUES('kompr','geomH','341',32,'','','','','','','','','');
INSERT INTO "wlb_test_io" VALUES('kompr','backColor','grey',32,'','','','','','','','','');
INSERT INTO "wlb_test_io" VALUES('kompr','bordWidth','2',32,'','','','','','','','','');
INSERT INTO "wlb_test_io" VALUES('kompr','sclHor','1',32,'','','tr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('kompr','geomY','13',32,'','','tr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('kompr','geomW','510',32,'','','tr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('kompr','geomH','250',32,'','','tr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('kompr','trcPer','1',32,'','','tr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('kompr','active','1',32,'','','kr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('kompr','value','0',6,'','','kr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('kompr','view','3',32,'','','kr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('kompr','geomX','222',32,'','','FormEl1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('kompr','geomY','265',32,'','','FormEl1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('kompr','geomW','80',32,'','','FormEl1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('kompr','geomH','30',32,'','','FormEl1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('kompr','active','1',32,'','','FormEl1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('kompr','view','3',32,'','','FormEl1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('kompr','geomX','306',32,'','','FormEl2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('kompr','geomY','265',32,'','','FormEl2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('kompr','geomW','80',32,'','','FormEl2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('kompr','geomH','30',32,'','','FormEl2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('kompr','active','1',32,'','','FormEl2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('kompr','view','3',32,'','','FormEl2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('kompr','geomX','17',32,'','','tr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('kompr','geomH','30',32,'','','kr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('kompr','geomY','265',32,'','','kr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('kompr','geomW','80',32,'','','kr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('kompr','geomX','19.3',32,'','','lkr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('kompr','geomY','265',32,'','','lkr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('kompr','geomX','138',32,'','','kr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('kompr','text','Crane position:',32,'','','lkr1','Положення крану:','','Положение крана:','','','');
INSERT INTO "wlb_test_io" VALUES('kompr','alignment','8',32,'','','lkr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('kompr','color','#000000',32,'','','lkr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('kompr','font','Arial 10',32,'','','lkr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('kompr','geomH','30',32,'','','lkr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('kompr','geomW','120.41',32,'','','lkr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('protTest','name','Tests "Protocol"',32,'','','','Тести "Протокол"','','Тесты "Протокол"','','','');
INSERT INTO "wlb_test_io" VALUES('protTest','geomW','751',32,'','','','','','','','','');
INSERT INTO "wlb_test_io" VALUES('protTest','geomH','509',32,'','','','','','','','','');
INSERT INTO "wlb_test_io" VALUES('protTest','backColor','#5A5A5A',32,'','','','','','','','','');
INSERT INTO "wlb_test_io" VALUES('protTest','bordWidth','1',32,'','','','','','','','','');
INSERT INTO "wlb_test_io" VALUES('protTest','geomY','262',32,'','','prt2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('protTest','col','lev;tm;mess',32,'','','prt1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('protTest','geomX','5',32,'','','prt2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('protTest','tSize','360',32,'','','prt1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('protTest','time','1196691650',32,'','','prt1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('protTest','geomW','736',32,'','','prt1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('protTest','geomH','251',32,'','','prt1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('protTest','geomY','7',32,'','','prt1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('protTest','geomX','5',32,'','','prt1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('test1','en','1',38,'Test|active','','','','','','','','');
INSERT INTO "wlb_test_io" VALUES('test1','geomW','650',32,'','','','','','','','','');
INSERT INTO "wlb_test_io" VALUES('test1','geomH','443',32,'','','','','','','','','');
INSERT INTO "wlb_test_io" VALUES('test1','geomMargin','5',32,'','','','','','','','','');
INSERT INTO "wlb_test_io" VALUES('test1','active','0',41,'','','','','','','','','');
INSERT INTO "wlb_test_io" VALUES('test1','backColor','green',32,'','','','','','','','','');
INSERT INTO "wlb_test_io" VALUES('test1','backImg','help',32,'','','','','','','','','');
INSERT INTO "wlb_test_io" VALUES('test1','prm1color','red',0,'','','tr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('test1','prm1addr','/DAQ/ModBus/Unidrive/pos/enc',0,'','','tr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('test1','prm1bordU','100',0,'','','tr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('test1','prm0color','yellow',32,'','','tr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('test1','geomW','603',32,'','','tr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('test1','geomH','259',32,'','','tr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('test1','bordColor','red',32,'','','tr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('test1','geomX','87',32,'','','media1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('test1','geomY','145',32,'','','media1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('test1','geomW','61',32,'','','media1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('test1','geomH','87',32,'','','media1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('test1','backColor','',32,'','','media1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('test1','src','mov',32,'','','media1','','','mov','','','');
INSERT INTO "wlb_test_io" VALUES('test1','fit','1',32,'','','media1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('test1','type','1',32,'','','media1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('test1','geomX','12',32,'','','tr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('test1','geomY','151',32,'','','tr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('test1','prm0addr','/DAQ/ModBus/Unidrive/pos/pos',32,'','','tr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('test1','prm0bordU','100',32,'','','tr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('test1','parNum','2',32,'','','tr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('test1','tSize','10',32,'','','tr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('test1','sclHor','1',32,'','','tr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('test1','sclVer','1',32,'','','tr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('test1','sclMarkFont','Arial 8',32,'','','tr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('DiagramTest','name','Tests "Diagrams"',32,'','','','Тести "Діаграма"','','Тесты "Диаграмма"','','','');
INSERT INTO "wlb_test_io" VALUES('DiagramTest','geomW','450',32,'','','','','','','','','');
INSERT INTO "wlb_test_io" VALUES('DiagramTest','geomH','150',32,'','','','','','','','','');
INSERT INTO "wlb_test_io" VALUES('DiagramTest','backColor','lightblue',32,'','','','','','','','','');
INSERT INTO "wlb_test_io" VALUES('DiagramTest','bordWidth','1',32,'','','','','','','','','');
INSERT INTO "wlb_test_io" VALUES('DiagramTest','bordColor','black',32,'','','','','','','','','');
INSERT INTO "wlb_test_io" VALUES('DiagramTest','tSize','120',32,'','','tr2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('DiagramTest','trcPer','1',32,'','','tr2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('DiagramTest','sclVer','3',32,'','','tr2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('DiagramTest','sclHor','2',32,'','','tr2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('DiagramTest','geomX','105',32,'','','tr2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('DiagramTest','geomY','3',32,'','','tr2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('DiagramTest','geomW','342',32,'','','tr2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('DiagramTest','geomH','144',32,'','','tr2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('DiagramTest','curSek','1374518072',0,'','','tr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('DiagramTest','prm0val','53.2663',32,'','','tr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('DiagramTest','geomX','3',32,'','','tr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('DiagramTest','geomY','3',32,'','','tr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('DiagramTest','active','1',32,'','','tr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('DiagramTest','backColor','grey',32,'','','tr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('DiagramTest','bordWidth','1',32,'','','tr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('DiagramTest','bordColor','black',32,'','','tr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('DiagramTest','trcPer','1',32,'','','tr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('DiagramTest','prm0addr','/DAQ/System/AutoDA/CPULoad/load',32,'','','tr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('DiagramTest','prm0bordU','100',32,'','','tr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('DiagramTest','prm0color','lightgreen',32,'','','tr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','name','Tests "FormEl"',32,'','','','Тести "FormEl"','','Тесты "FormEl"','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomW','447',32,'','','','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomH','253',32,'','','','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','backColor','#EEEEFF',32,'','','','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','bordWidth','2',32,'','','','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','bordColor','blue',32,'','','','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','elType','1',32,'','','text1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomZ','7',32,'','','text1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','active','1',32,'','','text1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomH','100',32,'','','text1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','value','Text editor with words wrapping',0,'','','text1','Текстовий редактор з переносом за словами','','Текстовый редактор с переносом по словам','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','elType','7',32,'','','FormEl2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','value','30',0,'','','FormEl2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomX','413',32,'','','FormEl3','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomY','10',32,'','','FormEl3','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomW','30',32,'','','FormEl3','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomH','110',32,'','','FormEl3','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomZ','10',32,'','','FormEl3','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','elType','6',32,'','','FormEl3','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','cfg','1:0:100:1:10',0,'','','FormEl3','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomX','413',32,'','','FormEl4','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomY','132',32,'','','FormEl4','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomW','30',32,'','','FormEl4','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomH','109',32,'','','FormEl4','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomZ','16',32,'','','FormEl4','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','elType','7',32,'','','FormEl4','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','cfg','1:0:100:1:10',0,'','','FormEl4','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomX','4',32,'','','comb1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomY','226',32,'','','comb1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomW','88',32,'','','comb1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomH','20',32,'','','comb1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomZ','17',32,'','','comb1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','active','1',32,'','','comb1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','elType','4',32,'','','comb1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','value','Value3',0,'','','comb1','Значення3','','Значение3','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','items','Value 1
Value 2
Value3
Value4
Value5',0,'','','comb1','Значення 1
Значення 2
Значення3
Значення4
Значення5','','Значение 1
Значение 2
Значение3
Значение4
Значение5','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','name','Flag',32,'','','chk1','Прапорець','','Флажок','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomX','4',32,'','','chk1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomY','161',32,'','','chk1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomW','107',32,'','','chk1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomH','20',32,'','','chk1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomZ','11',32,'','','chk1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','active','1',32,'','','chk1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','elType','2',32,'','','chk1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomX','4',32,'','','text1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomY','58',32,'','','text1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomW','150',32,'','','text1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomZ','19',32,'','','FormEl2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomW','100',32,'','','FormEl2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomH','20',32,'','','FormEl2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomY','226',32,'','','FormEl2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomX','98',32,'','','FormEl1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomY','226',32,'','','FormEl1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomW','100',32,'','','FormEl1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomH','20',32,'','','FormEl1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomZ','18',32,'','','FormEl1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','elType','6',32,'','','FormEl1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','value','50',0,'','','FormEl1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomX','205',32,'','','FormEl2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','items','Val1
Val2
Val3
Val4',0,'','','lst1','Знач1
Знач2
Знач3
Знач4','','Знач1
Знач2
Знач3
Знач4','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','elType','5',32,'','','lst1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','value','Val2',0,'','','lst1','Знач2','','Знач2','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','active','1',32,'','','lst1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomY','58',32,'','','lst1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomW','100',32,'','','lst1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomH','100',32,'','','lst1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomZ','9',32,'','','lst1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomX','309',32,'','','lst1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomW','110',32,'','','line6','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomH','20',32,'','','line6','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomY','34',32,'','','line6','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomX','172',32,'','','line6','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','view','5',32,'','','line5','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomZ','5',32,'','','line5','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','active','1',32,'','','line5','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomH','20',32,'','','line5','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomW','78',32,'','','line5','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomX','92',32,'','','line5','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomY','34',32,'','','line5','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','cfg','h:mm:ss',32,'','','line4','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','value','675',0,'','','line4','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','view','4',32,'','','line4','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','active','1',32,'','','line4','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomZ','4',32,'','','line4','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomH','20',32,'','','line4','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomW','85',32,'','','line4','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomX','4',32,'','','line4','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomY','34',32,'','','line4','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','cfg','1:500:0.5:: kgN/sm2:3',32,'','','line3','1:500:0.5:: кгс/см2:3','','1:500:0.5:: кгс/см2:3','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','view','3',32,'','','line3','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','value','0',8,'','','line3','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomZ','2',32,'','','line3','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','active','1',32,'','','line3','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomH','20',32,'','','line3','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomW','110',32,'','','line3','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomY','9',32,'','','line3','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','cfg','10:120:1:: tone/hour',32,'','','line2','0:120:1:: т/год','','10:120:1:: т/ч','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomX','186',32,'','','line3','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','view','2',32,'','','line2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','value','0',14,'','wdg:../a_extValue','line2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomX','4',32,'','','line1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomY','9',32,'','','line1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomW','100',32,'','','line1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomH','20',32,'','','line1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','active','1',32,'','','line1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','value','Edit line',0,'','','line1','Рядок редагування','','Строка редактирования','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomX','154',32,'','','text2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomY','58',32,'','','text2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomW','150',32,'','','text2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomH','100',32,'','','text2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomZ','8',32,'','','text2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','active','1',32,'','','text2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','elType','1',32,'','','text2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','value','Text editor without word wrapping',0,'','','text2','Текстовий редактор без переноса слів','','Текстовый редактор без переноса слов','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','wordWrap','0',0,'','','text2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomX','106',32,'','','line2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomY','9',32,'','','line2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomW','77',32,'','','line2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomH','20',32,'','','line2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomZ','1',32,'','','line2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','active','1',32,'','','line2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('TextEltests','name','Tests "Text"',32,'','','','Тести "Текст"','','Тесты "Текст"','','','');
INSERT INTO "wlb_test_io" VALUES('TextEltests','geomW','157',32,'','','','','','','','','');
INSERT INTO "wlb_test_io" VALUES('TextEltests','geomH','108',32,'','','','','','','','','');
INSERT INTO "wlb_test_io" VALUES('TextEltests','backColor','ivory',32,'','','','','','','','','');
INSERT INTO "wlb_test_io" VALUES('TextEltests','bordWidth','2',32,'','','','','','','','','');
INSERT INTO "wlb_test_io" VALUES('TextEltests','bordColor','blue',32,'','','','','','','','','');
INSERT INTO "wlb_test_io" VALUES('TextEltests','orient','180',32,'','','text6','','','','','','');
INSERT INTO "wlb_test_io" VALUES('TextEltests','text','180 degr. and strike out and red background',32,'','','text6','180 град перекреслений червоний фон','','180 град перечеркн красн. фон','','','');
INSERT INTO "wlb_test_io" VALUES('TextEltests','color','black',32,'','','text6','','','','','','');
INSERT INTO "wlb_test_io" VALUES('TextEltests','font','Arial 8 0 0 0 1',32,'','','text6','','','','','','');
INSERT INTO "wlb_test_io" VALUES('TextEltests','geomY','53',32,'','','text6','','','','','','');
INSERT INTO "wlb_test_io" VALUES('TextEltests','geomW','50',32,'','','text6','','','','','','');
INSERT INTO "wlb_test_io" VALUES('TextEltests','backColor','red',32,'','','text6','','','','','','');
INSERT INTO "wlb_test_io" VALUES('TextEltests','geomX','3',32,'','','text6','','','','','','');
INSERT INTO "wlb_test_io" VALUES('TextEltests','color','blue',32,'','','text5','','','','','','');
INSERT INTO "wlb_test_io" VALUES('TextEltests','orient','90',32,'','','text5','','','','','','');
INSERT INTO "wlb_test_io" VALUES('TextEltests','text','90 degr. underscore blue',32,'','','text5','90 град підкреслений блакитний','','90 град подчёркнутый синий','','','');
INSERT INTO "wlb_test_io" VALUES('TextEltests','geomW','50',32,'','','text5','','','','','','');
INSERT INTO "wlb_test_io" VALUES('TextEltests','font','Arial 11 0 0 1',32,'','','text5','','','','','','');
INSERT INTO "wlb_test_io" VALUES('TextEltests','geomY','53',32,'','','text5','','','','','','');
INSERT INTO "wlb_test_io" VALUES('TextEltests','text','Italic type to right',32,'','','text3','Курсив праворуч','','Курсив с права','','','');
INSERT INTO "wlb_test_io" VALUES('TextEltests','geomX','103',32,'','','text5','','','','','','');
INSERT INTO "wlb_test_io" VALUES('TextEltests','geomW','50',32,'','','text3','','','','','','');
INSERT INTO "wlb_test_io" VALUES('TextEltests','font','Arial 11 0 1',32,'','','text3','','','','','','');
INSERT INTO "wlb_test_io" VALUES('TextEltests','alignment','9',32,'','','text3','','','','','','');
INSERT INTO "wlb_test_io" VALUES('TextEltests','geomY','3',32,'','','text3','','','','','','');
INSERT INTO "wlb_test_io" VALUES('TextEltests','text','Bolded to center',32,'','','text2','Жирний по центру','','Жирный по центру','','','');
INSERT INTO "wlb_test_io" VALUES('TextEltests','geomX','103',32,'','','text3','','','','','','');
INSERT INTO "wlb_test_io" VALUES('TextEltests','alignment','10',32,'','','text2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('TextEltests','geomY','3',32,'','','text2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('TextEltests','geomW','50',32,'','','text2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('TextEltests','font','Arial 11 1',32,'','','text2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('TextEltests','geomX','53',32,'','','text2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('TextEltests','alignment','8',32,'','','text1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('TextEltests','text','Simple text',32,'','','text1','Простий текст','','Простой текст','','','');
INSERT INTO "wlb_test_io" VALUES('TextEltests','geomX','3',32,'','','text1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('TextEltests','geomY','3',32,'','','text1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('TextEltests','geomW','50',32,'','','text1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('MediaTests','name','Tests "Media"',32,'','','','Тести "Медіа"','','Тесты "Медиа"','','','');
INSERT INTO "wlb_test_io" VALUES('MediaTests','geomW','230',32,'','','','','','','','','');
INSERT INTO "wlb_test_io" VALUES('MediaTests','geomH','203',32,'','','','','','','','','');
INSERT INTO "wlb_test_io" VALUES('MediaTests','backColor','ivory',32,'','','','','','','','','');
INSERT INTO "wlb_test_io" VALUES('MediaTests','bordWidth','2',32,'','','','','','','','','');
INSERT INTO "wlb_test_io" VALUES('MediaTests','bordColor','blue',32,'','','','','','','','','');
INSERT INTO "wlb_test_io" VALUES('MediaTests','geomX','3',32,'','','column','','','','','','');
INSERT INTO "wlb_test_io" VALUES('MediaTests','src','bake',32,'','','bake','','','','','','');
INSERT INTO "wlb_test_io" VALUES('MediaTests','backColor','',32,'','','bake','','','','','','');
INSERT INTO "wlb_test_io" VALUES('MediaTests','geomH','197',32,'','','bake','','','','','','');
INSERT INTO "wlb_test_io" VALUES('MediaTests','geomY','3',32,'','','bake','','','','','','');
INSERT INTO "wlb_test_io" VALUES('MediaTests','geomW','164',32,'','','bake','','','','','','');
INSERT INTO "wlb_test_io" VALUES('MediaTests','geomX','63',32,'','','bake','','','','','','');
INSERT INTO "wlb_test_io" VALUES('MediaTests','speed','100',8,'','','mov1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('MediaTests','geomX','96',32,'','','fire','','','','','','');
INSERT INTO "wlb_test_io" VALUES('MediaTests','geomY','100',32,'','','fire','','','','','','');
INSERT INTO "wlb_test_io" VALUES('MediaTests','geomZ','2',32,'','','fire','','','','','','');
INSERT INTO "wlb_test_io" VALUES('MediaTests','backColor','',32,'','','fire','','','','','','');
INSERT INTO "wlb_test_io" VALUES('MediaTests','src','fire',32,'','','fire','','','','','','');
INSERT INTO "wlb_test_io" VALUES('MediaTests','type','1',32,'','','fire','','','','','','');
INSERT INTO "wlb_test_io" VALUES('MediaTests','geomX','3',32,'','','mov1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('MediaTests','geomY','132',32,'','','mov1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('MediaTests','geomW','67',32,'','','mov1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('MediaTests','geomH','68',32,'','','mov1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('MediaTests','geomZ','3',32,'','','mov1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('MediaTests','backColor','',32,'','','mov1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('MediaTests','src','pump',40,'','','mov1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('MediaTests','type','1',32,'','','mov1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','name','Tests set',32,'','','','Збірка тестів','','Сборка тестов','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','dscr','Mnemo: "Tests set"',32,'','','','Мнемосхема: "Збірка тестів"','','Мнемосхема: "Сборка тестов"','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','geomX','0',40,'','','','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','geomW','900',32,'','','','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','geomH','630',32,'','','','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','backColor','#5A5A5A',32,'','','','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','bordWidth','1',32,'','','','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','bordColor','black',32,'','','','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','geomY','258',32,'','','media','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','geomW','283',32,'','','media','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','geomH','227',32,'','','media','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','geomZ','3',32,'','','diagTests','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','geomX','219',32,'','','media','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','geomY','2',32,'','','TextEl','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','geomX','449',32,'','','diagTests','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','geomY','202',32,'','','diagTests','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','geomW','461',32,'','','diagTests','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','geomH','170',32,'','','diagTests','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','geomX','450',32,'','','TextEl','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','geomW','459',32,'','','FormEl','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','geomH','341',32,'','','FormEl','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','geomY','2',32,'','','FormEl','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','geomX','2',32,'','','FormEl','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','name','Pi_нд',32,'','','Pi_нд','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','geomX','451',32,'','','Pi_нд','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','geomY','113',32,'','','Pi_нд','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','pName','',34,'Параметр|NAME','prm:/LogicLev/experiment/F3/NAME','Pi_нд','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','pVal','0',34,'Параметр|var','prm:/LogicLev/experiment/F3/var','Pi_нд','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','value','1182710881',0,'','','line5','','','','','','');
INSERT INTO "wlb_test_io" VALUES('kompr','sclVer','1',32,'','','tr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('kompr','parNum','3',32,'','','tr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('kompr','prm0addr','/DAQ/BlockCalc/KM101/F101/Fi',32,'','','tr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('kompr','prm0bordU','100',32,'','','tr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('kompr','prm0color','blue',32,'','','tr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('kompr','prm1addr','/DAQ/BlockCalc/KM101/F102/Fi',0,'','','tr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('kompr','prm1bordU','100',0,'','','tr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('kompr','prm1color','lightblue',0,'','','tr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('kompr','prm2addr','/DAQ/BlockCalc/KM101/MKPP101/l_kl1',0,'','','tr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('kompr','prm2bordU','100',0,'','','tr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('kompr','prm2color','lightgreen',0,'','','tr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('protTest','geomW','735',32,'','','prt2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('protTest','geomH','237',32,'','','prt2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('protTest','tSize','360',32,'','','prt2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('protTest','trcPer','1',32,'','','prt2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','geomX','451',32,'','','Ti_нд','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','geomY','158',32,'','','Ti_нд','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','pName','',34,'Параметр|NAME','prm:/LogicLev/experiment/P3/NAME','Ti_нд','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','pVal','0',34,'Параметр|var','prm:/LogicLev/experiment/P3/var','Ti_нд','','','','','','');
INSERT INTO "wlb_test_io" VALUES('test1','geomX','212',32,'','','Text2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('test1','geomY','70',32,'','','Text2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('test1','geomX','81',32,'','','newText','','','','','','');
INSERT INTO "wlb_test_io" VALUES('test1','geomY','85',32,'','','newText','','','','','','');
INSERT INTO "wlb_test_io" VALUES('test1','geomW','85',32,'','','newText','','','','','','');
INSERT INTO "wlb_test_io" VALUES('test1','geomH','42',32,'','','newText','','','','','','');
INSERT INTO "wlb_test_io" VALUES('test1','geomZ','1',32,'','','newText','','','','','','');
INSERT INTO "wlb_test_io" VALUES('test1','bordWidth','2',32,'','','newText','','','','','','');
INSERT INTO "wlb_test_io" VALUES('test1','text','Test: %1;',32,'','','newText','Тест: %1;','','Тест: %1;','','','');
INSERT INTO "wlb_test_io" VALUES('test1','numbArg','1',32,'','','newText','','','','','','');
INSERT INTO "wlb_test_io" VALUES('test1','arg0tp','1',0,'','','newText','','','','','','');
INSERT INTO "wlb_test_io" VALUES('test1','arg0val','34.12',0,'','','newText','','','','','','');
INSERT INTO "wlb_test_io" VALUES('test1','arg0cfg','5;f;1',0,'','','newText','','','','','','');
INSERT INTO "wlb_test_io" VALUES('DiagramTest','parNum','3',32,'','','tr2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('DiagramTest','prm0addr','/DAQ/System/AutoDA/CPULoad/load',32,'','','tr2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('DiagramTest','prm0bordU','100',32,'','','tr2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('DiagramTest','prm0color','yellow',32,'','','tr2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('DiagramTest','prm1addr','/DAQ/System/AutoDA/MemInfo/use',0,'','','tr2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('DiagramTest','prm1color','red',0,'','','tr2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('DiagramTest','prm2addr','/DAQ/System/AutoDA/SensorsData/TEMP0',0,'','','tr2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('DiagramTest','prm2bordU','100',0,'','','tr2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('DiagramTest','prm2color','lightgreen',0,'','','tr2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomZ','6',32,'','','line6','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','active','1',32,'','','line6','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','value','1182710881',0,'','','line6','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','view','6',32,'','','line6','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomX','299',32,'','','line7','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomY','9',32,'','','line7','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomW','100',32,'','','line7','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomH','20',32,'','','line7','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomZ','3',32,'','','line7','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','active','1',32,'','','line7','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','value','Element',0,'','','line7','Елемент','','Элемент','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','view','1',32,'','','line7','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','cfg','El 1
El 2
El 3
El 4',32,'','','line7','El 1
El 2
El 3
El 4','','El 1
El 2
El 3
El 4','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','name','Simple button',32,'','','bt1','Проста кнопка','','Простая кнопка','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomX','4',32,'','','bt1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomY','184',32,'','','bt1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomW','100',32,'','','bt1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomH','35',32,'','','bt1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomZ','12',32,'','','bt1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','active','1',32,'','','bt1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','elType','3',32,'','','bt1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','color','',8,'','','bt1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','name','Fixed',32,'','','bt2','Фіксована','','Фиксированная','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomX','106',32,'','','bt2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomY','184',32,'','','bt2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomW','100',32,'','','bt2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomH','35',32,'','','bt2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomZ','13',32,'','','bt2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','active','1',32,'','','bt2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','elType','3',32,'','','bt2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','value','1',0,'','','bt2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','name','Colored button',32,'','','bt3','Кольорова кнопка','','Цветная кнопка','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomX','208',32,'','','bt3','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomY','184',32,'','','bt3','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomW','100',32,'','','bt3','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomH','35',32,'','','bt3','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomZ','14',32,'','','bt3','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','active','1',32,'','','bt3','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','elType','3',32,'','','bt3','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','color','green',0,'','','bt3','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomX','310',32,'','','bt4','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomY','163',32,'','','bt4','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomW','100',32,'','','bt4','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomH','81',32,'','','bt4','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','geomZ','15',32,'','','bt4','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','active','1',32,'','','bt4','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','elType','3',32,'','','bt4','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','img','test',0,'','','bt4','','','','','','');
INSERT INTO "wlb_test_io" VALUES('TextEltests','geomX','53',32,'','','text7','','','','','','');
INSERT INTO "wlb_test_io" VALUES('TextEltests','geomY','53',32,'','','text7','','','','','','');
INSERT INTO "wlb_test_io" VALUES('TextEltests','geomW','50',32,'','','text7','','','','','','');
INSERT INTO "wlb_test_io" VALUES('TextEltests','backImg','help',32,'','','text7','','','','','','');
INSERT INTO "wlb_test_io" VALUES('TextEltests','bordWidth','1',32,'','','text7','','','','','','');
INSERT INTO "wlb_test_io" VALUES('TextEltests','text','Border and image and center',32,'','','text7','Бордюр зображення центр','','Бордюр изображение центр','','','');
INSERT INTO "wlb_test_io" VALUES('MediaTests','geomY','3',32,'','','column','','','','','','');
INSERT INTO "wlb_test_io" VALUES('MediaTests','geomW','70',32,'','','column','','','','','','');
INSERT INTO "wlb_test_io" VALUES('MediaTests','geomH','117',32,'','','column','','','','','','');
INSERT INTO "wlb_test_io" VALUES('MediaTests','active','1',32,'','','column','','','','','','');
INSERT INTO "wlb_test_io" VALUES('MediaTests','backColor','',32,'','','column','','','','','','');
INSERT INTO "wlb_test_io" VALUES('MediaTests','src','column',32,'','','column','','','','','','');
INSERT INTO "wlb_test_io" VALUES('MediaTests','fit','1',32,'','','column','','','','','','');
INSERT INTO "wlb_test_io" VALUES('MediaTests','areas','1',32,'','','column','','','','','','');
INSERT INTO "wlb_test_io" VALUES('MediaTests','area0coord','10,10,30,30',0,'','','column','','','','','','');
INSERT INTO "wlb_test_io" VALUES('MediaTests','area0title','Test map',0,'','','column','Тестова карта','','Тестовая карта','','','');
INSERT INTO "wlb_test_io" VALUES('protTest','itProp','2',32,'','','prt2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('protTest','it0lev','3',0,'','','prt2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('protTest','it0fnt','Arial 11 0 1 0 0',0,'','','prt2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('protTest','it0color','yellow',0,'','','prt2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('protTest','it1lev','4',0,'','','prt2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('protTest','it1fnt','Arial 11 0 0 1 0',0,'','','prt2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('protTest','it1color','red',0,'','','prt2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('protTest','viewOrd','4',32,'','','prt2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','elType','0',32,'','','line3','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','elType','0',32,'','','line7','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','elType','0',32,'','','line2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','value','0',0,'','','chk1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('DiagramTest','perm','438',32,'','','tr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','perm','438',32,'','','FormEl1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','perm','438',32,'','','FormEl2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','perm','438',32,'','','FormEl3','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','perm','438',32,'','','FormEl4','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','perm','438',32,'','','bt1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','perm','438',32,'','','bt2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','perm','438',32,'','','bt3','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','perm','438',32,'','','bt4','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','perm','438',32,'','','chk1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','perm','438',32,'','','comb1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','perm','438',32,'','','line1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','perm','438',32,'','','line2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','perm','438',32,'','','line3','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','perm','438',32,'','','line4','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','perm','438',32,'','','line5','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','perm','438',32,'','','line6','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','perm','438',32,'','','line7','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','perm','438',32,'','','lst1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','perm','438',32,'','','text1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','perm','438',32,'','','text2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('TextEltests','geomH','50',32,'','','text1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('TextEltests','geomH','50',32,'','','text2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('TextEltests','geomH','50',32,'','','text3','','','','','','');
INSERT INTO "wlb_test_io" VALUES('TextEltests','geomH','50',32,'','','text5','','','','','','');
INSERT INTO "wlb_test_io" VALUES('TextEltests','alignment','10',32,'','','text5','','','','','','');
INSERT INTO "wlb_test_io" VALUES('TextEltests','geomH','50',32,'','','text6','','','','','','');
INSERT INTO "wlb_test_io" VALUES('TextEltests','alignment','10',32,'','','text6','','','','','','');
INSERT INTO "wlb_test_io" VALUES('TextEltests','geomH','50',32,'','','text7','','','','','','');
INSERT INTO "wlb_test_io" VALUES('TextEltests','alignment','10',32,'','','text7','','','','','','');
INSERT INTO "wlb_test_io" VALUES('test1','alignment','10',32,'','','Text2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('test1','alignment','10',32,'','','newText','','','','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','active','1',32,'','','FormEl1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('DiagramTest','prm0prop','1362235800:1374518072:1',32,'','','tr1','1362235800:1374517499:1','','1362235800:1374517910:1','','','');
INSERT INTO "wlb_test_io" VALUES('DiagramTest','prm0prop','1362235800:1374518072:1',32,'','','tr2','1362235800:1374517499:1','','1362235800:1374517910:1','','','');
INSERT INTO "wlb_test_io" VALUES('DiagramTest','prm1prop','1362235800:1374518072:1',0,'','','tr2','1362235800:1374517499:1','','1362235800:1374517910:1','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','geomZ','5',32,'','','Pi_нд','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','geomZ','4',32,'','','TextEl','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','geomZ','6',32,'','','Ti_нд','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','geomZ','4',32,'','','media','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','geomX','609',32,'','','prescrRunSimple','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','geomY','2',32,'','','prescrRunSimple','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','geomXsc','1.16',32,'','','prescrRunSimple','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','geomYsc','1.141',32,'','','prescrRunSimple','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','geomZ','7',32,'','','prescrRunSimple','','','','','','');
INSERT INTO "wlb_test_io" VALUES('ElFigureTests','geomW','159',32,'','','','','','','','','');
INSERT INTO "wlb_test_io" VALUES('ElFigureTests','geomH','300',32,'','','','','','','','','');
INSERT INTO "wlb_test_io" VALUES('ElFigureTests','backImg','greed',32,'','','','','','','','','');
INSERT INTO "wlb_test_io" VALUES('ElFigureTests','geomX','0',32,'','','ElFigure','','','','','','');
INSERT INTO "wlb_test_io" VALUES('ElFigureTests','geomY','0',32,'','','ElFigure','','','','','','');
INSERT INTO "wlb_test_io" VALUES('ElFigureTests','geomW','158',32,'','','ElFigure','','','','','','');
INSERT INTO "wlb_test_io" VALUES('ElFigureTests','geomH','300',32,'','','ElFigure','','','','','','');
INSERT INTO "wlb_test_io" VALUES('ElFigureTests','lineWdth','3',32,'','','ElFigure','','','','','','');
INSERT INTO "wlb_test_io" VALUES('ElFigureTests','elLst','arc:(70.854|149.4):(89.146|149.4):(80|80):(10|80):(80|150):5:#ffff00:2:#0000ff:
arc:(80|280):(80|280):(80|220):(20|220):(80|280):::::
line:(25|285):(140|285):::::
line:(15|275):(15|160):::::
line:(25|275):(85|215):::::
line:(20|270):(80|210):::::1
line:(30|280):(90|220):::::2
arc:(80|270):(80|270):(80|220):(30|220):(80|270):::::1
arc:(80|260):(80|260):(80|220):(40|220):(80|260):::::2
arc:(72.818|134.529):(87.182|134.529):(80|80):(25|80):(80|135):5:#ffff00:2:#0000ff:1
arc:(74.773|119.657):(85.227|119.657):(80|80):(40|80):(80|120):5:#ffff00:2:#0000ff:2
',32,'','','ElFigure','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','geomX','4',32,'','','ElFigureTests1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','geomY','259',32,'','','ElFigureTests1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','geomZ','8',32,'','','ElFigureTests1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('DiagramTest','geomH','143',32,'','','tr1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('DiagramTest','geomZ','1',32,'','','tr2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('MediaTests','geomZ','1',32,'','','column','','','','','','');
INSERT INTO "wlb_test_io" VALUES('TextEltests','geomZ','4',32,'','','text1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('TextEltests','geomZ','3',32,'','','text2','','','','','','');
INSERT INTO "wlb_test_io" VALUES('TextEltests','geomZ','2',32,'','','text3','','','','','','');
INSERT INTO "wlb_test_io" VALUES('TextEltests','geomZ','5',32,'','','text5','','','','','','');
INSERT INTO "wlb_test_io" VALUES('TextEltests','geomZ','1',32,'','','text7','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','geomW','212',32,'','','ElFigureTests1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','geomH','304',32,'','','ElFigureTests1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','geomZ','2',32,'','','FormEl','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','name','',32,'','','prt','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','geomX','449',32,'','','prt','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','geomY','353',32,'','','prt','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','geomW','450',32,'','','prt','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','geomH','237',32,'','','prt','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','geomZ','1',32,'','','prt','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','tSize','360',32,'','','prt','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','trcPer','1',32,'','','prt','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','viewOrd','4',32,'','','prt','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','col','tm;lev;cat;mess',32,'','','prt','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','itProp','2',32,'','','prt','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','it0lev','3',0,'','','prt','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','it0fnt','Arial 11 0 1 0 0',0,'','','prt','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','it0color','yellow',0,'','','prt','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','it1lev','4',0,'','','prt','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','it1fnt','Arial 11 0 0 1 0',0,'','','prt','','','','','','');
INSERT INTO "wlb_test_io" VALUES('mn_gen','it1color','red',0,'','','prt','','','','','','');
INSERT INTO "wlb_test_io" VALUES('protTest','geomZ','0',32,'','','prt1','','','','','','');
INSERT INTO "wlb_test_io" VALUES('ElFigureTests','name','Tests "ElFig"',32,'','','','Тести "ElFig"','','Тесты "ElFig"','','','');
INSERT INTO "wlb_test_io" VALUES('FormElTests','mode','1',0,'','','bt2','','','','','','');
CREATE TABLE 'wlb_test_mime' ("ID" TEXT DEFAULT '' ,"MIME" TEXT DEFAULT '' ,"DATA" TEXT DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO "wlb_test_mime" VALUES('bake','image/png;9.79492','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
HKIAAAAASUVORK5CYII=');
INSERT INTO "wlb_test_mime" VALUES('column','image/png;16.4912','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
/RV/snwAAAAASUVORK5CYII=');
INSERT INTO "wlb_test_mime" VALUES('fire','image/gif;10.8789','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');
INSERT INTO "wlb_test_mime" VALUES('help','image/png;1.736','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
4d2uOf0v0EISnzZ6ujoAAAAASUVORK5CYII=');
INSERT INTO "wlb_test_mime" VALUES('previous','image/png;2.14453','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
DmCHcDZqTv8NzUoG9odX3gIAAAAASUVORK5CYII=');
INSERT INTO "wlb_test_mime" VALUES('pump','image/gif;10.6895','R0lGODlhQABAAOf/AAACAAADBwEICw8IBgULDgoMCAgOEQwREw8RDhUQDxMRFQ8SGhgRFhoREREV
FxYVDRcVGBUWFCAXGBQaLw4cLxobGRAdMRUdLRIfMyMcIRwfKywcGhgiLCUgKRwjMxckOBgnNSIm
KBwnMSIkQCYoJTclHhwrOiIqOiEsNyAsQS8tMCcuPyIwQCYwOysyOSkzPiU0Qy41PCo1Syo4SC44
QzE5PzU5Ozc5NjM9STA/Ty1AVDlARzdBTTxBQ0JDQTxESjtGUThHV0RGQ0hGSkFITz5JVTtKWj9N
XkJNWE1LTz9QW01PTEhQV0JRYkZRXU1SVE9UVkpVYFNUUkVXYUdWZk9XXlJXWUNaaUlbZVVZXFpY
XFNaYUxbbFBbZ1dbXVlbWExeaU9eY01fak5ga1deZVleYFRfa1BibFJhclxhY1hjZFJlb2BlZ19n
bl5pamNoamlna1lrdlxrfF5tc2ZudWlucGJxg3Fvc3Byb25zdWl0gWJ1jGx0e4Jwcmd2iGh3fXl0
c3F2eHF5gWN9h2x7gW97e3V6fW97h3B/hXN/f3d+hnp/gXKBh3OCiHmBiHWBmnSClHWEiniEhHuD
i4aBgHyDl3eGjHeGmH6GjnmIjnOJmoqFhIOIioGIkHuKkH6KiniLlnyLkYKKkn+OlIKOj4WNlYGQ
loKRmIaSk4mRmISTmY+TloyUnIeWnI+Xn4mZn42ZmoycopKcl4+epJidn5Ghp5igqJSkqpujq5am
rJykuJinrpqms56mrqCosJuqsZirt5yssqWpuJ+srJmuraKrs56utKWttaKxuKiwuKays6qyuqS0
u6u0r66zta21vae3vau3uKm5wLC4wLG7trK6wqy8w7i5w7S+ua+/xsG6xre/x7TBwb2+yLHCyLPD
yr3CxLzEzbbGzL/Ex7jIzrrK0cDJ0cjJ08TM1L7O1MDQ1sPT2cfY3svY2c7X39HYzcrb4c7b3OPW
183d5Nnb19Pc5N3c09Lf4Nvd2tXe5tze29rf4t3i5eDm6Ovq4U5eayH/C05FVFNDQVBFMi4wAwEA
AAAh/hFDcmVhdGVkIHdpdGggR0lNUAAh+QQBCgD/ACwAAAAAQABAAAAI/gD/CRxIsKDBgwgTKlzI
sKHDhxAjSpxIsaLFixgzatzIsaPHjyAR9hhJkqSQkyd9qOyhsqXLlzClfKlY8o2iRYYMCRIUyEAD
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
+tCEDggAOw==');
INSERT INTO "wlb_test_mime" VALUES('test','image/png;4.321','iVBORw0KGgoAAAANSUhEUgAAAEAAAABACAYAAACqaXHeAAAABGdBTUEAAK/INwWK6QAAABl0RVh0
U29mdHdhcmUAQWRvYmUgSW1hZ2VSZWFkeXHJZTwAABBzSURBVHjaYvz//z/DSAYAAcQCYzAyMjLE
xsZq/vjxIxnIlfz37x8jNg3oAYYtAEFixKojBhCjlwg1TEA//fj169fW/fv37wDyvwHxX4AAggcA
0PNanJyce318fCQEBARIcgyt1FDTfFAEAyOXYd++fbHfvn1rPXny5HSg8GuAAIIHADBkivz8/CRC
QkLwWgAyiFTHk6Iem8OpZS4LCwuDjIwM071795KAQseB+ARAAMEDgImJSU1ISIjh58+fDN+/f8dr
OSUeGii1IMzKysrAx8fHwM7OzgcUVgfi6wABxIKk6N/v379RPP7371+seQ1frAxGzwMjF+xmEAb5
EeQvIAAFAjtAACEHACNysvvz5w/Do0ePGHh4eIBJh3lwFd1E+v8f0POfP39m4OfnB8c8cs6CYgaA
AGLBF3IcHBwM23fsYDh/+gQwOfxm+A8KRWp6hBHJM4z4FJEeQoyMzAxiUrIMMTGxeFUCBBALvsIO
VGjs2bmdYdOBJwwMSjbAIP2JcDB6MiTERxbDUIsjWtHF/xNICjBzmYDe+viUQYr9MENkRCTeLAsQ
QCz48xkwFbCxMjBoezHkd7QzSHIxMPwGZp///yD4338I/v/vP9husDiQ8Q9Gw8VQ8T+wBEINA1wc
4im42n8QL/7/DyvIIGr/QQVh4YMarsDsy8DCsHnLKgbOmy1Az6OWWehtFIAAYsGbQkEaQfjvTwYV
QWAiEGJg+PmLAe4xkIP/gj3ICPE0OFAg7H/QQPmHFFgwz4PUwMWQAugfPPAQHv2HHHj/EAGBCBhU
9eCC7h8zAzM0tRIqsAECiAVbyYoIJUZ4UvsFrCB+/AIFAMQBYI//g8Qgsif/wcSRPP0fHlAID0Ji
mwGuDkT8g/IZkDyOSCH/0QILVtAhAgBEg1z8G6qegZFwAAAEEAvBQooBySIkx6MncVyehwUOPObg
+qGBxvAfxQy4+H/UpI8pjpT0oeaAYxwI/6GVHfiqTYAAYsHXhmeC1Rb/UT35DxYb/1CzAzxFIHkU
OTtgZon/KOYygD2CZB5yeYAccAyogQpzH8zff6HlEiPUF/gAQAARTAFMTLCSBjWW/qHEOlp2IMLz
sPzOgBaryIXqv3+oeR1RPvzHDBBYimBEZC9ICxezAESOZIAAYiGuskbyLFI+RqSC/5hJG4vn4Vnj
HyL7/EMqwP5D9YDdDzXrL0qhCI1x9KyAFACMjIhaBuZ2bL1TGAAIIBZ8+QTe7MEoiVGT8b9/qPJ/
seR5ROCgeh7ZrH9I1ek/BrQq9N9/1HICK/6PKK8YYIUg/ugFCCCiC8H/aMkNo5RGKb2xe/7fP7S2
AXIZ8v8/RkGHCFhoewG9sEQrKGGR9Q+pRoD1AXABgADC3w5ggGn+j5bfEdkB2cHIMY8S67A8j+T5
v/8Q4gz/UbPOfwZEYcvwD7MsQK6J0LMByLV/oWULMbUAQAARlwKQW2v/sSd9rNkBqdZADry/SI2i
/0gexmhdIsc4WqsQvb0ACwRGeIvwP7wuwwcAAogF3zAWIgVgaZX9R3UcSmCglfb//yHFPLrnkatX
WNX3D1dWQGp/YGkXwOrBf/8ReQC5FsAGAAKIhej+FVKpjxw76LH/D9mjUHX/0JP9v/8osc2AnqWQ
UwVygKFXxf/gNTS8cIU1hEABSUw/EiCA8HeGGJF9j5aXUdrx/7HmT+yp4j9qsxk9+6DLQTtI8CzE
wIBm/3+khhEi4GA5gFAtABBALPhGXJCzALpH/6ElXfS8j5G8/6MVmsjNarSqEbnWQKkm/yO1FpF7
oUitQ1h2hRWChPoCAAHEgne4CdwbhFWDSLGApQr6j+TRf9jy6b//GHkaXtKjtB+Qqsx/mGUNtnIA
o1yClCQ4B1OQ/QgQQCx4R2QRbSlYRYAoif9hJv3/yKX3P+zdWdR+Ao4qE8nzmP2E/xgdJ+SxAxD5
F2oQJPKZ8M5XAAQQCwORLSHkagy5SkJp4qIHCJYYQ24RYovBf//+Y6kSUcsO1GyAlBL/QVLtv39I
lSAT/rYAQAAR0Q5gxNoOgDmUgQG1dYiO0VMEcosQFngMDLg9Dysj0LPBP2ggMKA1xiC9QUak3jD+
eQyAACIwIoQoRFBbYpg9PPSqETnm/6EXhliatMR6/t9/zO40SsHKiNwmgPjhP54yACCACDaFYXGM
rbDBiv8x4FeL3B/4hyPPow2u/Efrev+DFvXwJjJaCvgHFADPX0BrMXzTCAABhLc3iFILMGCv83FX
j/+xBAa2xgyWWgSf5///x5GCEE3gf/BkgCMJIAGAAMLbEGKEpQKkRhB6MxgdI1cXqG2F/2gdKVTP
oyRj5CoTzfP//mFWpag9QkbEGAIjLBXjDgGAACJQCDIiWlJYYh42WInsaVypBOsoL1rMIw96oA6y
osY8+tjk//+oA63//xE/hQIQQEzEjgj9xzIlgTIeh62ThK0DgyXpY9eD3FTGUhD+x971Bk37wdzF
CK0G8a0lAAggvMPiyO0olGoM3vz8j8Xx/4krLLH19NA6SRjN4X9ogyRoHSfUrEpwzg0MAAKIcDuA
ESmq/zFitvmxpIR/OArI/1hi7j+WUSRs3Wv0wRWUWgStQfT3HyNSNYh/RAgggPAOi4M1Iw2K/kce
HkMuvRlwtAT/M2CJPaTkjKQPowGF1ohCyfP/sLQp4IMrjPCpM2zdQfTsABBABMcDkPWjxDpSFxTW
J//HgKNfgJaV/kPb9nD6PwPBRhTWAVWk0WfkWSJ4O4gRNQNgq+oBAoi4pjAD6oAkA9qszP9/iDGD
fwyY2QMl30IDCtZl/4c+GYpjFunfPwacnoePRv9H9FsQDTlGjJSNzAYIIOKHxdFDEltq+Ic8Q/Mf
Rf4ftm4rtgbUf9SBV/SxCJThM6RmMvK44d9/CA8TGhIDCCCCKQC5IYGeBTAmJ5BSyn9sY3X/UOUZ
0IbWYUkGc3rtP9bBFczZKdgIEXJLEP+oMEAAEe4LMKIMkcJbh+A6lhHS+WBk+A8XA2v5h2NEjhEm
DwtNRnh9/Q8+7sCIpcXBiNLBgaQuRnjSZ0BpazBCA4C4FSYAAURcADAyM/z+z8TwE2jZr3/MKMNg
f5Fmiv+i8f9BB0f//fuPpdn8Hwcf0pZH5aOOO/xDTyUojSVgU5iBCRi2zBjDetgAQAARbAr/A/no
zUOGuctWMbD+/wnNd4gVHsjNQuT8D5vA/w9N7/+R5xcYEPXgfwbkWMVmLmK1yn+k6u3/P9TCCa6X
ARJhDC8vM/zm/IOxrhG9QAQIIJzzAjC2vJIqg6bgGoZ/19pQ1/YwMjDgGXbDmfgQsfEfY9gdqfON
VAQTuSQMzTJGXgYGVVU1BiZmFrxlAEAAYdQCiFHg/+AFk/X19QwVFeXAEP+HiEmG/xg1AzHtCcSM
DST0QOv3/gJTGKSkRu21kbqQFhZcjNCCiBFoNisLK8NXoB+g6wKxAoAAYsG1qpKZmZnhw4cPDE+e
PAGvsOTi4gKtsASLg1aPkQOePn3OcO/eA6B5TEBPM4MDBGTut29fGdTUVID0D6AdbAwiIsJkmQ/y
KAj/+vWL4duXzxD62zcGNjY2jAiGAYAAdJqxCsAgDEQzWhwqReeC//83jt0dgo4OUijNhSoUWjeR
05BLRPC9aPHRLyMB3vtJVpZSFJ7EAHSMJIQQFKSEk5hD8/vHIHszV0rpEI0h51bq/SRjFqq1UM4s
Fdcpxl3O3eYD5usCu6QaEQvWEQszU2tNucYH+1WzoLXWqoF/bXALwKUZ4wAIwlC0gwngBRy4iTP3
vwobGgGV16TEODQECv/T/1lIunxAH2soNiEAMlCcMnL6iSHOOWuwDxJGzqE4F2DNXgu5GDdJaVfM
ELzmSjmGyE2d8t5pXFedRhgfgRnMiX53qWeVNs661Qkd7uCaCVbw/NmOmv45VH4FYNLccQAEgSBK
aYgknMGaO3BxCyi5hDeQxIYojfLANVZk+cxmZ3eo5iOgJVhTSt45122lgA2NjrEiqQBRnBBDzHnO
ua/YUukGMUSwJ15dCtZ6evOdjcSBhySsnft/UOvVLbpSqLyTCQML3CYiFbagyl2UX7wyxnxTwRvu
/eVKw7DNxhjbxO0HZjJU8wggeABcunRpLtCxNvfv33eSlZVllZCQYJCUlARjEBtkAcjjIAzLb/+g
bVVQ7L148QJsIShLgMoOkENgsYIcmzCMmbwhHoWlJCYmJrxL6P///s/w+c9nhm+M38BqOTk54fkf
lFpgbnr+/Dmw7HkKzGLPGB4/fvzv4cOHH27fvn0SaMxzIP4KEECMSHmUFUgpiIuL+wI9YQl0uAww
/4gAU4MAEPMCywNWoBwTKN+LiYkxwGiQJ0EOBxoODiRg4DFcuXIF7BBNTU1woPz794+sFd+wcgmG
YSkA5MFfv38xfP7wGZwFv//8zvDy5UtwWQCigfg/EP/9+PHjN2CsfwHiD8AU8A6YMl8D1T8E2n0b
ul/gJkAArsodBUAYCKLExkYhhbEIeP+zSfxGtBLxDbggKYY0Icv8Nq7YjrI3gAg64Bnocb1HhAGC
EWECantEatkLDZGvEaJCICcxBAkj51Ul7Q7bBRZjI1L+PFY3RVlQ5+Wk4qvTSCphqlhK6YHknXO+
IHdy5+Dr3sAM6YQJC++tjNh/mD73R4X3FUCMWKoHEIMVGhgcUJoTiHmga+z5oTQvMGCEgIEhDgwM
SRAbGBiKBgYGig4ODuByAJTsQCkBPRZBAQErMGFZATkAkPM/CICyobCwMMP58+cZDhw48AGYhx8A
zX0L9PBboGffAwPqA1D9J6DSz1D8CcnDX4AYtAPkBxL+BWtqAAQQC47V+L+g+DNSoDAhBQwIswGT
IgcQcwJjBBQ4PFZWVpmGhoaKurq6DMCYAW1PAacEaWlpcKyBkiswthi+fPkCTsawBgqsYANVWaBU
AypIQR4GmQGKcVCqAsmBzDp9+vTfs2fPHgFquwBqWkA9CMKgTVA/0TDID3/xtasAAojYFs1/qEF/
oSGIPrLMIiUlxQUsIyarq6vD62GQg0FJF4QVFBQYlJWV4YUU0s4NeEmPXLsAkzdYHagAhqUekH55
eXlBYOAwAVPBZaCyG1CP/kEaYyEJAAQQeU06tHYJKKSNjIy0gY5VkZOTA+dfUN4HORpEgzwLKolB
JTXIk+gNJ1i1BVIHan6DAgyUBUANMeRtOqBCFhjATMDqWgMYAOzQGP5JieMBAogaAQAGQE+7q6qq
MoMcCdt0ha2pCqsOkQtBWAML1vgCZQFstQYo8DQ0NBiEhIRUQEUDEHNjSZEkAYAAoloAAEt6F5Dj
QEmYmE1VpGyPgwFQigHZASwUxYFlggqwoD0NFH5HTtKHAYAAokoA+Pr6ygDzpamWlha4MMPWJ6DK
FiNgIAHLGlD7gh1YIxgDA2AXUPgJtFVHFgAIIKoEADD5GgoKCvKBkii+hcnExjSBlAauIYApQBHI
FYdW0WQHAEAAUSUAgCX7U2CT88/ixYtZQI7DFwDkbpmFlQ+gAhLYbAc1jkAFDS+oOqbE7QABxIir
n0wiYDc1Nc0ElvipwBgSB5rJRKvN0sCa4jewa/7i7t27+4GF6nag0Alog4csABBA1AoARmgTWheI
1aAtRSYG2oD/0EbPAyC+CG0M/SHXMIAAAwAZTAixWD31ZQAAAABJRU5ErkJggg==');
INSERT INTO "wlb_test_mime" VALUES('greed','image/png;2.19238','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
RU5ErkJggg==');
CREATE TABLE 'wlb_test' ("ID" TEXT DEFAULT '' ,"ICO" TEXT DEFAULT '' ,"PARENT" TEXT DEFAULT '' ,"PROC" TEXT DEFAULT '' ,"ru#PROC" TEXT DEFAULT '' ,"uk#PROC" TEXT DEFAULT '' ,"PROC_PER" INTEGER DEFAULT '-1' ,"ATTRS" TEXT DEFAULT '*' ,"TIMESTAMP" INTEGER DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO "wlb_test" VALUES('kompr','iVBORw0KGgoAAAANSUhEUgAAAEAAAAAlCAIAAAB9MnhgAAAACXBIWXMAAAx1AAAMdQEteJR1AAAC
YklEQVRYhe1YsY7aQBAd48WLLIyQDMjQQEGBkEDwC/d3tHwTPcUVp6AUd5FOIVywA4ZdefHuXrEn
gjjbIRCMiPyKwbwZiXnsG49BG4/HlFK4T2CMEaXUdd3pdBqGYT6f3+12hxEA0iERQpxzKWVCvW3b
3W73SAMCAMbYZDIhhFiW5fv+YQSAdEjTNIMg4Jwn1Pd6vc8Cclc84FSA1ItlWbquW5alrvfx6O31
SNM0DcNQJxBXjzGOFeD7PiFEXRzFdEjOubJQQn0QBLEClHqMMWPsMCrdKZCGYQBAcg8IoVgBACCj
EMlfifxjD5+7/y2AxSAydQ0SIaQslFAfhmGsgGyIbzbE2R7ILAT/ZA/oMYhMXYM8pT6XizD8hwCM
8W0XmbLH+YuMEEII0XX9KCr16ZDKQgn1Sky0gGyIsz1wLjILZRa6EJmFbm2h7ARufQL/xRAbhvHw
8MA5RwiFYXgYASAdUtd1IYSUMqG+XC5HCygUCoPBQD24JqPT6VQqlTAMa7WaYjjnj49fpJQArNVq
EUIajYamaSr78vLddT3f/9nr9Siltm3vffz25j0/f3PdRRBc9NfyxwwsFgvlPymlEMI0ze12CwDF
YrFSqeyr6/W6bduMseFwqBgp5eury1iQyzHHcSil/X5//9xLqdB1LAR1HIcQ0m6399/i09PXzSZY
rZaXdA8A2mg0UuN7ChzHKZVKh4wQYrlcSSk1jVer1aN6z1sLITebX81m8yi1Xm84h+XyhxD87O5N
04z4iZCA+Xw+n8/jsp7nxaVms9lffdDpuPu7UCbg1kCR+/lekM/n3wGfkdnEFEBCyQAAAABJRU5E
rkJggg==','/wlb_originals/wdg_Box','','','',200,'geomW;geomH;backColor;bordWidth;','');
INSERT INTO "wlb_test" VALUES('protTest','iVBORw0KGgoAAAANSUhEUgAAAEAAAAArCAIAAABHOBkQAAAACXBIWXMAAAx1AAAMdQEteJR1AAAC
DklEQVRoge2Yy07bQBSG/zNzbMfOxU5CUItCBFSK1BUPwRvyBjxMJZZds2u5iIoWx4mcKo7Hpwu3
iBZXKiAYqOZbjOxzkc5nS2N5aDD4MJl00IQxRmvdFC+15sYW3/cbW54CERwff+eDg+jwcArg9PTT
1dWXOE6Gw1GS9EWqLEuTZAggTb9dXJxPp+89zwNwff1VRM7OPjN7SpFSWmttjNncfBPHYbvdfh6B
9Rqj0TkDVZ5nAAaDOI47RIoIeZ4BUparOuX7vL09Xq3yoiAAxqyiqL27u1tVhoi01iICkFLErLSm
5xGoKgDgfr+3t7d5z9771j8JRSEAlO0xHosTsI0TsI0TsI0TsAnhlQu4L/FLwAnY5lULuF3oJcAn
J3J0tLQ9xkMwRgDNVZWVpWbm2SztdLrMHoD5fNZud+vzBZEKoMVi7vt+EAREv700pSSKIisCaVoC
zFtb2Nm56nZ7ZUmLxQWzFwQtrSnPL5XSQRBkWTocjohUns/Lcr1c5mEYeZ5XFEUcJ+Px216v+VTm
qbm8LIgyDkNvf/8dAGMM8wSAiNQnQkT1+cKkjjBznQXwK4VWKwgCFpGbyB/Xt4vr1N21sfg2jcVh
qAFwGLY2NpJHPw76h+ub27trY/HfGn9GmBX+g13ICdjGCdjGCdjGCdjGCdjGCdjGCdiGReqfy9dH
PTYRfVQqtj3MAzFm+QOTos0i34dDswAAAABJRU5ErkJggg==','/wlb_originals/wdg_Box','
','
','',-1,'name;geomW;geomH;backColor;bordWidth;','');
INSERT INTO "wlb_test" VALUES('test1','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','/wlb_originals/wdg_Box','JavaLikeCalc.JavaScript
en=true;','JavaLikeCalc.JavaScript
en=true;','',-1,'en;active;geomW;geomH;geomMargin;backColor;backImg;','');
INSERT INTO "wlb_test" VALUES('DiagramTest','iVBORw0KGgoAAAANSUhEUgAAAEAAAAAVCAIAAAB5SH/NAAAAA3NCSVQICAjb4U/gAAAACXBIWXMA
AA0SAAANOgHo3ZneAAAEJElEQVRIic2XT28TVxTFf+/NjGc89kw8zv8sSiAWUoBGRSxbShsVsWHb
bdfdddNKXXXRD9Av0U+AhMSCDcoOKVIjBArUJDFJg03ieOw4sT1j+70uQlJBnMCghvasnvTOvXPO
u+/PHfHdDz+5lsHJcByn0+kAwnH/Winatg10Op1erxvH3VMCPwIyQd50LWMi65xCyufztVoN2FVs
rf7peR5QrVZt23748OFHUnoCrl7/2kwUYBSLa7UasHI2gj4AMhE7NzzcnpoSZ6Tlg5CsAkO53NzM
TLlez8pkzs8IlmUlM1AqlRaXlma0jsT/ogz9fj+ZgX6/3+12Ryd5Wj4jScmglEq2EwwppZQXzuN5
ZyQpMZJV4JubNwum2Vz6PZul2TwjScmQzMCzZ/fvr/W/nBy6dYs7d/77c2w7TjIDY2NVc4MoZm0N
z2N9HWBqipcv/+FMTQFUKih1Yh7LwrJotXBdWi3OnePFC4DZWZaXB4dISSbzRtmzWQSdQwMG9N9t
YGmJVIoworvApUtMzhDbjKdIpzEMpKRc5uJFgFQKw8A06XY5urFyOTY2SKdRiiAgil5riiIKBZSi
UCCOiWOqVSyLiQmiCKUwDHI5ymVGRxEC32d3l3yedBoTMCYN3da6q/W+BjARlpAZ2Q/7IiWMMYM6
HCznzOdzvn/v3j0g/QQbAqhAYFHp0zQYGuLBA2YEhsaFqiALSrMHrwAIAvaa2G02Nt5e4E9z3L2L
1rguQYDbxXyOI7AlazalEh5sCbRGCLRmZYWr1zGFJfSeFhkhPaldrXv64HVWTSVzUphCtzWHm6Hb
6xWLxSAIjj5cB6AMF5RalbLfJwjIKbUqZQ0+iaJ12wY8mFfqlZQatMOo2auZZgaqUDtMNdLrXxiS
ea2RMm7E3SiqDHkagPNRdD6wm+BBVqkdiPf3xz0vdhxTd7VqKgZdKTrSgMr/s5eLxeJBY3ccDQiP
jUOg1ToYrIMLAsbh4Mg8hmnIH0Y9Bg9CeH6UNHyd8ijPa6tQhadh+G2nk+wQn4ImzADgw5MTOAcS
VuEcZAEovUkYP+ScjurhYC9pM3flyhXHGdx7v4IVWIE/IH5XnhcnmHwKjSR6KknfAcMwgiAwDKPX
60VRdHzWdV3TNMMwHBgOmKYJjIyMtNvtRmOA2h7khXBd13Xd7e3t4wQhhGVZQCqVGhsfT2agXq9P
T08XCoXl5eXFxcW3ZoMguHHjxs7OzsLCwt7e3sAMs7OzUsrLly8/evSoVCoN5FiWdfv27Uqlsr+/
v7m5+das4zjz8/NbW1tzc3Obuy3x/Y8/v/cfmfzt11/e0+q/i3w+f+3atXq9rpSK4zgIgu3t7SAI
nJEJs2c5obBPCQ7DfYQNiLT92RdffSTJb8L3fX90VGeGfN9vNBrDw8NmbsT3/Z4w/gaGGJVZL2Zi
hgAAAABJRU5ErkJggg==','/wlb_originals/wdg_Box','','','',-1,'name;geomW;geomH;backColor;bordWidth;bordColor;',1374518074);
INSERT INTO "wlb_test" VALUES('FormElTests','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','/wlb_originals/wdg_Box','JavaLikeCalc.JavaScript
ev_cur = ev_rez = "";
off=0;
while(true)
{
  ev_cur=Special.FLibSYS.strParse(event,0,"\n",off);
  if( ev_cur == "" ) break;
  if( ev_cur == "ws_BtPress:/bt1") bt1_color=(bt1_color=="red")?"green":"red";
  else ev_rez += (ev_cur+"\n");
}
event=ev_rez;

line3_value+=0.1;','JavaLikeCalc.JavaScript
ev_cur = ev_rez = "";
off=0;
while(true)
{
  ev_cur=Special.FLibSYS.strParse(event,0,"\n",off);
  if( ev_cur == "" ) break;
  SYS.messErr("TEST","Event: "+ev_cur);
  if( ev_cur == "ws_BtPress:/bt1") bt1_color=(bt1_color=="red")?"green":"red";
  else ev_rez += (ev_cur+"\n");
}
event=ev_rez;

line3_value+=0.1;','JavaLikeCalc.JavaScript
ev_cur = ev_rez = "";
off=0;
while(true)
{
  ev_cur=Special.FLibSYS.strParse(event,0,"\n",off);
  if( ev_cur == "" ) break;
  if( ev_cur == "ws_BtPress:/bt1") bt1_color=(bt1_color=="red")?"green":"red";
  else ev_rez += (ev_cur+"\n");
}
event=ev_rez;

line3_value+=0.1;',1000,'name;geomW;geomH;backColor;bordWidth;bordColor;',1398169087);
INSERT INTO "wlb_test" VALUES('TextEltests','iVBORw0KGgoAAAANSUhEUgAAAEAAAAAsCAIAAABaPSmoAAAAA3NCSVQICAjb4U/gAAAACXBIWXMA
AA0SAAANOgHo3ZneAAAOmElEQVRogdVYeXRUZZb/va32PZVUtqpsRUISQnYSlkiAgAeEAC5oxL0d
wTnqGc/0mR45itNnsHva7jPdzIw9PdougESUARFpBBUFAdkSErKQBLKnslZSlapK7e+9b/5IUELY
GnKmj7+/3vmq7v1+v+/ed+99H5WfH+Z5Cj9NsCxheZ6qrR37WzO5Q+TkqOi/NYe7xU9eAHu3DoSx
nTsOzbAaB3xycbhrbvlqk0p6S6P6L3fvuUxv+vsHJHf99t21AEaVqBj651fe8Ync0ooNkj0fuBSm
7w6eSErRjoVFXXKBp/ps+cZVB/ae9PR7Nv3Hq2d2vl1nD1EyE9PyXW1byRxr1DQI8LjcIZ5nJVKN
Sknd6kiGuy6fbOhdtrxUPpF9xNbREaJYCcW3t9mipRcPtp1qP9XhdEYuf6bif7a8LwvaHRJPc+uo
vfpCdVdXUTzd6SW2tj6fy5/zlP8Ht6FgkGYZvz+oUiqoW5K4Ckx09GbH4Fa3d3DPvqMxJq1t0GXQ
cFXnamU643BX8+UBb3DEVlPfHILUYWsd8lMIeFw+v0Kp06pkAEBCn+77Zvl9S+LT5zCuzugYoz59
jlmKYGCkz+ErXrEqkkLFhgfddp9Gxmpj4rPS4pR6I2VIWZEfl5JfZI5QAQAhn1Z+2H6pft93bfmZ
sd3dQ7auTokmYrijqdvJaynfsF9we7z9rU19HhJl0AhBT9XZC4oI07Z3ZVR2Nv/s859sfHbhO//1
VkMXlZekFEVenZLcb3PxLs/MdGtunuWdd/6ybs3cI6fbg45eWq2PZIdd2nkvrr/nhsdChP3bdlqX
rsqI09/OKRLe8+tfbB6NzMuMCLMMHB5vvMXY0RVSGQ0WowaOHs4Sd/H7mjP1TVEzF7y56fGje3fx
WpM5Ln19ReqkKuR3Oz1hIpdy7lE3w0kIHxzzBw2m6KioqGijNhTw8qAHLzdo0gqUN69eFFP+1BO3
yR6Ad6g3dsGaxRZh2Nndx8T7nU6vw63Ryr2OPkNSus0+ePaLbzVRpuKyskgFQ4jY22vvHQ5bZ8YC
oLKz+a+/7ouIUI4MtPx5++mHHllhida0tnbHJSf7R3rdosJqNo6MuCMMmt7OdqI0ciSkN2jGvEKE
XnUjQmG/o3LbPj+QN3+ROHCxT4hYuaxIQt8ws8MBnzcEeHt+99bnT//d/d8crVlSOi8hVt/W1mO2
Wr1DvT6BMZn0na3tmmhLtEHpGR4YDnBJ8cbcHBWVnc1PdGJCCPBXvUA3PFF7w8o1//7E6tjv2sMD
DjFD2lXywm/XFCXc3IoQ8sPzbdKY3IkpalrYTzgDKEBlMKkFe29AYTZpb21yFW5/IxbA4f/9cPv+
+ucq5u/af8ScMdtvs0UmpsqDfYMBaXHBrI+3fbjksQ3rlhXeOAWuhcKYcfDIf7MM9ShoiLwIWirh
bp8TgKC7+/VNv3dqktJlw/1e6fMvrd/9pz+ySaVs/5nvW/nfvFbx/tvbCpY8DcxhAdxbXtps16pC
zrhZhWYtTlX1Zy5aamvw/ezRxYwmeqCleemSv4I9AIqiZbIf+vGd9MqR5vMZi+7z+0YGepSpscGG
qvMKpUrggr1eVYIxWFXTpJNzbEz0Fe+MZu6cmdnphsu7D5qzClJTZ3McPTvCevhU89rV5uyiOYpb
TUwBr8cv0CToV2jVrmGHRKWlwn6pSulxegBRIDRLQxAFmVwugFZKiJ/n9Br5TRyG5QlFubqB6g6v
xTIjPSWOGugbSl21orRRHvCqU5IVLqc5ryBVj/EqdPfj9Ffv/e5zm9F9dF/eqjkf7z0mKlKyVUOR
RUWHPzqztsxU7zcHz39puSe/q83tgeahjIA395mX1+Te0u2IrZszmTXcDaM/neN0wO8NhsO2jm7r
nIWryvIZMEnWRBacxRITazFzFGVOsoiqpHmazrdOuu8vzbgdnxHxlpuwH8ddD3MAgNmLVo2qLpLZ
D9W384/NjtamZEnnZxpTk5PeNCcnambxzK8/+1CpT9z8csqx7Z335pabtbeeWG8T05NCEyC80xXQ
66Y0OCL29fQbYmOI39075Iozx8slzLRsmJOjmp4ITIBir8MeAEXHWuIAQK2zqnXTueNECn16AIND
d2bfBBy7070pitqw4ek7tZ4ACwBvbsXpc3dmfwJ4NjzMMEwwTI7X+7+oDnYN8jQl6FTsggx2eaEy
UjephYV5cqIx8EVVsL0/3F1duWHDFI8Er74ib+7AwvmCOlZ46kHhNgTcNXxB8vqOUduofG1xRMUi
CUfxw17muzrXc1uHf/GgsjhTMz4d+ILiGx+NXh6UrSkyrFso2SNoruuNpsFR4DgIt3HfMw1lNBgm
m7fbhwftz5UGC1K4SA2i9FyCEU+XaSsWmbbs8vTZA4SQEE+2VI4GBNXrj2iL0zijGtGG69ciU5zw
3nt+vRTR0eL4SsgHzxi6OiinB2Tyn6chAicafN1Do9Sl/R0Z9+tVdO/lupzigrqzF5xUVFKUVOqo
/sPHyS+Uq788cqH28qy1WW3Hv5fNLc7NtMhlVz7piYja00y/H4sWCnIWDAMqTA2ExZdKRQCHD7O+
LjqoJB4Kfg+efy58dW/4UUAQ8AEiIAHOAV5gGRACCEADYwAH8AAL7ACeBLRXwneoKpCTJDl6ou/Y
gcpghzF5Tv72Dz7wBLSK8NlGpbnlwtlLI6T50+3OjGdlrn37ulCYphWK01lG+cPWv31NpkrlcyKp
Q9+TtfeIELFvF2dro/Z+RR5YKvT3U3lp4v4v2VGJOLdYuKYA/yhgO7AL0AF+wAUwwEkgHjgDiMAI
MAjIgEjACewBfg8UAAA6h4RIDVFqDVaTVKGWXaxrVGg1bS2X4iyxY/1tw2MBlddbuCDv09pmRYSU
H22nGZVaMSl5JGqSGENkcqQlTeSM3UGNDdG2Qar+IgWAF6CPFyMAck0CXS0gHrAAGkAAugAKSAPK
ADsAgAE0QCZgByKA2UDMFUNepHtcseXrN85IUM/N0PYPOvmwQ8LGFS1c9Oe33k9ZttJ+obrsgWdq
BKfeqELv+cWrV6ZEKXkB/tCV4Aeozz6UWPOEFfEUQACo1CTRQhRXgtRQw+SuDCXJqB272fAyQXrd
FFoOLL/y7AQ6gfFp65dXLbKA+tojQLQOLSOG1lFjuZQqyab7AjE/r4zZvDazw4EG/UtSGgmFUZE6
KtHM1doTWEPmqS5qQRbV0Is/HcHLLwGAVEnKVwiHTzGEBQhsNhqE6CNEOUuyMkj1Wax4OHy2mjkp
Ys0jYcnkujPpJd4NpAB+IAB8A/QDUUAi0AkEgUNAMSADnMC9VykpyaDbj9tVCv743u3S4eyMgmUq
JsTRcgkJGGUkFBZjdKMGlW5+Blt7aFBj0MsYRuAZIhIZO1Hj/R7q5BkGBK5RwIz4OHHUiYfX8nuP
TZBtbaK7++muVqp01bWVdZKALUAJ0ASogCAwBHQCIWAB8DkQCwwDg0A94AWevGKlGNrvqO3gdUmC
c+jrE7WHD3zJi9KDVFFTzZGuERkrV4bIpT+EcvMt7KWDx5LSS442cy17bb10tA62cQ/6WPGVZ8Jb
32XmZxEAoFGQRn6zVVL+eAgABOroaVqUEo/3Ot+akwQ8BawEOoERQARoQAuIAAtkATZgLtALlADl
V1l128c2vVT22q9OzphVyDvrnEFuVn7WhbpGSp3q7zy3bHb6mJNldZEBR8cr//jQ+3ttDrA8LziD
LcOO4LgHzwj13rucewSNHVRmEgEg8FRvD90/QgEAQ5L1VMKKcNUebupwPUnAy8AIYARUAAf0A6NA
+hTR1+DpilUHvq7Zs+OFyp0Hatl5fOu3Z+rHrBmLSf+JTf+wemZavEFaJrBMpGHu4UPfv/1vK/d/
46lrSZSFBYOsedyDGKZOHGXMOfyERxFeVnz11WDX4MTCvDJ+2w7JzBJePaXxXtvIPgYOAVZABTgB
FfAicBKoBUyAEYgA7p1soo5MePzxFELIL39eMTwa6OlJYOR6jYKO0lkVCinDMD/cMjz25MOEkByr
OOKyDjpCu3ZeHl9nJKRkAf/NaWYsHAYAGqkpZFclt2bDRIiOH2aDKnLpFONZKGgmN4LrdOJYoAGI
BToAN6AEGgEK+AKwADJAA8ydYhUWcLopdKgq1DGophAyqJmSDCzNl+gnly1ewNmW0KGqYGs/31En
vgEAEMJUzwCy8wROwIgTIGhrol1j6LFRBTMIAJkUreeYGKvA3jyFAKwHKoBWQA04ABHIB44C9UAJ
EAEogKIp7H1BcUvlaOugrDTZoYoS1e6mYWg+ePf8H3dmmQNfzV9WDPdQKEwWP/jE3mOhuh7JykL9
8jnSzwIT4tQR4tIcscMJZoiprEUghLWLeVoGbnyQFaiTTVTJYt5kFKeObteuaAE9UAjMBOYBCwA5
sBz4J+B5YB2wcopNiMe/7hwNEfXmR7RzrUEV41aJA70NX820sHkZ5ianqXDR/FG3sGTBjJffqB3x
Kf7lEV3pLGm0jkTrJeMevC6qtoHuuUyTWOFna3ifg3r7P6XnO6hEy3hRIjqKGrJTR/7CecRbReAO
cLLRd2mAe3ElN+YXKKJwtX876Oll5ZE8T+fNipWH0j7/alQY6nzrYzAR61YXSr0BgaJIWrxcLp04
inWPhRISfhwSTLFk44bgmIM6Vk3PtAggVN7yUNlSQXK90XkaBByq8ucm65wePsbA5c7OKsrPDnkH
ay55ZNro441Bt3FVe4dt470PnLPPiKJVbh8vl3Iz4uQc+yOdq9kDgIjPKrl+F9Vqx33LhIceDikU
uNF14zQI6BgQInREylJ5VpWEowFIlKaiXJMgkqMtsro+FrzOkqz4pD6UZiYMjXyrSsrd9DuExur1
YQjYe4ABoFTe7L8sAEilkMvujD0D9NTsDnVJmUxld821dw0BH3yN8PvGPvko2N4IZxPHZCj6Lkyc
2qlTpzZufPS6Prdsko64qYUPhKb+eg2m9Vrl/x05OSpKJhPT0qa82z8RtLTQ/wchCi3G/U9zOQAA
AABJRU5ErkJggg==','/wlb_originals/wdg_Box','','','',-1,'name;geomW;geomH;backColor;bordWidth;bordColor;',1374518075);
INSERT INTO "wlb_test" VALUES('MediaTests','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','/wlb_originals/wdg_Box','JavaLikeCalc.JavaScript
ev_cur = ev_rez = "";
off=0;
while(true)
{
  ev_cur=Special.FLibSYS.strParse(event,0,"\n",off);
  if( ev_cur == "" ) break;
  //Special.FLibSYS.messPut("Diagramm",0,"Event: "+ev_cur);
  if( ev_cur == "ws_MapAct0Left:/column" ) mov1_src=(mov1_src=="")?"pump":"";//speed-=10;
  else ev_rez += (ev_cur+"\n");
}
event=ev_rez;','','',200,'name;geomW;geomH;backColor;bordWidth;bordColor;','');
INSERT INTO "wlb_test" VALUES('mn_gen','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
RK5CYII=','/wlb_originals/wdg_Box','JavaLikeCalc.JavaScript
//if(geomX++ > 10) geomX=0;','','',-1,'name;dscr;geomX;geomW;geomH;backColor;bordWidth;bordColor;',1374518075);
INSERT INTO "wlb_test" VALUES('ElFigureTests','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
ZQAAAABJRU5ErkJggg==','/wlb_originals/wdg_Box','','','',-1,'name;geomW;geomH;backImg;',1374518075);
CREATE TABLE 'wlb_test_uio' ("IDW" TEXT DEFAULT '''''' ,"ID" TEXT DEFAULT '''''' ,"IDC" TEXT DEFAULT '''''' ,"NAME" TEXT DEFAULT '''''' ,"uk#NAME" TEXT DEFAULT '''''' ,"IO_TYPE" INTEGER DEFAULT '''''' ,"IO_VAL" TEXT DEFAULT '''''' ,"uk#IO_VAL" TEXT DEFAULT '''''' ,"SELF_FLG" INTEGER DEFAULT '''''' ,"CFG_TMPL" TEXT DEFAULT '''''' ,"uk#CFG_TMPL" TEXT DEFAULT '''''' ,"CFG_VAL" TEXT DEFAULT '''''' ,"uk#CFG_VAL" TEXT DEFAULT '''''' ,"ru#NAME" TEXT DEFAULT '' , PRIMARY KEY ("IDW","ID","IDC"));
INSERT INTO "wlb_test_uio" VALUES('FormElTests','extValue','','Ext. value','Зовнішне значення',131076,'0||','',6,'','','','','Внешнее значение');
COMMIT;
