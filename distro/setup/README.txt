LGCK Builder is an all-one Side-Scroller Contruction Kit

Getting Started
---------------------------------------------------------------------

LGCK Builder comes in three bundles.

1) The latest development snapshot can be found on github:

   https://github.com/cfrankb/lgck-src

2) The Windows installer (lgck-setup.exe) and portable edition (lgck-portable.zip)
	Latest official build
	https://sourceforge.net/projects/lgck/files/Lgck_Builder/

3) The game demos and tutorial material

   https://github.com/cfrankb/lgck-data

Try the example games "vla3demo2???.lgckdb" and "step???.lgckdb" 
included in the demo package. They contain much tutorial material
to get you started.

Press F5 to test the current Level.
Press ESC to return to the editor.
CTRL PageUp/PageDown to switch to another level.


Known Issues
---------------------------------------------------------------------

This program may crash when run along side some Logitech software 
(e.g. Logitech Setpoint 4.80, Logitech WebCam Software etc.). If 
you experience problems, close all other applications one by one 
until you find the culprit. Let us know about your experience 
so we may update the documentation.

Other applications that use textures may also interfere with LGCK
builder. This includes Video players and some browser plugins.


Keyboard layout
---------------------------------------------------------------------

SPACE          JUMP*
Z              ACTION (pull switch)
LEFT SHIFT     FIRE
UP, 
DOWN, 
LEFT, 
and RIGHT      MOVE UP, DOWN, LEFT and RIGHT

*Jump pattern are game dependant.
The engine supports these modes

VLA3 (engine default)

	To jump hold SPACE + [cursor key]
	The following are valid combinations.

	SPACE + UP
	SPACE + DOWN
	SPACE + LEFT
	SPACE + RIGHT
	SPACE + UP + LEFT
	SPACE + UP + RIGHT
	SPACE + DOWN + LEFT
	SPACE + DOWN + RIGHT

GIANA 

	UP is Jump Button
	LEFT/RIGHT to control jump

MIXED
	SPACE is Jump Button
	LEFT/RIGHT to control jump


The biggest difference between GIANA and MIXED is that the later also lets you climb ladders. :D


QuickStarter Five Minute Tutorial
---------------------------------------------------------------------

To help you get a grip of LGCK Builder we prepared 
a 5 minutes tutorial to walk you through the process.

You can view it at this url:
https://www.youtube.com/watch?v=lN3gkZWr-PM


Online Manual
---------------------------------------------------------------------
https://cfrankb.com/lgck


Scintilla and SciTE
---------------------------------------------------------------------

License for Scintilla and SciTE
Copyright 1998-2003 by Neil Hodgson <neilh@scintilla.org>
All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation.

 NEIL HODGSON DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 AND FITNESS, IN NO EVENT SHALL NEIL HODGSON BE LIABLE FOR ANY
 SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
 OR PERFORMANCE OF THIS SOFTWARE.

http://www.riverbankcomputing.co.uk/static/Docs/QScintilla2/index.html


General disclaimer
---------------------------------------------------------------------

DISCLAIMER: THE PRODUCT IN THIS ARCHIVE WAS COMPILED, IS
PROVIDED COURTESY AND AT THE DISCRETION OF THE AUTHOR. USERS
OF THIS PROGRAM AGREE TO ACCEPT IT *AS IS*. NO WARRANTY
EXPRESSED OR IMPLIED INCLUDING BUT NOT LIMITED TO ANY PARTICULAR
FITNESS, MERCHANTABILITY OR ACCURACY IS PROVIDED. THE AUTHOR
EXPRESSLY DISCLAIMS RESPONSIBILITY AS TO ANY DIRECT, INDIRECT OR
INCIDENTAL DAMAGES THAT MAY ARISE FROM THE USAGE OR INABILITY TO
USE THE MATERIAL PROVIDED HEREIN.


