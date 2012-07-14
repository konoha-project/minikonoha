/****************************************************************************
 * Copyright (c) 2012, the Konoha project authors. All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ***************************************************************************/

#ifndef KONOHA2_H_
#define KONOHA2_H_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifndef K_OSDLLEXT
#if defined(__APPLE__)
#define K_OSDLLEXT        ".dylib"
#elif defined(__linux__)
#define K_OSDLLEXT        ".so"
#endif
#endif

#define K_TYTABLE_INIT 64
#define K_PAGESIZE        4096

#define K_VERSION   "0.1"
#define K_MAJOR_VERSION 0
#define K_MINOR_VERSION 1
#define K_PATCH_LEVEL   0

#ifndef K_REVISION
#define K_REVISION 0
#endif

#ifndef K_PROGNAME
#define K_PROGNAME  "MiniKonoha"
/* - 2012/06/14 */
//#define K_CODENAME "Miyajima"
/*2012/06/14 -  */
#define K_CODENAME "The Summer Palace, Beijing"
#else
#define K_CODENAME "based on MiniKonoha-" K_VERSION
#endif

#define K_USING_UTF8 1
#define USE_BUILTINTEST  1

#ifndef PLATAPIFORM_KERNEL
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>
#else
#include "konoha_lkm.h"
#endif /* PLATAPIFORM_KERNEL */

#ifndef jmpbuf_i
#include <setjmp.h>
#define jmpbuf_i jmp_buf
#define ksetjmp  setjmp
#define klongjmp longjmp
#endif /*jmpbuf_i*/

#include <stddef.h>
#include <stdarg.h>

#ifdef __GCC__
#define __NoneTagFMT(idx1, idx2) __attribute__((format(printf, idx1, idx2)))
#else
#define __NoneTagFMT(idx1, idx2)
#endif

/* ------------------------------------------------------------------------ */
/* platform */

typedef const struct PlatformApiVar  PlatformApi;
typedef struct PlatformApiVar        PlatformApiVar;
typedef const struct KonohaLibVar    KonohaLib;
typedef struct KonohaLibVar          KonohaLibVar;

#define PLATAPI (kctx->plat)->
#define KLIB    (kctx->klib)->

typedef void FILE_i;

typedef enum {
	CritTag, ErrTag, WarnTag, NoticeTag, InfoTag, DebugTag, NoneTag
} kinfotag_t;

struct PlatformApiVar {
	// settings
	const char *name;
	size_t  stacksize;
	// low-level functions
	void*   (*malloc_i)(size_t);
	void    (*free_i)(void *);
	int     (*setjmp_i)(jmpbuf_i);
	void    (*longjmp_i)(jmpbuf_i, int);

	char*   (*realpath_i)(const char*, char*);
	FILE_i* (*fopen_i)(const char*, const char*);
	int     (*fgetc_i)(FILE_i *);
	int     (*feof_i)(FILE_i *);
	int     (*fclose_i)(FILE_i *);
	//
	void    (*syslog_i)(int priority, const char *message, ...) __NoneTagFMT(2, 3);
	void    (*vsyslog_i)(int priority, const char *message, va_list args);
	int     (*printf_i)(const char *fmt, ...) __NoneTagFMT(2, 3);
	int     (*vprintf_i)(const char *fmt, va_list args);
	int     (*snprintf_i)(char *str, size_t size, const char *fmt, ...);
	int     (*vsnprintf_i)(char *str, size_t size, const char *fmt, va_list args);
    void    (*qsort_i)(void *base, size_t nel, size_t width, int (*compar)(const void *, const void *));
    // abort
	void    (*exit_i)(int p);

	// high-level functions
	const char* (*packagepath)(char *buf, size_t bufsiz, const char *pkgname);
	const char* (*exportpath)(char *buf, size_t bufsiz, const char *pkgname);
	const char* (*begin)(kinfotag_t);
	const char* (*end)(kinfotag_t);
	void  (*dbg_p)(const char *file, const char *func, int line, const char *fmt, ...) __NoneTagFMT(4, 5);
};

/* ------------------------------------------------------------------------ */
/* type */

#ifndef __KERNEL__
#include <limits.h>
#include <float.h>
#include <stdbool.h>
#include <stdint.h>
#endif

#if defined(__LP64__) || defined(_WIN64)
#define K_USING_SYS64_    1
typedef int32_t           kshort_t;
typedef uint32_t          kushort_t;
#ifdef K_USING_NOFLOAT
typedef uintptr_t         kfloat_t;
#else
typedef double            kfloat_t;
#endif
#else
typedef int16_t           kshort_t;
typedef uint16_t          kushort_t;
#ifdef K_USING_NOFLOAT
typedef uintptr_t         kfloat_t;
#else
typedef float             kfloat_t;
#endif
#endif

typedef bool             kbool_t;

typedef enum {
	K_FAILED, K_CONTINUE, K_BREAK
} kstatus_t;

typedef intptr_t         kint_t;
typedef uintptr_t        kuint_t;

#ifdef K_USING_SYS64_

#ifndef LLONG_MIN
#define LLONG_MIN -9223372036854775807LL
#define LLONG_MAX  9223372036854775807LL
#endif

#ifndef ULLONG_MAX
#define ULLONG_MAX 18446744073709551615ULL
#endif

#define KINT_MAX               LLONG_MAX
#define KINT_MIN               LLONG_MIN
#define KINT_FMT               "%lld"

#else/*K_USING_SYS64_*/

#define KINT_MAX               LONG_MAX
#define KINT_MIN               LONG_MIN
#define KINT_FMT               "%ld"
#define KINT0                  0UL

#endif/*K_USING_SYS64_*/

typedef kushort_t        kshortflag_t;    /* flag field */

#define KFLAG_H(N)               ((sizeof(kshortflag_t)*8)-N)
#define KFLAG_H0                 ((kshortflag_t)(1 << KFLAG_H(1)))
#define KFLAG_H1                 ((kshortflag_t)(1 << KFLAG_H(2)))
#define KFLAG_H2                 ((kshortflag_t)(1 << KFLAG_H(3)))
#define KFLAG_H3                 ((kshortflag_t)(1 << KFLAG_H(4)))
#define KFLAG_H4                 ((kshortflag_t)(1 << KFLAG_H(5)))
#define KFLAG_H5                 ((kshortflag_t)(1 << KFLAG_H(6)))
#define KFLAG_H6                 ((kshortflag_t)(1 << KFLAG_H(7)))
#define KFLAG_H7                 ((kshortflag_t)(1 << KFLAG_H(8)))

#define TFLAG_is(T,f,op)          (((T)(f) & (T)(op)) == (T)(op))
#define TFLAG_set1(T,f,op)        f = (((T)(f)) | ((T)(op)))
#define TFLAG_set0(T,f,op)        f = (((T)(f)) & (~((T)(op))))
#define TFLAG_set(T,f,op,b)       if(b) {TFLAG_set1(T,f,op);} else {TFLAG_set0(T,f,op);}

#define FLAG_set(f,op)            TFLAG_set1(kshortflag_t,f,op)
#define FLAG_unset(f,op)          TFLAG_set0(kshortflag_t,f,op)
#define FLAG_is(f,op)             TFLAG_is(kshortflag_t,f,op)

/* fullsize id */
typedef uintptr_t                     kfileline_t;
#define NOPLINE                       0

// halfsize id
typedef kushort_t       kpackage_t;     /* package id*/
typedef kushort_t       ktype_t;        /* cid classid, ty type */
typedef kushort_t       ksymbol_t;
typedef kushort_t       kmethodn_t;
typedef kushort_t       kparamid_t;

/* ktype_t */
#define TY_newid        ((ktype_t)-1)
#define TY_unknown         ((ktype_t)-2)

#define CT_(t)              (kctx->share->classTable.classItems[t])
#define CT_cparam(CT)       (kctx->share->paramdomList->paramItems[(CT)->paramdom])
#define TY_isUnbox(t)       FLAG_is(CT_(t)->cflag, kClass_UnboxType)
#define CT_isUnbox(C)       FLAG_is(C->cflag, kClass_UnboxType)

