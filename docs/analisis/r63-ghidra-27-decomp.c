/* Decompilado de las funciones abiertas de la r63.
   PowerPC:BE:32:Gekko_Broadway + extension DWARF1 (ghidra-dwarf1 parcheada).
   GQR0 en 80109364 = 0  */

/* ================================================================== */
/* 8007c390  Update__8ICEMoverf
   tam=3868 B  insn=967  cobertura=3868/3868  paired-singles=12  firma=IMPORTED
   proto: undefined CameraMover::Update(ICEMover * this, float dT)
*/

/* WARNING: Removing unreachable block (ram,0x8007d29c) */
/* WARNING: Removing unreachable block (ram,0x8007d298) */
/* WARNING: Removing unreachable block (ram,0x8007d294) */
/* WARNING: Removing unreachable block (ram,0x8007d290) */
/* WARNING: Removing unreachable block (ram,0x8007d28c) */
/* WARNING: Removing unreachable block (ram,0x8007d288) */
/* WARNING: Removing unreachable block (ram,0x8007c3b0) */
/* WARNING: Removing unreachable block (ram,0x8007c3ac) */
/* WARNING: Removing unreachable block (ram,0x8007c3a8) */
/* WARNING: Removing unreachable block (ram,0x8007c3a4) */
/* WARNING: Removing unreachable block (ram,0x8007c3a0) */
/* WARNING: Removing unreachable block (ram,0x8007c39c) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Unknown calling convention -- yet parameter storage is locked */
/* WARNING: Enum "EVIEW_ID": Some values do not have unique names */

void CameraMover__Update(ICEMover *this,float dT)

{
  float fVar1;
  float fVar2;
  float fVar3;
  bool bVar4;
  ICETrack *this_00;
  int iVar5;
  bool bVar11;
  int iVar6;
  bool b_refresh;
  ICEScene *pIVar7;
  undefined4 uVar8;
  ushort uVar10;
  ICEShakeTrack *pIVar9;
  bMatrix4 *m;
  bMatrix4 *m_00;
  uint shake_type;
  ICEData *pIVar12;
  ICEAnchor *pIVar13;
  Camera *pCVar14;
  int iVar15;
  int iVar16;
  ICEShakeData *pIVar17;
  byte in_cr0;
  byte in_cr1;
  byte unaff_cr2;
  byte unaff_cr3;
  byte unaff_cr4;
  byte in_cr5;
  byte in_cr6;
  byte in_cr7;
  float fVar18;
  float fVar19;
  float fVar20;
  float fVar21;
  float fVar22;
  bMatrix4 local_1b0;
  bVector3 local_170;
  bVector3 local_160;
  bMatrix4 local_150;
  Vector3 local_110;
  undefined4 local_104;
  Matrix4 local_100;
  float local_c0;
  float local_bc;
  float local_b8;
  float local_b4;
  bMatrix4 local_b0;
  float local_70;
  float local_6c;
  float local_68;
  float local_64;
  undefined8 local_60;
  uint local_58;
  
  this_00 = TheICEManager.pPlaybackTrack;
  local_58 = (uint)(in_cr0 & 0xf) << 0x1c | (uint)(in_cr1 & 0xf) << 0x18 |
             (uint)(unaff_cr2 & 0xf) << 0x14 | (uint)(unaff_cr3 & 0xf) << 0x10 |
             (uint)(unaff_cr4 & 0xf) << 0xc | (uint)(in_cr5 & 0xf) << 8 | (uint)(in_cr6 & 0xf) << 4
             | (uint)(in_cr7 & 0xf);
  local_60 = CONCAT44(0x43300000,TheICEManager._124_4_);
  if (((float)(int)TheICEManager._124_4_ == 0.0) &&
     (bVar11 = GameFlowManager__IsPaused(&TheGameFlowManager), bVar11)) {
    return;
  }
  bVar4 = false;
  bVar11 = this_00 == (ICETrack *)0x0;
  if (!bVar11) {
    iVar6 = ICETrack__GetContext(this_00);
    bVar4 = iVar6 == 3;
    iVar6 = ICETrack__GetContext(this_00);
    if (iVar6 != 2) {
      _bMirrorICEData = 0;
    }
  }
  *(undefined4 *)&this->bViolatesTopology = 0;
  b_refresh = ICEManager__RefreshCameraSplines(&TheICEManager);
  m_00 = &local_1b0;
  ICEMover__SetDesired(this,false,b_refresh);
  local_1b0.v0.x = 0.0;
  local_1b0.v0.y = 0.0;
  local_1b0.v0.z = 0.0;
  local_1b0.v3.w = 0.0;
  local_1b0.v0.w = 0.0;
  local_1b0.v1.x = 0.0;
  local_1b0.v1.y = 0.0;
  local_1b0.v1.z = 0.0;
  local_1b0.v1.w = 0.0;
  local_1b0.v2.x = 0.0;
  local_1b0.v2.y = 0.0;
  local_1b0.v2.z = 0.0;
  local_1b0.v2.w = 0.0;
  local_1b0.v3.x = 0.0;
  local_1b0.v3.y = 0.0;
  local_1b0.v3.z = 0.0;
  PSMTX44Identity(m_00);
  if ((this->nSpaceEye == 3) || (this->nSpaceLook == 3)) {
    pIVar7 = FindAnimScene();
    if (pIVar7 == (ICEScene *)0x0) {
      return;
    }
    uVar8 = (**(code **)&pIVar7->_vptr_ICEScene[0xd].__delta2)
                      ((int)&pIVar7->_vptr_ICEScene + (int)pIVar7->_vptr_ICEScene[0xd].__delta);
    PSMTX44Copy(uVar8,m_00);
  }
  if (bVar11) {
    fVar18 = ICEManager__GetParameter(&TheICEManager);
  }
  else {
    fVar18 = ICETrack__GetParameter(this_00);
  }
  fVar1 = this->fParameter1 - this->fParameter0;
  fVar22 = 0.0;
  if (ABS(fVar1) > 0.0001) {
    fVar22 = (fVar18 - this->fParameter0) / fVar1;
  }
  if (fVar18 >= 1.0) {
    ICEManager__SetGenericCameraToPlay(&TheICEManager,"","");
  }
  fVar19 = Cubic1D__GetVal(this->pSimSpeed,fVar22);
  fVar1 = 0.0;
  if (fVar19 >= 0.0) {
    if (fVar19 >= 100.0) {
      fVar19 = 100.0;
    }
    fVar1 = fVar19 * 0.01;
    (((this->__base).pCamera)->CurrentKey).SimTimeMultiplier = fVar1;
  }
  if (fVar1 < 0.01) {
    fVar1 = 0.01;
  }
  fVar19 = Cubic1D__GetVal(this->pNearClip,fVar22);
  if (fVar19 >= 0.0) {
    (((this->__base).pCamera)->CurrentKey).NearZ = fVar19;
  }
  uVar10 = ICEMover__GetFOV(this,fVar22);
  if ((uVar10 != 0) && (_6Camera_StopUpdating == 0)) {
    (((this->__base).pCamera)->CurrentKey).FieldOfView = uVar10;
  }
  local_170.x = 0.0;
  local_170.y = 0.0;
  local_170.z = 0.0;
  local_170.pad = 0.0;
  ICEMover__GetEye(this,(Vector3 *)&local_170,fVar22);
  local_160.x = 0.0;
  local_160.y = 0.0;
  local_160.z = 0.0;
  local_160.pad = 0.0;
  ICEMover__GetLook(this,(Vector3 *)&local_160,fVar22);
  pIVar12 = this->pICEData;
  local_150.v0.x = 0.0;
  local_150.v0.y = 0.0;
  local_150.v0.z = 0.0;
  local_150.v0.w = 0.0;
  local_150.v1.x = 0.0;
  local_150.v1.y = 0.0;
  local_150.v1.z = 0.0;
  local_150.v1.w = 0.0;
  local_150.v2.x = 0.0;
  local_150.v2.y = 0.0;
  local_150.v2.z = 0.0;
  local_150.v2.w = 0.0;
  local_150.v3.x = 0.0;
  local_150.v3.y = 0.0;
  local_150.v3.z = 0.0;
  local_150.v3.w = 0.0;
  if (pIVar12 == (ICEData *)0x0) {
LAB_8007c8d0:
    pIVar13 = this->pCar;
    local_150.v0.x = (pIVar13->mGeomRot).v0.x;
    local_150.v0.y = (pIVar13->mGeomRot).v0.y;
    local_150.v0.z = (pIVar13->mGeomRot).v0.z;
    local_150.v0.w = (pIVar13->mGeomRot).v0.w;
    local_150.v1.w = (pIVar13->mGeomRot).v1.w;
    local_150.v1.x = (pIVar13->mGeomRot).v1.x;
    local_150.v1.y = (pIVar13->mGeomRot).v1.y;
    local_150.v1.z = (pIVar13->mGeomRot).v1.z;
    local_150.v2.x = (pIVar13->mGeomRot).v2.x;
    local_150.v2.y = (pIVar13->mGeomRot).v2.y;
    local_150.v2.z = (pIVar13->mGeomRot).v2.z;
    local_150.v2.w = (pIVar13->mGeomRot).v2.w;
    local_150.v3.z = (pIVar13->mGeomPos).z;
    local_150.v3.x = (pIVar13->mGeomPos).x;
    local_150.v3.y = (pIVar13->mGeomPos).y;
  }
  else if (pIVar12->bIgnoreOrientation == '\0') {
    if ((pIVar12->bCarSpaceLag == '\0') || (!bVar4)) goto LAB_8007c8d0;
    pIVar13 = this->pCar;
    local_110.y = fVar18 * 0.9 + 0.1;
    fVar19 = (this->vSmoothCarPos).y;
    fVar20 = (pIVar13->mGeomPos).x;
    fVar21 = (pIVar13->mGeomPos).z;
    fVar2 = (this->vSmoothCarPos).x;
    fVar3 = (this->vSmoothCarPos).z;
    (this->vSmoothCarPos).y = ((pIVar13->mGeomPos).y - fVar19) * local_110.y + fVar19;
    (this->vSmoothCarPos).x = (fVar20 - fVar2) * local_110.y + fVar2;
    (this->vSmoothCarPos).z = (fVar21 - fVar3) * local_110.y + fVar3;
    local_104 = 0;
    fVar19 = (this->vSmoothCarFwd).x;
    fVar20 = (this->vSmoothCarFwd).y;
    fVar21 = (this->vSmoothCarFwd).z;
    local_110.x = ((pIVar13->mGeomRot).v0.x - fVar19) * local_110.y;
    local_110.z = ((pIVar13->mGeomRot).v0.z - fVar21) * local_110.y;
    local_110.y = ((pIVar13->mGeomRot).v0.y - fVar20) * local_110.y;
    (this->vSmoothCarFwd).x = local_110.x + fVar19;
    (this->vSmoothCarFwd).y = local_110.y + fVar20;
    (this->vSmoothCarFwd).z = local_110.z + fVar21;
    PSMTX44Identity(&local_150);
    bNormalize((bVector3 *)&this->vSmoothCarFwd,(bVector3 *)&this->vSmoothCarFwd);
    local_150.v0.x = (this->vSmoothCarFwd).x;
    local_150.v0.y = (this->vSmoothCarFwd).y;
    local_150.v0.z = (this->vSmoothCarFwd).z;
    local_150.v0.w = 0.0;
    bCross((bVector3 *)&local_150.v1,(bVector3 *)&local_150.v2,(bVector3 *)&local_150);
    bCross((bVector3 *)&local_150.v2,(bVector3 *)&local_150,(bVector3 *)&local_150.v1);
    local_150.v3.x = (this->vSmoothCarPos).x;
    local_150.v3.y = (this->vSmoothCarPos).y;
    local_150.v3.z = (this->vSmoothCarPos).z;
  }
  else {
    local_150.v0.y = (this->mHybridToWorld).v0.y;
    local_150.v0.z = (this->mHybridToWorld).v0.z;
    local_150.v0.w = (this->mHybridToWorld).v0.w;
    local_150.v1.x = (this->mHybridToWorld).v1.x;
    local_150.v0.x = (this->mHybridToWorld).v0.x;
    local_150.v1.y = (this->mHybridToWorld).v1.y;
    local_150.v1.z = (this->mHybridToWorld).v1.z;
    local_150.v1.w = (this->mHybridToWorld).v1.w;
    local_150.v2.x = (this->mHybridToWorld).v2.x;
    local_150.v2.y = (this->mHybridToWorld).v2.y;
    local_150.v2.z = (this->mHybridToWorld).v2.z;
    local_150.v2.w = (this->mHybridToWorld).v2.w;
    pIVar13 = this->pCar;
    local_150.v3.z = (pIVar13->mGeomPos).z;
    local_150.v3.x = (pIVar13->mGeomPos).x;
    local_150.v3.y = (pIVar13->mGeomPos).y;
  }
  local_150.v3.w = 1.0;
  iVar6 = this->nSpaceEye;
  if (iVar6 == 2) {
    pIVar13 = this->pCar;
    local_170.x = local_170.x + (pIVar13->mGeomPos).x;
    local_170.y = local_170.y + (pIVar13->mGeomPos).y;
    local_170.z = local_170.z + (pIVar13->mGeomPos).z;
  }
  else if (iVar6 < 3) {
    if (iVar6 == 0) {
      m = &local_150;
LAB_8007c998:
      bMulMatrix(&local_170,m,&local_170);
    }
  }
  else {
    m = m_00;
    if (iVar6 == 3) goto LAB_8007c998;
  }
  iVar6 = this->nSpaceLook;
  if (iVar6 == 2) {
    pIVar13 = this->pCar;
    local_160.x = local_160.x + (pIVar13->mGeomPos).x;
    local_160.y = local_160.y + (pIVar13->mGeomPos).y;
    local_160.z = local_160.z + (pIVar13->mGeomPos).z;
  }
  else {
    if (iVar6 < 3) {
      if (iVar6 != 0) goto LAB_8007ca54;
      m_00 = &local_150;
    }
    else if (iVar6 != 3) goto LAB_8007ca54;
    bMulMatrix(&local_160,m_00,&local_160);
  }
LAB_8007ca54:
  if ((this->pICEData != (ICEData *)0x0) && (this->pICEData->bCarSpaceLag != '\0')) {
    local_110.x = 0.0;
    local_110.y = 0.0;
    local_110.z = 0.0;
    local_104 = 0;
    Cubic3D__SetValDesired(this->pAccelOffset,&this->pCar->mAccel);
    Cubic3D__Update(this->pAccelOffset,dT * fVar1,0.0,0.0);
    Cubic3D__GetVal(this->pAccelOffset,&local_110);
    fVar19 = local_110.x * vIceAccelLagScale.x;
    if (local_110.x * vIceAccelLagScale.x - vIceAccelLagMin.x < 0.0) {
      fVar19 = vIceAccelLagMin.x;
    }
    local_110.x = fVar19;
    if (vIceAccelLagMax.x - fVar19 < 0.0) {
      local_110.x = vIceAccelLagMax.x;
    }
    fVar19 = local_110.y * vIceAccelLagScale.y;
    if (local_110.y * vIceAccelLagScale.y - vIceAccelLagMin.y < 0.0) {
      fVar19 = vIceAccelLagMin.y;
    }
    fVar20 = local_110.z * vIceAccelLagScale.z;
    if (local_110.z * vIceAccelLagScale.z - vIceAccelLagMin.z < 0.0) {
      fVar20 = vIceAccelLagMin.z;
    }
    local_110.y = fVar19;
    if (vIceAccelLagMax.y - fVar19 < 0.0) {
      local_110.y = vIceAccelLagMax.y;
    }
    local_110.z = fVar20;
    if (vIceAccelLagMax.z - fVar20 < 0.0) {
      local_110.z = vIceAccelLagMax.z;
    }
    local_160.x = local_160.x - local_110.x;
    local_160.y = local_160.y - local_110.y;
    local_160.z = local_160.z - local_110.z;
    local_170.x = local_170.x - local_110.x;
    local_170.y = local_170.y - local_110.y;
    local_170.z = local_170.z - local_110.z;
  }
  iVar6 = TheICEManager.nState;
  fVar19 = ICEMover__GetDutch(this,fVar22);
  local_60 = (longlong)(int)(fVar19 * 65536.0);
  local_100.v0.x = 0.0;
  local_100.v0.y = 0.0;
  local_100.v0.z = 0.0;
  local_100.v0.w = 0.0;
  local_100.v1.x = 0.0;
  local_100.v1.y = 0.0;
  local_100.v1.z = 0.0;
  local_100.v1.w = 0.0;
  local_100.v2.x = 0.0;
  local_100.v2.y = 0.0;
  local_100.v2.z = 0.0;
  local_100.v2.w = 0.0;
  local_100.v3.x = 0.0;
  local_100.v3.y = 0.0;
  local_100.v3.z = 0.0;
  local_100.v3.w = 0.0;
  CreateLookAtMatrix(&local_100,(Vector3 *)&local_170,(Vector3 *)&local_160,
                     (ushort)(int)(fVar19 * 65536.0));
  fVar20 = Cubic1D__GetVal(this->pNoiseAmplitude,fVar22);
  fVar19 = Cubic1D__GetVal(this->pNoiseFrequency,fVar22);
  if (fVar20 < 0.0) {
    fVar20 = 0.0;
  }
  if (fVar19 < 0.0) {
    fVar19 = 0.0;
  }
  if ((this->pICEData == (ICEData *)0x0) ||
     (shake_type = this->pICEData->nShakeType, shake_type == 0)) {
    pCVar14 = (this->__base).pCamera;
    (pCVar14->CurrentKey).NoiseAmplitude1.w = fVar20;
    (pCVar14->CurrentKey).NoiseAmplitude1.x = 0.0;
    (pCVar14->CurrentKey).NoiseAmplitude1.y = 0.0;
    (pCVar14->CurrentKey).NoiseAmplitude1.z = fVar20;
    pCVar14 = (this->__base).pCamera;
    (pCVar14->CurrentKey).NoiseFrequency1.w = fVar19 * 1.382;
    (pCVar14->CurrentKey).NoiseFrequency1.z = fVar19 * 1.618;
    (pCVar14->CurrentKey).NoiseFrequency1.x = 0.0;
    (pCVar14->CurrentKey).NoiseFrequency1.y = 0.0;
    pCVar14 = (this->__base).pCamera;
    (pCVar14->CurrentKey).NoiseAmplitude2.w = 0.0;
    (pCVar14->CurrentKey).NoiseAmplitude2.x = 0.0;
    (pCVar14->CurrentKey).NoiseAmplitude2.y = 0.0;
    (pCVar14->CurrentKey).NoiseAmplitude2.z = 0.0;
    pCVar14 = (this->__base).pCamera;
    (pCVar14->CurrentKey).NoiseFrequency2.w = 0.0;
    (pCVar14->CurrentKey).NoiseFrequency2.x = 0.0;
    (pCVar14->CurrentKey).NoiseFrequency2.y = 0.0;
    (pCVar14->CurrentKey).NoiseFrequency2.z = 0.0;
    fVar19 = TheICEManager.fParameterLength;
    if (!bVar11) {
      fVar19 = this_00->Length;
    }
    Camera__ApplyNoise((this->__base).pCamera,(bMatrix4 *)&local_100,fVar18 * fVar19,1.0);
  }
  else {
    pIVar9 = ICEManager__GetShakeTrack(&TheICEManager,shake_type);
    if (pIVar9 != (ICEShakeTrack *)0x0) {
      fVar21 = TheICEManager.fParameterLength;
      if (!bVar11) {
        fVar21 = this_00->Length;
      }
      iVar15 = (int)pIVar9->NumKeys;
      iVar16 = (int)(fVar18 * fVar21 * fVar19 * 30.0);
      local_60 = (longlong)iVar16;
      if (iVar15 < 1) {
        iVar16 = 0;
      }
      else {
        for (; iVar16 < 0; iVar16 = iVar16 + iVar15) {
        }
        iVar16 = iVar16 - (iVar16 / iVar15) * iVar15;
      }
      iVar5 = pIVar9->NumKeys + -1;
      iVar15 = 0;
      if (iVar16 > 0) {
        iVar15 = iVar16;
      }
      if (iVar5 < iVar15) {
        iVar15 = iVar5;
      }
      if (iVar16 == iVar15) {
        pIVar17 = pIVar9->Keys + iVar16;
      }
      else {
        pIVar17 = (ICEShakeData *)0x0;
      }
      local_110.y = pIVar17->q[1] * fVar20;
      local_b4 = 0.0;
      local_110.x = pIVar17->q[0] * fVar20;
      local_110.z = pIVar17->q[2] * fVar20;
      fVar18 = local_110.z * local_110.z + local_110.x * local_110.x + local_110.y * local_110.y;
      local_104 = 0;
      if (fVar18 > 5e-11) {
        fVar19 = 1.0 / SQRT(fVar18);
        fVar19 = -(fVar18 * fVar19 * fVar19 - 1.0) * fVar19 * 0.5 + fVar19;
        local_b4 = (-(fVar18 * fVar19 * fVar19 - 1.0) * fVar19 * 0.5 + fVar19) * fVar18;
      }
      local_b4 = 1.0 - local_b4;
      if (local_b4 <= 5e-11) {
        local_b4 = 0.0;
      }
      else {
        fVar18 = 1.0 / SQRT(local_b4);
        fVar18 = -(local_b4 * fVar18 * fVar18 - 1.0) * fVar18 * 0.5 + fVar18;
        local_b4 = (-(local_b4 * fVar18 * fVar18 - 1.0) * fVar18 * 0.5 + fVar18) * local_b4;
      }
      local_b0.v0.x = 0.0;
      local_b0.v0.y = 0.0;
      local_b0.v0.z = 0.0;
      local_b0.v0.w = 0.0;
      local_b0.v1.x = 0.0;
      local_b0.v1.y = 0.0;
      local_b0.v1.z = 0.0;
      local_b0.v1.w = 0.0;
      local_b0.v2.x = 0.0;
      local_b0.v2.y = 0.0;
      local_b0.v2.z = 0.0;
      local_b0.v2.w = 0.0;
      local_b0.v3.x = 0.0;
      local_b0.v3.y = 0.0;
      local_b0.v3.z = 0.0;
      local_b0.v3.w = 0.0;
      local_c0 = local_110.x;
      local_bc = local_110.y;
      local_b8 = local_110.z;
      local_70 = local_110.x;
      local_6c = local_110.y;
      local_68 = local_110.z;
      local_64 = local_b4;
      GetMatrix__C11bQuaternionR8bMatrix4(&local_70,&local_b0);
      local_b0.v3.z = pIVar17->p[2] * fVar20;
      local_b0.v3.x = pIVar17->p[0] * fVar20;
      local_64 = 0.0;
      local_b0.v3.y = pIVar17->p[1] * fVar20;
      local_b0.v3.w = 1.0;
      local_70 = local_b0.v3.x;
      local_6c = local_b0.v3.y;
      local_68 = local_b0.v3.z;
      bMulMatrix((bMatrix4 *)&local_100,&local_b0,(bMatrix4 *)&local_100);
    }
  }
  if ((bVar11) || (iVar16 = ICETrack__GetContext(this_00), iVar16 != 2)) {
    bVar11 = false;
    if ((this->pICEData != (ICEData *)0x0) && (this->pICEData->bConstrainToWorld != '\0')) {
      bVar11 = iVar6 < 9;
    }
    if (bVar11) {
      CameraMover__MinGapTopology(&this->__base,(bMatrix4 *)&local_100,(bVector3 *)this->pCar);
    }
    bVar11 = false;
    if ((this->pICEData != (ICEData *)0x0) && (this->pICEData->bConstrainToCars != '\0')) {
      bVar11 = iVar6 < 9;
    }
    if (bVar11) {
      CameraMover__MinGapCars
                (&this->__base,(bMatrix4 *)&local_100,(bVector3 *)this->pCar,
                 (bVector3 *)&this->pCar->mVelocity);
    }
  }
  else {
    bVar11 = CameraMover__MinGapTopology
                       (&this->__base,(bMatrix4 *)&local_100,(bVector3 *)this->pCar);
    *(uint *)&this->bViolatesTopology = (uint)bVar11;
    bVar11 = CameraMover__MinGapCars
                       (&this->__base,(bMatrix4 *)&local_100,(bVector3 *)this->pCar,
                        (bVector3 *)&this->pCar->mVelocity);
    *(uint *)&this->bViolatesTopology = (uint)(*(int *)&this->bViolatesTopology != 0 || bVar11);
  }
  fVar18 = Cubic1D__GetVal(this->pLetterbox,fVar22);
  if (fVar18 >= 0.0) {
    (((this->__base).pCamera)->CurrentKey).LB_height = fVar18 * 0.00125;
  }
  fVar18 = Cubic1D__GetVal(this->pAperture,fVar22);
  if ((fVar18 < 0.0) || (local_60 = 0x4330000080000025, fVar18 >= 37.0)) {
    fVar18 = 0.0;
    if (_6Camera_StopUpdating == 0) {
      (((this->__base).pCamera)->CurrentKey).FocalDistance = 0.0;
    }
  }
  else {
    fVar19 = Cubic1D__GetVal(this->pFocalDistance,fVar22);
    if (fVar19 < 0.01) {
      fVar19 = 0.01;
    }
    fVar19 = fVar19 * 1000.0;
    uVar10 = ICEMover__GetFOV(this,fVar22);
    fVar21 = ConvertFovAngleToLensLength(uVar10);
    fVar22 = ConvertApertureNumberToFStop(fVar18);
    fVar18 = 100000.0;
    fVar20 = (fVar21 * fVar21) / (fVar22 * 0.03) + fVar21;
    fVar22 = (fVar20 - fVar21) * fVar19;
    fVar21 = (fVar22 / ((fVar20 + fVar19) - (fVar21 + fVar21))) * 0.001;
    if (fVar20 - fVar19 > 1e-05) {
      fVar18 = (fVar22 / (fVar20 - fVar19)) * 0.001;
    }
    if (fVar18 < fVar21) {
      fVar18 = fVar21 + 0.01;
    }
    if (_6Camera_StopUpdating == 0) {
      (((this->__base).pCamera)->CurrentKey).FocalDistance = (fVar18 + fVar21) * 0.5;
    }
    fVar18 = fVar18 - fVar21;
  }
  if (_6Camera_StopUpdating == 0) {
    (((this->__base).pCamera)->CurrentKey).DepthOfField = fVar18;
  }
  fVar18 = bDistBetween(&local_170,&local_160);
  if (_6Camera_StopUpdating == 0) {
    (((this->__base).pCamera)->CurrentKey).TargetDistance = fVar18;
  }
  SetCameraMatrix((this->__base).pCamera,(bMatrix4 *)&local_100,dT * fVar1);
  return;
}



/* ================================================================== */
/* 8008095c  staticinit_zCamera
   tam=3604 B  insn=901  cobertura=3604/3604  paired-singles=18  firma=IMPORTED
   proto: undefined __static_initialization_and_destruction_0(int __initialize_p, int __priority)
*/

/* WARNING: Removing unreachable block (ram,0x80081764) */
/* WARNING: Removing unreachable block (ram,0x80081760) */
/* WARNING: Removing unreachable block (ram,0x8008175c) */
/* WARNING: Removing unreachable block (ram,0x80081758) */
/* WARNING: Removing unreachable block (ram,0x80081754) */
/* WARNING: Removing unreachable block (ram,0x80081750) */
/* WARNING: Removing unreachable block (ram,0x8008174c) */
/* WARNING: Removing unreachable block (ram,0x80081748) */
/* WARNING: Removing unreachable block (ram,0x80081744) */
/* WARNING: Removing unreachable block (ram,0x80080984) */
/* WARNING: Removing unreachable block (ram,0x80080980) */
/* WARNING: Removing unreachable block (ram,0x8008097c) */
/* WARNING: Removing unreachable block (ram,0x80080978) */
/* WARNING: Removing unreachable block (ram,0x80080974) */
/* WARNING: Removing unreachable block (ram,0x80080970) */
/* WARNING: Removing unreachable block (ram,0x8008096c) */
/* WARNING: Removing unreachable block (ram,0x80080968) */
/* WARNING: Removing unreachable block (ram,0x80080964) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Unknown calling convention -- yet parameter storage is locked */
/* WARNING: Enum "EVIEW_ID": Some values do not have unique names */

void __static_initialization_and_destruction_0(int __initialize_p,int __priority)