Feedback and more information
---------------------------------------------------------------------

Don't be shy. Give us some feedback !
We need your suggestions to improve.

Visit the project homepage for more details
http://cfrankb.com/lgck


revision history
---------------------------------------------------------------------

*** 0.6.0.9

- EDITOR: FontWizard lets you import system fonts
- ENGINE: overlays now support multiline text
- ENGINE: overlays can have custom fonts
- ENGINE: major code clean up
- ENGINE: abstracted keys and buttons
- ENGINE: added Countdowns
- EDITOR: mark/unmark all goals matching a sprite 
- EDITOR: added debug output window
- EDITOR: tile paint, eraser modes added
- EDITOR: show TriggerKeys in editor
- EDITOR: added Project Assistant that report and help fix common design errors
- EDITOR: added sample sprite to New Games
- EDITOR: replaced QScintilla with a native Qt solution
- EDITOR: added GamePad support
- EDITOR: lgck builder can export sprite meta (proto)
- EDITOR: customize multiselection
- EDITOR: new Overlay type Counter
- EDITOR: added coins, lives and ammo bonuses
- EDITOR: integrating player fire bullet w/ editSprite
- EDITOR: added skill filter
- EDITOR: added sprite search to Toolbox
- TOOLS: migrated to qt 5.15
- LUA: display_setFlagXY, display_setFont, display_setSource
- LUA: countdown_set, countdown_start, countdown_stop
- LUA: spriteIdFromUuid, sprite_freezeAll, sprite_unfreezeAll
- BUG FIX: onleftClick/onRightClick doesn't work anymore

*** 0.6.0.8

- EDITOR: added portable option to package bundle (windows)
- EDITOR: added export game UI dialog
- EDITOR; replaced setup from InnoSetup with AdvancedInstaller
- EDITOR: added manifests to satisfy windows app cert kit
- EDITOR: new warnings for no player and no goal
- EDITOR: lgck-builder and obl5edit can now be associated with files
- EDITOR: tigher integration between lgck builder and obl5edit
- ENGINE: SFML runtime completed
- TOOLS: migrated gui to qt 5.10.1
- BUG FIX: ScriptWiz displayed incorrectly in Windows
- BUG FIX: uninitialized font causing game to crash (critical) introduced in 0.6.0.6
- BUG FIX: export game wasn't including musics
- BUG FIX: testing empty level was causing a crash

*** 0.6.0.7

- EDITOR: builder can export sprites (obl, png)
- EDITOR: runtime integration with the IDE
- EDITOR: improved LGCK builder performance on low-end hardware
- ENGINE: new level event: OnIntroDraw
- ENGINE: snapshot_clear()
- LUA: System Call - ss_notifyAll()
- TOOLS: Updated the gui to run on qt5.7 (replaced qglwidget w/ qopenglwidget)
- TOOLS: easydocs - introducted the concept of alias and improve the ui
- BUG FIX: lua script not always regenerated after an event was modified
- BUG FIX: when a snapshot is taken and restored the next level may be replaced by the snapsnot
- BUG FIX: LGCK builder would crash when no opengl2.0 driver were present
- BUG FIX: lgckdb would become corrupt when core libraries were compiled against GCC 4.x+

This new build addresses issues on VM and low-end hardware that doesn't
have native drivers supporting Opengl 2.0+

*** 0.6.0.6