#define SYM_MAX            KFLAG_H3
#define SYM_HEAD(sym)      (sym  & (KFLAG_H0|KFLAG_H1|KFLAG_H2))
#define SYM_UNMASK(sym)    (sym & (~(KFLAG_H0|KFLAG_H1|KFLAG_H2|KFLAG_H3)))

#define SYM_NONAME          ((ksymbol_t)-1)
#define SYM_NEWID           ((ksymbol_t)-2)
#define SYM_NEWRAW          ((ksymbol_t)-3)
#define _NEWID              SYM_NEWID
#define _NEWRAW             SYM_NEWRAW

#define SYMKEY_BOXED            KFLAG_H3
#define SYMKEY_unbox(sym)       (sym & ~(SYMKEY_BOXED))
#define SYMKEY_isBOXED(sym)     ((sym & SYMKEY_BOXED) == SYMKEY_BOXED)

#define FN_COERCION             KFLAG_H3
#define FN_Coersion             FN_COERCION
#define FN_isCOERCION(fn)       ((fn & FN_COERCION) == FN_COERCION)

#define MN_ISBOOL     KFLAG_H0
#define MN_GETTER     KFLAG_H1
#define MN_SETTER     KFLAG_H2

#define MN_Annotation (KFLAG_H1|KFLAG_H2)
#define MN_isAnnotation(S)   ((S & KW_PATTERN) == MN_Annotation)

#define MN_TOCID             (KFLAG_H0|KFLAG_H1)
#define MN_ASCID             (KFLAG_H0|KFLAG_H1|KFLAG_H2)
#define KW_PATTERN           (KFLAG_H0|KFLAG_H1|KFLAG_H2)
#define KW_isPATTERN(S)      ((S & KW_PATTERN) == KW_PATTERN)

#define MN_isISBOOL(mn)      (SYM_HEAD(mn) == MN_ISBOOL)
#define MN_toISBOOL(mn)      ((SYM_UNMASK(mn)) | MN_ISBOOL)
#define MN_isGETTER(mn)      (SYM_HEAD(mn) == MN_GETTER)
#define MN_toGETTER(mn)      ((SYM_UNMASK(mn)) | MN_GETTER)
#define MN_isSETTER(mn)      (SYM_HEAD(mn) == MN_SETTER)
#define MN_toSETTER(mn)      ((SYM_UNMASK(mn)) | MN_SETTER)

#define MN_to(cid)           ((CT_(cid)->nameid) | MN_TOCID)
#define MN_isTOCID(mn)       ((SYM_UNMASK(mn)) == MN_TOCID)
#define MN_as(cid)           ((CT_(cid)->nameid) | MN_ASCID)
#define MN_isASCID(mn)       ((SYM_UNMASK(mn)) == MN_ASCID)

/* ------------------------------------------------------------------------ */

#define kAbstractObject                 const void
typedef const struct kObjectVar         kObject;
typedef struct kObjectVar               kObjectVar;
typedef const struct kBooleanVar        kBoolean;
typedef struct kBooleanVar              kBooleanVar;
typedef const struct kIntVar            kInt;
typedef struct kIntVar                  kIntVar;
typedef const struct kStringVar         kString;
typedef struct kStringVar               kStringVar;
typedef const struct kArrayVar          kArray;
typedef struct kArrayVar                kArrayVar;
typedef const struct kParamVar          kParam;
typedef struct kParamVar                kParamVar;
typedef const struct kMethodVar         kMethod;
typedef struct kMethodVar               kMethodVar;
typedef const struct kFuncVar           kFunc;
typedef struct kFuncVar                 kFuncVar;

/* sugar.h */
typedef const struct kNameSpaceVar      kNameSpace;
typedef struct kNameSpaceVar            kNameSpaceVar;
typedef const struct kTokenVar          kToken;
typedef struct kTokenVar                kTokenVar;
typedef const struct kExprVar           kExpr;
typedef struct kExprVar                 kExprVar;
typedef const struct kStmtVar           kStmt;
typedef struct kStmtVar                 kStmtVar;
typedef const struct kBlockVar          kBlock;
typedef struct kBlockVar                kBlockVar;
typedef struct kGammaVar                kGamma;
typedef struct kGammaVar                kGammaVar;

/* ------------------------------------------------------------------------ */

typedef struct KUtilsKeyValue {
	ksymbol_t key;
	ktype_t   ty;
	union {
		uintptr_t                uval;  //unboxValue
		kObject                 *oval;  //objectValue
		kString                 *sval;  //stringValue
	};
} KUtilsKeyValue;

#define COMMON_BYTEARRAY \
		size_t bytesize;\
		union {\
			const char *byteptr;\
			const char *text;\
			const unsigned char *utext;\
			char *buf;\
			unsigned char *ubuf;\
		}\

#define KARRAYSIZE(BS, T)   ((BS)/sizeof(T##_t))

typedef struct KUtilsGrowingArray {
	size_t bytesize;
	union {
		char              *bytebuf;
		const struct KonohaClassVar      **classItems;
		KUtilsKeyValue    *keyvalueItems;
		struct VirtualMachineInstruction   *codeItems;
		kObject        **objectItems;
		kObjectVar     **refhead;  // stack->ref
	};
	size_t bytemax;
} KUtilsGrowingArray;

typedef struct KUtilsWriteBuffer {
	KUtilsGrowingArray *m;
	size_t pos;
} KUtilsWriteBuffer;

typedef struct KUtilsHashMapEntry {
	uintptr_t hcode;
	struct KUtilsHashMapEntry *next;
	union {
		kString         *skey;
		kParam          *paramkey;
		uintptr_t        ukey;
		void            *pkey;
	};
	union {
		kObject         *ovalue;
		void            *pvalue;
		uintptr_t        uvalue;
	};
} KUtilsHashMapEntry;

typedef struct KUtilsHashMap KUtilsHashMap;
struct KUtilsHashMap {
	KUtilsHashMapEntry *arena;
	KUtilsHashMapEntry *unused;
	KUtilsHashMapEntry **hentry;
	size_t arenasize;
	size_t size;
	size_t hmax;
};

/* ------------------------------------------------------------------------ */

typedef const struct KonohaContextVar   KonohaContext;
typedef struct KonohaContextVar         KonohaContextVar;
typedef const struct KonohaClassVar     KonohaClass;
typedef struct KonohaClassVar           KonohaClassVar;
typedef struct KonohaClassField         KonohaClassField;
typedef struct KonohaClassField         KonohaClassFieldVar;

typedef const struct KonohaRuntimeVar   SharedRuntime;
typedef struct KonohaRuntimeVar         KonohaRuntimeVar;
typedef const struct KonohaContextRuntimeVar    LocalRuntime;
typedef struct KonohaContextRuntimeVar          KonohaContextRuntimeVar;
typedef struct KonohaStack              KonohaStack;
typedef struct KonohaStack              KonohaStackVar;

struct KonohaContextVar {
	int						          safepoint; // set to 1
	KonohaStack                      *esp;
	KonohaLib                        *klib;
	PlatformApi                      *plat;
	KonohaRuntimeVar                  *share;
	KonohaContextRuntimeVar                   *stack;
	struct KonohaModule               **modshare;
	struct KonohaContextModule               **modlocal;
	/* TODO(imasahiro)
	 * checking modgc performance and remove
	 * memshare/memlocal from context
	 */
	struct kmemshare_t                *memshare;
	struct kmemlocal_t                *memlocal;
};

// share, local

struct KonohaRuntimeVar {
	KUtilsGrowingArray        classTable;
	KUtilsHashMap            *longClassNameMapNN;
	/* system shared const */
	kObject                  *constNull;
	kBoolean                 *constTrue;
	kBoolean                 *constFalse;
	kString                  *emptyString;
	kArray                   *emptyArray;