{
  IPlayer **ppIVar1;
  uint uVar2;
  uint uVar3;
  IPlayer **ppIVar4;
  int iVar5;
  
  if ((__priority == 0xffff) && (__initialize_p != 0)) {
    kFloatScaleDown = 8.507059e+37;
    aBaselineFovNoise = 0x2aaa;
    CameraNoiseHandheldFrequency.x = 0.213;
    CameraNoiseHandheldFrequency.w = 0.192;
    CameraNoiseChopperFrequency.x = 3.141;
    CameraNoiseChopperFrequency.w = 0.92345;
    kFloatScaleUp = 1.1754944e-38;
    _6Camera_JollyRancherResponse.UseMatrix = 0;
    CameraNoiseHandheldFrequency.y = 0.175;
    CameraNoiseHandheldFrequency.z = 0.153;
    CameraNoiseHandheldAmplitude.x = 0.01;
    CameraNoiseHandheldAmplitude.w = 0.03;
    CameraNoiseChopperFrequency.y = 2.971;
    CameraNoiseChopperFrequency.z = 0.84234;
    CameraNoiseChopperAmplitude.x = 0.01;
    CameraNoiseChopperAmplitude.y = 0.05;
    CameraNoiseHandheldAmplitude.y = 0.01;
    CameraNoiseHandheldAmplitude.z = 0.03;
    CameraNoiseChopperAmplitude.z = 1.1;
    CameraNoiseChopperAmplitude.w = 2.7;
    CameraNoiseSpeedFrequency.x = 1.8;
    CameraNoiseSpeedFrequency.w = 2.0;
    CameraNoiseSpeedAmplitude.w = 0.28;
    CameraNoiseTerrainFrequency.x = 3.0;
    CameraNoiseTerrainFrequency.w = 5.5;
    CameraNoiseTerrainAmplitude.x = 0.007;
    CameraNoiseTerrainAmplitude.w = 0.4;
    CameraNoiseSpeedFrequency.z = 2.125;
    CameraNoiseSpeedAmplitude.x = 0.03;
    CameraNoiseSpeedAmplitude.y = 0.025;
    CameraNoiseSpeedAmplitude.z = 0.68;
    CameraNoiseTerrainFrequency.y = 5.0;
    CameraNoiseTerrainFrequency.z = 7.0;
    CameraNoiseTerrainAmplitude.y = 0.01;
    CameraNoiseTerrainAmplitude.z = 0.3;
    CameraNoiseSpeedData[0].x = 0.0;
    CameraNoiseSpeedData[0].y = 0.0;
    CameraNoiseSpeedFrequency.y = 2.0;
    CameraNoiseSpeedData[0].z = 0.6;
    CameraNoiseSpeedData[2].x = 0.03;
    CameraNoiseSpeedData[1].x = 0.005;
    CameraNoiseSpeedData[1].w = 1.1;
    CameraNoiseSpeedData[0].w = 1.0;
    CameraNoiseSpeedData[1].z = 1.1;
    CameraNoiseSpeedData[1].y = 0.5;
    CameraNoiseSpeedData[2].y = 0.8;
    CameraNoiseSpeedData[2].w = 1.2;
    CameraNoiseSpeedData[2].z = 1.2;
    CameraNoiseSpeedData[3].x = 0.07;
    CameraNoiseSpeedData[3].w = 1.3;
    CameraNoiseSpeedData[3].y = 1.0;
    CameraNoiseSpeedData[3].z = 1.3;
    CameraNoiseSpeedData[4].x = 0.02;
    _Physics_System_CameraAI.mInit = Init;
    CameraNoiseSpeedData[4].w = 1.4;
    CameraNoiseSpeedData[4].z = 1.0;
    _Physics_System_CameraAI.mRestore = Shutdown;
    CameraNoiseSpeedData[4].y = 1.0;
    _Physics_System_CameraAI.mSig.mCRC = stringhash32("CameraAI");
    _Physics_System_CameraAI.mNext = _Q23Sim9SubSystem_mHead;
    _Physics_System_CameraAI.mName = "CameraAI";
    _Q33UTL11Collectionst8Listable2ZQ28CameraAI8Directori2__mTable.__base.__base.__base.mBegin =
         (IPlayer **)0x0;
    _Q33UTL11Collectionst8Listable2ZQ28CameraAI8Directori2__mTable.__base.__base.__base.mCapacity =
         0;
    _Q33UTL11Collectionst8Listable2ZQ28CameraAI8Directori2__mTable.__base.__base.__base.mSize = 0;
    _Q23Sim9SubSystem_mHead = &_Physics_System_CameraAI;
    _Q33UTL11Collectionst8Listable2ZQ28CameraAI8Directori2__mTable.__base.__base.__base.
    _vptr_Vector<IPlayer_*,16> =
         (__vtbl_ptr_type *)_vt_Q43UTL11Collectionst8Listable2ZQ28CameraAI8Directori2_4List;
    OnGrowRequest__Q23UTLt6Vector2ZPQ28CameraAI8Directori16Ui
              (&_Q33UTL11Collectionst8Listable2ZQ28CameraAI8Directori2__mTable,2);
    uVar3 = _Q33UTL11Collectionst8Listable2ZQ28CameraAI8Directori2__mTable.__base.__base.__base.
            mSize;
    uVar2 = _Q33UTL11Collectionst8Listable2ZQ28CameraAI8Directori2__mTable.__base.__base.__base.
            mCapacity;
    ppIVar1 = _Q33UTL11Collectionst8Listable2ZQ28CameraAI8Directori2__mTable.__base.__base.__base.
              mBegin;
    _Q33UTL11Collectionst8Listable2ZQ28CameraAI8Directori2__mTable.__base.__base.__base.mBegin =
         (IPlayer **)
         (**(code **)&_Q33UTL11Collectionst8Listable2ZQ28CameraAI8Directori2__mTable.__base.__base.
                      __base._vptr_Vector<IPlayer_*,16>[2].__delta2)
                   ((int)_Q33UTL11Collectionst8Listable2ZQ28CameraAI8Directori2__mTable.__base.
                         __base.mVectorSpace +
                    _Q33UTL11Collectionst8Listable2ZQ28CameraAI8Directori2__mTable.__base.__base.
                    __base._vptr_Vector<IPlayer_*,16>[2].__delta + -0x10,2,0x10);
    _Q33UTL11Collectionst8Listable2ZQ28CameraAI8Directori2__mTable.__base.__base.__base.mCapacity =
         2;
    if (ppIVar1 !=
        _Q33UTL11Collectionst8Listable2ZQ28CameraAI8Directori2__mTable.__base.__base.__base.mBegin)
    {
      iVar5 = 0;
      _Q33UTL11Collectionst8Listable2ZQ28CameraAI8Directori2__mTable.__base.__base.__base.mSize = 0;
      if ((int)uVar3 > 0) {
        do {
          ppIVar4 = ppIVar1 + iVar5;
          iVar5 = iVar5 + 1;
          push_back__Q23UTLt6Vector2ZPQ28CameraAI8Directori16RCPQ28CameraAI8Director
                    (&_Q33UTL11Collectionst8Listable2ZQ28CameraAI8Directori2__mTable,ppIVar4);
        } while (iVar5 < (int)uVar3);
      }
      if (ppIVar1 != (IPlayer **)0x0) {
        (**(code **)&_Q33UTL11Collectionst8Listable2ZQ28CameraAI8Directori2__mTable.__base.__base.
                     __base._vptr_Vector<IPlayer_*,16>[3].__delta2)
                  ((int)_Q33UTL11Collectionst8Listable2ZQ28CameraAI8Directori2__mTable.__base.__base
                        .mVectorSpace +
                   _Q33UTL11Collectionst8Listable2ZQ28CameraAI8Directori2__mTable.__base.__base.
                   __base._vptr_Vector<IPlayer_*,16>[3].__delta + -0x10,ppIVar1,uVar2);
      }
    }
    _Q33UTL11Collectionst8Listable2ZQ28CameraAI8Directori2__mTable.__base.__base.mVectorSpace[2] =
         stringhash32("CDActionDrive");
    _Q33UTL11Collectionst8Listable2ZQ28CameraAI8Directori2__mTable.__base.__base.mVectorSpace[4] =
         (int)
         _Q43UTL3COMt7Factory3ZPQ28CameraAI8DirectorZQ28CameraAI6ActionZ6UCrc32_9Prototype_mHead;
    _Q33UTL11Collectionst8Listable2ZQ28CameraAI8Directori2__mTable.__base.__base.mVectorSpace[3] =
         (int)CDActionDrive__Construct;
    _Q43UTL3COMt7Factory3ZPQ28CameraAI8DirectorZQ28CameraAI6ActionZ6UCrc32_9Prototype_mHead =
         (Prototype *)
         (_Q33UTL11Collectionst8Listable2ZQ28CameraAI8Directori2__mTable.__base.__base.mVectorSpace
         + 2);
    _Q33UTL11Collectionst8Listable2ZQ28CameraAI8Directori2__mTable.__base.__base.mVectorSpace[5] =
         stringhash32("CDActionTrackCar");
    _Q33UTL11Collectionst8Listable2ZQ28CameraAI8Directori2__mTable.__base.__base.mVectorSpace[7] =
         (int)
         _Q43UTL3COMt7Factory3ZPQ28CameraAI8DirectorZQ28CameraAI6ActionZ6UCrc32_9Prototype_mHead;
    _Q33UTL11Collectionst8Listable2ZQ28CameraAI8Directori2__mTable.__base.__base.mVectorSpace[6] =
         (int)CDActionTrackCar__Construct;
    _Q43UTL3COMt7Factory3ZPQ28CameraAI8DirectorZQ28CameraAI6ActionZ6UCrc32_9Prototype_mHead =
         (Prototype *)
         (_Q33UTL11Collectionst8Listable2ZQ28CameraAI8Directori2__mTable.__base.__base.mVectorSpace
         + 5);
    _CDActionTrackCop.mSignature.mCRC = stringhash32("CDActionTrackCop");
    _CDActionTrackCop.mTail =
         _Q43UTL3COMt7Factory3ZPQ28CameraAI8DirectorZQ28CameraAI6ActionZ6UCrc32_9Prototype_mHead;
    _CDActionTrackCop.mConstructor = CDActionTrackCop__Construct;
    _Q43UTL3COMt7Factory3ZPQ28CameraAI8DirectorZQ28CameraAI6ActionZ6UCrc32_9Prototype_mHead =
         &_CDActionTrackCop;
    _CDActionShowcase.mSignature.mCRC = stringhash32("CDActionShowcase");
    _CDActionShowcase.mTail =
         _Q43UTL3COMt7Factory3ZPQ28CameraAI8DirectorZQ28CameraAI6ActionZ6UCrc32_9Prototype_mHead;
    _CDActionShowcase.mConstructor = CDActionShowcase__Construct;
    _Q43UTL3COMt7Factory3ZPQ28CameraAI8DirectorZQ28CameraAI6ActionZ6UCrc32_9Prototype_mHead =
         &_CDActionShowcase;
    _CDActionDebug.mSignature.mCRC = stringhash32("CDActionDebug");
    _CDActionDebug.mTail =
         _Q43UTL3COMt7Factory3ZPQ28CameraAI8DirectorZQ28CameraAI6ActionZ6UCrc32_9Prototype_mHead;
    _CDActionDebug.mConstructor = CDActionDebug__Construct;
    _Q43UTL3COMt7Factory3ZPQ28CameraAI8DirectorZQ28CameraAI6ActionZ6UCrc32_9Prototype_mHead =
         &_CDActionDebug;
    _CDActionIce.mSignature.mCRC = stringhash32("CDActionIce");
    _CDActionIce.mTail =
         _Q43UTL3COMt7Factory3ZPQ28CameraAI8DirectorZQ28CameraAI6ActionZ6UCrc32_9Prototype_mHead;
    _Q33UTL11Collectionst8Listable2Z14IDebugWatchCari2__mTable.__base.__base.__base.mBegin =
         (IPlayer **)0x0;
    _Q33UTL11Collectionst8Listable2Z14IDebugWatchCari2__mTable.__base.__base.__base.mCapacity = 0;
    _Q33UTL11Collectionst8Listable2Z14IDebugWatchCari2__mTable.__base.__base.__base.mSize = 0;
    _Q43UTL3COMt7Factory3ZPQ28CameraAI8DirectorZQ28CameraAI6ActionZ6UCrc32_9Prototype_mHead =
         &_CDActionIce;
    _CDActionIce.mConstructor = CDActionIce__Construct;
    _Q33UTL11Collectionst8Listable2Z14IDebugWatchCari2__mTable.__base.__base.__base.
    _vptr_Vector<IPlayer_*,16> = (__vtbl_ptr_type *)&ICE_SceneLocations_ffffffff803d3b4c[2].y;
    OnGrowRequest__Q23UTLt6Vector2ZP14IDebugWatchCari16Ui
              (&_Q33UTL11Collectionst8Listable2Z14IDebugWatchCari2__mTable,2);
    uVar3 = _Q33UTL11Collectionst8Listable2Z14IDebugWatchCari2__mTable.__base.__base.__base.mSize;
    uVar2 = _Q33UTL11Collectionst8Listable2Z14IDebugWatchCari2__mTable.__base.__base.__base.
            mCapacity;
    ppIVar1 = _Q33UTL11Collectionst8Listable2Z14IDebugWatchCari2__mTable.__base.__base.__base.mBegin
    ;
    _Q33UTL11Collectionst8Listable2Z14IDebugWatchCari2__mTable.__base.__base.__base.mBegin =
         (IPlayer **)
         (**(code **)&_Q33UTL11Collectionst8Listable2Z14IDebugWatchCari2__mTable.__base.__base.
                      __base._vptr_Vector<IPlayer_*,16>[2].__delta2)
                   ((int)_Q33UTL11Collectionst8Listable2Z14IDebugWatchCari2__mTable.__base.__base.
                         mVectorSpace +
                    _Q33UTL11Collectionst8Listable2Z14IDebugWatchCari2__mTable.__base.__base.__base.
                    _vptr_Vector<IPlayer_*,16>[2].__delta + -0x10,2,0x10);
    _Q33UTL11Collectionst8Listable2Z14IDebugWatchCari2__mTable.__base.__base.__base.mCapacity = 2;
    if (ppIVar1 !=
        _Q33UTL11Collectionst8Listable2Z14IDebugWatchCari2__mTable.__base.__base.__base.mBegin) {
      iVar5 = 0;
      _Q33UTL11Collectionst8Listable2Z14IDebugWatchCari2__mTable.__base.__base.__base.mSize = 0;
      if ((int)uVar3 > 0) {
        do {
          ppIVar4 = ppIVar1 + iVar5;
          iVar5 = iVar5 + 1;
          push_back__Q23UTLt6Vector2ZP14IDebugWatchCari16RCP14IDebugWatchCar
                    (&_Q33UTL11Collectionst8Listable2Z14IDebugWatchCari2__mTable,ppIVar4);
        } while (iVar5 < (int)uVar3);
      }
      if (ppIVar1 != (IPlayer **)0x0) {
        (**(code **)&_Q33UTL11Collectionst8Listable2Z14IDebugWatchCari2__mTable.__base.__base.__base
                     ._vptr_Vector<IPlayer_*,16>[3].__delta2)
                  ((int)_Q33UTL11Collectionst8Listable2Z14IDebugWatchCari2__mTable.__base.__base.
                        mVectorSpace +
                   _Q33UTL11Collectionst8Listable2Z14IDebugWatchCari2__mTable.__base.__base.__base.
                   _vptr_Vector<IPlayer_*,16>[3].__delta + -0x10,ppIVar1,uVar2);
      }
    }
    _Q33UTL11Collectionst8Listable2Z14IDebugWatchCari2__mTable.__base.__base.mVectorSpace[2] =
         stringhash32("CDActionDebugWatchCar");
    _Q33UTL11Collectionst8Listable2Z14IDebugWatchCari2__mTable.__base.__base.mVectorSpace[4] =
         (int)
         _Q43UTL3COMt7Factory3ZPQ28CameraAI8DirectorZQ28CameraAI6ActionZ6UCrc32_9Prototype_mHead;
    _Q33UTL11Collectionst8Listable2Z14IDebugWatchCari2__mTable.__base.__base.mVectorSpace[5] =
         0x41000000;
    _Q43UTL3COMt7Factory3ZPQ28CameraAI8DirectorZQ28CameraAI6ActionZ6UCrc32_9Prototype_mHead =
         (Prototype *)
         (_Q33UTL11Collectionst8Listable2Z14IDebugWatchCari2__mTable.__base.__base.mVectorSpace + 2)
    ;
    _Q33UTL11Collectionst8Listable2Z14IDebugWatchCari2__mTable.__base.__base.mVectorSpace[3] =
         (int)CDActionDebugWatchCar__Construct;
    _Q33UTL11Collectionst8Listable2Z14IDebugWatchCari2__mTable.__base.__base.mVectorSpace[7] =
         0x41500000;
    _Q33UTL11Collectionst8Listable2Z14IDebugWatchCari2__mTable.__base.__base.mVectorSpace[6] = 0;
    _DAT_8045acac = 0x3f800000;
    Graph(&gDriftSpeed,
          (bVector2 *)
          (_Q33UTL11Collectionst8Listable2Z14IDebugWatchCari2__mTable.__base.__base.mVectorSpace + 5
          ),2);
    aCubicPovTables[0].__base.NumEntries = 2;
    SetMinMax__9TableBaseff(0,0x3ff0000000000000,aCubicPovTables);
    aCubicPovTables[0].pTable = CubicBumper;
    aCubicPovTables[1].__base.NumEntries = 2;
    SetMinMax__9TableBaseff(0,0x3ff0000000000000,0x8045accc);
    aCubicPovTables[1].pTable = CubicHood;
    aCubicPovTables[2].__base.NumEntries = 2;
    SetMinMax__9TableBaseff(0,0x3ff0000000000000,0x8045ace0);
    aCubicPovTables[2].pTable = CubicOutsideNear;
    aCubicPovTables[3].__base.NumEntries = 2;
    SetMinMax__9TableBaseff(0,0x3ff0000000000000,0x8045acf4);
    aCubicPovTables[3].pTable = CubicOutsideFar;
    aCubicPovTables[4].__base.NumEntries = 2;
    SetMinMax__9TableBaseff(0,0x3ff0000000000000,0x8045ad08);
    aCubicPovTables[4].pTable = CubicSuperFar;
    aCubicPovTables[5].__base.NumEntries = 2;
    SetMinMax__9TableBaseff(0,0x3ff0000000000000,0x8045ad1c);
    aCubicPovTables[5].pTable = CubicDrift;
    aCubicPovTables[6].__base.NumEntries = 2;
    SetMinMax__9TableBaseff(0,0x3ff0000000000000,0x8045ad30);
    CameraSpeedHugData[0].x = 1.0;
    CameraSpeedHugData[2].x = 1.02;
    CameraSpeedHugData[0].y = 1.0;
    CameraSpeedHugData[1].x = 1.0;
    CameraSpeedHugData[1].y = 1.0;
    CameraSpeedHugData[2].y = 1.0;
    CameraSpeedHugData[3].x = 1.125;
    CameraSpeedHugData[3].y = 1.0;
    CameraSpeedHugData[4].x = 1.2;
    vCubicBirdsEyeOffset[0].x = -17.2;
    vCubicBirdsEyeOffset[1].z = 4.4;
    SmokeShowEyeOffset.x = -10.0;
    SmokeShowEyeOffset.z = 5.0;
    SmokeShowLookAngle = 0xbbb;
    HydraulicsEyeOffset.x = -5.0;
    aCubicPovTables[6].pTable = CubicPursuit;
    CameraSpeedHugData[4].y = 1.0;
    vCubicBirdsEyeOffset[1].x = -15.8;
    vCubicBirdsEyeOffset[0].y = 0.0;
    vCubicBirdsEyeOffset[0].z = 6.0;
    vCubicBirdsEyeOffset[1].y = 0.0;
    SmokeShowEyeOffset.y = 0.0;
    HydraulicsEyeOffset.y = 0.0;
    NOSFovWidening = 0x666;
    HydraulicsLookAngle = 0x38e;
    Demo1EyeOffset.x = 0.78035;
    Demo1EyeOffset.z = 0.89595;
    Demo2EyeOffset.x = 1.35838;
    Demo2EyeOffset.z = 0.20231;
    Demo2LookOffset.x = 0.43353;
    HydraulicsEyeOffset.z = 1.5;
    Demo1EyeOffset.y = -3.09249;
    Demo1LookOffset.x = 0.20231;
    Demo1LookOffset.z = 0.0;
    Demo2EyeOffset.y = -1.35838;
    Demo2LookOffset.y = 1.76301;
    Demo2LookOffset.z = 0.0;
    PreviousEye.x = 0.0;
    PreviousEye.z = 0.0;
    vCopViewPivot.x = 1.0;
    vCopViewPivot.z = 2.0;
    vCopViewDistanceFovBand[0].x = 1.5;
    Demo1LookOffset.y = 0.0;
    PreviousEye.y = 0.0;
    vCopViewPivot.y = 0.0;
    vCopViewDistanceFovBand[0].y = 12000.0;
    tCopViewDistanceFovBand.__base.NumEntries = 2;
    vCopViewDistanceFovBand[0].z = 0.5;
    vCopViewDistanceFovBand[1].x = 1.2;
    vCopViewDistanceFovBand[1].y = 6000.0;
    vCopViewDistanceFovBand[1].z = 0.5;
    SetMinMax__9TableBaseff(0,0x3ff0000000000000,&tCopViewDistanceFovBand);
    vCopViewPoints[2].x = -2.0;
    vCopViewPoints[0].x = -2.0;
    vCopViewPoints[1].x = -2.0;
    vCopViewPoints[1].z = 1.3;
    vCopViewPoints[0].y = -3.0;
    vCopViewPoints[1].y = -2.5;
    vCopViewPoints[0].z = 1.2;
    vCopViewPoints[2].z = 1.5;
    vCopViewPoints[2].y = 0.0;
    vCopViewPoints[3].x = -2.0;
    vCopViewPoints[3].z = 1.3;
    vCopViewPoints[3].y = 2.5;
    vCopViewPoints[4].x = -2.0;
    tCopViewDistanceFovBand.pTable = vCopViewDistanceFovBand;
    vCopViewPoints[4].y = 3.0;
    vCopViewPoints[4].z = 1.2;
    vCopViewDistanceFov[0].x = 2.0;
    vCopViewDistanceFov[0].y = 12000.0;
    vCopViewDistanceFov[1].x = 6.0;
    vCopViewDistanceFov[1].y = 6000.0;
    tCopViewPosition.__base.NumEntries = 5;
    SetMinMax__9TableBaseff(0,0x3ff0000000000000,&tCopViewPosition);
    tCopViewPosition.pTable = vCopViewPoints;
    tCopViewDistanceFov.__base.NumEntries = 2;
    SetMinMax__9TableBaseff(0,0x3ff0000000000000,&tCopViewDistanceFov);
    _21DebugWorldCameraMover_Eye.z = 100.0;
    gDebugCameraInputGraph.GraphData = fDebugCameraInputData;
    gDebugCameraInputGraph.NumEntries = 5;
    tCopViewDistanceFov.pTable = vCopViewDistanceFov;
    gDebugCameraTweakableEye.x = 0.0;
    gDebugCameraTweakableEye.z = 0.0;
    gDebugCameraTweakableLook.x = 0.0;
    gDebugCameraTweakableLook.z = 0.0;
    _21DebugWorldCameraMover_Eye.x = 20.0;
    _21DebugWorldCameraMover_Eye.y = 20.0;
    _21DebugWorldCameraMover_Look.x = 0.0;
    _21DebugWorldCameraMover_Look.z = 0.0;
    _21DebugWorldCameraMover_Up.x = 0.0;
    _21DebugWorldCameraMover_Up.z = 1.0;
    JumpToPosition.x = 0.0;
    JumpToPosition.z = 0.0;
    spline_points[0].x = 0.0;
    spline_points[1].x = 1000.0;
    spline_points[1].z = 0.0;
    RVMOffsetInCar.x = 0.0;
    RVMOffsetInCar.y = 0.5;
    RVMOffsetInCar.z = 0.0;
    gDebugCameraTweakableEye.y = 0.0;
    gDebugCameraTweakableLook.y = 0.0;
    _21DebugWorldCameraMover_Look.y = 0.0;
    _21DebugWorldCameraMover_Up.y = 0.0;
    JumpToPosition.y = 0.0;
    spline_points[0].y = 0.0;
    spline_points[0].z = 0.0;
    spline_points[1].y = 0.0;
    gPhoto_CarPosBias.x = 2.0;
    DAT_8045afd4 = 0;
    gPhoto_CarPosBias.z = 0.0;
    StillEyeTweak.x = 0.0;
    StillEyeTweak.y = -80.0;
    StillEyeTweak.z = 0.0;
    StillLookTweak.x = 0.0;
    StillLookTweak.z = 0.0;
    StillUpTweak.x = 0.0;
    StillUpTweak.z = 1.0;
    vIceAccelLagMin.x = -1.0;
    vIceAccelLagMin.y = -1.0;
    vIceAccelLagMin.z = -0.2;
    DAT_8045afb4 = 0;
    vIceAccelLagMax.x = 1.0;
    vIceAccelLagMax.z = 1.0;
    DAT_8045afc4 = 0;
    vIceAccelLagScale.x = 0.05;
    vIceAccelLagScale.y = 0.05;
    vIceAccelLagScale.z = 0.02;
    gPhoto_CarPosBias.y = 0.0;
    StillLookTweak.y = 0.0;
    StillUpTweak.y = 0.0;
    vIceAccelLagMax.y = 1.0;
    ICEManager(&TheICEManager);
    _3ICE_ReplayCategoryTable[0].pCategoryName = "NOS";
    _3ICE_ReplayCategoryTable[0].ScoreFunction = ReplayNosScore;
    _3ICE_ReplayCategoryTable[0].MirrorFunction = ReplayNosMirror;
    _3ICE_ReplayCategoryTable[0].pSceneName = "ReplaySpeed";
    _3ICE_ReplayCategoryTable[0].nSceneHash = bStringHash("ReplaySpeed");
    _3ICE_ReplayCategoryTable[0].nCategoryHash = bStringHash("NOS");
    _3ICE_ReplayCategoryTable[1].ScoreFunction = ReplayJumpScore;
    _3ICE_ReplayCategoryTable[1].MirrorFunction = ReplayJumpMirror;
    _3ICE_ReplayCategoryTable[1].pCategoryName = "Jump";
    _3ICE_ReplayCategoryTable[1].pSceneName = "ReplaySpeed";
    _3ICE_ReplayCategoryTable[1].nSceneHash = bStringHash("ReplaySpeed");
    _3ICE_ReplayCategoryTable[1].nCategoryHash = bStringHash("Jump");
    _3ICE_ReplayCategoryTable[2].ScoreFunction = ReplaySpeedScore;
    _3ICE_ReplayCategoryTable[2].MirrorFunction = ReplaySpeedMirror;
    _3ICE_ReplayCategoryTable[2].pCategoryName = "Speed";
    _3ICE_ReplayCategoryTable[2].pSceneName = "ReplaySpeed";
    _3ICE_ReplayCategoryTable[2].nSceneHash = bStringHash("ReplaySpeed");
    _3ICE_ReplayCategoryTable[2].nCategoryHash = bStringHash("Speed");
    _3ICE_ReplayCategoryTable[3].ScoreFunction = ReplayCornerScore;
    _3ICE_ReplayCategoryTable[3].MirrorFunction = ReplayCornerMirror;
    _3ICE_ReplayCategoryTable[3].pSceneName = "ReplayCorner";
    _3ICE_ReplayCategoryTable[3].pCategoryName = "Corner";
    _3ICE_ReplayCategoryTable[3].nSceneHash = bStringHash("ReplayCorner");
    _3ICE_ReplayCategoryTable[3].nCategoryHash = bStringHash("Corner");
    _3ICE_ReplayCategoryTable[4].ScoreFunction = ReplayBurnoutScore;
    _3ICE_ReplayCategoryTable[4].MirrorFunction = ReplayBurnoutMirror;
    _3ICE_ReplayCategoryTable[4].pSceneName = "ReplayBurnout";
    _3ICE_ReplayCategoryTable[4].pCategoryName = "Burnout";
    _3ICE_ReplayCategoryTable[4].nSceneHash = bStringHash("ReplayBurnout");
    _3ICE_ReplayCategoryTable[4].nCategoryHash = bStringHash("Burnout");
    _3ICE_ReplayCategoryTable[5].ScoreFunction = ReplayPowerSlideScore;
    _3ICE_ReplayCategoryTable[5].MirrorFunction = ReplayPowerSlideMirror;
    _3ICE_ReplayCategoryTable[5].pSceneName = "ReplaySlide";
    _3ICE_ReplayCategoryTable[5].pCategoryName = "PowerSlide";
    _3ICE_ReplayCategoryTable[5].nSceneHash = bStringHash("ReplaySlide");
    _3ICE_ReplayCategoryTable[5].nCategoryHash = bStringHash("PowerSlide");
  }
  return;
}



/* ================================================================== */
/* 802feab0  HolePunchAvoidables__8WRoadNavP9NavCookieiff
   tam=2980 B  insn=745  cobertura=2980/2980  paired-singles=36  firma=IMPORTED
   proto: undefined WRoadNav::HolePunchAvoidables(WRoadNav * this, NavCookie * cookies, int num_cookies, float current_offset, float delta_offset)
*/

/* WARNING: Removing unreachable block (ram,0x802ff644) */
/* WARNING: Removing unreachable block (ram,0x802ff640) */
/* WARNING: Removing unreachable block (ram,0x802ff63c) */
/* WARNING: Removing unreachable block (ram,0x802ff638) */
/* WARNING: Removing unreachable block (ram,0x802ff634) */
/* WARNING: Removing unreachable block (ram,0x802ff630) */
/* WARNING: Removing unreachable block (ram,0x802ff62c) */
/* WARNING: Removing unreachable block (ram,0x802ff628) */
/* WARNING: Removing unreachable block (ram,0x802ff624) */
/* WARNING: Removing unreachable block (ram,0x802ff620) */
/* WARNING: Removing unreachable block (ram,0x802ff61c) */
/* WARNING: Removing unreachable block (ram,0x802ff618) */
/* WARNING: Removing unreachable block (ram,0x802ff614) */
/* WARNING: Removing unreachable block (ram,0x802ff610) */
/* WARNING: Removing unreachable block (ram,0x802ff60c) */
/* WARNING: Removing unreachable block (ram,0x802ff608) */
/* WARNING: Removing unreachable block (ram,0x802ff604) */
/* WARNING: Removing unreachable block (ram,0x802ff600) */
/* WARNING: Removing unreachable block (ram,0x802feb00) */
/* WARNING: Removing unreachable block (ram,0x802feafc) */
/* WARNING: Removing unreachable block (ram,0x802feaf8) */
/* WARNING: Removing unreachable block (ram,0x802feaf4) */
/* WARNING: Removing unreachable block (ram,0x802feaf0) */
/* WARNING: Removing unreachable block (ram,0x802feaec) */
/* WARNING: Removing unreachable block (ram,0x802feae8) */
/* WARNING: Removing unreachable block (ram,0x802feae4) */
/* WARNING: Removing unreachable block (ram,0x802feae0) */
/* WARNING: Removing unreachable block (ram,0x802feadc) */
/* WARNING: Removing unreachable block (ram,0x802fead8) */
/* WARNING: Removing unreachable block (ram,0x802fead4) */
/* WARNING: Removing unreachable block (ram,0x802fead0) */
/* WARNING: Removing unreachable block (ram,0x802feacc) */
/* WARNING: Removing unreachable block (ram,0x802feac8) */
/* WARNING: Removing unreachable block (ram,0x802feac4) */
/* WARNING: Removing unreachable block (ram,0x802feac0) */
/* WARNING: Removing unreachable block (ram,0x802feabc) */
/* WARNING: Unknown calling convention -- yet parameter storage is locked */
/* WARNING: Enum "SplineType": Some values do not have unique names */

void WRoadNav__HolePunchAvoidables
               (WRoadNav *this,NavCookie *cookies,int num_cookies,float current_offset,
               float delta_offset)

