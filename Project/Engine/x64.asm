.code
	systemClock PROC
	rdtsc
	mov [result],eax
	mov [result+4],edx
	systemClock ENDP
END