	kArray                   *fileidList;    // file, http://
	KUtilsHashMap            *fileidMapNN;   //
	kArray                   *packList;
	KUtilsHashMap            *packMapNN;
	kArray                   *symbolList;  // NAME, Name, INT_MAX Int_MAX
	KUtilsHashMap            *symbolMapNN;
	kArray                   *paramList;
	KUtilsHashMap            *paramMapNN;
	kArray                   *paramdomList;
	KUtilsHashMap            *paramdomMapNN;
};

#define kContext_Debug          ((kshortflag_t)(1<<0))
#define kContext_Interactive    ((kshortflag_t)(1<<1))
#define kContext_CompileOnly    ((kshortflag_t)(1<<2))

#define KonohaContext_isInteractive(X)  (TFLAG_is(kshortflag_t,(X)->stack->flag, kContext_Interactive))
#define KonohaContext_isCompileOnly(X)  (TFLAG_is(kshortflag_t,(X)->stack->flag, kContext_CompileOnly))
#define KonohaContext_setInteractive(X)  TFLAG_set1(kshortflag_t, (X)->stack->flag, kContext_Interactive)
#define KonohaContext_setCompileOnly(X)  TFLAG_set1(kshortflag_t, (X)->stack->flag, kContext_CompileOnly)

struct KonohaContextRuntimeVar {
	KonohaStack*               stack;
	size_t                     stacksize;
	KonohaStack*               stack_uplimit;
	kArray                    *gcstack;
	KUtilsGrowingArray         cwb;
	// local info
	kshortflag_t               flag;
	KonohaContext             *rootctx;
	void*                      cstack_bottom;  // for GC
	KUtilsGrowingArray         ref;   // reftrace
	kObjectVar**               reftail;
	ktype_t                    evalty;
	kushort_t                  evalidx;
	jmpbuf_i                  *evaljmpbuf;
};

// module
#define MOD_MAX    32
#define MOD_logger   0
#define MOD_gc       1
#define MOD_code     2
#define MOD_sugar    3
#define MOD_float     11
#define MOD_iterator  12
#define MOD_iconv   13
//#define MOD_IO      14
//#define MOD_llvm    15
//#define MOD_REGEX   16

struct KonohaContextModule;
typedef struct KonohaContextModule {
	uintptr_t unique;
	void (*reftrace)(KonohaContext*, struct KonohaContextModule *);
	void (*free)(KonohaContext*, struct KonohaContextModule *);
} KonohaContextModule;

struct KonohaModule;
typedef struct KonohaModule {
	const char *name;
	int mod_id;
	void (*setup)(KonohaContext*, struct KonohaModule *, int newctx);
	void (*reftrace)(KonohaContext*, struct KonohaModule *);
	void (*free)(KonohaContext*, struct KonohaModule *);
} KonohaModule;

#define K_FRAME_NCMEMBER \
		uintptr_t   ndata;  \
		kbool_t     bvalue; \
		kint_t      ivalue; \
		kuint_t     uvalue; \
		kfloat_t    fvalue; \
		intptr_t    shift;  \
		uintptr_t   uline; \
		struct VirtualMachineInstruction  *pc; \
		kMethod *mtdNC; \
		const char     *fname \

#define K_FRAME_MEMBER \
		kObject    *o;\
		kObject    *toObject;\
		kObjectVar *toObjectVar; \
		kInt       *i; \
		kString    *s; \
		kString    *toString;\
		kArray     *toArray;\
		kMethod    *toMethod;\
		kFunc      *toFunc; \
		kNameSpace  *toNameSpace;\
		kToken             *tk;\
		kStmt              *stmt;\
		kExpr              *expr;\
		kBlock             *bk;\
		kGamma  *gma;   \
		const struct _kFloat  *f; \
		const struct _kBytes  *ba; \
		struct _kIterator *itr; \
		struct kClass  *c; \
		struct kDate *dt;\
		struct kRegex  *re; \
		struct kRange  *range; \
		struct kIterator *it; \
		struct kMap           *m;    \
		struct kInputStream  *in; \
		struct kOutputStream *w;  \
		struct kException         *e;\
		struct kExceptionHandler  *hdr; \
		struct kConverter         *conv;\
		struct kContext           *cx;\
		struct kScript            *scr;\
		kint_t     dummy_ivalue;\
		kfloat_t   dummy_fvalue \

struct KonohaStack {
	union {
		K_FRAME_MEMBER;
	};
	union {
		K_FRAME_NCMEMBER;
	};
};

typedef struct krbp_t {
	union {
		K_FRAME_NCMEMBER;
		K_FRAME_MEMBER;
	};
} krbp_t;

#define P_STR    0
#define P_DUMP   1

#define KTYSPI \
		void (*init)(KonohaContext*, kObject*, void *conf);\
		void (*reftrace)(KonohaContext*, kObject*);\
		void (*free)(KonohaContext*, kObject*);\
		kObject* (*fnull)(KonohaContext*, KonohaClass*);\
		void (*p)(KonohaContext*, KonohaStack *, int, KUtilsWriteBuffer *, int);\
		uintptr_t (*unbox)(KonohaContext*, kObject*);\
		int  (*compareTo)(kObject*, kObject*);\
		void (*initdef)(KonohaContext*, KonohaClassVar*, kfileline_t);\
		kbool_t (*isSubType)(KonohaContext*, KonohaClass*, KonohaClass*);\
		KonohaClass* (*realtype)(KonohaContext*, KonohaClass*, KonohaClass*)

typedef struct KDEFINE_TY {
	const char *structname;
	ktype_t     cid;         kshortflag_t    cflag;
	ktype_t     bcid;        ktype_t     supcid;
	ktype_t    rtype;        kushort_t  psize;
	struct kparam_t   *cparams;
	size_t     cstruct_size;
	KonohaClassField   *fields;
	kushort_t  fsize;       kushort_t fallocsize;
	KTYSPI;
} KDEFINE_TY;