- ENGINE: replaced scene list w/ ptr 
- ENGINE: new events: Level.onHandler, Level.onGoalKilled
- ENGINE: added borderColor, introBkColor, introTextColor
- ENGINE: added experimental barebone runtimes (opengl, SDL, SFML)
- ENGINE: new game events: onPickup(), onSaveGame(), onLoadGame()
- ENGINE: new game events: onRestartLevel(), onNotifyClosure()
- ENGINE: new sprite event: EO_NOTIFYCLOSURE
- ENGINE: new level event: EL_NOTIFYCLOSURE
- ENGINE: custom scrolling layers (see layer_setOffset)
- ENGINE: tied EL_RESTART event with snapshot_reload
- ENGINE: replaced FTGL w/ lgck's own font rendering code
- ENGINE: added colorMod (primary light source)
- EDITOR: added scriptWizard
- EDITOR: autoupdater to check for new versions
- EDITOR: edit raw path (sprite)
- EDITOR: GUI to resize font size in the editor pane
- EDITOR: SpriteWiz waypoint added
- EDITOR: added Edit Images pop-menu item for sprites
- EDITOR: testLevel nows allows to play a full game
- LUA: updateJoyState()
- LUA: callGameEvent(), sprite_unmarkAsGoal()
- LUA: sprite:get(), sprite_set()
- LUA: sprite:get()/set() can access EXTRA_XXXX values directly
- LUA: sprite:hitTest()
- LUA: display_sizeText(), Display:sizeText()
- LUA: ss_drawText(), ss_paint(), ss_paintImage(), ss_clear()
- LUA: Counters:inc(), Counters:dec(), Counters:add()
- LUA: warpTo(), strv_get(), strv_set(), strv_del()
- LUA: snapshot_take(), snapshot_reload()
- LUA: layer_setOffsetX(), layer_setOffsetY()
- LUA: SHL(), SHR(), ss_notifyClosure()
- BUG FIX: When more than 10 Displays active, only the last one is displayed
- BUG FIX: random crash when sprite is added to main layer

*** 0.6.0.5

- ENGINE: customs strings for each sprite instance
- EDITOR: migrated to qt5
- EDITOR: GUI to change default frame per sprite
- EDITOR: toolbox playSound() menu item
- EDITOR: customizable shortcuts for the LGCK Builder
- EDITOR: GUI for toolbars (main, level, layer)
- EDITOR: GUI for path creation
- ENGINE: custom paths for each sprite
- ENGINE: up to four fwClass sprites can share the same map location
- ENGINE: hidden inventory item flag
- ENGINE: reimplemented the collision map w/ std::unordered_map
- ENGINE: interfaces w/ implementers: Graphic, Sound and Music
- ENGINE: replaced SFML::Graphics w/ OpenGL
- ENGINE: replaced SFML::Music w/ SDL::Music
- ENGINE: replaced SFML::Sound w/ SDL
- ENGINE: added Counters (that can be accessed via scripting)
- ENGINE: significantly improved level load time
- ENGINE: added events: Game:onGoalKilled and Level:onGoalKilled
- ENGINE: added Level UUID
- ENGINE: improved center on screen during action
- LUA: sprite:frameCount(), frameSet_getSize
- LUA: Sprite:getString(), Sprite:isFrozen(), Sprite:isGoal()
- LUA: Counters:get(), Counters:set()
- BUG FIX: layer element count is wrong for FW/BK layers
- BUG FIX: on wrap arround screens, viewport moves unexpectely
- BUG FIX: clearALL button now behaves as expected
- BUG FIX: keyDown msg continued infitely when focus was lost

*** 0.6.0.4

- ENGINE: layers can be moved by position
- EDITOR: importImage can now splice images
- EDITOR: importImageWiz added
- EDITOR: added gamewiz first version
- EDITOR: added copy, cut, paste and delete to levelView
- EDITOR: spritewiz added playerbullet template
- LUA: added Sprite:width(), Sprite:height() 
- LUA: added Sprite:childCount(), Sprite:setOwner()
- ENGINE: player.onHandler called in managePlayer()
- ENGINE: added sprite activation policy (auto, userdef)
- BUG FIX: spritewiz now properly displays image size (mask: 9999x9999)
- TOOLS: included is the new OBL5EDIT sprite editor to replace OBL4EDIT

*** 0.6.0.3

- EDITOR: edit pan can be resized
- BUG FIX: animations weren't cleared when sprite was deleted

