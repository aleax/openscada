#ifndef __log
#define __log

#ifdef LINUX
#define HAVE_PRINTF
#define logout stdout
#endif

#ifdef BCCWIN
#define HAVE_PRINTF
#define logout stdout
#endif

#ifdef KEILC51

#endif

#ifdef DOS
#define HAVE_PRINTF
#define logout stdout
#endif

#ifdef AVR
#define NO_PRINT_CODE
#endif


#ifdef HAVE_PRINTF

#define FLUSH fflush(logout)

//#define LINENUMBERS


#ifdef LINENUMBERS
#define LOG1(a) fprintf(logout,"%20s() %4d :"a,__FUNCTION__,__LINE__)
#define LOG2(a,b) fprintf(logout,"%20s() %4d :"a,__FUNCTION__,__LINE__,b)
#define LOG3(a,b,c) fprintf(logout,"%20s() %4d :"a,__FUNCTION__,__LINE__,b,c)
#define LOG4(a,b,c,d) fprintf(logout,"%20s() %4d :"a,__FUNCTION__,__LINE__,b,c,d)
#define LOG5(a,b,c,d,e) fprintf(logout,"%20s() %4d :"a,__FUNCTION__,__LINE__,b,c,d,e)
#define LOG6(a,b,c,d,e,f) fprintf(logout,"%20s() %4d :"a,__FUNCTION__,__LINE__,b,c,d,e,f)
#define LOG7(a,b,c,d,e,f,g) fprintf(logout,"%20s() %4d :"a,__FUNCTION__,__LINE__,b,c,d,e,f,g)

/*
#define LOG1(a) fprintf(logout,"%14s %20s() %4d :"a,__FILE__,__FUNCTION__,__LINE__)
#define LOG2(a,b) fprintf(logout,"%14s %20s() %4d :"a,__FILE__,__FUNCTION__,__LINE__,b)
#define LOG3(a,b,c) fprintf(logout,"%14s %20s() %4d :"a,__FILE__,__FUNCTION__,__LINE__,b,c)
#define LOG4(a,b,c,d) fprintf(logout,"%14s %20s() %4d :"a,__FILE__,__FUNCTION__,__LINE__,b,c,d)
#define LOG5(a,b,c,d,e) fprintf(logout,"%14s %20s() %4d :"a,__FILE__,__FUNCTION__,__LINE__,b,c,d,e)
#define LOG6(a,b,c,d,e,f) fprintf(logout,"%14s %20s() %4d :"a,__FILE__,__FUNCTION__,__LINE__,b,c,d,e,f)
#define LOG7(a,b,c,d,e,f,g) fprintf(logout,"%14s %20s() %4d :"a,__FILE__,__FUNCTION__,__LINE__,b,c,d,e,f,g)
*/
#else
#define LOG1(x) fprintf(logout,x)
#define LOG2(x,y) fprintf(logout,x,y)
#define LOG3(a,b,c) fprintf(logout,a,b,c)
#define LOG4(a,b,c,d) fprintf(logout,a,b,c,d)
#define LOG5(a,b,c,d,e) fprintf(logout,a,b,c,d,e)
#define LOG6(a,b,c,d,e,f) fprintf(logout,a,b,c,d,e,f)
#define LOG7(a,b,c,d,e,f,g) fprintf(logout,a,b,c,d,e,f,g)

#endif

#define LOG_1(a) fprintf(logout,a)
#define LOG_2(a,b) fprintf(logout,a,b)
#endif /* HAVE_PRINTF */

#ifdef NO_PRINT_CODE
#define LOG1(x)
#define LOG2(x,y)
#define LOG3(a,b,c)
#define LOG4(a,b,c,d)
#define LOG5(a,b,c,d,e)
#define LOG6(a,b,c,d,e,f)
#define LOG7(a,b,c,d,e,f,g)
#define FLUSH

#define LOG_1(a)
#define LOG_2(a,b) 
#endif /*  NO_PRINT_CODE */

				
#endif /* __log */
