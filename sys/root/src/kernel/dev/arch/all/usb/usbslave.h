/*
The contents of this file are subject to the Mozilla Public License Version 1.1 
(the "License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS" basis, 
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License for the 
specific language governing rights and limitations under the License.

The Original Code is ______________________________________.

The Initial Developer of the Original Code is ________________________.
Portions created by ______________________ are Copyright (C) ______ _______________________.
All Rights Reserved.

Contributor(s): ______________________________________.

Alternatively, the contents of this file may be used under the terms of the eCos GPL license 
(the  [eCos GPL] License), in which case the provisions of [eCos GPL] License are applicable 
instead of those above. If you wish to allow use of your version of this file only under the
terms of the [eCos GPL] License and not to allow others to use your version of this file under 
the MPL, indicate your decision by deleting  the provisions above and replace 
them with the notice and other provisions required by the [eCos GPL] License. 
If you do not delete the provisions above, a recipient may use your version of this file under 
either the MPL or the [eCos GPL] License."
*/


/*============================================
| Compiler Directive
==============================================*/
#ifndef _USBSLAVE_H_
#define _USBSLAVE_H_
/*============================================
| Includes
==============================================*/
#include "usbcore.h"

/*============================================
| Declaration
==============================================*/
//This file comes from $(ECOS_REPOSITORY)/io/usb/common/current/include/usbs.h

typedef struct usbs_enumeration_data {
    usb_device_descriptor                 device;
    usb_qualifier_descriptor					qualifier;
    int                                   total_number_interfaces;
    int                                   total_number_endpoints;
    int                                   total_number_strings;
    const usb_configuration_descriptor*   configurations;
    const usb_interface_descriptor**      interfaces;
    const unsigned char**                 strings;
} __attribute__((packed)) usbs_enumeration_data;

// The current state of a USB device. This involves a bit to mark
// whether or not the device has been suspended, plus a state machine.
// On some hardware it may not be possible to distinguish between the
// detached, attached and powered states. If so then the initial state
// will be POWERED.

#define USBS_STATE_DETACHED             0x01
#define USBS_STATE_ATTACHED             0x02
#define USBS_STATE_POWERED              0x03
#define USBS_STATE_DEFAULT              0x04
#define USBS_STATE_ADDRESSED            0x05
#define USBS_STATE_CONFIGURED           0x06
#define USBS_STATE_MASK                 0x7F
#define USBS_STATE_SUSPENDED            (1 << 7)

// State changes. Application code or higher-level packages should
// install an appropriate state change function which will get
// invoked with details of the state change.
typedef enum {
    USBS_STATE_CHANGE_DETACHED          = 1,
    USBS_STATE_CHANGE_ATTACHED          = 2,
    USBS_STATE_CHANGE_POWERED           = 3,
    USBS_STATE_CHANGE_RESET             = 4,
    USBS_STATE_CHANGE_ADDRESSED         = 5,
    USBS_STATE_CHANGE_CONFIGURED        = 6,
    USBS_STATE_CHANGE_DECONFIGURED      = 7,
    USBS_STATE_CHANGE_SUSPENDED         = 8,
    USBS_STATE_CHANGE_RESUMED           = 9
} usbs_state_change;

typedef enum {
    USBS_CONTROL_RETURN_HANDLED = 0,
    USBS_CONTROL_RETURN_UNKNOWN = 1,
    USBS_CONTROL_RETURN_STALL   = 2
} usbs_control_return;

