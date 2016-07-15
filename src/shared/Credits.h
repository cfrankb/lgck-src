/*
    LGCK Builder Runtime
    Copyright (C) 1999, 2012  Francois Blanchette

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

typedef struct {
    const char * name;
    const char * job;
    const char * email;
    const char * web;
} MEMBER;

const MEMBER team [] =
{
   { "Francois Blanchette", "Team Leader/programmer", "lgckbuilder@yahoo.ca", "http://www.sassociations.net/cfrankb" },
   { "Jonathan Vaughn", "Sprite Animator", "", ""},
   { "David Rosen", "Composer", "", ""},
   { "Ronnie Edwards", "GUI/Background Artist", "", ""},
   { "Philip Bradbury", "Sound composer", "", ""},
   { "Mathias Hoertnagl", "Background art", "", ""},

   { "", "", "", ""},
   { "Your name here!", "", "", ""},
   { NULL, NULL, NULL, NULL }
};

