/**
 * @addtogroup SRS
 *
 * One module to rule them all...
 *
 * @{
 */

/*! \mainpage SRS Overview
 *
 * The low-level API for interfacing with libSRS, a portable SRS framework.
 *
 * You may wish to use this for...
 *
 * 1. Binding the libSRS API to another language (such as Lua, Python, Javascript, etc.)
 * 2. Implementing a RESTful server for arbitrary clients to interface with.
 * 3. Implementing a self-contained client (for the brave).
 */
#ifndef _SRS_H
#define _SRS_H

#define SRS_VERSION "0.0.0"

#include "srs/decl.h"
#include "srs/types.h"
#include "srs/log.h"
#include "srs/filesystem.h"
#include "srs/git.h"
#include "srs/schedule.h"
#include "srs/string.h"
#include "srs/model.h"
#include "srs/card.h"
#include "srs/controller.h"
#include "srs/server.h"
#include "srs/rest.h"
#include "srs/result.h"
#include "srs/error.h"
#include "srs/enum.h"
#include "srs/datastructure.h"

#endif /* _SRS_H */

/** @} */
