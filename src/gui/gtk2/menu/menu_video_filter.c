/*
 * menu_video_filter.c
 *
 *  Created on: 16/dic/2011
 *      Author: fhorse
 */

#include "menu_video_filter.h"
#include "gfx.h"
#include "opengl.h"
#include "cfg_file.h"

#if defined (__SUNPRO_C)
#pragma align 4 (filter_icon_inline)
#endif
#if defined (__GNUC__)
static const guint8 filter_icon_inline[] __attribute__ ((__aligned__ (4))) =
#else
static const guint8 filter_icon_inline[] =
#endif
{ ""
  /* Pixbuf magic (0x47646b50) */
  "GdkP"
  /* length: header (24) + pixel_data (579) */
  "\0\0\2["
  /* pixdata_type (0x2010002) */
  "\2\1\0\2"
  /* rowstride (64) */
  "\0\0\0@"
  /* width (16) */
  "\0\0\0\20"
  /* height (16) */
  "\0\0\0\20"
  /* pixel_data: */
  "\211\377\377\377\0\2\365\3051\11\365\305/f\213\377\377\377\0\10\365\305"
  "3\36\365\3053l\377\377\377\0\365\305-K\365\303+\317\377\377\377\0\365"
  "\301!!\363\301\35f\211\377\377\377\0\7\365\305/\322\365\303+\267\365"
  "\301'\264\367\316P\377\365\301\37u\363\277\31\360\363\275\25Z\211\377"
  "\377\377\0\7\365\303)~\367\320Z\377\370\327m\377\370\334}\377\366\315"
  "K\377\363\275\21\366\363\273\15\14\206\377\377\377\0\13\365\305-\30\365"
  "\303+\275\365\301%\377\367\316N\377\370\335\203\377\302\244N\377\317"
  "\261[\377\334\277h\377\366\312A\377\361\274\10\377\361\271\0\227\207"
  "\377\377\377\0\11\365\301\37~\366\317Q\377\302\245O\377\370\370\370\377"
  "\376\376\376\377\315\260Z\377\367\326j\377\361\267\0\325\361\267\0\37"
  "\206\377\377\377\0\12\363\301\33\25\363\277\27\267LLL\377\312\312\312"
  "\377\367\367\367\377\323\323\323\377\277\241K\377\370\331t\377\361\265"
  "\0\363\361\265\0""8\205\377\377\377\0\13\363\277\31\36\363\275\23\261"
  "OOO\377ggg\377\214\214\214\377\251\251\251\377\264\227A\377\370\331w"
  "\377\361\265\0\374\361\265\0\314\361\263\0\177\206\377\377\377\0\11N"
  "NN\377ggg\377888\377BBB\377MMM\377\365\314K\377\362\275\31\377\362\267"
  "\15\377\361\261\0\36\206\377\377\377\0\12NNN\377ggg\377777\377@@@\377"
  "\26\26\26\377\361\265\0\223\361\263\0\374\361\263\0""9\361\261\0\317"
  "\361\257\0i\205\377\377\377\0\13XXX\377\204\204\204\377777\377\77\77"
  "\77\377\25\25\25\377\377\377\377\0\361\263\0<\361\261\0\267\377\377\377"
  "\0\357\257\0\6\357\257\0H\204\377\377\377\0\5GGG\377\203\203\203\377"
  "777\377\77\77\77\377\25\25\25\377\203\377\377\377\0\1\357\257\0E\206"
  "\377\377\377\0\5CCC\377\201\201\201\377555\377>>>\377\25\25\25\377\212"
  "\377\377\377\0\5iii\377\336\336\336\377111\377EEE\377\25\25\25\377\213"
  "\377\377\377\0\4iii\377\311\311\311\377\225\225\225\377\26\26\26\377"
  "\214\377\377\377\0\3FFFo>>>\377\27\27\27\377\215\377\377\377\0"};

