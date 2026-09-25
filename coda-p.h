#pragma once
/* www.coda-c.com  coda-p.h

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

	#include <coda-c.h>

CodaClassDef(Bool,_Bool,Root);
CodaClassDef(Data,void,Root);
CodaClassDef(DateString,char,Char);
CodaClassDef(Float,float,Root);
CodaClassDef(Huge,huge,Root);
CodaClassDef(HugeUID,huge,Huge);
CodaClassDef(Int4,int4,Root);
CodaClassDef(Real,double,Root);
CodaClassDef(Short,short,Root);
	#define Bool_count Root_get_count
	#define cc_inString(c,str) ((-1)!=cc_pos(c,str))
	#define cs_inString(key,str) ((-1)!=cs_pos(key,str))
	#define Data_count Root_get_count
 #define Float_count Root_get_count
	#define Huge_count Root_get_count
	#define Int4_count Root_get_count
	#define Error_F(...) OErrorSet(Char_F(__VA_ARGS__))
	#define OAbort(...) return(Error_F(__VA_ARGS__))
	#define Real_count Root_get_count
	#define Short_count Root_get_count
extern char Data_base64s[];
int Data_base64decode(char *dp,int dlen,char *sp,int slen);
int Data_base64encode(char *dp,int dlen,char *sp,int slen,int addpad);
Bool Bool_Value(bool value);
bool cc_isCap(int c);
bool cc_isLow(int c);
bool cc_isAlpha(int c);
bool cc_is09(int c);
bool cc_isWhite(int c);
int cc_toCap(int c);
int cc_toLow(int c);
int cc_pos(char c,const char *str);
int cc_posCap(char c,const char *str);
int cc_revPos(char c,const char *str);
bool cs_prefix(const char *key,const char *str);
bool cs_prefixCap(const char *key,const char *str);
void cs_copy(char *dest,const char *sour);
bool cs_exactCap(const char *str1,const char *str2);
int cs_pos(const char *key,const char *str);
int cs_posCap(const char *key,const char *str);
int4 cs_toInt4(const char *string);
double cs_toReal(const char *string);
Data Data_NewBlock(Data self,int count,pointer address);
DateString DateString_Value(char *string);
DateString DateString_FromGmtime(struct_tm *when);
struct_tm* DateString_toGmtime(DateString self);
pointer Gmtime_check(struct_tm *self);
Float Float_Value(float value);
Float Float_NewBlock(Float self,int count,pointer address);
Huge Huge_Value(huge value);
Huge Huge_NewBlock(Huge self,int count,pointer address);
Char Huge2_ToString(Huge self);
Huge Huge2_FromString(char *string);
HugeUID HugeUID_Value(huge value);
Int4 Int4_Value(int4 value);
Int4 Int4_NewBlock(Int4 self,int count,pointer address);
int Int4_length(Int4 self);
Void JsonNull_Value(void);
bool isa_JsonNull(Obj obj);
Obj OErrorSet(Obj $CONSUMED obj);
Obj OErrorObject(void);
Char OError(void);
Real Real_Value(double value);
Real Real_NewBlock(Real self,int count,pointer address);
Short Short_Value(short value);
Short Short_NewBlock(Short self,int count,pointer address);
int Short_length(Short self);
	typedef pointer (*UcEncoder)(pointer ptr,int4 value,pointer context);
int u2_codes(pointer u2str);
int u2_count(pointer u2str,int codes);
int u2_scan(pointer u2str,int countOrNeg1,int codes,int *counter,int *faults);
pointer u2_encode(pointer ptr,int4 value,pointer context);
pointer u4_encode(pointer ptr,int4 value,pointer context);
int uc_codes(char* ustr);
int uc_count(char *ustr,int codes);
int uc_scan(char *ustr,int lenOrNeg1,int codes,int *counter,int *faults);
pointer uc_encode(pointer ptr,int4 value,pointer context);
int uc_faults(char *ustr);
Obj uN_Convert(int isizeN,UcEncoder encoder,pointer context,pointer uNstr,int countOrNeg1,int *faults);
Short uc_ToU2(char *u1str,int countOrNeg1,int *faults);
Int4 uc_ToU4(char *u1str,int countOrNeg1,int *faults);
Char u2_ToUc(pointer u2str,int countOrNeg1,int *faults);
Int4 u2_ToU4(pointer u2fstr,int countOrNeg1,int *faults);
Char u4_ToUc(pointer u4str,int countOrNeg1,int *faults);
Short u4_ToU2(pointer u4str,int countOrNeg1,int *faults);
	extern ConstChar sig_oPrintf;
	extern ConstChar sig_oWrite;
	extern ConstChar sig_oPutc;
	extern ConstChar sig_oGetc;
	extern ConstChar sig_oRead;
	extern ConstChar sig_oTell;
CodaClassDef(FileMem,struct FileMem_,Root);
	typedef int  $FORMAT23 OSig(oPrintf)(Obj obj,char *format,...) ;
	typedef int  OSig(oWrite)(Obj obj,pointer buffer,int length) ;
	typedef int  OSig(oPutc)(Obj obj,int cc) ;
	typedef int  OSig(oGetc)(Obj obj) ;
	typedef int  OSig(oRead)(Obj obj,pointer buffer,int length) ;
	typedef huge OSig(oTell)(Obj obj) ;
	#define file_lsType(path) file_typeCode(path,1)
	#define file_type(path)   file_typeCode(path,0)
	#define file_putc(os,cc)  fputc(cc,os)
	#define file_read(is,buf,nel)  fread( buf,1,nel,is)
	#define file_write(os,buf,nel) fwrite(buf,1,nel,os)
 enum {
	PLIST_UnsortedDict=    1,
	PLIST_NL4Leafs=        2,
	PLIST_AddComputer=     4,
	PLIST_Amp38=           8,
	PLIST_Apple=          16,
	PLIST_NoEncoding=     32,
	PLIST_NoDoctype   =   64,
	PLIST_NoPVersion  =  128,
	JSON_HTML         =1<< 8,
	JSON_NoEscapeSlash=1<< 9,
	PLIST_Coda_C      =1<<10,
	PLIST_Binary      =1<<11,
	PLIST_ObjectStream=1<<12,
	PLIST_Json        =1<<13,
	JSON_Pretty       =1<<14,
	PLIST_Strict      =1<<15,
	JSON5_NoEscLF     =1<<28,
	BINARY_MaxComp    =1<<30,
	BINARY_NoComp     =1<<31,
	PLIST_ITUNES= ( PLIST_UnsortedDict | PLIST_AddComputer | PLIST_Amp38 ),
	};
Data Array_ToData(Array array);
Data Data_FromFile(char *path);
huge file_size(char *path);
huge file_time(char *path);
int file_typeCode(char *path,bool reportlinks);
bool File_protocol(Obj obj);
FileMem FileMem_Open(Data optional);
Array FileMem_array(FileMem self);
void FileMem_flush(FileMem self);
int FileMem_oPutc(FileMem self,int cc);
int $FORMAT23 FileMem_oPrintf(FileMem self,char *format,...);
Data FileMem_ToData(FileMem self);
int FileMem_oGetc(FileMem self);
int FileMem_oRead(FileMem self,char *buffer,int length);
void CList_addLeaf(char *xmltag,Obj cobj);
int4 PList_lastLoadType(void);
Obj PList_FromBlock(int count,pointer block,int flags);
Obj PList_toStream(Obj stream,Obj container,int flags);
Obj PList_save(char *file,Obj container,int flags);
Obj PList_Load(char *file,int flags);
CDictionary Plugin_dictionary(void);
void Plugin_register(char *proto,pointer address);
pointer Plugin_fromProto(char *proto);
void PList_Binary(void);
void pointer_sort(pointer base,int nel,void *IfunVVC,void *context);
Obj PList_BinaryLoad(int count,pointer block);
Obj PList_BinaryWrite(Obj stream,Obj container,int flags);
void PList_Binary(void);
	enum { UTF8max=0x10FFFF, };
int4 Json_lastLoadType(void);
Obj Json_FromBlock(int count,pointer block,int flags);
Obj Json_toStream(Obj stream,Obj container,int flags);
Obj Json_save(char *file,Obj container,int flags);
Obj Json_Load(char *file,int flags);
	#define cs_sort strcmp
	#define defer_call(fun,ptr) $CLEANUP(defer_call_cleanup) \
		pointer OBind1_2M(defer_,__LINE__)[2]={fun,ptr}; \
		defer_NOP(OBind1_2M(defer_,__LINE__))
	#define codaInfo(...) CODA_Zx(CodaInfoValue,ToInfo,__VA_ARGS__)
	#define codaValue(...) CODA_Zx(CodaEqualsValue,ToContainer,__VA_ARGS__)
void Array_sort(Array array,void *IcmpfunVVC,void *context);
int cc_count(char c,char *str);
Array Char_Split(char* string,char* delimiter);
char* cs_cap(char *str);
char* cs_low(char *str);
void cs_ccChange(char *str,char f,char t);
void cs_reverse(char *a);
int cs_rmc(char *a,char c);
int cs_sortCap(const char *s1,const char *s2);
bool cs_suffix(const char *key,const char *str);
bool cs_suffixCap(const char *key,const char *str);
void cs_rightTrim(char *str);
void cs_leftTrim(char *str);
void cs_trim(char *str);
void defer_call_cleanup(void *vp);
void defer_NOP(void *vp);
Array file_Folder(char *path);
int file_getCs(FILE *f,char *s,int mx);
Char ToInfo(Obj obj);
void CodaInfoValue(char *var,Char $CONSUMED value);
void CodaEqualsValue(char *var,Char $CONSUMED value);

	#include <stdarg.h>
	#include <stddef.h>

