/* www.coda-c.com  coda-p.c

Copyright (C) 2026 Stephen M. Jones

This file is part of Coda-P.

Coda-P is free software: you can redistribute it and/or modify it
under the terms of the GNU Affero General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at
your option) any later version.

Coda-P is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with Coda-P. If not, see <https://www.gnu.org/licenses/>.

*/
	#define QWebsite "www.coda-c.com"
	#define QCopyYears	"2026"
	#define QVersion	"3.1"

	#include "./coda-p.h"

Char FileMem_Info(FileMem self);
huge FileMem_oTell(FileMem self);
int FileMem_oWrite(FileMem self,pointer buf,int length);
Obj CLIst_leafToClass(char *xmltag);
bool CList_leafClass(Obj obj);
int PList_cdata_move(char *dest,char *sour,int remain,int *bytes);
Obj PLIst_leafToClass(char *xmlkind,bool apple);
bool PList_leafClass(Obj obj,bool apple);
bool PList_stringDecode(char *string,int *errPos);
char* PList_stringEncode(int bufsize,char *buffer,char *a,Char *extra,bool amp38);
Obj Unique_obj(Obj uniquer,Obj obj,bool *isadup,int flags);

	#define Assert_ assert

char Data_base64s[]=
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	#define base64s	Data_base64s

int Data_base64decode(char *dp,int dlen,char *sp,int slen) {
	int j,px=0,nel=0,out=0;
	for(j=0;j<slen;++j) { int cc=sp[j];
		if (cc_isWhite(cc)) continue;
		int ix=cc_pos(cc,base64s); if (ix==EOF) continue;
		if (out>=dlen)							return(-2);
		switch(nel%4) {
			case 0:  break;
			case 1:  dp[out++]= ((px<<2)&0xfc)|((ix>>4)&0x03); break;
			case 2:  dp[out++]= ((px<<4)&0xf0)|((ix>>2)&0x0f); break;
			case 3:  dp[out++]= ((px<<6)&0xc0)|( ix    &0x3f); break;
			}
		px=ix; ++nel;
		}
	return(out);
	}

int Data_base64encode(char *dp,int dlen,char *sp,int slen,int addpad) {
	int j,ix,px=0,out=0;
	for(j=0;j<slen;++j) { ix=sp[j];
		if (out+1>=dlen)						return(-2);
		switch(j%3) {
			case 0:  dp[out++]=base64s[                (ix>>2)&0x3f ]; break;
			case 1:  dp[out++]=base64s[((px<<4)&0x30)|((ix>>4)&0x0f)]; break;
			case 2:  dp[out++]=base64s[((px<<2)&0x3c)|((ix>>6)&0x03)];
					 dp[out++]=base64s[(                ix    &0x3f)]; break;
			}
		px=ix;
		}
	ix=0;
		if (out+(addpad?2:0)>=dlen)				return(-2);
		switch(j%3) {
			case 1:  dp[out++]=base64s[((px<<4)&0x30)|((ix>>4)&0x0f)];
					 if (addpad) dp[out++]='=';
					 if (addpad) dp[out++]='=';
					 break;
			case 2:  dp[out++]=base64s[((px<<2)&0x3c)|((ix>>6)&0x03)];
					 if (addpad) dp[out++]='=';
					 break;
			}
	return(out);
	}

	#undef base64s
#define class Bool
CodaClassZerosC();
CodaClass(Bool,_Bool,Root);
class Bool_Value(bool value) {
	class self=newO(class); *self=value; return(self);
	}

method$(Char,Info) { return Char_F("%s[%d]=(%d,...) aka _Bool",kindO(self),sizeO(self),*self); }

class $(FromString,char *string) {
	char *str=string;
	while(cc_isWhite(*str)) ++str;
	if (!*str) OAbort("empty <bool> value ''");
	int ival=strtol(str,0,10);
	if (cc_inString(cc_toCap(*str),"TY1")) ival=1;
	return Bool_Value(ival);
	}

Char $(ToString) { return ( *self ? Os("true"):Os("false")); }

method$(Char,xmlTag) { return Os("bool"); }

method$(Char,ToDelimiter,int index) {
	static Char da[]={ Os(", "), Os("<bool>["), Os("]"), Os("?") }; return da[index&3];
	}

method$(Char,ToStringSub,int index) { return Bool_ToString(self); }

$boot(StephenMJones) { CodaSig(FromString); CodaSig(ToString); }
#undef class

bool cc_isCap(int c)   { return( c>='A' && c<= 'Z' ); }
bool cc_isLow(int c)   { return( c>='a' && c<= 'z' ); }
bool cc_isAlpha(int c) { return( cc_isLow(c) || cc_isCap(c) ); }
bool cc_is09(int c)    { return( c>='0' && c<= '9' ); }
bool cc_isWhite(int c) { return cc_inString(c," \t\n\r\f"); }

int cc_toCap(int c)   { return( c>='a' && c<='z' ? c-32 : c); }
int cc_toLow(int c)   { return( c>='A' && c<='Z' ? c+32 : c); }

int cc_pos(char c,const char *str) {
	for(const char *cp=str;*cp;++cp) if (*cp==c) return(cp-str);
	return(EOF);
	}

int cc_posCap(char c,const char *str)  {
	c=cc_toCap(c);
	for(const char *cp=str;*cp;++cp) if (cc_toCap(*cp)==c) return(cp-str);
	return(EOF);
	}

int cc_revPos(char c,const char *str) {
	const char *cp=str+cs_length(str);
	while(--cp>=str) if (*cp==c) break;
	return(cp-str);
	}

bool cs_prefix(const char *key,const char *str) {
	while(*key) if (*key++ != *str++)	return(0);
	return(1);
	}

bool cs_prefixCap(const char *key,const char *str) {
	while(*key) if (cc_toCap(*key++)!=cc_toCap(*str++))	return(0);
	return(1);
	}

void cs_copy(char *dest,const char *sour) {
	int cc; while(1) { cc=(*sour++); if (!cc) break; (*dest++)=cc; }
	*dest=0;
	}

bool cs_exactCap(const char *str1,const char *str2) {
	while(*str1) if (cc_toCap(*str1++)!=cc_toCap(*str2++))	return(0);
	return(*str2==0);
	}

int cs_pos(const char *key,const char *str) {
	int pos=0,j,c; if (!*key) return(EOF);
	c= *key++; if (!*key) return cc_pos(c,str);
	while(1) {
		if (!(j=cc_pos(c,str)+1)) return(EOF);
		str+=j; pos+=j;
		if (cs_prefix(key,str)) return(--pos);
		}
	}

int cs_posCap(const char *key,const char *str) {
	int pos=0,j,cc; if (!*key) return(EOF);
	cc= *key++; if (!*key) return cc_posCap(cc,str);
	while(1) {
		if (!(j=cc_posCap(cc,str)+1)) return(EOF);
		str+=j; pos+=j;
		if (cs_prefixCap(key,str)) return(--pos);
		}
	}

int4 cs_toInt4(const char *string) {
	return strtol(string,0,10);
	}

double cs_toReal(const char *string) {
	return strtod(string,0);
	}

#define class Data
CodaClassZeros(dtor,itor,kize,etor,ekeep); CodaClass_Zerok();
CodaClass(Data,void,Root);
class $(NewBlock,int count,pointer address) {
	if (count<0) return(0);
	self=newOC(class,count); cs_blockCopy(self,address,count); return(self);
	}

method$(Char,Info) { return Char_F("%s[%d] aka void*",kindO(self),Data_count(self)); }

class $(FromString,char *string) {
	int len=cs_length(string); char temp[len+16];
	huge hval=Data_base64decode(temp,len+8,string,len);
	if (hval<0) OAbort("base64 data decode error.");
	return Data_NewBlock(0,hval,temp);
	}

Char $(ToString) {
	int len=Data_count(self);
	int maxstr=((len+2)/3)*4+1024;
	Char str=newOC(Char,maxstr);
	int nullat=Data_base64encode(str,maxstr-6,self,len,1);
	str[nullat]=0;
	return(str);
	}

method$(Char,xmlTag) { return Os("data"); }

$boot(StephenMJones) { CodaSig(NewBlock); CodaSig(FromString); CodaSig(ToString); }
#undef class

#define class DateString
CodaClassTransC();
CodaClass(DateString,char,Char);
DateString DateString_Value(char *string) {
	int len=cs_length(string)+1; if (len<32) len=32;
	DateString self=newOC(DateString,len); cs_strcopy(self,string); return(self);
	}

method$(Char,Info) { return Char_F("DateString[%d]=%s aka char*",sizeO(self),(char *)self); }

DateString $(FromString,char *str) {
	if (!DateString_toGmtime(str)) return(0);
	return DateString_Value(str);
	}

Char $(ToString) { return Char_Value(self); }

method$(Char,xmlTag) { return Os("date"); }

$boot(StephenMJones) { CodaSig(FromString); CodaSig(ToString); }
#undef class

	#include <time.h>

DateString DateString_FromGmtime(struct_tm *when) {
	char bb[128];
	snprintf(bb,sizeof(bb),"%04d-%02d-%02dT%02d:%02d:%02dZ",
		when->tm_year+1900, when->tm_mon+1, when->tm_mday,
		when->tm_hour, when->tm_min, when->tm_sec);
	return DateString_Value(bb);
	}

struct_tm* DateString_toGmtime(DateString self) {
	int len=cs_length(self);
	if (len!=20) OAbort("%s; string length %d, must be %d ?",__func__,len,20);
	char a[32]; cs_copy(a,self);
	static struct_tm when; cs_blockZero(&when,sizeof(when));
	if (a[ 4]!='-' || a[ 7]!='-' || a[10]!='T') OAbort("%s; bad date seperators",__func__);
	if (a[13]!=':' || a[16]!=':' || a[19]!='Z') OAbort("%s; bad time seperators",__func__);
	a[ 4]=0; when.tm_year=cs_toInt4(a+ 0)-1900;
	a[ 7]=0; when.tm_mon =cs_toInt4(a+ 5)-1;
	a[10]=0; when.tm_mday=cs_toInt4(a+ 8);
	a[13]=0; when.tm_hour=cs_toInt4(a+11);
	a[16]=0; when.tm_min =cs_toInt4(a+14);
	a[19]=0; when.tm_sec =cs_toInt4(a+17);
	if (!Gmtime_check(&when)) return(0);
	return(&when);
	}

pointer Gmtime_check(struct_tm *self) {
	if (_ tm_mon <0 || _ tm_mon >11) OAbort("bad date: month=%d", _ tm_mon );
	int dpm[12]={31,29,31,30,31,30,31,31,30,31,30,31};
	int days=dpm[_ tm_mon];
	if (_ tm_mday<1 || _ tm_mday>days) OAbort("bad date: day=%d",_ tm_mday);

	if (_ tm_hour<0 || _ tm_hour>23) OAbort("bad time: hour=%d",_ tm_hour);
	if (_ tm_min <0 || _ tm_min >59) OAbort("bad time: min=%d", _ tm_min );
	if (_ tm_sec <0 || _ tm_sec >60) OAbort("bad time: sec=%d", _ tm_sec );
	return(self);
	}

#define class Float
CodaClassZerosC();
CodaClass(Float,float,Root);
class Float_Value(float value) {
	class self=newO(class); *self=value; return(self);
	}
class $(NewBlock,int count,pointer address) {
	if (count<=0) return(0);
	self=newOC(class,count); cs_blockCopy(self,address,count*sizeat(class)); return(self);
	}

method$(Char,Info) { return Char_F("%s[%d]=(%3.2f,...) aka float*",kindO(self),Float_count(self),*self); }

class $(FromString,char *string) {
	char *str=string;
	while(cc_isWhite(*str)) ++str;
	if (!*str) OAbort("empty <float> value ''");
	return Float_Value( cs_toReal(str) );
	}

method$(Char,ToDelimiter,int index) {
	static Char da[]={ Os(", "), Os("<float>["), Os("]"), Os("?") }; return da[index&3];
	}

method$(Char,ToStringSub,int index) { return Char_F("%4.3f",self[index]); }

Char $(ToString) { return Char_F("%.8e",*self); }

$boot(StephenMJones) { CodaSig(NewBlock); CodaSig(ToString); CodaSig(FromString); }
#undef class
#define class Huge
CodaClassZerosC();
CodaClass(Huge,huge,Root);
class Huge_Value(huge value) {
	class self=newO(class); *self=value; return(self);
	}
class $(NewBlock,int count,pointer address) {
	if (count<=0) return(0);
	self=newOC(class,count); cs_blockCopy(self,address,count*sizeat(class)); return(self);
	}

method$(Char,Info) { return Char_F("%s[%d]=(%lld,...) aka huge*",kindO(self),Huge_count(self),*self); }

class $(FromString,char *string) {
	char *str=string;
	while(cc_isWhite(*str)) ++str;
	if (!*str) OAbort("empty <integer> value ''");
	if (str[0]=='0' && (str[1]=='x' || str[1]=='X'))
		return Huge_Value( strtoll(str,0,16) );
	return Huge_Value( strtoll(str,0,10) );
	}

method$(Char,xmlTag) { return Os("integer"); }

Char $(ToString) { return Char_F("%lld",*self); }

method$(Char,ToDelimiter,int index) {
	static Char da[]={ Os(", "), Os("<huge>["), Os("]"), Os("?") }; return da[index&3];
	}

method$(Char,ToStringSub,int index) { return Char_F("%lld",self[index]); }

$boot(StephenMJones) { CodaSig(NewBlock); CodaSig(FromString); CodaSig(ToString); }
#undef class
#define class HugeUID
CodaClassTransC();
CodaClass(HugeUID,huge,Huge);
class HugeUID_Value(huge value) {
	huge *hp=newO(class); *hp=value; return(hp);
	}

method$(Char,Info) { return Char_F("HugeUID[%d]=(%lld,...) aka UID", Root_count(self), *self); }

	Huge Huge_FromString(Huge self,char *string);

class $(FromString,char *string) {
	self=Huge_FromString(0,string); if (!self) return(0);
	Memory_reclass(self,&Isa_HugeUID);
	return(self);
	}

method$(Char,xmlTag) { return Os("uid"); }

method$(Char,ToDelimiter,int index) {
	static Char da[]={ Os(", "), Os("<uid>["), Os("]"), Os("?") }; return da[index&3];
	}