{
  float fVar1;
  float fVar2;
  bool bVar3;
  byte bVar4;
  bool bVar5;
  bool bVar6;
  Vector3 *pVVar7;
  float *pfVar8;
  IUnknown *pIVar9;
  Vector3 *position;
  int iVar10;
  Type TVar11;
  int iVar12;
  int *piVar13;
  __vtbl_ptr_type *p_Var14;
  uint cut_flags;
  int iVar15;
  NavCookie *cookie;
  undefined4 *puVar16;
  IBody *pIVar17;
  int iVar18;
  bool bVar19;
  bool bVar20;
  byte in_cr0;
  byte in_cr1;
  byte unaff_cr2;
  byte unaff_cr3;
  byte unaff_cr4;
  byte in_cr5;
  byte in_cr6;
  byte in_cr7;
  double dVar21;
  double dVar22;
  double dVar23;
  double dVar24;
  double dVar25;
  double dVar26;
  double dVar27;
  double dVar28;
  double dVar29;
  float fVar30;
  float fVar31;
  float fVar32;
  IBody *local_2e8 [32];
  Vector3 VStack_268;
  undefined1 auStack_258 [16];
  float local_248 [2];
  float local_240;
  Vector3 local_238 [2];
  Vector3 local_218;
  float local_208 [4];
  Vector3 local_1f8;
  float local_1e8 [2];
  float local_1e0;
  Vector3 local_1d8;
  Vector3 local_1c8;
  Vector3 local_1b8;
  Vector3 local_1a8;
  Vector3 local_198;
  Vector3 local_188;
  Vector3 local_178;
  Vector3 local_168;
  bVector2 local_158;
  float local_150 [2];
  float local_148;
  float local_144;
  float local_140;
  float local_13c;
  float local_138 [2];
  float local_130;
  float local_12c;
  float local_128 [2];
  int local_120;
  Vector3 *local_11c;
  int local_118;
  float *local_114;
  uint local_110;
  float *local_10c;
  float *local_108;
  float *local_104;
  Vector3 *local_100;
  float *local_fc;
  float *local_f8;
  float local_f0;
  float local_ec;
  IBody **local_e8;
  uint local_dc;
  
  local_dc = (uint)(in_cr0 & 0xf) << 0x1c | (uint)(in_cr1 & 0xf) << 0x18 |
             (uint)(unaff_cr2 & 0xf) << 0x14 | (uint)(unaff_cr3 & 0xf) << 0x10 |
             (uint)(unaff_cr4 & 0xf) << 0xc | (uint)(in_cr5 & 0xf) << 8 | (uint)(in_cr6 & 0xf) << 4
             | (uint)(in_cr7 & 0xf);
  if (num_cookies != 0) {
    puVar16 = (undefined4 *)0x0;
    if (this->pAIVehicle != (AIVehicle *)0x0) {
      puVar16 = (undefined4 *)&this->pAIVehicle->field_0x50;
    }
    if ((puVar16 != (undefined4 *)0x0) &&
       (local_120 = WRoadNav__FetchAvoidables(this,local_2e8,0x20), local_120 != 0)) {
      pIVar9 = _IList__Find((_IList *)*puVar16,_IHandle__10IRigidBody);
      position = (Vector3 *)
                 (**(code **)&pIVar9->_vptr_IUnknown[9].__delta2)
                           ((int)&pIVar9->_mCOMObject + (int)pIVar9->_vptr_IUnknown[9].__delta);
      iVar10 = WRoadNav__ClosestCookieAhead(this,position,cookies,num_cookies,(NavCookie *)0x0);
      if (iVar10 > -1) {
        bVar5 = false;
        local_118 = num_cookies;
        local_11c = (Vector3 *)
                    (**(code **)&pIVar9->_vptr_IUnknown[10].__delta2)
                              ((int)&pIVar9->_mCOMObject + (int)pIVar9->_vptr_IUnknown[10].__delta);
        (**(code **)&pIVar9->_vptr_IUnknown[0xf].__delta2)
                  ((int)&pIVar9->_mCOMObject + (int)pIVar9->_vptr_IUnknown[0xf].__delta,&VStack_268)
        ;
        (**(code **)&pIVar9->_vptr_IUnknown[0xe].__delta2)
                  ((int)&pIVar9->_mCOMObject + (int)pIVar9->_vptr_IUnknown[0xe].__delta,auStack_258)
        ;
        (**(code **)&pIVar9->_vptr_IUnknown[0x13].__delta2)
                  ((int)&pIVar9->_mCOMObject + (int)pIVar9->_vptr_IUnknown[0x13].__delta,local_248);
        dVar23 = (double)(**(code **)&pIVar9->_vptr_IUnknown[0xc].__delta2)
                                   ((int)&pIVar9->_mCOMObject +
                                    (int)pIVar9->_vptr_IUnknown[0xc].__delta);
        local_158.x = (this->fForwardVector).x;
        local_158.y = (this->fForwardVector).z;
        bNormalize(&local_158,&local_158);
        if ((this->fPathType == kPathRacer) || (this->fPathType == kPathPlayer)) {
          bVar5 = true;
        }
        bVar6 = false;
        bVar3 = this->fNavType == kTypeTraffic;
        if (_11GRaceStatus_fObj != (GRaceStatus *)0x0) {
          if (_11GRaceStatus_fObj->mRaceParms == (GRaceParameters *)0x0) {
            TVar11 = kRaceType_None;
          }
          else {
            TVar11 = GRaceParameters__GetRaceType(_11GRaceStatus_fObj->mRaceParms);
          }
          if (TVar11 == kRaceType_Drag) {
            bVar6 = true;
          }
        }
        iVar15 = 0;
        if (local_120 > 0) {
          local_110 = (uint)(byte)((dVar23 < 20.0) << 3 | (dVar23 > 20.0) << 2 |
                                   (dVar23 == 20.0) << 1 | NAN(dVar23)) << 0x1c |
                      (uint)(in_cr1 & 0xf) << 0x18 | (uint)(unaff_cr2 & 0xf) << 0x14 |
                      (uint)(unaff_cr3 & 0xf) << 0x10 | (uint)(unaff_cr4 & 0xf) << 0xc |
                      (uint)(in_cr5 & 0xf) << 8 | (uint)(in_cr6 & 0xf) << 4 | (uint)(in_cr7 & 0xf);
          local_e8 = local_2e8;
          local_114 = local_208;
          local_10c = &local_148;
          local_108 = &local_140;
          local_104 = local_138;
          local_100 = &local_168;
          local_fc = &local_130;
          local_f8 = local_128;
          do {
            pIVar17 = local_e8[iVar15];
            p_Var14 = (pIVar17->__base)._vptr_IUnknown;
            (**(code **)&p_Var14[2].__delta2)
                      ((int)&(pIVar17->__base)._mCOMObject + (int)p_Var14[2].__delta,local_238);
            local_1f8.y = local_114[1];
            local_1f8.z = local_114[2];
            local_1f8.x = local_208[0];
            fVar31 = local_1f8.y - cookies[iVar10].Centre.y;
            fVar30 = fVar31;
            if (fVar31 - -5.0 < 0.0) {
              fVar30 = -5.0;
            }
            if (5.0 - fVar30 < 0.0) {
              fVar30 = 5.0;
            }
            if (fVar31 == fVar30) {
              p_Var14 = (pIVar17->__base)._vptr_IUnknown;
              (**(code **)&p_Var14[5].__delta2)
                        ((int)&(pIVar17->__base)._mCOMObject + (int)p_Var14[5].__delta,local_1e8);
              pIVar9 = _IList__Find(&((pIVar17->__base)._mCOMObject)->_mInterfaces,
                                    _IHandle__8IVehicle);
              if (pIVar9 == (IUnknown *)0x0) {
                iVar12 = 4;
              }
              else {
                iVar12 = (**(code **)&pIVar9->_vptr_IUnknown[0x17].__delta2)
                                   ((int)&pIVar9->_mCOMObject +
                                    (int)pIVar9->_vptr_IUnknown[0x17].__delta);
              }
              bVar19 = false;
              if ((pIVar9 != (IUnknown *)0x0) && (iVar12 == 1 || (iVar12 == 4))) {
                bVar19 = true;
              }
              if ((bVar5) && (bVar19)) {
                fVar30 = local_238[0].x * cookies[iVar10].Forward.x +
                         local_238[0].z * cookies[iVar10].Forward.y;
                if (fVar30 < 0.0) {
                  fVar30 = -fVar30;
                }
                if ((fVar30 > 0.707) && (pIVar9 != (IUnknown *)0x0) &&
                   (piVar13 = (int *)(**(code **)&pIVar9->_vptr_IUnknown[0x12].__delta2)
                                               ((int)&pIVar9->_mCOMObject +
                                                (int)pIVar9->_vptr_IUnknown[0x12].__delta),
                   _Q33UTL11Collectionst12Instanceable3ZP8HCAUSE__Z6ICausei10__mList.__base.__base.
                   mVectorSpace[0x20] == *piVar13)) {
                  VU0_v3scaleadd(&local_218,-6.0,&local_1f8,&local_1f8);
                  local_1e8[0] = 1.8;
                  local_1e0 = 1.8;
                }
              }
              VU0_v3sub(&local_1f8,position,&local_1d8);
              fVar30 = cookies[iVar10].Forward.y;
              fVar31 = cookies[iVar10].Forward.x;
              fVar32 = fVar31 * local_218.x + fVar30 * local_218.z;
              if (fVar32 < 0.0) {
                fVar32 = -fVar32;
              }
              fVar1 = fVar31 * local_238[0].x + fVar30 * local_238[0].z;
              if (fVar1 < 0.0) {
                fVar1 = -fVar1;
              }
              dVar28 = (double)(fVar1 * local_1e8[0] + fVar32 * local_1e0 + local_1e8[0] + 0.5);
              dVar27 = (double)(local_1d8.x * fVar31 + local_1d8.z * fVar30);
              dVar29 = (double)(local_248[0] + 0.5 + local_240);
              fVar30 = VU0_v3dotprod(&VStack_268,&local_218);
              if (fVar30 < 0.0) {
                fVar30 = -fVar30;
              }
              fVar31 = VU0_v3dotprod(&VStack_268,local_238);
              if (fVar31 < 0.0) {
                fVar31 = -fVar31;
              }
              fVar30 = fVar31 * local_1e8[0] + fVar30 * local_1e0 + local_248[0];
              fVar31 = VU0_v3dotprod(&local_1d8,&VStack_268);
              if (fVar31 < 0.0) {
                fVar31 = -fVar31;
              }
              cut_flags = 0;
              if ((bVar3) ||
                 (bVar4 = (byte)(local_110 >> 0x18), !(bool)(bVar4 >> 6 & 1 | bVar4 >> 5 & 1))) {
                if ((double)(float)(dVar27 + dVar28) < dVar29) goto LAB_802ff060;
              }
              else if ((double)(float)(dVar27 - dVar28) <= dVar29) {
LAB_802ff060:
                cut_flags = 2;
              }
              dVar21 = (double)(float)(dVar29 + dVar28);
              if ((-dVar21 <= dVar27) && ((float)(dVar27 + dVar28) >= 0.0 || (fVar30 <= fVar31))) {
                p_Var14 = (pIVar17->__base)._vptr_IUnknown;
                (**(code **)&p_Var14[3].__delta2)
                          ((int)&(pIVar17->__base)._mCOMObject + (int)p_Var14[3].__delta,&local_1c8)
                ;
                dVar22 = 0.0;
                if ((double)(float)(dVar27 - dVar21) > 0.0) {
                  dVar22 = (double)(float)(dVar27 - dVar21);
                }
                local_1b8.y = position->y;
                dVar26 = dVar22;
                if (dVar29 < dVar22) {
                  dVar26 = dVar29;
                }
                dVar24 = (double)cookies[iVar10].Forward.x;
                dVar25 = (double)cookies[iVar10].Forward.y;
                local_1b8.x = (float)(dVar24 * dVar26 + (double)position->x);
                local_1b8.z = (float)(dVar25 * dVar26 + (double)position->z);
                local_1a8.y = local_1f8.y;
                if (dVar28 < dVar22) {
                  dVar22 = dVar28;
                }
                local_1a8.x = local_1f8.x - (float)(dVar24 * dVar22);
                local_1a8.z = local_1f8.z - (float)(dVar25 * dVar22);
                local_150[0] = 0.0;
                fVar32 = TimeToClosestApproach(&local_1b8,local_11c,&local_1a8,&local_1c8,local_150)
                ;
                dVar28 = (double)fVar32;
                bVar19 = false;
                if ((cut_flags == 0) && (bVar19 = bVar3, local_150[0] <= 0.0)) {
                  dVar22 = dVar21;
                  if (bVar3) {
                    dVar22 = (double)(float)(dVar21 + (double)(float)(dVar23 * 0.5 +
                                                                     (double)(float)(dVar29 + dVar29
                                                                                    )));
                  }
                  if ((dVar22 < dVar27) || (bVar3 && (fVar30 + 1.0 < fVar31))) goto LAB_802ff5d0;
                }
                if ((bVar19) || (dVar28 < 3.0)) {
                  fVar31 = cookies[iVar10].Forward.y;
                  local_198.y = local_1f8.y;
                  fVar32 = cookies[iVar10].Forward.x;
                  fVar1 = local_1c8.x * fVar32 + local_1c8.z * fVar31;
                  fVar30 = (float)(dVar28 * (double)fVar1 - dVar21);
                  local_198.z = fVar31 * fVar30 + local_1f8.z;
                  local_198.x = fVar32 * fVar30 + local_1f8.x;
                  iVar12 = WRoadNav__ClosestCookieAhead
                                     (this,&local_198,cookies,num_cookies,(NavCookie *)0x0);
                  if (iVar12 > -1) {
                    VU0_v3scale(local_238,local_1e8[0],&local_188);
                    VU0_v3scale(&local_218,local_1e0,&local_178);
                    iVar18 = local_118;
                    local_148 = local_178.x - local_188.x;
                    fVar30 = cookies[iVar12].Forward.x;
                    local_10c[1] = local_178.z - local_188.z;
                    local_140 = local_178.x + local_188.x;
                    fVar31 = cookies[iVar12].Forward.y;
                    local_108[1] = local_178.z + local_188.z;
                    local_138[0] = local_1c8.x;
                    local_104[1] = local_1c8.z;
                    pVVar7 = local_100;
                    if ((iVar12 < iVar18) && (dVar21 < dVar27)) {
                      this->fOccludingTrailSpeed = fVar1;
                      local_118 = iVar12;
                    }
                    if (dVar28 < 0.0) {
                      dVar28 = 0.0;
                    }
                    local_100->y = local_198.y;
                    if ((float)(1.0 - dVar28) < 0.0) {
                      dVar28 = 1.0;
                    }
                    pVVar7->z = local_198.z;
                    pfVar8 = local_fc;
                    fVar30 = (float)((double)(local_1c8.x * fVar31 - fVar30 * local_1c8.z) * dVar28)
                    ;
                    fVar31 = cookies[iVar12].Forward.y;
                    fVar32 = cookies[iVar12].Forward.x;
                    local_168.x = fVar30 * fVar31 * 0.8 + local_198.x;
                    fVar1 = cookies[iVar12].Centre.x;
                    fVar2 = cookies[iVar12].Centre.z;
                    local_168.z = local_168.z - fVar30 * fVar32 * 0.8;
                    local_130 = local_168.x - fVar1;
                    local_fc[1] = local_168.z - fVar2;
                    fVar2 = position->z - fVar2;
                    local_128[0] = position->x - fVar1;
                    local_f8[1] = fVar2;
                    dVar29 = (double)((((local_130 * fVar32 + pfVar8[1] * fVar31) -
                                       (local_128[0] * fVar32 + fVar2 * fVar31)) - -6.0) / 12.0);
                    dVar27 = 1.0;
                    if (dVar29 < 1.0) {
                      dVar27 = dVar29;
                    }
                    if (dVar27 < 0.0) {
                      dVar27 = 0.0;
                    }
                    dVar22 = ABS((double)(local_140 * fVar31 - fVar32 * local_13c));
                    dVar29 = ABS((double)(local_148 * fVar31 - fVar32 * local_144));
                    dVar28 = dVar28 * dVar27;
                    if ((float)(dVar29 - dVar22) < 0.0) {
                      dVar29 = dVar22;
                    }
                    fVar1 = local_158.x * fVar31 - fVar32 * local_158.y;
                    dVar29 = (double)(float)((double)(fVar30 * 0.19999999) * dVar27 + dVar29);
                    dVar22 = (double)(local_130 * fVar31 - fVar32 * local_12c);
                    if (bVar6) {
                      dVar27 = (double)(float)(dVar27 * 0.800000011920929);
                    }
                    fVar31 = this->fVehicleHalfWidth;
                    bVar19 = (double)((float)dVar28 * delta_offset * 0.2 + current_offset +
                                     fVar1 + fVar1) < dVar22;
                    fVar30 = (float)(dVar27 + (double)fVar31);
                    bVar20 = fVar30 < (float)(dVar22 - dVar29) - cookies[iVar12].LeftOffset;
                    if ((float)((double)(float)((double)cookies[iVar12].RightOffset - dVar22) -
                               dVar29) <= fVar30) {
                      if (bVar20) goto LAB_802ff528;
                    }
                    else if (!bVar20) {
LAB_802ff528:
                      bVar19 = bVar20;
                    }
                    iVar18 = iVar12;
                    if (iVar12 < num_cookies) {
                      while( true ) {
                        cookie = cookies + iVar18;
                        bVar20 = WRoadNav__CookieCutter
                                           (this,cookie,&local_168,
                                            (float)((double)(float)(dVar29 + (double)fVar31) +
                                                   dVar27),bVar19,cut_flags);
                        if ((!bVar20) && (iVar18 == iVar12)) break;
                        local_f0 = local_198.x - (cookie->Centre).x;
                        local_ec = local_198.y - (cookie->Centre).y;
                        if ((local_ec * (cookie->Forward).y +
                             local_f0 * (cookie->Forward).x + (float)(dVar21 + dVar21) < 0.0) ||
                           (iVar18 = iVar18 + 1, num_cookies <= iVar18)) break;
                      }
                    }
                  }
                }
              }
            }
LAB_802ff5d0:
            iVar15 = iVar15 + 1;
          } while (iVar15 < local_120);
        }
        WRoadNav__ClampCookieCentres(this,cookies,num_cookies);
      }
    }
  }
  return;
}



/* ================================================================== */
/* 802cb51c  RenderFlaresOnCar__13CarRenderInfo
   tam=2908 B  insn=727  cobertura=2908/2908  paired-singles=28  firma=IMPORTED
   proto: undefined CarRenderInfo::RenderFlaresOnCar(CarRenderInfo * this, eView * view, bVector3 * position, bMatrix4 * body_matrix, int force_light_state, int reflexion, int renderFlareFlags)
*/

/* WARNING: Removing unreachable block (ram,0x802cc068) */
/* WARNING: Removing unreachable block (ram,0x802cc064) */
/* WARNING: Removing unreachable block (ram,0x802cc060) */
/* WARNING: Removing unreachable block (ram,0x802cc05c) */
/* WARNING: Removing unreachable block (ram,0x802cc058) */
/* WARNING: Removing unreachable block (ram,0x802cc054) */
/* WARNING: Removing unreachable block (ram,0x802cc050) */
/* WARNING: Removing unreachable block (ram,0x802cc04c) */
/* WARNING: Removing unreachable block (ram,0x802cc048) */
/* WARNING: Removing unreachable block (ram,0x802cc044) */
/* WARNING: Removing unreachable block (ram,0x802cc040) */
/* WARNING: Removing unreachable block (ram,0x802cc03c) */
/* WARNING: Removing unreachable block (ram,0x802cc038) */
/* WARNING: Removing unreachable block (ram,0x802cc034) */
/* WARNING: Removing unreachable block (ram,0x802cb55c) */
/* WARNING: Removing unreachable block (ram,0x802cb558) */
/* WARNING: Removing unreachable block (ram,0x802cb554) */
/* WARNING: Removing unreachable block (ram,0x802cb550) */
/* WARNING: Removing unreachable block (ram,0x802cb54c) */
/* WARNING: Removing unreachable block (ram,0x802cb548) */
/* WARNING: Removing unreachable block (ram,0x802cb544) */
/* WARNING: Removing unreachable block (ram,0x802cb540) */
/* WARNING: Removing unreachable block (ram,0x802cb53c) */
/* WARNING: Removing unreachable block (ram,0x802cb538) */
/* WARNING: Removing unreachable block (ram,0x802cb534) */
/* WARNING: Removing unreachable block (ram,0x802cb530) */
/* WARNING: Removing unreachable block (ram,0x802cb52c) */
/* WARNING: Removing unreachable block (ram,0x802cb528) */
/* WARNING: Removing unreachable block (ram,0x802cbe68) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Unknown calling convention -- yet parameter storage is locked */
/* WARNING: Enum "EVIEW_ID": Some values do not have unique names */

void CarRenderInfo__RenderFlaresOnCar
               (CarRenderInfo *this,eView *view,bVector3 *position,bMatrix4 *body_matrix,
               int force_light_state,int reflexion,int renderFlareFlags)

{
  bool bVar1;
  bool bVar2;
  bool bVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float fVar8;
  float fVar9;
  float fVar10;
  float fVar11;
  float fVar12;
  float fVar13;
  float sizescale;
  bool bVar14;
  uchar *puVar15;
  uint uVar16;
  uint uVar17;
  int iVar18;
  EVIEWMODE EVar19;
  eVisibleState eVar20;
  CarPart *pCVar21;
  RideInfo *pRVar22;
  bMatrix4 *l_w;
  char cVar23;
  eLightFlare *light_flare;
  byte in_xer_so;
  byte in_cr0;
  byte bVar24;
  byte bVar25;
  byte in_cr1;
  byte unaff_cr2;
  byte bVar26;
  byte unaff_cr3;
  byte unaff_cr4;
  byte in_cr5;
  byte in_cr6;
  byte in_cr7;
  float fVar27;
  float fVar28;
  float fVar29;
  float intensity_scale;
  bVector3 local_f0;
  uint local_e0;
  uint local_dc;
  uint local_d8;
  eLightFlare *local_d4;
  longlong local_c8;
  uint local_bc;
  
  local_bc = (uint)(in_cr0 & 0xf) << 0x1c | (uint)(in_cr1 & 0xf) << 0x18 |
             (uint)(unaff_cr2 & 0xf) << 0x14 | (uint)(unaff_cr3 & 0xf) << 0x10 |
             (uint)(unaff_cr4 & 0xf) << 0xc | (uint)(in_cr5 & 0xf) << 8 | (uint)(in_cr6 & 0xf) << 4
             | (uint)(in_cr7 & 0xf);
  fVar27 = GetTime();
  fVar27 = fVar27 + this->CarTimebaseStart;
  l_w = (bMatrix4 *)CurrentBufferPos;
  puVar15 = CurrentBufferPos + 0x40;
  if (CurrentBufferEnd <= CurrentBufferPos + 0x40) {
    FrameMallocFailed = 1;
    FrameMallocFailAmount = FrameMallocFailAmount + 0x40;
    l_w = (bMatrix4 *)0x0;
    puVar15 = CurrentBufferPos;
  }
  CurrentBufferPos = puVar15;
  if (l_w != (bMatrix4 *)0x0) {
    PSMTX44Copy(body_matrix,l_w);
    bVar3 = reflexion == 0;
    bVar26 = (reflexion < 0) << 3 | (reflexion > 0) << 2 | bVar3 << 1 | in_xer_so & 1;
    (l_w->v3).x = position->x;
    (l_w->v3).y = position->y;
    fVar4 = position->z;
    (l_w->v3).w = 1.0;
    (l_w->v3).z = fVar4;
    if (!bVar3) {
      CarRenderInfo__RenderTextureHeadlights(this,view,l_w,0);
    }
    if ((this->pCarTypeInfo != (CarTypeInfo *)0x0) &&
       (this->pCarTypeInfo->UsageType == CAR_USAGE_TYPE_COP) && ((this->mOnLights & 0x1000) != 0)) {
      view->NumCopsCherry = view->NumCopsCherry + 1;
    }
    iVar18 = eView__GetPixelSize(view,position,*(float *)&this->field_0x1758);
    EVar19 = eGetCurrentViewMode();
    if (EVar19 == EVIEWMODE_TWOH) {
      iVar18 = (int)((float)iVar18 * 0.7);
      local_c8 = (longlong)iVar18;
    }
    if ((view->PixelMinSize <= iVar18) &&
       (eVar20 = eView__GetVisibleState(view,&this->AABBMin,&this->AABBMax,l_w),
       eVar20 != EVISIBLESTATE_NOT)) {
      pRVar22 = this->pRideInfo;
      bVar14 = CarTypeInfoArray[pRVar22->Type].UsageType == CAR_USAGE_TYPE_TRAFFIC;
      fVar7 = 1.0;
      fVar10 = 0.5;
      fVar4 = fVar7;
      if (_Q33UTL11Collectionst9Singleton1Z4INIS_mInstance != (INIS *)0x0) {
        fVar4 = 0.0;
      }
      fVar5 = 0.0;
      if (ForceHeadlightsOn != 0) {
        force_light_state = force_light_state | 1;
      }
      if (ForceBrakelightsOn != 0) {
        force_light_state = force_light_state | 2;
      }
      if ((force_light_state & 1U) == 0) {
        if ((force_light_state & 8U) != 0) {
          fVar4 = fVar5;
        }
      }
      else {
        fVar4 = fVar4 + 1.0;
      }
      fVar11 = fVar5;
      if ((force_light_state & 2U) != 0) {
        fVar10 = 1.5;
        fVar11 = 1.0;
      }
      uVar16 = this->mOnLights;
      fVar6 = fVar4;
      if ((uVar16 & 1) != 0) {
        fVar6 = fVar7;
      }
      uVar17 = this->mBrokenLights;
      if ((uVar17 & 1) != 0) {
        fVar6 = 0.0;
      }
      if ((uVar16 & 2) != 0) {
        fVar4 = fVar7;
      }
      if ((uVar17 & 2) != 0) {
        fVar4 = 0.0;
      }
      bVar1 = (uVar16 & 0x40) == 0;
      bVar24 = ((uVar16 & 0x40) != 0) << 2 | bVar1 << 1 | in_xer_so & 1;
      fVar7 = fVar5;
      if (!bVar1) {
        fVar7 = 1.0;
      }
      bVar2 = (uVar17 & 0x40) == 0;
      bVar25 = ((uVar17 & 0x40) != 0) << 2 | bVar2 << 1 | in_xer_so & 1;
      if (!bVar2) {
        fVar7 = 0.0;
      }
      fVar8 = fVar5;
      if (!bVar1) {
        fVar8 = 1.0;
      }
      if (!bVar2) {
        fVar8 = 0.0;
      }
      fVar9 = fVar10;
      if ((uVar16 & 8) != 0) {
        fVar9 = fVar10 + 16.0;
      }
      if ((uVar17 & 8) != 0) {
        fVar9 = 0.0;
      }
      if ((uVar16 & 0x10) != 0) {
        fVar10 = fVar10 + 16.0;
      }
      if ((uVar17 & 0x10) != 0) {
        fVar10 = 0.0;
      }
      if ((uVar16 & 0x20) != 0) {
        fVar11 = 16.5;
      }
      if ((uVar17 & 0x20) != 0) {
        fVar11 = 0.0;
      }
      if ((uVar16 & 0x40) != 0) {
        fVar7 = fVar7 + 16.0;
      }
      if ((uVar17 & 0x40) != 0) {
        fVar7 = 0.0;
      }
      if ((uVar16 & 0x80) != 0) {
        fVar8 = fVar8 + 16.0;
      }
      if ((uVar17 & 0x80) != 0) {
        fVar8 = 0.0;
      }
      fVar12 = fVar5;
      if ((uVar16 & 0x1000) != 0) {
        fVar12 = cpr;
      }
      fVar13 = fVar5;
      if ((uVar16 & 0x2000) != 0) {
        fVar13 = cpb;
      }
      bVar1 = (uVar16 & 0x4000) != 0;
      if (bVar1) {
        fVar5 = cpw;
      }
      if ((uVar17 & 0x1000) != 0) {
        fVar12 = 0.0;
      }
      if ((uVar17 & 0x2000) != 0) {
        fVar13 = 0.0;
      }
      if ((uVar17 & 0x4000) != 0) {
        fVar5 = 0.0;
      }
      pCVar21 = (CarPart *)0x0;
      if (pRVar22 != (RideInfo *)0x0) {
        pCVar21 = pRVar22->PreviewPart;
      }
      if (pCVar21 == (CarPart *)0x0) {
        cVar23 = 'W';
      }
      else {
        cVar23 = pCVar21->PartID;
      }
      fVar28 = coplightflicker(fVar27,0);
      local_e0 = (uint)(byte)(bVar14 << 2 | !bVar14 << 1 | in_xer_so & 1) << 0x1c |
                 (uint)(in_cr1 & 0xf) << 0x18 | (uint)bVar26 << 0x14 |
                 (uint)(unaff_cr3 & 0xf) << 0x10 | (uint)(unaff_cr4 & 0xf) << 0xc |
                 (uint)(in_cr5 & 0xf) << 8 | (uint)(bVar24 & 0xf) << 4 | (uint)(bVar25 & 0xf);
      local_d8 = (uint)(byte)((cVar23 < '@') << 3 | (cVar23 > '@') << 2 | (cVar23 == '@') << 1 |
                             in_xer_so & 1) << 0x1c | (uint)(in_cr1 & 0xf) << 0x18 |
                 (uint)bVar26 << 0x14 | (uint)(unaff_cr3 & 0xf) << 0x10 |
                 (uint)(unaff_cr4 & 0xf) << 0xc | (uint)(in_cr5 & 0xf) << 8 |
                 (uint)(bVar24 & 0xf) << 4 | (uint)(bVar25 & 0xf);
      local_d4 = (eLightFlare *)&this->LightFlareList;
      bVar14 = (renderFlareFlags & 1U) == 0;
      local_dc = (uint)(byte)((cVar23 < 'A') << 3 | (cVar23 > 'A') << 2 | (cVar23 == 'A') << 1 |
                             in_xer_so & 1) << 0x1c | (uint)(in_cr1 & 0xf) << 0x18 |
                 (uint)bVar26 << 0x14 | (uint)(unaff_cr3 & 0xf) << 0x10 |
                 (uint)(byte)(((renderFlareFlags & 1U) != 0) << 2 | bVar14 << 1 | in_xer_so & 1) <<
                 0xc | (uint)(in_cr5 & 0xf) << 8 | (uint)(bVar24 & 0xf) << 4 | (uint)(bVar25 & 0xf);
      iVar18 = 0;
      for (light_flare = (eLightFlare *)(this->LightFlareList).__base.HeadNode.Next;
          light_flare != local_d4; light_flare = (eLightFlare *)(light_flare->__base).__base.Next) {
        uVar16 = light_flare->NameHash;
        fVar29 = 0.0;
        sizescale = 1.0;
        if ((!(bool)((byte)(local_e0 >> 0x1d) & 1)) && (light_flare->Type == '\x01')) {
          light_flare->Type = '\x02';
        }
        if (((renderFlareFlags & 2U) == 0) || (light_flare->Type == '\x01')) {
          if (!bVar14) {
            if ((byte)light_flare->Type - 5 > 7) goto LAB_802cbf00;
            sizescale = 10.0;
          }
          intensity_scale = fVar8;
          if (uVar16 != 0x7adf7ef8) {
            if (uVar16 < 0x7adf7ef9) {
              intensity_scale = fVar9;
              if (uVar16 != 0x31a66786) {
                if (uVar16 < 0x31a66787) {
                  if (uVar16 == 0x1e4150b4) {
                    fVar29 = coplightflicker2(fVar27,0,iVar18);
                    fVar12 = fVar12 * fVar29;
                    intensity_scale = fVar12;
                  }
                  else {
                    intensity_scale = fVar29;
                    if (uVar16 == 0x28cd78f5) {
                      intensity_scale = 1.0;
                    }
                  }
                }
                else if (uVar16 == 0x6a52a241) {
                  fVar29 = coplightflicker2(fVar27,1,iVar18);
                  intensity_scale = fVar13 * fVar29;
                }
                else if (uVar16 < 0x6a52a242) {
                  intensity_scale = fVar29;
                  if (uVar16 == 0x41489594) {
                    fVar29 = coplightflicker2(fVar27,0,iVar18);
                    intensity_scale = fVar12 * fVar29;
                  }
                }
                else {
                  intensity_scale = fVar7;
                  if (uVar16 != 0x7a5b2f25) {
                    intensity_scale = fVar29;
                  }
                }
              }
            }
            else if (uVar16 == 0xb4348dba) {
              fVar29 = coplightflicker2(fVar27,2,iVar18);
              intensity_scale = bSin(fVar5 * fVar29 * copWhitemul);
            }
            else if (uVar16 < 0xb4348dbb) {
              if (uVar16 == 0x9db90133) {
                intensity_scale = fVar6;
                if (bVar1) {
                  intensity_scale = fVar6 * fVar28;
                }
              }
              else {
                intensity_scale = fVar11;
                if (uVar16 != 0xa2a2fc7c) {
                  intensity_scale = fVar29;
                }
              }
            }
            else if (uVar16 == 0xd09091c6) {
              intensity_scale = fVar4;
              if (bVar1) {
                intensity_scale = fVar4 * (1.0 - fVar28);
              }
            }
            else if (uVar16 < 0xd09091c7) {
              intensity_scale = fVar10;
              if (uVar16 != 0xbf700a79) {
                intensity_scale = fVar29;
              }
            }
            else {
              intensity_scale = fVar29;
              if (uVar16 == 0xe662c161) {
                fVar29 = coplightflicker2(fVar27,1,iVar18);
                fVar13 = fVar13 * fVar29;
                intensity_scale = fVar13;
              }
            }
          }
          if (intensity_scale > 0.0) {
            if (intensity_scale > 1.0) {
              intensity_scale = 1.0;
            }
            if (bVar3) {
              eRenderLightFlare(view,light_flare,l_w,intensity_scale,REF_NONE,(uint)!bVar14,0.0,0,
                                sizescale);
            }
            else {
              eRenderLightFlare(view,light_flare,l_w,intensity_scale,REF_TOPO,FLARE_REFLECT,
                                this->mCar_elevation,0,1.0);
            }
          }
          iVar18 = iVar18 + 1;
        }
LAB_802cbf00:
      }
      if ((view->ID == EVIEW_FIRST_PLAYER) && (bVar3)) {
        local_f0.x = gTWEAKER_NISLightPosX + position->x;
        local_f0.y = gTWEAKER_NISLightPosY + position->y;
        local_f0.z = gTWEAKER_NISLightPosZ + position->z;
        fVar27 = 1.0;
        if (_gTWEAKER_NISLightEnabled != 0) {
          position = &local_f0;
          fVar27 = gTWEAKER_NISLightIntensity;
        }
        if (fVar12 <= 0.0) {
          if (fVar13 <= 0.0) {
            RestoreShaperRig(&ShaperLightsCharacters,Lightslot,&ShaperLightsCharactersBackup);
            return;
          }
          if (fVar12 <= 0.0) {
            if (fVar13 <= 0.0) {
              return;
            }
            AddQuickDynamicLight
                      (&ShaperLightsCharacters,Lightslot,0.2,0.2,0.8,fVar13 * fVar27,position);
            return;
          }
        }
        AddQuickDynamicLight(&ShaperLightsCharacters,Lightslot,0.8,0.2,0.0,fVar12 * fVar27,position)
        ;
      }
    }
  }
  return;
}



