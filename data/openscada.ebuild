# Copyright 1999-2018 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
#
# OpenSCADA system author and main developer:
# Roman Savochenko
# Email: rom_as@oscada.org
#
# Home page: http://oscada.org"

EAPI="7"

inherit eutils autotools

DESCRIPTION="Open SCADA system"
HOMEPAGE="http://oscada.org"
SRC_URI="ftp://ftp.oscada.org/OpenSCADA/Work/${PF}.tar.xz 
ftp://ftp.oscada.org/OpenSCADA/Work/${PN}-res-${PVR}.tar.xz"

LICENSE="GPL-2"
SLOT="0"
KEYWORDS="amd64 x86"

IUSE="dbase dbgate sqlite ldap mysql firebird odbc postgresql
	sockets ssl serial http selfsystem userprotocol 
	amr bfn blockcalc daqgate dcon icp_das system javalikecalc logiclev modbus 
	opc_ua siemens snmp soundcard
	fsarch dbarch
	vcaengine vision qtstarter qtcfg webcfg webcfgd webvision webuser
	systemtest flibcomplex1 flibmath flibsys doc"

DEPEND="sys-devel/gettext
	dev-libs/expat
	media-libs/gd[fontconfig,jpeg,truetype,png,-xpm]
	dev-libs/libpcre
	sci-libs/fftw
	firebird? ( dev-db/firebird )
	ldap? ( net-nds/openldap )
	javalikecalc? ( sys-devel/bison )
	odbc? ( dev-db/libiodbc )
	postgresql? ( dev-db/postgresql )
	sqlite? ( dev-db/sqlite )
	mysql? ( dev-db/mysql )
	snmp? ( net-analyzer/net-snmp )
	soundcard? ( media-libs/portaudio )
	system? ( sys-apps/lm-sensors app-admin/hddtemp sys-apps/smartmontools )
	vision? ( dev-qt/qtgui )
	qtcfg? ( dev-qt/qtgui )
	qtstarter? ( dev-qt/qtgui )
	"
RDEPEND="${DEPEND}"

pkg_setup() {
	if use icp_das && ! x86 ; then 
		ewarn 'Module ICP_DAS can used for x86 architecture only! Disabled.'
		icp_das=false
	fi

	if use icp_das && ! use dcon ; then
		ewarn 'Was set USE="dcon" for using ICP-DAS module!'
		dcon=true
	fi

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
}

src_unpack() {
	unpack ${PF}.tar.xz
	cd "${WORKDIR}/${PF}"
	unpack ${PN}-res-${PVR}.tar.xz
}

src_prepare() {
	eautoreconf -if || die "autoreconf failed"
	eapply "${FILESDIR}/qt5.15.patch"
	eapply_user
}
src_configure() {
	econf \
	$(use_enable amr AMRDevs) \
	$(use_enable bfn BFN) \
	$(use_enable blockcalc BlockCalc) \
	$(use_enable daqgate DAQGate) \
	$(use_enable dbarch DBArch ) \
	$(use_enable dbase DBF ) \
	$(use_enable dbgate DBGate ) \
	$(use_enable dcon DCON) \
	$(use_enable firebird FireBird) \
	$(use_enable flibcomplex1 FLibComplex1) \
	$(use_enable flibmath FLibMath) \
	$(use_enable flibsys FLibSYS) \
	$(use_enable fsarch FSArch) \
	$(use_enable http HTTP) \
	$(use_enable icp_das ICP_DAS) \
	$(use_enable javalikecalc JavaLikeCalc) \
	$(use_enable ldap LDAP) \
	$(use_enable logiclev LogicLev) \
	$(use_enable modbus ModBus) \
	$(use_enable mysql MySQL) \
	$(use_enable odbc ODBC) \
	$(use_enable opc_ua OPC_UA) \
	$(use_enable postgresql PostgreSQL) \
	$(use_enable qtcfg QTCfg) \
	$(use_enable qtstarter QTStarter) \
	$(use_enable selfsystem SelfSystem) \
	$(use_enable serial Serial) \
	$(use_enable siemens Siemens) \
	$(use_enable snmp SNMP) \
	$(use_enable sockets Sockets) \
	$(use_enable soundcard SoundCard) \
	$(use_enable sqlite SQLite) \
	$(use_enable ssl SSL) \
	$(use_enable system System) \
	$(use_enable systemtest SystemTests) \
	$(use_enable userprotocol UserProtocol) \
	$(use_enable vcaengine VCAEngine) \
	$(use_enable vision Vision) \
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

	emake install DESTDIR=${D} || die "emake failed"
	dodoc ChangeLog README* AUTHORS COPYING NEWS TODO*

	if use doc ; then
		dodoc -r doc/*
	fi
}

pkg_postinst() {
	einfo "See ${HOMEPAGE} for more info."
	einfo "Config place on /etc/oscada.xml"
}