method$(Char,ToStringSub,int index) { return Char_F("%lld<uid>",self[index]); }

$boot(StephenMJones) { CodaSig(FromString); }
#undef class
#define class Int4
CodaClassZerosC();
CodaClass(Int4,int4,Root);
class Int4_Value(int4 value) {
	class self=newO(class); *self=value; return(self);
	}
class $(NewBlock,int count,pointer address) {
	if (count<=0) return(0);
	self=newOC(class,count); cs_blockCopy(self,address,count*sizeat(class)); return(self);
	}

method$(Char,Info) { return Char_F("%s[%d]=(%d,...), aka int4*",kindO(self),Int4_count(self),*self); }

method$(Char,ToDelimiter,int index) {
	static Char da[]={ Os(", "), Os("<int4>["), Os("]"), Os("?") }; return da[index&3];
	}

method$(Char,ToStringSub,int index) { return Char_F("%d",self[index]); }

Char $(ToString) { return Char_F("%d",*self); }

$boot(StephenMJones) { CodaSig(NewBlock); CodaSig(ToString); }

int $(length) {
	if (!self) return(0);
	int len=0; for(;self[len];++len) ;
	return(len);
	}
#undef class

Void JsonNull_Value() { return alocO(0); }

bool isa_JsonNull(Obj obj) { return( isa_(obj,Void) && sizeO(obj)==0 );  }

$boot() {
	Version_register( "Coda-P",
	Os(QVersion ", " QWebsite ", Copyright (c) " QCopyYears "  Stephen M. Jones, Affero GPL 3."
	"\000"));
	}

	static __thread Obj lastError=0;

Obj OErrorSet(Obj $CONSUMED obj) {
	freeO(lastError);
	lastError=obj;
	return(0);
	}

Obj OErrorObject() {
	return(lastError);
	}

Char OError() {
	if (!lastError) return(0);
	if (isa_(lastError,Char)) return(lastError);
	if (isa_(lastError,Dictionary)) {
		Char desc=Dict_sub(lastError,"description");
		if (isa_(desc,Char)) return(desc);
		return(Os("Error Object--Dictionary w/o 'description'"));
		}
	return Os("Error Object--unknown.");
	}

#define class Real
CodaClassZerosC();
CodaClass(Real,double,Root);
class Real_Value(double value) {
	class self=newO(class); *self=value; return(self);
	}
class $(NewBlock,int count,pointer address) {
	if (count<=0) return(0);
	self=newOC(class,count); cs_blockCopy(self,address,count*sizeat(class)); return(self);
	}

method$(Char,Info) { return Char_F("%s[%d]=(%3.2f,...) aka double*",kindO(self),Real_count(self),*self); }

class $(FromString,char *string) {
	char *str=string;
	while(cc_isWhite(*str)) ++str;
	if (!*str) OAbort("empty <real> value ''");
	return Real_Value( cs_toReal(str) );
	}

Char $(ToString) { return Char_F("%.16e",*self); }

method$(Char,xmlTag) { return Os("real"); }

method$(Char,ToDelimiter,int index) {
	static Char da[]={ Os(", "), Os("<real>["), Os("]"), Os("?") }; return da[index&3];
	}

method$(Char,ToStringSub,int index) { return Char_F("%4.3f",self[index]); }

$boot(StephenMJones) { CodaSig(NewBlock); CodaSig(FromString); CodaSig(ToString); }
#undef class
#define class Short
CodaClassZerosC(); CodaClass(Short,short,Root);
class Short_Value(short value) {
	class self=newO(class); *self=value; return(self);
	}
class $(NewBlock,int count,pointer address) {
	if (count<=0) return(0);
	self=newOC(class,count); cs_blockCopy(self,address,count*sizeat(class)); return(self);
	}

method$(Char,Info) { return Char_F("%s[%d]=(%d,...) aka short*",kindO(self),Short_count(self),*self); }

method$(Char,ToDelimiter,int index) {
	static Char da[]={ Os(", "), Os("<short>["), Os("]"), Os("?") }; return da[index&3];
	}

method$(Char,ToStringSub,int index) { return Char_F("%d",self[index]); }

Char $(ToString) { return Char_F("%d",*self); }

$boot(StephenMJones) { CodaSig(NewBlock); CodaSig(ToString); }

int $(length) {
	if (!self) return(0);
	int len=0; for(;self[len];++len) ;
	return(len);
	}
#undef class

int u2_codes(pointer u2str) {
	return u2_scan(u2str,-1,-1,NULL,NULL);
	}

int u2_count(pointer u2str,int codes) {
	int shorts=0;
	u2_scan(u2str,-1,codes,&shorts,NULL);
	return(shorts);
	}

int u2_scan(pointer u2str,int countOrNeg1,int codes,int *counter,int *faults) {
	unsigned short *str=u2str;
	int haslimit=(codes>=0);
	codes&=0x7FFFFFFF;
	int len,extra=0,utfch=0,fault=0;
	for(len=0;1;++len) {
		if (countOrNeg1>=0 && len>=countOrNeg1) break;
		int cc=str[len]; if (countOrNeg1<0 && !cc) break;
		int oldutflen=utfch;
		if (0xDC00==(cc&0xFC00)) {
			if (extra==1) { extra=0; ++utfch; }
			else       	  ++fault;
			}
		else {
			if (extra)  { extra=0; ++fault;
				if (utfch+fault>codes) { --fault; break; }
				}
			if (0xD800==(cc&0xFC00)) extra=1;
			else					 ++utfch;
			}
		if (haslimit) {
			if (utfch+fault+extra>codes) {
				if (oldutflen!=utfch) --utfch; else --fault;
				break;
				}
			}
		}
	if (extra) ++fault;
	if (counter) *counter=len;
	if (faults) *faults=(fault);
	return(utfch+fault);
	}

pointer u2_encode(pointer ptr,int4 value,pointer context) {
	if (!ptr) {
		if (!value) return Class_Short;
		return(context);
		}
	value&=0x1FFFFF;
	short* sp=ptr;
	if (value<0x10000) { *sp++ =value; }
	else {
		value-=0x10000;
		*sp++ = 0xD800|((value>>10)&0x3FF);
		*sp++ = 0xDC00|( value     &0x3FF);
		}
	return(sp);
	}

pointer u4_encode(pointer ptr,int4 value,pointer context) {
	if (!ptr) {
		if (!value) return Class_Int4;
		return(context);
		}
	value&=0x1FFFFF;
	int4* ip=ptr;
	*ip++ =value;
	return(ip);
	}

int uc_codes(char* ustr) {
	return uc_scan(ustr,-1,-1,NULL,NULL);
	}

int uc_count(char *ustr,int codes) {
	int bytes=0;
	uc_scan(ustr,-1,codes,&bytes,NULL);
	return(bytes);
	}

int uc_scan(char *ustr,int lenOrNeg1,int codes,int *counter,int *faults) {
	unsigned char *str=(unsigned char *)ustr;
	int haslimit=(codes>=0);
	codes&=0x7FFFFFFF;
	int len,extra=0,utfch=0,fault=0;
	for(len=0;1;++len) {
		if (lenOrNeg1>=0 && len>=lenOrNeg1) break;
		int cc=str[len]; if (lenOrNeg1<0 && !cc) break;
		int oldutflen=utfch;
		if (0x80==(cc&0xC0)) {
			if (extra==1) { extra=0; ++utfch; }
			ei (extra>0)  --extra;
			else       	  ++fault;
			}
		else {
			if (extra)  { extra=0; ++fault;
				if (utfch+fault>codes) { --fault; break; }
				}
			if (0x00==(cc&0x80)) { ++utfch; }
			ei (0xC0==(cc&0xE0)) { extra=1; }
			ei (0xE0==(cc&0xF0)) { extra=2; }
			ei (0xF0==(cc&0xF8)) { extra=3; }
			else 				 { ++fault; }
			}
		if (haslimit) {
			int parto=(extra ? 1:0);
			if (utfch+fault+parto>codes) {
				if (oldutflen!=utfch) --utfch; else --fault;
				break;
				}
			}
		}
	if (extra) ++fault;
	if (counter) *counter=len;
	if (faults) *faults=(fault);
	return(utfch+fault);
	}

pointer uc_encode(pointer ptr,int4 value,pointer context) {
	if (!ptr) {
		if (!value) return Class_Char;
		return(context);
		}
	unsigned char* cp=ptr;
	value&=0x1FFFFF;
	if (value<0x80)    { *cp++ = value; }
	ei (value<0x800)   { *cp++ = 0xC0|((value>> 6)&0x1F);
		*cp++ = 0x80|(value&0x3F);
		}
	ei (value<0x10000)  { *cp++ = 0xE0|((value>>12)&0x0F);
		*cp++ = 0x80|((value>>6)&0x3F); *cp++ = 0x80|(value&0x3F);
		}
	else 			   { *cp++ = 0xF0|((value>>18)&0x07);
		*cp++ = 0x80|((value>>12)&0x3F); *cp++ = 0x80|((value>>6)&0x3F); *cp++ = 0x80|(value&0x3F);
		}
	return(cp);
	}

int uc_faults(char *ustr) {
	int faults=0;
	uc_scan(ustr,-1,-1,NULL,&faults);
	return(faults);
	}

Obj uN_Convert(int isizeN,UcEncoder encoder,pointer context,pointer uNstr, int countOrNeg1,int *faults) {

	if (countOrNeg1<0) {
		if (isizeN==1) countOrNeg1=cs_length(uNstr);
		ei (isizeN==2) countOrNeg1=Short_length(uNstr);
		else		   countOrNeg1=Int4_length(uNstr);
		}
	int maxBytes=4*countOrNeg1+4;
	Char buffer=encoder(0,maxBytes,context);
	cleanO Char temp=(buffer?0:newOC(Char,maxBytes));
	if (!buffer) buffer=temp;
	char* cp=buffer; int fault=0,value=0;
	if (isizeN==1) {
		int extra=0;
		for(int len=0;len<countOrNeg1;++len) {
			int cc=((unsigned char*)uNstr)[len];
			if (0x80==(cc&0xC0)) {
				if (extra) { --extra;
					value=(value<<6)|(cc&0x3F);
					if (extra==0) { cp=encoder(cp,value,context); }
					}
				else { ++fault; cp=encoder(cp,cc,context); }
				}
			else {
				if (extra) { ++fault; cp=encoder(cp,value<<(6*extra),context); extra=0; }
				if (0x00==(cc&0x80)) { cp=encoder(cp,cc,context); }
				ei (0xC0==(cc&0xE0)) { extra=1; value=cc&0x1F; }
				ei (0xE0==(cc&0xF0)) { extra=2; value=cc&0x0F; }
				ei (0xF0==(cc&0xF8)) { extra=3; value=cc&0x07; }
				else 		{ ++fault; cp=encoder(cp,cc,context); }
				}
			}
		if (extra)  { ++fault; cp=encoder(cp,value<<(6*extra),context); };
		}
	ei (isizeN==2) {
		int extra=0;
		for(int len=0;len<countOrNeg1;++len) {
			int cc=((unsigned short*)uNstr)[len];
			if (0xDC00==(cc&0xFC00)) {
				if (extra) { cp=encoder(cp,((extra&0x3FF)<<10|(cc&0x3FF))+0x10000,context); extra=0; }
				else       { ++fault; cp=encoder(cp,cc,context); }
				}
			else {
				if (extra)  { ++fault; cp=encoder(cp,extra,context); extra=0; }
				if (0xD800==(cc&0xFC00)) { extra=cc; }
				else		{ cp=encoder(cp,cc,context); }
				}
			}
		if (extra)  { ++fault; cp=encoder(cp,extra,context); }
		}
	else {
		for(int len=0;len<countOrNeg1;++len) {
			cp=encoder(cp,((int4*)uNstr)[len],context);
			}
		}
	if (faults) *faults=(fault);
	cp=encoder(cp,0,context);
	Obj proto=encoder(0,0,context);
	if (proto) { int4 size=(cp-buffer); CodaCLASS *clas=classO(proto);
		int nel=(size/clas->size);
		return obj_(NewBlock,proto,nel,buffer);
		}
	return(NULL);
	}

Short uc_ToU2(char *u1str,int countOrNeg1,int *faults) {
	return uN_Convert(1,u2_encode,NULL,u1str,countOrNeg1,faults);
	}

Int4 uc_ToU4(char *u1str,int countOrNeg1,int *faults) {
	return uN_Convert(1,u4_encode,NULL,u1str,countOrNeg1,faults);
	}

Char u2_ToUc(pointer u2str,int countOrNeg1,int *faults) {
	return uN_Convert(2,uc_encode,NULL,u2str,countOrNeg1,faults);
	}

Int4 u2_ToU4(pointer u2fstr,int countOrNeg1,int *faults) {
	return uN_Convert(2,u4_encode,NULL,u2fstr,countOrNeg1,faults);
	}

Char u4_ToUc(pointer u4str,int countOrNeg1,int *faults) {
	return uN_Convert(4,uc_encode,NULL,u4str,countOrNeg1,faults);
	}

Short u4_ToU2(pointer u4str,int countOrNeg1,int *faults) {
	return uN_Convert(4,u2_encode,NULL,u4str,countOrNeg1,faults);
	}

Data Array_ToData(Array array) {
	if (!array) OAbort("null array?");
	int4 total=0,nel=Array_count(array);
	for(int j=0;j<nel;++j) total+=sizeO(Array_sub(array,j));
	Data data=newOC(Data,total);
	char *at=data;
	for(int j=0;j<nel;++j) {
		Obj obj=Array_sub(array,j);
		int4 size=sizeO(obj);
		if (size) { cs_blockCopy(at,obj,size); at+=size; }
		}
	return(data);
	}

Data Data_FromFile(char *path) {
	int type=file_type(path);
	if (type=='?') OAbort("file does not exist.");
	ei (type!='-') OAbort("file is not regular.");
	huge size=file_size(path); if (size<0) OAbort("file size is bad.");
	cleanO Data data=newOC(Data,size);
	FILE *is=fopen(path,"rb"); if (!is) OAbort("can't open/read file.");
	huge got=file_read(is,data,size);
	fclose(is);
	if (got!=size) OAbort("did not read entire file?");
	return(keepO(data));
	}

	#include <sys/stat.h>

