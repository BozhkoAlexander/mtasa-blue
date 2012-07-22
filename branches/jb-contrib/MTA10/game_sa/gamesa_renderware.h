/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/gamesa_renderware.h
*  PURPOSE:     RenderWare interface mappings to Grand Theft Auto: San Andreas
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*  RenderWare is � Criterion Software
*
*****************************************************************************/

#ifndef __GAMESA_RENDERWARE
#define __GAMESA_RENDERWARE

#define WIN32_LEAN_AND_MEAN

#include <game/RenderWare.h>
#include <windows.h>
#include <stdio.h>
#include "gamesa_init.h"

class CColModelSAInterface;

/*****************************************************************************/
/** Renderware functions                                                    **/
/*****************************************************************************/

/* RenderWare macros */
#define RpGetFrame(__c)            ((RwFrame*)(((RwObject *)(__c))->parent))
#define RpSetFrame(__c,__f)        ((((RwObject *)(__c))->parent) = (void *)(__f))

/* RenderWare function defines */
typedef RpAtomic *              (__cdecl *RpAtomicCreate_t)                     (void);
typedef RpAtomic *              (__cdecl *RpAtomicClone_t)                      (RpAtomic * atomic);
typedef int                 (__cdecl *RpAtomicDestroy_t)                    (RpAtomic * atomic);
typedef RpAtomic *              (__cdecl *RpAtomicSetGeometry_t)                (RpAtomic * atomic, RpGeometry * geometry, unsigned int flags);
typedef RpAtomic *              (__cdecl *RpAtomicSetFrame_t)                   (RpAtomic * atomic, RwFrame * frame);
typedef RpClump *               (__cdecl *RpClumpAddAtomic_t)                   (RpClump * clump, RpAtomic * atomic);
typedef RpClump *               (__cdecl *RpClumpAddLight_t)                    (RpClump * clump, RpLight * light);
typedef int                 (__cdecl *RpClumpGetNumAtomics_t)               (RpClump * clump);
typedef RpClump *               (__cdecl *RpClumpRemoveAtomic_t)                (RpClump * clump, RpAtomic * atomic);
typedef RwFrame *               (__cdecl *RwFrameAddChild_t)                    (RwFrame * parent, RwFrame * child);
typedef RwFrame *               (__cdecl *RwFrameRemoveChild_t)                 (RwFrame * child);
typedef RwFrame *               (__cdecl *RwFrameForAllObjects_t)               (RwFrame * frame, void * callback, void * data);
typedef RwFrame *               (__cdecl *RwFrameTranslate_t)                   (RwFrame * frame, const RwV3d * v, RwTransformOrder order);
typedef RwFrame *               (__cdecl *RwFrameScale_t)                       (RwFrame * frame, const RwV3d * v, RwTransformOrder order);
typedef RwFrame *               (__cdecl *RwFrameCreate_t)                      (void);
typedef RwFrame *               (__cdecl *RwFrameSetIdentity_t)                 (RwFrame * frame);
typedef RpGeometry *            (__cdecl *RpGeometryCreate_t)                   (int numverts, int numtriangles, unsigned int format);
typedef const RpGeometry *      (__cdecl *RpGeometryTriangleSetVertexIndices_t) (const RpGeometry * geo, RpTriangle * tri, unsigned short v1, unsigned short v2, unsigned short v3);
typedef RpGeometry *            (__cdecl *RpGeometryUnlock_t)                   (RpGeometry * geo);
typedef RpGeometry *            (__cdecl *RpGeometryLock_t)                     (RpGeometry * geo, int lockmode);
typedef RpGeometry *            (__cdecl *RpGeometryTransform_t)                (RpGeometry * geo, const RwMatrix * matrix);
typedef RpGeometry *            (__cdecl *RpGeometryTriangleSetMaterial_t)      (RpGeometry * geo, RpTriangle * tri, RpMaterial * mat);
typedef int                 (__cdecl *RpGeometryDestroy_t)                  (RpGeometry * geo);
typedef void *                  (__cdecl *RwIm3DTransform_t)                    (RwVertex *pVerts, unsigned int numVerts, RwMatrix *ltm, unsigned int flags);
typedef int                 (__cdecl *RwIm3DRenderIndexedPrimitive_t)       (RwPrimitiveType primType, unsigned short *indices, int numIndices);
typedef int                 (__cdecl *RwIm3DEnd_t)                          (void);
typedef RpLight *               (__cdecl *RpLightCreate_t)                      (int type);
typedef RpLight *               (__cdecl *RpLightSetRadius_t)                   (RpLight * light, float radius);
typedef RpLight *               (__cdecl *RpLightSetColor_t)                    (RpLight * light, const RwColorFloat * color);
typedef RwMatrix *              (__cdecl *RwMatrixCreate_t)                     (void);
typedef RwMatrix *              (__cdecl *RwMatrixInvert_t)                     (RwMatrix *dst, const RwMatrix *src);
typedef RwMatrix *              (__cdecl *RwMatrixTranslate_t)                  (RwMatrix * matrix, const RwV3d * translation, RwTransformOrder order);
typedef RwMatrix *              (__cdecl *RwMatrixScale_t)                      (RwMatrix * matrix, const RwV3d * translation, RwTransformOrder order);
typedef RpMaterial *            (__cdecl *RpMaterialCreate_t)                   (void);
typedef int                 (__cdecl *RpMaterialDestroy_t)                  (RpMaterial * mat);
typedef RwTexDictionary *       (__cdecl *RwTexDictionarySetCurrent_t)          (RwTexDictionary * dict);
typedef const RwTexDictionary * (__cdecl *RwTexDictionaryForAllTextures_t)      (const RwTexDictionary * dict, void * callback, void * data);
typedef RwTexture *             (__cdecl *RwTexDictionaryAddTexture_t)          (RwTexDictionary * dict, RwTexture * texture);
typedef RwTexDictionary *       (__cdecl *RwTexDictionaryGetCurrent_t)          (void);
typedef RwTexture *             (__cdecl *RwTexDictionaryFindNamedTexture_t)    (RwTexDictionary * dict, const char* name);
typedef void                    (__cdecl *RpPrtStdGlobalDataSetStreamEmbedded_t)(void * value);
typedef RpWorld *               (__cdecl *RpWorldAddAtomic_t)                   (RpWorld * world, RpAtomic * atomic);
typedef RpWorld *               (__cdecl *RpWorldAddClump_t)                    (RpWorld * world, RpClump * clump);
typedef RpWorld *               (__cdecl *RpWorldAddLight_t)                    (RpWorld * world, RpLight * light);
typedef float                   (__cdecl *RwV3dNormalize_t)                     (RwV3d * out, const RwV3d * in);
typedef RwV3d *                 (__cdecl *RwV3dTransformVector_t)               (RwV3d * out, const RwV3d * in, const RwMatrix * matrix);
typedef void                    (__cdecl *_rwObjectHasFrameSetFrame_t)          (void *object, RwFrame *frame);
typedef RwCamera *              (__cdecl *RwCameraClone_t)                      (RwCamera *camera);
typedef RpClump *               (__cdecl *RpClumpClone_t)                       (RpClump *clone);
typedef int                 (__cdecl *RwTexDictionaryDestroy_t)             (RwTexDictionary *txd);
typedef int                 (__cdecl *RwTextureDestroy_t)                   (RwTexture *texture);
typedef int                 (__cdecl *RwStreamFindChunk_t)                  (RwStream *stream, unsigned int type, unsigned int *lengthOut, unsigned int *versionOut);
typedef RpClump*                (__cdecl *RpClumpStreamRead_t)                  (RwStream *stream);
typedef RwError*                (__cdecl *RwErrorGet_t)                         (RwError *code);
typedef RwStream*               (__cdecl *RwStreamOpen_t)                       (RwStreamType type, RwStreamMode mode, const void *pData);
typedef int                 (__cdecl *RwStreamClose_t)                      (RwStream *stream, void *pData);
typedef int                 (__cdecl *RpClumpDestroy_t)                     (RpClump *clump);
typedef RpClump*                (__cdecl *RpClumpForAllAtomics_t)               (RpClump *clump, void* callback, void* pData);
typedef RwTexDictionary*        (__cdecl *RwTexDictionaryStreamRead_t)          (RwStream *stream);
typedef RwTexDictionary*        (__cdecl *RwTexDictionaryGtaStreamRead_t)       (RwStream *stream);
typedef RwRaster*               (__cdecl *RwRasterUnlock_t)                     (RwRaster *raster);
typedef RwRaster*               (__cdecl *RwRasterLock_t)                       (RwRaster *raster, unsigned char level, int lockmode);
typedef RwRaster*               (__cdecl *RwRasterCreate_t)                     (int width, int height, int depth, int flags);
typedef RwTexture*              (__cdecl *RwTextureCreate_t)                    (RwRaster *raster);