#if defined (__SUNPRO_C)
#pragma align 4 (scalex_icon_inline)
#endif
#if defined (__GNUC__)
static const guint8 scalex_icon_inline[] __attribute__ ((__aligned__ (4))) =
#else
static const guint8 scalex_icon_inline[] =
#endif
{ ""
  /* Pixbuf magic (0x47646b50) */
  "GdkP"
  /* length: header (24) + pixel_data (1015) */
  "\0\0\4\17"
  /* pixdata_type (0x2010002) */
  "\2\1\0\2"
  /* rowstride (64) */
  "\0\0\0@"
  /* width (16) */
  "\0\0\0\20"
  /* height (16) */
  "\0\0\0\20"
  /* pixel_data: */
  "\17\202]\0\377\226q\0\377\247\203\0\377\257\213\0\377\330\263e\377\306"
  "\241,\377\327\262g\377\301\237G\377.Nn\377\0\263\377\377\2\263\361\377"
  "\0\266\377\377\1\266\373\377\4\200\267\377\200R\0\377\202\222m\0\377"
  "N\233v\0\377\241~\0\377\262\217\0\377\331\263d\377\262\224R\377^X\256"
  "\377JI\260\377vv\304\377Sk\312\377)v\222\377\21\245\264\377\31\236\230"
  "\377\31CP\377\225l\0\377\222m\0\377\221l\0\377\233v\0\377\241~\0\377"
  "\262\217\0\377\332\264e\377qg\244\377GH\266\377HH\267\377AA\265\377\244"
  "\245\334\377Tb\265\377$\215l\377$\224m\377\"</\377\224l\0\377\222m\0"
  "\377\221l\0\377\234w\0\377\241}\0\377\277\233\40\377\324\261v\377ZV\256"
  "\377CC\266\377df\223\377\205w\200\377\210n\247\377\210\206f\377H\235"
  "`\377\"\224o\377!<1\377\224l\0\377\224o\0\377\221l\0\377\250\204\0\377"
  "\244\200\0\377\306\240-\377\314\252c\377ie\247\377\240\235\241\377\350"
  "\321g\377\312\211\206\377\377\270\266\377\377\324\250\377r\264R\377F"
  "\231}\3773\77""7\377\222l\0\377\221l\0\377\222m\0\377\244\200\0\377\253"
  "\210\0\377\320\237A\377\365\270\232\377\335\327\226\377\347\256\221\377"
  "\313\231\232\377\350\347\347\377\301\202\202\377\307\312\312\377Z\244"
  "\212\377\377\271\272\377\220^a\377\216i\0\377\202\222m\0\377.\244\200"
  "\0\377\253\210\0\377\330\237n\377\365\253\257\377\364\266\271\377\324"
  "\233\232\377\306\210\210\377\377\272\272\377\377\265\275\377\376\264"
  "\263\377\335\220\221\377\362\252\252\377\273\201\202\377\220k\0\377\221"
  "l\0\377\222m\0\377\244\200\0\377\255\210\0\377\260\211$\377m@5\377\275"
  "wy\377\376\274\274\377\324\262\262\377\355\253\252\377\377\251\270\377"
  "\364\270\267\377\220LO\377>L\77\377\"7-\377\224l\0\377\222n\0\377\222"
  "m\0\377\244\200\0\377\253\210\0\377\306\241-\377\323\257T\377T7\20\377"
  "\311\201\203\377\377\311\314\377\340\317\320\377\367\360\361\377\265"
  "\245\241\377\31uU\377$\234t\377!<1\377\224l\0\377\202\222m\0\377\16\244"
  "\200\0\377\254\210\0\377\305\240,\377\326\261U\377\224j3\377:\2164\377"
  "\210\261a\377\301\253\207\377\277\252\210\377Q\2239\377\211\250\225\377"
  "\32\221i\377\"=1\377\224l\0\377\202\222m\0\377U\244\200\0\377\254\204"
  "\0\377\304\241+\377\304\237K\377xaQ\377\377\377\377\377G\265F\377\0\261"
  "\0\377\0\233\0\377\377\273\301\377\377\377\377\377\221\311\266\377\35"
  ";.\377\224l\0\377\222m\0\377\225p\0\377\242\207\0\377\353Bh\377\342q"
  "R\377\207j\77\377\340\240\237\377\375\317\312\377\303\265r\377Ge\15\377"
  "\23%\0\377\203YU\377\356\267\267\377\350\277\273\377=E;\377\217m\0\377"
  "\223n\0\377\226r\0\377\215j\10\377\267\177\214\377\377\25\204\377\210"
  "\21""7\377\274qx\377\344d\206\377\375\17l\377\374\15f\377\355\0a\377"
  "\270\0F\377\177,9\377\270dn\377\364$x\377\303gK\377\222o\2\377\226q\0"
  "\377\240|\0\377\237|\2\377rKN\377\362\241\303\377\343\220\257\377\366"
  "\216\273\377\377\221\301\377\377\222\303\377\377\225\304\377\377\224"
  "\303\377\355\222\265\377\347\220\264\377\335\243\301\377\243|'\377\222"
  "n\1\377\226q\0\377\237z\0\377\271\225\3\377\341\271a\377G0,\377hn\306"
  "\377\250\232\200\377nM,\377cC\24\377)\24\12\37775/\377cb\275\377\201"
  "\266\252\377\26""5'\377\224l\0\377\222m\0\377\226q\0\377\237z\0\377\271"
  "\224\3\377\273\255`\3779~R\377\262\262\210\377\202\337\272q\377\10\317"
  "\252E\377GB*\377%\222n\377O\236\210\377.\232v\377!<1\377\224l\0\377\222"
  "m\0\377"};