huge file_size(char *path) {
	struct stat a; if (stat(path,&a)) return(EOF);
	return(a.st_size);
	}

huge file_time(char *path) {
	struct stat a; if (stat(path,&a)) return(EOF);
	return(a.st_mtime);
	}

	#if WIN32
		#define lstat stat
		#define S_IFLNK  0xC300
		#define S_IFSOCK 0xC400
	#endif

int file_typeCode(char *path,bool reportlinks) {
	struct stat a;
	if (reportlinks) { if (lstat(path,&a)) return('?'); }
	  else           { if (stat(path,&a))  return('?'); }
	switch(a.st_mode & S_IFMT)	{
		case S_IFREG:	return('-');
		case S_IFDIR:	return('d');
		case S_IFCHR:	return('c');
		case S_IFBLK:	return('b');
		case S_IFIFO:	return('p');
		case S_IFLNK:	return('l');
		case S_IFSOCK:	return('s');
		default: 		return('u');
		}
	}

typedef int  $FORMAT23 OSig(oPrintf)(Obj obj,char *format,...) ; sig_(oPrintf);
typedef int  OSig(oWrite)(Obj obj,pointer buffer,int length) ;   sig_(oWrite);
typedef int  OSig(oPutc)(Obj obj,int cc) ;                       sig_(oPutc);
typedef int  OSig(oGetc)(Obj obj) ;                              sig_(oGetc);
typedef int  OSig(oRead)(Obj obj,pointer buffer,int length) ;    sig_(oRead);
typedef huge OSig(oTell)(Obj obj) ;                              sig_(oTell);

bool File_protocol(Obj obj) {
	Char sigs[]={sig_oPrintf,sig_oWrite,sig_oPutc,sig_oGetc,sig_oRead,sig_oTell,0};
	for(int j=0;sigs[j];++j) {
		Char sig=sigs[j];
		if (!O_Responds(obj,sig)) {
			Error_F("%s; obj(type=%s) does not respond to(%s).",__func__,kindO(obj),sig);
			return(0);
			}
		}
	return(1);
	}

	struct FileMem_ {
		Array array;
		int4 readIndex,readPos;
		int4 bufPos,tellsize;
		char buffer[BUFSIZ];
		};

#define class FileMem

static void $(dtor) { freeO(_ array); }
static void $(itor) { _ array=newO(Array); }

	CodaClassZeros(etor,bits,kize,ekeep);
CodaClass(FileMem,struct FileMem_,Root);

Char $(Info) { return Char_F("FileMem[Array:%d]",Array_count(_ array)); }

class FileMem_Open(Data optional) {
	class self=newO(class);
	if (optional) { Array_addObject(_ array,optional); _ tellsize+=sizeO(optional); }
	return(self);
	}

Array $(array) { _$(flush); return(_ array); }

void $(flush) {
	if ( _ bufPos<1) return;
	Array_takeObject(_ array,Data_NewBlock(0,_ bufPos,_ buffer));
	_ bufPos=0;
	}

huge $(oTell) { return(_ tellsize); }

int $(oWrite,pointer buf,int length) {
	if (_ bufPos + length >  BUFSIZ) _$(flush);
	if (_ bufPos + length <= BUFSIZ) { cs_blockCopy(_ buffer + _ bufPos,buf,length); _ bufPos+=length; }
	else Array_takeObject(_ array,Data_NewBlock(0,length,buf));
	_ tellsize+=length;
	return(length);
	}

int $(oPutc,int cc) { char aa[]={cc,0}; return _$(oWrite,aa,1); }

int $FORMAT23 $(oPrintf,char *format,...) {
	va_list ap; va_start(ap,format);
		int len=0;
			char buf[4096]; len=vsnprintf(buf,sizeof(buf),format,ap);
			char *cp=0; if (len>=sizeof(buf)) {
				va_end(ap);
				va_start(ap,format);
				len=vasprintf(&cp,format,ap); Assert_(cp);
				}
			char *ptr=(cp?cp:buf);
			_$(oWrite,ptr,len);
			free(cp);
		va_end(ap);
	return(len);
	}

Data $(ToData) {
	_$(flush);
	return Array_ToData(_ array);
	}

int $(oGetc) {
	_$(flush);
	int nel=Array_count(_ array);
	if (_ readIndex<0 || _ readIndex>=nel) return(EOF);
	Data data=Array_sub(_ array,_ readIndex);
	int size=sizeO(data);
	if (_ readPos<0 || _ readPos>=size) return(EOF);
	unsigned char *buf=data; int cc=buf[_ readPos];
	++ _ readPos;
	if (_ readPos>=size) { _ readPos=0; ++ _ readIndex; }
	return(cc);
	}

int $(oRead,char *buffer,int length) {
	if (length<1) return(0);
	_$(flush);
	int nel=Array_count(_ array);
	int out=0;
	while(out<length) {
		if (_ readIndex<0 || _ readIndex>=nel) break;
		Data data=Array_sub(_ array,_ readIndex);
		int size=sizeO(data);
		if (_ readPos<0 || _ readPos>=size) break;
		int avail=size - _ readPos;
		int want=length - out;
		int xfer=(want<avail ? want : avail);
		cs_blockCopy(buffer+out,data + _ readPos,xfer);
		out += xfer; _ readPos += xfer;
		if (_ readPos>=size) { _ readPos=0; ++ _ readIndex; }
		}
	return(out ? out : EOF);
	}

$boot(StephenMJones) { CodaSig(Info);
	CodaSig(oPrintf); CodaSig(oWrite); CodaSig(oPutc);
	CodaSig(oGetc);   CodaSig(oRead);
	CodaSig(oTell);
	}

#undef class

	static CDictionary CListClassDict=0;

static CDictionary CList_leafs() {
	if (!CListClassDict) {
		static bool once=1;
		if (once) { once=0; Global_objectAddress(&CListClassDict); }

		CListClassDict=newO(CDictionary);
		}
	return CListClassDict;
	}

void CList_addLeaf(char *xmltag,Obj cobj) {
	Dict_set(CList_leafs(),xmltag,cobj);
	}

Obj CLIst_leafToClass(char *xmltag) {
	return Dict_sub(CList_leafs(),xmltag);
	}

bool CList_leafClass(Obj obj) {
	CodaCLASS *clas=classO(obj);
	for(Keyword key=Dictionary_scan(CList_leafs());key;key=Dictionary_next(key)) {
		if (clas==classO(key->item)) return(1);
		}
	return(0);
	}

$boot(CList) {
	CList_leafs();
	CList_addLeaf("uid",Class_HugeUID);
	}

	#define ALEN 4096

	typedef struct PLPTR1_ {
		unsigned char* blob;
		int lno,pos,blobNel,iLevel;
		char a[ALEN+16];
		bool apple;
		} *PLPTR1;
	#undef  Self
	#define Self PLPTR1

	enum { MAXioLevel=100, };

	static pointer ptrAbort(Self self,Char msg) {
		Error_F("line %d: %s",_ lno+1,msg);
		freeO(msg); msg=0;
		return(0);
		}

  	#define abortP(...)	return(ptrAbort(self,Char_F(__VA_ARGS__)))

	static int ptr_getCc(Self self) {
		if (_ pos >= _ blobNel) return(EOF);
		int cc=_ blob[_ pos++];
		if (cc=='\n') ++_ lno;
		return(cc);
		}
	static void ptr_ungetCc(Self self,int count) {
		_ pos-=count;
		if (_ pos<0) _ pos=0;
		}
	static pointer slurpComment(Self self) {
		if ('-' != ptr_getCc(self)) abortP("Bad comment starting '-'");
		ei ('-' != ptr_getCc(self)) abortP("Bad comment starting '--'");
		else for(int dashes=0;1;) {
			int cc=ptr_getCc(self); if (cc==EOF) abortP("EOF in comment?");
			if (cc=='-') ++dashes;
			ei (cc=='>') {
				if (dashes>=2) return(self);
				}
			else dashes=0;
			}
		}

static pointer ptrToken(Self self,int flags) {
	int cc,started=flags&1,killspace= !(flags&2);
	while(1) {
		if (!started) while(1)	{
			cc=ptr_getCc(self); if (cc==EOF) abortP("EOF starting Token.");
			if (cc=='<') break;
			if (cc=='/') {
				int ncc=ptr_getCc(self);
				if (ncc=='/') {
					while(1) { ncc=ptr_getCc(self); if (ncc=='\n' || ncc==EOF) break; }
					cc='\n';
					}
				}
			if (!cc_isWhite(cc)) abortP("Token start bad cc %d.",cc);
			}
		int p=0; bool restart=0;
		while(1) {
			cc=ptr_getCc(self); if (cc==EOF) abortP("EOF reading Token.");
			if (cc=='>') break;
			if (cc<=32 && killspace) continue;
			if (p>=ALEN)					abortP("Token too large. %d",p);
			if (p==0 && cc=='!') {
				int c1=ptr_getCc(self);
				int c2=ptr_getCc(self);
				ptr_ungetCc(self,2);
				if (c1=='-' && c2=='-') {
					if (!slurpComment(self)) return(0);
					restart=1; started=0; break;
					}
				}
			_ a[p++]=cc;
			}
		if (restart) continue;
		_ a[p]=0;
		return(self);
		}
	}
	static char *plistParts4[]={
		"?xml version=\"1.0\" encoding=\"UTF-8\"?",
		"!DOCTYPE plist PUBLIC",
		"!DOCTYPE codalist SYSTEM",
		NULL};

	static pointer ptrLoadTop(Self self,int4 *typeptr) {
		int type=0;
		if (!ptrToken(self,2)) return(0);
		if (cs_prefix("?xml",_ a)) {
			if (!cs_exact(plistParts4[0],_ a)) 	abortP("wrong 1st token: xml version.");
			if (!ptrToken(self,2)) return(0);
			}
		if (cs_prefix("!DOC",_ a)) {
			if (cs_prefix(plistParts4[1],_ a)) 	;
			ei (cs_prefix(plistParts4[2],_ a)) 	;
			else abortP("wrong 2nd token lhs: DOCTYPE plist | DOCTYPE codalist.");
			if (!ptrToken(self,2)) return(0);
			}
		if (cs_prefix("plist",_ a)) 	{ type=PLIST_Apple; _ apple=1; }
		ei (cs_exact("codalist",_ a)) 	{ type=0; _ apple=0; }
		else abortP("wrong 3rd token: plist | codalist.(%s)",_ a);

		if (typeptr) *typeptr=type;
		return(self);
		}

	static pointer ptrStringDecode(Self self,char *buffer) {
		int errpos=0;
		if (!PList_stringDecode(buffer,&errpos)) {
			abortP("StringDecode(%-8.8s) error.",_ a+errpos);
			}
		return(self);
		}
	static int ptr_cdata(Self self,int ix,int *lines) ;
	static int ptr_ltSize(Self self) {
		for(int j=_ pos; j< _ blobNel;++j) {
			if (_ blob[j]=='<') {
				int size=ptr_cdata(self,j,0);
				if (!size) return(j - _ pos);
				j+=size-1;
				}
			}
		return(EOF);
		}
	static pointer ptrLoadKey(Self self,Char *extra) {
		int cc,p=0;
		int size=ptr_ltSize(self); if (size==EOF) abortP("EOF loading KEY.");
		cleanO Char temp=(size<=ALEN ? 0 : newOC(Char,size+8));
		if (temp && !extra) abortP("Item too large: %d w/o extra",size);
		char *buffer=(temp ? temp : _ a);
		while(1)	{
			cc=ptr_getCc(self); if (cc==EOF) abortP("EOF in LoadKey.(NEVER)");
			if (cc=='<') {
				int lines=0;
				int size=ptr_cdata(self,_ pos - 1,&lines);
				if (size==0) break;
				memcpy(buffer+p,_ blob+ _ pos -1,size);
				p+=size; _ pos +=size-1;
				_ lno +=lines;
				continue;
				}
			if (p>=size) abortP("Key|Item too large. %d (extra %p) NEVER",p,extra);
			buffer[p++]=cc;
			}
		buffer[p]=0;
		if (!ptrStringDecode(self,buffer)) return(0);
		if (temp) *extra=keepO(temp);
		return(self);
		}
	static Char aNull=Os("<null>");
				static pointer ptrLoadItem(Self self,char *ender) ;
	static pointer ptrLoadDict(Self self,Dictionary dict) {
		while(1)	{
			if (!ptrToken(self,0)) return(0);
			if (cs_exact(_ a,"/dict")) break;
			if (!cs_exact(_ a,"key")) abortP("DictLoad: entries must start with <key> got <%s>",_ a);
			char key[ALEN+16];
				if (!ptrLoadKey(self,0)) return(0);
				if (_ apple && uc_faults(_ a)) abortP("Non-UTF8 dict:key(%s)",_ a);
				cs_strcopy(key,_ a);
			if (!ptrToken(self,1)) return(0);
			if (!cs_exact(_ a,"/key")) abortP("DictLoad: key ending token error </key> got <%s>",_ a);
			Obj obj=ptrLoadItem(self,0); if (!obj) return(0);
			if (obj==aNull) obj=JsonNull_Value();
			Dict_take(dict,key,obj);
			}
		return("OK");
		}
	#undef ALEN

	static pointer ptrLoadArray(Self self,Array array) {
		while(1) {
			static Char ender=Os("/array");
			Obj obj=ptrLoadItem(self,ender); if (!obj) return(0);
			if (obj==ender) break;
			if (obj==aNull) obj=0;
			Array_take(array,obj);
			}
		return("OK");
		}
	static void decLevel(const void *vp) {
		Self self=(*(Self*)vp);
		--_ iLevel;
		}
	static void SelfNOP(Self junk) { }

	static Obj ptrLoadItem(Self self,char *ender) {
		if (++_ iLevel>MAXioLevel) OAbort("too many levels(%d). Circular?",_ iLevel);
		const $CLEANUP(decLevel) Self selfcopy= self;
			SelfNOP(selfcopy);

		if (!ptrToken(self,0)) return(0);
		char *kind=_ a;
		if (ender && cs_exact(kind,ender)) return(ender);
		if (cs_exact("dict",kind)) {
			Dictionary dict=newO(Dictionary);
			if (!ptrLoadDict(self,dict)) { freeO(dict); return(0); }
			if (Dictionary_count(dict)==1) {
				Keyword key=Dictionary_scan(dict);
				if (cs_exact("CF$UID",key->word) && isa_(key->item,Huge)) {
					Huge hhh=key->item;
					HugeUID uuu=HugeUID_Value(*hhh);
					freeO(dict);
					return(uuu);
					}
				ei (cs_exact("CF$Null",key->word) && isa_(key->item,Bool)) {
					freeO(dict);
					return aNull;
					}
				}
			return dict;
			}
		if (cs_exact("array",kind)) {
			Array array=newO(Array);
			if (!ptrLoadArray(self,array)) { freeO(array); return(0); }
			return array;
			}
		if (cs_exact("keyword",kind)) {
			if (!ptrLoadKey(self,0)) return(0);
			cleanO Keyword kkk=Keyword_Value(_ a,0);
			if (!ptrToken(self,1)) return(0);
			if (*_ a!='/' || !cs_exact(_ a+1,"keyword"))
				abortP("Keyword: end token </%s> got <%s>","keyword",_ a);
			Obj obj=ptrLoadItem(self,0); if (!obj) return(0);
			if (OResponds(obj,set_name)) {
				obj_(set_name,obj,kkk->word);
				if (isa_(obj,Array) && cs_exact(kkk->word,Os_Set)) Array_toaSet(obj);
				return(obj);
				}
			kkk->item=obj;
			return keepO(kkk);
			}
		int endslash=(*kind && kind[cs_length(kind)-1]=='/');
		if (endslash) {
			if (cs_exact("dict/", kind)) return newO(Dictionary);
			if (cs_exact("array/",kind)) return newO(Array);
			if (cs_exact("true/", kind)) return Bool_Value(1);
			if (cs_exact("false/",kind)) return Bool_Value(0);
			if (cs_exact("string/",kind)) return Char_Value("");
			if (cs_exact("null/",kind)) return aNull;
			}
			int toklen=cs_length(kind); if (toklen>60) abortP("token too large(%-20.20s...)",kind);
			char token[toklen+1]; cs_strcopy(token,kind);
			kind=token;
		Char extra=0;
		if (!ptrLoadKey(self,&extra)) return(0);
		cleanO Char temp=extra;
		char *input=(temp?temp:_ a);

		Obj cobj=PLIst_leafToClass(kind,_ apple);
		if (!cobj) abortP("unknown xml token: <%s>",kind);
		Obj obj=obj_(FromString,cobj,input); if (!obj) abortP("<%s> %s",kind,OError());

		if (cobj==OClass(Char)) {
			if (_ apple && uc_faults(obj))
				abortP("Non-UTF8 string(%s)",(char*)obj);
			}

		if (!ptrToken(self,1)) return(0);
		if (*_ a!='/' || !cs_exact(_ a+1,token)) abortP("Item: end token </%s> got <%s>",token,_ a);

		return(obj);
		}

	static __thread int4 PList_lastLoadTypeThread=0;
