/*
 * This file is part of the libopencm3 project.
 *
 * Copyright (C) 2014 Karl Palsson <karlp@tweak.net.au>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SYSCFG_H
#define	SYSCFG_H

#ifdef	__cplusplus
extern "C" {
#endif

#define STIMULUS_RING_DRAIN 2
#define STIMULUS_RING_PUSH 3
#define STIMULUS_TXC 4
#define STIMULUS_TX 5

#if defined STM32F1
#define LED_RX_PORT GPIOC
#define LED_RX_PIN GPIO13
#define LED_TX_PORT GPIOC
#define LED_TX_PIN GPIO13
#define RS485DE_PORT GPIOA
#define RS485DE_PIN GPIO8
/* TODO: should really make a stm32f4discovery.h file... */
#elif defined STM32F4

#define LED_RX_PORT	GPIOD
#define LED_RX_PIN	GPIO15  /* Blue, but you won't see this one much */
#define LED_TX_PORT	GPIOD
#define LED_TX_PIN	GPIO13  /* orange */
/* On stm32f4 discovery, this is actually another led... */
#define RS485DE_PORT	GPIOD
#define RS485DE_PIN	GPIO14  /* red */

#define STREAM_USART2_TX 6
#endif


#ifdef	__cplusplus
}
#endif

#endif	/* SYSCFG_H */

