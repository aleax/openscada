PRAGMA foreign_keys=OFF;
BEGIN TRANSACTION;
CREATE TABLE 'JavaLikePrm_prescr' ("SHIFR" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '' ,"FLD" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' , PRIMARY KEY ("SHIFR"));
INSERT INTO "JavaLikePrm_prescr" VALUES('cntr','Control','',1,'mode
prog
startTm
curCom
comLs
work','Управление','','Контроль','');
CREATE TABLE 'LogLevPrm_prescription_io' ("PRM_ID" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"VALUE" TEXT DEFAULT '' ,"ru#VALUE" TEXT DEFAULT '' ,"uk#VALUE" TEXT DEFAULT '' , PRIMARY KEY ("PRM_ID","ID"));
INSERT INTO "LogLevPrm_prescription_io" VALUES('timer','run','0','','');
INSERT INTO "LogLevPrm_prescription_io" VALUES('timer','pause','0','','');
INSERT INTO "LogLevPrm_prescription_io" VALUES('timer','error','0','','');
INSERT INTO "LogLevPrm_prescription_io" VALUES('timer','abort','0','','');
INSERT INTO "LogLevPrm_prescription_io" VALUES('timer','rez','1','1:Ожидание 20с истекло','1');
INSERT INTO "LogLevPrm_prescription_io" VALUES('timer','arg1','20','','');
INSERT INTO "LogLevPrm_prescription_io" VALUES('timer','tmp1','0','','');
INSERT INTO "LogLevPrm_prescription_io" VALUES('timer','f_frq','5','','');
INSERT INTO "LogLevPrm_prescription_io" VALUES('timer','f_start','0','','');
INSERT INTO "LogLevPrm_prescription_io" VALUES('timer','f_stop','1','','');
INSERT INTO "LogLevPrm_prescription_io" VALUES('timer','f_err','0','0','0');
INSERT INTO "LogLevPrm_prescription_io" VALUES('backTimer','run','0','','');
INSERT INTO "LogLevPrm_prescription_io" VALUES('backTimer','pause','0','','');
INSERT INTO "LogLevPrm_prescription_io" VALUES('backTimer','error','0','','');
INSERT INTO "LogLevPrm_prescription_io" VALUES('backTimer','abort','0','','');
INSERT INTO "LogLevPrm_prescription_io" VALUES('backTimer','rez','1','1:Ожидание 15с истекло','1');
INSERT INTO "LogLevPrm_prescription_io" VALUES('backTimer','arg1','15','','');
INSERT INTO "LogLevPrm_prescription_io" VALUES('backTimer','tmp1','0','','');
INSERT INTO "LogLevPrm_prescription_io" VALUES('backTimer','f_frq','5','','');
INSERT INTO "LogLevPrm_prescription_io" VALUES('backTimer','f_start','0','','');
INSERT INTO "LogLevPrm_prescription_io" VALUES('backTimer','f_stop','1','','');
INSERT INTO "LogLevPrm_prescription_io" VALUES('backTimer','f_err','0','0','0');
INSERT INTO "LogLevPrm_prescription_io" VALUES('backTimer','start','0','','');
INSERT INTO "LogLevPrm_prescription_io" VALUES('backTimer','stop','1','','');
INSERT INTO "LogLevPrm_prescription_io" VALUES('timer','start','0','','');
INSERT INTO "LogLevPrm_prescription_io" VALUES('timer','stop','1','','');
CREATE TABLE 'ParamTemplLibs' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"DB" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO "ParamTemplLibs" VALUES('S7','S7','S7','Templates library of parameters for PLC series S7 of firm Siemens.','Бібліотека шаблонів параметрів для контролерів фірми Siemens серії S7.','tmplib_S7','','Библиотека шаблонов параметров для контроллеров фирмы Siemens серии S7.');
INSERT INTO "ParamTemplLibs" VALUES('base','Main templates','Базові шаблони','Allow realisation of the main templates.','Надає реалізацію базових шаблонів.','tmplib_base','Базовые шаблоны','Предоставляет реализацию базовых шаблонов.');
INSERT INTO "ParamTemplLibs" VALUES('DevLib','Devices lib','Бібліотека пристроїв','Library for realization acquisition from various devices.','Бібліотека реалізації опитування для різноманітних пристроїв.','tmplib_DevLib','Библиотека устройств','Библиотека реализации сбора с различных устройств.');
INSERT INTO "ParamTemplLibs" VALUES('PrescrTempl','Prescription templates','Шаблони рецепту','','','tmplib_PrescrTempl','Шаблоны рецепта','');
CREATE TABLE 'UserFuncLibs' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"DB" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"PROG_TR" INTEGER DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO "UserFuncLibs" VALUES('techApp','Technological devices','The models of the technological process devices.

Founded: october 2005
Author: Roman Savochenko
Version: 0.9.0
License: GPL','techApp','Технологічні апарати','Моделі апаратів технологічних процесів.

Засновано: жовтень 2005р
Автор: Роман Савоченко
Версія: 0.9.0
Ліцензія: GPL','Технологические аппараты','Модели аппаратов технологических процессов.

Основан: октябрь 2005г
Автор: Роман Савоченко
Версия: 0.9.0
Лицензия: GPL',0);
INSERT INTO "UserFuncLibs" VALUES('servProc','Service procedures','Library of service procedures.','lib_servProc','Сервісні процедури','Бібліотека різноманітних сервісних процедур.','Сервисные процедуры','Библиотека различных сервисных процедур.',1);
INSERT INTO "UserFuncLibs" VALUES('doc','Report''s documents','Library of functions to facilitate the implementation of typical computations the primitive form of reporting documentation VCA "Document".

Founded: January 2008
Author: Roman Savochenko
Version: 0.5.0
License: GPL','flb_doc','Звітна документація','Бібліотека функцій для полегшення виконання типових обчислень примітиву формування звітної документації СВК "Документ".

Засновано: січень 2008р
Автор: Роман Савоченко
Версія: 0.5.0
Ліцензія: GPL','Отчётная документация','Библиотека функций для облегчения выполнения типовых
вычислений примитива формирования отчётной документации
СВУ "Документ".

Основан: январь 2008г
Автор: Роман Савоченко
Версия: 0.5.0
Лицензия: GPL',1);
INSERT INTO "UserFuncLibs" VALUES('regEl','Regulation elements','Regulation elements library','flb_regEl','Елементи регулювання','Бібліотека функцій регулювання.','Элементы регулирования','',0);
INSERT INTO "UserFuncLibs" VALUES('Controller','Controllers','Programms of controllers based on JavaLikeCalc.','lib_Controllers','Контролери','Програми контролерів базованих на JavaLikeCalc.','Контроллеры','Программы контроллеров основанных на JavaLikeCalc.',1);
INSERT INTO "UserFuncLibs" VALUES('web','XHTML-template','Pages processing functions library for XHTMP-template user''s Web-interface','flb_web','','','','',0);
CREATE TABLE 'UserProtocol_uPrt' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '' ,"InPROG" TEXT DEFAULT '' ,"OutPROG" TEXT DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO "UserProtocol_uPrt" VALUES('SMS','','',1,'','JavaLikeCalc.JavaScript
// Process SEND SMS message
// <name> - "send"
// pin - SIM card PIN-cod
// tel - recipient telephone number
// <text> - message
if(io.name()=="send")
{
	//> Prepare PDU
	var pdu = "001100";	//SMS center number (default) + SMS-Submit
	//> Telephone number encode
	var tel = io.attr("tel");
	if(!tel.length || tel[0] != "+") { io.setAttr("err","100:Telephone number error."); return; }
	tel = tel.slice(1);
	pdu += tel.length.toString(16,2)+"91";	//Telephone length and type
	while(tel.length < 12) tel += "F";
	for(i=0; i < 6; i++) pdu += tel[i*2+1]+tel[i*2];
	//> Message encode
	var text = SYS.strCodeConv(io.text(),"","UCS2");
	if((text.length/2) > 70) { io.setAttr("err","101:Long length ("+(text.length/2)+") of the message."); return; }
	pdu += "0018C1"+(text.length).toString(16,2);
	for(i=0; i < text.length/2; i++) pdu += text.charCodeAt(i*2+1).toString(16,2)+text.charCodeAt(i*2).toString(16,2);
	//SYS.messDebug("TEST SMS","PDU :"+pdu);
	//> Send request
	if(!io.attr("pin").isEVal())
	{
		var rez = tr.messIO("AT+CPIN="+io.attr("pin")+"\r");
		if(rez.search("OK\r") < 0)	{ io.setAttr("err","10:Error set PIN-code."); return; }
	}
	//>> Switch to PDU SMS mode
	var rez = tr.messIO("AT+CMGF=0\r");
	if(rez.search("OK\r") < 0)	{ io.setAttr("err","10:Error set PDU mode."); return; }
	//>> Send PDU message
	var rez = tr.messIO("AT+CMGS="+(pdu.length/2-1)+"\r");
	if(rez.search(">") < 0)	{ io.setAttr("err","10:Error sent SMS."); return; }
	var rez = tr.messIO(pdu+"\x1A");
	for(var i_tr = 0; i_tr < 5 && rez.search("OK\r") < 0; i_tr++) rez += tr.messIO("");
	if(rez.search("OK\r") < 0)	{ io.setAttr("err","10:Error sent SMS PDU"); return; }
	io.setAttr("err","0");
	//SYS.messDebug("TEST SMS","PDU REZ :"+rez);
}');
INSERT INTO "UserProtocol_uPrt" VALUES('SCU750','Cotrol Unit SCU750','Transport level protocol realization for EDWARDS TURBOMOLECULAR PUMPS.',1,'JavaLikeCalc.JavaScript
','JavaLikeCalc.JavaScript
//Request form:
//<mess addr="1">{req}</mess> - message tag
//  addr - remote station address (<0 - single; >=0 - multy port)

if(io.text().length > 255*255) { io.setAttr("err","1:Message''s length more 255*255"); return; }
addr = io.attr("addr").toInt();
k=ceil(io.text().length/255);  //transmission blocks
for(i_k = 1; i_k <= k; i_k++)
{
	request = "\x02"+k.toString(16,3)+io.text().slice((i_k-1)*255,(i_k-1)*255+min(255,io.text().length-(i_k-1)*255))+((k>1&&i_k<k)?"\x17":"\x03");
	//> Calc LRC
	LRC = 0xFF;
	for(i = 0; i < request.length; i++) LRC = LRC^request.charCodeAt(i);
	request += SYS.strFromCharCode(LRC);

	//> Multy port
	if(addr>=0) request = "@"+addr.toString(16,2)+request;
	//SYS.messDebug("PRT","Request: "+Special.FLibSYS.strDec4Bin(request));

	//Send request
	resp = tr.messIO(request);
	while(resp.length)
	{
		tresp = tr.messIO("");
		if(!tresp.length) break;
  		resp += tresp;
	}
	if(!resp.length) { io.setAttr("err","2:No respond"); return; }
	//SYS.messDebug("PRT","Ack: "+Special.FLibSYS.strDec4Bin(resp));
	if(resp.charCodeAt(0) != 6) { io.setAttr("err","3:No acknowledgment"); return; }
	//> Pass included acknowledgement
	resp = resp.slice((addr>=0)?3:1);

	//> Read data blocks
	io.setText("");
	for(i_k = 1; true; i_k++)
	{
		//Send application acknowledgement and wait data
		if(!resp.length)
		{
			request = "\x06";
			if(addr>=0) request += addr.toString(16,2);
			resp = tr.messIO(request);
			while(resp.length)
			{
				tresp = tr.messIO("");
				if(!tresp.length) break;
  				resp += tresp;
			}
			if(!resp.length) { io.setAttr("err","4:No data block get"); return; }
		}
		if(resp.length < ((addr>=0)?10:7) || resp.charCodeAt(0) != 0x40) { io.setAttr("err","5:Data block short or error"); return; }

		//SYS.messDebug("PRT","BLK "+i_k+": "+Special.FLibSYS.strDec4Bin(resp));

		if(addr>=0) resp = resp.slice(3);
		LRC = 0xFF;
		for(i = 0; i < (resp.length-1); i++) LRC = LRC^resp.charCodeAt(i);
		if(LRC != resp.charCodeAt(resp.length-1)) { io.setAttr("err","6:LRC error."); return; }
		if(i_k != resp.slice(1,4).toInt(16)) { io.setAttr("err","7:Block sequence."); return; }
		io.setText(io.text()+resp.slice(4,resp.length-2));
		if(resp.charCodeAt(resp.length-2) == 0x03) break;
		if(resp.charCodeAt(resp.length-2) == 0x17) { resp = ""; continue; }
		io.setAttr("err","8:Unknown block end.");
		return;
	}
}');
INSERT INTO "UserProtocol_uPrt" VALUES('TMH','TMP-xx03','Power supply for turbomolecular pumps, model EI-R04M.',1,'JavaLikeCalc.JavaScript
','JavaLikeCalc.JavaScript
//Request form:
//<mess addr="1">{req}</mess> - message tag
//  addr - remote station address (1...32)

io.setAttr("err","");
addr = io.attr("addr").toInt();
if(addr < 1 || addr > 32) { io.setAttr("err","1:Device address out of range 1...32"); return; }
request = "MJ"+addr.toString(10,2)+io.text();
//> Calc CRC
CRC = 0;
for(i = 0; i < request.length; i++) CRC += request.charCodeAt(i);
request += (CRC&0xFF).toString(16,2)+"\r";
//SYS.messDebug("PRT","Request: "+Special.FLibSYS.strDec4Bin(request));

//Send request
resp = tr.messIO(request);
while(resp.length && resp[resp.length-1] != "\r")
{
	tresp = tr.messIO("");
	if(!tresp.length) break;
  	resp += tresp;
}
if(resp.length < 6 || resp[resp.length-1] != "\r" || resp.slice(0,2) != "MJ" || resp.slice(2,4).toInt() != addr)
{ io.setAttr("err","2:No or error respond"); return; }
//SYS.messDebug("PRT","Respond: "+Special.FLibSYS.strDec4Bin(resp));
CRC = 0;
for(i = 0; i < (resp.length-3); i++) CRC += resp.charCodeAt(i);
if((CRC&0xFF) != resp.slice(resp.length-3,resp.length-1).toInt(16)) { io.setAttr("err","6:CRC error."); return; }
io.setText(resp.slice(4,resp.length-3));');
INSERT INTO "UserProtocol_uPrt" VALUES('VKT7','VKT-7','Firm "Teplocom" (http://www.teplocom.spb.ru) computer "VKT-7", St.Peterburg.',1,'','JavaLikeCalc.JavaScript
//Request form:
//<mess addr="1">{req}</mess> - message tag
//  addr - remote station address (0...254)

io.setAttr("err","");
addr = io.attr("addr").toInt();
if(addr < 0 || addr > 254) { io.setAttr("err","1:Device address out of range 0...254"); return; }
request = SYS.strFromCharCode(addr)+io.text();
//> Calc KS
KS = 0xFFFF;
for(i = 0; i < request.length; i++)
{
	KS = KS ^ request.charCodeAt(i);
	for(j = 0; j < 8; j++)
		KS = (KS&0x01) ? (KS >> 1)^0xA001 : (KS >> 1);
}
request = SYS.strFromCharCode(0xFF,0xFF)+request+SYS.strFromCharCode(KS,KS>>8);
//SYS.messDebug("PRT","Request: "+Special.FLibSYS.strDec4Bin(request));

//Send request
resp = tr.messIO(request);
while(resp.length)
{
	tresp = tr.messIO("");
	if(!tresp.length) break;
  	resp += tresp;
}
if(resp.length < 4 || resp.charCodeAt(0) != addr)	{ io.setAttr("err","2:No or error respond"); return; }
//SYS.messDebug("PRT","Respond: "+Special.FLibSYS.strDec4Bin(resp));

//> Calc KS
KS = 0xFFFF;
for(i = 0; i < (resp.length-2); i++)
{
	KS = KS ^ resp.charCodeAt(i);
	for(j = 0; j < 8; j++)
		KS = (KS&0x01) ? (KS >> 1)^0xA001 : (KS >> 1);
}
if(KS != ((resp.charCodeAt(resp.length-1)<<8)|resp.charCodeAt(resp.length-2)))
{ io.setAttr("err","6:KS error."); return; }
if(resp.charCodeAt(1)&0x80)
{ io.setAttr("err","7:"+resp.charCodeAt(2)+":Request error."); return; }
io.setText(resp.slice(1,-2));');
CREATE TABLE 'flb_doc' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"MAXCALCTM" INTEGER DEFAULT '' ,"FORMULA" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"uk#FORMULA" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"ru#FORMULA" TEXT DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO "flb_doc" VALUES('getVal','Getting value from archive','Query the value for a specified time from the assigned archive and issuing the result with the specified number of decimal points.',10,'using Special.FLibSYS;
srcTime = time;
val = vArh(strPath2Sep(addr)).get(time,uTime,0,archtor);
return (val.isEVal() || (strong && srcTime != time)) ? "Empty" : real2str(val,prec);','Запит значення архіву','Запит значення на визначений час з вказаного архіву та видача результату з вказаною кількістю знаків після точки.','using Special.FLibSYS;
srcTime = time;
val = vArh(strPath2Sep(addr)).get(time,uTime,0,archtor);
return (val.isEVal() || (strong && srcTime != time)) ? "Порожньо" : real2str(val,prec);','Запрос знач. архива','Запрос значения на указанное время из установленного архива и выдача результата с указанным количеством знаков после точки.','using Special.FLibSYS;
srcTime = time;
val = vArh(strPath2Sep(addr)).get(time,uTime,0,archtor);
return (val.isEVal() || (strong && srcTime != time)) ? "Пусто" : real2str(val,prec);');
INSERT INTO "flb_doc" VALUES('sumVal','Sum values of archive','Calculating the sum of values ​​for a specified period of time an archive and issuing results with the specified number of decimal points.',10,'using Special.FLibSYS;

a_src = vArh(strPath2Sep(addr));

vl=0.0; buf_beg_u=buf_end_u=0;
buf_beg=max(a_src.begin(0,archtor),bTime);
buf_end=min(a_src.end(0,archtor),eTime);
while( true )
{
  r_val = a_src.get(buf_beg,buf_beg_u,true,archtor);
  if(r_val != EVAL_REAL) vl+=r_val;
  if( !buf_beg || buf_beg>=buf_end ) break;
  buf_beg_u++;
}

return (vl==EVAL_REAL) ? "Empty" : real2str(vl,prec);','Сума значень архіву','Обчислення суми значень за проміжок часу визначеного архіву та видача результату з вказаною кількістю знаків після точки.','using Special.FLibSYS;

a_src = vArh(strPath2Sep(addr));

tvar=0.0; buf_beg_u=buf_end_u=0;
buf_beg=max(a_src.begin(0,archtor),bTime);
buf_end=min(a_src.end(0,archtor),eTime);
while( true )
{
  r_val = a_src.get(buf_beg,buf_beg_u,true,archtor);
  if(r_val != EVAL_REAL) tvar+=r_val;
  if( !buf_beg || buf_beg>=buf_end ) break;
  buf_beg_u++;
}

return (tvar==EVAL_REAL) ? "Порожньо" : real2str(tvar,prec);','Сумма знач. архива','Вычисление суммы значений за промежуток времени указанного архива и выдача результата с указанным количеством знаков после точки.','using Special.FLibSYS;

a_src = vArh(strPath2Sep(addr));

vl=0.0; buf_beg_u=buf_end_u=0;
buf_beg=max(a_src.begin(0,archtor),bTime);
buf_end=min(a_src.end(0,archtor),eTime);
while( true )
{
  r_val = a_src.get(buf_beg,buf_beg_u,true,archtor);
  if(r_val != EVAL_REAL) vl+=r_val;
  if( !buf_beg || buf_beg>=buf_end ) break;
  buf_beg_u++;
}

return (vl==EVAL_REAL) ? "Пусто" : real2str(vl,prec);');
INSERT INTO "flb_doc" VALUES('averVal','Average value of archive','Calculate the average value over a specified period of time an archive and issuing results with the specified number of decimal points.',10,'using Special.FLibSYS;
a_src = vArh(strPath2Sep(addr));

vl=EVAL_REAL;
buf_beg=max(a_src.begin(0,archtor),bTime);
buf_end=min(a_src.end(0,archtor),eTime);
while( true )
{
  buf_beg1=buf_beg;
  r_val = a_src.get(buf_beg,0,true,archtor);
  if(vl ==  EVAL_REAL) vl = r_val; 
  else if(r_val != EVAL_REAL)
    vl=(vl*(buf_beg-bTime)+r_val*(buf_beg-buf_beg1))/(buf_beg-bTime+buf_beg-buf_beg1);
  if( buf_beg == 0 || buf_beg>=buf_end ) break;
  buf_beg++;
}

return (vl==EVAL_REAL) ? "Empty" : real2str(vl,prec);','Середнє значення архіву','Обчислення середнього значення за проміжок часу визначеного архіву та видача результату з вказаною кількістю знаків після точки.','using Special.FLibSYS;
a_src = vArh(strPath2Sep(addr));

tvar=EVAL_REAL;
buf_beg=max(a_src.begin(0,archtor),bTime);
buf_end=min(a_src.end(0,archtor),eTime);
while( true )
{
  buf_beg1=buf_beg;
  r_val = a_src.get(buf_beg,0,true,archtor);
  if(tvar ==  EVAL_REAL) tvar = r_val; 
  else if(r_val != EVAL_REAL)
    tvar=(tvar*(buf_beg-bTime)+r_val*(buf_beg-buf_beg1))/(buf_beg-bTime+buf_beg-buf_beg1);
  if( buf_beg == 0 || buf_beg>=buf_end ) break;
  buf_beg++;
}

return (tvar==EVAL_REAL) ? "Порожньо" : real2str(tvar,prec);','Среднее знач. архива','Вычисление среднего значения за промежуток времени указанного архива и выдача результата с указанным количеством знаков после точки.','using Special.FLibSYS;
a_src = vArh(strPath2Sep(addr));

vl=EVAL_REAL;
buf_beg=max(a_src.begin(0,archtor),bTime);
buf_end=min(a_src.end(0,archtor),eTime);
while( true )
{
  buf_beg1=buf_beg;
  r_val = a_src.get(buf_beg,0,true,archtor);
  if(vl ==  EVAL_REAL) vl = r_val; 
  else if(r_val != EVAL_REAL)
    vl=(vl*(buf_beg-bTime)+r_val*(buf_beg-buf_beg1))/(buf_beg-bTime+buf_beg-buf_beg1);
  if( buf_beg == 0 || buf_beg>=buf_end ) break;
  buf_beg++;
}

return (vl==EVAL_REAL) ? "Пусто" : real2str(vl,prec);');
CREATE TABLE 'flb_doc_io' ("F_ID" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"TYPE" INTEGER DEFAULT '' ,"MODE" INTEGER DEFAULT '' ,"DEF" TEXT DEFAULT '' ,"HIDE" INTEGER DEFAULT '' ,"POS" INTEGER DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#DEF" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DEF" TEXT DEFAULT '' , PRIMARY KEY ("F_ID","ID"));
INSERT INTO "flb_doc_io" VALUES('getVal','rez','Result',0,2,'',0,0,'Результат','','Результат','');
INSERT INTO "flb_doc_io" VALUES('getVal','addr','Address',0,0,'',0,1,'Адреса','','Адрес','');
INSERT INTO "flb_doc_io" VALUES('getVal','time','Time (s)',1,0,'',0,2,'Час (сек.)','','Время (сек.)','');
INSERT INTO "flb_doc_io" VALUES('getVal','uTime','Time (us)',1,0,'',0,3,'Час (мксек.)','','Время (мксек.)','');
INSERT INTO "flb_doc_io" VALUES('getVal','prec','Signs after dot',1,0,'3',0,4,'Знаків після точки','','Знаков после запятой','');
INSERT INTO "flb_doc_io" VALUES('getVal','archtor','Archivator',0,0,'',0,5,'Архіватор','','Архиватор','');
INSERT INTO "flb_doc_io" VALUES('sumVal','rez','Result',0,2,'',0,0,'Результат','','Результат','');
INSERT INTO "flb_doc_io" VALUES('sumVal','addr','Address',0,0,'',0,1,'Адреса','','Адрес','');
INSERT INTO "flb_doc_io" VALUES('averVal','addr','Address',0,0,'',0,1,'Адреса','','Адрес','');
INSERT INTO "flb_doc_io" VALUES('averVal','rez','Result',0,2,'',0,0,'Результат','','Результат','');
INSERT INTO "flb_doc_io" VALUES('sumVal','prec','Signs after dot',1,0,'3',0,4,'Знаків після точки','','Знаков после запятой','');
INSERT INTO "flb_doc_io" VALUES('sumVal','archtor','Archivator',0,0,'',0,5,'Архіватор','','Архиватор','');
INSERT INTO "flb_doc_io" VALUES('sumVal','bTime','Begin time (s)',1,0,'',0,2,'Час початку (сек.)','','Время начала (сек.)','');
INSERT INTO "flb_doc_io" VALUES('sumVal','eTime','End time (s)',1,0,'',0,3,'Час кінця (сек.)','','Время конца (сек.)','');
INSERT INTO "flb_doc_io" VALUES('averVal','bTime','Begin time (s)',1,0,'',0,2,'Час початку (сек.)','','Время начала (сек.)','');
INSERT INTO "flb_doc_io" VALUES('averVal','eTime','End time (s)',1,0,'',0,3,'Час кінця (сек.)','','Время конца (сек.)','');
INSERT INTO "flb_doc_io" VALUES('averVal','prec','Signs after dot',1,0,'3',0,4,'Знаків після точки','','Знаков после запятой','');
INSERT INTO "flb_doc_io" VALUES('averVal','archtor','Archivator',0,0,'',0,5,'Архіватор','','Архиватор','');
INSERT INTO "flb_doc_io" VALUES('getVal','strong','Strong',3,0,'0',0,6,'Точно','','Жёсткое соответствие времени','');
CREATE TABLE 'flb_regEl' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"MAXCALCTM" INTEGER DEFAULT '' ,"FORMULA" TEXT DEFAULT '' ,"uk#FORMULA" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"ru#FORMULA" TEXT DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO "flb_regEl" VALUES('pidUnif','PID (unified)','ПІД (уніфікований)','Composite-unified analog and pulse PID. At the heart of the regulator is core a standard analog PID controller from the library "FLibComplex1" (http://wiki.oscada.org/HomePageEn/Doc/FLibComplex1#h902-15) and the implementation of the PWM for the pulse part.','Суміщений-уніфікований аналоговий та імпульсний ПІД-регулятор. У основі регулятора лежить мова стандартного аналогового ПІД-регулятора з бібліотеки "FLibComplex1" та реалізація ШІМ для імпульсної частини.',10,'//> Call standard analog PID
outA=Special.FLibComplex1.pid(var,sp,max,min,manIn,auto,casc,Kp,Ti,Kd,Td,Tzd,Hup,Hdwn,Zi,followSp,K1,in1,K2,in2,K3,in3,K4,in4,f_frq,int,dif,lag);
if( analog ) return outA;

if( f_start || f_stop ) { impQup = impQdwn = false; return 0; }

//> Call impuls generator
outA = 2*(outA-50);
if( perLag <= 0 && ((outA*KImpRfact) >= (100*TImpMin/TImpPer) || (-outA/KImpRfact) >= (100*TImpMin/TImpPer) || (!auto && abs(outA) > 0.1)) )
{
  impLag = max(abs(outA)*TImpPer/100,TImpMin);
  impLag = (outA>0) ? impLag*KImpRfact : impLag/KImpRfact;
  perLag = TImpPer;
  if( outA > 0 ) impQup = true; else impQdwn = true;  
}
else if( perLag > 0 )
{
  perLag -= 1e3/f_frq;
  if( (TImpPer-perLag) >= impLag ) 
  {
    impQup = impQdwn = false;
    if( !auto ) perLag = 0;
  }
}

if( !auto ) manIn = 50;

if( impQupTm > 1 || (impQupTm && !impQup) ) impQupTm--;
if( impQup && !impQupTm ) impQupTm = 2*f_frq;
if( impQdwnTm > 1 || (impQdwnTm && !impQdwn) ) impQdwnTm--;
if( impQdwn && !impQdwnTm ) impQdwnTm = 2*f_frq;

if( impAnImit )
{
  if( impQup ) out += 100/(impAnOut*f_frq);
  if( impQdwn ) out -= 100/(impAnOut*f_frq);
  return max(0,min(100,out));
}
return impAnOut;','','ПИД (унифицированный)','Совмещённый-унифицированный аналоговый и импульсный ПИД-регулятор. В основе регулятора лежит ядро стандартного аналогового ПИД-регулятора из библиотеки "FLibComplex1" (http://wiki.oscada.org/Doc/FLibComplex1#h91-15) и реализация ШИМ для импульсной части.','');
INSERT INTO "flb_regEl" VALUES('pidUnifD','PID dynamic','PID динамічний','Completely identical to unified PID regulator is implemented dynamically on ((/HomePageEn/Doc/JavaLikeCalc JavaLikeCalc)). The dynamic implementation allows you to easily adapt the regulator to the desired requirements, simply by editing it.','Повністю ідентичний уніфікованому ПІД регулятор, реалізований динамічно на JavaLikeCalc. Динамічна реалізація дозволяє легко адаптувати регулятор під потрібні вимоги, просто відредагувати його.',10,'//> Call standard analog PID
Kzd = min(1e3/(f_frq*Tzd),1);
Kint = min(1e3/(f_frq*Ti),1);
Kdif = min(1e3/(f_frq*Td),1);

//> Scale error
if(max <= min) return 0;

//> Prepare values
spv = 100*(sp-min)/(max-min);
val = 100*(var-min)/(max-min);
val += K1*in1 + K2*in2;
val = min(100,max(-100,val));

//> Error
err = spv - val;
err = (abs(err)<Zi) ? 0 : ((err>0) ? err-Zi : err+Zi );
err *= Kp;
err = min(100,max(-100,err));

integ += Kint*err;					//Integral
difer -= Kdif*(difer-val);			//Differecial wait
dlag  += Kzd*((difer-val)-dlag);	//Differecial lag

//> Automatic mode enabled
if(auto) outA = err + integ + Kd*dlag + K3*in3 + K4*in4;
else
{
  casc = false;
  if(followSp) sp = var;
  outA = manIn;
}

//> Check output limits
outA = manIn = min(Hup,max(Hdwn,outA));

//> Fix integral for manual and limits
integ = outA - err - Kd*dlag - K3*in3 - K4*in4;

if(analog) return outA;

if(f_start || f_stop) { impQup = impQdwn = false; return 0; }

//> Call impuls generator
outA = 2*(outA-50);
if(perLag <= 0 && ((outA*KImpRfact) >= (100*TImpMin/TImpPer) || (-outA/KImpRfact) >= (100*TImpMin/TImpPer) || (!auto && abs(outA) > 0.1)))
{
  impLag = max(abs(outA)*TImpPer/100,TImpMin);
  impLag = (outA>0) ? impLag*KImpRfact : impLag/KImpRfact;
  perLag = TImpPer;
  if(outA > 0) impQup = true; else impQdwn = true;  
}
else if(perLag > 0)
{
  perLag -= 1e3/f_frq;
  if((TImpPer-perLag) >= impLag)
  {
    impQup = impQdwn = false;
    if(!auto) perLag = 0;
  }
}

if(!auto) manIn = 50;

if(impQupTm > 1 || (impQupTm && !impQup)) impQupTm--;
if(impQup && !impQupTm) impQupTm = 2*f_frq;
if(impQdwnTm > 1 || (impQdwnTm && !impQdwn)) impQdwnTm--;
if(impQdwn && !impQdwnTm) impQdwnTm = 2*f_frq;

if(impAnImit)
{
  if(impQup)	out += 100/(impAnOut*f_frq);
  if(impQdwn)	out -= 100/(impAnOut*f_frq);
  return max(0,min(100,out));
}
return impAnOut;','','PID динамический','Полностью идентичный унифицированному ПИД регулятор, реализованный динамически на JavaLikeCalc. Динамическая реализация позволяет легко адаптировать регулятор под нужные требования, просто отредактировав его.','');
INSERT INTO "flb_regEl" VALUES('pidImp','PID pulse','ПІД імпульсний','Specialized pulse PID regulator is implemented on a special algorithm with compensation of double integration.','Спеціалізований імпульсний ПІД регулятор реалізований за особливим алгоритмом з компенсацією подвійного інтегрування.',10,'//> Call standard analog PID
Kzd = min(1e3/(f_frq*Tzd),1);
Kint = min(1e3/(f_frq*Ti),1);
Kdif = min(1e3/(f_frq*Td),1);

//> Scale error
if( max <= min ) return 0;

//> Prepare values
spv = 100*(sp-min)/(max-min);
val = 100*(var-min)/(max-min);
val += K1*in1 + K2*in2;
val = min(100,max(-100,val));

//> Error
err = spv - val;
err = (abs(err)<Zi) ? 0 : ((err>0) ? err-Zi : err+Zi );
err *= Kp;
err = min(100,max(-100,err));
errP = err-err1;
errD = err1-err2;
err2 = err1; err1 = err;

integ += Kint*err;          //Integral
difer -= Kdif*(difer-errD);  //Differecial wait
dlag  += Kzd*((errP-difer)-dlag);    //Differecial lag

//> Automatic mode enabled
if( auto ) { outPID = errP + Ki*err + integ + Kd*dlag; outA = 50 + outPID + K3*in3 + K4*in4; }
else
{
  outPID = 0;
  casc = false;
  if( followSp ) sp = var;
  outA = manIn;
}

//> Check output limits
outA = manIn = min(Hup,max(Hdwn,outA));

//> Fix integral for manual and limits
//integ = outA - errP - Kd*dlag - K3*in3 - K4*in4;

if( f_start || f_stop ) { impQup = impQdwn = false; return 0; }

//> Call impuls generator
outA = 2*(outA-50);
if( perLag <= 0 && ((outA*KImpRfact) >= (100*TImpMin/TImpPer) || (-outA/KImpRfact) >= (100*TImpMin/TImpPer) || (!auto && abs(outA) > 0.1)) )
{
  impLag = max(abs(outA)*TImpPer/100,TImpMin);
  impLag = (outA>0) ? impLag*KImpRfact : impLag/KImpRfact;
  perLag = TImpPer;
  if( outA > 0 ) impQup = true; else impQdwn = true;  
}
else if( perLag > 0 )
{
  perLag -= 1e3/f_frq;
  if( (TImpPer-perLag) >= impLag ) 
  {
    impQup = impQdwn = false;
    if( !auto ) perLag = 0;
  }
}

if( !auto ) manIn = 50;

if( impQupTm > 1 || (impQupTm && !impQup) ) impQupTm--;
if( impQup && !impQupTm ) impQupTm = 2*f_frq;
if( impQdwnTm > 1 || (impQdwnTm && !impQdwn) ) impQdwnTm--;
if( impQdwn && !impQdwnTm ) impQdwnTm = 2*f_frq;

if( impQup || impQdwn ) integ = 0;

if( impAnImit )
{
  if( impQup ) out += 100/(impAnOut*f_frq);
  if( impQdwn ) out -= 100/(impAnOut*f_frq);
  return max(0,min(100,out));
}
return impAnOut;','','PID импульсный','Специализированный импульсный ПИД регулятор реализованный по особому алгоритму с компенсацией двойного интегрирования.','');
CREATE TABLE 'flb_regEl_io' ("F_ID" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"TYPE" INTEGER DEFAULT '' ,"MODE" INTEGER DEFAULT '' ,"DEF" TEXT DEFAULT '' ,"uk#DEF" TEXT DEFAULT '' ,"HIDE" INTEGER DEFAULT '' ,"POS" INTEGER DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DEF" TEXT DEFAULT '' , PRIMARY KEY ("F_ID","ID"));
INSERT INTO "flb_regEl_io" VALUES('pidUnif','var','Variable','Змінна',2,0,'0','',0,0,'Переменная','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','sp','Setpoint','Завдання',2,0,'0','',0,1,'Задание','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','max','Scale: maximum','Макс. шкали',2,0,'100','',0,2,'Максимум шкалы','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','min','Scale: minimum','Мін. шкали',2,0,'0','',0,3,'Минимум шкалы','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','manIn','Manual input','Ввід ручного',2,0,'0','',0,4,'Ручной ввод','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','out','Output (%)','Вихід (%)',2,2,'0','',0,5,'Выход (%)','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','impQup','Impulse output up','Імп. вихід у гору',3,1,'0','',0,6,'Имп. выход вверх','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','impQdwn','Impulse output down','Імп. вихід до низу',3,1,'0','',0,7,'Имп. выход вниз','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','analog','Analog','Аналоговий',3,0,'1','',0,8,'Аналоговый','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','auto','Automate','Автомат',3,0,'0','',0,9,'Автомат','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','casc','Cascade','Каскад',3,0,'0','',0,10,'Каскад','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','Kp','Gain','Kп',2,0,'1','',0,11,'Kп','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','Ti','Ti (ms)','Tі (мс)',1,0,'1000','',0,12,'Tи (мс)','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','Td','Td (ms)','Tд (мс)',1,0,'0','',0,14,'Tд (мс)','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','TImpPer','T impulses period (ms)','T періодичність імпульсів (мс)',1,0,'5000','',0,16,'T период импульса (мс)','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','TImpMin','T minimal impulse (ms)','Мінімальна довжина імпульсу (мс)',1,0,'500','',0,17,'T минимальная длина импульса (мс)','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','KImpRfact','Impulse rate factor','Рейтинг фактор імпульсів',2,0,'1','',0,18,'Рейтинг фактор импульса','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','Hup','Upper output border (%)','Верхня границя виходу (%)',2,0,'100','',0,19,'Верхняя граница выхода (%)','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','Hdwn','Neither output border (%)','Нижня границя виходу (%)',2,0,'0','',0,20,'Нижняя граница выхода (%)','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','Zi','Insensitivity (%)','Нечутливість (%)',2,0,'0','',0,21,'Нечувствительность (%)','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','K1','Coefficient of input 1','Коеф. входу 1',2,0,'0','',0,23,'Коеф. входа 1','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','in1','Input 1','Вхід 1',2,0,'0','',0,24,'Вход 1','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','K2','Coefficient of input 2','Коеф. входу 2',2,0,'0','',0,25,'Коеф. входа 2','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','in2','Input 2','Вхід 2',2,0,'0','',0,26,'Вход 2','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','K3','Coefficient of input 3','Коеф. входу 3',2,0,'0','',0,27,'Коеф. входа 3','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','in3','Input 3','Вхід 3',2,0,'0','',0,28,'Вход 3','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','K4','Coefficient of input 4','Коеф. входу 4',2,0,'0','',0,29,'Коеф. входа 4','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','in4','Input 4','Вхід 4',2,0,'0','',0,30,'Вход 4','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','impAnOut','Analog position of impulse regulator','Аналогове положення імпульсного регулятора або шви',2,0,'0','',0,31,'Аналоговое положение импульсного регулятора','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','int','Integral accumulation','Знач. інтегралу',2,1,'0','',1,38,'Накопление интеграла','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','dif','Differential accumulation','Знач. диференціалу',2,1,'0','',1,39,'Накопление производной','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','lag','Lag accumulation','Значення затримки',2,1,'0','',1,40,'Накопление задержки','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','impLag','Impulse lag time','Час утримання імпульсу',2,1,'0','',1,41,'Время удержания импульса','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','perLag','New impulse generation time','Час очікування генерації нового імпульсу',2,1,'0','',1,42,'Время генерации нового импульса','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','var','Variable','Змінна',2,0,'0','',0,0,'Переменная','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','sp','Setpoint','Завдання',2,0,'0','',0,1,'Задание','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','max','Scale: maximum','Макс. шкали',2,0,'100','',0,2,'Максимум шкалы','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','min','Scale: minimum','Мін. шкали',2,0,'0','',0,3,'Минимум шкалы','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','manIn','Manual input','Ввід ручного',2,0,'0','',0,4,'Ручной ввод','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','out','Output (%)','Вихід (%)',2,2,'0','',0,5,'Выход (%)','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','impQup','Impulse output up','Імп. вихід у гору',3,1,'0','',0,6,'Имп. выход вверх','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','impQdwn','Impulse output down','Імп. вихід до низу',3,1,'0','',0,7,'Имп. выход вниз','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','analog','Analog','Аналоговий',3,0,'1','',0,8,'Аналог','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','auto','Automate','Автомат',3,0,'0','',0,9,'Автомат','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','casc','Cascade','Каскад',3,0,'0','',0,10,'Каскад','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','Kp','Gain','Kп',2,0,'1','',0,11,'Kп','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','Ti','Тi (ms)','Tі (мс)',1,0,'1000','',0,12,'Tи (мс)','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','Kd','Gain differential','Кп диференціалу',2,0,'1','',0,13,'Кп производной','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','Td','Td (ms)','Tд (мс)',1,0,'0','',0,14,'Tд (мс)','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','Tzd','T differential lag (ms)','T фільтра диференціалу (мс)',1,0,'0','',0,15,'T задержки диференциров. (мс)','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','TImpPer','T impulses period (ms)','T періодичність імпульсів (мс)',1,0,'5000','',0,16,'T период импульса (мс)','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','TImpMin','T minimal impulse (ms)','Мінімальна довжина імпульсу (мс)',1,0,'500','',0,17,'T минимальная длина импульса (мс)','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','KImpRfact','Impulse rate factor','Рейтинг фактор імпульсів',2,0,'1','',0,18,'Рейтинг фактор импульса','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','Hup','Upper output border (%)','Верхня границя виходу (%)',2,0,'100','',0,19,'Верхняя граница выхода (%)','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','Hdwn','Neither output border (%)','Нижня границя виходу (%)',2,0,'0','',0,20,'Нижняя граница выхода (%)','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','Zi','Insensitivity (%)','Нечутливість (%)',2,0,'0','',0,21,'Нечувствительность (%)','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','followSp','Follow to variable by setpoint into manual','Відслідковувати завдання за змінною у ручному',3,0,'1','',0,22,'Следить заданием за переменной в ручном','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','K1','Coefficient of input 1','Коеф. входу 1',2,0,'0','',0,23,'Коеф. входа 1','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','in1','Input 1','Вхід 1',2,0,'0','',0,24,'Вход 1','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','K2','Coefficient of input 2','Коеф. входу 2',2,0,'0','',0,25,'Коеф. входа 2','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','in2','Input 2','Вхід 2',2,0,'0','',0,26,'Вход 2','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','K3','Coefficient of input 3','Коеф. входу 3',2,0,'0','',0,27,'Коеф. входа 3','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','in3','Input 3','Вхід 3',2,0,'0','',0,28,'Вход 3','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','K4','Coefficient of input 4','Коеф. входу 4',2,0,'0','',0,29,'Коеф. входа 4','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','in4','Input 4','Вхід 4',2,0,'0','',0,30,'Вход 4','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','impAnOut','Analog position of impulse regulator','Аналогове положення імпульсного регулятора або шви',2,0,'0','',0,31,'Аналоговое положение импульсного регулятора или ск','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','Kd','Gain differential','Кп диференціалу',2,0,'1','',0,13,'Кп производной','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','impAnImit','Analog position imitation','Імітація аналогового положення',3,0,'0','',0,32,'Имитация аналогового положения','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','impQupTm','Imp. output up lag for GUI','Імп. вихід вгору утримано для GUI',1,1,'0','',0,33,'Имп. выход вверх удерж. для GUI','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','impQdwnTm','Imp. output down lag for GUI','Імп. вихід додолу утримано для GUI',1,1,'0','',0,34,'Имп. выход вниз удерж. для GUI','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','f_frq','Calc frequency (Гц)','Частота обчислення (Гц)',2,0,'1','',1,35,'Частота вычисления (Гц)','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','f_start','First start','Перший запуск',3,0,'0','',1,36,'Первый запуск','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','f_stop','Last start','Останній запуск',3,0,'0','',1,37,'Последний запуск','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','impLag','Impulse lag time','Час утримання імпульсу',2,1,'0','',1,38,'Время удержания импульса','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','perLag','New impulse generation time','Час очікування генерації нового імпульсу',2,1,'0','',1,39,'Время генерации нового импульса','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','integ','Integral accumulation','Накопичення інтегралу',2,1,'0','',1,40,'Накопление интеграла','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','difer','Differential accumulation','Накопичення диференціалу',2,1,'0','',1,41,'Накопление производной','');
INSERT INTO "flb_regEl_io" VALUES('pidUnifD','dlag','Differential lag accumulation','Фільтр диференціалу',2,1,'0','',1,42,'Накопление задержки производной','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','Tzd','T differential lag (ms)','T фільтра диференціалу (мс)',1,0,'0','',0,15,'T задержки диференциров. (мс)','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','followSp','Follow to variable by setpoint into manual','Відслідковувати завдання за змінною у ручному',3,0,'1','',0,22,'Следить заданием за переменной в ручном','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','impAnImit','Analog position imitation','Імітація аналогового положення',3,0,'0','',0,32,'Имитация аналогового положения','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','impQupTm','Imp. output up lag for GUI',' Імп. вихід вгору утримано для GUI',1,1,'0','',0,33,'Имп. выход вверх удерж. для GUI','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','impQdwnTm','Imp. output down lag for GUI','Імп. вихід додолу утримано для GUI',1,1,'0','',0,34,'Имп. выход вниз удерж. для GUI','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','f_frq','Calc frequency (Hz)','Частота обчислення (Гц)',1,0,'1','',1,35,'Частота вычисления (Гц)','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','f_start','First start','Перший запуск',3,0,'0','',1,36,'Первый запуск','');
INSERT INTO "flb_regEl_io" VALUES('pidUnif','f_stop','Last start','Останній запуск',3,0,'0','',1,37,'Последний запуск','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','var','Variable','Змінна',2,0,'0','',0,0,'Переменная','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','sp','Setpoint','Завдання',2,0,'0','',0,1,'Задание','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','max','Scale: maximum','Макс. шкали',2,0,'100','',0,2,'Максимум шкалы','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','min','Scale: minimum','Мін. шкали',2,0,'0','',0,3,'Минимум шкалы','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','manIn','Manual input','Ввід ручного',2,0,'0','',0,4,'Ручной ввод','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','out','Output (%)','Вихід (%)',2,2,'0','',0,5,'Выход (%)','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','outPID','PID output','Вихід ПІД',2,1,'0','',0,6,'Выход PID','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','impQup','Impulse output up','Імп. вихід вгору',3,1,'0','',0,7,'Имп. выход вверх','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','impQdwn','Impulse output down','Імп. вихід додолу',3,1,'0','',0,8,'Имп. выход вниз','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','auto','Automate','Автомат',3,0,'0','',0,9,'Автомат','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','casc','Cascade','Каскад',3,0,'0','',0,10,'Каскад','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','Kp','Gain','Kп',2,0,'1','',0,11,'Kп','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','Ki','Gain integer','Кп інтегралу',2,0,'1','',0,12,'Кп интеграла','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','Ti','Ti (ms)','Tі (мс)',1,0,'1000','',0,13,'Tи (мс)','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','Kd','Gain differential','Кп диференціалу',2,0,'1','',0,14,'Кп производной','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','Td','Td (ms)','Tд (мс)',1,0,'0','',0,15,'Tд (мс)','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','Tzd','T differential lag (ms)','T фільтра диференціалу (мс)',1,0,'0','',0,16,'T задержки диференциров. (мс)','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','TImpPer','T impulses period (ms)','T періодичність імпульсів (мс)',1,0,'5000','',0,17,'T период импульса (мс)','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','TImpMin','T minimal impulse (ms)','Мінімальна довжина імпульсу (мс)',1,0,'500','',0,18,'T минимальная длина импульса (мс)','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','KImpRfact','Impulse rate factor','Рейтинг фактор імпульсів',2,0,'1','',0,19,'Рейтинг фактор импульса','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','Hup','Upper output border (%)','Верхня границя виходу (%)',2,0,'100','',0,20,'Верхняя граница выхода (%)','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','Hdwn','Neither output border (%)','Нижня границя виходу (%)',2,0,'0','',0,21,'Нижняя граница выхода (%)','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','Zi','Insensitivity (%)','Нечутливість (%)',2,0,'0','',0,22,'Нечувствительность (%)','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','followSp','Follow to variable by setpoint into manual','Відслідковувати завдання за змінною у ручному',3,0,'1','',0,23,'Следить заданием за переменной в ручном','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','K1','Coefficient of input 1','Коеф. входу 1',2,0,'0','',0,24,'Коеф. входа 1','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','in1','Input 1','Вхід 1',2,0,'0','',0,25,'Вход 1','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','K2','Coefficient of input 2','Коеф. входу 2',2,0,'0','',0,26,'Коеф. входа 2','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','in2','Input 2','Вхід 2',2,0,'0','',0,27,'Вход 2','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','K3','Coefficient of input 3','Коеф. входу 3',2,0,'0','',0,28,'Коеф. входа 3','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','in3','Input 3','Вхід 3',2,0,'0','',0,29,'Вход 3','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','K4','Coefficient of input 4','Коеф. входу 4',2,0,'0','',0,30,'Коеф. входа 4','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','in4','Input 4','Вхід 4',2,0,'0','',0,31,'Вход 4','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','impAnOut','Analog position of impulse regulator','Аналогове положення імпульсного регулятора або шви',2,0,'0','',0,32,'Аналоговое положение импульсного регулятора или ск','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','impAnImit','Analog position imitation','Імітація аналогового положення',3,0,'0','',0,33,'Имитация аналогового положения','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','impQupTm','Imp. output up lag for GUI','Імп. вихід вгору утримано для GUI',1,1,'0','',0,34,'Имп. выход вверх удерж. для GUI','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','impQdwnTm','Imp. output down lag for GUI','Імп. вихід додолу утримано для GUI',1,1,'0','',0,35,'Имп. выход вниз удерж. для GUI','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','f_frq','Calc frequency (Hz)','Частота обчислення (Гц)',2,0,'1','',1,36,'Частота вычисления (Гц)','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','f_start','First start','Перший запуск',3,0,'0','',1,37,'Первый запуск','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','f_stop','Last start','Останній запуск',3,0,'0','',1,38,'Последний запуск','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','impLag','Impulse lag time','Час утримання імпульсу',2,1,'0','',1,39,'Время удержания импульса','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','perLag','New impulse generation time','Час очікування генерації нового імпульсу',2,1,'0','',1,40,'Время генерации нового импульса','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','integ','Integral accumulation','Накопичення інтегралу',2,1,'0','',1,41,'Накопление интеграла','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','difer','Differential accumulation','Накопичення диференціалу',2,1,'0','',1,42,'Накопление производной','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','dlag','Differential lag accumulation','Фільтр диференціалу',2,1,'0','',1,43,'Накопление задержки производной','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','err1','Error step back','Помилка крок назад',2,1,'0','',1,44,'Ошибка шаг назад','');
INSERT INTO "flb_regEl_io" VALUES('pidImp','err2','Error two step back','Помилка два кроки назад',2,1,'0','',1,45,'Ошибка два шага назад','');
CREATE TABLE 'lib_Controllers_io' ("F_ID" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"TYPE" INTEGER DEFAULT '' ,"MODE" INTEGER DEFAULT '' ,"DEF" TEXT DEFAULT '' ,"HIDE" INTEGER DEFAULT '' ,"POS" INTEGER DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DEF" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#DEF" TEXT DEFAULT '' , PRIMARY KEY ("F_ID","ID"));
INSERT INTO "lib_Controllers_io" VALUES('prescr','dbDB','DB:DB with tables name',0,1,'',0,4,'БД:Имя БД с таблицами','','БД:Ім''я БД з таблицями','');
INSERT INTO "lib_Controllers_io" VALUES('prescr','dbComs','DB:Table with commands',0,1,'',0,5,'БД:Таблица с коммандами','','БД:Таблиця з командами','');
INSERT INTO "lib_Controllers_io" VALUES('prescr','dbProgs','DB:Table with programms',0,1,'',0,6,'БД:Таблица с программами','','БД:Таблиця з програмами','');
INSERT INTO "lib_Controllers_io" VALUES('prescr','mode','Mode: Finish(-2); Error(-1); Stop(0); Run (1); Pau',1,1,'',0,8,'Режим: Оконч.(-2); Ошибка(-1); Стоп(0); Исп.(1); П','','Режим: Окінч.(-2); Помилка(-1); Стоп(0); Вик.(1); ','');
INSERT INTO "lib_Controllers_io" VALUES('prescr','curMode','Current mode',1,0,'',0,9,'Текущий режим','','Поточний режим','');
INSERT INTO "lib_Controllers_io" VALUES('prescr','prog','Name of selected programm',0,1,'',0,10,'Имя выбранной программы','','Ім''я обраної програми','');
INSERT INTO "lib_Controllers_io" VALUES('prescr','startTm','Start time (seconds)',1,0,'',0,11,'Время запуска (секунды)','','Час запуску (секунди)','');
INSERT INTO "lib_Controllers_io" VALUES('prescr','curCom','Current command-step',1,0,'',0,12,'Текущая комманда-шаг','','Поточна команда-крок','');
INSERT INTO "lib_Controllers_io" VALUES('prescr','work','Work programm',4,0,'',0,13,'Рабочая программа','','Робоча програма','');
INSERT INTO "lib_Controllers_io" VALUES('prescr','comsCntr','Commands controller',0,0,'',0,7,'Контроллер команд','','Контролер команд','');
INSERT INTO "lib_Controllers_io" VALUES('prescr','comLs','Commands list',4,0,'',0,14,'Список команд','','Перелік команд','');
INSERT INTO "lib_Controllers_io" VALUES('prescr','clcCnt','Cycles count',1,0,'',0,15,'Счётчик циклов','','Лічильник циклів','');
INSERT INTO "lib_Controllers_io" VALUES('test','offset','Offset',1,0,'100',0,4,'','','','');
INSERT INTO "lib_Controllers_io" VALUES('test','out','Out',2,1,'',0,5,'','','','');
INSERT INTO "lib_Controllers_io" VALUES('test','test','Test',0,1,'',0,6,'','','','');
INSERT INTO "lib_Controllers_io" VALUES('test','rez','Rezult',0,1,'',0,7,'','','','');
INSERT INTO "lib_Controllers_io" VALUES('test1','clc','Clock',2,0,'',0,0,'','','','');
INSERT INTO "lib_Controllers_io" VALUES('test1','p_tm','Previous time (s)',1,1,'0',0,1,'','','','');
INSERT INTO "lib_Controllers_io" VALUES('test1','rez','Rezult',2,1,'0',0,2,'','','','');
CREATE TABLE 'lib_servProc' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"MAXCALCTM" INTEGER DEFAULT '' ,"FORMULA" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"uk#FORMULA" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"ru#FORMULA" TEXT DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO "lib_servProc" VALUES('procArh','Archives recalc','',600,'using Special.FLibSYS;

//messPut("UserProc/ProcessArch",1,"Archive open");
a_src = vArh(fromarch);
a_dst = vArh(toarch);

//messPut("UserProc/ProcessArch",1,"Archive process");

bf_per = 100;	//Buffers period microseconds
bf_size = 100000;	//Buffers size

//Open value buffers
bf_src = vArhBuf(1,bf_size,bf_per,true,true);
bf_dst = vArhBuf(4,bf_size,bf_per,true,true);

//Parse time string
begtm = tmStrPTime(begs);
endtm = tmStrPTime(ends);

begwtm = begtm;
while( begwtm<endtm )
{
    //Get value
    endwtm = begwtm+bf_size*bf_per/1000000;
    if(endwtm>endtm) endwtm = endtm;
    messPut("UserProc/ProcessArch",1,"Values from: "+begwtm+", to: "+endwtm);
    bf_src.copy(a_src,begwtm,0,endwtm,0);

    buf_beg = begwtm; buf_beg_u = 0;
    buf_end = endwtm; buf_end_u = 0;
    while( !(buf_beg>=buf_end && buf_beg_u>buf_end_u) )
    {
        r_val = bf_src.get(buf_beg,buf_beg_u,true);
        if(r_val == EVAL_INT) bf_dst.set(EVAL_REAL,buf_beg,buf_beg_u);
        else bf_dst.set(200.*((10.*r_val/32768.)-1.)/4.,buf_beg,buf_beg_u);

        buf_beg_u++;
    }

    a_dst.copy(bf_dst,begwtm,0,endwtm,0);
    begwtm = endwtm;
}','Перерахунок архівів','','using Special.FLibSYS;

//messPut("UserProc/ProcessArch",1,"Відкриття архівів");
a_src = vArh(fromarch);
a_dst = vArh(toarch);

//messPut("UserProc/ProcessArch",1,"Обробка архівів");

bf_per = 100;	//Buffers period microseconds
bf_size = 100000;	//Buffers size

//Open value buffers
bf_src = vArhBuf(1,bf_size,bf_per,true,true);
bf_dst = vArhBuf(4,bf_size,bf_per,true,true);

//Parse time string
begtm = tmStrPTime(begs);
endtm = tmStrPTime(ends);

begwtm = begtm;
while( begwtm<endtm )
{
    //Get value
    endwtm = begwtm+bf_size*bf_per/1000000;
    if(endwtm>endtm) endwtm = endtm;
    messPut("UserProc/ProcessArch",1,"Values from: "+begwtm+", to: "+endwtm);
    bf_src.copy(a_src,begwtm,0,endwtm,0);

    buf_beg = begwtm; buf_beg_u = 0;
    buf_end = endwtm; buf_end_u = 0;
    while( !(buf_beg>=buf_end && buf_beg_u>buf_end_u) )
    {
        r_val = bf_src.get(buf_beg,buf_beg_u,true);
        if(r_val == EVAL_INT) bf_dst.set(EVAL_REAL,buf_beg,buf_beg_u);
        else bf_dst.set(200.*((10.*r_val/32768.)-1.)/4.,buf_beg,buf_beg_u);

        buf_beg_u++;
    }

    a_dst.copy(bf_dst,begwtm,0,endwtm,0);
    begwtm = endwtm;
}','Пересчёт архивов','','using Special.FLibSYS;

//messPut("UserProc/ProcessArch",1,"Открытие архивов");
a_src = vArh(fromarch);
a_dst = vArh(toarch);

//messPut("UserProc/ProcessArch",1,"Обработка архивов");

bf_per = 100;	//Buffers period microseconds
bf_size = 100000;	//Buffers size

//Open value buffers
bf_src = vArhBuf(1,bf_size,bf_per,true,true);
bf_dst = vArhBuf(4,bf_size,bf_per,true,true);

//Parse time string
begtm = tmStrPTime(begs);
endtm = tmStrPTime(ends);

begwtm = begtm;
while( begwtm<endtm )
{
    //Get value
    endwtm = begwtm+bf_size*bf_per/1000000;
    if(endwtm>endtm) endwtm = endtm;
    messPut("UserProc/ProcessArch",1,"Values from: "+begwtm+", to: "+endwtm);
    bf_src.copy(a_src,begwtm,0,endwtm,0);

    buf_beg = begwtm; buf_beg_u = 0;
    buf_end = endwtm; buf_end_u = 0;
    while( !(buf_beg>=buf_end && buf_beg_u>buf_end_u) )
    {
        r_val = bf_src.get(buf_beg,buf_beg_u,true);
        if(r_val == EVAL_INT) bf_dst.set(EVAL_REAL,buf_beg,buf_beg_u);
        else bf_dst.set(200.*((10.*r_val/32768.)-1.)/4.,buf_beg,buf_beg_u);

        buf_beg_u++;
    }

    a_dst.copy(bf_dst,begwtm,0,endwtm,0);
    begwtm = endwtm;
}');
INSERT INTO "lib_servProc" VALUES('crc16','CRC 16','',10,'out = 0xFFFF;
for( i = 0; i < in.length; i++ )
{
  out = out^in.charCodeAt(i);
  for( j=0; j<8; j++ ) out = (out&1) ? ((out>>1)^0xA001) : out >> 1;
}','','','','','','');
INSERT INTO "lib_servProc" VALUES('releaseTests','Release tests','',10,'nNm = "testNode";
node  = false;		//Process node
testREZ = "";		//Test rezult
rez = "";

//**********************************
//> Get config file name
SYS.cntrReq(req=SYS.XMLNode("get").setAttr("path","/%2fgen%2fconfig"));
configPath = req.text();
//> Save original config file
SYS.fileWrite(configPath+".orig",SYS.fileRead(configPath));
//> Load config file tree
cfgTree=SYS.XMLNode();
cfgTree.load(configPath,true,true);

//**********************************
//> System
if(sub <= 0)
{
	testREZ = SYS.Special.SystemTests.XML(configPath);
	rez += "<tr><td>Control interface: Test of the XML file parsing</td><td>"+testREZ+"</td></tr>\n";
	testREZ = SYS.Special.SystemTests.SysContrLang("/Archive/FSArch/mess_StatErrors/%2fprm%2fst");
	rez += "<tr><td>Control interface: Test of the control system language</td><td>"+testREZ+"</td></tr>\n";
	testREZ = SYS.Special.SystemTests.Base64Code();
	rez += "<tr><td>SYS: Tests of the Mime Base64 encoding algorithm</td><td>"+testREZ+"</td></tr>\n";
}

//**********************************
//> Security User/Group tests
if(sub < 0 || sub == 1)
{
	testREZ = "PASSED";
	//>> User
	nAddr = "/Security/usr_"+nNm;
	if(SYS.cntrReq(SYS.XMLNode("add").setAttr("path","/Security/%2fbr%2fusr_").setText(nNm)) != 0 || !(node=SYS.nodeAt(nAddr))) testREZ = "FAILED: user add";
	else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fprm%2fdscr").setText("Test")) != 0 || node.cfg("DESCR") != "Test") testREZ = "FAILED: change user";
	else if(SYS.cntrReq(SYS.XMLNode("save").setAttr("path",nAddr+"/%2fobj")) != 0) testREZ = "FAILED: save user";
	else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fprm%2fdscr").setText("Test 1")) != 0 || 
		SYS.cntrReq(SYS.XMLNode("load").setAttr("path",nAddr+"/%2fobj")) != 0 || node.cfg("DESCR") != "Test") testREZ = "FAILED: load user";
	else
	{
		node = false;
		if(SYS.cntrReq(SYS.XMLNode("del").setAttr("path","/Security/%2fbr%2fusr_").setText(nNm)) != 0 || SYS.nodeAt(nAddr)) testREZ = "FAILED: user delete";
		else
		{
			cfgTree.getElementBy("sub_Security").childAdd("tbl").setAttr("id","Security_user").childAdd("fld").setAttr("NAME",nNm+"_cfg");
			cfgTree.save(0x20, configPath);
			SYS.cntrReq(SYS.XMLNode("scan").setAttr("path","/%2fgen%2fconfig"));		//Reload config command
			if(!SYS.nodeAt(nAddr+"_cfg")) testREZ = "FAILED: load user from config";
		}
	}
	//> Group
	nAddr = "/Security/grp_"+nNm;
	if(testREZ == "PASSED" && (SYS.cntrReq(SYS.XMLNode("add").setAttr("path","/Security/%2fbr%2fgrp_").setText(nNm)) != 0 || !(node=SYS.nodeAt(nAddr)))) testREZ = "FAILED: group add";
	else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fprm%2fdscr").setText("Test")) != 0 || node.cfg("DESCR") != "Test") testREZ = "FAILED: change group";
	else if(SYS.cntrReq(SYS.XMLNode("save").setAttr("path",nAddr+"/%2fobj")) != 0) testREZ = "FAILED: save group";
	else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fprm%2fdscr").setText("Test 1")) != 0 || 
		SYS.cntrReq(SYS.XMLNode("load").setAttr("path",nAddr+"/%2fobj")) != 0 || node.cfg("DESCR") != "Test") testREZ = "FAILED: load group";
	else
	{
		node = false;
		if(SYS.cntrReq(SYS.XMLNode("del").setAttr("path","/Security/%2fbr%2fgrp_").setText(nNm)) != 0 || SYS.nodeAt(nAddr)) testREZ = "FAILED: group delete";
		else
		{
			cfgTree.getElementBy("sub_Security").childAdd("tbl").setAttr("id","Security_grp").childAdd("fld").setAttr("NAME",nNm+"_cfg").setAttr("USERS",nNm+"_cfg");
			cfgTree.save(0x20, configPath);
			SYS.cntrReq(SYS.XMLNode("scan").setAttr("path","/%2fgen%2fconfig"));		//Reload config command
			if(!SYS.nodeAt(nAddr+"_cfg")) testREZ = "FAILED: load group from config";
		}
	}
	rez += "<tr><td>Security: User/Group create/remove, change, load, save and load from config</td><td>"+testREZ+"</td></tr>\n";
}

//**********************************
//> Transport subsystem tests
if(sub < 0 || sub == 2)
{
	nList = SYS.Transport.nodeList("mod_");
	cfgTree.getElementBy("sub_Transport").childAdd("tbl").setAttr("id","Transport_in");
	cfgTree.getElementBy("sub_Transport").childAdd("tbl").setAttr("id","Transport_out");
	for(i_n = 0; i_n < nList.length; i_n++)
	{
		testREZ = "PASSED";
		//> Input transport
		nAddr = "/Transport/"+nList[i_n]+"/in_"+nNm;
		if(SYS.cntrReq(SYS.XMLNode("add").setAttr("path","/Transport/"+nList[i_n]+"/%2fbr%2fin_").setAttr("id",nNm)) != 0 || !(node=SYS.nodeAt(nAddr))) testREZ = "FAILED: input add";
		else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fprm%2fcfg%2fdscr").setText("Test")) != 0 || node.cfg("DESCRIPT") != "Test") testREZ = "FAILED: change input";
		else if(SYS.cntrReq(SYS.XMLNode("save").setAttr("path",nAddr+"/%2fobj")) != 0) testREZ = "FAILED: save input";
		else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fprm%2fcfg%2fdscr").setText("Test 1")) != 0 || 
			SYS.cntrReq(SYS.XMLNode("load").setAttr("path",nAddr+"/%2fobj")) != 0 || node.cfg("DESCRIPT") != "Test") testREZ = "FAILED: load input";
		else
		{
			node = false;
			if(SYS.cntrReq(SYS.XMLNode("del").setAttr("path","/Transport/"+nList[i_n]+"/%2fbr%2fin_").setAttr("id",nNm)) != 0 || SYS.nodeAt(nAddr)) testREZ = "FAILED: input delete";
			else
			{
				cfgTree.getElementBy("Transport_in").childAdd("fld").setAttr("ID",nNm+"_cfg").setAttr("MODULE",nList[i_n].slice(4));
				cfgTree.save(0x20, configPath);
				SYS.cntrReq(SYS.XMLNode("scan").setAttr("path","/%2fgen%2fconfig"));		//Reload config command
				if(!SYS.nodeAt(nAddr+"_cfg")) testREZ = "FAILED: load input from config";
			}
		}
		//> Output transport
		nAddr = "/Transport/"+nList[i_n]+"/out_"+nNm;
		if(testREZ == "PASSED" && (SYS.cntrReq(SYS.XMLNode("add").setAttr("path","/Transport/"+nList[i_n]+"/%2fbr%2fout_").setAttr("id",nNm)) != 0 || !(node=SYS.nodeAt(nAddr)))) testREZ = "FAILED: output add";
		else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fprm%2fcfg%2fdscr").setText("Test")) != 0 || node.cfg("DESCRIPT") != "Test") testREZ = "FAILED: change output";
		else if(SYS.cntrReq(SYS.XMLNode("save").setAttr("path",nAddr+"/%2fobj")) != 0) testREZ = "FAILED: save output";
		else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fprm%2fcfg%2fdscr").setText("Test 1")) != 0 || 
			SYS.cntrReq(SYS.XMLNode("load").setAttr("path",nAddr+"/%2fobj")) != 0 || node.cfg("DESCRIPT") != "Test") testREZ = "FAILED: load output";
		else
		{
			node = false;
			if(SYS.cntrReq(SYS.XMLNode("del").setAttr("path","/Transport/"+nList[i_n]+"/%2fbr%2fout_").setAttr("id",nNm)) != 0 || SYS.nodeAt(nAddr)) testREZ = "FAILED: output delete";
			else
			{
				cfgTree.getElementBy("Transport_out").childAdd("fld").setAttr("ID",nNm+"_cfg").setAttr("MODULE",nList[i_n].slice(4));
				cfgTree.save(0x20, configPath);
				SYS.cntrReq(SYS.XMLNode("scan").setAttr("path","/%2fgen%2fconfig"));		//Reload config command
				if(!SYS.nodeAt(nAddr+"_cfg")) testREZ = "FAILED: load output from config";
			}
		}
		rez += "<tr><td>Transport."+nList[i_n].slice(4)+": Input/Output create/remove, change, load, save and load from config</td><td>"+testREZ+"</td></tr>\n";

		//> Disconnect/Connect module
		req = SYS.XMLNode("get").setAttr("path","/Transport/"+nList[i_n]+"/%2fhelp%2fm_inf%2fSource");
		SYS.cntrReq(req);
		testREZ = SYS.Special.SystemTests.SOAttach(req.text(),0);
		testREZ += "/"+SYS.Special.SystemTests.SOAttach(req.text(),0);
		rez += "<tr><td>Transport."+nList[i_n].slice(4)+": Disconnect/Connect</td><td>"+testREZ+"</td></tr>\n";
	}
}

//**********************************
//> Archive subsystem tests
if(sub < 0 || sub == 3)
{
	testREZ = SYS.Special.SystemTests.ValBuf();
	rez += "<tr><td>Archive: Tests of the values'' buffer.</td><td>"+testREZ+"</td></tr>\n";

	//>> Value archive
	testREZ = "PASSED";
	nAddr = "/Archive/va_"+nNm;
	if(SYS.cntrReq(SYS.XMLNode("add").setAttr("path","/Archive/%2fbr%2fva_").setAttr("id",nNm)) != 0 || !(node=SYS.nodeAt(nAddr))) testREZ = "FAILED: add";
	else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fprm%2fcfg%2fdscr").setText("Test")) != 0 || node.cfg("DESCR") != "Test") testREZ = "FAILED: change";
	else if(SYS.cntrReq(SYS.XMLNode("save").setAttr("path",nAddr+"/%2fobj")) != 0) testREZ = "FAILED: save";
	else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fprm%2fcfg%2fdscr").setText("Test 1")) != 0 || 
		SYS.cntrReq(SYS.XMLNode("load").setAttr("path",nAddr+"/%2fobj")) != 0 || node.cfg("DESCR") != "Test") testREZ = "FAILED: load";
	else
	{
		node = false;
		if(SYS.cntrReq(SYS.XMLNode("del").setAttr("path","/Archive/%2fbr%2fva_").setAttr("id",nNm)) != 0 || SYS.nodeAt(nAddr)) testREZ = "FAILED: delete";
		else
		{
			cfgTree.getElementBy("sub_Archive").childAdd("tbl").setAttr("id","Archive_val").childAdd("fld").setAttr("ID",nNm+"_cfg");
			cfgTree.save(0x20, configPath);
			SYS.cntrReq(SYS.XMLNode("scan").setAttr("path","/%2fgen%2fconfig"));		//Reload config command
			if(!SYS.nodeAt(nAddr+"_cfg")) testREZ = "FAILED: load from config";
		}
	}
	rez += "<tr><td>Archive: Value archive create/remove, change, load, save and load from config</td><td>"+testREZ+"</td></tr>\n";

	//>> Archivators
	nList = SYS.Archive.nodeList("mod_");
	cfgTree.getElementBy("sub_Archive").childAdd("tbl").setAttr("id","Archive_mess_proc");
	cfgTree.getElementBy("sub_Archive").childAdd("tbl").setAttr("id","Archive_val_proc");
	for(i_n = 0; i_n < nList.length; i_n++)
	{
		testREZ = "PASSED";
		//> Messages archivator
		nAddr = "/Archive/"+nList[i_n]+"/mess_"+nNm;
		if(SYS.cntrReq(SYS.XMLNode("add").setAttr("path","/Archive/"+nList[i_n]+"/%2fbr%2fmess_").setAttr("id",nNm)) != 0 || !(node=SYS.nodeAt(nAddr))) testREZ = "FAILED: messages add";
		else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fprm%2fcfg%2fdscr").setText("Test")) != 0 || node.cfg("DESCR") != "Test") testREZ = "FAILED: change messages";
		else if(SYS.cntrReq(SYS.XMLNode("save").setAttr("path",nAddr+"/%2fobj")) != 0) testREZ = "FAILED: save messages";
		else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fprm%2fcfg%2fdscr").setText("Test 1")) != 0 || 
			SYS.cntrReq(SYS.XMLNode("load").setAttr("path",nAddr+"/%2fobj")) != 0 || node.cfg("DESCR") != "Test") testREZ = "FAILED: load messages";
		else 
		{
			node = false;
			if(SYS.cntrReq(SYS.XMLNode("del").setAttr("path","/Archive/"+nList[i_n]+"/%2fbr%2fmess_").setAttr("id",nNm)) != 0 || SYS.nodeAt(nAddr)) testREZ = "FAILED: messages delete";
			else
			{
				cfgTree.getElementBy("Archive_mess_proc").childAdd("fld").setAttr("ID",nNm+"_cfg").setAttr("MODUL",nList[i_n].slice(4));
				cfgTree.save(0x20, configPath);
				SYS.cntrReq(SYS.XMLNode("scan").setAttr("path","/%2fgen%2fconfig"));		//Reload config command
				if(!SYS.nodeAt(nAddr+"_cfg")) testREZ = "FAILED: load messages from config";
			}
		}
		//> Value archivator
		nAddr = "/Archive/"+nList[i_n]+"/val_"+nNm;
		if(testREZ == "PASSED" && (SYS.cntrReq(SYS.XMLNode("add").setAttr("path","/Archive/"+nList[i_n]+"/%2fbr%2fval_").setAttr("id",nNm)) != 0 || !(node=SYS.nodeAt(nAddr)))) testREZ = "FAILED: values add";
		else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fprm%2fcfg%2fdscr").setText("Test")) != 0 || node.cfg("DESCR") != "Test") testREZ = "FAILED: change values";
		else if(SYS.cntrReq(SYS.XMLNode("save").setAttr("path",nAddr+"/%2fobj")) != 0) testREZ = "FAILED: save values";
		else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fprm%2fcfg%2fdscr").setText("Test 1")) != 0 || 
			SYS.cntrReq(SYS.XMLNode("load").setAttr("path",nAddr+"/%2fobj")) != 0 || node.cfg("DESCR") != "Test") testREZ = "FAILED: load values";
		else 
		{
			node = false;
			if(SYS.cntrReq(SYS.XMLNode("del").setAttr("path","/Archive/"+nList[i_n]+"/%2fbr%2fval_").setAttr("id",nNm)) != 0 || SYS.nodeAt(nAddr)) testREZ = "FAILED: values delete";
			else
			{
				cfgTree.getElementBy("Archive_val_proc").childAdd("fld").setAttr("ID",nNm+"_cfg").setAttr("MODUL",nList[i_n].slice(4));
				cfgTree.save(0x20, configPath);
				SYS.cntrReq(SYS.XMLNode("scan").setAttr("path","/%2fgen%2fconfig"));		//Reload config command
				if(!SYS.nodeAt(nAddr+"_cfg")) testREZ = "FAILED: load values from config";
			}
		}
		rez += "<tr><td>Archive."+nList[i_n].slice(4)+": Messages/Values create/remove, change, load, save and load from config</td><td>"+testREZ+"</td></tr>\n";

		//> Disconnect/Connect module
		req = SYS.XMLNode("get").setAttr("path","/Archive/"+nList[i_n]+"/%2fhelp%2fm_inf%2fSource");
		SYS.cntrReq(req);
		testREZ = SYS.Special.SystemTests.SOAttach(req.text(),0);
		testREZ += "/"+SYS.Special.SystemTests.SOAttach(req.text(),0);
		rez += "<tr><td>Archive."+nList[i_n].slice(4)+": Disconnect/Connect</td><td>"+testREZ+"</td></tr>\n";
	}
}

//**********************************
//> DB subsystem tests
if(sub < 0 || sub == 4)
{
	nList = SYS.BD.nodeList("mod_");
	for(i_n = 0; i_n < nList.length; i_n++)
	{
		testREZ = "PASSED";
		//> Messages archivator
		nAddr = "/BD/"+nList[i_n]+"/db_"+nNm;
		if(SYS.cntrReq(SYS.XMLNode("add").setAttr("path","/BD/"+nList[i_n]+"/%2fbr%2fdb_").setAttr("id",nNm)) != 0 || !(node=SYS.nodeAt(nAddr))) testREZ = "FAILED: add";
		else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fprm%2fcfg%2fdscr").setText("Test")) != 0 || node.cfg("DESCR") != "Test") testREZ = "FAILED: change";
		else if(SYS.cntrReq(SYS.XMLNode("save").setAttr("path",nAddr+"/%2fobj")) != 0) testREZ = "FAILED: save";
		else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fprm%2fcfg%2fdscr").setText("Test 1")) != 0 || 
			SYS.cntrReq(SYS.XMLNode("load").setAttr("path",nAddr+"/%2fobj")) != 0 || node.cfg("DESCR") != "Test") testREZ = "FAILED: load";
		else 
		{
			node = false;
			if(SYS.cntrReq(SYS.XMLNode("del").setAttr("path","/BD/"+nList[i_n]+"/%2fbr%2fdb_").setAttr("id",nNm)) != 0 || SYS.nodeAt(nAddr)) testREZ = "FAILED: delete";
			else
			{
				cfgTree.getElementBy("DB").childAdd("fld").setAttr("ID",nNm+"_cfg").setAttr("TYPE",nList[i_n].slice(4)).setAttr("EN",0);
				cfgTree.save(0x20, configPath);
				SYS.cntrReq(SYS.XMLNode("scan").setAttr("path","/%2fgen%2fconfig"));		//Reload config command
				if(!SYS.nodeAt(nAddr+"_cfg")) testREZ = "FAILED: load from config";
			}
		}
		rez += "<tr><td>DB."+nList[i_n].slice(4)+": create/remove, change, load, save and load from config</td><td>"+testREZ+"</td></tr>\n";

		//> Disconnect/Connect module
		req = SYS.XMLNode("get").setAttr("path","/BD/"+nList[i_n]+"/%2fhelp%2fm_inf%2fSource");
		SYS.cntrReq(req);
		testREZ = SYS.Special.SystemTests.SOAttach(req.text(),0);
		testREZ += "/"+SYS.Special.SystemTests.SOAttach(req.text(),0);
		rez += "<tr><td>DB."+nList[i_n].slice(4)+": Disconnect/Connect</td><td>"+testREZ+"</td></tr>\n";
	}
}

//**********************************
//> Protocol subsystem tests
if(sub < 0 || sub == 5)
{
	nList = SYS.Protocol.nodeList("mod_");
	for(i_n = 0; i_n < nList.length; i_n++)
	{
		//> Disconnect/Connect module
		req = SYS.XMLNode("get").setAttr("path","/Protocol/"+nList[i_n]+"/%2fhelp%2fm_inf%2fSource");
		SYS.cntrReq(req);
		testREZ = SYS.Special.SystemTests.SOAttach(req.text(),0);
		testREZ += "/"+SYS.Special.SystemTests.SOAttach(req.text(),0);
		rez += "<tr><td>Protocol."+nList[i_n].slice(4)+": Disconnect/Connect</td><td>"+testREZ+"</td></tr>\n";
	}
}

//**********************************
//> DAQ subsystem tests
if(sub < 0 || sub == 6)
{
	//>> Templates
	testREZ = "PASSED";
	nAddr = "/DAQ/tmplb_"+nNm;
	if(SYS.cntrReq(SYS.XMLNode("add").setAttr("path","/DAQ/%2fbr%2ftmplb_").setAttr("id",nNm)) != 0 || !(node=SYS.nodeAt(nAddr))) testREZ = "FAILED: template library add";
	else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2flib%2fcfg%2fdescr").setText("Test")) != 0 || node.cfg("DESCR") != "Test") testREZ = "FAILED: change template library";
	else if(SYS.cntrReq(SYS.XMLNode("save").setAttr("path",nAddr+"/%2fobj")) != 0) testREZ = "FAILED: save template library";
	else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2flib%2fcfg%2fdescr").setText("Test 1")) != 0 || 
		SYS.cntrReq(SYS.XMLNode("load").setAttr("path",nAddr+"/%2fobj")) != 0 || node.cfg("DESCR") != "Test") testREZ = "FAILED: load template library";
	else
	{
		node = false;

		//>> Template
		nAddrLb = nAddr+"/tmpl_"+nNm;
		if(SYS.cntrReq(SYS.XMLNode("add").setAttr("path",nAddr+"/%2fbr%2ftmpl_").setAttr("id",nNm)) != 0 || !(node=SYS.nodeAt(nAddrLb))) testREZ = "FAILED: template add";
		else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddrLb+"/%2ftmpl%2fcfg%2fdescr").setText("Test")) != 0 || node.cfg("DESCR") != "Test") testREZ = "FAILED: change template";
		else if(SYS.cntrReq(SYS.XMLNode("save").setAttr("path",nAddrLb+"/%2fobj")) != 0) testREZ = "FAILED: save template";
		else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddrLb+"/%2ftmpl%2fcfg%2fdescr").setText("Test 1")) != 0 || 
			SYS.cntrReq(SYS.XMLNode("load").setAttr("path",nAddrLb+"/%2fobj")) != 0 || node.cfg("DESCR") != "Test") testREZ = "FAILED: load template";
		node = false;

		if(SYS.cntrReq(SYS.XMLNode("del").setAttr("path","/DAQ/%2fbr%2ftmplb_").setAttr("id",nNm)) != 0 || SYS.nodeAt(nAddr)) testREZ = "FAILED: template library delete";
		else
		{
			cfgTree.getElementBy("sub_DAQ").childAdd("tbl").setAttr("id","tmplib").childAdd("fld").setAttr("ID",nNm+"_cfg").setAttr("DB","tmplib_"+nNm+"_cfg");
			cfgTree.getElementBy("sub_DAQ").childAdd("tbl").setAttr("id","tmplib_"+nNm+"_cfg").childAdd("fld").setAttr("ID",nNm+"_cfg");
			cfgTree.save(0x20, configPath);
			SYS.cntrReq(SYS.XMLNode("scan").setAttr("path","/%2fgen%2fconfig"));		//Reload config command
			if(!SYS.nodeAt(nAddr+"_cfg") || !SYS.nodeAt(nAddr+"_cfg/tmpl_"+nNm+"_cfg")) testREZ = "FAILED: load template library from config";
		}
	}
	rez += "<tr><td>DAQ: Templates library create/remove, change, load, save and load from config</td><td>"+testREZ+"</td></tr>\n";

	//>> Modules
	nList = SYS.DAQ.nodeList("mod_");
	for(i_n = 0; i_n < nList.length; i_n++)
	{
		testREZ = "PASSED";
		testREZ_ = "PASSED";
		//> Controller
		nAddr = "/DAQ/"+nList[i_n]+"/cntr_"+nNm;
		if(SYS.cntrReq(SYS.XMLNode("add").setAttr("path","/DAQ/"+nList[i_n]+"/%2fbr%2fcntr_").setAttr("id",nNm)) != 0 || !(node=SYS.nodeAt(nAddr))) testREZ = "FAILED: controller add";
		else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fcntr%2fcfg%2fDESCR").setText("Test")) != 0 || node.cfg("DESCR") != "Test") testREZ = "FAILED: controller change";
		else if(SYS.cntrReq(SYS.XMLNode("save").setAttr("path",nAddr+"/%2fobj")) != 0) testREZ = "FAILED: controller save";
		else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fcntr%2fcfg%2fDESCR").setText("Test 1")) != 0 || 
			SYS.cntrReq(SYS.XMLNode("load").setAttr("path",nAddr+"/%2fobj")) != 0 || node.cfg("DESCR") != "Test") testREZ = "FAILED: controller load";
		else 
		{
			node = false;

			//>> Parameter
			if(nList[i_n] != "mod_DAQGate")
			{
				nAddr1 = nAddr+"/prm_"+nNm;
				if(SYS.cntrReq(SYS.XMLNode("add").setAttr("path",nAddr+"/%2fbr%2fprm_").setAttr("id",nNm)) != 0 || !(node=SYS.nodeAt(nAddr1))) testREZ = "FAILED: parameter add";
				else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr1+"/%2fprm%2fcfg%2fDESCR").setText("Test")) != 0 || node.cfg("DESCR") != "Test") testREZ = "FAILED: parameter change";
				else if(SYS.cntrReq(SYS.XMLNode("save").setAttr("path",nAddr1+"/%2fobj")) != 0) testREZ = "FAILED: parameter save";
				else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr1+"/%2fprm%2fcfg%2fDESCR").setText("Test 1")) != 0 || 
					SYS.cntrReq(SYS.XMLNode("load").setAttr("path",nAddr1+"/%2fobj")) != 0 || node.cfg("DESCR") != "Test") testREZ = "FAILED: parameter load";
				node = false;
			}

			if(nList[i_n] == "mod_BlockCalc")
			{
				nAddr1 = nAddr+"/blk_"+nNm;
				if(SYS.cntrReq(SYS.XMLNode("add").setAttr("path",nAddr+"/%2fbr%2fblk_").setAttr("id",nNm)) != 0 || !(node=SYS.nodeAt(nAddr1))) testREZ_ = "FAILED: block add";
				else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr1+"/%2fblck%2fcfg%2fdescr").setText("Test")) != 0 || node.cfg("DESCR") != "Test") testREZ_ = "FAILED: block change";
				else if(SYS.cntrReq(SYS.XMLNode("save").setAttr("path",nAddr1+"/%2fobj")) != 0) testREZ_ = "FAILED: block save";
				else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr1+"/%2fblck%2fcfg%2fdescr").setText("Test 1")) != 0 || 
					SYS.cntrReq(SYS.XMLNode("load").setAttr("path",nAddr1+"/%2fobj")) != 0 || node.cfg("DESCR") != "Test") testREZ_ = "FAILED: block load";
				node = false;
			}
			if(nList[i_n] == "mod_JavaLikeCalc")
			{
				nAddr_ = "/DAQ/"+nList[i_n]+"/lib_"+nNm;
				if(SYS.cntrReq(SYS.XMLNode("add").setAttr("path","/DAQ/"+nList[i_n]+"/%2fbr%2flib_").setAttr("id",nNm)) != 0 || !(node=SYS.nodeAt(nAddr_))) testREZ_ = "FAILED: library add";
				else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr_+"/%2flib%2fcfg%2fdescr").setText("Test")) != 0 || node.cfg("DESCR") != "Test") testREZ_ = "FAILED: library change";
				else if(SYS.cntrReq(SYS.XMLNode("save").setAttr("path",nAddr_+"/%2fobj")) != 0) testREZ_ = "FAILED: library save";
				else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr_+"/%2flib%2fcfg%2fdescr").setText("Test 1")) != 0 || 
					SYS.cntrReq(SYS.XMLNode("load").setAttr("path",nAddr_+"/%2fobj")) != 0 || node.cfg("DESCR") != "Test") testREZ_ = "FAILED: library load";
				else
				{
					node = false;

					//>> Function
					nAddr_1 = nAddr_+"/fnc_"+nNm;
					if(SYS.cntrReq(SYS.XMLNode("add").setAttr("path",nAddr_+"/%2fbr%2ffnc_").setAttr("id",nNm)) != 0 || !(node=SYS.nodeAt(nAddr_1))) testREZ_ = "FAILED: function add";
					else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr_1+"/%2ffunc%2fcfg%2fdescr").setText("Test")) != 0 || node.cfg("DESCR") != "Test") testREZ_ = "FAILED: function change";
					else if(SYS.cntrReq(SYS.XMLNode("save").setAttr("path",nAddr_1+"/%2fobj")) != 0) testREZ_ = "FAILED: function save";
					else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr_1+"/%2ffunc%2fcfg%2fdescr").setText("Test 1")) != 0 || 
						SYS.cntrReq(SYS.XMLNode("load").setAttr("path",nAddr_1+"/%2fobj")) != 0 || node.cfg("DESCR") != "Test") testREZ_ = "FAILED: function load";
					node = false;
					if(SYS.cntrReq(SYS.XMLNode("del").setAttr("path","/DAQ/"+nList[i_n]+"/%2fbr%2flib_").setAttr("id",nNm)) != 0 || SYS.nodeAt(nAddr_)) testREZ_ = "FAILED: function delete";
				}			
			}

			if(SYS.cntrReq(SYS.XMLNode("del").setAttr("path","/DAQ/"+nList[i_n]+"/%2fbr%2fcntr_").setAttr("id",nNm)) != 0 || SYS.nodeAt(nAddr)) testREZ = "FAILED: controller delete";
			else
			{
				treeNd = cfgTree.getElementBy("sub_DAQ");
				if(!treeNd.getElementBy(nList[i_n])) treeNd.childAdd("node").setAttr("id",nList[i_n]);
				treeNd = treeNd.getElementBy(nList[i_n]);

				treeNd_t = treeNd.childAdd("tbl").setAttr("id","DAQ").childAdd("fld").setAttr("ID",nNm+"_cfg").setAttr("PRM_BD",nNm+"_cfg_prm").setAttr("ENABLE",1);
				if(nList[i_n] == "mod_JavaLikeCalc") treeNd_t.setAttr("FUNC","servProc.fnc_crc16");
				treeNd.childAdd("tbl").setAttr("id",nNm+"_cfg_prm").childAdd("fld").setAttr("SHIFR",nNm+"_cfg");
				if(nList[i_n] == "mod_BlockCalc")
				{
					treeNd_t.setAttr("BLOCK_SH",nNm+"_cfg_blcks");
					treeNd.childAdd("tbl").setAttr("id",nNm+"_cfg_blcks").childAdd("fld").setAttr("ID",nNm+"_cfg");
				}
				if(nList[i_n] == "mod_JavaLikeCalc")
				{
					treeNd.childAdd("tbl").setAttr("id","lib").childAdd("fld").setAttr("ID",nNm+"_cfg").setAttr("DB","lib_"+nNm+"_cfg");
					treeNd.childAdd("tbl").setAttr("id","lib_"+nNm+"_cfg").childAdd("fld").setAttr("ID",nNm+"_cfg");
				}

				cfgTree.save(0x20, configPath);
				SYS.cntrReq(SYS.XMLNode("scan").setAttr("path","/%2fgen%2fconfig"));		//Reload config command
				SYS.nodeAt(nAddr+"_cfg").enable(true);
				if(!SYS.nodeAt(nAddr+"_cfg")) testREZ = "FAILED: load controller from config";
				else
				{
					if(!SYS.nodeAt(nAddr+"_cfg/prm_"+nNm+"_cfg")) testREZ = "FAILED: load parameter from config";
					if(nList[i_n] == "mod_BlockCalc" && !SYS.nodeAt(nAddr+"_cfg/blk_"+nNm+"_cfg")) testREZ_ = "FAILED: load block from config";
					if(nList[i_n] == "mod_JavaLikeCalc")
					{
						if(!SYS.nodeAt(nAddr_+"_cfg")) testREZ_ = "FAILED: load library from config";
						else if(!SYS.nodeAt(nAddr_+"_cfg/fnc_"+nNm+"_cfg")) testREZ_ = "FAILED: load function from config";
					}
				}
			}
		}
		rez += "<tr><td>DAQ."+nList[i_n].slice(4)+": create/remove, change, load, save and load from config</td><td>"+testREZ+"</td></tr>\n";
		if(nList[i_n] == "mod_BlockCalc")
			rez += "<tr><td>DAQ."+nList[i_n].slice(4)+": Blocks create/remove, change, load, save and load from config</td><td>"+testREZ_+"</td></tr>\n";
		else if(nList[i_n] == "mod_JavaLikeCalc")
			rez += "<tr><td>DAQ."+nList[i_n].slice(4)+": Functions and its libraries create/remove, change, load, save and load from config</td><td>"+testREZ_+"</td></tr>\n";

		//> Disconnect/Connect module
		if(nList[i_n] != "mod_JavaLikeCalc")
		{
			req = SYS.XMLNode("get").setAttr("path","/DAQ/"+nList[i_n]+"/%2fhelp%2fm_inf%2fSource");
			SYS.cntrReq(req);
			testREZ = SYS.Special.SystemTests.SOAttach(req.text(),0);
			testREZ += "/"+SYS.Special.SystemTests.SOAttach(req.text(),0);
			rez += "<tr><td>DAQ."+nList[i_n].slice(4)+": Disconnect/Connect</td><td>"+testREZ+"</td></tr>\n";
		}
	}
}

//**********************************
//> UI subsystem tests
if(sub < 0 || sub == 7)
{
	nList = SYS.UI.nodeList("mod_");
	for(i_n = 0; i_n < nList.length; i_n++)
	{
		if(nList[i_n] == "mod_QTStarter" || nList[i_n] == "mod_QTCfg") continue;

		//> UI.VCAEngine tests
		if(nList[i_n] == "mod_VCAEngine")
		{
			//>> Widgets library
			testREZ = "PASSED";
			nAddr = "/UI/"+nList[i_n]+"/wlb_"+nNm;
			if(SYS.cntrReq(SYS.XMLNode("add").setAttr("path","/UI/"+nList[i_n]+"/%2fbr%2fwlb_").setAttr("id",nNm)) || !(node=SYS.nodeAt(nAddr))) testREZ = "FAILED: widgets library add";
			else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fobj%2fcfg%2fdescr").setText("Test")) || 
				SYS.cntrReq(req=SYS.XMLNode("get").setAttr("path",nAddr+"/%2fobj%2fcfg%2fdescr")) || req.text() != "Test") testREZ = "FAILED: change widgets library";
			else if(SYS.cntrReq(SYS.XMLNode("save").setAttr("path",nAddr+"/%2fobj"))) testREZ = "FAILED: save widgets library";
			else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fobj%2fcfg%2fdescr").setText("Test 1")) || 
				SYS.cntrReq(SYS.XMLNode("load").setAttr("path",nAddr+"/%2fobj")) || SYS.cntrReq(req=SYS.XMLNode("get").setAttr("path",nAddr+"/%2fobj%2fcfg%2fdescr")) ||
				req.text() != "Test") testREZ = "FAILED: load widgets library";
			else
			{
				node = false;

				//>> Widget
				nAddr1 = nAddr+"/wdg_"+nNm;
				if(SYS.cntrReq(SYS.XMLNode("add").setAttr("path",nAddr+"/%2fbr%2fwdg_").setAttr("id",nNm)) || !(node=SYS.nodeAt(nAddr1))) testREZ = "FAILED: widget add";
				else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr1+"/%2fwdg%2fcfg%2fdescr").setText("Test")) || 
					SYS.cntrReq(req=SYS.XMLNode("get").setAttr("path",nAddr1+"/%2fwdg%2fcfg%2fdescr")) || req.text() != "Test") testREZ = "FAILED: change widget";
				else if(SYS.cntrReq(SYS.XMLNode("save").setAttr("path",nAddr1+"/%2fobj"))) testREZ = "FAILED: save widget";
				else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr1+"/%2fwdg%2fcfg%2fdescr").setText("Test 1")) || 
					SYS.cntrReq(SYS.XMLNode("load").setAttr("path",nAddr1+"/%2fobj")) || SYS.cntrReq(req=SYS.XMLNode("get").setAttr("path",nAddr1+"/%2fwdg%2fcfg%2fdescr")) ||
						req.text() != "Test") testREZ = "FAILED: load widget";
				node = false;

				if(SYS.cntrReq(SYS.XMLNode("del").setAttr("path","/UI/"+nList[i_n]+"/%2fbr%2fwlb_").setAttr("id",nNm)) != 0 || SYS.nodeAt(nAddr)) testREZ = "FAILED: widgets library delete";
				else
				{
					cfgTree.getElementBy("mod_VCAEngine").childAdd("tbl").setAttr("id","LIB").childAdd("fld").setAttr("ID",nNm+"_cfg").setAttr("DB_TBL","wlib_"+nNm+"_cfg");
					cfgTree.getElementBy("mod_VCAEngine").childAdd("tbl").setAttr("id","wlib_"+nNm+"_cfg").childAdd("fld").setAttr("ID",nNm+"_cfg").setAttr("PARENT","/wlb_originals/wdg_Box");
					cfgTree.save(0x20, configPath);
					SYS.cntrReq(SYS.XMLNode("scan").setAttr("path","/%2fgen%2fconfig"));		//Reload config command
					if(!SYS.nodeAt(nAddr+"_cfg") || !SYS.nodeAt(nAddr+"_cfg/wdg_"+nNm+"_cfg")) testREZ = "FAILED: load widgets library from config";
				}
			}
			rez += "<tr><td>UI.VCAEngine: Widgets and widgets'' library create/remove, change, load, save and load from config</td><td>"+testREZ+"</td></tr>\n";

			//>> Project
			testREZ = "PASSED";
			nAddr = "/UI/"+nList[i_n]+"/prj_"+nNm;
			if(SYS.cntrReq(SYS.XMLNode("add").setAttr("path","/UI/"+nList[i_n]+"/%2fbr%2fprj_").setAttr("id",nNm)) || !(node=SYS.nodeAt(nAddr))) testREZ = "FAILED: project add";
			else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fobj%2fcfg%2fdescr").setText("Test"))  || 
				SYS.cntrReq(req=SYS.XMLNode("get").setAttr("path",nAddr+"/%2fobj%2fcfg%2fdescr")) || req.text() != "Test") testREZ = "FAILED: change project";
			else if(SYS.cntrReq(SYS.XMLNode("save").setAttr("path",nAddr+"/%2fobj")) != 0) testREZ = "FAILED: save project";
			else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr+"/%2fobj%2fcfg%2fdescr").setText("Test 1")) != 0 || 
				SYS.cntrReq(SYS.XMLNode("load").setAttr("path",nAddr+"/%2fobj")) != 0 || SYS.cntrReq(req=SYS.XMLNode("get").setAttr("path",nAddr+"/%2fobj%2fcfg%2fdescr")) ||
				req.text() != "Test") testREZ = "FAILED: load project";
			else
			{
				node = false;

				//>> Page
				nAddr1 = nAddr+"/pg_"+nNm;
				if(SYS.cntrReq(SYS.XMLNode("add").setAttr("path",nAddr+"/%2fbr%2fpg_").setAttr("id",nNm)) != 0 || !(node=SYS.nodeAt(nAddr1))) testREZ = "FAILED: page add";
				else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr1+"/%2fwdg%2fst%2fparent").setText("/wlb_originals/wdg_Box")) != 0) testREZ = "FAILED: change page";
				else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr1+"/%2fwdg%2fst%2fen").setText(1)) != 0) testREZ = "FAILED: change page";
				else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr1+"/%2fwdg%2fcfg%2fdescr").setText("Test")) != 0 || 
					SYS.cntrReq(req=SYS.XMLNode("get").setAttr("path",nAddr1+"/%2fwdg%2fcfg%2fdescr")) || req.text() != "Test") testREZ = "FAILED: change page";
				else if(SYS.cntrReq(SYS.XMLNode("save").setAttr("path",nAddr1+"/%2fobj")) != 0) testREZ = "FAILED: save page";
				else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr1+"/%2fwdg%2fcfg%2fdescr").setText("Test 1")) != 0 || 
					SYS.cntrReq(SYS.XMLNode("load").setAttr("path",nAddr1+"/%2fobj")) != 0 || SYS.cntrReq(req=SYS.XMLNode("get").setAttr("path",nAddr1+"/%2fwdg%2fcfg%2fdescr")) ||
					req.text() != "Test") testREZ = "FAILED: load page";

				nAddr2 = nAddr1+"/wdg_"+nNm;
				if(SYS.cntrReq(SYS.XMLNode("add").setAttr("path",nAddr1+"/%2fbr%2fwdg_").setAttr("id",nNm)) != 0 || !(node=SYS.nodeAt(nAddr2))) testREZ = "FAILED: widget add";
				else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr2+"/%2fwdg%2fcfg%2fdescr").setText("Test")) != 0 || 
					SYS.cntrReq(req=SYS.XMLNode("get").setAttr("path",nAddr2+"/%2fwdg%2fcfg%2fdescr")) || req.text() != "Test") testREZ = "FAILED: change widget";
				else if(SYS.cntrReq(SYS.XMLNode("save").setAttr("path",nAddr2+"/%2fobj")) != 0) testREZ = "FAILED: save widget";
				else if(SYS.cntrReq(SYS.XMLNode("set").setAttr("path",nAddr2+"/%2fwdg%2fcfg%2fdescr").setText("Test 1")) != 0 || 
					SYS.cntrReq(SYS.XMLNode("load").setAttr("path",nAddr2+"/%2fobj")) != 0 || SYS.cntrReq(req=SYS.XMLNode("get").setAttr("path",nAddr2+"/%2fwdg%2fcfg%2fdescr")) ||
					req.text() != "Test") testREZ = "FAILED: load widget";
				node = false;

				if(SYS.cntrReq(SYS.XMLNode("del").setAttr("path","/UI/"+nList[i_n]+"/%2fbr%2fprj_").setAttr("id",nNm)) != 0 || SYS.nodeAt(nAddr)) testREZ = "FAILED: project delete";
				else
				{
					cfgTree.getElementBy("mod_VCAEngine").childAdd("tbl").setAttr("id","PRJ").childAdd("fld").setAttr("ID",nNm+"_cfg").setAttr("DB_TBL","prj_"+nNm+"_cfg");
					cfgTree.getElementBy("mod_VCAEngine").childAdd("tbl").setAttr("id","prj_"+nNm+"_cfg").childAdd("fld").setAttr("OWNER","/"+nNm+"_cfg").setAttr("ID",nNm+"_cfg").setAttr("PARENT","/wlb_originals/wdg_Box");//.setAttr("FLGS",1);
					cfgTree.getElementBy("mod_VCAEngine").childAdd("tbl").setAttr("id","prj_"+nNm+"_cfg_incl").childAdd("fld").setAttr("IDW","/prj_"+nNm+"_cfg/pg_"+nNm+"_cfg").setAttr("ID",nNm+"_cfg").setAttr("PARENT","/wlb_originals/wdg_Box");
					cfgTree.save(0x20, configPath);
					SYS.cntrReq(SYS.XMLNode("scan").setAttr("path","/%2fgen%2fconfig"));		//Reload config command
					if(!SYS.nodeAt(nAddr+"_cfg"))	testREZ = "FAILED: load project from config";
					else if(!SYS.nodeAt(nAddr+"_cfg/pg_"+nNm+"_cfg"))	testREZ = "FAILED: load page from config";
					else if(!SYS.nodeAt(nAddr+"_cfg/pg_"+nNm+"_cfg/wdg_"+nNm+"_cfg"))	testREZ = "FAILED: load widget from config";
				}
			}
			rez += "<tr><td>UI.VCAEngine: Project, pages and widgets create/remove, change, load, save and load from config</td><td>"+testREZ+"</td></tr>\n";

			//> Session
			testREZ = "PASSED";
			nAddr = "/UI/"+nList[i_n]+"/ses_"+nNm;
			req = SYS.XMLNode("CntrReqs").setAttr("path","/UI/"+nList[i_n]);
			req.childAdd("add").setAttr("path","/%2fbr%2fses_").setText(nNm);
			req.childAdd("set").setAttr("path","/ses_"+nNm+"/%2fobj%2fst%2fprj").setText("tmplSO");
			req.childAdd("set").setAttr("path","/ses_"+nNm+"/%2fobj%2fst%2fstart").setText(1);
			if(SYS.cntrReq(req) != 0 || !SYS.nodeAt(nAddr)) testREZ = "FAILED: session add/start";
			else if(SYS.cntrReq(req=SYS.XMLNode("del").setAttr("path","/UI/"+nList[i_n]+"/%2fbr%2fses_").setText(nNm)) || SYS.nodeAt(nAddr)) testREZ = "FAILED: session delete";
			rez += "<tr><td>UI.VCAEngine: Session create, start and remove</td><td>"+testREZ+"</td></tr>\n";
		}

		//> Disconnect/Connect module
		req = SYS.XMLNode("get").setAttr("path","/UI/"+nList[i_n]+"/%2fhelp%2fm_inf%2fSource");
		SYS.cntrReq(req);
		testREZ = SYS.Special.SystemTests.SOAttach(req.text(),0);
		testREZ += "/"+SYS.Special.SystemTests.SOAttach(req.text(),0);
		rez += "<tr><td>UI."+nList[i_n].slice(4)+": Disconnect/Connect</td><td>"+testREZ+"</td></tr>\n";
	}
}

//**********************************
//> Special subsystem tests
if(sub < 0 || sub == 8)
{
	nList = SYS.Special.nodeList("mod_");
	for(i_n = 0; i_n < nList.length; i_n++)
	{
		if(nList[i_n] == "mod_SystemTests") continue;
		//> Disconnect/Connect module
		req = SYS.XMLNode("get").setAttr("path","/Special/"+nList[i_n]+"/%2fhelp%2fm_inf%2fSource");
		SYS.cntrReq(req);
		testREZ = SYS.Special.SystemTests.SOAttach(req.text(),0);
		testREZ += "/"+SYS.Special.SystemTests.SOAttach(req.text(),0);
		rez += "<tr><td>Special."+nList[i_n].slice(4)+": Disconnect/Connect</td><td>"+testREZ+"</td></tr>\n";
	}
}

//> Restore original config file
SYS.fileWrite(configPath,SYS.fileRead(configPath+".orig"));

//Save result to file ReleaseTests.html
SYS.fileWrite("ReleaseTests.html", "<?xml version=''1.0'' ?>\n"
	"<!DOCTYPE html PUBLIC ''-//W3C//DTD XHTML 1.0 Transitional//EN''\n"
	"''DTD/xhtml1-transitional.dtd''>\n"
	"<html xmlns=''http://www.w3.org/1999/xhtml''>\n"
	"<head>\n"
	"  <meta http-equiv=''Content-Type'' content=''text/html; charset=UTF-8''/>\n"
	"</head>\n"
	"<body>\n"
	"<h1>Release tests report table</h1>\n"
	"<TABLE border=\"1\" cellpadding=\"2\" cellspacing=\"0\" width=\"100%\">\n"
	"  <TR align=\"center\"><TH>Test</TH><TH>Result</TH></TR>\n"+
	rez+
	"</TABLE>\n"
	"</body>"
	"</html>");','','','','','','');
CREATE TABLE 'lib_servProc_io' ("F_ID" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"TYPE" INTEGER DEFAULT '' ,"MODE" INTEGER DEFAULT '' ,"DEF" TEXT DEFAULT '' ,"HIDE" INTEGER DEFAULT '' ,"POS" INTEGER DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#DEF" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DEF" TEXT DEFAULT '' , PRIMARY KEY ("F_ID","ID"));
INSERT INTO "lib_servProc_io" VALUES('procArh','fromarch','From archive',0,0,'Archive.va_ai3_code',0,0,'З архіву','Archive.va_ai3_code','Из архива','Archive.va_ai3_code');
INSERT INTO "lib_servProc_io" VALUES('procArh','toarch','To archive',0,0,'Archive.va_ai3_dP',0,1,'У архів','Archive.va_ai3_dP','В архив','Archive.va_ai3_dP');
INSERT INTO "lib_servProc_io" VALUES('procArh','begs','Begin time',0,0,'2006-08-08 9:21:56',0,2,'Час початку','2006-08-08 9:21:56','Время начала','2006-08-08 9:21:56');
INSERT INTO "lib_servProc_io" VALUES('procArh','ends','End time',0,0,'2006-08-08 11:21:55',0,3,'Час закінчення','2006-08-08 11:21:55','Время конца','2006-08-08 11:21:55');
INSERT INTO "lib_servProc_io" VALUES('crc16','in','Input',0,0,'',0,0,'','','','');
INSERT INTO "lib_servProc_io" VALUES('crc16','out','Result',1,1,'',0,1,'','','','');
INSERT INTO "lib_servProc_io" VALUES('releaseTests','sub','Subsystem (-1:All;0:SYS;1:Security;2:Transport;3:A',1,0,'-1',0,0,'','','','');
INSERT INTO "lib_servProc_io" VALUES('releaseTests','rez','Rezult',0,1,'',0,1,'','','','');
CREATE TABLE 'prescr_val' ("ID" TEXT DEFAULT '' ,"VAL" TEXT DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO "prescr_val" VALUES('dbDB','SQLite.vcaBase');
INSERT INTO "prescr_val" VALUES('dbComs','PrescrComs');
INSERT INTO "prescr_val" VALUES('dbProgs','PrescrProgs');
INSERT INTO "prescr_val" VALUES('mode','-2');
INSERT INTO "prescr_val" VALUES('curMode','-2');
INSERT INTO "prescr_val" VALUES('prog','');
INSERT INTO "prescr_val" VALUES('startTm','1372173007');
INSERT INTO "prescr_val" VALUES('curCom','3');
INSERT INTO "prescr_val" VALUES('work','<XMLNodeObj:prg>
</XMLNodeObj:prg>
');
INSERT INTO "prescr_val" VALUES('comsCntr','LogicLev.prescription');
INSERT INTO "prescr_val" VALUES('comLs','<TVarObj>
<TVarObj p=''Таймер''>
<str p=''arg1''>Час (с)</str>
<str p=''descr''>Типовий таймер. Утримує виконання до завершення часу.</str>
<str p=''prmID''>timer</str>
</TVarObj>
<TVarObj p=''Фоновий таймер''>
<str p=''arg1''>Час (с)</str>
<str p=''descr''>Фоновий таймер. Оновлюється паралельно з поточною командою.</str>
<str p=''prmID''>backTimer</str>
</TVarObj>
</TVarObj>
');
INSERT INTO "prescr_val" VALUES('clcCnt','10086974');
CREATE TABLE 'techApp' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"MAXCALCTM" INTEGER DEFAULT '' ,"FORMULA" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO "techApp" VALUES('lag','Lag','Lag model. You can use this for sensors'' variables lag imitation.',10,'out-=(out-in)/(t_lg*f_frq);','Затримка','Модель затримки. Може використовуватися для імітації запізнювання значень давачів.','Запаздывание','Модель задержки. Может использоваться для имитации запаздывания
значений датчиков.
');
INSERT INTO "techApp" VALUES('noise','Noise (2 harmonic + rand)','Noise model. Contain three parts:
- first harmonic part;
- second harmonic part;
- noise based on randomize generator of numbers.',10,'tmp_g1=(tmp_g1>6.28)?0:tmp_g1+6.28/(per_g1*f_frq);
tmp_g2=(tmp_g2>6.28)?0:tmp_g2+6.28/(per_g2*f_frq);
out=off+a_g1*sin(tmp_g1)+a_g2*sin(tmp_g2)+a_rnd*(rand(2)-1);','Шум (2 гарм. + випадк.)','Модель шуму. Містить три складові:
- перша гармоніка;
- друга гармоніка;
- шум на основі генератору випадкових чисел.','Шум (2 гарм. + случ)','Модель шума. Содержит три составляющие:
- первая гармоника;
- вторая гармоника;
- шум на основе генератора случайных чисел.');
INSERT INTO "techApp" VALUES('ballCrane','Ball crane','Ball crane model. Include going and estrangement time.',10,'if( !(st_close && !com) && !(st_open && com) )
{
  tmp_up=(pos>0&&pos<100)?0:(tmp_up>0&&lst_com==com)?tmp_up-1./f_frq:t_up;
  pos+=(tmp_up>0)?0:(100.*(com?1.:-1.))/(t_full*f_frq);
  pos=(pos>100)?100:(pos<0)?0:pos;
  st_open=(pos>=100)?true:false;
  st_close=(pos<=0)?true:false;
  lst_com=com;
}','Шаровий кран','Модель шарового крану. Включає час ходу та час відриву.','Шаровой кран','Модель шарового крана. Включает время хода и время отрыва.');
INSERT INTO "techApp" VALUES('separator','Separator','Separator model included two phase: liquid and gas.',10,'Fж=max(0,Fi*ProcЖ);
DAQ.JavaLikeCalc.lib_techApp.pipeBase(Fi,Pi,293,Si,Fo+Fж,Po,293,So,lo,Q0,0.95,0.01,f_frq);
Lж = max(0,min(100,Lж+0.27*(Fж-Fo_ж)/(Vap*Qж*f_frq)));
Po_ж = Po + Lж*Vap/Qж;','Сепаратор','Модель сепаратору з двома фазами рідинною та газовою.','Сепаратор','Модель сепаратора с двумя фазами, жидкой и газовой.');
INSERT INTO "techApp" VALUES('klap','Valve','Valve model, include:
- two valve in one;
- super-critical speed;
- temperature change on baffling;
- work to one side, back valve;
- valve position speed control;
- nonlinear cut changing by open position.',10,'Qr=Q0+Q0*Kpr*(Pi-1);
tmp_l1 += (abs(l_kl1-tmp_l1) > 5) ? 100*sign(l_kl1-tmp_l1)/(t_kl1*f_frq) : (l_kl1-tmp_l1)/(t_kl1*f_frq);
tmp_l2 += (abs(l_kl2-tmp_l2) > 5) ? 100*sign(l_kl2-tmp_l2)/(t_kl2*f_frq) : (l_kl2-tmp_l2)/(t_kl2*f_frq);
Sr=(S_kl1*pow(tmp_l1,Kln)+S_kl2*pow(tmp_l2,Kln))/pow(100,Kln);

DAQ.JavaLikeCalc.lib_techApp.pipeBase(Fi,Pi,Ti,Sr,EVAL_REAL,Po,293,So,lo,Q0,Kpr,0.01,f_frq);
if( noBack ) Fi = max(0,Fi);
Po = max(0,min(100,Po+0.27*(Fi-Fo)/(Q0*Kpr*So*lo*f_frq)));

To = max(0,min(2e3,To+(abs(Fi)*(Ti*pow(Po/Pi,0.02)-To)+(Fwind+1)*(Twind-To)/Riz)/(Ct*So*lo*Qr*f_frq)));','Клапан','Модель клапану яка враховує:
- два клапана в одному; 
- зверхкритичне витікання; 
- зміна температури при дроселюванні; 
- робота тільки у одному напрямку, зворотний клапан;
- керування швидкісттю зміни положення;
- нелінійність прохідного перетину від положення.','Клапан','Модель клапана, учитывающая:
- два клапана в одном; 
- сверхкритическое истечение; 
- изменение температуры при дросселировании; 
- работа только в одном направлении, обратный клапан; 
- управляемая скорость изменения положения; 
- нелинейность проходного сечения от положения.');
INSERT INTO "techApp" VALUES('lagClean','Lag (clear)','Model of clear lag (transportable). Realised by include some simple lag chains. Appointed for lags into long pipes.',10,'cl1-=(cl1-in)/(t_lg*f_frq/4);
cl2-=(cl2-cl1)/(t_lg*f_frq/4);
cl3-=(cl3-cl2)/(t_lg*f_frq/4);
out-=(out-cl3)/(t_lg*f_frq/4);','Затримка (чиста)','Модель чистої(транспортної) затримки. Реалізується шляхом включення декількох ланцюгів простої затримки. Призначено для імітації затримок у довгих трубопроводах.','Запаздывание (чистое)','Модель чистого(транспортного) запаздывания. Реализуется путём включения нескольких простых цепей запаздывания. Предназначен для имитации запаздывания в длинных трубопроводах.');
INSERT INTO "techApp" VALUES('net','Network (loading)','Loading with constant preasure on network. Contain parameter for noise connection.',10,'DAQ.JavaLikeCalc.lib_techApp.pipeBase(Fi,Pi,293,So,EVAL_REAL,Po,293,So,10,Q0,Kpr,0.01,f_frq);','Мережа (навантаження)','Навантаження з фіксованим тиском мережі. Містить параметр для підключення шуму.','Сеть (нагрузка)','Нагрузка с фиксированным давлением сети. Содержит параметр для подключения шума.');
INSERT INTO "techApp" VALUES('src_press','Source (pressure)','Source pressure with constant pressure. Contained the parameter for noise connection.',10,'DAQ.JavaLikeCalc.lib_techApp.pipeBase(Fit,Pi*Noise,293,So,Fo,Po,293,So,lo,Q0,Kpr,0.01,f_frq);','Джерело (тиск)','Джерело з фіксованим тиском. Містить параметр для підключення шуму.','Источник (давление)','Источник с фиксированным давлением. Содержит параметр для подключения шума.');
INSERT INTO "techApp" VALUES('cooler','Air cooler','Model of the air cooler for gas flow.',10,'DAQ.JavaLikeCalc.lib_techApp.pipeBase(Fi,Pi,293,Si,Fo,Po,293,So,lo,Q0,0.95,0.01,f_frq);
Qr = Q0+Q0*0.95*(Pi-1);
To+=(Fi*(Ti-To)+Wc*(Tair-To)/Rt)/(Ct*(Si*li+So*lo)*Qr*f_frq);','Повітряний холодильник','Модель повітряного охолоджувача газового потоку.','Возд. холодильник','Модель воздушного охладителя газового потока.');
INSERT INTO "techApp" VALUES('compressor','Gas compressor','Model of the gas compressor. Realised surge effect.
Sarge count from the dynamic-gas curve, and next count coefficient of sarge margin.',10,'Pmax = max(Pi,Po);
Pmin = min(Pi,Po);
Qr = Q0+Q0*Kpr*(Pi-1);
Qrf = Q0+Q0*Kpr*(Pmax-1);
Ftmp=(N>0.1)?(1-10*(Po-Pi)/(Qr*(pow(N,3)+0.1)*Kpmp)):1;
Kzp=1-Ftmp;  //Коэффиц. запаса
Fi=V*N*Qr*sign(Ftmp)*pow(abs(Ftmp),Kslp)+
     0.3*(4*So*Qrf/(0.01*lo*1.7724+4*Qrf))*sign(Pi-Po)*pow(Qrf*(Pmax-max(Pmax*0.528,Pmin)),0.5);
Fit -= (Fit-Fi)/max(1,(lo*f_frq)/max(1e-4,abs(Fi/(Qrf*So))));
Po = max(0,min(100,Po+0.27*(Fi-Fo)/(Q0*Kpr*So*lo*f_frq)));

To+=(abs(Fi)*(Ti*pow(Po/Pi,0.3)-To)+(Fwind+1)*(Twind-To)/Riz)/(Ct*(V+So*lo)*Qr*f_frq);','Компресор газовий','Модель газового компресора. Враховує ефект помпажу. 
Помпаж розраховується за газово-динамічною кривою, виходячи з якої розраховується коефіцієнт запасу по помпажу.','Компрессор газовый','Модель газового компрессора. Учитывает эффект помпажа. Помпаж вычисляется по газо-динамической кривой, исходя из которой получается коэффициент запаса по помпажу.');
INSERT INTO "techApp" VALUES('src_flow','Source (flow)','Source of constant flow. Contained parameter for noise connection.',10,'Po = max(0,min(100,Po+0.27*(Noise*Fi-Fo)/(Q0*Kpr*So*lo*f_frq)));','Джерело (витрати)','Джерело з фіксованими витратами. Містить параметр для підключення шуму.','Источник (расход)','Источник с фиксированным расходом. Содержит параметр для подключения шума.');
INSERT INTO "techApp" VALUES('pipe1_1','Pipe 1->1','Model of the pipe by scheme: 1 -> 1.',10,'DAQ.JavaLikeCalc.lib_techApp.pipeBase(Fi,Pi,293,So,Ft1,Pti,293,So,0.33*lo,Q0,Kpr,0.01,f_frq);
DAQ.JavaLikeCalc.lib_techApp.pipeBase(Ft1,Pti,293,So,Fto,Pt1,293,So,0.33*lo,Q0,Kpr,0.01,f_frq);
DAQ.JavaLikeCalc.lib_techApp.pipeBase(Fto,Pt1,293,So,Fo,Po,293,So,0.33*lo,Q0,Kpr,0.01,f_frq);','Труба 1->1','Модель вузла труб за схемою: 1 -> 1.','Труба 1->1','Модель узла труб по схеме: 1 -> 1.');
INSERT INTO "techApp" VALUES('pipe1_2','Pipe 1->2','Model of the pipe by scheme: 1 -> 2.',10,'DAQ.JavaLikeCalc.lib_techApp.pipeBase(F1tmp,Pi,293,So1,Fo1,Po1,293,So1,lo1,Q0,Kpr,0.01,f_frq);
DAQ.JavaLikeCalc.lib_techApp.pipeBase(F2tmp,Pi,293,So2,Fo2,Po2,293,So2,lo2,Q0,Kpr,0.01,f_frq);
Fi=F1tmp+F2tmp;','Труба 1->2','Модель вузла труб за схемою: 1 -> 2.','Труба 1->2','Модель узла труб по схеме: 1 -> 2.');
INSERT INTO "techApp" VALUES('pipe1_3','Pipe 1->3','Model of the pipe by scheme: 1 -> 3.',10,'DAQ.JavaLikeCalc.lib_techApp.pipeBase(F1tmp,Pi,293,So1,Fo1,Po1,293,So1,lo1,Q0,Kpr,0.01,f_frq);
DAQ.JavaLikeCalc.lib_techApp.pipeBase(F2tmp,Pi,293,So2,Fo2,Po2,293,So2,lo2,Q0,Kpr,0.01,f_frq);
DAQ.JavaLikeCalc.lib_techApp.pipeBase(F3tmp,Pi,293,So3,Fo3,Po3,293,So3,lo3,Q0,Kpr,0.01,f_frq);
Fi=F1tmp+F2tmp+F3tmp;','Труба 1->3','Модель вузла труб за схемою: 1 -> 3.','Труба 1->3','Модель узла труб по схеме: 1 -> 3.');
INSERT INTO "techApp" VALUES('pipe1_4','Pipe 1->4','Model of the pipe by scheme: 1 -> 4.',10,'DAQ.JavaLikeCalc.lib_techApp.pipeBase(F1tmp,Pi,293,So1,Fo1,Po1,293,So1,lo1,Q0,Kpr,0.01,f_frq);
DAQ.JavaLikeCalc.lib_techApp.pipeBase(F2tmp,Pi,293,So2,Fo2,Po2,293,So2,lo2,Q0,Kpr,0.01,f_frq);
DAQ.JavaLikeCalc.lib_techApp.pipeBase(F3tmp,Pi,293,So3,Fo3,Po3,293,So3,lo3,Q0,Kpr,0.01,f_frq);
DAQ.JavaLikeCalc.lib_techApp.pipeBase(F4tmp,Pi,293,So4,Fo4,Po4,293,So4,lo4,Q0,Kpr,0.01,f_frq);
Fi=F1tmp+F2tmp+F3tmp+F4tmp;','Труба 1->4','Модель вузла труб за схемою: 1 -> 4.','Труба 1->4','Модель узла труб по схеме: 1 -> 4.');
INSERT INTO "techApp" VALUES('klapMech','Valve proc. mechanism','Model of the valve process mechanism.
Include going time (aperiodic chain of two level) and estrangement time.',10,'if( (pos >= 99 && com >= 99) || (pos <= 1 && com <=1 ) )
{ 
  tmp_up = t_up;
  if(pos>=99) { pos=100; st_open=true; }
  else { pos = 0; st_close=true; }
}
else if( tmp_up > 0 ) tmp_up-=1./f_frq;
else
{
  st_open=st_close=false;
  lst_com+=(com-lst_com)/(0.5*t_full*f_frq);
  pos+=(lst_com-pos)/(0.5*t_full*f_frq);
}
pos_sensor+=(pos-pos_sensor)/(t_sensor*f_frq);','Виконавчий мех. клапану','Модель виконавчого механізму клапана. Включає час ходу та час відриву.','Исполн. мех. клапана','Модель исполнительного механизма клапана. Включает время хода и время отрыва.');
INSERT INTO "techApp" VALUES('diafragma','Diaphragm','Diaphragm model.',10,'DAQ.JavaLikeCalc.lib_techApp.pipeBase(Fi,Pi,293,Sdf,Fo,Po,293,So,lo,Q0,Kpr,0.01,f_frq);
dP -= (dP-100*(Pi-Po))/f_frq;','Діафрагма','Модель діафрагми.','Диафрагма','Модель диафрагмы.');
INSERT INTO "techApp" VALUES('pipe3_1','Pipe 3->1','Model of the pipe by scheme: 3 -> 1.',10,'DAQ.JavaLikeCalc.lib_techApp.pipeBase(Fi1,Pi1,293,Si1,EVAL_REAL,Po,293,So,lo,Q0,Kpr,0.01,f_frq);
DAQ.JavaLikeCalc.lib_techApp.pipeBase(Fi2,Pi2,293,Si2,EVAL_REAL,Po,293,So,lo,Q0,Kpr,0.01,f_frq);
DAQ.JavaLikeCalc.lib_techApp.pipeBase(Fi3,Pi3,293,Si3,EVAL_REAL,Po,293,So,lo,Q0,Kpr,0.01,f_frq);
Po = max(0,min(100,Po+0.27*(Fi1+Fi2+Fi3-Fo)/(Q0*Kpr*So*lo*f_frq)));
To = max(0,To+(Fi1*(Ti1-To)+Fi2*(Ti2-To)+Fi3*(Ti3-To)+(Fwind+1)*(Twind-To)/Riz)/(Ct*So*lo*Q0*f_frq));','Труба 3->1','Модель вузла труб за схемою: 3 -> 1.','Труба 3->1','Модель узла труб по схеме: 3 -> 1.');
INSERT INTO "techApp" VALUES('pipeBase','Pipe-base','Implementation of the basic foundations of the model pipe:
- Flow in the pipe, taking into account the speed, pressure drop, resistance due to friction and the critical flow.
- Calculation of pressure.
- Accounting for medium density and degree of compressibility for both gases and liquids.',10,'Pmax = max(Pi,Po);
Pmin = min(Pi,Po);
Qr = Q0+Q0*Kpr*(Pmax-1);
Fit = 630*(4*Si*So*Qr/(Ktr*lo*1.7724*Si+4*So*Qr))*sign(Pi-Po)*pow(Qr*(Pmax-max(Pmax*0.528,Pmin)),0.5);
Fi -= (Fi-Fit)/max(1,(lo*f_frq)/max(1,abs(Fit/(Qr*So))));
if( !Fo.isEVal() ) Po = max(0,min(100,Po+0.27*(Fi-Fo)/(Q0*Kpr*So*lo*f_frq)));','Труба-база','Реалізація базових основ моделі труби:
- Витрати у трубі з урахуванням швидкості руху, різниці тиску, опору за рахунок тертя та критичної течі.
- Розрахунок тиску. 
- Урахування щільності середовища та ступеня його стиснення як для газів, так і для рідин.','Труба-база','Реализация базовых основ модели трубы:
- Расход в трубе с учётом скорости движения, перепада давления, сопротивления за счёт трения и критического истечения.
- Расчёт давления.
- Учёт плотности среды и степени её сжимаемости как для газов, так и для жидкостей.');
INSERT INTO "techApp" VALUES('heatExch','Heat exchanger','The model of the heat exchanger, it calculates the heat exchange of the two streams.',10,'DAQ.JavaLikeCalc.lib_techApp.pipeBase(Fi1,Pi1,Ti1,Si1,Fo1,Po1,293,So1,lo1,Q0i1,Kpr1,0.01,f_frq);
DAQ.JavaLikeCalc.lib_techApp.pipeBase(Fi2,Pi2,Ti2,Si2,Fo2,Po2,293,So2,lo2,Q0i2,Kpr2,0.01,f_frq);

To1=max(0,min(1e4,(Fi1*Ti1*Ci1+ki*Fi2*Ti2*Ci2)/(Fi1*Ci1+ki*Fi2*Ci2)));
To2=max(0,min(1e4,(ki*Fi1*Ti1*Ci1+Fi2*Ti2*Ci2)/(ki*Fi1*Ci1+Fi2*Ci2)));','Теплообмінник','Модель теплообміннику, яка розраховує теплообмін двох потоків.','Теплообменник','Модель теплообменника, рассчитывающая теплообмен двух потоков.');
INSERT INTO "techApp" VALUES('boilerBarrel','Boiler: barrel','The model of the boiler''s barrel. ',10,'// Water
DAQ.JavaLikeCalc.lib_techApp.pipeBase(Fi1,Pi1,293,Si1,EVAL_REAL,Po1,293,So1,lo1,1e3,0.001,0.01,f_frq);
Fi1 = max(0,Fi1);

// Steam
Lo = max(0,min(100,Lo+(Fi1-Fpara)*100/(Vi1*1000*f_frq)));
To1 = (100*pow(Po1,0.241)+5)+273;

if( Tv<To1 )
{
  Tv+=(k*S*(Ti2-Tv)-Fi1*0.00418*(Tv-Ti1))/f_frq;
  Fpara=0;
}
if( Tv >= To1 )
{
  Tv=To1;
  Lambda=2750.0-0.00418*(Tv-273);
  Fpara=(5*S*Fi2*(Ti2-Tv)-Fi1*0.00418*(Tv-Ti1))/(Po1*Lambda);
}

To2=Ti2-Tv/k;
Po1 = max(0,min(100,Po1+0.27*(Fpara-Fo)/(1.2*0.98*((1-Lo/100)*Vi1+So1*lo1)*f_frq)));

// Smoke gas
DAQ.JavaLikeCalc.lib_techApp.pipeBase(Fi2,Pi2,293,Si2,Fo2,Po2,293,Si2,30,1.2,0.98,0.01,f_frq);','Котел: барабан','Модель барабану котлоагрегату.','Котёл: барабан','Модель барабана котлоагрегата.');
INSERT INTO "techApp" VALUES('boilerBurner','Boiler: burner','The fire chamber''s of the boiler model which works with three fuels: blast-furnace gas, coke and natural gas. ',10,'using DAQ.JavaLikeCalc.lib_techApp;
pipeBase(Fi1,Pi1,Ti1,Si1,EVAL_REAL,Po,293,So,lo,1.2,0.95,0.01,f_frq); Fi1 = max(0,Fi1);
pipeBase(Fi2,Pi2,Ti2,Si2,EVAL_REAL,Po,293,So,lo,0.7,0.95,0.01,f_frq); Fi2 = max(0,Fi2);
pipeBase(Fi3,Pi3,Ti3,Si3,EVAL_REAL,Po,293,So,lo,1.33,0.95,0.01,f_frq); Fi3 = max(0,Fi3);
pipeBase(Fi4,Pi4,Ti4,Si4,EVAL_REAL,Po,293,So,lo,1.293,0.95,0.01,f_frq); Fi4 = max(0,Fi4);

Neobhod_vzd = Fi1+10*Fi2+4*Fi3;
F_DG = Fi1+Fi2+Fi3+Fi4;
O2 = max(0,min(100,(Fi4-Neobhod_vzd)*100/F_DG));
CO = min(100, (O2<1) ? (1.2*abs(O2)) : 0);
koef = min(1,Fi4/Neobhod_vzd);
Q = koef*(8050*Fi2+3900*Fi3+930*Fi1);
delta_t = Q/(F_DG*1.047);
To = max(0,min(2000,(delta_t+(Ti4-273)+(Ti3-273)*(Fi3/Fi1)+(Ti2-273)*(Fi2/Fi1)+(Ti1-273)*(Fi1/Fi4))+273));

Po = max(0,min(10,Po+0.27*(F_DG-Fo)/(1.2*0.95*(So*lo+V)*f_frq)));','Котел: топка','Модель топки котлоагрегату, який працює на трьох видах палива: доменному, коксовому та природному газах.','Котёл: топка','Модель топки котлоагрегата, работающего на трех видах топлива: доменном, коксовом и природном газе.');
INSERT INTO "techApp" VALUES('pipe2_1','Pipe 2->1','Model of the pipe by scheme: 2 -> 1.',10,'DAQ.JavaLikeCalc.lib_techApp.pipeBase(Fi1,Pi1,293,Si1,EVAL_REAL,Po,293,So,lo,Q0,Kpr,0.01,f_frq);
DAQ.JavaLikeCalc.lib_techApp.pipeBase(Fi2,Pi2,293,Si2,EVAL_REAL,Po,293,So,lo,Q0,Kpr,0.01,f_frq);
Po = max(0,min(100,Po+0.27*(Fi1+Fi2-Fo)/(Q0*Kpr*So*lo*f_frq)));
To = max(0,To+(Fi1*(Ti1-To)+Fi2*(Ti2-To)+(Fwind+1)*(Twind-To)/Riz)/(Ct*So*lo*Q0*f_frq));','Труба 2->1','Модель вузла труб за схемою: 2 -> 1.','Труба 2->1','Модель узла труб по схеме: 2 -> 1.');
CREATE TABLE 'techApp_io' ("F_ID" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"TYPE" INTEGER DEFAULT '' ,"MODE" INTEGER DEFAULT '' ,"DEF" TEXT DEFAULT '' ,"HIDE" INTEGER DEFAULT '' ,"POS" INTEGER DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DEF" TEXT DEFAULT '' ,"uk#DEF" TEXT DEFAULT '' , PRIMARY KEY ("F_ID","ID"));
INSERT INTO "techApp_io" VALUES('lag','out','Output',2,2,'0',0,0,'Вихід','Выход','','');
INSERT INTO "techApp_io" VALUES('lag','in','Input',2,0,'0',0,1,'Вхід','Вход','','');
INSERT INTO "techApp_io" VALUES('lag','t_lg','Lag time (s)',2,0,'10',0,2,'Час запізнення (с)','Время запазд. (с)','','');
INSERT INTO "techApp_io" VALUES('lag','f_frq','Calc frequency (Hz)',2,0,'100',1,3,'Частота обчислення (Гц)','Частота обсчёта (Гц)','','');
INSERT INTO "techApp_io" VALUES('noise','out','Output',2,2,'0',0,0,'Вихід','Выход','','');
INSERT INTO "techApp_io" VALUES('noise','off','Main offset',2,0,'1',0,1,'Загальний зсув','Общее смещение','','');
INSERT INTO "techApp_io" VALUES('noise','a_g1','Harmonic part 1 amplitude',2,0,'10',0,2,'Амплітуда гармоніки 1','Амплитуда гармоники 1','','');
INSERT INTO "techApp_io" VALUES('noise','per_g1','Harmonic part 1 period (s)',2,0,'10',0,3,'Період гармоніки 1 (сек)','Период гармоники 1 (сек)','','');
INSERT INTO "techApp_io" VALUES('noise','a_g2','Harmonic part 2 amplitude',2,0,'5',0,4,'Амплітуда гармоніки 2','Амплитуда гармоники 2','','');
INSERT INTO "techApp_io" VALUES('noise','per_g2','Harmonic part 2 period (s)',2,0,'0.1',0,5,'Період гармоніки 2 (сек)','Период гармоники 2 (сек)','','');
INSERT INTO "techApp_io" VALUES('noise','a_rnd','Random numbers amplitude',2,0,'1',0,6,'Амплітуда випадкових значень','Амплитуда случ. значений','','');
INSERT INTO "techApp_io" VALUES('noise','f_frq','Calc function period (Hz)',2,0,'100',1,7,'Частота обчислення функції (Гц)','Частота обсчёта функции (Гц)','','');
INSERT INTO "techApp_io" VALUES('noise','tmp_g1','Harmonic part 1 counter',2,0,'0',1,8,'Лічильник гармоніки 1','Счётчик гармоники 1','','');
INSERT INTO "techApp_io" VALUES('noise','tmp_g2','Harmonic part 2 counter',2,0,'0',1,9,'Лічильник гармоніки 2','Счётчик гармоники 2','','');
INSERT INTO "techApp_io" VALUES('ballCrane','pos','Position (%)',2,1,'0',0,0,'Положення (%)','Положение (%)','','');
INSERT INTO "techApp_io" VALUES('ballCrane','com','Command',3,0,'0',0,1,'Команда','Команда','','');
INSERT INTO "techApp_io" VALUES('ballCrane','st_open','State "Open"',3,1,'0',0,2,'Стан "Відкрито"','Сост. "Открыто"','','');
INSERT INTO "techApp_io" VALUES('ballCrane','st_close','State "Close"',3,1,'1',0,3,'Стан "Закрито"','Сост. "Закрыто"','','');
INSERT INTO "techApp_io" VALUES('ballCrane','t_full','Going time (s)',2,0,'5',0,4,'Час ходу (с)','Время хода (с)','','');
INSERT INTO "techApp_io" VALUES('ballCrane','t_up','Estrangement time (s)',2,0,'0.5',0,5,'Час зриву (с)','Время срыва (с)','','');
INSERT INTO "techApp_io" VALUES('ballCrane','f_frq','Calc frequency (Hz)',2,0,'100',1,6,'Частота обчислення функції (Гц)','Частота обсчёта (Гц)','','');
INSERT INTO "techApp_io" VALUES('ballCrane','tmp_up','Estrangement counter',2,0,'0',1,7,'Лічильник зриву','Счётчик срыва','','');
INSERT INTO "techApp_io" VALUES('ballCrane','lst_com','Last command',3,0,'0',1,8,'Остання команда','Последняя команда','','');
INSERT INTO "techApp_io" VALUES('separator','Fi','Input flow (tones/h)',2,1,'0',0,0,'Вхідн. витрати (т/год)','Входн. расход (т/ч)','','');
INSERT INTO "techApp_io" VALUES('separator','Pi','Input pressure (ata)',2,0,'1',0,1,'Вхідн. тиск (ата)','Входн. давление (ата)','','');
INSERT INTO "techApp_io" VALUES('separator','Si','Input cutset (m2)',2,0,'0.2',0,2,'Вхідн. перетин (м2)','Входн. сечение (м2)','','');
INSERT INTO "techApp_io" VALUES('separator','Fo','Output flow (tones/h)',2,0,'0',0,3,'Вихідн. витрати (т/год)','Выходн. расход (т/ч)','','');
INSERT INTO "techApp_io" VALUES('separator','Po','Output pressure (ata)',2,1,'1',0,4,'Вихідн. тиск (ата)','Выходн. давление (ата)','','');
INSERT INTO "techApp_io" VALUES('separator','So','Output cutset (m2)',2,0,'0.2',0,5,'Вихідн. перетин (м2)','Выходн. сеч. (м2)','','');
INSERT INTO "techApp_io" VALUES('separator','lo','Output length (m)',2,0,'10',0,6,'Вихідн. довжина (м)','Выходн. длина (м)','','');
INSERT INTO "techApp_io" VALUES('separator','Fo_ж','Output liquid flow (tones/h)',2,0,'0',0,7,'Вихідн. витрати рідини (т/год)','Выходн. расход жидк. (т/ч)','','');
INSERT INTO "techApp_io" VALUES('separator','Po_ж','Output liquid pressure (ata)',2,1,'1',0,8,'Вихідн. тиск рідини (ата)','Выходн. давление жидк. (ата)','','');
INSERT INTO "techApp_io" VALUES('separator','Lж','Liquid level (%)',2,1,'0',0,9,'Рівень рідини (%)','Уровень жидкости (%)','','');
INSERT INTO "techApp_io" VALUES('separator','ProcЖ','% liquid.',2,0,'0.01',0,10,'% рідини.','% жидкости.','','');
INSERT INTO "techApp_io" VALUES('separator','Vap','Device capacity (m3)',2,0,'10',0,11,'Об''єм апарату (м3)','Объём аппарата (м3)','','');
INSERT INTO "techApp_io" VALUES('separator','Q0','Norm density of environs (kg/m3)',2,0,'1',0,12,'Норм. щільність середовища (кг/м3)','Норм. плотн. среды (кг/м3)','','');
INSERT INTO "techApp_io" VALUES('separator','Qж','Liquid density (kg/m3)',2,0,'1000',0,13,'Щильність рідини (кг/м3)','Плотн. жидкости (кг/м3)','','');
INSERT INTO "techApp_io" VALUES('separator','f_frq','Calc frequency (Hz)',2,0,'200',1,14,'Частота обчислення функції (Гц)','Частота обсчёта (Гц)','','');
INSERT INTO "techApp_io" VALUES('klap','t_kl1','Valve 1 open time (s)',2,0,'10',0,10,'Час відкриття клапану 1 (с)','Valve 1 open time (s)','','');
INSERT INTO "techApp_io" VALUES('klap','t_kl2','Valve 2 open time (s)',2,0,'5',0,13,'Час відкриття клапану 2 (с)','Valve 2 open time (s)','','');
INSERT INTO "techApp_io" VALUES('klap','tmp_l1','Position 1 lag',2,1,'0',1,23,'Затримка положення 1','Удержанное положение 1','','');
INSERT INTO "techApp_io" VALUES('klap','tmp_l2','Position 2 lag',2,1,'0',1,24,'Затримка положення 2','Удержанное положение 2','','');
INSERT INTO "techApp_io" VALUES('klap','Fi','Input flow (tones/h)',2,1,'0',0,0,'Вхідн. витрати (т/год)','Входн. расход (т/ч)','','');
INSERT INTO "techApp_io" VALUES('klap','Pi','Input pressure (ata)',2,0,'1',0,1,'Вхідн. тиск (ата)','Входн. давление (ата)','','');
INSERT INTO "techApp_io" VALUES('klap','Ti','Input temperature (K)',2,0,'273',0,2,'Вхідн. температура (К)','Входн. температура (К)','','');
INSERT INTO "techApp_io" VALUES('klap','Fo','Output flow (tones/h)',2,0,'0',0,3,'Вихідн. витрати (т/год)','Выходн. расход (т/ч)','','');
INSERT INTO "techApp_io" VALUES('klap','Po','Output pressure (ata)',2,1,'1',0,4,'Вихідн. тиск (ата)','Выходн. давление (ата)','','');
INSERT INTO "techApp_io" VALUES('klap','To','Output temperature (K)',2,1,'273',0,5,'Вихідн. температура (К)','Выходн. температура (К)','','');
INSERT INTO "techApp_io" VALUES('klap','So','Output pipe cutset (m2)',2,0,'.2',0,6,'Вихідн. перетин труби (м2)','Выходн. сеч. трубы (м2)','','');
INSERT INTO "techApp_io" VALUES('klap','lo','Output pipe length (m)',2,0,'10',0,7,'Вихідн. довжина труби (м)','Длина выходн. трубы (м)','','');
INSERT INTO "techApp_io" VALUES('klap','S_kl1','Valve 1 cutset (m2)',2,0,'.1',0,8,'Перетин клапану 1 (м2)','Сечение клапана 1 (м2)','','');
INSERT INTO "techApp_io" VALUES('klap','S_kl2','Valve 2 cutset (m2)',2,0,'.05',0,11,'Перетин клапану 2 (м2)','Сечение клапана 2 (м2)','','');
INSERT INTO "techApp_io" VALUES('klap','l_kl1','Valve 1 open (%)',2,0,'0',0,9,'Полож. клапану 1 (%)','Полож. клапана 1 (%)','','');
INSERT INTO "techApp_io" VALUES('klap','l_kl2','Valve 2 open (%)',2,0,'0',0,12,'Полож. клапану 2 (%)','Полож. клапана 2 (%)','','');
INSERT INTO "techApp_io" VALUES('klap','Q0','Norm density of environs (kg/m3)',2,0,'1',0,14,'Норм. щільність середовища (кг/м3)','Норм. плотн. ср. (кг/м3)','','');
INSERT INTO "techApp_io" VALUES('klap','Kln','Linearity coefficient',2,0,'1',0,15,'Коефіц. нелінійності','Коэфиц. нелинейности','','');
INSERT INTO "techApp_io" VALUES('klap','Kpr','Compressibility coefficient (0...1)',2,0,'0.95',0,16,'Коефіц. стискання середовища','Коэфиц. сжим. среды','','');
INSERT INTO "techApp_io" VALUES('klap','Ct','Warm capacity of environs',2,0,'20',0,17,'Теплоемність середовища','Теплоёмкость среды','','');
INSERT INTO "techApp_io" VALUES('klap','Riz','Warm resistance of isolation',2,0,'20',0,18,'Тепл. опір ізоляції','Тепл. сопр. изоляции','','');
INSERT INTO "techApp_io" VALUES('klap','noBack','Back valve',3,0,'0',0,19,'Зворотній клапан','Обратный клапан','','');
INSERT INTO "techApp_io" VALUES('klap','Fwind','Air speed',2,0,'1',0,20,'Швидкість повітря','Скорость воздуха','','');
INSERT INTO "techApp_io" VALUES('klap','Twind','Air temperature',2,0,'273',0,21,'Темпер. повітря','Темпер. воздуха','','');
INSERT INTO "techApp_io" VALUES('klap','f_frq','Calc frequency (Hz)',2,0,'200',1,22,'Частота обчислення функції (Гц)','Частота обсчёта (Гц)','','');
INSERT INTO "techApp_io" VALUES('src_press','Kpr','Compressibility coefficient (0...1)',2,0,'0.95',0,7,'Коефіцієнт стиснення (0...1)','Коэфиц. сжим. среды','','');
INSERT INTO "techApp_io" VALUES('lagClean','out','Output',2,2,'0',0,0,'Вихід','Выход','','');
INSERT INTO "techApp_io" VALUES('lagClean','in','Input',2,0,'0',0,1,'Вхід','Вход','','');
INSERT INTO "techApp_io" VALUES('lagClean','t_lg','Lag time (s)',2,0,'10',0,2,'Час затримки (с)','Время запазд. (с)','','');
INSERT INTO "techApp_io" VALUES('lagClean','f_frq','Calc frequency (Hz)',2,0,'100',1,3,'Частота обрахунку (Гц)','Частота обсчёта (Гц)','','');
INSERT INTO "techApp_io" VALUES('lagClean','cl1','Chain 1',2,0,'0',1,4,'Звено 1','Звено 1','','');
INSERT INTO "techApp_io" VALUES('lagClean','cl2','Chain 2',2,0,'0',1,5,'Звено 2','Звено 2','','');
INSERT INTO "techApp_io" VALUES('lagClean','cl3','Chain 3',2,0,'0',1,6,'Звено 3','Звено 3','','');
INSERT INTO "techApp_io" VALUES('net','Fi','Input flow (tones/h)',2,1,'10',0,0,'Вхідн. витрати (т/год)','Входн. расхода (т/ч)','','');
INSERT INTO "techApp_io" VALUES('net','Pi','Input pressure (ata)',2,0,'1',0,1,'Вхідн. тиск (ата)','Входн. давление (ата)','','');
INSERT INTO "techApp_io" VALUES('net','Po','Output pressure setpoint (ata)',2,0,'1',0,2,'Завдання вихідного тиску (ата)','Задание выходн. давления (ата)','','');
INSERT INTO "techApp_io" VALUES('net','So','Output pipe cutset (m2)',2,0,'0.1',0,3,'Вихідн. перетин труби (м2)','Сечение трубы на выходе (м2)','','');
INSERT INTO "techApp_io" VALUES('net','Kpr','Compressibility coefficient (0...1)',2,0,'0.95',0,4,'Коефіцієнт стискання (0...1)','Коэффициент сжимаемости (0...1)','','');
INSERT INTO "techApp_io" VALUES('net','Noise','Input flow''s noise',2,0,'1',0,5,'Шум вхідн. витрат','Шум входн. расхода','','');
INSERT INTO "techApp_io" VALUES('net','Q0','Norm density of environs (kg/m3)',2,0,'1',0,6,'Норм. щільність середовища (кг/м3)','Норм. плотн. среды (кг/м3)','','');
INSERT INTO "techApp_io" VALUES('net','f_frq','Calc frequency (Hz)',2,0,'200',1,7,'Частота обчислення функції (Гц)','Частота обсчёта функции (Гц)','','');
INSERT INTO "techApp_io" VALUES('src_press','Pi','Input pressure setpoint (at)',2,0,'10',0,0,'Завдання вхідного тиску (ата)','Задание входного давления (ата)','','');
INSERT INTO "techApp_io" VALUES('src_press','Fo','Output flow (tones/h)',2,0,'0',0,1,'Вихідн. витрати (т/год)','Выходн. расхода (т/ч)','','');
INSERT INTO "techApp_io" VALUES('src_press','Po','Output pressure (at)',2,1,'1',0,2,'Вихідн. тиск (ата)','Выходн. давление (ата)','','');
INSERT INTO "techApp_io" VALUES('src_press','So','Output pipe cutset (m2)',2,0,'0.1',0,3,'Вихідн. перетин труби (м2)','Сечение трубы на выходе (м2)','','');
INSERT INTO "techApp_io" VALUES('src_press','lo','Output pipe length (m)',2,0,'100',0,4,'Вихідн. довжина труби (м)','Длина трубы на выходе (м)','','');
INSERT INTO "techApp_io" VALUES('src_press','Noise','Input flow''s noise',2,0,'1',0,5,'Шум вхідн. витрат','Шум входн. расхода','','');
INSERT INTO "techApp_io" VALUES('src_press','Q0','Norm density of environs (kg/m3)',2,0,'1',0,6,'Норм. щільність середовища (кг/м3)','Плотность среды в норм. услов. (кг/м3)','','');
INSERT INTO "techApp_io" VALUES('src_press','f_frq','Calc frequency (Hz)',2,0,'200',1,8,'Частота обчислення функції (Гц)','Частота обсчёта функции (Гц)','','');
INSERT INTO "techApp_io" VALUES('src_press','Fit','Input flow laged',2,1,'0',1,9,'Вхід. витрати утримані','Вход. расход удержаный','','');
INSERT INTO "techApp_io" VALUES('cooler','Fi','Input flow (tones/h)',2,1,'0',0,0,'Вхідн. витрати (т/год)','Входн. расход (т/ч)','','');
INSERT INTO "techApp_io" VALUES('cooler','Pi','Input pressure (at)',2,0,'1',0,1,'Вхідн. тиск (ата)','Входн. давление (ата)','','');
INSERT INTO "techApp_io" VALUES('cooler','Ti','Input temperature (K)',2,0,'273',0,2,'Вхідн. температура (К)','Входн. температура (К)','','');
INSERT INTO "techApp_io" VALUES('cooler','Si','Cooler''s pipes cutset (m2)',2,0,'0.05',0,3,'Перетин трубок (м2)','Сечение трубок (м2)','','');
INSERT INTO "techApp_io" VALUES('cooler','li','Full cooler''s pipes length (m)',2,0,'10',0,4,'Загальна довжина трубок (м)','Общая длина трубок (м)','','');
INSERT INTO "techApp_io" VALUES('cooler','Fo','Output flow (tones/h)',2,0,'0',0,5,'Вихідн. витрати (т/год)','Выходн. расход (т/ч)','','');
INSERT INTO "techApp_io" VALUES('cooler','Po','Output pressure (at)',2,1,'1',0,6,'Вихідн. тиск (ата)','Выходн. давление (ата)','','');
INSERT INTO "techApp_io" VALUES('cooler','To','Output temperature (K)',2,1,'273',0,7,'Вихідн. температура (К)','Выходн. температура (К)','','');
INSERT INTO "techApp_io" VALUES('cooler','So','Output pipe cutset (m2)',2,0,'.2',0,8,'Вихідн. перетин труби (м2)','Выходн. сеч. трубы (м2)','','');
INSERT INTO "techApp_io" VALUES('cooler','lo','Output pipe length (m)',2,0,'10',0,9,'Вихідн. довжина труби (м)','Длина выходн. трубы (м)','','');
INSERT INTO "techApp_io" VALUES('cooler','Tair','Cooling air temperature (К)',2,0,'283',0,10,'Темп. охолодж. повітря (К)','Темп. охложд. воздуха (К)','','');
INSERT INTO "techApp_io" VALUES('cooler','Wc','Cooler performance',2,0,'200',0,11,'Продуктивність холод.','Производит. холод.','','');
INSERT INTO "techApp_io" VALUES('cooler','Q0','Norm density of environs (kg/m3)',2,0,'1',0,12,'Норм. щільність середовища (кг/м3)','Норм. плотн. ср. (кг/м3)','','');
INSERT INTO "techApp_io" VALUES('cooler','Ct','Warm capacity of environs',2,0,'100',0,13,'Теплоємність середовища','Теплоёмкость среды','','');
INSERT INTO "techApp_io" VALUES('cooler','Rt','Warm resistance of isolation',2,0,'1',0,14,'Тепл. опір','Тепл. сопротивление','','');
INSERT INTO "techApp_io" VALUES('cooler','f_frq','Calc frequency (Hz)',2,0,'200',1,15,'Частота обчислення функції (Гц)','Частота обсчёта (Гц)','','');
INSERT INTO "techApp_io" VALUES('src_flow','Kpr','Compressibility coefficient (0...1)',2,0,'0.95',0,7,'Коефіцієнт стиснення (0...1)','Коеф. сжимаемости среды','','');
INSERT INTO "techApp_io" VALUES('pipe1_1','Kpr','Compressibility coefficient (0...1)',2,0,'0.95',0,7,'Коефіцієнт стиснення (0...1)','Коеф. сжимаемости среды','','');
INSERT INTO "techApp_io" VALUES('compressor','Fi','Input flow (tones/h)',2,1,'0',0,0,'Вхідн. витрати (т/год)','Входн. расход (т/ч)','','');
INSERT INTO "techApp_io" VALUES('compressor','Pi','Input pressure (at)',2,0,'1',0,1,'Вхідн. тиск (ата)','Входн. давление (ата)','','');
INSERT INTO "techApp_io" VALUES('compressor','Ti','Input temperature (K)',2,0,'273',0,2,'Вхідн. температура (К)','Входн. температура (К)','','');
INSERT INTO "techApp_io" VALUES('compressor','Fo','Output flow (tones/h)',2,0,'0',0,3,'Вихідн. витрати (т/год)','Выходн. расход (т/ч)','','');
INSERT INTO "techApp_io" VALUES('compressor','Po','Output pressure (at)',2,1,'1',0,4,'Вихідн. тиск (ата)','Выходн. давление (ата)','','');
INSERT INTO "techApp_io" VALUES('compressor','To','Output temperature (K)',2,1,'273',0,5,'Вихідн. температура (К)','Выходн. температура (К)','','');
INSERT INTO "techApp_io" VALUES('compressor','So','Output pipe cutset (m2)',2,0,'0.2',0,6,'Вихідн. перетин труби (м2)','Выходн. сеч. трубы (м2)','','');
INSERT INTO "techApp_io" VALUES('compressor','lo','Output pipe length (m)',2,0,'2',0,7,'Вихідн. довжина труби (м)','Длина выходн. трубы (м)','','');
INSERT INTO "techApp_io" VALUES('compressor','Kzp','Surge protect margin coefficient',2,1,'0.1',0,8,'Коеф. запасу по помпажу','Коэф. запаса по помпажу','','');
INSERT INTO "techApp_io" VALUES('compressor','N','Turnovers (1000 x turn/min)',2,0,'0',0,9,'Об. компр. (тис. об./хвил)','Об. компр. (тыс. об./мин)','','');
INSERT INTO "techApp_io" VALUES('compressor','V','Capacity (m3)',2,0,'7',0,10,'Об''єм компресору (м3)','Объём компрессора (м3)','','');
INSERT INTO "techApp_io" VALUES('compressor','Kpmp','Surge coefficient (surge point)',2,0,'0.066',0,11,'Коеф. помп. (точка помп.)','Коэф. помп. (точка помп.)','','');
INSERT INTO "techApp_io" VALUES('compressor','Kslp','Slope coefficient of surge curve',2,0,'0.08',0,12,'Коеф. нахилу помп. кривої','Коэф. накл. помп. кривой','','');
INSERT INTO "techApp_io" VALUES('compressor','Q0','Norm density of environs (kg/m3)',2,0,'1',0,13,'Норм. щільність середовища (кг/м3)','Норм. плотн. ср. (кг/м3)','','');
INSERT INTO "techApp_io" VALUES('compressor','Ct','Warm capacity of environs',2,0,'100',0,15,'Теплоємність середовища','Теплоёмкость среды','','');
INSERT INTO "techApp_io" VALUES('compressor','Riz','Warm resistance of isolation',2,0,'100',0,16,'Тепл. опір ізоляції','Тепл. сопр. изоляции','','');
INSERT INTO "techApp_io" VALUES('compressor','Fwind','Air speed',2,0,'1',0,17,'Швидкість повітря','Скорость воздуха','','');
INSERT INTO "techApp_io" VALUES('compressor','Twind','Air temperature',2,0,'273',0,18,'Темпер. повітря','Темпер. воздуха','','');
INSERT INTO "techApp_io" VALUES('compressor','f_frq','Calc frequency (Hz)',2,0,'200',1,19,'Частота обчислення функції (Гц)','Частота обсчёта (Гц)','','');
INSERT INTO "techApp_io" VALUES('pipe1_2','Kpr','Compressibility coefficient (0...1)',2,0,'0.95',0,11,'Коефіцієнт стиснення (0...1)','Коеф. сжимаемости среды','','');
INSERT INTO "techApp_io" VALUES('compressor','Fit','Input flow laged',2,1,'0',1,20,'Вхід. витрати утримані','Вход. расход удержаный','','');
INSERT INTO "techApp_io" VALUES('src_flow','Fi','Input flow setpoint (tones/h)',2,0,'10',0,0,'Завдання вхідних витрат (т/год)','Задание входн. расхода (т/ч)','','');
INSERT INTO "techApp_io" VALUES('src_flow','Fo','Output flow (tones/h)',2,0,'10',0,1,'Вихідні витрати (т/год)','Выходн. расхода (т/ч)','','');
INSERT INTO "techApp_io" VALUES('src_flow','Po','Output pressure (at)',2,1,'1',0,2,'Вихідний тиск (ата)','Выходн. давление (ата)','','');
INSERT INTO "techApp_io" VALUES('src_flow','So','Output pipe cutset (m2)',2,0,'0.1',0,3,'Вихідний перетин труби (м2)','Сечение трубы на выходе (м2)','','');
INSERT INTO "techApp_io" VALUES('src_flow','lo','Output pipe length (m)',2,0,'100',0,4,'Вихідна довжина труби (м)','Длина трубы на выходе (м)','','');
INSERT INTO "techApp_io" VALUES('src_flow','Noise','Input flow''s noise',2,0,'1',0,5,'Шум вхідних витрат','Шум входн. расхода','','');
INSERT INTO "techApp_io" VALUES('src_flow','Q0','Norm density of environs (kg/m3)',2,0,'1',0,6,'Норм. щільність середовища (кг/м3)','Плотность среды в норм. услов. (кг/м3)','','');
INSERT INTO "techApp_io" VALUES('src_flow','f_frq','Calc frequency (Hz)',2,0,'100',1,8,'Частота обчислення функції (Гц)','Частота обсчёта функции (Гц)','','');
INSERT INTO "techApp_io" VALUES('pipe1_1','Fi','Input flow (tones/h)',2,1,'0',0,0,'Вхідні витрати (т/год)','Входн. расход (т/ч)','','');
INSERT INTO "techApp_io" VALUES('pipe1_1','Pi','Input pressure (at)',2,0,'1',0,1,'Вхідний тиск (ата)','Входн. давление (ата)','','');
INSERT INTO "techApp_io" VALUES('pipe1_1','Fo','Output flow (tones/h)',2,0,'0',0,2,'Вихідні витрати (т/год)','Выходн. расход (т/ч)','','');
INSERT INTO "techApp_io" VALUES('pipe1_1','Po','Output pressure (at)',2,1,'1',0,3,'Вихідний тиск (ата)','Выходн. давление (ата)','','');
INSERT INTO "techApp_io" VALUES('pipe1_1','So','Output cutset (m2)',2,0,'.2',0,4,'Вихідний перетин труби (м2)','Выходн. сеч. (м2)','','');
INSERT INTO "techApp_io" VALUES('pipe1_1','lo','Output length (m)',2,0,'10',0,5,'Вихідна довжина труби (м)','Выходн. длина (м)','','');
INSERT INTO "techApp_io" VALUES('pipe1_1','Q0','Norm density of environs (kg/m3)',2,0,'1',0,6,'Норм. щільність середовища (кг/м3)','Норм. плотн. среды (кг/м3)','','');
INSERT INTO "techApp_io" VALUES('pipe1_1','f_frq','Calc frequency (Hz)',2,0,'200',1,8,'Частота обчислення функції (Гц)','Частота обсчёта функции (Гц)','','');
INSERT INTO "techApp_io" VALUES('pipe1_1','Pti','Pti',2,1,'1',1,9,'Pti','Pti','','');
INSERT INTO "techApp_io" VALUES('pipe1_1','Fto','Fto',2,1,'0',1,10,'Fto','Fto','','');
INSERT INTO "techApp_io" VALUES('pipe1_1','Pt1','Pt1',2,1,'1',1,11,'Pt1','Pt1','','');
INSERT INTO "techApp_io" VALUES('pipe1_1','Ft1','Ft1',2,1,'0',1,12,'Ft1','Ft1','','');
INSERT INTO "techApp_io" VALUES('pipe1_2','Fi','Input flow (tones/h)',2,1,'0',0,0,'Вхідні витрати (т/год)','Входн. расход (т/ч)','','');
INSERT INTO "techApp_io" VALUES('pipe1_2','Pi','Input pressure (at)',2,0,'1',0,1,'Вхідний тиск (ата)','Входн. давление (ата)','','');
INSERT INTO "techApp_io" VALUES('pipe1_2','Fo1','Output 1 flow (tones/h)',2,0,'0',0,2,'Вихідні витрати 1 (т/год)','Выходн. расход 1 (т/ч)','','');
INSERT INTO "techApp_io" VALUES('pipe1_2','Po1','Output 1 pressure (at)',2,1,'1',0,3,'Вихідний тиск 1 (ата)','Выходн. давление 1 (ата)','','');
INSERT INTO "techApp_io" VALUES('pipe1_2','So1','Output 1 cutset (m2)',2,0,'.2',0,4,'Вихідний перетин труби 1 (м2)','Выходн. сеч. 1 (м2)','','');
INSERT INTO "techApp_io" VALUES('pipe1_2','lo1','Output 1 length (m)',2,0,'10',0,5,'Вихідна довжина труби 1 (м)','Выходн. длина 1 (м)','','');
INSERT INTO "techApp_io" VALUES('pipe1_2','Fo2','Output 2 flow (tones/h)',2,0,'0',0,6,'Вихідні витрати 2 (т/год)','Выходн. расход 2 (т/ч)','','');
INSERT INTO "techApp_io" VALUES('pipe1_2','Po2','Output 2 pressure (at)',2,1,'1',0,7,'Вихідний тиск 2 (ата)','Выходн. давление 2 (ата)','','');
INSERT INTO "techApp_io" VALUES('pipe1_2','So2','Output 2 cutset (m2)',2,0,'.2',0,8,'Вихідний перетин труби 2 (м2)','Выходн. сеч. 2 (м2)','','');
INSERT INTO "techApp_io" VALUES('pipe1_2','lo2','Output 2 length (m)',2,0,'10',0,9,'Вихідна довжина труби 2 (м)','Выходн. длина 2 (м)','','');
INSERT INTO "techApp_io" VALUES('pipe1_2','Q0','Norm density of environs (kg/m3)',2,0,'1',0,10,'Норм. щільність середовища (кг/м3)','Норм. плотн. среды (кг/м3)','','');
INSERT INTO "techApp_io" VALUES('pipe1_2','f_frq','Calc frequency (Hz)',2,0,'100',1,12,'Частота обчислення функції (Гц)','Частота обсчёта (Гц)','','');
INSERT INTO "techApp_io" VALUES('pipe1_2','F1tmp','Temporary flow 1',2,1,'0',1,13,'Вхід. витрати утримані 1','Врем. расход 1','','');
INSERT INTO "techApp_io" VALUES('pipe1_2','F2tmp','Temporary flow 2',2,1,'0',1,14,'Вхід. витрати утримані 2','Врем. расход 2','','');
INSERT INTO "techApp_io" VALUES('pipe1_2','Pot1','Temporary pressure 1',2,1,'1',1,15,'Вих. тиск утриманий 1','Вых. давл. удержанное','','');
INSERT INTO "techApp_io" VALUES('pipe1_2','Pot2','Temporary pressure 2',2,1,'1',1,16,'Вих. тиск утриманий 2','Вых. давл. удержанное','','');
INSERT INTO "techApp_io" VALUES('pipe1_3','Fi','Input flow (tones/h)',2,1,'0',0,0,'Вхідні витрати (т/год)','Входн. расход (т/ч)','','');
INSERT INTO "techApp_io" VALUES('pipe1_3','Pi','Input pressure (at)',2,0,'1',0,1,'Вхідний тиск (ата)','Входн. давление (ата)','','');
INSERT INTO "techApp_io" VALUES('pipe1_3','Fo1','Output 1 flow (tones/h)',2,0,'0',0,2,'Вихідні витрати 1 (т/год)','Выходн. расход 1 (т/ч)','','');
INSERT INTO "techApp_io" VALUES('pipe1_3','Po1','Output 1 pressure (at)',2,1,'1',0,3,'Вихідний тиск 1 (ата)','Выходн. давление 1 (ата)','','');
INSERT INTO "techApp_io" VALUES('pipe1_3','So1','Output 1 cutset (m2)',2,0,'0.2',0,4,'Вихідний перетин труби 1 (м2)','Выходн. сеч. 1 (м2)','','');
INSERT INTO "techApp_io" VALUES('pipe1_3','lo1','Output 1 length (m)',2,0,'10',0,5,'Вихідна довжина труби 1 (м)','Выходн. длина 1 (м)','','');
INSERT INTO "techApp_io" VALUES('pipe1_3','Fo2','Output 2 flow (tones/h)',2,0,'0',0,6,'Вихідні витрати 2 (т/год)','Выходн. расход 2 (т/ч)','','');
INSERT INTO "techApp_io" VALUES('pipe1_3','Po2','Output 2 pressure (at)',2,1,'1',0,7,'Вихідний тиск 2 (ата)','Выходн. давление 2 (ата)','','');
INSERT INTO "techApp_io" VALUES('pipe1_3','So2','Output 2 cutset (m2)',2,0,'0.2',0,8,'Вихідний перетин труби 2 (м2)','Выходн. сеч. 2 (м2)','','');
INSERT INTO "techApp_io" VALUES('pipe1_3','lo2','Output 2 length (m)',2,0,'10',0,9,'Вихідна довжина труби 2 (м)','Выходн. длина 2 (м)','','');
INSERT INTO "techApp_io" VALUES('pipe1_3','Fo3','Output 3 flow (tones/h)',2,0,'0',0,10,'Вихідні витрати 3 (т/год)','Выходн. расход 3 (т/ч)','','');
INSERT INTO "techApp_io" VALUES('pipe1_3','Po3','Output 3 pressure (at)',2,1,'1',0,11,'Вихідний тиск 3 (ата)','Выходн. давление 3 (ата)','','');
INSERT INTO "techApp_io" VALUES('pipe1_3','So3','Output 3 cutset (m2)',2,0,'0.2',0,12,'Вихідний перетин труби 3 (м2)','Выходн. сеч. 3 (м2)','','');
INSERT INTO "techApp_io" VALUES('pipe1_3','lo3','Output 3 length (m)',2,0,'10',0,13,'Вихідна довжина труби 3 (м)','Выходн. длина 3 (м)','','');
INSERT INTO "techApp_io" VALUES('pipe1_3','Q0','Norm density of environs (kg/m3)',2,0,'1',0,14,'Норм. щільність середовища (кг/м3)','Норм. плотн. среды (кг/м3)','','');
INSERT INTO "techApp_io" VALUES('pipe1_3','f_frq','Calc frequency (Hz)',2,0,'200',1,16,'Частота обчислення функції (Гц)','Частота обсчёта (Гц)','','');
INSERT INTO "techApp_io" VALUES('pipe1_3','F1tmp','Temporary flow 1',2,1,'0',0,17,'Вхід. витрати утримані 1','Врем. расход 1','','');
INSERT INTO "techApp_io" VALUES('pipe1_3','F2tmp','Temporary flow 2',2,1,'0',0,18,'Вхід. витрати утримані 2','Врем. расход 2','','');
INSERT INTO "techApp_io" VALUES('pipe1_3','F3tmp','Temporary flow 3',2,1,'0',0,19,'Вхід. витрати утримані 3','Врем. расход 3','','');
INSERT INTO "techApp_io" VALUES('pipe1_3','Pot1','Temporary pressure 1',2,1,'1',1,20,'Вих. тиск утриманий 1','Вых. давл. удержанное','','');
INSERT INTO "techApp_io" VALUES('pipe1_3','Pot2','Temporary pressure 2',2,1,'1',1,21,'Вих. тиск утриманий 2','Вых. давл. удержанное','','');
INSERT INTO "techApp_io" VALUES('pipe1_3','Pot3','Temporary pressure 3',2,1,'1',1,22,'Вих. тиск утриманий 3','Вых. давл. удержанное','','');
INSERT INTO "techApp_io" VALUES('pipe1_4','Fi','Input flow (tones/h)',2,1,'0',0,0,'Вхідні витрати (т/год)','Входн. расход (т/ч)','','');
INSERT INTO "techApp_io" VALUES('pipe1_4','Pi','Input pressure (at)',2,0,'1',0,1,'Вхідний тиск (ата)','Входн. давление (ата)','','');
INSERT INTO "techApp_io" VALUES('pipe1_4','Fo1','Output 1 flow (tones/h)',2,0,'0',0,2,'Вихідні витрати 1 (т/год)','Выходн. расход 1 (т/ч)','','');
INSERT INTO "techApp_io" VALUES('pipe1_4','Po1','Output 1 pressure (at)',2,1,'1',0,3,'Вихідний тиск 1 (ата)','Выходн. давление 1 (ата)','','');
INSERT INTO "techApp_io" VALUES('pipe1_4','So1','Output 1 cutset (m2)',2,0,'0.2',0,4,'Вихідний перетин труби 1 (м2)','Выходн. сеч. 1 (м2)','','');
INSERT INTO "techApp_io" VALUES('pipe1_4','lo1','Output 1 length (m)',2,0,'10',0,5,'Вихідна довжина труби 1 (м)','Выходн. длина 1 (м)','','');
INSERT INTO "techApp_io" VALUES('pipe1_4','Fo2','Output 2 flow (tones/h)',2,0,'0',0,6,'Вихідні витрати 2 (т/год)','Выходн. расход 2 (т/ч)','','');
INSERT INTO "techApp_io" VALUES('pipe1_4','Po2','Output 2 pressure (at)',2,1,'1',0,7,'Вихідний тиск 2 (ата)','Выходн. давление 2 (ата)','','');
INSERT INTO "techApp_io" VALUES('pipe1_4','So2','Output 2 cutset (m2)',2,0,'0.2',0,8,'Вихідний перетин труби 2 (м2)','Выходн. сеч. 2 (м2)','','');
INSERT INTO "techApp_io" VALUES('pipe1_4','lo2','Output 2 length (m)',2,0,'10',0,9,'Вихідна довжина труби 2 (м)','Выходн. длина 2 (м)','','');
INSERT INTO "techApp_io" VALUES('pipe1_4','Fo3','Output 3 flow (tones/h)',2,0,'0',0,10,'Вихідні витрати 3 (т/год)','Выходн. расход 3 (т/ч)','','');
INSERT INTO "techApp_io" VALUES('pipe1_4','Po3','Output 3 pressure (at)',2,1,'1',0,11,'Вихідний тиск 3 (ата)','Выходн. давление 3 (ата)','','');
INSERT INTO "techApp_io" VALUES('pipe1_4','So3','Output 3 cutset (m2)',2,0,'0.2',0,12,'Вихідний перетин труби 3 (м2)','Выходн. сеч. 3 (м2)','','');
INSERT INTO "techApp_io" VALUES('pipe1_4','lo3','Output 3 length (m)',2,0,'10',0,13,'Вихідна довжина труби 3 (м)','Выходн. длина 3 (м)','','');
INSERT INTO "techApp_io" VALUES('pipe1_4','Fo4','Output 4 flow (tones/h)',2,0,'0',0,14,'Вихідні витрати 4 (т/год)','Выходн. расход 4 (т/ч)','','');
INSERT INTO "techApp_io" VALUES('pipe1_4','Po4','Output 4 pressure (at)',2,1,'1',0,15,'Вихідний тиск 4 (ата)','Выходн. давление 4 (ата)','','');
INSERT INTO "techApp_io" VALUES('pipe1_4','So4','Output 4 cutset (m2)',2,0,'0.2',0,16,'Вихідний перетин труби 4 (м2)','Выходн. сеч. 4 (м2)','','');
INSERT INTO "techApp_io" VALUES('pipe1_4','lo4','Output 4 length (m)',2,0,'10',0,17,'Вихідна довжина труби 4 (м)','Выходн. длина 4 (м)','','');
INSERT INTO "techApp_io" VALUES('pipe1_4','Q0','Norm density of environs (kg/m3)',2,0,'1',0,18,'Норм. щільність середовища (кг/м3)','Норм. плотн. среды (кг/м3)','','');
INSERT INTO "techApp_io" VALUES('pipe1_4','f_frq','Calc frequency (Hz)',2,0,'200',1,20,'Частота обчислення функції (Гц)','Частота обсчёта (Гц)','','');
INSERT INTO "techApp_io" VALUES('pipe1_4','F1tmp','Temporary flow 1',2,1,'0',1,21,'Вхід. витрати утримані 1','Врем. расход 1','','');
INSERT INTO "techApp_io" VALUES('pipe1_4','F2tmp','Temporary flow 2',2,1,'0',1,22,'Вхід. витрати утримані 2','Врем. расход 2','','');
INSERT INTO "techApp_io" VALUES('pipe1_4','F3tmp','Temporary flow 3',2,1,'0',1,23,'Вхід. витрати утримані 3','Врем. расход 3','','');
INSERT INTO "techApp_io" VALUES('pipe1_4','F4tmp','Temporary flow 4',2,1,'0',1,24,'Вхід. витрати утримані 4','Врем. расход 4','','');
INSERT INTO "techApp_io" VALUES('pipe1_4','Pot1','Temporary pressure 1',2,1,'1',1,25,'Вих. тиск утриманий 1','Вых. давл. удержанное','','');
INSERT INTO "techApp_io" VALUES('pipe1_4','Pot2','Temporary pressure 2',2,1,'1',1,26,'Вих. тиск утриманий 2','Вых. давл. удержанное','','');
INSERT INTO "techApp_io" VALUES('pipe1_4','Pot3','Temporary pressure 3',2,1,'1',1,27,'Вих. тиск утриманий 3','Вых. давл. удержанное','','');
INSERT INTO "techApp_io" VALUES('pipe1_4','Pot4','Temporary pressure 4',2,1,'1',1,28,'Вих. тиск утриманий 4','Вых. давл. удержанное','','');
INSERT INTO "techApp_io" VALUES('klapMech','pos','Position (%)',2,1,'0',0,0,'Положення (%)','Положение (%)','','');
INSERT INTO "techApp_io" VALUES('klapMech','pos_sensor','Position by sensor (%)',2,1,'0',0,1,'Положення за давачем (%)','Положение по датчику (%)','','');
INSERT INTO "techApp_io" VALUES('klapMech','com','Command',2,0,'0',0,2,'Команда','Команда','','');
INSERT INTO "techApp_io" VALUES('klapMech','st_open','State "Open"',3,1,'0',0,3,'Стан "Відкрито"','Сост. "Открыто"','','');
INSERT INTO "techApp_io" VALUES('klapMech','st_close','State "Close"',3,1,'1',0,4,'Стан "Закрито"','Сост. "Закрыто"','','');
INSERT INTO "techApp_io" VALUES('klapMech','t_full','Going time (s)',2,0,'3',0,5,'Час ходу (с)','Время хода (с)','','');
INSERT INTO "techApp_io" VALUES('klapMech','t_up','Estrangement time (s)',2,0,'1',0,6,'Час зриву (с)','Время срыва (с)','','');
INSERT INTO "techApp_io" VALUES('klapMech','t_sensor','Sensors'' lag time (s)',2,0,'1',0,7,'Час затримки сенсора (с)','Время задержки сенсора (с)','','');
INSERT INTO "techApp_io" VALUES('klapMech','f_frq','Calc frequency (Hz)',2,0,'100',1,8,'Частота обчислення функції (Гц)','Частота обсчёта (Гц)','','');
INSERT INTO "techApp_io" VALUES('klapMech','tmp_up','Estrangement count',2,1,'0',0,9,'Лічильник зриву','Счётчик срыва','','');
INSERT INTO "techApp_io" VALUES('klapMech','lst_com','Last command',2,1,'0',0,10,'Остання команда','Последняя команда','','');
INSERT INTO "techApp_io" VALUES('diafragma','Fi','Input flow (tones/h)',2,1,'0',0,0,'Вхідні витрати (т/год)','Входн. расход (т/ч)','','');
INSERT INTO "techApp_io" VALUES('diafragma','Pi','Input pressure (at)',2,0,'1',0,1,'Вхідний тиск (ата)','Входн. давление (ata)','','');
INSERT INTO "techApp_io" VALUES('diafragma','Fo','Output flow (tones/h)',2,0,'0',0,2,'Вихідні витрати (т/год)','Выходн. расход (т/ч)','','');
INSERT INTO "techApp_io" VALUES('diafragma','Po','Output pressure (at)',2,1,'1',0,3,'Вихідний тиск (ата)','Выходн. давление (ata)','','');
INSERT INTO "techApp_io" VALUES('diafragma','dP','Pressure differential (kPa)',2,1,'0',0,4,'Перепад тиску (кПа)','Перепад давления (кПа)','','');
INSERT INTO "techApp_io" VALUES('diafragma','Sdf','Diaphragm cutset (m2)',2,0,'0.1',0,5,'Перетин діафрагми (м2)','Сеч. диафрагмы (м2)','','');
INSERT INTO "techApp_io" VALUES('diafragma','So','Output pipe cutset (m2)',2,0,'0.2',0,6,'Вихідний перетин труби (м2)','Сеч. тр. на выходе (м2)','','');
INSERT INTO "techApp_io" VALUES('diafragma','lo','Output pipe length (m)',2,0,'10',0,7,'Вихідна довжина труби (м)','Длина тр. на выходе (м)','','');
INSERT INTO "techApp_io" VALUES('diafragma','Q0','Norm density of environs (kg/m3)',2,0,'1',0,8,'Норм. щільність середовища (кг/м3)','Пл. при реальн. усл. (кг/м3)','','');
INSERT INTO "techApp_io" VALUES('diafragma','f_frq','Calc frequency (Hz)',2,0,'100',1,10,'Частота обчислення функції (Гц)','Частота обсчёта (Гц)','','');
INSERT INTO "techApp_io" VALUES('pipe3_1','Kpr','Compressibility coefficient (0...1)',2,0,'0.95',0,18,'Коефіцієнт стиснення (0...1)','Коеф. сжимаемости среды','','');
INSERT INTO "techApp_io" VALUES('pipe3_1','Fi1','Input 1 flow (tones/h)',2,1,'0',0,0,'Вхідні витрати 1 (т/год)','Входн. расход 1 (т/ч)','','');
INSERT INTO "techApp_io" VALUES('pipe3_1','Pi1','Input 1 pressure (ata)',2,0,'1',0,1,'Вхідний тиск 1 (ата)','Входн. давление 1 (ата)','','');
INSERT INTO "techApp_io" VALUES('pipe3_1','Ti1','Input 1 temperature (K)',2,0,'273',0,2,'Вхідна температура 1 (К)','Входн. температура 1 (К)','','');
INSERT INTO "techApp_io" VALUES('pipe3_1','Si1','Input 1 cutset (m2)',2,0,'0.2',0,3,'Вхідний перетин 1 (м2)','Входн. сечение 1 (м2)','','');
INSERT INTO "techApp_io" VALUES('pipe3_1','Fi2','Input 2 flow (tones/h)',2,1,'0',0,4,'Вхідні витрати 2 (т/год)','Входн. расход 2 (т/ч)','','');
INSERT INTO "techApp_io" VALUES('pipe3_1','Pi2','Input 2 pressure (ata)',2,0,'1',0,5,'Вхідний тиск 2 (ата)','Входн. давление 2 (ата)','','');
INSERT INTO "techApp_io" VALUES('pipe3_1','Ti2','Input 2 temperature (K)',2,0,'273',0,6,'Вхідна температура 2 (К)','Входн. температура 2 (К)','','');
INSERT INTO "techApp_io" VALUES('pipe3_1','Si2','Input 2 cutset (m2)',2,0,'0.2',0,7,'Вхідний перетин 2 (м2)','Входн. сечение 2 (м2)','','');
INSERT INTO "techApp_io" VALUES('pipe3_1','Fi3','Input 3 flow (tones/h)',2,1,'0',0,8,'Вхідні витрати 3 (т/год)','Входн. расход 3 (т/ч)','','');
INSERT INTO "techApp_io" VALUES('pipe3_1','Pi3','Input 3 pressure (ata)',2,0,'1',0,9,'Вхідний тиск 3 (ата)','Входн. давление 3 (ата)','','');
INSERT INTO "techApp_io" VALUES('pipe3_1','Ti3','Input 3 temperature (K)',2,0,'273',0,10,'Вхідна температура 3 (К)','Входн. температура 3 (К)','','');
INSERT INTO "techApp_io" VALUES('pipe3_1','Si3','Input 3 cutset (m2)',2,0,'0.2',0,11,'Вхідний перетин 3 (м2)','Входн. сечение 3 (м2)','','');
INSERT INTO "techApp_io" VALUES('pipe3_1','Fo','Output flow (tones/h)',2,0,'0',0,12,'Вихідні витрати (т/год)','Выходн. расход (т/ч)','','');
INSERT INTO "techApp_io" VALUES('pipe3_1','Po','Output pressure (ata)',2,1,'1',0,13,'Вихідний тиск (ата)','Выходн. давление (ата)','','');
INSERT INTO "techApp_io" VALUES('pipe3_1','To','Output temperature (K)',2,1,'273',0,14,'Вихідна температура (К)','Выходн. температура (К)','','');
INSERT INTO "techApp_io" VALUES('pipe3_1','So','Output cutset (m2)',2,0,'.2',0,15,'Вихідний перетин труби (м2)','Выходн. сечение 1 (м2)','','');
INSERT INTO "techApp_io" VALUES('pipe3_1','lo','Output length (m)',2,0,'10',0,16,'Вихідна довжина труби (м)','Выходн. длина 1 (м)','','');
INSERT INTO "techApp_io" VALUES('pipe3_1','Q0','Norm density of environs (kg/m3)',2,0,'1',0,17,'Норм. щільність середовища (кг/м3)','Норм. плотн. среды (кг/м3)','','');
INSERT INTO "techApp_io" VALUES('pipe3_1','Ct','Warm capacity of environs',2,0,'20',0,19,'Теплоємність середовища','Теплоёмкость среды','','');
INSERT INTO "techApp_io" VALUES('pipe3_1','Riz','Warm resistance of isolation',2,0,'20',0,20,'Тепл. опір ізоляції','Тепл. сопр. изоляции','','');
INSERT INTO "techApp_io" VALUES('pipe3_1','Fwind','Air speed',2,0,'1',0,21,'Швидкість повітря','Скорость воздуха','','');
INSERT INTO "techApp_io" VALUES('pipe3_1','Twind','Air temperature (К)',2,0,'273',0,22,'Темпер. повітря','Температура воздуха (К)','','');
INSERT INTO "techApp_io" VALUES('pipe3_1','f_frq','Calc frequency (Hz)',2,0,'100',1,23,'Частота обчислення функції (Гц)','Частота обсчёта (Гц)','','');
INSERT INTO "techApp_io" VALUES('compressor','Kpr','Compressibility coefficient (0...1)',2,0,'0.95',0,14,'Коефіцієнт стиснення (0...1)','Коеф. сжимаемости среды','','');
INSERT INTO "techApp_io" VALUES('pipeBase','Fi','Input flow (tones/h)',2,1,'0',0,0,'Вхідні витрати (т/год)','Входн. расход (т/ч)','','');
INSERT INTO "techApp_io" VALUES('pipeBase','Pi','Input pressure (at)',2,0,'1',0,1,'Вхідний тиск (ата)','Входн. давление (ата)','','');
INSERT INTO "techApp_io" VALUES('pipeBase','Ti','Input temperature (K)',2,0,'293',0,2,'Вхідна температура (К)','Входн. температура (К)','','');
INSERT INTO "techApp_io" VALUES('pipeBase','Si','Input cutset (m2)',2,0,'.2',0,3,'Вхідний перетин (м2)','Входн. сечение (м2)','','');
INSERT INTO "techApp_io" VALUES('pipeBase','Fo','Output flow (tones/h)',2,0,'0',0,4,'Вихідні витрати (т/год)','Выходн. расход (т/ч)','','');
INSERT INTO "techApp_io" VALUES('pipeBase','Po','Output pressure (at)',2,1,'1',0,5,'Вихідний тиск (ата)','Выходн. давление (ата)','','');
INSERT INTO "techApp_io" VALUES('pipeBase','To','Output temperature (K)',2,1,'293',0,6,'Вихідна температура (К)','Выходн. температура (К)','','');
INSERT INTO "techApp_io" VALUES('pipeBase','So','Output cutset (m2)',2,0,'.2',0,7,'Вихідний перетин труби (м2)','Выходн. сеч. (м2)','','');
INSERT INTO "techApp_io" VALUES('pipeBase','lo','Output length (m)',2,0,'10',0,8,'Вихідна довжина труби (м)','Выходн. длина (м)','','');
INSERT INTO "techApp_io" VALUES('pipeBase','Q0','Norm density of environs (kg/m3)',2,0,'1',0,9,'Норм. щільність середовища (кг/м3)','Норм. плотн. среды (кг/м3)','','');
INSERT INTO "techApp_io" VALUES('pipeBase','Kpr','Compressibility coefficient (0...1)',2,0,'0.98',0,10,'Коефіцієнт стиснення (0...1)','Коеф. сжимаемости среды','','');
INSERT INTO "techApp_io" VALUES('pipeBase','Ktr','Coefficient of friction',2,0,'0.01',0,11,'Коефіцієнт тертя','Коеф. трения','','');
INSERT INTO "techApp_io" VALUES('pipeBase','f_frq','Calc frequency (Hz)',2,0,'100',0,12,'Частота обчислення функції (Гц)','Частота обсчёта (Гц)','','');
INSERT INTO "techApp_io" VALUES('heatExch','Fi1','Input 1 flow (tones/h)',2,0,'20',0,0,'Вхідні витрати 1 (т/год)','Вх. расход первого потока (т/ч)','','');
INSERT INTO "techApp_io" VALUES('heatExch','Pi1','Input 1 pressure (at)',2,0,'1',0,1,'Вхідний тиск 1 (ата)','Вх. давление первого потока (ата)','','');
INSERT INTO "techApp_io" VALUES('heatExch','Ti1','Input 1 temperature (K)',2,0,'20',0,2,'Вхідна температура 1 (К)','Вх. температура первого потока (K)','','');
INSERT INTO "techApp_io" VALUES('heatExch','Si1','Input 1 cutset (m2)',2,0,'1',0,3,'Вхідний перетин 1 (м2)','Вх. сечение труб первого потока (м2)','','');
INSERT INTO "techApp_io" VALUES('heatExch','li1','Input 1 length (m)',2,0,'10',0,4,'Вхідна довжина 1 (м2)','Вх. длина труб первого потока (м)','','');
INSERT INTO "techApp_io" VALUES('heatExch','Q0i1','Input 1 norm density (kg/m3)',2,0,'1',0,5,'Вхідна норм. щільність 1 (кг/м3)','Нормальная плотность первого потока(кг/м3)','','');
INSERT INTO "techApp_io" VALUES('heatExch','Kpr1','Input 1 compressibility coefficient (0...1)',2,0,'0.9',0,6,'Вхідний коефіцієнт стиснення 1 (0...1)','Коэфиц. сжим. среды 1','','');
INSERT INTO "techApp_io" VALUES('heatExch','Ci1','Input 1 warm capacity',2,0,'1',0,7,'Вхідна теплоємність 1','Теплоемкость первого потока(Дж/т*К)','','');
INSERT INTO "techApp_io" VALUES('heatExch','Fi2','Input 2 flow (tones/h)',2,0,'20',0,8,'Вхідні витрати 2 (т/год)','Вх. расход второго потока (т/ч)','','');
INSERT INTO "techApp_io" VALUES('heatExch','Pi2','Input 2 pressure (at)',2,0,'1',0,9,'Вхідний тиск 2 (ата)','Вх. давление второго потока (ата)','','');
INSERT INTO "techApp_io" VALUES('heatExch','Ti2','Input 2 temperature (K)',2,0,'40',0,10,'Вхідна температура 2 (К)','Вх. температура второго потока (K)','','');
INSERT INTO "techApp_io" VALUES('heatExch','Si2','Input 2 cutset (m2)',2,0,'1',0,11,'Вхідний перетин 2 (м2)','Вх. сечение труб второго потока (м2)','','');
INSERT INTO "techApp_io" VALUES('heatExch','li2','Input 2 length (m)',2,0,'10',0,12,'Вхідна довжина 2 (м2)','Вх. длина труб второго потока (м)','','');
INSERT INTO "techApp_io" VALUES('heatExch','Q0i2','Input 2 norm density (kg/m3)',2,0,'1',0,13,'Вхідна норм. щільність 2 (кг/м3)','Нормальная плотность второго потока(кг/м3)','','');
INSERT INTO "techApp_io" VALUES('heatExch','Kpr2','Input 2 compressibility coefficient (0...1)',2,0,'0.9',0,14,'Вхідний коефіцієнт стиснення 2 (0...1)','Коэфиц. сжим. среды 2','','');
INSERT INTO "techApp_io" VALUES('heatExch','Ci2','Input 2 warm capacity',2,0,'1',0,15,'Вхідна теплоємність 2','Теплоемкость второго потока(Дж/т*К)','','');
INSERT INTO "techApp_io" VALUES('heatExch','ki','Heat transfer coefficient',2,0,'0.9',0,16,'Коефіцієнт тепловіддачі','Коэффициент теплоотдачи ','','');
INSERT INTO "techApp_io" VALUES('heatExch','Fo1','Output 1 flow (tones/h)',2,0,'0',0,17,'Вихідні витрати 1 (т/год)','Вых. расход первого потока (т/ч)','','');
INSERT INTO "techApp_io" VALUES('heatExch','Po1','Output 1 pressure (at)',2,1,'1',0,18,'Вихідний тиск 1 (ата)','Вых. давление первого потока (ата)','','');
INSERT INTO "techApp_io" VALUES('heatExch','To1','Output 1 temperature (K)',2,1,'273',0,19,'Вихідна температура 1 (К)','Вых. температура первого потока (K)','','');
INSERT INTO "techApp_io" VALUES('heatExch','So1','Output 1 cutset (m2)',2,1,'1',0,20,'Вихідний перетин труби 1 (м2)','Вых. сечение труб первого потока (м2)','','');
INSERT INTO "techApp_io" VALUES('heatExch','lo1','Output 1 length (m)',2,1,'10',0,21,'Вихідна довжина труби 1 (м)','Вых. длина труб первого потока (м)','','');
INSERT INTO "techApp_io" VALUES('heatExch','Fo2','Output 2 flow (tones/h)',2,0,'0',0,22,'Вихідні витрати 2 (т/год)','Вых. расход второго потока (т/ч)','','');
INSERT INTO "techApp_io" VALUES('heatExch','Po2','Output 2 pressure (at)',2,1,'1',0,23,'Вихідний тиск 2 (ата)','Вых. давление второго потока (ата)','','');
INSERT INTO "techApp_io" VALUES('heatExch','To2','Output 2 temperature (K)',2,1,'273',0,24,'Вихідна температура 2 (К)','Вых. температура второго потока (K)','','');
INSERT INTO "techApp_io" VALUES('heatExch','So2','Output 2 cutset (m2)',2,1,'1',0,25,'Вихідний перетин труби 2 (м2)','Вых. сечение труб второго потока (м2)','','');
INSERT INTO "techApp_io" VALUES('heatExch','lo2','Output 2 length (m)',2,1,'10',0,26,'Вихідна довжина труби 2 (м)','Вых. длина труб второго потока (м)','','');
INSERT INTO "techApp_io" VALUES('heatExch','f_frq','Calc frequency (Hz)',2,0,'200',0,27,'Частота обчислення функції (Гц)','Частота обсчёта функции (Гц)','','');
INSERT INTO "techApp_io" VALUES('boilerBarrel','Fi1','Input water flow (tones/h)',2,1,'22',0,0,'Вхідн. витрати води (т/год)','Вх. расход воды (т/ч)','','');
INSERT INTO "techApp_io" VALUES('boilerBarrel','Pi1','Input water pressure (at)',2,0,'43',0,1,'Вхідн. тиск води (ата)','Вх. давление воды (ата)','','');
INSERT INTO "techApp_io" VALUES('boilerBarrel','Ti1','Input water temperature (K)',2,0,'523',0,2,'Вхідн. температура води (К)','Вх. температура воды (K)','','');
INSERT INTO "techApp_io" VALUES('boilerBarrel','Si1','Input water cutset (m2)',2,0,'0.6',0,3,'Вхідн. перетин труби води (м2)','Вх. сечение труб с водой (м2)','','');
INSERT INTO "techApp_io" VALUES('boilerBarrel','Fi2','Input smoke gas flow (tones/h)',2,1,'',0,4,'Вхідн. витрати димових газів (т/год)','Вх. расход дымовых газов (т/ч)','','');
INSERT INTO "techApp_io" VALUES('boilerBarrel','Pi2','Input smoke gas pressure (at)',2,0,'1.3',0,5,'Вхідн. тиск димових газів (ата)','Вх. давление дымовых газов (ата)','','');
INSERT INTO "techApp_io" VALUES('boilerBarrel','Ti2','Input smoke gas temperature (K)',2,0,'1700',0,6,'Вхідн. температура димових газів (К)','Вх. температура дымовых газов (K)','','');
INSERT INTO "techApp_io" VALUES('boilerBarrel','Si2','Input smoke gas cutset (m2)',2,0,'10',0,7,'Вхідн. перетин труби димових газів (м2)','Вх. сечение трубы дымовых газов (м2)','','');
INSERT INTO "techApp_io" VALUES('boilerBarrel','Vi1','Barrel volume (m3)',2,0,'3',0,8,'Об''єм барабану (м3)','Объем барабана (м3)','','');
INSERT INTO "techApp_io" VALUES('boilerBarrel','Lo','Barrel level (%)',2,1,'10',0,9,'Рівень у барабані (%)','Уровень в барабане(%)','','');
INSERT INTO "techApp_io" VALUES('boilerBarrel','S','Heated surface (м2)',2,0,'15',0,10,'Поверхня нагріву (м2)','Поверхность нагрева (м2)','','');
INSERT INTO "techApp_io" VALUES('boilerBarrel','k','Heat transfer coefficient',2,0,'0.8',0,11,'Коефіцієнт тепловіддачі','Коэффициент теплоотдачи','','');
INSERT INTO "techApp_io" VALUES('boilerBarrel','Fo','Output steam flow (tones/h)',2,0,'20',0,12,'Вихідн. витрати пару (т/год)','Вых. расход пара (т/ч)','','');
INSERT INTO "techApp_io" VALUES('boilerBarrel','Po1','Output steam pressure (at)',2,1,'41.68',0,13,'Вихідн. тиск пару (ата)','Вых. давление пара (ата)','','');
INSERT INTO "techApp_io" VALUES('boilerBarrel','To1','Output steam temperature (K)',2,1,'10',0,14,'Вихідн. температура пару (К)','Вых. температура пара (K)','','');
INSERT INTO "techApp_io" VALUES('boilerBarrel','So1','Output steam pipe cutset (m2)',2,0,'0.5',0,15,'Вихідн. перетин труби пару (м2)','Вых. сечение трубы по пару (м2)','','');
INSERT INTO "techApp_io" VALUES('boilerBarrel','lo1','Output steam pipe length (m)',2,0,'5',0,16,'Вихідн. довжина труби пару (м)','Вых. длина трубы пара (м)','','');
INSERT INTO "techApp_io" VALUES('boilerBarrel','Fo2','Output smoke gas flow (tones/h)',2,0,'180',0,17,'Вихідн. витрати димових газів (т/год)','Вых. расход дымовых газов (т/ч)','','');
INSERT INTO "techApp_io" VALUES('boilerBarrel','Po2','Output smoke gas pressure (at)',2,1,'1',0,18,'Вихідн. тиск димових газів (ата)','Вых. давление дымовых газов (ата)','','');
INSERT INTO "techApp_io" VALUES('boilerBarrel','To2','Output smoke gas temperature (K)',2,0,'0',0,19,'Вихідн. температура димових газів (К)','Вых. температура дымовых газов (K)','','');
INSERT INTO "techApp_io" VALUES('boilerBarrel','Fpara','Inner barrel steam flow (tones/h)',2,1,'0',0,20,'Витрати пару у барабані (т/год)','Расход пара в барабане(т/ч)','','');
INSERT INTO "techApp_io" VALUES('boilerBarrel','Tv','Inner water temperature (K)',2,1,'0',0,21,'Температура води у барабані (K)','Температура воды в барабане(K)','','');
INSERT INTO "techApp_io" VALUES('boilerBarrel','f_frq','Calc frequency (Hz)',2,0,'200',0,22,'Частота обчислення функції (Гц)','Частота обсчета(Гц)','','');
INSERT INTO "techApp_io" VALUES('boilerBurner','Fi1','Input blast furnace gas flow (tone/h)',2,1,'',0,0,'Вхідн. витрати димових газів (т/год)','Вх. расход доменного газа (т/ч)','','');
INSERT INTO "techApp_io" VALUES('boilerBurner','Pi1','Input blast furnace gas pressure (at)',2,0,'',0,1,'Вхідн. тиск димових газів (ата)','Вх. давление доменного газа (ата)','','');
INSERT INTO "techApp_io" VALUES('boilerBurner','Ti1','Input blast furnace gas temperature (K)',2,0,'40',0,2,'Вхідн. температура димових газів (К)','Вх. температура доменного газа (K)','','');
INSERT INTO "techApp_io" VALUES('boilerBurner','Si1','Input blast furnace gas pipe cutset (m2)',2,0,'',0,3,'Вхідн. перетин труби димових газів (м2)','Вх. сечение трубы доменного газа (м2)','','');
INSERT INTO "techApp_io" VALUES('boilerBurner','Fi2','Input natural gas flow (tone/h)',2,1,'',0,4,'Вхідн. витрати природного газу (т/год)','Вх. расход природного газа (т/ч)','','');
INSERT INTO "techApp_io" VALUES('boilerBurner','Pi2','Input natural gas pressure (at)',2,0,'',0,5,'Вхідн. тиск природного газу (ата)','Вх. давление природного газа (ата)','','');
INSERT INTO "techApp_io" VALUES('boilerBurner','Ti2','Input natural gas temperature (K)',2,0,'20',0,6,'Вхідн. температура природного газу (К)','Вх. температура природного газа (K)','','');
INSERT INTO "techApp_io" VALUES('boilerBurner','Si2','Input natural gas pipe cutset (m2)',2,0,'',0,7,'Вхідн. перетин труби природного газу (м2)','Вх. сечение трубы природного газа (м2)','','');
INSERT INTO "techApp_io" VALUES('boilerBurner','Fi3','Input coke oven gas flow (tone/h)',2,1,'',0,8,'Вхідн. витрати коксового газу (т/год)','Вх. расход коксового газа (т/ч)','','');
INSERT INTO "techApp_io" VALUES('boilerBurner','Pi3','Input coke oven gas pressure (at)',2,0,'',0,9,'Вхідн. тиск коксового газу (ата)','Вх. давление коксового газа (ата)','','');
INSERT INTO "techApp_io" VALUES('boilerBurner','Ti3','Input coke oven gas temperature (K)',2,0,'0',0,10,'Вхідн. температура коксового газу (К)','Вх. температура коксового газа (K)','','');
INSERT INTO "techApp_io" VALUES('boilerBurner','Si3','Input coke oven gas pipe cutset (m2)',2,0,'',0,11,'Вхідн. перетин труби коксового газу (м2)','Вх. сечение трубы коксового газа (м2)','','');
INSERT INTO "techApp_io" VALUES('boilerBurner','Fi4','Input air flow (tone/h)',2,1,'',0,12,'Вхідн. витрати повітря (т/год)','Вх. расход воздуха (т/ч)','','');
INSERT INTO "techApp_io" VALUES('boilerBurner','Pi4','Input air pressure (at)',2,0,'',0,13,'Вхідн. тиск повітря (ата)','Вх. давление воздуха (ата)','','');
INSERT INTO "techApp_io" VALUES('boilerBurner','Ti4','Input air temperature (K)',2,0,'20',0,14,'Вхідн. температура повітря (К)','Вх. температура воздуха (K)','','');
INSERT INTO "techApp_io" VALUES('boilerBurner','Si4','Input air cutset (m2)',2,0,'',0,15,'Вхідн. перетин труби повітря (м2)','Вх. сечение трубы воздуха (м2)','','');
INSERT INTO "techApp_io" VALUES('boilerBurner','Fo','Output smoke gas flow (tones/h)',2,0,'',0,16,'Вихідн. витрати димових газів (т/год)','Вых. расход дымовых газов (т/ч)','','');
INSERT INTO "techApp_io" VALUES('boilerBurner','Po','Output smoke gas pressure (at)',2,1,'',0,17,'Вихідн. тиск димових газів (ата)','Вых. давление и в топке (ата)','','');
INSERT INTO "techApp_io" VALUES('boilerBurner','To','Output smoke gas temperature (K)',2,1,'',0,18,'Вихідн. температура димових газів (К)','Вых. температура и в топке (K)','','');
INSERT INTO "techApp_io" VALUES('boilerBurner','So','Output smoke gas pipe cutset (m2)',2,0,'90',0,19,'Вихідн. перетин труби димових газів (м2)','Вых. сечение трубы (м2)','','');
INSERT INTO "techApp_io" VALUES('boilerBurner','lo','Output smoke gas pipe length (m)',2,0,'',0,20,'Вихідн. довжина труби димових газів (м2)','Выходная длина трубы(м)','','');
INSERT INTO "techApp_io" VALUES('boilerBurner','V','Burner volume (m3)',2,0,'830',0,21,'Об''єм топки (м3)','Объём топки(м3)','','');
INSERT INTO "techApp_io" VALUES('boilerBurner','CO','The percentage of CO in the flue stack gases (%)',2,1,'',0,22,'Процент вмісту CO у димових газах (%)','Процент содержания CO в уходящих дымовых газах(%)','','');
INSERT INTO "techApp_io" VALUES('boilerBurner','O2','The percentage of O2 in the flue stack gases (%)',2,1,'',0,23,'Процент вмісту Q2 у димових газах (%)','Процент содержания O2 в уходящих дымовых газах(%)','','');
INSERT INTO "techApp_io" VALUES('boilerBurner','f_frq','Calc frequency (Hz)',2,0,'200',0,24,'Частота обчислення функції (Гц)','Частота дискретизации(Гц)','','');
INSERT INTO "techApp_io" VALUES('pipe2_1','Fi1','Input 1 flow (tones/h)',2,1,'0',0,0,'Вхідні витрати 1 (т/год)','Входн. расход 1 (т/ч)','','');
INSERT INTO "techApp_io" VALUES('pipe2_1','Pi1','Input 1 pressure (at)',2,0,'1',0,1,'Вхідний тиск 1 (ата)','Входн. давление 1 (ата)','','');
INSERT INTO "techApp_io" VALUES('pipe2_1','Ti1','Input 1 temperature (K)',2,0,'273',0,2,'Вхідна температура 1 (К)','Входн. температура 1 (К)','','');
INSERT INTO "techApp_io" VALUES('pipe2_1','Si1','Input 1 cutset (m2)',2,0,'0.2',0,3,'Вхідний перетин 1 (м2)','Входн. сечение 1 (м2)','','');
INSERT INTO "techApp_io" VALUES('pipe2_1','Fi2','Input 2 flow (tones/h)',2,1,'0',0,4,'Вхідні витрати 2 (т/год)','Входн. расход 2 (т/ч)','','');
INSERT INTO "techApp_io" VALUES('pipe2_1','Pi2','Input 2 pressure (at)',2,0,'1',0,5,'Вхідний тиск 2 (ата)','Входн. давление 2 (ата)','','');
INSERT INTO "techApp_io" VALUES('pipe2_1','Ti2','Input 2 temperature (K)',2,0,'273',0,6,'Вхідна температура 2 (К)','Входн. температура 2 (К)','','');
INSERT INTO "techApp_io" VALUES('pipe2_1','Si2','Input 2 cutset (m2)',2,0,'0.2',0,7,'Вхідний перетин 2 (м2)','Входн. сечение 2 (м2)','','');
INSERT INTO "techApp_io" VALUES('pipe2_1','Fo','Output flow (tones/h)',2,0,'0',0,8,'Вихідні витрати (т/год)','Выходн. расход (т/ч)','','');
INSERT INTO "techApp_io" VALUES('pipe2_1','Po','Output pressure (at)',2,1,'1',0,9,'Вихідний тиск (ата)','Выходн. давление (ата)','','');
INSERT INTO "techApp_io" VALUES('pipe2_1','To','Output temperature (K)',2,1,'273',0,10,'Вихідна температура (К)','Выходн. температура (К)','','');
INSERT INTO "techApp_io" VALUES('pipe2_1','So','Output cutset (m2)',2,0,'.2',0,11,'Вихідний перетин труби (м2)','Выходн. сечение 1 (м2)','','');
INSERT INTO "techApp_io" VALUES('pipe2_1','lo','Output length (m)',2,0,'10',0,12,'Вихідна довжина труби (м)','Выходн. длина 1 (м)','','');
INSERT INTO "techApp_io" VALUES('pipe2_1','Q0','Norm density of environs (kg/m3)',2,0,'1',0,13,'Норм. щільність середовища (кг/м3)','Норм. плотн. среды (кг/м3)','','');
INSERT INTO "techApp_io" VALUES('pipe2_1','Kpr','Compressibility coefficient (0...1)',2,0,'0.95',0,14,'Коефіцієнт стиснення (0...1)','Коеф. сжимаемости среды','','');
INSERT INTO "techApp_io" VALUES('pipe2_1','Ct','Warm capacity of environs',2,0,'20',0,15,'Теплоємність середовища','Теплоёмкость среды','','');
INSERT INTO "techApp_io" VALUES('pipe2_1','Riz','Warm resistance of isolation',2,0,'20',0,16,'Тепл. опір ізоляції','Тепл. сопр. изоляции','','');
INSERT INTO "techApp_io" VALUES('pipe2_1','Fwind','Air speed',2,0,'1',0,17,'Швидкість повітря','Скорость воздуха','','');
INSERT INTO "techApp_io" VALUES('pipe2_1','Twind','Air temperature (К)',2,0,'273',0,18,'Темпер. повітря','Температура воздуха (К)','','');
INSERT INTO "techApp_io" VALUES('pipe2_1','f_frq','Calc frequency (Hz)',2,0,'100',1,19,'Частота обчислення функції (Гц)','Частота обсчёта (Гц)','','');
INSERT INTO "techApp_io" VALUES('pipe1_3','Kpr','Compressibility coefficient (0...1)',2,0,'0.95',0,15,'Коефіцієнт стиснення (0...1)','Коеф. сжимаемости среды','','');
INSERT INTO "techApp_io" VALUES('pipe1_4','Kpr','Compressibility coefficient (0...1)',2,0,'0.95',0,19,'Коефіцієнт стиснення (0...1)','Коеф. сжимаемости среды','','');
INSERT INTO "techApp_io" VALUES('diafragma','Kpr','Compressibility coefficient (0...1)',2,0,'0.95',0,9,'Коефіцієнт стиснення (0...1)','Коеф. сжимаемости среды','','');
CREATE TABLE 'tmplib_DevLib_io' ("TMPL_ID" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"TYPE" INTEGER DEFAULT '' ,"FLAGS" INTEGER DEFAULT '' ,"VALUE" TEXT DEFAULT '' ,"POS" INTEGER DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#VALUE" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#VALUE" TEXT DEFAULT '' , PRIMARY KEY ("TMPL_ID","ID"));
INSERT INTO "tmplib_DevLib_io" VALUES('SCU750','transport','Transport',0,64,'SCU750',0,'Transport','SCU750','Transport','SCU750');
INSERT INTO "tmplib_DevLib_io" VALUES('SCU750','addr','Device address (-1...255)',1,64,'1',1,'Device address (-1...255)','','Device address (-1...255)','');
INSERT INTO "tmplib_DevLib_io" VALUES('SCU750','perGet','Period data getting (s)',2,64,'10',2,'Period data getting (s)','','Period data getting (s)','');
INSERT INTO "tmplib_DevLib_io" VALUES('SCU750','version','Version',0,16,'',3,'Version','','Version','');
INSERT INTO "tmplib_DevLib_io" VALUES('SCU750','snCntrUnit','Serial number: Control Unit',0,16,'',4,'Serial number: Control Unit','','Serial number: Control Unit','');
INSERT INTO "tmplib_DevLib_io" VALUES('SCU750','snPump','Serial number: Pump',0,16,'',5,'Serial number: Pump','','Serial number: Pump','');
INSERT INTO "tmplib_DevLib_io" VALUES('SCU750','cntPumpHour','Pump hour counter (minutes)',2,16,'',6,'Pump hour counter (minutes)','','Pump hour counter (minutes)','');
INSERT INTO "tmplib_DevLib_io" VALUES('SCU750','cntCntrUnitHour','Control unit hour counter (minutes)',2,16,'',7,'Control unit hour counter (minutes)','','Control unit hour counter (minutes)','');
INSERT INTO "tmplib_DevLib_io" VALUES('SCU750','cntStart','Start counter',2,16,'',8,'Start counter','','Start counter','');
INSERT INTO "tmplib_DevLib_io" VALUES('SCU750','operMode','Pump Operation Mode',0,16,'',9,'Pump Operation Mode','','Pump Operation Mode','');
INSERT INTO "tmplib_DevLib_io" VALUES('SCU750','errors','Errors',0,16,'',10,'Errors','','Errors','');
INSERT INTO "tmplib_DevLib_io" VALUES('SCU750','events','Events',0,16,'',11,'Events','','Events','');
INSERT INTO "tmplib_DevLib_io" VALUES('SCU750','tTMS','TMS temperature (°С)',1,16,'',12,'TMS temperature (°С)','','TMS temperature (°С)','');
INSERT INTO "tmplib_DevLib_io" VALUES('SCU750','tMotor','Motor temperature (°С)',1,16,'',13,'Motor temperature (°С)','','Motor temperature (°С)','');
INSERT INTO "tmplib_DevLib_io" VALUES('SCU750','rotSpeed','Rotational speed (HZ)',1,16,'',14,'Rotational speed (HZ)','','Rotational speed (HZ)','');
INSERT INTO "tmplib_DevLib_io" VALUES('SCU750','comStart','Command: START',3,32,'',15,'Command: START','','Command: START','');
INSERT INTO "tmplib_DevLib_io" VALUES('SCU750','comStop','Command: STOP',3,32,'',16,'Command: STOP','','Command: STOP','');
INSERT INTO "tmplib_DevLib_io" VALUES('SCU750','comReset','Command: RESET',3,32,'',17,'Command: RESET','','Command: RESET','');
INSERT INTO "tmplib_DevLib_io" VALUES('SCU750','spSpeed','Set point: Speed',1,32,'',18,'Set point: Speed','','Set point: Speed','');
INSERT INTO "tmplib_DevLib_io" VALUES('SCU750','spTMSt','Set point: TMS temperature',1,16,'',19,'Set point: TMS temperature','','Set point: TMS temperature','');
INSERT INTO "tmplib_DevLib_io" VALUES('SCU750','spSpeedPrev','Set point: Speed (previous)',1,1,'',20,'Set point: Speed (previous)','','Set point: Speed (previous)','');
INSERT INTO "tmplib_DevLib_io" VALUES('SCU750','cnt','Counter',1,1,'',21,'Counter','','Counter','');
INSERT INTO "tmplib_DevLib_io" VALUES('SCU750','operModes','Operation modes',4,1,'',22,'Operation modes','','Operation modes','');
INSERT INTO "tmplib_DevLib_io" VALUES('TMH','transport','Transport',0,64,'TMH',0,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('TMH','addr','Device address (-1...255)',1,64,'1',1,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('TMH','perGet','Period data getting (s)',2,64,'10',2,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('TMH','modelID','Model ID',0,16,'',3,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('TMH','errors','Errors',0,16,'',4,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('TMH','rotSpeed','Rotational speed (HZ)',1,16,'',5,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('TMH','I','Current (A)',2,16,'',6,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('TMH','axle1disb','Axle 1 disbalance',1,16,'',7,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('TMH','axle2disb','Axle 2 disbalance',1,16,'',8,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('TMH','MP_X1','MP X1',1,16,'',9,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('TMH','MP_Y1','MP Y1',1,16,'',10,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('TMH','MP_X2','MP X2',1,16,'',11,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('TMH','MP_Y2','MP Y2',1,16,'',12,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('TMH','MP_Z','MP Z',1,16,'',13,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('TMH','operMode','Operation Mode',0,16,'',14,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('TMH','comStart','Command: START',3,32,'',15,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('TMH','comStop','Command: STOP',3,32,'',16,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('TMH','comReset','Command: RESET',3,32,'',17,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('TMH','operCntr','Operation Control Mode',0,16,'',18,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('TMH','comInteract','Interactive mode',3,32,'',19,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('TMH','comAutonom','Autonomous mode',3,32,'',20,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('TMH','cnt','Counter',1,1,'',21,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('TMH','operModes','Operation modes',4,1,'',22,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('TM510x','transport','Transport',0,64,'tm510x',0,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('TM510x','addr','Device address (1...254)',1,64,'1',1,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('TM510x','devTp','Device type',1,16,'',2,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('TM510x','errors','Errors',0,16,'',3,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('TM510x','in1','Input 1',2,16,'',4,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('TM510x','in2','Input 2',2,16,'',5,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('TM510x','in3','Input 3',2,16,'',6,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('TM510x','in4','Input 4',2,16,'',7,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('TM510x','in5','Input 5',2,16,'',8,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('TM510x','in6','Input 6',2,16,'',9,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('TM510x','in7','Input 7',2,16,'',10,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('TM510x','in8','Input 8',2,16,'',11,'','','','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','trAddr','Transport',0,64,'Transport.Serial.out_VKT7',0,'','','Transport','Transport.Serial.out_VKT7');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','firmVer','Firmware version',2,16,'',1,'','','Firmware version','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','abonId','Abonent ID',0,16,'',2,'','','Abonent ID','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','netNum','Network number',1,16,'',3,'','','Network number','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','modelImpl','Model implementaton',1,16,'',4,'','','Model implementaton','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','tTypeM','t: dimension',0,16,'',5,'','','t: dimension','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','GTypeM','G: dimension',0,16,'',6,'','','G: dimension','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','VTypeM','V: dimension',0,16,'',7,'','','V: dimension','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','MTypeM','M: dimension',0,16,'',8,'','','M: dimension','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','PTypeM','P: dimension',0,16,'',9,'','','P: dimension','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','QoTypeM','Qo: dimension',0,16,'',10,'','','Qo: dimension','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','QntTypeHIM','ВНР: dimension',0,16,'',11,'','','ВНР: dimension','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','QntTypeM','ВОК: dimension',0,16,'',12,'','','ВОК: dimension','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','t1_1','t1 (Tв1)',2,16,'',13,'','','t1 (Tв1)','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','t2_1','t2 (Tв1)',2,16,'',14,'','','t2 (Tв1)','');
INSERT INTO "tmplib_DevLib_io" VALUES('VKT7','dt_1','dt (Tв1)',2,16,'',15,'','','dt (Tв1)','');
CREATE TABLE 'tmplib_PrescrTempl_io' ("TMPL_ID" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"TYPE" INTEGER DEFAULT '' ,"FLAGS" INTEGER DEFAULT '' ,"VALUE" TEXT DEFAULT '' ,"POS" INTEGER DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#VALUE" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#VALUE" TEXT DEFAULT '' , PRIMARY KEY ("TMPL_ID","ID"));
INSERT INTO "tmplib_PrescrTempl_io" VALUES('timer','run','Command: run',3,32,'0',4,'Команда: исполнение','','Команда: виконання','');
INSERT INTO "tmplib_PrescrTempl_io" VALUES('timer','pause','Command: pause',3,32,'0',5,'Команда: пауза','','Команда: пауза','');
INSERT INTO "tmplib_PrescrTempl_io" VALUES('timer','error','Prescription: error',3,32,'0',2,'Рецепт: ошибка','','Рецепт: помилка','');
INSERT INTO "tmplib_PrescrTempl_io" VALUES('timer','abort','Prescription: abort',3,32,'0',3,'Рецепт: прерывание','','Рецепт: перервано','');
INSERT INTO "tmplib_PrescrTempl_io" VALUES('timer','rez','Command: result',0,32,'1',6,'Команда: результат','1','Команда: результат','1');
INSERT INTO "tmplib_PrescrTempl_io" VALUES('timer','arg1','Time (s)',2,32,'',7,'Время (с)','','Час (с)','');
INSERT INTO "tmplib_PrescrTempl_io" VALUES('timer','tmp1','Temporary 1',2,1,'0',8,'Временный 1','','Тимчасово 1','');
INSERT INTO "tmplib_PrescrTempl_io" VALUES('backTimer','run','Command: run',3,32,'0',4,'Команда: исполнение','','Команда: виконання','');
INSERT INTO "tmplib_PrescrTempl_io" VALUES('backTimer','pause','Command: pause',3,32,'0',5,'Команда: пауза','','Команда: пауза','');
INSERT INTO "tmplib_PrescrTempl_io" VALUES('backTimer','error','Prescription: error',3,32,'0',2,'Рецепт: ошибка','','Рецепт: помилка','');
INSERT INTO "tmplib_PrescrTempl_io" VALUES('backTimer','abort','Prescription: abort',3,32,'0',3,'Рецепт: прерывание','','Рецепт: перервано','');
INSERT INTO "tmplib_PrescrTempl_io" VALUES('backTimer','rez','Command: result',0,32,'1',6,'Команда: результат','1','Команда: результат','1');
INSERT INTO "tmplib_PrescrTempl_io" VALUES('backTimer','arg1','Time (s)',2,32,'',7,'Время (с)','','Час (с)','');
INSERT INTO "tmplib_PrescrTempl_io" VALUES('backTimer','tmp1','Temporary 1',2,1,'0',8,'Временный 1','','Тимчасово 1','');
INSERT INTO "tmplib_PrescrTempl_io" VALUES('backTimer','start','Prescription: start',3,32,'0',0,'Рецепт: старт','','Рецепт: старт','');
INSERT INTO "tmplib_PrescrTempl_io" VALUES('backTimer','stop','Prescription: stop',3,32,'0',1,'Рецепт: стоп','','Рецепт: стоп','');
INSERT INTO "tmplib_PrescrTempl_io" VALUES('timer','start','Prescription: start',3,32,'0',0,'Рецепт: старт','','Рецепт: старт','');
INSERT INTO "tmplib_PrescrTempl_io" VALUES('timer','stop','Prescription: stop',3,32,'0',1,'Рецепт: стоп','','Рецепт: стоп','');
CREATE TABLE 'tmplib_S7' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"PROGRAM" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"uk#PROGRAM" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"ru#PROGRAM" TEXT DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO "tmplib_S7" VALUES('ai_simple','Simple AI','Simple analog parameter.','JavaLikeCalc.JavaScript
val=val_cod;
if(val!=EVAL_REAL)
{
  val=bDwn+(bUp-bDwn)*(val/27648.0);
  if(bUp!=bDwn && val>bUp)		f_err="101:Upper work border violation";
  else if(bUp!=bDwn && val<bDwn)	f_err="102:Lower work border violation";
  else if(bErrUp!=bErrDwn && val>bErrUp)	f_err="103:Upper alarm border violation";
  else if(bErrUp!=bErrDwn && val<bErrDwn)	f_err="104:Lower alarm border violation";
  else if(bWUp!=bWDwn && val>bWUp)	f_err="105:Upper warning border violation";
  else if(bWUp!=bWDwn && val<bWDwn)	f_err="106:Lower warning border violation";
  else f_err="0";
}','Простий AI','Простий аналоговий параметр.','JavaLikeCalc.JavaScript
val=val_cod;
if(val!=EVAL_REAL)
{
  val=bDwn+(bUp-bDwn)*(val/27648.0);
  if(bUp!=bDwn && val>bUp)		f_err="101:Порушення верхньої робочої границі";
  else if(bUp!=bDwn && val<bDwn)	f_err="102:Порушення нижньої робочої границі";
  else if(bErrUp!=bErrDwn && val>bErrUp)	f_err="103:Порушення верхньої аварійної границі";
  else if(bErrUp!=bErrDwn && val<bErrDwn)	f_err="104:Порушення нижньої аварійної границі";
  else if(bWUp!=bWDwn && val>bWUp)	f_err="105:Порушення верхньої попереджувальної границі";
  else if(bWUp!=bWDwn && val<bWDwn)	f_err="106:Порушення нижньої попереджувальної границі";
  else f_err="0";
}','Простой AI','Простой аналоговый параметр.','JavaLikeCalc.JavaScript
val=val_cod;
if(val!=EVAL_REAL)
{
  val=bDwn+(bUp-bDwn)*(val/27648.0);
  if(bUp!=bDwn && val>bUp)		f_err="101:Нарушение верхней рабочей границы";
  else if(bUp!=bDwn && val<bDwn)	f_err="102:Нарушение нижней рабочей границы";
  else if(bErrUp!=bErrDwn && val>bErrUp)	f_err="103:Нарушение верхней аварийной границы";
  else if(bErrUp!=bErrDwn && val<bErrDwn)	f_err="104:Нарушение нижней аварийной границы";
  else if(bWUp!=bWDwn && val>bWUp)	f_err="105:Нарушение верхней предупредительной границы";
  else if(bWUp!=bWDwn && val<bWDwn)	f_err="106:Нарушение нижней предупредительной границы";
  else f_err="0";
}');
INSERT INTO "tmplib_S7" VALUES('ai_man','Manual input of AI','Manual input of a standard analog parameter.','JavaLikeCalc.JavaScript
//Check and write new value
if(val!=val_o)
{
  val_cod=(27648.0*(val-bDwn))/(bUp-bDwn);
  val_o=val;
}
else
{
  val=val_cod;
}

if(val!=EVAL_REAL)
{
  val=val_o=bDwn+(bUp-bDwn)*(val/27648.0);
  if(bUp!=bDwn && val>bUp)		f_err="101:Upper work border violation";
  else if(bUp!=bDwn && val<bDwn)	f_err="102:Lower work border violation";
  else if(bErrUp!=bErrDwn && val>bErrUp)	f_err="103:Upper alarm border violation";
  else if(bErrUp!=bErrDwn && val<bErrDwn)	f_err="104:Lower alarm border violation";
  else if(bWUp!=bWDwn && val>bWUp)	f_err="105:Upper warning border violation";
  else if(bWUp!=bWDwn && val<bWDwn)	f_err="106:Lower warning border violation";
  else f_err="0";
}','Ручний ввід AI','Стандартний аналоговий параметр ручного вводу.','JavaLikeCalc.JavaScript
//Check and write new value
if(val!=val_o)
{
  val_cod=(27648.0*(val-bDwn))/(bUp-bDwn);
  val_o=val;
}
else
{
  val=val_cod;
}

if(val!=EVAL_REAL)
{
  val=val_o=bDwn+(bUp-bDwn)*(val/27648.0);
  if(bUp!=bDwn && val>bUp)		f_err="101:Порушення верхньої робочої границі";
  else if(bUp!=bDwn && val<bDwn)	f_err="102:Порушення нижньої робочої границі";
  else if(bErrUp!=bErrDwn && val>bErrUp)	f_err="103:Порушення верхньої аварійної границі";
  else if(bErrUp!=bErrDwn && val<bErrDwn)	f_err="104:Порушення нижньої аварійної границі";
  else if(bWUp!=bWDwn && val>bWUp)	f_err="105:Порушення верхньої попереджувальної границі";
  else if(bWUp!=bWDwn && val<bWDwn)	f_err="106:Порушення нижньої попереджувальної границі";
  else f_err="0";
}','Ручной ввод AI','Стандартный аналоговый параметр ручного ввода.','JavaLikeCalc.JavaScript
//Check and write new value
if(val!=val_o)
{
  val_cod=(27648.0*(val-bDwn))/(bUp-bDwn);
  val_o=val;
}
else
{
  val=val_cod;
}

if(val!=EVAL_REAL)
{
  val=val_o=bDwn+(bUp-bDwn)*(val/27648.0);
  if(bUp!=bDwn && val>bUp)		f_err="101:Нарушение верхней рабочей границы";
  else if(bUp!=bDwn && val<bDwn)	f_err="102:Нарушение нижней рабочей границы";
  else if(bErrUp!=bErrDwn && val>bErrUp)	f_err="103:Нарушение верхней аварийной границы";
  else if(bErrUp!=bErrDwn && val<bErrDwn)f_err="104:Нарушение нижней аварийной границы";
  else if(bWUp!=bWDwn && val>bWUp)	f_err="105:Нарушение верхней предупредительной границы";
  else if(bWUp!=bWDwn && val<bWDwn)	f_err="106:Нарушение нижней предупредительной границы";
  else f_err="0";
}');
INSERT INTO "tmplib_S7" VALUES('test','Test','S7 controller testing','
','Test','Тестування контролера S7','
','','Тестирование контроллера S7','
');
CREATE TABLE 'tmplib_S7_io' ("TMPL_ID" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"TYPE" INTEGER DEFAULT '' ,"FLAGS" INTEGER DEFAULT '' ,"VALUE" TEXT DEFAULT '' ,"POS" INTEGER DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#VALUE" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#VALUE" TEXT DEFAULT '' , PRIMARY KEY ("TMPL_ID","ID"));
INSERT INTO "tmplib_S7_io" VALUES('ai_simple','val_cod','Value''s source code',1,128,'',0,'Вихідний код значення',' ','Исходный код значения','');
INSERT INTO "tmplib_S7_io" VALUES('ai_simple','val','Value',2,16,'0',1,'Значення','','Значение','');
INSERT INTO "tmplib_S7_io" VALUES('ai_simple','val_tp','Dimension variable',0,32,'',2,'Одиниця виміру',' ','Еденица измерения','');
INSERT INTO "tmplib_S7_io" VALUES('ai_simple','bDwn','Lower value border',2,32,'0',3,'Нижня границя значення','','Нижняя граница значения','');
INSERT INTO "tmplib_S7_io" VALUES('ai_simple','bUp','Upper value border',2,32,'100',4,'Верхня границя значення','','Верхняя граница значения','');
INSERT INTO "tmplib_S7_io" VALUES('ai_simple','bWDwn','Lower technical border',2,32,'20',5,'Нижня технолог. границя','','Нижняя технологич. граница','');
INSERT INTO "tmplib_S7_io" VALUES('ai_simple','bWUp','Upper technical border',2,32,'80',6,'Верхня технолог. границя','','Верхняя технологич. ганица','');
INSERT INTO "tmplib_S7_io" VALUES('ai_simple','bErrDwn','Lower alarm border',2,32,'10',7,'Нижня аварійна границя','','Нижняя аварийная граница','');
INSERT INTO "tmplib_S7_io" VALUES('ai_simple','bErrUp','Upper alarm border',2,32,'90',8,'Верхня аварійна границя','','Верхняя аварийная граница','');
INSERT INTO "tmplib_S7_io" VALUES('ai_man','val_cod','Value''s source code',1,129,'',0,'Вихідний код значення',' ','Исходный код значения','');
INSERT INTO "tmplib_S7_io" VALUES('ai_man','val','Value',2,32,'0',1,'Значення','','Значение','');
INSERT INTO "tmplib_S7_io" VALUES('ai_man','val_o','Previous value',2,0,'0',2,'Попередне значення','','Предыдущ значение','');
INSERT INTO "tmplib_S7_io" VALUES('ai_man','val_tp','Dimension variable',0,32,'',3,'Одиниця виміру',' ','Единица измерения','');
INSERT INTO "tmplib_S7_io" VALUES('ai_man','bDwn','Lower value border',2,32,'0',4,'Нижня границя значення','','Нижняя граница значения','');
INSERT INTO "tmplib_S7_io" VALUES('ai_man','bUp','Upper value border',2,32,'100',5,'Верхня границя значення','','Верхняя граница значения','');
INSERT INTO "tmplib_S7_io" VALUES('ai_man','bWDwn','Lower technical border',2,32,'20',6,'Нижня технологічна границя','','Нижняя технологическая граница','');
INSERT INTO "tmplib_S7_io" VALUES('ai_man','bWUp','Upper technical border',2,32,'80',7,'Верхня технологічна границя','','Верхняя технологическая граница','');
INSERT INTO "tmplib_S7_io" VALUES('ai_man','bErrDwn','Lower alarm border',2,32,'10',8,'Нижня аварійна границя','','Нижняя аварийная граница','');
INSERT INTO "tmplib_S7_io" VALUES('ai_man','bErrUp','Upper alarm border',2,32,'90',9,'Верхня аварійна границя','','Верхняя аварийная граница','');
INSERT INTO "tmplib_S7_io" VALUES('test','in50','Input50',1,160,'DB Test|108',54,'Вхід50','DB Тест|108','Вход50','DB Тест|108');
INSERT INTO "tmplib_S7_io" VALUES('test','in49','Input49',1,160,'DB Test|106',53,'Вхід49','DB Тест|106','Вход49','DB Тест|106');
INSERT INTO "tmplib_S7_io" VALUES('test','in48','Input48',1,160,'DB Test|104',52,'Вхід48','DB Тест|104','Вход48','DB Тест|104');
INSERT INTO "tmplib_S7_io" VALUES('test','in47','Input47',1,160,'DB Test|102',51,'Вхід47','DB Тест|102','Вход47','DB Тест|102');
INSERT INTO "tmplib_S7_io" VALUES('test','in46','Input46',1,160,'DB Test|100',50,'Вхід46','DB Тест|100','Вход46','DB Тест|100');
INSERT INTO "tmplib_S7_io" VALUES('test','in45','Input45',1,160,'DB Test|98',49,'Вхід45','DB Тест|98','Вход45','DB Тест|98');
INSERT INTO "tmplib_S7_io" VALUES('test','in44','Input44',1,160,'DB Test|96',48,'Вхід44','DB Тест|96','Вход44','DB Тест|96');
INSERT INTO "tmplib_S7_io" VALUES('test','in43','Input43',1,160,'DB Test|94',47,'Вхід43','DB Тест|94','Вход43','DB Тест|94');
INSERT INTO "tmplib_S7_io" VALUES('test','in42','Input42',1,160,'DB Test|92',46,'Вхід42','DB Тест|92','Вход42','DB Тест|92');
INSERT INTO "tmplib_S7_io" VALUES('test','in41','Input41',1,160,'DB Test|90',45,'Вхід41','DB Тест|90','Вход41','DB Тест|90');
INSERT INTO "tmplib_S7_io" VALUES('test','in40','Input40',1,160,'DB Test|86',44,'Вхід40','DB Тест|86','Вход40','DB Тест|86');
INSERT INTO "tmplib_S7_io" VALUES('test','in39','Input39',1,160,'DB Test|84',43,'Вхід39','DB Тест|84','Вход39','DB Тест|84');
INSERT INTO "tmplib_S7_io" VALUES('test','in38','Input38',1,160,'DB Test|82',42,'Вхід38','DB Тест|82','Вход38','DB Тест|82');
INSERT INTO "tmplib_S7_io" VALUES('test','in37','Input37',1,160,'DB Test|80',41,'Вхід37','DB Тест|80','Вход37','DB Тест|80');
INSERT INTO "tmplib_S7_io" VALUES('test','in36','Input36',1,160,'DB Test|78',40,'Вхід36','DB Тест|78','Вход36','DB Тест|78');
INSERT INTO "tmplib_S7_io" VALUES('test','in35','Input35',1,160,'DB Test|76',39,'Вхід35','DB Тест|76','Вход35','DB Тест|76');
INSERT INTO "tmplib_S7_io" VALUES('test','in34','Input34',1,160,'DB Test|74',38,'Вхід34','DB Тест|74','Вход34','DB Тест|74');
INSERT INTO "tmplib_S7_io" VALUES('test','in33','Input33',1,160,'DB Test|72',37,'Вхід33','DB Тест|72','Вход33','DB Тест|72');
INSERT INTO "tmplib_S7_io" VALUES('test','in32','Input32',1,160,'DB Test|70',36,'Вхід32','DB Тест|70','Вход32','DB Тест|70');
INSERT INTO "tmplib_S7_io" VALUES('test','in31','Input31',1,160,'DB Test|68',35,'Вхід31','DB Тест|68','Вход31','DB Тест|68');
INSERT INTO "tmplib_S7_io" VALUES('test','in30','Input30',1,160,'DB Test|64',34,'Вхід30','DB Тест|64','Вход30','DB Тест|64');
INSERT INTO "tmplib_S7_io" VALUES('test','in29','Input29',1,160,'DB Test|62',33,'Вхід29','DB Тест|62','Вход29','DB Тест|62');
INSERT INTO "tmplib_S7_io" VALUES('test','in28','Input28',1,160,'DB Test|60',32,'Вхід28','DB Тест|60','Вход28','DB Тест|60');
INSERT INTO "tmplib_S7_io" VALUES('test','in27','Input27',1,160,'DB Test|58',31,'Вхід27','DB Тест|58','Вход27','DB Тест|58');
INSERT INTO "tmplib_S7_io" VALUES('test','in26','Input26',1,160,'DB Test|56',30,'Вхід26','DB Тест|56','Вход26','DB Тест|56');
INSERT INTO "tmplib_S7_io" VALUES('test','in25','Input25',1,160,'DB Test|54',29,'Вхід25','DB Тест|54','Вход25','DB Тест|54');
INSERT INTO "tmplib_S7_io" VALUES('test','in24','Input24',1,160,'DB Test|52',28,'Вхід24','DB Тест|52','Вход24','DB Тест|52');
INSERT INTO "tmplib_S7_io" VALUES('test','in23','Input23',1,160,'DB Test|50',27,'Вхід23','DB Тест|50','Вход23','DB Тест|50');
INSERT INTO "tmplib_S7_io" VALUES('test','in22','Input22',1,160,'DB Test|48',26,'Вхід22','DB Тест|48','Вход22','DB Тест|48');
INSERT INTO "tmplib_S7_io" VALUES('test','in21','Input21',1,160,'DB Test|46',25,'Вхід21','DB Тест|46','Вход21','DB Тест|46');
INSERT INTO "tmplib_S7_io" VALUES('test','in20','Input20',1,160,'DB Test|44',24,'Вхід20','DB Тест|44','Вход20','DB Тест|44');
INSERT INTO "tmplib_S7_io" VALUES('test','in19','Input19',1,160,'DB Test|42',23,'Вхід19','DB Тест|42','Вход19','DB Тест|42');
INSERT INTO "tmplib_S7_io" VALUES('test','in18','Input18',1,160,'DB Test|40',22,'Вхід18','DB Тест|40','Вход18','DB Тест|40');
INSERT INTO "tmplib_S7_io" VALUES('test','in17','Input17',1,160,'DB Test|38',21,'Вхід17','DB Тест|38','Вход17','DB Тест|38');
INSERT INTO "tmplib_S7_io" VALUES('test','in16','Input16',1,160,'DB Test|36',20,'Вхід16','DB Тест|36','Вход16','DB Тест|36');
INSERT INTO "tmplib_S7_io" VALUES('test','in15','Input15',1,160,'DB Test|34',19,'Вхід15','DB Тест|34','Вход15','DB Тест|34');
INSERT INTO "tmplib_S7_io" VALUES('test','in14','Input14',1,160,'DB Test|32',18,'Вхід14','DB Тест|32','Вход14','DB Тест|32');
INSERT INTO "tmplib_S7_io" VALUES('test','in13','Input13',1,160,'DB Test|30',17,'Вхід13','DB Тест|30','Вход13','DB Тест|30');
INSERT INTO "tmplib_S7_io" VALUES('test','in12','Input12',1,160,'DB Test|28',16,'Вхід12','DB Тест|28','Вход12','DB Тест|28');
INSERT INTO "tmplib_S7_io" VALUES('test','in11','Input11',1,160,'DB Test|26',15,'Вхід11','DB Тест|26','Вход11','DB Тест|26');
INSERT INTO "tmplib_S7_io" VALUES('test','in10','Input10',1,160,'DB Test|24',14,'Вхід10','DB Тест|24','Вход10','DB Тест|24');
INSERT INTO "tmplib_S7_io" VALUES('test','in9','Input9',1,160,'DB Test|22',13,'Вхід9','DB Тест|22','Вход9','DB Тест|22');
INSERT INTO "tmplib_S7_io" VALUES('test','in8','Input8',1,160,'DB Test|20',12,'Вхід8','DB Тест|20','Вход8','DB Тест|20');
INSERT INTO "tmplib_S7_io" VALUES('test','in7','Input7',1,160,'DB Test|18',11,'Вхід7','DB Тест|18','Вход7','DB Тест|18');
INSERT INTO "tmplib_S7_io" VALUES('test','in6','Input6',1,160,'DB Test|16',10,'Вхід6','DB Тест|16','Вход6','DB Тест|16');
INSERT INTO "tmplib_S7_io" VALUES('test','in5','Input5',1,160,'DB Test|14',9,'Вхід5','DB Тест|14','Вход5','DB Тест|14');
INSERT INTO "tmplib_S7_io" VALUES('test','in4','Input4',1,160,'DB Test|12',8,'Вхід4','DB Тест|12','Вход4','DB Тест|12');
INSERT INTO "tmplib_S7_io" VALUES('test','in3','Input3',1,160,'DB Test|10',7,'Вхід3','DB Тест|10','Вход3','DB Тест|10');
INSERT INTO "tmplib_S7_io" VALUES('test','in2','Input2',1,160,'DB Test|8',6,'Вхід2','DB Тест|8','Вход2','DB Тест|8');
INSERT INTO "tmplib_S7_io" VALUES('test','in1','Input1',1,160,'DB Test|6',5,'Вхід1','DB Тест|6','Вход1','DB Тест|6');
INSERT INTO "tmplib_S7_io" VALUES('test','in','Input',1,160,'DB Test|4',4,'Вхід','DB Тест|4','Вход','DB Тест|4');
CREATE TABLE 'tmplib_base_io' ("TMPL_ID" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"TYPE" INTEGER DEFAULT '' ,"FLAGS" INTEGER DEFAULT '' ,"VALUE" TEXT DEFAULT '' ,"POS" INTEGER DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"uk#VALUE" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"ru#VALUE" TEXT DEFAULT '' , PRIMARY KEY ("TMPL_ID","ID"));
INSERT INTO "tmplib_base_io" VALUES('digAlarm','alrm_md','Signal mode',3,64,' ',0,'Режим сигналу',' ','Режим сигнала','');
INSERT INTO "tmplib_base_io" VALUES('digAlarm','alrm_mess','Error message',0,64,' ',1,'Повідомлення помилки',' ','Сообщение ошибки','');
INSERT INTO "tmplib_base_io" VALUES('digAlarm','in','Input',3,144,' ',2,'Вхід',' ','Вход','');
INSERT INTO "tmplib_base_io" VALUES('simleBoard','in','Input',2,128,'Parameter|var',0,'Вхід','Параметр|var','Вход','Параметр|var');
INSERT INTO "tmplib_base_io" VALUES('simleBoard','var','Variable',2,32,'0',1,'Змінна','0','Переменная','');
INSERT INTO "tmplib_base_io" VALUES('simleBoard','ed','Dimension variable',0,32,' ',2,'Одиниця виміру',' ','Еденица измерения','');
INSERT INTO "tmplib_base_io" VALUES('simleBoard','min','Lower work border',2,32,'0',3,'Нижня робоча границя','0','Нижн. раб. граница','');
INSERT INTO "tmplib_base_io" VALUES('simleBoard','max','Upper work border',2,32,'100',4,'Верхня робоча границя','100','Верхн. раб. граница','');
INSERT INTO "tmplib_base_io" VALUES('simleBoard','aMin','Lower alarm border',2,32,'0',5,'Нижня аварійна границя','0','Нижн. аварийная граница','');
INSERT INTO "tmplib_base_io" VALUES('simleBoard','aMax','Upper alarm border',2,32,'0',6,'Верхня аварійна границя','0','Верхн. аварийная граница','');
INSERT INTO "tmplib_base_io" VALUES('simleBoard','wMin','Lower warning border',2,32,'0',7,'Нижня попереджувальна границя','0','Нижн. предупредительная граница','');
INSERT INTO "tmplib_base_io" VALUES('simleBoard','wMax','Upper warning border',2,32,'0',8,'Верхня попереджувальна границя','0','Верхн. предупредительная граница','');
INSERT INTO "tmplib_base_io" VALUES('simleBoard','iAdd','Addon to input',2,64,'0',9,'Додаток до входу','0','Добавка ко входу','');
INSERT INTO "tmplib_base_io" VALUES('simleBoard','iMult','Input multiplication',2,64,'1',10,'Множник до входу','1','Множитель входа','');
INSERT INTO "tmplib_base_io" VALUES('digitBlock','cmdOpen','Command "Open"',3,161,'Crane|com',0,'Команда "Відкрити"','Кран|com','Команда "Открыть"','Кран|com');
INSERT INTO "tmplib_base_io" VALUES('digitBlock','cmdClose','Command "Close"',3,161,'Crane|close',1,'Команда "Закрити"','Кран|close','Команда "Закрыть"','Кран|close');
INSERT INTO "tmplib_base_io" VALUES('digitBlock','cmdStop','Command "Stop"',3,161,'Crane|stop',2,'Команда "Стоп"','Кран|stop','Команда "Стоп"','Кран|stop');
INSERT INTO "tmplib_base_io" VALUES('digitBlock','stOpen','State "Opened"',3,144,'Crane|st_open',3,'Стан "Відкрито"','Кран|st_open','Состояние "Открыт"','Кран|st_open');
INSERT INTO "tmplib_base_io" VALUES('digitBlock','stClose','State "Closed"',3,144,'Crane|st_close',4,'Стан "Закрито"','Кран|st_close','Состояние "Закрыт"','Кран|st_close');
INSERT INTO "tmplib_base_io" VALUES('digitBlock','tCmd','Hold command time (s)',1,32,'5',5,'Час утримання команди (с)','5','Время удерж. команды (с)','');
INSERT INTO "tmplib_base_io" VALUES('digitBlock','last_cmd','Last command',1,1,'0',6,'Остання команда','0','Последняя команда','');
INSERT INTO "tmplib_base_io" VALUES('digitBlock','w_tm','Process command counter',2,1,'0',7,'Лічильник опрацювання команди','0','Счётчик отраб. команды','');
INSERT INTO "tmplib_base_io" VALUES('gasPoint','DS','Density (kg/m3)',2,16,'0',9,'Щильність (кг/м3)','0','Плотность (кг/м3)','');
INSERT INTO "tmplib_base_io" VALUES('gasPoint','dP','Differential pressure (kgH/m2)',2,16,'0',8,'Перепад (кГс/м2)','0','Перепад (кГс/м2)','');
INSERT INTO "tmplib_base_io" VALUES('gasPoint','T','Temperature (grad.C)',2,16,'0',7,'Температура (град.С)','0','Температура (град.С)','');
INSERT INTO "tmplib_base_io" VALUES('gasPoint','P','Pressure (kgH/sm2)',2,16,'0',6,'Тиск (кГс/см2)','0','Давление (кГс/см2)','');
INSERT INTO "tmplib_base_io" VALUES('gasPoint','Q','Volume (1000 x m3)',2,16,'0',5,'Об''єм (тис.м3)','0','Объём (тыс.м3)','');
INSERT INTO "tmplib_base_io" VALUES('gasPoint','F','Flow (1000 x m3/h)',2,16,'0',4,'Витрати (тис.м3)','0','Расход (тыс.м3)','');
INSERT INTO "tmplib_base_io" VALUES('manInUnif','inout','Input/Output',2,129,'Parameter|var',0,'Вхід/Вихід','Parameter|var','Вход/Выход','Параметр|var');
INSERT INTO "tmplib_base_io" VALUES('manInUnif','var','Variable',2,16,'0',1,'Змінна','','Переменная','');
INSERT INTO "tmplib_base_io" VALUES('manInUnif','ed','Dimension',0,32,' ',3,'Од. виміру',' ','Единица измерения',' ');
INSERT INTO "tmplib_base_io" VALUES('manInUnif','min','Scale: minimum',2,32,'0',4,'Шкала: мінімум','','Шкала: минимум','');
INSERT INTO "tmplib_base_io" VALUES('manInUnif','max','Scale: maximum',2,32,'100',5,'Шкала: максимум','','Шкала: максимум','');
INSERT INTO "tmplib_base_io" VALUES('manInUnif','scSqr','Scale: square',3,0,'0',6,'Шкала: квадратична','','Шкала: квадратичная','');
INSERT INTO "tmplib_base_io" VALUES('manInUnif','aMin','Border down alarm',2,32,'0',8,'Границя нижня аварійна','','Граница нижняя ав.','');
INSERT INTO "tmplib_base_io" VALUES('manInUnif','aMax','Border up alarm',2,32,'0',9,'Границя верхня аварійна','','Граница верхняя ав.','');
INSERT INTO "tmplib_base_io" VALUES('manInUnif','wMin','Border down warning',2,32,'0',10,'Границя нижня попереджув.','','Граница нижняя пред.','');
INSERT INTO "tmplib_base_io" VALUES('manInUnif','wMax','Border up warning',2,32,'0',11,'Границя верхня попереджув.','','Граница верхняя пред.','');
INSERT INTO "tmplib_base_io" VALUES('manInUnif','speed','Motion speed',2,32,'0',13,'Швидкість зміни','','Скорость изменения','');
INSERT INTO "tmplib_base_io" VALUES('manInUnif','Tf','Filter time (s)',2,32,'0',15,'Час фільтру (с)','','Время фильтрации (с)','');
INSERT INTO "tmplib_base_io" VALUES('manInUnif','iAdd','Input addon',2,64,'0',16,'Додаток до входу','','Дополнение ко входу','');
INSERT INTO "tmplib_base_io" VALUES('manInUnif','iMult','Input multiplication',2,64,'1',17,'Множник входу','','Множитель ко входу','');
INSERT INTO "tmplib_base_io" VALUES('manInUnif','plcMin','Minimum module scale',2,64,'0',18,'Мінімальна шкала модуля','','Минимум шкалы модуля','');
INSERT INTO "tmplib_base_io" VALUES('manInUnif','plcMax','Maximum module scale',2,64,'0',19,'Максимальна шкала модуля','','Максимум шкалы модуля','');
INSERT INTO "tmplib_base_io" VALUES('manInUnif','plcExcess','Allowed scale exceeding (%)',2,64,'0',20,'Дозволене перевищення шкали (%)','','Допустимое превышение шкалы модуля (%)','');
INSERT INTO "tmplib_base_io" VALUES('anUnif','in','Input',2,128,'Input|var',0,'Вхід','Input|var','Вход','Вход|var');
INSERT INTO "tmplib_base_io" VALUES('anUnif','var','Variable',2,16,'0',2,'Змінна','','Переменная','');
INSERT INTO "tmplib_base_io" VALUES('anUnif','ed','Dimension',0,32,' ',3,'Од. виміру',' ','Ед. изм.',' ');
INSERT INTO "tmplib_base_io" VALUES('anUnif','min','Scale: minimum',2,32,'0',4,'Шкала: мінімум','','Шкала: минимум','');
INSERT INTO "tmplib_base_io" VALUES('anUnif','max','Scale: maximum',2,32,'100',5,'Шкала: максимум','','Шкала: максимум','');
INSERT INTO "tmplib_base_io" VALUES('anUnif','scSqr','Scale: square',3,32,'0',6,'Шкала: квадратична','','Шкала: квадратичная','');
INSERT INTO "tmplib_base_io" VALUES('anUnif','aMin','Border down alarm',2,32,'0',10,'Границя нижня аварійна','','Граница нижняя ав.','');
INSERT INTO "tmplib_base_io" VALUES('anUnif','aMax','Border up alarm',2,32,'0',11,'Границя верхня аварійна','','Граница верхняя ав.','');
INSERT INTO "tmplib_base_io" VALUES('anUnif','wMin','Border down warning',2,32,'0',12,'Границя нижня попереджув.','','Граница нижняя пред.','');
INSERT INTO "tmplib_base_io" VALUES('anUnif','wMax','Border up warning',2,32,'0',13,'Границя верхня попереджув.','','Граница верхняя пред.','');
INSERT INTO "tmplib_base_io" VALUES('anUnif','speed','Motion speed',2,32,'0',15,'Швидкість зміни','','Скорость изменения','');
INSERT INTO "tmplib_base_io" VALUES('anUnif','prec','Precision (signs)',1,32,'2',16,'Точність (знаків)','','Точность (знаков)','');
INSERT INTO "tmplib_base_io" VALUES('anUnif','Tf','Filter time (s)',2,32,'0',17,'Час фільтру (с)','','Время фильтрации (с)','');
INSERT INTO "tmplib_base_io" VALUES('anUnif','iAdd','Input addon',2,64,'0',18,'Додаток до входу','','Дополнение ко входу','');
INSERT INTO "tmplib_base_io" VALUES('anUnif','iMult','Input multiplication',2,64,'1',19,'Множник входу','','Множитель ко входу','');
INSERT INTO "tmplib_base_io" VALUES('anUnif','plcMin','Minimum module scale',2,64,'0',20,'Мінімальна шкала модуля','','Минимум шкалы модуля','');
INSERT INTO "tmplib_base_io" VALUES('anUnif','plcMax','Maximum module scale',2,64,'0',21,'Максимальна шкала модуля','','Максимум шкалы модуля','');
INSERT INTO "tmplib_base_io" VALUES('anUnif','plcExcess','Allowed scale exceeding (%)',2,64,'0',22,'Дозволене перевищення шкали (%)','','Допустимое превышение шкалы модуля (%)','');
INSERT INTO "tmplib_base_io" VALUES('anUnif','plcImit','PLC imitation',3,64,'0',23,'Імітація ПЛК','','Имитация PLC','');
INSERT INTO "tmplib_base_io" VALUES('anUnif','plcImitIn','PLC imitation input',2,128,'Imitation|var',24,'Вхід імітації ПЛК','Imitation|var','Вход имитации PLC','Имитация|var');
INSERT INTO "tmplib_base_io" VALUES('digitBlockUnif','com','Command "Open"',3,161,'Crane|com',0,'Команда "Відкрити"','Crane|com','Команда "Открыть"','Кран|com');
INSERT INTO "tmplib_base_io" VALUES('digitBlockUnif','close','Command "Close"',3,161,'Crane|close',1,'Команда "Закрити"','Crane|close','Команда "Закрыть"','Кран|close');
INSERT INTO "tmplib_base_io" VALUES('digitBlockUnif','stop','Command "Stop"',3,161,'Crane|stop',2,'Команда "Зупинити"','Crane|stop','Команда "Стоп"','Кран|stop');
INSERT INTO "tmplib_base_io" VALUES('digitBlockUnif','st_open','State "Opened"',3,144,'Crane|st_open',3,'Стан "Відкрито"','Crane|st_open','Состояние "Открыт"','Кран|st_open');
INSERT INTO "tmplib_base_io" VALUES('digitBlockUnif','st_close','State "Closed"',3,144,'Crane|st_close',4,'Стан "Закрито"','Crane|st_close','Состояние "Закрыт"','Кран|st_close');
INSERT INTO "tmplib_base_io" VALUES('digitBlockUnif','tCmd','Hold command time (s)',1,64,'5',5,'Час утримання команди (с)','','Время удерж. команды (с)','');
INSERT INTO "tmplib_base_io" VALUES('digitBlockUnif','last_cmd','Last command',1,1,'0',6,'Остання команда','','Последняя команда','');
INSERT INTO "tmplib_base_io" VALUES('digitBlockUnif','w_tm','Process command counter',2,1,'0',7,'Лічильнк обробки команди','','Счётчик отраб. команды','');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','in','Input',2,128,'Signal|in',0,'Вхід','Signal|in','Вход','Сигнал|in');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','sp','Setpoint',2,160,'PID|sp',1,'Завдання','PID|sp','Задание','ПИД|sp');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','var','Variable',2,16,'0',2,'Змінна','','Переменная','');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','manIn','Manual input (%)',2,160,'PID|manIn',3,'Ручний ввід (%)','PID|manIn','Ручной ввод (%)','ПИД|manIn');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','out','Output (%)',2,144,'PID|out',4,'Вихід (%)','PID|out','Выход (%)','ПИД|out');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','impQup','Impulse output up',3,144,'PID|impQupTm',5,'Імпульсний вихід вгору','PID|impQupTm','Имп. выход вверх','ПИД|impQupTm');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','impQdwn','Impulse output down',3,144,'PID|impQdwnTm',6,'Імпульсний вихід вниз','PID|impQdwnTm','Имп. выход вниз','ПИД|impQdwnTm');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','auto','Mode (automat)',3,160,'PID|auto',7,'Режим: автомат','PID|auto','Режим (автомат)','ПИД|auto');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','analog','Analog',3,144,'PID|analog',8,'Аналоговий','PID|analog','Аналоговый','ПИД|analog');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','ed','Dimension',0,32,' ',9,'Од. виміру',' ','Ед. изм.',' ');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','min','Scale: minimum',2,160,'PID|min',10,'Шкала: мінімум','PID|min','Шкала: минимум','ПИД|min');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','max','Scale: maximum',2,160,'PID|max',11,'Шкала: максимум','PID|max','Шкала: максимум','ПИД|max');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','scSqr','Scale: square',3,32,'0',12,'Шкала: квадратична','','Шкала: квадратичная','');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','aMin','Border down alarm',2,32,'0',13,'Границя нижня аварійна','','Граница нижняя ав.','');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','aMax','Border up alarm',2,32,'0',14,'Границя верхня аварійна','','Граница верхняя ав.','');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','wMin','Border down warning',2,32,'0',15,'Границя нижня попереджув.','','Граница нижняя пред.','');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','wMax','Border up warning',2,32,'0',16,'Границя верхня попереджув.','','Граница верхняя пред.','');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','HystBnd','Hysteresis of boders violation',2,32,'0',17,'Гістерезис порушень границь','','Гистерезиз наруш. границ','');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','speed','Motion speed',2,32,'0',18,'Швидкість зміни','','Скорость изменения','');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','prec','Precision (signs)',1,32,'2',19,'Точність (знаків)','','Точность (знаков)','');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','H','Border up warning violation',3,17,'0',20,'Порушення верхньої попереджув. границі','','Нарушение верхней предупредительной границы','');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','L','Border down warning violation',3,17,'0',21,'Порушення нижньої попереджув. границі','','Нарушение нижней предупредительной границы','');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','HH','Border up alarm violation',3,17,'0',22,'Порушення верхньої аварійної границі','','Нарушение верхней аварийной границы','');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','LL','Border down alarm violation',3,17,'0',23,'Порушення нижньої аварійної границі','','Нарушение нижней аварийной границы','');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','EVAL','Value is eval',3,17,'0',24,'Значення невірогідне','','Значение недостоверно','');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','Tf','Filter time (s)',2,32,'0',25,'Час фільтру (с)','','Время фильтрации (с)','');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','Kp','Gain',2,160,'PID|Kp',26,'Пропорц.','PID|Kp','Kp','ПИД|Kp');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','Ki','Gain integration',2,160,'PID|Ki',27,'Пропорц. інтегралу','PID|Ki','Кп интеграла','ПИД|Ki');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','Ti','Тi (ms)',1,160,'PID|Ti',28,'Тi (мс)','PID|Ti','Ти (мс)','ПИД|Ti');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','Kd','Gain differential',2,160,'PID|Kd',29,'Пропорц. діференціалу','PID|Kd','Кп производной','ПИД|Kd');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','Td','Td (ms)',1,160,'PID|Td',30,'Td (мс)','PID|Td','Тд (мс)','ПИД|Td');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','Tzd','T differential lag (ms)',1,160,'PID|Tzd',31,'T затримки діференціалу (мс)','PID|Tzd','T задержка произв. (мс)','ПИД|Tzd');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','TImpPer','T impulses period (ms)',1,160,'PID|TImpPer',32,'T періоду імпульсів (мс)','PID|TImpPer','T период импульса (мс)','ПИД|TImpPer');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','TImpMin','T minimal impulse (ms)',1,160,'PID|TImpMin',33,'T мінімального імпульсу (мс)','PID|TImpMin','T минимальная длина импульса (мс)','ПИД|TImpMin');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','KImpRfact','Impulse rate factor',2,160,'PID|KImpRfact',34,'Рейтинг фактор імпульсу','PID|KImpRfact','Рейтинг фактор импульса','ПИД|KImpRfact');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','Zi','Insensitivity (%)',2,160,'PID|Zi',35,'Нечутливість (%)','PID|Zi','Нечувствительность (%)','ПИД|Zi');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','Hup','Upper output border (%)',2,160,'PID|Hup',36,'Верхня границя виходу (%)','PID|Hup','Верхняя граница выхода (%)','ПИД|Hup');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','Hdwn','Neither output border (%)',2,160,'PID|Hdwn',37,'Нижня границя виходу (%)','PID|Hdwn','Нижняя граница выхода (%)','ПИД|Hdwn');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','K1','Coefficient of input 1',2,160,'PID|K1',38,'Коефіцієнт входу 1','PID|K1','Коэфициент входа 1','ПИД|K1');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','K2','Coefficient of input 2',2,160,'PID|K2',39,'Коефіцієнт входу 2','PID|K2','Коэфициент входа 2','ПИД|K2');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','K3','Coefficient of input 3',2,160,'PID|K3',40,'Коефіцієнт входу 3','PID|K3','Коэфициент входа 3','ПИД|K3');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','K4','Coefficient of input 4',2,160,'PID|K4',41,'Коефіцієнт входу 4','PID|K4','Коэфициент входа 4','ПИД|K4');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','dmsk','Maximum value on eval (Max/Min)',2,64,'0',42,'Максимальне значення при недостовірності (Макс/Мин','','Маск. парам. при недост. (Мин/Макс)','');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','iAdd','Input addon',2,64,'0',43,'Додаток до входу','','Дополнение ко входу','');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','iMult','Input multiplication',2,64,'1',44,'Множник входу','','Множитель ко входу','');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','plcMin','Minimum module scale',2,64,'1',45,'Мінімальна шкала модуля','','Минимум шкалы модуля','');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','plcMax','Maximum module scale',2,64,'5',46,'Максимальна шкала модуля','','Максимум шкалы модуля','');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','plcExcess','Allowed scale exceeding (%)',2,64,'0',47,'Дозволене перевищення шкали (%)','','Допустимое превышение шкалы модуля (%)','');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','plcImit','PLC imitation',3,64,'0',48,'Імітація ПЛК','','Имитация PLC','');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','plcImitIn','PLC imitation input',2,128,'Imitation|var',49,'Вхід імітації ПЛК','Imitation|var','Вход имитации PLC','Имитация|var');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','in','Input',2,128,'Input|var',0,'Вхід','Input|var','Вход','Вход|var');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','var','Variable',2,16,'0',2,'Змінна','','Переменная','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','H','Border up warning violation',3,17,'',3,'Порушення верхньої попереджув. границі','','Нарушение верхней предупредительной границы','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','L','Border down warning violation',3,17,'',4,'Порушення нижньої попереджув. границі','','Нарушение нижней предупредительной границы','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','HH','Border up alarm violation',3,17,'',5,'Порушення верхньої аварійної границі','','Нарушение верхней аварийной границы','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','LL','Border down alarm violation',3,17,'',6,'Порушення нижньої аварійної границі','','Нарушение нижней аварийной границы','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','EVAL','Value is eval',3,17,'',7,'Значення невірогідне','','Значение недостоверно','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','ed','Dimension',0,32,' ',8,'Од. виміру',' ','Ед. изм.',' ');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','min','Scale: minimum',2,32,'0',9,'Шкала: мінімум','','Шкала: минимум','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','max','Scale: maximum',2,32,'100',10,'Шкала: максимум','','Шкала: максимум','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','scSqr','Scale: square',3,32,'0',11,'Шкала: квадратична','','Шкала: квадратичная','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','aMin','Border down alarm',2,32,'0',15,'Границя нижня аварійна','','Граница нижняя ав.','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','aMax','Border up alarm',2,32,'0',16,'Границя верхня аварійна','','Граница верхняя ав.','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','wMin','Border down warning',2,32,'0',17,'Границя нижня попереджув.','','Граница нижняя пред.','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','wMax','Border up warning',2,32,'0',18,'Границя верхня попереджув.','','Граница верхняя пред.','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','HystBnd','Hysteresis of boders violation',2,32,'0',19,'Гистерезис нарушений границ','','Гистерезиз наруш. границ','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','speed','Motion speed',2,32,'0',20,'Швидкість зміни','','Скорость изменения','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','prec','Precision (signs)',1,32,'2',21,'Точність (знаків)','','Точность (знаков)','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','Tf','Filter time (s)',2,32,'0',22,'Час фільтру (с)','','Время фильтрации (с)','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','iAdd','Input addon',2,64,'0',23,'Додаток до входу','','Дополнение ко входу','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','iMult','Input multiplication',2,64,'1',24,'Множник входу','','Множитель ко входу','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','plcMin','Minimum module scale',2,64,'1',25,'Мінімальна шкала модуля','','Минимум шкалы модуля','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','plcMax','Maximum module scale',2,64,'5',26,'Максимальна шкала модуля','','Максимум шкалы модуля','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','plcExcess','Allowed scale exceeding (%)',2,64,'0',27,'Дозволене перевищення шкали (%)','','Допустимое превышение шкалы модуля (%)','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','plcImit','PLC imitation',3,64,'0',28,'Імітація ПЛК','','Имитация PLC','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','plcImitIn','PLC imitation input',2,128,'Imitation|var',29,'Вхід імітації ПЛК','Imitation|var','Вход имитации PLC','Имитация|var');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','in','Input',2,128,'Signal|in',0,'Вхід','Signal|in','Вход','Сигнал|in');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','sp','Setpoint',2,160,'PID|sp',1,'Завдання','PID|sp','Задание','ПИД|sp');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','var','Variable',2,16,'0',2,'Змінна','','Переменная','');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','manIn','Manual input (%)',2,160,'PID|manIn',3,'Ручний ввід (%)','PID|manIn','Ручной ввод (%)','ПИД|manIn');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','out','Output (%)',2,160,'PID|out',4,'Вихід (%)','PID|out','Выход (%)','ПИД|out');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','auto','Mode (automat)',3,160,'PID|auto',5,'Режим: автомат','PID|auto','Режим (автомат)','ПИД|auto');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','casc','Mode (cascad)',3,160,'PID|casc',6,'Режим: каскад','PID|casc','Режим (каскад)','ПИД|casc');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','ed','Dimension',0,32,' ',7,'Од. виміру',' ','Ед. изм.',' ');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','min','Scale: minimum',2,160,'PID|min',8,'Шкала: мінімум','PID|min','Шкала: минимум','ПИД|min');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','max','Scale: maximum',2,160,'PID|max',9,'Шкала: максимум','PID|max','Шкала: максимум','ПИД|max');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','scSqr','Scale: square',3,32,'0',10,'Шкала: квадратична','','Шкала: квадратичная','');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','aMin','Border down alarm',2,32,'0',11,'Границя нижня аварійна','','Граница нижняя ав.','');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','aMax','Border up alarm',2,32,'0',12,'Границя верхня аварійна','','Граница верхняя ав.','');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','wMin','Border down warning',2,32,'0',13,'Границя нижня попереджув.','','Граница нижняя пред.','');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','wMax','Border up warning',2,32,'0',14,'Границя верхня попереджув.','','Граница верхняя пред.','');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','HystBnd','Hysteresis of boders violation',2,32,'0',15,'Гістерезис порушень границь','','Гистерезиз наруш. границ','');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','speed','Motion speed',2,32,'0',16,'Швидкість зміни','','Скорость изменения','');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','prec','Precision (signs)',1,32,'2',17,'Точність (знаків)','','Точность (знаков)','');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','H','Border up warning violation',3,17,'0',18,'Порушення верхньої попереджув. границі','','Нарушение верхней предупредительной границы','');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','L','Border down warning violation',3,17,'0',19,'Порушення нижньої попереджув. границі','','Нарушение нижней предупредительной границы','');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','HH','Border up alarm violation',3,17,'0',20,'Порушення верхньої аварійної границі','','Нарушение верхней аварийной границы','');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','LL','Border down alarm violation',3,17,'0',21,'Порушення нижньої аварійної границі','','Нарушение нижней аварийной границы','');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','EVAL','Value is eval',3,17,'0',22,'Значення невірогідне','','Значение недостоверно','');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','Tf','Filter time (s)',2,32,'0',23,'Час фільтру (с)','','Время фильтрации (с)','');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','Kp','Gain',2,160,'PID|Kp',24,'Пропорц.','PID|Kp','Kp','ПИД|Kp');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','Ti','Тi (ms)',1,160,'PID|Ti',25,'Ті (мс)','PID|Ti','Ти (мс)','ПИД|Ti');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','Kd','Gain differential',2,160,'PID|Kd',26,'Пропорц. діференціалу','PID|Kd','Кп производной','ПИД|Kd');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','Td','Td (ms)',1,160,'PID|Td',27,'Tд (мс)','PID|Td','Тд (мс)','ПИД|Td');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','Tzd','T differential lag (ms)',1,160,'PID|Tzd',28,'T затримки діференціалу (мс)','PID|Tzd','T задержка произв. (мс)','ПИД|Tzd');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','Zi','Insensitivity (%)',2,160,'PID|Zi',29,'Нечутливість (%)','PID|Zi','Нечувствительность (%)','ПИД|Zi');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','Hup','Upper output border (%)',2,160,'PID|Hup',30,'Верхня границя виходу (%)','PID|Hup','Верхняя граница выхода (%)','ПИД|Hup');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','Hdwn','Neither output border (%)',2,160,'PID|Hdwn',31,'Нижня границя виходу (%)','PID|Hdwn','Нижняя граница выхода (%)','ПИД|Hdwn');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','K1','Coefficient of input 1',2,160,'PID|K1',32,'Коефіцієнт входу 1','PID|K1','Коэфициент входа 1','ПИД|K1');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','K2','Coefficient of input 2',2,160,'PID|K2',33,'Коефіцієнт входу 2','PID|K2','Коэфициент входа 2','ПИД|K2');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','K3','Coefficient of input 3',2,160,'PID|K3',34,'Коефіцієнт входу 3','PID|K3','Коэфициент входа 3','ПИД|K3');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','K4','Coefficient of input 4',2,160,'PID|K4',35,'Коефіцієнт входу 4','PID|K4','Коэфициент входа 4','ПИД|K4');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','dmsk','Maximum value on eval (Max/Min)',2,64,'0',36,'Максимальне значення при недостовірності (Макс/Мин','','Маск. парам. при недост. (Мин/Макс)','');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','iAdd','Input addon',2,64,'0',37,'Додаток до входу','','Дополнение ко входу','');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','iMult','Input multiplication',2,64,'1',38,'Множник входу','','Множитель ко входу','');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','plcMin','Minimum module scale',2,64,'1',39,'Мінімальна шкала модуля','','Минимум шкалы модуля','');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','plcMax','Maximum module scale',2,64,'5',40,'Максимальна шкала модуля','','Максимум шкалы модуля','');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','plcExcess','Allowed scale exceeding (%)',2,64,'0',41,'Дозволене перевищення шкали (%)','','Допустимое превышение шкалы модуля (%)','');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','plcImit','PLC imitation',3,64,'0',42,'Імітація ПЛК','','Имитация PLC','');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','plcImitIn','PLC imitation input',2,128,'Imitation|var',43,'Вхід імітації ПЛК','Imitation|var','Вход имитации PLC','Имитация|var');
INSERT INTO "tmplib_base_io" VALUES('manInUnif','varIn','Variable input',2,32,'0',2,'Введення змінної','','Ввод переменной','');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','SHIFR','Code',0,0,'',45,'Шифр','','Шифр','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','this','Object',4,1,'',30,'Об''єкт','','Объект','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','SHIFR','Code',0,0,'',31,'Шифр','','Шифр','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','NAME','Name',0,0,'',32,'Ім''я','','Имя','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','DESCR','Description',0,0,'',33,'Опис','','Описание','');
INSERT INTO "tmplib_base_io" VALUES('anUnif','this','Object',4,1,'',25,'Об''єкт','','Объект','');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','NAME','Name',0,0,'',46,'Ім''я','','Имя','');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','this','Object',4,1,'',44,'Об''єкт','','Объект','');
INSERT INTO "tmplib_base_io" VALUES('anUnif','SHIFR','Code',0,0,'',26,'Шифр','','Шифр','');
INSERT INTO "tmplib_base_io" VALUES('anUnif','NAME','Name',0,0,'',27,'Ім''я','','Имя','');
INSERT INTO "tmplib_base_io" VALUES('anUnif','DESCR','Description',0,0,'',28,'Опис','','Описание','');
INSERT INTO "tmplib_base_io" VALUES('pidUnif','DESCR','Description',0,0,'',47,'Опис','','Описания','');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','this','Object',4,1,'',50,'Об''єкт','','Объект','');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','SHIFR','Code',0,0,'',51,'Шифр','','Шифр','');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','NAME','Name',0,0,'',52,'Ім''я','','Имя','');
INSERT INTO "tmplib_base_io" VALUES('pidUnifImp','DESCR','Description',0,0,'',53,'Опис','','Описания','');
INSERT INTO "tmplib_base_io" VALUES('anUnif','HystBnd','Hysteresis of boders violation',2,32,'0',14,'Гістерезиз поруш. границь','','Гистерезиз наруш. границ','');
INSERT INTO "tmplib_base_io" VALUES('anUnif','inProc','Input processing procedure',0,68,'',1,'Процедура обробки входу','','Процедура обработки входа','');
INSERT INTO "tmplib_base_io" VALUES('anUnif','subMode','Substitute: mode (0-no; 1-last; 2-substitute)',1,32,'0',7,'Заміна: режим (0-немає; 1-останне; 2-заміна)','','Замена: режим (0-нет; 1-последний; 2-замена)','');
INSERT INTO "tmplib_base_io" VALUES('anUnif','subVar','Substitute: variable',2,32,'0',8,'Заміна: змінна','','Замена: переменная','');
INSERT INTO "tmplib_base_io" VALUES('anUnif','alSup','Alarms suppress',3,32,'0',9,'Придушення порушень','','Подавление нарушений','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','inProc','Input processing procedure',0,68,'',1,'Процедура обробки входу','','Процедура обработки входа','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','subMode','Substitute: mode (0-no; 1-last; 2-substitute)',1,32,'0',12,'Заміна: режим (0-немає; 1-останне; 2-заміна)','','Замена: режим (0-нет; 1-последний; 2-замена)','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','subVar','Substitute: variable',2,32,'0',13,'Заміна: змінна','','Замена: переменная','');
INSERT INTO "tmplib_base_io" VALUES('anUnifSt','alSup','Alarms suppress',3,32,'0',14,'Придушення порушень','','Подавление нарушений','');
INSERT INTO "tmplib_base_io" VALUES('manInUnif','alSup','Alarms suppress',3,32,'0',7,'Придушення порушень','','Подавление нарушений','');
INSERT INTO "tmplib_base_io" VALUES('manInUnif','HystBnd','Hysteresis of boders violation',2,32,'0',12,'Гістерезиз поруш. границь','','Гистерезиз наруш. границ','');
INSERT INTO "tmplib_base_io" VALUES('manInUnif','prec','Precision (signs)',1,32,'2',14,'Точність (знаків)','','Точность (знаков)','');
INSERT INTO "tmplib_base_io" VALUES('manInUnif','this','Object',4,1,'',21,'Об''єкт','','Объект','');
INSERT INTO "tmplib_base_io" VALUES('manInUnif','SHIFR','Code',0,0,'',22,'Шифр','','Шифр','');
INSERT INTO "tmplib_base_io" VALUES('manInUnif','NAME','Name',0,0,'',23,'Ім''я','','Имя','');
INSERT INTO "tmplib_base_io" VALUES('manInUnif','DESCR','Description',0,0,'',24,'Опис','','Описание','');
CREATE TABLE 'DAQ_JavaLikeCalc' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ENABLE" INTEGER DEFAULT '0' ,"START" INTEGER DEFAULT '0' ,"MESS_LEV" INTEGER DEFAULT '3' ,"REDNT" INTEGER DEFAULT '0' ,"REDNT_RUN" TEXT DEFAULT '<high>' ,"PRM_BD" TEXT DEFAULT 'system' ,"FUNC" TEXT DEFAULT '' ,"SCHEDULE" TEXT DEFAULT '1' ,"PRIOR" INTEGER DEFAULT '0' ,"ITER" INTEGER DEFAULT '1' , PRIMARY KEY ("ID"));
INSERT INTO "DAQ_JavaLikeCalc" VALUES('prescr','Prescriptions','Prescriptions','Рецепт','','','',1,1,3,0,'<high>','JavaLikePrm_prescr','Controller.prescr','0.2',0,1);
CREATE TABLE 'tmplib_PrescrTempl' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"MAXCALCTM" INTEGER DEFAULT '10' ,"PROGRAM" TEXT DEFAULT '' ,"uk#PROGRAM" TEXT DEFAULT '' ,"ru#PROGRAM" TEXT DEFAULT '' ,"TIMESTAMP" INTEGER DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO "tmplib_PrescrTempl" VALUES('timer','Timer','Таймер','Таймер','Typical timer. Hold run up to time elapse.','Типовой таймер. Удерживает выполнение до завершения времени.','Типовой таймер. Удерживает выполнение до завершения времени.',10,'JavaLikeCalc.JavaScript
//> Reset to default
if(f_start || (!run && tmp1)) { run = pause = error = abort = false; tmp1 = 0; rez = 1; }
//> Run
if(run && !pause)
{
	if(tmp1 == 0) tmp1 = arg1+1/f_frq;
	tmp1 = max(0, tmp1 - 1/f_frq);
	if(tmp1 <= 0) { rez = "1:Waiting "+arg1.toPrecision(3)+"s expired"; run = false; }
	else rez = "0:Waiting "+tmp1.toPrecision(3)+"с";
}','JavaLikeCalc.JavaScript
//> Reset to default
if(f_start || (!run && tmp1)) { run = pause = error = abort = false; tmp1 = 0; rez = 1; }
//> Run
if(run && !pause)
{
	if(tmp1 == 0) tmp1 = arg1+1/f_frq;
	tmp1 = max(0, tmp1 - 1/f_frq);
	if(tmp1 <= 0) { rez = "1:Очікування "+arg1.toPrecision(3)+"с вичерпано"; run = false; }
	else rez = "0:Очікування "+tmp1.toPrecision(3)+"с";
}','JavaLikeCalc.JavaScript
//> Reset to default
if(f_start || (!run && tmp1)) { run = pause = error = abort = false; tmp1 = 0; rez = 1; }
//> Run
if(run && !pause)
{
	if(tmp1 == 0) tmp1 = arg1+1/f_frq;
	tmp1 = max(0, tmp1 - 1/f_frq);
	if(tmp1 <= 0) { rez = "1:Ожидание "+arg1.toPrecision(3)+"с истекло"; run = false; }
	else rez = "0:Ожидание "+tmp1.toPrecision(3)+"с";
}',1372172781);
INSERT INTO "tmplib_PrescrTempl" VALUES('backTimer','Background timer','Фоновий таймер','Фоновый таймер','Background timer. Updating parallel with current command.','Фоновий таймер. Обновляется параллельно с текущей командой.','Фоновый таймер. Обновляется параллельно с текущей командой.',10,'JavaLikeCalc.JavaScript
//> Reset to default
if(f_start || (!run && tmp1)) { run = pause = error = abort = false; tmp1 = 0; rez = 1; }
//> Run
if(run && !pause)
{
	if(tmp1 == 0) tmp1 = arg1+1/f_frq;
	tmp1 = max(0, tmp1 - 1/f_frq);
	if(tmp1 <= 0) { rez = "1:Ожидание "+arg1.toPrecision(3)+"с истекло"; run = false; }
	//if(tmp1 <= 10)	rez = "-1:Waiting "+arg1.toPrecision(3)+"s expired";
	else rez = "10:Background waiting "+tmp1.toPrecision(3)+"s";
}','JavaLikeCalc.JavaScript
//> Reset to default
if(f_start || (!run && tmp1)) { run = pause = error = abort = false; tmp1 = 0; rez = 1; }
//> Run
if(run && !pause)
{
	if(tmp1 == 0) tmp1 = arg1+1/f_frq;
	tmp1 = max(0, tmp1 - 1/f_frq);
	if(tmp1 <= 0) { rez = "1:Ожидание "+arg1.toPrecision(3)+"с истекло"; run = false; }
	//if(tmp1 <= 10)	rez = "-1:Очікування "+arg1.toPrecision(3)+"с вичерпано";
	else rez = "10:Фонове очікування "+tmp1.toPrecision(3)+"с";
}','JavaLikeCalc.JavaScript
//> Reset to default
if(f_start || (!run && tmp1)) { run = pause = error = abort = false; tmp1 = 0; rez = 1; }
//> Run
if(run && !pause)
{
	if(tmp1 == 0) tmp1 = arg1+1/f_frq;
	tmp1 = max(0, tmp1 - 1/f_frq);
	if(tmp1 <= 0) { rez = "1:Ожидание "+arg1.toPrecision(3)+"с истекло"; run = false; }
	//if(tmp1 <= 10)	rez = "-1:Ожидание "+arg1.toPrecision(3)+"с истекло";
	else rez = "10:Фоновое ожидание "+tmp1.toPrecision(3)+"с";
}',1372172772);
CREATE TABLE 'lib_Controllers' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"MAXCALCTM" INTEGER DEFAULT '10' ,"FORMULA" TEXT DEFAULT '' ,"ru#FORMULA" TEXT DEFAULT '' ,"uk#FORMULA" TEXT DEFAULT '' ,"TIMESTAMP" INTEGER DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO "lib_Controllers" VALUES('prescr','prescr','prescr','prescr','','','',10,'clcCnt++;

if(f_start)	work = SYS.XMLNode("prg");

//> Check to commands controller present
comCntrO = false;
if(comsCntr.length)	comCntrO = SYS.DAQ.nodeAt(comsCntr,".");

//> Commands list update
if(f_start || !(clcCnt%(60*f_frq)))
{
	if(!comCntrO)
	{
		rez = SYS.BD.nodeAt(dbDB,".").SQLReq("SELECT * FROM "+dbComs+";");
		comLs = new Object();
		for(i_t = 1; i_t < rez.length; i_t++)
		{
			comNm = rez[i_t]["name"];
			comLs[comNm] = new Object();
			comLs[comNm]["descr"] = rez[i_t]["descr"];
			comLs[comNm]["proc"] = rez[i_t]["proc"];
			for(i_a = 1; i_a <= 5; i_a++)
				comLs[comNm]["arg"+i_a] = rez[i_t]["arg"+i_a];
		}
	}
	else
	{
		var nL = comCntrO.nodeList("prm_");
		comLs = new Object();
		for(i_n = 0; i_n < nL.length; i_n++)
		{
			comO = comCntrO[nL[i_n]];
			comNm = comO.cfg("NAME");
			comLs[comNm] = new Object();
			comLs[comNm]["prmID"] = nL[i_n].slice(4);
			comLs[comNm]["descr"] = comO.cfg("DESCR");
			for(i_a = 1; i_a <= 5; i_a++)
				if(!(comA=comO["arg"+i_a]).isEVal())
					comLs[comNm]["arg"+i_a] = comA.descr();
		}
	}
}

//> Programm selection change
if(curMode <= 0 && prog.length && (prog != work.attr("name") || mode == 1))
{
	work = SYS.XMLNode("prg");
	rez = SYS.BD.nodeAt(dbDB,".").SQLReq("SELECT prgTxt FROM "+dbProgs+" WHERE name=''"+prog+"'';");
	if(rez.length > 1)
	{
		//> Parse programm and insert procedure text and argument''s labels to here
		work.load(rez[1][0]);
		work.setAttr("name",prog);
		for(i_c = 0; i_c < work.childSize(); i_c++)
		{
			comId = work.childGet(i_c).attr("id");
			if(comLs[comId].isEVal())	continue;
			work.childGet(i_c).setAttr("proc",comLs[comId].proc);
			for(i_a = 1; i_a <= 5; i_a++)
				work.childGet(i_c).setAttr("labArg"+i_a,comLs[comId]["arg"+i_a]);
		}
	}
	else prog = "";
}

//> Start process
if(curMode <= 0 && mode == 1 && prog.length)
{
	startTm = SYS.time();
	curCom = 0;
	curMode = mode;	

	if(comCntrO)
	{
		var cL = comCntrO.nodeList("prm_");
		for(i_c = 0; i_c < cL.length; i_c++)
		{
			var cLi = comCntrO[cL[i_c]];
			cLi.run.set(false);
			cLi.stop.set(false);
			cLi.error.set(false);
			cLi.abort.set(false);
			cLi.start.set(true);
		}
	}
}
//> Pause and other process
else if((mode == 0 && curMode < 0) || (curMode == 1 && mode == 2) || (curMode == 2 && mode == 1))
{
	curMode = mode;
	if((curMode == 1 || curMode == 2) && comCntrO)
	{
		var cL = comCntrO.nodeList("prm_");
		for(i_c = 0; i_c < cL.length; i_c++)
			comCntrO[cL[i_c]].pause.set(curMode==2);
	}
}

//> Call program
if(curMode == 1 || curMode == 2)
{
	prog = work.attr("name");

	//> Internal commands call
	if(!comCntrO)
	{
		if(mode == 3 && curCom >= 0 && curCom < work.childSize())
		{
			work.childGet(curCom).setAttr("rez","-10:Step missed");
			curCom++;
		}
		if(curCom >= 0 && curCom < work.childSize())
		{
			comEl = work.childGet(curCom);
			//> Stop process
			if(mode == 0)
			{
				comEl.setAttr("rez","-10:Program terminated");
				//>> Call stop command
				if(!comLs["Stop"].isEVal())
				{
					off = 0;
					prLang = comLs["Stop"].proc.parse(0,"\n",off);
					SYS.DAQ.funcCall(prLang,new Object(),comLs["Stop"].proc.slice(off));
				}
				SYS.messInfo("uprg"+prog,"Terminated by user session of program \""+prog+"\" : "+SYS.strftime(startTm)+" : "+SYS.strftime(SYS.time()));
				curMode = mode = -3;
			}
			//> Pass empty command		
			else if(!comEl.attr("proc").length) curCom++;
			//> Execute command
			else if(curMode == 1)
			{
				args = new Object();
				args.rez = "";
				args.f_start = !comEl.attr("tm").length;
				if(args.f_start) comEl.setAttr("tm",SYS.time());
				args.f_frq = f_frq;
				for(i_a = 1; i_a <= 5; i_a++) args["arg"+i_a] = comEl.attr("arg"+i_a);
				for(i_a = 1; i_a <= 10; i_a++) args["tmp"+i_a] = comEl.attr("tmp"+i_a);
				off = 0;
				prLang = comEl.attr("proc").parse(0,"\n",off);
				rez = SYS.DAQ.funcCall(prLang,args,comEl.attr("proc").slice(off));
				if(rez && args.rez.toInt() > 0) curCom++;
				else if(!rez || args.rez.toInt() < 0)
				{
					//>> Call error command
					if(!comLs["Error"].isEVal())
					{
						off = 0;
						prLang = comLs["Error"].proc.parse(0,"\n",off);
						SYS.DAQ.funcCall(prLang,new Object(),comLs["Error"].proc.slice(off));
					}
					SYS.messInfo("uprg"+prog,"Terminated by error session of program \""+prog+"\" : "+SYS.strftime(startTm)+" : "+SYS.strftime(SYS.time()));
					curMode = mode = -1;
				}
				comEl.setAttr("rez",args.rez);
				for(i_a = 1; i_a <= 5; i_a++) comEl.setAttr("arg"+i_a, args["arg"+i_a]);
				for(i_a = 1; i_a <= 10; i_a++) comEl.setAttr("tmp"+i_a, args["tmp"+i_a]);
				//SYS.messDebug("TEST Calc","TEST Calc rezult: "+args.y);
			}
		}

		//> End call
		if(curCom < 0 || curCom >= work.childSize())
		{
			//>> Call stop command
			if(!comLs["Stop"].isEVal())
			{
				off = 0;
				prLang = comLs["Stop"].proc.parse(0,"\n",off);
				SYS.DAQ.funcCall(prLang,new Object(),comLs["Stop"].proc.slice(off));
			}

			curMode = mode = -2;
			SYS.messInfo("uprg"+prog,"Successful session of program \""+prog+"\" : "+SYS.strftime(startTm)+" : "+SYS.strftime(SYS.time()));

			//> Place to program last execution time
			rez = SYS.BD.nodeAt(dbDB,".").SQLReq("SELECT prgTxt FROM "+dbProgs+" WHERE name=''"+prog+"'';");
			if(rez.length > 1)
			{
				//> Parse programm and insert procedure text and argument''s labels to here
				comTree = SYS.XMLNode("prg");
				comTree.load(rez[1][0]);
				comTree.setAttr("wtm",SYS.time()-startTm);
				SYS.BD.nodeAt(dbDB,".").SQLReq("UPDATE "+dbProgs+" SET prgTxt=''"+comTree.save().replace("''","''''")+"'' WHERE name=''"+prog+"'';");
			}
		}
	}
	//> External commands call
	else
	{
		//>> Get current command parameter-object		
		curComPrm = EVAL_BOOL;
		if(curCom >= 0 && curCom < work.childSize())
		{
			if(!(comId=comLs[work.childGet(curCom).attr("id")]).isEVal())	curComPrm = comCntrO["prm_"+comId.prmID];
			if(curComPrm.isEVal())
			{
				curMode = mode = -1;
				work.childGet(curCom).setAttr("rez","-11:Comand missed: "+work.childGet(curCom).attr("id"));
				return;
			}
		}

		if(mode == 3 && curCom >= 0 && curCom < work.childSize())
		{
			work.childGet(curCom).setAttr("rez","-10:Step missed");
			curComPrm.run.set(false);
			curCom++;
			mode = curMode;
			return;
		}
		if(curCom >= 0 && curCom < work.childSize())
		{
			comEl = work.childGet(curCom);
			//> Stop process
			if(mode == 0)
			{
				comEl.setAttr("rez","-12:Program terminated");
				//>> Stop all typical and set "abort" flag
				var cL = comCntrO.nodeList("prm_");
				for(i_c = 0; i_c < cL.length; i_c++)
				{
					var cLi = comCntrO[cL[i_c]];
					cLi.run.set(false);
					cLi.start.set(false);
					cLi.abort.set(true);
				}

				SYS.messInfo("uprg"+prog,"Terminated by user session of program \""+prog+"\" : "+SYS.strftime(startTm)+" : "+SYS.strftime(SYS.time()));
				curMode = mode = -3;
			}
			//> Commands process
			else
			{
				//>> Start command
				if(!comEl.attr("tm").length)
				{
					curComPrm.run.set(0);	//> Stop for possible background call
					comEl.setAttr("tm",SYS.time());
					for(i_a = 1; i_a <= 5; i_a++)
						if(!(comA=curComPrm["arg"+i_a]).isEVal())
							comA.set(comEl.attr("arg"+i_a));
					curComPrm.rez.set(0);
					curComPrm.run.set(true);
				}

				//> Update steps status, up to current comand
				comTo = min(curCom+1,work.childSize());
				for(i_c = 0; i_c < comTo; i_c++)
				{
					comElI = work.childGet(i_c);
					curComPI = comCntrO["prm_"+comLs[comElI.attr("id")].prmID];
					if(i_c < curCom && !(curComPI.run.get() && 
						comElI.attr("rez").toInt() != 1 && comElI.attr("rez").toInt() > -10))	continue;
					rez = curComPI.rez.get();
					if(i_c == curCom && rez.toInt() > 0) curCom++;
					if(rez.toInt() < 0)
					{
						//>> Stop all typical and call "error" command
						var cL = comCntrO.nodeList("prm_");
						for(i_c = 0; i_c < cL.length; i_c++)
						{
							var cLi = comCntrO[cL[i_c]];
							cLi.run.set(false);
							cLi.start.set(false);
							cLi.error.set(true);
						}

						SYS.messInfo("uprg"+prog,"Terminated by error session of program \""+prog+"\" : "+SYS.strftime(startTm)+" : "+SYS.strftime(SYS.time()));
						curMode = mode = -1;
					}
					comElI.setAttr("rez",rez);
					for(i_a = 1; i_a <= 5; i_a++)
						if(!(comA=curComPI["arg"+i_a]).isEVal())
							comElI.setAttr("arg"+i_a, comA.get());
				}
			}
		}
		
		//> End call
		if(curCom < 0 || curCom >= work.childSize())
		{
			//>> Stop all typical and call "stop" command
			var cL = comCntrO.nodeList("prm_");
			for(i_c = 0; i_c < cL.length; i_c++)
			{
				var cLi = comCntrO[cL[i_c]];
				cLi.run.set(false);
				cLi.start.set(false);
				cLi.stop.set(true);
			}

			curMode = mode = -2;
			SYS.messInfo("uprg"+prog,"Successful session of program \""+prog+"\" : "+SYS.strftime(startTm)+" : "+SYS.strftime(SYS.time()));

			//> Place to program last execution time
			rez = SYS.BD.nodeAt(dbDB,".").SQLReq("SELECT prgTxt FROM "+dbProgs+" WHERE name=''"+prog+"'';");
			if(rez.length > 1)
			{
				//> Parse programm and insert procedure text and argument''s labels to here
				comTree = SYS.XMLNode("prg");
				comTree.load(rez[1][0]);
				comTree.setAttr("wtm",SYS.time()-startTm);
				SYS.BD.nodeAt(dbDB,".").SQLReq("UPDATE "+dbProgs+" SET prgTxt=''"+comTree.save().replace("''","''''")+"'' WHERE name=''"+prog+"'';");
			}
		}
	}
}
mode = curMode;','clcCnt++;

if(f_start)	work = SYS.XMLNode("prg");

//> Check to commands controller present
comCntrO = false;
if(comsCntr.length)	comCntrO = SYS.DAQ.nodeAt(comsCntr,".");

//> Commands list update
if(f_start || !(clcCnt%(60*f_frq)))
{
	if(!comCntrO)
	{
		rez = SYS.BD.nodeAt(dbDB,".").SQLReq("SELECT * FROM "+dbComs+";");
		comLs = new Object();
		for(i_t = 1; i_t < rez.length; i_t++)
		{
			comNm = rez[i_t]["name"];
			comLs[comNm] = new Object();
			comLs[comNm]["descr"] = rez[i_t]["descr"];
			comLs[comNm]["proc"] = rez[i_t]["proc"];
			for(i_a = 1; i_a <= 5; i_a++)
				comLs[comNm]["arg"+i_a] = rez[i_t]["arg"+i_a];
		}
	}
	else
	{
		var nL = comCntrO.nodeList("prm_");
		comLs = new Object();
		for(i_n = 0; i_n < nL.length; i_n++)
		{
			comO = comCntrO[nL[i_n]];
			comNm = comO.cfg("NAME");
			comLs[comNm] = new Object();
			comLs[comNm]["prmID"] = nL[i_n].slice(4);
			comLs[comNm]["descr"] = comO.cfg("DESCR");
			for(i_a = 1; i_a <= 5; i_a++)
				if(!(comA=comO["arg"+i_a]).isEVal())
					comLs[comNm]["arg"+i_a] = comA.descr();
		}
	}
}

//> Programm selection change
if(curMode <= 0 && prog.length && (prog != work.attr("name") || mode == 1))
{
	work = SYS.XMLNode("prg");
	rez = SYS.BD.nodeAt(dbDB,".").SQLReq("SELECT prgTxt FROM "+dbProgs+" WHERE name=''"+prog+"'';");
	if(rez.length > 1)
	{
		//> Parse programm and insert procedure text and argument''s labels to here
		work.load(rez[1][0]);
		work.setAttr("name",prog);
		for(i_c = 0; i_c < work.childSize(); i_c++)
		{
			comId = work.childGet(i_c).attr("id");
			if(comLs[comId].isEVal())	continue;
			work.childGet(i_c).setAttr("proc",comLs[comId].proc);
			for(i_a = 1; i_a <= 5; i_a++)
				work.childGet(i_c).setAttr("labArg"+i_a,comLs[comId]["arg"+i_a]);
		}
	}
	else prog = "";
}

//> Start process
if(curMode <= 0 && mode == 1 && prog.length)
{
	startTm = SYS.time();
	curCom = 0;
	curMode = mode;	

	if(comCntrO)
	{
		var cL = comCntrO.nodeList("prm_");
		for(i_c = 0; i_c < cL.length; i_c++)
		{
			var cLi = comCntrO[cL[i_c]];
			cLi.run.set(false);
			cLi.stop.set(false);
			cLi.error.set(false);
			cLi.abort.set(false);
			cLi.start.set(true);
		}
	}
}
//> Pause and other process
else if((mode == 0 && curMode < 0) || (curMode == 1 && mode == 2) || (curMode == 2 && mode == 1))
{
	curMode = mode;
	if((curMode == 1 || curMode == 2) && comCntrO)
	{
		var cL = comCntrO.nodeList("prm_");
		for(i_c = 0; i_c < cL.length; i_c++)
			comCntrO[cL[i_c]].pause.set(curMode==2);
	}
}

//> Call program
if(curMode == 1 || curMode == 2)
{
	prog = work.attr("name");

	//> Internal commands call
	if(!comCntrO)
	{
		if(mode == 3 && curCom >= 0 && curCom < work.childSize())
		{
			work.childGet(curCom).setAttr("rez","-10:Шаг пропущен");
			curCom++;
		}
		if(curCom >= 0 && curCom < work.childSize())
		{
			comEl = work.childGet(curCom);
			//> Stop process
			if(mode == 0)
			{
				comEl.setAttr("rez","-10:Программа прервана");
				//>> Call stop command
				if(!comLs["Stop"].isEVal())
				{
					off = 0;
					prLang = comLs["Stop"].proc.parse(0,"\n",off);
					SYS.DAQ.funcCall(prLang,new Object(),comLs["Stop"].proc.slice(off));
				}
				SYS.messInfo("uprg"+prog,"Прерванный пользователем сеанс программы \""+prog+"\" : "+SYS.strftime(startTm)+" : "+SYS.strftime(SYS.time()));
				curMode = mode = -3;
			}
			//> Pass empty command		
			else if(!comEl.attr("proc").length) curCom++;
			//> Execute command
			else if(curMode == 1)
			{
				args = new Object();
				args.rez = "";
				args.f_start = !comEl.attr("tm").length;
				if(args.f_start) comEl.setAttr("tm",SYS.time());
				args.f_frq = f_frq;
				for(i_a = 1; i_a <= 5; i_a++) args["arg"+i_a] = comEl.attr("arg"+i_a);
				for(i_a = 1; i_a <= 10; i_a++) args["tmp"+i_a] = comEl.attr("tmp"+i_a);
				off = 0;
				prLang = comEl.attr("proc").parse(0,"\n",off);
				rez = SYS.DAQ.funcCall(prLang,args,comEl.attr("proc").slice(off));
				if(rez && args.rez.toInt() > 0) curCom++;
				else if(!rez || args.rez.toInt() < 0)
				{
					//>> Call error command
					if(!comLs["Error"].isEVal())
					{
						off = 0;
						prLang = comLs["Error"].proc.parse(0,"\n",off);
						SYS.DAQ.funcCall(prLang,new Object(),comLs["Error"].proc.slice(off));
					}
					SYS.messInfo("uprg"+prog,"Прерванный ошибкой сеанс программы \""+prog+"\" : "+SYS.strftime(startTm)+" : "+SYS.strftime(SYS.time()));
					curMode = mode = -1;
				}
				comEl.setAttr("rez",args.rez);
				for(i_a = 1; i_a <= 5; i_a++) comEl.setAttr("arg"+i_a, args["arg"+i_a]);
				for(i_a = 1; i_a <= 10; i_a++) comEl.setAttr("tmp"+i_a, args["tmp"+i_a]);
				//SYS.messDebug("TEST Calc","TEST Calc rezult: "+args.y);
			}
		}

		//> End call
		if(curCom < 0 || curCom >= work.childSize())
		{
			//>> Call stop command
			if(!comLs["Stop"].isEVal())
			{
				off = 0;
				prLang = comLs["Stop"].proc.parse(0,"\n",off);
				SYS.DAQ.funcCall(prLang,new Object(),comLs["Stop"].proc.slice(off));
			}

			curMode = mode = -2;
			SYS.messInfo("uprg"+prog,"Успешный сеанс программы \""+prog+"\" : "+SYS.strftime(startTm)+" : "+SYS.strftime(SYS.time()));

			//> Place to program last execution time
			rez = SYS.BD.nodeAt(dbDB,".").SQLReq("SELECT prgTxt FROM "+dbProgs+" WHERE name=''"+prog+"'';");
			if(rez.length > 1)
			{
				//> Parse programm and insert procedure text and argument''s labels to here
				comTree = SYS.XMLNode("prg");
				comTree.load(rez[1][0]);
				comTree.setAttr("wtm",SYS.time()-startTm);
				SYS.BD.nodeAt(dbDB,".").SQLReq("UPDATE "+dbProgs+" SET prgTxt=''"+comTree.save().replace("''","''''")+"'' WHERE name=''"+prog+"'';");
			}
		}
	}
	//> External commands call
	else
	{
		//>> Get current command parameter-object		
		curComPrm = EVAL_BOOL;
		if(curCom >= 0 && curCom < work.childSize())
		{
			if(!(comId=comLs[work.childGet(curCom).attr("id")]).isEVal())	curComPrm = comCntrO["prm_"+comId.prmID];
			if(curComPrm.isEVal())
			{
				curMode = mode = -1;
				work.childGet(curCom).setAttr("rez","-11:Отсутствует команда: "+work.childGet(curCom).attr("id"));
				return;
			}
		}

		if(mode == 3 && curCom >= 0 && curCom < work.childSize())
		{
			work.childGet(curCom).setAttr("rez","-10:Шаг пропущен");
			curComPrm.run.set(false);
			curCom++;
			mode = curMode;
			return;
		}
		if(curCom >= 0 && curCom < work.childSize())
		{
			comEl = work.childGet(curCom);
			//> Stop process
			if(mode == 0)
			{
				comEl.setAttr("rez","-12:Программа прервана");
				//>> Stop all typical and set "abort" flag
				var cL = comCntrO.nodeList("prm_");
				for(i_c = 0; i_c < cL.length; i_c++)
				{
					var cLi = comCntrO[cL[i_c]];
					cLi.run.set(false);
					cLi.start.set(false);
					cLi.abort.set(true);
				}

				SYS.messInfo("uprg"+prog,"Прерванный пользователем сеанс программы \""+prog+"\" : "+SYS.strftime(startTm)+" : "+SYS.strftime(SYS.time()));
				curMode = mode = -3;
			}
			//> Commands process
			else
			{
				//>> Start command
				if(!comEl.attr("tm").length)
				{
					curComPrm.run.set(0);	//> Stop for possible background call
					comEl.setAttr("tm",SYS.time());
					for(i_a = 1; i_a <= 5; i_a++)
						if(!(comA=curComPrm["arg"+i_a]).isEVal())
							comA.set(comEl.attr("arg"+i_a));
					curComPrm.rez.set(0);
					curComPrm.run.set(true);
				}

				//> Update steps status, up to current comand
				comTo = min(curCom+1,work.childSize());
				for(i_c = 0; i_c < comTo; i_c++)
				{
					comElI = work.childGet(i_c);
					curComPI = comCntrO["prm_"+comLs[comElI.attr("id")].prmID];
					if(i_c < curCom && !(curComPI.run.get() && 
						comElI.attr("rez").toInt() != 1 && comElI.attr("rez").toInt() > -10))	continue;
					rez = curComPI.rez.get();
					if(i_c == curCom && rez.toInt() > 0) curCom++;
					if(rez.toInt() < 0)
					{
						//>> Stop all typical and call "error" command
						var cL = comCntrO.nodeList("prm_");
						for(i_c = 0; i_c < cL.length; i_c++)
						{
							var cLi = comCntrO[cL[i_c]];
							cLi.run.set(false);
							cLi.start.set(false);
							cLi.error.set(true);
						}

						SYS.messInfo("uprg"+prog,"Прерванный ошибкой сеанс программы \""+prog+"\" : "+SYS.strftime(startTm)+" : "+SYS.strftime(SYS.time()));
						curMode = mode = -1;
					}
					comElI.setAttr("rez",rez);
					for(i_a = 1; i_a <= 5; i_a++)
						if(!(comA=curComPI["arg"+i_a]).isEVal())
							comElI.setAttr("arg"+i_a, comA.get());
				}
			}
		}
		
		//> End call
		if(curCom < 0 || curCom >= work.childSize())
		{
			//>> Stop all typical and call "stop" command
			var cL = comCntrO.nodeList("prm_");
			for(i_c = 0; i_c < cL.length; i_c++)
			{
				var cLi = comCntrO[cL[i_c]];
				cLi.run.set(false);
				cLi.start.set(false);
				cLi.stop.set(true);
			}

			curMode = mode = -2;
			SYS.messInfo("uprg"+prog,"Успешный сеанс программы \""+prog+"\" : "+SYS.strftime(startTm)+" : "+SYS.strftime(SYS.time()));

			//> Place to program last execution time
			rez = SYS.BD.nodeAt(dbDB,".").SQLReq("SELECT prgTxt FROM "+dbProgs+" WHERE name=''"+prog+"'';");
			if(rez.length > 1)
			{
				//> Parse programm and insert procedure text and argument''s labels to here
				comTree = SYS.XMLNode("prg");
				comTree.load(rez[1][0]);
				comTree.setAttr("wtm",SYS.time()-startTm);
				SYS.BD.nodeAt(dbDB,".").SQLReq("UPDATE "+dbProgs+" SET prgTxt=''"+comTree.save().replace("''","''''")+"'' WHERE name=''"+prog+"'';");
			}
		}
	}
}
mode = curMode;','clcCnt++;

if(f_start)	work = SYS.XMLNode("prg");

//> Check to commands controller present
comCntrO = false;
if(comsCntr.length)	comCntrO = SYS.DAQ.nodeAt(comsCntr,".");

//> Commands list update
if(f_start || !(clcCnt%(60*f_frq)))
{
	if(!comCntrO)
	{
		rez = SYS.BD.nodeAt(dbDB,".").SQLReq("SELECT * FROM "+dbComs+";");
		comLs = new Object();
		for(i_t = 1; i_t < rez.length; i_t++)
		{
			comNm = rez[i_t]["name"];
			comLs[comNm] = new Object();
			comLs[comNm]["descr"] = rez[i_t]["descr"];
			comLs[comNm]["proc"] = rez[i_t]["proc"];
			for(i_a = 1; i_a <= 5; i_a++)
				comLs[comNm]["arg"+i_a] = rez[i_t]["arg"+i_a];
		}
	}
	else
	{
		var nL = comCntrO.nodeList("prm_");
		comLs = new Object();
		for(i_n = 0; i_n < nL.length; i_n++)
		{
			comO = comCntrO[nL[i_n]];
			comNm = comO.cfg("NAME");
			comLs[comNm] = new Object();
			comLs[comNm]["prmID"] = nL[i_n].slice(4);
			comLs[comNm]["descr"] = comO.cfg("DESCR");
			for(i_a = 1; i_a <= 5; i_a++)
				if(!(comA=comO["arg"+i_a]).isEVal())
					comLs[comNm]["arg"+i_a] = comA.descr();
		}
	}
}

//> Programm selection change
if(curMode <= 0 && prog.length && (prog != work.attr("name") || mode == 1))
{
	work = SYS.XMLNode("prg");
	rez = SYS.BD.nodeAt(dbDB,".").SQLReq("SELECT prgTxt FROM "+dbProgs+" WHERE name=''"+prog+"'';");
	if(rez.length > 1)
	{
		//> Parse programm and insert procedure text and argument''s labels to here
		work.load(rez[1][0]);
		work.setAttr("name",prog);
		for(i_c = 0; i_c < work.childSize(); i_c++)
		{
			comId = work.childGet(i_c).attr("id");
			if(comLs[comId].isEVal())	continue;
			work.childGet(i_c).setAttr("proc",comLs[comId].proc);
			for(i_a = 1; i_a <= 5; i_a++)
				work.childGet(i_c).setAttr("labArg"+i_a,comLs[comId]["arg"+i_a]);
		}
	}
	else prog = "";
}

//> Start process
if(curMode <= 0 && mode == 1 && prog.length)
{
	startTm = SYS.time();
	curCom = 0;
	curMode = mode;	

	if(comCntrO)
	{
		var cL = comCntrO.nodeList("prm_");
		for(i_c = 0; i_c < cL.length; i_c++)
		{
			var cLi = comCntrO[cL[i_c]];
			cLi.run.set(false);
			cLi.stop.set(false);
			cLi.error.set(false);
			cLi.abort.set(false);
			cLi.start.set(true);
		}
	}
}
//> Pause and other process
else if((mode == 0 && curMode < 0) || (curMode == 1 && mode == 2) || (curMode == 2 && mode == 1))
{
	curMode = mode;
	if((curMode == 1 || curMode == 2) && comCntrO)
	{
		var cL = comCntrO.nodeList("prm_");
		for(i_c = 0; i_c < cL.length; i_c++)
			comCntrO[cL[i_c]].pause.set(curMode==2);
	}
}

//> Call program
if(curMode == 1 || curMode == 2)
{
	prog = work.attr("name");

	//> Internal commands call
	if(!comCntrO)
	{
		if(mode == 3 && curCom >= 0 && curCom < work.childSize())
		{
			work.childGet(curCom).setAttr("rez","-10:Крок пропущено");
			curCom++;
		}
		if(curCom >= 0 && curCom < work.childSize())
		{
			comEl = work.childGet(curCom);
			//> Stop process
			if(mode == 0)
			{
				comEl.setAttr("rez","-10:Програму перервано");
				//>> Call stop command
				if(!comLs["Stop"].isEVal())
				{
					off = 0;
					prLang = comLs["Stop"].proc.parse(0,"\n",off);
					SYS.DAQ.funcCall(prLang,new Object(),comLs["Stop"].proc.slice(off));
				}
				SYS.messInfo("uprg"+prog,"Перерваний користувачем сеанс програми \""+prog+"\" : "+SYS.strftime(startTm)+" : "+SYS.strftime(SYS.time()));
				curMode = mode = -3;
			}
			//> Pass empty command		
			else if(!comEl.attr("proc").length) curCom++;
			//> Execute command
			else if(curMode == 1)
			{
				args = new Object();
				args.rez = "";
				args.f_start = !comEl.attr("tm").length;
				if(args.f_start) comEl.setAttr("tm",SYS.time());
				args.f_frq = f_frq;
				for(i_a = 1; i_a <= 5; i_a++) args["arg"+i_a] = comEl.attr("arg"+i_a);
				for(i_a = 1; i_a <= 10; i_a++) args["tmp"+i_a] = comEl.attr("tmp"+i_a);
				off = 0;
				prLang = comEl.attr("proc").parse(0,"\n",off);
				rez = SYS.DAQ.funcCall(prLang,args,comEl.attr("proc").slice(off));
				if(rez && args.rez.toInt() > 0) curCom++;
				else if(!rez || args.rez.toInt() < 0)
				{
					//>> Call error command
					if(!comLs["Error"].isEVal())
					{
						off = 0;
						prLang = comLs["Error"].proc.parse(0,"\n",off);
						SYS.DAQ.funcCall(prLang,new Object(),comLs["Error"].proc.slice(off));
					}
					SYS.messInfo("uprg"+prog,"Перерваний помилкою сеанс програми \""+prog+"\" : "+SYS.strftime(startTm)+" : "+SYS.strftime(SYS.time()));
					curMode = mode = -1;
				}
				comEl.setAttr("rez",args.rez);
				for(i_a = 1; i_a <= 5; i_a++) comEl.setAttr("arg"+i_a, args["arg"+i_a]);
				for(i_a = 1; i_a <= 10; i_a++) comEl.setAttr("tmp"+i_a, args["tmp"+i_a]);
				//SYS.messDebug("TEST Calc","TEST Calc rezult: "+args.y);
			}
		}

		//> End call
		if(curCom < 0 || curCom >= work.childSize())
		{
			//>> Call stop command
			if(!comLs["Stop"].isEVal())
			{
				off = 0;
				prLang = comLs["Stop"].proc.parse(0,"\n",off);
				SYS.DAQ.funcCall(prLang,new Object(),comLs["Stop"].proc.slice(off));
			}

			curMode = mode = -2;
			SYS.messInfo("uprg"+prog,"Вдалий сеанс програми \""+prog+"\" : "+SYS.strftime(startTm)+" : "+SYS.strftime(SYS.time()));

			//> Place to program last execution time
			rez = SYS.BD.nodeAt(dbDB,".").SQLReq("SELECT prgTxt FROM "+dbProgs+" WHERE name=''"+prog+"'';");
			if(rez.length > 1)
			{
				//> Parse programm and insert procedure text and argument''s labels to here
				comTree = SYS.XMLNode("prg");
				comTree.load(rez[1][0]);
				comTree.setAttr("wtm",SYS.time()-startTm);
				SYS.BD.nodeAt(dbDB,".").SQLReq("UPDATE "+dbProgs+" SET prgTxt=''"+comTree.save().replace("''","''''")+"'' WHERE name=''"+prog+"'';");
			}
		}
	}
	//> External commands call
	else
	{
		//>> Get current command parameter-object		
		curComPrm = EVAL_BOOL;
		if(curCom >= 0 && curCom < work.childSize())
		{
			if(!(comId=comLs[work.childGet(curCom).attr("id")]).isEVal())	curComPrm = comCntrO["prm_"+comId.prmID];
			if(curComPrm.isEVal())
			{
				curMode = mode = -1;
				work.childGet(curCom).setAttr("rez","-11:Відсутня команда: "+work.childGet(curCom).attr("id"));
				return;
			}
		}

		if(mode == 3 && curCom >= 0 && curCom < work.childSize())
		{
			work.childGet(curCom).setAttr("rez","-10:Крок пропущено");
			curComPrm.run.set(false);
			curCom++;
			mode = curMode;
			return;
		}
		if(curCom >= 0 && curCom < work.childSize())
		{
			comEl = work.childGet(curCom);
			//> Stop process
			if(mode == 0)
			{
				comEl.setAttr("rez","-12:Програму перервано");
				//>> Stop all typical and set "abort" flag
				var cL = comCntrO.nodeList("prm_");
				for(i_c = 0; i_c < cL.length; i_c++)
				{
					var cLi = comCntrO[cL[i_c]];
					cLi.run.set(false);
					cLi.start.set(false);
					cLi.abort.set(true);
				}

				SYS.messInfo("uprg"+prog,"Перерваний користувачем сеанс програми \""+prog+"\" : "+SYS.strftime(startTm)+" : "+SYS.strftime(SYS.time()));
				curMode = mode = -3;
			}
			//> Commands process
			else
			{
				//>> Start command
				if(!comEl.attr("tm").length)
				{
					curComPrm.run.set(0);	//> Stop for possible background call
					comEl.setAttr("tm",SYS.time());
					for(i_a = 1; i_a <= 5; i_a++)
						if(!(comA=curComPrm["arg"+i_a]).isEVal())
							comA.set(comEl.attr("arg"+i_a));
					curComPrm.rez.set(0);
					curComPrm.run.set(true);
				}

				//> Update steps status, up to current comand
				comTo = min(curCom+1,work.childSize());
				for(i_c = 0; i_c < comTo; i_c++)
				{
					comElI = work.childGet(i_c);
					curComPI = comCntrO["prm_"+comLs[comElI.attr("id")].prmID];
					if(i_c < curCom && !(curComPI.run.get() && 
						comElI.attr("rez").toInt() != 1 && comElI.attr("rez").toInt() > -10))	continue;
					rez = curComPI.rez.get();
					if(i_c == curCom && rez.toInt() > 0) curCom++;
					if(rez.toInt() < 0)
					{
						//>> Stop all typical and call "error" command
						var cL = comCntrO.nodeList("prm_");
						for(i_c = 0; i_c < cL.length; i_c++)
						{
							var cLi = comCntrO[cL[i_c]];
							cLi.run.set(false);
							cLi.start.set(false);
							cLi.error.set(true);
						}

						SYS.messInfo("uprg"+prog,"Перерваний помилкою сеанс програми \""+prog+"\" : "+SYS.strftime(startTm)+" : "+SYS.strftime(SYS.time()));
						curMode = mode = -1;
					}
					comElI.setAttr("rez",rez);
					for(i_a = 1; i_a <= 5; i_a++)
						if(!(comA=curComPI["arg"+i_a]).isEVal())
							comElI.setAttr("arg"+i_a, comA.get());
				}
			}
		}
		
		//> End call
		if(curCom < 0 || curCom >= work.childSize())
		{
			//>> Stop all typical and call "stop" command
			var cL = comCntrO.nodeList("prm_");
			for(i_c = 0; i_c < cL.length; i_c++)
			{
				var cLi = comCntrO[cL[i_c]];
				cLi.run.set(false);
				cLi.start.set(false);
				cLi.stop.set(true);
			}
			curMode = mode = -2;
			SYS.messInfo("uprg"+prog,"Вдалий сеанс програми \""+prog+"\" : "+SYS.strftime(startTm)+" : "+SYS.strftime(SYS.time()));

			//> Place to program last execution time
			rez = SYS.BD.nodeAt(dbDB,".").SQLReq("SELECT prgTxt FROM "+dbProgs+" WHERE name=''"+prog+"'';");
			if(rez.length > 1)
			{
				//> Parse programm and insert procedure text and argument''s labels to here
				comTree = SYS.XMLNode("prg");
				comTree.load(rez[1][0]);
				comTree.setAttr("wtm",SYS.time()-startTm);
				SYS.BD.nodeAt(dbDB,".").SQLReq("UPDATE "+dbProgs+" SET prgTxt=''"+comTree.save().replace("''","''''")+"'' WHERE name=''"+prog+"'';");
			}
		}
	}
}
mode = curMode;',1372172656);
INSERT INTO "lib_Controllers" VALUES('test','test','','','','','',10,'using Special.FLibSYS;

out+=10;
if(out>100) out=0;

test = (10).toString(16,2,true);
test = (-10.34).toFixed(4,8);

//Request prepare
//req = SYS.XMLNode("#").setAttr("ProtIt","DCON").setAttr("addr",10);
//Send request
//SYS.Transport["Serial"]["out_TestDCON"].messIO(req,"UserProtocol");
//if(!req.attr("err").length) SYS.messDebug("TEST REQ","RES: "+req.text());

//test = "Text \n value[\041abc\x21].";

//test = arguments[4];
//arguments[4] = 10;

//test="Java123Script".search(new RegExp("script","i"));
//text = "Javascript3\"sdf\"3javaScript __ javascript";
//test = text.replace(4,3,"67");
//test = text.replace("3","55");
//test = text.replace(new RegExp("javascript","ig"),"JavaScript");//
//test = text.replace(new RegExp("\"([^\"]*)\"","g"),"``$1''''");
//text = "1 плюс 2 плюс 3";
//test = text.match("\\d+","g");
//text = "Посетите мою домашнюю страницу http://www.isp.com/~david";
//test = text.match("(\\w+):\\/\\/([\\w.]+)\\/(\\S*)");
//test = "1,2, 3 , 4 ,5".split(new RegExp("\\s*,\\s*"));
//test = "hello <b>world</b>".split(new RegExp("(<[^>]*>)"));
//var p = new RegExp("(\\d\\d)[-/](\\d\\d)[-/](\\d\\d(?:\\d\\d)?)","");
//test = p.test("abc12/30/1969def");

/*CRC = new Array(0,1,2,3,4,5);vSz = 3
hi=CRC[out%6];
test = hi;*/

//test = SYS.Transport.Sockets.out_testModBus.messIO("123456");

/*obj = new Object();
obj["test1"]=1;l
obj["test2"]=1;
for( var i in obj ) SYS.messDebug("test","Prop: "+i);
SYS.messDebug("test","Props test");*/

//vl = SYS.strFromCharCode(100,200,50,60);
//test = vl.charCodeAt(1);

//test = SYS.system("ps -Ao pid,comm");

//ModBus.Unidrive.pos.pos=out;
//ModBus.testRTU.test.pos=out;
//test = rand(offset*0.4);
//val="Test";
//if( val=="Test1" || val=="Test2" ) test="TestN"; else test="Test";

//test1=new Array(1,"a",2,"b",3,"c",4);
//test1[7]=new Array(5,6,7);
//test=test1[7][1];

//test1="ab;cde;fgh;jk;lm";
//test = test1.split(";").join();

//req = xmlNode("get").setAttr("path","/%2fgen%2fid");
//test = xmlCntrReq(req);
//test = req.text();

/*test1 = vArh("DAQ.System.AutoDA.CPULoad.load");
test2 = test1.FFT(0,60);
test = "";
for( i = 0; i < test2.length; i++ ) test += ""+test2[i]+";";*/

//messPut("cron",0,"Time: "+tmFStr(tmTime()));

/*req = SYS.XMLNode();
test = req.load("test1.xml",true);*/
//for( i = 0; i < req.childSize(); i++ )
//  test += req.childGet(i).attr("id")+"; ";
/*sutm = 0; stm = SYS.time(sutm);
req.save(0,"test2.xml");
eutm = 0; etm = SYS.time(eutm);
test = 1e6*(etm-stm)+(eutm-sutm);*/

//test = strDec4Bin(Transport.Sockets.out_testModBus.messIO(strEnc2Bin("15 01 00 00 00 06 01 03 00 00 00 05")));

/*req = SYS.XMLNode("GET");
req.setAttr("URI","/");
SYS.Transport.Sockets.out_testHTTP.messIO(req,"HTTP");
test = req.text();*/

/*req = SYS.XMLNode("POST");
req.setAttr("URI","/WebUser/FlowTec.txt");
cntNode = req.childAdd("cnt").setAttr("name","pole0").setAttr("filename","Object2-k001-100309-17.txt");
cntNode.childAdd("prm").setAttr("id","Content-Type").setText("text/plain");
cntText = "Object2-k001\r\n";
cntText += "\r\n";
cntText += "v002\r\n";
cntText += " n1\r\n";
cntText += "  09.03.10 16   Polnyj    7155.25    216.0  32.000  17.5\r\n";
cntText += "v005\r\n";
cntText += " n1\r\n";
cntText += "  09.03.10 16   Polnyj     188.81    350.0   4.000  40.0\r\n";
cntText += "\r\n";
cntNode.setText(cntText);
SYS.Transport.Sockets.out_testHTTP.messIO(req,"HTTP");*/

//test = SYS.UI.VCAEngine["ses_AGLKS"].alrmSndPlay();
//test = "1;2;5;7;9";
//test = "1;2;5;7;9".type();

//SYS.messDebug("test",strEnc2Bin("68 74 74 70 3a 2f 2f 77 77 77 2e 77 33 2e 6f 72 67 2f 32 30 30 30 2f 30 39 2f 78 6d 6c 64 73 69 67 23 72 73 61 2d 73 68 61 31"));

//test = SYS.DAQ.ModBus.testTCP.test.a_8_1_1_ПС_ОПД_стенд_температура__ОПД_.get();

/*req = SYS.XMLNode("get").setAttr("path","/%2fgen%2fid");
rez = SYS.cntrReq(req,"loop");
test = req.text();*/

/*DBTbl=SYS.BD.MySQL.GenDB.SQLReq("SELECT * from DB;");
for( var i_rw = 0; i_rw < DBTbl.length; i_rw++ )
{
  var rec = "";
  for( var i_fld = 0; i_fld < DBTbl[i_rw].length; i_fld++ )
    rec += DBTbl[i_rw][i_fld]+"\t";
  SYS.messDebug("TEST DB","Row "+i_rw+": "+rec);
}*/','','','');
INSERT INTO "lib_Controllers" VALUES('test1','test1','','','','','',10,'//clc=0;
//First getting previous time
//if(!p_tm){ p_tm = Special.FLibSYS.tmTime(); break; }

//Get curent time
//c_tm = Special.FLibSYS.tmTime()-2;

//Open value archive
//a_id = Special.FLibSYS.avalOpen("Archive.va_test");
//if(a_id==-1) break;

//Calc average archive
//for(i_tm=p_tm;i_tm<c_tm;i_tm++)
//{ 
//  val = Special.FLibSYS.avalGetR(a_id,i_tm,0,"");
//  rez = (rez*(i_tm-p_tm)+val)/(i_tm-p_tm+1);
//  clc++;
//}

//Close value archive
//Special.FLibSYS.avalClose(a_id);
//p_tm=c_tm;','','','');
CREATE TABLE 'tmplib_DevLib' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"MAXCALCTM" INTEGER DEFAULT '10' ,"PROGRAM" TEXT DEFAULT '' ,"uk#PROGRAM" TEXT DEFAULT '' ,"ru#PROGRAM" TEXT DEFAULT '' ,"TIMESTAMP" INTEGER DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO "tmplib_DevLib" VALUES('SCU750','','','','Typical EDWARDS TURBOMOLECULAR PUMPS data request by SCU750 Cotrol Unit protocol.','Опитування даних типових EDWARDS TURBOMOLECULAR PUMPS за допомогою протоколу контрольного модуля SCU750.','',10,'JavaLikeCalc.JavaScript
if(f_start)
{
	version = "";
	operModes = new Object();
	operModes[1] = "Levitation";
	operModes[2] = "No Levitation";
	operModes[3] = "Acceleration";
	operModes[4] = "Normal";
	operModes[5] = "Deceleration (Brake)";
	operModes[6] = "Autotest";
	operModes[7] = "Tuning";
	operModes[8] = "Tuning Complete";
}

tr = SYS.Transport.Serial.nodeAt("out_"+transport);
req = SYS.XMLNode("mess").setAttr("ProtIt","SCU750").setAttr("addr",addr);

//======= Commands =========
//> START, STOP and RESET process
if(comReset == true) req.setText(" E04");
else if(comStop == true) req.setText(" E02");
else if(comStart == true) req.setText(" E01");
else req.setText("");
if(req.text().length) { tr.messIO(req,"UserProtocol"); comReset=comStop=comStart=false; }

//> Speed setpoint set
if(spSpeed != spSpeedPrev)
{
	req.setText(" h"+spSpeed.toString(16,4));
	tr.messIO(req,"UserProtocol");
	spSpeedPrev = spSpeed;
}

//======= Data getting (by self bigger period) ========
if((cnt++)%max(1,perGet*f_frq))	return;
t_err = "";

//Read version, single
if(!version.length || version.isEVal())
{
	req.setText("?V");
	tr.messIO(req,"UserProtocol");
	if(req.attr("err").length) { t_err = req.attr("err"); version = EVAL_STR; }
	else
	{
		if(req.text().length != 42) version = EVAL_STR;
		else
		{
			CUSv = "";
			lstNoSp = 0;
			for(i_s = 2; i_s < 32; i_s+=2)
			{
				cod = req.text().slice(i_s,i_s+2).toInt(16);
				CUSv += SYS.strFromCharCode(cod);
				if(cod != 0x20) lstNoSp = i_s/2;
			}
			version = "Contr. Unit Soft V.="+CUSv.slice(0,lstNoSp)+"; "
					    "Motor Drive Soft V.="+req.text().charAt(35)+"."+req.text().charAt(36)+"; "
					    "DSP Soft V.="+req.text().charAt(39)+"."+req.text().charAt(40);
		}
	}
}

//Read rotational speed.
if(t_err.length) tTMS = tMotor = rotSpeed = EVAL_INT;
else
{
	req.setText("?[");
	tr.messIO(req,"UserProtocol");
	if(req.attr("err").length) { t_err = req.attr("err"); tTMS = tMotor = rotSpeed = EVAL_INT; }
	else
	{
		tTMS = req.text().slice(32,36).toInt(16);
		tMotor = req.text().slice(36,40).toInt(16);
		rotSpeed = req.text().slice(50,54).toInt(16);
	}
}

//Pump operation mode and errors read
if(t_err.length) { operMode = EVAL_INT; errors = EVAL_STR; }
else
{
	req.setText("?M");
	tr.messIO(req,"UserProtocol");
	if(req.attr("err").length) { t_err = req.attr("err"); operMode = EVAL_INT; errors = EVAL_STR; }
	else
	{
		oMode = req.text().slice(2,4).toInt(16);
		operMode = oMode.toString()+":"+(operModes[oMode].isEVal()?"Unknown":operModes[oMode]);
		//SYS.messDebug("CNTR","pumpOperMode: "+pumpOperMode);
		
		errCnt = req.text().slice(4,6).toInt(16);
		errors = "";
		for(i_e = 0; i_e < errCnt; i_e++)
			errors += req.text().slice(6+i_e*2,8+i_e*2)+"; ";
	}
}

//Events read
if(t_err.length)	events = EVAL_STR;
else
{
	req.setText("?g");
	tr.messIO(req,"UserProtocol");
	if(req.attr("err").length) { t_err = req.attr("err"); events = EVAL_STR; }
	else
	{
		errCnt = req.text().slice(2,4).toInt(16);
		events = "";
		for(i_e = 0; i_e < errCnt; i_e++)
			events += req.text().slice(4+i_e*2,6+i_e*2)+"; ";
	}
}

//Counters
if(t_err.length) { snCntrUnit = snPump = EVAL_STR; cntPumpHour = cntCntrUnitHour = cntStart = EVAL_REAL; }
else
{
	req.setText("?c");
	tr.messIO(req,"UserProtocol");
	if(req.attr("err").length) { t_err = req.attr("err"); snCntrUnit = snPump = EVAL_STR; cntPumpHour = cntCntrUnitHour = cntStart = EVAL_REAL; }
	else
	{
		snCntrUnit = req.text().slice(2,12);
		snPump = req.text().slice(12,22);
		cntPumpHour = req.text().slice(22,30).toInt(16);
		cntCntrUnitHour = req.text().slice(30,38).toInt(16);
		cntStart = req.text().slice(38,46).toInt(16);
	}
}

//Read setpoints
if(t_err.length)	spSpeed = spSpeedPrev = spTMSt = EVAL_INT;
else
{
	req.setText("?d");
	tr.messIO(req,"UserProtocol");
	if(req.attr("err").length) { t_err = req.attr("err"); spSpeed = spSpeedPrev = spTMSt = EVAL_INT; }
	else
	{
		spSpeed = spSpeedPrev = req.text().slice(2,6).toInt(16);
		spTMSt = req.text().slice(6,10).toInt(16);
	}
}

f_err = t_err;','JavaLikeCalc.JavaScript
if(f_start)
{
	version = "";
	operModes = new Object();
	operModes[1] = "Levitation";
	operModes[2] = "No Levitation";
	operModes[3] = "Acceleration";
	operModes[4] = "Normal";
	operModes[5] = "Deceleration (Brake)";
	operModes[6] = "Autotest";
	operModes[7] = "Tuning";
	operModes[8] = "Tuning Complete";
}

tr = SYS.Transport.Serial.nodeAt("out_"+transport);
req = SYS.XMLNode("mess").setAttr("ProtIt","SCU750").setAttr("addr",addr);

//======= Commands =========
//> START, STOP and RESET process
if(comReset == true) req.setText(" E04");
else if(comStop == true) req.setText(" E02");
else if(comStart == true) req.setText(" E01");
else req.setText("");
if(req.text().length) { tr.messIO(req,"UserProtocol"); comReset=comStop=comStart=false; }

//> Speed setpoint set
if(spSpeed != spSpeedPrev)
{
	req.setText(" h"+spSpeed.toString(16,4));
	tr.messIO(req,"UserProtocol");
	spSpeedPrev = spSpeed;
}

//======= Data getting (by self bigger period) ========
if((cnt++)%max(1,perGet*f_frq))	return;
t_err = "";

//Read version, single
if(!version.length || version.isEVal())
{
	req.setText("?V");
	tr.messIO(req,"UserProtocol");
	if(req.attr("err").length) { t_err = req.attr("err"); version = EVAL_STR; }
	else
	{
		if(req.text().length != 42) version = EVAL_STR;
		else
		{
			CUSv = "";
			lstNoSp = 0;
			for(i_s = 2; i_s < 32; i_s+=2)
			{
				cod = req.text().slice(i_s,i_s+2).toInt(16);
				CUSv += SYS.strFromCharCode(cod);
				if(cod != 0x20) lstNoSp = i_s/2;
			}
			version = "Contr. Unit Soft V.="+CUSv.slice(0,lstNoSp)+"; "
					    "Motor Drive Soft V.="+req.text().charAt(35)+"."+req.text().charAt(36)+"; "
					    "DSP Soft V.="+req.text().charAt(39)+"."+req.text().charAt(40);
		}
	}
}

//Read rotational speed.
if(t_err.length) tTMS = tMotor = rotSpeed = EVAL_INT;
else
{
	req.setText("?[");
	tr.messIO(req,"UserProtocol");
	if(req.attr("err").length) { t_err = req.attr("err"); tTMS = tMotor = rotSpeed = EVAL_INT; }
	else
	{
		tTMS = req.text().slice(32,36).toInt(16);
		tMotor = req.text().slice(36,40).toInt(16);
		rotSpeed = req.text().slice(50,54).toInt(16);
	}
}

//Pump operation mode and errors read
if(t_err.length) { operMode = EVAL_INT; errors = EVAL_STR; }
else
{
	req.setText("?M");
	tr.messIO(req,"UserProtocol");
	if(req.attr("err").length) { t_err = req.attr("err"); operMode = EVAL_INT; errors = EVAL_STR; }
	else
	{
		oMode = req.text().slice(2,4).toInt(16);
		operMode = oMode.toString()+":"+(operModes[oMode].isEVal()?"Unknown":operModes[oMode]);
		//SYS.messDebug("CNTR","pumpOperMode: "+pumpOperMode);
		
		errCnt = req.text().slice(4,6).toInt(16);
		errors = "";
		for(i_e = 0; i_e < errCnt; i_e++)
			errors += req.text().slice(6+i_e*2,8+i_e*2)+"; ";
	}
}

//Events read
if(t_err.length)	events = EVAL_STR;
else
{
	req.setText("?g");
	tr.messIO(req,"UserProtocol");
	if(req.attr("err").length) { t_err = req.attr("err"); events = EVAL_STR; }
	else
	{
		errCnt = req.text().slice(2,4).toInt(16);
		events = "";
		for(i_e = 0; i_e < errCnt; i_e++)
			events += req.text().slice(4+i_e*2,6+i_e*2)+"; ";
	}
}

//Counters
if(t_err.length) { snCntrUnit = snPump = EVAL_STR; cntPumpHour = cntCntrUnitHour = cntStart = EVAL_REAL; }
else
{
	req.setText("?c");
	tr.messIO(req,"UserProtocol");
	if(req.attr("err").length) { t_err = req.attr("err"); snCntrUnit = snPump = EVAL_STR; cntPumpHour = cntCntrUnitHour = cntStart = EVAL_REAL; }
	else
	{
		snCntrUnit = req.text().slice(2,12);
		snPump = req.text().slice(12,22);
		cntPumpHour = req.text().slice(22,30).toInt(16);
		cntCntrUnitHour = req.text().slice(30,38).toInt(16);
		cntStart = req.text().slice(38,46).toInt(16);
	}
}

//Read setpoints
if(t_err.length)	spSpeed = spSpeedPrev = spTMSt = EVAL_INT;
else
{
	req.setText("?d");
	tr.messIO(req,"UserProtocol");
	if(req.attr("err").length) { t_err = req.attr("err"); spSpeed = spSpeedPrev = spTMSt = EVAL_INT; }
	else
	{
		spSpeed = spSpeedPrev = req.text().slice(2,6).toInt(16);
		spTMSt = req.text().slice(6,10).toInt(16);
	}
}

f_err = t_err;','','');
INSERT INTO "tmplib_DevLib" VALUES('TMH','TMP-xx03','','','Power supply for turbomolecular pumps, model EI-R04M.','','',10,'JavaLikeCalc.JavaScript
tr = SYS.Transport.Serial.nodeAt("out_"+transport);
req = SYS.XMLNode("mess").setAttr("ProtIt","TMH").setAttr("addr",addr);

if(f_start)
{
	modelID = "";
	operModes = new Object();
	//Modes
	operModes["NS"] = "1:Levitation";
	operModes["NA"] = "3:Acceleration";
	operModes["NN"] = "4:Normal";
	operModes["NB"] = "5:Deceleration";
	operModes["FS"] = "6:Stop operation failure";
	operModes["FF"] = "7:Idling failure";
	operModes["FR"] = "8:Regeneration breaking failure";
	operModes["FB"] = "9:Deceleration failure";
	//Conrols
	operModes["LL"] = "Local";
	operModes["LR"] = "Remote";
	operModes["LC"] = "RS-232C";
	operModes["LD"] = "RS-485";

	//Set to //> Interactive
	req.setText("LN");
	tr.messIO(req,"UserProtocol");
}

//======= Commands =========
//> START, STOP and RESET process
if(comReset == true) req.setText("RR");
else if(comStop == true) req.setText("RP");
else if(comStart == true) req.setText("RT");
else req.setText("");
if(req.text().length)	{ tr.messIO(req,"UserProtocol"); comReset=comStop=comStart=false; }

//> Interactive, Autonomous
if(comInteract == true) req.setText("LN");
else if(comAutonom == true) req.setText("LF");
else req.setText("");
if(req.text().length)	{ tr.messIO(req,"UserProtocol"); comInteract=comAutonom=false; }

//======= Data getting (by self bigger period) ========
if((cnt++)%max(1,perGet*f_frq))	return;
t_err = "0";

//> Oper mode
req.setText("CS");
tr.messIO(req,"UserProtocol");
if(req.attr("err").length) { t_err = req.attr("err"); operMode = EVAL_STR; }
else operMode = /*req.text().slice(2)+":"+*/operModes[req.text().slice(0,2)];

//> Oper control
if(t_err.toInt())	operCntr = EVAL_STR;
else
{
	req.setText("LS");
	tr.messIO(req,"UserProtocol");
	if(req.attr("err").length) { t_err = req.attr("err"); operCntr = EVAL_STR; }
	else operCntr = operModes[req.text().slice(0,2)];
}

//> Alarms list obtain
if(t_err.toInt())	errors = EVAL_STR;
else
{
	errors = "";
	for(i = 0; true; i++)
	{
		req.setText("CF"+i.toString(10,2));
		tr.messIO(req,"UserProtocol");
		if(req.attr("err").length) { t_err = req.attr("err"); errors = EVAL_STR; break; }
		//SYS.messDebug("TMH","Result: "+req.text());
		if(req.text().slice(0,2) != "CA")	break;
		errors += req.text().slice(2)+";";
		continue;
	}
}

//> Get parameters
//>> Model ID
if(t_err.toInt())	modelID = EVAL_STR;
else if(!modelID.length || modelID == EVAL_STR)
{
	req.setText("PR01");
	tr.messIO(req,"UserProtocol");
	if(req.attr("err").length) { t_err = req.attr("err"); modelID = EVAL_STR;}
	else if(req.text().slice(0,2) != "PA") modelID = EVAL_STR;
	else modelID = "TMP-"+req.text().slice(4);
}

//>> Rotation speed
if(t_err.toInt())	rotSpeed = EVAL_INT;
else
{
	req.setText("PR03");
	tr.messIO(req,"UserProtocol");
	if(req.attr("err").length) { t_err = req.attr("err"); rotSpeed = EVAL_INT;}
	else if(req.text().slice(0,2) != "PA") rotSpeed = EVAL_INT;
	else rotSpeed = req.text().slice(4).toInt()*10;
}

//>> Current
if(t_err.toInt())	I = EVAL_REAL;
else
{
	req.setText("PR04");
	tr.messIO(req,"UserProtocol");
	if(req.attr("err").length) { t_err = req.attr("err"); I = EVAL_REAL;}
	else if(req.text().slice(0,2) != "PA") I = EVAL_REAL;
	else I = req.text().slice(4).toInt()/10;
}

//>> Axle 1 disbalance
if(t_err.toInt())	axle1disb = EVAL_INT;
else
{
	req.setText("PR21");
	tr.messIO(req,"UserProtocol");
	if(req.attr("err").length) { t_err = req.attr("err"); axle1disb = EVAL_INT;}
	else if(req.text().slice(0,2) != "PA") axle1disb = EVAL_INT;
	else axle1disb = req.text().slice(4).toInt();
}

//>> Axle 2 disbalance
if(t_err.toInt())	axle2disb = EVAL_INT;
else
{
	req.setText("PR22");
	tr.messIO(req,"UserProtocol");
	if(req.attr("err").length) { t_err = req.attr("err"); axle2disb = EVAL_INT;}
	else if(req.text().slice(0,2) != "PA") axle2disb = EVAL_INT;
	else axle2disb = req.text().slice(4).toInt();
}

//>> MP X1
if(t_err.toInt())	MP_X1 = EVAL_INT;
else
{
	req.setText("PR26");
	tr.messIO(req,"UserProtocol");
	if(req.attr("err").length) { t_err = req.attr("err"); MP_X1 = EVAL_INT;}
	else if(req.text().slice(0,2) != "PA") MP_X1 = EVAL_INT;
	else MP_X1 = req.text().slice(4).toInt();
}

//>> MP Y1
if(t_err.toInt())	MP_Y1 = EVAL_INT;
else
{
	req.setText("PR27");
	tr.messIO(req,"UserProtocol");
	if(req.attr("err").length) { t_err = req.attr("err"); MP_Y1 = EVAL_INT;}
	else if(req.text().slice(0,2) != "PA") MP_Y1 = EVAL_INT;
	else MP_Y1 = req.text().slice(4).toInt();
}

//>> MP X2
if(t_err.toInt())	MP_X2 = EVAL_INT;
else
{
	req.setText("PR28");
	tr.messIO(req,"UserProtocol");
	if(req.attr("err").length) { t_err = req.attr("err"); MP_X2 = EVAL_INT;}
	else if(req.text().slice(0,2) != "PA") MP_X2 = EVAL_INT;
	else MP_X2 = req.text().slice(4).toInt();
}

//>> MP Y2
if(t_err.toInt())	MP_Y2 = EVAL_INT;
else
{
	req.setText("PR29");
	tr.messIO(req,"UserProtocol");
	if(req.attr("err").length) { t_err = req.attr("err"); MP_Y2 = EVAL_INT;}
	else if(req.text().slice(0,2) != "PA") MP_Y2 = EVAL_INT;
	else MP_Y2 = req.text().slice(4).toInt();
}

//>> MP Z
if(t_err.toInt())	MP_Z = EVAL_INT;
else
{
	req.setText("PR30");
	tr.messIO(req,"UserProtocol");
	if(req.attr("err").length) { t_err = req.attr("err"); MP_Z = EVAL_INT;}
	else if(req.text().slice(0,2) != "PA") MP_Z = EVAL_INT;
	else MP_Z = req.text().slice(4).toInt();
}

f_err = t_err;','','','');
INSERT INTO "tmplib_DevLib" VALUES('TM510x','Elemer TM510x','','','Elemer TM5102 and TM5103.','','',10,'JavaLikeCalc.JavaScript
if(f_start)
{
	devTp = EVAL_INT;
}

tr = SYS.Transport.Serial.nodeAt("out_"+transport);
req = SYS.XMLNode("mess").setAttr("ProtIt","TM510x").setAttr("addr",addr);

t_err = "";

//> Device type get
if(devTp.isEVal() || !devTp)
{
	req.setText("0");
	tr.messIO(req,"UserProtocol");
	if(req.attr("err").length)	t_err = req.attr("err");
	else devTp = req.text().toInt();
}

//> Inputs read
errors = "";
if(t_err.length) in1 = in2 = in3 = in4 = in5 = in6 = in7 = in8 = EVAL_REAL;
else
{
	req.setText("6;0;8");
	tr.messIO(req,"UserProtocol");
	if(req.attr("err").length)
	{ t_err = req.attr("err"); in1 = in2 = in3 = in4 = in5 = in6 = in7 = in8 = EVAL_REAL; }
	else
	{
		off = 0;
		tvl = "";
		in1 = ((tvl=req.text().parse(0, ";", off))[0] == "$") ? EVAL_INT : tvl.toReal();
		errors += ((tvl[0] == "$") ? tvl.slice(1) : "0")+";";
		in2 = ((tvl=req.text().parse(0, ";", off))[0] == "$") ? EVAL_INT : tvl.toReal();
		errors += ((tvl[0] == "$") ? tvl.slice(1) : "0")+";";
		in3 = ((tvl=req.text().parse(0, ";", off))[0] == "$") ? EVAL_INT : tvl.toReal();
		errors += ((tvl[0] == "$") ? tvl.slice(1) : "0")+";";
		in4 = ((tvl=req.text().parse(0, ";", off))[0] == "$") ? EVAL_INT : tvl.toReal();
		errors += ((tvl[0] == "$") ? tvl.slice(1) : "0")+";";
		in5 = ((tvl=req.text().parse(0, ";", off))[0] == "$") ? EVAL_INT : tvl.toReal();
		errors += ((tvl[0] == "$") ? tvl.slice(1) : "0")+";";
		in6 = ((tvl=req.text().parse(0, ";", off))[0] == "$") ? EVAL_INT : tvl.toReal();
		errors += ((tvl[0] == "$") ? tvl.slice(1) : "0")+";";
		in7 = ((tvl=req.text().parse(0, ";", off))[0] == "$") ? EVAL_INT : tvl.toReal();
		errors += ((tvl[0] == "$") ? tvl.slice(1) : "0")+";";
		in8 = ((tvl=req.text().parse(0, ";", off))[0] == "$") ? EVAL_INT : tvl.toReal();
		errors += ((tvl[0] == "$") ? tvl.slice(1) : "0")+";";
	}
}

f_err = t_err;','','','');
INSERT INTO "tmplib_DevLib" VALUES('VKT7','','VKT-7','','Firm "Teplocom" (http://www.teplocom.spb.ru) computer "VKT-7", St.Peterburg.','Firm "Teplocom" (http://www.teplocom.spb.ru) computer "VKT-7", St.Peterburg.','',10,'JavaLikeCalc.JavaScript
t_err = "";
//> Set transport and RTS
if(f_start){ tr = SYS.nodeAt(trAddr,"."); tr.TS(true); valsPrec = false; valsSz = false; }
if(!tr)	t_err = "1:Transport ''"+trAddr+"'' error.";

req = SYS.XMLNode("mess").setAttr("ProtIt","VKT7").setAttr("addr",0);

//> Session start
if(!t_err.length)
{
	req.setText(SYS.strFromCharCode(0x10, 0x3F, 0xFF, 0x00, 0x00, 0xCC, 0x80, 0x00, 0x00, 0x00));
	tr.messIO(req,"UserProtocol");
	t_err = req.attr("err");
}

//> Request for service information
if(!t_err.length && (f_start || firmVer.isEVal()))
{
	req.setText(SYS.strFromCharCode(0x03, 0x3f, 0xf9, 0x00, 0x00));
	tr.messIO(req,"UserProtocol");
	t_err = req.attr("err");
	if(!t_err.length)
	{
		rez = req.text();
		firmVer = (rez.charCodeAt(2)>>4)+(rez.charCodeAt(2)&0x0F)/10;
		abonId = rez.slice(7,7+8);
		netNum = rez.charCodeAt(15);
		modelImpl = rez.charCodeAt(17);
	}
}

//> Request for dimensions and precisions
if(!t_err.length && (f_start || !valsPrec))
{
	req.setText(SYS.strFromCharCode(0x10, 0x3F, 0xFF, 0x00, 0x00, 0x60,
		0x2C, 0x00, 0x00, 0x40, 0x07, 0x00,	//tTypeM
		0x2D, 0x00, 0x00, 0x40, 0x07, 0x00,	//GTypeM
		0x2E, 0x00, 0x00, 0x40, 0x07, 0x00,	//VTypeM
		0x2F, 0x00, 0x00, 0x40, 0x07, 0x00,	//MTypeM
		0x30, 0x00, 0x00, 0x40, 0x07, 0x00,	//PTypeM
		0x35, 0x00, 0x00, 0x40, 0x07, 0x00,	//QoTypeM
		0x37, 0x00, 0x00, 0x40, 0x07, 0x00,	//QntTypeHIM
		0x38, 0x00, 0x00, 0x40, 0x07, 0x00,	//QntTypeM
		0x39, 0x00, 0x00, 0x40, 0x01, 0x00,	//tTypeFractDiNum
		0x3B, 0x00, 0x00, 0x40, 0x01, 0x00,	//VTypeFractDigNum1
		0x3C, 0x00, 0x00, 0x40, 0x01, 0x00,	//MTypeFractDigNum1
		0x3D, 0x00, 0x00, 0x40, 0x01, 0x00,	//PTypeFractDigNum1
		0x3E, 0x00, 0x00, 0x40, 0x01, 0x00,	//dtTypeFractDigNum1
		0x42, 0x00, 0x00, 0x40, 0x01, 0x00,	//QoTypeFractDigNum1
		0x46, 0x00, 0x00, 0x40, 0x01, 0x00,	//MTypeFractDigNum2
		0x45, 0x00, 0x00, 0x40, 0x01, 0x00,	//VTypeFractDigNum2
		0x4C, 0x00, 0x00, 0x40, 0x01, 0x00));	//QoTypeFractDigNum2
	tr.messIO(req,"UserProtocol");
	t_err = req.attr("err");
	if(!t_err.length)
	{
		req.setText(SYS.strFromCharCode(0x03, 0x3F, 0xFE, 0x00, 0x00));
		tr.messIO(req,"UserProtocol");
		t_err = req.attr("err");
		if(!t_err.length)
		{
			rez = req.text();	off = 2;
			vLen = (rez.charCodeAt(off+1)<<8)|rez.charCodeAt(off);
			tTypeM = SYS.strCodeConv(rez.slice(off+2,off+2+vLen),"CP866","");	off += vLen+4;
			vLen = (rez.charCodeAt(off+1)<<8)|rez.charCodeAt(off);
			GTypeM = SYS.strCodeConv(rez.slice(off+2,off+2+vLen),"CP866","");	off += vLen+4;
			vLen = (rez.charCodeAt(off+1)<<8)|rez.charCodeAt(off);
			VTypeM = SYS.strCodeConv(rez.slice(off+2,off+2+vLen),"CP866","");	off += vLen+4;
			vLen = (rez.charCodeAt(off+1)<<8)|rez.charCodeAt(off);
			MTypeM = SYS.strCodeConv(rez.slice(off+2,off+2+vLen),"CP866","");	off += vLen+4;
			vLen = (rez.charCodeAt(off+1)<<8)|rez.charCodeAt(off);
			PTypeM = SYS.strCodeConv(rez.slice(off+2,off+2+vLen),"CP866","");	off += vLen+4;
			vLen = (rez.charCodeAt(off+1)<<8)|rez.charCodeAt(off);
			QoTypeM = SYS.strCodeConv(rez.slice(off+2,off+2+vLen),"CP866","");	off += vLen+4;
			vLen = (rez.charCodeAt(off+1)<<8)|rez.charCodeAt(off);
			QntTypeHIM = SYS.strCodeConv(rez.slice(off+2,off+2+vLen),"CP866","");	off += vLen+4;
			vLen = (rez.charCodeAt(off+1)<<8)|rez.charCodeAt(off);
			QntTypeM = SYS.strCodeConv(rez.slice(off+2,off+2+vLen),"CP866","");	off += vLen+4;
			valsPrec = new Object();
			valsPrec["tTypeFractDiNum"] = rez.charCodeAt(off);	off += 3;
			valsPrec["VTypeFractDigNum1"] = rez.charCodeAt(off);	off += 3;
			valsPrec["MTypeFractDigNum1"] = rez.charCodeAt(off);	off += 3;
			valsPrec["PTypeFractDigNum1"] = rez.charCodeAt(off);	off += 3;
			valsPrec["dtTypeFractDigNum1"] = rez.charCodeAt(off);	off += 3;
			valsPrec["QoTypeFractDigNum1"] = rez.charCodeAt(off);	off += 3;
			valsPrec["MTypeFractDigNum2"] = rez.charCodeAt(off);	off += 3;
			valsPrec["VTypeFractDigNum2"] = rez.charCodeAt(off);	off += 3;
			valsPrec["QoTypeFractDigNum2"] = rez.charCodeAt(off);	off += 3;
		}
	}
}

//> Request for values present list and size.
if(!t_err.length && (f_start || !valsSz || !valsSz.length))
{
	req.setText(SYS.strFromCharCode(0x03, 0x3f, 0xfc, 0x00, 0x00));
	tr.messIO(req,"UserProtocol");
	t_err = req.attr("err");
	if(!t_err.length)
	{
		rez = req.text();
		valsSz = new Array();
		for(off = 2; off < rez.length; off += 6)
			valsSz[rez.charCodeAt(off)] = rez.charCodeAt(off+4);
	}
}

//> Readed items list write
if(!t_err.length)
{
	data = "";
	itId = 0;		//t1(Тв1)
	if(!valsSz[itId].isEVal())	data += SYS.strFromCharCode(itId, 0x00, 0x00, 0x40, valsSz[itId], 0x00);
	itId = 1;		//t2(Тв1)
	if(!valsSz[itId].isEVal())	data += SYS.strFromCharCode(itId, 0x00, 0x00, 0x40, valsSz[itId], 0x00);
	itId = 14;	//dt(Тв1)
	if(!valsSz[itId].isEVal())	data += SYS.strFromCharCode(itId, 0x00, 0x00, 0x40, valsSz[itId], 0x00);
	if(!data.length)	{ t1_1 = t2_1 = dt_1 = EVAL_REAL; f_err = "10:Data list for request empty."; return; }
	req.setText(SYS.strFromCharCode(0x10, 0x3f, 0xff, 0x00, 0x00, data.length)+data);
	//SYS.messInfo("TMPL","SetDataList: "+Special.FLibSYS.strDec4Bin(req.text()));
	tr.messIO(req,"UserProtocol");
	t_err = req.attr("err");
}

//> Request for curent values (4)
if(!t_err.length)
{
	req.setText(SYS.strFromCharCode(0x10, 0x3f, 0xfd, 0x00, 0x00, 0x02, 0x04, 0x00));	
	tr.messIO(req,"UserProtocol");
	t_err = req.attr("err");
}

//> Request for data read
if(!t_err.length)
{
	req.setText(SYS.strFromCharCode(0x03, 0x3F, 0xFE, 0x00, 0x00));
	tr.messIO(req,"UserProtocol");
	t_err = req.attr("err");
	if(!t_err.length)
	{
		rez = req.text(); off = 2; errAttrs = "";
		vSz = 0;	//Observing: If init without it by "vSz = valsSz[itId];" then next operaion "off += vSz+2;" is wrong!!
		itId = 0;	vSz = valsSz[itId];	//t1(Тв1)
		//SYS.messInfo("TMPL","off0="+off+"; vSz="+vSz+"; typeof(2)="+typeof(2));
		if(!vSz.isEVal())
		{
			for(pVal = 0, i_b = 0; i_b < vSz; i_b++)	pVal = pVal | rez.charCodeAt(off+i_b)<<(i_b*8);
			if(rez.charCodeAt(off+vSz)==0xC0)	t1_1 = pVal/pow(10,valsPrec["tTypeFractDiNum"]);
			else { t1_1 = EVAL_REAL; errAttrs += ""+itId+"="+rez.charCodeAt(off+vSz).toString(16)+"; "; }
			off += vSz+2;
		}
		itId = 1;	vSz = valsSz[itId];	//t2(Тв1)
		//SYS.messInfo("TMPL","off1="+off+"; vSz="+vSz);
		if(!vSz.isEVal())
		{
			for(pVal = 0, i_b = 0; i_b < vSz; i_b++)	pVal = pVal | rez.charCodeAt(off+i_b)<<(i_b*8);
			if(rez.charCodeAt(off+vSz)==0xC0)	t2_1 = pVal/pow(10,valsPrec["tTypeFractDiNum"]);
			else { t2_1 = EVAL_REAL; errAttrs += ""+itId+"="+rez.charCodeAt(off+vSz).toString(16)+"; "; }
			off += vSz+2;
		}
		itId = 14;	vSz = valsSz[itId];	//dt(Тв1)
		//SYS.messInfo("TMPL","off2="+off+"; vSz="+vSz);
		if(!vSz.isEVal())
		{
			for(pVal = 0, i_b = 0; i_b < vSz; i_b++)	pVal = pVal | rez.charCodeAt(off+i_b)<<(i_b*8);
			if(pVal&0x8000)	pVal -= 0x10000;
			if(rez.charCodeAt(off+vSz)==0xC0)	dt_1 = pVal/pow(10,valsPrec["dtTypeFractDigNum1"]);
			else { dt_1 = EVAL_REAL; errAttrs += ""+itId+"="+rez.charCodeAt(off+vSz).toString(16)+"; "; }
			off += vSz+2;
		}
	}
}

SYS.messInfo("TMPL","ServResp: "+Special.FLibSYS.strDec4Bin(rez));

if(t_err.length)
{
	f_err = t_err;
	t1_1 = t2_1 = dt_1 = EVAL_REAL;
}
else f_err = errAttrs.length ? "11:Quality errors: "+errAttrs : "0";','JavaLikeCalc.JavaScript
t_err = "";
//> Set transport and RTS
if(f_start){ tr = SYS.nodeAt(trAddr,"."); tr.TS(true); valsPrec = false; valsSz = false; }
if(!tr)	t_err = "1:Transport ''"+trAddr+"'' error.";

req = SYS.XMLNode("mess").setAttr("ProtIt","VKT7").setAttr("addr",0);

//> Session start
if(!t_err.length)
{
	req.setText(SYS.strFromCharCode(0x10, 0x3F, 0xFF, 0x00, 0x00, 0xCC, 0x80, 0x00, 0x00, 0x00));
	tr.messIO(req,"UserProtocol");
	t_err = req.attr("err");
}

//> Request for service information
if(!t_err.length && (f_start || firmVer.isEVal()))
{
	req.setText(SYS.strFromCharCode(0x03, 0x3f, 0xf9, 0x00, 0x00));
	tr.messIO(req,"UserProtocol");
	t_err = req.attr("err");
	if(!t_err.length)
	{
		rez = req.text();
		firmVer = (rez.charCodeAt(2)>>4)+(rez.charCodeAt(2)&0x0F)/10;
		abonId = rez.slice(7,7+8);
		netNum = rez.charCodeAt(15);
		modelImpl = rez.charCodeAt(17);
	}
}

//> Request for dimensions and precisions
if(!t_err.length && (f_start || !valsPrec))
{
	req.setText(SYS.strFromCharCode(0x10, 0x3F, 0xFF, 0x00, 0x00, 0x60,
		0x2C, 0x00, 0x00, 0x40, 0x07, 0x00,	//tTypeM
		0x2D, 0x00, 0x00, 0x40, 0x07, 0x00,	//GTypeM
		0x2E, 0x00, 0x00, 0x40, 0x07, 0x00,	//VTypeM
		0x2F, 0x00, 0x00, 0x40, 0x07, 0x00,	//MTypeM
		0x30, 0x00, 0x00, 0x40, 0x07, 0x00,	//PTypeM
		0x35, 0x00, 0x00, 0x40, 0x07, 0x00,	//QoTypeM
		0x37, 0x00, 0x00, 0x40, 0x07, 0x00,	//QntTypeHIM
		0x38, 0x00, 0x00, 0x40, 0x07, 0x00,	//QntTypeM
		0x39, 0x00, 0x00, 0x40, 0x01, 0x00,	//tTypeFractDiNum
		0x3B, 0x00, 0x00, 0x40, 0x01, 0x00,	//VTypeFractDigNum1
		0x3C, 0x00, 0x00, 0x40, 0x01, 0x00,	//MTypeFractDigNum1
		0x3D, 0x00, 0x00, 0x40, 0x01, 0x00,	//PTypeFractDigNum1
		0x3E, 0x00, 0x00, 0x40, 0x01, 0x00,	//dtTypeFractDigNum1
		0x42, 0x00, 0x00, 0x40, 0x01, 0x00,	//QoTypeFractDigNum1
		0x46, 0x00, 0x00, 0x40, 0x01, 0x00,	//MTypeFractDigNum2
		0x45, 0x00, 0x00, 0x40, 0x01, 0x00,	//VTypeFractDigNum2
		0x4C, 0x00, 0x00, 0x40, 0x01, 0x00));	//QoTypeFractDigNum2
	tr.messIO(req,"UserProtocol");
	t_err = req.attr("err");
	if(!t_err.length)
	{
		req.setText(SYS.strFromCharCode(0x03, 0x3F, 0xFE, 0x00, 0x00));
		tr.messIO(req,"UserProtocol");
		t_err = req.attr("err");
		if(!t_err.length)
		{
			rez = req.text();	off = 2;
			vLen = (rez.charCodeAt(off+1)<<8)|rez.charCodeAt(off);
			tTypeM = SYS.strCodeConv(rez.slice(off+2,off+2+vLen),"CP866","");	off += vLen+4;
			vLen = (rez.charCodeAt(off+1)<<8)|rez.charCodeAt(off);
			GTypeM = SYS.strCodeConv(rez.slice(off+2,off+2+vLen),"CP866","");	off += vLen+4;
			vLen = (rez.charCodeAt(off+1)<<8)|rez.charCodeAt(off);
			VTypeM = SYS.strCodeConv(rez.slice(off+2,off+2+vLen),"CP866","");	off += vLen+4;
			vLen = (rez.charCodeAt(off+1)<<8)|rez.charCodeAt(off);
			MTypeM = SYS.strCodeConv(rez.slice(off+2,off+2+vLen),"CP866","");	off += vLen+4;
			vLen = (rez.charCodeAt(off+1)<<8)|rez.charCodeAt(off);
			PTypeM = SYS.strCodeConv(rez.slice(off+2,off+2+vLen),"CP866","");	off += vLen+4;
			vLen = (rez.charCodeAt(off+1)<<8)|rez.charCodeAt(off);
			QoTypeM = SYS.strCodeConv(rez.slice(off+2,off+2+vLen),"CP866","");	off += vLen+4;
			vLen = (rez.charCodeAt(off+1)<<8)|rez.charCodeAt(off);
			QntTypeHIM = SYS.strCodeConv(rez.slice(off+2,off+2+vLen),"CP866","");	off += vLen+4;
			vLen = (rez.charCodeAt(off+1)<<8)|rez.charCodeAt(off);
			QntTypeM = SYS.strCodeConv(rez.slice(off+2,off+2+vLen),"CP866","");	off += vLen+4;
			valsPrec = new Object();
			valsPrec["tTypeFractDiNum"] = rez.charCodeAt(off);	off += 3;
			valsPrec["VTypeFractDigNum1"] = rez.charCodeAt(off);	off += 3;
			valsPrec["MTypeFractDigNum1"] = rez.charCodeAt(off);	off += 3;
			valsPrec["PTypeFractDigNum1"] = rez.charCodeAt(off);	off += 3;
			valsPrec["dtTypeFractDigNum1"] = rez.charCodeAt(off);	off += 3;
			valsPrec["QoTypeFractDigNum1"] = rez.charCodeAt(off);	off += 3;
			valsPrec["MTypeFractDigNum2"] = rez.charCodeAt(off);	off += 3;
			valsPrec["VTypeFractDigNum2"] = rez.charCodeAt(off);	off += 3;
			valsPrec["QoTypeFractDigNum2"] = rez.charCodeAt(off);	off += 3;
		}
	}
}

//> Request for values present list and size.
if(!t_err.length && (f_start || !valsSz || !valsSz.length))
{
	req.setText(SYS.strFromCharCode(0x03, 0x3f, 0xfc, 0x00, 0x00));
	tr.messIO(req,"UserProtocol");
	t_err = req.attr("err");
	if(!t_err.length)
	{
		rez = req.text();
		valsSz = new Array();
		for(off = 2; off < rez.length; off += 6)
			valsSz[rez.charCodeAt(off)] = rez.charCodeAt(off+4);
	}
}

//> Readed items list write
if(!t_err.length)
{
	data = "";
	itId = 0;		//t1(Тв1)
	if(!valsSz[itId].isEVal())	data += SYS.strFromCharCode(itId, 0x00, 0x00, 0x40, valsSz[itId], 0x00);
	itId = 1;		//t2(Тв1)
	if(!valsSz[itId].isEVal())	data += SYS.strFromCharCode(itId, 0x00, 0x00, 0x40, valsSz[itId], 0x00);
	itId = 14;	//dt(Тв1)
	if(!valsSz[itId].isEVal())	data += SYS.strFromCharCode(itId, 0x00, 0x00, 0x40, valsSz[itId], 0x00);
	if(!data.length)	{ t1_1 = t2_1 = dt_1 = EVAL_REAL; f_err = "10:Data list for request empty."; return; }
	req.setText(SYS.strFromCharCode(0x10, 0x3f, 0xff, 0x00, 0x00, data.length)+data);
	//SYS.messInfo("TMPL","SetDataList: "+Special.FLibSYS.strDec4Bin(req.text()));
	tr.messIO(req,"UserProtocol");
	t_err = req.attr("err");
}

//> Request for curent values (4)
if(!t_err.length)
{
	req.setText(SYS.strFromCharCode(0x10, 0x3f, 0xfd, 0x00, 0x00, 0x02, 0x04, 0x00));	
	tr.messIO(req,"UserProtocol");
	t_err = req.attr("err");
}

//> Request for data read
if(!t_err.length)
{
	req.setText(SYS.strFromCharCode(0x03, 0x3F, 0xFE, 0x00, 0x00));
	tr.messIO(req,"UserProtocol");
	t_err = req.attr("err");
	if(!t_err.length)
	{
		rez = req.text(); off = 2; errAttrs = "";
		vSz = 0;	//Observing: If init without it by "vSz = valsSz[itId];" then next operaion "off += vSz+2;" is wrong!!
		itId = 0;	vSz = valsSz[itId];	//t1(Тв1)
		//SYS.messInfo("TMPL","off0="+off+"; vSz="+vSz+"; typeof(2)="+typeof(2));
		if(!vSz.isEVal())
		{
			for(pVal = 0, i_b = 0; i_b < vSz; i_b++)	pVal = pVal | rez.charCodeAt(off+i_b)<<(i_b*8);
			if(rez.charCodeAt(off+vSz)==0xC0)	t1_1 = pVal/pow(10,valsPrec["tTypeFractDiNum"]);
			else { t1_1 = EVAL_REAL; errAttrs += ""+itId+"="+rez.charCodeAt(off+vSz).toString(16)+"; "; }
			off += vSz+2;
		}
		itId = 1;	vSz = valsSz[itId];	//t2(Тв1)
		//SYS.messInfo("TMPL","off1="+off+"; vSz="+vSz);
		if(!vSz.isEVal())
		{
			for(pVal = 0, i_b = 0; i_b < vSz; i_b++)	pVal = pVal | rez.charCodeAt(off+i_b)<<(i_b*8);
			if(rez.charCodeAt(off+vSz)==0xC0)	t2_1 = pVal/pow(10,valsPrec["tTypeFractDiNum"]);
			else { t2_1 = EVAL_REAL; errAttrs += ""+itId+"="+rez.charCodeAt(off+vSz).toString(16)+"; "; }
			off += vSz+2;
		}
		itId = 14;	vSz = valsSz[itId];	//dt(Тв1)
		//SYS.messInfo("TMPL","off2="+off+"; vSz="+vSz);
		if(!vSz.isEVal())
		{
			for(pVal = 0, i_b = 0; i_b < vSz; i_b++)	pVal = pVal | rez.charCodeAt(off+i_b)<<(i_b*8);
			if(pVal&0x8000)	pVal -= 0x10000;
			if(rez.charCodeAt(off+vSz)==0xC0)	dt_1 = pVal/pow(10,valsPrec["dtTypeFractDigNum1"]);
			else { dt_1 = EVAL_REAL; errAttrs += ""+itId+"="+rez.charCodeAt(off+vSz).toString(16)+"; "; }
			off += vSz+2;
		}
	}
}

SYS.messInfo("TMPL","ServResp: "+Special.FLibSYS.strDec4Bin(rez));

if(t_err.length)
{
	f_err = t_err;
	t1_1 = t2_1 = dt_1 = EVAL_REAL;
}
else f_err = errAttrs.length ? "11:Quality errors: "+errAttrs : "0";','','');
CREATE TABLE 'flb_web' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"MAXCALCTM" INTEGER DEFAULT '10' ,"FORMULA" TEXT DEFAULT '' ,"TIMESTAMP" INTEGER DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO "flb_web" VALUES('alarms','Alarms','',10,'//> Load rules
var alarmsTree = SYS.XMLNode("ALARMS");
alarmsTree.load(rules);
for(i_rl = 0; i_rl < alarmsTree.childSize(); i_rl++)
{
	if(!alarmsTree.childGet(i_rl).text().length) continue;
	var subId = alarmsTree.childGet(i_rl).attr("id").parse(0,".");
	var devId =  alarmsTree.childGet(i_rl).attr("id").parse(1,".");
	var curAttrVal = SYS.DAQ.ModBus.ZegBee[subId][devId].get();
	var curErr = "";
	if(curAttrVal.isEVal()) curErr = "Value error. Perhaps the device is missing.";
	else
	{
		var progArgs = new Object();
		progArgs.err = "";
		progArgs.x = curAttrVal;
		SYS.DAQ.funcCall("JavaLikeCalc.JavaScript",progArgs,alarmsTree.childGet(i_rl).text());
		if(progArgs.err.length) curErr = progArgs.err;
	}
	//> Create alarm
	if(curErr.length && curErr != alarmsTree.childGet(i_rl).attr("err"))
	{
		SYS.message("ALARM:House:"+alarmsTree.childGet(i_rl).attr("id"),-4,curErr);
		//>> Send SMS for new alarm messages to users
		var smsReq = SYS.XMLNode("send").setAttr("ProtIt","SMS").setText(alarmsTree.childGet(i_rl).attr("id")+": "+curErr);
		var userLst = SYS.XMLNode("get").setAttr("path","/sub_Security/grp_Web/%2fprm%2fusers");
		SYS.cntrReq(userLst);
		for(var i = 0; i < userLst.childSize(); i++)
		{
			var isSelAdmin = SYS.Security.access(userLst.childGet(i).text(),4,"root","WebRoot",0440);
			var treeEl = SYS.XMLNode("get").setAttr("path","/sub_Security/usr_"+userLst.childGet(i).text()+"/%2fprm%2fldscr");
			SYS.cntrReq(treeEl);
			var re = treeEl.text().match("SMS:\\s+([^\\n]*)");
			if(!re.length || re[1] != "true") continue;
			if(!isSelAdmin)
			{
				re=treeEl.text().match("sub_"+subId+":\\s+([^\\n]*)");
				if(!re.length || re[1] == "--") continue;
			}
			re = treeEl.text().match("TEL:\\s+([^\\n]*)");
			if(!re.length) continue;
			var telLs = re[1].split(new RegExp("\\s+"));
			for(var i_t = 0; i_t < telLs.length; i_t++)
			{
				smsReq.setAttr("tel",telLs[i_t]);
				SYS.Transport.Serial.out_GSM.messIO(smsReq,"UserProtocol");
				if(smsReq.attr("err") != "0") SYS.messWarning("SMS send",smsReq.attr("err"));
			}
		}
	}
	//> Clear alarm
	else if(!curErr.length && !alarmsTree.childGet(i_rl).attr("err").isEVal())
		SYS.message("ALARM:House:"+alarmsTree.childGet(i_rl).attr("id"),1,"Norma");
	alarmsTree.childGet(i_rl).setAttr("err",curErr);	
}
rules = alarmsTree.save();',1377261539);
INSERT INTO "flb_web" VALUES('devMon','Devices monitor','',10,'var treeEl = false;

//> Parse template
var pgTree = SYS.XMLNode();
var err = pgTree.load(tmpl,false,true);

//> Access
var isAdmin = SYS.Security.access(user,4,"root","WebRoot",0440);
treeEl = SYS.XMLNode("get").setAttr("path","/sub_Security/usr_"+user+"/%2fprm%2fldscr");
SYS.cntrReq(treeEl);
re=treeEl.text().match("sub_"+pgTree.attr("class")+":\\s+([^\\n]*)");
var subAccess = re.length ? re[1] : "--";

//> Devices list place to template
var cntrPrm = SYS.DAQ.ModBus.ZegBee[pgTree.attr("class")];
catAttrReq = SYS.XMLNode("get").setAttr("path","/DAQ/ModBus/ZegBee/"+pgTree.attr("class")+"/%2fprm%2fcfg%2fATTR_LS");
SYS.cntrReq(catAttrReq);
catAttrLs = catAttrReq.text();

//> Direct requests by XML process
if(!urlPrms["com"].isEVal())
{
	var xmlRespTree = SYS.XMLNode(urlPrms["com"]);
	if(urlPrms["com"] == "info")
	{
		//> Get timers
		var timersList = SYS.XMLNode("get").setAttr("path","/sub_DAQ/mod_JavaLikeCalc/cntr_timers/prm_rules/%2fval%2fvar");
		SYS.cntrReq(timersList);
		var timersTree = SYS.XMLNode("TIMERS");
		timersTree.load(timersList.text());

		//> Devices process
		for(var off = 0; (catAttrId=catAttrLs.parse(0,"\n",off)).length; )
		{
			if((catAttrId.parse(2,":") == "r" && (isAdmin || subAccess != "--")) || (catAttrId.parse(2,":") == "rw" && (isAdmin || subAccess == "rw")))
			{
				var xmlRespEl = xmlRespTree.childAdd(catAttrId.parse(5,":")).setAttr("name",catAttrId.parse(4,":")).setAttr("id",catAttrId.parse(3,":")).setAttr("acs",catAttrId.parse(2,":"));
				if(catAttrId.parse(5,":") == "bin")
					xmlRespEl.setAttr("vl1",catAttrId.parse(6,":")).setAttr("nm1",catAttrId.parse(7,":"))
								  .setAttr("vl2",catAttrId.parse(8,":")).setAttr("nm2",catAttrId.parse(9,":")).setAttr("sensorPos",catAttrId.parse(10,":"));
				else if(catAttrId.parse(5,":") == "dec") xmlRespEl.setAttr("ed",catAttrId.parse(6,":")).setAttr("sensorPos",catAttrId.parse(8,":"));;
				//> Timer get
				var timerNd = timersTree.getElementBy(pgTree.attr("class")+"."+catAttrId.parse(3,":"));
				if(typeof(timerNd)=="XMLNode" && timerNd.attr("tm").toInt(10) > 0) xmlRespEl.setAttr("tm",timerNd.attr("tm"));
			}
		}
	}
	else if(urlPrms["com"] == "vals")
		for(var off = 0; (catAttrId=catAttrLs.parse(0,"\n",off)).length; )
		{
			if((catAttrId.parse(2,":") == "r" && (isAdmin || subAccess != "--")) || (catAttrId.parse(2,":") == "rw" && (isAdmin || subAccess == "rw")))
			{
				var curAttrVal = cntrPrm[catAttrId.parse(3,":")].get();
				var xmlRespEl = xmlRespTree.childAdd(catAttrId.parse(5,":")).setAttr("id",catAttrId.parse(3,":"));
				//>>> Value error
				if(curAttrVal.isEVal()) xmlRespEl.setAttr("val","<err>").setText("Ошибка");
				else
				{
					xmlRespEl.setAttr("val",curAttrVal);
					//>>> Prepare value for different type
					if(catAttrId.parse(5,":") == "dec")
					{
						//>>>> Calc by formule add
						var calcProg = catAttrId.parse(7,":").replace("%3A",":");
						if(calcProg.length && catAttrId.parse(2,":") != "rw")
						{
							var progArgs = new Object();
							progArgs.y = curAttrVal;
							progArgs.x = curAttrVal;
							SYS.DAQ.funcCall("JavaLikeCalc.JavaScript",progArgs,calcProg);
							curAttrVal = progArgs.y;
						}
						xmlRespEl.setText(curAttrVal);
					}
					else if(catAttrId.parse(5,":") == "bin")
					{
						if(curAttrVal == catAttrId.parse(6,":").replace("%3A",":")) curAttrVal = catAttrId.parse(7,":").replace("%3A",":");
 						else if(curAttrVal == catAttrId.parse(8,":").replace("%3A",":")) curAttrVal = catAttrId.parse(9,":").replace("%3A",":");
						else curAttrVal = "Неизвестно";
						xmlRespEl.setText(curAttrVal);
					}
				}
			}
		}
	else if(urlPrms["com"] == "set")
		for(var off = 0; (catAttrId=catAttrLs.parse(0,"\n",off)).length; )
		{
			if(catAttrId.parse(2,":") != "rw" || catAttrId.parse(3,":") != urlPrms["dev"]) continue;
			var setVal = EVAL_BOOL;
			if(catAttrId.parse(5,":") == "bin")
			{
				if(urlPrms["val"] != cntrPrm[urlPrms["dev"]].get())	setVal = urlPrms["val"];
			}
			else if(catAttrId.parse(5,":") == "dec")
			{
				var calcProg = catAttrId.parse(7,":").replace("%3A",":");
				if(!calcProg.length)
				{
					if(urlPrms["val"] != cntrPrm[urlPrms["dev"]].get()) setVal = urlPrms["val"];
				}
				else
				{
					var progArgs = new Object();
					progArgs.y = urlPrms["val"];
					progArgs.x = urlPrms["val"];
					SYS.DAQ.funcCall("JavaLikeCalc.JavaScript",progArgs,calcProg);
					setVal = progArgs.y;
				}
			}
			if(!setVal.isEVal())
			{				
				if(urlPrms["tm"].toInt(10) <= 0)
				{
					cntrPrm[urlPrms["dev"]].set(setVal);
					SYS.message("SET:House:"+pgTree.attr("class")+"."+urlPrms["dev"],2,user+": "+pgTree.attr("class")+"."+urlPrms["dev"]+": установлен в: "+setVal);
				}
				else
				{
					var timersList = SYS.XMLNode("get").setAttr("path","/sub_DAQ/mod_JavaLikeCalc/cntr_timers/prm_rules/%2fval%2fvar");
					SYS.cntrReq(timersList);
					var timersTree = SYS.XMLNode("TIMERS");
					timersTree.load(timersList.text());

					var timerNd = timersTree.getElementBy(pgTree.attr("class")+"."+urlPrms["dev"]);
					if(typeof(timerNd) != "XMLNode") timerNd = timersTree.childAdd("timer").setAttr("id",pgTree.attr("class")+"."+urlPrms["dev"]);
					timerNd.setAttr("tm",urlPrms["tm"]).setAttr("user",user).setText(setVal);
					timersList = SYS.XMLNode("set").setAttr("path","/sub_DAQ/mod_JavaLikeCalc/cntr_timers/prm_rules/%2fval%2fvar").setText(timersTree.save());
					SYS.cntrReq(timersList);
					SYS.message("SET:House:"+pgTree.attr("class")+"."+urlPrms["dev"],2,user+": "+pgTree.attr("class")+"."+urlPrms["dev"]+": установка в: "+setVal+", по таймеру: "+urlPrms["tm"]+"секунд.");
				}
			}
			break;
		}
	else if(urlPrms["com"] == "timerClear")
		for(var off = 0; (catAttrId=catAttrLs.parse(0,"\n",off)).length; )
		{
			if(catAttrId.parse(2,":") != "rw" || catAttrId.parse(3,":") != urlPrms["dev"]) continue;
			var timersList = SYS.XMLNode("get").setAttr("path","/sub_DAQ/mod_JavaLikeCalc/cntr_timers/prm_rules/%2fval%2fvar");
			SYS.cntrReq(timersList);
			var timersTree = SYS.XMLNode("TIMERS");
			timersTree.load(timersList.text());

			var timerNd = timersTree.getElementBy(pgTree.attr("class")+"."+urlPrms["dev"]);
			if(typeof(timerNd) == "XMLNode")
			{
				timerNd.setAttr("tm",0);
				timersList = SYS.XMLNode("set").setAttr("path","/sub_DAQ/mod_JavaLikeCalc/cntr_timers/prm_rules/%2fval%2fvar").setText(timersTree.save());
				SYS.cntrReq(timersList);
			}
			break;
		}

	return "200 OK\nContent-Type:text/xml\n\n"+xmlRespTree.save();
}

//Save template
rez = pgTree.save();
//SYS.messDebug("TEST Dev","TEST 00: "+typeof(devLs[formEl["cat"]+"."+itId]));',1377260842);
INSERT INTO "flb_web" VALUES('devices','Devices dispatcher','',10,'var treeEl = false;

//> Parse template
var pgTree = SYS.XMLNode();
var err = pgTree.load(tmpl,false,true);

//> Get categories list
var catLs = SYS.XMLNode("get").setAttr("path","/DAQ/ModBus/ZegBee/%2fprm%2fprm");
SYS.cntrReq(catLs);

//> Get devices list
var devLs = new Object();
for(var i = 0; i < catLs.childSize(); i++)
{
	var curCat = catLs.childGet(i).attr("id");
	catAttrReq = SYS.XMLNode("get").setAttr("path","/DAQ/ModBus/ZegBee/"+curCat+"/%2fprm%2fcfg%2fATTR_LS");
	SYS.cntrReq(catAttrReq);
	catAttrLs = catAttrReq.text();
	for(var off = 0; (catAttrId=catAttrLs.parse(0,"\n",off)).length; )
		devLs[curCat+"."+catAttrId.parse(3,":")] = true;
}

//> Process page
//>> Place categories list
if(typeof(treeEl=pgTree.getElementBy("cat","name")) == "XMLNode")
	for(var i = 0; i < catLs.childSize(); i++)
		treeEl.childAdd("OPTION").setAttr("value",catLs.childGet(i).attr("id")).setText(catLs.childGet(i).text());

//> Check POST content for commands
if(cnts.length)
{
	var formEl = new Object();
	re = new RegExp("name=\"(.+)\"");
	//>> POST form elements process
	for(var i = 0; i < cnts.length; i++)
		formEl[re.exec(cnts[i].attr("Content-Disposition"))[1]] = cnts[i].text();
	//>> Check for apply
	if(!formEl["apply"].isEVal())
	{
		var applyOK = true;
		//>> ID: Check for correct form''s values
		var itId = formEl["id"].match("[^\\s].+[^\\s]");
		if(!itId.length)
		{
			treeEl = pgTree.getElementBy("idNm","name");
			treeEl.childAdd("br");
			treeEl.childAdd("span").setAttr("style","color : red;").setText("(Пустое значение недопустимо)");
			itId = "";
			applyOK = false;
		}
		else itId = itId[0];
		pgTree.getElementBy("id","name").setAttr("value",itId);

		//>> Name: Check for correct form''s values
		var itName = formEl["name"].match("[^\\s].+[^\\s]");
		if(!itName.length)
		{
			treeEl = pgTree.getElementBy("nameNm","name");
			treeEl.childAdd("br");
			treeEl.childAdd("span").setAttr("style","color : red;").setText("(Пустое значение недопустимо)");
			itName = "";
			applyOK = false;
		}
		else itName = itName[0];
		pgTree.getElementBy("name","name").setAttr("value",itName);

		//>> Address: Check for correct form''s values
		var itAddr = formEl["addr"].toInt();
		if(itAddr < 0 || itAddr > 65535)
		{
			treeEl = pgTree.getElementBy("addrNm","name");
			treeEl.childAdd("br");
			treeEl.childAdd("span").setAttr("style","color : red;").setText("(Адрес ModBus должен быть в диапазоне (1-65535))");
			applyOK = false;
		}
		pgTree.getElementBy("addr","name").setAttr("value",itAddr);

		//>> Type: Check for correct form''s values
		pgTree.getElementBy(formEl["type"],"value").setAttr("selected","selected");

		//>> sensorPos: Check for correct form''s values
		pgTree.getElementBy("sensorPos","name").setAttr("value",formEl["sensorPos"]);

		//>> Category: Check for correct form''s values
		pgTree.getElementBy(formEl["cat"],"value").setAttr("selected","selected");

		//>> Data type: 
		pgTree.getElementBy(formEl["typeData"],"value").setAttr("selected","selected");
		pgTree.getElementBy("in1").setAttr("style","display: "+((formEl["typeData"] == "bin")?"":"none"));
		pgTree.getElementBy("in2").setAttr("style","display: "+((formEl["typeData"] == "bin")?"":"none"));
		pgTree.getElementBy("ed").setAttr("style","display: "+((formEl["typeData"] == "dec")?"":"none"));
		pgTree.getElementBy("formul").setAttr("style","display: "+((formEl["typeData"] == "dec")?"":"none"));

		//>> Binary type properties
		pgTree.getElementBy("in1","name").setAttr("value",formEl["in1"]);
		pgTree.getElementBy("view1","name").setAttr("value",formEl["view1"]);
		pgTree.getElementBy("in2","name").setAttr("value",formEl["in2"]);
		pgTree.getElementBy("view2","name").setAttr("value",formEl["view2"]);

		//>> Digital type properties
		pgTree.getElementBy("ed","name").setAttr("value",formEl["ed"]);
		pgTree.getElementBy("formul","name").setAttr("value",formEl["formul"]);	

		//>> Alarm
		pgTree.getElementBy("alarm","name").childGet(0).setText(formEl["alarm"]);

		//> Add or update device
		if(applyOK)
		{
			//>> Get curent attributes list for category
			catAttrReq = SYS.XMLNode("get").setAttr("path","/DAQ/ModBus/ZegBee/"+formEl["cat"]+"/%2fprm%2fcfg%2fATTR_LS");
			SYS.cntrReq(catAttrReq);
			catAttrLs = catAttrReq.text();
			var newAttrReq = "R:"+itAddr+":"+(formEl["type"]=="out"?"rw":"r")+":"+itId+":"+itName.replace(":","%3A")+":"+formEl["typeData"];
			if(formEl["typeData"] == "bin") newAttrReq += ":"+formEl["in1"].replace(":","%3A")+":"+formEl["view1"].replace(":","%3A")+":"+formEl["in2"].replace(":","%3A")+":"+formEl["view2"].replace(":","%3A");
			else if(formEl["typeData"] == "dec") newAttrReq += ":"+formEl["ed"].replace(":","%3A")+":"+formEl["formul"].replace(":","%3A");
			newAttrReq += ":"+formEl["sensorPos"]+"\n";
			//>> Append record
			var isAppend = typeof(devLs[formEl["cat"]+"."+itId]) == "null";
			if(isAppend)
			{
				catAttrLs += newAttrReq;
				devLs[formEl["cat"]+"."+itId] = true;
			}
			//>> Replace
			else catAttrLs = catAttrLs.replace(new RegExp("R:[^:]*:[^:]*:"+itId+":[^\n]*\n"),newAttrReq);
			//>> Write new attributes list
			catAttrReq = SYS.XMLNode("CntrReqs").setAttr("path","/DAQ/ModBus/ZegBee/"+formEl["cat"]);
			catAttrReq.childAdd("set").setAttr("path","/%2fprm%2fcfg%2fATTR_LS").setText(catAttrLs);
			catAttrReq.childAdd("set").setAttr("path","/%2fprm%2fst%2fen").setText(0);
			catAttrReq.childAdd("set").setAttr("path","/%2fprm%2fst%2fen").setText(1);
			catAttrReq.childAdd("save").setAttr("path","/%2fobj");
			//XMLNode("set").setAttr("path","/DAQ/ModBus/ZegBee/"+formEl["cat"]+"/%2fprm%2fcfg%2fATTR_LS").setText(catAttrLs);
			SYS.cntrReq(catAttrReq);
			//>> Set message about correct apply
			treeEl=pgTree.getElementBy("mess","name");
			treeEl.childAdd("span").setAttr("style","color : green;").setText(
				isAppend?"Новое устройство успешно добавлено. Можно продолжать.":"Устройство успешно обновлено. Можно продолжать.");
			//>> Set alarm procedure
			var alarmsList = SYS.XMLNode("get").setAttr("path","/sub_DAQ/mod_JavaLikeCalc/cntr_alarms/prm_rules/%2fval%2fvar");
			SYS.cntrReq(alarmsList);
			var alarmsTree = SYS.XMLNode("ALARMS");
			alarmsTree.load(alarmsList.text());
			var alarmNd = alarmsTree.getElementBy(formEl["cat"]+"."+formEl["id"]);
			if(typeof(alarmNd)=="XMLNode") alarmNd.setText(formEl["alarm"]);
			else alarmsTree.childAdd("it").setAttr("id",formEl["cat"]+"."+formEl["id"]).setText(formEl["alarm"]);
			alarmsList = SYS.XMLNode("CntrReqs").setAttr("path","/sub_DAQ/mod_JavaLikeCalc/cntr_alarms/prm_rules");
			alarmsList.childAdd("set").setAttr("path","/%2fval%2fvar").setText(alarmsTree.save());
			alarmsList.childAdd("save").setAttr("path","/%2fobj");
			SYS.cntrReq(alarmsList);
		}
		//> Form error
		else
		{
			treeEl=pgTree.getElementBy("mess","name");
			treeEl.childAdd("span").setAttr("style","color : red;").setText("Ошибка в формировании формы! Исправьте отмеченные поля и повторите.");
		}
	}
	else if(!formEl["edDel"].isEVal())
	{
		if(formEl["edId"].length && typeof(devLs[formEl["edId"]]) != "null")
		{
			//>> Get curent attributes list for category
			catAttrReq = SYS.XMLNode("get").setAttr("path","/DAQ/ModBus/ZegBee/"+(formEl["edId"].parse(0))+"/%2fprm%2fcfg%2fATTR_LS");
			SYS.cntrReq(catAttrReq);
			catAttrLs = catAttrReq.text();
			catAttrLs = catAttrLs.replace(new RegExp("R:[^:]*:[^:]*:"+formEl["edId"].parse(1)+":[^\n]*\n"),"");
			//>> Write new attributes list
			catAttrReq = SYS.XMLNode("set").setAttr("path","/DAQ/ModBus/ZegBee/"+(formEl["edId"].parse(0))+"/%2fprm%2fcfg%2fATTR_LS").setText(catAttrLs);
			SYS.cntrReq(catAttrReq);
			//>> Set message about correct apply
			treeEl=pgTree.getElementBy("mess","name");
			treeEl.childAdd("span").setAttr("style","color : green;").setText("Устройство успешно удалено. Можно продолжать.");
			devLs[formEl["edId"]] = false;
		}
		else
		{
			treeEl=pgTree.getElementBy("mess","name");
			treeEl.childAdd("span").setAttr("style","color : red;").setText("Устройство для удаления отсутствует.");
		}
	}
}

//>> Devices list place to template
treeEl=pgTree.getElementBy("edId","name");
treeEl.childAdd("OPTION");
for(var i_o in devLs)
	if(devLs[i_o]) treeEl.childAdd("OPTION").setAttr("value",i_o).setText(i_o);

//>> Load variables for select device
if(!cnts.length && !urlPrms["selDev"].isEVal() && urlPrms["selDev"].length) 
{
	//> Select device
	pgTree.getElementBy(urlPrms["selDev"],"value").setAttr("selected","selected");
	//> Load values for select device
	catAttrReq = SYS.XMLNode("get").setAttr("path","/DAQ/ModBus/ZegBee/"+(urlPrms["selDev"].parse(0))+"/%2fprm%2fcfg%2fATTR_LS");
	SYS.cntrReq(catAttrReq);
	catAttrLs = catAttrReq.text().match("R:([^:]*):([^:]*):"+urlPrms["selDev"].parse(1)+":([^\n]*)");
	//>> ID: Check for correct form''s values
	pgTree.getElementBy("id","name").setAttr("value",urlPrms["selDev"].parse(1));
	//>> Name: Check for correct form''s values
	pgTree.getElementBy("name","name").setAttr("value",catAttrLs[3].parse(0,":").replace("%3A",":"));
	//>> Address: Check for correct form''s values
	pgTree.getElementBy("addr","name").setAttr("value",catAttrLs[1]);
	//>> Type: Check for correct form''s values
	pgTree.getElementBy(((catAttrLs[2]=="rw")?"out":"sensor"),"value").setAttr("selected","selected");
	//>> Category: Check for correct form''s values
	pgTree.getElementBy(urlPrms["selDev"].parse(0),"value").setAttr("selected","selected");
	//>> Data type: 
	var dtType = catAttrLs[3].parse(1,":");
	pgTree.getElementBy(dtType,"value").setAttr("selected","selected");
	if(dtType == "bin")
	{
		pgTree.getElementBy("in1","name").setAttr("value",catAttrLs[3].parse(2,":").replace("%3A",":"));
		pgTree.getElementBy("view1","name").setAttr("value",catAttrLs[3].parse(3,":").replace("%3A",":"));
		pgTree.getElementBy("in2","name").setAttr("value",catAttrLs[3].parse(4,":").replace("%3A",":"));
		pgTree.getElementBy("view2","name").setAttr("value",catAttrLs[3].parse(5,":").replace("%3A",":"));
		pgTree.getElementBy("sensorPos","name").setAttr("value",catAttrLs[3].parse(6,":"));
	}
	else if(dtType == "dec")
	{
		pgTree.getElementBy("ed","name").setAttr("value",catAttrLs[3].parse(2,":").replace("%3A",":"));
		pgTree.getElementBy("formul","name").setAttr("value",catAttrLs[3].parse(3,":").replace("%3A",":"));
		pgTree.getElementBy("sensorPos","name").setAttr("value",catAttrLs[3].parse(4,":"));
	}
	pgTree.getElementBy("in1").setAttr("style","display: "+((dtType == "bin")?"":"none"));
	pgTree.getElementBy("in2").setAttr("style","display: "+((dtType == "bin")?"":"none"));
	pgTree.getElementBy("ed").setAttr("style","display: "+((dtType == "dec")?"":"none"));
	pgTree.getElementBy("formul").setAttr("style","display: "+((dtType == "dec")?"":"none"));

	//>> Get alarm procedure
	alarmsList = SYS.XMLNode("get").setAttr("path","/sub_DAQ/mod_JavaLikeCalc/cntr_alarms/prm_rules/%2fval%2fvar");
	SYS.cntrReq(alarmsList);
	var alarmsTree = SYS.XMLNode();
	alarmsTree.load(alarmsList.text());
	var alarmNd = alarmsTree.getElementBy(urlPrms["selDev"]);
	pgTree.getElementBy("alarm","name").childGet(0).setText((typeof(alarmNd)=="XMLNode")?alarmNd.text():"");
}
else pgTree.getElementBy("edDel","name").setAttr("style","visibility : hidden;");

//Save template
rez = pgTree.save();
//SYS.messDebug("TEST Dev","TEST 00: "+typeof(devLs[formEl["cat"]+"."+itId]));',1377260944);
INSERT INTO "flb_web" VALUES('mess','Alarm messages','',10,'var treeEl = false;

//> Parse template
var pgTree = SYS.XMLNode();
var err = pgTree.load(tmpl,false,true);
treeEl = pgTree.getElementBy("mess");

var curTime = SYS.time();
var messAr = SYS.Archive.messGet(curTime,curTime,"ALARM:House:*",-1);
var trEl = treeEl.childAdd("tr");
trEl.childAdd("th").setText("Время");
trEl.childAdd("th").setText("Категория");
trEl.childAdd("th").setText("Уровень");
trEl.childAdd("th").setText("Сообщение");
for(i = 0; i < messAr.length; i++)
{
	var trEl = treeEl.childAdd("tr");
	trEl.childAdd("td").setAttr("class","mTm").setText(SYS.strftime(messAr[i].tm));
	trEl.childAdd("td").setAttr("class","mCat").setText(messAr[i].categ);
	trEl.childAdd("td").setAttr("class","mLev").setText(messAr[i].level);
	trEl.childAdd("td").setAttr("class","mMess").setText(messAr[i].mess);
}

//Save template
rez = pgTree.save();
//SYS.messDebug("TEST Dev","TEST 00: "+typeof(devLs[formEl["cat"]+"."+itId]));',1377261138);
INSERT INTO "flb_web" VALUES('report','Reports','',10,'var treeEl = false;
var pgDir = "./HouseSpirit/Web/";

//> Parse template
var pgTree = SYS.XMLNode();
var err = pgTree.load(tmpl,false,true);

//> Load default values
pgTree.getElementBy("btm","name").setAttr("value",SYS.strftime(SYS.time()));
pgTree.getElementBy("etm","name").setAttr("value",SYS.strftime(SYS.time()));
var catLs = SYS.XMLNode("get").setAttr("path","/DAQ/ModBus/ZegBee/%2fprm%2fprm");
SYS.cntrReq(catLs);
treeEl = pgTree.getElementBy("subsys");
for(var i = 0; i < catLs.childSize(); i++)
{
	treeEl.childAdd("span").setText(catLs.childGet(i).text());
	treeEl.childAdd("input").setAttr("type","checkbox").setAttr("name",catLs.childGet(i).attr("id"));
	treeEl.childAdd("br");
}

//> Check POST content for commands - forming report
if(cnts.length)
{
	var formEl = new Object();
	var re = new RegExp("name=\"(.+)\"");
	//>> POST form elements process
	for(i_c = 0; i_c < cnts.length; i_c++)
		formEl[re.exec(cnts[i_c].attr("Content-Disposition"))[1]] = cnts[i_c].text();

	//>> Check for apply
	if(!formEl["show"].isEVal())
	{
		var reqRule = "";
		var applyOK = true;
		//>> Time: begin and end time check
		var btm = SYS.strptime(formEl["btm"]);
		pgTree.getElementBy("btm","name").setAttr("value",SYS.strftime(btm));
		var etm = SYS.strptime(formEl["etm"]);
		pgTree.getElementBy("etm","name").setAttr("value",SYS.strftime(etm));

		//>> System
		if(formEl["sys"].length)
		{
			pgTree.getElementBy("sys","name").setAttr("checked","checked");
			reqRule = "\\/sub_Protocol\\/mod_HTTP\\/";
		}

		//>> Subsystems
		var catLs = SYS.XMLNode("get").setAttr("path","/DAQ/ModBus/ZegBee/%2fprm%2fprm");
		SYS.cntrReq(catLs);
		for(var i = 0; i < catLs.childSize(); i++)
		{
			var subId = catLs.childGet(i).attr("id");
			if(formEl[subId].length)
			{
				pgTree.getElementBy(subId,"name").setAttr("checked","checked");
				if(reqRule.length) reqRule += "|";
				reqRule += "(ALARM|SET)\\:House\\:"+subId;
			}
		}

		//>> Report generate
		if(btm < etm && (etm-btm) < (365*24*60*60) && reqRule.length)
		{
			var messAr = SYS.Archive.messGet(btm,etm,"/("+reqRule+")/",1);
			treeEl = pgTree.getElementBy("report");
			var trEl = treeEl.childAdd("tr");
			trEl.childAdd("th").setText("Время");
			trEl.childAdd("th").setText("Категория");
			trEl.childAdd("th").setText("Уровень");
			trEl.childAdd("th").setText("Сообщение");
			for(i = 0; i < messAr.length; i++)
			{
				var trEl = treeEl.childAdd("tr");
				trEl.childAdd("td").setAttr("class","mTm").setText(SYS.strftime(messAr[i].tm));
				trEl.childAdd("td").setAttr("class","mCat").setText(messAr[i].categ);
				trEl.childAdd("td").setAttr("class","mLev").setText(messAr[i].level);
				trEl.childAdd("td").setAttr("class","mMess").setText(messAr[i].mess);
			}
			//>> Save report to file
			var saveFile = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"+
					"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n"+
					"<html xmlns=''http://www.w3.org/1999/xhtml''>\n"+
					"<head>\n"+
					"  <meta http-equiv=''Content-Type'' content=''text/html; charset=UTF-8''/>\n"+
					"  <title>Отчёт</title>\n"+
					"  <style type=''text/css''>\n"+
					"    #report { width: 100%; border: 1px solid black; border-collapse: collapse; empty-cells: show; }\n"+
					"    #report TH { border: 1px solid black; background-color: #E6E6E6; }\n"+
					"    #report TD { border: 1px solid black; }\n"+
					"    #report .mTm { white-space: nowrap; }\n"+
					"    #report .mLev { text-align: center; }\n"+
					"  </style>\n"+
					"</head>\n"+
					"<body>\n"+treeEl.save()+"\n</body>\n"+
					"</html>";
			SYS.fileWrite(pgDir+"reports/rep_"+user+".html",saveFile);
			pgTree.getElementBy("saveFile").setAttr("href","reports/rep_"+user+".html").setText("rep_"+user+".html");
		}
	}
}

//Save template
rez = pgTree.save();
//SYS.messDebug("TEST Dev","TEST 00: "+typeof(devLs[formEl["cat"]+"."+itId]));',1377261196);
INSERT INTO "flb_web" VALUES('test','Test','',10,'if(f_start)
{
	var req = SYS.XMLNode("send").setAttr("ProtIt","SMS").setAttr("tel","+380679859815").setText("Тестовое сообщение");
	SYS.Transport.Serial.out_GSM.messIO(req,"UserProtocol");
}',1377261398);
INSERT INTO "flb_web" VALUES('timerWrite','Write by the timer','',10,'//> Load rules
var timersTree = SYS.XMLNode("TIMERS");
timersTree.load(rules);
for(i_rl = 0; i_rl < timersTree.childSize(); i_rl++)
{
	var curTm = timersTree.childGet(i_rl);
	if(!curTm.text().length) continue;
	var subId = curTm.attr("id").parse(0);
	var devId = curTm.attr("id").parse(1);
	var timer = curTm.attr("tm").toInt(10);
	if(timer && max(0,timer-1) == 0)
	{
		SYS.DAQ.ModBus.ZegBee[subId][devId].set(curTm.text());
		SYS.message("SET:House:"+curTm.attr("id"),2,curTm.attr("user")+": "+curTm.attr("id")+": установлен в: "+curTm.text());
	}
	curTm.setAttr("tm",max(0,timer-1));
}
rules = timersTree.save();',1377261462);
INSERT INTO "flb_web" VALUES('user','Users dispatcher','',10,'var treeEl = false;

//> Parse template
var pgTree = SYS.XMLNode();
var err = pgTree.load(tmpl,false,true);
var isAdmin = SYS.Security.access(user,4,"root","WebRoot",0440);
var curUser = user;

//> Check POST content for commands
if(cnts.length)
{
	var formEl = new Object();
	re = new RegExp("name=\"(.+)\"");
	//>> POST form elements process
	for(var i = 0; i < cnts.length; i++)
		formEl[re.exec(cnts[i].attr("Content-Disposition"))[1]] = cnts[i].text();
	//>> Check for apply
	if(!formEl["apply"].isEVal())
	{
		var applyOK = true;
		//>> Login: Check for correct form''s values
		if(!(userLogin=formEl["login"].match("[^\\s].+[^\\s]")).length)
		{
			treeEl = pgTree.getElementBy("loginNm","name");
			treeEl.childAdd("br");
			treeEl.childAdd("span").setAttr("style","color : red;").setText("(Пустое значение недопустимо)");
			userLogin = "";
			applyOK = false;
		}
		else userLogin = userLogin[0];
		if(userLogin.isEVal()) userLogin = user;
		pgTree.getElementBy("login","name").setAttr("value",userLogin);
		var isUserPres = (typeof(SYS.Security.nodeAt("usr_"+userLogin)) == "TCntrNode");
		var isWebUser = isUserPres && SYS.Security.access(userLogin,4,"root","Web",0440);
		var isWebRootUser = isUserPres && SYS.Security.access(userLogin,4,"root","WebRoot",0440);
		if(isUserPres && !(isWebUser||isWebRootUser)) applyOK = false;
		if(!isAdmin && userLogin != user) applyOK = false;

		//>> Name: Check for correct form''s values
		var userName = formEl["fio"].match("[^\\s].+[^\\s]");
		if(!userName.length)
		{
			treeEl = pgTree.getElementBy("fioNm","name");
			treeEl.childAdd("br");
			treeEl.childAdd("span").setAttr("style","color : red;").setText("(Пустое значение недопустимо)");
			userName = "";
			applyOK = false;
		}
		else userName = userName[0];
		pgTree.getElementBy("fio","name").setAttr("value",userName);

		//>> Password: Check for correct form''s values
		if((!isUserPres && formEl["pass"].length < 8) || (isUserPres && formEl["pass"].length && formEl["pass"].length < 8))
		{
			treeEl = pgTree.getElementBy("passNm","name");
			treeEl.childAdd("br");
			treeEl.childAdd("span").setAttr("style","color : red;").setText("(Пароль должен быть более 8 символов)");
			userName = "";
			applyOK = false;
		}

		if(isAdmin)
		{
			//>> Telepfons list, SMS send and report view
			pgTree.getElementBy("tel","name").setAttr("value",formEl["tel"]);
			if(formEl["sms"].length) pgTree.getElementBy("sms","name").setAttr("checked","checked");
			if(formEl["report"].length) pgTree.getElementBy("report","name").setAttr("checked","checked");

			if(!isWebRootUser)
			{
				//>> Subsystem''s permition
				var catLs = SYS.XMLNode("get").setAttr("path","/DAQ/ModBus/ZegBee/%2fprm%2fprm");
				SYS.cntrReq(catLs);
				treeEl = pgTree.getElementBy("sub_access");
				for(var i = 0; i < catLs.childSize(); i++)
				{
					var subId = "sub_"+catLs.childGet(i).attr("id");
					treeEl.childAdd("span").setText(catLs.childGet(i).text());
					treeEl.childAdd("br");
					treeEl1 = treeEl.childAdd("select").setAttr("name",subId);
					treeEl.childAdd("br");
					treeEl2 = treeEl1.childAdd("option").setAttr("value","--").setText("Нет доступа");
					if(formEl[subId] == "--") treeEl2.setAttr("selected","selected");
					treeEl2 = treeEl1.childAdd("option").setAttr("value","r-").setText("Наблюдение");
					if(formEl[subId] == "r-") treeEl2.setAttr("selected","selected");
					treeEl2 = treeEl1.childAdd("option").setAttr("value","rw").setText("Управление");
					if(formEl[subId] == "rw") treeEl2.setAttr("selected","selected");
				}
			}
			else
			{
				//> Session life time
				treeEl = pgTree.getElementBy("sub_access");
				treeEl.childAdd("span").setText("Время жизни сеанса аутентификации (минут)");
				treeEl.childAdd("br");
				treeEl.childAdd("input").setAttr("type","text").setAttr("name","sesAuth").setAttr("value",formEl["sesAuth"]);
			}
		}

		//> Add or update user
		if(applyOK)
		{
			//> Create new user if no present
			if(!isUserPres)
			{
				userReq = SYS.XMLNode("CntrReqs").setAttr("path","/sub_Security");
				userReq.childAdd("add").setAttr("path","%2fusgr%2fusers").setText(userLogin);
				userReq.childAdd("add").setAttr("path","/grp_Web/%2fprm%2fusers").setText(userLogin);
				SYS.cntrReq(userReq); 
			}
			//> Set user''s properties
			userReq = SYS.XMLNode("CntrReqs").setAttr("path","/sub_Security/usr_"+userLogin);
			userReq.childAdd("set").setAttr("path","%2fprm%2fdscr").setText(userName);
			if(formEl["pass"].length)
				userReq.childAdd("set").setAttr("path","%2fprm%2fpass").setText(formEl["pass"]);
			if(isAdmin)
			{
				var descrVal = "TEL: "+formEl["tel"]+"\n"+
							"SMS: "+(formEl["sms"].length?"true":"false")+"\n"+
							"Report: "+(formEl["report"].length?"true":"false")+"\n";
				if(!isWebRootUser)
				{
					//>> Subsystem''s permition
					for(var i = 0; i < catLs.childSize(); i++)
					{
						var subId = "sub_"+catLs.childGet(i).attr("id");
						descrVal += subId+": "+formEl[subId]+"\n";
					}
				}
				else
				{
					var sessLife = SYS.XMLNode("CntrReqs").setAttr("path","/sub_Protocol/mod_HTTP");
					sessLife.childAdd("set").setAttr("path","/%2fprm%2flf_tm").setText(max(1,formEl["sesAuth"].toInt()));
					sessLife.childAdd("save").setAttr("path","/%2fobj");
					SYS.cntrReq(sessLife);

				}
				userReq.childAdd("set").setAttr("path","%2fprm%2fldscr").setText(descrVal);
			}
			userReq.childAdd("save").setAttr("path","/%2fobj");
			SYS.cntrReq(userReq); 
			//>> Set message about correct apply
			treeEl=pgTree.getElementBy("mess","name");
			treeEl.childAdd("span").setAttr("style","color : green;").setText(
				(!isUserPres)?"Новый пользователь успешно добавлен. Можно продолжать.":"Пользователь успешно обновлен. Можно продолжать.");
		}
		//> Form error
		else
		{
			treeEl=pgTree.getElementBy("mess","name").childAdd("span").setAttr("style","color : red;");
			if(!isAdmin && userLogin != user) treeEl.setText("Обычный пользователь может редактировать только собственную учётную запись.");
			else if(isUserPres && !isWebUser) treeEl.setText("Пользователь присутствует, но не является Web-пользователем. Измените логин пользователя и повторите.");
			else treeEl.setText("Ошибка в формировании формы! Исправьте отмеченные поля и повторите.");
		}
	}
	else if(!formEl["edDel"].isEVal())
	{
		var isWebUser = SYS.Security.access(formEl["edLogin"],4,"root","Web",0440);
		var isWebRootUser = SYS.Security.access(formEl["edLogin"],4,"root","WebRoot",0440);
		if(isWebUser && !isWebRootUser)
		{
			userReq = SYS.XMLNode("CntrReqs").setAttr("path","/sub_Security");
			userReq.childAdd("del").setAttr("path","%2fusgr%2fusers").setText(formEl["edLogin"]);
			userReq.childAdd("del").setAttr("path","/grp_Web/%2fprm%2fusers").setText(formEl["edLogin"]);
			userReq.childAdd("save").setAttr("path","/%2fobj");
			SYS.cntrReq(userReq); 

			//>> Set message about correct remove
			treeEl=pgTree.getElementBy("mess","name");
			treeEl.childAdd("span").setAttr("style","color : green;").setText("Пользователь успешно удален. Можно продолжать.");
		}
		else
		{
			treeEl=pgTree.getElementBy("mess","name").childAdd("span").setAttr("style","color : red;");
			if(isWebRootUser) treeEl.setText("Удаление суперпользователя недопустимо!");
			else treeEl.setText("Пользователь для удаления отсутствует.");
		}
	}
}

//>> Load variables for select device
if(isAdmin)
{
	//> Load users list
	var userLst = SYS.XMLNode("get").setAttr("path","/sub_Security/grp_Web/%2fprm%2fusers");
	SYS.cntrReq(userLst);

	treeEl = pgTree.getElementBy("edLogin","name");
	treeEl.childAdd("OPTION").setAttr("value","").setText("");
	treeEl.childAdd("OPTION").setAttr("value","new").setText("Новый пользователь");
	for(var i = 0; i < userLst.childSize(); i++)
	{
		treeEl1 = SYS.XMLNode("get").setAttr("path","/sub_Security/usr_"+userLst.childGet(i).text()+"/%2fprm%2fdscr");
		SYS.cntrReq(treeEl1);
		treeEl.childAdd("OPTION").setAttr("value",userLst.childGet(i).text()).setText(treeEl1.text());
	}
}
else
{
	//> Hide select input and block login edit
	pgTree.getElementBy("edLogin","name").setAttr("style","display : none;");
	pgTree.getElementBy("login","name").setAttr("disabled","disabled");
	pgTree.getElementBy("tel").setAttr("style","visibility : hidden;");
	pgTree.getElementBy("sms").setAttr("style","visibility : hidden;");
	pgTree.getElementBy("report").setAttr("style","visibility : hidden;");
}

if(!cnts.length || !formEl["edDel"].isEVal())
{
	if(!urlPrms["selUser"].isEVal() && urlPrms["selUser"].length)
	{
		curUser = urlPrms["selUser"];
		pgTree.getElementBy(curUser,"value").setAttr("selected","selected");
	}
	var isSelAdmin = SYS.Security.access(curUser,4,"root","WebRoot",0440);

	//> Connect to user object
	//>> User login
	pgTree.getElementBy("login","name").setAttr("value",curUser);
	if(isAdmin && SYS.Security.access(curUser,4,"root","WebRoot",0440))
	{
		pgTree.getElementBy("login","name").setAttr("disabled","disabled");
		pgTree.getElementBy("edDel","name").setAttr("style","display : none;");
	}
	//>> User FN
	treeEl = SYS.XMLNode("get").setAttr("path","/sub_Security/usr_"+curUser+"/%2fprm%2fdscr");
	SYS.cntrReq(treeEl);
	pgTree.getElementBy("fio","name").setAttr("value",treeEl.text());
	if(isAdmin)
	{
		//>> Description
		treeEl = SYS.XMLNode("get").setAttr("path","/sub_Security/usr_"+curUser+"/%2fprm%2fldscr");
		SYS.cntrReq(treeEl);
		//>> Phones, SMS and repor
		re=treeEl.text().match("TEL:\\s+([^\\n]*)");
		pgTree.getElementBy("tel","name").setAttr("value",re.length ? re[1] : "");
		re=treeEl.text().match("SMS:\\s+([^\\n]*)");
		if(re.length && re[1] == "true") pgTree.getElementBy("sms","name").setAttr("checked","checked");
		re=treeEl.text().match("Report:\\s+([^\\n]*)");
		if(re.length && re[1] == "true") pgTree.getElementBy("report","name").setAttr("checked","checked");
		//>> Subsystem''s permition
		if(!isSelAdmin)
		{
			var catLs = SYS.XMLNode("get").setAttr("path","/DAQ/ModBus/ZegBee/%2fprm%2fprm");
			SYS.cntrReq(catLs);
			treeEl1 = pgTree.getElementBy("sub_access");
			for(var i = 0; i < catLs.childSize(); i++)
			{
				var subId = "sub_"+catLs.childGet(i).attr("id");
				re=treeEl.text().match(subId+":\\s+([^\\n]*)");
				treeEl1.childAdd("span").setText(catLs.childGet(i).text());
				treeEl1.childAdd("br");
				treeEl2 = treeEl1.childAdd("select").setAttr("name",subId);
				treeEl1.childAdd("br");
				treeEl3 = treeEl2.childAdd("option").setAttr("value","--").setText("Нет доступа");
				if(re.length && re[1] == "--") treeEl3.setAttr("selected","selected");
				treeEl3 = treeEl2.childAdd("option").setAttr("value","r-").setText("Наблюдение");
				if(re.length && re[1] == "r-") treeEl3.setAttr("selected","selected");
				treeEl3 = treeEl3.childAdd("option").setAttr("value","rw").setText("Управление");
				if(re.length && re[1] == "rw") treeEl3.setAttr("selected","selected");
			}
		}
		else
		{
			//> Session life time
			treeEl1 = pgTree.getElementBy("sub_access");
			treeEl1.childAdd("span").setText("Время жизни сеанса аутентификации (минут)");
			treeEl1.childAdd("br");
			var sessLife = SYS.XMLNode("get").setAttr("path","/sub_Protocol/mod_HTTP/%2fprm%2flf_tm");
			SYS.cntrReq(sessLife);
			treeEl1.childAdd("input").setAttr("type","text").setAttr("name","sesAuth").setAttr("value",sessLife.text());
		}
	}
}

//> Delete button hide
if(!isAdmin || cnts.length || urlPrms["selUser"].isEVal() || !urlPrms["selUser"].length)
	pgTree.getElementBy("edDel","name").setAttr("style","display : none;");

//Save template
rez = pgTree.save();
//SYS.messDebug("TEST Dev","TEST 00: "+typeof(devLs[formEl["cat"]+"."+itId]));',1377261537);
CREATE TABLE 'flb_web_io' ("F_ID" TEXT DEFAULT '' ,"ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"TYPE" INTEGER DEFAULT '' ,"MODE" INTEGER DEFAULT '' ,"DEF" TEXT DEFAULT '' ,"HIDE" INTEGER DEFAULT '' ,"POS" INTEGER DEFAULT '' , PRIMARY KEY ("F_ID","ID"));
INSERT INTO "flb_web_io" VALUES('alarms','rules','Rules',0,1,'\n',0,0);
INSERT INTO "flb_web_io" VALUES('devMon','rez','Result',0,2,'',0,0);
INSERT INTO "flb_web_io" VALUES('devMon','urlPrms','Address parameters',4,0,'',0,1);
INSERT INTO "flb_web_io" VALUES('devMon','tmpl','Template',0,0,'',0,2);
INSERT INTO "flb_web_io" VALUES('devMon','cnts','POST request''s context',4,0,'',0,3);
INSERT INTO "flb_web_io" VALUES('devMon','user','User',0,0,'',0,4);
INSERT INTO "flb_web_io" VALUES('devices','rez','Result',0,2,'',0,0);
INSERT INTO "flb_web_io" VALUES('devices','urlPrms','Address parameters',4,0,'',0,1);
INSERT INTO "flb_web_io" VALUES('devices','tmpl','Template',0,0,'',0,2);
INSERT INTO "flb_web_io" VALUES('devices','cnts','POST request''s context',4,0,'',0,3);
INSERT INTO "flb_web_io" VALUES('devices','user','User',0,0,'',0,4);
INSERT INTO "flb_web_io" VALUES('mess','rez','Result',0,2,'',0,0);
INSERT INTO "flb_web_io" VALUES('mess','urlPrms','Address parameters',4,0,'',0,1);
INSERT INTO "flb_web_io" VALUES('mess','tmpl','Template',0,0,'',0,2);
INSERT INTO "flb_web_io" VALUES('mess','cnts','POST request''s context',4,0,'',0,3);
INSERT INTO "flb_web_io" VALUES('mess','user','User',0,0,'',0,4);
INSERT INTO "flb_web_io" VALUES('report','rez','Result',0,2,'',0,0);
INSERT INTO "flb_web_io" VALUES('report','urlPrms','Address parameters',4,0,'',0,1);
INSERT INTO "flb_web_io" VALUES('report','tmpl','Template',0,0,'',0,2);
INSERT INTO "flb_web_io" VALUES('report','cnts','POST request''s context',4,0,'',0,3);
INSERT INTO "flb_web_io" VALUES('report','user','User',0,0,'',0,4);
INSERT INTO "flb_web_io" VALUES('test','f_start','Start the function flag',3,0,'0',0,0);
INSERT INTO "flb_web_io" VALUES('test','test','Test',0,1,'',0,1);
INSERT INTO "flb_web_io" VALUES('timerWrite','rules','Rules',0,1,'\n',0,0);
INSERT INTO "flb_web_io" VALUES('user','rez','Result',0,2,'',0,0);
INSERT INTO "flb_web_io" VALUES('user','urlPrms','Address parameters',4,0,'',0,1);
INSERT INTO "flb_web_io" VALUES('user','tmpl','Template',0,0,'',0,2);
INSERT INTO "flb_web_io" VALUES('user','cnts','POST request''s context',4,0,'',0,3);
INSERT INTO "flb_web_io" VALUES('user','user','User',0,0,'',0,4);
CREATE TABLE 'WebUser_uPg' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '0' ,"PROG" TEXT DEFAULT '' ,"uk#PROG" TEXT DEFAULT '' ,"TIMESTAMP" INTEGER DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO "WebUser_uPg" VALUES('tmpl','XHTML-template','XHTML-template','Site from XHTML template.','Site on XHTML templates.',1,'JavaLikeCalc.JavaScript
var pgDir = "Web/";
var firstEl = url.parsePath(0);
var procPg = false;
var tmplPg = "";
var tmplPrcPg = false;
var pgTree = false;
var treeEl = false;
var err = false;
var isUser = SYS.Security.access(user,4,"root","Web",0440);
var isRootUser = SYS.Security.access(user,4,"root","WebRoot",0440);

//> Pages requests process
if(!firstEl.length || (tmplPg=SYS.fileRead(pgDir+firstEl+".html")).length)
{
	//> Early call page''s script
	if(tmplPg.length)
	{
		procPg = SYS.DAQ.JavaLikeCalc.lib_web[(URLprms["script"].isEVal() ? firstEl : URLprms["script"])];
		if(typeof(procPg) == "TCntrNode")
		{
			tmplPrcPg = procPg.call(URLprms,tmplPg,cnts,user);
			//>> Check for full XML or resource request
			var poff = 0;
			if(tmplPrcPg.parse(0,"\n",poff) == "200 OK")
			{
				while((pEl=tmplPrcPg.parse(0,"\n",poff)).length) HTTPvars[pEl.parse(0,":")] = pEl.parse(1,":");
				page = tmplPrcPg.slice(poff);
				return "200 OK";
			}
		}
		else tmplPrcPg = tmplPg;
	}

	//> Parse template
	pgTree = SYS.XMLNode();
	err = pgTree.load(pgDir+"main.html",true,true);

	//> Page time place
	pgTree.getElementBy("time_vl").setAttr("value",SYS.time());
	var sessLife = SYS.XMLNode("get").setAttr("path","/sub_Protocol/mod_HTTP/%2fprm%2flf_tm");
	SYS.cntrReq(sessLife);
	pgTree.getElementBy("ses_vl").setAttr("value",sessLife.text().toInt()*60);

	//> Check access to WEB resource
	if(!user.length || !isUser)
	{
		pgTree.getElementBy("ses_vl").setAttr("value",10);
		pgTree.getElementBy("menu_l").setAttr("style","visibility : hidden;");
		pgTree.getElementBy("menu_r").setAttr("style","visibility : hidden;");
		pgTree.getElementBy("status").setAttr("style","visibility : hidden;");
		tmplPrcPg = SYS.fileRead(pgDir+"loginError.html");
	}
	//> Process include page template
	else if(tmplPg.length)
	{
		//>> Select page display
		treeEl = pgTree.getElementBy(firstEl);
		if(typeof(treeEl) == "XMLNode")
			treeEl.setAttr("class",treeEl.attr("class").replace(new RegExp("menu([^\"]*)"),"menuSel$1"));
	}
	else tmplPrcPg = SYS.fileRead(pgDir+"welcome.html");

	//> Status make
	pgTree.getElementBy("path").setText("/"+firstEl);
	pgTree.getElementBy("user_st").childIns(0,"span").setText(user+"("+(isRootUser?"суперпользователь":"пользователь")+")      ");

	//> Super user access
	if(isUser && !isRootUser)
	{
		pgTree.getElementBy("cfg").parent().setAttr("style","visibility : hidden;");
		pgTree.getElementBy("devices").parent().setAttr("style","visibility : hidden;");

		treeEl = SYS.XMLNode("get").setAttr("path","/sub_Security/usr_"+user+"/%2fprm%2fldscr");
		SYS.cntrReq(treeEl);
		//>> Report disable
		re=treeEl.text().match("Report:\\s+([^\\n]*)");
		if(!re.length || re[1] != "true") pgTree.getElementBy("report").parent().setAttr("style","visibility : hidden;");
		//>> Subsystem''s permition
		var catLs = SYS.XMLNode("get").setAttr("path","/DAQ/ModBus/ZegBee/%2fprm%2fprm");
		SYS.cntrReq(catLs);
		for(var i = 0; i < catLs.childSize(); i++)
		{
			re=treeEl.text().match("sub_"+catLs.childGet(i).attr("id")+":\\s+([^\\n]*)");
			if(!re.length || re[1] == "--") pgTree.getElementBy(catLs.childGet(i).attr("id")).parent().setAttr("style","visibility : hidden;");
		}
	}

	//> Save template
	page = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"+
			"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n"+
			pgTree.save();
	page = page.replace("#####CONTEXT#####",tmplPrcPg);
	return "200 OK";
}

//> Resources get process
if(HTTPreq == "GET")
{
	page = SYS.fileRead(pgDir+url);
	if(!page.length) { page = "Resource ''"+url+"'' call error!"; return "404 Not Found"; }
	//>> Extesion types processing
	var fext = url.lastIndexOf(".");
	fext = (fext > 0) ? url.slice(fext+1) : "";
	if(fext == "png" || fext == "jpg" || fext == "ico") HTTPvars["Content-Type"] = "image/"+fext;
	else if(fext == "css" || fext == "html" || fext == "xml") HTTPvars["Content-Type"] = "text/"+fext;
	else if(fext == "js") HTTPvars["Content-Type"] = "text/javascript";
	//SYS.messDebug("TEST Web","TEST 10: ''"+fext+"'' .");
	return "200 OK";
}

return "404 Not Found";','JavaLikeCalc.JavaScript
var pgDir = "Web/";
var firstEl = url.parsePath(0);
var procPg = false;
var tmplPg = "";
var tmplPrcPg = false;
var pgTree = false;
var treeEl = false;
var err = false;
var isUser = SYS.Security.access(user,4,"root","Web",0440);
var isRootUser = SYS.Security.access(user,4,"root","WebRoot",0440);

//> Pages requests process
if(!firstEl.length || (tmplPg=SYS.fileRead(pgDir+firstEl+".html")).length)
{
	//> Early call page''s script
	if(tmplPg.length)
	{
		procPg = SYS.DAQ.JavaLikeCalc.lib_web[(URLprms["script"].isEVal() ? firstEl : URLprms["script"])];
		if(typeof(procPg) == "TCntrNode")
		{
			tmplPrcPg = procPg.call(URLprms,tmplPg,cnts,user);
			//>> Check for full XML or resource request
			var poff = 0;
			if(tmplPrcPg.parse(0,"\n",poff) == "200 OK")
			{
				while((pEl=tmplPrcPg.parse(0,"\n",poff)).length) HTTPvars[pEl.parse(0,":")] = pEl.parse(1,":");
				page = tmplPrcPg.slice(poff);
				return "200 OK";
			}
		}
		else tmplPrcPg = tmplPg;
	}

	//> Parse template
	pgTree = SYS.XMLNode();
	err = pgTree.load(pgDir+"main.html",true,true);

	//> Page time place
	pgTree.getElementBy("time_vl").setAttr("value",SYS.time());
	var sessLife = SYS.XMLNode("get").setAttr("path","/sub_Protocol/mod_HTTP/%2fprm%2flf_tm");
	SYS.cntrReq(sessLife);
	pgTree.getElementBy("ses_vl").setAttr("value",sessLife.text().toInt()*60);

	//> Check access to WEB resource
	if(!user.length || !isUser)
	{
		pgTree.getElementBy("ses_vl").setAttr("value",10);
		pgTree.getElementBy("menu_l").setAttr("style","visibility : hidden;");
		pgTree.getElementBy("menu_r").setAttr("style","visibility : hidden;");
		pgTree.getElementBy("status").setAttr("style","visibility : hidden;");
		tmplPrcPg = SYS.fileRead(pgDir+"loginError.html");
	}
	//> Process include page template
	else if(tmplPg.length)
	{
		//>> Select page display
		treeEl = pgTree.getElementBy(firstEl);
		if(typeof(treeEl) == "XMLNode")
			treeEl.setAttr("class",treeEl.attr("class").replace(new RegExp("menu([^\"]*)"),"menuSel$1"));
	}
	else tmplPrcPg = SYS.fileRead(pgDir+"welcome.html");

	//> Status make
	pgTree.getElementBy("path").setText("/"+firstEl);
	pgTree.getElementBy("user_st").childIns(0,"span").setText(user+"("+(isRootUser?"суперпользователь":"пользователь")+")      ");

	//> Super user access
	if(isUser && !isRootUser)
	{
		pgTree.getElementBy("cfg").parent().setAttr("style","visibility : hidden;");
		pgTree.getElementBy("devices").parent().setAttr("style","visibility : hidden;");

		treeEl = SYS.XMLNode("get").setAttr("path","/sub_Security/usr_"+user+"/%2fprm%2fldscr");
		SYS.cntrReq(treeEl);
		//>> Report disable
		re=treeEl.text().match("Report:\\s+([^\\n]*)");
		if(!re.length || re[1] != "true") pgTree.getElementBy("report").parent().setAttr("style","visibility : hidden;");
		//>> Subsystem''s permition
		var catLs = SYS.XMLNode("get").setAttr("path","/DAQ/ModBus/ZegBee/%2fprm%2fprm");
		SYS.cntrReq(catLs);
		for(var i = 0; i < catLs.childSize(); i++)
		{
			re=treeEl.text().match("sub_"+catLs.childGet(i).attr("id")+":\\s+([^\\n]*)");
			if(!re.length || re[1] == "--") pgTree.getElementBy(catLs.childGet(i).attr("id")).parent().setAttr("style","visibility : hidden;");
		}
	}

	//> Save template
	page = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"+
			"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n"+
			pgTree.save();
	page = page.replace("#####CONTEXT#####",tmplPrcPg);
	return "200 OK";
}

//> Resources get process
if(HTTPreq == "GET")
{
	page = SYS.fileRead(pgDir+url);
	if(!page.length) { page = "Resource ''"+url+"'' call error!"; return "404 Not Found"; }
	//>> Extesion types processing
	var fext = url.lastIndexOf(".");
	fext = (fext > 0) ? url.slice(fext+1) : "";
	if(fext == "png" || fext == "jpg" || fext == "ico") HTTPvars["Content-Type"] = "image/"+fext;
	else if(fext == "css" || fext == "html" || fext == "xml") HTTPvars["Content-Type"] = "text/"+fext;
	else if(fext == "js") HTTPvars["Content-Type"] = "text/javascript";
	//SYS.messDebug("TEST Web","TEST 10: ''"+fext+"'' .");
	return "200 OK";
}

return "404 Not Found";',1377264714);
CREATE TABLE 'tmplib_base' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"MAXCALCTM" INTEGER DEFAULT '10' ,"PROGRAM" TEXT DEFAULT '' ,"uk#PROGRAM" TEXT DEFAULT '' ,"ru#PROGRAM" TEXT DEFAULT '' ,"TIMESTAMP" INTEGER DEFAULT '' , PRIMARY KEY ("ID"));
INSERT INTO "tmplib_base" VALUES('digAlarm','Alarm digital','Сигн. дискретна','Сигн. дискретная','Alarm from a digital parameter.','Сигналізація за дискретним параметром.','Сигнализация по дискретному параметру.',10,'JavaLikeCalc.JavaScript
f_err=(in==alrm_md)?"1:"+alrm_mess:"0";','JavaLikeCalc.JavaScript
f_err=(in==alrm_md)?"1:"+alrm_mess:"0";','JavaLikeCalc.JavaScript
f_err=(in==alrm_md)?"1:"+alrm_mess:"0";','');
INSERT INTO "tmplib_base" VALUES('simleBoard','Analog alarm by borders','Сигн. аналог. за границями','Сигн. аналог. по границам','The template of simple parameter included boders and dimension variable.','Шаблон простого параметру з перевіркою границь та одиницею виміру.','Шаблон простого параметра с проверкой границ и еденицей измерения.',10,'JavaLikeCalc.JavaScript
var=iMult*(in+iAdd);
if(var>max)			f_err="1:Upper work border violation";
else if(var<min)			f_err="2:Bottom work border violation";
else if(aMax>aMin && var>aMax)		f_err="3:Upper alarm border violation";
else if(aMax>aMin && var<aMin)		f_err="4:Bottom alarm border violation";
else if(wMax>wMin && var>wMax)	f_err="5:Upper warning border violation";
else if(wMax>wMin && var<wMin)	f_err="6:Bottom warning border violation";
else f_err="0";','JavaLikeCalc.JavaScript
var=iMult*(in+iAdd);
if(var>max)			f_err="1:Порушення верхньої робочої границі";
else if(var<min)			f_err="2:Порушення нижньої робочої границі";
else if(aMax>aMin && var>aMax)		f_err="3:Порушення верхньої аварійної границі";
else if(aMax>aMin && var<aMin)		f_err="4:Порушення нижньої аварійної границі";
else if(wMax>wMin && var>wMax)	f_err="5:Порушення верхньої попереджувальної границі";
else if(wMax>wMin && var<wMin)	f_err="6:Порушення нижньої попереджувальної границі";
else f_err="0";','JavaLikeCalc.JavaScript
var=iMult*(in+iAdd);
if(var>max)		f_err="1:Нарушение верхней рабочей границы";
else if(var<min)	f_err="2:Нарушение нижней рабочей границы";
else if(aMax>aMin && var>aMax)	f_err="3:Нарушение верхней аварийной границы";
else if(aMax>aMin && var<aMin)	f_err="4:Нарушение нижней аварийной границы";
else if(wMax>wMin && var>wMax)	f_err="5:Нарушение верхней предупредительной границы";
else if(wMax>wMin && var<wMin)	f_err="6:Нарушение нижней предупредительной границы";
else f_err="0";','');
INSERT INTO "tmplib_base" VALUES('digitBlock','Diskret parameters block','Блок дискр. параметрів','Блок дискр. параметров','The block for union of Diskret parameters for one device control.','Блок для збору дискретних параметрів, керуючих одним апаратом.','Блок для сборки дискретных параметров управляющих одним аппаратом.',10,'JavaLikeCalc.JavaScript
set=false;
if(cmdOpen && last_cmd!=1) { last_cmd=1; set=true; }
if(cmdClose && last_cmd!=2) { last_cmd=2; set=true; }
if(cmdStop && last_cmd!=3) { last_cmd=3; set=true; }
if(set && tCmd>0) w_tm=tCmd;
if(w_tm>0) w_tm-=1./f_frq;
else
{
  w_tm=0;
  if(tCmd>0)
  {
    if(last_cmd==1) cmdOpen=false;
    if(last_cmd==2) cmdClose=false;
    if(last_cmd==3) cmdStop=false;
  }
}','JavaLikeCalc.JavaScript
set=false;
if(cmdOpen && last_cmd!=1) { last_cmd=1; set=true; }
if(cmdClose && last_cmd!=2) { last_cmd=2; set=true; }
if(cmdStop && last_cmd!=3) { last_cmd=3; set=true; }
if(set && tCmd>0) w_tm=tCmd;
if(w_tm>0) w_tm-=1./f_frq;
else
{
  w_tm=0;
  if(tCmd>0)
  {
    if(last_cmd==1) cmdOpen=false;
    if(last_cmd==2) cmdClose=false;
    if(last_cmd==3) cmdStop=false;
  }
}','JavaLikeCalc.JavaScript
set=false;
if(cmdOpen && last_cmd!=1) { last_cmd=1; set=true; }
if(cmdClose && last_cmd!=2) { last_cmd=2; set=true; }
if(cmdStop && last_cmd!=3) { last_cmd=3; set=true; }
if(set && tCmd>0) w_tm=tCmd;
if(w_tm>0) w_tm-=1./f_frq;
else
{
  w_tm=0;
  if(tCmd>0)
  {
    if(last_cmd==1) cmdOpen=false;
    if(last_cmd==2) cmdClose=false;
    if(last_cmd==3) cmdStop=false;
  }
}','');
INSERT INTO "tmplib_base" VALUES('gasPoint','Flow control point','Витрато-вимірювальний вузол','Расходомерный узел',' ',' ','',10,'JavaLikeCalc.JavaScript
F=200+(rand(5)-2.5);
Q+=F/3600;
P=9+(rand(2)-1);
T=15+(rand(2)-1);
dP=F/33;
DS=1+(rand(0.2)-0.1);
','JavaLikeCalc.JavaScript
F=200+(rand(5)-2.5);
Q+=F/3600;
P=9+(rand(2)-1);
T=15+(rand(2)-1);
dP=F/33;
DS=1+(rand(0.2)-0.1);
','JavaLikeCalc.JavaScript
F=200+(rand(5)-2.5);
Q+=F/3600;
P=9+(rand(2)-1);
T=15+(rand(2)-1);
dP=F/33;
DS=1+(rand(0.2)-0.1);
','');
INSERT INTO "tmplib_base" VALUES('manInUnif','Manual input (Unif)','Ручний ввід (Уніф)','Ручной ввод (Униф)','Unified template for manual input signals.','Уніфікований шаблон для ручного вводу значень сигналів.','Унифицированный шаблон ручного ввода сигнала.',10,'JavaLikeCalc.JavaScript
if(f_start)	f_err = "0";

pMax = plcMax; pMin = plcMin;	//Copy for local modifies using
if(passIn=(pMax==pMin)) { pMax = max/iMult - iAdd; pMin = min/iMult - iAdd; }

//Manual input set process
if(!varIn.isEVal() && (varIn != var || inout.isEVal()))
{
	if(passIn) { pMin = iMult*(pMin+iAdd); pMax = iMult*(pMax+iAdd); }
	vCalibr = (varIn-min)/(max-min);
	if(scSqr)	vCalibr = pow(vCalibr,2);
	if(pMax < pMin) vCalibr = 1-vCalibr;
	vCalibr = min(pMax,pMin)+vCalibr*abs(pMax-pMin);
	inout = vCalibr/iMult - iAdd;
}

levErr = 0;
tErr = "0";
//Input data check and postprocess
if(inout > (max(pMax,pMin)+plcExcess*abs(pMax-pMin)/100)) {
	tErr = "1:The signal exceed to upper hardware border"; levErr = -5;
	var = max + plcExcess*(max-min)/100;
}
else if(inout < (min(pMax,pMin)-plcExcess*abs(pMax-pMin)/100)) {
	tErr = "2:The signal exceed to bottom hardware border"; levErr = -5;
	var = min - plcExcess*(max-min)/100;
}
if(!tErr) {
	vCalibr = iMult*(inout+iAdd);
	if(passIn) { pMin = iMult*(pMin+iAdd); pMax = iMult*(pMax+iAdd); }
	if(!passIn || scSqr) {
		vCalibr = (vCalibr-min(pMax,pMin))/abs(pMax-pMin);
		if(pMax < pMin) vCalibr = 1-vCalibr;
		vCalibr = min + (max-min)*(scSqr?pow(vCalibr,0.5):vCalibr);
	}
	if(var.isEVal())	var = vCalibr;
	varDt = vCalibr - var;
	var += varDt/max(1,Tf*f_frq);
	varIn = var;

	bndVarHyst = (max-min)*HystBnd/100;
	if(aMax < max && aMax > aMin && (var >= aMax || (f_err.toInt() == 3 && var >= (aMax-bndVarHyst))))
	{ tErr="3:Upper alarm border error"; levErr = -4; }
	else if(aMin > min && aMax > aMin && (var <= aMin || (f_err.toInt() == 4 && var <= (aMin+bndVarHyst))))
	{ tErr="4:Nether alarm border error"; levErr = -4; }
	else if(wMax < max && wMax > wMin && (var >= wMax || (f_err.toInt() == 5 && var >= (wMax-bndVarHyst))))
	{ tErr="5:Upper warning border error"; levErr = -2; }
	else if(wMin > min && wMax > wMin && (var <= wMin || (f_err.toInt() == 6 && var <= (wMin+bndVarHyst))))
	{ tErr="6:Nether warning border error"; levErr = -2; }
	else if(speed && varDt > speed)	{ tErr="7:Too big parameter''s motion speed"; levErr = -2; }
}

//Alarms forming
if(alSup)	f_err = "0";
else {
	if(tErr.toInt() && tErr.toInt() != f_err.toInt())
		this.nodePrev().alarmSet((NAME.length?NAME:SHIFR)+": "+DESCR+": "+tErr.parse(1,":"), levErr, SHIFR);
	else if(f_err.toInt() && !tErr.toInt())
		this.nodePrev().alarmSet((NAME.length?NAME:SHIFR)+": "+DESCR+": NORMA", 1, SHIFR);
	f_err = tErr;
}','JavaLikeCalc.JavaScript
if(f_start)	f_err = "0";

pMax = plcMax; pMin = plcMin;	//Copy for local modifies using
if(passIn=(pMax==pMin)) { pMax = max/iMult - iAdd; pMin = min/iMult - iAdd; }

//Manual input set process
if(!varIn.isEVal() && (varIn != var || inout.isEVal()))
{
	if(passIn) { pMin = iMult*(pMin+iAdd); pMax = iMult*(pMax+iAdd); }
	vCalibr = (varIn-min)/(max-min);
	if(scSqr)	vCalibr = pow(vCalibr,2);
	if(pMax < pMin) vCalibr = 1-vCalibr;
	vCalibr = min(pMax,pMin)+vCalibr*abs(pMax-pMin);
	inout = vCalibr/iMult - iAdd;
}

levErr = 0;
tErr = "0";
//Input data check and postprocess
if(inout > (max(pMax,pMin)+plcExcess*abs(pMax-pMin)/100)) {
	tErr = "1:Сигнал перевищив верхню апаратну границю"; levErr = -5;
	var = max + plcExcess*(max-min)/100;
}
else if(inout < (min(pMax,pMin)-plcExcess*abs(pMax-pMin)/100)) {
	tErr = "2:Сигнал перевищив нижню апаратну границю"; levErr = -5;
	var = min - plcExcess*(max-min)/100;
}
if(!tErr) {
	vCalibr = iMult*(inout+iAdd);
	if(passIn) { pMin = iMult*(pMin+iAdd); pMax = iMult*(pMax+iAdd); }
	if(!passIn || scSqr) {
		vCalibr = (vCalibr-min(pMax,pMin))/abs(pMax-pMin);
		if(pMax < pMin) vCalibr = 1-vCalibr;
		vCalibr = min + (max-min)*(scSqr?pow(vCalibr,0.5):vCalibr);
	}
	if(var.isEVal())	var = vCalibr;
	varDt = vCalibr - var;
	var += varDt/max(1,Tf*f_frq);
	varIn = var;

	bndVarHyst = (max-min)*HystBnd/100;
	if(aMax < max && aMax > aMin && (var >= aMax || (f_err.toInt() == 3 && var >= (aMax-bndVarHyst))))
	{ tErr="3:Помилка верхньої аварійної границі"; levErr = -4; }
	else if(aMin > min && aMax > aMin && (var <= aMin || (f_err.toInt() == 4 && var <= (aMin+bndVarHyst))))
	{ tErr="4:Помилка нижньої аварійної границі"; levErr = -4; }
	else if(wMax < max && wMax > wMin && (var >= wMax || (f_err.toInt() == 5 && var >= (wMax-bndVarHyst))))
	{ tErr="5:Помилка верхньої поперджув. границі"; levErr = -2; }
	else if(wMin > min && wMax > wMin && (var <= wMin || (f_err.toInt() == 6 && var <= (wMin+bndVarHyst))))
	{ tErr="6:Помилка нижньої поперджув. границі"; levErr = -2; }
	else if(speed && varDt > speed)	{ tErr="7:Дуже велика швидкість зміни параметру"; levErr = -2; }
}

//Alarms forming
if(alSup)	f_err = "0";
else {
	if(tErr.toInt() && tErr.toInt() != f_err.toInt())
		this.nodePrev().alarmSet((NAME.length?NAME:SHIFR)+": "+DESCR+": "+tErr.parse(1,":"), levErr, SHIFR);
	else if(f_err.toInt() && !tErr.toInt())
		this.nodePrev().alarmSet((NAME.length?NAME:SHIFR)+": "+DESCR+": НОРМА", 1, SHIFR);
	f_err = tErr;
}','JavaLikeCalc.JavaScript
if(f_start)	f_err = "0";

pMax = plcMax; pMin = plcMin;	//Copy for local modifies using
if(passIn=(pMax==pMin)) { pMax = max/iMult - iAdd; pMin = min/iMult - iAdd; }

//Manual input set process
if(!varIn.isEVal() && (varIn != var || inout.isEVal()))
{
	if(passIn) { pMin = iMult*(pMin+iAdd); pMax = iMult*(pMax+iAdd); }
	vCalibr = (varIn-min)/(max-min);
	if(scSqr)	vCalibr = pow(vCalibr,2);
	if(pMax < pMin) vCalibr = 1-vCalibr;
	vCalibr = min(pMax,pMin)+vCalibr*abs(pMax-pMin);
	inout = vCalibr/iMult - iAdd;
}

levErr = 0;
tErr = "0";
//Input data check and postprocess
if(inout > (max(pMax,pMin)+plcExcess*abs(pMax-pMin)/100)) {
	tErr = "1:Выход сигнала за верхнюю аппаратную границу"; levErr = -5;
	var = max + plcExcess*(max-min)/100;
}
else if(inout < (min(pMax,pMin)-plcExcess*abs(pMax-pMin)/100)) {
	tErr = "2:Выход сигнала за нижнюю аппаратную границу"; levErr = -5;
	var = min - plcExcess*(max-min)/100;
}
if(!tErr) {
	vCalibr = iMult*(inout+iAdd);
	if(passIn) { pMin = iMult*(pMin+iAdd); pMax = iMult*(pMax+iAdd); }
	if(!passIn || scSqr) {
		vCalibr = (vCalibr-min(pMax,pMin))/abs(pMax-pMin);
		if(pMax < pMin) vCalibr = 1-vCalibr;
		vCalibr = min + (max-min)*(scSqr?pow(vCalibr,0.5):vCalibr);
	}
	if(var.isEVal())	var = vCalibr;
	varDt = vCalibr - var;
	var += varDt/max(1,Tf*f_frq);
	varIn = var;

	bndVarHyst = (max-min)*HystBnd/100;
	if(aMax < max && aMax > aMin && (var >= aMax || (f_err.toInt() == 3 && var >= (aMax-bndVarHyst))))
	{ tErr="3:Нарушение верхней аварийной границы"; levErr = -4; }
	else if(aMin > min && aMax > aMin && (var <= aMin || (f_err.toInt() == 4 && var <= (aMin+bndVarHyst))))
	{ tErr="4:Нарушение нижней аварийной границы"; levErr = -4; }
	else if(wMax < max && wMax > wMin && (var >= wMax || (f_err.toInt() == 5 && var >= (wMax-bndVarHyst))))
	{ tErr="5:Нарушение верхней предупредительной границы"; levErr = -2; }
	else if(wMin > min && wMax > wMin && (var <= wMin || (f_err.toInt() == 6 && var <= (wMin+bndVarHyst))))
	{ tErr="6:Нарушение нижней предупредительной границы"; levErr = -2; }
	else if(speed && varDt > speed)	{ tErr="7:Очень большая скорость изменения параметра"; levErr = -2; }
}

//Alarms forming
if(alSup)	f_err = "0";
else {
	if(tErr.toInt() && tErr.toInt() != f_err.toInt())
		this.nodePrev().alarmSet((NAME.length?NAME:SHIFR)+": "+DESCR+": "+tErr.parse(1,":"), levErr, SHIFR);
	else if(f_err.toInt() && !tErr.toInt())
		this.nodePrev().alarmSet((NAME.length?NAME:SHIFR)+": "+DESCR+": НОРМА", 1, SHIFR);
	f_err = tErr;
}',1402043101);
INSERT INTO "tmplib_base" VALUES('anUnif','Analog sign. (Unif)','Аналог. сигнал (Уніф)','Аналог. сигн. (Униф)','Unified template for analog input signals processing.','Уніфікований шаблон для обробки аналогових вхідних сигналів.','Унифицированный шаблон обработки аналогового сигнала.',10,'JavaLikeCalc.JavaScript
if(f_start) {
	f_err = "0";
	prevVar = EVAL_REAL;
	//Prepare data for preprocessing
	inPrcLng = "JavaLikeCalc.JavaScript";
	inPrcArgs = new Object();
	inPrcArgs.this = this;
}
pMax = plcMax; pMin = plcMin;	//Copy for local modifies using
if(passIn=(pMax==pMin)) { pMax = max/iMult - iAdd; pMin = min/iMult - iAdd; }

if(plcImit) {	//Data imitation
  if(!plcImitIn.isEVal()) in = plcImitIn;
  else {
    plcDif = abs(pMax-pMin);
    in = pMin + plcDif/2 + rand(plcDif/10)-plcDif/20;
  }
}

//Call specific preprocessing procedure
if(inProc.length)	{
	inPrcArgs.in = in; inPrcArgs.min = min; inPrcArgs.max = max;
	inPrcArgs.plcMin = pMin; inPrcArgs.plcMax = pMax;
	inPrcArgs.plcImit = plcImit; inPrcArgs.plcImitIn = plcImitIn;
	SYS.DAQ.funcCall(inPrcLng, inPrcArgs, inProc);
	in = inPrcArgs.in;
}

levErr = 0;
tErr = "0";
//Input data check and postprocess
if(in.isEVal()) {
	tErr = "1:No data or connection with source"; levErr = -5;
	var = EVAL_REAL;
	if(subMode == 1) var = prevVar;
	else if(subMode == 2) var = subVar;
}
else if(in > (max(pMax,pMin)+plcExcess*abs(pMax-pMin)/100)) {
	tErr = "1:The signal exceed to upper hardware border"; levErr = -5;
	var = EVAL_REAL;
	if(subMode == 1) var = prevVar.isEVal() ? max+plcExcess*(max-min)/100 : prevVar;
	else if(subMode == 2) var = subVar;
}
else if(in < (min(pMax,pMin)-plcExcess*abs(pMax-pMin)/100)) {
	tErr = "2:The signal exceed to bottom hardware border"; levErr = -5;
	var = EVAL_REAL;
	if(subMode == 1) var = prevVar.isEVal() ? min-plcExcess*(max-min)/100 : prevVar;
	else if(subMode == 2) var = subVar;
}
if(!tErr) {
	vCalibr = iMult*(in+iAdd);
	if(passIn) { pMin = iMult*(pMin+iAdd); pMax = iMult*(pMax+iAdd); }
	if(!passIn || scSqr) {
		vCalibr = (vCalibr-min(pMax,pMin))/abs(pMax-pMin);
		if(pMax < pMin) vCalibr = 1-vCalibr;
		vCalibr = min + (max-min)*(scSqr?pow(vCalibr,0.5):vCalibr);
	}
	if(var.isEVal())	var = vCalibr;
	varDt = vCalibr - var;
	var += varDt/max(1,Tf*f_frq);
	prevVar = var;

	bndVarHyst = (max-min)*HystBnd/100;
	if(aMax < max && aMax > aMin && (var >= aMax || (f_err.toInt() == 3 && var >= (aMax-bndVarHyst))))
	{ tErr="3:Upper alarm border error"; levErr = -4; }
	else if(aMin > min && aMax > aMin && (var <= aMin || (f_err.toInt() == 4 && var <= (aMin+bndVarHyst))))
	{ tErr="4:Nether alarm border error"; levErr = -4; }
	else if(wMax < max && wMax > wMin && (var >= wMax || (f_err.toInt() == 5 && var >= (wMax-bndVarHyst))))
	{ tErr="5:Upper warning border error"; levErr = -2; }
	else if(wMin > min && wMax > wMin && (var <= wMin || (f_err.toInt() == 6 && var <= (wMin+bndVarHyst))))
	{ tErr="6:Nether warning border error"; levErr = -2; }
	else if(speed && varDt > speed)	{ tErr="7:Too big parameter''s motion speed"; levErr = -2; }
}

//Alarms forming
if(alSup)	f_err = "0";
else {
	if(tErr.toInt() && tErr.toInt() != f_err.toInt())
		this.nodePrev().alarmSet((NAME.length?NAME:SHIFR)+": "+DESCR+": "+tErr.parse(1,":"), levErr, SHIFR);
	else if(f_err.toInt() && !tErr.toInt())
		this.nodePrev().alarmSet((NAME.length?NAME:SHIFR)+": "+DESCR+": NORMA", 1, SHIFR);
	f_err = tErr;
}','JavaLikeCalc.JavaScript
if(f_start) {
	f_err = "0";
	prevVar = EVAL_REAL;
	//Prepare data for preprocessing
	inPrcLng = "JavaLikeCalc.JavaScript";
	inPrcArgs = new Object();
	inPrcArgs.this = this;
}
pMax = plcMax; pMin = plcMin;	//Copy for local modifies using
if(passIn=(pMax==pMin)) { pMax = max/iMult - iAdd; pMin = min/iMult - iAdd; }

if(plcImit) {	//Data imitation
  if(!plcImitIn.isEVal()) in = plcImitIn;
  else {
    plcDif = abs(pMax-pMin);
    in = pMin + plcDif/2 + rand(plcDif/10)-plcDif/20;
	}
}

//Call specific preprocessing procedure
if(inProc.length)	{
	inPrcArgs.in = in; inPrcArgs.min = min; inPrcArgs.max = max;
	inPrcArgs.plcMin = pMin; inPrcArgs.plcMax = pMax;
	inPrcArgs.plcImit = plcImit; inPrcArgs.plcImitIn = plcImitIn;
	SYS.DAQ.funcCall(inPrcLng, inPrcArgs, inProc);
	in = inPrcArgs.in;
}

levErr = 0;
tErr = "0";
//Input data check and postprocess
if(in.isEVal()) {
	tErr = "1:Немає даних або зв''язку із джерелом"; levErr = -5;
	var = EVAL_REAL;
	if(subMode == 1) var = prevVar;
	else if(subMode == 2) var = subVar;
}
else if(in > (max(pMax,pMin)+plcExcess*abs(pMax-pMin)/100)) {
	tErr = "1:Сигнал перевищив верхню апаратну границю"; levErr = -5;
	var = EVAL_REAL;
	if(subMode == 1) var = prevVar.isEVal() ? max+plcExcess*(max-min)/100 : prevVar;
	else if(subMode == 2) var = subVar;
}
else if(in < (min(pMax,pMin)-plcExcess*abs(pMax-pMin)/100)) {
	tErr = "2:Сигнал перевищив нижню апаратну границю"; levErr = -5;
	var = EVAL_REAL;
	if(subMode == 1) var = prevVar.isEVal() ? min-plcExcess*(max-min)/100 : prevVar;
	else if(subMode == 2) var = subVar;
}
if(!tErr) {
	vCalibr = iMult*(in+iAdd);
	if(passIn) { pMin = iMult*(pMin+iAdd); pMax = iMult*(pMax+iAdd); }
	if(!passIn || scSqr) {
		vCalibr = (vCalibr-min(pMax,pMin))/abs(pMax-pMin);
		if(pMax < pMin) vCalibr = 1-vCalibr;
		vCalibr = min + (max-min)*(scSqr?pow(vCalibr,0.5):vCalibr);
	}
	if(var.isEVal())	var = vCalibr;
	varDt = vCalibr - var;
	var += varDt/max(1,Tf*f_frq);
	prevVar = var;

	bndVarHyst = (max-min)*HystBnd/100;
	if(aMax < max && aMax > aMin && (var >= aMax || (f_err.toInt() == 3 && var >= (aMax-bndVarHyst))))
	{ tErr = "3:Помилка верхньої аварійної границі"; levErr = -4; }
	else if(aMin > min && aMax > aMin && (var <= aMin || (f_err.toInt() == 4 && var <= (aMin+bndVarHyst))))
	{ tErr = "4:Помилка нижньої аварійної границі"; levErr = -4; }
	else if(wMax < max && wMax > wMin && (var >= wMax || (f_err.toInt() == 5 && var >= (wMax-bndVarHyst))))
	{ tErr = "5:Помилка верхньої поперджув. границі"; levErr = -2; }
	else if(wMin > min && wMax > wMin && (var <= wMin || (f_err.toInt() == 6 && var <= (wMin+bndVarHyst))))
	{ tErr = "6:Помилка нижньої поперджув. границі"; levErr = -2; }
	else if(speed && varDt > speed)	{ tErr = "7:Дуже велика швидкість зміни параметру"; levErr = -2; }
}

//Alarms forming
if(alSup)	f_err = "0";
else {
	if(tErr.toInt() && tErr.toInt() != f_err.toInt())
		this.nodePrev().alarmSet((NAME.length?NAME:SHIFR)+": "+DESCR+": "+tErr.parse(1,":"), levErr, SHIFR);
	else if(f_err.toInt() && !tErr.toInt())
		this.nodePrev().alarmSet((NAME.length?NAME:SHIFR)+": "+DESCR+": НОРМА", 1, SHIFR);
	f_err = tErr;
}','JavaLikeCalc.JavaScript
if(f_start) {
	f_err = "0";
	prevVar = EVAL_REAL;
	//Prepare data for preprocessing
	inPrcLng = "JavaLikeCalc.JavaScript";
	inPrcArgs = new Object();
	inPrcArgs.this = this;
}
pMax = plcMax; pMin = plcMin;	//Copy for local modifies using
if(passIn=(pMax==pMin)) { pMax = max/iMult - iAdd; pMin = min/iMult - iAdd; }

if(plcImit) {	//Data imitation
	if(!plcImitIn.isEVal()) in = plcImitIn;
	else {
		plcDif = abs(pMax-pMin);
		in = pMin + plcDif/2 + rand(plcDif/10)-plcDif/20;
	}
}

//Call specific preprocessing procedure
if(inProc.length)	{
	inPrcArgs.in = in; inPrcArgs.min = min; inPrcArgs.max = max;
	inPrcArgs.plcMin = pMin; inPrcArgs.plcMax = pMax;
	inPrcArgs.plcImit = plcImit; inPrcArgs.plcImitIn = plcImitIn;
	SYS.DAQ.funcCall(inPrcLng, inPrcArgs, inProc);
	in = inPrcArgs.in;
}

levErr = 0;
tErr = "0";
//Input data check and postprocess
if(in.isEVal()) {
	tErr = "1:Нет данных или связи с источником"; levErr = -5;
	var = EVAL_REAL;
	if(subMode == 1) var = prevVar;
	else if(subMode == 2) var = subVar;
}
else if(in > (max(pMax,pMin)+plcExcess*abs(pMax-pMin)/100)) {
	tErr = "1:Выход сигнала за верхнюю аппаратную границу"; levErr = -5;
	var = EVAL_REAL;
	if(subMode == 1) var = prevVar.isEVal() ? max+plcExcess*(max-min)/100 : prevVar;
	else if(subMode == 2) var = subVar;
}
else if(in < (min(pMax,pMin)-plcExcess*abs(pMax-pMin)/100)) {
	tErr = "2:Выход сигнала за нижнюю аппаратную границу"; levErr = -5;
	var = EVAL_REAL;
	if(subMode == 1) var = prevVar.isEVal() ? min-plcExcess*(max-min)/100 : prevVar;
	else if(subMode == 2) var = subVar;
}
if(!tErr) {
	vCalibr = iMult*(in+iAdd);
	if(passIn) { pMin = iMult*(pMin+iAdd); pMax = iMult*(pMax+iAdd); }
	if(!passIn || scSqr) {
		vCalibr = (vCalibr-min(pMax,pMin))/abs(pMax-pMin);
		if(pMax < pMin) vCalibr = 1-vCalibr;
		vCalibr = min + (max-min)*(scSqr?pow(vCalibr,0.5):vCalibr);
	}
	if(var.isEVal())	var = vCalibr;
	varDt = vCalibr - var;
	var += varDt/max(1,Tf*f_frq);
	prevVar = var;

	bndVarHyst = (max-min)*HystBnd/100;
	if(aMax < max && aMax > aMin && (var >= aMax || (f_err.toInt() == 3 && var >= (aMax-bndVarHyst))))
	{ tErr = "3:Нарушение верхней аварийной границы"; levErr = -4; }
	else if(aMin > min && aMax > aMin && (var <= aMin || (f_err.toInt() == 4 && var <= (aMin+bndVarHyst))))
	{ tErr = "4:Нарушение нижней аварийной границы"; levErr = -4; }
	else if(wMax < max && wMax > wMin && (var >= wMax || (f_err.toInt() == 5 && var >= (wMax-bndVarHyst))))
	{ tErr = "5:Нарушение верхней предупредительной границы"; levErr = -2; }
	else if(wMin > min && wMax > wMin && (var <= wMin || (f_err.toInt() == 6 && var <= (wMin+bndVarHyst))))
	{ tErr = "6:Нарушение нижней предупредительной границы"; levErr = -2; }
	else if(speed && varDt > speed)	{ tErr = "7:Очень большая скорость изменения параметра"; levErr = -2; }
}

//Alarms forming
if(alSup)	f_err = "0";
else {
	if(tErr.toInt() && tErr.toInt() != f_err.toInt())
		this.nodePrev().alarmSet((NAME.length?NAME:SHIFR)+": "+DESCR+": "+tErr.parse(1,":"), levErr, SHIFR);
	else if(f_err.toInt() && !tErr.toInt())
		this.nodePrev().alarmSet((NAME.length?NAME:SHIFR)+": "+DESCR+": НОРМА", 1, SHIFR);
	f_err = tErr;
}',1402043014);
INSERT INTO "tmplib_base" VALUES('digitBlockUnif','Diskret block (Unif)','Блок дискретних (Уніф)','Блок дискр. (Униф)','The block for union of Diskret parameters for one device control.','Блок поєднання дискретних сигналів контролю одним пристроєм.','Блок для дискретных параметров управляющих одним аппаратом.',10,'JavaLikeCalc.JavaScript
set=false;
if( com != EVAL_BOOL && com && last_cmd!=1 ) { last_cmd=1; set=true; }
if( close != EVAL_BOOL && close && last_cmd!=2 ) { last_cmd=2; set=true; }
if( stop != EVAL_BOOL && stop && last_cmd!=3 ) { last_cmd=3; set=true; }
if( set && tCmd > 0 ) w_tm = tCmd;
if( w_tm > 0 ) w_tm -= 1./f_frq;
else
{
  w_tm=0;
  if( tCmd > 0 )
  {
    if( last_cmd==1 ) com = false;
    if( last_cmd==2 ) close = false;
    if( last_cmd==3 ) stop = false;
    last_cmd = 0;
  }
}','JavaLikeCalc.JavaScript
set=false;
if( com != EVAL_BOOL && com && last_cmd!=1 ) { last_cmd=1; set=true; }
if( close != EVAL_BOOL && close && last_cmd!=2 ) { last_cmd=2; set=true; }
if( stop != EVAL_BOOL && stop && last_cmd!=3 ) { last_cmd=3; set=true; }
if( set && tCmd > 0 ) w_tm = tCmd;
if( w_tm > 0 ) w_tm -= 1./f_frq;
else
{
  w_tm=0;
  if( tCmd > 0 )
  {
    if( last_cmd==1 ) com = false;
    if( last_cmd==2 ) close = false;
    if( last_cmd==3 ) stop = false;
    last_cmd = 0;
  }
}','JavaLikeCalc.JavaScript
set=false;
if( com != EVAL_BOOL && com && last_cmd!=1 ) { last_cmd=1; set=true; }
if( close != EVAL_BOOL && close && last_cmd!=2 ) { last_cmd=2; set=true; }
if( stop != EVAL_BOOL && stop && last_cmd!=3 ) { last_cmd=3; set=true; }
if( set && tCmd > 0 ) w_tm = tCmd;
if( w_tm > 0 ) w_tm -= 1./f_frq;
else
{
  w_tm=0;
  if( tCmd > 0 )
  {
    if( last_cmd==1 ) com = false;
    if( last_cmd==2 ) close = false;
    if( last_cmd==3 ) stop = false;
    last_cmd = 0;
  }
}','');
INSERT INTO "tmplib_base" VALUES('pidUnifImp','Impulse PID sign. (Unif, stats)','Імпульсний ПІД сигнал (Уніф, стани)','ПИД импульсный сигн. (Униф, состояния)','The unified template for process analog signals with properties impulse PID.','Уніфікований шаблон для обробки аналогового сигналу з властивостями імпульсного ПІД.','Унифицированный шаблон обработки аналогового сигнала со свойствами импульсного ПИД.',10,'JavaLikeCalc.JavaScript
if(f_start) f_err = "0";

if(plcImit)
{
	if(plcImitIn != EVAL_REAL) in = plcImitIn;
	else
	{
		plcDif = plcMax-plcMin;
		in = plcMin + plcDif/2 + rand(plcDif/10)-plcDif/20;
	}
}

tErr = "0";
if( in > (plcMax+plcExcess*(plcMax-plcMin)/100) )
{ tErr = "1:The signal exceed to upper hardware border"; var = max+plcExcess*(max-min)/100;}
else if( in < (plcMin-plcExcess*(plcMax-plcMin)/100) )
{ tErr = "2:The signal exceed to bottom hardware border"; var = min-plcExcess*(max-min)/100;}
if( tErr ) 
{
  //var = dmsk ? max+plcExcess*(max-min)/100 : min-plcExcess*(max-min)/100;
  EVAL = true;
}
else
{
	vCalibr = iMult*(in+iAdd);

	vCalibr = (vCalibr-plcMin)/(plcMax-plcMin);
	varDt = min+(max-min)*(scSqr?pow(vCalibr,0.5):vCalibr)-var;
	var += varDt/max(1,Tf/1000*f_frq);

	bndVarHyst = (aMax-aMin)*HystBnd/100;
	if( aMax>aMin && (var>=aMax || (HH && var>=(aMax-bndVarHyst))) )
	{ tErr="3:Upper alarm border error"; HH=true; EVAL=H=LL=L=false; }
	else if( aMax>aMin && (var<=aMin || (LL && var<=(aMin+bndVarHyst))) )
	{ tErr="4:Bottom alarm border error"; LL=true; EVAL=HH=H=L=false; }
	else if( wMax>wMin && (var>=wMax || (H && var>=(wMax-bndVarHyst))) )
	{ tErr="5:Upper warning border error"; H=true; EVAL=HH=LL=L=false; }
	else if( wMax>wMin && (var<=wMin || (L && var<=(wMin+bndVarHyst))) )
	{ tErr="6:Bottom warning border error"; L=true; EVAL=HH=H=LL=false; }
	else if( speed && varDt > speed ) { tErr="7:Too big parameter''s motion speed";   EVAL=true; HH=H=LL=L=false; }
	else EVAL=HH=H=LL=L=false;
}

if(!f_err.toInt() && tErr.toInt()) this.nodePrev().alarmSet((NAME.length?NAME:SHIFR)+": "+DESCR+": "+tErr.parse(1,":"), -4, SHIFR);
else if(f_err.toInt() && !tErr.toInt()) this.nodePrev().alarmSet((NAME.length?NAME:SHIFR)+": "+DESCR+": NORMA", 1, SHIFR);
f_err = tErr;','JavaLikeCalc.JavaScript
if(f_start) f_err = "0";

if(plcImit)
{
	if(plcImitIn != EVAL_REAL) in = plcImitIn;
	else
	{
		plcDif = plcMax-plcMin;
		in = plcMin + plcDif/2 + rand(plcDif/10)-plcDif/20;
	}
}

tErr = "0";
if( in > (plcMax+plcExcess*(plcMax-plcMin)/100) )
{ tErr = "1:Сигнал перевищив верхню апаратну границю"; var = max+plcExcess*(max-min)/100;}
else if( in < (plcMin-plcExcess*(plcMax-plcMin)/100) )
{ tErr = "2:Сигнал перевищив нижню апаратну границю"; var = min-plcExcess*(max-min)/100;}
if( tErr ) 
{
  //var = dmsk ? max+plcExcess*(max-min)/100 : min-plcExcess*(max-min)/100;
  EVAL = true;
}
else
{
	vCalibr = iMult*(in+iAdd);

	vCalibr = (vCalibr-plcMin)/(plcMax-plcMin);
	varDt = min+(max-min)*(scSqr?pow(vCalibr,0.5):vCalibr)-var;
	var += varDt/max(1,Tf/1000*f_frq);

	bndVarHyst = (aMax-aMin)*HystBnd/100;
	if( aMax>aMin && (var>=aMax || (HH && var>=(aMax-bndVarHyst))) )
	{ tErr="3:Помилка верхньої границі аварії"; HH=true; EVAL=H=LL=L=false; }
	else if( aMax>aMin && (var<=aMin || (LL && var<=(aMin+bndVarHyst))) )
	{ tErr="4:Помилка нижньої границі аварії"; LL=true; EVAL=HH=H=L=false; }
	else if( wMax>wMin && (var>=wMax || (H && var>=(wMax-bndVarHyst))) )
	{ tErr="5:Помилка верхньої границі попередження"; H=true; EVAL=HH=LL=L=false; }
	else if( wMax>wMin && (var<=wMin || (L && var<=(wMin+bndVarHyst))) )
	{ tErr="6:Помилка нижньої границі попередження"; L=true; EVAL=HH=H=LL=false; }
	else if( speed && varDt > speed ) { tErr="7:Дуже велика швидкість зміни параметру";   EVAL=true; HH=H=LL=L=false; }
	else EVAL=HH=H=LL=L=false;
}

if(!f_err.toInt() && tErr.toInt()) this.nodePrev().alarmSet((NAME.length?NAME:SHIFR)+": "+DESCR+": "+tErr.parse(1,":"), -4, SHIFR);
else if(f_err.toInt() && !tErr.toInt()) this.nodePrev().alarmSet((NAME.length?NAME:SHIFR)+": "+DESCR+": NORMA", 1, SHIFR);
f_err = tErr;','JavaLikeCalc.JavaScript
if(f_start) f_err = "0";

if(plcImit)
{
	if(plcImitIn != EVAL_REAL) in = plcImitIn;
	else
	{
		plcDif = plcMax-plcMin;
		in = plcMin + plcDif/2 + rand(plcDif/10)-plcDif/20;
	}
}

tErr = "0";
if(in > (plcMax+plcExcess*(plcMax-plcMin)/100))
{ tErr = "1:Выход сигнала за верхнюю аппаратную границу"; var = max+plcExcess*(max-min)/100;}
else if(in < (plcMin-plcExcess*(plcMax-plcMin)/100))
{ tErr = "2:Выход сигнала за нижнюю аппаратную границу"; var = min-plcExcess*(max-min)/100;}
if(tErr)
{
	//var = dmsk ? max+plcExcess*(max-min)/100 : min-plcExcess*(max-min)/100;
	EVAL = true;
}
else
{
	vCalibr = iMult*(in+iAdd);

	vCalibr = (vCalibr-plcMin)/(plcMax-plcMin);
	varDt = min+(max-min)*(scSqr?pow(vCalibr,0.5):vCalibr)-var;
	var += varDt/max(1,Tf/1000*f_frq);

	bndVarHyst = (aMax-aMin)*HystBnd/100;
	if(aMax>aMin && (var>=aMax || (HH && var>=(aMax-bndVarHyst))))
	{ tErr="3:Нарушение верхней аварийной границы"; HH=true; EVAL=H=LL=L=false; }
	else if(aMax>aMin && (var<=aMin || (LL && var<=(aMin+bndVarHyst))))
	{ tErr="4:Нарушение нижней аварийной границы"; LL=true; EVAL=HH=H=L=false; }
	else if(wMax>wMin && (var>=wMax || (H && var>=(wMax-bndVarHyst))))
	{ tErr="5:Нарушение верхней предупредительной границы"; H=true; EVAL=HH=LL=L=false; }
	else if(wMax>wMin && (var<=wMin || (L && var<=(wMin+bndVarHyst))))
	{ tErr="6:Нарушение нижней предупредительной границы"; L=true; EVAL=HH=H=LL=false; }
	else if(speed && varDt > speed) { tErr="7:Очень большая скорость изменения параметра";   EVAL=true; HH=H=LL=L=false; }
	else EVAL=HH=H=LL=L=false;
}

if(!f_err.toInt() && tErr.toInt()) this.nodePrev().alarmSet((NAME.length?NAME:SHIFR)+": "+DESCR+": "+tErr.parse(1,":"), -4, SHIFR);
else if(f_err.toInt() && !tErr.toInt()) this.nodePrev().alarmSet((NAME.length?NAME:SHIFR)+": "+DESCR+": НОРМА", 1, SHIFR);
f_err = tErr;','');
INSERT INTO "tmplib_base" VALUES('anUnifSt','Analog sign. (Unif, stats)','Аналог. сигнал (Уніф, стани)','Аналог. сигн. (Униф, состояния)','Unified template for analog input signals processing.','Уніфікований шаблон для обробки аналогових вхідних сигналів.','Унифицированный шаблон обработки аналогового сигнала.',10,'JavaLikeCalc.JavaScript
if(f_start) {
	f_err = "0";
	prevVar = EVAL_REAL;
	//Prepare data for preprocessing
	inPrcLng = "JavaLikeCalc.JavaScript";
	inPrcArgs = new Object();
	inPrcArgs.this = this;
}
pMax = plcMax; pMin = plcMin;	//Copy for local modifies using
if(passIn=(pMax==pMin)) { pMax = max/iMult - iAdd; pMin = min/iMult - iAdd; }

if(plcImit) {	//Data imitation
	if(!plcImitIn.isEVal()) in = plcImitIn;
	else {
		plcDif = abs(pMax-pMin);
		in = pMin + plcDif/2 + rand(plcDif/10)-plcDif/20;
	}
}

//Call specific preprocessing procedure
if(inProc.length)	{
	inPrcArgs.in = in; inPrcArgs.min = min; inPrcArgs.max = max;
	inPrcArgs.plcMin = pMin; inPrcArgs.plcMax = pMax;
	inPrcArgs.plcImit = plcImit; inPrcArgs.plcImitIn = plcImitIn;
	SYS.DAQ.funcCall(inPrcLng, inPrcArgs, inProc);
	in = inPrcArgs.in;
}

levErr = 0;
tErr = "0";
//Input data check and postprocess
if(in.isEVal()) {
	tErr = "1:No data or connection with source"; levErr = -5;
	var = EVAL_REAL;
	if(subMode == 1) var = prevVar;
	else if(subMode == 2) var = subVar;
}
else if(in > (max(pMax,pMin)+plcExcess*abs(pMax-pMin)/100)) {
	tErr = "1:The signal exceed to upper hardware border"; levErr = -5;
	var = EVAL_REAL;
	if(subMode == 1) var = prevVar.isEVal() ? max+plcExcess*(max-min)/100 : prevVar;
	else if(subMode == 2) var = subVar;
}
else if(in < (min(pMax,pMin)-plcExcess*abs(pMax-pMin)/100)) {
	tErr = "2:The signal exceed to bottom hardware border"; levErr = -5;
	var = EVAL_REAL;
	if(subMode == 1) var = prevVar.isEVal() ? min-plcExcess*(max-min)/100 : prevVar;
	else if(subMode == 2) var = subVar;
}
if(tErr) {
  EVAL = true;
  HH = H = LL = L = false;
}
else {
	vCalibr = iMult*(in+iAdd);
	if(passIn) { pMin = iMult*(pMin+iAdd); pMax = iMult*(pMax+iAdd); }
	if(!passIn || scSqr) {
		vCalibr = (vCalibr-min(pMax,pMin))/abs(pMax-pMin);
		if(pMax < pMin) vCalibr = 1-vCalibr;
		vCalibr = min + (max-min)*(scSqr?pow(vCalibr,0.5):vCalibr);
	}
	if(var.isEVal())	var = vCalibr;
	varDt = vCalibr - var;
	var += varDt/max(1,Tf*f_frq);
	prevVar = var;

	bndVarHyst = (max-min)*HystBnd/100;
	if(aMax < max && aMax > aMin && (var >= aMax || (HH && var >= (aMax-bndVarHyst))))
	{ tErr = "3:Upper alarm border error"; levErr = -4; HH = true; EVAL = H = LL = L = false; }
	else if(aMin > min && aMax > aMin && (var <= aMin || (LL && var <= (aMin+bndVarHyst))))
	{ tErr = "4:Nether alarm border error"; levErr = -4; LL = true; EVAL = HH = H = L = false; }
	else if(wMax < max && wMax > wMin && (var >= wMax || (H && var >= (wMax-bndVarHyst))))
	{ tErr = "5:Upper warning border error"; levErr = -2; H = true; EVAL = HH = LL = L = false; }
	else if(wMin > min && wMax > wMin && (var <= wMin || (L && var <= (wMin+bndVarHyst))))
	{ tErr = "6:Nether warning border error"; levErr = -2; L = true; EVAL = HH = H = LL = false; }
	else if(speed && varDt > speed)	{ tErr = "7:Too big parameter''s motion speed"; levErr = -2; }
	else EVAL = HH = H = LL = L = false;
}

//Alarms forming
if(alSup)	f_err = "0";
else {
	if(tErr.toInt() && tErr.toInt() != f_err.toInt())
		this.nodePrev().alarmSet((NAME.length?NAME:SHIFR)+": "+DESCR+": "+tErr.parse(1,":"), levErr, SHIFR);
	else if(f_err.toInt() && !tErr.toInt())
		this.nodePrev().alarmSet((NAME.length?NAME:SHIFR)+": "+DESCR+": NORMA", 1, SHIFR);
	f_err = tErr;
}','JavaLikeCalc.JavaScript
if(f_start) {
	f_err = "0";
	prevVar = EVAL_REAL;
	//Prepare data for preprocessing
	inPrcLng = "JavaLikeCalc.JavaScript";
	inPrcArgs = new Object();
	inPrcArgs.this = this;
}
pMax = plcMax; pMin = plcMin;	//Copy for local modifies using
if(passIn=(pMax==pMin)) { pMax = max/iMult - iAdd; pMin = min/iMult - iAdd; }

if(plcImit) {	//Data imitation
	if(!plcImitIn.isEVal()) in = plcImitIn;
	else {
		plcDif = abs(pMax-pMin);
		in = pMin + plcDif/2 + rand(plcDif/10)-plcDif/20;
	}
}

//Call specific preprocessing procedure
if(inProc.length)	{
	inPrcArgs.in = in; inPrcArgs.min = min; inPrcArgs.max = max;
	inPrcArgs.plcMin = pMin; inPrcArgs.plcMax = pMax;
	inPrcArgs.plcImit = plcImit; inPrcArgs.plcImitIn = plcImitIn;
	SYS.DAQ.funcCall(inPrcLng, inPrcArgs, inProc);
	in = inPrcArgs.in;
}

levErr = 0;
tErr = "0";
//Input data check and postprocess
if(in.isEVal()) {
	tErr = "1:Немає даних або зв''язку із джерелом"; levErr = -5;
	var = EVAL_REAL;
	if(subMode == 1) var = prevVar;
	else if(subMode == 2) var = subVar;
}
else if(in > (max(pMax,pMin)+plcExcess*abs(pMax-pMin)/100)) {
	tErr = "1:Сигнал перевищив верхню апаратну границю"; levErr = -5;
	var = EVAL_REAL;
	if(subMode == 1) var = prevVar.isEVal() ? max+plcExcess*(max-min)/100 : prevVar;
	else if(subMode == 2) var = subVar;
}
else if(in < (min(pMax,pMin)-plcExcess*abs(pMax-pMin)/100)) {
	tErr = "2:Сигнал перевищив нижню апаратну границю"; levErr = -5;
	var = EVAL_REAL;
	if(subMode == 1) var = prevVar.isEVal() ? min-plcExcess*(max-min)/100 : prevVar;
	else if(subMode == 2) var = subVar;
}
if(tErr) {
  EVAL = true;
  HH = H = LL = L = false;
}
else {
	vCalibr = iMult*(in+iAdd);
	if(passIn) { pMin = iMult*(pMin+iAdd); pMax = iMult*(pMax+iAdd); }
	if(!passIn || scSqr) {
		vCalibr = (vCalibr-min(pMax,pMin))/abs(pMax-pMin);
		if(pMax < pMin) vCalibr = 1-vCalibr;
		vCalibr = min + (max-min)*(scSqr?pow(vCalibr,0.5):vCalibr);
	}
	if(var.isEVal())	var = vCalibr;
	varDt = vCalibr - var;
	var += varDt/max(1,Tf*f_frq);
	prevVar = var;

	bndVarHyst = (max-min)*HystBnd/100;
	if(aMax < max && aMax > aMin && (var >= aMax || (HH && var >= (aMax-bndVarHyst))))
	{ tErr = "3:Помилка верхньої аварійної границі"; levErr = -4; HH = true; EVAL = H = LL = L = false; }
	else if(aMin > min && aMax > aMin && (var <= aMin || (LL && var <= (aMin+bndVarHyst))))
{ tErr = "4:Помилка нижньої аварійної границі"; levErr = -4; LL = true; EVAL = HH = H = L = false; }
	else if(wMax < max && wMax > wMin && (var >= wMax || (H && var >= (wMax-bndVarHyst))))
{ tErr = "5:Помилка верхньої поперджув. границі"; levErr = -2; H = true; EVAL = HH = LL = L = false; }
	else if(wMin > min && wMax > wMin && (var <= wMin || (L && var <= (wMin+bndVarHyst))))
{ tErr = "6:Помилка нижньої поперджув. границі"; levErr = -2; L = true; EVAL = HH = H = LL = false; }
	else if(speed && varDt > speed)	{ tErr = "7:Дуже велика швидкість зміни параметру"; levErr = -2; }
	else EVAL = HH = H = LL = L = false;
}

//Alarms forming
if(alSup)	f_err = "0";
else {
	if(tErr.toInt() && tErr.toInt() != f_err.toInt())
		this.nodePrev().alarmSet((NAME.length?NAME:SHIFR)+": "+DESCR+": "+tErr.parse(1,":"), levErr, SHIFR);
	else if(f_err.toInt() && !tErr.toInt())
		this.nodePrev().alarmSet((NAME.length?NAME:SHIFR)+": "+DESCR+": НОРМА", 1, SHIFR);
	f_err = tErr;
}','JavaLikeCalc.JavaScript
if(f_start) {
	f_err = "0";
	prevVar = EVAL_REAL;
	//Prepare data for preprocessing
	inPrcLng = "JavaLikeCalc.JavaScript";
	inPrcArgs = new Object();
	inPrcArgs.this = this;
}
pMax = plcMax; pMin = plcMin;	//Copy for local modifies using
if(passIn=(pMax==pMin)) { pMax = max/iMult - iAdd; pMin = min/iMult - iAdd; }

if(plcImit) {	//Data imitation
	if(!plcImitIn.isEVal()) in = plcImitIn;
	else {
		plcDif = abs(pMax-pMin);
		in = pMin + plcDif/2 + rand(plcDif/10)-plcDif/20;
	}
}

//Call specific preprocessing procedure
if(inProc.length)	{
	inPrcArgs.in = in; inPrcArgs.min = min; inPrcArgs.max = max;
	inPrcArgs.plcMin = pMin; inPrcArgs.plcMax = pMax;
	inPrcArgs.plcImit = plcImit; inPrcArgs.plcImitIn = plcImitIn;
	SYS.DAQ.funcCall(inPrcLng, inPrcArgs, inProc);
	in = inPrcArgs.in;
}

levErr = 0;
tErr = "0";
//Input data check and postprocess
if(in.isEVal()) {
	tErr = "1:Нет данных или связи с источником"; levErr = -5;
	var = EVAL_REAL;
	if(subMode == 1) var = prevVar;
	else if(subMode == 2) var = subVar;
}
else if(in > (max(pMax,pMin)+plcExcess*abs(pMax-pMin)/100)) {
	tErr = "1:Выход сигнала за верхнюю аппаратную границу"; levErr = -5;
	var = EVAL_REAL;
	if(subMode == 1) var = prevVar.isEVal() ? max+plcExcess*(max-min)/100 : prevVar;
	else if(subMode == 2) var = subVar;
}
else if(in < (min(pMax,pMin)-plcExcess*abs(pMax-pMin)/100)) {
	tErr = "2:Выход сигнала за нижнюю аппаратную границу"; levErr = -5;
	var = EVAL_REAL;
	if(subMode == 1) var = prevVar.isEVal() ? min-plcExcess*(max-min)/100 : prevVar;
	else if(subMode == 2) var = subVar;
}
if(tErr) {
  EVAL = true;
  HH = H = LL = L = false;
}
else {
	vCalibr = iMult*(in+iAdd);
	if(passIn) { pMin = iMult*(pMin+iAdd); pMax = iMult*(pMax+iAdd); }
	if(!passIn || scSqr) {
		vCalibr = (vCalibr-min(pMax,pMin))/abs(pMax-pMin);
		if(pMax < pMin) vCalibr = 1-vCalibr;
		vCalibr = min + (max-min)*(scSqr?pow(vCalibr,0.5):vCalibr);
	}
	if(var.isEVal())	var = vCalibr;
	varDt = vCalibr - var;
	var += varDt/max(1,Tf*f_frq);
	prevVar = var;

	bndVarHyst = (max-min)*HystBnd/100;
	if(aMax < max && aMax > aMin && (var >= aMax || (HH && var >= (aMax-bndVarHyst))))
	{ tErr = "3:Нарушение верхней аварийной границы"; levErr = -4; HH = true; EVAL = H = LL = L = false; }
	else if(aMin > min && aMax > aMin && (var <= aMin || (LL && var <= (aMin+bndVarHyst))))
	{ tErr = "4:Нарушение нижней аварийной границы"; levErr = -4; LL = true; EVAL = HH = H = L = false; }
	else if(wMax < max && wMax > wMin && (var >= wMax || (H && var >= (wMax-bndVarHyst))))
	{ tErr = "5:Нарушение верхней предупредительной границы"; levErr = -2; H = true; EVAL = HH = LL = L = false; }
	else if(wMin > min && wMax > wMin && (var <= wMin || (L && var <= (wMin+bndVarHyst))))
	{ tErr = "6:Нарушение нижней предупредительной границы"; levErr = -2; L = true; EVAL = HH = H = LL = false; }
	else if(speed && varDt > speed)	{ tErr = "7:Очень большая скорость изменения параметра"; levErr = -2; }
	else EVAL = HH = H = LL = L = false;
}

//Alarms forming
if(alSup)	f_err = "0";
else {
	if(tErr.toInt() && tErr.toInt() != f_err.toInt())
		this.nodePrev().alarmSet((NAME.length?NAME:SHIFR)+": "+DESCR+": "+tErr.parse(1,":"), levErr, SHIFR);
	else if(f_err.toInt() && !tErr.toInt())
		this.nodePrev().alarmSet((NAME.length?NAME:SHIFR)+": "+DESCR+": НОРМА", 1, SHIFR);
	f_err = tErr;
}',1402043046);
INSERT INTO "tmplib_base" VALUES('pidUnif','PID sign. (Unif, stats)','ПІД сигнал (Уніф, стани)','ПИД сигн. (Униф, состояния)','The unified template for process analog signals with properties PID.','Уніфікований шаблон для обробки аналогового сигналу з властивостями ПІД.','Унифицированный шаблон обработки аналогового сигнала со свойствами ПИД.',10,'JavaLikeCalc.JavaScript
if(f_start) f_err = "0";

if(plcImit)
{
	if(plcImitIn != EVAL_REAL) in = plcImitIn;
	else
	{
		plcDif = plcMax-plcMin;
		in = plcMin + plcDif/2 + rand(plcDif/10)-plcDif/20;
	}
}

tErr = "0";
if( in > (plcMax+plcExcess*(plcMax-plcMin)/100) )
{ tErr = "1:The signal exceed to upper hardware border"; var = max+plcExcess*(max-min)/100;}
else if( in < (plcMin-plcExcess*(plcMax-plcMin)/100) )
{ tErr = "2:The signal exceed to bottom hardware border"; var = min-plcExcess*(max-min)/100;}
if( tErr ) 
{
  //var = dmsk ? max+plcExcess*(max-min)/100 : min-plcExcess*(max-min)/100;
  EVAL = true;
}
else
{
	vCalibr = iMult*(in+iAdd);

	vCalibr = (vCalibr-plcMin)/(plcMax-plcMin);
	varDt = min+(max-min)*(scSqr?pow(vCalibr,0.5):vCalibr)-var;
	var += varDt/max(1,Tf/1000*f_frq);

	bndVarHyst = (aMax-aMin)*HystBnd/100;
	if( aMax>aMin && (var>=aMax || (HH && var>=(aMax-bndVarHyst))) )
	{ tErr="3:Upper alarm border error"; HH=true; EVAL=H=LL=L=false; }
	else if( aMax>aMin && (var<=aMin || (LL && var<=(aMin+bndVarHyst))) )
	{ tErr="4:Nether alarm border error"; LL=true; EVAL=HH=H=L=false; }
	else if( wMax>wMin && (var>=wMax || (H && var>=(wMax-bndVarHyst))) )
	{ tErr="5:Upper warning border error"; H=true; EVAL=HH=LL=L=false; }
	else if( wMax>wMin && (var<=wMin || (L && var<=(wMin+bndVarHyst))) )
	{ tErr="6:Nether warning border error"; L=true; EVAL=HH=H=LL=false; }
	else if( speed && varDt > speed ) { tErr="7:Too big parameter''s motion speed";   EVAL=true; HH=H=LL=L=false; }
	else EVAL=HH=H=LL=L=false;
}

if(!f_err.toInt() && tErr.toInt()) this.nodePrev().alarmSet((NAME.length?NAME:SHIFR)+": "+DESCR+": "+tErr.parse(1,":"), -4, SHIFR);
else if(f_err.toInt() && !tErr.toInt()) this.nodePrev().alarmSet((NAME.length?NAME:SHIFR)+": "+DESCR+": NORMA", 1, SHIFR);
f_err = tErr;','JavaLikeCalc.JavaScript
if(f_start) f_err = "0";

if(plcImit)
{
	if(plcImitIn != EVAL_REAL) in = plcImitIn;
	else
	{
		plcDif = plcMax-plcMin;
		in = plcMin + plcDif/2 + rand(plcDif/10)-plcDif/20;
	}
}

tErr = "0";
if( in > (plcMax+plcExcess*(plcMax-plcMin)/100) )
{ tErr = "1:Сигнал перевищив верхню апаратну границю"; var = max+plcExcess*(max-min)/100;}
else if( in < (plcMin-plcExcess*(plcMax-plcMin)/100) )
{ tErr = "2:Сигнал перевищив нижню апаратну границю"; var = min-plcExcess*(max-min)/100;}
if( tErr ) 
{
  //var = dmsk ? max+plcExcess*(max-min)/100 : min-plcExcess*(max-min)/100;
  EVAL = true;
}
else
{
	vCalibr = iMult*(in+iAdd);

	vCalibr = (vCalibr-plcMin)/(plcMax-plcMin);
	varDt = min+(max-min)*(scSqr?pow(vCalibr,0.5):vCalibr)-var;
	var += varDt/max(1,Tf/1000*f_frq);

	bndVarHyst = (aMax-aMin)*HystBnd/100;
	if( aMax>aMin && (var>=aMax || (HH && var>=(aMax-bndVarHyst))) )
	{ tErr="3:Помилка верхньої границі аварії"; HH=true; EVAL=H=LL=L=false; }
	else if( aMax>aMin && (var<=aMin || (LL && var<=(aMin+bndVarHyst))) )
	{ tErr="4:Помилка нижньої границі аварії"; LL=true; EVAL=HH=H=L=false; }
	else if( wMax>wMin && (var>=wMax || (H && var>=(wMax-bndVarHyst))) )
	{ tErr="5:Помилка верхньої границі попередження"; H=true; EVAL=HH=LL=L=false; }
	else if( wMax>wMin && (var<=wMin || (L && var<=(wMin+bndVarHyst))) )
	{ tErr="6:Помилка нижньої границі попередження"; L=true; EVAL=HH=H=LL=false; }
	else if( speed && varDt > speed ) { tErr="7:Дуже велика швидкість зміни параметру";   EVAL=true; HH=H=LL=L=false; }
	else EVAL=HH=H=LL=L=false;
}

if(!f_err.toInt() && tErr.toInt()) this.nodePrev().alarmSet((NAME.length?NAME:SHIFR)+": "+DESCR+": "+tErr.parse(1,":"), -4, SHIFR);
else if(f_err.toInt() && !tErr.toInt()) this.nodePrev().alarmSet((NAME.length?NAME:SHIFR)+": "+DESCR+": NORMA", 1, SHIFR);
f_err = tErr;','JavaLikeCalc.JavaScript
if(f_start) f_err = "0";

if(plcImit)
{
	if(plcImitIn != EVAL_REAL) in = plcImitIn;
	else
	{
		plcDif = plcMax-plcMin;
		in = plcMin + plcDif/2 + rand(plcDif/10)-plcDif/20;
	}
}

tErr = "0";
if(in > (plcMax+plcExcess*(plcMax-plcMin)/100))
{ tErr = "1:Выход сигнала за верхнюю аппаратную границу"; var = max+plcExcess*(max-min)/100;}
else if(in < (plcMin-plcExcess*(plcMax-plcMin)/100))
{ tErr = "2:Выход сигнала за нижнюю аппаратную границу"; var = min-plcExcess*(max-min)/100;}
if(tErr) 
{
  //var = dmsk ? max+plcExcess*(max-min)/100 : min-plcExcess*(max-min)/100;
  EVAL = true;
}
else
{
	vCalibr = iMult*(in+iAdd);

	vCalibr = (vCalibr-plcMin)/(plcMax-plcMin);
	varDt = min+(max-min)*(scSqr?pow(vCalibr,0.5):vCalibr)-var;
	var += varDt/max(1,Tf/1000*f_frq);

	bndVarHyst = (aMax-aMin)*HystBnd/100;
	if(aMax>aMin && (var>=aMax || (HH && var>=(aMax-bndVarHyst))))
	{ tErr="3:Нарушение верхней аварийной границы"; HH=true; EVAL=H=LL=L=false; }
	else if(aMax>aMin && (var<=aMin || (LL && var<=(aMin+bndVarHyst))))
	{ tErr="4:Нарушение нижней аварийной границы"; LL=true; EVAL=HH=H=L=false; }
	else if(wMax>wMin && (var>=wMax || (H && var>=(wMax-bndVarHyst))))
	{ tErr="5:Нарушение верхней предупредительной границы"; H=true; EVAL=HH=LL=L=false; }
	else if(wMax>wMin && (var<=wMin || (L && var<=(wMin+bndVarHyst))))
	{ tErr="6:Нарушение нижней предупредительной границы"; L=true; EVAL=HH=H=LL=false; }
	else if(speed && varDt > speed) { tErr="7:Очень большая скорость изменения параметра";   EVAL=true; HH=H=LL=L=false; }
	else EVAL=HH=H=LL=L=false;
}

if(!f_err.toInt() && tErr.toInt()) this.nodePrev().alarmSet((NAME.length?NAME:SHIFR)+": "+DESCR+": "+tErr.parse(1,":"), -4, SHIFR);
else if(f_err.toInt() && !tErr.toInt()) this.nodePrev().alarmSet((NAME.length?NAME:SHIFR)+": "+DESCR+": НОРМА", 1, SHIFR);
f_err = tErr;','');
CREATE TABLE 'LogLevPrm_prescription' ("SHIFR" TEXT DEFAULT '' ,"OWNER" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"EN" INTEGER DEFAULT '0' ,"PRM" TEXT DEFAULT '' , PRIMARY KEY ("SHIFR","OWNER"));
INSERT INTO "LogLevPrm_prescription" VALUES('timer','','Timer','Таймер','Таймер','Typical timer. Hold run up to time elapse.','Типовой таймер. Удерживает выполнение до завершения времени.','Типовий таймер. Утримує виконання до завершення часу.',1,'PrescrTempl.timer');
INSERT INTO "LogLevPrm_prescription" VALUES('backTimer','','Background timer','Фоновый таймер','Фоновий таймер','Background timer. Updating parallel with current command.','Фоновый таймер. Обновляется параллельно с текущей командой.','Фоновий таймер. Оновлюється паралельно з поточною командою.',1,'PrescrTempl.backTimer');
CREATE TABLE 'DAQ_LogicLev' ("ID" TEXT DEFAULT '' ,"NAME" TEXT DEFAULT '' ,"ru#NAME" TEXT DEFAULT '' ,"uk#NAME" TEXT DEFAULT '' ,"DESCR" TEXT DEFAULT '' ,"ru#DESCR" TEXT DEFAULT '' ,"uk#DESCR" TEXT DEFAULT '' ,"ENABLE" INTEGER DEFAULT '0' ,"START" INTEGER DEFAULT '0' ,"MESS_LEV" INTEGER DEFAULT '3' ,"REDNT" INTEGER DEFAULT '0' ,"REDNT_RUN" TEXT DEFAULT '<high>' ,"PRM_BD" TEXT DEFAULT '' ,"PRM_BD_REFL" TEXT DEFAULT '' ,"PERIOD" INTEGER DEFAULT '0' ,"SCHEDULE" TEXT DEFAULT '1' ,"PRIOR" INTEGER DEFAULT '0' , PRIMARY KEY ("ID"));
INSERT INTO "DAQ_LogicLev" VALUES('prescription','Prescription commands','Команды рецепта','Команди рецепту','','','',1,0,3,0,'<high>','LogLevPrm_prescription','LogLevPrmRefl_prescription',0,'0.2',0);
COMMIT;