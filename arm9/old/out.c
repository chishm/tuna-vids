# 1 "colorspace.c"
# 1 "<built-in>"
# 1 "<command line>"
# 1 "colorspace.c"
# 26 "colorspace.c"
# 1 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/string.h" 1 3
# 10 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/string.h" 3
# 1 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/_ansi.h" 1 3
# 15 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/_ansi.h" 3
# 1 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/newlib.h" 1 3
# 16 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/_ansi.h" 2 3
# 1 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/sys/config.h" 1 3



# 1 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/machine/ieeefp.h" 1 3
# 5 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/sys/config.h" 2 3
# 17 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/_ansi.h" 2 3
# 11 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/string.h" 2 3
# 1 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/sys/reent.h" 1 3
# 13 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/sys/reent.h" 3
# 1 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/_ansi.h" 1 3
# 14 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/sys/reent.h" 2 3
# 1 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/sys/_types.h" 1 3
# 12 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/sys/_types.h" 3
# 1 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/sys/lock.h" 1 3





typedef int _LOCK_T;
typedef int _LOCK_RECURSIVE_T;
# 13 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/sys/_types.h" 2 3

typedef long _off_t;
__extension__ typedef long long _off64_t;


typedef int _ssize_t;





# 1 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/include/stddef.h" 1 3 4
# 355 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/include/stddef.h" 3 4
typedef unsigned int wint_t;
# 25 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/sys/_types.h" 2 3


typedef struct
{
  int __count;
  union
  {
    wint_t __wch;
    unsigned char __wchb[4];
  } __value;
} _mbstate_t;

typedef _LOCK_RECURSIVE_T _flock_t;


typedef void *_iconv_t;
# 15 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/sys/reent.h" 2 3






typedef unsigned long __ULong;
# 42 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/sys/reent.h" 3
struct _Bigint
{
  struct _Bigint *_next;
  int _k, _maxwds, _sign, _wds;
  __ULong _x[1];
};


struct __tm
{
  int __tm_sec;
  int __tm_min;
  int __tm_hour;
  int __tm_mday;
  int __tm_mon;
  int __tm_year;
  int __tm_wday;
  int __tm_yday;
  int __tm_isdst;
};







struct _on_exit_args {
 void * _fnargs[32];
 void * _dso_handle[32];

 __ULong _fntypes;


 __ULong _is_cxa;
};
# 87 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/sys/reent.h" 3
struct _atexit {
 struct _atexit *_next;
 int _ind;

 void (*_fns[32])(void);
        struct _on_exit_args _on_exit_args;
};
# 103 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/sys/reent.h" 3
struct __sbuf {
 unsigned char *_base;
 int _size;
};






typedef long _fpos_t;
# 168 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/sys/reent.h" 3
struct __sFILE {
  unsigned char *_p;
  int _r;
  int _w;
  short _flags;
  int _file;
  struct __sbuf _bf;
  int _lbfsize;






  void * _cookie;

  int (*_read) (void * _cookie, char *_buf, int _n);
  int (*_write) (void * _cookie, const char *_buf, int _n);

  _fpos_t (*_seek) (void * _cookie, _fpos_t _offset, int _whence);
  int (*_close) (void * _cookie);


  struct __sbuf _ub;
  unsigned char *_up;
  int _ur;


  unsigned char _ubuf[3];
  unsigned char _nbuf[1];


  struct __sbuf _lb;


  int _blksize;
  int _offset;


  struct _reent *_data;



  _flock_t _lock;

};
# 261 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/sys/reent.h" 3
typedef struct __sFILE __FILE;


struct _glue
{
  struct _glue *_next;
  int _niobs;
  __FILE *_iobs;
};
# 292 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/sys/reent.h" 3
struct _rand48 {
  unsigned short _seed[3];
  unsigned short _mult[3];
  unsigned short _add;




};
# 561 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/sys/reent.h" 3
struct _reent
{
  int _errno;




  __FILE *_stdin, *_stdout, *_stderr;

  int _inc;
  char _emergency[25];

  int _current_category;
  const char *_current_locale;

  int __sdidinit;

  void (*__cleanup) (struct _reent *);


  struct _Bigint *_result;
  int _result_k;
  struct _Bigint *_p5s;
  struct _Bigint **_freelist;


  int _cvtlen;
  char *_cvtbuf;

  union
    {
      struct
        {
          unsigned int _unused_rand;
          char * _strtok_last;
          char _asctime_buf[26];
          struct __tm _localtime_buf;
          int _gamma_signgam;
          __extension__ unsigned long long _rand_next;
          struct _rand48 _r48;
          _mbstate_t _mblen_state;
          _mbstate_t _mbtowc_state;
          _mbstate_t _wctomb_state;
          char _l64a_buf[8];
          char _signal_buf[24];
          int _getdate_err;
          _mbstate_t _mbrlen_state;
          _mbstate_t _mbrtowc_state;
          _mbstate_t _mbsrtowcs_state;
          _mbstate_t _wcrtomb_state;
          _mbstate_t _wcsrtombs_state;
        } _reent;



      struct
        {

          unsigned char * _nextf[30];
          unsigned int _nmalloc[30];
        } _unused;
    } _new;


  struct _atexit *_atexit;
  struct _atexit _atexit0;


  void (**(_sig_func))(int);




  struct _glue __sglue;
  __FILE __sf[3];
};
# 793 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/sys/reent.h" 3
extern struct _reent *_impure_ptr ;
extern struct _reent *const _global_impure_ptr ;