int4 PList_lastLoadType() { return(PList_lastLoadTypeThread); }

Obj PList_FromBlock(int count,pointer block,int flags) {
	if (!block) OAbort("Null data!");
	Self self=alocS(sizeat(Self));
		_ blob=block;
		_ blobNel=count;
	if (_ blobNel<16) abortP("Not enough data!");
	if (0==cs_blockCmp(block,"bplist",6)) {
		PList_lastLoadTypeThread=PLIST_Binary;

		char *proto="Obj PList_BinaryLoad(int,pointer);";
		Obj (*loader)(int,pointer)=Plugin_fromProto(proto);
		if (!loader) OAbort("%s; no plugin: %s.",__func__,proto);
		return loader(count,block);
		}
	if (!ptrLoadTop(self,&PList_lastLoadTypeThread)) return(0);

	cleanO Obj obj=ptrLoadItem(self,0); if (!obj) return(0);
		if (!ptrToken(self,0)) return(0);
		if (cs_exact(_ a,"/plist"))    ;
		ei (cs_exact(_ a,"/codalist")) ;
		else abortP("last token must be </plist|codalist> got <%s>",_ a);

	return(keepO(obj));
	}

	static char cdkeysta[]="<![CDATA["; static_assert(sizeof(cdkeysta)-1==9,"sklen?");
	static char cdkeyend[]="]]>";       static_assert(sizeof(cdkeyend)-1==3,"eklen?");
	static int ptr_cdata(Self self,int ix,int *lines) {
		   static int sklen=sizeof(cdkeysta)-1;
		   static int tklen=sizeof(cdkeysta)-1+sizeof(cdkeyend)-1;
		int remain=_ blobNel - ix;
		if (remain<tklen || !cs_prefix(cdkeysta,(Char)_ blob+ix)) return(0);
		int nel=remain-tklen+1;
		char* cp=(Char)_ blob+ix+sklen;
		int nlf=0;
		for(int j=0;j<nel;++j) {
			if (cp[j]=='\n') ++nlf;
			if (cs_prefix(cdkeyend,cp+j)) {
				if (lines) *lines=nlf;
				return(tklen+j);
				}
			}
		return(0);
		}

int PList_cdata_move(char *dest,char *sour,int remain,int *bytes) {
	   static int sklen=sizeof(cdkeysta)-1;
	   static int tklen=sizeof(cdkeysta)-1+sizeof(cdkeyend)-1;
	if (remain<tklen || !cs_prefix(cdkeysta,sour)) return(0);
	int nel=remain-tklen+1;
	char* cp=sour+sklen;
	for(int j=0;j<nel;++j) {
		if (cs_prefix(cdkeyend,cp+j)) {
			*bytes=j;
			for(int k=0;k<j;++k) dest[k]=cp[k];
			return(tklen+j);
			}
		}
	return(0);
	}

	static pointer table[]={
		&OClass(Char), &OClass(Huge), &OClass(Bool),
		&OClass(Real), &OClass(Data), &OClass(DateString),
		0};

	static CDictionary PListClassDict=0;

	static void PList_leafs_init() {
		if (!PListClassDict) {
			static bool once=1;
			if (once) { once=0; Global_objectAddress(&PListClassDict); }

			PListClassDict=newO(CDictionary);
			}
		}

	static CDictionary PList_leafs() {
		PList_leafs_init();
		if (Dictionary_count(PListClassDict)==0) {
			void **vpp=0;
			for(int j=0;0!=(vpp=table[j]);++j) {
				Obj cobj=(*vpp); Assert_(cobj);
				Char str=obj_(xmlTag,cobj); Assert_(str);
				Dict_set(PListClassDict,str,cobj);
				}
			}
		return PListClassDict;
		}

Obj PLIst_leafToClass(char *xmlkind,bool apple) {
	Obj ret=Dict_sub(PList_leafs(),xmlkind);
	if (apple || ret) return(ret);
	return CLIst_leafToClass(xmlkind);
	}

bool PList_leafClass(Obj obj,bool apple) {
	CodaCLASS *clas=classO(obj);
	void **vpp=0;
	for(int j=0;0!=(vpp=table[j]);++j) {
		Obj cobj=(*vpp);
		if (clas==classO(cobj)) return(1);
		}
	if (clas== &Isa_ConstChar) return(1);
	if (apple) return(0);
	return CList_leafClass(obj);
	}

$boot(PList) { PList_leafs_init(); }

	static char *xmlkeya[]={"amp;","lt;","gt;","quot;","apos;",NULL};
	static char  xmlchra[]="&<>\"\'";

	static int decode1amp(char *string,int j,int *kp) {
		char *str=string+j+1;
		int x=0; for(x=0;xmlkeya[x];++x) {
			if (cs_prefix(xmlkeya[x],str)) break;
			}
		if (xmlkeya[x]) {
			string[(*kp)++]=xmlchra[x];
			return cs_length(xmlkeya[x]);
			}
		if (*str=='#') {
			++str;
			int pos=cc_pos(';',str);
			if (pos!=EOF) {
				int utf=0;
				if (*str=='x') utf=strtoll(str+1,0,16);
				   else		   utf=strtoll(str,0,10);
				if (utf<=0) return(0);
				char *buff=string+(*kp);
				char *endcp=uc_encode(buff,utf,0);
				int len=endcp-buff; if (len<1 || len>4) return(0);
				(*kp)+=len;
				return(pos+2);
				}
			}
		return(0);
		}

bool PList_stringDecode(char *string,int *errPos) {
	int j,k,len=cs_length(string);
	for(j=0,k=0;j<len;++j) {
		int cc=string[j];
		if (cc=='&') {
			int len=decode1amp(string,j,&k);
			if (!len) {
				if (errPos) *errPos=j;
				return(0);
				}
			j+=len;
			}
		ei (cc=='<') {
			int bytes=0,size=PList_cdata_move(string+k,string+j,len-j,&bytes);
			if (!size) { string[k++]=cc; continue; }
			j+=size-1;
			k+=bytes;
			}
		else { string[k++]=cc; }
		}
	string[k]=0;
	return(1);
	}

static int encodedLength(char *a) {
	int len=0;
	for(int j=0;a[j];++j) {
		int cc=a[j];
		if (cc=='&') len+=5;
		ei (cc=='<' || cc=='>') len+=4;
		else ++len;
		}
	return(len);
	}

	static char *enckeya[]={"amp;","lt;","gt;","#38;"};
	static char  encchra[]="&<>&";
	static char *ituneka[]={"#38;","lt;","gt;","amp;"};

char* PList_stringEncode(int bufsize,char *buffer,char *a,Char *extra,bool amp38) {
	bufsize-=6;
	if (extra) {
		int size=encodedLength(a)+1;
		if (size>bufsize) { bufsize=size; buffer=alocO(size); *extra=buffer; }
		}
	int k=0;
	for(int j=0;a[j];++j) {
		int cc=a[j];
		if (k>=bufsize) OAbort("%s; overflow %d.",__func__,k);
		int p=cc_pos(cc,encchra);
		if (p==EOF)	{ buffer[k++]=cc; }
		  else	{
			char *key=(!amp38 ? enckeya[p] : ituneka[p]);
			sprintf(buffer+k,"&%s",key);
			k+=cs_length(key)+1;
			}
		}
	buffer[k]=0;
	return(buffer);
	}

	#define BLEN (4096+4096)
	typedef struct PLPTR2_ {
		int  iLevel,flags;
		char b[BLEN+16];
		oPrintf oprintf;
		Obj stream;
		} *PLPTR2;
	#undef  Self
	#define Self PLPTR2

	enum { MAXiLevel=100, };

	#define ePrintf(...) self->oprintf(self->stream,__VA_ARGS__)

	#define hasFlag(bitflag) (self->flags & bitflag)

	static pointer saveItem(Self self,Obj obj,int isArray,int indent) ;