/* ================================================================== */
/* 8008f098  Initialize__Q25EAGL413DynamicLoader
   tam=2352 B  insn=588  cobertura=2352/2352  paired-singles=0  firma=IMPORTED
   proto: undefined DynamicLoader::Initialize(DynamicLoader * this, void *(char * , bool * ) * pSearchFunction)
*/

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void DynamicLoader__Initialize(DynamicLoader *this,void___char_____bool____ *pSearchFunction)

{
  void___uint___char____ *pvVar1;
  ulong uVar2;
  ELFSectionHeader *pEVar3;
  uint uVar4;
  char *pcVar5;
  void *pvVar6;
  HashPointer *pHVar7;
  size_t sVar8;
  char *pcVar9;
  size_t sVar10;
  size_t sVar11;
  ulong *puVar12;
  bool *pbVar13;
  ulong uVar14;
  ELF32_Sym *pEVar15;
  uint uVar16;
  uint uVar17;
  uint uVar18;
  uint uVar19;
  void *pvVar20;
  uint uVar21;
  uint uVar22;
  uint uVar23;
  int iVar24;
  uint uVar25;
  char acStack_e0 [128];
  DynamicLoader *local_60;
  void___char_____bool____ *local_5c;
  ulong local_58;
  ELFHeader *local_54;
  ELFSectionHeader *local_50;
  ulong *local_4c;
  
  local_60 = this;
  local_5c = pSearchFunction;
  pHVar7 = (HashPointer *)(*_13EAGL4Internal_EAGL4Malloc)(0x430,(char *)0x0);
  pHVar7->e = (ELFHeader *)0x0;
  pHVar7->mpDynamicLoader = local_60;
  pHVar7->pSearchFunction = (void___char_____bool____ *)0x0;
  pHVar7->next = (HashPointer *)0x0;
  pHVar7->strtab = (char *)0x0;
  *(undefined4 *)&pHVar7->resolved = 0;
  pHVar7->symbols_num = 0;
  pHVar7->symtab = (ELF32_Sym *)0x0;
  pHVar7->sections = (ELFSectionHeader *)0x0;
  pHVar7->chain = (ulong *)0x0;
  pHVar7->isOriginal = (bool *)0x0;
  local_54 = (ELFHeader *)local_60->mpData;
  pHVar7->e = local_54;
  uVar16 = local_54->e_phoff;
  uVar4 = local_54->e_flags;
  uVar23 = local_54->e_version;
  uVar21 = local_54->e_entry;
  uVar18 = local_54->e_shoff;
  uVar18 = (uVar18 >> 0x10 & 0xff) << 8 | uVar18 >> 0x18 |
           ((uVar18 & 0xff) << 8 | (uVar18 & 0xffff) >> 8) << 0x10;
  local_54->e_type = local_54->e_type << 8 | local_54->e_type >> 8;
  local_54->e_machine = local_54->e_machine << 8 | local_54->e_machine >> 8;
  local_54->e_version =
       (uVar23 >> 0x10 & 0xff) << 8 | uVar23 >> 0x18 |
       ((uVar23 & 0xff) << 8 | (uVar23 & 0xffff) >> 8) << 0x10;
  local_54->e_entry =
       (uVar21 >> 0x10 & 0xff) << 8 | uVar21 >> 0x18 |
       ((uVar21 & 0xff) << 8 | (uVar21 & 0xffff) >> 8) << 0x10;
  local_54->e_phoff =
       (uVar16 >> 0x10 & 0xff) << 8 | uVar16 >> 0x18 |
       ((uVar16 & 0xff) << 8 | (uVar16 & 0xffff) >> 8) << 0x10;
  local_54->e_ehsize = local_54->e_ehsize << 8 | local_54->e_ehsize >> 8;
  local_54->e_phentsize = local_54->e_phentsize << 8 | local_54->e_phentsize >> 8;
  local_54->e_phnum = local_54->e_phnum << 8 | local_54->e_phnum >> 8;
  local_54->e_flags =
       (uVar4 >> 0x10 & 0xff) << 8 | uVar4 >> 0x18 |
       ((uVar4 & 0xff) << 8 | (uVar4 & 0xffff) >> 8) << 0x10;
  local_54->e_shoff = uVar18;
  local_54->e_shentsize = local_54->e_shentsize << 8 | local_54->e_shentsize >> 8;
  local_54->e_shnum = local_54->e_shnum << 8 | local_54->e_shnum >> 8;
  local_54->e_shstrndx = local_54->e_shstrndx << 8 | local_54->e_shstrndx >> 8;
  uVar4 = local_60->mDataLen;
  if (uVar18 < uVar4) {
    pcVar5 = local_60->mpData;
  }
  else {
    if (local_60->mpReloc != (char *)0x0) {
      local_50 = (ELFSectionHeader *)(local_60->mpReloc + (uVar18 - uVar4));
      goto LAB_8008f300;
    }
    if (uVar4 < uVar18) {
      local_50 = (ELFSectionHeader *)0x0;
      goto LAB_8008f300;
    }
    pcVar5 = local_60->mpData;
  }
  local_50 = (ELFSectionHeader *)(pcVar5 + uVar18);
LAB_8008f300:
  pHVar7->symtab = (ELF32_Sym *)0x0;
  pHVar7->sections = local_50;
  local_4c = pHVar7->hash;
  local_58 = 0;
  if (local_54->e_shnum != 0) {
    do {
      uVar4 = local_50[local_58].sh_name;
      local_50[local_58].sh_name =
           (uVar4 >> 0x10 & 0xff) << 8 | uVar4 >> 0x18 |
           ((uVar4 & 0xff) << 8 | (uVar4 & 0xffff) >> 8) << 0x10;
      uVar18 = local_50[local_58].sh_type;
      uVar16 = local_50[local_58].sh_flags;
      uVar25 = local_50[local_58].sh_addr;
      uVar21 = local_50[local_58].field4_0x10.sh_offset;
      uVar23 = local_50[local_58].sh_size;
      uVar17 = local_50[local_58].field6_0x18.sh_link;
      uVar22 = local_50[local_58].field7_0x1c.sh_info;
      uVar19 = local_50[local_58].sh_addralign;
      uVar4 = local_50[local_58].sh_entsize;
      pvVar20 = (void *)((uVar21 >> 0x10 & 0xff) << 8 | uVar21 >> 0x18 |
                        ((uVar21 & 0xff) << 8 | (uVar21 & 0xffff) >> 8) << 0x10);
      local_50[local_58].sh_type =
           (uVar18 >> 0x10 & 0xff) << 8 | uVar18 >> 0x18 |
           ((uVar18 & 0xff) << 8 | (uVar18 & 0xffff) >> 8) << 0x10;
      local_50[local_58].sh_entsize =
           (uVar4 >> 0x10 & 0xff) << 8 | uVar4 >> 0x18 |
           ((uVar4 & 0xff) << 8 | (uVar4 & 0xffff) >> 8) << 0x10;
      local_50[local_58].sh_flags =
           (uVar16 >> 0x10 & 0xff) << 8 | uVar16 >> 0x18 |
           ((uVar16 & 0xff) << 8 | (uVar16 & 0xffff) >> 8) << 0x10;
      local_50[local_58].sh_addr =
           (uVar25 >> 0x10 & 0xff) << 8 | uVar25 >> 0x18 |
           ((uVar25 & 0xff) << 8 | (uVar25 & 0xffff) >> 8) << 0x10;
      local_50[local_58].sh_size =
           (uVar23 >> 0x10 & 0xff) << 8 | uVar23 >> 0x18 |
           ((uVar23 & 0xff) << 8 | (uVar23 & 0xffff) >> 8) << 0x10;
      local_50[local_58].field6_0x18.sh_link =
           (uVar17 >> 0x10 & 0xff) << 8 | uVar17 >> 0x18 |
           ((uVar17 & 0xff) << 8 | (uVar17 & 0xffff) >> 8) << 0x10;
      local_50[local_58].field7_0x1c.sh_info =
           (uVar22 >> 0x10 & 0xff) << 8 | uVar22 >> 0x18 |
           ((uVar22 & 0xff) << 8 | (uVar22 & 0xffff) >> 8) << 0x10;
      local_50[local_58].sh_addralign =
           (uVar19 >> 0x10 & 0xff) << 8 | uVar19 >> 0x18 |
           ((uVar19 & 0xff) << 8 | (uVar19 & 0xffff) >> 8) << 0x10;
      local_50[local_58].field4_0x10.sh_voffset = pvVar20;
      pvVar6 = (void *)local_60->mDataLen;
      if (pvVar20 < pvVar6) {
        pcVar5 = local_60->mpData;
LAB_8008f570:
        pcVar5 = pcVar5 + (int)pvVar20;
      }
      else if (local_60->mpReloc == (char *)0x0) {
        if (pvVar20 <= pvVar6) {
          pcVar5 = local_60->mpData;
          goto LAB_8008f570;
        }
        pcVar5 = (char *)0x0;
      }
      else {
        pcVar5 = local_60->mpReloc + ((int)pvVar20 - (int)pvVar6);
      }
      iVar24 = local_58 + 1;
      local_50[local_58].field4_0x10.sh_voffset = pcVar5;
      local_58 = iVar24;
    } while (iVar24 < (int)(uint)local_54->e_shnum);
  }
  local_58 = 0;
  pvVar6 = local_50[local_54->e_shstrndx].field4_0x10.sh_voffset;
  if (local_54->e_shnum != 0) {
    do {
      pEVar3 = local_50;
      uVar2 = local_58;
      uVar4 = local_50[local_58].sh_type;
      if (uVar4 != 8) {
        if (uVar4 < 9) {
          if (uVar4 == 3) {
            iVar24 = strcmp(".strtab",(char *)((int)pvVar6 + local_50[local_58].sh_name));
            if (iVar24 == 0) {
              pcVar5 = (char *)pEVar3[uVar2].field4_0x10.sh_voffset;
              pHVar7->strtab = pcVar5;
              for (uVar4 = pEVar3[uVar2].sh_size; (int)uVar4 > 0; uVar4 = (uVar4 - 1) - sVar8) {
                sVar8 = strlen(pcVar5);
                if ((*pcVar5 == '_') && (pcVar5[1] == '_')) {
                  pcVar9 = strstr(pcVar5 + 2,":::");
                  if (pcVar9 != (char *)0x0) {
                    *pcVar9 = '\0';
                    strcpy(acStack_e0,pcVar5 + 2);
                    sVar10 = strlen(":::");
                    sVar11 = strlen(pcVar9 + sVar10);
                    memmove(pcVar5,pcVar9 + sVar10,sVar11 + 1);
                    pcVar5[sVar11 + 1] = '\x7f';
                    strcpy(pcVar5 + sVar11 + 2,acStack_e0);
                  }
                }
                pcVar5 = pcVar5 + sVar8 + 1;
              }
            }
          }
          else if ((uVar4 < 4) && (uVar4 > 1)) {
            local_50[local_58].field6_0x18 =
                 *(anon_12327151 *)&local_50[local_50[local_58].field6_0x18.sh_link].field4_0x10;
            iVar24 = strcmp(".symtab",(char *)((int)pvVar6 + local_50[local_58].sh_name));
            if (iVar24 == 0) {
              pHVar7->symtab = (ELF32_Sym *)pEVar3[uVar2].field4_0x10;
              uVar4 = pEVar3[uVar2].sh_size;
              if ((int)uVar4 < 0) {
                uVar4 = uVar4 + 0xf;
              }
              pHVar7->symbols_num = (int)uVar4 >> 4;
            }
          }
        }
        else if ((uVar4 < 0x70000007) && (uVar4 < 0x70000005) && (uVar4 == 9)) {
          local_50[local_58].field7_0x1c =
               *(anon_12327301 *)&local_50[local_50[local_58].field7_0x1c.sh_info].field4_0x10;
          local_50[local_58].field6_0x18 =
               *(anon_12327151 *)&local_50[local_50[local_58].field6_0x18.sh_link].field4_0x10;
        }
      }
      local_58 = local_58 + 1;
    } while ((int)local_58 < (int)(uint)local_54->e_shnum);
  }
  pHVar7->pSearchFunction = local_5c;
  local_58 = 0;
  do {
    iVar24 = local_58 + 1;
    local_4c[local_58] = 0xffffffff;
    local_58 = iVar24;
  } while (iVar24 < 0x100);
  local_58 = 0;
  puVar12 = (ulong *)(*_13EAGL4Internal_EAGL4Malloc)
                               (pHVar7->symbols_num << 2,"EAGL4::dynamic symbols");
  pvVar1 = _13EAGL4Internal_EAGL4Malloc;
  pHVar7->chain = puVar12;
  pbVar13 = (bool *)(*pvVar1)(pHVar7->symbols_num << 2,"EAGL4::dynamic symbols 2");
  puVar12 = local_4c;
  pHVar7->isOriginal = pbVar13;
  if ((int)local_58 < pHVar7->symbols_num) {
    do {
      uVar2 = local_58;
      pEVar15 = pHVar7->symtab;
      uVar4 = pEVar15[local_58].st_name;
      pEVar15[local_58].st_name =
           (uVar4 >> 0x10 & 0xff) << 8 | uVar4 >> 0x18 |
           ((uVar4 & 0xff) << 8 | (uVar4 & 0xffff) >> 8) << 0x10;
      uVar16 = pEVar15[local_58].st_size;
      uVar4 = pEVar15[local_58].st_value;
      pEVar15[local_58].st_value =
           (uVar4 >> 0x10 & 0xff) << 8 | uVar4 >> 0x18 |
           ((uVar4 & 0xff) << 8 | (uVar4 & 0xffff) >> 8) << 0x10;
      pEVar15[local_58].st_size =
           (uVar16 >> 0x10 & 0xff) << 8 | uVar16 >> 0x18 |
           ((uVar16 & 0xff) << 8 | (uVar16 & 0xffff) >> 8) << 0x10;
      pEVar15[local_58].st_shndx = pEVar15[local_58].st_shndx << 8 | pEVar15[local_58].st_shndx >> 8
      ;
      uVar14 = elfhash(pHVar7->strtab + pEVar15[local_58].st_name);
      pHVar7->chain[local_58] = puVar12[uVar14];
      puVar12[uVar14] = local_58;
      if ((pEVar15[uVar2].st_shndx == 0) || (local_54->e_shnum <= pEVar15[uVar2].st_shndx)) {
        pbVar13 = pHVar7->isOriginal + local_58 * 4;
        pbVar13[0] = false;
        pbVar13[1] = false;
        pbVar13[2] = false;
        pbVar13[3] = false;
      }
      else {
        pbVar13 = pHVar7->isOriginal + local_58 * 4;
        pbVar13[0] = false;
        pbVar13[1] = false;
        pbVar13[2] = false;
        pbVar13[3] = true;
      }
      local_58 = local_58 + 1;
    } while ((int)local_58 < pHVar7->symbols_num);
  }
  pHVar7->next = _5EAGL4_hashhead;
  if (_5EAGL4_hashhead != (HashPointer *)0x0) {
    _5EAGL4_hashhead->prev = pHVar7;
  }
  pHVar7->prev = (HashPointer *)0x0;
  _5EAGL4_hashhead = pHVar7;
  local_60->handle = pHVar7;
  return;
}



/* ================================================================== */
/* 80109358  epCalculateLocalDirectionalPOS16
   tam=2072 B  insn=518  cobertura=2072/2072  paired-singles=185  firma=IMPORTED
   proto: undefined epCalculateLocalDirectionalPOS16(uint * colour_table0, uint * colour_table1, int num_colour_entries, ushort * position_table_16, int * normal_table, uchar * index_buffer, int vertex_description, int num_indicies, eLightMaterial * light_material, eLightContext * light_context)
*/

/* WARNING: Removing unreachable block (ram,0x80109b60) */
/* WARNING: Removing unreachable block (ram,0x80109b5c) */
/* WARNING: Removing unreachable block (ram,0x80109b58) */
/* WARNING: Removing unreachable block (ram,0x80109b54) */
/* WARNING: Removing unreachable block (ram,0x80109b50) */
/* WARNING: Removing unreachable block (ram,0x80109b4c) */
/* WARNING: Removing unreachable block (ram,0x80109b48) */
/* WARNING: Removing unreachable block (ram,0x80109b44) */
/* WARNING: Removing unreachable block (ram,0x80109b40) */
/* WARNING: Removing unreachable block (ram,0x80109b3c) */
/* WARNING: Removing unreachable block (ram,0x80109b38) */
/* WARNING: Removing unreachable block (ram,0x80109b34) */
/* WARNING: Removing unreachable block (ram,0x80109b30) */
/* WARNING: Removing unreachable block (ram,0x80109b2c) */
/* WARNING: Removing unreachable block (ram,0x80109b28) */
/* WARNING: Removing unreachable block (ram,0x80109b24) */
/* WARNING: Removing unreachable block (ram,0x80109b20) */
/* WARNING: Removing unreachable block (ram,0x80109b1c) */
/* WARNING: Removing unreachable block (ram,0x8010982c) */
/* WARNING: Removing unreachable block (ram,0x80109a20) */
/* WARNING: Removing unreachable block (ram,0x80109870) */
/* WARNING: Removing unreachable block (ram,0x80109820) */
/* WARNING: Removing unreachable block (ram,0x80109800) */
/* WARNING: Removing unreachable block (ram,0x8010980c) */
/* WARNING: Removing unreachable block (ram,0x80109880) */
/* WARNING: Removing unreachable block (ram,0x801099dc) */
/* WARNING: Removing unreachable block (ram,0x801097f4) */
/* WARNING: Removing unreachable block (ram,0x801099c4) */
/* WARNING: Removing unreachable block (ram,0x80109888) */
/* WARNING: Removing unreachable block (ram,0x80109528) */
/* WARNING: Removing unreachable block (ram,0x80109524) */
/* WARNING: Removing unreachable block (ram,0x80109520) */
/* WARNING: Removing unreachable block (ram,0x8010951c) */
/* WARNING: Removing unreachable block (ram,0x80109518) */
/* WARNING: Removing unreachable block (ram,0x80109514) */
/* WARNING: Removing unreachable block (ram,0x80109510) */
/* WARNING: Removing unreachable block (ram,0x8010950c) */
/* WARNING: Removing unreachable block (ram,0x80109508) */
/* WARNING: Removing unreachable block (ram,0x80109504) */
/* WARNING: Removing unreachable block (ram,0x80109500) */
/* WARNING: Removing unreachable block (ram,0x801094fc) */
/* WARNING: Removing unreachable block (ram,0x801094f8) */
/* WARNING: Removing unreachable block (ram,0x801094f4) */
/* WARNING: Removing unreachable block (ram,0x801094f0) */
/* WARNING: Removing unreachable block (ram,0x801094ec) */
/* WARNING: Removing unreachable block (ram,0x801094dc) */
/* WARNING: Removing unreachable block (ram,0x801094ac) */
/* WARNING: Removing unreachable block (ram,0x801094a8) */
/* WARNING: Removing unreachable block (ram,0x801094a4) */
/* WARNING: Removing unreachable block (ram,0x801094a0) */
/* WARNING: Removing unreachable block (ram,0x8010949c) */
/* WARNING: Removing unreachable block (ram,0x80109498) */
/* WARNING: Removing unreachable block (ram,0x80109494) */
/* WARNING: Removing unreachable block (ram,0x80109490) */
/* WARNING: Removing unreachable block (ram,0x8010948c) */
/* WARNING: Removing unreachable block (ram,0x80109488) */
/* WARNING: Removing unreachable block (ram,0x80109484) */
/* WARNING: Removing unreachable block (ram,0x80109480) */
/* WARNING: Removing unreachable block (ram,0x8010947c) */
/* WARNING: Removing unreachable block (ram,0x80109478) */
/* WARNING: Removing unreachable block (ram,0x80109464) */
/* WARNING: Removing unreachable block (ram,0x80109460) */
/* WARNING: Removing unreachable block (ram,0x801093a8) */
/* WARNING: Removing unreachable block (ram,0x801093a4) */
/* WARNING: Removing unreachable block (ram,0x801093a0) */
/* WARNING: Removing unreachable block (ram,0x8010939c) */
/* WARNING: Removing unreachable block (ram,0x80109398) */
/* WARNING: Removing unreachable block (ram,0x80109394) */
/* WARNING: Removing unreachable block (ram,0x80109390) */
/* WARNING: Removing unreachable block (ram,0x8010938c) */
/* WARNING: Removing unreachable block (ram,0x80109388) */
/* WARNING: Removing unreachable block (ram,0x80109384) */
/* WARNING: Removing unreachable block (ram,0x80109380) */
/* WARNING: Removing unreachable block (ram,0x8010937c) */
/* WARNING: Removing unreachable block (ram,0x80109378) */
/* WARNING: Removing unreachable block (ram,0x80109374) */
/* WARNING: Removing unreachable block (ram,0x80109370) */
/* WARNING: Removing unreachable block (ram,0x8010936c) */
/* WARNING: Removing unreachable block (ram,0x80109368) */
/* WARNING: Removing unreachable block (ram,0x80109364) */
/* WARNING: Removing unreachable block (ram,0x80109858) */
/* WARNING: Removing unreachable block (ram,0x80109938) */
/* WARNING: Removing unreachable block (ram,0x801099f0) */
/* WARNING: Removing unreachable block (ram,0x80109a08) */
/* WARNING: Removing unreachable block (ram,0x80109804) */
/* WARNING: Removing unreachable block (ram,0x80109890) */
/* WARNING: Removing unreachable block (ram,0x8010983c) */
/* WARNING: Removing unreachable block (ram,0x801099a0) */
/* WARNING: Removing unreachable block (ram,0x80109818) */
/* WARNING: Removing unreachable block (ram,0x80109844) */
/* WARNING: Removing unreachable block (ram,0x80109954) */
/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void epCalculateLocalDirectionalPOS16
               (uint *colour_table0,uint *colour_table1,int num_colour_entries,
               ushort *position_table_16,int *normal_table,uchar *index_buffer,
               int vertex_description,int num_indicies,eLightMaterial *light_material,
               eLightContext *light_context)