void _reclaim_reent (struct _reent *);
# 12 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/string.h" 2 3


# 1 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/include/stddef.h" 1 3 4
# 214 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/include/stddef.h" 3 4
typedef unsigned int size_t;
# 15 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/string.h" 2 3







void * memchr (const void *, int, size_t);
int memcmp (const void *, const void *, size_t);
void * memcpy (void *, const void *, size_t);
void * memmove (void *, const void *, size_t);
void * memset (void *, int, size_t);
char *strcat (char *, const char *);
char *strchr (const char *, int);
int strcmp (const char *, const char *);
int strcoll (const char *, const char *);
char *strcpy (char *, const char *);
size_t strcspn (const char *, const char *);
char *strerror (int);
size_t strlen (const char *);
char *strncat (char *, const char *, size_t);
int strncmp (const char *, const char *, size_t);
char *strncpy (char *, const char *, size_t);
char *strpbrk (const char *, const char *);
char *strrchr (const char *, int);
size_t strspn (const char *, const char *);
char *strstr (const char *, const char *);


char *strtok (char *, const char *);


size_t strxfrm (char *, const char *, size_t);


char *strtok_r (char *, const char *, char **);

int bcmp (const void *, const void *, size_t);
void bcopy (const void *, void *, size_t);
void bzero (void *, size_t);
int ffs (int);
char *index (const char *, int);
void * memccpy (void *, const void *, int, size_t);
void * mempcpy (void *, const void *, size_t);



char *rindex (const char *, int);
int strcasecmp (const char *, const char *);
char *strdup (const char *);
char *_strdup_r (struct _reent *, const char *);
char *strndup (const char *, size_t);
char *_strndup_r (struct _reent *, const char *, size_t);
char *strerror_r (int, char *, size_t);
size_t strlcat (char *, const char *, size_t);
size_t strlcpy (char *, const char *, size_t);
int strncasecmp (const char *, const char *, size_t);
size_t strnlen (const char *, size_t);
char *strsep (char **, const char *);
char *strlwr (char *);
char *strupr (char *);
# 99 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/string.h" 3
# 1 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/sys/string.h" 1 3
# 100 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/string.h" 2 3


# 27 "colorspace.c" 2

# 1 "../global.h" 1
# 29 "../global.h"
# 1 "../xvid.h" 1
# 118 "../xvid.h"
typedef struct {
 int csp;
 void * plane[4];
 int stride[4];
} xvid_image_t;
# 188 "../xvid.h"
typedef struct {
 int version;
 unsigned int cpu_flags;
 int debug;
} xvid_gbl_init_t;



typedef struct {
 int version;
 int actual_version;
 const char * build;
 unsigned int cpu_flags;
 int num_threads;
} xvid_gbl_info_t;



typedef struct {
 int version;
 xvid_image_t input;
 xvid_image_t output;
 int width;
 int height;
 int interlacing;
} xvid_gbl_convert_t;






extern int xvid_global(void *handle, int opt, void *param1, void *param2);
# 231 "../xvid.h"
extern int xvid_decore(void *handle, int opt, void *param1, void *param2);




typedef struct {
 int version;
 int width;
 int height;
 void * handle;
} xvid_dec_create_t;
# 258 "../xvid.h"
typedef struct {
 int version;
 int general;
 void *bitstream;
 int length;
 xvid_image_t output;

 int brightness;
} xvid_dec_frame_t;



typedef struct
{
 int version;

 int type;
 union {
  struct {
   int general;
   int time_base;
   int time_increment;


   int * qscale;
   int qscale_stride;

  } vop;
  struct {
   int general;
   int width;
   int height;
   int par;
   int par_width;
   int par_height;
  } vol;
 } data;
} xvid_dec_stats_t;




typedef struct
{
 int frame;
 int mode;
 int increment;
 int base;
} xvid_enc_zone_t;
# 319 "../xvid.h"
typedef struct {
 int version;


 int type;
 int quant;
 int vol_flags;
 int vop_flags;


 int length;

 int hlength;
 int kblks;
 int mblks;
 int ublks;

 int sse_y;
 int sse_u;
 int sse_v;
} xvid_enc_stats_t;
# 364 "../xvid.h"
typedef struct
{
 int version;
 int flags;
} xvid_plg_info_t;


typedef struct
{
 int version;

 int num_zones;
 xvid_enc_zone_t * zones;

 int width;
 int height;
 int mb_width;
 int mb_height;
 int fincr;
 int fbase;

 void * param;
} xvid_plg_create_t;


typedef struct
{
 int version;

 int num_frames;
} xvid_plg_destroy_t;

typedef struct
{
 int version;

 xvid_enc_zone_t * zone;

 int width;
 int height;
 int mb_width;
 int mb_height;
 int fincr;
 int fbase;

 int min_quant[3];
 int max_quant[3];

 xvid_image_t reference;
 xvid_image_t current;
 xvid_image_t original;
 int frame_num;

 int type;
 int quant;

 int * dquant;
 int dquant_stride;

 int vop_flags;
 int vol_flags;
 int motion_flags;



 int length;
 int kblks;
 int mblks;
 int ublks;
 int sse_y;
 int sse_u;
 int sse_v;


 int bquant_ratio;
 int bquant_offset;

 xvid_enc_stats_t stats;
} xvid_plg_data_t;
# 464 "../xvid.h"
typedef int (xvid_plugin_func)(void * handle, int opt, void * param1, void * param2);