#if defined (__SUNPRO_C)
#pragma align 4 (hqx_icon_inline)
#endif
#if defined (__GNUC__)
static const guint8 hqx_icon_inline[] __attribute__ ((__aligned__ (4))) =
#else
static const guint8 hqx_icon_inline[] =
#endif
{ ""
  /* Pixbuf magic (0x47646b50) */
  "GdkP"
  /* length: header (24) + pixel_data (511) */
  "\0\0\2\27"
  /* pixdata_type (0x2010002) */
  "\2\1\0\2"
  /* rowstride (64) */
  "\0\0\0@"
  /* width (16) */
  "\0\0\0\20"
  /* height (16) */
  "\0\0\0\20"
  /* pixel_data: */
  "\261\377\377\377\0\2\376xt[\376xt\355\202\376xt\360J\375ur\360\375ro"
  "\360\374mk\360\373ig\360\372dc\360\371^^\360\370XY\360\367ST\360\366"
  "LO\354\364FI\\\377\377\377\0\376xtV\376xt\364\377\271\270\377\377\306"
  "\306\377\377\321\321\377\377\333\333\377\377\326\326\377\375\300\277"
  "\377\354\245\245\377\356\242\242\377\362\240\240\377\357\231\232\377"
  "\343\220\220\377\343~\177\377\363>C\364\3628=O\376xt\314\377\235\233"
  "\377\377\263\263\377\377\253\253\377\377\325\325\377\377\320\320\377"
  "\377\255\255\377\375uu\377\343hh\377\346dd\377\356cc\377\353\\\\\377"
  "\315MM\377\330\210\210\377\333oq\377\36017\343\376xt/\376vr\312\376\211"
  "\206\377\377\245\245\377\377\341\341\377\377\377\377\377\377\301\301"
  "\377\375TT\377\346KK\377\351JJ\377\361JJ\377\336BB\377\327\205\205\377"
  "\336df\377\360/6\347\357)1V\377\377\377\0\375ro\21\374mk\266\375\200"
  "\177\377\377\261\261\377\377\347\347\377\377\317\317\377\376[[\377\354"
  "JJ\377\357KK\377\357HH\377\335\206\206\377\337ad\377\360.4\335\356(0"
  "0\203\377\377\377\0\14\373hf\16\372bb\265\374ww\377\377\307\307\377\377"
  "\324\324\377\376dd\377\360EE\377\364GG\377\353\212\212\377\340ad\377"
  "\357,3\335\356'.-\205\377\377\377\0\12\370[\\\16\367VW\265\372vw\377"
  "\377\324\324\377\376uu\377\364AA\377\370\213\213\377\342mo\377\357+2"
  "\351\356%--\207\377\377\377\0\10\366NP\16\365HK\272\371{}\377\376\250"
  "\250\377\373\211\211\377\356pr\377\357)1\367\356$,P\211\377\377\377\0"
  "\6\363@D\34\362:\77\333\371rt\377\367pr\377\356(0\367\355#+V\213\377"
  "\377\377\0\4\36028+\357,3\311\356'.\340\355!*Q\266\377\377\377\0"};

#if defined (__SUNPRO_C)
#pragma align 4 (ntsc_icon_inline)
#endif
#if defined (__GNUC__)
static const guint8 ntsc_icon_inline[] __attribute__ ((__aligned__ (4))) =
#else
static const guint8 ntsc_icon_inline[] =
#endif
{ ""
  /* Pixbuf magic (0x47646b50) */
  "GdkP"
  /* length: header (24) + pixel_data (893) */
  "\0\0\3\225"
  /* pixdata_type (0x2010002) */
  "\2\1\0\2"
  /* rowstride (64) */
  "\0\0\0@"
  /* width (16) */
  "\0\0\0\20"
  /* height (16) */
  "\0\0\0\20"
  /* pixel_data: */
  "\5\377\377\377\0]y\245\6\212\237\277\343\216\243\300\353]y\245\17\206"
  "\377\377\377\0\4FU}\17n\201\237\353at\225\3437Fa\6\203\377\377\377\0"
  "\4]y\245\6\212\237\277\343\216\243\300\353Yu\241\17\204\377\377\377\0"
  "\4BQw\17h|\232\353]r\220\3433BZ\6\205\377\377\377\0\4[x\243\6\205\232"
  "\270\343\205\231\267\353Nb\215\17\202\377\377\377\0\4\77Np\17cw\225\353"
  "Yl\214\3430\77V\6\207\377\377\377\0\10Pd\221\6w\212\252\342t\210\246"
  "\353APt\17;Jj\17at\221\353Vh\210\342.=T\6\205\377\377\377\0\2uuu\201"
  "uuu\374\202ttt\374\1sss\374\202\211\211\211\377\7\206\206\206\377\204"
  "\204\204\377mmm\374lll\374kkk\374hhh\374]]]\201\202\377\377\377\0\1t"
  "tt\376\205\230\314\373\377\10\227\313\373\377\224\312\373\377\222\310"
  "\372\377\202\277\367\377g\261\362\377^\256\362\377]\256\362\377LLL\376"
  "\202\377\377\377\0\3~~~\377\256\327\374\377\232\315\373\377\202\230\314"
  "\373\377\11\227\313\373\377\224\312\373\377\220\307\372\377x\271\365"
  "\377e\260\362\377c\257\363\377a\257\362\377_\256\362\377GGG\377\202\377"
  "\377\377\0\16}}}\377\266\333\374\377\262\331\374\377\234\316\373\377"
  "\227\313\373\377\224\312\373\377\214\304\370\377s\267\364\377k\263\363"
  "\377h\261\363\377f\261\363\377c\257\362\377a\257\363\377DDD\377\202\377"
  "\377\377\0\1xxx\377\202\261\330\374\377\13\255\326\374\377\230\314\373"
  "\377\214\304\370\377w\270\364\377r\265\364\377n\264\364\377k\263\364"
  "\377h\261\364\377e\260\363\377c\256\363\377AAA\377\202\377\377\377\0"
  "\16rrr\377\253\326\374\377\253\325\374\377\250\324\374\377\236\315\372"
  "\377}\274\365\377w\270\365\377s\266\364\377p\265\364\377l\263\364\377"
  "i\262\363\377g\260\363\377c\257\363\377>>>\377\202\377\377\377\0\16o"
  "oo\377\246\323\374\377\244\322\374\377\241\320\372\377\221\310\370\377"
  "\207\302\367\377x\271\366\377u\270\365\377p\265\365\377l\264\364\377"
  "j\262\364\377f\260\364\377c\257\364\377;;;\377\202\377\377\377\0\16k"
  "kk\377\237\320\373\377\236\316\373\377\225\312\371\377\216\306\370\377"
  "\211\303\370\377}\274\366\377t\267\366\377p\266\365\377m\264\365\377"
  "i\262\365\377e\260\365\377b\257\365\377999\377\202\377\377\377\0\16e"
  "ee\377\224\311\372\377\217\307\372\377\206\302\370\377\202\277\370\377"
  "\177\276\367\377z\273\367\377s\270\366\377p\265\366\377k\263\366\377"
  "h\262\366\377d\260\366\377`\257\365\377777\377\202\377\377\377\0\16l"
  "ll\377\204\204\204\377ccc\377\351<<\377[[[\377VVV\377\200\200\200\377"
  "NNN\377\200\200\200\377GGG\377\200\200\200\377AAA\377eee\377444\377\202"
  "\377\377\377\0\16ggg\377\177\177\177\377|||\377yyy\377vvv\377sss\377"
  "ppp\377mmm\377jjj\377hhh\377eee\377ccc\377aaa\377333\377\202\377\377"
  "\377\0\13QQQ\377;;;\377999\377777\377555\377444\377222\377000\377///"
  "\377---\377,,,\377\202+++\377\2***\377\377\377\377\0"};

