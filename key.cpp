/*
 * filename: key.cpp
 * describe: This is the implementation of abstract base class EdsKey, used
 *           by the datastore engine - EDatastore of my open source project:
 *           EDS (Embedded Data Store)
 * Author:   Jerry Sun <jerysun0818@gmail.com>
 * Date:     December 19, 2015
 * Remark:   No dependency. Handy
 * Linkedin: http://nl.linkedin.com/in/jerysun
 * Website:  https://sites.google.com/site/geekssmallworld
 * Github:   https://github.com/jerysun/
 */

#include "stdafx.h"
#include "edatastore.h"

EdsKey::EdsKey(NodeNbr fa) {
  fileaddr = fa;
  lowernode = 0;
  indexno = 0;
  relatedclass = 0;
  if (Serialize::objconstructed != 0)  {
    // register the key with the object being built
    Serialize::objconstructed->RegisterKey(this);
    // assign index number based on position in object
    indexno = ++Serialize::objconstructed->indexcount;
  }
}

EdsKey& EdsKey::operator=(const EdsKey& key) {
  if (this != &key) {
    fileaddr = key.fileaddr;
    lowernode = key.lowernode;
    indexno = key.indexno;
    keylength = key.keylength;
    relatedclass = key.relatedclass;
  }
  return *this;
}
