# Copyright 1999-2011 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
#
# OpenSCADA system author and main developer:
# Roman Savochenko
# Email: rom_as@oscada.org
#
# Home page: http://oscada.org"

EAPI="5"

inherit eutils autotools qt4-r2

DESCRIPTION="Open SCADA system"
HOMEPAGE="http://oscada.org"
SRC_URI="ftp://ftp.oscada.org/OpenSCADA/Work/${PF}.tar.xz
ftp://ftp.oscada.org/OpenSCADA/Work/${PN}-res-${PVR}.tar.xz"

LICENSE="GPL-2"
SLOT="0"
KEYWORDS="~amd64 ~x86"

IUSE="dbase sqlite mysql firebird postgresql
    sockets ssl serial http selfsystem userprotocol
    bfn blockcalc daqgate dcon icp_das system javalikecalc logiclev modbus
	opc_ua siemens snmp soundcard
    fsarch dbarch
	vcaengine vision qtstarter qtcfg webcfg webcfgd webvision webuser
	systemtest flibcomplex1 flibmath flibsys demo doc"

DEPEND="sys-devel/gettext
	dev-libs/expat
	media-libs/gd[fontconfig,jpeg,truetype,png,-xpm]
	dev-libs/libpcre
	sci-libs/fftw:3.0
	firebird? ( dev-db/firebird )
	javalikecalc? ( sys-devel/bison )
	postgresql? ( dev-db/postgresql )
	sqlite? ( dev-db/sqlite )
	mysql? ( dev-db/mysql )
	snmp? ( net-analyzer/net-snmp )
	soundcard? ( media-libs/portaudio )
	system? ( sys-apps/lm_sensors app-admin/hddtemp sys-apps/smartmontools )
	vision? ( dev-qt/qtgui:4 )
	qtcfg? ( dev-qt/qtgui )
	qtstarter? ( dev-qt/qtgui )
	"
RDEPEND="${DEPEND}"

pkg_setup() {
	if use webvision || use webcfg || use webcfgd && ! use http ; then
		ewarn 'Was set USE="http" for support Web services!'
		http=true
	fi

	if use vision || use webvision && ! use vcaengine ; then
	    ewarn 'Was set USE="vcaengine" for environment visualization and management!'
		vcaengine=true
	fi

	if ! use qtcfg || ! use vision && use qtstarter ; then
	    ewarn 'Was set USE="vision" and USE="qtcfg" for defined Qt-based user interface!'
		vision=true
		qtcfg=true
	fi

	if use icp_das && ! use  dcon ; then
	    ewarn 'Was set USE="dcon" for using ICP-DAS module!'
		dcon=true
	fi
}

src_unpack() {
	unpack ${PF}.tar.xz
	cd "${WORKDIR}/${PF}"
	unpack ${PN}-res-${PVR}.tar.xz
}

src_configure() {
	eautoreconf -if || die "autoreconf failed"
	econf \
	$(use_enable bfn BFN) \
	$(use_enable blockcalc BlockCalc) \
	$(use_enable daqgate DAQGate) \
	$(use_enable dcon DCON) \
	$(use_enable dbase DBF ) \
	$(use_enable dbarch DBArch ) \
	$(use_enable firebird FireBird) \
	$(use_enable flibcomplex1 FLibComplex1) \
	$(use_enable flibmath FLibMath) \
	$(use_enable flibsys FLibSYS) \
	$(use_enable fsarch FSArch) \
	$(use_enable http HTTP) \
	$(use_enable icp_das ICP_DAS) \
	$(use_enable javalikecalc JavaLikeCalc) \
	$(use_enable logiclev LogicLev) \
	$(use_enable modbus ModBus) \
	$(use_enable mysql MySQL) \
	$(use_enable opc_ua OPC_UA) \
	$(use_enable postgresql PostgreSQL) \
	$(use_enable siemens Siemens) \
	$(use_enable sockets Sockets) \
	$(use_enable soundcard SoundCard) \
	$(use_enable snmp SNMP) \
	$(use_enable ssl SSL) \
	$(use_enable serial Serial) \
	$(use_enable selfsystem SelfSystem) \
	$(use_enable system System) \
	$(use_enable systemtest SystemTests) \
	$(use_enable sqlite SQLite) \
	$(use_enable userprotocol UserProtocol) \
	$(use_enable vcaengine VCAEngine) \
	$(use_enable vision Vision) \
	$(use_enable qtstarter QTStarter) \
	$(use_enable qtcfg QTCfg) \
	$(use_enable webcfg WebCfg) \
	$(use_enable webcfgd WebCfgD) \
	$(use_enable webvision WebVision) \
	$(use_enable webuser WebUser)  || die "configure failed"
}

src_compile() {
    emake || die "emake failed"
}

src_install() {
	newinitd "data/oscada_gentoo.init" oscadad
	dodir /var/spool/openscada
	dodir /var/spool/openscada/{ARCHIVES,LibsDB,AGLKS,Boiler,icons}
	dodir /var/spool/openscada/ARCHIVES/{MESS,VAL}
	insinto /etc
	doins data/oscada.xml
	doins data/oscada_start.xml
	dobin data/openscada_start

    if use vision || use webvision || use qtcfg || use webcfg || use webcfgd ; then
	insinto /var/spool/openscada/icons
	doins data/icons/*
    fi

    if use demo && use webvision && ! use vision ; then
	insinto /etc
	doins data/ModelsDB/AGLKS/oscada_AGLKS.xml
	dobin data/ModelsDB/AGLKS/openscada_AGLKS
	insinto /var/spool/openscada/AGLKS
	doins data/ModelsDB/AGLKS/*.db

	insinto /etc
	doins data/ModelsDB/Boiler/oscada_Boiler.xml
	dobin data/ModelsDB/Boiler/openscada_Boiler
	insinto /var/spool/openscada/Boiler
	doins data/ModelsDB/Boiler/*.db

	insinto /var/spool/openscada/LibsDB
	doins data/LibsDB/*.db
    fi

    if use demo && use vision ; then
	insinto /etc
	doins data/ModelsDB/AGLKS/oscada_AGLKS.xml
	dobin data/ModelsDB/AGLKS/openscada_AGLKS
	insinto /var/spool/openscada/AGLKS
	doins data/ModelsDB/AGLKS/*.db

	insinto /etc
	doins data/ModelsDB/Boiler/oscada_Boiler.xml
	dobin data/ModelsDB/Boiler/openscada_Boiler
	insinto /var/spool/openscada/Boiler
	doins data/ModelsDB/Boiler/*.db

	insinto /var/spool/openscada/LibsDB
	doins data/LibsDB/*.db
	doicon data/ModelsDB/AGLKS/openscada_AGLKS.png
	domenu data/ModelsDB/AGLKS/openscada_AGLKS.desktop
	doicon data/ModelsDB/Boiler/openscada_Boiler.png
	domenu data/ModelsDB/Boiler/openscada_Boiler.desktop
    fi

    if use vision || use qtcfg ; then
	doicon data/openscada.png
	domenu data/openscada.desktop
    fi

	emake install DESTDIR=${D} || die "emake failed"
	dodoc ChangeLog README* AUTHORS COPYING NEWS TODO*

	if use doc ; then
	dodoc -r doc/*
	fi
}

pkg_postinst() {
    ewarn "THIS PACKAGE IS IN ITS DEVELOPMENT STAGE!"
    ewarn "See ${HOMEPAGE} for more info."
    einfo "Config place on /etc/oscada.xml"
}