{
  bNode *pbVar1;
  bNode *pbVar2;
  byte bVar3;
  byte bVar4;
  byte bVar5;
  bNode *pbVar6;
  bNode *pbVar7;
  bNode *pbVar8;
  bNode *pbVar9;
  bNode *pbVar10;
  bNode *pbVar11;
  bNode *pbVar12;
  bNode *pbVar13;
  bNode *pbVar14;
  float fVar15;
  float fVar16;
  float fVar17;
  float fVar18;
  float fVar19;
  float fVar20;
  float fVar21;
  float fVar22;
  bNode *pbVar23;
  bNode *pbVar24;
  bNode *pbVar25;
  bNode *pbVar26;
  bNode *pbVar27;
  float fVar28;
  float fVar29;
  float fVar30;
  float fVar31;
  float fVar32;
  float fVar33;
  float fVar34;
  bool bVar35;
  bool bVar36;
  uint uVar37;
  uint uVar38;
  int iVar39;
  bNode *pbVar40;
  int iVar41;
  int iVar42;
  float *pfVar43;
  float *pfVar44;
  uint uVar45;
  uchar *puVar46;
  float *pfVar47;
  uint uVar48;
  int iVar49;
  int iVar50;
  float *pfVar51;
  uint unaff_GQR5;
  uint unaff_GQR6;
  uint unaff_GQR7;
  double dVar52;
  double dVar53;
  double dVar54;
  double dVar55;
  double dVar56;
  double dVar57;
  double dVar58;
  double dVar59;
  double dVar60;
  double dVar61;
  double dVar62;
  double dVar63;
  double dVar64;
  double dVar65;
  double dVar66;
  double dVar67;
  double dVar68;
  double dVar69;
  double dVar70;
  double dVar71;
  double dVar72;
  double dVar73;
  double dVar74;
  double dVar75;
  float local_104;
  undefined8 local_e8;
  
  pbVar40 = (light_material->__base).__base.Next;
  bVar35 = false;
  if ((light_material->NameHash == 0x93c0eaf6) || (light_material->NameHash == 0xd3a9e9fa)) {
    bVar35 = true;
  }
  bVar36 = false;
  if ((bVar35) && (TheGameFlowManager.CurrentGameFlowState == GAMEFLOW_STATE_RACING)) {
    bVar36 = true;
  }
  pbVar6 = pbVar40[7].Next;
  pbVar23 = pbVar40[7].Prev;
  pbVar1 = pbVar40[7].Next;
  pbVar2 = pbVar40[8].Next;
  pbVar7 = pbVar40->Next;
  pbVar24 = pbVar40->Prev;
  pbVar8 = pbVar40[2].Next;
  pbVar25 = pbVar40[2].Prev;
  pbVar9 = pbVar40[1].Next;
  dVar73 = (double)(float)pbVar40[1].Prev;
  pbVar10 = pbVar40[4].Next;
  pbVar26 = pbVar40[4].Prev;
  pbVar11 = pbVar40[3].Next;
  pbVar12 = pbVar40[5].Next;
  pbVar13 = pbVar40[5].Prev;
  pbVar27 = pbVar40[6].Next;
  pbVar14 = pbVar40[6].Prev;
  pbVar40 = pbVar40[3].Prev;
  if (arn_HackAlpha != 0) {
    dVar73 = (double)arn_AlphaMin;
    pbVar40 = (bNode *)(arn_AlphaMax - arn_AlphaMin);
  }
  if (light_context == (eLightContext *)0x0) {
    return;
  }
  if (light_context->Type != 0) {
    return;
  }
  local_104 = hack_LightColourScale;
  fVar15 = (float)light_context[0x31].Type;
  fVar28 = (float)light_context[0x32].Type;
  fVar16 = (float)light_context[0x33].Type;
  fVar29 = (float)light_context[0x34].Type;
  fVar17 = (float)light_context[0x17].Type;
  dVar61 = (double)(float)light_context[0x11].Type;
  fVar30 = (float)light_context[0x12].Type;
  dVar62 = (double)(float)light_context[0x15].Type;
  fVar31 = (float)light_context[0x16].Type;
  dVar63 = (double)(float)light_context[0x19].Type;
  fVar32 = (float)light_context[0x1a].Type;
  fVar18 = (float)light_context[0x13].Type;
  fVar19 = (float)light_context[0x1b].Type;
  fVar20 = (float)light_context[5].Type;
  fVar33 = (float)light_context[6].Type;
  fVar21 = (float)light_context[9].Type;
  fVar34 = (float)light_context[10].Type;
  fVar22 = (float)light_context[7].Type;
  if (bVar36) {
    local_104 = hack_LightColourScale * 0.7;
  }
  dVar54 = (double)local_104;
  iVar42 = 0;
  iVar50 = 0;
  uVar37 = vertex_description & 0x3f;
  dVar64 = (double)(float)light_context[0xb].Type * dVar54;
  dVar74 = (double)(float)light_context[0xc].Type * dVar54;
  dVar65 = (double)(float)light_context[1].Type * dVar54;
  dVar75 = (double)(float)light_context[2].Type * dVar54;
  dVar67 = dVar64 - dVar64;
  dVar74 = dVar74 - dVar74;
  dVar66 = (double)(float)light_context[3].Type * dVar54;
  if ((vertex_description & 0x80U) == 0) goto LAB_80109644;
  iVar42 = 3;
  if (uVar37 != 0xe) {
    if (uVar37 < 0xf) {
      if (uVar37 != 6) {
        if (uVar37 < 7) {
          if (uVar37 != 2) {
            if (uVar37 > 2) {
              if (uVar37 != 4) goto LAB_80109644;
              goto LAB_80109640;
            }
            if (uVar37 != 0) goto LAB_80109644;
          }
        }
        else if (uVar37 != 10) {
          if (uVar37 > 10) {
            if (uVar37 != 0xc) goto LAB_80109644;
            goto LAB_80109640;
          }
          if (uVar37 != 8) goto LAB_80109644;
        }
LAB_80109638:
        iVar50 = 1;
        goto LAB_80109644;
      }
    }
    else if (uVar37 != 0x16) {
      if (uVar37 < 0x17) {
        if (uVar37 != 0x12) {
          if (uVar37 > 0x12) {
            if (uVar37 != 0x14) goto LAB_80109644;
            goto LAB_80109640;
          }
          if (uVar37 != 0x10) goto LAB_80109644;
        }
      }
      else if (uVar37 != 0x1a) {
        if (uVar37 > 0x1a) {
          if ((uVar37 != 0x1c) && (uVar37 != 0x1e)) goto LAB_80109644;
          goto LAB_80109640;
        }
        if (uVar37 != 0x18) goto LAB_80109644;
      }
      goto LAB_80109638;
    }
  }
LAB_80109640:
  iVar50 = 2;
LAB_80109644:
  iVar41 = 0;
  if (num_indicies > 0) {
    do {
      uVar48 = 0;
      uVar45 = 0;
      uVar38 = 0;
      iVar49 = iVar50 + 7;
      if (uVar37 == 0xe) {
LAB_801097a0:
        iVar39 = iVar41 * iVar49 + 1;
        uVar45 = (uint)*(ushort *)(index_buffer + iVar39 + iVar42);
        uVar48 = (uint)index_buffer[iVar41 * iVar49 + iVar42];
        uVar38 = (uint)*(ushort *)(index_buffer + iVar39 + iVar42 + 2);
      }
      else if (uVar37 < 0xf) {
        if (uVar37 == 0) {
          iVar49 = iVar41 * (iVar50 + 4);
          puVar46 = index_buffer + iVar49 + iVar42;
          uVar38 = (uint)puVar46[2];
LAB_80109764:
          uVar48 = (uint)index_buffer[iVar49 + iVar42];
          uVar45 = (uint)puVar46[1];
        }
        else if (uVar37 != 0) {
          if (uVar37 == 4) {
            iVar49 = iVar41 * (iVar50 + 5);
            puVar46 = index_buffer + iVar49 + iVar42;
            uVar38 = (uint)*(ushort *)(puVar46 + 2);
            goto LAB_80109764;
          }
          iVar49 = iVar50 + 6;
          if (uVar37 == 0xc) goto LAB_801097a0;
        }
      }
      else if (uVar37 == 0x1e) {
        iVar49 = iVar41 * (iVar50 + 8);
LAB_801097bc:
        uVar38 = (uint)*(ushort *)(index_buffer + iVar49 + iVar42 + 4);
        uVar48 = (uint)*(ushort *)(index_buffer + iVar49 + iVar42);
        uVar45 = (uint)*(ushort *)(index_buffer + iVar49 + iVar42 + 2);
      }
      else if (uVar37 < 0x1f) {
        if (uVar37 == 0x14) {
          iVar49 = iVar41 * (iVar50 + 6);
          uVar48 = (uint)*(ushort *)(index_buffer + iVar49 + iVar42);
          uVar45 = (uint)index_buffer[iVar49 + iVar42 + 2];
          uVar38 = (uint)*(ushort *)(index_buffer + iVar49 + iVar42 + 3);
        }
        else if (uVar37 == 0x1c) {
          iVar49 = iVar41 * (iVar50 + 7);
          goto LAB_801097bc;
        }
      }
      else {
        if (uVar37 == 0x20) {
          iVar49 = iVar41 * 3;
          puVar46 = index_buffer + iVar49 + iVar42;
          uVar38 = (uint)puVar46[2];
          goto LAB_80109764;
        }
        if (uVar37 == 0x3c) {
          iVar49 = iVar41 * 6;
          goto LAB_801097bc;
        }
      }
      pfVar43 = (float *)(colour_table0 + uVar38);
      pfVar51 = (float *)(colour_table1 + uVar38);
      if ((colour_table0[uVar38] & 0x1000000) == 0) {
        pfVar47 = (float *)(normal_table + uVar45);
        pfVar44 = (float *)(position_table_16 + uVar48 * 3);
        bVar3 = (byte)(unaff_GQR5 >> 0x10);
        bVar4 = bVar3 & 7;
        bVar5 = (byte)(unaff_GQR5 >> 0x18);
        if ((unaff_GQR5 & 0x3f000000) == 0) {
          dVar72 = 1.0;
        }
        else {
          dVar72 = (double)ldexpf(-(bVar5 & 0x3f));
        }
        if (bVar4 == 4 || bVar4 == 6) {
          dVar59 = dVar72 * (double)*(char *)pfVar44;
          dVar72 = dVar72 * (double)*(char *)((int)pfVar44 + 1);
        }
        else if (bVar4 == 5 || bVar4 == 7) {
          dVar59 = dVar72 * (double)*(short *)pfVar44;
          dVar72 = dVar72 * (double)*(short *)((int)pfVar44 + 2);
        }
        else {
          dVar59 = (double)*pfVar44;
          dVar72 = (double)pfVar44[1];
        }
        pfVar44 = (float *)(position_table_16 + uVar48 * 3 + 2);
        bVar3 = bVar3 & 7;
        if ((unaff_GQR5 & 0x3f000000) == 0) {
          dVar58 = 1.0;
        }
        else {
          dVar58 = (double)ldexpf(-(bVar5 & 0x3f));
        }
        if (bVar3 == 4 || bVar3 == 6) {
          dVar58 = dVar58 * (double)*(char *)pfVar44;
        }
        else if (bVar3 == 5 || bVar3 == 7) {
          dVar58 = dVar58 * (double)*(short *)pfVar44;
        }
        else {
          dVar58 = (double)*pfVar44;
        }
        bVar3 = (byte)(unaff_GQR7 >> 0x10);
        bVar4 = bVar3 & 7;
        bVar5 = (byte)(unaff_GQR7 >> 0x18);
        if ((unaff_GQR7 & 0x3f000000) == 0) {
          dVar70 = 1.0;
        }
        else {
          dVar70 = (double)ldexpf(-(bVar5 & 0x3f));
        }
        if (bVar4 == 4 || bVar4 == 6) {
          dVar56 = dVar70 * (double)*(char *)pfVar47;
          dVar70 = dVar70 * (double)*(char *)((int)pfVar47 + 1);
        }
        else if (bVar4 == 5 || bVar4 == 7) {
          dVar56 = dVar70 * (double)*(short *)pfVar47;
          dVar70 = dVar70 * (double)*(short *)((int)pfVar47 + 2);
        }
        else {
          dVar56 = (double)*pfVar47;
          dVar70 = (double)pfVar47[1];
        }
        pfVar47 = (float *)((int)pfVar47 + 2);
        bVar3 = bVar3 & 7;
        if ((unaff_GQR7 & 0x3f000000) == 0) {
          dVar55 = 1.0;
        }
        else {
          dVar55 = (double)ldexpf(-(bVar5 & 0x3f));
        }
        if (bVar3 == 4 || bVar3 == 6) {
          dVar55 = dVar55 * (double)*(char *)pfVar47;
        }
        else if (bVar3 == 5 || bVar3 == 7) {
          dVar55 = dVar55 * (double)*(short *)pfVar47;
        }
        else {
          dVar55 = (double)*pfVar47;
        }
        dVar59 = (double)fVar15 - dVar59;
        dVar72 = (double)fVar28 - dVar72;
        dVar58 = (double)fVar16 - dVar58;
        dVar53 = (double)testc1[0];
        dVar69 = (double)testc1[1];
        dVar52 = 1.0 / SQRT(dVar58 * dVar58 + dVar59 * dVar59 + dVar72 * dVar72);
        dVar57 = dVar58 * dVar52 * dVar55 + dVar59 * dVar52 * dVar56 + dVar72 * dVar52 * dVar70;
        dVar59 = (dVar56 * dVar57 + dVar56 * dVar57) - dVar59 * dVar52;
        dVar68 = (dVar70 * dVar57 + dVar70 * dVar57) - dVar72 * dVar52;
        dVar72 = ((dVar55 * dVar57 + dVar55 * dVar57) - dVar58 * dVar52) * dVar63 +
                 dVar59 * dVar62 + dVar59 * dVar61;
        dVar59 = ((dVar57 * 1.0 + dVar57 * 1.0) - ((double)fVar29 - 1.0) * dVar52) * dVar63 +
                 dVar68 * dVar62 + dVar68 * dVar61;
        if (dVar72 < 0.0) {
          dVar72 = dVar67;
        }
        if (dVar59 < 0.0) {
          dVar59 = dVar74;
        }
        dVar72 = dVar72 * dVar72 * dVar72;
        dVar59 = dVar59 * dVar59 * dVar59;
        dVar72 = dVar72 * dVar72;
        dVar59 = dVar59 * dVar59;
        dVar58 = dVar59;
        if ((float)(dVar59 - 0.009999999776482582) < 0.0) {
          dVar58 = 0.009999999776482582;
        }
        dVar52 = 1.0;
        if ((float)(dVar59 - 1.0) < 0.0) {
          dVar52 = dVar58;
        }
        if (dVar57 < 0.0) {
          dVar57 = dVar67;
        }
        local_e8 = (double)CONCAT44(0x43300000,-(float)dVar52);
        dVar59 = ((double)testl3[0] * dVar57 * dVar57 * dVar57 +
                 (double)testl2[0] * dVar57 * dVar57 + (double)testl1[0] * dVar57) *
                 (double)(float)pbVar6;
        dVar58 = ((double)testl3[1] * dVar57 * dVar57 * dVar57 +
                 (double)testl2[1] * dVar57 * dVar57 + (double)testl1[1] * dVar57) *
                 (double)(float)pbVar23;
        dVar59 = (double)teste3[0] * dVar59 * dVar59 * dVar59 +
                 (double)teste2[0] * dVar59 * dVar59 + (double)teste1[0] * dVar59 + dVar53;
        dVar58 = (double)teste3[1] * dVar58 * dVar58 * dVar58 +
                 (double)teste2[1] * dVar58 * dVar58 + (double)teste1[1] * dVar58 + dVar69;
        pt.field1.i0 = (int)((float)(local_e8 - 4503601774854144.0) * (float)pbVar1 + (float)pbVar2)
        ;
        dVar59 = dVar59 * dVar59;
        dVar58 = dVar58 * dVar58;
        dVar52 = (double)pt.field0.f0;
        dVar59 = (double)(double)(1.0 / (dVar59 * dVar59));
        dVar58 = (double)(double)(1.0 / (dVar58 * dVar58));
        if (dVar59 < 0.0) {
          dVar59 = (double)testc0[0];
        }
        if (dVar58 < 0.0) {
          dVar58 = (double)testc0[1];
        }
        dVar68 = dVar52;
        if ((float)(dVar52 - 0.009999999776482582) < 0.0) {
          dVar68 = 0.009999999776482582;
        }
        if (dVar53 < 0.0) {
          dVar53 = dVar59;
        }
        if (dVar69 < 0.0) {
          dVar69 = dVar58;
        }
        dVar59 = 1.0;
        if ((float)(dVar52 - 1.0) < 0.0) {
          dVar59 = dVar68;
        }
        dVar58 = dVar63 * dVar55 + dVar62 * dVar70 + dVar61 * dVar56;
        dVar52 = (double)fVar32 * dVar55 + (double)fVar31 * dVar70 + (double)fVar30 * dVar56;
        if (dVar58 < 0.0) {
          dVar58 = dVar67;
        }
        if (dVar52 < 0.0) {
          dVar52 = dVar74;
        }
        dVar60 = (double)(float)(dVar59 * 0.5);
        dVar68 = (double)(float)pbVar13 * dVar53 + (double)(float)pbVar10;
        dVar59 = (double)fVar19 * dVar55 + (double)fVar17 * dVar70 + (double)fVar18 * dVar56;
        if (dVar59 < 0.0) {
          dVar59 = dVar67;
        }
        dVar71 = (double)(float)pbVar40 * dVar57 + dVar73;
        dVar55 = (double)(float)pbVar8 * dVar57 + (double)(float)pbVar7;
        dVar70 = (dVar64 * dVar59 + (double)fVar22 * dVar54 * dVar52 + dVar66 * dVar58) *
                 ((double)(float)pbVar11 * dVar57 + (double)(float)pbVar9);
        dVar57 = dVar65 * dVar72;
        dVar56 = ((double)fVar21 * dVar54 * dVar59 +
                 (double)fVar20 * dVar54 * dVar52 + dVar65 * dVar58) * dVar55;
        dVar59 = ((double)fVar34 * dVar54 * dVar59 +
                 (double)fVar33 * dVar54 * dVar52 + dVar75 * dVar58) *
                 ((double)(float)pbVar25 * dVar55 + (double)(float)pbVar24);
        if (TheGameFlowManager.CurrentGameFlowState == GAMEFLOW_STATE_RACING) {
          local_104 = hack_SpecScale_InGame;
        }
        else {
          local_104 = hack_SpecScale;
        }
        dVar58 = (double)local_104;
        dVar55 = (double)(float)(dVar69 + dVar69);
        dVar72 = dVar66 * dVar72 *
                 ((double)(float)pbVar14 * dVar53 + (double)(float)pbVar12) * dVar60 * dVar58;
        dVar53 = dVar57 * dVar68 * dVar60 * dVar58;
        dVar58 = dVar75 * dVar57 *
                 ((double)(float)pbVar27 * dVar68 + (double)(float)pbVar26) * dVar60 * dVar58;
        bVar3 = (byte)unaff_GQR6;
        bVar4 = bVar3 & 7;
        bVar5 = (byte)(unaff_GQR6 >> 8);
        if ((unaff_GQR6 & 0x3f00) == 0) {
          dVar57 = 1.0;
        }
        else {
          dVar57 = (double)ldexpf(bVar5 & 0x3f);
        }
        if (bVar4 == 4 || bVar4 == 6) {
          *(char *)pfVar43 = (char)(dVar57 * dVar56);
          *(char *)((int)pfVar43 + 1) = (char)(dVar57 * dVar59);
        }
        else if (bVar4 == 5 || bVar4 == 7) {
          *(short *)pfVar43 = (short)(dVar57 * dVar56);
          *(short *)((int)pfVar43 + 2) = (short)(dVar57 * dVar59);
        }
        else {
          *pfVar43 = (float)dVar56;
          pfVar43[1] = (float)dVar59;
        }
        pfVar44 = (float *)((int)pfVar43 + 2);
        bVar4 = bVar3 & 7;
        if ((unaff_GQR6 & 0x3f00) == 0) {
          dVar59 = 1.0;
        }
        else {
          dVar59 = (double)ldexpf(bVar5 & 0x3f);
        }
        if (bVar4 == 4 || bVar4 == 6) {
          *(char *)pfVar44 = (char)(dVar59 * dVar70);
          *(char *)((int)pfVar43 + 3) = (char)(dVar59 * dVar71);
        }
        else if (bVar4 == 5 || bVar4 == 7) {
          *(short *)pfVar44 = (short)(dVar59 * dVar70);
          *(short *)(pfVar43 + 1) = (short)(dVar59 * dVar71);
        }
        else {
          *pfVar44 = (float)dVar70;
          *(float *)((int)pfVar43 + 6) = (float)dVar71;
        }
        bVar4 = bVar3 & 7;
        if ((unaff_GQR6 & 0x3f00) == 0) {
          dVar59 = 1.0;
        }
        else {
          dVar59 = (double)ldexpf(bVar5 & 0x3f);
        }
        if (bVar4 == 4 || bVar4 == 6) {
          *(char *)pfVar51 = (char)(dVar59 * dVar53);
          *(char *)((int)pfVar51 + 1) = (char)(dVar59 * dVar58);
        }
        else if (bVar4 == 5 || bVar4 == 7) {
          *(short *)pfVar51 = (short)(dVar59 * dVar53);
          *(short *)((int)pfVar51 + 2) = (short)(dVar59 * dVar58);
        }
        else {
          *pfVar51 = (float)dVar53;
          pfVar51[1] = (float)dVar58;
        }
        pfVar44 = (float *)((int)pfVar51 + 2);
        bVar3 = bVar3 & 7;
        if ((unaff_GQR6 & 0x3f00) == 0) {
          dVar59 = 1.0;
        }
        else {
          dVar59 = (double)ldexpf(bVar5 & 0x3f);
        }
        if (bVar3 == 4 || bVar3 == 6) {
          *(char *)pfVar44 = (char)(dVar59 * dVar72);
          *(char *)((int)pfVar51 + 3) = (char)(dVar59 * dVar55);
        }
        else if (bVar3 == 5 || bVar3 == 7) {
          *(short *)pfVar44 = (short)(dVar59 * dVar72);
          *(short *)(pfVar51 + 1) = (short)(dVar59 * dVar55);
        }
        else {
          *pfVar44 = (float)dVar72;
          *(float *)((int)pfVar51 + 6) = (float)(dVar69 + dVar69);
        }
        if (arn_EnableDiffuse == 0) {
          *(byte *)((int)pfVar43 + 2) = 0;
          *(byte *)pfVar43 = 0;
          *(byte *)((int)pfVar43 + 1) = 0;
        }
        if (arn_EnableAlpha == 0) {
          *(byte *)((int)pfVar43 + 3) = 0;
        }
        if (arn_EnableSpec == 0) {
          *(char *)((int)pfVar51 + 2) = '\0';
          *(char *)pfVar51 = '\0';
          *(char *)((int)pfVar51 + 1) = '\0';
        }
        if (arn_EnableEnv == 0) {
          *(char *)((int)pfVar51 + 3) = '\0';
        }
        *(byte *)pfVar43 = *(byte *)pfVar43 | 1;
      }
      iVar41 = iVar41 + 1;
    } while (iVar41 < num_indicies);
  }
  return;
}



/* ================================================================== */
/* 80109bb8  UpdatePlatInfo__27eLightMaterialPlatInterface
   tam=2044 B  insn=511  cobertura=2044/2044  paired-singles=36  firma=IMPORTED
   proto: undefined eLightMaterialPlatInterface::UpdatePlatInfo(eLightMaterialPlatInterface * this)
*/

/* WARNING: Removing unreachable block (ram,0x8010a3a8) */
/* WARNING: Removing unreachable block (ram,0x8010a3a4) */
/* WARNING: Removing unreachable block (ram,0x8010a3a0) */
/* WARNING: Removing unreachable block (ram,0x8010a39c) */
/* WARNING: Removing unreachable block (ram,0x8010a398) */
/* WARNING: Removing unreachable block (ram,0x8010a394) */
/* WARNING: Removing unreachable block (ram,0x8010a390) */
/* WARNING: Removing unreachable block (ram,0x8010a38c) */
/* WARNING: Removing unreachable block (ram,0x8010a388) */
/* WARNING: Removing unreachable block (ram,0x8010a384) */
/* WARNING: Removing unreachable block (ram,0x8010a380) */
/* WARNING: Removing unreachable block (ram,0x8010a37c) */
/* WARNING: Removing unreachable block (ram,0x8010a378) */
/* WARNING: Removing unreachable block (ram,0x8010a374) */
/* WARNING: Removing unreachable block (ram,0x8010a370) */
/* WARNING: Removing unreachable block (ram,0x8010a36c) */
/* WARNING: Removing unreachable block (ram,0x8010a368) */
/* WARNING: Removing unreachable block (ram,0x8010a364) */
/* WARNING: Removing unreachable block (ram,0x80109c00) */
/* WARNING: Removing unreachable block (ram,0x80109bfc) */
/* WARNING: Removing unreachable block (ram,0x80109bf8) */
/* WARNING: Removing unreachable block (ram,0x80109bf4) */
/* WARNING: Removing unreachable block (ram,0x80109bf0) */
/* WARNING: Removing unreachable block (ram,0x80109bec) */
/* WARNING: Removing unreachable block (ram,0x80109be8) */
/* WARNING: Removing unreachable block (ram,0x80109be4) */
/* WARNING: Removing unreachable block (ram,0x80109be0) */
/* WARNING: Removing unreachable block (ram,0x80109bdc) */
/* WARNING: Removing unreachable block (ram,0x80109bd8) */
/* WARNING: Removing unreachable block (ram,0x80109bd4) */
/* WARNING: Removing unreachable block (ram,0x80109bd0) */
/* WARNING: Removing unreachable block (ram,0x80109bcc) */
/* WARNING: Removing unreachable block (ram,0x80109bc8) */
/* WARNING: Removing unreachable block (ram,0x80109bc4) */
/* WARNING: Removing unreachable block (ram,0x80109bc0) */
/* WARNING: Removing unreachable block (ram,0x80109bbc) */
/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void eLightMaterialPlatInterface__UpdatePlatInfo(eLightMaterialPlatInterface *this)

{
  undefined *puVar1;
  undefined *puVar2;
  undefined *puVar3;
  undefined *puVar4;
  undefined *puVar5;
  undefined *puVar6;
  undefined *puVar7;
  float fVar8;
  float fVar9;
  float fVar10;
  float fVar11;
  float fVar12;
  float fVar13;
  float fVar14;
  float fVar15;
  float fVar16;
  float fVar17;
  float fVar18;
  float fVar19;
  float fVar20;
  float fVar21;
  float fVar22;
  float fVar23;
  float fVar24;
  float fVar25;
  float fVar26;
  float fVar27;
  undefined *puVar28;
  int iVar29;
  int iVar30;
  float *pfVar31;
  int iVar32;
  int iVar33;
  undefined *puVar34;
  int iVar35;
  int iVar36;
  
  puVar1 = this[0x16].PlatInfo;
  puVar2 = this[0xc].PlatInfo;
  puVar3 = this[0x17].PlatInfo;
  puVar4 = this[0x10].PlatInfo;
  puVar34 = this[3].PlatInfo;
  pfVar31 = (float *)this->PlatInfo;
  fVar11 = (float)puVar2 * (float)this[0xd].PlatInfo;
  fVar12 = (float)puVar2 * (float)this[0xf].PlatInfo;
  fVar17 = (float)puVar4 * (float)this[0x11].PlatInfo;
  fVar18 = (float)puVar4 * (float)this[0x12].PlatInfo;
  fVar13 = (float)puVar2 * (float)this[0xe].PlatInfo;
  fVar19 = (float)puVar4 * (float)this[0x13].PlatInfo;
  fVar14 = (float)puVar3 * (float)this[0x18].PlatInfo;
  puVar2 = this[0x1f].PlatInfo;
  fVar15 = (float)puVar3 * (float)this[0x19].PlatInfo;
  fVar16 = (float)puVar3 * (float)this[0x1a].PlatInfo;
  puVar3 = this[0x1b].PlatInfo;
  fVar10 = (float)puVar2 * 1.0653532e+09;
  puVar4 = this[0x20].PlatInfo;
  puVar28 = this[0x24].PlatInfo;
  fVar20 = fVar17 - fVar11;
  puVar5 = this[0x1c].PlatInfo;
  fVar22 = fVar18 - fVar13;
  puVar6 = this[0x1d].PlatInfo;
  fVar21 = fVar19 - fVar12;
  puVar7 = this[0x1e].PlatInfo;
  fVar8 = (float)puVar4 * (float)this[0x21].PlatInfo;
  fVar23 = (float)puVar4 * (float)this[0x22].PlatInfo;
  fVar24 = (float)puVar4 * (float)this[0x23].PlatInfo;
  puVar4 = this[0x14].PlatInfo;
  fVar25 = (float)puVar28 * (float)this[0x25].PlatInfo;
  fVar26 = (float)puVar28 * (float)this[0x26].PlatInfo;
  fVar9 = (float)this[0x15].PlatInfo - (float)puVar4;
  fVar27 = (float)puVar28 * (float)this[0x27].PlatInfo;
  if (puVar34 != (undefined *)0x8a616c4c) {
    if (puVar34 < (undefined *)0x8a616c4d) {
      if (puVar34 != (undefined *)0x68e97f75) {
        if (puVar34 < (undefined *)0x68e97f76) {
          if (puVar34 == (undefined *)0x23e6b396) {
LAB_8010a108:
            puVar2 = (undefined *)0x40c00000;
            fVar17 = 0.3;
            if (this[3].PlatInfo == (undefined *)0x2388dd82) {
              fVar17 = 0.0;
            }
            fVar18 = (float)this[0x13].PlatInfo * fVar17;
            fVar19 = (float)this[0x11].PlatInfo * fVar17;
            fVar17 = (float)this[0x12].PlatInfo * fVar17;
            fVar9 = 0.86;
            fVar20 = fVar11 - fVar19;
            fVar22 = fVar13 - fVar17;
            fVar21 = fVar12 - fVar18;
            puVar4 = (undefined *)0x3e6147ae;
            fVar11 = fVar19;
            fVar12 = fVar18;
            fVar13 = fVar17;
          }
          else {
            if (puVar34 < (undefined *)0x23e6b397) {
              if (puVar34 == (undefined *)0x2388dd82) goto LAB_8010a108;
              if (puVar34 > (undefined *)0x2388dd82) {
                puVar28 = (undefined *)0x23e6a3f4;
                goto LAB_80109f88;
              }
              if (puVar34 == (undefined *)0x12c9453c) {
LAB_8010a058:
                fVar8 = 0.4;
                puVar4 = (undefined *)((float)puVar4 * 0.25);
                fVar24 = fVar8;
                fVar23 = fVar8;
                goto LAB_8010a1ac;
              }
            }
            else {
              if (puVar34 == (undefined *)0x33a26cb6) {
                puVar4 = (undefined *)((float)puVar4 * 0.25);
                goto LAB_8010a1ac;
              }
              if (puVar34 < (undefined *)0x33a26cb7) {
                if (puVar34 == (undefined *)0x33310077) {
                  fVar20 = fVar11 - fVar17;
                  fVar22 = fVar13 - fVar18;
                  fVar21 = fVar12 - fVar19;
                  fVar11 = fVar17;
                  fVar12 = fVar19;
                  fVar13 = fVar18;
                  goto LAB_8010a1ac;
                }
              }
              else {
                if (puVar34 == (undefined *)0x3ed70c43) goto LAB_8010a058;
                if (puVar34 == (undefined *)0x471a1dca) {
                  puVar2 = (undefined *)0x40000000;
                  fVar9 = (float)this[0x15].PlatInfo * 1.18 - (float)puVar4 * 0.25;
                  puVar4 = (undefined *)((float)puVar4 * 0.25);
                  goto LAB_8010a1ac;
                }
              }
            }
LAB_80109ffc:
            fVar8 = fVar8 * 0.1;
            fVar26 = fVar26 * 0.1;
            fVar27 = fVar27 * 0.1;
            fVar25 = fVar25 * 0.1;
            fVar24 = fVar24 * 0.1;
            fVar23 = fVar23 * 0.1;
          }
          goto LAB_8010a1ac;
        }
        if (puVar34 == (undefined *)0x8812634b) goto LAB_8010a0c4;
        if (puVar34 > (undefined *)0x8812634b) {
          if (puVar34 != (undefined *)0x89f23a14) {
            if (puVar34 > (undefined *)0x89f23a14) {
              if (puVar34 != (undefined *)0x8a5626d8) {
                puVar28 = (undefined *)0x8a60f52c;
                goto LAB_80109ff4;
              }
              goto LAB_8010a0c4;
            }
            if (puVar34 != (undefined *)0x89946400) goto LAB_80109ffc;
          }
          puVar2 = (undefined *)0x40000000;
          fVar9 = (float)this[0x15].PlatInfo * 1.18 - (float)puVar4 * 0.25;
          puVar4 = (undefined *)((float)puVar4 * 0.25);
          goto LAB_8010a1ac;
        }
        if (puVar34 != (undefined *)0x8050ca3f) {
          if (puVar34 < (undefined *)0x8050ca40) {
            puVar28 = (undefined *)0x6cf41096;
          }
          else {
            puVar28 = (undefined *)0x8062e2c0;
          }
          goto LAB_80109f88;
        }
      }
    }
    else {
      if (puVar34 == (undefined *)0xa6348ee3) {
        fVar9 = arnMaxA - arnMinA;
        puVar4 = (undefined *)arnMinA;
        fVar26 = arnEnvMax;
        fVar27 = arnEnvMax;
        fVar25 = arnEnvMax;
        goto LAB_8010a1ac;
      }
      if (puVar34 < (undefined *)0xa6348ee4) {
        if (puVar34 != (undefined *)0x8cb61dcd) {
          if (puVar34 < (undefined *)0x8cb61dce) {
            if (puVar34 != (undefined *)0x8a66da4b) {
              if (puVar34 < (undefined *)0x8a66da4c) {
                puVar28 = (undefined *)0x8a6435ca;
              }
              else {
                puVar28 = (undefined *)0x8a690bf6;
              }
              goto LAB_80109ff4;
            }
            goto LAB_8010a0ec;
          }
          if (puVar34 != (undefined *)0x8cb95e6b) {
            if (puVar34 < (undefined *)0x8cb95e6c) {
              puVar28 = (undefined *)0x8cb694ed;
            }
            else {
              if (puVar34 == (undefined *)0x8cbc02ec) goto LAB_8010a108;
              puVar28 = (undefined *)0x8cbe3497;
            }
LAB_80109f88:
            if (puVar34 != puVar28) goto LAB_80109ffc;
          }
        }
        goto LAB_8010a108;
      }
      if (puVar34 == (undefined *)0xd57882e1) {
LAB_8010a0c4:
        puVar2 = (undefined *)0x41000000;
        fVar9 = (float)this[0x15].PlatInfo * 1.18 - (float)puVar4 * 0.25;
        puVar4 = (undefined *)((float)puVar4 * 0.25);
        goto LAB_8010a1ac;
      }
      if (puVar34 < (undefined *)0xd57882e2) {
        if (puVar34 != (undefined *)0xc7228aa0) {
          if (puVar34 < (undefined *)0xc7228aa1) {
            puVar28 = (undefined *)0xb3645f81;
            goto LAB_80109f88;
          }
          if (puVar34 == (undefined *)0xc9d2b801) goto LAB_8010a108;
          if (puVar34 != (undefined *)0xd51aaccd) goto LAB_80109ffc;
          goto LAB_8010a0c4;
        }
      }
      else if (puVar34 != (undefined *)0xd6ec6733) {
        if (puVar34 < (undefined *)0xd6ec6734) {
          puVar28 = (undefined *)0xd68ea0c1;
        }
        else {
          if (puVar34 == (undefined *)0xd6ec76d5) goto LAB_8010a0ec;
          puVar28 = (undefined *)0xdd90e320;
        }
LAB_80109ff4:
        if (puVar34 != puVar28) goto LAB_80109ffc;
      }
    }
  }
LAB_8010a0ec:
  puVar2 = (undefined *)0x40c00000;
  fVar9 = 0.86;
  puVar4 = (undefined *)0x3e6147ae;
LAB_8010a1ac:
  iVar35 = 0;
  if ((int)(fVar8 * 128.0) > 0) {
    iVar35 = (int)(fVar8 * 128.0);
  }
  if (iVar35 > 0xff) {
    iVar35 = 0xff;
  }
  iVar36 = 0;
  if ((int)(fVar23 * 128.0) > 0) {
    iVar36 = (int)(fVar23 * 128.0);
  }
  if (iVar36 > 0xff) {
    iVar36 = 0xff;
  }
  iVar30 = 0;
  if ((int)(fVar24 * 128.0) > 0) {
    iVar30 = (int)(fVar24 * 128.0);
  }
  if (iVar30 > 0xff) {
    iVar30 = 0xff;
  }
  iVar29 = 0;
  if ((int)(fVar25 * 128.0) > 0) {
    iVar29 = (int)(fVar25 * 128.0);
  }
  if (iVar29 > 0xff) {
    iVar29 = 0xff;
  }
  iVar32 = 0;
  if ((int)(fVar26 * 128.0) > 0) {
    iVar32 = (int)(fVar26 * 128.0);
  }
  if (iVar32 > 0xff) {
    iVar32 = 0xff;
  }
  iVar33 = 0;
  if ((int)(fVar27 * 128.0) > 0) {
    iVar33 = (int)(fVar27 * 128.0);
  }
  if (iVar33 > 0xff) {
    iVar33 = 0xff;
  }
  *pfVar31 = fVar11;
  pfVar31[1] = fVar13;
  pfVar31[2] = fVar12;
  pfVar31[0x13] = (float)(iVar29 << 0x18 | iVar32 << 0x10 | iVar33 << 8);
  pfVar31[3] = (float)puVar4;
  pfVar31[4] = fVar20;
  pfVar31[5] = fVar22;
  pfVar31[6] = fVar21;
  pfVar31[7] = fVar9;
  pfVar31[8] = fVar14;
  pfVar31[9] = fVar15;
  pfVar31[10] = fVar16;
  pfVar31[0xb] = (float)puVar3 * (float)puVar5 - fVar14;
  pfVar31[0xc] = (float)puVar3 * (float)puVar6 - fVar15;
  pfVar31[0xd] = (float)puVar3 * (float)puVar7 - fVar16;
  pfVar31[0xe] = (float)puVar1;
  pfVar31[0xf] = (float)puVar2;
  pfVar31[0x10] = 1.0653532e+09 - (float)puVar1 * 1.0653532e+09;
  pfVar31[0x12] = (float)(iVar35 << 0x18 | iVar36 << 0x10 | iVar30 << 8);
  pfVar31[0x11] = 1.0653532e+09 - fVar10;
  return;
}