typedef struct usbs_control_endpoint {
	// The state is maintained by the USB code and should not be
	// modified by anything higher up.
	int                 state;

	// The enumeration data should be supplied by higher level code,
	// usually the application. Often this data will be constant.
	const usbs_enumeration_data* enumeration_data;

	// When a new control message arrives it will be in this buffer
	// where the appropriate callback functions can examine it. The
	// USB code will not modify the buffer unless a new control
	// message arrives. The control_buffer can also be re-used
	// by handlers to maintain some state information, e.g.
	// for coping with complicated IN requests, but this is only
	// allowed if they actually handle the request.
	unsigned char       control_buffer[8];

	// This callback will be invoked by the USB code following a
	// change in USB state, e.g. to SUSPENDED mode. Higher-level code
	// should install a suitable function. There is some callback data
	// as well. This gets passed explicitly to the callback function,
	// in addition to the control endpoint structure. The reason is
	// that the actual state change callback may be some sort of
	// multiplexer inside a multifunction peripheral, and this
	// multiplexer wants to invoke device-specific state change
	// functions. However in simple devices those device-specific
	// state change functions could be invoked directly.
	void                (*state_change_fn)(struct usbs_control_endpoint*, void*, usbs_state_change, int /* old state */);
	void*               state_change_data;
	// When a standard control message arrives, the device driver will
	// detect some requests such as SET_ADDRESS and handle it
	// internally. Otherwise if higher-level code has installed a
	// callback then that will be invoked. If the callback returns
	// UNKNOWN then the default handler usbs_handle_standard_control()
	// is used to process the request.
	usbs_control_return (*standard_control_fn)(struct usbs_control_endpoint*, void*);
	void*               standard_control_data;

	// These three callbacks are used for other types of control
	// messages. The generic USB code has no way of knowing what
	// such control messages are about.
	usbs_control_return (*class_control_fn)(struct usbs_control_endpoint*, void*);
	void*               class_control_data;
	usbs_control_return (*vendor_control_fn)(struct usbs_control_endpoint*, void*);
	void*               vendor_control_data;
	usbs_control_return (*reserved_control_fn)(struct usbs_control_endpoint*, void*);
	void*               reserved_control_data;

	//Callback for clear feature (NOW only use for MSC => Reset Recovery)
	usbs_control_return (*class_clear_feature_fn)(struct usbs_control_endpoint*, void*);

	// If a control operation involves transferring more data than
	// just the initial eight-byte packet, the following fields are
	// used to keep track of the current operation. The original
	// control request indicates the direction of the transfer (IN or
	// OUT) and a length field. For OUT this length is exact, for IN
	// it is an upper bound. The transfer operates mostly as per the
	// bulk protocol, but if the length requested is an exact multiple
	// of the control fifo size (typically eight bytes) then there
	// is no need for an empty packet at the end.
	//
	// For an OUT operation the control message handler should supply
	// a suitable buffer via the "buffer" field below. The only other
	// field of interest is the complete_fn which must be provided and
	// will be invoked once all the data has arrived. Alternatively
	// the OUT operation may get aborted if a new control message
	// arrives. The second argument is an error code -EPIPE or -EIO,
	// or zero to indicate success. The return code is used by the
	// device driver during the status phase.
	//
	// IN is more complicated and the defined interface makes it
	// possible to gather data from multiple locations, eliminating
	// the need for copying into large buffers in some circumstances.
	// Basically when an IN request arrives the device driver will
	// look at the buffer and buffer_size fields, extracting data from
	// there if possible. If the current buffer has been exhausted
	// then the the refill function will be invoked, and this can
	// reset the buffer and size fields to point somewhere else.
	// This continues until such time that there is no longer
	// a refill function and the current buffer is empty. The
	// refill function can use the refill_data and refill_index
	// to keep track of the current state. The control_buffer
	// fields are available as well. At the end of the transfer,
	// if a completion function has been supplied then it will
	// be invoked. The return code will be ignored.
	unsigned char*      buffer;
	int                 buffer_size;
	void                (*fill_buffer_fn)(struct usbs_control_endpoint*);
	void*               fill_data;
	int                 fill_index;
	usbs_control_return (*complete_fn)(struct usbs_control_endpoint*, int);
} usbs_control_endpoint;

//
typedef char bool_t;

//
//usbs_data_endpoint can be rx or tx
typedef struct usbs_data_endpoint {
    void                (*set_halted_fn)(int, bool_t);
    void                (*complete_fn)(void*, int);
    void*               complete_data;
    unsigned char*      buffer;
    int                 buffer_size;
    bool_t              halted;
} usbs_data_endpoint;

//
bool_t usbs_data_endpoint_halted(usbs_data_endpoint* endpoint);
void usbs_set_data_endpoint_halted(usbs_data_endpoint* endpoint, bool_t halted);
static void usbs_configuration_descriptor_refill(usbs_control_endpoint* endpoint);
usbs_control_return usbs_handle_standard_control(usbs_control_endpoint* endpoint);

//implement in hardware driver
void usbs_set_class_handler(usbs_control_return (*class_control_fn)(struct usbs_control_endpoint*, void*));
unsigned int usbs_get_state(void);
int usbs_timer_callback_isset_read(int desc);

//struct to pass complete function to an endpoint
typedef struct usbs_completefn_endpoint {
	void                (*complete_fn)(void*, int);
	unsigned int			epn;
}usbs_completefn_endpoint;

typedef struct usbs_data_endpoint_halted_req {
	int epn;
	int halted;
}usbs_data_endpoint_halted_req;
#endif /*_USBSLAVE_H_*/
