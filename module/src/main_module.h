/***************************************************************************
 *   Copyright (C) 2020 by Christoph Thelen                                *
 *   doc_bacardi@users.sourceforge.net                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#ifndef __MAIN_MODULE_H__
#define __MAIN_MODULE_H__


typedef enum STM32_RESULT_ENUM
{
	STM32_RESULT_Ok                           =  0,
	STM32_RESULT_UnknownCommand               =  1,
	STM32_RESULT_UnalignedAddress             =  2,
	STM32_RESULT_InvalidSequenceCommand       =  3,
	STM32_RESULT_NotEnoughSequenceData        =  4,
	STM32_RESULT_UartTimout                   =  5,
	STM32_RESULT_NACK                         =  6,
	STM32_RESULT_UnexpectedSizeOfResult       =  7,
	STM32_RESULT_UnexpectedCpuID              =  8,
	STM32_RESULT_RequiredCommandsUnavailable  =  9,
	STM32_RESULT_VerifyError                  = 10
} STM32_RESULT_T;


typedef enum STM32_COMMAND_ENUM
{
	STM32_COMMAND_Initialize                = 0,
	STM32_COMMAND_ReadData32                = 1,
	STM32_COMMAND_WriteData32               = 2,
	STM32_COMMAND_RmwData32                 = 3,
	STM32_COMMAND_RunSequence               = 4
} STM32_COMMAND_T;


unsigned long module(unsigned long ulParameter0, unsigned long ulParameter1, unsigned long ulParameter2, unsigned long ulParameter3);


#endif  /* __MAIN_MODULE_H__ */

