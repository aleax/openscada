
//OPC_UA implementation test: test.cpp
/******************************************************************************
 *   Copyright (C) 2013 by Roman Savochenko                                   *
 *   rom_as@oscada.org, rom_as@fromru.com                                     *
 *                                                                            *
 *   This library is free software; you can redistribute it and/or modify     *
 *   it under the terms of the GNU Lesser General Public License as           *
 *   published by the Free Software Foundation; version 3 of the License.     *
 *                                                                            *
 *   This library is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *   GNU Lesser General Public License for more details.                      *
 *                                                                            *
 *   You should have received a copy of the GNU Lesser General Public License *
 *   along with this library; if not, write to the                            *
 *   Free Software Foundation, Inc.,                                          *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.                *
 ******************************************************************************/

#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "test.h"

int main( int argc, char *argv[], char *envp[] )
{
    if(argc < 3)
    {
	printf("OPC UA client test program need command line arguments:\n"
	    "  \"testOPC_UA opc.tcp://{Host}:{Port}/{SecurePolicy}/{MessSecMode} {NodeId} {user:pass}\"\n"
	    "Example: \"testOPC_UA opc.tcp://127.0.0.1:4841/Basic128Rsa15/SignEnc 84 user:pass");
	return 0;
    }

    // Create client connection object
    TestClient clnt(argv[1], (argc>3)?argv[3]:"");

    // Create requests object
    XML_N req("opc.tcp");

    //  Browse request for pointed node
    req.setAttr("id","Browse");
    req.childAdd("node")->setAttr("nodeId",argv[2])->setAttr("browseDirection",int2str(BD_BOTH))->
			  setAttr("resultMask",int2str(RdRm_IsForward|RdRm_BrowseName));
    clnt.reqService(req);
    if(!req.attr("err").empty()) printf("ERROR: Browse: '%s'\n",req.attr("err").c_str());
    else
    {
	XML_N *rn = req.childGet(0);
	printf("Browse node '%s' list from %d items\n", argv[2], rn->childSize());
	for(unsigned i_n = 0; i_n < rn->childSize(); i_n++)
	{
	    //if(atoi(rn->childGet(i_n)->attr("isForward").c_str())) continue;
	    printf("  node%d: \"%s\"\n", i_n, (rn->childGet(i_n)->attr("browseName")+" ("+rn->childGet(i_n)->attr("nodeId")+")").c_str());
	}
    }

    //  Read pointed node value and access attribute
    req.clear()->setAttr("id", "Read")->setAttr("timestampsToReturn", int2str(TS_SOURCE));
	req.childAdd("node")->setAttr("nodeId", argv[2])->setAttr("attributeId", int2str(AId_NodeClass));
	req.childAdd("node")->setAttr("nodeId", argv[2])->setAttr("attributeId", int2str(AId_Value));
	req.childAdd("node")->setAttr("nodeId", argv[2])->setAttr("attributeId", int2str(AId_AccessLevel));
    clnt.reqService(req);
    if(!req.attr("err").empty()) printf("ERROR: Read: '%s'\n", req.attr("err").c_str());
    else
    {
	uint32_t state = strtol(req.childGet(0)->attr("Status").c_str(),NULL,0);
	uint32_t mode = atoi(req.childGet(0)->text().c_str());
	printf("Read node '%s'=%u\n", argv[2], state);
	if(!state && mode == NC_Variable)
	{
	    uint32_t accs = atoi(req.childGet(2)->text().c_str());
	    uint8_t eMask = atoi(req.childGet(1)->attr("EncodingMask").c_str());
	    if(accs&ACS_Read) printf(" value='%s', timeStamp=%s\n",
		req.childGet(1)->text().c_str(), req.childGet(1)->attr("SourceTimestamp").c_str());
	    if(accs&ACS_Write)
	    {
		req.clear()->setAttr("id", "Write")->
		    childAdd("node")->setAttr("nodeId", argv[2])->setAttr("attributeId", int2str(AId_Value))->
				      setAttr("EncodingMask", int2str(eMask))->setText("3.14159265");
		clnt.reqService(req);
	    }
	}
    }

    return 0;
}

/************************************************
 * TestClient                                   *
 ************************************************/