typedef struct
{
 xvid_plugin_func * func;
 void * param;
} xvid_enc_plugin_t;


extern xvid_plugin_func xvid_plugin_single;
extern xvid_plugin_func xvid_plugin_2pass1;
extern xvid_plugin_func xvid_plugin_2pass2;

extern xvid_plugin_func xvid_plugin_lumimasking;

extern xvid_plugin_func xvid_plugin_psnr;
extern xvid_plugin_func xvid_plugin_dump;




typedef struct
{
 int version;

 int bitrate;
 int reaction_delay_factor;
 int averaging_period;
 int buffer;
} xvid_plugin_single_t;


typedef struct {
 int version;

 char * filename;
} xvid_plugin_2pass1_t;





typedef struct {
 int version;

 int bitrate;
 char * filename;

 int keyframe_boost;
 int curve_compression_high;
 int curve_compression_low;
 int overflow_control_strength;
 int max_overflow_improvement;
 int max_overflow_degradation;

 int kfreduction;
 int kfthreshold;




 int container_frame_overhead;


 int vbv_size;
 int vbv_initial;
 int vbv_maxrate;
 int vbv_peakrate;

}xvid_plugin_2pass2_t;
# 553 "../xvid.h"
extern int xvid_encore(void *handle, int opt, void *param1, void *param2);
# 696 "../xvid.h"
typedef struct {
 int version;

 int profile;
 int width;
 int height;

 int num_zones;
 xvid_enc_zone_t * zones;

 int num_plugins;
 xvid_enc_plugin_t * plugins;

 int num_threads;
 int max_bframes;

 int global;


 int fincr;
 int fbase;



 int max_key_interval;

 int frame_drop_ratio;

 int bquant_ratio;
 int bquant_offset;

 int min_quant[3];
 int max_quant[3];


 void *handle;
} xvid_enc_create_t;
# 749 "../xvid.h"
typedef struct {
 int version;





 int vol_flags;
 unsigned char *quant_intra_matrix;
 unsigned char *quant_inter_matrix;

 int par;
 int par_width;
 int par_height;



 int fincr;
 int vop_flags;
 int motion;

 xvid_image_t input;

 int type;
 int quant;
 int bframe_threshold;

 void *bitstream;
 int length;

 int out_flags;
} xvid_enc_frame_t;
# 30 "../global.h" 2
# 1 "../portab.h" 1
# 69 "../portab.h"
# 1 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/inttypes.h" 1 3
# 16 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/inttypes.h" 3
# 1 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/stdint.h" 1 3
# 45 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/stdint.h" 3
typedef signed char int8_t ;
typedef unsigned char uint8_t ;




typedef signed char int_least8_t;
typedef unsigned char uint_least8_t;




typedef signed short int16_t;
typedef unsigned short uint16_t;
# 71 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/stdint.h" 3
typedef int16_t int_least16_t;
typedef uint16_t uint_least16_t;
# 83 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/stdint.h" 3
typedef signed long int32_t;
typedef unsigned long uint32_t;
# 101 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/stdint.h" 3
typedef int32_t int_least32_t;
typedef uint32_t uint_least32_t;
# 123 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/stdint.h" 3
typedef signed long long int64_t;
typedef unsigned long long uint64_t;
# 133 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/stdint.h" 3
typedef int64_t int_least64_t;
typedef uint64_t uint_least64_t;
# 163 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/stdint.h" 3
  typedef signed int int_fast8_t;
  typedef unsigned int uint_fast8_t;




  typedef signed int int_fast16_t;
  typedef unsigned int uint_fast16_t;




  typedef signed int int_fast32_t;
  typedef unsigned int uint_fast32_t;
# 217 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/stdint.h" 3
  typedef int_least64_t int_fast64_t;
  typedef uint_least64_t uint_fast64_t;







  typedef long long int intmax_t;
# 235 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/stdint.h" 3
  typedef long long unsigned int uintmax_t;
# 247 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/stdint.h" 3
typedef signed int intptr_t;
typedef unsigned int uintptr_t;
# 17 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/inttypes.h" 2 3

# 1 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/include/stddef.h" 1 3 4
# 326 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/include/stddef.h" 3 4
typedef unsigned int wchar_t;
# 19 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/inttypes.h" 2 3
# 270 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/inttypes.h" 3
typedef struct {
  intmax_t quot;
  intmax_t rem;
} imaxdiv_t;





extern intmax_t imaxabs(intmax_t j);
extern imaxdiv_t imaxdiv(intmax_t numer, intmax_t denomer);
extern intmax_t strtoimax(const char *__restrict, char **__restrict, int);
extern uintmax_t strtoumax(const char *__restrict, char **__restrict, int);
extern intmax_t wcstoimax(const wchar_t *__restrict, wchar_t **__restrict, int);
extern uintmax_t wcstoumax(const wchar_t *__restrict, wchar_t **__restrict, int);
# 70 "../portab.h" 2
# 229 "../portab.h"
static __inline void DPRINTF(int level, char *format, ...) {}
# 315 "../portab.h"
# 1 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/time.h" 1 3
# 18 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/time.h" 3
# 1 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/machine/time.h" 1 3
# 19 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/time.h" 2 3
# 27 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/time.h" 3
# 1 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/include/stddef.h" 1 3 4
# 28 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/time.h" 2 3

