#ifndef ECSTASY_ESOLID_H
#define ECSTASY_ESOLID_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

void eInitSolids();
void eSolidNotifyTextureLoading(struct TexturePack *texture_pack /* r27 */, struct TextureInfo *texture_info /* r4 */, bool loading /* r5 */);

#endif