TestClient::TestClient( const string &iep, const string &aData ) : mEp(iep), mSecPol("None"), mSecMessMode(MS_None), sock_fd(-1)
{
    //Parse EndPoint for TCP connection and other properties obtain
    if(mEp.compare(0,10,"opc.tcp://") == 0)
    {
	size_t uriPos = mEp.find("/", 10), uriPos1;
	mURI = (uriPos != string::npos) ? mEp.substr(uriPos) : "";

	mAddr = mEp.substr(10, (uriPos==string::npos) ? uriPos : (uriPos-10));
	size_t portPos = mAddr.find(":");
	if(portPos == string::npos || !atoi(mAddr.substr(portPos+1).c_str())) mAddr = mAddr.substr(0,portPos)+":4840";

	//URI parse
	if(mURI.size())
	{
	    string secMessMd;

	    uriPos = 1;
	    if(uriPos >= mURI.size()) mSecPol = "None";
	    else
	    {
		uriPos1 = mURI.find("/", uriPos);
		mSecPol = (uriPos1 != string::npos) ? mURI.substr(uriPos,uriPos1-uriPos) : mURI.substr(uriPos);
	    }

	    uriPos = uriPos1+1;
	    if(uriPos >= mURI.size()) secMessMd = "None";
	    {
		uriPos1 = mURI.find("/", uriPos);
		secMessMd = (uriPos1 != string::npos) ? mURI.substr(uriPos,uriPos1-uriPos) : mURI.substr(uriPos);
	    }

	    if(mSecPol == "Basic128Rsa15" || mSecPol == "Basic256") mSecMessMode = MS_SignAndEncrypt;
	    else { mSecPol = "None"; mSecMessMode = MS_None; }
	    if(secMessMd == "SignEnc" && mSecPol != "None")	mSecMessMode = MS_SignAndEncrypt;
	    else if(secMessMd == "Sign" && mSecPol != "None")	mSecMessMode = MS_Sign;

	    printf("Set and uses secure policy '%s' and messages mode '%s'\n",
		mSecPol.c_str(), (mSecMessMode==MS_Sign)?"Sign":((mSecMessMode==MS_SignAndEncrypt)?"SignEnc":"None"));
	}
    }

    //Load client certificate and private key from file
    int hd = -1, len;
    char buf[10240];
    if((hd=open("cert.pem",O_RDONLY)) >= 0)
    {
	len = read(hd,buf,sizeof(buf));
	mCert.assign(buf,len);
	close(hd);
    }

    if((hd=open("key.pem",O_RDONLY)) >= 0)
    {
	len = read(hd,buf,sizeof(buf));
	mPvKey.assign(buf,len);
	close(hd);
    }

    //User and password set
    if(aData.size()) mAuthData = aData.size() ? strParse(aData,0,":")+"\n"+strParse(aData,1,":") : "";

    start();
}

TestClient::~TestClient( )
{
    stop();
}

void TestClient::start( )
{
    if(sock_fd >= 0)	stop();

    //Open socket from EndPoint address
    struct sockaddr_in	name_in;
    memset(&name_in, 0, sizeof(name_in));
    name_in.sin_family = AF_INET;
    size_t portPos = mAddr.find(":");
    struct hostent *loc_host_nm = gethostbyname(mAddr.substr(0,portPos).c_str());
    name_in.sin_port = htons(atol(mAddr.substr(portPos+1).c_str()));
    sock_fd = socket(PF_INET, SOCK_STREAM, 0);
    int connRes = -1;
    if(loc_host_nm && loc_host_nm->h_length && name_in.sin_port > 0 && sock_fd >= 0)
    {
	name_in.sin_addr.s_addr = *((int*)(loc_host_nm->h_addr_list[0]));

	int vl = 1;
	setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &vl, sizeof(int));

	// Real connect to socket
	connRes = connect(sock_fd, (sockaddr*)&name_in, sizeof(name_in));
    }

    //Error connection
    if(connRes)
    {
	if(sock_fd >= 0) close(sock_fd);
	sock_fd = -1;
    }
}

void TestClient::stop( )
{
    if(sock_fd >= 0) close(sock_fd);
    sock_fd = -1;
}

int TestClient::messIO( const char *obuf, int len_ob, char *ibuf, int len_ib )
{
    int reqTry = 0;

    repeate:
    if(sock_fd < 0 || (reqTry++) >= 2) return 0;

    int kzw = 0, kzr;
    if(obuf != NULL && len_ob > 0) kzw = write(sock_fd, obuf, len_ob);
    if(ibuf != NULL && len_ib > 0)
    {
	kzr = read(sock_fd, ibuf, len_ib);
	if(kzr <= 0)
	{
	    start();
	    goto repeate;
	}
    }

    return std::max(0,kzr);
}
