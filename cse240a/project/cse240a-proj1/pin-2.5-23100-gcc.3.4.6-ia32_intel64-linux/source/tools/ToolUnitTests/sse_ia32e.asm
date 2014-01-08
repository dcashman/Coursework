.code

set_xmm_reg0 PROC xmm_reg:QWORD
  movdqu xmm0,  [rcx]
  RET
set_xmm_reg0 ENDP

get_xmm_reg0 PROC xmm_reg:QWORD
  movdqu [rcx], xmm0
  RET
get_xmm_reg0 ENDP

set_mmx_reg0 PROC mmx_reg:QWORD
  movq   mm0,   [rcx]
  RET
set_mmx_reg0 ENDP

get_mmx_reg0 PROC mmx_reg:QWORD
  movq   [rcx], mm0
  RET
get_mmx_reg0 ENDP

end
