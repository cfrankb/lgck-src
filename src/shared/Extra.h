/*
    LGCK Builder Runtime
    Copyright (C) 1999, 2016  Francois Blanchette

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
namespace lgck {
    enum Extra
    {
        EXTRA_ORGPROTO          = 0x01,
        EXTRA_ORGX              = 0x02,
        EXTRA_ORGY              = 0x03,
        EXTRA_HP                = 0x04,
        EXTRA_OLDAIM1           = 0x05,
        EXTRA_OLDAIM2           = 0x06,
        EXTRA_FALLHEIGHT        = 0x07,
        EXTRA_LIVES             = 0x08,
        EXTRA_ACTIVE            = 0x09,
        EXTRA_BULLETCOUNT       = 0x0a,
        EXTRA_OWNER             = 0x0b,
        EXTRA_FLAGS             = 0x0c,
        EXTRA_PATHDIR           = 0x0d,
        EXTRA_PATHPTR           = 0x0e,
        EXTRA_ANIMSEQ           = 0x0f,
        EXTRA_ANIMPTR           = 0x10,
        EXTRA_ANIMSPEED         = 0x11,
        EXTRA_DEATHINDICATOR    = 0x12,
        EXTRA_AGE               = 0x13,
        EXTRA_TIMEOUT           = 0x14,
        EXTRA_TAGFLAGS          = 0x15,
        EXTRA_INVENTORY         = 0x16,
        // this is the last item
        EXTRA_COUNT
    };
}