Obj PList_toStream(Obj stream,Obj container,int flags) {
	if (flags & PLIST_Binary) {
		char *proto="Obj PList_BinaryWrite(Obj,Obj,int);";
		Obj (*writer)(Obj,Obj,int)=Plugin_fromProto(proto);
		if (!writer) OAbort("%s; no plugin: %s.",__func__,proto);
		return writer(stream,container,flags);
		}
	oPrintf oprintf=(pointer)fprintf;
	if (flags & PLIST_ObjectStream) {
		oprintf=OResponds(stream,oPrintf);
		if (!oprintf) OAbort("stream does not respond to: %s",sig_oPrintf);
		}
	if (!stream)  OAbort("Void stream?");
	Self self=alocS(sizeat(Self));
		_ flags=flags;
		_ oprintf=oprintf;
		_ stream=stream;

	if (!(flags & PLIST_NoEncoding))
		ePrintf("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");

	if (hasFlag(PLIST_Apple)) {
		if (!(flags & PLIST_NoDoctype))
			ePrintf("<!DOCTYPE plist PUBLIC \"-/" "/Apple%s/" "/DTD PLIST 1.0/" "/EN\" "
				"\"http:/" "/www.apple.com/DTDs/PropertyList-1.0.dtd\">\n",
				(hasFlag(PLIST_AddComputer)?" Computer":""));
		if (!(flags & PLIST_NoPVersion)) ePrintf("<plist version=\"1.0\">\n");
		  else						   ePrintf("<plist>\n");
		if (!saveItem(self,container,2,-1)) return(0);
		ePrintf("</plist>\n");
		}
	  else	{
		if (!(flags & PLIST_NoDoctype))
		ePrintf("<!DOCTYPE codalist SYSTEM \"http:/" "/www.coda-c.com/dtds/codalist.dtd\">\n");
		ePrintf("<codalist>\n");
		if (!saveItem(self,container,2,-1)) return(0);
		ePrintf("</codalist>\n");
	  	}
	return(container);
	}

	static void indentEqn(Self self,int indent) {
		char a[32];
		int j; for(j=0;j<indent && j<20;++j) a[j]='\t';
		a[j]=0;
		if (j>0) ePrintf("%s",a);
		}

	static void Data2_PLData2os(Data obj,Self self,int chunk,int indent,Char str) ;

	static pointer saveLeaf(Self self,Obj obj,int isArray,int indent) {
		if (!PList_leafClass(obj, hasFlag(PLIST_Apple)) ) {
			if (isa_(obj,HugeUID)) {
				HugeUID hhh=obj;
				cleanO Dictionary cfuid=newO(Dictionary);
				Dict_take(cfuid,"CF$UID",Huge_Value(*hhh));
				return saveItem(self,cfuid,isArray,indent);
				}
			if (obj==0 || isa_JsonNull(obj)) {
				if(hasFlag(PLIST_Apple)) {
					if (hasFlag(PLIST_Strict))
						OAbort("Strict: <null> is not allowed in Apple XML.");
					cleanO Dictionary cfnull=newO(Dictionary);
					Dict_take(cfnull,"CF$Null",Bool_Value(1));
					return saveItem(self,cfnull,isArray,indent);
					}
				  else {
					int leaf=(isArray==2);
					int dent=(isArray &1);
					if (dent && !leaf) { indentEqn(self,indent); ePrintf("\t"); }

					ePrintf("<null/>\n");
					return(self);
					}
				}
			OAbort("class[%s] is not a supported leaf type.",kindO(obj));
			}
		Char kind=obj_(xmlTag,obj); if (!kind) OAbort("[%s] not is supported a leaf type?",kindO(obj));
		cleanO Char aa=obj_(ToString,obj);
		if (!aa) OAbort("class[%s] did not produce a string with 'ToString'.",kindO(obj));

		Char extra=0;
		if (!isa_(obj,Data)) {
			if (!PList_stringEncode(BLEN,_ b,aa,&extra,hasFlag(PLIST_Amp38))) return(0);
			}
		cleanO Char temp=extra;
		char *buffer=(temp?temp:_ b);

		int leaf=(isArray==2);
		int dent=(isArray &1);
		if (!isArray && (hasFlag(PLIST_NL4Leafs) || isa_(obj,Data))) {
			ePrintf("\n");
			dent=1;
			}
		if (dent && !leaf) { indentEqn(self,indent); ePrintf("\t"); }

		if (isa_(obj,Bool)) ePrintf("<%s/>\n",buffer);
		ei (isa_(obj,Data)) {
			Data2_PLData2os(obj,self,(_ flags>>16)&0x0FFF,indent+1-leaf,aa);
			}
		else ePrintf("<%s>%s</%s>\n",kind,buffer,kind);

		if (hasFlag(PLIST_Apple) && isa_(obj,Char) && uc_faults(buffer))
			OAbort("Non-UTF8 string(%s)",buffer);

		if (isa_(obj,DateString)) {
			if (!DateString_toGmtime(buffer)) return(0);
			}

		return(self);
		}

	static pointer autoName(Self self,Obj container) {
		if (!hasFlag(PLIST_Apple) && OResponds(container,get_name)) {
			Char name=obj_(get_name,container);
			if (name && *name) {
				if (!PList_stringEncode(BLEN,_ b,name,0,0)) return(0);
				ePrintf("<keyword>%s</keyword>",_ b);
				}
			}
		return(self);
		}

	static pointer plist3Array(Self self,Array container,int indent) {
		if (Array_isaSet(container) && hasFlag(PLIST_Strict) && hasFlag(PLIST_Apple)) {
			OAbort("Strict: <Set> not allowed in Apple PList as <array>.");
			}
		indentEqn(self,indent);
		autoName(self,container);
		int j,nel=Array_get_count(container);
		if (nel==0) { ePrintf("<%s/>\n","array"); return(self); }
		ePrintf("<%s>\n","array");
		for(j=0;j<nel;++j) {
			Obj obj=Array_subInt(container,j);
			if (!saveItem(self,obj,1,indent)) return(0);
			}
		indentEqn(self,indent);
		ePrintf("</%s>\n","array");
		return(self);
		}

	static pointer plist3Dict(Self self,Dictionary container,int indent) {
		indentEqn(self,indent);
		autoName(self,container);
		int j,nel=Dictionary_get_count(container);
		if (nel==0) { ePrintf("<%s/>\n","dict"); return(self); }
		ePrintf("<%s>\n","dict");
		cleanO Pointer vector=Dictionary_AllKeys(container);
		if (!hasFlag(PLIST_UnsortedDict)) pointer_sort(vector,nel,strcmp,0);
		for(j=0;j<nel;++j) {
			char *cp=vector[j];
			Obj obj=Dictionary_keywordFromWord(cp)->item;
			if (!PList_stringEncode(BLEN,_ b,cp,0,0)) return(0);
			indentEqn(self,indent);
			ePrintf("\t<key>%s</key>",_ b);
			if (hasFlag(PLIST_Apple) && uc_faults(_ b))
				OAbort("Non-UTF8 Dict Key(%s)",_ b);
			if (!saveItem(self,obj,0,indent)) return(0);
			}
		indentEqn(self,indent);
		ePrintf("</%s>\n","dict");
		return(self);
		}

	static pointer saveItem(Self self,Obj obj,int isArray,int indent) {
		if (++_ iLevel>MAXiLevel) OAbort("too many levels(%d). Circular?",_ iLevel);
		if (isa_(obj,Dictionary)) {
			if (!isArray) ePrintf("\n");
			if (!plist3Dict(self,obj,indent+1)) return(0);
			}
		ei (isa_(obj,Array)) {
			if (!isArray) ePrintf("\n");
			if (!plist3Array(self,obj,indent+1)) return(0);
			}
		ei (isa_(obj,Keyword)) {
			if (hasFlag(PLIST_Apple)) OAbort("Keyword is not supported for Apple PLists");
			Keyword kkk=obj;
			if (!PList_stringEncode(BLEN,_ b,kkk->word,0,0)) return(0);
			  else {
				ePrintf("\t<keyword>%s</keyword>",_ b);
				if (!saveItem(self,kkk->item,0,indent)) return(0);
				}
			}
		else {
			if (!saveLeaf(self,obj,isArray,indent)) return(0);
			}
		--_ iLevel;
		return(self);
		}

	static char* data2White(int ix,int chunk,int indent,int atend) {
		if (chunk<=0) return(0);
		if (ix%chunk && !atend) return(0);
		static char buf[32]="";
		char *cp=buf; *cp++ ='\n';
		int j; for(j=0;j<indent && j<20;++j) *cp++ = '\t';
		*cp=0;
		return(buf);
		}

	static void Data2_PLData2os(Data obj,Self self,int chunk,int indent,Char str) {
		if (chunk && (chunk<4 || chunk>4095)) chunk=72;
		ePrintf("<%s>","data");
			int len=cs_length(str);
		cleanO Char work=newOC(Char,2*len+1024);
		int j,cc,pos=0;
		for(j=0;0!=(cc=str[j]);++j) {
			char *temp=data2White(j,chunk,indent,0);
			if (temp) { int lll=cs_length(temp); cs_strcopy(work+pos,temp); pos+=lll; }
			work[pos++]=cc;
			}
			char *temp=data2White(j,chunk,indent,1);
			if (temp) { int lll=cs_length(temp); cs_strcopy(work+pos,temp); pos+=lll; }
		work[pos]=0;
		ePrintf("%s",work);
		ePrintf("</%s>\n","data");
		}

Obj PList_save(char *file,Obj container,int flags) {
	cleanO FileMem fff=FileMem_Open(0); if (!fff) return(0);
	if (!PList_toStream(fff,container,flags|PLIST_ObjectStream)) return(0);
	cleanO Data data=FileMem_ToData(fff); if (!data) OAbort("NEVER empty?");
	FILE *os=fopen(file,"wb"); if (!os) OAbort("can't write file.");
		huge size=Data_count(data);
		huge nout=file_write(os,data,size);
		if (nout!=size) OAbort("WRITE error %d vs %d?",(int)nout,(int)size);
		if (fclose(os)) OAbort("fclose Error?");
	return(container);
	}

Obj PList_Load(char *file,int flags) {
	cleanO Data data=Data_FromFile(file); if (!data) return(0);
	return PList_FromBlock(Data_count(data),data,flags);
	}

static CDictionary Plugin_dictionary_dict=0;

CDictionary Plugin_dictionary() {
	if (!Plugin_dictionary_dict) {
		Plugin_dictionary_dict=newO(CDictionary);
		Global_objectAddress(&Plugin_dictionary_dict);
		}
	return(Plugin_dictionary_dict);
	}

void Plugin_register(char *proto,pointer address) {
	Dict_set(Plugin_dictionary(),proto,address);
	}

pointer Plugin_fromProto(char *proto) {
    return Dict_sub(Plugin_dictionary(),proto);
	}

#if 1==0
void PList_Binary() {
	}
#endif
	typedef struct { void *context; int (*fun)(const void *aa, const void *bb, void *context); } ZsortData;

#if LINUX
	static int zsortSwap(const void *aa, const void *bb,void *context) {
		ZsortData *data=(ZsortData*)context;
		return data->fun(*((void **)aa),*((void **)bb),data->context);
		}
#else
	static int zsortSwap(void *context, const void *aa, const void *bb) {
		ZsortData *data=(ZsortData*)context;
		return data->fun(*((void **)aa),*((void **)bb),data->context);
		}
#endif

void pointer_sort(pointer base,int nel,void *IfunVVC,void *context) {
	ZsortData tmp; tmp.context=context; tmp.fun=IfunVVC;
	#if LINUX
		qsort_r(base, nel, sizeof(void *), zsortSwap, &tmp);
	#elif WIN32
		qsort_s(base, nel, sizeof(void *), zsortSwap, &tmp);
	#elif APPLE
		qsort_r(base, nel, sizeof(void *), &tmp, zsortSwap);
	#else
		#error "pointer_sort: unknown operating system";
	#endif
	}

	#include <math.h>
	#include <time.h>

	#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
		#define bswap32host(x) (x)=__builtin_bswap32(x)
		#define bswap64host(x) (x)=__builtin_bswap64(x)
	#else
		#define bswap32host(x)
		#define bswap64host(x)
	#endif

enum {  MarkerNull=0x00, MarkerFalse=0x08, MarkerTrue=0x09, MarkerFill=0x0F, MarkerInt=0x10,
        MarkerReal=0x20, MarkerDate =0x33, MarkerData=0x40, MarkerASCII=0x50,
		MarkerUnicode16=0x60, MarkerUID=0x80, MarkerArray=0xA0, MarkerSet=0xC0, MarkerDict=0xD0 };

	#define delta_1970_2001 978307200

	typedef unsigned long long uhuge;
	typedef unsigned char      uchar;

	typedef struct {
		uchar unused[6],offsWid,refSize;
		uhuge nObj,topZero,offsAt;
		} Trailer;

	static_assert(sizeof(Trailer)==32,"Wrong Trailer/end-block size?");

	static Char ANull=Os("<0?>");

	typedef struct { uchar magicXXX[6],versionXXX[2]; } Header;

	enum { MAXLEVEL=100, };

	typedef struct PLPTR3_ { unsigned char* blob; int blobNel; Trailer ender; } *PLPTR3;
	#undef  Self
	#define Self PLPTR3

	#define numObjects	self->ender.nObj
	#define topObject	self->ender.topZero
	#define offsetsAt	self->ender.offsAt
	#define offWidth	self->ender.offsWid
	#define refWidth	self->ender.refSize

	static uhuge getint(uchar *data,int size) {
		uhuge ival=0; Assert_(size>=1 && size<=8);
		for(int j=0;j<size;++j) ival=(ival<<8)+data[j];
		return(ival);
		}
	static uhuge getoffset(Self self,int at) {
		return getint(_ blob + offsetsAt + at * offWidth, offWidth);
		}

	static void *bplistAbort(Self self,char *msg) {
		Error_F("PList_BinaryLoad; %s",msg);
		freeO(msg);
		return(0);
		}

  	#define abortF(...)	return bplistAbort(self,Char_F(__VA_ARGS__))

	static void *bplistTop(Self self) ;
	static void *bplistObject(Self self,int idx,int level,bool isakey) ;

Obj PList_BinaryLoad(int count,pointer block) {
	Self self=alocS(sizeat(Self));
		_ blob=block;
		_ blobNel=count;
	if (_ blobNel<sizeof(_ ender)+sizeof(Header)+1) abortF("file too small");
		if (!bplistTop(self)) return(0);
	void* obj=bplistObject(self,topObject,0,0); if (!obj) return(0);
	return(obj);
	}

	static void *bplistTop(Self self) {

		if (memcmp("bplist0",_ blob,7)) abortF("BP: bad magic/header.");

		memcpy(&_ ender,_ blob+_ blobNel - sizeof(_ ender),sizeof(_ ender));

			bswap64host(numObjects);
			bswap64host(topObject);
			bswap64host(offsetsAt);

		if (numObjects > 0x7FFFFFFF) abortF("BP: too many objects.");
		if (offsetsAt  > 0x7FFFFFFF) abortF("BP: off tbl off too large.");
		if (numObjects < 1) 		 abortF("BP: no objects.");
		if (topObject >= numObjects) abortF("BP: bad top object.");
		if (offsetsAt  < 9) 		 abortF("BP: off tbl off too small.");
		if (offsetsAt >= _ blobNel-sizeof(_ ender))
									 abortF("BP: size %d OTO %d",_ blobNel,(int)offsetsAt);
		if (offWidth<1||offWidth>8)  abortF("BP: offWidth bad %d",offWidth);
		if (refWidth<1||refWidth>8)	 abortF("BP: refWidth bad %d",refWidth);

		uhuge tablesize = numObjects * offWidth;
		if (_ blobNel != offsetsAt + tablesize + sizeof(_ ender))
									 abortF("BP: file size recompute error.");
		for(int j=0;j<numObjects;++j) {
			uhuge off=getoffset(self,j);
			if (off<sizeof(Header) || off>=offsetsAt) abortF("BP: obj #%d bad",j);
			}

		return(self);
		}

	static void *buildDict(Self self,void *dict,int level,int nel,uchar *args) {
		++level; int j,k;
		for(j=0;j<nel;++j) {
			k=getint(args+j*refWidth,refWidth);
			if (k<0 || k>=numObjects) abortF("BP: Dict: Key range error %d/%d.",k,(int)numObjects);
			cleanO Char key=bplistObject(self,k,level,1); if (!key) return(0);

			k=getint(args+(nel+j)*refWidth,refWidth);
			if (k<0 || k>=numObjects) abortF("BP: Dict: Obj range error %d/%d.",k,(int)numObjects);
			Obj obj=bplistObject(self,k,level,0); if (!obj) return(0);

			if (obj==ANull) obj=JsonNull_Value();
			Dict_take(dict,key,obj);
			}
		return(dict);
		}

	static void *buildArray(Self self,void *array,int level,int nel,uchar *args) {
		++level; int j,k;
		for(j=0;j<nel;++j) {
			k=getint(args+j*refWidth,refWidth);
			if (k<0 || k>=numObjects) abortF("BP: Array: Obj range error %d/%d.",k,(int)numObjects);
			Obj obj=bplistObject(self,k,level,0); if (!obj) return(0);
			if (obj==ANull) obj=0;
			Array_take(array,obj);
			}
		return(array);
		}

	static void *bplistObject(Self self,int idx,int level,bool isakey) {
		if (level>=MAXLEVEL) abortF("BPO; too deep, %d / %d",level,MAXLEVEL);
		huge off=getoffset(self,idx);

		uchar *cp=(unsigned char *)_ blob+off;
		uchar mark= *cp++;
		uchar nib1= mark & 0xF0;
		uchar nib2= mark & 0x0F;
		uhuge nel=nib2;
		if (nel==0x0F)	{
			uchar intmark= *cp++;
			if ((intmark & 0xF0)!=MarkerInt) abortF("BPO: intmark error.");
			int shift=(intmark & 0x0F);
			if (shift>3)	abortF("BPO; intmark shift error.");
			int valSize = 1<<shift;
			nel=getint(cp,valSize);
			cp+=valSize;
			}
		if (isakey && nib1!=MarkerASCII && nib1!=MarkerUnicode16)
			abortF("BPO; dict key !ASCII,!UTF 0x%02x",mark);
		int valSize; huge hval;
		switch(nib1) {

			case MarkerDict: ;
				Dictionary dict=newO(Dictionary);
				if (!buildDict(self,dict,level,nel,cp)) { freeO(dict); return(0); }
				return dict;

			case MarkerSet:
			case MarkerArray: ;
				Array array=newO(Array);
				if (!buildArray(self,array,level,nel,cp)) { freeO(array); return(0); }
				if (nib1==MarkerSet) Array_toaSet(array);
				return array;

			case MarkerUnicode16: ;
				#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
				{ unsigned short *u16=(unsigned short*)cp;
				for(int j=0;j<nel;++j) u16[j]= ((u16[j]<<8)&0xFF00) | ((u16[j]>>8)&0xFF);
					}
				#endif

				int faults=0; Char ustr=u2_ToUc(cp,nel,&faults);
				if (faults)	{ freeO(ustr); abortF("UTF-16[%d] -> UTF-8 failed, %d faults.",(int)nel,faults); }
				return(ustr);

			case MarkerASCII:
				{ Char retstr=newOC(Char,nel+1);
				cs_blockCopy(retstr,cp,nel);
				return(retstr); }

			case MarkerUID:
				valSize=nib2+1;
				if (valSize>8)	abortF("BPO; UID/Index nib2 value error.");
				hval=getint(cp,valSize);
				return HugeUID_Value(hval);

			case MarkerInt:
				if (nib2==4) {
					Huge hhh=newOC(Huge,2);
						hhh[1]=getint(cp,8);
						hhh[0]=getint(cp+8,8);
					return(hhh);
					}
				if (nib2>3)	abortF("BPO; Int nib2 shift error.");
				valSize = 1<<nib2;
				hval=getint(cp,valSize);
				return Huge_Value(hval);

			case (MarkerDate&0xF0):
				if (nib2!=3) abortF("BPO; Date nib2 shift error.");
			case MarkerReal:
				valSize = 1<<nib2;
				if (nib2==2) {
					int4 ival=getint(cp,valSize);
					float *fp=(float *)&ival;
					float fval=(*fp);
					return Real_Value(fval);
					}
				if (nib2!=3) abortF("BPO; Real nib2 shift error.");
				hval=getint(cp,valSize);

				double *dp=(double *)&hval;
				double dval=(*dp);
				if (nib1==MarkerReal) return Real_Value(dval);

				hval=llround(dval);
				time_t clock=(hval+delta_1970_2001);
				struct tm *when=gmtime(&clock);
				if (!when) abortF("bad gmtime: %s",OError());
				return DateString_FromGmtime(when);

			case MarkerData:
				return Data_NewBlock(NULL,nel,cp);

			case 0x00:
			if (nib2==MarkerTrue)	return Bool_Value(1);
			if (nib2==MarkerFalse)	return Bool_Value(0);
			if (nib2==MarkerNull)	return ANull;
			default: 				abortF("unknown marker: 0x%02x",mark);
			}
		abortF("Unknown Marker: 0x%02x",mark);
		}