/* ================================================================== */
/* 8026c574  ActualReadJoystickData__Fv
   tam=1588 B  insn=397  cobertura=1588/1588  paired-singles=14  firma=IMPORTED
   proto: int ActualReadJoystickData(void)
*/

/* WARNING: Removing unreachable block (ram,0x8026cb9c) */
/* WARNING: Removing unreachable block (ram,0x8026cb98) */
/* WARNING: Removing unreachable block (ram,0x8026cb94) */
/* WARNING: Removing unreachable block (ram,0x8026cb90) */
/* WARNING: Removing unreachable block (ram,0x8026cb8c) */
/* WARNING: Removing unreachable block (ram,0x8026cb88) */
/* WARNING: Removing unreachable block (ram,0x8026cb84) */
/* WARNING: Removing unreachable block (ram,0x8026c594) */
/* WARNING: Removing unreachable block (ram,0x8026c590) */
/* WARNING: Removing unreachable block (ram,0x8026c58c) */
/* WARNING: Removing unreachable block (ram,0x8026c588) */
/* WARNING: Removing unreachable block (ram,0x8026c584) */
/* WARNING: Removing unreachable block (ram,0x8026c580) */
/* WARNING: Removing unreachable block (ram,0x8026c57c) */
/* WARNING: Unknown calling convention -- yet parameter storage is locked */

int ActualReadJoystickData(void)

{
  char cVar1;
  byte bVar2;
  byte bVar3;
  float fVar4;
  ushort uVar5;
  PADStatus *pPVar6;
  uint uVar7;
  float fVar8;
  PADStatus *pPVar9;
  char cVar10;
  uchar uVar11;
  uchar uVar12;
  char cVar13;
  undefined1 uVar14;
  char cVar15;
  char cVar16;
  uchar uVar17;
  uchar uVar18;
  LGWheels *pLVar19;
  uint uVar20;
  bool bVar22;
  int iVar21;
  short sVar23;
  int iVar24;
  JoyData *dest;
  
  if ((JoystickInitialized == 0) ||
     (uVar7 = JoystickRingBufferTop + 1U & 0x1f, uVar7 == JoystickRingBufferBottom)) {
    iVar24 = 0;
  }
  else {
    PADRead(HardwarePadStatus);
    iVar24 = 0;
    PADClamp(HardwarePadStatus);
    LGWheels__ReadAll(plat_lgwheels);
    do {
      iVar21 = JoystickRingBufferTop;
      dest = PadRingData[iVar24] + JoystickRingBufferTop;
      bMemSet(dest,0xff,0x3c);
      if (HardwarePadStatus[iVar24].err == '\0') {
        pPVar6 = HardwarePadStatus + iVar24;
        uVar11 = pPVar6->analogA;
        uVar12 = pPVar6->analogB;
        cVar13 = pPVar6->err;
        uVar14 = pPVar6->field_0xb;
        pPVar6 = HardwarePadStatus + iVar24;
        cVar1 = pPVar6->stickX;
        cVar10 = pPVar6->stickY;
        pPVar9 = HardwarePadStatus + iVar24;
        cVar15 = pPVar9->substickX;
        cVar16 = pPVar9->substickY;
        uVar17 = pPVar9->triggerLeft;
        uVar18 = pPVar9->triggerRight;
        (dest->padSTATUS).button = pPVar6->button;
        (dest->padSTATUS).stickX = cVar1;
        (dest->padSTATUS).stickY = cVar10;
        (dest->padSTATUS).analogA = uVar11;
        (dest->padSTATUS).analogB = uVar12;
        (dest->padSTATUS).err = cVar13;
        (dest->padSTATUS).field_0xb = uVar14;
        (dest->padSTATUS).substickX = cVar15;
        (dest->padSTATUS).substickY = cVar16;
        (dest->padSTATUS).triggerLeft = uVar17;
        (dest->padSTATUS).triggerRight = uVar18;
        dest->ThePadData[0].Type = 'A';
        uVar5 = (dest->padSTATUS).button;
        dest->ThePadData[0].Error = '\0';
        PadRingData[iVar24][iVar21].ThePadData[0].DigitalButtons =
             ~(uVar5 >> 8 & 1 | uVar5 >> 8 & 2 | uVar5 >> 8 & 4 | uVar5 >> 8 & 8 | uVar5 & 0x10 |
               uVar5 >> 7 & 0x20 | (uVar5 & 8) << 5 | (uVar5 & 4) << 7 | (uVar5 & 1) << 10 |
              (uVar5 & 2) << 10);
        uVar20 = (int)((float)(int)(dest->padSTATUS).substickX * 2.15) + 0x80;
        if ((uVar20 & 0x8000) != 0) {
          uVar20 = 0;
        }
        if ((short)uVar20 > 0xff) {
          uVar20 = 0xff;
        }
        cVar1 = (dest->padSTATUS).substickY;
        dest->ThePadData[0].AnalogRightX = (uchar)uVar20;
        uVar20 = 0x80 - (int)((float)(int)cVar1 * 2.15);
        if ((uVar20 & 0x8000) != 0) {
          uVar20 = 0;
        }
        if ((short)uVar20 > 0xff) {
          uVar20 = 0xff;
        }
        cVar1 = (dest->padSTATUS).stickX;
        dest->ThePadData[0].AnalogRightY = (uchar)uVar20;
        uVar20 = (int)((float)(int)cVar1 * 1.75) + 0x80;
        sVar23 = (short)uVar20;
        if ((uVar20 & 0x8000) != 0) {
          sVar23 = 0;
        }
        if (sVar23 > 0xff) {
          sVar23 = 0xff;
        }
        dest->ThePadData[0].AnalogLeftX = (uchar)sVar23;
        bVar2 = (dest->padSTATUS).triggerLeft;
        bVar3 = (dest->padSTATUS).triggerRight;
        dest->ThePadData[0].AnalogLeftY =
             0x80 - (char)(int)((float)(int)(dest->padSTATUS).stickY * 1.75);
        dest->ThePadData[0].LTrigger = (uchar)(int)((float)bVar2 * 1.7);
        dest->ThePadData[0].RTrigger = (uchar)(int)((float)bVar3 * 1.7);
      }
      else {
        bVar22 = LGWheels__IsConnected(plat_lgwheels,iVar24);
        iVar21 = RealTimer.PackedTime;
        if (bVar22) {
          if (wasWheelConnected[iVar24] == '\0') {
            wasWheelConnected[iVar24] = '\x01';
            calibrationTimer[iVar24] = 7.0;
            lastCalibTime[iVar24] = (float)iVar21 * 0.00025;
          }
          if ((calibrationTimer[iVar24] < 5.0) && (notYetCalibrating[iVar24] != '\0')) {
            AutoCalibrateWheel(iVar24);
            notYetCalibrating[iVar24] = '\0';
          }
          if (calibrationTimer[iVar24] > 0.0) {
            fVar4 = lastCalibTime[iVar24];
            fVar8 = (float)RealTimer.PackedTime;
            lastCalibTime[iVar24] = fVar8 * 0.00025;
            calibrationTimer[iVar24] = calibrationTimer[iVar24] - (fVar8 * 0.00025 - fVar4);
          }
          pLVar19 = plat_lgwheels;
          (dest->padSTATUS).button = plat_lgwheels->Position[iVar24].button;
          uVar5 = pLVar19->Position[iVar24].button;
          dest->ThePadData[0].Error = '\0';
          HardwarePadStatus[iVar24].button = uVar5;
          bVar22 = LGWheels__PedalsConnected(pLVar19,iVar24);
          if (bVar22) {
            dest->ThePadData[0].Type = 'Q';
          }
          else {
            dest->ThePadData[0].Type = 'P';
          }
          pLVar19 = plat_lgwheels;
          uVar5 = (dest->padSTATUS).button;
          dest->ThePadData[0].DigitalButtons =
               ~(uVar5 >> 8 & 1 | uVar5 >> 8 & 2 | uVar5 >> 8 & 4 | uVar5 >> 8 & 8 | uVar5 & 0x10 |
                 uVar5 >> 7 & 0x20 | (uVar5 & 8) << 5 | (uVar5 & 4) << 7 | (uVar5 & 1) << 10 |
                (uVar5 & 2) << 10);
          cVar1 = pLVar19->Position[iVar24].wheel;
          dest->ThePadData[0].AnalogRightX = '\0';
          dest->ThePadData[0].AnalogLeftX = cVar1 + 0x80;
          bVar22 = LGWheels__PedalsConnected(pLVar19,iVar24);
          pLVar19 = plat_lgwheels;
          if (bVar22) {
            dest->ThePadData[0].AnalogRightY = plat_lgwheels->Position[iVar24].accelerator;
            dest->ThePadData[0].AnalogLeftY = pLVar19->Position[iVar24].brake;
          }
          else {
            dest->ThePadData[0].AnalogLeftY = bVar22;
            dest->ThePadData[0].AnalogRightY = bVar22;
          }
          pLVar19 = plat_lgwheels;
          dest->ThePadData[0].LTrigger = plat_lgwheels->Position[iVar24].triggerLeft;
          dest->ThePadData[0].RTrigger = pLVar19->Position[iVar24].triggerRight;
        }
        else {
          dest->ThePadData[0].Type = 0xff;
          iVar21 = PADMASKS[iVar24];
          wasWheelConnected[iVar24] = bVar22;
          notYetCalibrating[iVar24] = '\x01';
          dest->ThePadData[0].Error = '\x01';
          PADReset(iVar21);
          HardwarePadStatus[iVar24].button = (ushort)bVar22;
        }
      }
      iVar24 = iVar24 + 1;
    } while (iVar24 < 4);
    iVar24 = 1;
    JoystickRingBufferTop = uVar7;
  }
  return iVar24;
}



/* ================================================================== */
/* 800d80fc  GenerateRoadNoise__16CARSFX_RoadNoise
   tam=1240 B  insn=310  cobertura=1240/1240  paired-singles=8  firma=IMPORTED
   proto: undefined CARSFX_RoadNoise::GenerateRoadNoise(CARSFX_RoadNoise * this)
*/

/* WARNING: Removing unreachable block (ram,0x800d85c8) */
/* WARNING: Removing unreachable block (ram,0x800d85c4) */
/* WARNING: Removing unreachable block (ram,0x800d85c0) */
/* WARNING: Removing unreachable block (ram,0x800d85bc) */
/* WARNING: Removing unreachable block (ram,0x800d8110) */
/* WARNING: Removing unreachable block (ram,0x800d810c) */
/* WARNING: Removing unreachable block (ram,0x800d8108) */
/* WARNING: Removing unreachable block (ram,0x800d8104) */
/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void CARSFX_RoadNoise__GenerateRoadNoise(CARSFX_RoadNoise *this)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  EAX_CarState *pEVar6;
  SFXCTL_Wheel *pSVar7;
  float fVar8;
  
  pEVar6 = ((this->__base).__base.__base.m_pStateBase)->m_pCar;
  fVar1 = (pEVar6->mVel0).y;
  fVar2 = (pEVar6->mVel0).x;
  fVar3 = (pEVar6->mVel0).z;
  fVar2 = fVar3 * fVar3 + fVar2 * fVar2 + fVar1 * fVar1;
  fVar1 = 0.0;
  if (fVar2 > 5e-11) {
    fVar1 = 1.0 / SQRT(fVar2);
    fVar1 = -(fVar2 * fVar1 * fVar1 - 1.0) * fVar1 * 0.5 + fVar1;
    fVar1 = (-(fVar2 * fVar1 * fVar1 - 1.0) * fVar1 * 0.5 + fVar1) * fVar2;
  }
  fVar8 = Graph__GetValue(&RoadNoiseVolGraph,fVar1 * 2.23699);
  fVar2 = (this->m_pWheelCtl->m_bvTotalRightWheelSlip).y;
  fVar3 = (this->m_pWheelCtl->m_bvTotalRightWheelSlip).x;
  fVar2 = fVar3 * fVar3 + fVar2 * fVar2;
  fVar3 = (float)((int)fVar8 * 0x7fff >> 0xf);
  if (fVar2 <= 5e-11) {
    fVar2 = 0.0;
  }
  else {
    fVar8 = 1.0 / SQRT(fVar2);
    fVar8 = -(fVar2 * fVar8 * fVar8 - 1.0) * fVar8 * 0.5 + fVar8;
    fVar2 = (-(fVar2 * fVar8 * fVar8 - 1.0) * fVar8 * 0.5 + fVar8) * fVar2;
  }
  fVar8 = fVar2 * 0.01;
  if (fVar2 * 0.01 > 0.15) {
    fVar8 = 0.15;
  }
  fVar5 = fVar3 * fVar8 + fVar3;
  fVar2 = (this->m_pWheelCtl->m_bvTotalLeftWheelSlip).y;
  fVar8 = (this->m_pWheelCtl->m_bvTotalLeftWheelSlip).x;
  fVar2 = fVar8 * fVar8 + fVar2 * fVar2;
  fVar5 = fVar5 * 0.1 + fVar5;
  if (fVar2 <= 5e-11) {
    fVar2 = 0.0;
  }
  else {
    fVar8 = 1.0 / SQRT(fVar2);
    fVar8 = -(fVar2 * fVar8 * fVar8 - 1.0) * fVar8 * 0.5 + fVar8;
    fVar2 = (-(fVar2 * fVar8 * fVar8 - 1.0) * fVar8 * 0.5 + fVar8) * fVar2;
  }
  fVar8 = fVar2 * 0.01;
  if (fVar2 * 0.01 > 0.15) {
    fVar8 = 0.15;
  }
  fVar3 = fVar3 * fVar8 + fVar3;
  fVar2 = (this->m_pWheelCtl->m_fWheelTractionMag[0] + this->m_pWheelCtl->m_fWheelTractionMag[3]) *
          0.5 * 0.1;
  if (fVar2 > 0.1) {
    fVar2 = 0.1;
  }
  fVar3 = fVar3 * fVar2 + fVar3;
  if (fVar3 > 32000.0) {
    fVar3 = 32000.0;
  }
  if (fVar5 > 32000.0) {
    fVar5 = 32000.0;
  }
  fVar8 = Slope__GetValue(&RoadNoiseSpeedToPitch,fVar1 * 2.23699);
  fVar1 = (this->m_pWheelCtl->m_bvTotalLeftWheelSlip).y;
  fVar2 = (this->m_pWheelCtl->m_bvTotalLeftWheelSlip).x;
  fVar1 = fVar2 * fVar2 + fVar1 * fVar1;
  if (fVar1 <= 5e-11) {
    fVar1 = 0.0;
  }
  else {
    fVar2 = 1.0 / SQRT(fVar1);
    fVar2 = -(fVar1 * fVar2 * fVar2 - 1.0) * fVar2 * 0.5 + fVar2;
    fVar1 = (-(fVar1 * fVar2 * fVar2 - 1.0) * fVar2 * 0.5 + fVar2) * fVar1;
  }
  fVar2 = fVar1 * 0.01;
  if (fVar1 * 0.01 > 0.2) {
    fVar2 = 0.2;
  }
  pSVar7 = this->m_pWheelCtl;
  fVar1 = fVar8 * fVar2 + fVar8;
  fVar2 = (pSVar7->m_fWheelTractionMag[0] + pSVar7->m_fWheelTractionMag[3]) * 0.5 * 0.15;
  if (fVar2 > 0.15) {
    fVar2 = 0.15;
  }
  fVar1 = fVar1 * fVar2 + fVar1;
  if (fVar1 > 6000.0) {
    fVar1 = 6000.0;
  }
  fVar2 = (pSVar7->m_bvTotalRightWheelSlip).y;
  fVar4 = (pSVar7->m_bvTotalRightWheelSlip).x;
  fVar2 = fVar4 * fVar4 + fVar2 * fVar2;
  if (fVar2 <= 5e-11) {
    fVar2 = 0.0;
  }
  else {
    fVar4 = 1.0 / SQRT(fVar2);
    fVar4 = -(fVar2 * fVar4 * fVar4 - 1.0) * fVar4 * 0.5 + fVar4;
    fVar2 = (-(fVar2 * fVar4 * fVar4 - 1.0) * fVar4 * 0.5 + fVar4) * fVar2;
  }
  fVar4 = fVar2 * 0.01;
  if (fVar2 * 0.01 > 0.2) {
    fVar4 = 0.2;
  }
  fVar8 = fVar8 * fVar4 + fVar8;
  fVar2 = (pSVar7->m_fWheelTractionMag[1] + pSVar7->m_fWheelTractionMag[2]) * 0.5 * 0.15;
  if (fVar2 > 0.15) {
    fVar2 = 0.15;
  }
  fVar8 = fVar8 * fVar2 + fVar8;
  if (fVar8 > 6000.0) {
    fVar8 = 6000.0;
  }
  this->m_nLTRoadNoiseVol = (int)fVar3;
  this->m_nRTRoadNoiseVol = (int)fVar5;
  this->m_nLTRoadNoisePitch = (int)fVar1;
  this->m_nRTRoadNoisePitch = (int)fVar8;
  return;
}



/* ================================================================== */
/* 80075c44  Update__19TrackCarCameraMoverf
   tam=992 B  insn=248  cobertura=992/992  paired-singles=6  firma=IMPORTED
   proto: undefined CameraMover::Update(TrackCarCameraMover * this, float dT)
*/

/* WARNING: Removing unreachable block (ram,0x80076018) */
/* WARNING: Removing unreachable block (ram,0x80076014) */
/* WARNING: Removing unreachable block (ram,0x80076010) */
/* WARNING: Removing unreachable block (ram,0x80075c54) */
/* WARNING: Removing unreachable block (ram,0x80075c50) */
/* WARNING: Removing unreachable block (ram,0x80075c4c) */
/* WARNING: Unknown calling convention -- yet parameter storage is locked */
/* WARNING: Enum "EVIEW_ID": Some values do not have unique names */

void CameraMover__Update(TrackCarCameraMover *this,float dT)

{
  float fVar1;
  float fVar2;
  uint uVar3;
  int iVar4;
  bool bVar6;
  ushort uVar5;
  CameraAnchor *pCVar7;
  uint uVar8;
  float fVar9;
  bVector3 local_d0;
  bVector3 local_c0;
  bVector3 local_b0;
  bVector3 local_a0;
  bVector3 bStack_90;
  float local_80;
  float local_7c;
  float local_78;
  bMatrix4 bStack_70;
  
  bVar6 = GameFlowManager__IsPaused(&TheGameFlowManager);
  if (!bVar6) {
    fVar9 = 0.0;
    (((this->__base).pCamera)->CurrentKey).SimTimeMultiplier = 0.2;
    local_d0.x = 0.0;
    local_d0.y = 0.0;
    local_d0.z = 1.0;
    pCVar7 = this->CarToFollow;
    local_b0.x = (this->Eye).x - (pCVar7->mGeomPos).x;
    local_b0.y = (this->Eye).y - (pCVar7->mGeomPos).y;
    local_b0.z = (this->Eye).z - (pCVar7->mGeomPos).z;
    __8bVector3RC8bVector3(&local_c0,&local_b0);
    fVar1 = local_c0.z * local_c0.z + local_c0.x * local_c0.x + local_c0.y * local_c0.y;
    if (fVar1 > 5e-11) {
      fVar9 = 1.0 / SQRT(fVar1);
      fVar9 = -(fVar1 * fVar9 * fVar9 - 1.0) * fVar9 * 0.5 + fVar9;
      fVar9 = (-(fVar1 * fVar9 * fVar9 - 1.0) * fVar9 * 0.5 + fVar9) * fVar1;
    }
    if (fVar9 < 1.0) {
      fVar9 = 1.0;
    }
    uVar5 = bATan(fVar9,TrackCarIsoZoomDistance[this->CameraType]);
    uVar3 = (uVar5 & 0x7fff) << 1;
    if ((uVar5 & 0x7fff) != 0) {
      uVar8 = 800;
      if (uVar3 > 800) {
        uVar8 = uVar3;
      }
      uVar5 = (ushort)uVar8;
      if (uVar8 > 0x332c) {
        uVar5 = 0x332c;
      }
      if (_6Camera_StopUpdating == 0) {
        (((this->__base).pCamera)->CurrentKey).FieldOfView = uVar5;
      }
    }
    pCVar7 = this->CarToFollow;
    fVar1 = (pCVar7->mGeomPos).x;
    fVar2 = (pCVar7->mGeomPos).y;
    (this->Look).z = (pCVar7->mGeomPos).z;
    (this->Look).x = fVar1;
    (this->Look).y = fVar2;
    fVar9 = 1.0 / fVar9;
    local_c0.z = local_c0.z * fVar9;
    local_c0.x = local_c0.x * fVar9;
    local_c0.y = local_c0.y * fVar9;
    bCross(&local_b0,&local_c0,&local_d0);
    iVar4 = this->CameraType;
    local_a0.z = TrackCarLookOffsetZ[iVar4];
    local_a0.x = TrackCarLookOffsetX[iVar4];
    local_a0.y = TrackCarLookOffsetY[iVar4];
    local_b0.x = 0.0;
    local_b0.y = 0.0;
    local_b0.z = 0.0;
    eMulVector(&local_a0,&this->CarToFollow->mGeomRot,&local_a0);
    fVar9 = (this->Look).x + local_a0.x;
    fVar1 = (this->Look).y + local_a0.y;
    fVar2 = (this->Look).z + local_a0.z;
    local_80 = fVar9 - (this->Eye).x;
    local_7c = fVar1 - (this->Eye).y;
    local_78 = fVar2 - (this->Eye).z;
    (this->Look).x = fVar9;
    (this->Look).y = fVar1;
    (this->Look).z = fVar2;
    __8bVector3RC8bVector3(&bStack_90,&local_80);
    bNormalize(&bStack_90,&bStack_90);
    eCreateLookAtMatrix(&bStack_70,&this->Eye,&this->Look,&local_d0);
    fVar9 = bDistBetween(&this->CarToFollow->mGeomPos,&this->Eye);
    if (_6Camera_StopUpdating == 0) {
      (((this->__base).pCamera)->CurrentKey).TargetDistance = fVar9;
    }
    (this->FocalDistCubic).dValDesired = (this->FocalDistCubic).Val * 6.27;
    cPoint__SplineSeek(&this->FocalDistCubic,dT,0.0,0.0);
    fVar1 = (this->FocalDistCubic).Val;
    fVar9 = fVar9 + fVar1;
    if (fVar9 < 2.0) {
      fVar9 = 2.0;
    }
    if (*(int *)&this->FocusEffects != 0) {
      if (_6Camera_StopUpdating == 0) {
        (((this->__base).pCamera)->CurrentKey).FocalDistance = fVar9 + fVar1;
      }
      if (_6Camera_StopUpdating == 0) {
        (((this->__base).pCamera)->CurrentKey).DepthOfField = 7.5;
      }
    }
    SetCameraMatrix((this->__base).pCamera,&bStack_70,dT);
    bVar6 = CameraMover__IsSomethingInBetween
                      (&this->__base,&(((this->__base).pCamera)->CurrentKey).Position,
                       &this->CarToFollow->mGeomPos);
    if (bVar6) {
      MaybeKillJumpCam(this->CarToFollow->mWorldID);
    }
  }
  return;
}



/* ================================================================== */
/* 80321474  SimThread_Step
   tam=924 B  insn=231  cobertura=924/924  paired-singles=0  firma=DEFAULT
   proto: undefined SimThread_Step()
*/

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int SimThread_Step(int *param_1)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  
  iVar9 = 0;
  iVar8 = 0;
  uVar2 = OSGetTick();
  iVar1 = param_1[1];
  uVar2 = uVar2 / (___OSBusClock / 4000);
  param_1[1] = uVar2;
  iVar1 = uVar2 - iVar1;
  iVar3 = param_1[2];
  param_1[2] = (int)*(char *)(*(int *)(*param_1 + 0xf0) + 0x47);
  param_1[3] = param_1[2] << 0xc;
  param_1[0x3a] = param_1[0x3a] + -1;
  if (param_1[0x3a] < 0) {
    param_1[0x3a] = 0xf;
  }
  iVar7 = 0;
  uVar2 = 0;
  iVar6 = 0;
  param_1[param_1[0x3a] + 0x1a] = param_1[2] - iVar3;
  param_1[param_1[0x3a] + 0x2a] = iVar1;
  iVar3 = param_1[0x3a];
  while ((iVar7 < 0x10 && (uVar2 < 0x10))) {
    iVar5 = iVar3 + 1;
    iVar6 = iVar6 + param_1[iVar3 + 0x1a];
    uVar2 = uVar2 + param_1[iVar3 + 0x2a];
    if (iVar5 == 0x10) {
      iVar5 = 0;
    }
    iVar7 = iVar7 + 1;
    iVar3 = iVar5;
  }
  if (uVar2 > 0x10) {
    iVar3 = iVar3 + -1;
    if (iVar3 < 0) {
      iVar3 = 0xf;
    }
    uVar4 = param_1[iVar3 + 0x2a];
    iVar7 = ((int)uVar4 >> 1) + (uint)((int)uVar4 < 0 && (uVar4 & 1) != 0);
    if ((int)(uVar2 - 0x10) < 0) {
      iVar7 = -iVar7;
    }
    iVar6 = iVar6 - (int)((uVar2 - 0x10) * param_1[iVar3 + 0x1a] + iVar7) / (int)uVar4;
  }
  iVar3 = param_1[0x18];
  param_1[0x18] = iVar3 + 1;
  param_1[iVar3 + 0xe] = iVar6;
  if (param_1[0x18] > 9) {
    param_1[0x18] = 0;
  }
  iVar6 = 0;
  iVar3 = 0x24;
  iVar7 = 10;
  do {
    iVar5 = param_1[0x18];
    param_1[0x18] = iVar5 + 1;
    iVar6 = iVar6 + param_1[iVar5 + 0xe] * *(int *)((int)param_1 + iVar3 + 0x10);
    if (param_1[0x18] > 9) {
      param_1[0x18] = 0;
    }
    iVar3 = iVar3 + -4;
    iVar7 = iVar7 + -1;
  } while (iVar7 != 0);
  param_1[0x19] = iVar6;
  uVar2 = 0;
  iVar3 = 0;
  do {
    iVar6 = *(int *)(*param_1 + iVar3 + 0xf4);
    if ((iVar6 != 0) && ((*(uint *)(iVar6 + 0x28) & 1) != 0)) {
      Effect_Update(iVar6,iVar1);
      if (((*(uint *)(iVar6 + 0x28) & 1) == 0) || (*(int *)(iVar6 + 0x30) != 0)) {
        iVar7 = 0;
        uVar4 = 0;
      }
      else {
        iVar5 = -0x7f;
        if (*(int *)(iVar6 + 0x34) > -0x80) {
          iVar5 = *(int *)(iVar6 + 0x34);
        }
        iVar7 = 0x7f;
        if (iVar5 < 0x80) {
          iVar7 = iVar5;
        }
        uVar4 = *(uint *)(iVar6 + 0x28) & 4;
      }
      if (uVar4 == 0) {
        iVar8 = iVar8 + iVar7;
      }
      else {
        iVar9 = iVar9 + iVar7;
      }
    }
    uVar2 = uVar2 + 1;
    iVar3 = iVar3 + 4;
  } while (uVar2 < 0x40);
  iVar3 = *(int *)(*param_1 + 0x11f4);
  iVar1 = 0x80;
  if (iVar9 < 0x81) {
    iVar1 = iVar9;
  }
  iVar9 = -0x80;
  if (iVar1 > -0x81) {
    iVar9 = iVar1;
  }
  if (iVar9 < 0) {
    uVar2 = -(uint)(byte)(&g_iGammaLookup)[-iVar9];
  }
  else {
    uVar2 = (uint)(byte)(&g_iGammaLookup)[iVar9];
  }
  if (iVar3 < 0xff) {
    iVar1 = 0x7f;
    if ((int)uVar2 < 0) {
      iVar1 = -0x7f;
    }
    iVar9 = 0x7f;
    uVar2 = (int)(uVar2 * iVar3 + iVar1) / 0xff;
    if (iVar8 < 0) {
      iVar9 = -0x7f;
    }
    iVar8 = (iVar8 * iVar3 + iVar9) / 0xff;
  }
  iVar1 = 0x80;
  if ((int)(uVar2 + iVar8) < 0x81) {
    iVar1 = uVar2 + iVar8;
  }
  iVar3 = -0x80;
  if (iVar1 > -0x81) {
    iVar3 = iVar1;
  }
  if (param_1[0x3b] != iVar3) {
    param_1[0x3b] = iVar3;
    if (iVar3 == 0) {
      SIControlSteering(**(undefined4 **)(*param_1 + 0xf0),0x400,0);
    }
    else {
      SIControlSteering(**(undefined4 **)(*param_1 + 0xf0),0x600,iVar3);
    }
  }
  return iVar3;
}



/* ================================================================== */
/* 802c9fb8  UpdateWheelYRenderOffset__13CarRenderInfo
   tam=876 B  insn=219  cobertura=876/876  paired-singles=14  firma=IMPORTED
   proto: undefined CarRenderInfo::UpdateWheelYRenderOffset(CarRenderInfo * this)
*/

