vs.1.1
;------------------------------------------------------------------------------
; Constants specified by the app
;    c0-c83  = bone matrix index 0 ~ 28
;    c84-c87 = matViewProj
;    c88-c91 = reserved
;    c92     = light direction
;    c93     = material diffuse color * light diffuse color
;    c94     = material ambient color
;    c95     = const 1.0f, 1.0f, 1.0f, 100.0f
;	
;
; Vertex components (as specified in the vertex DECL)
;   v0    = Position
;	v1	  = w1, w2
;	v2.x  = matrix idx
;	v3    = Normal
;	v4    = Texcoords
;------------------------------------------------------------------------------

dcl_position v0;
dcl_blendweight v1;
dcl_blendindices v2;
dcl_normal v3;
dcl_texcoord0 v4;


;------------------------------------------------------------------------------
; Vertex blending
;------------------------------------------------------------------------------

; Transform position for world0 matrix
mov a0.x, v2.x				; matrix index 1
m4x3 r0, v0, c[a0.x]		; r0 = pos * (InvTM * AniTM)
m3x3 r3, v3, c[a0.x]		;normal transform
mul  r1, r0.xyz, v1.x		; r1 = r0 * w1
mul  r4, r3.xyz, v1.x		

mov  a0.x, v2.y				; matrix index 2
m4x3 r0, v0, c[a0.x]		; r0 = pos * (InvTM * AniTM)
m3x3 r3, v3, c[a0.x]		;normal transform
mad  r1, r0.xyz, v1.y, r1.xyz ; w2곱하고 위에거랑 더함
mad  r3, r3.xyz, v1.y, r4.xyz ; w2곱하고 위에거랑 더함

mov  r1.w, c95.x			; r1.w = 1.0f

;m4x4 r0, r0, c88
m4x4 oPos, r1, c84			; oPos = 결과벡터 * matViewProj
;mov r0, v0
;mov r0.w, c95.x
;mov oPos, r0


;------------------------------------------------------------------------------
; Lighting calculation
;------------------------------------------------------------------------------
; directional light
dp3 r1.x, r3, -c92   ; N 와 L 의 내적
lit r1, r1
mul r2, r1.y, c93	; n * diffuse
add r2, r2, c94		; + ambient
min oD0, r2, c95.x     ; clamp if > 1
;mov oD0, r2

;mov oD0, c95.xxxx	; no lighting

;dp3 r0.x, r3, -c92	; vLight dot normal
;sub r1, c88, v0		; vertex -> eyepos
;dp3 r1.w, r1, r1	; vertex->eyepos vector normalize
;rsq r1.w, r1.w
;mul r1, r1, r1.w

;add r2, r1, -c92
;dp3 r2.w, r2, r2
;rsq r2.w, r2.w
;mul r2, r2, r2.w

;dp3 r0.y, r2, r3

;mov r0.w, c89.w		; specular power

;lit r4, r0

;mul r5, r4.z, c89	; specular
;mul r6, r4.y, c90	; c90을 light color로 잠시 썼다. 1, 1, 1, 1
;mul r6, r6, c93
;add r6, r6, c94		; * diffuse + ambient
;add oD0, r5, r6

;------------------------------------------------------------------------------
; Texture coordinates
;------------------------------------------------------------------------------

; Just copy the texture coordinates
;sub r1.xy, v4.xy, c91
mov oT0.xy, v4.xy
add r1.xy, v4.xy, c91.xy
mov oT1.xy,  r1.xy
;mov oT1.xy,  v4.xy

mov oFog, c95.w


