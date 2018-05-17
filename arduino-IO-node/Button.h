/*
  Copyright (C) Air Liquide S.A,  2017
  Author : Sébastien Lalaurette, La Factory, Creative Foundry
  This file is part of Predictable Farm embedded software project.

  Predictable Farm embedded software is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  Predictable Farm embedded software is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _BUTTON_H_
#define _BUTTON_H_

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

/*=========================================================================
    VALUE OF THE BUTTON
    -----------------------------------------------------------------------*/
#define BTN_UP            (840)
#define BTN_DOWN          (900)
#define BTN_BACK          (700)
#define BTN_ENTER         (980)

#define UP            (1)
#define DOWN          (2)
#define BACK          (3)
#define ENTER         (4)

#define BTN1           (1)
#define BTN2           (2)
#define BTN3           (3)
#define BTN4           (4)

//


class Button{
 public:
  Button();
  unsigned char get(void);
  unsigned char getCurrent(void);
  unsigned short getRawCurrent(void) ;
  unsigned char getPrev(unsigned char current);

 private:
   unsigned short prev;
};


#endif /*_BUTTON_H_*/