#define STRUCTNAME(C) \
	.structname = #C,\
	.cid = TY_newid,\
	.cstruct_size = sizeof(k##C)\

//KonohaClassVar;
typedef uintptr_t kmagicflag_t;

struct KonohaClassVar {
	KTYSPI;
	kpackage_t   packageId;  kpackage_t   packageDomain;
	ktype_t   cid;           kshortflag_t  cflag;
	ktype_t   bcid;          ktype_t   supcid;
	ktype_t  p0;            kparamid_t paramdom;
	kmagicflag_t magicflag;
	size_t     cstruct_size;
	KonohaClassField         *fieldItems;
	kushort_t  fsize;         kushort_t fallocsize;
	const char               *DBG_NAME;
	ksymbol_t   nameid;       kushort_t   optvalue;

	kArray     *methodList;
	kString    *shortNameNULL;
	union {   // default value
		kObject           *nulvalNULL;
		kObjectVar        *nulvalNULL_;
	};
	KUtilsHashMap            *constPoolMapNO;
	KonohaClass              *searchSimilarClassNULL;
	KonohaClass              *searchSuperMethodClassNULL;
} ;

struct KonohaClassField {
	kshortflag_t    flag;
	kshort_t        isobj;
	ktype_t         ty;
	ksymbol_t       fn;
};

/* ----------------------------------------------------------------------- */

#define TY_void             ((ktype_t)0)
#define TY_var              ((ktype_t)1)
#define TY_Object            ((ktype_t)2)
#define TY_Boolean           ((ktype_t)3)
#define TY_Int               ((ktype_t)4)
#define TY_String            ((ktype_t)5)
#define TY_Array             ((ktype_t)6)
#define TY_Param             ((ktype_t)7)
#define TY_Method            ((ktype_t)8)
#define TY_Func              ((ktype_t)9)
#define TY_System            ((ktype_t)10)
#define TY_0                ((ktype_t)11)    /* Parameter Type*/

#define CT_Object               CT_(TY_Object)
#define CT_Boolean              CT_(TY_Boolean)
#define CT_Int                  CT_(TY_Int)
#define CT_String               CT_(TY_String)
#define CT_Array                CT_(TY_Array)
#define CT_Param                CT_(TY_Param)
#define CT_Method               CT_(TY_Method)
#define CT_Func                 CT_(TY_Func)
#define CT_System               CT_(TY_System)

#define CT_StringArray          CT_Array
#define kStringArray            kArray
#define CT_MethodArray          CT_Array
#define kMethodArray            kArray

#define kClass_Ref              ((kshortflag_t)(1<<0))
#define kClass_Prototype        ((kshortflag_t)(1<<1))
#define kClass_Immutable        ((kshortflag_t)(1<<2))
#define kClass_Private          ((kshortflag_t)(1<<4))
#define kClass_Final            ((kshortflag_t)(1<<5))
#define kClass_Singleton        ((kshortflag_t)(1<<6))
#define kClass_UnboxType        ((kshortflag_t)(1<<7))
#define kClass_Interface        ((kshortflag_t)(1<<8))
#define kClass_TypeVar          ((kshortflag_t)(1<<9))
#define kClass_Forward          ((kshortflag_t)(1<<10))

#define CFLAG_void              kClass_TypeVar|kClass_UnboxType|kClass_Singleton|kClass_Final
#define CFLAG_var               kClass_TypeVar|kClass_UnboxType|kClass_Singleton|kClass_Final
#define CFLAG_Object            0
#define CFLAG_Boolean           kClass_Immutable|kClass_UnboxType|kClass_Final
#define CFLAG_Int               kClass_Immutable|kClass_UnboxType|kClass_Final
#define CFLAG_String            kClass_Immutable|kClass_Final
#define CFLAG_Array             kClass_Final
#define CFLAG_Param             kClass_Final
#define CFLAG_Method            kClass_Final
#define CFLAG_Func              kClass_Final
#define CFLAG_System            kClass_Singleton|kClass_Final
#define CFLAG_0                kClass_TypeVar|kClass_UnboxType|kClass_Singleton|kClass_Final


#define CT_isPrivate(ct)      (TFLAG_is(kshortflag_t,(ct)->cflag, kClass_Private))
#define TY_isSingleton(T)     (TFLAG_is(kshortflag_t,(CT_(T))->cflag, kClass_Singleton))
#define CT_isSingleton(ct)    (TFLAG_is(kshortflag_t,(ct)->cflag, kClass_Singleton))

#define CT_isFinal(ct)         (TFLAG_is(kshortflag_t,(ct)->cflag, kClass_Final))

#define TY_isForward(T)     (TFLAG_is(kshortflag_t,(CT_(T))->cflag, kClass_Forward))
#define CT_isForward(ct)    (TFLAG_is(kshortflag_t,(ct)->cflag, kClass_Forward))


// this is used in konoha.class
#define CT_isDefined(ct)  ((ct)->fallocsize == 0 || (ct)->fsize == (ct)->fallocsize)

//#define TY_isUnboxType(t)    (TFLAG_is(kshortflag_t,(ClassTBL(t))->cflag, kClass_UnboxType))
//#define T_isInterface(t)    (TFLAG_is(kshortflag_t,(ClassTBL(t))->cflag, kClass_Interface))
//#define T_isTypeVar(t)      (TFLAG_is(kshortflag_t,(ClassTBL(t))->cflag, kClass_TypeVar))

#define TY_isFunc(T)    (CT_(T)->bcid == TY_Func)

/* magic flag */
#define MAGICFLAG(f)             (K_OBJECT_MAGIC | ((kmagicflag_t)(f) & K_CFLAGMASK))

#define kObject_NullObject       ((kmagicflag_t)(1<<0))

#define kObject_Local6           ((kmagicflag_t)(1<<10))
#define kObject_Local5           ((kmagicflag_t)(1<<11))
#define kObject_Local4           ((kmagicflag_t)(1<<12))
#define kObject_Local3           ((kmagicflag_t)(1<<13))
#define kObject_Local2           ((kmagicflag_t)(1<<14))
#define kObject_Local1           ((kmagicflag_t)(1<<15))

#define kObject_is(O,A)            (TFLAG_is(kmagicflag_t,(O)->h.magicflag,A))
#define kObject_set(O,A,B)         TFLAG_set(kmagicflag_t, ((kObjectVar*)O)->h.magicflag,A,B)

#define kField_Hidden          ((kshortflag_t)(1<<0))
#define kField_Protected       ((kshortflag_t)(1<<1))
#define kField_Getter          ((kshortflag_t)(1<<2))
#define kField_Setter          ((kshortflag_t)(1<<3))
#define kField_Key             ((kshortflag_t)(1<<4))
#define kField_Volatile        ((kshortflag_t)(1<<5))
#define kField_ReadOnly        ((kshortflag_t)(1<<6))
#define kField_Property        ((kshortflag_t)(1<<7))

///* ------------------------------------------------------------------------ */
///* Type Variable */
////## @TypeVariable class Tvoid Tvoid;
////## @TypeVariable class Tvar  Tvoid;
//
//#define OFLAG_Tvoid              MAGICFLAG(0)
//#define TY_void                  TY_void
//#define OFLAG_Tvar               MAGICFLAG(0)
//#define CFLAG_Tvar               CFLAG_Tvoid
//#define TY_var                   TY_Tvar

/* ------------------------------------------------------------------------ */
/* Object */

#define kObject_NullObject         ((kmagicflag_t)(1<<0))
#define kObject_isNullObject(o)    (TFLAG_is(kmagicflag_t,(o)->h.magicflag,kObject_NullObject))
#define kObject_setNullObject(o,b) TFLAG_set(kmagicflag_t,((kObjectVar*)o)->h.magicflag,kObject_NullObject,b)

#define IS_NULL(o)                 ((((o)->h.magicflag) & kObject_NullObject) == kObject_NullObject)
#define IS_NOTNULL(o)              ((((o)->h.magicflag) & kObject_NullObject) != kObject_NullObject)

#define K_FASTMALLOC_SIZE  (sizeof(void*) * 8)

#define K_OBJECT_MAGIC           (578L << ((sizeof(kshortflag_t)*8)))
#define K_CFLAGMASK              (FLAG_Object_Ref)
#define KNH_MAGICFLAG(f)         (K_OBJECT_MAGIC | ((kmagicflag_t)(f) & K_CFLAGMASK))
#define DBG_ASSERT_ISOBJECT(o)   DBG_ASSERT(TFLAG_is(uintptr_t,(o)->h.magicflag, K_OBJECT_MAGIC))

typedef struct KonohaObjectHeader {
	kmagicflag_t magicflag;
	KonohaClass *ct;  //@RENAME
	union {
		uintptr_t refc;  // RCGC
		void *gcinfo;
		uintptr_t hashcode; // reserved
	};
	KUtilsGrowingArray *kvproto;
} KonohaObjectHeader ;

struct kObjectVar {
	KonohaObjectHeader h;
	union {
		kObject  *fieldObjectItems[4];
		uintptr_t fieldUnboxItems[4];
	};
};

#define O_ct(o)             ((o)->h.ct)
#define O_cid(o)            (O_ct(o)->cid)
#define O_bcid(o)           (O_ct(o)->bcid)
#define O_unbox(o)          (O_ct(o)->unbox(kctx, o))
#define O_p0(o)             (O_ct(o)->p0)

/* ------------------------------------------------------------------------ */
/* Boolean */

#define ABSTRACT_NUMBER \
		union {\
			uintptr_t  ndata;\
			kbool_t    bvalue;\
			kint_t     ivalue;\
			kuint_t    uvalue;\
			kfloat_t   fvalue;\
		}\

typedef const struct kNumberVar kNumber;
typedef struct kNumberVar       kNumberVar;

struct kNumberVar {
	KonohaObjectHeader h;
	ABSTRACT_NUMBER;
};

struct kBooleanVar /* extends kNumber */ {
	KonohaObjectHeader h;
	ABSTRACT_NUMBER;
};

#define IS_Boolean(o)              (O_cid(o) == TY_Boolean)
#define IS_TRUE(o)                 (O_bcid(o) == TY_Boolean && N_tobool(o))
#define IS_FALSE(o)                (O_bcid(o) == TY_Boolean && N_tobool(o) == 0)
#define new_Boolean(kctx, c)       ((c) ? K_TRUE : K_FALSE)
#define N_toint(o)                 (((kBoolean*)o)->ivalue)
#define N_tofloat(o)               (((kBoolean*)o)->fvalue)
#define N_tobool(o)                (((kBoolean*)o)->bvalue)

/* ------------------------------------------------------------------------ */
/* Int */

struct kIntVar /* extends kNumber */ {
	KonohaObjectHeader h;
	ABSTRACT_NUMBER;
};

#define IS_Int(o)              (O_cid(o) == TY_Int)

/* ------------------------------------------------------------------------ */
/* String */

#define TY_TEXT                   TY_void    /*special use for const char*/
#define TY_TYPE                   TY_var     /*special use for KonohaClass*/
#define IS_String(o)              (O_cid(o) == TY_String)

/*
 * Bit encoding for Rope String
 * 5432109876543210
 * 000xxxxxxxxxxxxx ==> magicflag bit representation
 * 001xxxxxxxxxxxxx LinerString
 * 011xxxxxxxxxxxxx ExterenalString
 * 010xxxxxxxxxxxxx InlinedString
 * 100xxxxxxxxxxxxx RopeString
 */

#define S_FLAG_MASK_BASE (13)
#define S_FLAG_LINER     ((1UL << (0)))
#define S_FLAG_NOFREE    ((1UL << (1)))
#define S_FLAG_ROPE      ((1UL << (2)))
#define S_FLAG_INLINE    (S_FLAG_NOFREE)
#define S_FLAG_EXTERNAL  (S_FLAG_LINER | S_FLAG_NOFREE)

#define S_isRope(o)          (TFLAG_is(uintptr_t,(o)->h.magicflag,kObject_Local1))
#define S_isTextSgm(o)       (TFLAG_is(uintptr_t,(o)->h.magicflag,kObject_Local2))
#define S_setTextSgm(o,b)    TFLAG_set(uintptr_t,(o)->h.magicflag,kObject_Local2,b)
#define S_isMallocText(o)    (TFLAG_is(uintptr_t,(o)->h.magicflag,kObject_Local3))
#define S_setMallocText(o,b) TFLAG_set(uintptr_t,(o)->h.magicflag,kObject_Local3,b)
#define S_isASCII(o)         (TFLAG_is(uintptr_t,(o)->h.magicflag,kObject_Local4))
#define S_setASCII(o,b)      TFLAG_set(uintptr_t,((kObjectVar*)o)->h.magicflag,kObject_Local4,b)
#define S_isPooled(o)        (TFLAG_is(uintptr_t,(o)->h.magicflag,kObject_Local5))
#define S_setPooled(o,b)     TFLAG_set(uintptr_t,(o)->h.magicflag,kObject_Local5,b)
#define SIZEOF_INLINETEXT    (sizeof(void*)*8 - sizeof(kBytes))

typedef const struct _kBytes kBytes;
struct _kBytes {
	KonohaObjectHeader h;
	COMMON_BYTEARRAY;
};

struct kStringVar /* extends _Bytes */ {
	KonohaObjectHeader h;
	COMMON_BYTEARRAY;
	const char inline_text[SIZEOF_INLINETEXT];
};

#define SPOL_TEXT          (1<<0)
#define SPOL_ASCII         (1<<1)
#define SPOL_UTF8          (1<<2)
#define SPOL_POOL          (1<<3)
#define SPOL_NOPOOL        (1<<5)
#define SPOL_NOCOPY        (1<<4)

#define new_T(t)            (KLIB new_kString(kctx, t, knh_strlen(t), SPOL_TEXT|SPOL_ASCII|SPOL_POOL))
#define new_S(T, L)         (KLIB new_kString(kctx, T, L, SPOL_ASCII|SPOL_POOL))
#define S_text(s)           ((const char*) (O_ct(s)->unbox(kctx, (kObject*)s)))
#define S_size(s)           ((s)->bytesize)

//#define S_equals(s, b)        knh_bytes_equals(S_tobytes(s), b)
//#define S_startsWith(s, b)    knh_bytes_startsWith_(S_tobytes(s), b)
//#define S_endsWith(s, b)      knh_bytes_endsWith_(S_tobytes(s), b)

/* ------------------------------------------------------------------------ */
//## class Array   Object;

#define IS_Array(o)              (O_bcid(o) == TY_Array)
#define kArray_isUnboxData(o)    (TFLAG_is(uintptr_t,(o)->h.magicflag,kObject_Local1))
#define kArray_setUnboxData(o,b) TFLAG_set(uintptr_t,(o)->h.magicflag,kObject_Local1,b)

struct kArrayVar {
	KonohaObjectHeader h;
	size_t bytesize;
	union {
		uintptr_t              *unboxItems;
		kint_t                 *kintItems;
#ifndef K_USING_NOFLOAT
		kfloat_t               *kfloatItems;
#endif
		kObject        **objectItems;
		kString        **stringItems;
		kParam         **paramItems;
		kMethod        **methodItems;
		kFunc          **funcItems;
		kToken         **tokenItems;
		kTokenVar      **tokenVarItems;
		kExpr          **exprItems;
		kExprVar       **exprVarItems;
		kStmt          **stmtItems;
		kStmtVar       **stmtVarItems;
	};
	size_t bytemax;
};

/* ------------------------------------------------------------------------ */
/* Param */

#define IS_Param(o)              (O_bcid(o) == TY_Param)

typedef struct kparam_t {
	ktype_t    ty;  ksymbol_t  fn;
} kparam_t;

struct kParamVar {
	KonohaObjectHeader h;
	ktype_t rtype; kushort_t psize;
	kparam_t p[3];
};

/* ------------------------------------------------------------------------ */
/* Method */

#define IS_Method(o)              (O_bcid(o) == TY_Method)

#define kMethod_Public               ((uintptr_t)(1<<0))
#define kMethod_Virtual              ((uintptr_t)(1<<1))
#define kMethod_Hidden               ((uintptr_t)(1<<2))
#define kMethod_Const                ((uintptr_t)(1<<3))
#define kMethod_Static               ((uintptr_t)(1<<4))
#define kMethod_Immutable            ((uintptr_t)(1<<5))
#define kMethod_Restricted           ((uintptr_t)(1<<6))
#define kMethod_Overloaded           ((uintptr_t)(1<<7))
#define kMethod_CALLCC               ((uintptr_t)(1<<8))
#define kMethod_FASTCALL             ((uintptr_t)(1<<9))
#define kMethod_D                    ((uintptr_t)(1<<10))
#define kMethod_Abstract             ((uintptr_t)(1<<11))
#define kMethod_Coercion             ((uintptr_t)(1<<12))
#define kMethod_SmartReturn          ((uintptr_t)(1<<13))

#define kMethod_isPublic(o)     (TFLAG_is(uintptr_t, (o)->flag,kMethod_Public))
//#define kMethod_setPublic(o,B)  TFLAG_set(uintptr_t, (o)->flag,kMethod_Public,B)
#define kMethod_isVirtual(o)     (TFLAG_is(uintptr_t, (o)->flag,kMethod_Virtual))
//#define kMethod_setVirtual(o,B)  TFLAG_set(uintptr_t, (o)->flag,kMethod_Virtual,B)
#define kMethod_isHidden(o)     (TFLAG_is(uintptr_t, (o)->flag,kMethod_Hidden))
#define kMethod_setHidden(o,B)  TFLAG_set(uintptr_t, (o)->flag,kMethod_Hidden,B)
#define kMethod_isStatic(o)     (TFLAG_is(uintptr_t, (o)->flag,kMethod_Static))
#define kMethod_setStatic(o,B)  TFLAG_set(uintptr_t, (o)->flag,kMethod_Static,B)
#define kMethod_isConst(o)      (TFLAG_is(uintptr_t, (o)->flag,kMethod_Const))
//#define kMethod_setConst(o,B)   TFLAG_set(uintptr_t, (o)->flag,kMethod_Const,B)
#define kMethod_isVirtual(o)     (TFLAG_is(uintptr_t, (o)->flag,kMethod_Virtual))
#define kMethod_setVirtual(o,B)  TFLAG_set(uintptr_t, (o)->flag,kMethod_Virtual,B)

#define kMethod_isSmartReturn(o)     (TFLAG_is(uintptr_t, (o)->flag, kMethod_SmartReturn))

#define kMethod_isTransCast(mtd)    MN_isTOCID(mtd->mn)
#define kMethod_isCast(mtd)         MN_isASCID(mtd->mn)
#define kMethod_isCoercion(mtd)    (TFLAG_is(uintptr_t, (mtd)->flag,kMethod_Coercion))
//#define kMethod_isOverload(o)  (TFLAG_is(uintptr_t,DP(o)->flag,kMethod_Overload))
//#define kMethod_setOverload(o,b) TFLAG_set(uintptr_t,DP(o)->flag,kMethod_Overload,b)

#define kMethod_param(mtd)        kctx->share->paramList->paramItems[mtd->paramid]
#define kMethod_rtype(mtd)        (kMethod_param(mtd))->rtype

/* method data */
#define DEND     (-1)

//#if 1
//#define _RIX         ,long _rix
//#define (-(K_CALLDELTA))        _rix
//#define    ,K_RTNIDX
//#define _KFASTCALL   ,long _rix
//#define K_FASTRIX    _rix
//#else
//#define _RIX
//#define (-(K_CALLDELTA)) (-4)
//#define
//#define _KFASTCALL
//#define K_FASTRIX
//#endif

#ifdef K_USING_WIN32_
//#define KMETHOD  void CC_EXPORT
//#define ITRNEXT int   CC_EXPORT
//typedef void (CC_EXPORT *MethodFunc)(KonohaContext*, KonohaStack*);
//typedef int  (CC_EXPORT *knh_Fitrnext)(KonohaContext*, KonohaStack *);
#else
#define KMETHOD    void  /*CC_FASTCALL_*/
#define KMETHODCC  int  /*CC_FASTCALL_*/
typedef KMETHOD   (*MethodFunc)(KonohaContext*, KonohaStack*);
typedef KMETHOD   (*FastCallMethodFunc)(KonohaContext*, KonohaStack * _KFASTCALL);
typedef KMETHODCC (*FmethodCallCC)(KonohaContext*, KonohaStack *, int, int, struct VirtualMachineInstruction*);
#endif

struct kMethodVar {
	KonohaObjectHeader     h;
	union {
		MethodFunc              invokeMethodFunc;
		FastCallMethodFunc      invokeFastCallMethodFunc;
	};
	union {/* body*/
		struct VirtualMachineInstruction        *pc_start;
		FmethodCallCC         callcc_1;
	};
	uintptr_t         flag;
	ktype_t            cid;      kmethodn_t  mn;
	kparamid_t        paramid;  kparamid_t paramdom;
	kshort_t          delta;    kpackage_t packageId;
	kToken        *tcode;
	union {
		kObject      *objdata;
		const struct kByteCodeVar    *kcode;
		kNameSpace   *lazyns;       // lazy compilation
	};
	kMethod           *proceedNUL;   // proceed
};

#define K_CALLDELTA   4
#define K_RTNIDX    (-4)
#define K_SHIFTIDX  (-3)
#define K_PCIDX     (-2)
#define K_MTDIDX    (-1)
#define K_TMRIDX    (0)
#define K_SELFIDX   0

//#define K_NEXTIDX    2
#define K_ULINEIDX2  (-7)
#define K_SHIFTIDX2  (-5)
#define K_PCIDX2     (-3)
#define K_MTDIDX2    (-1)

/* ------------------------------------------------------------------------ */
/* Func */

#define IS_Func(o)              (O_bcid(o) == TY_Func)

struct kFuncVar {
	KonohaObjectHeader h;
	kObject *self;
	kMethod *mtd;
};

/* ------------------------------------------------------------------------ */
/* System */

#define IS_System(o)              (O_cid(o) == TY_System)

typedef const struct _kSystem kSystem;

struct _kSystem {
	KonohaObjectHeader h;
};

/* ------------------------------------------------------------------------ */
/* T0 */



/* ------------------------------------------------------------------------ */
/* macros */

#define klr_setesp(kctx, newesp)  ((KonohaContextVar*)kctx)->esp = (newesp)
#define klr_setmtdNC(sfpA, mtdO)   sfpA.mtdNC = mtdO

//#define Method_isByteCode(mtd) ((mtd)->invokeMethodFunc == MethodFunc_runVirtualMachine)
#define Method_isByteCode(mtd) (0)

#define BEGIN_LOCAL(V,N) \
	KonohaStack *V = kctx->esp, *esp_ = kctx->esp; (void)V;((KonohaContextVar*)kctx)->esp = esp_+N;\

#define END_LOCAL() ((KonohaContextVar*)kctx)->esp = esp_;

#define KCALL(LSFP, RIX, MTD, ARGC, DEFVAL) { \
		KonohaStack *tsfp = LSFP + RIX + K_CALLDELTA;\
		tsfp[K_MTDIDX].mtdNC = MTD;\
		tsfp[K_PCIDX].fname = __FILE__;\
		tsfp[K_SHIFTIDX].shift = 0;\
		KSETv(tsfp[K_RTNIDX].o, ((kObject*)DEFVAL));\
		tsfp[K_RTNIDX].uline = __LINE__;\
		klr_setesp(kctx, tsfp + ARGC + 1);\
		(MTD)->invokeMethodFunc(kctx, tsfp);\
		tsfp[K_MTDIDX].mtdNC = NULL;\
	} \