#if defined (__SUNPRO_C)
#pragma align 4 (crt_icon_inline)
#endif
#if defined (__GNUC__)
static const guint8 crt_icon_inline[] __attribute__ ((__aligned__ (4))) =
#else
static const guint8 crt_icon_inline[] =
#endif
{ ""
  /* Pixbuf magic (0x47646b50) */
  "GdkP"
  /* length: header (24) + pixel_data (401) */
  "\0\0\1\251"
  /* pixdata_type (0x2010002) */
  "\2\1\0\2"
  /* rowstride (64) */
  "\0\0\0@"
  /* width (16) */
  "\0\0\0\20"
  /* height (16) */
  "\0\0\0\20"
  /* pixel_data: */
  "\321\377\377\377\0\2fF':\221k>\277\212\233q:\377\2\213a0\274X3\20\15"
  "\202\377\377\377\0\1LFC\250\211**+\377\203:::\377\1LF\77b\202\377\377"
  "\377\0\16:::\377**+\377YZ[\377\214\214\215\377\251\251\252\377\254\255"
  "\256\377\236\236\237\377\213\214\215\377ttu\377**+\377PQR\377UVW\377"
  "PQR\377:::\377\202\377\377\377\0\16:::\377**+\377\207\207\207\377\312"
  "\312\312\377\356\355\355\377\347\347\347\377\312\312\312\377\254\254"
  "\255\377\225\225\226\377**+\377POO\377VUU\377POO\377:::\377\202\377\377"
  "\377\0\16:::\377**+\377\212\212\212\377\312\312\312\377\355\354\354\377"
  "\345\344\344\377\306\306\306\377\247\246\246\377\220\217\220\377**+\377"
  "PPO\377NMM\377PPO\377:::\377\202\377\377\377\0\12:::\377**+\377\201\201"
  "\201\377\261\261\261\377\313\312\312\377\316\314\314\377\275\274\274"
  "\377\246\245\244\377\223\221\221\377**+\377\203\36\36\36\377\1:::\377"
  "\202\377\377\377\0\12:::\377**+\377ttt\377\232\232\232\377\255\254\254"
  "\377\266\265\265\377\257\257\256\377\241\240\240\377\221\220\217\377"
  "**+\377\203\36\36\36\377\1:::\377\202\377\377\377\0\1:::\377\211**+\377"
  "\203\24\24\24\377\1:::\377\202\377\377\377\0\1""888\235\214:::\377\1"
  "444^\241\377\377\377\0"};

