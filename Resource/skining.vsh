vs.1.1
;------------------------------------------------------------------------------
; Constants specified by the app
;    c0-c83  = bone matrix index 0 ~ 28
;    c84-c87 = matViewProj
;    c88-c91 = reserved
;    c92     = light direction
;    c93     = material diffuse color * light diffuse color
;    c94     = material ambient color
;    c95     = const 1.0f, 0, ....
;	
;
; Vertex components (as specified in the vertex DECL)
;    v0    = Position
;    v1.x  = matrix idx
;    v2    = Normal
;    v3    = Texcoords
;------------------------------------------------------------------------------


;------------------------------------------------------------------------------
; Vertex blending
;------------------------------------------------------------------------------

; Transform position for world0 matrix
mov a0.x, v1.x

m4x3 r0, v0, c[a0.x]		;pos transform
m3x3 r3, v2, c[a0.x]		;normal transform
mov  r0.w, c95.x			; r0.w = 1.0f

;m4x4 r0, r0, c88
m4x4 oPos, r0, c84			; oPos = 

;------------------------------------------------------------------------------
; Lighting calculation
;------------------------------------------------------------------------------
; directional light
;dp3 r1.x, r3, c92   ; N 와 L 의 내적
;mul r2, r1.x, c93	; n * diffuse
;add oD0, r2, c94	; + ambient
mov oD0, c95.xxxx	; no lighting

;------------------------------------------------------------------------------
; Texture coordinates
;------------------------------------------------------------------------------

; Just copy the texture coordinates
mov oT0.xy,  v3.xy
;mov oT1.xy,  v3.xy


