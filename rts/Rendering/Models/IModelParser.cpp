/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#include "StdAfx.h"
#include "Rendering/GL/myGL.h"
#include <algorithm>
#include <cctype>
#include "mmgr.h"

#include "IModelParser.h"
#include "3DModel.h"
#include "3DModelLog.h"
#include "3DOParser.h"
#include "S3OParser.h"
#include "OBJParser.h"
#ifdef _MSC_VER
#define _INC_MATH // a hack to prevent ambiguous math calls
#endif
#include "AssParser.h"
#include "assimp.hpp"
#include "Sim/Misc/CollisionVolume.h"
#include "Sim/Units/Unit.h"
#include "System/FileSystem/FileSystem.h"
#include "System/Util.h"
#include "System/LogOutput.h"
#include "System/Exceptions.h"

C3DModelLoader* modelParser = NULL;

//////////////////////////////////////////////////////////////////////
// C3DModelLoader
//

C3DModelLoader::C3DModelLoader()
{
	// file-extension should be lowercase
	parsers["3do"] = new C3DOParser();
	parsers["s3o"] = new CS3OParser();
	//parsers["obj"] = new COBJParser(); // replaced by Assimp

	// assimp library
	CAssParser* unitassparser = new CAssParser();
	std::string extensionlist;

	Assimp::Importer importer;
	importer.GetExtensionList(extensionlist); // get a ";" separated list of wildcards
	char* charextensionlist = new char[extensionlist.size() +1];
	strcpy (charextensionlist, extensionlist.c_str());
	logOutput.Print("Assimp: Supported model formats: %s", extensionlist.c_str());
	char* extensionchar = strtok( charextensionlist, ";" );
	while( extensionchar )
	{
		std::string extension = extensionchar;
		extension = extension.substr( 2 ); // strip wildcard and dot
		parsers[extension] = unitassparser; // register extension
		extensionchar = strtok( NULL, ";" );
	}
	delete[] charextensionlist;
}


C3DModelLoader::~C3DModelLoader()
{
	// delete model cache
	std::map<std::string, S3DModel*>::iterator ci;
	for (ci = cache.begin(); ci != cache.end(); ++ci) {
		S3DModel* model = ci->second;

		DeleteChilds(model->GetRootPiece());
		delete model;
	}
	cache.clear();

	// delete parsers
	std::set<IModelParser*> dedupe_parsers; // this is to avoid deleting the same parser twice, if it's assigned to multiple model formats
	std::map<std::string, IModelParser*>::iterator pi;
	for (pi = parsers.begin(); pi != parsers.end(); ++pi) {
		if ( dedupe_parsers.count( pi->second ) != 0 ) continue;
		dedupe_parsers.insert( pi->second );
		delete pi->second;
	}
	parsers.clear();
	dedupe_parsers.clear();

#if defined(USE_GML) && GML_ENABLE_SIM
	createLists.clear();
	fixLocalModels.clear();
	Update(); // delete remaining local models
#endif
}



static inline ModelType ModelExtToModelType(const std::string& ext) {
	if (ext == "3do") { return MODELTYPE_3DO; }
	if (ext == "s3o") { return MODELTYPE_S3O; }
	if (ext == "obj") { return MODELTYPE_OBJ; }
	return MODELTYPE_ASS; // FIXME: Return -1 if Assimp cant handle extension
}


static inline S3DModelPiece* ModelTypeToModelPiece(const ModelType& type) {
	if (type == MODELTYPE_3DO) { return (new S3DOPiece()); }
	if (type == MODELTYPE_S3O) { return (new SS3OPiece()); }
	if (type == MODELTYPE_OBJ) { return (new SOBJPiece()); }
	return NULL;
}