*** 0.6.0.2

- ENGINE: antigravity flag extends to monsters
- ENGINE: added solidity concept (overide)
- EDITOR: added `sound tab` to SpriteWizard
- ENGINE: added multiselection
- BIG FIX: Spelling error, "Brint to front" (reported by Tim)
- BUG FIX: Menu not updated when sprite dragged (reported by Tim)
- BUG FIX: WINXP crash when sprite was dropped on empty pane (reported by Tim)
- BUG FIX: next button greyed out in SpriteWiz

*** 0.6.0.1a

- BUG FIX: crash cause by bad sprite mapping
- BUG FIX: crash when no player object was present

*** 0.6.0.1

- ENGINE: fixed CPath array so it can grow
- ENGINE: added seed uid for all sprites
- ENGINE: added Game:OnCompleteLevel
- EDITOR: implemented continuity (test game)
- BUG FIX: fixed a few mapping bugs introduced in 0.5.0.7
- BUG FIX: icon incorrect modified in SpriteList
- EDITOR: SpriteWizard updated with many new pages
- ENGINE: added antigravity flag for all fwClass objects
- ENGINE: added automatic goal flag
- BUG FIX: misaligned sprites on grid
- ENGINE: nosplat flag extended to objects
- ENGINE: implemented monster/object rebirth
- BUG FIX: deleting the last and only level resulted in a crash (fixed)

*** 0.6.0.0

- LUA: MIN(), MAX(), testKey(), setKey()
- LUA: getLastKey(), clearKeys()
- ENGINE: new Event: level.onKeyPressed 
- ENGINE: new Event: level.onKeyUp
- ENGINE: improved qt keyReflector
- EDITOR: source editor w/ word completion and colored syntax
- EDITOR: sprite event dialog replaced with inline editor
- BUG FIX: fixed an error in obl5t (frameSet.cpp)

*** 0.5.0.9

- GUI: added a new toolbox to drag&drop sprites on the level
- GUI: added popup menu to spriteBox
- GUI: added popup menu to LevelView
- GUI: added checkmarks to sprite event combo
- BUG FIX: png transparency bug in obl5t
- BUG FIX: unMap bug w/ random attacker
- ENGINE: OBL5 format moved to version 0x501
- TOOLS: obl5t merged now uses the shared libraries

*** 0.5.0.8

- ENGINE: added CMusic class to handle streaming music (ogg)
- ENGINE: more reorganization of the code
- ENGINE: added closureTime, closureEvent
- ENGINE: added Level property : music
- BUG FIX: windows-setup now uses static build (ati issue on win7)

*** 0.5.0.7

- ENGINE: Created CActor which replaces CLevelEntry at runtime.
- ENGINE: reorganized the code to be more OO-like
- ENGINE: added NO_SPLAT flag to prevent player from crushing monsters
- ENGINE: added properties `max active bullets`, `fire rate`
- ENGINE: improved the collision detection code
- ENGINE: maxFall property now applies to monster
- ENGINE: bullet to monster attack now do damage
- LUA: added isHidden(), isDead(), land(), triggerHitState() 
- LUA: added isMonster(), getPlayerC(), getPlayer()
- LUA: added tryPath(), stopAnimation(), callObjEvent(), killSprite(), spawn()
- EDITOR: added menu items: view source, edit object 
- EDITOR: new shortcuts F9 and SHIFT-F9 (edit local, edit object)

*** 0.5.0.6

- GUI: added status text to all implemented menu items
- LUA: added setImage(), getImage(), Sprite:SetImage()
- LUA: added setState(), findSprite(), alert()
- LUA: added getExtra(), getExtraC(), getExtraUID()
- LUA: added triggerPlayerHitState()
- LUA: added hitTest(), hitTestC()
- LUA: added Extra class to provide access to sprites runtime data (readonly)
- ENGINE: onZKEY event called for player object when Z-KEY is pressed
- ENGINE: ZKeyTrigger class no longer changes the image of the source object
- ENGINE: noChangeAtDeath flag to preserve sprite corpses
- ENGINE: three jump modes available (VLA3, GIANA and MIXED)
- BUG FIX: hitTest logic not working in some cases