# 1 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/sys/types.h" 1 3
# 25 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/sys/types.h" 3
# 1 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/machine/_types.h" 1 3
# 22 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/machine/_types.h" 3
# 1 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/include/limits.h" 1 3 4
# 23 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/machine/_types.h" 2 3



typedef signed char __int8_t ;
typedef unsigned char __uint8_t ;
# 36 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/machine/_types.h" 3
typedef signed short __int16_t;
typedef unsigned short __uint16_t;
# 46 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/machine/_types.h" 3
typedef __int16_t __int_least16_t;
typedef __uint16_t __uint_least16_t;
# 58 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/machine/_types.h" 3
typedef signed int __int32_t;
typedef unsigned int __uint32_t;
# 76 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/machine/_types.h" 3
typedef __int32_t __int_least32_t;
typedef __uint32_t __uint_least32_t;
# 99 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/machine/_types.h" 3
typedef signed long long __int64_t;
typedef unsigned long long __uint64_t;
# 26 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/sys/types.h" 2 3
# 69 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/sys/types.h" 3
# 1 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/include/stddef.h" 1 3 4
# 152 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/include/stddef.h" 3 4
typedef int ptrdiff_t;
# 70 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/sys/types.h" 2 3
# 1 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/machine/types.h" 1 3
# 19 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/machine/types.h" 3
typedef long int __off_t;
typedef int __pid_t;

__extension__ typedef long long int __loff_t;
# 71 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/sys/types.h" 2 3
# 92 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/sys/types.h" 3
typedef unsigned char u_char;
typedef unsigned short u_short;
typedef unsigned int u_int;
typedef unsigned long u_long;



typedef unsigned short ushort;
typedef unsigned int uint;



typedef unsigned long clock_t;




typedef long time_t;




struct timespec {
  time_t tv_sec;
  long tv_nsec;
};

struct itimerspec {
  struct timespec it_interval;
  struct timespec it_value;
};


typedef long daddr_t;
typedef char * caddr_t;






typedef unsigned int ino_t;
# 166 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/sys/types.h" 3
typedef int dev_t;




typedef long off_t;

typedef unsigned short uid_t;
typedef unsigned short gid_t;


typedef int pid_t;

typedef long key_t;

typedef _ssize_t ssize_t;
# 195 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/sys/types.h" 3
typedef unsigned int mode_t __attribute__ ((__mode__ (__SI__)));




typedef unsigned short nlink_t;
# 222 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/sys/types.h" 3
typedef long fd_mask;







typedef struct _types_fd_set {
 fd_mask fds_bits[(((64)+(((sizeof (fd_mask) * 8))-1))/((sizeof (fd_mask) * 8)))];
} _types_fd_set;
# 253 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/sys/types.h" 3
typedef unsigned long clockid_t;




typedef unsigned long timer_t;



typedef unsigned long useconds_t;
typedef long suseconds_t;

typedef __uint32_t fsblkcnt_t;
typedef __uint32_t fsfilcnt_t;

# 1 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/sys/features.h" 1 3
# 269 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/sys/types.h" 2 3
# 30 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/time.h" 2 3



struct tm
{
  int tm_sec;
  int tm_min;
  int tm_hour;
  int tm_mday;
  int tm_mon;
  int tm_year;
  int tm_wday;
  int tm_yday;
  int tm_isdst;
};

clock_t clock (void);
double difftime (time_t _time2, time_t _time1);
time_t mktime (struct tm *_timeptr);
time_t time (time_t *_timer);

char *asctime (const struct tm *_tblock);
char *ctime (const time_t *_time);
struct tm *gmtime (const time_t *_timer);
struct tm *localtime (const time_t *_timer);

size_t strftime (char *_s, size_t _maxsize, const char *_fmt, const struct tm *_t);

char *asctime_r (const struct tm *, char *);
char *ctime_r (const time_t *, char *);
struct tm *gmtime_r (const time_t *, struct tm *);
struct tm *localtime_r (const time_t *, struct tm *);








char *strptime (const char *, const char *, struct tm *);
void tzset (void);
void _tzset_r (struct _reent *);

typedef struct __tzrule_struct
{
  char ch;
  int m;
  int n;
  int d;
  int s;
  time_t change;
  long offset;
} __tzrule_type;

typedef struct __tzinfo_struct
{
  int __tznorth;
  int __tzyear;
  __tzrule_type __tzrule[2];
} __tzinfo_type;

__tzinfo_type *__gettzinfo (void);
# 118 "c:\\dev\\devkitpro\\devkitarm\\bin\\../lib/gcc/arm-eabi/4.1.2/../../../../arm-eabi/include/time.h" 3
extern long _timezone;
extern int _daylight;
extern char *_tzname[2];
# 316 "../portab.h" 2
static __inline int64_t read_counter(void)
{
 return (int64_t)clock();
}
# 31 "../global.h" 2
# 63 "../global.h"
static __inline int
coding2type(int coding_type)
{
 return coding_type + 1;
}


static __inline int
type2coding(int xvid_type)
{
 return xvid_type - 1;
}


typedef struct
{
 int x;
 int y;
}
VECTOR;



typedef struct
{
 VECTOR duv[3];
}
WARPPOINTS;
# 99 "../global.h"
typedef struct
{
 int num_wp;

 int s;

 int W;
 int H;

 int ss;
 int smask;
 int sigma;

 int r;
 int rho;

 int i0s;
 int j0s;
 int i1s;
 int j1s;
 int i2s;
 int j2s;

 int i1ss;
 int j1ss;
 int i2ss;
 int j2ss;

 int alpha;
 int beta;
 int Ws;
 int Hs;

 int dxF, dyF, dxG, dyG;
 int Fo, Go;
 int cFo, cGo;
} GMC_DATA;

