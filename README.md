A maphack for Warcraft 3 1.24d. WC3 sets the visibility of the map every frame via a call to memset that looks like:
```
6F40B446   . 52             PUSH EDX                                
6F40B447     6A 00          PUSH 0
6F40B449   . 50             PUSH EAX                                
6F40B44A   . E8 73643D00    CALL <JMP.&MSVCR80.memset>       
```
By changing the push 0 instruction to push 0fh, we reveal all the squares on the map. This method will cause a desync in multiplayer games.       

The injector was written in C++ and compiled with VS6 but should compile with any modern C++ compiler.

The hack was written in mASM and needs to be linked as a dll. To do this:
```
\masm32\bin\ml /c /coff WC3Maphack.asm
\masm32\bin\Link /SUBSYSTEM:WINDOWS /DLL WC3Maphack.obj
```

Originally written 2010/02/16 by attilathedud.

Injector:

![Injector Screenshot](screenshot_i.png?raw=true "Screenshot Injector")

Hack:

![Hack Screenshot](screenshot_h.jpg?raw=true "Screenshot Hack")

## Related Addresses

The following are a list of addresses all related to drawing functions:

Addresses are for patch 1.24c. To translate to patch 1.24d, simply add 60h to each address. 

```
6F39A415   CALL Game.6F2AC170 - draw units (whole function)

6F39A826   CALL Game.6F2AC170 - trees (whole function)

6F39BC6D   CALL Game.6F2AC170 - draw shade (whole function)

6F00C8B3   PUSH 80000004 - debug protection (change to push 0 to disable)

6F39D03D   CALL Game.6F017850 - shade + map tile formatting?

6F39D061   CALL Game.6F427730 - shade on characters

6F39D00A   CALL Game.6F015FA0 - draws map tiles

6F39D185   CALL Game.6F610330 - draws click arrows

6F39CD7E   CALL Game.6F7B6E70 - blacks out screen

6F39CD83   CALL Game.6F509E80 - draws units

6F39CB6B   CALL Game.6F38CA60 - draws idle animation and resets animation bones

6F39CB79   CALL Game.6F53FC20 - unit animation

6F430762   CALL Game.6F4276B0 - unit won't respond to movement
```
