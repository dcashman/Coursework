.386
.XMM
.model flat, c

.code

mmx_save PROC buf:DWORD
  push   eax
  mov    eax,  buf
  fxsave [eax]
  pop    eax
  RET
mmx_save ENDP

mmx_restore PROC buf:DWORD
  push    eax
  mov     eax,  buf 
  fxrstor [eax]
  pop     eax
  RET
mmx_restore ENDP

end