#ifdef __SUNPRO_C
#pragma align 4 (xbrz_icon_inline)
#endif
#ifdef __GNUC__
static const guint8 xbrz_icon_inline[] __attribute__ ((__aligned__ (4))) =
#else
static const guint8 xbrz_icon_inline[] =
#endif
{ ""
  /* Pixbuf magic (0x47646b50) */
  "GdkP"
  /* length: header (24) + pixel_data (739) */
  "\0\0\2\373"
  /* pixdata_type (0x2010002) */
  "\2\1\0\2"
  /* rowstride (64) */
  "\0\0\0@"
  /* width (16) */
  "\0\0\0\20"
  /* height (16) */
  "\0\0\0\20"
  /* pixel_data: */
  "\207\0\0\0\0\2\325\221r\325\325\224z\322\215\0\0\0\0\4\312wOE\364\343"
  "\260\377\364\333\220\377\312uVB\214\0\0\0\0\4\324\220z\340\377\366k\377"
  "\377\352>\377\323\212`\340\213\0\0\0\0\6\313tR3\352\303\216\377\377\364"
  "P\377\377\360I\377\347\266U\377\312|`0\210\0\0\0\0\12\310p;\34\304e0"
  "4\314|e\325\377\373\206\377\376\344\77\377\377\345Q\377\377\352R\377"
  "\313yS\325\305f84\310p:\35\203\0\0\0\0\77\322\203W\226\320\206g\272\332"
  "\245\223\332\347\302\276\372\354\316\312\377\367\352\272\377\377\353"
  "A\377\376\344P\377\376\337P\377\377\342D\377\366\332\206\377\354\317"
  "\310\377\347\306\272\371\332\250\221\331\321\213i\273\322\206V\226\332"
  "\235}\377\377\377\305\377\377\377\221\377\377\372Z\377\377\365L\377\377"
  "\357G\377\375\344J\377\376\343P\377\376\340R\377\376\334M\377\377\342"
  "N\377\377\343T\377\377\342c\377\377\353\201\377\377\377\307\377\332\235"
  "y\377\315zST\344\266A\377\377\377\77\377\375\356H\377\375\346L\377\375"
  "\346P\377\376\347W\377\376\344W\377\376\341X\377\376\337X\377\376\333"
  "Y\377\376\324G\377\377\324\77\377\377\3338\377\340\2215\377\316|RT\0"
  "\0\0\0\310q`k\343\263X\377\377\377R\377\377\354\\\377\375\351O\377\375"
  "\340C\377\375\3310\377\375\317\26\377\375\306\7\377\375\270\0\377\377"
  "\267\0\377\377\273\0\377\313P\0\377\2512\30k\203\0\0\0\0\14\315}is\337"
  "\246K\377\377\351\27\377\374\326\10\377\373\315\0\377\375\305\0\377\374"
  "\303\0\377\375\276\0\377\375\273\0\377\377\277\1\377\331k\12\377\231"
  "\26\4s\205\0\0\0\0\12\245\40\22\261\374\335\2\377\375\322\0\377\374\314"
  "\0\377\374\310\1\377\374\303\1\377\375\275\1\377\377\272\2\377\377\263"
  "\2\377\235\35\15\257\206\0\0\0\0\12\263B)\274\377\354\21\377\374\317"
  "\0\377\374\312\2\377\375\310\1\377\376\303\1\377\375\272\1\377\376\266"
  "\3\377\377\274\4\377\263@\27\273\206\0\0\0\0\12\304b#\335\377\343\0\377"
  "\373\316\1\377\377\327\0\377\377\322\0\377\377\315\0\377\377\307\0\377"
  "\375\262\3\377\377\271\3\377\303O\20\332\205\0\0\0\0\14\240\31\4'\326"
  "\214%\377\377\361\2\377\377\343\0\377\364\262\3\377\275H\13\377\275J"
  "\13\377\366\243\4\377\377\304\1\377\377\303\2\377\325j\16\377\240$\14"
  "%\204\0\0\0\0\5\235\15\6H\355\303!\377\375\331\2\377\312^\10\377\242"
  "\36\16\236\202\0\0\0\0\5\242\"\16\236\312V\11\377\375\255\4\377\354\220"
  "\15\377\235\36\15F\204\0\0\0\0\4\250,\10>\275[\26\377\2604\15\242\242"
  "\40\17#\204\0\0\0\0\4\242'\16#\2602\15\242\275L\13\377\2500\15>\202\0"
  "\0\0\0"};

enum {
	MNO_FILTER,
	MSCALE2X,
	MSCALE3X,
	MSCALE4X,
	MHQ2X,
	MHQ3X,
	MHQ4X,
	MXBRZ2X,
	MXBRZ3X,
	MXBRZ4X,
	MRGBNTSC,
	MPHOSPHOR,
	MPHOSPHOR2,
	MSCANLINE,
	MDBL,
	MDARKROOM,
	MCRT,
	MCRTCURVE,
	MCRTNOCURVE,
	MRGBNTSCCOM,
	MRGBNTSCSVD,
	MRGBNTSCRGB,
	NUMCHKS
};

void set_filter(int filter);

static GtkWidget *check[NUMCHKS];

