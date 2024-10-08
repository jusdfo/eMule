// -*- C++ -*-
// $Id: frame_def.h,v 1.3 2002/09/13 15:38:45 t1mpy Exp $

// id3lib: a C++ library for creating and manipulating id3v1/v2 tags
// Copyright 1999, 2000  Scott Thomas Haug
// Copyright 2002 Thijmen Klok (thijmen@id3lib.org)

// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Library General Public License as published by
// the Free Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
// License for more details.
//
// You should have received a copy of the GNU Library General Public License
// along with this library; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

// The id3lib authors encourage improvements and optimisations to be sent to
// the id3lib coordinator.  Please see the README file for details on where to
// send such submissions.  See the AUTHORS file for a list of people who have
// contributed to id3lib.  See the ChangeLog file for a list of changes to
// id3lib.  These files are distributed with id3lib at
// http://download.sourceforge.net/id3lib/

#ifndef _ID3LIB_FRAME_DEF_H_
#define _ID3LIB_FRAME_DEF_H_

struct ID3_FieldDef;
class ID3_Frame;

// Structure used for defining how frames are defined internally.
struct ID3_FrameDef
{
  ID3_FrameID   eID;
  char          sShortTextID[3 + 1];
  char          sLongTextID[4 + 1];
  ID3_V2Spec    eFirstAppearance;
  ID3_V2Spec    eLastAppearance;
  ID3_Frame*     (*convert)(const ID3_Frame* frame, ID3_V2Spec tospec);
//  ID3_FrameID   eReplacedBy;
  bool          bTagDiscard;
  bool          bFileDiscard;
  const ID3_FieldDef* aeFieldDefs;
  const char *  sDescription;
};

#endif