#define KSELFCALL(TSFP, MTD) { \
		KonohaStack *tsfp = TSFP;\
		tsfp[K_MTDIDX].mtdNC = MTD;\
		(MTD)->invokeMethodFunc(kctx, tsfp);\
		tsfp[K_MTDIDX].mtdNC = NULL;\
	} \


/* ----------------------------------------------------------------------- */
// kklib

struct klogconf_t;

struct KonohaLibVar {
	void* (*Kmalloc)(KonohaContext*, size_t);
	void* (*Kzmalloc)(KonohaContext*, size_t);
	void  (*Kfree)(KonohaContext*, void *, size_t);

	void  (*Karray_init)(KonohaContext *, KUtilsGrowingArray *, size_t);
	void  (*Karray_resize)(KonohaContext*, KUtilsGrowingArray *, size_t);
	void  (*Karray_expand)(KonohaContext*, KUtilsGrowingArray *, size_t);
	void  (*Karray_free)(KonohaContext*, KUtilsGrowingArray *);

	void                (*Kwb_init)(KUtilsGrowingArray *, KUtilsWriteBuffer *);
	void                (*Kwb_write)(KonohaContext*, KUtilsWriteBuffer *, const char *, size_t);
	void                (*Kwb_putc)(KonohaContext*, KUtilsWriteBuffer *, ...);
	void                (*Kwb_vprintf)(KonohaContext*, KUtilsWriteBuffer *, const char *fmt, va_list ap);
	void                (*Kwb_printf)(KonohaContext*, KUtilsWriteBuffer *, const char *fmt, ...);
	const char*         (*Kwb_top)(KonohaContext*, KUtilsWriteBuffer *, int);
	void                (*Kwb_free)(KUtilsWriteBuffer *);

