.code

mmx_save PROC buf:QWORD
;  push   eax
;  mov    eax,  [rcx]
;  fxsave [eax]
  fxsave [rcx]
;  pop    eax
  RET
mmx_save ENDP

mmx_restore PROC buf:QWORD
;  push    eax
;  mov     eax,  [rcx]
;  fxrstor [eax]
  fxrstor [rcx]
;  pop     eax
  RET
mmx_restore ENDP

end