*** 0.5.0.5 

- GUI: added pause/restart/debug menu items.
- GUI: added splash screen
- BUG FIX: exiting the game from the menu corrupts the imageSets
- BUG FIX: UI for animation (add, duplicate and delete) showing wrong tooltips
- BUG FIX: a monster repeating animation prevents 'death sequence'
- BUG FIX: player sprite disapears from view when killed (animSeq bug)
- ENGINE: maxFall now an object property (player only atm)
- ENGINE: added game.prepareLevel() event
- ENGINE: added object.mouseClickLeft and object.mouseClickRight events
- DEMOS: updated the demos

0.5.0.4a 

- GUI: new icons curtosy of Ronnie Edwards 
- BUG FIX: player hurt animation not running
- BUG FIX: animation doesn't switch to UP while default animation running (loop)  
- BUG FIX: skill level not initialized properly through gui (editor)
- BUG FIX: layer UI not updated correctly in editor
- BUG FIX: class_immediatly_deadly not working
- BUG FIX: player can be hurt after being killed
- BUG FIX: player leaves the viewport
- BUF FIX: mysterious crash condition reported by model76 (windows only)
- LUA: Inventory Class 
- LUA: addToInventory(), removeFromInventory(), hasItem() and resetInventory()
- LUA: getSkill() (alias for SKILL)
- ENGINE: fallSpeed now an object property
- CODE: reorganized some files to speed up compilation time


*** 0.5.0.4 

- ENGINE: added custom animation sequences for all objects
- ENGINE: per player object jump paths  
- LUA: tryAnimation()
- LUA: setBkColor, setBkColorC
- LUA: getBkColor, getBkColorC
- LUA: setLookUp, getLookUp
- LUA: setLevelGoal, getLevelGoal
- LUA: getTicks(), ticks()
- LUA: getSpriteCount()
- LUA: newColor
- LUA: copySpriteC, copySprite
- etc.

- DEMOS: updated the demos to use the latest features


*** 0.5.0.3

- FIXED: an application crash when an imageSet is added to a fresh database.
         this bus was introduced in 0.5.0.2

*** 0.5.0.2a

- ENGINE: LGCK now implements images caching and hardware acceleration

- BUG FIX: fixed a critical bug in vla3demo224.lgckdb

- TOOLS: added LGCK-TEST.EXE which lets you benchmark your machine

- TOOLS: added LGCK.EXE which is a stand alone runtime 
         (drag and drop an lgckdb file on LGCK.EXE to open)


*** 0.5.0.2

- ENGINE: implemented optional lookup/lookdn
  Players can look up and down to see what's ahead.

- ENGINE: added new scroll ratio 1/2 and 1/4
  Smaller scroll ration allows for faster background scroll

- ENGINE: per level game speed (1-200)
  You can set the game speed to optimize user experience

- ENGINE: systemClass demoted to ObjectInvisibleTag
  This new change allows all objects to be made system invisible

- ENGINE: added foreground layers and support scripting functions
  Foregroung layers make it possible to add weather system (snow, rain etc.) 
  at no extra cost. Scripting support allows you to manage the elements
  as you see fit.

- LUA: too many new functions to list
- LUA: new Layer and Element class 
  
- TOOLS: include a copy obl5t
  This is a special command line utility to split strips into individual
  images.

*** 0.5.0.1

- parallax scrolling support 
- inventory system 
- scripting event model 
- customizable overlay displays (scores, time left, lives etc.)
- implemented wrap around  
- fixed monster ai in gravity free mode

*** 0.5.0.0

- ported the source from MFC to qt