	KUtilsHashMap*      (*Kmap_init)(KonohaContext*, size_t);
	KUtilsHashMapEntry* (*Kmap_newentry)(KonohaContext*, KUtilsHashMap *, uintptr_t);
	KUtilsHashMapEntry* (*Kmap_get)(KonohaContext*, KUtilsHashMap *, uintptr_t);
	void                (*Kmap_remove)(KUtilsHashMap *, KUtilsHashMapEntry *);
	void                (*Kmap_reftrace)(KonohaContext*, KUtilsHashMap *, void (*)(KonohaContext*, KUtilsHashMapEntry*));
	void                (*Kmap_free)(KonohaContext*, KUtilsHashMap *, void (*)(KonohaContext*, void *));
	ksymbol_t           (*Kmap_getcode)(KonohaContext*, KUtilsHashMap *, kArray *, const char *, size_t, uintptr_t, int, ksymbol_t);


	kfileline_t     (*KfileId)(KonohaContext*, const char *, size_t, int spol, ksymbol_t def);
	kpackage_t      (*KpackageId)(KonohaContext*, const char *, size_t, int spol, ksymbol_t def);
	ksymbol_t       (*Ksymbol)(KonohaContext*, const char*, size_t, int spol, ksymbol_t def);

	kbool_t         (*KimportPackage)(KonohaContext*, kNameSpace*, const char *, kfileline_t);
	KonohaClass*    (*Kclass)(KonohaContext*, ktype_t, kfileline_t);
	kString*        (*KonohaClass_shortName)(KonohaContext*, KonohaClass *ct);
	KonohaClass*    (*KonohaClass_Generics)(KonohaContext*, KonohaClass *ct, ktype_t rty, int psize, kparam_t *p);