/* WARNING: Removing unreachable block (ram,0x802ca314) */
/* WARNING: Removing unreachable block (ram,0x802ca310) */
/* WARNING: Removing unreachable block (ram,0x802ca30c) */
/* WARNING: Removing unreachable block (ram,0x802ca308) */
/* WARNING: Removing unreachable block (ram,0x802ca304) */
/* WARNING: Removing unreachable block (ram,0x802ca300) */
/* WARNING: Removing unreachable block (ram,0x802ca2fc) */
/* WARNING: Removing unreachable block (ram,0x802c9fdc) */
/* WARNING: Removing unreachable block (ram,0x802c9fd8) */
/* WARNING: Removing unreachable block (ram,0x802c9fd4) */
/* WARNING: Removing unreachable block (ram,0x802c9fd0) */
/* WARNING: Removing unreachable block (ram,0x802c9fcc) */
/* WARNING: Removing unreachable block (ram,0x802c9fc8) */
/* WARNING: Removing unreachable block (ram,0x802c9fc4) */
/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void CarRenderInfo__UpdateWheelYRenderOffset(CarRenderInfo *this)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  uint uVar5;
  CarPart *this_00;
  uint uVar6;
  byte *pbVar7;
  uint uVar8;
  float *pfVar9;
  void *pvVar10;
  float fVar11;
  int iVar12;
  uint uVar13;
  Private *pPVar14;
  int iVar15;
  
  if (this->pCarTypeInfo == (CarTypeInfo *)0x0) {
    bMemSet(this->WheelYRenderOffset,'\0',0x10);
  }
  else {
    if (this->pRideInfo != (RideInfo *)0x0) {
      RideInfo__GetPart(this->pRideInfo,0x42);
    }
    if (this->pRideInfo != (RideInfo *)0x0) {
      RideInfo__GetPart(this->pRideInfo,0x43);
    }
    uVar13 = 0;
    do {
      pPVar14 = *(Private **)&this->field_0x1764;
      uVar8 = ((uint)(byte)(((int)uVar13 > 1) << 2) << 0x1c) >> 0x1e;
      uVar5 = Private__GetLength(pPVar14);
      if (uVar13 < uVar5) {
        iVar12 = uVar13 * 0x10;
        pPVar14 = pPVar14 + uVar13 * 2 + 2;
      }
      else {
        iVar12 = uVar13 << 4;
        pPVar14 = (Private *)DefaultDataArea(0x10);
      }
      uVar5 = 0;
      this->WheelYRenderOffset[uVar13] = -*(float *)(pPVar14->mData + 4);
      this_00 = RideInfo__GetPart(this->pRideInfo,0x17);
      if (this_00 != (CarPart *)0x0) {
        uVar5 = CarPart__GetAppliedAttributeIParam(this_00,0x796c0cb0,0);
      }
      if (uVar8 == 0) {
        iVar15 = TweakKitWheelOffsetFront;
        if (TweakKitWheelOffsetFront == 0) {
          iVar15 = *(int *)&this->field_0x1764;
          uVar6 = Private__GetLength((Private *)(iVar15 + 0x10e));
          if (uVar5 < uVar6) {
            pbVar7 = (byte *)(iVar15 + uVar5 + 0x116);
          }
          else {
LAB_802ca16c:
            pbVar7 = (byte *)DefaultDataArea(1);
          }
LAB_802ca174:
          iVar15 = (int)*pbVar7;
        }
      }
      else {
        iVar15 = TweakKitWheelOffsetRear;
        if (TweakKitWheelOffsetRear == 0) {
          iVar15 = *(int *)&this->field_0x1764;
          uVar6 = Private__GetLength((Private *)(iVar15 + 0x100));
          if (uVar6 <= uVar5) goto LAB_802ca16c;
          pbVar7 = (byte *)(iVar15 + uVar5 + 0x108);
          goto LAB_802ca174;
        }
      }
      fVar3 = this->WheelYRenderOffset[uVar13];
      if (fVar3 > 0.0) {
        fVar3 = fVar3 + (float)iVar15 * 0.001;
      }
      else {
        fVar3 = fVar3 - (float)iVar15 * 0.001;
      }
      this->WheelYRenderOffset[uVar13] = fVar3;
      iVar15 = *(int *)&this->field_0x1764;
      fVar3 = this->WheelRadius[uVar8];
      fVar4 = this->WheelWidths[uVar8];
      uVar8 = Private__GetLength((Private *)(iVar15 + 0xd4));
      if (uVar13 < uVar8) {
        pfVar9 = (float *)(iVar15 + uVar13 * 4 + 0xdc);
      }
      else {
        pfVar9 = (float *)DefaultDataArea(4);
      }
      fVar1 = *pfVar9;
      if ((int)uVar13 > 1) {
        iVar15 = *(int *)&this->field_0x1764;
        uVar8 = Private__GetLength((Private *)(iVar15 + 0x50));
        if (uVar5 < uVar8) {
          pvVar10 = (void *)(iVar15 + uVar5 * 8 + 0x58);
        }
        else {
          pvVar10 = DefaultDataArea(8);
        }
        fVar2 = *(float *)((int)pvVar10 + 4);
      }
      else {
        iVar15 = *(int *)&this->field_0x1764;
        uVar8 = Private__GetLength((Private *)(iVar15 + 0x50));
        if (uVar5 < uVar8) {
          pfVar9 = (float *)(iVar15 + uVar5 * 8 + 0x58);
        }
        else {
          pfVar9 = (float *)DefaultDataArea(8);
        }
        fVar2 = *pfVar9;
      }
      fVar11 = *(float *)(iVar12 + *(int *)&this->field_0x1764 + 0x1c);
      if ((fVar4 <= 0.0) || (fVar1 * fVar2 <= 0.0)) {
        this->WheelWidthScales[uVar13] = 1.0;
      }
      else {
        this->WheelWidthScales[uVar13] = (fVar1 * fVar2) / fVar4;
      }
      if ((fVar3 <= 0.0) || (fVar11 <= 0.0)) {
        this->WheelRadiusScales[uVar13] = 1.0;
      }
      else {
        this->WheelRadiusScales[uVar13] = fVar11 / fVar3;
      }
      uVar13 = uVar13 + 1;
    } while ((int)uVar13 < 4);
  }
  return;
}



/* ================================================================== */
/* 8024b310  UpdateLoaded__Q217SuspensionTraffic4Tire
   tam=856 B  insn=214  cobertura=856/856  paired-singles=6  firma=IMPORTED
   proto: undefined Tire::UpdateLoaded(Tire * this, float lat_vel, float fwd_vel, float load, float dT)
*/

/* WARNING: Removing unreachable block (ram,0x8024b658) */
/* WARNING: Removing unreachable block (ram,0x8024b654) */
/* WARNING: Removing unreachable block (ram,0x8024b650) */
/* WARNING: Removing unreachable block (ram,0x8024b324) */
/* WARNING: Removing unreachable block (ram,0x8024b320) */
/* WARNING: Removing unreachable block (ram,0x8024b31c) */
/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void Tire__UpdateLoaded(Tire *this,float lat_vel,float fwd_vel,float load,float dT)

{
  float fVar1;
  float fVar2;
  float fVar3;
  
  if (this->mLoad <= 0.0) {
    this->mAV = fwd_vel / this->mRadius;
  }
  this->mSlip = fwd_vel;
  this->mAngularAcc = lat_vel;
  fVar3 = 0.0;
  if (load > 0.0) {
    fVar3 = load;
  }
  this->mLoad = fVar3;
  if (this->mBrake > 0.0) {
    fVar3 = this->mBrake *
            *(float *)((int)this->mSpecs * 4 + *(int *)(this->mWheelIndex + 8) + 8) * 1.3558 *
            BrakingTorque;
    if (this->mAV > 0.0) {
      fVar3 = -fVar3;
    }
    this->mDriveTorque = this->mDriveTorque + fVar3;
  }
  if (this->mEBrake > 0.0) {
    fVar3 = this->mEBrake *
            *(float *)(*(int *)(this->mWheelIndex + 8) + 0x10) * 1.3558 * EBrakingTorque;
    if (this->mAV > 0.0) {
      fVar3 = -fVar3;
    }
    this->mDriveTorque = this->mDriveTorque + fVar3;
  }
  fVar3 = fwd_vel;
  if (fwd_vel < 0.0) {
    fVar3 = -fwd_vel;
  }
  fVar2 = VU0_Atan2(lat_vel,fVar3);
  fVar3 = this->mAV * this->mRadius - fwd_vel;
  this->mRoadSpeed = fVar2;
  if (this->mEBrake > 0.0) {
    fVar2 = fwd_vel;
    if (fwd_vel < 0.0) {
      fVar2 = -fwd_vel;
    }
    if (fVar2 > 1.0) {
      this->mBrakeTorque = fVar3;
      goto LAB_8024b4c0;
    }
  }
  this->mBrakeTorque = 0.0;
LAB_8024b4c0:
  fVar3 = VU0_sqrt(fVar3 * fVar3 + lat_vel * lat_vel);
  if ((this->mEBrake <= 0.5) || (fVar3 <= 1.0)) {
    fVar2 = this->mDriveTorque / this->mRadius;
  }
  else {
    this->mBrakes = (brakes *)&DAT_00000001;
    fVar2 = ((-fwd_vel + -fwd_vel) * this->mLoad *
            *(float *)((int)this->mSpecs * 4 + *(int *)((int)this->mLastTorque + 8) + 0x18)) / fVar3
    ;
  }
  this->mLongitudeForce = fVar2;
  fVar2 = -lat_vel;
  fVar1 = (fVar2 + fVar2) * this->mLoad *
          *(float *)((int)this->mSpecs * 4 + *(int *)((int)this->mLastTorque + 8) + 0x18);
  this->mLateralForce = fVar1;
  if (fVar3 > 1.0) {
    this->mLateralForce = fVar1 / fVar3;
  }
  fVar3 = fwd_vel;
  if (fwd_vel < 0.0) {
    fVar3 = -fwd_vel;
  }
  if (fVar3 <= 1.0) {
    if (lat_vel < 0.0) {
      lat_vel = fVar2;
    }
    fVar3 = 1.0;
    if (lat_vel < 1.0) {
      fVar3 = lat_vel;
    }
    this->mLateralForce = this->mLateralForce * fVar3;
  }
  else {
    fVar3 = sinf(this->mRoadSpeed * 6.2831855);
    this->mLongitudeForce = this->mLongitudeForce - fVar3 * this->mLateralForce * 0.5;
  }
  this->mAV = ((1.0 - this->mEBrake) * fwd_vel) / this->mRadius;
  return;
}



/* ================================================================== */
/* 8030183c  InitAtSegment__8WRoadNavscf
   tam=816 B  insn=204  cobertura=816/816  paired-singles=6  firma=IMPORTED
   proto: undefined WRoadNav::InitAtSegment(WRoadNav * this, short segInd, char laneInd, float timeStep)
*/

/* WARNING: Removing unreachable block (ram,0x80301b60) */
/* WARNING: Removing unreachable block (ram,0x80301b5c) */
/* WARNING: Removing unreachable block (ram,0x80301b58) */
/* WARNING: Removing unreachable block (ram,0x8030184c) */
/* WARNING: Removing unreachable block (ram,0x80301848) */
/* WARNING: Removing unreachable block (ram,0x80301844) */
/* WARNING: Unknown calling convention -- yet parameter storage is locked */
/* WARNING: Enum "SplineType": Some values do not have unique names */

void WRoadNav__InitAtSegment(WRoadNav *this,short segInd,char laneInd,float timeStep)

{
  ushort uVar1;
  WRoadNetwork *this_00;
  WRoadNode *pWVar2;
  bool bVar5;
  int iVar3;
  int iVar4;
  float fVar6;
  float fVar7;
  WRoadSegment *segment;
  Vector3 local_88;
  float local_78;
  float local_74;
  float local_70;
  float local_68;
  float local_64;
  float local_60;
  float local_58;
  float local_54;
  float local_50;
  WRoadNode *apWStack_48 [3];
  uint uStack_3c;
  
  this_00 = _12WRoadNetwork_fgRoadNetwork;
  iVar4 = (int)laneInd;
  this->fDeadEnd = '\0';
  segment = _12WRoadNetwork_fSegments + segInd;
  *(undefined4 *)&this->fValid = 1;
  this->fSegmentInd = segInd;
  WRoadNetwork__GetSegmentForwardVector(this_00,(int)segInd,&local_88);
  bVar5 = WRoadNetwork__GetSegmentTrafficLaneRightSide(this_00,segment,iVar4);
  if ((bVar5) || ((segment->fFlags & 0x40) != 0)) {
    local_58 = local_88.x;
    local_54 = local_88.y;
    local_50 = local_88.z;
    local_74 = local_88.y;
    local_70 = local_88.z;
    local_78 = local_88.x;
    this->fNodeInd = '\x01';
    (this->fForwardVector).x = local_88.x;
    (this->fForwardVector).z = local_88.z;
    (this->fForwardVector).y = local_88.y;
    this->fSegTime = timeStep;
  }
  else {
    local_78 = -local_88.x;
    local_74 = -local_88.y;
    local_70 = -local_88.z;
    this->fNodeInd = (byte)segment->fFlags & 0x40;
    (this->fForwardVector).x = local_78;
    (this->fForwardVector).z = local_70;
    (this->fForwardVector).y = local_74;
    this->fSegTime = ABS(1.0 - timeStep);
    local_68 = local_78;
    local_64 = local_74;
    local_60 = local_70;
  }
  pWVar2 = _12WRoadNetwork_fNodes;
  uVar1 = *(ushort *)((int)segment->fNodeIndex + (-(uint)(this->fNodeInd == '\0') & 2));
  fVar6 = _12WRoadNetwork_fNodes[uVar1].fPosition.y;
  fVar7 = _12WRoadNetwork_fNodes[uVar1].fPosition.z;
  (this->fStartPos).x = _12WRoadNetwork_fNodes[uVar1].fPosition.x;
  (this->fStartPos).y = fVar6;
  (this->fStartPos).z = fVar7;
  uVar1 = segment->fNodeIndex[this->fNodeInd];
  fVar6 = pWVar2[uVar1].fPosition.z;
  fVar7 = pWVar2[uVar1].fPosition.y;
  (this->fEndPos).x = pWVar2[uVar1].fPosition.x;
  (this->fEndPos).z = fVar6;
  (this->fEndPos).y = fVar7;
  this->fToLaneInd = laneInd;
  this->fLaneInd = laneInd;
  this->fFromLaneOffset = 0.0;
  this->fLaneOffset = 0.0;
  this->fToLaneOffset = 0.0;
  WRoadNetwork__GetSegmentNodes(this_00,segment,apWStack_48);
  iVar3 = GetBitsSigned__C9WRoadLaneii
                    (_12WRoadNetwork_fProfiles
                     [*(short *)(*(int *)((-(uint)(this->fNodeInd == '\0') & 4) + (int)apWStack_48)
                                + 0xe)].mLanes + iVar4,0x12,0xe);
  uStack_3c = iVar3 ^ 0x80000000;
  apWStack_48[2] = (WRoadNode *)0x43300000;
  fVar6 = (float)iVar3 * 0.012208521;
  iVar4 = GetBitsSigned__C9WRoadLaneii
                    (_12WRoadNetwork_fProfiles[apWStack_48[this->fNodeInd]->fProfileIndex].mLanes +
                     iVar4,0x12,0xe);
  uStack_3c = iVar4 ^ 0x80000000;
  apWStack_48[2] = (WRoadNode *)0x43300000;
  fVar7 = ((float)iVar4 * 0.012208521 - fVar6) * this->fSegTime + fVar6;
  this->fFromLaneOffset = fVar7;
  this->fLaneOffset = fVar7;
  this->fToLaneOffset = fVar7;
  WRoadNav__SetStartEndPos(this,segment,fVar6,(float)iVar4 * 0.012208521);
  WRoadNav__SetStartEndControls(this,segment);
  WRoadNav__RebuildSplines(this,segment);
  WRoadNav__EvaluateSplines(this,segment);
  WRoadNav__ResetCookieTrail(this);
  return;
}



/* ================================================================== */
/* 803598fc  VP6_PredictFilteredBlock
   tam=740 B  insn=185  cobertura=740/740  paired-singles=0  firma=IMPORTED
   proto: undefined VP6_PredictFilteredBlock(PB_INSTANCE * pbi, short * OutputPtr, anon_79933716 bp)
*/

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void VP6_PredictFilteredBlock(PB_INSTANCE *pbi,short *OutputPtr,anon_79933716 bp)

{
  uint uVar1;
  uint uVar2;
  int iVar3;
  uint uVar4;
  code *pcVar5;
  int unaff_r13;
  uint uVar6;
  int SourceStride;
  uchar *puVar7;
  int iVar8;
  uint uVar9;
  uint uVar10;
  bool bVar11;
  
  uVar10 = (pbi->mbi).MvShift;
  uVar2 = (pbi->mbi).MvModMask;
  puVar7 = pbi->LastFrameRecon;
  if (VP6_Mode2Frame[(pbi->mbi).Mode] == 2) {
    puVar7 = pbi->GoldenFrame;
  }
  if ((pbi->VpProfile == '\0') || (pbi->UseLoopFilter == '\0')) {
    uVar1 = (uint)(pbi->mbi).Mv[bp].y;
    uVar4 = (uint)(pbi->mbi).Mv[bp].x;
    uVar6 = uVar1 & uVar2;
    uVar9 = uVar4 & uVar2;
    SourceStride = (pbi->mbi).CurrentReconStride;
    iVar8 = 0;
    puVar7 = puVar7 + (pbi->mbi).FrameReconStride *
                      ((int)(uVar1 + (uVar2 & (int)uVar1 >> 0x1f)) >> (uVar10 & 0x3f)) +
                      ((int)(uVar4 + (uVar2 & (int)uVar4 >> 0x1f)) >> (uVar10 & 0x3f)) +
                      (pbi->mbi).Recon;
    iVar3 = 0;
  }
  else {
    SourceStride = 0x10;
    iVar8 = 0x22;
    VP6_PredictFiltered(pbi,puVar7 + (pbi->mbi).Recon,(int)(pbi->mbi).Mv[bp].x,
                        (int)(pbi->mbi).Mv[bp].y);
    uVar9 = (pbi->mbi).MvModMask;
    puVar7 = pbi->LoopFilteredBlock;
    iVar3 = 0x22;
    uVar6 = (int)(pbi->mbi).Mv[bp].y & uVar9;
    uVar9 = (int)(pbi->mbi).Mv[bp].x & uVar9;
  }
  if (uVar9 != 0) {
    if ((pbi->mbi).Mv[bp].x < 1) {
      iVar8 = iVar8 + -1;
    }
    else {
      iVar8 = iVar8 + 1;
    }
  }
  if (uVar6 != 0) {
    if ((pbi->mbi).Mv[bp].y < 1) {
      iVar8 = iVar8 - SourceStride;
    }
    else {
      iVar8 = iVar8 + SourceStride;
    }
  }
  if (iVar3 == iVar8) {
    (**(code **)(unaff_r13 + -0x70ac))(puVar7 + iVar3,OutputPtr,SourceStride);
    return;
  }
  if (bp < U_BLOCK) {
    uVar9 = uVar9 << 1;
    uVar6 = uVar6 << 1;
    if (pbi->VpProfile != '\0') {
      if (pbi->PredictionFilterMode != '\x02') {
        pcVar5 = *(code **)(unaff_r13 + -0x7098);
        bVar11 = pbi->PredictionFilterMode == '\x01';
        goto LAB_80359ba8;
      }
      if ((pbi->PredictionFilterMvSizeThresh == 0) ||
         (uVar10 = 4 << (pbi->PredictionFilterMvSizeThresh - 1 & 0x3f),
         uVar2 = (uint)(pbi->mbi).Mv[bp].x, uVar1 = (int)uVar2 >> 0x1f,
         (uVar1 ^ uVar2) - uVar1 <= uVar10 &&
         (uVar2 = (uint)(pbi->mbi).Mv[bp].y, uVar1 = (int)uVar2 >> 0x1f,
         (uVar1 ^ uVar2) - uVar1 <= uVar10))) {
        if (pbi->PredictionFilterVarThresh == 0) {
          pcVar5 = *(code **)(unaff_r13 + -0x7098);
          bVar11 = true;
        }
        else {
          uVar2 = Var16Point(puVar7 + iVar3,SourceStride);
          pcVar5 = *(code **)(unaff_r13 + -0x7098);
          bVar11 = pbi->PredictionFilterVarThresh <= uVar2;
        }
        goto LAB_80359ba8;
      }
    }
  }
  pcVar5 = *(code **)(unaff_r13 + -0x7098);
  bVar11 = false;
LAB_80359ba8:
  (*pcVar5)(puVar7 + iVar3,puVar7 + iVar8,OutputPtr,SourceStride,uVar9,uVar6,bVar11);
  return;
}



/* ================================================================== */
/* 802bc0a4  GetLoadingPriority__13TrackStreamer
   tam=708 B  insn=177  cobertura=708/708  paired-singles=6  firma=IMPORTED
   proto: int TrackStreamer::GetLoadingPriority(TrackStreamer * this, TrackStreamingSection * section, StreamingPositionEntry * position_entry, bool calculating_jettison)
*/

/* WARNING: Removing unreachable block (ram,0x802bc358) */
/* WARNING: Removing unreachable block (ram,0x802bc354) */
/* WARNING: Removing unreachable block (ram,0x802bc350) */
/* WARNING: Removing unreachable block (ram,0x802bc0b8) */
/* WARNING: Removing unreachable block (ram,0x802bc0b4) */
/* WARNING: Removing unreachable block (ram,0x802bc0b0) */
/* WARNING: Unknown calling convention -- yet parameter storage is locked */

int TrackStreamer__GetLoadingPriority
              (TrackStreamer *this,TrackStreamingSection *section,
              StreamingPositionEntry *position_entry,bool calculating_jettison)

{
  int iVar1;
  float fVar2;
  float fVar3;
  ushort uVar5;
  int iVar4;
  byte in_cr0;
  byte in_cr1;
  byte unaff_cr2;
  byte unaff_cr3;
  byte unaff_cr4;
  byte in_cr5;
  byte in_cr6;
  byte in_cr7;
  float fVar6;
  bVector2 local_68;
  bVector2 local_60;
  bVector2 local_58;
  bVector2 local_50;
  float local_48;
  float local_44;
  uint local_2c;
  
  local_2c = (uint)(in_cr0 & 0xf) << 0x1c | (uint)(in_cr1 & 0xf) << 0x18 |
             (uint)(unaff_cr2 & 0xf) << 0x14 | (uint)(unaff_cr3 & 0xf) << 0x10 |
             (uint)(unaff_cr4 & 0xf) << 0xc | (uint)(in_cr5 & 0xf) << 8 | (uint)(in_cr6 & 0xf) << 4
             | (uint)(in_cr7 & 0xf);
  if (section->pBoundary != (VisibleSectionBoundary *)0x0) {
    local_68.y = (position_entry->Velocity).y;
    local_68.x = (position_entry->Velocity).x;
    fVar2 = local_68.x * local_68.x + local_68.y * local_68.y;
    if (fVar2 <= 5e-11) {
      fVar2 = 0.0;
    }
    else {
      fVar6 = 1.0 / SQRT(fVar2);
      fVar6 = -(fVar2 * fVar6 * fVar6 - 1.0) * fVar6 * 0.5 + fVar6;
      fVar2 = (-(fVar2 * fVar6 * fVar6 - 1.0) * fVar6 * 0.5 + fVar6) * fVar2;
    }
    if (calculating_jettison) {
      fVar2 = 100.0;
    }
    if (fVar2 >= 1.0) {
      local_68.x = (position_entry->Position).x + local_68.x;
      local_68.y = (position_entry->Position).y + local_68.y;
      fVar6 = VisibleSectionBoundary__GetDistanceOutside(section->pBoundary,&local_68,999.0);
      if (!calculating_jettison) {
        bNormalize(&local_60,&position_entry->Velocity);
      }
      else {
        bNormalize(&local_60,&position_entry->Direction);
      }
      local_58.x = (section->Centre).x - local_68.x;
      local_58.y = (section->Centre).y - local_68.y;
      bNormalize(&local_50,&local_58);
      local_48 = local_50.x;
      local_44 = local_50.y;
      local_58.x = local_50.x;
      fVar3 = 1.0;
      if (fVar2 * 0.016666668 - 1.0 < 0.0) {
        fVar3 = fVar2 * 0.016666668;
      }
      local_58.y = local_50.y;
      uVar5 = bASin(local_50.x * local_60.x + local_50.y * local_60.y);
      fVar2 = (float)(0x4000 - uVar5 & 0xffff) * 0.005493164;
      if (fVar2 - 20.0 < 0.0) {
        fVar2 = 20.0;
      }
      if (90.0 - fVar2 < 0.0) {
        fVar2 = 90.0;
      }
      iVar1 = (int)(fVar6 * (1.0 - (90.0 - fVar2) * 0.014285714 * fVar3 * 0.66999996) * 0.013333334)
      ;
      iVar4 = 0;
      if (iVar1 > 0) {
        iVar4 = iVar1;
      }
      if (iVar4 < 3) {
        return iVar4;
      }
      return 2;
    }
  }
  return 0;
}



/* ================================================================== */
/* 802dfcac  DefragmentPool__9CarLoader
   tam=684 B  insn=171  cobertura=684/684  paired-singles=0  firma=IMPORTED
   proto: int CarLoader::DefragmentPool(CarLoader * this)
*/

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

int CarLoader__DefragmentPool(CarLoader *this)

{
  undefined4 *puVar1;
  int iVar2;
  int iVar3;
  char *pcVar4;
  bool bVar5;
  void *ptr;
  int iVar6;
  int iVar7;
  void *ptr_00;
  void *pvVar8;
  int movement_offset;
  void *local_12c8 [1152];
  undefined4 local_c8 [33];
  
  if (this->MayNeedDefragmentation == 0) {
    iVar2 = 0;
  }
  else {
    bGetTicker();
    iVar7 = 0;
    iVar2 = bMemoryGetAllocations(CarLoaderMemoryPoolNumber,local_12c8,0x480);
    bMemSet(&DefragmentParams,'\0',0xd0);
    DefragmentParams.LargestAllocationSize = 0;
    if (iVar2 > 0) {
      do {
        pvVar8 = local_12c8[iVar7];
        iVar3 = bGetMallocSize(pvVar8);
        if (iVar3 < DefragmentParams.LargestAllocationSize) {
          iVar3 = DefragmentParams.LargestAllocationSize;
        }
        DefragmentParams.LargestAllocationSize = iVar3;
        pcVar4 = bGetMallocName(pvVar8);
        iVar7 = iVar7 + 1;
        bSafeStrCpy(DefragmentParams.LargestAllocationName,pcVar4,0x40);
      } while (iVar7 < iVar2);
    }
    bVar5 = CarLoader__AllocateDefragmentStorage(this);
    if (bVar5) {
      eWaitUntilRenderingDone();
      iVar3 = 0;
      gDefragFixer.NumRanges = 0;
      gDefragFixer.MemLow = 0;
      gDefragFixer.MemHigh = 0;
      iVar7 = 0;
      pvVar8 = bMalloc(0x80,CarLoaderMemoryPoolNumber & 0xfU | 0x2000);
      bFree(pvVar8);
      if (iVar2 > 0) {
        do {
          iVar6 = iVar7 + 1;
          ptr_00 = local_12c8[iVar7];
          movement_offset = 0;
          iVar7 = bGetMallocSize(ptr_00);
          if ((int)pvVar8 < (int)ptr_00) {
            DefragmentParams.pAllocation = ptr_00;
            pcVar4 = bGetMallocName(ptr_00);
            bStrNCpy(DefragmentParams.AllocationName,pcVar4,0x3f);
            while (ptr = bMalloc(1,CarLoaderMemoryPoolNumber & 0xfU | 0x2000),
                  (int)ptr < (int)pvVar8 + -0x80) {
              local_c8[iVar3] = ptr;
              iVar3 = iVar3 + 1;
            }
            bFree(ptr);
            movement_offset = (int)ptr - (int)DefragmentParams.pAllocation;
            ChunkMovementOffset = movement_offset;
            DefragmentParams.pNewAllocation = ptr;
            bVar5 = CarLoader__DefragmentAllocation(this,ptr_00);
            if (!bVar5) {
              movement_offset = 0;
            }
            ChunkMovementOffset = 0;
          }
          DefragFixer__Add(&gDefragFixer,ptr_00,iVar7,movement_offset);
          loop_number_35434 = loop_number_35434 + 1;
          iVar7 = iVar6;
        } while (iVar6 < iVar2);
      }
      iVar2 = 0;
      if (iVar3 > 0) {
        do {
          puVar1 = local_c8 + iVar2;
          iVar2 = iVar2 + 1;
          bFree((void *)*puVar1);
        } while (iVar2 < iVar3);
      }
      CarLoader__FreeDefragmentStorage(this);
      bMemSet(&DefragmentParams,'\0',0xd0);
      eFixupReplacementTextureTables();
      RefreshAllRenderInfo(CARTYPE_NONE);
      gDefragFixer.MemLow = 0;
      gDefragFixer.NumRanges = 0;
      iVar2 = 1;
      gDefragFixer.MemHigh = 0;
      this->MayNeedDefragmentation = 0;
    }
    else {
      CarLoader__FreeDefragmentStorage(this);
      iVar2 = 0;
    }
  }
  return iVar2;
}



