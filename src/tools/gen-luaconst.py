'''
    LGCK Builder Runtime
    Copyright (C) 1999, 2020  Francois Blanchette

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
'''

classes = '''
CLASS_BACKGROUND_DECORATION                    = 0x00;
CLASS_ANTIGRAVITY                              = 0x01;
CLASS_METAL_LADDER                             = 0x03;
CLASS_SOLID_DOWN_ONLY                          = 0x0a;
CLASS_REVERT_TO_BACKGROUND                     = 0x0b;
CLASS_IMMEDIATELY_DEADLY                       = 0x0d;
CLASS_SOLID_BACKGROUND                         = 0x0e;
CLASS_STOP                                     = 0x0f;
CLASS_ANIMATED_THING                           = 0x10;
CLASS_PICKUP_TRIGGER                           = 0x11;
CLASS_INVENTORY_ITEM                           = 0x12;
CLASS_OPEN_TO_OWNER                            = 0x13;
CLASS_TELEPORTATION_PAD                        = 0x14;
CLASS_TELEPORTATION_DESTINATION                = 0x15;
CLASS_Z_KEY_SWITCH                             = 0x16;
CLASS_ANIMATE_ONCE                             = 0x1d;
CLASS_SOLID_ANIMATED                           = 0x1e;
CLASS_PLAYER_OBJECT                            = 0x1f;
CLASS_GENERIC_MONSTER                          = 0x20;
CLASS_USER_DEFINED_MONSTER                     = 0x21;
CLASS_DRONE_UP_DOWN                            = 0x22;
CLASS_DRONE_LEFT_RIGHT                         = 0x23;
CLASS_ATTACKER                                 = 0x24;
CLASS_VAMPIRE_PLANT                            = 0x27;
CLASS_WHACKER_UP_DOWN                          = 0x2c;
CLASS_WHACKER_LEFT_RIGHT                       = 0x2d;
CLASS_PLATFORM_UP_DOWN                         = 0x2e;
CLASS_PLATFORM_LEFT_RIGHT                      = 0x2f;
CLASS_PLAYER_BULLER                            = 0x40;
CLASS_MONSTER_BULLET                           = 0x41;'''

c = []
with open('../lgck-builder/scripts/common.lua') as s:
    for line in s:
        if '=' in line:
            a = line.split('=',2)
            print(a[0].strip())
            c.append(a[0].strip())

for line in classes.strip().split('\n'):
    a = line.split('=',2)
    print(a[0].strip())
    c.append(a[0].strip())


with open('../lgck-builder/res/const.txt', 'w') as t:
    t.write('\n'.join([z for z in c]))