void menu_video_filter(GtkWidget *video, GtkAccelGroup *accel_group) {
	GtkWidget *menu[2], *filter, *scalex, *hqx, *xBRZx, *ntsc;

	menu[0] = gtk_menu_new();
	filter = gtk_image_menu_item_new_with_mnemonic("Fi_lter");

	gtk_menu_item_set_submenu(GTK_MENU_ITEM(filter), menu[0]);
	gtk_menu_shell_append(GTK_MENU_SHELL(video), filter);

	gw_image_from_inline(filter, filter_icon_inline);

	check[MNO_FILTER] = gtk_check_menu_item_new_with_mnemonic("_No Filter");
	check[MPHOSPHOR] = gtk_check_menu_item_new_with_mnemonic("_Phosphor");
	check[MPHOSPHOR2] = gtk_check_menu_item_new_with_mnemonic("P_hosphor2");
	check[MSCANLINE] = gtk_check_menu_item_new_with_mnemonic("S_canline");
	check[MDBL] = gtk_check_menu_item_new_with_mnemonic("_DBL");
	check[MDARKROOM] = gtk_check_menu_item_new_with_mnemonic("Dark R_oom");

	gtk_menu_shell_append(GTK_MENU_SHELL(menu[0]), check[MNO_FILTER]);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu[0]), check[MPHOSPHOR]);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu[0]), check[MPHOSPHOR2]);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu[0]), check[MSCANLINE]);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu[0]), check[MDBL]);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu[0]), check[MDARKROOM]);

	g_signal_connect_swapped(G_OBJECT(check[MNO_FILTER]), "activate",
			G_CALLBACK(set_filter), GINT_TO_POINTER(NO_FILTER));
	g_signal_connect_swapped(G_OBJECT(check[MPHOSPHOR]), "activate",
			G_CALLBACK(set_filter), GINT_TO_POINTER(PHOSPHOR));
	g_signal_connect_swapped(G_OBJECT(check[MPHOSPHOR2]), "activate",
			G_CALLBACK(set_filter), GINT_TO_POINTER(PHOSPHOR2));
	g_signal_connect_swapped(G_OBJECT(check[MSCANLINE]), "activate",
			G_CALLBACK(set_filter), GINT_TO_POINTER(SCANLINE));
	g_signal_connect_swapped(G_OBJECT(check[MDBL]), "activate",
			G_CALLBACK(set_filter), GINT_TO_POINTER(DBL));
	g_signal_connect_swapped(G_OBJECT(check[MDARKROOM]), "activate",
			G_CALLBACK(set_filter), GINT_TO_POINTER(DARK_ROOM));

	menu[1] = gtk_menu_new();
	check[MCRT] = gtk_image_menu_item_new_with_mnemonic("_CRT");

	gtk_menu_item_set_submenu(GTK_MENU_ITEM(check[MCRT]), menu[1]);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu[0]), check[MCRT]);

	gw_image_from_inline(check[MCRT], crt_icon_inline);

	check[MCRTCURVE] = gtk_check_menu_item_new_with_mnemonic("With _Curve");
	check[MCRTNOCURVE] = gtk_check_menu_item_new_with_mnemonic("_Without Curve");

	gtk_menu_shell_append(GTK_MENU_SHELL(menu[1]), check[MCRTCURVE]);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu[1]), check[MCRTNOCURVE]);

	g_signal_connect_swapped(G_OBJECT(check[MCRTCURVE]), "activate",
			G_CALLBACK(set_filter), GINT_TO_POINTER(CRT_CURVE));
	g_signal_connect_swapped(G_OBJECT(check[MCRTNOCURVE]), "activate",
			G_CALLBACK(set_filter), GINT_TO_POINTER(CRT_NO_CURVE));

	/* Settings/Video/Filters/Scalex */
	menu[1] = gtk_menu_new();
	scalex = gtk_image_menu_item_new_with_mnemonic("_Scalex");

	gtk_menu_item_set_submenu(GTK_MENU_ITEM(scalex), menu[1]);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu[0]), scalex);

	gw_image_from_inline(scalex, scalex_icon_inline);

	check[MSCALE2X] = gtk_check_menu_item_new_with_mnemonic("Scale_2X");
	check[MSCALE3X] = gtk_check_menu_item_new_with_mnemonic("Scale_3X");
	check[MSCALE4X] = gtk_check_menu_item_new_with_mnemonic("Scale_4X");

	gtk_menu_shell_append(GTK_MENU_SHELL(menu[1]), check[MSCALE2X]);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu[1]), check[MSCALE3X]);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu[1]), check[MSCALE4X]);

	g_signal_connect_swapped(G_OBJECT(check[MSCALE2X]), "activate",
			G_CALLBACK(set_filter), GINT_TO_POINTER(SCALE2X));
	g_signal_connect_swapped(G_OBJECT(check[MSCALE3X]), "activate",
			G_CALLBACK(set_filter), GINT_TO_POINTER(SCALE3X));
	g_signal_connect_swapped(G_OBJECT(check[MSCALE4X]), "activate",
			G_CALLBACK(set_filter), GINT_TO_POINTER(SCALE4X));

	/* Settings/Video/Filters/Hqx */
	menu[1] = gtk_menu_new();
	hqx = gtk_image_menu_item_new_with_mnemonic("_Hqx");

	gtk_menu_item_set_submenu(GTK_MENU_ITEM(hqx), menu[1]);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu[0]), hqx);

	gw_image_from_inline(hqx, hqx_icon_inline);

	check[MHQ2X] = gtk_check_menu_item_new_with_mnemonic("Hq_2X");
	check[MHQ3X] = gtk_check_menu_item_new_with_mnemonic("Hq_3X");
	check[MHQ4X] = gtk_check_menu_item_new_with_mnemonic("Hq_4X");

	gtk_menu_shell_append(GTK_MENU_SHELL(menu[1]), check[MHQ2X]);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu[1]), check[MHQ3X]);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu[1]), check[MHQ4X]);

	g_signal_connect_swapped(G_OBJECT(check[MHQ2X]), "activate",
			G_CALLBACK(set_filter), GINT_TO_POINTER(HQ2X));
	g_signal_connect_swapped(G_OBJECT(check[MHQ3X]), "activate",
			G_CALLBACK(set_filter), GINT_TO_POINTER(HQ3X));
	g_signal_connect_swapped(G_OBJECT(check[MHQ4X]), "activate",
			G_CALLBACK(set_filter), GINT_TO_POINTER(HQ4X));

	/* Settings/Video/Filters/xBRZ */
	menu[1] = gtk_menu_new();
	xBRZx = gtk_image_menu_item_new_with_mnemonic("_xBRZ");

	gtk_menu_item_set_submenu(GTK_MENU_ITEM(xBRZx), menu[1]);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu[0]), xBRZx);

	gw_image_from_inline(xBRZx, xbrz_icon_inline);

	check[MXBRZ2X] = gtk_check_menu_item_new_with_mnemonic("xBRZ _2X");
	check[MXBRZ3X] = gtk_check_menu_item_new_with_mnemonic("xBRZ _3X");
	check[MXBRZ4X] = gtk_check_menu_item_new_with_mnemonic("xBRZ _4X");

	gtk_menu_shell_append(GTK_MENU_SHELL(menu[1]), check[MXBRZ2X]);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu[1]), check[MXBRZ3X]);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu[1]), check[MXBRZ4X]);

	g_signal_connect_swapped(G_OBJECT(check[MXBRZ2X]), "activate",
			G_CALLBACK(set_filter), GINT_TO_POINTER(XBRZ2X));
	g_signal_connect_swapped(G_OBJECT(check[MXBRZ3X]), "activate",
			G_CALLBACK(set_filter), GINT_TO_POINTER(XBRZ3X));
	g_signal_connect_swapped(G_OBJECT(check[MXBRZ4X]), "activate",
			G_CALLBACK(set_filter), GINT_TO_POINTER(XBRZ4X));

	/* Settings/Video/Filters/NTSC */
	menu[1] = gtk_menu_new();
	ntsc = gtk_image_menu_item_new_with_mnemonic("N_TSC");

	gtk_menu_item_set_submenu(GTK_MENU_ITEM(ntsc), menu[1]);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu[0]), ntsc);

	gw_image_from_inline(ntsc, ntsc_icon_inline);

	check[MRGBNTSCCOM] = gtk_check_menu_item_new_with_mnemonic("_Composite");
	check[MRGBNTSCSVD] = gtk_check_menu_item_new_with_mnemonic("_S-Video");
	check[MRGBNTSCRGB] = gtk_check_menu_item_new_with_mnemonic("_RGB");

	gtk_menu_shell_append(GTK_MENU_SHELL(menu[1]), check[MRGBNTSCCOM]);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu[1]), check[MRGBNTSCSVD]);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu[1]), check[MRGBNTSCRGB]);

	g_signal_connect_swapped(G_OBJECT(check[MRGBNTSCCOM]), "activate",
			G_CALLBACK(set_filter), GINT_TO_POINTER(MRGBNTSCCOM));
	g_signal_connect_swapped(G_OBJECT(check[MRGBNTSCSVD]), "activate",
			G_CALLBACK(set_filter), GINT_TO_POINTER(MRGBNTSCSVD));
	g_signal_connect_swapped(G_OBJECT(check[MRGBNTSCRGB]), "activate",
			G_CALLBACK(set_filter), GINT_TO_POINTER(MRGBNTSCRGB));

}
void menu_video_filter_check(void) {
	int index = 0;

	if (gfx.bit_per_pixel < 32) {
		gtk_widget_set_sensitive(check[MHQ2X], FALSE);
		gtk_widget_set_sensitive(check[MHQ3X], FALSE);
		gtk_widget_set_sensitive(check[MHQ4X], FALSE);
		gtk_widget_set_sensitive(check[MXBRZ2X], FALSE);
		gtk_widget_set_sensitive(check[MXBRZ3X], FALSE);
		gtk_widget_set_sensitive(check[MXBRZ4X], FALSE);
	} else {
		gtk_widget_set_sensitive(check[MHQ2X], TRUE);
		gtk_widget_set_sensitive(check[MHQ3X], TRUE);
		gtk_widget_set_sensitive(check[MHQ4X], TRUE);
		gtk_widget_set_sensitive(check[MXBRZ2X], TRUE);
		gtk_widget_set_sensitive(check[MXBRZ3X], TRUE);
		gtk_widget_set_sensitive(check[MXBRZ4X], TRUE);
	}

	if (opengl.glsl.compliant && opengl.glsl.enabled) {
		gtk_widget_set_sensitive(check[MPHOSPHOR], TRUE);
		gtk_widget_set_sensitive(check[MPHOSPHOR2], TRUE);
		gtk_widget_set_sensitive(check[MSCANLINE], TRUE);
		gtk_widget_set_sensitive(check[MDBL], TRUE);
		gtk_widget_set_sensitive(check[MDARKROOM], TRUE);
		gtk_widget_set_sensitive(check[MCRT], TRUE);
	} else {
		gtk_widget_set_sensitive(check[MPHOSPHOR], FALSE);
		gtk_widget_set_sensitive(check[MPHOSPHOR2], FALSE);
		gtk_widget_set_sensitive(check[MSCANLINE], FALSE);
		gtk_widget_set_sensitive(check[MDBL], FALSE);
		gtk_widget_set_sensitive(check[MDARKROOM], FALSE);
		gtk_widget_set_sensitive(check[MCRT], FALSE);
	}

	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(check[MNO_FILTER]), FALSE);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(check[MSCALE2X]), FALSE);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(check[MSCALE3X]), FALSE);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(check[MSCALE4X]), FALSE);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(check[MHQ2X]), FALSE);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(check[MHQ3X]), FALSE);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(check[MHQ4X]), FALSE);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(check[MXBRZ2X]), FALSE);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(check[MXBRZ3X]), FALSE);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(check[MXBRZ4X]), FALSE);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(check[MRGBNTSCCOM]), FALSE);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(check[MRGBNTSCSVD]), FALSE);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(check[MRGBNTSCRGB]), FALSE);

	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(check[MPHOSPHOR]), FALSE);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(check[MPHOSPHOR2]), FALSE);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(check[MSCANLINE]), FALSE);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(check[MDBL]), FALSE);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(check[MDARKROOM]), FALSE);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(check[MCRTCURVE]), FALSE);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(check[MCRTNOCURVE]), FALSE);

	switch (cfg->filter) {
		case NO_FILTER:
			index = MNO_FILTER;
			break;
		case PHOSPHOR:
			index = MPHOSPHOR;
			break;
		case PHOSPHOR2:
			index = MPHOSPHOR2;
			break;
		case SCANLINE:
			index = MSCANLINE;
			break;
		case DBL:
			index = MDBL;
			break;
		case DARK_ROOM:
			index = MDARKROOM;
			break;
		case CRT_CURVE:
			index = MCRTCURVE;
			break;
		case CRT_NO_CURVE:
			index = MCRTNOCURVE;
			break;
		case SCALE2X:
			index = MSCALE2X;
			break;
		case SCALE3X:
			index = MSCALE3X;
			break;
		case SCALE4X:
			index = MSCALE4X;
			break;
		case HQ2X:
			index = MHQ2X;
			break;
		case HQ3X:
			index = MHQ3X;
			break;
		case HQ4X:
			index = MHQ4X;
			break;
		case XBRZ2X:
			index = MXBRZ2X;
			break;
		case XBRZ3X:
			index = MXBRZ3X;
			break;
		case XBRZ4X:
			index = MXBRZ4X;
			break;
		case NTSC_FILTER: {
			switch (cfg->ntsc_format) {
				case COMPOSITE:
					index = MRGBNTSCCOM;
					break;
				case SVIDEO:
					index = MRGBNTSCSVD;
					break;
				case RGBMODE:
					index = MRGBNTSCRGB;
					break;
			}
			break;
		}
	}
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(check[index]), TRUE);
}

