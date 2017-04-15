; A maphack for Warcraft 3 1.24d. WC3 sets the visibility of the map every frame via a call to memset that
; looks like:
; 6F40B446   . 52             PUSH EDX                                
; 6F40B447     6A 00          PUSH 0
; 6F40B449   . 50             PUSH EAX                                
; 6F40B44A   . E8 73643D00    CALL <JMP.&MSVCR80.memset>       
;
; By changing the push 0 instruction to push 0fh, we reveal all the squares on the map. This method will
; cause a desync in multiplayer games.       
;
; Originally written 2010/02/16 by attilathedud.

; System descriptors
.386
.model flat,stdcall
option casemap:none

include \masm32\include\kernel32.inc
include \masm32\include\user32.inc
includelib \masm32\lib\kernel32.lib
includelib \masm32\lib\user32.lib

.data
	dllname db "Game.dll"
	toggle db 0
	on db "Maphack on",0
	off db "Maphack off",0
	
.data?
	hookbase dd ?
	ori_chat_out dd ?
	ori_map_draw dd ?
	ori_globalWC3class dd ?		
	
.code
	main:
		; Save the base pointer and load in the stack pointer
		push ebp
	    mov ebp,esp

		; Check to see if the dll is being loaded validly.
	    mov eax,dword ptr ss:[ebp+0ch]
	    cmp eax,1
	    jnz @returnf

		; Save eax on the stack for restoring after we create our thread.
	    push eax

		; All of our functions reside in Game.dll which is dynamically loaded. Because of this, we
		; have to get the base address of it and then add the offsets of the functions we need.
		lea eax,dllname
		push eax
		call GetModuleHandle 

		; Load our functions with their values. Note that eax is adjusted with each add, i.e.
		; hookbase's offset is 40b444h.
		add eax,2fbf60h
		mov ori_chat_out,eax
		add eax,10f4e4h
		mov hookbase,eax
		add eax,5h
		mov ori_map_draw,eax
		add eax,6c098fh
		mov ori_globalWC3class,eax

		; Allocate a section of memory to hold the old protection type for when we unprotect code.
		; Store this memory in ebx.
		push 40h
        push 1000h
        push 4h
        push 0
        call VirtualAlloc 	
		mov ebx,eax

		; Unprotect the code that we intend to modify so that we can make changes to it.
		push ebx
        push 40h
        push 5h
		push hookbase
        call VirtualProtect

		; Before the call to memset, create a codecave that will jmp to our hook function.
		; e9h is the opcode to jmp, with the address of the jump being calculated by subtracting
		; the address of the function to jump to from our current location.
        mov eax,hookbase
        mov byte ptr ds:[eax],0e9h
        lea ecx,@hook
        sub ecx,ori_map_draw
        mov dword ptr ds:[eax+1],ecx

		; Reprotect the code we just modified.
        push ebx
        push 40h
        push 5h
        push hookbase
       	call VirtualProtect 

		; Free the memory we allocated to hold the protection type.
        push 4000h
	    push 4h
	    push ebx
	    call VirtualFree 

		; Create a thread that will check for us pressing a hotkey
		push 0
		push 0
		push 0
		push @hotkey
		push 0
		push 0
		call CreateThread 

		; Restore the stack and exit dllmain
		pop eax
		@returnf:
			leave
			retn 0ch
			
		
		; The hotkey thread will scan for F5 being pressed and then checks 6fae54d3h to see if
		; we are currently in a game or not. The value of toggle is switched between 0 and 0fh.
		; Depending on the value of toggle, the chat out function is called with either an on or off
		; value.
		;
		; @@ sets up a temporary label. @B jumps back to the last defined temporary label. 
		@hotkey:
			@@:
				push 74h
				call GetAsyncKeyState 
				and eax,1
				test eax,eax
				jz @b
				cmp dword ptr ds:[6fae54d3h],09h
				jl @b
				cmp toggle,0
				jnz @toggle_off
				mov toggle,0fh
				lea edx,on
				call @chat_out
				jmp @b
				@toggle_off:
					mov toggle,0
					lea edx,off
					call @chat_out
			jmp @b
			retn

		; Our hook function.
		@hook:
			; The instructions that we replaced with our codecave up in dll main.
			add edx,edx
			push edx

			; Clear out edx and set its value to toggle's value.
			xor edx,edx
			mov dl,byte ptr ds:[toggle]
			push edx

			; Jump back to the original code.
			jmp ori_map_draw
			
		; Function that calls WC3's internal chat out function to display messages. Expects
		; edx to hold the address of the text you want displayed.
		@chat_out:
			push 1fh
			push 0
			push edx
			push 0
			mov ecx,dword ptr ds:[ori_globalWC3class]
			mov ecx,dword ptr ds:[ecx]
			call ori_chat_out
			retn
			
	end main