	kObject*        (*new_kObject)(KonohaContext*, KonohaClass *, uintptr_t);  // GCUNSAFE
	kObject*        (*new_kObjectOnGCSTACK)(KonohaContext*, KonohaClass *, uintptr_t);
	kObject*        (*Knull)(KonohaContext*, KonohaClass *);
	kObject*        (*kObject_getObject)(KonohaContext*, kAbstractObject *, ksymbol_t, kAbstractObject *);
	void            (*kObject_setObject)(KonohaContext*, kAbstractObject *, ksymbol_t, ktype_t, kAbstractObject *);
	uintptr_t       (*kObject_getUnboxValue)(KonohaContext*, kAbstractObject *, ksymbol_t, uintptr_t);
	void            (*kObject_setUnboxValue)(KonohaContext*, kAbstractObject *, ksymbol_t, ktype_t, uintptr_t);
	void            (*kObject_protoEach)(KonohaContext*, kAbstractObject *, void *thunk, void (*f)(KonohaContext*, void *, KUtilsKeyValue *d));
	void            (*kObject_removeKey)(KonohaContext*, kAbstractObject *, ksymbol_t);

	kString*    (*new_kString)(KonohaContext*, const char *, size_t, int);
	kString*    (*new_kStringf)(KonohaContext*, int, const char *, ...);

	void (*kArray_add)(KonohaContext*, kArray *, kAbstractObject *);
	void (*kArray_insert)(KonohaContext*, kArray *, size_t, kAbstractObject *);
	void (*kArray_clear)(KonohaContext*, kArray *, size_t);

	kMethod *  (*new_kMethod)(KonohaContext*, uintptr_t, ktype_t, kmethodn_t, MethodFunc);
	kParam*    (*kMethod_setParam)(KonohaContext*, kMethod *, ktype_t, int, kparam_t *);
	void       (*kMethod_setFunc)(KonohaContext*, kMethod*, MethodFunc);
	void       (*kMethod_genCode)(KonohaContext*, kMethod*, kBlock *bk);
	intptr_t   (*kMethod_indexOfField)(kMethod *);

	kbool_t      (*Konoha_setModule)(KonohaContext*, int, struct KonohaModule *, kfileline_t);
	KonohaClass* (*Konoha_defineClass)(KonohaContext*, kpackage_t, kpackage_t, kString *, KDEFINE_TY *, kfileline_t);

	KonohaClass*  (*kNameSpace_getCT)(KonohaContext*, kNameSpace *, KonohaClass *, const char *, size_t, ktype_t def);
	void          (*kNameSpace_loadMethodData)(KonohaContext*, kNameSpace *, intptr_t *d);
	void          (*kNameSpace_loadConstData)(KonohaContext*, kNameSpace *, const char **d, kfileline_t);
	kMethod*      (*kNameSpace_getMethodNULL)(KonohaContext*, kNameSpace *, ktype_t cid, kmethodn_t mn);
//	kMethod*      (*kNameSpace_getGetterMethodNULL)(KonohaContext*, kNameSpace *, ktype_t cid, ksymbol_t sym);
	void          (*kNameSpace_syncMethods)(KonohaContext *kctx);

	void          (*KCodeGen)(KonohaContext*, kMethod *, kBlock *);
	void          (*Kreportf)(KonohaContext*, kinfotag_t, kfileline_t, const char *fmt, ...);
	void          (*Kraise)(KonohaContext*, int isContinue);     // module

	uintptr_t     (*Ktrace)(KonohaContext*, struct klogconf_t *logconf, ...);
};

#define K_NULL            (kctx->share->constNull)
#define K_TRUE            (kctx->share->constTrue)
#define K_FALSE           (kctx->share->constFalse)
#define K_NULLPARAM       (kctx->share->paramList->paramItems[0])
#define K_EMPTYARRAY      (kctx->share->emptyArray)
#define TS_EMPTY          (kctx->share->emptyString)

#define UPCAST(o)         ((kObject*)o)

#define KPI                     (kctx->klib)

#define KMALLOC(size)          (KPI)->Kmalloc(kctx, size)
#define KCALLOC(size, item)    (KPI)->Kzmalloc(kctx, ((size) * (item)))
#define KFREE(p, size)         (KPI)->Kfree(kctx, p, size)

#define kwb_putc(W,...)          (KPI)->Kwb_putc(kctx,W, ## __VA_ARGS__, -1)
#define Kwb_bytesize(W)                 (((W)->m)->bytesize - (W)->pos)

#define kclass(CID, UL)           (KPI)->Kclass(kctx, CID, UL)

#define FILEID_(T)                KLIB KfileId(kctx, T, sizeof(T)-1, SPOL_TEXT|SPOL_ASCII, _NEWID)

#define PN_konoha                 0
#define PN_sugar                  1
#define PN_(T)                    KLIB KpackageId(kctx, T, sizeof(T)-1, SPOL_TEXT|SPOL_ASCII|SPOL_POOL, _NEWID)

#define ksymbolA(T, L, DEF)       (KPI)->Ksymbol(kctx, T, L, SPOL_ASCII, DEF)
#define ksymbolSPOL(T, L, SPOL, DEF)       (KPI)->Ksymbol(kctx, T, L, SPOL, DEF)
#define SYM_(T)                   (KPI)->Ksymbol(kctx, T, (sizeof(T)-1), SPOL_TEXT|SPOL_ASCII, _NEWID)
#define FN_(T)                    (KPI)->Ksymbol(kctx, T, (sizeof(T)-1), SPOL_TEXT|SPOL_ASCII, _NEWID)
#define MN_(T)                    (KPI)->Ksymbol(kctx, T, (sizeof(T)-1), SPOL_TEXT|SPOL_ASCII, _NEWID)
#define T_mn(X)                   SYM_PRE(X), SYM_t(X)

// #define KW_new (((ksymbol_t)39)|0) /*new*/
#define MN_new                    39  /* @see */

#define new_(C, A)                (k##C*)(KLIB new_kObject(kctx,      CT_##C, ((uintptr_t)A)))
#define new_Var(C,A)              (k##C##Var*)(KLIB new_kObject(kctx, CT_##C, ((uintptr_t)A)))

#define knull(C)                  (KPI)->Knull(kctx, C)
#define KNULL(C)                  (k##C*)(KPI)->Knull(kctx, CT_##C)

#define kArray_size(A)            (((A)->bytesize)/sizeof(void*))
#define kArray_setsize(A, N)      ((kArrayVar*)A)->bytesize = N * sizeof(void*)
//#define KLIB kArray_add(kctx, A, V)          (KPI)->KLIB kArray_add(kctx, kctx, A, UPCAST(V))
//#define KLIB kArray_insert(kctx, A, N, V)    (KPI)->KLIB kArray_insert(kctx, kctx, A, N, UPCAST(V))
//#define KLIB kArray_clear(kctx, A, S)        (KPI)->KLIB kArray_clear(kctx, kctx, A, S)

