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
