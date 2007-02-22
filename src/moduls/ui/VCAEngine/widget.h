
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
	    IsUser  = 0x2000	//User created element
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

typedef map< string, Attr* >	TAttrMap;

class LibWdg;

class Widget : public TCntrNode, public TConfig
{
    friend class Attr;
    
    public:
        //Methods			    
	Widget( const string &id, bool lib_loc = true, const string &isrcwdg = "", bool container = false );
	~Widget();
	
	Widget &operator=(Widget &wdg);				//Attribute values is copy 
	
	string id()			{ return m_id; }	//Identifier
	string name();						//Name
	string descr()			{ return m_descr; }	//Description
	string ico();						//Icon
	string user();						//Widget user 
	string grp();						//Widget group
	short  permit();					//Permition for access to widget
	string progLang();					//Calc procedure language
	string prog();						//Calc procedure
	
	virtual string original()	{ return "Base"; }	//Original widget name
	bool libLocated()		{ return m_lib_loc; }	//The widget location into library flag
	bool container()		{ return m_container; }	//The widget is container flag
	string srcWdgNm();					//The widget of container is a source widget name
	AutoHD<Widget> srcWdg()		{ return m_srcWdg; }	//The widget of container is a source widget
    
	void name( const string &inm )	{ m_name = inm; }
	void descr( const string &idscr ){ m_descr = idscr; }
	void ico( const string &ico );
	void user( const string &iuser );
	void grp( const string &igrp );
	void permit( short iperm );
	void progLang( const string &ilng );
        void prog( const string &iprg );

	void srcWdgNm( const string &isw );
	
	//- Storing -
        virtual void load( );	//Load from DB
	virtual void loadIO( );	//Load widget's IO
        virtual void save( );	//Save to DB
	virtual void saveIO( );	//Save widget's IO

	//- Enable stat -
        bool enable( )   		{ return m_enable; }
        virtual void enable( bool val );
	
	//- Widget's attributes -
	virtual void attrProc( Widget *uwdg, bool init = false );
	void attrList( vector<string> &list );
	void attrAdd( TFld *attr );
	void attrDel( const string &attr );		
	bool attrPresent( const string &n_val );
	AutoHD<Attr> attrAt( const string &n_val );
	
        //- Include widgets -
        void wdgList( vector<string> &list );
        bool wdgPresent( const string &wdg );
	void wdgAdd( const string &wid, const string &path );
        void wdgDel( const string &wid, bool full = false );
        AutoHD<Widget> wdgAt( const string &wdg );
							
	//- Owner object -
	LibWdg &ownLib();
	Widget &ownWdg();
    
    protected:
	//Methods
	string nodeName()	{ return m_id; }
	void postEnable();
	void preDisable( int flag );
	void postDisable( int flag );
	void cntrCmdProc( XMLNode *opt );       //Control interface command process

        virtual bool attrChange( Attr &cfg );
	
        void addFld( TElem *el, unsigned id );
        void delFld( TElem *el, unsigned id );

	TElem *attrCfg()	{ return attr_cfg; }

	//Data
	//- Generic data -
	string	&m_id,			//Widget identifier
		&m_name,		//Widget name
		&m_descr;		//Widget description
	
	bool	m_enable,		//Enable status
		m_lib_loc,		//Library location status
		m_container;		//Widget is container for other widgets
	int 	inclWdg;		//The widget's container id
	AutoHD<Widget>	m_srcWdg;	//Be connected source widget

	//- Attributes data -
	TElem	*attr_cfg;		//Attributes structure element
	TAttrMap attrs;			//Attributes map container
	int	attr_res;		//Attribute access resource
};

}

#endif //WIDGET_H
