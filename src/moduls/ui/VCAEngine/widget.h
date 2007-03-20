
//OpenSCADA system module UI.VCSEngine file: widget.h
/***************************************************************************
 *   Copyright (C) 2006 by Roman Savochenko
 *   rom_as@diyaorg.dp.ua
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 ***************************************************************************/

#ifndef WIDGET_H
#define WIDGET_H

#include <tcntrnode.h>
#include <tconfig.h>

namespace VCA
{

//************************************************
//* Widget attribute                             *
//************************************************
class Widget;

class Attr
{
    public:
        //Data
	//- Attribute flags -
        enum GlobalAttrFlgs
        {
	    Prevent = 0x0100,	//Prevent for a attribute change
            Image   = 0x0200,	//Store image link to DB or to file
            Color   = 0x0400,	//Store color
            Font    = 0x0800,	//Store font
	    Address = 0x1000,	//Store address to a parameter or an attribute
	    IsUser  = 0x2000,	//User created element
            IsInher = 0x4000    //Inherit attribute
        };

	//- Link types -
	enum SelfAttrFlgs
	{
	    LnkIn   = 0x01,	//Input link
	    LnkOut  = 0x02,	//Output link
	    LnkGlob = 0x04,	//Global link
	    AttrCalc= 0x08	//Calculated attribute
	};

	//Methods
	Attr( TFld &fld, Widget *owner );
	~Attr();

	string id();
	string name();
        TFld::Type type();
	int flgGlob();				//Global attribite's flags
	SelfAttrFlgs flgSelf()		{ return self_flg; }
	void flgSelf( SelfAttrFlgs flg );

	unsigned short modifVal()	{ return vl_modif; }
	void modifVal( unsigned short set ){ vl_modif = set; }
	unsigned short modifCfg()       { return cfg_modif; }
        void modifCfg( unsigned short set ){ cfg_modif = set; }

	//- Get value -
	string getSEL( );
	string getS( );
	double getR( );
	int    getI( );
	bool   getB( );

	//- Set value -
	void setSEL( const string &val );
	void setS( const string &val );
	void setR( double val );
	void setI( int val );
	void setB( bool val );

	//- Link -
	string lnkTempl()		{ return lnk_tmpl; }
	string lnkVal()			{ return lnk_val; }

	void lnkTempl( const string &vl );
	void lnkVal( const string &vl );

	TFld &fld()  			{ return *m_fld; }

	void AHDConnect();
        void AHDDisConnect();

    private:
        //Data
	//- Storing -
        union
        {
            string *s_val;		//String
            double r_val;		//Real
            int    i_val;		//Integer
            bool   b_val;		//Boolean
        }m_val;
        //- Attributes -
        TFld	*m_fld;			//Base field
        Widget	*m_owner;		//Owner widget
	unsigned short	vl_modif,	//Value modify counter
			cfg_modif;	//Configuration modify counter
	SelfAttrFlgs self_flg;		//Self attributes flags
	//- Link -
	string	lnk_tmpl,		//Link template
		lnk_val;		//Link value

};

//************************************************
//* Widget                                       *
//************************************************
typedef map< string, Attr* >	TAttrMap;

class Widget : public TCntrNode, public TValElem
{
    friend class Attr;

    public:
        //Methods
	Widget( const string &id, const string &isrcwdg = "" );
	~Widget();

	Widget &operator=(Widget &wdg);                    //Attribute values is copy

	string id( )               { return m_id; }        //Identifier
	virtual string name( )     { return ""; }          //Name
	virtual string descr( )    { return ""; }          //Description
	virtual string ico( )      { return ""; }          //Icon
	virtual string user( )     { return "root"; }      //Widget user
        virtual string grp( )      { return "UI"; }        //Widget group
	virtual short  permit( )   { return 0644; }        //Permition for access to widget
	virtual string calcLang( ) { return ""; }          //Calc procedure language
	virtual string calcProg( ) { return ""; }          //Calc procedure
	virtual bool isContainer( ){ return false; }       //Is container (Is define of the terminator)
        virtual bool isLink()      { return m_lnk; }       //Widget as link

        virtual void setName( const string &inm )     { };
	virtual void setDescr( const string &idscr )  { };
	virtual void setIco( const string &ico )      { };
	virtual void setUser( const string &iuser )   { };
	virtual void setGrp( const string &igrp )     { };
	virtual void setPermit( short iperm )         { };
	virtual void setCalcLang( const string &ilng ){ };
        virtual void setCalcProg( const string &iprg ){ };

	//- Storing -
        virtual void load( )    { }     //Load widget
	virtual void loadIO( )  { }     //Load widget's IO
        virtual void save( )    { }     //Save widget
	virtual void saveIO( )  { }     //Save widget's IO

	//- Enable stat -
        bool enable( );
        virtual void setEnable( bool val );

        //- Inheritance methods -
	virtual string parentNm( )	{ return m_parent_nm; }	//Parent widget name
	virtual string rootId( );                       	//Root widget id
	AutoHD<Widget> parent( );				//Parent widget
	virtual void setParentNm( const string &isw )	{ m_parent_nm = isw; }

        //- Widget's attributes -
        void attrInherit( Widget *wdg );        //Inherit all parent attributes
	void attrList( vector<string> &list );
	void attrAdd( TFld *attr );
	void attrDel( const string &attr );
	bool attrPresent( const string &n_val );
	AutoHD<Attr> attrAt( const string &n_val );

        //- Include widgets -
        void wdgList( vector<string> &list );
        bool wdgPresent( const string &wdg );
	virtual void wdgAdd( const string &wid, const string &name, const string &path );
        void wdgDel( const string &wid, bool full = false );
        AutoHD<Widget> wdgAt( const string &wdg );

    protected:
	//Methods
	string nodeName()	{ return m_id; }

	void postEnable( int flag );
	void preDisable( int flag );

	void cntrCmdProc( XMLNode *opt );       //Control interface command process

        virtual bool attrChange( Attr &cfg );   //Process attribute change into terminator

        void addFld( TElem *el, unsigned id );
        void delFld( TElem *el, unsigned id );
        void detElem( TElem *el );

	//Data
	//- Generic data -
	string	m_id;                   //Widget identifier

	bool	m_enable;               //Enable status
        bool    m_lnk;                  //Widget as link
	int 	inclWdg;		//The widget's container id
        string  m_parent_nm;            //Parent widget name
	AutoHD<Widget>	m_parent;	//Parent widget

	//- Attributes data -
	TElem	*attr_cfg;		//Attributes structure element
	TAttrMap attrs;			//Attributes map container
	int	attr_res;		//Attribute access resource
};

}

#endif //WIDGET_H