/* ================================================================== */
/* 8034bac4  IdctColumn
   tam=632 B  insn=158  cobertura=632/632  paired-singles=0  firma=IMPORTED
   proto: undefined IdctColumn(int * src, int * dest)
*/

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void IdctColumn(int *src,int *dest)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  int iVar7;
  uint uVar8;
  int iVar9;
  int iVar10;
  uint uVar11;
  uint uVar12;
  int iVar13;
  uint uVar14;
  uint uVar15;
  uint uVar16;
  int iVar17;
  uint uVar18;
  
  iVar1 = src[2];
  iVar4 = src[4];
  iVar17 = src[1];
  iVar3 = src[3];
  iVar5 = src[6];
  iVar10 = src[5];
  iVar7 = src[7];
  if ((((((iVar17 == 0 && iVar1 == 0) && iVar3 == 0) && iVar4 == 0) && iVar10 == 0) && iVar5 == 0)
      && iVar7 == 0) {
    *dest = *src;
    dest[8] = *src;
    dest[0x10] = *src;
    dest[0x18] = *src;
    dest[0x20] = *src;
    dest[0x28] = *src;
    dest[0x30] = *src;
    dest[0x38] = *src;
  }
  else {
    iVar9 = iVar17 - iVar7;
    iVar2 = iVar10 - iVar3;
    uVar12 = (iVar2 + iVar9) * 0x61f8;
    uVar6 = (iVar1 - iVar5) * 0xb505;
    iVar13 = (iVar17 + iVar7) - (iVar10 + iVar3);
    uVar11 = iVar13 * 0xb505;
    uVar16 = uVar6 + 0x8000;
    uVar15 = uVar12 + 0x8000;
    uVar8 = iVar9 * 0x14e7b + 0x8000;
    uVar18 = iVar2 * 0x8a8c + 0x8000;
    uVar14 = uVar11 + 0x8000;
    uVar16 = ((int)((ulonglong)((longlong)(iVar1 - iVar5) * 0xb505) >> 0x20) +
             (uint)(uVar16 < uVar6)) * 0x10000 | uVar16 >> 0x10;
    uVar11 = ((int)((ulonglong)((longlong)iVar13 * 0xb505) >> 0x20) + (uint)(uVar14 < uVar11)) *
             0x10000 | uVar14 >> 0x10;
    uVar6 = ((int)((ulonglong)((longlong)(iVar2 + iVar9) * 0x61f8) >> 0x20) +
            (uint)(uVar15 < uVar12)) * 0x10000 | uVar15 >> 0x10;
    iVar2 = uVar6 + (((int)((ulonglong)((longlong)iVar2 * 0x8a8c) >> 0x20) +
                     (uint)(uVar18 < (uint)(iVar2 * 0x8a8c))) * 0x10000 | uVar18 >> 0x10);
    iVar9 = (((int)((ulonglong)((longlong)iVar9 * 0x14e7b) >> 0x20) +
             (uint)(uVar8 < (uint)(iVar9 * 0x14e7b))) * 0x10000 | uVar8 >> 0x10) - uVar6;
    iVar5 = iVar1 + iVar5 + uVar16;
    iVar1 = *src + iVar4;
    iVar13 = iVar1 - iVar5;
    iVar1 = iVar1 + iVar5;
    iVar5 = iVar10 + iVar3 + iVar17 + iVar7 + iVar9;
    iVar4 = *src - iVar4;
    iVar9 = iVar9 + uVar11;
    iVar3 = iVar4 - uVar16;
    iVar7 = uVar11 + iVar2;
    iVar4 = iVar4 + uVar16;
    dest[0x38] = iVar1 - iVar5;
    *dest = iVar1 + iVar5;
    dest[8] = iVar4 + iVar9;
    dest[0x10] = iVar3 + iVar7;
    dest[0x18] = iVar13 + iVar2;
    dest[0x20] = iVar13 - iVar2;
    dest[0x28] = iVar3 - iVar7;
    dest[0x30] = iVar4 - iVar9;
  }
  return;
}



/* ================================================================== */
/* 802a2248  Setup__Q26Speech13RoadblockFlow
   tam=596 B  insn=149  cobertura=596/596  paired-singles=0  firma=IMPORTED
   proto: undefined RoadblockFlow::Setup(RoadblockFlow * this)
*/

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void RoadblockFlow__Setup(RoadblockFlow *this)

{
  bool bVar1;
  SoundAI *this_00;
  EAXCop *pEVar2;
  EAXCop *pEVar3;
  int iVar4;
  IRoadBlock *pIVar5;
  bool bVar6;
  __vtbl_ptr_type *p_Var7;
  float fVar8;
  
  this_00 = _Q33UTL11Collectionst9Singleton1Z7SoundAI_mInstance;
  if (((this->mFlags ^ 1) & 1) == 0) {
    if ((this->mFlags & 0x100) == 0) {
      bVar1 = false;
      bVar6 = Manager__IsCopSpeechPlaying(kSPCH1_EventID_DispRBReply);
      if ((bVar6) || (bVar6 = Manager__IsCopSpeechPlaying(kSPCH1_EventID_DispRBUpdate), bVar6)) {
        bVar1 = true;
      }
      fVar8 = bRandom(1.0);
      if ((fVar8 <= 0.5) || (bVar1)) {
        MiscSpeech__RBWarning();
      }
      else {
        pEVar2 = SoundAI__GetRandomActiveCop(this_00,1,false);
        if (pEVar2 == (EAXCop *)0x0) {
          return;
        }
        EAXDispatch__RBUpdate(this_00->mDispatch,pEVar2,'\x01');
        p_Var7 = (pEVar2->__base).__base._vptr_AudioMemBase;
        (**(code **)&p_Var7[0x5c].__delta2)
                  ((int)&(pEVar2->__base).__base._vptr_AudioMemBase + (int)p_Var7[0x5c].__delta);
      }
    }
    else {
      bVar1 = true;
      pIVar5 = SoundAI__GetRoadblock(_Q33UTL11Collectionst9Singleton1Z7SoundAI_mInstance);
      if (pIVar5 != (IRoadBlock *)0x0) {
        pIVar5 = SoundAI__GetRoadblock(this_00);
        iVar4 = (**(code **)(*(int *)&pIVar5->field_0x4 + 0x9c))
                          (&pIVar5->__base + *(short *)(*(int *)&pIVar5->field_0x4 + 0x98));
        bVar1 = iVar4 < 2;
      }
      if (bVar1) {
        MiscSpeech__RBPosition(this->mSpikeOffset);
      }
      else {
        pEVar2 = SoundAI__GetCopInRB(this_00);
        if ((pEVar2 == (EAXCop *)0x0) &&
           (pEVar2 = SoundAI__GetRandomCop(this_00,2), pEVar2 == (EAXCop *)0x0)) {
          MiscSpeech__RBWarning();
        }
        else {
          p_Var7 = (pEVar2->__base).__base._vptr_AudioMemBase;
          (**(code **)&p_Var7[0x5a].__delta2)
                    ((int)&(pEVar2->__base).__base._vptr_AudioMemBase + (int)p_Var7[0x5a].__delta);
        }
      }
    }
  }
  else {
    pEVar2 = SoundAI__GetRandomCop(_Q33UTL11Collectionst9Singleton1Z7SoundAI_mInstance,1);
    pEVar3 = SoundAI__GetRandomCop(this_00,2);
    if ((this->mNumBlocks < 2) || (pEVar3 == (EAXCop *)0x0)) {
      EAXDispatch__RBUpdate(this_00->mDispatch,pEVar2,'\0');
      p_Var7 = (pEVar2->__base).__base._vptr_AudioMemBase;
      iVar4 = (**(code **)&p_Var7[0x69].__delta2)
                        ((int)&(pEVar2->__base).__base._vptr_AudioMemBase +
                         (int)p_Var7[0x69].__delta);
      if (iVar4 == 0) {
        p_Var7 = (pEVar2->__base).__base._vptr_AudioMemBase;
        (**(code **)&p_Var7[0x59].__delta2)
                  ((int)&(pEVar2->__base).__base._vptr_AudioMemBase + (int)p_Var7[0x59].__delta);
      }
    }
    else {
      fVar8 = bRandom(1.0);
      if (fVar8 <= 0.5) {
        EAXDispatch__SubRBReply(this_00->mDispatch);
      }
      else {
        p_Var7 = (pEVar3->__base).__base._vptr_AudioMemBase;
        (**(code **)&p_Var7[0x5e].__delta2)
                  ((int)&(pEVar3->__base).__base._vptr_AudioMemBase + (int)p_Var7[0x5e].__delta);
      }
    }
  }
  this->mFlags = this->mFlags & 0xefffffff;
  return;
}



/* ================================================================== */
/* 8031f7fc  HandleTriggers
   tam=588 B  insn=147  cobertura=588/588  paired-singles=0  firma=DEFAULT
   proto: undefined HandleTriggers()
*/

void HandleTriggers(int param_1,ushort *param_2,int param_3)

{
  byte bVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  
  uVar4 = (uint)*(byte *)(param_3 + 0x1b);
  uVar6 = (uint)*(byte *)(param_3 + 0x11);
  bVar1 = *(byte *)(param_2 + 3);
  uVar5 = (uint)bVar1;
  if ((int)(uVar6 + uVar4 * -2) < (int)uVar5) {
    if (((*param_2 ^ *(ushort *)(param_3 + 0x1e)) & 0x40) == 0) {
      if ((*param_2 & 0x40) == 0) {
        uVar3 = (uint)*(byte *)(param_3 + 7);
        if (uVar5 < *(byte *)(param_3 + 7)) {
          *(byte *)(param_3 + 7) = bVar1;
          uVar3 = uVar5;
        }
        if (uVar6 < uVar5) {
          *(byte *)(param_3 + 0x11) = bVar1;
          uVar6 = uVar5;
        }
        iVar2 = (uVar6 - uVar3) + uVar4 * -2;
        if (iVar2 == 0) {
          iVar2 = 1;
        }
        iVar2 = (int)((uVar5 - (uVar3 + uVar4)) * 0xff) / iVar2;
        if (iVar2 < 0) {
          iVar2 = 0;
        }
        if (iVar2 > 0xff) {
          iVar2 = 0xff;
        }
        *(char *)(param_1 + 7) = (char)iVar2;
      }
      else {
        *(undefined1 *)(param_1 + 7) = 0xff;
      }
    }
    else {
      *(byte *)(param_3 + 0x11) = (byte)(uVar5 - uVar4) & ~(byte)((int)(uVar5 - uVar4) >> 0x1f);
      *(undefined1 *)(param_1 + 7) = 0xff;
    }
  }
  else {
    uVar3 = (uint)*(byte *)(param_3 + 7);
    if (uVar5 < *(byte *)(param_3 + 7)) {
      *(byte *)(param_3 + 7) = bVar1;
      uVar3 = uVar5;
    }
    if (uVar6 < uVar5) {
      *(byte *)(param_3 + 0x11) = bVar1;
      uVar6 = uVar5;
    }
    iVar2 = (uVar6 - uVar3) + uVar4 * -2;
    if (iVar2 == 0) {
      iVar2 = 1;
    }
    iVar2 = (int)((uVar5 - (uVar3 + uVar4)) * 0xff) / iVar2;
    if (iVar2 < 0) {
      iVar2 = 0;
    }
    if (iVar2 > 0xff) {
      iVar2 = 0xff;
    }
    *(char *)(param_1 + 7) = (char)iVar2;
  }
  uVar4 = (uint)*(byte *)(param_3 + 0x1c);
  uVar6 = (uint)*(byte *)(param_3 + 0x12);
  bVar1 = *(byte *)((int)param_2 + 7);
  uVar5 = (uint)bVar1;
  if ((int)uVar5 <= (int)(uVar6 + uVar4 * -2)) {
    uVar3 = (uint)*(byte *)(param_3 + 8);
    if (uVar5 < *(byte *)(param_3 + 8)) {
      *(byte *)(param_3 + 8) = bVar1;
      uVar3 = uVar5;
    }
    if (uVar6 < uVar5) {
      *(byte *)(param_3 + 0x12) = bVar1;
      uVar6 = uVar5;
    }
    iVar2 = (uVar6 - uVar3) + uVar4 * -2;
    if (iVar2 == 0) {
      iVar2 = 1;
    }
    iVar2 = (int)((uVar5 - (uVar3 + uVar4)) * 0xff) / iVar2;
    if (iVar2 < 0) {
      iVar2 = 0;
    }
    if (iVar2 > 0xff) {
      iVar2 = 0xff;
    }
    *(char *)(param_1 + 8) = (char)iVar2;
    return;
  }
  if (((*param_2 ^ *(ushort *)(param_3 + 0x1e)) & 0x20) != 0) {
    *(byte *)(param_3 + 0x12) = (byte)(uVar5 - uVar4) & ~(byte)((int)(uVar5 - uVar4) >> 0x1f);
    *(undefined1 *)(param_1 + 8) = 0xff;
    return;
  }
  if ((*param_2 & 0x20) != 0) {
    *(undefined1 *)(param_1 + 8) = 0xff;
    return;
  }
  uVar3 = (uint)*(byte *)(param_3 + 8);
  if (uVar5 < *(byte *)(param_3 + 8)) {
    *(byte *)(param_3 + 8) = bVar1;
    uVar3 = uVar5;
  }
  if (uVar6 < uVar5) {
    *(byte *)(param_3 + 0x12) = bVar1;
    uVar6 = uVar5;
  }
  iVar2 = (uVar6 - uVar3) + uVar4 * -2;
  if (iVar2 == 0) {
    iVar2 = 1;
  }
  iVar2 = (int)((uVar5 - (uVar3 + uVar4)) * 0xff) / iVar2;
  if (iVar2 < 0) {
    iVar2 = 0;
  }
  if (iVar2 > 0xff) {
    iVar2 = 0xff;
  }
  *(char *)(param_1 + 8) = (char)iVar2;
  return;
}



/* ================================================================== */
/* 8034bd3c  IdctRow
   tam=516 B  insn=129  cobertura=516/516  paired-singles=0  firma=IMPORTED
   proto: undefined IdctRow(int * src, int * dest)
*/

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void IdctRow(int *src,int *dest)

{
  uint uVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  uint uVar7;
  int iVar8;
  uint uVar9;
  uint uVar10;
  uint uVar11;
  int iVar12;
  int iVar13;
  uint uVar14;
  int iVar15;
  int iVar16;
  int iVar17;
  
  iVar5 = src[1] - src[7];
  iVar12 = src[5] - src[3];
  iVar2 = src[2] - src[6];
  uVar7 = (iVar12 + iVar5) * 0x61f8;
  iVar16 = src[1] + src[7];
  iVar4 = src[5] + src[3];
  uVar1 = iVar2 * 0xb505;
  iVar8 = iVar16 - iVar4;
  uVar6 = iVar8 * 0xb505;
  uVar11 = uVar1 + 0x8000;
  uVar10 = uVar7 + 0x8000;
  uVar3 = iVar5 * 0x14e7b + 0x8000;
  uVar9 = uVar6 + 0x8000;
  uVar14 = iVar12 * 0x8a8c + 0x8000;
  uVar1 = ((int)((ulonglong)((longlong)iVar2 * 0xb505) >> 0x20) + (uint)(uVar11 < uVar1)) * 0x10000
          | uVar11 >> 0x10;
  uVar6 = ((int)((ulonglong)((longlong)iVar8 * 0xb505) >> 0x20) + (uint)(uVar9 < uVar6)) * 0x10000 |
          uVar9 >> 0x10;
  uVar7 = ((int)((ulonglong)((longlong)(iVar12 + iVar5) * 0x61f8) >> 0x20) + (uint)(uVar10 < uVar7))
          * 0x10000 | uVar10 >> 0x10;
  iVar13 = uVar7 + (((int)((ulonglong)((longlong)iVar12 * 0x8a8c) >> 0x20) +
                    (uint)(uVar14 < (uint)(iVar12 * 0x8a8c))) * 0x10000 | uVar14 >> 0x10);
  iVar8 = (((int)((ulonglong)((longlong)iVar5 * 0x14e7b) >> 0x20) +
           (uint)(uVar3 < (uint)(iVar5 * 0x14e7b))) * 0x10000 | uVar3 >> 0x10) - uVar7;
  iVar5 = src[2] + src[6] + uVar1;
  iVar2 = *src + src[4];
  iVar17 = iVar2 - iVar5;
  iVar15 = *src - src[4];
  iVar2 = iVar2 + iVar5;
  iVar4 = iVar4 + iVar16 + iVar8;
  iVar5 = iVar15 - uVar1;
  iVar8 = iVar8 + uVar6;
  iVar15 = iVar15 + uVar1;
  iVar12 = uVar6 + iVar13;
  dest[7] = iVar2 - iVar4;
  *dest = iVar2 + iVar4;
  dest[1] = iVar15 + iVar8;
  dest[2] = iVar5 + iVar12;
  dest[3] = iVar17 + iVar13;
  dest[4] = iVar17 - iVar13;
  dest[5] = iVar5 - iVar12;
  dest[6] = iVar15 - iVar8;
  return;
}



/* ================================================================== */
/* 800a3090  EvalState__Q29EAGL4Anim14FnRawStateChanf
   tam=456 B  insn=114  cobertura=456/456  paired-singles=0  firma=IMPORTED
   proto: bool FnAnim::EvalState(FnRawStateChan * this, float time, State * s)
*/

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

bool FnAnim__EvalState(FnRawStateChan *this,float time,State *s)

{
  byte bVar1;
  byte bVar2;
  uint uVar3;
  AnimTypeId *pAVar4;
  ushort *puVar5;
  int iVar6;
  AnimMemoryMap *pAVar7;
  
  pAVar7 = (this->__base).mpAnim;
  bVar1 = *(byte *)&pAVar7[2].mAnimTypeId.mId;
  if ((bVar1 & 1) == 0) {
    pAVar4 = &pAVar7[3].mAnimTypeId;
  }
  else {
    pAVar4 = (AnimTypeId *)&pAVar7[2].mTargetCheckSum;
  }
  bVar2 = (byte)pAVar7[2].mAnimTypeId.mId;
  iVar6 = this->mKeyIdx;
  uVar3 = (uint)bVar2;
  if (time < *(float *)((int)&pAVar4[bVar1].mId + iVar6 * uVar3)) {
    iVar6 = iVar6 + -1;
    bVar1 = *(byte *)&pAVar7[2].mAnimTypeId.mId;
    if (iVar6 > -1) {
      do {
        puVar5 = &pAVar7[2].mTargetCheckSum + bVar1 + 1;
        if ((bVar1 & 1) != 0) {
          puVar5 = &pAVar7[2].mTargetCheckSum + bVar1;
        }
        puVar5 = (ushort *)((int)puVar5 + iVar6 * uVar3);
        if ((float)puVar5 <= time) goto LAB_800a320c;
        iVar6 = iVar6 + -1;
      } while (iVar6 > -1);
    }
    if ((bVar1 & 1) == 0) {
      pAVar4 = &pAVar7[3].mAnimTypeId;
    }
    else {
      pAVar4 = (AnimTypeId *)&pAVar7[2].mTargetCheckSum;
    }
    FnRawStateChan__Decode(this,(uchar *)(pAVar4 + bVar1 + 2),(uchar *)s);
    this->mKeyIdx = 0;
  }
  else {
    bVar1 = *(byte *)&pAVar7[2].mAnimTypeId.mId;
    for (; iVar6 < (int)(uint)pAVar7[1].mTargetCheckSum; iVar6 = iVar6 + 1) {
      puVar5 = &pAVar7[2].mTargetCheckSum + bVar1 + 1;
      if ((bVar1 & 1) != 0) {
        puVar5 = &pAVar7[2].mTargetCheckSum + bVar1;
      }
      puVar5 = (ushort *)((int)puVar5 + iVar6 * uVar3);
      if (time < *(float *)((int)puVar5 + (uint)(byte)pAVar7[2].mAnimTypeId.mId)) {
LAB_800a320c:
        FnRawStateChan__Decode(this,(uchar *)(puVar5 + 2),(uchar *)s);
        this->mKeyIdx = iVar6;
        return true;
      }
    }
    if ((bVar1 & 1) == 0) {
      pAVar4 = &pAVar7[3].mAnimTypeId;
    }
    else {
      pAVar4 = (AnimTypeId *)&pAVar7[2].mTargetCheckSum;
    }
    FnRawStateChan__Decode
              (this,(uchar *)((int)(pAVar4 + bVar1 + 2) +
                             (uint)bVar2 * (pAVar7[1].mTargetCheckSum - 1)),(uchar *)s);
    this->mKeyIdx = pAVar7[1].mTargetCheckSum - 1;
  }
  return true;
}



/* ================================================================== */
/* 80321330  SimThread_Init
   tam=324 B  insn=81  cobertura=324/324  paired-singles=0  firma=DEFAULT
   proto: undefined SimThread_Init()
*/

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void SimThread_Init(undefined4 *param_1,undefined4 param_2)

{
  uint uVar1;
  int iVar2;
  int unaff_r13;
  int iVar3;
  undefined4 local_18 [5];
  
  local_18[0] = *(undefined4 *)(unaff_r13 + -0x65a0);
  param_1[1] = 0;
  param_1[0x18] = 0;
  param_1[0x3a] = 0;
  iVar3 = 0x10;
  iVar2 = 0;
  do {
    *(undefined4 *)((int)param_1 + iVar2 + 0x68) = 0;
    *(undefined4 *)((int)param_1 + iVar2 + 0xa8) = 0;
    iVar3 = iVar3 + -1;
    iVar2 = iVar2 + 4;
  } while (iVar3 != 0);
  *param_1 = param_2;
  param_1[0x3b] = 0;
  param_1[3] = 0;
  param_1[2] = 0;
  param_1[0x19] = 0;
  uVar1 = OSGetTick();
  param_1[1] = uVar1 / (___OSBusClock / 4000);
  iVar3 = 10;
  iVar2 = 0;
  do {
    *(undefined4 *)((int)param_1 + iVar2 + 0x10) = *(undefined4 *)((int)local_18 + iVar2);
    *(undefined4 *)((int)param_1 + iVar2 + 0x38) = 0;
    iVar3 = iVar3 + -1;
    iVar2 = iVar2 + 4;
  } while (iVar3 != 0);
  param_1[0x18] = 0;
  iVar2 = 0;
  iVar3 = 10;
  do {
    *(int *)((int)&ia_1518 + iVar2) =
         (int)(*(float *)(unaff_r13 + -0x659c) * *(float *)((int)&a_1517 + iVar2));
    iVar2 = iVar2 + 4;
    iVar3 = iVar3 + -1;
  } while (iVar3 != 0);
  iVar3 = 10;
  iVar2 = 0;
  do {
    *(undefined4 *)((int)param_1 + iVar2 + 0x10) = *(undefined4 *)((int)&ia_1518 + iVar2);
    *(undefined4 *)((int)param_1 + iVar2 + 0x38) = 0;
    iVar3 = iVar3 + -1;
    iVar2 = iVar2 + 4;
  } while (iVar3 != 0);
  param_1[0x18] = 0;
  return;
}



/* ================================================================== */
/* 802dd360  SetMemoryPoolSize__9CarLoaderi
   tam=304 B  insn=76  cobertura=304/304  paired-singles=0  firma=IMPORTED
   proto: undefined CarLoader::SetMemoryPoolSize(CarLoader * this, int size)
*/

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void CarLoader__SetMemoryPoolSize(CarLoader *this,int size)

{
  void **ppvVar1;
  void *pvVar2;
  int iVar3;
  
  if (this->MemoryPoolSize != size) {
    if (this->MemoryPoolSize != 0) {
      iVar3 = 0;
      if (this->NumSpongeAllocations > 0) {
        do {
          ppvVar1 = this->SpongeAllocations + iVar3;
          iVar3 = iVar3 + 1;
          bFree(*ppvVar1);
        } while (iVar3 < this->NumSpongeAllocations);
      }
      this->NumSpongeAllocations = 0;
      CarLoader__UnloadUnallocatedRideInfos(this,0);
      if ((this->LoadedRideInfoList).__base.HeadNode.Next != (bNode *)&this->LoadedRideInfoList) {
        return;
      }
      bCloseMemoryPool(CarLoaderMemoryPoolNumber);
      bFree(this->MemoryPoolMem);
      this->MemoryPoolSize = 0;
      this->MemoryPoolMem = (void *)0x0;
    }
    if (size != 0) {
      TrackStreamer__FlushHibernatingSections(&TheTrackStreamer);
      TrackStreamer__MakeSpaceInPool(&TheTrackStreamer,size,true);
      pvVar2 = bMalloc(size,7);
      this->MemoryPoolSize = size;
      this->MemoryPoolMem = pvVar2;
      CarLoaderMemoryPoolNumber = bGetFreeMemoryPoolNum();
      bInitMemoryPool(CarLoaderMemoryPoolNumber,this->MemoryPoolMem,this->MemoryPoolSize,"Cars");
      bSetMemoryPoolDebugFill(CarLoaderMemoryPoolNumber,false);
      bSetMemoryPoolTopDirection(CarLoaderMemoryPoolNumber,true);
      this->NumSpongeAllocations = 0;
    }
  }
  return;
}



/* ================================================================== */
/* 80320840  Effect_Init
   tam=276 B  insn=69  cobertura=276/276  paired-singles=0  firma=DEFAULT
   proto: undefined Effect_Init()
*/

/* WARNING: Removing unreachable block (ram,0x80320894) */
/* WARNING: Removing unreachable block (ram,0x80320898) */

void Effect_Init(int param_1)

{
  float fVar1;
  int iVar2;
  int iVar3;
  undefined4 *puVar4;
  int unaff_r13;
  uint uVar5;
  int iVar6;
  double dVar7;
  
  puVar4 = (undefined4 *)(param_1 + -4);
  iVar3 = 2;
  do {
    puVar4[1] = 0;
    iVar3 = iVar3 + -1;
    puVar4[2] = 0;
    puVar4[3] = 0;
    puVar4[4] = 0;
    puVar4[5] = 0;
    puVar4[6] = 0;
    puVar4[7] = 0;
    puVar4 = puVar4 + 8;
    *puVar4 = 0;
  } while (iVar3 != 0);
  if (*(int *)(unaff_r13 + -0x74f0) == 0) {
    uVar5 = 0;
    iVar6 = 0;
    iVar3 = 0;
    do {
      dVar7 = sin((double)(*(float *)(unaff_r13 + -0x65a8) *
                           (float)((double)CONCAT44(0x43300000,uVar5 ^ 0x80000000) -
                                  *(double *)(unaff_r13 + -0x65b0)) *
                          *(float *)(unaff_r13 + -0x65c0)));
      uVar5 = uVar5 + 1;
      iVar2 = iVar6 / 0x3f;
      fVar1 = *(float *)(unaff_r13 + -0x65a4);
      iVar6 = iVar6 + 0x400;
      *(short *)((int)&g_iRampUpTable + iVar3) = (short)iVar2;
      *(short *)((int)&g_iQuarterSineTable + iVar3) = (short)(int)(fVar1 * (float)dVar7);
      iVar3 = iVar3 + 2;
    } while ((int)uVar5 < 0x40);
    *(undefined4 *)(unaff_r13 + -0x74f0) = 1;
  }
  return;
}



/* ================================================================== */
/* 8010ac40  eProject__FfffPA3_fPfN44
   tam=268 B  insn=67  cobertura=268/268  paired-singles=0  firma=IMPORTED
   proto: undefined eProject(float x, float y, float z, float[4] * mtx, float * pm, float * vp, float * sx, float * sy, float * sz)
*/

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void eProject(float x,float y,float z,float (*mtx) [4],float *pm,float *vp,float *sx,float *sy,
             float *sz)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float local_38;
  float local_34;
  float local_30;
  float local_28;
  float local_24;
  float local_20;
  
  local_38 = x;
  local_34 = y;
  local_30 = z;
  PSMTXMultVec(mtx,&local_38,&local_28);
  if (local_20 == 0.0) {
    *sy = 0.0;
    fVar1 = -2.0;
    *sx = 0.0;
  }
  else {
    fVar6 = 1.0 / -local_20;
    fVar1 = pm[6];
    fVar2 = pm[3];
    fVar3 = pm[4];
    fVar4 = pm[5];
    fVar5 = vp[3];
    *sx = vp[2] * 0.5 * (local_28 * pm[1] + local_20 * pm[2]) * fVar6 + *vp + vp[2] * 0.5;
    *sy = fVar5 * 0.5 * -(local_24 * fVar2 + local_20 * fVar3) * fVar6 + vp[1] + fVar5 * 0.5;
    fVar1 = (vp[5] - vp[4]) * (local_20 * fVar4 + fVar1) * fVar6 + vp[5];
  }
  *sz = fVar1;
  return;
}



/* ================================================================== */
/* 8031fa48  CookValues
   tam=260 B  insn=65  cobertura=260/260  paired-singles=0  firma=DEFAULT
   proto: undefined CookValues()
*/

void CookValues(undefined2 *param_1,undefined4 *param_2,int param_3)

{
  char cVar1;
  int iVar2;
  int iVar3;
  undefined4 uVar4;
  int iVar5;
  
  cVar1 = *(char *)((int)param_2 + 3);
  iVar5 = (int)cVar1;
  iVar2 = (int)*(char *)(param_3 + 0xd);
  iVar3 = (int)*(char *)(param_3 + 3);
  if (iVar5 < *(char *)(param_3 + 3)) {
    *(char *)(param_3 + 3) = cVar1;
    iVar3 = iVar5;
  }
  if (iVar2 < iVar5) {
    iVar2 = (int)cVar1;
    *(char *)(param_3 + 0xd) = cVar1;
  }
  iVar2 = (iVar2 - iVar3) + *(char *)(param_3 + 0x17) * -2;
  if (iVar2 == 0) {
    iVar2 = 1;
  }
  iVar3 = ((iVar5 - (iVar3 + *(char *)(param_3 + 0x17))) * 0xff) / iVar2 + -0x7f;
  if (iVar3 < -0x80) {
    iVar3 = -0x80;
  }
  if (iVar3 > 0x7f) {
    iVar3 = 0x7f;
  }
  *(char *)((int)param_1 + 3) = (char)iVar3;
  HandlePedals(param_1,param_2,param_3);
  HandleTriggers(param_1,param_2,param_3);
  *param_1 = *(undefined2 *)param_2;
  *(undefined1 *)(param_1 + 1) = *(undefined1 *)((int)param_2 + 2);
  uVar4 = param_2[1];
  *(undefined4 *)(param_3 + 0x1e) = *param_2;
  *(undefined4 *)(param_3 + 0x22) = uVar4;
  *(undefined2 *)(param_3 + 0x26) = *(undefined2 *)(param_2 + 2);
  *(undefined1 *)((int)param_1 + 9) = 0;
  return;
}



/* ================================================================== */
/* 803210b8  Effect_PerformEnvelope
   tam=196 B  insn=49  cobertura=196/196  paired-singles=0  firma=DEFAULT
   proto: undefined Effect_PerformEnvelope()
*/

uint Effect_PerformEnvelope(int param_1,uint *param_2,uint param_3)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  
  uVar5 = *(uint *)(param_1 + 0x2c);
  uVar3 = *param_2;
  uVar6 = ((int)param_3 >> 0x1f ^ param_3) - ((int)param_3 >> 0x1f);
  if (uVar5 < uVar3) {
    iVar1 = ((int)uVar3 >> 1) + (uint)((int)uVar3 < 0 && (uVar3 & 1) != 0);
    if ((int)uVar5 < 0) {
      iVar1 = -iVar1;
    }
    uVar6 = (uint)*(byte *)(param_2 + 2) +
            (int)(uVar5 * (uVar6 - *(byte *)(param_2 + 2)) + iVar1) / (int)uVar3;
  }
  else {
    uVar3 = *(uint *)(param_1 + 4);
    if (uVar3 != 0xffffffff) {
      uVar4 = param_2[1];
      if (uVar3 - uVar4 < uVar5) {
        if (uVar5 < uVar3) {
          iVar2 = uVar5 - (uVar3 - uVar4);
          iVar1 = ((int)uVar4 >> 1) + (uint)((int)uVar4 < 0 && (uVar4 & 1) != 0);
          if (iVar2 < 0) {
            iVar1 = -iVar1;
          }
          uVar6 = uVar6 + (int)(iVar2 * (*(byte *)((int)param_2 + 9) - uVar6) + iVar1) / (int)uVar4;
        }
        else {
          uVar6 = (uint)*(byte *)((int)param_2 + 9);
        }
      }
    }
  }
  if ((int)param_3 < 0) {
    uVar6 = -uVar6;
  }
  return uVar6;
}



