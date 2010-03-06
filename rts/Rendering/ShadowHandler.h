/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#ifndef SHADOWHANDLER_H
#define SHADOWHANDLER_H

#include <vector>

#include "GL/myGL.h"
#include "Rendering/GL/FBO.h"
#include "System/Matrix44f.h"

namespace Shader {
	struct IProgramObject;
};

class CShadowHandler
{
public:
	CShadowHandler(void);
	~CShadowHandler(void);
	void CreateShadows(void);

	int shadowMapSize;
	GLuint shadowTexture;

	static bool canUseShadows;
	static bool useFPShadows;

	bool drawShadows;
	bool inShadowPass;
	bool showShadowMap;
	bool drawTerrainShadow;

	float3 centerPos;
	float3 cross1;
	float3 cross2;

	float x1, x2, y1, y2;
	float xmid, ymid;
	float p17, p18;

	CMatrix44f shadowMatrix;
	void DrawShadowTex(void);
	void CalcMinMaxView(void);

	void GetShadowMapSizeFactors(float &param17, float &param18);

	enum ShadowGenProgram {
		SHADOWGEN_PROGRAM_MODEL      = 0,
		SHADOWGEN_PROGRAM_MAP        = 1,
		SHADOWGEN_PROGRAM_TREE_NEAR  = 2,
		SHADOWGEN_PROGRAM_TREE_FAR   = 3,
		SHADOWGEN_PROGRAM_PROJECTILE = 4,
		SHADOWGEN_PROGRAM_LAST       = 5,
	};

	Shader::IProgramObject* GetShadowGenProg(ShadowGenProgram p) {
		return shadowGenProgs[p];
	}

protected:
	void GetFrustumSide(float3& side, bool upside);
	bool InitDepthTarget();
	void DrawShadowPasses();
	void LoadShadowGenShaderProgs();

	struct fline {
		float base;
		float dir;
		int left;
		float minz;
		float maxz;
	};
	std::vector<fline> left;
	FBO fb;

	static bool firstInstance;

	//! these project geometry into light-space
	//! to write the (FBO) depth-buffer texture
	std::vector<Shader::IProgramObject*> shadowGenProgs;
};

extern CShadowHandler* shadowHandler;

#endif /* SHADOWHANDLER_H */