void set_filter(int filter) {
	if (gui_in_update) {
		return;
	}

	switch (filter) {
		case NO_FILTER:
			gfx_set_screen(NO_CHANGE, NO_FILTER, NO_CHANGE, NO_CHANGE, FALSE, FALSE);
			return;
		case PHOSPHOR:
			gfx_set_screen(NO_CHANGE, PHOSPHOR, NO_CHANGE, NO_CHANGE, FALSE, FALSE);
			return;
		case PHOSPHOR2:
			gfx_set_screen(NO_CHANGE, PHOSPHOR2, NO_CHANGE, NO_CHANGE, FALSE, FALSE);
			return;
		case SCANLINE:
			gfx_set_screen(NO_CHANGE, SCANLINE, NO_CHANGE, NO_CHANGE, FALSE, FALSE);
			return;
		case DBL:
			gfx_set_screen(NO_CHANGE, DBL, NO_CHANGE, NO_CHANGE, FALSE, FALSE);
			return;
		case DARK_ROOM:
			gfx_set_screen(NO_CHANGE, DARK_ROOM, NO_CHANGE, NO_CHANGE, FALSE, FALSE);
			return;
		case CRT_CURVE:
			gfx_set_screen(NO_CHANGE, CRT_CURVE, NO_CHANGE, NO_CHANGE, FALSE, FALSE);
			return;
		case CRT_NO_CURVE:
			gfx_set_screen(NO_CHANGE, CRT_NO_CURVE, NO_CHANGE, NO_CHANGE, FALSE, FALSE);
			return;
		case SCALE2X:
			gfx_set_screen(X2, SCALE2X, NO_CHANGE, NO_CHANGE, FALSE, FALSE);
			return;
		case SCALE3X:
			gfx_set_screen(X3, SCALE3X, NO_CHANGE, NO_CHANGE, FALSE, FALSE);
			return;
		case SCALE4X:
			gfx_set_screen(X4, SCALE4X, NO_CHANGE, NO_CHANGE, FALSE, FALSE);
			return;
		case HQ2X:
			gfx_set_screen(X2, HQ2X, NO_CHANGE, NO_CHANGE, FALSE, FALSE);
			return;
		case HQ3X:
			gfx_set_screen(X3, HQ3X, NO_CHANGE, NO_CHANGE, FALSE, FALSE);
			return;
		case HQ4X:
			gfx_set_screen(X4, HQ4X, NO_CHANGE, NO_CHANGE, FALSE, FALSE);
			return;
		case XBRZ2X:
			gfx_set_screen(X2, XBRZ2X, NO_CHANGE, NO_CHANGE, FALSE, FALSE);
			return;
		case XBRZ3X:
			gfx_set_screen(X3, XBRZ3X, NO_CHANGE, NO_CHANGE, FALSE, FALSE);
			return;
		case XBRZ4X:
			gfx_set_screen(X4, XBRZ4X, NO_CHANGE, NO_CHANGE, FALSE, FALSE);
			return;
		case MRGBNTSCCOM:
		case MRGBNTSCSVD:
		case MRGBNTSCRGB:
			gfx_set_screen(NO_CHANGE, NTSC_FILTER, NO_CHANGE, NO_CHANGE, FALSE, FALSE);
			/*
			 * faccio il resto solo se lo switch del nuovo effetto
			 * e' stato effettuato con successo.
			 */
			if (cfg->filter == NTSC_FILTER) {
				switch (filter) {
					case MRGBNTSCCOM:
						cfg->ntsc_format = COMPOSITE;
						break;
					case MRGBNTSCSVD:
						cfg->ntsc_format = SVIDEO;
						break;
					case MRGBNTSCRGB:
						cfg->ntsc_format = RGBMODE;
						break;
				}
				ntsc_set(cfg->ntsc_format, 0, 0, (BYTE *) palette_RGB, 0);
				gui_update();
			}
			return;
	}
}