typedef struct _NEW_GMC_DATA
{


 int num_wp;


 int accuracy;


 int sW, sH;


 int dU[2], dV[2], Uo, Vo, Uco, Vco;

 void (*predict_16x16)(const struct _NEW_GMC_DATA * const This,
        uint8_t *dst, const uint8_t *src,
        int dststride, int srcstride, int x, int y, int rounding);
 void (*predict_8x8) (const struct _NEW_GMC_DATA * const This,
        uint8_t *uDst, const uint8_t *uSrc,
        uint8_t *vDst, const uint8_t *vSrc,
        int dststride, int srcstride, int x, int y, int rounding);
 void (*get_average_mv)(const struct _NEW_GMC_DATA * const Dsp, VECTOR * const mv,
         int x, int y, int qpel);
} NEW_GMC_DATA;

typedef struct
{
 uint8_t *y;
 uint8_t *u;
 uint8_t *v;
}
IMAGE;


typedef struct
{
 uint32_t bufa;
 uint32_t bufb;
 uint32_t buf;
 uint32_t pos;
 uint32_t *tail;
 uint32_t *start;
 uint32_t length;
 uint32_t initpos;
}
Bitstream;





typedef struct
{

 VECTOR mvs[4];

 short int pred_values[6][15];
 int acpred_directions[6];

 int mode;
 int quant;

 int field_dct;
 int field_pred;
 int field_for_top;
 int field_for_bot;



 VECTOR pmvs[4];
 VECTOR qmvs[4];

 int32_t sad8[4];
 int32_t sad16;

 int dquant;
 int cbp;



 VECTOR b_mvs[4];
 VECTOR b_qmvs[4];

 VECTOR amv;
 int32_t mcsel;

 VECTOR mvs_avg;



}
MACROBLOCK;

static __inline uint32_t
get_dc_scaler(uint32_t quant,
     uint32_t lum)
{
 if (quant < 5)
  return 8;

 if (quant < 25 && !lum)
  return (quant + 13) / 2;

 if (quant < 9)
  return 2 * quant;

 if (quant < 25)
  return quant + 8;

 if (lum)
  return 2 * quant - 16;
 else
  return quant - 6;
}
# 29 "colorspace.c" 2
# 1 "colorspace.h" 1
# 29 "colorspace.h"
# 1 "../portab.h" 1
# 30 "colorspace.h" 2



void colorspace_init(void);





typedef void (packedFunc) (uint8_t * x_ptr,
         int x_stride,
         uint8_t * y_src,
         uint8_t * v_src,
         uint8_t * u_src,
         int y_stride,
         int uv_stride,
         int width,
         int height,
         int vflip);

typedef packedFunc *packedFuncPtr;






extern packedFuncPtr yv12_to_rgb555;
extern packedFuncPtr yv12_to_rgb565;
extern packedFuncPtr yv12_to_bgr;
extern packedFuncPtr yv12_to_bgra;
extern packedFuncPtr yv12_to_abgr;
extern packedFuncPtr yv12_to_rgba;
extern packedFuncPtr yv12_to_argb;
extern packedFuncPtr yv12_to_yuyv;
extern packedFuncPtr yv12_to_uyvy;

extern packedFuncPtr yv12_to_rgb555i;
extern packedFuncPtr yv12_to_rgb565i;
extern packedFuncPtr yv12_to_bgri;
extern packedFuncPtr yv12_to_bgrai;
extern packedFuncPtr yv12_to_abgri;
extern packedFuncPtr yv12_to_rgbai;
extern packedFuncPtr yv12_to_argbi;
extern packedFuncPtr yv12_to_yuyvi;
extern packedFuncPtr yv12_to_uyvyi;


packedFunc yv12_to_rgb555_c;

packedFunc yv12_to_rgb555i_c;


typedef void (planarFunc) (
    uint8_t * y_dst, uint8_t * u_dst, uint8_t * v_dst,
    int y_dst_stride, int uv_dst_stride,
    uint8_t * y_src, uint8_t * u_src, uint8_t * v_src,
    int y_src_stride, int uv_src_stride,
    int width, int height, int vflip);
typedef planarFunc *planarFuncPtr;

extern planarFuncPtr yv12_to_yv12;

planarFunc yv12_to_yv12_c;
# 30 "colorspace.c" 2




packedFuncPtr rgb555_to_yv12;
packedFuncPtr rgb565_to_yv12;
packedFuncPtr bgr_to_yv12;
packedFuncPtr bgra_to_yv12;
packedFuncPtr abgr_to_yv12;
packedFuncPtr rgba_to_yv12;
packedFuncPtr argb_to_yv12;
packedFuncPtr yuyv_to_yv12;
packedFuncPtr uyvy_to_yv12;

packedFuncPtr rgb555i_to_yv12;
packedFuncPtr rgb565i_to_yv12;
packedFuncPtr bgri_to_yv12;
packedFuncPtr bgrai_to_yv12;
packedFuncPtr abgri_to_yv12;
packedFuncPtr rgbai_to_yv12;
packedFuncPtr argbi_to_yv12;
packedFuncPtr yuyvi_to_yv12;
packedFuncPtr uyvyi_to_yv12;


