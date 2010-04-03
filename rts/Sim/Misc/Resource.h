/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#ifndef _RESOURCE_H
#define _RESOURCE_H

#include <string>
#include "creg/creg_cond.h"

class CResource
{
public:
	CR_DECLARE(CResource);

	CResource();
	~CResource();

	/// The name of this resource, eg. "Energy" or "Metal"
	std::string name;
	/// The optimum value for this resource, eg. 0 for "Waste" or MAX_FLOAT for "Metal"
	float optimum;
	/// The default extractor radius for the resource map, 0.0f if non applicable
	float extractorRadius;
	/// What value 255 in the resource map is worth
	float maxWorth;
	/**
	 * This is used for comparing the overall value of a resources.
	 * The resulting value comparison is an aproximation value only,
	 * and differes from mod to mod.
	 * It may be usefull for AIs, for example.
	 * example:
	 * metal.normalizationFactor = 60.0;
	 * energy.normalizationFactor = 2.0;
	 * means:
	 * 60 units of energy are worth 2 units of metal
	 */
	float normalizationFactor;
};

#endif // _RESOURCE_H