S3DModel* C3DModelLoader::Load3DModel(std::string name, const float3& centerOffset)
{
	GML_STDMUTEX_LOCK(model); // Load3DModel

	StringToLowerInPlace(name);

	//! search in cache first
	std::map<std::string, S3DModel*>::iterator ci;
	if ((ci = cache.find(name)) != cache.end()) {
		return ci->second;
	}

	//! not found in cache, create the model and cache it
	const std::string& fileExt = filesystem.GetExtension(name);
	const std::map<std::string, IModelParser*>::iterator pi = parsers.find(fileExt);

	if (pi != parsers.end()) {
		IModelParser* p = pi->second;
		S3DModel* model = NULL;
		S3DModelPiece* root = NULL;

		try {
			model = p->Load(name);
			model->relMidPos += centerOffset;
		} catch (const content_error& e) {
			// crash-dummy
			model = new S3DModel();
			model->type = ModelExtToModelType(StringToLower(fileExt));
			model->numPieces = 1;
			model->SetRootPiece(ModelTypeToModelPiece(model->type));
			model->GetRootPiece()->SetCollisionVolume(new CollisionVolume("box", UpVector * -1.0f, ZeroVector, CollisionVolume::COLVOL_HITTEST_CONT));

			logOutput.Print("WARNING: could not load model \"" + name + "\" (reason: " + e.what() + ")");
		}

		if ((root = model->GetRootPiece()) != NULL) {
			CreateLists(root);
		}

		cache[name] = model;    //! cache the model
		model->id = cache.size(); //! IDs start with 1
		return model;
	}

	logOutput.Print("ERROR: could not find a parser for model \"" + name + "\" (unknown format?)");
	return NULL;
}

void C3DModelLoader::Update() {
#if defined(USE_GML) && GML_ENABLE_SIM
	GML_STDMUTEX_LOCK(model); // Update

	for (std::vector<S3DModelPiece*>::iterator it = createLists.begin(); it != createLists.end(); ++it) {
		CreateListsNow(*it);
	}
	createLists.clear();

	for (std::set<CUnit*>::iterator i = fixLocalModels.begin(); i != fixLocalModels.end(); ++i) {
		FixLocalModel(*i);
	}
	fixLocalModels.clear();

	for (std::vector<LocalModel*>::iterator i = deleteLocalModels.begin(); i != deleteLocalModels.end(); ++i) {
		delete *i;
	}
	deleteLocalModels.clear();
#endif
}


void C3DModelLoader::DeleteChilds(S3DModelPiece* o)
{
	for (std::vector<S3DModelPiece*>::iterator di = o->childs.begin(); di != o->childs.end(); ++di) {
		DeleteChilds(*di);
	}

	o->childs.clear();
	delete o;
}


void C3DModelLoader::DeleteLocalModel(CUnit* unit)
{
#if defined(USE_GML) && GML_ENABLE_SIM
	GML_STDMUTEX_LOCK(model); // DeleteLocalModel

	fixLocalModels.erase(unit);
	deleteLocalModels.push_back(unit->localmodel);
#else
	delete unit->localmodel;
#endif
}

void C3DModelLoader::CreateLocalModel(CUnit* unit)
{
#if defined(USE_GML) && GML_ENABLE_SIM
	GML_STDMUTEX_LOCK(model); // CreateLocalModel

	unit->localmodel = new LocalModel(unit->model);
	fixLocalModels.insert(unit);
#else
	unit->localmodel = new LocalModel(unit->model);
	// FixLocalModel(unit);
#endif
}


void C3DModelLoader::FixLocalModel(CUnit* unit) //FIXME rename! (it adds (delayed) the DL-Ids to the localmodel)
{
	int piecenum = 0;
	FixLocalModel(unit->model->GetRootPiece(), unit->localmodel, &piecenum);
}

void C3DModelLoader::FixLocalModel(S3DModelPiece* model, LocalModel* lmodel, int* piecenum)
{
	lmodel->pieces[*piecenum]->dispListID = model->dispListID;

	for (unsigned int i = 0; i < model->childs.size(); i++) {
		(*piecenum)++;
		FixLocalModel(model->childs[i], lmodel, piecenum);
	}
}


void C3DModelLoader::CreateListsNow(S3DModelPiece* o)
{
	o->dispListID = glGenLists(1);
	glNewList(o->dispListID, GL_COMPILE);
		o->DrawForList();
	glEndList();

	for (std::vector<S3DModelPiece*>::iterator bs = o->childs.begin(); bs != o->childs.end(); ++bs) {
		CreateListsNow(*bs);
	}
}


void C3DModelLoader::CreateLists(S3DModelPiece* o) {
#if defined(USE_GML) && GML_ENABLE_SIM
	createLists.push_back(o);
#else
	CreateListsNow(o);
#endif
}

/******************************************************************************/
/******************************************************************************/