$boot(plload) {
	Plugin_register("Obj PList_BinaryLoad(int,pointer);", PList_BinaryLoad);
	}

	#include <time.h>
	#include <errno.h>

	typedef struct PLPTR4_ { Obj stream; huge outTell; oWrite owrite; } *PLPTR4;
	#undef Self
	#define Self PLPTR4

	static pointer ewrite(Self self,pointer buffer,int len) {
		if (len!=_ owrite(_ stream,buffer,len)) OAbort("stream write error?");
		_ outTell+=len;
		return(self);
		}

	#define eWrite(buf,len) if (!ewrite(self,buf,len)) return(0)

static void PList_setoat(Dict objdict,pointer address,huge oat) {
	char key[64]; snprintf(key,sizeof(key),"%p",address);
	if (oat==0) oat=(-2);
	Dict_set(objdict,key,(Obj)(intptr_t)oat);
	}

static huge PList_getoat(Dict objdict,pointer address) {
	char key[64]; snprintf(key,sizeof(key),"%p",address);
	huge oat=(intptr_t)Dict_sub(objdict,key);
	if (!oat) return(EOF);
	if (oat==(-2)) oat=0;
	return(oat);
	}

static int4 PList_flatten(Obj container,Array objlist,Dict objdict,Obj uniquer,int flags) {

	bool isadup=0;
	Obj this=Unique_obj(uniquer,container,&isadup,flags);
	if (isadup) {
		int4 isat=PList_getoat(objdict,this); Assert_(isat>=0);
		PList_setoat(objdict,container,isat);
		return(isat);
		}

	int oat=Array_count(objlist);
	Array_addObject(objlist,container);
	PList_setoat(objdict,container,oat);

	if (isa_(container,Dict)) {
		cleanO Pointer vector=Dictionary_AllKeys(container);
		int nnn=Pointer_count(vector);
		if (!(flags & PLIST_UnsortedDict)) pointer_sort(vector,nnn,strcmp,0);
		for(int j=0;j<nnn;++j) {
			cleanO Char word=Char_Value(vector[j]);
			int4 loc1=PList_flatten(word, objlist, objdict,uniquer,flags);
			PList_setoat(objdict,vector[j],loc1);
			}
		for(int j=0;j<nnn;++j) {
			Keyword key=Dictionary_keywordFromWord(vector[j]);
			PList_flatten(key->item, objlist, objdict,uniquer,flags);
			}
		}
	ei (isa_(container,Array)) {
		int count=Array_count(container);
		for(int j=0;j<count;++j) {
			PList_flatten(Array_sub(container,j), objlist, objdict,uniquer,flags);
			}
		}
	return(oat);
	}

static int byteCount(uhuge count) {
	if (count<=0xFF) return(1);
	ei (count<=0xFFFF) return(2);
	ei (count<=0xFFFFFFFF) return(4);
	return(8);
	}

static pointer writeInt(Self self, uhuge bigint) {
	uchar marker=MarkerInt;
	int nbytes=byteCount(bigint);
	switch(nbytes) {
		case 1: marker |= 0; break;
		case 2: marker |= 1; break;
		case 4: marker |= 2; break;
		default:marker |= 3; break;
		}
	bswap64host(bigint);
    eWrite(&marker, 1);
    eWrite( ((Char)&bigint) + sizeof(bigint) - nbytes, nbytes);
	return(self);
	}

static pointer outref(Self self,Obj ele,Dict objdict,int size) {
	int4 ref=PList_getoat(objdict,ele); Assert_(ref>=0);
	bswap32host(ref);
	eWrite(((Char)&ref)+sizeof(ref)-size,size);
	return(self);
	}

	static int FILE_write(FILE *os,pointer buf,int nel) { return fwrite(buf,1,nel,os); }
	static double DateString_toReal(DateString obj) ;

Obj PList_BinaryWrite(Obj stream,Obj container,int flags) {

	Self self=alocS(sizeat(Self));
		_ stream=stream;

	if (flags & PLIST_ObjectStream) {
		_ owrite=OResponds(stream,oWrite);
		if (!_ owrite) OAbort("%s; stream does not respond to 'oWrite'",__func__);
		}
	  else {
	  	_ owrite=(pointer)FILE_write;
		}

	cleanO Array objlist=newO(Array);
	cleanO Dict objdict=newO(CDictionary);
	cleanO Obj uniquer=Unique_obj(0,0,0,0);
    PList_flatten(container,objlist,objdict,uniquer,flags);
    int nobj=Array_count(objlist);
    cleanO Huge offsets=newOC(Huge,nobj);
	eWrite("bplist00", 8);

    Trailer ender; cs_blockZero(&ender,sizeof(ender));
		ender.nObj=nobj; bswap64host(ender.nObj);
		ender.refSize = byteCount(nobj);
		int refsize=ender.refSize;

    for (int jj=0;jj<nobj;++jj) {
		Obj obj=Array_sub(objlist,jj);
		offsets[jj]=_ outTell;

		if (0) ;
		ei (isa_(obj,DateString)) {
			double date=DateString_toReal(obj); if (date==0.0) return(0);
			uchar marker=MarkerDate;
			eWrite(&marker,1);
			huge temp=(*( (Huge)&date) ); bswap64host(temp);
			eWrite(&temp,sizeof(temp));
			}
		ei (isa_(obj,Char)) {
	    	int count=cs_length(obj);
			int faults=0; int codes=uc_scan(obj,count,-1,NULL,&faults);
			if (codes==count) {
				uchar marker = (MarkerASCII | (count < 15 ? count : MarkerFill));
				eWrite(&marker,1);
				if (count>=15) if (!writeInt(self,count)) return(0);
				eWrite(obj, count);
				}
			ei (faults) OAbort("%s; string contains %d faults ?",__func__,faults);
			else {
				cleanO Short u16=uc_ToU2(obj,count,NULL);
				int nel=Short_count(u16)-1;
				#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
				for(int j=0;j<nel;++j) u16[j]= ((u16[j]<<8)&0xFF00) | ((u16[j]>>8)&0xFF);
				#endif
				uchar marker = (MarkerUnicode16 | (nel < 15 ? nel : MarkerFill));
				eWrite(&marker,1);
				if (nel>=15) if (!writeInt(self,nel)) return(0);
				eWrite(u16, nel*sizeof(short));
				}
			}

		ei (isa_(obj,Real)) {
			huge temp=(*( (Huge)obj) ); bswap64host(temp);
			uchar marker = MarkerReal | 3;
			eWrite(&marker,1);
			eWrite(&temp,sizeof(temp));
			}

		ei (isa_(obj,Huge) && sizeO(obj)==16) {
			Huge hhh=obj;
			huge h0=hhh[0]; bswap64host(h0);
			huge h1=hhh[1]; bswap64host(h1);
			uchar marker=MarkerInt | 4;
			eWrite(&marker, 1);
			eWrite( ((Char)&h1), sizeof(huge));
			eWrite( ((Char)&h0), sizeof(huge));
			}

		ei (isa_(obj,HugeUID)) {
			uhuge bigint= *( (HugeUID)obj );
			int nbytes=byteCount(bigint);
			uchar marker=MarkerUID | (nbytes-1);
			bswap64host(bigint);
			eWrite(&marker, 1);
			eWrite( ((Char)&bigint) + sizeof(bigint) - nbytes, nbytes);
			}

		ei (isa_(obj,Huge)) {
			if (!writeInt(self, *( (Huge)obj) )) return(0);
			}

		ei (isa_(obj,Bool)) {
			uchar marker =( *( (Bool)obj ) ? MarkerTrue : MarkerFalse);
			eWrite(&marker, 1);
			}

		ei (isa_(obj,Data)) {
			int count=sizeO(obj);
			uchar marker = (MarkerData | (count < 15 ? count : MarkerFill));
			eWrite(&marker,1);
			if (count>=15) if (!writeInt(self,count)) return(0);
			eWrite(obj,count);
			}

		ei (isa_(obj,Dictionary)) {
			int count=Dictionary_count(obj);
			uchar marker = (MarkerDict | (count < 15 ? count : MarkerFill));
			eWrite(&marker,1);
			if (count>=15) if (!writeInt(self,count)) return(0);

			cleanO Pointer vector=Dictionary_AllKeys(obj);
			int nnn=Pointer_count(vector);
			if (!(flags & PLIST_UnsortedDict)) pointer_sort(vector,nnn,strcmp,0);
			for(int j=0;j<nnn;++j) {
				if (!outref(self,vector[j],objdict,refsize)) return(0);
				}
			for(int j=0;j<nnn;++j) {
				Keyword key=Dictionary_keywordFromWord(vector[j]);
				if (!outref(self,key->item,objdict,refsize)) return(0);
				}
			}

		ei (isa_(obj,Array)) {
			int count=Array_count(obj);
			uchar marker = (MarkerArray | (count < 15 ? count : MarkerFill));
			if (Array_isaSet(obj))
				  marker = (MarkerSet   | (count < 15 ? count : MarkerFill));
			eWrite(&marker,1);
			if (count>=15) if (!writeInt(self,count)) return(0);
			for(int jj=0;jj<count;++jj) {
				Obj ele=Array_sub(obj,jj);
				if (!outref(self,ele,objdict,refsize)) return(0);
				}
			}

		ei (obj==0 || isa_JsonNull(obj)) {
			uchar marker = MarkerNull;
			eWrite(&marker, 1);
			}

		else OAbort("%s; unknown object(%s) ?",__func__,kindO(obj));
		}

    int so_far=_ outTell;
    ender.offsAt = so_far; bswap64host(ender.offsAt);
    ender.offsWid = byteCount(so_far);

	int width=ender.offsWid;
    for(int jj=0;jj<nobj;++jj) {
		uhuge swapped=offsets[jj]; bswap64host(swapped);
		eWrite(	((Char)&swapped)+sizeof(swapped)-width,width);
    	}
    eWrite(&ender,sizeof(ender));
	return(container);
	}

static double DateString_toReal(DateString obj) {
	struct_tm *when=DateString_toGmtime(obj); if (!when) return(0.0);
	time_t clock=timegm(when);
	if (clock==(time_t)(-1)) { Error_F("%s; bad timegm(%s), errno %d ?",__func__,obj,errno); return(0.0); }
	clock -= delta_1970_2001;
	return((double)clock);
	}

$boot(plwrite) {
	Plugin_register("Obj PList_BinaryWrite(Obj,Obj,int);",PList_BinaryWrite);
	}

	pointer bogus1plist() { return PList_BinaryLoad; }
	pointer bogus2plist() { return PList_BinaryWrite; }

void PList_Binary() {
	bogus1plist();
	bogus2plist();
	}

static Array Unique_array(Obj uniquer,Obj obj) {
	Char kind=kindO(obj);
	Array list=Dict_sub(uniquer,kind);
	if (!list) { list=newO(Array); Dict_take(uniquer,kind,list); }
	return(list);
	}