packedFuncPtr yv12_to_rgb555;
packedFuncPtr yv12_to_rgb565;
packedFuncPtr yv12_to_bgr;
packedFuncPtr yv12_to_bgra;
packedFuncPtr yv12_to_abgr;
packedFuncPtr yv12_to_rgba;
packedFuncPtr yv12_to_argb;
packedFuncPtr yv12_to_yuyv;
packedFuncPtr yv12_to_uyvy;

packedFuncPtr yv12_to_rgb555i;
packedFuncPtr yv12_to_rgb565i;
packedFuncPtr yv12_to_bgri;
packedFuncPtr yv12_to_bgrai;
packedFuncPtr yv12_to_abgri;
packedFuncPtr yv12_to_rgbai;
packedFuncPtr yv12_to_argbi;
packedFuncPtr yv12_to_yuyvi;
packedFuncPtr yv12_to_uyvyi;

planarFuncPtr yv12_to_yv12;


static int32_t RGB_Y_tab[256];
static int32_t B_U_tab[256];
static int32_t G_U_tab[256];
static int32_t G_V_tab[256];
static int32_t R_V_tab[256];
# 259 "colorspace.c"
void yv12_to_rgb555_c
(uint8_t * x_ptr, int x_stride,
 uint8_t * y_ptr, uint8_t * u_ptr, uint8_t * v_ptr,
 int y_stride, int uv_stride,
 int width, int height, int vflip) 
{
	int fixed_width = (width + 1) & ~1;
	int x_dif = x_stride - (2)*fixed_width;
	int y_dif = y_stride - fixed_width;
	int uv_dif = uv_stride - (fixed_width / 2);
	int x, y;
	if (vflip) {
		x_ptr += (height - 1) * x_stride;
		x_dif = -(2)*fixed_width - x_stride;
		x_stride = -x_stride;
	}
	for (y = 0; y < height; y+=(2)) {
		int r[2], g[2], b[2];
		r[0] = r[1] = g[0] = g[1] = b[0] = b[1] = 0;

		for (x = 0; x < fixed_width; x+=(2)) {
			int rgb_y;
			int b_u0 = B_U_tab[ u_ptr[0] ];
			int g_uv0 = G_U_tab[ u_ptr[0] ] + G_V_tab[ v_ptr[0] ];
			int r_v0 = R_V_tab[ v_ptr[0] ];

			rgb_y = RGB_Y_tab[ y_ptr[y_stride*(0) + 0] ];
			b[0] = (b[0] & 0x7) + ((rgb_y + b_u0) >> 13);
			g[0] = (g[0] & 0x7) + ((rgb_y - g_uv0) >> 13);
			r[0] = (r[0] & 0x7) + ((rgb_y + r_v0) >> 13);
			*(uint16_t *) (x_ptr+((0)*x_stride)+0) =
			((((0)>(((255)<(b[0])?(255):(b[0])))?(0):(((255)<(b[0])?(255):(b[0])))) << 7) & 0x7c00) | 
			((((0)>(((255)<(g[0])?(255):(g[0])))?(0):(((255)<(g[0])?(255):(g[0])))) << 2) & 0x03e0) | 
			((((0)>(((255)<(r[0])?(255):(r[0])))?(0):(((255)<(r[0])?(255):(r[0])))) >> 3) & 0x001f);
			
			rgb_y = RGB_Y_tab[ y_ptr[y_stride*(0) + 1] ];
			b[0] = (b[0] & 0x7) + ((rgb_y + b_u0) >> 13);
			g[0] = (g[0] & 0x7) + ((rgb_y - g_uv0) >> 13);
			r[0] = (r[0] & 0x7) + ((rgb_y + r_v0) >> 13);
			*(uint16_t *) (x_ptr+((0)*x_stride)+2) =
				((((0)>(((255)<(b[0])?(255):(b[0])))?(0):(((255)<(b[0])?(255):(b[0])))) << 7) & 0x7c00) |
				((((0)>(((255)<(g[0])?(255):(g[0])))?(0):(((255)<(g[0])?(255):(g[0])))) << 2) & 0x03e0) |
				((((0)>(((255)<(r[0])?(255):(r[0])))?(0):(((255)<(r[0])?(255):(r[0])))) >> 3) & 0x001f);
				
			rgb_y = RGB_Y_tab[ y_ptr[y_stride*(1) + 0] ];
			b[1] = (b[1] & 0x7) + ((rgb_y + b_u0) >> 13);
			g[1] = (g[1] & 0x7) + ((rgb_y - g_uv0) >> 13);
			r[1] = (r[1] & 0x7) + ((rgb_y + r_v0) >> 13);
			*(uint16_t *) (x_ptr+((1)*x_stride)+0) =
				((((0)>(((255)<(b[1])?(255):(b[1])))?(0):(((255)<(b[1])?(255):(b[1])))) << 7) & 0x7c00) |
				((((0)>(((255)<(g[1])?(255):(g[1])))?(0):(((255)<(g[1])?(255):(g[1])))) << 2) & 0x03e0) |
				((((0)>(((255)<(r[1])?(255):(r[1])))?(0):(((255)<(r[1])?(255):(r[1])))) >> 3) & 0x001f);
				
			rgb_y = RGB_Y_tab[ y_ptr[y_stride*(1) + 1] ];
			b[1] = (b[1] & 0x7) + ((rgb_y + b_u0) >> 13);
			g[1] = (g[1] & 0x7) + ((rgb_y - g_uv0) >> 13);
			r[1] = (r[1] & 0x7) + ((rgb_y + r_v0) >> 13);
			*(uint16_t *) (x_ptr+((1)*x_stride)+2) =
				((((0)>(((255)<(b[1])?(255):(b[1])))?(0):(((255)<(b[1])?(255):(b[1])))) << 7) & 0x7c00) |
				((((0)>(((255)<(g[1])?(255):(g[1])))?(0):(((255)<(g[1])?(255):(g[1])))) << 2) & 0x03e0) |
				((((0)>(((255)<(r[1])?(255):(r[1])))?(0):(((255)<(r[1])?(255):(r[1])))) >> 3) & 0x001f);
			x_ptr += (2)*(2);
			y_ptr += 2;
			u_ptr += 1;
			v_ptr += 1;
		}
		x_ptr += x_dif + x_stride;
		y_ptr += y_dif + y_stride;
		u_ptr += uv_dif + ((2/2)-1)*uv_stride;
		v_ptr += uv_dif + ((2/2)-1)*uv_stride;
	}
}