//#define new_kParam(R,S,P)        (KPI)->Knew_Method(kctx, R, S, P)
//#define KLIB new_kMethod(kctx, F,C,M,FF)  (KPI)->Knew_Method(kctx, F, C, M, FF)
//#define KLIB kMethod_setParam(kctx, M, R, PSIZE, P)      (KPI)->KLIB kMethod_setParam(kctx, kctx, M, R, PSIZE, P)
#define new_kParam(CTX, R, PSIZE, P)       (KLIB kMethod_setParam(CTX, NULL, R, PSIZE, P))
//#define KLIB kMethod_setFunc(kctx, M,F)     (KPI)->KLIB kMethod_setFunc(kctx, kctx, M, F)

#define KREQUIRE_PACKAGE(NAME, UL)                   (KPI)->KimportPackage(kctx, NULL, NAME, UL)
#define KEXPORT_PACKAGE(NAME, KS, UL)                (KPI)->KimportPackage(kctx, KS, NAME, UL)

//#define KTY(cid)                          S_text(CT(cid)->name)
//#define onohaClass_Generics(kctx, CT, RTY, PSIZE, P)    (KPI)->KonohaClass_Generics(kctx, CT, RTY, PSIZE, P)

//#define kNameSpace_getCT(NS, THIS, S, L, C)      (KPI)->NameSpace_getCT(kctx, NS, THIS, S, L, C)
//#define kNameSpace_syncMethods()    (KPI)->NameSpace_syncMethods(kctx)

typedef intptr_t  KDEFINE_METHOD;

#define KonohaConst_(D)  ((const char **)D)

typedef struct {
	const char *key;
	uintptr_t ty;
	kint_t value;
} KDEFINE_INT_CONST;

typedef struct {
	const char *key;
	uintptr_t ty;
	const char* value;
} KDEFINE_TEXT_CONST;

typedef struct {
	const char *key;
	uintptr_t ty;
	kfloat_t value;
} KDEFINE_FLOAT_CONST;

typedef struct {
	const char *key;
	uintptr_t ty;
	kObject *value;
} KDEFINE_OBJECT_CONST;

#define kreportf(LEVEL, UL, fmt, ...)  (KPI)->Kreportf(kctx, LEVEL, UL, fmt, ## __VA_ARGS__)
#define kraise(PARAM)                  (KPI)->Kraise(kctx, PARAM)

#define KSET_KLIB(T, UL)   do {\
		void *func = kctx->klib->T;\
		((KonohaLibVar*)kctx->klib)->T = T;\
		if(func != NULL) {\
			kreportf(DebugTag, UL, "override of klib->" #T ", file=%s, line=%d", __FILE__, __LINE__);\
		}\
	}while(0)\

#define KSET_KLIB2(T, F, UL)   do {\
		void *func = kctx->klib->T;\
		((KonohaLibVar*)kctx->klib)->T = F;\
		if(func != NULL) {\
			kreportf(DebugTag, UL, "override of kklib->" #T ", file=%s, line=%d", __FILE__, __LINE__);\
		}\
	}while(0)\

#define KSET_TYFUNC(ct, T, PREFIX, UL)   do {\
		void *func = ct->T;\
		((KonohaClassVar*)ct)->T = PREFIX##_##T;\
		if(func != NULL) {\
			kreportf(DebugTag, UL, "override of %s->" #T ", file=%s, line=%d", CT_t(ct), __FILE__, __LINE__);\
		}\
	}while(0)\

// gc

#if defined(_MSC_VER)
#define OBJECT_SET(var, val) do {\
	kObject **var_ = (kObject**)&val; \
	var_[0] = (val_); \
} while (0)
#else
#define OBJECT_SET(var, val) var = (typeof(var))(val)
#endif /* defined(_MSC_VER) */

/* macros */

#define INIT_GCSTACK()         size_t gcstack_ = kArray_size(kctx->stack->gcstack)
#define PUSH_GCSTACK(o)        KLIB kArray_add(kctx, kctx->stack->gcstack, o)
#define RESET_GCSTACK()        KLIB kArray_clear(kctx, kctx->stack->gcstack, gcstack_)

#define KINITv(VAR, VAL)   OBJECT_SET(VAR, VAL)
#define KSETv(VAR, VAL)    /*OBJECT_SET(VAR, VAL)*/ VAR = VAL
#define KINITp(parent, v, o) KINITv(v, o)
#define KSETp(parent,  v, o) KSETv(v, o)
#define KUNUSEv(V)         (V)->h.ct->free(kctx, (V))

#define BEGIN_REFTRACE(SIZE)  int _ref_ = (SIZE); kObjectVar** _tail = KONOHA_reftail(kctx, (SIZE));
#define END_REFTRACE()        (void)_ref_; kctx->stack->reftail = _tail;

#define KREFTRACEv(p)  do {\
	DBG_ASSERT(p != NULL);\
	_tail[0] = (kObjectVar*)p;\
	_tail++;\
} while (0)

#define KREFTRACEn(p) do {\
	if(p != NULL) {\
		_tail[0] = (kObjectVar*)p;\
		_tail++;\
	}\
} while (0)

#define KNH_SAFEPOINT(kctx, sfp)

// method macro

#define RETURN_(vv) do {\
	KSETv(sfp[(-(K_CALLDELTA))].o, ((kObject*)vv));\
	KNH_SAFEPOINT(kctx, sfp);\
	return; \
} while (0)

#define RETURNd_(d) do {\
	sfp[(-(K_CALLDELTA))].ndata = d; \
	return; \
} while (0)

#define RETURNb_(c) do {\
	sfp[(-(K_CALLDELTA))].bvalue = c; \
	return; \
} while(0)

#define RETURNi_(c) do {\
	sfp[(-(K_CALLDELTA))].ivalue = c; \
	return; \
} while (0)

#define RETURNf_(c) do {\
	sfp[(-(K_CALLDELTA))].fvalue = c; \
	return; \
} while (0)

#define RETURNvoid_() do {\
	return; \
} while (0)

//#ifndef K_NODEBUG
#define KNH_ASSERT(a)       assert(a)
#define DBG_ASSERT(a)       assert(a)
#define TODO_ASSERT(a)      assert(a)
#define DBG_P(fmt, ...)     PLATAPI dbg_p(__FILE__, __FUNCTION__, __LINE__, fmt, ## __VA_ARGS__)
#define DBG_ABORT(fmt, ...) PLATAPI dbg_p(__FILE__, __FUNCTION__, __LINE__, fmt, ## __VA_ARGS__); PLATAPI exit_i(EXIT_FAILURE)
#define DUMP_P(fmt, ...)    PLATAPI printf_i(fmt, ## __VA_ARGS__)
//#else
//#define KNH_ASSERT(a)
//#define DBG_ASSERT(a)
//#define TODO_ASSERT(a)
//#define DBG_P(fmt, ...)
//#define DBG_ABORT(fmt, ...)
//#define DUMP_P(fmt, ...)
//#endif

#ifndef unlikely
#define unlikely(x)   __builtin_expect(!!(x), 0)
#define likely(x)     __builtin_expect(!!(x), 1)

#endif /*unlikely*/

///* Konoha API */
extern KonohaContext* konoha_open(const PlatformApi *);
extern void konoha_close(KonohaContext* konoha);
extern kbool_t konoha_load(KonohaContext* konoha, const char *scriptfile);
extern kbool_t konoha_eval(KonohaContext* konoha, const char *script, kfileline_t uline);
extern kbool_t konoha_run(KonohaContext* konoha);  // TODO

#ifdef USE_BUILTINTEST
typedef int (*BuiltInTestFunc)(KonohaContext *kctx);
typedef struct DEFINE_TESTFUNC {
	const char *name;
	BuiltInTestFunc f;
} DEFINE_TESTFUNC ;
#endif

#include "logger.h"

#endif /* KONOHA2_H_ */