Obj Unique_obj(Obj uniquer,Obj obj,bool *isadup,int flags) {
	if (!uniquer) return newO(Dictionary);
	if (flags & BINARY_NoComp) { *isadup=0; return(obj); }

	bool isChar=isa_(obj,Char);
	if (isChar) ;
	ei (isa_(obj,Huge)) ;
	ei ((flags & BINARY_MaxComp) && (
		isa_(obj,Bool) ||
		isa_(obj,Real) ||
		isa_(obj,Data) ||
		isa_(obj,Void) ||
		obj==0)) ;
	else { *isadup=0; return(obj); }

	*isadup=1;

	Array list=Unique_array(uniquer,obj);
	Pointer ptr=Array_rawAddress(list);
	int objsize=sizeO(obj);
	int nel=Array_count(list);
	for(int jj=0;jj<nel;++jj) {
		pointer prev= ptr[jj];
		if (obj==0) {
			if (prev==0) return(prev);
			}
		ei (isChar) {
			if (cs_exact(prev,obj)) return(prev);
			}
		else {
			int prevsize=sizeO(prev);
			if (prevsize==objsize) {
				if (0==cs_blockCmp(prev,obj,objsize)) return(prev);
				}
			}
		}
	Array_addObject(list,obj);
	*isadup=0; return(obj);
	}

void Json_data2os(oPrintf jprintf,pointer stream,char *str,int4 flags) {
	int chunk=((flags>>16)&0x0FFF);
	if (chunk && (chunk<4 || chunk>4095)) chunk=72;

	if (!chunk) { jprintf(stream,"%s",str); return; }

	int len=cs_length(str);
	char buf[chunk+1];
	for(int j=0;j<len;++j) {
		int mod=j%chunk;
		if (mod==0) jprintf(stream,"\n");
		buf[mod]=str[j];
		if (mod==chunk-1 || j==len-1) {
			buf[mod+1]=0;
			jprintf(stream,"%s",buf);
			}
		}
	}

	#include <math.h>

	#define ALEN 4096

	typedef struct PLPTR5_ {
		unsigned char* blob;
		int lno,pos,blobNel,iLevel,flags;
		char a[ALEN+16];
		} *PLPTR5;
	#undef  Self
	#define Self PLPTR5

	enum { MAXjiLevel=100, };

	#define hasFlag(bitflag) (self->flags & bitflag)

	static pointer jsonAbort(Self self,Char msg) {
		Error_F("line %d: %s",_ lno+1,msg);
		freeO(msg); msg=0;
		return(0);
		}

	#undef  abortP
  	#define abortP(...)	return(jsonAbort(self,Char_F(__VA_ARGS__)))

	static int jsn_getCc(Self self) {
		if (_ pos >= _ blobNel) return(EOF);
		int cc=_ blob[_ pos++];
		if (cc=='\n') ++_ lno;
		return(cc);
		}
	static void jsn_ungetCc(Self self,int count) {
		_ pos-=count;
		if (_ pos<0) _ pos=0;
		}

	static ConstChar kColon=Os(":");
	static ConstChar kComma=Os(",");

	static int jsn_ltSize(Self self,int thequote) {
		int backs=0;
		for(int j=_ pos; j< _ blobNel;++j) {
			int cc= _ blob[j];
			if (cc==thequote && (backs&1)==0) return(j - _ pos);
			if (cc=='\\') ++backs;
			  else        backs=0;
			}
		return(EOF);
		}
	static pointer gethex4(Self self,int *cpointptr) {
		char buf[8];
		for(int j=0;j<4;++j) {
			int cc=jsn_getCc(self); if (cc==EOF) abortP("EOF Hex[%d]",j);
			if ( (cc>='0' && cc<='9') || (cc>='a' && cc<='z') || (cc>='A' && cc<='Z') ) ;
			else abortP("Bad Hex[%d]= #%d",j,cc);
			buf[j]=cc;
			}
		buf[4]=0;
		*cpointptr=(int)strtoll(buf,0,16);
		return(self);
		}

	static pointer jsnLoadKey(Self self,Char *extra,int thequote) {
		int cc,p=0;
		int size=jsn_ltSize(self,thequote); if (size==EOF) abortP("EOF loading KEY.");
		cleanO Char temp=(size<=ALEN ? 0 : newOC(Char,size+8));
		if (temp && !extra) abortP("Item too large: %d w/o extra",size);
		char *buffer=(temp ? temp : _ a);
		int backs=0;
		while(1) {
			cc=jsn_getCc(self); if (cc==EOF) abortP("EOF in LoadKey.(NEVER)");
			if (cc==thequote && (backs&1)==0) break;
			if (cc=='\\' && !backs) { ++backs; continue; }
			if (backs) { backs=0;
				if (cc=='b') cc='\b';
				ei (cc=='f') cc='\f';
				ei (cc=='t') cc='\t';
				ei (cc=='r') cc='\r';
				ei (cc=='n') cc='\n';
				ei (cc=='u') {
					int cpoint=0; if (!gethex4(self,&cpoint)) return(0);
					if (0xD800==(cpoint &0xFC00)) {
						cc=jsn_getCc(self);
						if (cc!='\\') {
							if (cc!=EOF) jsn_ungetCc(self,1);
							}
						  else {
							int c2=jsn_getCc(self);
							if (c2!='u') {
								jsn_ungetCc(self,c2!=EOF ? 2 : 1);
								}
							  else {
								int lowsur=0; if (!gethex4(self,&lowsur)) return(0);
								if (0xDC00!=(lowsur &0xFC00)) abortP("Bad low surrogate 0x%04x",lowsur);
								cpoint=(((cpoint &0x3FF)<<10 | (lowsur &0x3FF))+0x10000);
								}
							}
						}
					ei (0xDC00==(cpoint &0xFC00)) {
						}
					ei (cpoint==0) {
						cpoint=UTF8max;
						}
					if (p+4>=size) abortP("String too large. %d (extra %p) Never?",p,extra);
					char *buff=buffer+p;
					char *endcp=uc_encode(buff,cpoint,0);
					int len=endcp-buff; if (len<1 || len>4) len=0;
					p+=len;
					continue;
					}
				}
			if (p>=size) abortP("Key|Item too large. %d (extra %p) NEVER",p,extra);
			buffer[p++]=cc;
			}
		buffer[p]=0;
		if (uc_faults(buffer)) abortP("Non-UTF8 dict:key(%s)",buffer);
		if (temp) *extra=keepO(temp);
		return(self);
		}

	#undef ALEN

				static pointer jsnLoadItem(Self self,char *ender,bool top) ;
	static pointer jsnLoadDict(Self self,Dictionary dict) {
		while(1)	{
			static Char ender=Os("}");
			cleanO Obj key=jsnLoadItem(self,ender,0); if (!key) return(0);
			if (key==ender) break;
			if (!isa_(key,Char)) abortP("Bad Dictionary key?");

			cleanO Obj colon=jsnLoadItem(self,0,0); if (!colon) return(0);
			if (colon!=kColon) {
				cleanO Char msg=ToContainer(colon);
				abortP("Dict: missing colon? ( %s )",msg);
				}

			cleanO Obj obj=jsnLoadItem(self,0,0); if (!obj) return(0);
			Dict_set(dict,key,obj);
			cleanO Obj comma=jsnLoadItem(self,ender,0); if (!comma) return(0);
			if (comma==ender) break;
			if (comma!=kComma) {
				cleanO Char msg=ToContainer(comma);
				abortP("Dict: missing comma? ( %s )",msg);
				}
			}
		return("OK");
		}

	static pointer jsnLoadArray(Self self,Array array) {
		while(1) {
			static Char ender=Os("]");
			Obj obj=jsnLoadItem(self,ender,0); if (!obj) return(0);
			if (obj==ender) break;
			if (obj==kComma) continue;
			if (obj==kColon) OAbort("Colon in array?");
			if (isa_JsonNull(obj)) { freeO(obj); obj=0; }
			Array_take(array,obj);
			}
		return("OK");
		}
	static pointer ptrHex(Self self) {
		int j=0;
		for(j=0;j+_ pos < _ blobNel && j<63;++j) {
			int cc= cc_toCap(_ blob[j+_ pos]);
			if (cc>='0' && cc<='9') ;
			ei (cc>='A' && cc<='F') ;
			else break;
			}
		Char string=alocS(j+1); cs_blockCopy(string,_ blob+ _ pos,j);
		_ pos +=j;
		huge ival=strtoll(string,0,16);
		return Huge_Value(ival);
		}

	static pointer ptrNumber(Self self) {
		int c1=jsn_getCc(self);
		int c2=jsn_getCc(self);
		if (c1=='0' && c2=='x') return ptrHex(self);
		jsn_ungetCc(self,c2==EOF ? 1 : 2);

		int j=0;
		for(j=0;j+_ pos < _ blobNel && j<127;++j) {
			int cc= _ blob[j+_ pos];
			if (cc>='0' && cc<='9') ;
			ei (cc=='-' || cc=='.' || cc=='e' || cc=='E' || cc=='+') ;
			else break;
			}
		Char string=alocS(j+1); cs_blockCopy(string,_ blob+ _ pos,j);
		_ pos +=j;
		double dval=strtod(string,0);

		char buf[64]; snprintf(buf,sizeof(buf),"%.16g",dval);
		bool isreal=cc_inString('.',buf);
		if (isreal) return Real_Value(dval);
		return Huge_Value(llround(dval));
		}
		Data Data_FromString(Data self,char *string);
		DateString DateString_FromString(DateString self,char *str);

	static bool nameCc(int cc,bool notfirst) {
		cc&=0xFF;
		if (cc>127) return(1);
		if ( (cc>='a' && cc<='z') || (cc>='A' && cc<='Z') || cc=='_' || cc=='$') return(1);
		if (notfirst && (cc>='0' && cc<='9') ) return(1);
		return(0);
		}

	static void jsndecLevel(const void *vp) {
		Self self=(*(Self*)vp);
		--_ iLevel;
		}
	static void jsnSelfNOP(Self junk) { }

	static Obj jsnLoadItem(Self self,char *ender,bool top) {
		if (++_ iLevel>MAXjiLevel) OAbort("too many levels(%d). Circular?",_ iLevel);
		const $CLEANUP(jsndecLevel) Self selfcopy= self;
			jsnSelfNOP(selfcopy);

		int cc=jsn_getCc(self);
		while(cc==32 || cc=='\t' || cc=='\r' || cc=='\n') cc=jsn_getCc(self);

		if (ender && *ender==cc) return(ender);

		if (cc=='{') {
			Dictionary dict=newO(Dictionary);
			if (!jsnLoadDict(self,dict)) { freeO(dict); return(0); }

			if (Dictionary_count(dict)==1 && !hasFlag(PLIST_Strict)) {
				Keyword key=Dictionary_scan(dict);
				if (cs_exact("CF$UID",key->word) && isa_(key->item,Huge)) {
					Huge rrr=key->item;
					HugeUID uuu=HugeUID_Value( *rrr );
					freeO(dict);
					return(uuu);
					}
				ei (cs_exact("CF$Date",key->word) && isa_(key->item,Char)) {
					Obj ret=DateString_FromString(0,key->item);
					freeO(dict);
					return(ret);
					}
				ei (cs_exact("CF$Data",key->word) && isa_(key->item,Char)) {
					Obj ret=Data_FromString(0,key->item);
					freeO(dict);
					return(ret);
					}
				}
			return dict;
			}
		if (cc=='[') {
			Array array=newO(Array);
			if (!jsnLoadArray(self,array)) { freeO(array); return(0); }
			return array;
			}

		if (cc==':') return kColon;
		if (cc==',') return kComma;

		if (cc=='\"' || cc=='\'') {
			Char extra=0;
			if (!jsnLoadKey(self,&extra,cc)) return(0);
			return(extra ? extra : Char_Value(_ a));
			}

		if (cc_inString(cc,"0123456789-.+")) {
			jsn_ungetCc(self,1);
			return ptrNumber(self);
			}

		if (nameCc(cc,0)) {
			char name[128]={cc};
			int j=0;
			for(j=1;j<sizeof(name)-1;++j) {
				cc=jsn_getCc(self); if (cc==EOF) break;
				if (!nameCc(cc,1)) { jsn_ungetCc(self,1); break; }
				name[j]=cc;
				}
			name[j]=0;
			if (cs_exact(name,"true"))  return Bool_Value(1);
			if (cs_exact(name,"false")) return Bool_Value(0);
			if (cs_exact(name,"null"))  return JsonNull_Value();
			if (top) OAbort("unquoted string as only/top object? (%s)",name);
			return Char_Value(name);
			}

		if (cc=='/') {
			cc=jsn_getCc(self);
			bool toeol=(cc=='/');
			if (!toeol && cc!='*') abortP("Comment start? Slash with out Slash or asterisk.");
			int stars=0;
			while(1) {
				cc=jsn_getCc(self); if (cc==EOF) break;
				if (toeol && cc=='\n') break;
				if (!toeol) {
					if (stars && cc=='/') break;
					if (cc=='*') ++stars;
					  else         stars=0;
					}
				}
			return jsnLoadItem(self,ender,top);
			}

		abortP("Bad char: 0x%02x?",cc);
		}

	static __thread int4 Json_lastLoadTypeThread=0;
int4 Json_lastLoadType() { return(Json_lastLoadTypeThread); }

Obj Json_FromBlock(int count,pointer block,int flags) {
	if (!block) OAbort("Null data!");

	bool redirect=(count>8 && 0==cs_blockCmp(block,"bplist0",7));

	if (!redirect) {
		for(int j=0;j<count;++j) {
			int cc= *(Char)(block+j);
			if (!cc) OAbort("null char found at pos %d?",j);
			}
		}

	if (!redirect) {
		for(int j=0;j<count;++j) {
			int cc= *(Char)(block+j);
			if (cc_isWhite(cc)) ;
			ei (cc=='<') { redirect=1; break; }
			else break;
			}
		}
	if (redirect) {
		Obj obj=PList_FromBlock(count,block,flags);
		if (obj) {
			int flags=PList_lastLoadType();
			if (0==(flags & (PLIST_Apple|PLIST_Binary))) flags|=PLIST_Coda_C;
			Json_lastLoadTypeThread=PList_lastLoadType();
			}
		return(obj);
		}

	Self self=alocS(sizeat(Self));
		_ blob=block;
		_ blobNel=count;
		_ flags=flags;
	Obj obj=jsnLoadItem(self,0,1);
	if (obj) Json_lastLoadTypeThread=PLIST_Json;
	return(obj);
	}