/*****************************************************************************/
/** GTA function definitions and mappings                                   **/
/*****************************************************************************/

typedef bool                (__cdecl *SetTextureDict_t)                 (unsigned short id);
typedef bool                (__cdecl *LoadClumpFile_t)                  (RwStream *stream, unsigned int id);        // (stream, model id)
typedef bool                (__cdecl *LoadModel_t)                      (RwBuffer *filename, unsigned int id);      // (memory chunk, model id)
typedef void                (__cdecl *LoadCollisionModel_t)             (unsigned char*, CColModelSAInterface*, const char*);
typedef void                (__cdecl *LoadCollisionModelVer2_t)         (unsigned char*, unsigned int, CColModelSAInterface*, const char*);
typedef void                (__cdecl *LoadCollisionModelVer3_t)         (unsigned char*, unsigned int, CColModelSAInterface*, const char*); // buf, bufsize, ccolmodel&, keyname
typedef bool                (__cdecl *CTxdStore_LoadTxd_t)              (unsigned int id, RwStream *filename);
typedef void                (__cdecl *CTxdStore_RemoveTxd_t)            (unsigned int id);
typedef void                (__cdecl *CTxdStore_RemoveRef_t)            (unsigned int id);
typedef void                (__cdecl *CTxdStore_AddRef_t)               (unsigned int id);
typedef RwTexDictionary*    (__cdecl *CTxdStore_GetTxd_t)               (unsigned int id);
typedef RwTexture*          (__cdecl *CClothesBuilder_CopyTexture_t)    (RwTexture *texture);

