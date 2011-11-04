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
| Includes
==============================================*/
#include "usbslave.h"

#define  GET_INTERFACE_ENDPOINT_NUMBER(endpoint,interface_no) \
   endpoint->enumeration_data->interfaces[interface_no][INTERFACE_NUMEP_OFFSET]

#define  GET_INTERFACE_INTERFACE_ID(endpoint,interface_no) \
   endpoint->enumeration_data->interfaces[interface_no][INTERFACE_INTERFACEID_OFFSET]

#define GET_INTERFACE_ALTERNATE_SETTINGS(endpoint,interface_no) \
   endpoint->enumeration_data->interfaces[interface_no][INTERFACE_ALTERSETTING_OFFSET]

/*============================================
| Global Declaration
==============================================*/

static void usbs_configuration_descriptor_refill(usbs_control_endpoint* endpoint);

/*============================================
| Implementation
==============================================*/
/*-------------------------------------------
| Name:usbs_data_endpoint_halted
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
bool_t usbs_data_endpoint_halted(usbs_data_endpoint* endpoint) {
   return endpoint->halted;
}

/*-------------------------------------------
| Name:usbs_set_data_endpoint_halted
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
/*void usbs_set_data_endpoint_halted(usbs_data_endpoint* endpoint, bool_t halted) {
    (*endpoint->set_halted_fn)(endpoint, halted);
}
*/
/*-------------------------------------------
| Name:usbs_configuration_descriptor_refill
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
static void usbs_configuration_descriptor_refill(usbs_control_endpoint* endpoint) {
   usb_devreq* req                 = (usb_devreq*) endpoint->control_buffer;
   
   //all interfaces, (crap) functionnal headers and endpoints are contigous so just fill endpoint buffer
   endpoint->buffer = (unsigned char*)endpoint->enumeration_data->interfaces[0];

   //req->value_lo contain the request configuration
   endpoint->buffer_size = ((endpoint->enumeration_data->configurations[req->value_lo].total_length_hi << 8)  |
         endpoint->enumeration_data->configurations[req->value_lo].total_length_lo)
         - USB_CONFIGURATION_DESCRIPTOR_LENGTH;
   endpoint->fill_buffer_fn    = (void (*)(usbs_control_endpoint*)) 0;
}

/*-------------------------------------------
| Name:usbs_handle_standard_control
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
usbs_control_return usbs_handle_standard_control(usbs_control_endpoint* endpoint) {
   usbs_control_return result = USBS_CONTROL_RETURN_UNKNOWN;
   usb_devreq*         req    = (usb_devreq*) endpoint->control_buffer;
   int                 length;
   int                 direction;
   int                 recipient;

   length      = (req->length_hi << 8) | req->length_lo;
   direction   = req->type & USB_DEVREQ_DIRECTION_MASK;
   recipient   = req->type & USB_DEVREQ_RECIPIENT_MASK;

   //CLEAR_FEATURE host request
   if (USB_DEVREQ_CLEAR_FEATURE == req->request) {
      if (USB_DEVREQ_RECIPIENT_INTERFACE == recipient) {
         // The host should expect no data back, the device must
         // be configured, and there are no defined features to clear.
         if ((0 == length) &&
               (USBS_STATE_CONFIGURED == (endpoint->state & USBS_STATE_MASK)) &&
               (0 == req->value_lo)) {
            int interface_id = req->index_lo;
            //
            //if (interface_id == endpoint->enumeration_data->interfaces[0].interface_id) {
            if (interface_id == GET_INTERFACE_INTERFACE_ID(endpoint,0)) {
               result = USBS_CONTROL_RETURN_HANDLED;
            }
            else {
               result = USBS_CONTROL_RETURN_STALL;
            }
         }
         else {
            result = USBS_CONTROL_RETURN_STALL;
         }
      }
   }
   //GET_CONFIGURATION host request
   else if (USB_DEVREQ_GET_CONFIGURATION == req->request) {

      // Return a single byte 0 if the device is not currently
      // configured. Otherwise assume a single configuration
      // in the enumeration data and return its id.
      if ((1 == length) && (USB_DEVREQ_DIRECTION_IN == direction)) {

         if (USBS_STATE_CONFIGURED == (endpoint->state & USBS_STATE_MASK)) {
            endpoint->control_buffer[0] = endpoint->enumeration_data->configurations[0].configuration_id;
         }
         else {
            endpoint->control_buffer[0] = 0;
         }
         endpoint->buffer            = endpoint->control_buffer;
         endpoint->buffer_size       = 1;
         endpoint->fill_buffer_fn    = (void (*)(usbs_control_endpoint*)) 0;
         endpoint->complete_fn       = (usbs_control_return (*)(struct usbs_control_endpoint*, int)) 0;
         result = USBS_CONTROL_RETURN_HANDLED;

      }
      else {
         result = USBS_CONTROL_RETURN_STALL;
      }
   }
   //GET_DESCRIPTOR host request
   else if (USB_DEVREQ_GET_DESCRIPTOR == req->request) {

      // The descriptor type is in value_hi. The descriptor index
      // is in value_lo.
      // The hsot must expect at least one byte of data.
      if ((0 == length) || (USB_DEVREQ_DIRECTION_IN != direction)) {
         result = USBS_CONTROL_RETURN_STALL;
      }
      else if (USB_DEVREQ_DESCRIPTOR_TYPE_DEVICE == req->value_hi) {

         // The device descriptor is easy, it is a single field in the
         // enumeration data.
         endpoint->buffer            = (unsigned char*) &(endpoint->enumeration_data->device);
         endpoint->fill_buffer_fn    = (void (*)(usbs_control_endpoint*)) 0;
         endpoint->complete_fn       = (usbs_control_return (*)(usbs_control_endpoint*, int)) 0;
         if (length < USB_DEVICE_DESCRIPTOR_LENGTH) {
            endpoint->buffer_size = length;
         }
         else {
            endpoint->buffer_size = USB_DEVICE_DESCRIPTOR_LENGTH;
         }
         result = USBS_CONTROL_RETURN_HANDLED;

      }
      else if (USB_DEVREQ_DESCRIPTOR_TYPE_CONFIGURATION == req->value_hi) {

         // This is where things get messy. We need to supply the
         // specified configuration data, followed by some number of
         // interfaces and endpoints. Plus there are length limits
         // to consider. First check that the specified index is valid.
         if (req->value_lo >= endpoint->enumeration_data->device.number_configurations) {
            result = USBS_CONTROL_RETURN_STALL;
         }
         else {
            // No such luck. OK, supplying the initial block is easy.
            endpoint->buffer        = (unsigned char*) &(endpoint->enumeration_data->configurations[req->value_lo]);
            endpoint->complete_fn   = (usbs_control_return (*)(usbs_control_endpoint*, int)) 0;

            // How much data was actually requested. If only the
            // configuration itself is of interest then there is
            // no need to worry about the rest.
            if (length <= USB_CONFIGURATION_DESCRIPTOR_LENGTH) {
               endpoint->buffer_size       = length;
               endpoint->fill_buffer_fn    = (void (*)(usbs_control_endpoint*)) 0;
            }
            else {
               //we just fill
               endpoint->buffer_size       = USB_CONFIGURATION_DESCRIPTOR_LENGTH;
               endpoint->fill_buffer_fn    = &usbs_configuration_descriptor_refill;
            }

            result = USBS_CONTROL_RETURN_HANDLED;
         }
      }
      else if (USB_DEVREQ_DESCRIPTOR_TYPE_STRING == req->value_hi) {

         // As long as the index is valid, the rest is easy since
         // the strings are just held in a simple array.
         // NOTE: if multiple languages have to be supported
         // then things get more difficult.
         if (req->value_lo >= endpoint->enumeration_data->total_number_strings) {
            result = USBS_CONTROL_RETURN_STALL;
         }
         else {
            endpoint->buffer                = (unsigned char*) endpoint->enumeration_data->strings[req->value_lo];
            endpoint->fill_buffer_fn        = (void (*)(usbs_control_endpoint*)) 0;
            endpoint->complete_fn           = (usbs_control_return (*)(usbs_control_endpoint*, int)) 0;
            //
            if (length < endpoint->buffer[0]) {
               endpoint->buffer_size = length;
            }
            else {
               endpoint->buffer_size = endpoint->buffer[0];
            }
            result = USBS_CONTROL_RETURN_HANDLED;
         }
      }
      //
      else if(USB_DEVREQ_DESCRIPTOR_TYPE_QUALIFIER == req->value_hi) {
      	endpoint->buffer = (unsigned char*) &endpoint->enumeration_data->qualifier;
      	endpoint->buffer_size    = length;
      	endpoint->fill_buffer_fn        = (void (*)(usbs_control_endpoint*)) 0;
         endpoint->complete_fn           = (usbs_control_return (*)(usbs_control_endpoint*, int)) 0;
      	result = USBS_CONTROL_RETURN_HANDLED;
		}
		else {
         result = USBS_CONTROL_RETURN_STALL;
      }
   }
   //GET_INTERFACE host request
   else if (USB_DEVREQ_GET_INTERFACE == req->request) {
      if ((1 != length) ||
            (USB_DEVREQ_DIRECTION_IN != direction) ||
            (USBS_STATE_CONFIGURED != (endpoint->state & USBS_STATE_MASK))) {

         result = USBS_CONTROL_RETURN_STALL;
      }
      else {
         int interface_id;
         //
         interface_id = (req->index_hi << 8) | req->index_lo;
         //if (interface_id != endpoint->enumeration_data->interfaces[0].interface_id) {
         if (interface_id != GET_INTERFACE_INTERFACE_ID(endpoint,0)) {
            result = USBS_CONTROL_RETURN_STALL;
         }
         else {
            //endpoint->control_buffer[0] = endpoint->enumeration_data->interfaces[0].alternate_setting;
            endpoint->control_buffer[0] = GET_INTERFACE_ALTERNATE_SETTINGS(endpoint,0);
            endpoint->buffer            = endpoint->control_buffer;
            endpoint->buffer_size       = 1;
            endpoint->fill_buffer_fn    = (void (*)(usbs_control_endpoint*)) 0;
            endpoint->complete_fn       = (usbs_control_return (*)(usbs_control_endpoint*, int)) 0;
            result = USBS_CONTROL_RETURN_HANDLED;
         }
      }
   }
   //GET_STATUS host request
   else if (USB_DEVREQ_GET_STATUS == req->request) {
      if (USB_DEVREQ_RECIPIENT_INTERFACE == recipient) {
         // The host should expect two bytes back, the device must
         // be configured, the interface number must be valid.
         // The host should expect no data back, the device must
         // be configured, and there are no defined features to clear.
         if ((2 == length) &&
               (USB_DEVREQ_DIRECTION_IN == direction) &&
               (USBS_STATE_CONFIGURED == (endpoint->state & USBS_STATE_MASK))) {
            int interface_id = req->index_lo;
            //
            //if (interface_id == endpoint->enumeration_data->interfaces[0].interface_id) {
            if (interface_id == GET_INTERFACE_INTERFACE_ID(endpoint,0)) {

               // The request is legit, but there are no defined features for an interface...
               endpoint->control_buffer[0] = 0;
               endpoint->control_buffer[1] = 0;
               endpoint->buffer            = endpoint->control_buffer;
               endpoint->buffer_size       = 2;
               endpoint->fill_buffer_fn    = (void (*)(usbs_control_endpoint*)) 0;
               endpoint->complete_fn       = (usbs_control_return (*)(usbs_control_endpoint*, int)) 0;
               result = USBS_CONTROL_RETURN_HANDLED;

            }
            else {
               result = USBS_CONTROL_RETURN_STALL;
            }
         }
         else {
            result = USBS_CONTROL_RETURN_STALL;
         }
      }

   }
   //SET_CONFIGURATION host request
   else if (USB_DEVREQ_SET_CONFIGURATION == req->request) {

      // Changing to configuration 0 means a state change from
      // configured to addressed. Changing to anything else means a
      // state change to configured. Both involve invoking the
      // state change callback. If there are multiple configurations
      // to choose from then this request has to be handled at
      // a higher level.
      int old_state = endpoint->state;
      if (0 == req->value_lo) {
         endpoint->state = USBS_STATE_ADDRESSED;
         if ((void (*)(usbs_control_endpoint*, void*, usbs_state_change, int))0 != endpoint->state_change_fn) {
            (*endpoint->state_change_fn)(endpoint, endpoint->state_change_data,
                  USBS_STATE_CHANGE_DECONFIGURED, old_state);
         }
         result = USBS_CONTROL_RETURN_HANDLED;
      }
      else {
         if (req->value_lo == endpoint->enumeration_data->configurations[0].configuration_id) {
            endpoint->state = USBS_STATE_CONFIGURED;
            if ((void (*)(usbs_control_endpoint*, void*, usbs_state_change, int))0 != endpoint->state_change_fn) {
               (*endpoint->state_change_fn)(endpoint, endpoint->state_change_data,
                     USBS_STATE_CHANGE_CONFIGURED, old_state);
            }
            result = USBS_CONTROL_RETURN_HANDLED;
         }
         else {
            result = USBS_CONTROL_RETURN_STALL;
         }
      }
   }
   //SET_FEATURE host request
   else if (USB_DEVREQ_SET_FEATURE == req->request) {
      if (USB_DEVREQ_RECIPIENT_INTERFACE == recipient) {
         // The host should expect no data back, the device must
         // be configured, and there are no defined features to clear.
         if ((0 == length) &&
               (USBS_STATE_CONFIGURED == (endpoint->state & USBS_STATE_MASK)) &&
               (0 == req->value_lo)) {
            int interface_id = req->index_lo;
            //
            //if (interface_id == endpoint->enumeration_data->interfaces[0].interface_id) {
            if (interface_id == GET_INTERFACE_INTERFACE_ID(endpoint,0)) {
               result = USBS_CONTROL_RETURN_HANDLED;
            }
            else {
               result = USBS_CONTROL_RETURN_STALL;
            }
         }
         else {
            result = USBS_CONTROL_RETURN_STALL;
         }
      }
	}
	//SET_INTERFACE host request
	else if (USB_DEVREQ_SET_INTERFACE == req->request) {
		int interface_id =(req->index_hi << 8) | req->index_lo;
		int setting = (req->value_hi << 8) | req->value_lo;
		result = USBS_CONTROL_RETURN_HANDLED;
	}

   return result;
}


/*============================================
| End of Source  : usbslave.c
|---------------------------------------------
| Historic:
|---------------------------------------------
| Authors     | Date     | Comments
| $Log:$
==============================================*/