void yv12_to_rgb555i_c(uint8_t * x_ptr, int x_stride, uint8_t * y_ptr, uint8_t * u_ptr, uint8_t * v_ptr, int y_stride, int uv_stride, int width, int height, int vflip) { int fixed_width = (width + 1) & ~1; int x_dif = x_stride - (2)*fixed_width; int y_dif = y_stride - fixed_width; int uv_dif = uv_stride - (fixed_width / 2); int x, y; if (vflip) { x_ptr += (height - 1) * x_stride; x_dif = -(2)*fixed_width - x_stride; x_stride = -x_stride; } for (y = 0; y < height; y+=(4)) { int r[4], g[4], b[4]; r[0] = r[1] = r[2] = r[3] = 0; g[0] = g[1] = g[2] = g[3] = 0; b[0] = b[1] = b[2] = b[3] = 0;; for (x = 0; x < fixed_width; x+=(2)) { int rgb_y; int b_u0 = B_U_tab[ u_ptr[0] ]; int g_uv0 = G_U_tab[ u_ptr[0] ] + G_V_tab[ v_ptr[0] ]; int r_v0 = R_V_tab[ v_ptr[0] ]; int b_u1 = B_U_tab[ u_ptr[uv_stride] ]; int g_uv1 = G_U_tab[ u_ptr[uv_stride] ] + G_V_tab[ v_ptr[uv_stride] ]; int r_v1 = R_V_tab[ v_ptr[uv_stride] ]; rgb_y = RGB_Y_tab[ y_ptr[y_stride*(0) + 0] ]; b[0] = (b[0] & 0x7) + ((rgb_y + b_u0) >> 13); g[0] = (g[0] & 0x7) + ((rgb_y - g_uv0) >> 13); r[0] = (r[0] & 0x7) + ((rgb_y + r_v0) >> 13); *(uint16_t *) (x_ptr+((0)*x_stride)+0) = ((((0)>(((255)<(b[0])?(255):(b[0])))?(0):(((255)<(b[0])?(255):(b[0])))) << 7) & 0x7c00) | ((((0)>(((255)<(g[0])?(255):(g[0])))?(0):(((255)<(g[0])?(255):(g[0])))) << 2) & 0x03e0) | ((((0)>(((255)<(r[0])?(255):(r[0])))?(0):(((255)<(r[0])?(255):(r[0])))) >> 3) & 0x001f); rgb_y = RGB_Y_tab[ y_ptr[y_stride*(0) + 1] ]; b[0] = (b[0] & 0x7) + ((rgb_y + b_u0) >> 13); g[0] = (g[0] & 0x7) + ((rgb_y - g_uv0) >> 13); r[0] = (r[0] & 0x7) + ((rgb_y + r_v0) >> 13); *(uint16_t *) (x_ptr+((0)*x_stride)+2) = ((((0)>(((255)<(b[0])?(255):(b[0])))?(0):(((255)<(b[0])?(255):(b[0])))) << 7) & 0x7c00) | ((((0)>(((255)<(g[0])?(255):(g[0])))?(0):(((255)<(g[0])?(255):(g[0])))) << 2) & 0x03e0) | ((((0)>(((255)<(r[0])?(255):(r[0])))?(0):(((255)<(r[0])?(255):(r[0])))) >> 3) & 0x001f); rgb_y = RGB_Y_tab[ y_ptr[y_stride*(1) + 0] ]; b[1] = (b[1] & 0x7) + ((rgb_y + b_u1) >> 13); g[1] = (g[1] & 0x7) + ((rgb_y - g_uv1) >> 13); r[1] = (r[1] & 0x7) + ((rgb_y + r_v1) >> 13); *(uint16_t *) (x_ptr+((1)*x_stride)+0) = ((((0)>(((255)<(b[1])?(255):(b[1])))?(0):(((255)<(b[1])?(255):(b[1])))) << 7) & 0x7c00) | ((((0)>(((255)<(g[1])?(255):(g[1])))?(0):(((255)<(g[1])?(255):(g[1])))) << 2) & 0x03e0) | ((((0)>(((255)<(r[1])?(255):(r[1])))?(0):(((255)<(r[1])?(255):(r[1])))) >> 3) & 0x001f); rgb_y = RGB_Y_tab[ y_ptr[y_stride*(1) + 1] ]; b[1] = (b[1] & 0x7) + ((rgb_y + b_u1) >> 13); g[1] = (g[1] & 0x7) + ((rgb_y - g_uv1) >> 13); r[1] = (r[1] & 0x7) + ((rgb_y + r_v1) >> 13); *(uint16_t *) (x_ptr+((1)*x_stride)+2) = ((((0)>(((255)<(b[1])?(255):(b[1])))?(0):(((255)<(b[1])?(255):(b[1])))) << 7) & 0x7c00) | ((((0)>(((255)<(g[1])?(255):(g[1])))?(0):(((255)<(g[1])?(255):(g[1])))) << 2) & 0x03e0) | ((((0)>(((255)<(r[1])?(255):(r[1])))?(0):(((255)<(r[1])?(255):(r[1])))) >> 3) & 0x001f); rgb_y = RGB_Y_tab[ y_ptr[y_stride*(2) + 0] ]; b[2] = (b[2] & 0x7) + ((rgb_y + b_u0) >> 13); g[2] = (g[2] & 0x7) + ((rgb_y - g_uv0) >> 13); r[2] = (r[2] & 0x7) + ((rgb_y + r_v0) >> 13); *(uint16_t *) (x_ptr+((2)*x_stride)+0) = ((((0)>(((255)<(b[2])?(255):(b[2])))?(0):(((255)<(b[2])?(255):(b[2])))) << 7) & 0x7c00) | ((((0)>(((255)<(g[2])?(255):(g[2])))?(0):(((255)<(g[2])?(255):(g[2])))) << 2) & 0x03e0) | ((((0)>(((255)<(r[2])?(255):(r[2])))?(0):(((255)<(r[2])?(255):(r[2])))) >> 3) & 0x001f); rgb_y = RGB_Y_tab[ y_ptr[y_stride*(2) + 1] ]; b[2] = (b[2] & 0x7) + ((rgb_y + b_u0) >> 13); g[2] = (g[2] & 0x7) + ((rgb_y - g_uv0) >> 13); r[2] = (r[2] & 0x7) + ((rgb_y + r_v0) >> 13); *(uint16_t *) (x_ptr+((2)*x_stride)+2) = ((((0)>(((255)<(b[2])?(255):(b[2])))?(0):(((255)<(b[2])?(255):(b[2])))) << 7) & 0x7c00) | ((((0)>(((255)<(g[2])?(255):(g[2])))?(0):(((255)<(g[2])?(255):(g[2])))) << 2) & 0x03e0) | ((((0)>(((255)<(r[2])?(255):(r[2])))?(0):(((255)<(r[2])?(255):(r[2])))) >> 3) & 0x001f); rgb_y = RGB_Y_tab[ y_ptr[y_stride*(3) + 0] ]; b[3] = (b[3] & 0x7) + ((rgb_y + b_u1) >> 13); g[3] = (g[3] & 0x7) + ((rgb_y - g_uv1) >> 13); r[3] = (r[3] & 0x7) + ((rgb_y + r_v1) >> 13); *(uint16_t *) (x_ptr+((3)*x_stride)+0) = ((((0)>(((255)<(b[3])?(255):(b[3])))?(0):(((255)<(b[3])?(255):(b[3])))) << 7) & 0x7c00) | ((((0)>(((255)<(g[3])?(255):(g[3])))?(0):(((255)<(g[3])?(255):(g[3])))) << 2) & 0x03e0) | ((((0)>(((255)<(r[3])?(255):(r[3])))?(0):(((255)<(r[3])?(255):(r[3])))) >> 3) & 0x001f); rgb_y = RGB_Y_tab[ y_ptr[y_stride*(3) + 1] ]; b[3] = (b[3] & 0x7) + ((rgb_y + b_u1) >> 13); g[3] = (g[3] & 0x7) + ((rgb_y - g_uv1) >> 13); r[3] = (r[3] & 0x7) + ((rgb_y + r_v1) >> 13); *(uint16_t *) (x_ptr+((3)*x_stride)+2) = ((((0)>(((255)<(b[3])?(255):(b[3])))?(0):(((255)<(b[3])?(255):(b[3])))) << 7) & 0x7c00) | ((((0)>(((255)<(g[3])?(255):(g[3])))?(0):(((255)<(g[3])?(255):(g[3])))) << 2) & 0x03e0) | ((((0)>(((255)<(r[3])?(255):(r[3])))?(0):(((255)<(r[3])?(255):(r[3])))) >> 3) & 0x001f);; x_ptr += (2)*(2); y_ptr += (2); u_ptr += (2)/2; v_ptr += (2)/2; } x_ptr += x_dif + (4 -1)*x_stride; y_ptr += y_dif + (4 -1)*y_stride; u_ptr += uv_dif + ((4/2)-1)*uv_stride; v_ptr += uv_dif + ((4/2)-1)*uv_stride; } }


void
colorspace_init(void)
{
 int32_t i;

 for (i = 0; i < 256; i++) {
  RGB_Y_tab[i] = ((uint16_t) ((1.164) * (1L<<13) + 0.5)) * (i - 16);
  B_U_tab[i] = ((uint16_t) ((2.018) * (1L<<13) + 0.5)) * (i - 128);
  G_U_tab[i] = ((uint16_t) ((0.391) * (1L<<13) + 0.5)) * (i - 128);
  G_V_tab[i] = ((uint16_t) ((0.813) * (1L<<13) + 0.5)) * (i - 128);
  R_V_tab[i] = ((uint16_t) ((1.596) * (1L<<13) + 0.5)) * (i - 128);
 }
}
