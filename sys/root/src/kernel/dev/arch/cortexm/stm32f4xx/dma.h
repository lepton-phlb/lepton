/*
The contents of this file are subject to the Mozilla Public License Version 1.1
(the "License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License for the
specific language governing rights and limitations under the License.

The Original Code is Lepton.

The Initial Developer of the Original Code is Philippe Le Boulanger.
Portions created by Philippe Le Boulanger are Copyright (C) 2014 <lepton.phlb@gmail.com>.
All Rights Reserved.

Contributor(s): Jean-Jacques Pitrolle <lepton.jjp@gmail.com>.

Alternatively, the contents of this file may be used under the terms of the eCos GPL license
(the  [eCos GPL] License), in which case the provisions of [eCos GPL] License are applicable
instead of those above. If you wish to allow use of your version of this file only under the
terms of the [eCos GPL] License and not to allow others to use your version of this file under
the MPL, indicate your decision by deleting  the provisions above and replace
them with the notice and other provisions required by the [eCos GPL] License.
If you do not delete the provisions above, a recipient may use your version of this file under
either the MPL or the [eCos GPL] License."
*/


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DMA_H
#define __DMA_H

/* Includes ------------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
#define DMA_IT_ALL_0  (DMA_IT_FEIF0 | DMA_IT_DMEIF0 | DMA_IT_TEIF0 | DMA_IT_HTIF0 | DMA_IT_TCIF0)
#define DMA_IT_ALL_1  (DMA_IT_FEIF1 | DMA_IT_DMEIF1 | DMA_IT_TEIF1 | DMA_IT_HTIF1 | DMA_IT_TCIF1)
#define DMA_IT_ALL_2  (DMA_IT_FEIF2 | DMA_IT_DMEIF2 | DMA_IT_TEIF2 | DMA_IT_HTIF2 | DMA_IT_TCIF2)
#define DMA_IT_ALL_3  (DMA_IT_FEIF3 | DMA_IT_DMEIF3 | DMA_IT_TEIF3 | DMA_IT_HTIF3 | DMA_IT_TCIF3)
#define DMA_IT_ALL_4  (DMA_IT_FEIF4 | DMA_IT_DMEIF4 | DMA_IT_TEIF4 | DMA_IT_HTIF4 | DMA_IT_TCIF4)
#define DMA_IT_ALL_5  (DMA_IT_FEIF5 | DMA_IT_DMEIF5 | DMA_IT_TEIF5 | DMA_IT_HTIF5 | DMA_IT_TCIF5)
#define DMA_IT_ALL_6  (DMA_IT_FEIF6 | DMA_IT_DMEIF6 | DMA_IT_TEIF6 | DMA_IT_HTIF6 | DMA_IT_TCIF6)
#define DMA_IT_ALL_7  (DMA_IT_FEIF7 | DMA_IT_DMEIF7 | DMA_IT_TEIF7 | DMA_IT_HTIF7 | DMA_IT_TCIF7)

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define dma_startup_init()  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1 | RCC_AHB1Periph_DMA2, ENABLE)

/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

#endif /* __DMA_H */
