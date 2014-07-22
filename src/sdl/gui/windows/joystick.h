/*
 * joystick.h
 *
 *  Created on: 03/nov/2011
 *      Author: fhorse
 */

#ifndef JOYSTICK_H_
#define JOYSTICK_H_

#include "input.h"

#define name_to_jsv(name) js_from_name(name, jsv_list, LENGTH(jsv_list))
#define name_to_jsn(name) js_from_name(name, jsn_list, LENGTH(jsn_list))
#define jsv_to_name(jsvl) js_to_name(jsvl, jsv_list, LENGTH(jsv_list))
#define jsn_to_name(jsvl) js_to_name(jsvl, jsn_list, LENGTH(jsn_list))

enum { X, Y, Z, R, U, V, POV, MAXAXIS };
enum {
	JOYSTICKID3 = JOYSTICKID2 + 1,
	JOYSTICKID4,
	JOYSTICKID5,
	JOYSTICKID6,
	JOYSTICKID7,
	JOYSTICKID8,
	JOYSTICKID9,
	JOYSTICKID10,
	JOYSTICKID11,
	JOYSTICKID12,
	JOYSTICKID13,
	JOYSTICKID14,
	JOYSTICKID15
};
enum {
	MINUS,
	CENTER = 0x7FFF,
	PLUS = 0xFFFF
};
enum { MAX_BUTTONS = 32 };

typedef struct {
	char dev[30];
	DBWORD id;
	BYTE present;
	BYTE open_try;
	BYTE clock;
	DWORD last_axis[MAXAXIS];
	DWORD last_buttons;
	JOYINFOEX joy_info;
	JOYCAPS joy_caps;
	BYTE (*input_decode_event)(BYTE mode, DBWORD event, BYTE type, _port *port);
} _js;
typedef struct {
	DBWORD value;
	char name[20];
} _js_element;

_js js[PORT_MAX];

static const _js_element jsn_list[] = {
	{ 0xFF,         "NULL"         },
	{ JOYSTICKID1,  "JOYSTICKID1"  },
	{ JOYSTICKID2,  "JOYSTICKID2"  },
	{ JOYSTICKID3,  "JOYSTICKID3"  },
	{ JOYSTICKID4,  "JOYSTICKID4"  },
	{ JOYSTICKID5,  "JOYSTICKID5"  },
	{ JOYSTICKID6,  "JOYSTICKID6"  },
	{ JOYSTICKID7,  "JOYSTICKID7"  },
	{ JOYSTICKID8,  "JOYSTICKID8"  },
	{ JOYSTICKID9,  "JOYSTICKID9"  },
	{ JOYSTICKID10, "JOYSTICKID10" },
	{ JOYSTICKID11, "JOYSTICKID11" },
	{ JOYSTICKID12, "JOYSTICKID12" },
	{ JOYSTICKID13, "JOYSTICKID13" },
	{ JOYSTICKID14, "JOYSTICKID14" },
	{ JOYSTICKID15, "JOYSTICKID15" }
};
static const _js_element jsv_list[] = {
	{ 0x000, "NULL"   },
	{ 0x001, "JA0MIN" }, { 0x002, "JA0PLS" },
	{ 0x003, "JA1MIN" }, { 0x004, "JA1PLS" },
	{ 0x005, "JA2MIN" }, { 0x006, "JA2PLS" },
	{ 0x007, "JA3MIN" }, { 0x008, "JA3PLS" },
	{ 0x009, "JA4MIN" }, { 0x00A, "JA4PLS" },
	{ 0x00B, "JA5MIN" }, { 0x00C, "JA5PLS" },
	{ 0x00D, "JA6MIN" }, { 0x00E, "JA6PLS" },
	{ 0x00F, "JA7MIN" }, { 0x010, "JA7PLS" },
	{ 0x011, "JA8MIN" }, { 0x012, "JA8PLS" },
	{ 0x013, "JA9MIN" }, { 0x014, "JA9PLS" },
	{ 0x100, "JPOVF"  }, { 0x101, "JPOVR"  },
	{ 0x102, "JPOVB"  }, { 0x103, "JPOVL"  },
	{ 0x400, "JB0"    }, { 0x401, "JB1"    },
	{ 0x402, "JB2"    }, { 0x403, "JB3"    },
	{ 0x404, "JB4"    }, { 0x405, "JB5"    },
	{ 0x406, "JB6"    }, { 0x407, "JB7"    },
	{ 0x408, "JB8"    }, { 0x409, "JB9"    },
	{ 0x40A, "JB10"   }, { 0x40B, "JB11"   },
	{ 0x40C, "JB12"   }, { 0x40D, "JB13"   },
	{ 0x40E, "JB14"   }, { 0x40F, "JB15"   },
	{ 0x410, "JB16"   }, { 0x411, "JB17"   },
	{ 0x412, "JB18"   }, { 0x413, "JB19"   },
	{ 0x414, "JB20"   }, { 0x415, "JB21"   },
	{ 0x416, "JB22"   }, { 0x417, "JB23"   },
	{ 0x418, "JB24"   }, { 0x419, "JB25"   },
	{ 0x41A, "JB26"   }, { 0x41B, "JB27"   },
	{ 0x41C, "JB28"   }, { 0x41D, "JB29"   },
	{ 0x41E, "JB30"   }, { 0x41F, "JB31"   }
};

void js_init(void);
void js_open(_js *joy);
void js_control(_js *joy, _port *port);
void js_close(_js *joy);
void js_quit(void);
BYTE js_is_connected(int dev);
char *js_to_name(const DBWORD val, const _js_element *list, const DBWORD length);
DBWORD js_from_name(const char *name, const _js_element *list, const DBWORD lenght);

#endif /* JOYSTICK_H_ */