SetTextureDict_t                SetTextureDict                  = (SetTextureDict_t)                0xDEAD;
LoadClumpFile_t                 LoadClumpFile                   = (LoadClumpFile_t)                 0xDEAD;
LoadModel_t                     LoadModel                       = (LoadModel_t)                     0xDEAD;
LoadCollisionModel_t            LoadCollisionModel              = (LoadCollisionModel_t)            0xDEAD;
LoadCollisionModelVer2_t        LoadCollisionModelVer2          = (LoadCollisionModelVer2_t)        0xDEAD;
LoadCollisionModelVer3_t        LoadCollisionModelVer3          = (LoadCollisionModelVer3_t)        0xDEAD;
CTxdStore_LoadTxd_t             CTxdStore_LoadTxd               = (CTxdStore_LoadTxd_t)             0xDEAD;
CTxdStore_GetTxd_t              CTxdStore_GetTxd                = (CTxdStore_GetTxd_t)              0xDEAD;
CTxdStore_RemoveTxd_t           CTxdStore_RemoveTxd             = (CTxdStore_RemoveTxd_t)           0xDEAD;
CTxdStore_RemoveRef_t           CTxdStore_RemoveRef             = (CTxdStore_RemoveRef_t)           0xDEAD;
CTxdStore_AddRef_t              CTxdStore_AddRef                = (CTxdStore_AddRef_t)              0xDEAD;
CClothesBuilder_CopyTexture_t   CClothesBuilder_CopyTexture     = (CClothesBuilder_CopyTexture_t)   0xDEAD;

/*****************************************************************************/
/** Function inlines                                                        **/
/*****************************************************************************/

// Matrix copying
void RwFrameCopyMatrix ( RwFrame * dst, RwFrame * src );

// Recursive RwFrame children searching function
RwFrame * RwFrameFindFrame ( RwFrame * parent, const char * name );

#endif