static int json_encodedLength(char *a) {
	int len=0;
	for(int j=0;a[j];++j) {
		int cc=a[j]&0xFF;
		if (cc=='/' || cc=='\\' || cc=='"') len+=2;
		ei (cc<32) len+=6;
		else ++len;
		}
	return(len);
	}

char* Json_stringEncode(int bufsize,char *buffer,char *a,Char *extra,int4 flags) {
	bool noes=(flags & JSON_NoEscapeSlash);
	bool html=(flags & JSON_HTML);
	bool nelf=(flags & JSON5_NoEscLF);
	bufsize-=4;
	if (extra) {
		int size=json_encodedLength(a)+1;
		if (size>bufsize) { bufsize=size; buffer=alocO(size); *extra=buffer; }
		}
	int k=0,lcc=0;
	for(int j=0;a[j];++j) {
		int cc=a[j]&0xFF;
		if (k>=bufsize) OAbort("%s; overflow %d.",__func__,k);
		bool es= ( (!noes && !html) || (html && lcc=='<'));
		if ( (es && cc=='/') || cc=='\\' || cc=='"') buffer[k++]='\\';
		ei (nelf && cc=='\n') ;
		ei (cc<32) {
			buffer[k++]='\\';
			if (cc=='\t') cc='t';
			ei (cc=='\n') cc='n';
			ei (cc=='\r') cc='r';
			ei (cc=='\f') cc='f';
			ei (cc=='\b') cc='b';
			else {
				char temp[16]; snprintf(temp,sizeof(temp),"u%04X",cc);
				if (k+6>=bufsize) OAbort("%s; Overflow %d.",__func__,k);
				cs_copy(buffer+k,temp);
				k+=cs_length(temp);
				lcc=0;
				continue;
				}
			}
		buffer[k++]=cc; lcc=cc;
		}
	buffer[k]=0;
	return(buffer);
	}

	#define BLEN (4096+4096)
	typedef struct PLPTR6_ {
		int  iLevel,flags,indent;
		char b[BLEN+16];
		oPrintf oprintf;
		Obj stream;
		} *PLPTR6;
	#undef  Self
	#define Self PLPTR6

	enum { MAXjoLevel=100, };

	#define ePrintf(...) self->oprintf(self->stream,__VA_ARGS__)

	#define hasFlag(bitflag) (self->flags & bitflag)
	#define isPretty()       (self->flags & JSON_Pretty)

	#define pretty(str) prettySelf(self,str)

	static void prettySelf(Self self,char *str) {
		if (isPretty()) ePrintf("%s",str);
		}

	static pointer jsn_saveItem(Self self,Obj obj,int isArray) ;

Obj Json_toStream(Obj stream,Obj container,int flags) {
	if (!stream)    OAbort("Void stream?");
	if (!container) OAbort("Void container?");

	if (flags & (PLIST_Coda_C | PLIST_Apple | PLIST_Binary))
		return PList_toStream(stream,container,flags);

	oPrintf oprintf=(pointer)fprintf;
	if (flags & PLIST_ObjectStream) {
		oprintf=OResponds(stream,oPrintf);
		if (!oprintf) OAbort("stream does not respond to: %s",sig_oPrintf);
		}
	Self self=alocS(sizeat(Self));
		_ flags=flags;
		_ oprintf=oprintf;
		_ stream=stream;

	if (!jsn_saveItem(self,container,2)) return(0);
	pretty("\n");
	return(container);
	}

	static pointer jprints(Self self,char *string) {
		if (uc_faults(string)) OAbort("Non-UTF8 string(%s)",string);
		ePrintf("\"");
		while(1) {
			static char maxutf[]="\xF4\x8F\xBF\xBF";
			int pos=cs_pos(maxutf,string); if (pos==EOF) break;
			if (pos>0) { ePrintf("%.*s",pos,string); string+=pos; }
			ePrintf("\\u0000");
			string+=4;
			}
		ePrintf("%s\"",string);
		return(self);
		}

		enum { json_max_dent=60, };

	static void jsn_indentEqn(Self self) {
		if (!isPretty()) return;
		char a[json_max_dent*2+8]; int nspace= _ indent*2;
		int j; for(j=0;j<nspace && j<json_max_dent;++j) a[j]=32;
		a[j]=0;
		if (j>0) ePrintf("%s",a);
		}

		bool PList_leafClass(Obj obj,bool apple) ;
	static pointer jsn_saveLeaf(Self self,Obj obj,int isArray) {
		if (!PList_leafClass(obj, 1)) {
			if (isa_JsonNull(obj)) { ePrintf("null"); return(self); }
			if (isa_(obj,HugeUID)) {
				HugeUID hhh=obj;
				if (hasFlag(PLIST_Strict)) OAbort("Strict: <uid> is not supported in JSON.");
				ePrintf("{\"CF$UID\":%lld}",*hhh);
				return(self);
				}
			OAbort("class[%s] is not a supported JSON leaf type.",kindO(obj));
			}
		if (!OResponds(obj,ToString)) OAbort("class[%s] does not support 'ToString'.",kindO(obj));
		cleanO Char aa=obj_(ToString,obj);
		if (!aa) OAbort("class[%s] did not produce a string with 'ToString'.",kindO(obj));

		if (isa_(obj,Bool)) ePrintf("%s",aa);
		ei (isa_(obj,Real)) {
			char aaa[64]; snprintf(aaa,sizeof(aaa),"%.16g",*(Real)obj);
			int cc=aaa[0];
			if (!cc_inString(cc,"0123456789-.+")) OAbort("Bad Real Value(%s)?",aaa);
			ePrintf("%s",aaa);
			}
		ei (isa_(obj,Huge)) ePrintf("%s",aa);
		ei (isa_(obj,Data)) {
			if (hasFlag(PLIST_Strict)) OAbort("Strict: <data> is not supported in JSON.");
			ePrintf("{\"CF$Data\":\"");
			if (hasFlag(JSON_Pretty) && hasFlag(JSON5_NoEscLF))
				Json_data2os(_ oprintf,_ stream,aa,_ flags);
			  else ePrintf("%s",aa);
			ePrintf("\"}");
			}
		ei (isa_(obj,Char)) {
			Char extra=0;
			if (!Json_stringEncode(BLEN,_ b,aa,&extra,_ flags)) return(0);
			cleanO Char temp=extra;
			char *buffer=(temp?temp:_ b);

			if (isa_(obj,DateString)) {
					if (hasFlag(PLIST_Strict)) OAbort("Strict: <date> is not supported in JSON.");
				   if (!DateString_toGmtime(obj)) return(0);
		 		   ePrintf("{\"CF$Date\":\"%s\"}",buffer);
				   }
			  else {
				if (!jprints(self,buffer)) return(0);
				}
			}
		else OAbort("class[%s] is not a supported leaf type?",kindO(obj));
		return(self);
		}

	static pointer json_3Array(Self self,Array container) {
		int j,nel=Array_get_count(container);
		ePrintf("[");
		++ _ indent;
		for(j=0;j<nel;++j) {
			if (j) ePrintf(",");
			Obj obj=Array_subInt(container,j);
			pretty("\n"); jsn_indentEqn(self);
			if (!obj) { ePrintf("null"); continue; }
			if (!jsn_saveItem(self,obj,1)) return(0);
			}
		pretty("\n"); jsn_indentEqn(self);
		ePrintf("]");
		-- _ indent;
		return(self);
		}

	static pointer json_3Dict(Self self,Dictionary container) {
		ePrintf("{"); pretty("\n");
		++_ indent;
		int j,nel=Dictionary_get_count(container);
		cleanO Pointer vector=Dictionary_AllKeys(container);
		if (!hasFlag(PLIST_UnsortedDict)) pointer_sort(vector,nel,strcmp,0);
		for(j=0;j<nel;++j) {
			if (j) { ePrintf(","); pretty("\n"); }
			char *cp=vector[j];
			Obj obj=Dictionary_subKey(container,cp);
			if (!obj) OAbort("can't find obj for(%s) internal error? NEVER",cp);
			if (!Json_stringEncode(BLEN,_ b,cp,0,_ flags)) return(0);
			jsn_indentEqn(self);
			if (!jprints(self,_ b)) return(0);
			if (isPretty()) ePrintf(" : "); else ePrintf(":");
			if (!jsn_saveItem(self,obj,0)) return(0);
			}
		if (nel) pretty("\n");
		jsn_indentEqn(self);
		ePrintf("}");
		--_ indent;
		return(self);
		}

	#undef BLEN

	static pointer jsn_saveItem(Self self,Obj obj,int isArray) {
		if (++_ iLevel>MAXjoLevel) OAbort("too many levels(%d). Circular?",_ iLevel);
		if (isa_(obj,Dictionary)) {
			if (!json_3Dict(self,obj)) return(0);
			}
		ei (isa_(obj,Array)) {
			if (Array_isaSet(obj) && hasFlag(PLIST_Strict)) {
				OAbort("Strict: <Set> not allowed in JSON as <array>.");
				}
			if (!json_3Array(self,obj)) return(0);
			}
		else {
			if (!jsn_saveLeaf(self,obj,isArray)) return(0);
			}
		--_ iLevel;
		return(self);
		}

Obj Json_save(char *file,Obj container,int flags) {
	cleanO FileMem fff=FileMem_Open(0); if (!fff) return(0);
	if (!Json_toStream(fff,container,flags|PLIST_ObjectStream)) return(0);
	cleanO Data data=FileMem_ToData(fff); if (!data) OAbort("NEVER empty?");
	FILE *os=fopen(file,"wb"); if (!os) OAbort("can't write file.");
		huge size=sizeO(data);
		huge nout=file_write(os,data,size);
		if (nout!=size) OAbort("WRITE error %d vs %d?",(int)nout,(int)size);
		if (fclose(os)) OAbort("fclose Error?");
	return(container);
	}

Obj Json_Load(char *file,int flags) {
	cleanO Data data=Data_FromFile(file); if (!data) return(0);
	return Json_FromBlock(sizeO(data),data,flags);
	}

void Array_sort(Array array,void *IcmpfunVVC,void *context) {
	int nel=Array_count(array);
	if (nel>1) pointer_sort(Array_rawAddress(array),nel,IcmpfunVVC,context);
	}

int cc_count(char c,char *str) {
	int j=0; for(;*str;++str) if (*str==c) ++j;
	return(j);
	}

Array Char_Split(char* string,char* delimiter) {
	Array array=newO(Array);
	while(1) {
		int p=cs_pos(delimiter,string); if (p==EOF) break;
		Char hunk=Char_F("%.*s",p,string);
		Array_takeObject(array,hunk);
		string+=(p+cs_length(delimiter));
		}
	Array_takeObject(array,Char_Value(string));
	return(array);
	}

char* cs_cap(char *str) { char* org=str;
	while(*str) { *str =cc_toCap(*str); ++str; }
	return(org);
	}

char* cs_low(char *str) { char* org=str;
	while(*str) { *str =cc_toLow(*str); ++str; }
	return(org);
	}

void cs_ccChange(char *str,char f,char t) {
	for(;*str;++str) if (*str==f) *str=t;
	}

void cs_reverse(char *a) {
	int j=cs_length(a)-1;
	for(int i=0;i<j;++i,--j) {
		int c=a[i]; a[i]=a[j]; a[j]=c;
		}
	}

int cs_rmc(char *a,char c) {
	char *d; int count=0;
	for(d=a;*a;++a) if (*a!=c) *d++ = *a; else ++count;
	*d=0;
	return(count);
	}

int cs_sortCap(const char *s1,const char *s2) {
	for(const unsigned char *a=(const unsigned char*)s1,*b=(const unsigned char*)s2;1;++a,++b) {
		int ca=cc_toCap(*a);
		int cb=cc_toCap(*b);
		if (ca != cb) return(ca - cb);
		if (!ca) break;
		}
	return(0);
	}

bool cs_suffix(const char *key,const char *str) {
	int klen=cs_length(key);
	int slen=cs_length(str);
	if (slen<klen || klen<1) return(0);
	return cs_exact(key,str+slen-klen);
	}

bool cs_suffixCap(const char *key,const char *str) {
	int klen=cs_length(key);
	int slen=cs_length(str);
	if (slen<klen || klen<1) return(0);
	return cs_exactCap(key,str+slen-klen);
	}

void cs_rightTrim(char *str) {
	for(int i=cs_length(str)-1; i>=0 && cc_isWhite(str[i]); --i) str[i]=0;
	}

void cs_leftTrim(char *str) {
	int i; for(i=0;cc_isWhite(str[i]);++i) ;
	if (i) cs_copy(str,str+i);
	}

void cs_trim(char *str) {
	cs_leftTrim(str);
	cs_rightTrim(str);
	}

void defer_call_cleanup(void *vp) {
	if (vp) {
		Pointer self=(Pointer)vp;
		void (*fun)(pointer)=self[0];
		if (fun) fun(self[1]);
		}
	}

void defer_NOP(void *vp) { }

	#include <dirent.h>

Array file_Folder(char *path) {
	DIR *dir1=opendir(path); if (!dir1) return(0);
	Array array=newO(Array);
	Array_set_name(array,path);
	struct dirent *dirfile;
	while(0!=(dirfile=readdir(dir1))) {
		char *file=dirfile->d_name;
		if (file[0]=='.' && (!file[1] || (file[1]=='.' && !file[2]))) continue;
		Array_takeObject(array,Char_Value(file));
		}
	closedir(dir1);
	return array;
	}

int file_getCs(FILE *f,char *s,int mx) {
	int c=0,i;
	for(i=0;--mx>0 && (c=fgetc(f))!=EOF && c!='\n';++i) s[i]=c;
	s[i]=0;
	return( (c==EOF && i==0) ? EOF : i );
	}

Char ToInfo(Obj obj) {
	return obj_(Info,obj);
	}

void CodaInfoValue(char *var,Char $CONSUMED value) {
	Msg_("%s => %s",var,value);
	freeO(value);
	}

void CodaEqualsValue(char *var,Char $CONSUMED value) {
	Msg_("%s = %s",var,value);
	freeO(value);
	}

